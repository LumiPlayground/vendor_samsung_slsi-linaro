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

#ifndef _SOUND_TRIGGER_INFO_H_
#define _SOUND_TRIGGER_INFO_H_

/* Audio Board Device's Information */

#define ST_INFO_XML_PATH     "/vendor/etc/sound_trigger_info.xml"

typedef enum {
    INFO_NONE,
    COMMON_INFO,
    SOUND_MODEL,
    MAIN_MIC_INFO,
    HEADSET_MIC_INFO,
} set_information;

static set_information set_info;

void st_xml_parser(void *dev);

#endif  // _SOUND_TRIGGER_INFO_H_
