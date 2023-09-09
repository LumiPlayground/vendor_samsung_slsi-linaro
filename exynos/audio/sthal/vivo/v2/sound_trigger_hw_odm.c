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

#define LOG_TAG "soundtrigger_hw_odm"
#define LOG_NDEBUG 0

//#define VERY_VERBOSE_LOGGING
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <cutils/str_parms.h>
#include <log/log.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/ioctl.h>
#include <cutils/properties.h>
#include <tinyalsa/asoundlib.h>
#include <system/sound_trigger.h>

#include "sound_trigger_hw_odm.h"
#include "sound_trigger_hw.h"
#include "vts.h"

extern void *streaming_thread_loop(void *context);
extern int set_mixer_ctrls(struct sound_trigger_device *stdev, char *path_name[],
                            int *path_ctlvalue, int ctrl_count, bool reverse);
extern void stdev_vts_set_power(struct sound_trigger_device *stdev, int enabled_algorithms);
extern int vts_load_sound_model(struct sound_trigger_device *stdev, struct sound_model_info *sm_info);
extern int vts_set_parameters(struct sound_trigger_device *stdev, int msg, int arg0, int arg1, int arg2);
extern int stdev_active_callback_bitmask(struct sound_trigger_device* stdev);
struct sound_trigger_device *g_odm_stdev;

static int odm_recognize_start_ctlvalue[] = {
    1,  //"VTS Active Keyphrase",
    VTS_RECOGNIZE_START,  //"VTS_RECOGNIZE_STATE",
    1800, //back log size from trigger point
};

static int odm_recognize_stop_ctlvalue[] = {
    1, //"VTS Active Keyphrase",
    VTS_RECOGNIZE_STOP,  //"VTS_RECOGNIZE_STATE",
};

struct pcm_config pcm_config_bargein_vt_capture = {
    .channels = BARGEIN_RECORD_CHANNELS,
    .rate = BARGEIN_RECORD_SAMPLE_RATE,
    .period_size = BARGEIN_RECORD_PERIOD_SIZE,
    .period_count = BARGEIN_RECORD_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

static struct pcm_config pcm_config_bargein_trigger = {
    .channels        = BARGEIN_TRIGGER_CHANNELS,
    .rate            = BARGEIN_TRIGGER_SAMPLE_RATE,
    .period_size     = BARGEIN_TRIGGER_PERIOD_SIZE,
    .period_count    = BARGEIN_TRIGGER_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .stop_threshold = BARGEIN_TRIGGER_STOP,
};

void stdev_odm_init(struct sound_trigger_device *stdev) {
    stdev->odm_voicemodel_mode = ODM_TRIGGER_MODE;
    g_odm_stdev = stdev;

#ifdef SUPPORT_BARGEIN_MODE
    stdev->odm_stdev.is_bargein_mode_configed = false;
    stdev->odm_stdev.requires_bargein_mode = false;
    stdev->odm_stdev.sound_card_abnormal_state = false;
#endif

#ifdef ODM_SOUND_TRIGGER
    stdev->odm_stdev.voice_cmd_fp = NULL;
    stdev->odm_stdev.stream_amp_buf = NULL;
    stdev->odm_stdev.streaming_thread = 0;
    stdev->odm_stdev.stream_gain = 0;
    stdev->odm_stdev.kw_length = VTS_KW_LENGTH;
    stdev->odm_stdev.kw_count = 0;
    stdev->odm_stdev.kw_type = 0;
    pthread_mutex_init(&stdev->odm_stdev.streaming_open_lock, (const pthread_mutexattr_t *) NULL);
    pthread_cond_init(&stdev->odm_stdev.streaming_open_cond, (const pthread_condattr_t *) NULL);
    pthread_mutex_init(&stdev->odm_stdev.streaming_pcm_lock, (const pthread_mutexattr_t *) NULL);
    pthread_mutex_init(&stdev->odm_stdev.streaming_thread_loop_lock, (const pthread_mutexattr_t *) NULL);
#endif
}

#ifdef SUPPORT_BARGEIN_MODE
static struct pcm *bargein_trigger; /* bargein trigger device */
static struct pcm *bargein_dummy_bridge;

/* Disables BargeIn Trigger Device */
void close_bargein_device() {
    char pcm_path[MAX_PCM_PATH_LEN];

    if (bargein_trigger) {
        snprintf(pcm_path, sizeof(pcm_path), "/dev/snd/pcmC%uD%u%c",
            BARGEIN_SOUND_CARD, BARGEIN_TRIGGER_DEVICE, 'c');

        pcm_stop(bargein_trigger);
        pcm_close(bargein_trigger);
        bargein_trigger = NULL;

        ALOGI("proxy-%s: BargeIn Trigger PCM Device(%s) is stopped & closed!", __func__, pcm_path);
    }

#ifdef SUPPORT_ULTRASOUND
    if (bargein_dummy_bridge) {
        snprintf(pcm_path, sizeof(pcm_path), "/dev/snd/pcmC%uD%u%c",
            BARGEIN_TRIGGER_CARD, BARGEIN_DUMMY_BRIDGE_DEVICE, 'c');

        pcm_stop(bargein_dummy_bridge);
        pcm_close(bargein_dummy_bridge);
        bargein_dummy_bridge = NULL;

        ALOGI("proxy-%s: BargeIn Bridge PCM Device(%s) is stopped & closed!", __func__, pcm_path);
    }
#endif
}

/* Enables BargeIn Trigger Device */
int open_bargein_device() {
    char pcm_path[MAX_PCM_PATH_LEN];
    struct pcm_config pcmconfig = pcm_config_bargein_trigger;

    if (bargein_trigger == NULL) {
        snprintf(pcm_path, sizeof(pcm_path), "/dev/snd/pcmC%uD%u%c",
            BARGEIN_SOUND_CARD, BARGEIN_TRIGGER_DEVICE, 'c');

        bargein_trigger = pcm_open(BARGEIN_SOUND_CARD, BARGEIN_TRIGGER_DEVICE,
            PCM_IN | PCM_MONOTONIC, &pcmconfig);
        if (bargein_trigger && !pcm_is_ready(bargein_trigger)) {
            /* pcm_open does always return pcm structure, not NULL */
            ALOGE("sthal-%s: BargeIn Trigger PCM Device(%s) with SR(%u) PF(%d) CC(%d) is not ready as error(%s)",
                __func__, pcm_path, pcmconfig.rate, pcmconfig.format, pcmconfig.channels,
                pcm_get_error(bargein_trigger));
            goto err_open;
        }
        ALOGI("sthal-%s: BargeIn Trigger PCM Device(%s) with SR(%u) PF(%d) CC(%d) is opened",
            __func__, pcm_path, pcmconfig.rate, pcmconfig.format, pcmconfig.channels);

        if (pcm_start(bargein_trigger) == 0) {
            ALOGI("sthal-%s: BargeIn Trigger PCM Device(%s) with SR(%u) PF(%d) CC(%d) is started",
                __func__, pcm_path, pcmconfig.rate, pcmconfig.format, pcmconfig.channels);
        } else {
            ALOGE("sthal-%s: BargeIn Trigger PCM Device(%s) with SR(%u) PF(%d) CC(%d) cannot be started as error(%s)",
                __func__, pcm_path, pcmconfig.rate, pcmconfig.format, pcmconfig.channels,
                pcm_get_error(bargein_trigger));
            goto err_open;
        }
    }
#ifdef SUPPORT_ULTRASOUND
    if (bargein_dummy_bridge == NULL) {
        snprintf(pcm_path, sizeof(pcm_path), "/dev/snd/pcmC%uD%u%c",
            BARGEIN_SOUND_CARD, BARGEIN_DUMMY_BRIDGE_DEVICE, 'c');

        bargein_dummy_bridge = pcm_open(BARGEIN_SOUND_CARD, BARGEIN_DUMMY_BRIDGE_DEVICE,
            PCM_IN | PCM_MONOTONIC, &pcmconfig);
        if (bargein_dummy_bridge && !pcm_is_ready(bargein_dummy_bridge)) {
            /* pcm_open does always return pcm structure, not NULL */
            ALOGE("sthal-%s: BargeIn Bridge PCM Device(%s) with SR(%u) PF(%d) CC(%d) is not ready as error(%s)",
                __func__, pcm_path, pcmconfig.rate, pcmconfig.format, pcmconfig.channels,
                pcm_get_error(bargein_dummy_bridge));
            goto err_open;
        }
        ALOGI("sthal-%s: BargeIn Trigger PCM Device(%s) with SR(%u) PF(%d) CC(%d) is opened",
            __func__, pcm_path, pcmconfig.rate, pcmconfig.format, pcmconfig.channels);

        if (pcm_start(bargein_dummy_bridge) == 0) {
            ALOGI("sthal-%s: BargeIn Bridge PCM Device(%s) with SR(%u) PF(%d) CC(%d) is started",
                __func__, pcm_path, pcmconfig.rate, pcmconfig.format, pcmconfig.channels);
        } else {
            ALOGE("sthal-%s: BargeIn Bridge PCM Device(%s) with SR(%u) PF(%d) CC(%d) cannot be started as error(%s)",
                __func__, pcm_path, pcmconfig.rate, pcmconfig.format, pcmconfig.channels,
                pcm_get_error(bargein_dummy_bridge));
            goto err_open;
        }
    }
#endif
    return 0;

err_open:
    close_bargein_device();
    return -1;
}

void set_bargein_keyword_type(struct sound_trigger_device *stdev, char *sm_data) {
    struct mixer_ctl *mixerctl = NULL;
    int keyword_type = 0;
    int ret = 0;
    unsigned char *buf = (unsigned char*)sm_data;
    stdev->odm_stdev.kw_count = buf[0];
    stdev->odm_stdev.kw_type = buf[1];

    if (stdev->odm_stdev.sound_card_abnormal_state)
        return;

    mixerctl = mixer_get_ctl_by_name(stdev->mixer, BARGEIN_KWDTYPE_CTL_NAME);
    if (mixerctl) {
        if (stdev->odm_stdev.kw_type == 1) // APP use 1 XVXV 2 HiJovi, vts use 2 XVXV 1 HiJovi
            keyword_type = 2;
        else if (stdev->odm_stdev.kw_type == 2)
            keyword_type = 1;

        ALOGI("%s: configure bargein kwd type control(%s)", __func__, BARGEIN_KWDTYPE_CTL_NAME);
        ret = mixer_ctl_set_value(mixerctl, 0, keyword_type);
        if (ret != 0) {
            ALOGI("%s: failed to set the key word type for Barge-In", __func__);
        } else {
            ALOGE("%s: successfuly to set the key word type for Barge-In ", __func__);
        }
    } else {
        ALOGI("%s: (%s) not exist", __func__, BARGEIN_KWDTYPE_CTL_NAME);
    }
}

void enable_bargein(struct sound_trigger_device *stdev)
{
	int i;
	int active_bitmask = stdev_active_callback_bitmask(stdev);
	
	if (!active_bitmask) {
        ALOGE("%s: enabled_algorithms status error", __func__);
        return;
    }

	if (stdev->odm_stdev.is_bargein_mode_configed){
		ALOGE("%s: bargein already enabled", __func__);
		return;
	}

	stdev->recognize_started |= active_bitmask;
	ALOGI("%s: Model %d recognization started", __func__, active_bitmask);

    if (stdev->odm_stdev.sound_card_abnormal_state)
        return;

	for (i = 0; i < stdev->num_sound_model; i++) {
		if (active_bitmask & stdev->sm_info[i].bitmask) {
			set_bargein_keyword_type(stdev, &stdev->sm_info[i]);
		}
	}

    if (stdev->odm_stdev.notify_set_bargein_route)
        stdev->odm_stdev.notify_set_bargein_route(true);

    open_bargein_device();

    stdev->odm_stdev.is_bargein_mode_configed = true;
}

void disable_bargein(struct sound_trigger_device *stdev) {
    int i;

    if (!stdev->odm_stdev.is_bargein_mode_configed) {
        ALOGE("%s: bargein already disabled", __func__);
        return;
    }

    for (i = 0; i < stdev->num_sound_model; i++) {
        if (stdev->recognize_started & stdev->sm_info[i].bitmask) {
             stdev->recognize_started &= ~(stdev->sm_info[i].bitmask);
             ALOGI("%s: Model %d recognization stopped", __func__, i);
        }
    }

    if (stdev->odm_stdev.sound_card_abnormal_state)
        return;

    close_bargein_device();

    if (stdev->odm_stdev.notify_set_bargein_route)
        stdev->odm_stdev.notify_set_bargein_route(false);

    stdev->odm_stdev.is_bargein_mode_configed = false;
}
int trigger_capture_close(void) {
    struct sound_trigger_device *stdev = g_odm_stdev;
    size_t ret = 0;

    ALOGV("%s", __func__);

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

    pthread_mutex_lock(&stdev->odm_stdev.streaming_pcm_lock);
    if (stdev->streaming_pcm != NULL) {
        /* close streaming pcm node */
        pcm_close(stdev->streaming_pcm);
        stdev->streaming_pcm = NULL;
    }
    pthread_mutex_unlock(&stdev->odm_stdev.streaming_pcm_lock);

    stdev->is_seamless_recording = false;

#ifdef SUPPORT_BARGEIN_MODE
    if (stdev->odm_stdev.is_bargein_mode_configed)
        disable_bargein(stdev);
	else
#endif
        stdev_vts_set_power(stdev, 0);

exit:
    return ret;
}

void streaming_stop(struct sound_trigger_device *stdev) {
    ALOGD("%s start", __func__);

    if (stdev->odm_stdev.streaming_thread_start) {
        stdev->odm_stdev.streaming_thread_start = false;
        pthread_mutex_lock(&stdev->odm_stdev.streaming_thread_loop_lock);
        if (stdev->odm_stdev.streaming_thread) {
            ALOGD("%s pthread_join", __func__);
            pthread_join(stdev->odm_stdev.streaming_thread, (void **)NULL);
            ALOGD("%s pthread_join exit", __func__);
            stdev->odm_stdev.streaming_thread = 0;
        }
        pthread_mutex_unlock(&stdev->odm_stdev.streaming_thread_loop_lock);
        trigger_capture_close();
    }

    stdev->is_streaming = 0;
    DEINIT_RBUF(stdev->odm_stdev.rbuf);
    ALOGD("%s exit", __func__);

    return;
}

void reset_bargein(struct sound_trigger_device *stdev)
{
	int active_bitmask = stdev_active_callback_bitmask(stdev);
    if (active_bitmask) {
		ALOGI("%s", __func__);
		disable_bargein(stdev);
		enable_bargein(stdev);
    }
}

__attribute__ ((visibility ("default")))
int sound_trigger_update_sound_card_status(bool state) {
    struct sound_trigger_device *stdev = g_odm_stdev;
    int ret = 0;

    ALOGD("%s: update sound card status(%d), stdev->is_bargein_mode_configed(%d), stdev->requires_bargein_mode(%d)",
           __func__, state,
           stdev->odm_stdev.is_bargein_mode_configed, stdev->odm_stdev.requires_bargein_mode);

    pthread_mutex_lock(&stdev->lock);
    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = 0;
        goto exit;
    }

    if (stdev->odm_stdev.sound_card_abnormal_state == state) {
        ALOGE("%s: same as previous state, stdev->sound_card_abnormal_state:%d",
               __func__, stdev->odm_stdev.sound_card_abnormal_state);
        ret = 0;
        goto exit;
    }

    stdev->odm_stdev.sound_card_abnormal_state = state;

    if (state && stdev->odm_stdev.is_bargein_mode_configed){
#ifdef ODM_SOUND_TRIGGER
       streaming_stop(stdev);
#endif
       close_bargein_device();
    } else if (stdev->recognize_started && !stdev->voicecall_state &&
            !state && stdev->odm_stdev.requires_bargein_mode) {

        if (!stdev->odm_stdev.is_bargein_mode_configed)
            enable_bargein(stdev);
        else
            open_bargein_device();
    }

exit:
    pthread_mutex_unlock(&stdev->lock);
    ALOGD("%s, exit", __func__);

    return ret;
}

int set_bargein(struct sound_trigger_device *stdev, int state) {
    int ret = 0;
    int active_bitmask = 0;

    ALOGD("%s, state %d", __func__, state);

    if (stdev->odm_stdev.requires_bargein_mode == state) {
        ret = state;
        ALOGD("%s: stdev->requires_bargein_mode %d, no need change", __func__,
               stdev->odm_stdev.requires_bargein_mode);
        goto exit;
    }

    stdev->odm_stdev.requires_bargein_mode = state;
    if (!stdev->recognize_started || stdev->voicecall_state) {
        ALOGD("%s: wakeup off, no need to change state", __func__);
        goto exit;
    }

    if (stdev->is_streaming) {
        ALOGD("%s: detected state, no need to change state", __func__);
        goto exit;
    }

    if (stdev->audio_input_state) {
        ALOGD("%s: there's normal recording, can't set bargein", __func__);
        goto exit;
    }

    if (state) {
        if (stdev->is_mic_configured) {
            if (stdev->is_recording) {
                ALOGI("%s: Close VTS Record PCM to switch to ABOX Bargein mode", __func__);
                    if (stdev->recording_pcm) {
                        pcm_close(stdev->recording_pcm);
                        stdev->recording_pcm = NULL;
                    }
                    stdev->is_recording = false;
                }

			stdev_vts_set_power(stdev, 0);
        }

        enable_bargein(stdev);
        ret = 1;
    } else {
        int tmp_recognize_started = stdev->recognize_started;
        disable_bargein(stdev);
        if (stdev->is_recording) {
            ALOGI("%s: Close BargeIn Record PCM ", __func__);
            if (stdev->recording_pcm) {
                pcm_close(stdev->recording_pcm);
                stdev->recording_pcm = NULL;
                stdev->is_recording = false;
            }
        }

        if (!stdev->audio_input_state) {
            active_bitmask = stdev_active_callback_bitmask(stdev);
            if (active_bitmask) {
                stdev_vts_set_power(stdev, active_bitmask);
            }
        }
        ret = 0;
        stdev->recognize_started = tmp_recognize_started;
    }

exit:
    ALOGD("%s, exit", __func__);

    return 0;
}

__attribute__ ((visibility ("default")))
int sound_trigger_update_ahal_status(bool state)
{
    struct sound_trigger_device *stdev = g_odm_stdev;
    int ret = 0;

#ifdef ODM_SOUND_TRIGGER
    ALOGD("%s, state %d", __func__, state);
    pthread_mutex_lock(&stdev->lock);
    if (!stdev->sthal_opened) {
        ALOGE("%s: stdev has not been opened", __func__);
        ret = -EFAULT;
        goto exit;
    }
#ifdef SUPPORT_BARGEIN_MODE
    set_bargein(stdev, state);
#endif
exit:
    pthread_mutex_unlock(&stdev->lock);
    ALOGD("%s, exit", __func__);
#endif

    return ret;
}
#endif

#ifdef ODM_SOUND_TRIGGER
static int checkDumpAudio() {
    int flag = 0;
    char propValue[128];

    property_get("log.tag.audio.vts.in", propValue, "0");
    return (atoi(propValue) & 0x01);
}

static int checkDumpVoiceCmd() {
    int flag = 0;
    char propValue[128];

    property_get("log.tag.audio.vts.in", propValue, "0");
    return (atoi(propValue) & 0x02);
}

static inline int16_t clamp16(int32_t sample) {
    if ((sample >> 15) ^ (sample >> 31)) {
        return 0x7fff ^ (sample >> 31);
    }

    return sample;
}

static int16_t apply_gain(int32_t gain, int16_t input)
{
    int16_t output = clamp16((gain * input) >> 13);
    return output;
}

void failure_callback(struct sound_trigger_device* stdev) {
    struct sound_trigger_generic_recognition_event event;
    int trigger_index = 0;

    for (trigger_index = 0; trigger_index < MAX_SOUND_MODELS; trigger_index++) {
        if (stdev->sm_info[trigger_index].recognition_callback != NULL) {

            event.common.status = RECOGNITION_STATUS_FAILURE;
            event.common.type = SOUND_MODEL_TYPE_GENERIC;
            event.common.model = stdev->sm_info[trigger_index].model_handle;
            event.common.capture_available = false;
            event.common.audio_config = AUDIO_CONFIG_INITIALIZER;
            event.common.audio_config.sample_rate = 16000;
            event.common.audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
            event.common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;
            event.common.data_offset = 0;
            event.common.data_size = 0;
            event.common.trigger_in_data = 0;

            ALOGI("%s found the model_handle %d", __func__, trigger_index);
            stdev->sm_info[trigger_index].recognition_callback(&event, stdev->sm_info[trigger_index].recognition_cookie);
        }
    }
}

static long long get_cur_time_ms() {
    long long cur_time;
    struct timeval time;
    gettimeofday(&time, NULL);
    cur_time = (long long)time.tv_sec * 1000 + time.tv_usec / 1000;
    return cur_time;
}

static int read_data_from_ringbuf(struct sound_trigger_device *stdev, unsigned char *buff, int size) {
    int read_size = 0;
    long long start_time = get_cur_time_ms();
    ALOGD("%s, start_time %lld", __func__, start_time);

    if (stdev->streaming_pcm == NULL) {
        ALOGD("%s, stdev->streaming_pcm NULL, keyword NULL", __func__);
        return -1;
    }

    stdev->odm_stdev.stop_read_data = 0;

    do {
        int avail = availSize(stdev->odm_stdev.rbuf);
        if (avail > 0) {
            if (stdev->odm_stdev.kw_length - read_size >= avail) {
                readRbuf(stdev->odm_stdev.rbuf, buff + read_size, avail);
                read_size += avail;
            } else {
                int needReadSize = stdev->odm_stdev.kw_length - read_size;
                readRbuf(stdev->odm_stdev.rbuf, buff + read_size, needReadSize);
                read_size += needReadSize;
            }
        } else {
            usleep(5000);
        }

        //ALOGD("%s, read_size %d, stdev->kw_length %d,", __func__, read_size, stdev->kw_length);
    } while((get_cur_time_ms() - start_time < 2000)
             && (stdev->odm_stdev.kw_length != read_size) && !stdev->odm_stdev.stop_read_data);

    ALOGD("%s, read_data time = %lldms", __func__, get_cur_time_ms() - start_time);
    return stdev->odm_stdev.kw_length == read_size ? 0 : -1;
}

int odm_open_streaming(struct sound_trigger_device *stdev) {
    if (!stdev->odm_stdev.streaming_thread_start) {
        ALOGE("%s: streaming thread is not working", __func__);
        return -1;
}
#ifdef SUPPORT_BARGEIN_MODE	
    if (stdev->odm_stdev.is_bargein_mode_configed) {
        stdev->odm_stdev.stream_gain = (int32_t)(pow(10, (VTS_BARGEIN_ONESHOT_ADD_DB / 20.0f)) * (1 << 13));
        stdev->odm_stdev.stream_gain = (int32_t)(pow(10, (VTS_ONESHOT_ADD_DB / 20.0f)) * (1 << 13));
    }
#endif
    if (checkDumpVoiceCmd()) {
        struct tm* p_time;
        time_t cur_time;
        int fileIdx;
        char filePath[128] = {0};

        time (&cur_time);
        p_time = gmtime(&cur_time);
        sprintf(filePath, "%s%s%d_%d_%d_%d_%d_%d.pcm", VTS_HAL_DUMP, "vtsvoicecmd",
                1900 + p_time->tm_year, 1 + p_time->tm_mon, p_time->tm_mday,
                8 + p_time->tm_hour, p_time->tm_min, p_time->tm_sec);
	
        stdev->odm_stdev.voice_cmd_fp = fopen(filePath, "wb+");
    }

	return 0;
}

void odm_close_streaming(struct sound_trigger_device *stdev) {
    if (stdev->odm_stdev.voice_cmd_fp != NULL) {
        fclose(stdev->odm_stdev.voice_cmd_fp);
        stdev->odm_stdev.voice_cmd_fp = NULL;
    }

    if (stdev->odm_stdev.stream_amp_buf != NULL) {
        ALOGD("%s free stream_amp_buf", __func__);
        free(stdev->odm_stdev.stream_amp_buf);
        stdev->odm_stdev.stream_amp_buf = NULL;
    }

    streaming_stop(stdev);

    return;
}

int streaming_read(struct sound_trigger_device *stdev, void *buffer, size_t  buffer_len) {
    size_t ret = 0;

    if (stdev->voicecall_state == VOICECALL_STARTED) {
        ALOGI("%s VoiceCall in progress", __func__);
        ret = -EBUSY;
        goto exit;
    }

    pthread_mutex_lock(&stdev->odm_stdev.streaming_pcm_lock);

#ifdef SUPPORT_BARGEIN_MODE
    if ((stdev->odm_stdev.sound_card_abnormal_state) && stdev->odm_stdev.requires_bargein_mode){
        ret = 0;
        ALOGE("%s: Read Failed for sound card is in abnormal status for bargein", __func__);
        pthread_mutex_unlock(&stdev->odm_stdev.streaming_pcm_lock);
        goto exit;
    }
#endif

    if (stdev->streaming_pcm) {
        size_t len = pcm_read(stdev->streaming_pcm, buffer, buffer_len);
        if (len == 0) {
            ret = buffer_len;
        } else {
            ret = 0;
            ALOGE("%s: Read Fail = %s", __func__, pcm_get_error(stdev->streaming_pcm));
        }
    }
    pthread_mutex_unlock(&stdev->odm_stdev.streaming_pcm_lock);

exit:
    return ret;
}

int streaming_start(struct sound_trigger_device *stdev) {
    int ret = 0;

    DEINIT_RBUF(stdev->odm_stdev.rbuf);

    stdev->odm_stdev.rbuf = (ringbuf*)calloc(1, sizeof(ringbuf));
    if (stdev->odm_stdev.rbuf) {
        if (initRbuf(stdev->odm_stdev.rbuf)) {
            ALOGE("%s initRbuf failed", __func__);
            goto ERR_FAILED;
        }
    } else {
        ALOGE("%s initRbuf allocation failed", __func__);
        goto ERR_FAILED;
    }

    stdev->odm_stdev.stream_stop = 0;

    pthread_mutex_lock(&stdev->odm_stdev.streaming_open_lock);

    ret = pthread_create(&stdev->odm_stdev.streaming_thread, (const pthread_attr_t *) NULL,
                    streaming_thread_loop, stdev);

    //This lock is to make sure that we open the device before streaming start function returns
    if (!ret) {
        ALOGD("%s streaming_thread created, waiting on streaming_open", __func__);
        pthread_cond_wait(&stdev->odm_stdev.streaming_open_cond, &stdev->odm_stdev.streaming_open_lock);
    }

    pthread_mutex_unlock( &stdev->odm_stdev.streaming_open_lock);

    ALOGD("%s end, ret %d", __func__, ret);

    return ret;

ERR_FAILED:

    DEINIT_RBUF(stdev->odm_stdev.rbuf);
    ALOGD("%s failed", __func__);
    return -1;
}

static int trigger_capture_open(void)
{
    struct sound_trigger_device *stdev = g_odm_stdev;
    int ret = 0;
    char fn[256];

    ALOGV("%s", __func__);

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

#ifdef SUPPORT_BARGEIN_MODE
    if (stdev->odm_stdev.is_bargein_mode_configed) {
        struct mixer_ctl *mixerctl = NULL;
        int ret = 0;

        snprintf(fn, sizeof(fn), "/dev/snd/pcmC%uD%u%c", BARGEIN_SOUND_CARD, BARGEIN_RECORD_DEVICE, 'c');
        ALOGI("%s: Opening PCM Device for streaming PCM in Bargein Mode%s", __func__, fn);

        /* open vts streaming PCM node */
        stdev->streaming_pcm = pcm_open(BARGEIN_SOUND_CARD, BARGEIN_RECORD_DEVICE, PCM_IN, &pcm_config_bargein_vt_capture);
        if (stdev->streaming_pcm && !pcm_is_ready(stdev->streaming_pcm)) {
            ALOGE("%s: failed to open streaming PCM (%s) in Bargein Mode", __func__, pcm_get_error(stdev->streaming_pcm));
            ret = -EFAULT;
            goto exit;
        }

        mixerctl = mixer_get_ctl_by_name(stdev->mixer, BARGEIN_STARTCOPY_CTL_NAME);
        if (mixerctl) {
            ALOGI("%s: configure bargein start copy control(%s)", __func__, BARGEIN_STARTCOPY_CTL_NAME);
            ret = mixer_ctl_set_value(mixerctl, 0, 1);
            if (ret != 0){
                ALOGE("%s: failed to set %s", __func__, BARGEIN_STARTCOPY_CTL_NAME);
                pcm_close(stdev->streaming_pcm);
                stdev->streaming_pcm = NULL;
                goto exit;
            }
        } else {
                pcm_close(stdev->streaming_pcm);
                stdev->streaming_pcm = NULL;
                ALOGE("%s: cannot find %s Mixer Control", __func__, BARGEIN_STARTCOPY_CTL_NAME);
                goto exit;
        }
    } else
#endif
    {
        snprintf(fn, sizeof(fn), "/dev/snd/pcmC%uD%u%c", VTS_SOUND_CARD, VTS_TRICAP_DEVICE_NODE, 'c');
        ALOGI("%s: Opening PCM Device %s", __func__, fn);

        /* open vts streaming PCM node */
        stdev->streaming_pcm = pcm_open(VTS_SOUND_CARD, VTS_TRICAP_DEVICE_NODE, PCM_IN, &pcm_config_vt_capture);
        if (stdev->streaming_pcm && !pcm_is_ready(stdev->streaming_pcm)) {
            ALOGE("%s: failed to open streaming PCM (%s)", __func__, pcm_get_error(stdev->streaming_pcm));
            ret = -EFAULT;
            goto exit;
        }
    }

    stdev->is_seamless_recording = true;

exit:
    return ret;
}

void *streaming_thread_loop(void *context) {
    int ret = 0;
    int total_size = 0;

    struct sound_trigger_device *stdev =
               (struct sound_trigger_device *)context;

    ALOGD("%s start", __func__);
    unsigned char read_buffer[KW_FRAME_SIZE];
    memset(read_buffer, 0, KW_FRAME_SIZE);

    ret = trigger_capture_open();
    if (ret) {
        ALOGD("%s trigger_capture_open fail", __func__);
        pthread_mutex_lock(&stdev->odm_stdev.streaming_open_lock);
        pthread_cond_signal(&stdev->odm_stdev.streaming_open_cond);
        pthread_mutex_unlock( &stdev->odm_stdev.streaming_open_lock);
        goto ERR_FAILED;
    }

    pthread_mutex_lock(&stdev->odm_stdev.streaming_open_lock);
    stdev->odm_stdev.streaming_thread_start = true;
    ALOGD("%s trigger_capture_open finished,streming thread start", __func__);

    pthread_cond_signal(&stdev->odm_stdev.streaming_open_cond);
    pthread_mutex_unlock( &stdev->odm_stdev.streaming_open_lock);

    while (stdev->odm_stdev.streaming_thread_start) {
        if (streaming_read(stdev, read_buffer, KW_FRAME_SIZE) > 0) {
            writeRbuf(stdev->odm_stdev.rbuf, read_buffer, KW_FRAME_SIZE);
            total_size += KW_FRAME_SIZE;
            ALOGD("%s: --Sent %d bytes to buffer", __func__, total_size);
        } else {
            writeRbuf(stdev->odm_stdev.rbuf, read_buffer, 0);
            break;
        }
    }

    ALOGD("%s exit", __func__);
    return NULL;

ERR_FAILED:
    trigger_capture_close();
    pthread_mutex_lock(&stdev->odm_stdev.streaming_thread_loop_lock);

    if (stdev->odm_stdev.streaming_thread)
        pthread_detach(pthread_self());

    stdev->odm_stdev.streaming_thread = 0;
    pthread_mutex_unlock(&stdev->odm_stdev.streaming_thread_loop_lock);
    ALOGE("%s error exit", __func__);

    return NULL;
}

void get_vts_keyword_length(struct sound_trigger_device *stdev) {
    unsigned ret = 0;
    struct mixer_ctl *mixerctl = NULL;

    stdev->odm_stdev.kw_length = VTS_KW_LENGTH;

    mixerctl = mixer_get_ctl_by_name(stdev->mixer, VTS_KWDLENGTH_CTL_NAME);
    if (mixerctl) {
        ALOGI("%s: get vts kwd length(%s)", __func__, VTS_KWDLENGTH_CTL_NAME);
        ret = mixer_ctl_get_value(mixerctl, 0);
        if (ret > 0 && ret <= VTS_KW_LENGTH) {
            stdev->odm_stdev.kw_length = ret;
            ALOGI("%s: buffer size of the key word is %d", __func__, stdev->odm_stdev.kw_length);
        } else {
            ALOGE("%s: failed to get %s , use default", __func__, VTS_KWDLENGTH_CTL_NAME);
        }
    } else {
        ALOGI("%s: (%s) not exist", __func__, VTS_KWDLENGTH_CTL_NAME);
    }
}

struct sound_trigger_phrase_recognition_event *odm_sound_trigger_event_alloc(
        void *stdev_ptr, int index) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;
    int start = 0;
    char buf_kw[VTS_KW_LENGTH];
	int err = 0;
	int data_offset = sizeof(struct sound_trigger_phrase_recognition_event);
    struct sound_trigger_phrase_recognition_event *event =
         (struct sound_trigger_phrase_recognition_event *)calloc(1,
             sizeof(struct sound_trigger_phrase_recognition_event) + stdev->odm_stdev.kw_length);

    if (!event)
        return NULL;

    event->common.status = RECOGNITION_STATUS_SUCCESS;
    event->common.type = SOUND_MODEL_TYPE_KEYPHRASE;
    event->common.model = stdev->sm_info[index].model_handle;
    event->common.capture_available = true;
    event->common.audio_config = AUDIO_CONFIG_INITIALIZER;
    event->common.audio_config.sample_rate = 16000;
    event->common.audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
    event->common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;

    event->common.data_offset = data_offset;
    event->common.data_size = stdev->odm_stdev.kw_length;
    event->common.trigger_in_data = 1;

    memset(buf_kw, 0, VTS_KW_LENGTH);
    err = read_data_from_ringbuf(stdev, buf_kw, stdev->odm_stdev.kw_length);

    if (err) {
        free(event);
        event = NULL;
        ALOGE("%s, read keyword error!", __func__);
        return NULL;
    }

    // callback
    memcpy((char *)event + data_offset, &start, sizeof(int));
    memcpy((char *)event + data_offset + sizeof(int), &stdev->odm_stdev.kw_length, sizeof(int));
    memcpy((char *)event + data_offset + (2 * sizeof(int)), buf_kw, stdev->odm_stdev.kw_length - (2 * sizeof(int)));

    ALOGD("%s data_offset %d, data_size %d", __func__, data_offset, stdev->odm_stdev.kw_length);

    return event;
}

struct sound_trigger_generic_recognition_event *odm_sound_trigger_generic_event_alloc(
        void *stdev_ptr, int index) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;
    int start = 0;
    char buf_kw[VTS_KW_LENGTH];
    int err = 0;
    int data_offset = sizeof(struct sound_trigger_generic_recognition_event);
    struct sound_trigger_generic_recognition_event *event =
        (struct sound_trigger_generic_recognition_event *)calloc(1,
            sizeof(struct sound_trigger_generic_recognition_event) + stdev->odm_stdev.kw_length);

    if (!event)
        return NULL;

    event->common.status = RECOGNITION_STATUS_SUCCESS;
    event->common.type = SOUND_MODEL_TYPE_GENERIC;
    event->common.model = stdev->sm_info[index].model_handle;

    event->common.capture_available = true;
    event->common.trigger_in_data = 1;

    event->common.audio_config = AUDIO_CONFIG_INITIALIZER;
    event->common.audio_config.sample_rate = 16000;
    event->common.audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
    event->common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;

    memset(buf_kw, 0, VTS_KW_LENGTH);
    err = read_data_from_ringbuf(stdev, buf_kw, stdev->odm_stdev.kw_length);

    if (err) {
        free(event);
        event = NULL;
        ALOGE("%s, read keyword error!", __func__);
        return NULL;
    }

    // callback
    memcpy((char *)event + data_offset, &start, sizeof(int));
   memcpy((char *)event + data_offset + sizeof(int), &stdev->odm_stdev.kw_length, sizeof(int));
    memcpy((char *)event + data_offset + (2 * sizeof(int)), buf_kw, stdev->odm_stdev.kw_length - (2 * sizeof(int)));

    ALOGD("%s data_offset %d, data_size %d", __func__, data_offset, stdev->odm_stdev.kw_length);

    return event;
}
#endif

#ifdef SUPPORT_BARGEIN_MODE
void get_bargein_keyword_length(struct sound_trigger_device *stdev) {
    unsigned ret = 0;
    struct mixer_ctl *mixerctl = NULL;

    stdev->odm_stdev.kw_length = VTS_KW_LENGTH;
#ifdef SUPPORT_AUDIO_MONITOR
    if (stdev->sound_card_abnormal_state)
        return;
#endif
    mixerctl = mixer_get_ctl_by_name(stdev->mixer, BARGEIN_KWDLENGTH_CTL_NAME);
    if (mixerctl) {
        ALOGI("%s: get bargein kwd length(%s)", __func__, BARGEIN_KWDLENGTH_CTL_NAME);
        ret = mixer_ctl_get_value(mixerctl, 0);
        if (ret > 0 && ret <= VTS_KW_LENGTH) {
            stdev->odm_stdev.kw_length = ret;
            ALOGI("%s: buffer size of the key word is %d", __func__, stdev->odm_stdev.kw_length);
        } else {
            ALOGE("%s: failed to get %s, use default", __func__, BARGEIN_KWDLENGTH_CTL_NAME);
        }
    } else {
        ALOGI("%s: (%s) not exist", __func__, BARGEIN_KWDLENGTH_CTL_NAME);
    }
}
#endif

/*
 * SoundTriggerHAL internal ODM specific scenario
 */
int odm_callback_thread_loop(void *stdev_ptr, struct pollfd poll_event) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;
    struct sound_trigger_recognition_event *event = NULL;
    int kwDetected = 0;
    int model_execstate = -1;
    int trigger_index = 0;
    int err = 0;
    int i;
    void *cookies = NULL;
    recognition_callback_t callbacks = NULL;
    sound_model_handle_t model = 0;

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
                ALOGI("%s: hardfault issue happens", __func__);
                stdev_vts_reset(stdev);
            } else if (event_type & EVENT_RESTART) {
                ALOGI("%s: re-start VTS", __func__);
                stdev_vts_reset(stdev);
            } else if (event_type & EVENT_TRIGGERED) {
                ALOGD("%s: onDetected!", __func__);

                int triggered_keyword;
                bool matched = false;

                trigger_index = 0;
                err = 0;
                triggered_keyword = event_type & ~(EVENT_TRIGGERED);

#ifdef SUPPORT_BARGEIN_MODE
                if (stdev->odm_stdev.is_bargein_mode_configed && (triggered_keyword & EVENT_BARGEIN)){
                    get_bargein_keyword_length(stdev);
                    matched = true;
                    trigger_index = 0; // for now, only 1 sound model is supported
                } else
#endif
                if (stdev->is_mic_configured) {
                    for (i = 0; i < stdev->num_sound_model; i++) {
                        if (triggered_keyword == stdev->sm_info[i].id) {
                            matched = true;
                            trigger_index = i;
#ifdef ODM_SOUND_TRIGGER
                            get_vts_keyword_length(stdev);
#endif
                            break;
                        }
                    }
                }

                if (!matched){
#ifdef SUPPORT_BARGEIN_MODE
                    if (stdev->odm_stdev.requires_bargein_mode) {
                        ALOGI("%s re-config bargein when sthal and driver not compatible", __func__);
                        reset_bargein(stdev);
                    } else {
#endif
                        ALOGI("%s re-config vts when when sthal and driver not compatible", __func__);
                        stdev_vts_reset(stdev);
#ifdef SUPPORT_BARGEIN_MODE
                    }
#endif
                    continue;
                } else {
#ifdef ODM_SOUND_TRIGGER
                    stdev->is_streaming = stdev->sm_info[trigger_index].bitmask;;
                    err = streaming_start(stdev);

                    if (err < 0) {
                        ALOGE("%s, failed to start steaming", __func__);
                        streaming_stop(stdev);
                        continue;
                    }
#endif
                    if (stdev->sm_info[trigger_index].is_generic){
                        event = odm_sound_trigger_generic_event_alloc(stdev, trigger_index);
                    } else
                        event = odm_sound_trigger_event_alloc(stdev, trigger_index);

                    if (event != NULL) {
                        callbacks = stdev->sm_info[trigger_index].recognition_callback;
                        cookies = stdev->sm_info[trigger_index].recognition_cookie;
                        model_execstate = stdev->sm_info[trigger_index].model_execstate;
                        model = stdev->sm_info[trigger_index].model_handle;
                    } else {
                        continue;
                    }
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
                    callbacks(event, cookies);
                } else {
                    ALOGE("%s no callback for model %d", __func__, trigger_index);
#ifdef ODM_SOUND_TRIGGER
                    failure_callback(stdev);
#endif
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
#ifdef SUPPORT_BARGEIN_MODE
                    if (stdev->odm_stdev.requires_bargein_mode) {
                        ALOGI("%s re-config bargein when sthal and driver not compatible", __func__);
                        reset_bargein(stdev);
                    } else {
#endif
                        ALOGI("%s re-config vts when when sthal and driver not compatible", __func__);
                        stdev_vts_reset(stdev);
#ifdef SUPPORT_BARGEIN_MODE
                    }
#endif
                }
                stdev->recog_cbstate = RECOG_CB_NONE;
            } else {
                ALOGE("%s: invalid trigger or out of memory, failure_callback", __func__);
#ifdef ODM_SOUND_TRIGGER
                failure_callback(stdev);
#endif
                stdev->sm_info[trigger_index].recognition_callback = NULL;
                stdev->recog_cbstate = RECOG_CB_NONE;
            }
        }
    }

    return err;
}

void odm_stop_thread(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    if (stdev->odm_stdev.streaming_thread_start) {
        stdev->odm_stdev.streaming_thread_start = false;
        pthread_mutex_lock(&stdev->odm_stdev.streaming_thread_loop_lock);

        if (stdev->odm_stdev.streaming_thread) {
            ALOGD("%s pthread_join", __func__);
            pthread_join(stdev->odm_stdev.streaming_thread, (void **)NULL);
            ALOGD("%s pthread_join exit", __func__);
            stdev->odm_stdev.streaming_thread = 0;
        }
        pthread_mutex_unlock(&stdev->odm_stdev.streaming_thread_loop_lock);
        trigger_capture_close();
    }

    stdev->is_streaming = 0;
    DEINIT_RBUF(stdev->odm_stdev.rbuf);

    return;
}

void odm_stop_recognition_l(void *stdev_ptr, sound_model_handle_t handle, int active_bitmask) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    if (active_bitmask == 0) {
        /* Make callback_thread_loop always exist, otherwise deadlock issue for stdev->lock may happen*/
#ifdef SUPPORT_BARGEIN_MODE
        if (stdev->odm_stdev.requires_bargein_mode)
            disable_bargein(stdev);
        else
#endif
        stdev_vts_set_power(stdev, 0);
    } else {
#ifdef SUPPORT_BARGEIN_MODE
        if (stdev->odm_stdev.requires_bargein_mode) {
            ALOGD("%s, Re-Start Bargein mode",__func__);
            reset_bargein(stdev);
        } else {
#endif
            ALOGD("%s, reconfig VTS",__func__);
            stdev_vts_reset(stdev);
#ifdef SUPPORT_BARGEIN_MODE
        }
#endif
    }
}

int odm_open_pcm_for_streaming(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;
    struct tm *p_time;
    time_t cur_time;
    char filePath[128] = {0};

    if (!stdev->odm_stdev.streaming_thread_start) {
        ALOGE("%s: streaming thread is not working", __func__);
        return -1;
    }
#ifdef SUPPORT_BARGEIN_MODE	
    if (stdev->odm_stdev.is_bargein_mode_configed) {
        stdev->odm_stdev.stream_gain = (int32_t)(pow(10, (VTS_BARGEIN_ONESHOT_ADD_DB / 20.0f)) * (1 << 13));
        stdev->odm_stdev.stream_gain = (int32_t)(pow(10, (VTS_ONESHOT_ADD_DB / 20.0f)) * (1 << 13));
    }
#endif
    if (checkDumpVoiceCmd()) {
        time (&cur_time);
        p_time = gmtime(&cur_time);
        sprintf(filePath, "%s%s%d_%d_%d_%d_%d_%d.pcm", VTS_HAL_DUMP, "vtsvoicecmd",
                1900 + p_time->tm_year, 1 + p_time->tm_mon, p_time->tm_mday,
                8 + p_time->tm_hour, p_time->tm_min, p_time->tm_sec);
                stdev->odm_stdev.voice_cmd_fp = fopen(filePath, "wb+");
    }

        return 0;
}

int odm_pcm_read_sample(void *stdev_ptr, void *buffer, size_t buffer_len) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;
    signed short *amp_out = (signed short *)buffer;

    if (stdev->odm_stdev.stream_amp_buf == NULL) {
        stdev->odm_stdev.stream_amp_buf = (signed short *)malloc(buffer_len * 2); // stereo

        if (!stdev->odm_stdev.stream_amp_buf) {
            ALOGE("%s, malloc stream_amp_buf fail", __func__);
            return -1;
        }
    }
	
    readRbuf(stdev->odm_stdev.rbuf, stdev->odm_stdev.stream_amp_buf, buffer_len * 2);
    for (int i = 0; i < buffer_len / 2; i ++) { // 2ch -> 1ch
        *(amp_out + i) = apply_gain(stdev->odm_stdev.stream_gain, *(stdev->odm_stdev.stream_amp_buf + 2 * i));
    }
	
    if (stdev->odm_stdev.voice_cmd_fp != NULL) {
        ALOGD("vts dump vocie cmd buffer_len %d", buffer_len);
        fwrite(buffer, 1, buffer_len, stdev->odm_stdev.voice_cmd_fp);
    }

    return 0;
}

void odm_pcm_close_for_streaming(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    if (stdev->odm_stdev.voice_cmd_fp != NULL) {
        fclose(stdev->odm_stdev.voice_cmd_fp);
        stdev->odm_stdev.voice_cmd_fp = NULL;
    }

    if (stdev->odm_stdev.stream_amp_buf != NULL) {
        ALOGD("%s free stream_amp_buf", __func__);
        free(stdev->odm_stdev.stream_amp_buf);
        stdev->odm_stdev.stream_amp_buf = NULL;
    }

    odm_stop_thread(stdev);
}

void odm_sound_trigger_voicecall_status(void *stdev_ptr, int callstate) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    /* check whether vts mic is configured or not */
    if (callstate == VOICECALL_STARTED) {
        int tmp_recognize_started = stdev->recognize_started;

        //Check if recording is in progress
        if (stdev->is_recording) {
            ALOGI("%s: Close VTS Record PCM to reconfigure active Mic", __func__);
            // Close record PCM before changing MIC
            if (stdev->recording_pcm) {
                pcm_close(stdev->recording_pcm);
                stdev->recording_pcm = NULL;
            }
            stdev->is_recording = false;
        }
#ifdef ODM_SOUND_TRIGGER
        streaming_stop(stdev);

        //Check if seamless capture is in progress
        if (stdev->is_streaming) {
            ALOGI("%s: Close VTS Seamless PCM to reconfigure active Mic", __func__);
            // Close seamless PCM before changing MIC
            if (stdev->streaming_pcm) {
                pcm_close(stdev->streaming_pcm);
                stdev->streaming_pcm = NULL;
            }
            stdev->is_seamless_recording = false;
            stdev->is_streaming = false;
        }
#endif

#ifdef SUPPORT_BARGEIN_MODE
        if (stdev->odm_stdev.is_bargein_mode_configed)
            disable_bargein(stdev);
        else
#endif
            stdev_vts_set_power(stdev, 0);

        stdev->recognize_started = tmp_recognize_started;
        ALOGI("%s: VoiceCall START notification received", __func__);
    } else {
#ifdef SUPPORT_BARGEIN_MODE
        if (stdev->odm_stdev.requires_bargein_mode)
            enable_bargein(stdev);
        else
#endif
            stdev_vts_reset(stdev);

        ALOGI("%s: VoiceCall STOP notification received", __func__);
    }
}

void odm_get_ahal_api(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

#ifdef SUPPORT_BARGEIN_MODE
    stdev->odm_stdev.notify_set_bargein_route = (int (*)(bool))dlsym(stdev->audio_primary_lib,
                                        "notify_set_bargein_route");
    if (!stdev->odm_stdev.notify_set_bargein_route) {
        ALOGE("%s: Error in grabbing function from %s", __func__, AUDIO_PRIMARY_HAL_LIBRARY_RELPATH);
        stdev->odm_stdev.notify_set_bargein_route = 0;
    }
#endif
}

/*
 * Check and Set function for ODM specific scenario
 */
void odm_vts_set_power(void *stdev_ptr, int req) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;
#ifdef SUPPORT_BARGEIN_MODE
    if (stdev->odm_stdev.requires_bargein_mode)
        disable_bargein(stdev);
    else
#endif
    stdev_vts_set_power(stdev, req);

    return;
}

void odm_chk_n_set_rec_call_state(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    if (stdev->voicecall_state == VOICECALL_STARTED || stdev->audio_input_state) {
        int active_bitmask = stdev_active_callback_bitmask(stdev);
        ALOGI("%s: Voice call or recording is in progress\n", __func__);
        stdev->recognize_started |= active_bitmask;
    } else {
#ifdef SUPPORT_BARGEIN_MODE
        ALOGI("Starting Voice Recognition, requires_bargein_mode: %d",
               stdev->odm_stdev.requires_bargein_mode);
        if (stdev->odm_stdev.requires_bargein_mode) {
            ALOGI("%s: re-config Bargein mode: %d", __func__,
                   stdev->odm_stdev.is_bargein_mode_configed);
            reset_bargein(stdev);
        } else {
#endif
            ALOGI("%s, re-config for VTS Recognition", __func__);
            stdev_vts_reset(stdev);
#ifdef SUPPORT_BARGEIN_MODE
        }
#endif
    }
}

void odm_chk_n_set_read_status(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    stdev->odm_stdev.stop_read_data = 1;
}

// Utility function for loading model binary to kernel through mmap interface
int odm_vts_load_sound_model(struct sound_trigger_device *stdev, struct sound_model_info *sm_info) {
    int ret = 0;
    struct vts_model_bin_info *sm_config = &sm_info->sm_config;
    unsigned char *buf = (unsigned char *)sm_info->sm_data;
    char* data = (char*)calloc(1, sm_config->actual_sz);

    if (data == NULL){
        ALOGE("%s, memory allocation failed, size:%lld", __func__,sm_config->actual_sz);
        return -1;
    }

    ALOGV("%s: Actual size %lld", __func__, sm_config->actual_sz);

#ifdef ODM_SOUND_TRIGGER
    stdev->odm_stdev.kw_count = buf[0];
    stdev->odm_stdev.kw_type = buf[1];

    if (stdev->odm_stdev.kw_type == 1) // APP use 1 XVXV 2 HiJovi, vts use 2 XVXV 1 HiJovi
        data[0] = 2;
    else if (stdev->odm_stdev.kw_type == 2)
        data[0] = 1;

    memcpy(stdev->mapped_addr, data, sm_config->actual_sz);
#else
    memcpy(stdev->mapped_addr, buf, sm_config->actual_sz);
#endif

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

int stdev_odm_set_power(struct sound_trigger_device *stdev,
        struct sound_model_info *sm_info, bool enable) {
    int *ctrl_values = NULL;

    if (enable) {
        if (stdev->odm_voicemodel_mode == ODM_TRIGGER_MODE) {
            ctrl_values = odm_recognize_start_ctlvalue;

			if (sm_info->backlog_size) {
				ALOGD("%s: Model Backlog size [%d] configured", __func__, sm_info->backlog_size);
				ctrl_values[2] = sm_info->backlog_size;
			}
        } else {
            ALOGE("%s: Unknown recognition mode to start, set default bixby mode", __func__);
            ctrl_values = odm_recognize_start_ctlvalue;
        }
        /* load sound model */
        if (odm_vts_load_sound_model(stdev, sm_info)) {
            ALOGE("%s: Failed to load sound model", __func__);
            return -EINVAL;
        }

        if (set_mixer_ctrls(stdev, model_recognize_start_ctlname,
                    ctrl_values, MODEL_START_CONTROL_COUNT, false)) {
            ALOGE("%s: Model recognization start Failed", __func__);
            return -EINVAL;
        }

        if (vts_set_parameters(stdev, VTS_IRQ_AP_SET_MODE, stdev->odm_voicemodel_mode, 0, 0)) {
            ALOGE("%s: VTS_IRQ_AP_SET_MODE Failed", __func__);
            return -EINVAL;
        }

        stdev->recognize_started |= sm_info->bitmask;
        ALOGD("%s: Model recognization started", __func__);
    } else {
        if (stdev->odm_voicemodel_mode == ODM_TRIGGER_MODE) {
            ctrl_values = odm_recognize_stop_ctlvalue;
        }

        if (set_mixer_ctrls(stdev, model_recognize_stop_ctlname,
            ctrl_values, MODEL_STOP_CONTROL_COUNT, false)) {
            ALOGE("%s: Svoice Model recognization stop Failed", __func__);
            return -EINVAL;
        }
        stdev->recognize_started &= ~(sm_info->bitmask);
        ALOGD("%s: Model recognization stopped", __func__);
    }

    return 0;
}

//Parses Extra config structure data, for SVoice specification information
void stdev_odm_parse_extra_config_data(struct sound_trigger_device *stdev,
        struct sound_model_info *sm_info, const char *key_value_pairs) {
    struct str_parms *parms;

    ALOGD("%s: enter with key_value_pairs: %s", __func__, key_value_pairs);

    if (!key_value_pairs) {
        ALOGE("%s: key_value_pairs NULL", __func__);
        return;
    }
    parms = str_parms_create_str(key_value_pairs);

    if (!parms) {
        ALOGE("%s: str_params NULL", __func__);
        return;
    }

    str_parms_destroy(parms);
}
