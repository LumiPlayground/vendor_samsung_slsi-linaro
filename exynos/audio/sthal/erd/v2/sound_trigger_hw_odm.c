/*
 * Copyright (C) 2022 The Android Open Source Project
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
#include <poll.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <dlfcn.h>

#include "sound_trigger_hw_odm.h"
#include "sound_trigger_hw.h"
#include "vts.h"

extern int set_mixer_ctrls(struct sound_trigger_device *stdev, char *path_name[],
                            int *path_ctlvalue, int ctrl_count, bool reverse);
extern int vts_load_sound_model(struct sound_trigger_device *stdev, struct sound_model_info *sm_info);
extern int vts_set_parameters(struct sound_trigger_device *stdev, int msg, int arg0, int arg1, int arg2);

void stdev_odm_init(struct sound_trigger_device *stdev)
{
    stdev->odm_voicemodel_mode = ODM_TRIGGER_MODE;
}

/*
 * SoundTriggerHidl interface ODM specific scenario
 */

/*
 * SoundTriggerHAL internal ODM specific scenario
 */
struct sound_trigger_phrase_recognition_event *odm_sound_trigger_event_alloc(void *stdev_ptr,
        int index) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    struct sound_trigger_phrase_recognition_event *event =
            (struct sound_trigger_phrase_recognition_event *)calloc(
                    1, sizeof(struct sound_trigger_phrase_recognition_event));
    if (!event)
        return NULL;

    event->common.status = RECOGNITION_STATUS_SUCCESS;
    event->common.type = SOUND_MODEL_TYPE_KEYPHRASE;
    event->common.model = stdev->sm_info[index].model_handle;

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
    event->common.capture_available = true;
    event->common.trigger_in_data = false;

    event->common.audio_config = AUDIO_CONFIG_INITIALIZER;
    event->common.audio_config.sample_rate = 16000;
    event->common.audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
    event->common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;

    return event;
}

struct sound_trigger_generic_recognition_event *odm_sound_trigger_generic_event_alloc(void *stdev_ptr,
        int index) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    struct sound_trigger_generic_recognition_event *event =
            (struct sound_trigger_generic_recognition_event *)calloc(
                    1, sizeof(struct sound_trigger_generic_recognition_event));

    if (!event)
        return NULL;

    event->common.status = RECOGNITION_STATUS_SUCCESS;
    event->common.type = SOUND_MODEL_TYPE_GENERIC;
    event->common.model = stdev->sm_info[index].model_handle;

    event->common.capture_available = true;
    event->common.trigger_in_data = false;

    event->common.audio_config = AUDIO_CONFIG_INITIALIZER;
    event->common.audio_config.sample_rate = 16000;
    event->common.audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
    event->common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;

    return event;
}

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

                trigger_index = 0;
                err = 0;
                triggered_keyword = event_type & ~(EVENT_TRIGGERED);

                for (i = 0; i < stdev->num_sound_model; i++) {
                    if (triggered_keyword == stdev->sm_info[i].id) {
                        if (stdev->sm_info[i].is_generic)
                            event = odm_sound_trigger_generic_event_alloc(stdev, i);
                        else
                            event = odm_sound_trigger_event_alloc(stdev, i);

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
                    callbacks(event, cookies);
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
    }

    return err;
}

void odm_stop_thread(void *stdev_ptr) {
    /* Do Nothing */

    return;
}

void odm_stop_recognition_l(void *stdev_ptr, sound_model_handle_t handle, int active_bitmask) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    // If we're streaming, then we shouldn't touch the VTS's current state.
    if (!stdev->is_streaming || (stdev->is_streaming & stdev->sm_info[handle].bitmask)) {
        // Only stop when it's the last one, otherwise, turn off the VTS and reconfigure it for the
        // new list of algorithms with callbacks.
        if (active_bitmask == 0) {
            stdev_join_callback_thread(stdev, false);
        } else {
            // Callback thread should already be running, but make sure.
            // stdev_start_callback_thread(stdev);
            if (stdev->voicecall_state == VOICECALL_STOPPED)
                stdev_vts_reset(stdev);
            else
                ALOGI("%s: Voice call is in progress. Skip stopping VTS \n", __func__);
        }
    }
}

int odm_open_pcm_for_streaming(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;
    int ret = 0;
    char fn[256];

    snprintf(fn, sizeof(fn), "/dev/snd/pcmC%uD%u%c", VTS_SOUND_CARD, VTS_TRICAP_DEVICE_NODE, 'c');
    ALOGI("%s: Opening PCM Device %s", __func__, fn);

    /* open vts streaming PCM node */
    stdev->streaming_pcm = pcm_open(VTS_SOUND_CARD, VTS_TRICAP_DEVICE_NODE, PCM_IN, &pcm_config_vt_capture);
    if (stdev->streaming_pcm && !pcm_is_ready(stdev->streaming_pcm)) {
        ALOGE("%s: failed to open streaming PCM (%s)", __func__, pcm_get_error(stdev->streaming_pcm));
        ret = -EFAULT;
    }

    return ret;
}

int odm_pcm_read_sample(void *stdev_ptr, void *buffer, size_t  buffer_len) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;
    int ret = 0;

    if (stdev->streaming_pcm) {
        ret = pcm_read(stdev->streaming_pcm, buffer, buffer_len);
    } else {
        ALOGE("%s: Streaming PCM node is NULL", __func__);
        ret = -EFAULT;
    }

    return ret;
}

void odm_pcm_close_for_streaming(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    /* close streaming pcm node */
    if (stdev->streaming_pcm) {
        pcm_close(stdev->streaming_pcm);
        stdev->streaming_pcm = NULL;
    }
    stdev->is_seamless_recording = false;
}

void odm_sound_trigger_voicecall_status(void *stdev_ptr, int callstate) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    /* check whether vts mic is configured or not */
    if (callstate == VOICECALL_STARTED) {
        if (stdev->is_mic_configured) {
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
}

void odm_get_ahal_api(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    stdev->notify_sthal_status = (int (*)(int))dlsym(stdev->audio_primary_lib,
                                   "notify_sthal_status");

    if (!stdev->notify_sthal_status) {
        ALOGE("%s: Error in grabbing function from %s", __func__, AUDIO_PRIMARY_HAL_LIBRARY_RELPATH);
        stdev->notify_sthal_status = 0;
    }
}

/*
 * Check and Set function for ODM specific scenario
 */
void odm_vts_set_power(void *stdev_ptr, int req) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    stdev_vts_set_power(stdev, req);

    return;
}

int stdev_odm_set_power(struct sound_trigger_device *stdev,
        struct sound_model_info *sm_info, bool enable)
{
    int *ctrl_values = NULL;

    if (enable) {
        if (stdev->odm_voicemodel_mode == ODM_TRIGGER_MODE) {
            ctrl_values = erd_recognize_start_ctlvalue;
        } else {
            ALOGE("%s: Unknown recognition mode to start, set default bixby mode", __func__);
            ctrl_values = erd_recognize_start_ctlvalue;
        }
        /* load sound model */
        if (vts_load_sound_model(stdev, sm_info)) {
            ALOGE("%s: Failed to load sound model", __func__);
            return -EINVAL;
        }

        if (set_mixer_ctrls(stdev, model_recognize_start_ctlname,
                    ctrl_values, MODEL_START_CONTROL_COUNT, false)) {
            ALOGE("%s: Model recognization start Failed", __func__);
            return -EINVAL;
        }

        /* handle backlog control size */
        if ((stdev->odm_voicemodel_mode == ODM_TRIGGER_MODE)
                && sm_info->backlog_size) {
            if (set_mixer_ctrls(stdev, model_backlog_size_ctlname,
                        &sm_info->backlog_size, MODEL_BACKLOG_CONTROL_COUNT, false)) {
                ALOGE("%s: Model backlog size configuration Failed", __func__);
                return -EINVAL;
            }
            ALOGD("%s: Model Backlog size [%d] configured", __func__, sm_info->backlog_size);
        }

        if (vts_set_parameters(stdev, VTS_IRQ_AP_SET_MODE, stdev->odm_voicemodel_mode, 0, 0)) {
            ALOGE("%s: VTS_IRQ_AP_SET_MODE Failed", __func__);
            return -EINVAL;
        }

        stdev->recognize_started |= sm_info->bitmask;
        ALOGD("%s: Model recognization started", __func__);
    } else {
        if (stdev->odm_voicemodel_mode == ODM_TRIGGER_MODE) {
            ctrl_values = erd_recognize_stop_ctlvalue;
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

void odm_chk_n_set_rec_call_state(void *stdev_ptr) {
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)stdev_ptr;

    if (stdev->voicecall_state == VOICECALL_STOPPED) {
        stdev_vts_reset(stdev);
    } else {
        ALOGI("%s: Voice call is in progress. Skip starting VTS \n", __func__);
    }
}

void odm_chk_n_set_read_status(void *stdev_ptr) {
    /* Do Nothing */
    return;
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
