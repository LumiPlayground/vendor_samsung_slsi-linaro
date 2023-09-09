/*
 * Copyright (C) 2021 The Android Open Source Project
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

#ifndef __ODM_STHAL_DEFNITION_H__
#define __ODM_STHAL_DEFNITION_H__

typedef enum {
   ODM_OFF_MODE            = 0,        // OFF
   ODM_TRIGGER_MODE        = 1, // LPSD + voice trigger
   ODM_LPSD_MODE           = 2, // LPSD (babycry & doorbell)
   ODM_TRIGGER_ALWAYS_MODE = 3, // voice trigger
} ODM_VOICE_MODEL_MODE;

static int svoice_bixbyrecognize_start_ctlvalue[] = {
    1,  //"VTS Active Keyphrase",
    VTS_RECOGNIZE_START,  //"VTS_RECOGNIZE_STATE",
    1800, //back log size from trigger point
};

static int svoice_bixbyrecognize_stop_ctlvalue[] = {
    1, //"VTS Active Keyphrase",
    VTS_RECOGNIZE_STOP,  //"VTS_RECOGNIZE_STATE",
};

static int svoice_lpsdrecognize_start_ctlvalue[] = {
    1,  //"VTS Active Keyphrase",
    VTS_RECOGNIZE_START,  //"VTS_RECOGNIZE_STATE",
    0, //back log size from trigger point
};

static int svoice_lpsdrecognize_stop_ctlvalue[] = {
    1, //"VTS Active Keyphrase",
    VTS_RECOGNIZE_STOP,  //"VTS_RECOGNIZE_STATE",
};

static int svoice_bixbyalwaysrecognize_start_ctlvalue[] = {
    1,  //"VTS Active Keyphrase",
    VTS_RECOGNIZE_START,  //"VTS_RECOGNIZE_STATE",
    1800, //back log size from trigger point
};

static int svoice_bixbyalwaysrecognize_stop_ctlvalue[] = {
    1, //"VTS Active Keyphrase",
    VTS_RECOGNIZE_STOP,  //"VTS_RECOGNIZE_STATE",
};

/* Sound model binaries */
#define SOUND_MODEL_OKGOOGLE_BINARY "/data/voice_dva_okgoogle.bin"
#define SOUND_MODEL_ODM_BINARY      "/data/voice_dva_svoice.bin"

#define VTSDRV_MISC_ODM_MODEL_BIN_MAXSZ (0x8000 + 0x300)
#define VTS_IRQ_AP_SET_MODE (22)

#endif /* __ODM_STHAL_DEFNITION_H__ */
