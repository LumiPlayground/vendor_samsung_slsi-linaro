/*
 * Copyright (C) 2020 The Android Open Source Project
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

#define LOG_TAG "st_xml_parser"
//#define LOG_NDEBUG 0

#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <expat.h>
#include <string.h>
#include <log/log.h>
#include <cutils/str_parms.h>

#include "SoundTriggerGtest.h"
#include "st_xml_parser.h"

static int sm_index = 0;
int mic_ctrl_cnt_idx;

static bool str_to_bool (char str[]) {
    if (strcmp(str, "true") == 0)
        return true;
    else if (strcmp(str, "false") == 0)
        return false;
    else
        return false;
}

static void get_name_value(const XML_Char **attr, char name[], char value[]) {
    uint32_t cur_idx = 0;

    if (strcmp(attr[cur_idx++], "name") == 0) {
        strcpy(name, attr[cur_idx++]);
    }

    if (strcmp(attr[cur_idx++], "value") == 0) {
        strcpy(value, attr[cur_idx++]);
    }
}

static void set_sound_trigger_config(struct sound_trigger_device *stdev, const XML_Char **attr)
{
    char *ptr = NULL;
    char name[30];
    char value[100];
    struct sound_model_info *sm_info = NULL;
    ALOGV("%s: enter", __func__);

    get_name_value(attr, name, value);

    if (set_info == COMMON_INFO) {
        if (strcmp(name, "concurrent_capture") == 0)
            stdev->concurrent_capture = str_to_bool(value);
        else if (strcmp(name, "num_sound_model") == 0)
            stdev->num_sound_model = atoi(value);
        else if (strcmp(name, "num_supported_mic") == 0)
            stdev->num_supported_mic = atoi(value);
        else if (strcmp(name, "sysclk_div") == 0)
            stdev->sysclk_div = atoi(value);
        else if (strcmp(name, "support_headset_mic") == 0)
            stdev->support_headset_mic = str_to_bool(value);
        else if (strcmp(name, "pcm_dump_enabled") == 0)
            stdev->pcm_dump_enabled = str_to_bool(value);
        else if (strcmp(name, "debug_verify_mode") == 0)
            stdev->debug_verify_mode = str_to_bool(value);
    } else if (set_info == SOUND_MODEL) {
        if (sm_index >= stdev->num_sound_model || sm_index < 0) {
            ALOGE("%s: sm_index: %d, Error, num_sound_model: %d",
                    __func__, sm_index, stdev->num_sound_model);
            return;
        }
        sm_info = &stdev->sm_info[sm_index];

        if (strcmp(name, "name") == 0)
            strcpy(sm_info->name, value);
        else if (strcmp(name, "backlog_ms") == 0)
            sm_info->backlog_size = atoi(value);
        else if (strcmp(name, "vendor_uuid") == 0) {
            ptr = strtok(value, ",");
            sm_info->uuid.timeLow = strtol(ptr, NULL, 16);
            ptr = strtok(NULL, ",");
            sm_info->uuid.timeMid = strtol(ptr, NULL, 16);
            ptr = strtok(NULL, ",");
            sm_info->uuid.timeHiAndVersion = strtol(ptr, NULL, 16);
            ptr = strtok(NULL, ",");
            sm_info->uuid.clockSeq = strtol(ptr, NULL, 16);
            ptr = strtok(NULL, ",");
            sm_info->uuid.node[0] = strtol(ptr, NULL, 16);
            ptr = strtok(NULL, ",");
            sm_info->uuid.node[1] = strtol(ptr, NULL, 16);
            ptr = strtok(NULL, ",");
            sm_info->uuid.node[2] = strtol(ptr, NULL, 16);
            ptr = strtok(NULL, ",");
            sm_info->uuid.node[3] = strtol(ptr, NULL, 16);
            ptr = strtok(NULL, ",");
            sm_info->uuid.node[4] = strtol(ptr, NULL, 16);
            ptr = strtok(NULL, ",");
            sm_info->uuid.node[5] = strtol(ptr, NULL, 16);
        }
        else if (strcmp(name, "sm_max_size") == 0)
            sm_info->sm_config.max_sz = strtol(value, NULL, 16);
        else if (strcmp(name, "mem_offset") == 0)
            sm_info->sm_config.offset = strtol(value, NULL, 16);
        else if (strcmp(name, "support_odm_function") == 0)
            sm_info->support_odm_function = str_to_bool(value);
    }

    ALOGV("%s: exit", __func__);
}

static void set_sound_model_id(struct sound_trigger_device *stdev, const XML_Char **attr)
{
    uint32_t cur_idx = 0;

    if (sm_index >= stdev->num_sound_model || sm_index < 0) {
        ALOGE("%s: sm_index: %d, Error, num_sound_model: %d",
                __func__, sm_index, stdev->num_sound_model);
        return;
    }

    if (strcmp(attr[cur_idx++], "id") == 0)
        stdev->sm_info[sm_index].id = atoi(attr[cur_idx++]);
    if (strcmp(attr[cur_idx++], "bitmask") == 0)
         stdev->sm_info[sm_index].bitmask =  strtol(attr[cur_idx++], NULL, 16);

    ALOGV("%s: id: %d, sm_index: %d, bitmask: 0x%x", __func__,
            stdev->sm_info[sm_index].id, sm_index, stdev->sm_info[sm_index].bitmask);
}

static void set_mic_ctrl_value(struct sound_trigger_device *stdev, const XML_Char **attr) {
    uint32_t cur_idx = 0;

    if (set_info == MAIN_MIC_INFO) {
        if (strcmp(attr[cur_idx++], "name") == 0)
            strcpy(stdev->mic_ctrl.main_mic_ctlname[mic_ctrl_cnt_idx], attr[cur_idx++]);

        if (strcmp(attr[cur_idx++], "value") == 0)
            stdev->mic_ctrl.main_mic_ctlvalue[mic_ctrl_cnt_idx] = atoi(attr[cur_idx++]);

        mic_ctrl_cnt_idx++;
    } else if (set_info == HEADSET_MIC_INFO) {
        if (strcmp(attr[cur_idx++], "name") == 0)
            strcpy(stdev->mic_ctrl.headset_mic_ctlname[mic_ctrl_cnt_idx], attr[cur_idx++]);

        if (strcmp(attr[cur_idx++], "value") == 0)
            stdev->mic_ctrl.headset_mic_ctlvalue[mic_ctrl_cnt_idx] = atoi(attr[cur_idx++]);

        mic_ctrl_cnt_idx++;
    }
}

static void set_mic_ctrl_cnt(uint32_t *ctrlcnt, const XML_Char **attr)
{
    uint32_t cur_idx = 0;

    if (strcmp(attr[cur_idx++], "cnt") == 0)
        *ctrlcnt = atoi(attr[cur_idx++]);
}

static void print_sound_trigger_info(struct sound_trigger_device *stdev)
{
    struct sound_model_info *sm_info = NULL;
    ALOGD("%s: concurrent_capture: %d", __func__,  stdev->concurrent_capture);
    ALOGD("%s: num_sound_model: %d", __func__,  stdev->num_sound_model);
    ALOGD("%s: support_headset_mic: %d", __func__, stdev->support_headset_mic);
    ALOGD("%s: pcm_dump_enabled: %d", __func__, stdev->pcm_dump_enabled);
    ALOGD("%s: debug_verify_mode: %d", __func__, stdev->debug_verify_mode);

    for (int id = 0; id < stdev->num_sound_model; ++id) {
        sm_info = &stdev->sm_info[id];
        ALOGD("%s: id: %d, name: %s, bitmask: %d", __func__,  sm_info->id, sm_info->name, sm_info->bitmask);
        ALOGD("%s: backlog_ms: %d", __func__,  sm_info->backlog_size);
        ALOGD("%s: vendor_uuid: 0x%08x, 0x%04x, 0x%04x, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x", __func__,
                sm_info->uuid.timeLow, sm_info->uuid.timeMid,
                sm_info->uuid.timeHiAndVersion, sm_info->uuid.clockSeq,
                sm_info->uuid.node[0], sm_info->uuid.node[1],
                sm_info->uuid.node[2], sm_info->uuid.node[3],
                sm_info->uuid.node[4], sm_info->uuid.node[5]);
        ALOGD("%s: sm_max_size: 0x%"PRIx64"", __func__,  sm_info->sm_config.max_sz);
        ALOGD("%s: mem_offset: 0x%"PRIx64"", __func__,  sm_info->sm_config.offset);
        ALOGD("%s: support_odm_function: %d", __func__, sm_info->support_odm_function);
    }
}


static void print_mic_setting_info(struct sound_trigger_device *stdev)
{
    int i;

    ALOGD("%s: main mic", __func__);
    for (i = 0; i < stdev->mic_ctrl.main_mic_ctrl_cnt; i++) {
        ALOGD("%s: %d", stdev->mic_ctrl.main_mic_ctlname[i], stdev->mic_ctrl.main_mic_ctlvalue[i]);
    }
}

static void end_tag(__attribute__((unused)) void *data, const XML_Char *tag_name)
{
    if (strcmp(tag_name, "common_info") == 0 || strcmp(tag_name, "sound_model_info") == 0
        || strcmp(tag_name, "mic_configuration") == 0) {
        set_info = INFO_NONE;
        ALOGV("%s: set_info is reset: %d", __func__, set_info);
    } else if (strcmp(tag_name, "sound_model") == 0) {
        sm_index++;
    }
}

static void start_tag(void *data, const XML_Char *tag_name, const XML_Char **attr)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)data;

    if (strcmp(tag_name, "common_info") == 0) {
        ALOGV("%s: tag_name is common_info", __func__);
        set_info = COMMON_INFO;
    } else if (strcmp(tag_name, "sound_model_info") == 0) {
        ALOGV("%s: tag_name is sound_model_info", __func__);
        set_info = SOUND_MODEL;
    } else if (strcmp(tag_name, "sound_model") == 0) {
        ALOGV("%s: tag_name is sound_model", __func__);
        if (set_info != SOUND_MODEL) {
            ALOGE("%s: sound_model tag should be supported with sound_model_info tag", __func__);
            return;
        }
        set_sound_model_id(stdev, attr);
    } else if (strcmp(tag_name, "config") == 0) {
        if (set_info == MAIN_MIC_INFO || set_info == HEADSET_MIC_INFO)
            set_mic_ctrl_value(stdev, attr);
        else
            set_sound_trigger_config(stdev, attr);
    } else if (strcmp(tag_name, "mic_configuration") == 0) {
        ALOGV("%s: tag_name is mic_configuration", __func__);
    } else if (strcmp(tag_name, "main_mic") == 0) {
        ALOGV("%s: tag_name is main_mic", __func__);
        set_info = MAIN_MIC_INFO;

        mic_ctrl_cnt_idx = 0;
        set_mic_ctrl_cnt(&stdev->mic_ctrl.main_mic_ctrl_cnt, attr);
    } else if (strcmp(tag_name, "headset_mic") == 0) {
        ALOGV("%s: tag_name is headset_mic", __func__);
        set_info = HEADSET_MIC_INFO;

        mic_ctrl_cnt_idx = 0;
        set_mic_ctrl_cnt(&stdev->mic_ctrl.headset_mic_ctrl_cnt, attr);
    }
}

void st_xml_parser(void *dev)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    XML_Parser parser = 0;
    FILE *file = NULL;
    char info_file_name[50] = {0};
    void *buf = NULL;
    uint32_t buf_size = 1024;
    int32_t bytes_read = 0;

    strcpy(info_file_name, ST_INFO_XML_PATH);

    file = fopen(info_file_name, "r");
    if (file == NULL) {
        ALOGE("%s: open error: %s: file: %s", __func__, strerror(errno), info_file_name);
        goto exit;
    } else {
        ALOGI("%s: xml file name is %s", __func__, info_file_name);
    }

    parser = XML_ParserCreate(NULL);
    if (!parser) {
        ALOGE("%s: failed to create XML parser", __func__);
        goto exit;
    }

    XML_SetUserData(parser, stdev);
    XML_SetElementHandler(parser, start_tag, end_tag);

    while (1) {
        buf = XML_GetBuffer(parser, buf_size);
        if (buf == NULL) {
            ALOGE("%s: fail to get buffer", __func__);
            break;
        }

        bytes_read = fread(buf, 1, buf_size, file);
        if (bytes_read < 0) {
            ALOGE("%s: fail to read from file", __func__);
            break;
        }

        XML_ParseBuffer(parser, bytes_read, bytes_read == 0);

        if (bytes_read == 0)
            break;
    }

    print_sound_trigger_info(stdev);
    print_mic_setting_info(stdev);
exit:
    if (parser)
        XML_ParserFree(parser);

    if (file)
        fclose(file);

    return;
}
