/*
 * Copyright (C) 2017 The Android Open Source Project
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
 * @file            : sound_trigger_hw.c
 * @brief           : Sound Trigger primary HAL implmentation
 * @author          : Palli Satish Kumar Reddy (palli.satish@samsung.com)
 * @version         : 1.0
 * @history
 *   2017.02.20     : Create
 */

#define LOG_TAG "soundtrigger_hw_primary"
#define LOG_NDEBUG 0

//#define VERY_VERBOSE_LOGGING
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <log/log.h>
#include <cutils/uevent.h>
#include <dlfcn.h>

#include "sound_trigger_hw.h"
#include "vts.h"
#include "st_xml_parser.h"
#include "st_pcm_dump.h"

// A few function declarations to have some sort of logical ordering of code below.
static void *callback_thread_loop(void *context);
static void stdev_join_callback_thread(struct sound_trigger_device *stdev,
                                       bool keep_vts_powered);
static int stdev_stop_recognition_l(struct sound_trigger_device *stdev,
                                    sound_model_handle_t handle);
static void handle_stop_recognition_l(struct sound_trigger_device *stdev);
static int sound_trigger_open_recording_l(struct sound_trigger_device *stdev);
static int sound_trigger_close_recording_l(struct sound_trigger_device *stdev);

__attribute__ ((visibility ("default")))
int sound_trigger_headset_status(int is_connected);
__attribute__ ((visibility ("default")))
int sound_trigger_notify_ahal_record_status(int state);
__attribute__ ((visibility ("default")))
int sound_trigger_voicecall_status(int callstate);
__attribute__ ((visibility ("default")))
int sound_trigger_update_ahal_input_status(bool state);

extern void stdev_odm_init(struct sound_trigger_device *stdev);
extern int stdev_odm_set_power(struct sound_trigger_device *stdev, struct sound_model_info *sm_info, bool enable);
extern void stdev_odm_parse_extra_config_data(struct sound_trigger_device *stdev,
                struct sound_model_info *sm_info, const char *key_value_pairs);

// Since there's only ever one sound_trigger_device, keep it as a global so that other people can
// dlopen this lib to get at the streaming audio.
static struct sound_trigger_device g_stdev = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .pcm_lock = PTHREAD_MUTEX_INITIALIZER,
};

// Utility function for configuration MIC mixer controls
int set_mic_mixer_ctrls(struct sound_trigger_device *stdev, char path_name[MAX_CNT][MAX_LEN],
                        int *path_ctlvalue, int ctrl_count, bool reverse) {
    int i = (reverse ? (ctrl_count - 1): 0);
    int ret = 0;
    struct mixer_ctl *mixerctl = NULL;

    if (stdev->mixer) {
        while(ctrl_count) {
            ALOGVV("%s, ctrl_count: %d Loop index: %d", __func__, ctrl_count, i);
            /* Get required control from mixer */
            mixerctl = mixer_get_ctl_by_name(stdev->mixer, path_name[i]);
            if (mixerctl) {
                /* Enable control */
                if (path_ctlvalue)
                    ret = mixer_ctl_set_value(mixerctl, 0, path_ctlvalue[i]);
                else
                    ret = mixer_ctl_set_value(mixerctl, 0, 0);

                if (ret) {
                    ALOGE("%s: %s Failed to configure\n", __func__, path_name[i]);
                    ret = -EINVAL;
                    break;
                } else {
                    ALOGV("%s: %s configured value: %d\n", __func__, path_name[i],
                         (path_ctlvalue ? path_ctlvalue[i] : 0));
                }
            } else {
                ALOGE("%s: %s control doesn't exist\n", __func__, path_name[i]);
            }

            ctrl_count--;
            if (reverse)
                i--;
            else
                i++;
        }
    } else{
        ALOGE("%s: Failed to open mixer\n", __func__);
        return -EINVAL;
    }

    return ret;
}

int set_mixer_ctrls(
        struct sound_trigger_device *stdev,
        char *path_name[],
        int *path_ctlvalue,
        int ctrl_count,
        bool reverse)
{
    int i = (reverse ? (ctrl_count - 1): 0);
    int ret = 0;
    struct mixer_ctl *mixerctl = NULL;

    ALOGV("%s, path: %s", __func__, path_name[0]);

    if (stdev->mixer) {
        //for (i=0; i < ctrl_count; i++) {
        while(ctrl_count) {
            ALOGVV("%s, ctrl_count: %d Loop index: %d", __func__, ctrl_count, i);
            /* Get required control from mixer */
            mixerctl = mixer_get_ctl_by_name(stdev->mixer, path_name[i]);
            if (mixerctl) {
                /* Enable the control */
                if (path_ctlvalue)
                    ret = mixer_ctl_set_value(mixerctl, 0, path_ctlvalue[i]);
                else
                    ret = mixer_ctl_set_value(mixerctl, 0, 0);

                if (ret) {
                    ALOGE("%s: %s Failed to configure\n", __func__, path_name[i]);
                    ret = -EINVAL;
                    break;
                } else {
                    ALOGV("%s: %s configured value: %d\n", __func__, path_name[i],
                        (path_ctlvalue ? path_ctlvalue[i] : 0));
                }
             } else {
                ALOGE("%s: %s control doesn't exist\n", __func__, path_name[i]);
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
        ALOGE("%s: Failed to open mixer\n", __func__);
        return -EINVAL;
    }

    return ret;
}

int vts_set_parameters(
        struct sound_trigger_device *stdev,
        int msg, int arg0, int arg1, int arg2)
{
    struct vts_ipc_msg ipc_msg;
    int ret = 0;

    ALOGD("%s: msg:%d, values:0x%x 0x%x 0x%x", __func__, msg, arg0, arg1, arg2);

    ipc_msg.msg = msg;
    ipc_msg.values[0] = arg0;
    ipc_msg.values[1] = arg1;
    ipc_msg.values[2] = arg2;

    if (ioctl(stdev->vtsdev_fd, VTSDRV_MISC_IOCTL_SET_PARAM, &ipc_msg) < 0) {
        ALOGE("%s: Failed to set_parameter", __func__);
        ret = -EINVAL;
    }

    return ret;
}

// Utility function for loading model binary to kernel through mmap interface
int vts_load_sound_model(
    struct sound_trigger_device *stdev,
    struct sound_model_info *sm_info)
{
    int ret = 0;
    struct vts_model_bin_info *sm_config = &sm_info->sm_config;

    ALOGV("%s: Actual size %lld", __func__, sm_config->actual_sz);

    /* Copy model binary to VTS mapped address */
    memcpy(stdev->mapped_addr, sm_info->sm_data,
            sm_config->actual_sz);

    ALOGV("%s: %s Model loaded", __func__, sm_info->name);

    if (sm_config->actual_sz > sm_config->max_sz) {
        ALOGW("%s: buffer overflow Model size[%d] > Mapped bufsize[%d]",
                __func__, sm_config->actual_sz, sm_config->max_sz);
        sm_config->actual_sz = sm_config->max_sz;
    }

    /* Update model binary information to VTS misc driver */
    if (ioctl(stdev->vtsdev_fd, VTSDRV_MISC_IOCTL_LOAD_SOUND_MODEL, sm_config) < 0) {
        ALOGE("%s: VTS device IOCTL failed", __func__);
        return -EINVAL;
    }

    return ret;
}

static void sound_trigger_set_event_common_info(
        struct sound_trigger_device *stdev,
        struct sound_trigger_recognition_event *event_common,
        int index,
        int sm_type)
{
    event_common->status = RECOGNITION_STATUS_SUCCESS;
    event_common->type = sm_type;
    event_common->model = stdev->sm_info[index].model_handle;
    event_common->capture_available = true;
    event_common->trigger_in_data = false;
    event_common->audio_config = AUDIO_CONFIG_INITIALIZER;
    event_common->audio_config.sample_rate = 16000;
    event_common->audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
    event_common->audio_config.format = AUDIO_FORMAT_PCM_16_BIT;
}

// Utility function for allocating a hotword recognition event. Caller receives ownership of
// allocated struct.
static struct sound_trigger_recognition_event *sound_trigger_event_alloc(
        struct sound_trigger_device *stdev, int index)
{
    struct sound_trigger_phrase_recognition_event *event =
            (struct sound_trigger_phrase_recognition_event *)calloc(
                    1, sizeof(struct sound_trigger_phrase_recognition_event));
    if (!event)
        return NULL;

    if (stdev->sm_info[index].st_config) {
        unsigned int i;

        event->num_phrases = stdev->sm_info[index].st_config->num_phrases;
        if (event->num_phrases > SOUND_TRIGGER_MAX_PHRASES)
            event->num_phrases = SOUND_TRIGGER_MAX_PHRASES;

        for (i=0; i < event->num_phrases; i++)
            memcpy(&event->phrase_extras[i], &stdev->sm_info[index].st_config->phrases[i],
                   sizeof(struct sound_trigger_phrase_recognition_extra));
    }

    event->num_phrases = 1;
    event->phrase_extras[0].confidence_level = 100;
    event->phrase_extras[0].num_levels = 1;
    event->phrase_extras[0].levels[0].level = 100;
    event->phrase_extras[0].levels[0].user_id = 0;
    // Signify that all the data is coming through streaming, not through the
    // buffer.

    sound_trigger_set_event_common_info(stdev, &event->common, index, SOUND_MODEL_TYPE_KEYPHRASE);

    return &event->common;
}

// Utility function for allocating a hotsound recognition event. Caller receives ownership of
// allocated struct.
static struct sound_trigger_recognition_event *sound_trigger_generic_event_alloc(
        struct sound_trigger_device *stdev, int index)
{
    struct sound_trigger_generic_recognition_event *event =
            (struct sound_trigger_generic_recognition_event *)calloc(
                    1, sizeof(struct sound_trigger_generic_recognition_event));

    if (!event)
        return NULL;

    sound_trigger_set_event_common_info(stdev, &event->common, index, SOUND_MODEL_TYPE_GENERIC);

    return &event->common;
}

static int stdev_get_properties(
        const struct sound_trigger_hw_device *dev,
        struct sound_trigger_properties *properties)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    int google_version = 0;

    ALOGI("%s", __func__);
    if (stdev == NULL || properties == NULL) {
        return -EINVAL;
    }

    pthread_mutex_lock(&stdev->lock);
    if (ioctl(stdev->vtsdev_fd, VTSDRV_MISC_IOCTL_READ_GOOGLE_VERSION, &google_version) < 0) {
        ALOGE("%s: VTSDRV_MISC_IOCTL_READ_GOOGLE_VERSION failed", __func__);
        //return -EINVAL;
    }
    pthread_mutex_unlock(&stdev->lock);
    ALOGI("%s Google Version : %d", __func__, google_version);
    memcpy(properties, &hw_properties, sizeof(struct sound_trigger_properties));
    properties->version = google_version;
    return 0;
}

struct sound_trigger_properties_header* stdev_get_properties_extended(
        const struct sound_trigger_hw_device *dev) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    int google_version = 0;
    char version[10] = {0, };
    char uuid[40] = {0, };

    ALOGI("%s: enter", __func__);

    if (stdev == NULL)
        return NULL;

    pthread_mutex_lock(&stdev->lock);
    if (ioctl(stdev->vtsdev_fd, VTSDRV_MISC_IOCTL_READ_GOOGLE_VERSION, &google_version) < 0) {
        ALOGE("%s: VTSDRV_MISC_IOCTL_READ_GOOGLE_VERSION failed", __func__);
        //return -EINVAL;
    }

    if (ioctl(stdev->vtsdev_fd, VTSDRV_MISC_IOCTL_READ_GOOGLE_UUID, uuid) < 0) {
        ALOGE("%s: VTSDRV_MISC_IOCTL_READ_GOOGLE_UUID failed", __func__);
        //return -EINVAL;
    }

    pthread_mutex_unlock(&stdev->lock);

    snprintf(hw_properties_1_3.supported_model_arch, SOUND_TRIGGER_MAX_STRING_LEN,
             "%d, %s", google_version, uuid);
    ALOGI("%s: supported_model: %s", __func__, hw_properties_1_3.supported_model_arch);

end:
    return &hw_properties_1_3;
}

// If enable_mic = 0, then the VTS MIC controls disabled.
// Must be called with the stdev->lock held.
static void stdev_vts_set_mic(struct sound_trigger_device *stdev, int enable_mic)
{
    if (enable_mic != 0) {
        /* Check whether MIC controls are configured or not, if not configure first */
        /* FXIME: add condition to check whether MAIN or HEADSET MIC should be configured */
        if (!stdev->is_mic_configured) {
#if SUPPORT_AUDIO_ROUTE
            /* stdev->active_mic is VTS_MAIN_MIC or VTS_HEADSET_MIC. */
            stdev->notify_set_vts_route(true, stdev->active_mic);
#else
            if (stdev->active_mic == VTS_MAIN_MIC) {
                set_mic_mixer_ctrls(stdev, stdev->mic_ctrl.main_mic_ctlname,
                                    stdev->mic_ctrl.main_mic_ctlvalue, stdev->mic_ctrl.main_mic_ctrl_cnt, false);

            } else if (stdev->active_mic == VTS_HEADSET_MIC) {
                set_mic_mixer_ctrls(stdev, stdev->mic_ctrl.headset_mic_ctlname,
                                    stdev->mic_ctrl.headset_mic_ctlvalue, stdev->mic_ctrl.headset_mic_ctrl_cnt, false);
            }
#endif
            stdev->is_mic_configured = 1;
            ALOGD("%s: Enable MIC Controls ", __func__);
        }
    } else {
        /* Reset MIC controls for disabling VTS */
        if (stdev->is_mic_configured) {
#if SUPPORT_AUDIO_ROUTE
            stdev->notify_set_vts_route(false, stdev->active_mic);
#else
            if (stdev->active_mic == VTS_MAIN_MIC) {
                set_mic_mixer_ctrls(stdev, stdev->mic_ctrl.main_mic_ctlname, NULL,
                                    stdev->mic_ctrl.main_mic_ctrl_cnt, true);
            } else if (stdev->active_mic == VTS_HEADSET_MIC) {
                set_mic_mixer_ctrls(stdev, stdev->mic_ctrl.headset_mic_ctlname, NULL,
                                    stdev->mic_ctrl.headset_mic_ctrl_cnt, true);
            }
#endif
            stdev->is_mic_configured = 0;
            ALOGD("%s: Disable MIC Controls ", __func__);
        }
    }

    return;
}

static int32_t stdev_set_parameter(struct sound_trigger_device *dev, sound_model_handle_t handle,
        sound_trigger_model_parameter_t param, int value) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;

    ALOGI("%s: enter", __func__);

    return 0;
}

static int32_t stdev_get_parameter(struct sound_trigger_device *dev, sound_model_handle_t handle,
        sound_trigger_model_parameter_t param, int *value) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;

    ALOGI("%s: enter", __func__);

    return 0;
}

static int32_t stdev_query_parameter(struct sound_trigger_device *dev, sound_model_handle_t handle,
        sound_trigger_model_parameter_t param, sound_trigger_model_parameter_range_t *param_range) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;

    ALOGI("%s: enter", __func__);

    return 0;
}

static int stdev_pcm_open(struct sound_trigger_device *stdev,
        struct pcm **pcm_dev, int pcm_id)
{
    int ret = 0;

    pthread_mutex_lock(&stdev->pcm_lock);

    if (*pcm_dev)
        pcm_close(*pcm_dev);

    *pcm_dev = pcm_open(VTS_SOUND_CARD, pcm_id, PCM_IN, &pcm_config_vt_capture);

    if (*pcm_dev && !pcm_is_ready(*pcm_dev)) {
        pcm_close(*pcm_dev);
        ALOGE("%s: failed to open PCM, pcm_id:%d",
                __func__, pcm_id);
        ret = -EFAULT;
        goto exit;
    }

    if (stdev->pcm_dump_enabled) {
        if (pcm_id == VTS_TRICAP_DEVICE_NODE)
            st_pcm_dump_init(pcm_config_vt_capture, ST_TRIGGER_PCM_DUMP_FILE);
        else if (pcm_id == VTS_RECORD_DEVICE_NODE)
            st_pcm_dump_init(pcm_config_vt_capture, ST_RECORDING_PCM_DUMP_FILE);
    }

    ALOGD("%s: success, pcm_id: %d", __func__, pcm_id);
exit:
    pthread_mutex_unlock(&stdev->pcm_lock);

    return ret;
}

static int stdev_pcm_read(struct sound_trigger_device *stdev,
        struct pcm *pcm_dev,
        void *buffer,
        size_t buffer_len)
{
    int ret = 0;

    pthread_mutex_lock(&stdev->pcm_lock);

    if (pcm_dev) {
        ret = pcm_read(pcm_dev, buffer, buffer_len);
        if (ret < 0) {
            ALOGE("%s: pcm_read failed, ret: %d", __func__, ret);
            goto exit;
        }
    } else {
        ALOGE("%s: PCM node is NULL", __func__);
        ret = -EFAULT;
        goto exit;
    }

    if (stdev->pcm_dump_enabled)
        st_pcm_dump_write(buffer, buffer_len);

exit:
    pthread_mutex_unlock(&stdev->pcm_lock);

    return ret;
}

static void stdev_pcm_close(struct sound_trigger_device *stdev, struct pcm **pcm_dev)
{
    pthread_mutex_lock(&stdev->pcm_lock);

    if (*pcm_dev) {
        ALOGD("%s: try to close pcm", __func__);
        if (stdev->pcm_dump_enabled)
            st_pcm_dump_close(*pcm_dev);

        pcm_stop(*pcm_dev);
        pcm_close(*pcm_dev);
        *pcm_dev = NULL;
    }

    pthread_mutex_unlock(&stdev->pcm_lock);
}

// If enabled_algorithms = 0, then the VTS will be turned off. Otherwise, treated as a bit mask for
// which algorithms should be enabled on the VTS. Must be called with the stdev->lock held.
static void stdev_vts_set_power(
        struct sound_trigger_device *stdev,
        int enabled_algorithms)
{
    int i = 0;
    int ret = 0;

    ALOGD("%s: enabled: %d", __func__, enabled_algorithms);

    stdev->is_streaming = 0;

    stdev_pcm_close(stdev, &stdev->streaming_pcm);
    stdev->is_seamless_recording = false;

    if (enabled_algorithms != 0) {
        /* Configure MIC controls first */
        stdev_vts_set_mic(stdev, true);

        /* Start recognition of bit masked algorithms */
        for (i = 0; i < stdev->num_sound_model; i++) {
            if (enabled_algorithms & stdev->sm_info[i].bitmask) {
                if (stdev->sm_info[i].support_odm_function) {
                    if (stdev_odm_set_power(stdev, &stdev->sm_info[i], true)) {
                         ALOGE("%s: Error odm set power on", __func__);
                         continue;
                    }
                } else {
                    /* load sound model */
                    if (vts_load_sound_model(stdev, &stdev->sm_info[i])) {
                        ALOGE("%s: Failed to load sound model", __func__);
                        continue;
                    }

                    hotword_recognize_start_ctlvalue[0] = stdev->sm_info[i].id;
                    hotword_recognize_start_ctlvalue[2] = stdev->sm_info[i].backlog_size;

                    if (set_mixer_ctrls(stdev, model_recognize_start_ctlname,
                        hotword_recognize_start_ctlvalue, MODEL_START_CONTROL_COUNT, false)) {
                        ALOGE("%s: %s Model recognization start Failed", __func__, stdev->sm_info[i].name);
                        continue;
                    }
                    stdev->recognize_started |= stdev->sm_info[i].bitmask;

                    if(stdev->notify_sthal_status)
                        stdev->notify_sthal_status(MODEL_RECOGNIZE_STARTED);
                    ALOGD("%s: %s Model recognization started & Notified to AudioHAL", __func__, stdev->sm_info[i].name);
                }

                for (int j = 0; j < stdev->sm_info[i].param_cnt && j < MAX_PARAM; j++) {
                    if (vts_set_parameters(stdev, stdev->sm_info[i].param[j].msg,
                            stdev->sm_info[i].param[j].values[0],
                            stdev->sm_info[i].param[j].values[1],
                            stdev->sm_info[i].param[j].values[2])) {
                        ALOGW("%s: vts_set_parameters failed", __func__);
                    }
                }
            }
        }
    } else {
        /* Stop recognition of previous started models */
        if (stdev->recording_pcm) {
            ALOGD("%s: close recording", __func__);

            ret = sound_trigger_close_recording_l(stdev);
            if (ret) {
                ALOGW("%s: close recording error, ret: %d", __func__, ret);
            }
        }

        for (i = 0; i < stdev->num_sound_model; i++) {
            if (stdev->recognize_started & stdev->sm_info[i].bitmask) {
                if (stdev->sm_info[i].support_odm_function) {
                    if (stdev_odm_set_power(stdev, &stdev->sm_info[i], false)) {
                        ALOGE("%s: Error odm set power off", __func__);
                        continue;
                    }
                } else {
                    hotword_recognize_stop_ctlvalue[0] = stdev->sm_info[i].id;

                    if (set_mixer_ctrls(stdev, model_recognize_stop_ctlname,
                        hotword_recognize_stop_ctlvalue, MODEL_STOP_CONTROL_COUNT, false)) {
                        ALOGE("%s: %s Model recognization stop Failed", __func__, stdev->sm_info[i].name);
                        continue;
                    }
                    stdev->recognize_started &= ~(stdev->sm_info[i].bitmask);

                    if(stdev->notify_sthal_status)
                        stdev->notify_sthal_status(MODEL_RECOGNIZE_STOPPED);
                    ALOGD("%s: %s Model recognization stopped & Notified to AudioHAL", __func__, stdev->sm_info[i].name);
                }
            }
        }

        if (!stdev->recognize_started && !stdev->is_recording) {
            /* Reset MIC controls for disabling VTS */
            stdev_vts_set_mic(stdev, false);
        }
    }

    return;
}

static int stdev_init_mixer(struct sound_trigger_device *stdev)
{
    int ret = -1;
    int retry = 30;

    ALOGV("%s: enter", __func__);

    do {
        stdev->mixer = mixer_open(VTS_MIXER_CARD);
        if (stdev->mixer) {
            ret = 0;
            goto end;
        } else {
            usleep(100000);
            retry--;
            ALOGW("%s: Mixer open failed retrying cnt: %d", __func__, (retry % 30));
        }
    } while (retry);

    if (stdev->mixer)
        mixer_close(stdev->mixer);

end:
    return ret;
}

static void stdev_close_mixer(struct sound_trigger_device *stdev)
{
    ALOGV("%s", __func__);

    stdev_vts_set_power(stdev, 0);
    stdev_join_callback_thread(stdev, false);
    mixer_close(stdev->mixer);
}

static int stdev_set_initial_mixer(struct sound_trigger_device *stdev)
{
    int forcereset = 1;

    if (set_mixer_ctrls(stdev, vts_forcereset_ctlname, &forcereset, 1, false)) {
        ALOGE("%s: VTS Force Reset configuration Failed", __func__);
        return -1;
    }

    set_mixer_ctrls(stdev, vts_mic_cnt_ctlname, &stdev->num_supported_mic, 1, false);
    set_mixer_ctrls(stdev, vts_sysclk_div_cltname, &stdev->sysclk_div, 1, false);
    set_mixer_ctrls(stdev, vts_support_silent_reset_ctlname, &stdev->support_silent_reset, 1, false);

    return 0;
}

// Starts the callback thread if not already running. Returns 0 on success, or a negative error code
// otherwise. Must be called with the stdev->lock held.
static int stdev_start_callback_thread(struct sound_trigger_device *stdev)
{
    ALOGV("%s", __func__);

    if (stdev->callback_thread_active) {
        ALOGV("%s callback thread is already running", __func__);
        return 0;
    }
    int ret = 0;

    stdev->callback_thread_active = true;
    ret = pthread_create(&stdev->callback_thread, (const pthread_attr_t *) NULL,
                         callback_thread_loop, stdev);
    if (ret) {
        goto err;
    }
    return 0;

err:
    stdev->callback_thread_active = false;
    return -ret;
}


// If the callback thread is active, stops it and joins the thread. Also closes all resources needed
// to talk to the thread. If keep_vts_powered is false, then the VTS will be shut down after the
// thread joins. Must be called with the stdev->lock held.
static void stdev_join_callback_thread(
        struct sound_trigger_device *stdev,
        bool keep_vts_powered)
{
    int val = 0;
    ALOGV("%s", __func__);

    if (stdev->callback_thread_active) {
        // If the thread is active, send the termination signal and join up with it. Also, turn off
        // the VTS, since we're no longer listening for events. callback_thread_active will be set
        // to false when the thread joins.
        if (ioctl(stdev->vtsdev_fd, VTSDRV_MISC_IOCTL_WRITE_EXIT_POLLING, &val) < 0) {
            ALOGE("%s: VTS device VTSDRV_MISC_IOCTL_WRITE_EXIT_POLL failed", __func__);
            return;
        }

        if (stdev->recog_cbstate != RECOG_CB_CALLED) {
            pthread_mutex_unlock(&stdev->lock);
            pthread_join(stdev->callback_thread, (void **)NULL);
            pthread_mutex_lock(&stdev->lock);
        } else {
            ALOGI("%s: callback is running, skip join thread", __func__);
        }
    }
    if (!keep_vts_powered) {
        stdev_vts_set_power(stdev, 0);
    }
}

// Returns a bitmask where each bit is set if there is a recognition callback function set for that
// index. Must be called with the stdev->lock held.
static inline int stdev_active_callback_bitmask(struct sound_trigger_device* stdev)
{
    int bitmask = 0;
    int i;

    for (i = 0; i < stdev->num_sound_model; ++i) {
        if (stdev->sm_info[i].recognition_callback != NULL) {
            bitmask |= stdev->sm_info[i].bitmask;
        }
    }

    ALOGV("%s: (%d)", __func__, bitmask);

    return bitmask;
}

static void stdev_vts_reset(struct sound_trigger_device* stdev)
{
    int active_bitmask = stdev_active_callback_bitmask(stdev);
    if (active_bitmask || stdev->is_recording) {
        ALOGI("%s: reset VTS", __func__);
        stdev_vts_set_power(stdev, 0);
        stdev_vts_set_power(stdev, active_bitmask);
    }
}

static void stdev_pcm_close_for_reset(struct sound_trigger_device* stdev)
{
    if (stdev->recording_pcm) {
        ALOGI("%s: recording pcm close, forcefully", __func__);
        pcm_stop(stdev->recording_pcm);
        pcm_close(stdev->recording_pcm);
        stdev->recording_pcm = NULL;
    }

    return;
}

static void *callback_thread_loop(void *context)
{
    struct sound_trigger_device *stdev =
               (struct sound_trigger_device *)context;
    struct sound_trigger_recognition_event *event = NULL;
    recognition_callback_t callbacks = NULL;
    sound_model_handle_t model = 0;
    void *cookies = NULL;
    int trigger_index = 0;
    int model_execstate = -1;
    struct pollfd poll_event;
    int kwDetected = 0;
    int err = 0;
    int i = 0;

    ALOGI("%s: enter", __func__);
    prctl(PR_SET_NAME, (unsigned long)"sound trigger callback", 0, 0, 0);

    pthread_mutex_lock(&stdev->lock);

    poll_event.fd = stdev->vtsdev_fd;
    poll_event.events = POLLIN|POLLERR;
    poll_event.revents = 0;

    stdev->recog_cbstate = RECOG_CB_NONE;

    while (1) {
        pthread_mutex_unlock(&stdev->lock);
        err = poll((struct pollfd *)&poll_event, 1, -1);
        pthread_mutex_lock(&stdev->lock);
        stdev->recog_cbstate = RECOG_CB_STARTED;
        if (err < 0) {
            ALOGE_IF(err < 0, "Error in poll: %d", err);
            break;
        }

        if (poll_event.revents & POLLIN) {
            int event_type;
            if (ioctl(stdev->vtsdev_fd, VTSDRV_MISC_IOCTL_READ_EVENT_TYPE, &event_type) < 0) {
                ALOGE("%s: VTSDRV_MISC_IOCTL_READ_EVENT failed", __func__);
                stdev_vts_reset(stdev);
                continue;
            }

            ALOGD("%s: POLLIN event occurs. event_type[0x%x]", __func__, event_type);

            if (event_type & EVENT_STOP_POLLING) {
                stdev->recog_cbstate = RECOG_CB_NONE;
                ALOGD("%s: stop polling", __func__);
                break;
            } else if (event_type & EVENT_ERROR_RECOVERY) {
                stdev->vts_status = ABNORMAL;
                ALOGI("%s: fault issue happens", __func__);

                stdev_pcm_close_for_reset(stdev);
                stdev_vts_reset(stdev);
                stdev->vts_status = NORMAL;
            } else if (event_type & EVENT_RESTART) {
                ALOGI("%s: re-start VTS", __func__);
                stdev_vts_reset(stdev);
            } else if (event_type & EVENT_TRIGGERED) {
                ALOGD("%s: onDetected!", __func__);

                int triggered_keyword;

                trigger_index = 0;
                err = 0;
                triggered_keyword = event_type & ~(EVENT_TRIGGERED);

                for (i = 0; i < stdev->num_sound_model; i++) {
                    if (triggered_keyword == stdev->sm_info[i].id) {
                        if (stdev->sm_info[i].is_generic)
                            event = sound_trigger_generic_event_alloc(stdev, i);
                        else
                            event = sound_trigger_event_alloc(stdev, i);

                        trigger_index = i;
                        break;
                    }
                }

                if (event != NULL) {
                    callbacks = stdev->sm_info[trigger_index].recognition_callback;
                    cookies = stdev->sm_info[trigger_index].recognition_cookie;
                    model_execstate = stdev->sm_info[trigger_index].model_execstate;
                    model = stdev->sm_info[trigger_index].model_handle;
                } else {
                    continue;
                }

                stdev->recog_cbstate = RECOG_CB_CALLED;
                kwDetected = 1;
            }
        }

        if (kwDetected == 1) {
            kwDetected = 0;
            if (event) {
                if (model_execstate == MODEL_STATE_RUNNING && callbacks != NULL) {
                    ALOGI("%s send callback model %d", __func__, trigger_index);
                    pthread_mutex_unlock(&stdev->lock);
                    callbacks(event, cookies);
                    pthread_mutex_lock(&stdev->lock);
                } else {
                    ALOGE("%s no callback for model %d", __func__, trigger_index);
                }
                free(event);
                event = NULL;

                ALOGI("%s %s Event Triggerred", __func__, stdev->sm_info[trigger_index].name);
                stdev->sm_info[trigger_index].recognition_callback = NULL;

                // Start reading data from the VTS while the upper levels do their thing.
                if (stdev->sm_info[trigger_index].model_execstate == MODEL_STATE_RUNNING &&
                    stdev->sm_info[trigger_index].st_config &&
                    stdev->sm_info[trigger_index].st_config->capture_requested) {
                    ALOGI("%s Streaming Enabled for %s", __func__, stdev->sm_info[trigger_index].name);
                    stdev->is_streaming = stdev->sm_info[trigger_index].bitmask;
                    //goto exit;
                } else {
                    /* Error handling if models stop-recognition failed */
                    for (i = 0; i < stdev->num_sound_model; i++) {
                        if ((stdev->sm_info[i].model_stopfailedhandle != HANDLE_NONE) ||
                            stdev->sm_info[trigger_index].model_execstate == MODEL_STATE_STOPABORT) {
                            handle_stop_recognition_l(stdev);
                            stdev->sm_info[trigger_index].model_execstate = MODEL_STATE_NONE;
                            ALOGI("%s stop-recognition error state handled", __func__);
                            break;
                        }
                    }

                    // If we're not supposed to capture data, power cycle the VTS and start
                    // whatever algorithms are still active.
                    stdev_vts_reset(stdev);
                }
                stdev->recog_cbstate = RECOG_CB_NONE;
            } else {
                ALOGE("%s: invalid trigger or out of memory, failure_callback", __func__);

                stdev->sm_info[trigger_index].recognition_callback = NULL;
                stdev->recog_cbstate = RECOG_CB_NONE;
            }
        }

        if (stdev_active_callback_bitmask(stdev) == 0) {
            ALOGI("%s: no active callback, terminate the thread loop", __func__);
            break;
        }
    }

    if (!stdev->is_streaming && !stdev->is_recording) {
        stdev_vts_set_power(stdev, 0);
    }

    stdev->callback_thread_active = false;
    stdev->recog_cbstate = RECOG_CB_NONE;
    pthread_mutex_unlock(&stdev->lock);
    ALOGI("%s: exit", __func__);

    return (void *)(long)err;
}

static struct sound_model_info *find_matched_sm_by_id(
        const struct sound_trigger_device *stdev,
        int id)
{
    int i = 0;

    for (i = 0; i < stdev->num_sound_model; i++) {
        if (id == stdev->sm_info[i].id)
            return &stdev->sm_info[i];
    }

    return NULL;
}

static struct sound_model_info *find_matched_sm_by_uuid(
        const struct sound_trigger_device *stdev,
        struct sound_trigger_sound_model *sound_model)
{
    int i = 0;

    for (i = 0; i < stdev->num_sound_model; i++) {
        if (!memcmp(&sound_model->vendor_uuid, &stdev->sm_info[i].uuid, sizeof(sound_trigger_uuid_t)))
            return &stdev->sm_info[i];
    }

    return NULL;
}

static int stdev_load_sound_model(
        const struct sound_trigger_hw_device *dev,
        struct sound_trigger_sound_model *sound_model,
        sound_model_callback_t callback,
        void *cookie,
        sound_model_handle_t *handle)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    struct sound_model_info *sm = NULL;
    int ret = 0;

    ALOGI("%s", __func__);
    pthread_mutex_lock(&stdev->lock);
    if (handle == NULL || sound_model == NULL) {
        ALOGE("%s: handle or sound_model pointer NULL error", __func__);
        ret = -EINVAL;
        goto exit;
    }
    if (sound_model->data_size == 0 ||
            sound_model->data_offset < sizeof(struct sound_trigger_sound_model)) {
        ALOGE("%s: Model data size [%d] or data offset [%d] expected offset [%zu]  invalid",
                    __func__, sound_model->data_size, sound_model->data_offset,
                    sizeof(struct sound_trigger_sound_model));
        ret =  -EINVAL;
        goto exit;
    }

    sm = find_matched_sm_by_uuid(stdev, sound_model);
    if (!sm) {
        ALOGE("%s Invalid UUID: {0x%x, 0x%x, 0x%x, 0x%x \n {0x%x 0x%x 0x%x 0x%x 0x%x 0x%x}}", __func__,
                sound_model->vendor_uuid.timeLow, sound_model->vendor_uuid.timeMid,
                sound_model->vendor_uuid.timeHiAndVersion, sound_model->vendor_uuid.clockSeq,
                sound_model->vendor_uuid.node[0], sound_model->vendor_uuid.node[1],
                sound_model->vendor_uuid.node[2], sound_model->vendor_uuid.node[3],
                sound_model->vendor_uuid.node[4], sound_model->vendor_uuid.node[5]);

        ret = -EINVAL;
        goto exit;
    }

    if (sound_model->type == SOUND_MODEL_TYPE_KEYPHRASE) {
        sm->is_generic = false;
    } else if (sound_model->type == SOUND_MODEL_TYPE_GENERIC) {
        sm->is_generic = true;
    } else {
        ALOGE("%s: Could not determine model type", __func__);
        ret = -EINVAL;
        goto exit;
    }

    if (sm->model_handle != -1) {
        ALOGE("%s: unknown Model type or already running", __func__);
        ret = -ENOSYS;
        goto exit;
    }

    if (sm->sm_data) {
        free(sm->sm_data);
        sm->sm_data = NULL;
    }
    sm->sm_data = (char *)malloc(sound_model->data_size);
    if (!sm->sm_data) {
        ALOGE("%s: Failed to allocate sm_data", __func__);
        ret = -EINVAL;
        goto exit;
    }
    memcpy(sm->sm_data, ((char *)sound_model) + sound_model->data_offset,
            sound_model->data_size);
    sm->sm_config.actual_sz = sound_model->data_size;

    sm->model_handle = sm->id;
    sm->sound_model_callback = callback;
    sm->sound_model_cookie = cookie;
    *handle = sm->model_handle;

exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static int stdev_unload_sound_model(
        const struct sound_trigger_hw_device *dev,
        sound_model_handle_t handle)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    struct sound_model_info *sm = NULL;
    int ret = 0;

    ALOGI("%s handle: %d", __func__, handle);
    pthread_mutex_lock(&stdev->lock);

    sm = find_matched_sm_by_id(stdev, handle);
    if (!sm) {
        ALOGE("%s: Can't find matched sound model", __func__);
        ret = -EINVAL;
        goto exit;
    }

    // If we still have a recognition callback, that means we should cancel the
    // recognition first.
    if (sm->recognition_callback != NULL ||
            (stdev->is_streaming & sm->bitmask)) {
        ret = stdev_stop_recognition_l(stdev, handle);
    }

    if (sm->sm_data) {
        free(sm->sm_data);
        sm->sm_data = NULL;
    }

    sm->model_handle = -1;

exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static int stdev_start_recognition(
        const struct sound_trigger_hw_device *dev,
        sound_model_handle_t handle,
        const struct sound_trigger_recognition_config *config,
        recognition_callback_t callback,
        void *cookie)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    int ret = 0;
    struct sound_model_info *sm = NULL;

    ALOGI("%s Handle %d", __func__, handle);

    pthread_mutex_lock(&stdev->lock);
    sm = find_matched_sm_by_id(stdev, handle);
    if (!sm) {
        ALOGE("%s: Can't find matched sound model", __func__);
        ret = -EINVAL;
        goto exit;
    }

    if (sm->recognition_callback != NULL) {
         ALOGW("%s:model recognition is already started Checking for error state", __func__);
         /* Error handling if models stop-recognition failed */
         if (sm->model_stopfailedhandle != HANDLE_NONE) {
             handle_stop_recognition_l(stdev);
             ALOGI("%s stop-recognition error state handled", __func__);
         }
    }

    // Copy the config for this handle.
    if (config) {
        if (sm->st_config) {
            free(sm->st_config);
        }
        sm->st_config = malloc(sizeof(*config));
        if (!sm->st_config) {
            ret = -ENOMEM;
            goto exit;
        }
        memcpy(sm->st_config, config, sizeof(*config));

        /* Check whether config has extra inforamtion */
        if (config->data_size > 0) {
            if (sm->support_odm_function) {
                char *params = (char*)config + sizeof(*config);

                stdev_odm_parse_extra_config_data(stdev, sm, params);
            }
        }
    }

    ret = stdev_start_callback_thread(stdev);
    if (ret) {
        goto exit;
    }

    sm->recognition_callback = callback;
    sm->recognition_cookie = cookie;

    // Reconfigure the VTS to run any algorithm that have a callback.
    if (!stdev->is_streaming || (stdev->is_streaming & sm->bitmask)) {
        if (stdev->voicecall_state == VOICECALL_STOPPED) {
            stdev_vts_reset(stdev);
        } else {
            ALOGI("%s: Voice call is in progress. Skip starting VTS \n", __func__);
        }
    }

    sm->model_stopfailedhandle = HANDLE_NONE;
    sm->model_execstate = MODEL_STATE_RUNNING;
    ALOGI("%s Handle Exit %d", __func__, handle);
exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static int stdev_start_recognition_extended(const struct sound_trigger_hw_device *dev,
        sound_model_handle_t handle, const struct sound_trigger_recognition_config_header *header,
        recognition_callback_t callback, void *cookie) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    struct sound_trigger_recognition_config_extended_1_3 *config = NULL;
    struct sound_model_info *sm = NULL;
    int ret = 0;

    ALOGI("%s Handle %d", __func__, handle);

    pthread_mutex_lock(&stdev->lock);

    sm = find_matched_sm_by_id(stdev, handle);
    if (!sm) {
        ALOGE("%s: Can't find matched sound model", __func__);
        ret = -EINVAL;
        goto exit;
    }

    config = (struct sound_trigger_recognition_config_extended_1_3 *)header;

    if (sm->recognition_callback != NULL) {
         ALOGW("%s:model recognition is already started Checking for error state", __func__);
         /* Error handling if models stop-recognition failed */
        if (sm->model_stopfailedhandle != HANDLE_NONE) {
            handle_stop_recognition_l(stdev);
            ALOGI("%s stop-recognition error state handled", __func__);
        }
    }

    // Copy the config for this handle.
    if (config) {
        if (sm->st_config) {
            free(sm->st_config);
        }
        sm->st_config = malloc(sizeof(config->base));
        if (!sm->st_config) {
            ret = -ENOMEM;
            goto exit;
        }

        memcpy(sm->st_config, &config->base, sizeof(config->base));

        /* Check whether config has extra inforamtion */
        if (config->base.data_size > 0) {
            if (sm->support_odm_function) {
                char *params = (char*)config +
                    sizeof(struct sound_trigger_recognition_config_extended_1_3);

                stdev_odm_parse_extra_config_data(stdev, sm, params);
            }
        }
    }

    ret = stdev_start_callback_thread(stdev);
    if (ret) {
        goto exit;
    }

    sm->recognition_callback = callback;
    sm->recognition_cookie = cookie;

    // Reconfigure the VTS to run any algorithm that have a callback.
    if (!stdev->is_streaming ||
            (stdev->is_streaming & sm->bitmask)) {
        if (stdev->voicecall_state == VOICECALL_STOPPED) {
            stdev_vts_reset(stdev);
        } else {
            ALOGI("%s: Voice call is in progress. Skip starting VTS \n", __func__);
        }
    }

    sm->model_stopfailedhandle = HANDLE_NONE;
    sm->model_execstate = MODEL_STATE_RUNNING;
    ALOGI("%s Handle Exit %d", __func__, handle);
exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static int stdev_stop_recognition(
        const struct sound_trigger_hw_device *dev,
        sound_model_handle_t handle)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    int ret = 0;

    ALOGI("%s Handle %d", __func__, handle);

    pthread_mutex_lock(&stdev->lock);
    ret = stdev_stop_recognition_l(stdev, handle);
    pthread_mutex_unlock(&stdev->lock);

    ALOGI("%s Handle Exit %d", __func__, handle);
    return ret;
}

static int stdev_stop_recognition_l(
        struct sound_trigger_device *stdev,
        sound_model_handle_t handle)
{
    struct sound_model_info *sm;
    ALOGV("%s", __func__);

    sm = find_matched_sm_by_id(stdev, handle);
    if (!sm) {
        ALOGE("%s: Can't find matched sound model", __func__);
        return -EINVAL;
    }

    if (sm->recognition_callback == NULL &&
            !(stdev->is_streaming & sm->bitmask)) {
        ALOGE("%s:model recognition is already stopped", __func__);
        return -ENOSYS;
    }
    free(sm->st_config);
    sm->st_config = NULL;
    sm->recognition_callback = NULL;

    // If we're streaming, then we shouldn't touch the VTS's current state.
    if (!stdev->is_streaming ||
            (stdev->is_streaming & sm->bitmask)) {
        // Only stop when it's the last one, otherwise, turn off the VTS and reconfigure it for the
        // new list of algorithms with callbacks.
        int active_bitmask = stdev_active_callback_bitmask(stdev);
        if (active_bitmask == 0 && stdev->is_recording == 0) {
            stdev_join_callback_thread(stdev, false);
        } else {
            // Callback thread should already be running, but make sure.
            // stdev_start_callback_thread(stdev);
            if (stdev->voicecall_state == VOICECALL_STOPPED) {
                stdev_vts_reset(stdev);
            } else {
                ALOGI("%s: Voice call is in progress. Skip stopping VTS \n", __func__);
            }
        }
    }

    sm->model_stopfailedhandle = HANDLE_NONE;
    sm->model_execstate = MODEL_STATE_NONE;
    return 0;
}

/* calling function should acquire stdev lock */
static void handle_stop_recognition_l(struct sound_trigger_device *stdev)
{
    int i;

    ALOGV("%s", __func__);

    for (i = 0; i < stdev->num_sound_model; ++i) {
        if (stdev->sm_info[i].model_execstate == MODEL_STATE_STOPABORT &&
            stdev->sm_info[i].model_stopfailedhandle == i) {
            if (stdev->sm_info[i].recognition_callback == NULL &&
                    !(stdev->is_streaming & stdev->sm_info[i].bitmask)) {
                ALOGI("%s:model recognition is already stopped", __func__);
            } else {
                if (stdev->sm_info[i].st_config)
                    free(stdev->sm_info[i].st_config);
                stdev->sm_info[i].st_config = NULL;
                stdev->sm_info[i].recognition_callback = NULL;
                ALOGI("%s:model recognition callback released", __func__);
            }

            // If we're streaming, then we shouldn't touch the VTS's current state.
            if (!stdev->is_streaming ||
                    (stdev->is_streaming & stdev->sm_info[i].bitmask)) {
                // force stop recognition
                stdev_vts_set_power(stdev, 0);
                ALOGI("%s:model recognition force stopped", __func__);
                // reconfigure if other model is running.
                int active_bitmask = stdev_active_callback_bitmask(stdev);
                if (active_bitmask) {
                    stdev_vts_set_power(stdev, active_bitmask);
                }
            }
        }

        stdev->sm_info[i].model_execstate = MODEL_STATE_NONE;
        stdev->sm_info[i].model_stopfailedhandle = HANDLE_NONE;
    }

    return;
}

/* Add functions to verify in debug_verify_mode */
static void verify_ahal_function(struct sound_trigger_device *stdev)
{
    sound_trigger_headset_status(1);
    sound_trigger_headset_status(0);
    sound_trigger_notify_ahal_record_status(1);
    sound_trigger_notify_ahal_record_status(0);
    sound_trigger_voicecall_status(1);
    sound_trigger_voicecall_status(0);
    sound_trigger_update_ahal_input_status(1);
    sound_trigger_update_ahal_input_status(0);
    stdev->sm_info[0].model_stopfailedhandle = 0;
}

__attribute__ ((visibility ("default")))
int sound_trigger_open_for_streaming()
{
    struct sound_trigger_device *stdev = &g_stdev;
    int ret = 0;
    char fn[256];

    ALOGV("%s", __func__);

    pthread_mutex_lock(&stdev->lock);

    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = -EFAULT;
        goto exit;
    }

    if (stdev->voicecall_state == VOICECALL_STARTED) {
        ALOGI("%s VoiceCall in progress", __func__);
        ret = -EBUSY;
        goto exit;
    }

    if (!stdev->is_streaming) {
        ALOGE("%s: VTS is not streaming currently", __func__);
        ret = -EBUSY;
        goto exit;
    }

    if (stdev->is_seamless_recording) {
        ALOGE("%s: VTS is already seamless recording currently", __func__);
        ret = -EBUSY;
        goto exit;
    }

    snprintf(fn, sizeof(fn), "/dev/snd/pcmC%uD%u%c", VTS_SOUND_CARD, VTS_TRICAP_DEVICE_NODE, 'c');
    ALOGI("%s: Opening PCM Device %s", __func__, fn);

    ret = stdev_pcm_open(stdev, &stdev->streaming_pcm, VTS_TRICAP_DEVICE_NODE);
    if (ret < 0)
        goto exit;

    stdev->is_seamless_recording = true;
    ret = 1;
exit:
    pthread_mutex_unlock(&stdev->lock);

    return ret;
}

__attribute__ ((visibility ("default")))
size_t sound_trigger_read_samples(
        int audio_handle,
        void *buffer,
        size_t  buffer_len)
{
    struct sound_trigger_device *stdev = &g_stdev;
    size_t ret = 0;

    //ALOGV("%s", __func__);

    if (audio_handle <= 0) {
        ALOGE("%s: invalid audio handle", __func__);
        return -EINVAL;
    }

    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = -EFAULT;
        goto exit;
    }

    if (stdev->voicecall_state == VOICECALL_STARTED) {
        ALOGI("%s VoiceCall in progress", __func__);
        ret = -EBUSY;
        goto exit;
    }

    if (!stdev->is_streaming) {
        ALOGE("%s: VTS is not streaming currently", __func__);
        ret = -EINVAL;
        goto exit;
    }

    ret = stdev_pcm_read(stdev, stdev->streaming_pcm, buffer, buffer_len);
    if (ret < 0)
        goto exit;

    if (ret == 0) {
        ALOGVV("%s: --Sent %zu bytes to buffer", __func__, buffer_len);
    } else {
        if (stdev->streaming_pcm)
            ALOGE("%s: Read Fail = %s", __func__, pcm_get_error(stdev->streaming_pcm));
        else
            ALOGE("%s: Read Fail, also pcm is null for some reason", __func__);
    }

exit:
    return ret;
}

__attribute__ ((visibility ("default")))
int sound_trigger_close_for_streaming(int audio_handle __unused)
{
    struct sound_trigger_device *stdev = &g_stdev;
    size_t ret = 0;

    ALOGV("%s", __func__);

    if (audio_handle <= 0) {
        ALOGE("%s: invalid audio handle", __func__);
        return -EINVAL;
    }

    if (stdev->debug_verify_mode) {
        verify_ahal_function(stdev);
    }

    pthread_mutex_lock(&stdev->lock);

    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = -EFAULT;
        goto exit;
    }

    if (stdev->voicecall_state == VOICECALL_STARTED) {
        ALOGI("%s VoiceCall in progress", __func__);
        ret = -EBUSY;
        goto exit;
    }

    if (!stdev->is_seamless_recording) {
        ALOGE("%s: VTS Seamless Recording PCM Node is not opened", __func__);
        ret = -EINVAL;
        goto exit;
    }

    if (!stdev->is_streaming) {
        ALOGE("%s: VTS is not currently streaming", __func__);
        ret = -EINVAL;
        goto exit;
    }

    stdev_pcm_close(stdev, &stdev->streaming_pcm);
    stdev->is_seamless_recording = false;
    // Power off the VTS, but then re-enable any algorithms that have callbacks.
    int active_bitmask = stdev_active_callback_bitmask(stdev);
    stdev_vts_set_power(stdev, 0);
    if (active_bitmask) {
        stdev_start_callback_thread(stdev);
        stdev_vts_set_power(stdev, active_bitmask);
    }
exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}


/* VTS recording sthal interface */
__attribute__ ((visibility ("default")))
int sound_trigger_open_recording()
{
    struct sound_trigger_device *stdev = &g_stdev;
    int ret = 0;

    ALOGD("%s: enter", __func__);

    pthread_mutex_lock(&stdev->lock);
    ret = sound_trigger_open_recording_l(stdev);
    pthread_mutex_unlock(&stdev->lock);

    ALOGD("%s: exit", __func__);

    return ret;
}

/* calling function should acquire stdev/recording lock */
static int sound_trigger_open_recording_l(struct sound_trigger_device *stdev)
{
    int ret = 0;
    char fn[256];

    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = -EFAULT;
        goto exit;
    }

    if (stdev->voicecall_state == VOICECALL_STARTED) {
        ALOGI("%s VoiceCall in progress", __func__);
        ret = -EBUSY;
        goto exit;
    }

    if (stdev->is_recording) {
        ALOGW("%s: VTS is already recording currently", __func__);

        /* workaround to forcefully close current execution */
        /* close streaming pcm node */
        stdev_pcm_close(stdev, &stdev->recording_pcm);

        /* disable VTS MIC controls */
        int active_bitmask = stdev_active_callback_bitmask(stdev);
        if (!active_bitmask && !stdev->is_streaming) {
            stdev_vts_set_mic(stdev, false);
        }
        stdev->is_recording = false;
        ALOGI("%s: Forcefully closed current recording", __func__);
    }

    /* Check & enable VTS MIC controls */
    stdev_vts_set_mic(stdev, true);

    /* To chcek VTS status during recording */
    stdev_start_callback_thread(stdev);

    snprintf(fn, sizeof(fn), "/dev/snd/pcmC%uD%u%c", VTS_SOUND_CARD, VTS_RECORD_DEVICE_NODE, 'c');
    ALOGI("%s: Opening PCM Device %s", __func__, fn);

    ret = stdev_pcm_open(stdev, &stdev->recording_pcm, VTS_RECORD_DEVICE_NODE);
    if (ret < 0)
        goto exit;

    stdev->is_recording = true;
    ret = 1;

exit:
    return ret;
}

__attribute__ ((visibility ("default")))
size_t sound_trigger_read_recording_samples(
        void *buffer,
        size_t  buffer_len)
{
    struct sound_trigger_device *stdev = &g_stdev;
    // int i;
    size_t ret = 0;

    //ALOGV("%s", __func__);

    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        return -EFAULT;
    }

    if (stdev->voicecall_state == VOICECALL_STARTED) {
        ALOGI("%s VoiceCall in progress", __func__);
        return -EBUSY;
    }

    if (stdev->vts_status == ABNORMAL) {
        memset((void *)buffer, 0, buffer_len);
        ret = buffer_len;
        goto exit;
    }

    if (!stdev->recording_pcm) {
        ALOGW("%s: VTS Recording PCM Node is NULL. Try open PCM Node",
                __func__);
        pthread_mutex_lock(&stdev->lock);
        sound_trigger_open_recording_l(stdev);
        pthread_mutex_unlock(&stdev->lock);
    }

    if (!stdev->is_recording) {
        ALOGE("%s: VTS Recording is inactive", __func__);
        return -EINVAL;
    }

    ret = stdev_pcm_read(stdev, stdev->recording_pcm, buffer, buffer_len);
    if (ret < 0)
        goto exit;

    if (ret == 0) {
        ALOGVV("%s: --Sent %zu bytes to buffer", __func__, buffer_len);
    } else {
        if (stdev->recording_pcm)
            ALOGE("%s: Read Fail = %s", __func__, pcm_get_error(stdev->recording_pcm));
        else
            ALOGE("%s: Read Fail, also pcm is null for some reason", __func__);
    }

exit:
    return ret;
}

__attribute__ ((visibility ("default")))
int sound_trigger_close_recording()
{
    struct sound_trigger_device *stdev = &g_stdev;
    size_t ret = 0;
    int i = 0;

    ALOGD("%s: enter", __func__);

    if (stdev->debug_verify_mode) {
        verify_ahal_function(stdev);
    }

    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = -EFAULT;
        goto exit;
    }

    if (stdev->voicecall_state == VOICECALL_STARTED) {
        ALOGI("%s VoiceCall in progress", __func__);
        ret = -EBUSY;
        goto exit;
    }
    pthread_mutex_lock(&stdev->lock);

    /* Error handling if models stop-recognition failed */
    for (i = 0; i < stdev->num_sound_model; i++) {
        if (stdev->sm_info[i].model_stopfailedhandle != HANDLE_NONE) {
            handle_stop_recognition_l(stdev);
            ALOGI("%s stop-recognition error state handled", __func__);
            break;
        }
    }

    ret = sound_trigger_close_recording_l(stdev);

    pthread_mutex_unlock(&stdev->lock);

    ALOGD("%s: exit", __func__);
exit:
    return ret;
}

/* calling function should acquire stdev/recording lock */
static int sound_trigger_close_recording_l(struct sound_trigger_device *stdev)
{
    int ret = 0;

    ALOGD("%s", __func__);

    if (!stdev->is_recording) {
        ALOGE("%s: VTS Recording PCM Node is not opened", __func__);
        ret = -EINVAL;
        goto exit;
    }

    /* close streaming pcm node */
    stdev_pcm_close(stdev, &stdev->recording_pcm);

    /* disable VTS MIC controls */
    int active_bitmask = stdev_active_callback_bitmask(stdev);
    if (!active_bitmask && !stdev->is_streaming) {
        stdev_join_callback_thread(stdev, false);
        stdev_vts_set_mic(stdev, false);
    }

    stdev->is_recording = false;

exit:
    return ret;
}

int sound_trigger_headset_status(int is_connected)
{
    struct sound_trigger_device *stdev = &g_stdev;
    int active_bitmask = 0;
    int ret = 0;

    if (!stdev->support_headset_mic) {
        ALOGD("%s: Headset Mic is not supported", __func__);
        return -ENODEV;
    }

    pthread_mutex_lock(&stdev->lock);

    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = -EFAULT;
        goto exit;
    }

    /* check whether vts mic is configured or not */
    if (stdev->is_mic_configured) {
        int tmp_streaming = stdev->is_streaming;
        int tmp_recording = stdev->is_recording;
        int tmp_seamlessrecording = stdev->is_seamless_recording;
        //Check if recording is in progress
        if (stdev->is_recording) {
            ALOGI("%s: Close VTS Record PCM to reconfigure active Mic", __func__);
            // Close record PCM before changing MIC
            stdev_pcm_close(stdev, &stdev->recording_pcm);
            stdev->is_recording = false;
        }

        //Check if seamless capture is in progress
        if (stdev->is_streaming) {
            ALOGI("%s: Close VTS Seamless PCM to reconfigure active Mic", __func__);
            // Close seamless PCM before changing MIC
            stdev_pcm_close(stdev, &stdev->streaming_pcm);
            stdev->is_seamless_recording = false;
        }

        // Power off the VTS, but then re-enable any algorithms that have callbacks.
        active_bitmask = stdev_active_callback_bitmask(stdev);
        stdev_vts_set_power(stdev, 0);
        /* update active mic only after disabling previous mic configuraiton */
        if (is_connected)
            stdev->active_mic = VTS_HEADSET_MIC;
        else
            stdev->active_mic = VTS_MAIN_MIC;

        ALOGI("%s: Active MIC Changed to [%s] Active Models: 0x%x", __func__,
                    (is_connected ? "HEADSET MIC" : "MAIN MIC"), active_bitmask);

        // Restore recording status
        stdev->is_recording = tmp_recording;

        if (active_bitmask || tmp_streaming) {
            active_bitmask |= tmp_streaming;
            ALOGI("%s: Re-started Models: 0x%x", __func__, active_bitmask);
            stdev_vts_set_power(stdev, active_bitmask);
            stdev->is_streaming = tmp_streaming;
            stdev->is_seamless_recording = tmp_seamlessrecording;
        }

        //Check if recording enabled then start again
        if (stdev->is_recording) {
            ALOGI("%s: Re-route active Mic for recording", __func__);
            /* Check & enable VTS MIC controls */
            stdev_vts_set_mic(stdev, true);

            /* open vts streaming PCM node */
            if (!stdev->recording_pcm) {
                ret = stdev_pcm_open(stdev, &stdev->recording_pcm, VTS_RECORD_DEVICE_NODE);
                if (ret < 0)
                    goto exit;

            }
            ALOGI("%s: VTS Record reconfiguration & open PCM Completed", __func__);
        }

        //Check if seamless capture enable then start again
        if (stdev->is_streaming) {
            /* open vts streaming PCM node */
            if (stdev->is_seamless_recording && !stdev->streaming_pcm) {
                ret = stdev_pcm_open(stdev, &stdev->streaming_pcm, VTS_TRICAP_DEVICE_NODE);
                if (ret < 0)
                    goto exit;
            }
        }
    } else {
        /* update the active mic information */
        if (is_connected)
            stdev->active_mic = VTS_HEADSET_MIC;
        else
            stdev->active_mic = VTS_MAIN_MIC;
    }

exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

int sound_trigger_notify_ahal_record_status(int state)
{
    struct sound_trigger_device *stdev = &g_stdev;
    int ret = 0;

    /* If concurrent catpure enabled, STHAL doesn't need to stop or start VTS processing. */
    if (stdev->concurrent_capture)
        return ret;

    switch (state) {
    case RECORDING_STOP:
        /* no further impl for now */
        break;
    case RECORDING_START:
        ALOGI("%s: audiohal is about to start recording", __func__);
        if (stdev->is_mic_configured)
            stdev_vts_set_power(stdev, 0);

        break;
    default:
        ALOGE("%s: state(%d) is not defined", __func__, state);
        ret = -1;
        break;
    }

    return ret;
}

int sound_trigger_voicecall_status(int callstate)
{
    struct sound_trigger_device *stdev = &g_stdev;
    int ret = 0;

    pthread_mutex_lock(&stdev->lock);

    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = -EFAULT;
        goto exit;
    }

    /* check whether vts mic is configured or not */
    if (callstate == VOICECALL_STARTED) {
        if (stdev->is_mic_configured) {
            //Check if recording is in progress
            if (stdev->is_recording) {
                ALOGI("%s: Close VTS Record PCM to reconfigure active Mic", __func__);
                // Close record PCM before changing MIC
                stdev_pcm_close(stdev, &stdev->recording_pcm);
                stdev->is_recording = false;
            }

            //Check if seamless capture is in progress
            if (stdev->is_streaming) {
                ALOGI("%s: Close VTS Seamless PCM to reconfigure active Mic", __func__);
                // Close seamless PCM before changing MIC
                stdev_pcm_close(stdev, &stdev->streaming_pcm);
                stdev->is_seamless_recording = false;
                stdev->is_streaming = false;
            }

            // Power off the VTS. And When voicecall is stopped, power will be on.
            stdev_vts_set_power(stdev, 0);
        }
        /* update voicecall status */
        stdev->voicecall_state = VOICECALL_STARTED;
        ALOGI("%s: VoiceCall START notification received", __func__);
    } else {
        stdev->voicecall_state = VOICECALL_STOPPED;
        stdev_vts_reset(stdev);
        ALOGI("%s: VoiceCall STOP notification received", __func__);
    }

exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

int sound_trigger_update_ahal_input_status(bool state)
{   struct sound_trigger_device *stdev = &g_stdev;
    int active_bitmask = 0;
    int ret = 0;

    pthread_mutex_lock(&stdev->lock);
    ALOGD("%s, state %d", __func__, state);
    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = -EFAULT;
        goto exit;
    }

    if (stdev->audio_input_state == state) {
        ALOGE("%s: same as previous state", __func__);
        ret = 0;
        goto exit;
    }

    stdev->audio_input_state = state;

    if (!stdev->recognize_started) {
        ALOGD("%s: wakeup off, no need to voice trigger state", __func__);
        goto exit;
    }

    //stop voice recognition when normal recording started
    if (stdev->audio_input_state) {
        int tmp_recognize_started = stdev->recognize_started;

        //should stop streaming read thread here

        if (stdev->is_recording) {
            ALOGI("%s: Close VTS Record PCM", __func__);
            stdev_pcm_close(stdev, &stdev->recording_pcm);
            stdev->is_recording = false;
        }

        if (stdev->is_streaming) {
            ALOGI("%s: Close VTS Seamless PCM", __func__);
            stdev_pcm_close(stdev, &stdev->streaming_pcm);
            stdev->is_seamless_recording = false;
            ALOGD("%s stdev->is_streaming %d", __func__, stdev->is_streaming);
            stdev->is_streaming = false;
        }

        if (stdev->is_mic_configured) {
            stdev_vts_set_power(stdev, 0);
        }

        stdev->recognize_started = tmp_recognize_started;
    } else {
        active_bitmask = stdev_active_callback_bitmask(stdev);
        if (active_bitmask) {
            {
                stdev_vts_set_power(stdev, active_bitmask);
            }
        }
    }

exit:
    pthread_mutex_unlock(&stdev->lock);
    ALOGD("%s, exit", __func__);

    return ret;
}

static int stdev_close(hw_device_t *device)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)device;
    int ret = 0, i;

    ALOGV("%s", __func__);

    pthread_mutex_lock(&stdev->lock);
    if (!stdev->sthal_opened) {
        ALOGE("%s: device already closed", __func__);
        ret = -EFAULT;
        goto exit;
    }

    stdev_join_callback_thread(stdev, false);
    stdev_close_mixer(stdev);

    if (munmap(stdev->mapped_addr,VTSDRV_MISC_MODEL_BIN_MAXSZ) < 0) {
        ALOGE("%s: munmap failed  %s", __func__, strerror(errno));
    }

    close(stdev->vtsdev_fd);
    stdev->vtsdev_fd = -1;

    for (i = 0; i < stdev->num_sound_model; ++i) {
        stdev->sm_info[i].recognition_callback = NULL;
        stdev->sm_info[i].sound_model_callback = NULL;
        if (stdev->sm_info[i].st_config) {
            free(stdev->sm_info[i].st_config);
            stdev->sm_info[i].st_config = NULL;
        }
    }

    stdev->sthal_opened = false;

exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static int stdev_open(
        const hw_module_t *module,
        const char *name,
        hw_device_t **device)
{
    struct sound_trigger_device *stdev;
    int ret = -EINVAL;

    ALOGV("%s", __func__);

    if (strcmp(name, SOUND_TRIGGER_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    stdev = &g_stdev;
    pthread_mutex_lock(&stdev->lock);

    if (stdev->sthal_opened) {
        ALOGE("%s: Only one sountrigger can be opened at a time", __func__);
        ret = -EBUSY;
        goto exit;
    }

    /* Open VTS Misc device for loading Model binary through MMAP interface */
    stdev->vtsdev_fd = open("/dev/vts_fio_dev", O_RDWR);
    if (stdev->vtsdev_fd < 0) {
        ALOGE("%s: Failed to open VTS-Misc device %s", __func__, strerror(errno));
        goto exit;
    }

    /* memory map VTS misc driver */
    stdev->mapped_addr = mmap(NULL, VTSDRV_MISC_MODEL_BIN_MAXSZ,
                              PROT_READ | PROT_WRITE, MAP_SHARED, stdev->vtsdev_fd, 0);

    if (stdev->mapped_addr == MAP_FAILED) {
        ALOGE("%s: VTS Device MMAP failed", __func__);
        close(stdev->vtsdev_fd);
        goto exit;
    }
    ALOGI("%s: VTS device opened Successfully for MMAP Interface", __func__);

    ret = stdev_init_mixer(stdev);
    if (ret) {
        ALOGE("Error mixer init");
        goto exit;
    }

    stdev->device.common.tag = HARDWARE_DEVICE_TAG;
    stdev->device.common.version = SOUND_TRIGGER_DEVICE_API_VERSION_1_3;
    stdev->device.common.module = (struct hw_module_t *)module;
    stdev->device.common.close = stdev_close;
    stdev->device.get_properties = stdev_get_properties;
    stdev->device.get_properties_extended = stdev_get_properties_extended;
    stdev->device.load_sound_model = stdev_load_sound_model;
    stdev->device.unload_sound_model = stdev_unload_sound_model;
    stdev->device.start_recognition = stdev_start_recognition;
    stdev->device.start_recognition_extended = stdev_start_recognition_extended;
    stdev->device.stop_recognition = stdev_stop_recognition;
    stdev->device.set_parameter = stdev_set_parameter;
    stdev->device.get_parameter = stdev_get_parameter;
    stdev->device.query_parameter = stdev_query_parameter;
    stdev->streaming_pcm = NULL;
    stdev->is_seamless_recording = false;
    stdev->sthal_opened = true;
    stdev->recognize_started = 0;
    stdev->is_mic_configured = 0;
    stdev->active_mic = VTS_MAIN_MIC;
    stdev->is_recording = false;
    stdev->recording_pcm = NULL;
    stdev->voicecall_state = VOICECALL_STOPPED;
    stdev->recog_cbstate = RECOG_CB_NONE;
    stdev->audio_input_state = false;
    stdev->support_headset_mic = false;
    stdev->support_silent_reset = false;

    st_xml_parser(stdev);

    int i;
    for (i = 0; i < stdev->num_sound_model; ++i) {
        stdev->sm_info[i].model_handle = -1;
        stdev->sm_info[i].model_execstate = MODEL_STATE_NONE;
        stdev->sm_info[i].model_stopfailedhandle = HANDLE_NONE;
        stdev->sm_info[i].sm_data = NULL;
        stdev->sm_info[i].sm_config.actual_sz = 0;
    }

    hw_properties.concurrent_capture = stdev->concurrent_capture;
    hw_properties.max_sound_models = stdev->num_sound_model;

    hw_properties_1_3.base.concurrent_capture = stdev->concurrent_capture;
    hw_properties_1_3.base.max_sound_models = stdev->num_sound_model;

    stdev_odm_init(stdev);

    *device = &stdev->device.common; // same address as stdev

    stdev->audio_primary_lib = dlopen(AUDIO_PRIMARY_HAL_LIBRARY_RELPATH, RTLD_NOW);
    if (stdev->audio_primary_lib == NULL) {
        ALOGE("%s: DLOPEN failed for %s", __func__, AUDIO_PRIMARY_HAL_LIBRARY_RELPATH);
        goto hal_exit;
    } else {
        ALOGV("%s: DLOPEN successful for %s", __func__, AUDIO_PRIMARY_HAL_LIBRARY_RELPATH);

        stdev->notify_sthal_status = (int (*)(int))dlsym(stdev->audio_primary_lib,
                                         "notify_sthal_status");
        if (!stdev->notify_sthal_status) {
            ALOGE("%s: Error: fail to grab notify_sthal_status func", __func__);
            stdev->notify_sthal_status = 0;
        }

        stdev->notify_set_vts_route = (int (*)(bool, int))dlsym(stdev->audio_primary_lib,
                                         "notify_set_vts_route");
        if (!stdev->notify_set_vts_route) {
            ALOGE("%s: Error: fail to grab notify_set_vts_route func", __func__);
            stdev->notify_set_vts_route = 0;
        }
    }

    if (stdev_set_initial_mixer(stdev))
        goto exit;

    pthread_mutex_unlock(&stdev->lock);
    return 0;

hal_exit:
    stdev_close_mixer(stdev);
exit:
    pthread_mutex_unlock(&stdev->lock);

    ALOGI("%s: failed to open SoundTrigger HW Device", __func__);
    return ret;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = stdev_open,
};

struct sound_trigger_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = SOUND_TRIGGER_MODULE_API_VERSION_1_0,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = SOUND_TRIGGER_HARDWARE_MODULE_ID,
        .name = "Exynos Primary SoundTrigger HAL",
        .author = "Samsung SLSI",
        .methods = &hal_module_methods,
    },
};
