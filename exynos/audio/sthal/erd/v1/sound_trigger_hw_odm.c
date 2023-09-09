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

#include <cutils/str_parms.h>
#include <log/log.h>
#include <errno.h>
#include "sound_trigger_hw.h"
#include "sthal_odm_definition.h"

extern int set_mixer_ctrls(struct sound_trigger_device *stdev, char *path_name[],
                            int *path_ctlvalue, int ctrl_count, bool reverse);
extern int vts_load_sound_model(struct sound_trigger_device *stdev, struct sound_model_info *sm_info);
extern int vts_set_parameters(struct sound_trigger_device *stdev, int msg, int arg0, int arg1, int arg2);

void stdev_odm_init(struct sound_trigger_device *stdev)
{
    stdev->odm_voicemodel_mode = ODM_TRIGGER_MODE;
}

int stdev_odm_set_power(struct sound_trigger_device *stdev,
        struct sound_model_info *sm_info, bool enable)
{
    int *ctrl_values = NULL;
    struct vts_ipc_msg ipc_msg;

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

//Parses Extra config structure data, for SVoice specification information
void stdev_odm_parse_extra_config_data(
        struct sound_trigger_device *stdev,
        struct sound_model_info *sm_info,
        const char *key_value_pairs)
{
    ALOGD("%s: enter with key_value_pairs: %s", __func__, key_value_pairs);

    struct str_parms *parms;
    int value;
    int ret;

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
