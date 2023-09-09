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

#ifndef __EXYNOS_SOUNDTRIGGERHAL_H__
#define __EXYNOS_SOUNDTRIGGERHAL_H__

#include <pthread.h>
#include <tinyalsa/asoundlib.h>

#include <hardware/hardware.h>
#include <system/sound_trigger.h>
#include <hardware/sound_trigger.h>

#include "soundtrigger_conf.h"

#define UEVENT_MSG_LEN          64*1024
#define MAX_SOUND_MODELS 3
#define HANDLE_NONE      -1

#define MODEL_START_CONTROL_COUNT       3
#define MODEL_STOP_CONTROL_COUNT        2
#define MODEL_BACKLOG_CONTROL_COUNT     1

#define ST_TRIGGER_PCM_DUMP_FILE "st-trigger"
#define ST_RECORDING_PCM_DUMP_FILE "st-recording"

static struct sound_trigger_properties hw_properties = {
    "Samsung SLSI", // implementor
    "Exynos Primary SoundTrigger HAL", // description
    1, // version
    { 0x1817de20, 0xfa3b, 0x11e5, 0xbef2, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } }, // uuid
    2, // max_sound_models
    1, // max_key_phrases
    1, // max_users
    RECOGNITION_MODE_VOICE_TRIGGER, // recognition_modes
    true, // capture_transition
    0, // max_buffer_ms
    false, // concurrent_capture
    false, // trigger_in_event
    0 // power_consumption_mw
};

static struct sound_trigger_properties_extended_1_3 hw_properties_1_3 = {
    SOUND_TRIGGER_DEVICE_API_VERSION_1_3, // API version
    320, // rough total size
    "Samsung SLSI", // implementor
    "Exynos Primary SoundTrigger HAL", // description
    1, // version
    { 0x1817de20, 0xfa3b, 0x11e5, 0xbef2, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } }, // uuid
    2, // max_sound_models
    1, // max_key_phrases
    1, // max_users
    RECOGNITION_MODE_VOICE_TRIGGER, // recognition_modes
    true, // capture_transition
    0, // max_buffer_ms
    false, // concurrent_capture
    false, // trigger_in_event
    0, // power_consumption_mw
    "374236809, 4ac4bfe8-a4c9-3f5c-8f02-6db7c7123cb6", // supportedModelArch
    0 // audioCapabilities
};

typedef enum {
       VTS_MAIN_MIC         = 0,    //Main mic
       VTS_HEADSET_MIC      = 1,    //Headset mic
}VTS_MIC_CONF;

typedef enum {
       VTS_RECOGNIZE_STOP                 = 0,    //Stop recognization
       VTS_RECOGNIZE_START                = 1,    //start recognization
}VTS_RECOGNIZE_STATE;

typedef enum {
       MODEL_RECOGNIZE_STOPPED          = 0,    //Voice Recognition stopped
       MODEL_RECOGNIZE_STARTED          = 1,    //Voice Recognition started
}MODEL_RUNNING_STATE;

typedef enum {
    VOICECALL_STOPPED = 0,  // Stop recognization
    VOICECALL_STARTED = 1,  // start recognization
} VOICECALL_STATE;

typedef enum {
    RECORDING_STOP = 0,
    RECORDING_START,
} AUDIOHAL_STATE;

typedef enum {
        RECOG_CB_NONE           = 0,    // Recognition event callback function not called
        RECOG_CB_STARTED        = 1,    // Recognition event received
        RECOG_CB_CALLED         = 2,    //Recognition event callback of STHW Service Called
}RECOG_CBSTATE;

typedef enum {
        MODEL_STATE_NONE            = 0,    // Model is not stated
        MODEL_STATE_RUNNING         = 1,    // Model is not stated
        MODEL_STATE_STOPABORT       = 2,    // Model is not stated
}MODEL_EXECSTATE;

/*
    triggered: event_type = EVENT_TRIGGERED + triggered_id
    stop polling: event_type = EVENT_STOP_POLLING
    error recoverty: event_type = EVENT_ERROR_RECOVERY
    restart: event_type = EVENT_RESTART
    EVENT_READY should be add(bit operation) whenever event occurs
*/
typedef enum {
    EVENT_NONE           = 0,
    EVENT_TRIGGERED      = 0x10,
    EVENT_STOP_POLLING   = 0x20,
    EVENT_ERROR_RECOVERY = 0x40,
    EVENT_RESTART        = 0x80,
} POLL_EVENT_TYPE;

typedef enum {
    NORMAL   = 0,
    ABNORMAL = 1,
} VTS_STATUS;

/* VTS the number of supported mic */
static char *vts_mic_cnt_ctlname[] = {
    "VTS Supported Mic Num",
};

static char *vts_sysclk_div_cltname[] = {
    "VTS SYSCLK DIV",
};

/* VTS Force Reset control name */
static char *vts_forcereset_ctlname[] = {
    "VTS Force Reset",
};

/* VTS Support Silent Reset control name */
static char *vts_support_silent_reset_ctlname[] = {
    "VTS RESET SUPPORT",
};

/* Backlog size control name */
static char *model_backlog_size_ctlname[] = {
    "VTS VoiceTrigger Value",
};

/* VTS Model recognization start/stop controls  */
static char *model_recognize_start_ctlname[] = {
    "VTS Active Keyphrase",
    "VTS VoiceRecognization Mode",
    "VTS VoiceTrigger Value",
};

// Delete Trigger Value setting in the stop control
// : unnecessary and can cause abnormal operation of the firmware
static char *model_recognize_stop_ctlname[] = {
    "VTS Active Keyphrase",
    "VTS VoiceRecognization Mode",
};

/* Mic control cnt define */
#define MAX_CNT 20
#define MAX_LEN 30

/* Max predefined IPC param count */
#define MAX_PARAM 10

/* ODM recognize command is in ODM git */

static int hotword_recognize_start_ctlvalue[] = {
    2,  //"VTS Active Keyphrase",
    VTS_RECOGNIZE_START,  //"VTS_RECOGNIZE_STATE",
    2000, //back log size from trigger point
};

static int hotword_recognize_stop_ctlvalue[] = {
    2, //"VTS Active Keyphrase",
    VTS_RECOGNIZE_STOP, //"VTS_RECOGNIZE_STATE",
};

static struct pcm_config pcm_config_vt_capture = {
    .channels = DEFAULT_VTS_CHANNELS,
    .rate = DEFAULT_VTS_SAMPLING_RATE,
    .period_size = PRIMARY_VTS_PERIOD_SIZE,
    .period_count = PRIMARY_VTS_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct vts_model_bin_info {
    int64_t actual_sz;
    int64_t max_sz;
    int64_t offset;
};

struct vts_ipc_msg {
    int msg;
    unsigned int values[3];
};

struct sound_model_info {
    sound_model_handle_t model_handle;
    recognition_callback_t recognition_callback;
    void *recognition_cookie;
    sound_model_callback_t sound_model_callback;
    void *sound_model_cookie;
    struct sound_trigger_recognition_config *st_config;
    int model_execstate;
    sound_model_handle_t model_stopfailedhandle;

    char name[30];
    int id;
    int bitmask;
    sound_trigger_uuid_t uuid;
    char *sm_data;
    struct vts_model_bin_info sm_config;
    int backlog_size;
    bool is_generic;
    bool support_odm_function;
    struct vts_ipc_msg param[MAX_PARAM];
    int param_cnt;
};

struct mic_ctrl_info {
    char main_mic_ctlname[MAX_CNT][MAX_LEN];
    int main_mic_ctlvalue[MAX_CNT];
    uint32_t main_mic_ctrl_cnt;

    char headset_mic_ctlname[MAX_CNT][MAX_LEN];
    int headset_mic_ctlvalue[MAX_CNT];
    uint32_t headset_mic_ctrl_cnt;
};

struct sound_trigger_device {
    struct sound_trigger_hw_device device;
    pthread_t callback_thread;
    pthread_mutex_t lock;
    pthread_mutex_t pcm_lock;
    struct mixer         *mixer;
    int vtsdev_fd;
    void *mapped_addr;
    int is_streaming;
    int triggered_model;
    int sthal_opened;
    bool callback_thread_active;

    int is_seamless_recording;
    struct pcm *streaming_pcm;
    int recognize_started;
    int active_mic;    //Mic to be configured
    int is_mic_configured;
    void *audio_primary_lib;
    int (*notify_sthal_status)(int);
    int (*notify_set_vts_route)(bool state, int type);

    int is_recording;
    struct pcm *recording_pcm;

    int odm_voicemodel_mode;

    int voicecall_state;
    int recog_cbstate;
    bool audio_input_state;

    struct sound_model_info sm_info[MAX_SOUND_MODELS];
    int num_sound_model;
    int num_supported_mic;
    bool concurrent_capture;
    bool support_headset_mic;
    uint32_t sysclk_div;
    bool support_silent_reset;

    /* MIC Mixer controls for VTS */
    struct mic_ctrl_info mic_ctrl;

    /* for debugging */
    bool pcm_dump_enabled;
    bool debug_verify_mode;

    uint32_t vts_status;
};

#endif  // __EXYNOS_SOUNDTRIGGERHAL_H__
