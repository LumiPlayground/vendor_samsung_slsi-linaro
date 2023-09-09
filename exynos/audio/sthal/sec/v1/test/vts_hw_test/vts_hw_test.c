/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * @file            : vts_hw_test.c
 * @brief           : Voice Trigger Subsystem IP Unit test case application
 * @author          : Palli Satish Kumar Reddy (palli.satish@samsung.com)
 * @version         : 1.0
 * @history
 *   2016.05.16     : Create
 *   2018.06.07     : Updated to make it a generic unit test application which
 *                         can support all possible SoCs
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <poll.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#include <errno.h>
#include <cutils/uevent.h>
#include <tinyalsa/asoundlib.h>

#include "sound_trigger_hw.h"
#include "sthal_odm_definition.h"
#include "vts.h"
#include "soundtrigger_conf.h"
#include "st_xml_parser.h"
#include "st_pcm_dump.h"

#define UEVENT_MSG_LEN          64*1024

#define MISC_DEV "/dev/vts_fio_dev"

typedef enum {
    VTS_RECOGNITION_STOPPED_STATE,
    VTS_RECOGNITION_STARTED_STATE,
    VTS_RECOGNITION_TRIGGERED_STATE,
} VTS_HW_STATE;

typedef enum {
    VTS_RECORD_STOPPED_STATE,
    VTS_RECORD_STARTED_STATE,
} VTS_RECORD_STATE;

typedef enum {
    VTS_TRIGGERED_CAPTURE,
    VTS_NORMAL_CAPTURE,
} VTS_CAP_MODE;

typedef enum {
    ODM_MODEL = 1,
    GOOGLE_MODEL
} VTS_MODEL_TYPE;

#define VTS_HW_TRIGGER_PCM_DUMP_FILE "vtshw-trigger"
#define VTS_HW_RECORDING_PCM_DUMP_FILE  "vtshw-recording"

#define VTS_STREAMING_BUFFER_SIZE  4800 //(4 * 1024)
#define PLAY_BACK_BUFFER_SIZE VTS_STREAMING_BUFFER_SIZE*2
#define NUM_OF_SAMPLES_TO_CAPTURE 10
#define LOOPBACK_BUFFER_SIZE VTS_STREAMING_BUFFER_SIZE*NUM_OF_SAMPLES_TO_CAPTURE

struct sound_trigger_device stdev;

struct vts_hw_device {
    VTS_HW_STATE vts_state;
    VTS_RECORD_STATE vts_rec_state;
    int vts_active_model_type;
    pthread_mutex_t lock;
    pthread_mutex_t pcm_lock;
    int send_sock;
    int term_sock;
    bool thread_exit;
    bool rec_thread_exit;
    pthread_t callback_thread;
    pthread_t rec_thread;
    int model_loaded;

    pthread_t vts_play_thread;
};

static struct vts_hw_device g_vtsdev = { .lock = PTHREAD_MUTEX_INITIALIZER };
static struct mixer            *vtsMixerHandle = NULL;
static struct mixer_ctl        *vtsMixerCtl = NULL;
static int   vtsdev_fd = -1;
static void *mapped_addr = NULL;

enum state {
    MENU_START_BIXBY_RECOGNITION = 1,
    MENU_START_GOOGLE_RECOGNITION,
    MENU_STOP_RECOGNITION,
    MENU_START_RECORD,
    MENU_STOP_RECORD,
    MENU_SET_PARAM,
    MENU_EXIT
};

void vts_recognition_stop(struct vts_hw_device *vts_dev);
void vts_record_stop(struct vts_hw_device *vts_dev);

static int dmic_usagecnt = 0;

/* Utility function for configuration MIC mixer controls */
int set_mic_mixer_ctrls(struct mixer *mixer_handle, char path_name[20][30], int *path_ctlvalue,
                        int ctrl_count, bool reverse) {
    int i = (reverse ? (ctrl_count - 1): 0);
    int ret = 0;
    int temp[2] = {1,1};
    struct mixer_ctl *mixerctl = NULL;

    if (mixer_handle) {
        while(ctrl_count) {
            //printf("%s, ctrl_count: %d Loop index: %d", __func__, ctrl_count, i);
            /* Get required control from mixer */
            mixerctl = mixer_get_ctl_by_name(mixer_handle, path_name[i]);
            if (mixerctl) {
                /* Enable the control */
                if (path_ctlvalue)
                    ret = mixer_ctl_set_value(mixerctl, 0, path_ctlvalue[i]);
                else
                    ret = mixer_ctl_set_value(mixerctl, 0, 0);

                if (ret) {
                    printf("%s: %s Failed to configure\n", __func__, path_name[i]);
                    ret = -EINVAL;
                    break;
                } else {
                    printf("%s: %s configured value: %d\n", __func__, path_name[i],
                        (path_ctlvalue ? path_ctlvalue[i] : 0));
                }
            } else {
                printf("%s: %s control doesn't exist\n", __func__, path_name[i]);
            }

            ctrl_count--;
            if (reverse)
                i--;
            else
                i++;
        }
    } else{
        printf("%s: Failed to open mixer\n", __func__);
        return -EINVAL;
    }

    return ret;
}

int set_mixer_ctrls(
        struct mixer            *mixer_handle,
        char *path_name[],
        int *path_ctlvalue,
        int ctrl_count,
        bool reverse)
{
    int i = (reverse ? (ctrl_count - 1): 0);
    int ret = 0;
    int temp[2] = {1,1};
    struct mixer_ctl *mixerctl = NULL;

    printf("%s, path: %s \n", __func__, path_name[0]);

    if (mixer_handle) {
        //for (i=0; i < ctrl_count; i++) {
        while(ctrl_count) {
            //printf("%s, ctrl_count: %d Loop index: %d", __func__, ctrl_count, i);
            /* Get required control from mixer */
            mixerctl = mixer_get_ctl_by_name(mixer_handle, path_name[i]);
            if (mixerctl) {
                /* Enable the control */
                if (path_ctlvalue)
                    ret = mixer_ctl_set_value(mixerctl, 0, path_ctlvalue[i]);
                else
                    ret = mixer_ctl_set_value(mixerctl, 0, 0);

                if (ret) {
                    printf("%s: %s Failed to configure\n", __func__, path_name[i]);
                    ret = -EINVAL;
                    break;
                } else {
                    printf("%s: %s configured value: %d\n", __func__, path_name[i],
                        (path_ctlvalue ? path_ctlvalue[i] : 0));
                }
             } else {
                printf("%s: %s control doesn't exist\n", __func__, path_name[i]);
                ret = -EINVAL;
                break;
            }
            ctrl_count--;
            if (reverse)
                i--;
            else
                i++;
        }
    } else{
        printf("%s: Failed to open mixer\n", __func__);
        return -EINVAL;
    }
    return ret;
}

int set_mixer_ctrls_array(
        struct mixer *mixer_handle,
        char *path_name[],
        int *path_ctlvalue[],
        int ctrl_count,
        bool reverse,
        int value_cnt[])
{
    int i = (reverse ? (ctrl_count - 1): 0);
    int j = 0;
    int ret = 0;
    int buf[3];
    struct mixer_ctl *mixerctl = NULL;

    printf("%s, path: %s\n", __func__, path_name[0]);

    if (mixer_handle) {
        while(ctrl_count) {
            //printf("%s, ctrl_count: %d Loop index: %d", __func__, ctrl_count, i);
            /* Get required control from mixer */
            mixerctl = mixer_get_ctl_by_name(mixer_handle, path_name[i]);
            if (mixerctl) {
                printf("value count : %d \n", value_cnt[i]);
                for(j = 0; j<value_cnt[i]; j++){
                    buf[j] = path_ctlvalue[i][j];
                }
                /* Enable the control */
                ret = mixer_ctl_set_array(mixerctl, buf, value_cnt[i]);
                if (ret) {
                    printf("%s: %s Failed to configure\n", __func__, path_name[i]);
                    ret = -EINVAL;
                    break;
                } else {
                    for(j = 0; j<value_cnt[i]; j++){
                        printf("%s: %s %d configured value: %d\n", __func__, path_name[i],j,buf[j]);
                    }
                }
             } else {
                printf("%s: %s control doesn't exist\n", __func__, path_name[i]);
                ret = -EINVAL;
                break;
            }
            ctrl_count--;
            if (reverse)
                i--;
            else
                i++;
        }
    } else {
        printf("%s: Failed to open mixer\n", __func__);
        return -EINVAL;
    }
    return ret;
}


static void load_modelbinary(char *data, int len, int model)
{
    struct vts_model_bin_info sm_info;
    int model_index;
    int i;

    printf("%s: Size: %d\n", __func__, len);

    /* Copy model binary to VTS mapped address */
    memcpy(mapped_addr, data, len);

    printf("%s: Model binary copied to Mapped memory Size: %d\n", __func__, len);

    for (i = 0; i < stdev.num_sound_model; i++) {
        if (stdev.sm_info[i].id == model) {
            model_index = i;
            break;
        }
    }

    if (i >= stdev.num_sound_model) {
        printf("%s: Cannot find sound model index\n", __func__);
        return;
    }

    /* Should move to xml */
    sm_info.actual_sz = len;
    sm_info.max_sz = stdev.sm_info[model_index].sm_config.max_sz;
    sm_info.offset = stdev.sm_info[model_index].sm_config.offset;

    if (len > sm_info.max_sz) {
        printf("%s: MMAP buffer overflow Model Binary size greater then mapped size !!!\n", __func__);
        len = sm_info.max_sz;
    }

    /* Update model binary information to VTS misc driver */
    if (ioctl(vtsdev_fd, VTSDRV_MISC_IOCTL_LOAD_SOUND_MODEL, &sm_info) < 0) {
        printf("%s: Failed to update model binary size\n", __func__);
    }

    return;
}

static void sysfs_write(const char *path, char *data, int len)
{
    char buf[80];
    int fd = open(path, O_WRONLY);
    int tmp = 0, written = 0;

    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        printf("Error opening %s: %s\n", path, buf);
        return;
    }

    while (len) {
        tmp = write(fd, data+written, len);
        if (tmp < 0) {
            strerror_r(errno, buf, sizeof(buf));
            printf("Error writing to %s: %s\n", path, buf);
            break;
        }
        len -= tmp;
        written += tmp;
        printf("%s: current written %d Actual %d Total written %d\n",__func__, tmp, len, written);
    }

    close(fd);
    return;
}

int set_dmic_ctrls(int flag)
{
    int i;
    int ret = EXIT_SUCCESS;
    char **active_mic_ctrls = NULL;
    int *ctrl_values = NULL;

    if (vtsMixerHandle) {
        if (!flag) {
            if (dmic_usagecnt) {
                dmic_usagecnt--;
                printf("Dmic Disabled usage count %d \n", dmic_usagecnt);
            } else {
                printf("Dmic usage count is Zero \n");
                return ret;
            }
        }

        if (!dmic_usagecnt) {
            active_mic_ctrls = stdev.mic_ctrl.main_mic_ctlname;
            ctrl_values = stdev.mic_ctrl.main_mic_ctlvalue;

            if (set_mic_mixer_ctrls(vtsMixerHandle, active_mic_ctrls, ctrl_values,
                                    stdev.mic_ctrl.main_mic_ctrl_cnt, !flag)) {
                printf("%s: %s MIC control configuration Failed", __func__,
                        flag ? "Enabling" : "Disabling");
                mixer_close(vtsMixerHandle);
                vtsMixerHandle = NULL;
                return -EINVAL;
            }
            printf("%s: %s MIC Controls ", __func__, flag ? "Enable" : "Disable");
        }

        if (flag) {
            dmic_usagecnt++;
            printf("Dmic Enabled usage count %d \n", dmic_usagecnt);
        }
    } else{
        printf("%s: Failed to open mixer \n", __func__);
        return -EINVAL;
    }

    return ret;
}

void check_vts_state(struct vts_hw_device *vts_dev)
{
    if (vts_dev->vts_state != VTS_RECOGNITION_STOPPED_STATE) {
        vts_recognition_stop(vts_dev);
    } else if (vts_dev->vts_rec_state != VTS_RECORD_STOPPED_STATE) {
        vts_record_stop(vts_dev);
    } else {
        printf("Is is not in any state\n");
    }
    printf("%s: Exit \n", __func__);
    return;
}

// The vts_dev should be locked when you call this function.
static int fetch_streaming_buffer(struct vts_hw_device *vts_dev, int cap_mode)
{
    int ret = 0;
    unsigned int flags, flags_out, frames;
    int i = 0;
    struct pcm *vcap_pcm = NULL;
    char *streaming_buf;
    unsigned int pcmnode = (cap_mode == VTS_TRIGGERED_CAPTURE ? VTS_TRICAP_DEVICE_NODE : VTS_RECORD_DEVICE_NODE);
    struct pcm_config pcmconfig = {
        .channels = 1,
        .rate = 16000,
        .period_size = 160,
        .period_count = 1024,
        .format = PCM_FORMAT_S16_LE,
    };

    char file_name[30];

    streaming_buf = malloc(VTS_STREAMING_BUFFER_SIZE);
    if (!streaming_buf) {
        printf("Failed to malloc streaming buffer!!\n");
        goto out;
    }

    printf("%s: Fetching bytes \n", __func__);
    if (!vcap_pcm) {
        /* Open vts capture pcm node */
        pcmnode = (cap_mode == VTS_TRIGGERED_CAPTURE ? VTS_TRICAP_DEVICE_NODE : VTS_RECORD_DEVICE_NODE);
        flags = PCM_IN;
        vcap_pcm = pcm_open(VTS_SOUND_CARD, pcmnode, flags, &pcmconfig);
        if (vcap_pcm && !pcm_is_ready(vcap_pcm)) {
            printf("%s - FAILED to open VTS PCM Node : %d\n", __func__, pcmnode);
            /* Release VTS capture node */
            pcm_close(vcap_pcm);
            vcap_pcm = NULL;
            goto out;
        }

        sysfs_write("/sys/power/wake_lock", "vtshw-test", sizeof("vtshw-test"));
        printf("%s - Wake Lock Acquired\n", __func__);

        if (cap_mode == VTS_TRIGGERED_CAPTURE)
            strcpy(file_name, VTS_HW_TRIGGER_PCM_DUMP_FILE);
        else
            strcpy(file_name, VTS_HW_RECORDING_PCM_DUMP_FILE);

        st_pcm_dump_init(pcmconfig, file_name);

        i = 0;
        while(1) {
            ret = pcm_read(vcap_pcm, (void*)streaming_buf, (unsigned int)VTS_STREAMING_BUFFER_SIZE);
            if (ret == 0) {
                printf("%s - Captured %d samples\n", __func__, VTS_STREAMING_BUFFER_SIZE);
                st_pcm_dump_write((void*)streaming_buf, (unsigned int)VTS_STREAMING_BUFFER_SIZE);
            } else {
                printf("%s - Failed to capture requested samples %s \n", __func__, pcm_get_error(vcap_pcm));
                sleep(10);
            }

            if ((cap_mode == VTS_TRIGGERED_CAPTURE && vts_dev->thread_exit == true) ||
                (cap_mode == VTS_NORMAL_CAPTURE && vts_dev->rec_thread_exit == true))
                break;
            i++;
        }
        st_pcm_dump_close(vcap_pcm);

        /* Release VTS capture node */
        pcm_close(vcap_pcm);
        vcap_pcm = NULL;
        sysfs_write("/sys/power/wake_unlock", "vtshw-test", sizeof("vtshw-test"));
        printf("%s - Wake Lock Released\n", __func__);
    }

out:
    if (streaming_buf)
        free(streaming_buf);
    return ret;
}

/******** VTS Trigger Mode support function ***************/
static void *callback_thread_loop(void * context)
{
    struct pollfd poll_event;
    int err = 0;
    struct vts_hw_device *vts_dev = (struct vts_hw_device *)context;

    prctl(PR_SET_NAME, (unsigned long)"VTS callback", 0, 0, 0);

    poll_event.fd = vtsdev_fd;
    poll_event.events = POLLIN | POLLERR;
    poll_event.revents = 0;

    if (poll_event.fd == -1) {
        printf("Error opening %s", MISC_DEV);
        goto func_exit;
    }

    while (1) {
        printf("%s: Before poll \n", __func__);
        /* wait for VTS trigger Uevent */
        err = poll((struct pollfd *)&poll_event, 1, -1);
        printf("%s: After poll \n", __func__);

        if (poll_event.revents & POLLIN) {
            int event_type;
            if (ioctl(vtsdev_fd, VTSDRV_MISC_IOCTL_READ_EVENT_TYPE, &event_type) < 0) {
                printf("%s: VTSDRV_MISC_IOCTL_READ_EVENT failed\n", __func__);
                continue;
            }

            if (event_type & EVENT_STOP_POLLING) {
                printf("%s: EVENT_STOP_POLLING\n", __func__);
                break;
            } else if (event_type & EVENT_ERROR_RECOVERY) {
                printf("%s: hardfault issue happens\n", __func__);
                break;
            } else if (event_type & EVENT_RESTART) {
                printf("%s: re-start reqeust happen\n", __func__);
                break;
            } else if (event_type & EVENT_TRIGGERED) {
                int triggered_keyword = event_type & ~EVENT_TRIGGERED;

                vts_dev->vts_state = VTS_RECOGNITION_TRIGGERED_STATE;

                printf("%s: VTS Trigger received for model %s\n", __func__,
                       (triggered_keyword == ODM_MODEL) ? "ODM" : "GOOGLE");
                fetch_streaming_buffer(vts_dev, VTS_TRIGGERED_CAPTURE);
                printf("\n%s: Want to Continue...then Start Recognitoin again\n", __func__);
                goto found;

            }

         }  else {
            printf("%s: Poll returned %d\n", __func__, err);
        }
        if (vts_dev->thread_exit == true)
            break;
    }

found:
func_exit:
    if (vts_dev->thread_exit == false) {
        /* reset the state as Loaded by stopping the recognition,
             * so that we restart the recogniton again
             * if we have received the Hotwork notification */
        /* once callback thread is closed set vts_stop using sysfs */
        /* Set active keyphrase for voice recognition */
        set_mixer_ctrls(vtsMixerHandle, model_recognize_stop_ctlname,
                svoice_bixbyrecognize_stop_ctlvalue, MODEL_STOP_CONTROL_COUNT, false);
        vts_dev->vts_state = VTS_RECOGNITION_STOPPED_STATE;
        /* reset DMIC controls */
        set_dmic_ctrls(false);
    }
    vts_dev->thread_exit = false;
    printf("%s: Exit \n", __func__);
    return (void *)(long)err;
}

void vts_recognition_start(struct vts_hw_device *vts_dev, int model)
{
    int ret = 0;
    char model_bin[50];
    struct vts_ipc_msg ipc_msg;

    if (vts_dev->vts_active_model_type & model) {
        printf("Sound Model[%s] is already started\n", (model==ODM_MODEL) ? "ODM" : (model==GOOGLE_MODEL) ? "GOOGLE" : "ODM_RESERVED");
        return;
    }
    vts_dev->vts_active_model_type |= model;

    if (model == ODM_MODEL)
        strcpy(model_bin, SOUND_MODEL_ODM_BINARY);
    else
        strcpy(model_bin, SOUND_MODEL_OKGOOGLE_BINARY);

    pthread_mutex_lock(&vts_dev->lock);
    if(!(vts_dev->model_loaded & (0x1 << model))) {
        FILE *pfile = NULL;
        int rd_sz, bin_sz;
        char * data = NULL;
        /* Read model net binay file*/
        pfile = fopen(model_bin, "rb+");
        if (!pfile) {
            printf("Model Binary voice_dva_svoice.bin should be copied to \\data\\firmware folder \n");
            printf("Failed to Open Model Binary from [%s]\n", model_bin);
            goto error;
        } else {
            printf("Successfully [%s] file opened!! \n", model_bin);
        }

        fseek(pfile, 0L, SEEK_END);
        bin_sz = ftell(pfile);
        fseek(pfile, 0L, SEEK_SET);
        printf(" Model %s File size %d \n", model_bin, bin_sz);

        data = (char *)calloc(1, bin_sz);
        if (!data) {
            printf("Failed to allocated buffer of Size: %d\n", bin_sz);
            fclose(pfile);
            goto error;
        }
        /* read file data to allocated buffer */
        rd_sz = fread(data, 1, bin_sz, pfile);

        if (rd_sz != bin_sz) {
            printf("%s -  Failed to read data from %s file\n", __func__, model_bin);
            fclose(pfile);
            free(data);
            goto error;
        }

        /* Load net binary to VTS driver */
        load_modelbinary(data, rd_sz, model);

        fclose(pfile);
        free(data);
        vts_dev->model_loaded |= (0x1 << model);
    }
    if (vts_dev->vts_state == VTS_RECOGNITION_STOPPED_STATE) {

        /* configure DMIC controls */
        set_dmic_ctrls(true);

        /* Create Callback thread to catch VTS trigger notification through uevent*/
        vts_dev->thread_exit = false;
        pthread_create(&vts_dev->callback_thread, (const pthread_attr_t *) NULL,
                            callback_thread_loop, vts_dev);
    } else {
        printf("VTS Voice Recognition already running \n");
    }

        if (model == ODM_MODEL) {
            set_mixer_ctrls(vtsMixerHandle, model_recognize_start_ctlname,
                svoice_bixbyrecognize_start_ctlvalue, MODEL_START_CONTROL_COUNT, false);

            ipc_msg.msg = VTS_IRQ_AP_SET_MODE;
            ipc_msg.values[0] = ODM_TRIGGER_ALWAYS_MODE;
            ipc_msg.values[1] = 0;
            ipc_msg.values[2] = 0;

            if (ioctl(vtsdev_fd, VTSDRV_MISC_IOCTL_SET_PARAM, &ipc_msg) < 0) {
                printf("%s: Failed to set_parameter", __func__);
                return;
            }
        } else {
            set_mixer_ctrls(vtsMixerHandle, model_recognize_start_ctlname,
                hotword_recognize_start_ctlvalue, MODEL_START_CONTROL_COUNT, false);
        }

        vts_dev->vts_state = VTS_RECOGNITION_STARTED_STATE;
error:
    pthread_mutex_unlock(&vts_dev->lock);
    printf("%s: Exit Tar-size 1000ms\n", __func__);
    return;
}

void vts_recognition_stop(struct vts_hw_device *vts_dev)
{
    int val = 0;

    pthread_mutex_lock(&vts_dev->lock);
    if (vts_dev->vts_state != VTS_RECOGNITION_STOPPED_STATE) {
        vts_dev->thread_exit = true;
        if (vts_dev->vts_state != VTS_RECOGNITION_TRIGGERED_STATE) {
            /* Stop Callback thread first */
            if (ioctl(vtsdev_fd, VTSDRV_MISC_IOCTL_WRITE_EXIT_POLLING, &val) < 0) {
                printf("%s: VTS device VTSDRV_MISC_IOCTL_WRITE_EXIT_POLL failed\n", __func__);
                return;
            }
        }

        pthread_mutex_unlock(&vts_dev->lock);
        pthread_join(vts_dev->callback_thread, (void**)NULL);

        pthread_mutex_lock(&vts_dev->lock);
    } else {
        printf("Sound Model Recognition is NOT Started\n");
    }

    pthread_mutex_lock(&vts_dev->pcm_lock);

    set_mixer_ctrls(vtsMixerHandle, model_recognize_stop_ctlname,
        svoice_bixbyrecognize_stop_ctlvalue, MODEL_STOP_CONTROL_COUNT, false);
    set_mixer_ctrls(vtsMixerHandle, model_recognize_stop_ctlname,
        hotword_recognize_stop_ctlvalue, MODEL_STOP_CONTROL_COUNT, false);

    pthread_mutex_unlock(&vts_dev->pcm_lock);
    /* configure DMIC controls */
    set_dmic_ctrls(false);

    vts_dev->vts_state = VTS_RECOGNITION_STOPPED_STATE;
    vts_dev->vts_active_model_type = 0;
    vts_dev->model_loaded = 0;

    pthread_mutex_unlock(&vts_dev->lock);
    printf("%s: Exit \n", __func__);
    return;
}

/******** VTS Recording support function ***************/
static void *record_thread_loop(void * context)
{
    int err = 0;
    int i, n;
    struct vts_hw_device *vts_dev = (struct vts_hw_device *)context;

    prctl(PR_SET_NAME, (unsigned long)"VTSRecord", 0, 0, 0);

    printf("%s Started!!\n", __func__);
    fetch_streaming_buffer(vts_dev, VTS_NORMAL_CAPTURE); // VTS_TRIGGERED_CAPTURE,VTS_NORMAL_CAPTURE

    vts_dev->vts_rec_state = VTS_RECORD_STOPPED_STATE;

    if (vts_dev->rec_thread_exit == false) {
        /* reset DMIC controls */
        set_dmic_ctrls(false);
    }
    printf("%s: Exit \n", __func__);
    return (void *)(long)err;
}

void vts_record_start(struct vts_hw_device *vts_dev)
{
    pthread_mutex_lock(&vts_dev->lock);
    if (vts_dev->vts_rec_state == VTS_RECORD_STOPPED_STATE) {
        /* configure DMIC controls */
        set_dmic_ctrls(true);

        /* Create recorrd thread to capture data from VTS IP*/
        vts_dev->rec_thread_exit = false;
        pthread_create(&vts_dev->rec_thread, (const pthread_attr_t *) NULL,
                            record_thread_loop, vts_dev);

        vts_dev->vts_rec_state = VTS_RECORD_STARTED_STATE;
    } else {
        printf("VTS Recording Already started \n");
    }
    pthread_mutex_unlock(&vts_dev->lock);
    printf("%s: Exit \n", __func__);
    return;
}

void vts_record_stop(struct vts_hw_device *vts_dev)
{
    pthread_mutex_lock(&vts_dev->lock);
    if (vts_dev->vts_rec_state == VTS_RECORD_STARTED_STATE) {
        /* Stop record thread first */
        vts_dev->rec_thread_exit = true;
        pthread_mutex_unlock(&vts_dev->lock);

        pthread_join(vts_dev->rec_thread, (void**)NULL);

        vts_dev->vts_rec_state = VTS_RECORD_STOPPED_STATE;

        /* reset DMIC controls */
        set_dmic_ctrls(false);
        pthread_mutex_lock(&vts_dev->lock);

    } else {
        printf("VTS Recording NOT started yet \n");
    }
    pthread_mutex_unlock(&vts_dev->lock);
    printf("%s: Exit \n", __func__);
    return;
}

void set_parameter(struct vts_hw_device *vts_dev)
{
    struct vts_ipc_msg ipc_msg;

    printf("send IPC command\n");
    printf("msg : ");
    scanf("%d", &ipc_msg.msg);
    printf("value[0](hex) : 0x");
    scanf("%x", &ipc_msg.values[0]);
    printf("value[1](hex) : 0x");
    scanf("%x", &ipc_msg.values[1]);
    printf("value[2](hex) : 0x");
    scanf("%x", &ipc_msg.values[2]);

    if (ioctl(vtsdev_fd, VTSDRV_MISC_IOCTL_SET_PARAM, &ipc_msg) < 0) {
        printf("Failed to set_parameter");
    }
}

/****************** Unit test main function *************************/

void print_options(struct vts_hw_device *vts_dev __unused)
{
    printf("********************** Generic Dual VA VTS HW Test ***********************\n");
    printf("********************** MMAP interface for Model Binary loading ***********************\n");
    printf("1. Voice Bixby Recoginition Start\n");
    printf("2. Voice Google Recoginition Start\n");
    printf("3. Voice Recoginition Stop\n");
    printf("4. VTS Record Start\n");
    printf("5. VTS Record Stop\n");
    printf("6. Set Parameter\n");
    printf("7. Exit - VTS Test Application\n");
    printf("****************************************************************!\n");
    printf("Enter an Option: \n");
    return;
}

int main(void)
{
    struct vts_hw_device *vts_dev = NULL;
    int option, i;
    int ret = EXIT_SUCCESS;
    char pstr[50];
    struct pollfd fds[1];

    vts_dev = &g_vtsdev;
    dmic_usagecnt = 0;

    /* Open VTS Misc device for loading Model binary through MMAP interface */
    vtsdev_fd = open("/dev/vts_fio_dev", O_RDWR);
    if (vtsdev_fd < 0) {
        printf("%s: Failed to open VTS-Misc device %d\n", __func__,errno);
        return -EINVAL;
    }

    /* memory map VTS misc driver */
    mapped_addr = mmap(NULL, VTSDRV_MISC_MODEL_BIN_MAXSZ, PROT_READ | PROT_WRITE, MAP_SHARED, vtsdev_fd, 0);
    if (mapped_addr == MAP_FAILED) {
        printf("%s: Unable to MMAP VTS Model downloadable memory \n", __func__);
        close(vtsdev_fd);
        return -EINVAL;
    }

    /* open mixer control */
    vtsMixerHandle = mixer_open(VTS_MIXER_CARD);
    if (!vtsMixerHandle) {
        printf("%s: Failed to open mixer \n", __func__);
        close(vtsdev_fd);
        return -EINVAL;
    }

    pthread_mutex_lock(&vts_dev->lock);

    vts_dev->vts_state = VTS_RECOGNITION_STOPPED_STATE;
    vts_dev->vts_rec_state = VTS_RECORD_STOPPED_STATE;
    vts_dev->thread_exit = false;
    vts_dev->model_loaded = 0;

    memset(fds, 0, sizeof(struct pollfd));
    fds[0].events = POLLIN;
    fds[0].fd = fileno(stdin);

    st_xml_parser(&stdev);

    pthread_mutex_unlock(&vts_dev->lock);

    while (1) {
        print_options(vts_dev);
        ret = poll(fds, 1, -1);
        if (fds[0].revents & POLLIN) {
            if (fgets(pstr, 50, stdin) == NULL) {
                printf("Failed to get data from stdin \n");
                continue;
            }
        } else {
            printf("%s - Poll ret value %d\n", __func__, ret);
            continue;
        }
        option = atoi(pstr);
        printf("%s - Selected option %d\n", __func__, option);
        /* Called corresponding function based on Option selected */
        switch (option) {
        case MENU_START_BIXBY_RECOGNITION: /* Start loaded sound Model Recognition */
            vts_recognition_start(vts_dev, ODM_MODEL);
            break;
        case MENU_START_GOOGLE_RECOGNITION:
            vts_recognition_start(vts_dev, GOOGLE_MODEL);
            break;
        case MENU_STOP_RECOGNITION: /* Stop loaded sound Model Recognition */
            vts_recognition_stop(vts_dev);
            break;
        case MENU_START_RECORD:
            vts_record_start(vts_dev);
            break;
        case MENU_STOP_RECORD:
            vts_record_stop(vts_dev);
            break;
        case MENU_SET_PARAM:
            set_parameter(vts_dev);
            break;
        case MENU_EXIT:
            check_vts_state(vts_dev);
            printf("VTS HW Testing completed\n");
            break;
        default:
            printf("UNSUPPORTED Option - Try again !\n");
            break;
        }

        if (option == MENU_EXIT)
            break;
        option = 0;
    }

    if (vtsMixerHandle) {
        mixer_close(vtsMixerHandle);
        vtsMixerHandle = NULL;
    }

    close(vtsdev_fd);

    return EXIT_SUCCESS;
}
