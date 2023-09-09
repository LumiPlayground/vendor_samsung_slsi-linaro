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

#ifndef __VOLCANO_STHAL_DEFNITION_H__
#define __VOLCANO_STHAL_DEFNITION_H__

#include "sound_trigger_interface_odm.h"
#include "ringbuf.h"

typedef enum {
    ODM_OFF_MODE            = 0,
    ODM_TRIGGER_MODE        = 1,
} ODM_VOICE_MODEL_MODE;

#define VTS_ONESHOT_ADD_DB 6 // db
#define VTS_BARGEIN_ONESHOT_ADD_DB 30 // db

#define DEINIT_RBUF(q)    if (q != NULL) {q->deinitRbuf(q); free(q); q = NULL;}

#define MAX_PCM_PATH_LEN 256

/* Sound model binaries */
#define SOUND_MODEL_OKGOOGLE_BINARY "/data/voice_dva_okgoogle.bin"
#define VTS_HAL_DUMP  "/data/vendor/audio_dump/"

#define BARGEIN_KWDTYPE_CTL_NAME       "ABOX VIVO BI KWD Type"
#define BARGEIN_KWDLENGTH_CTL_NAME     "ABOX VIVO BI KWD Length"
#define VTS_KWDLENGTH_CTL_NAME         "VTS Keyword Length"

#define VTSDRV_MISC_ODM_MODEL_BIN_MAXSZ (0x8000 + 0x300)
#define VTS_IRQ_AP_SET_MODE (22)

struct odm_sound_trigger_device {
    int stop_read_data;
    FILE *voice_cmd_fp;
    int kw_length;
    int kw_count;
    int kw_type; // APP use 1 XVXV 2 HiJovi, vts use 2 XVXV 1 HiJovi
    int stream_stop;
    int stream_gain;
    bool streaming_thread_start;
    ringbuf *rbuf;
    signed short *stream_amp_buf;
    pthread_t streaming_thread;
    pthread_mutex_t streaming_pcm_lock;
    pthread_mutex_t streaming_open_lock;
    pthread_cond_t streaming_open_cond;
    pthread_mutex_t streaming_thread_loop_lock;
#ifdef SUPPORT_BARGEIN_MODE
    bool requires_bargein_mode;
    bool is_bargein_mode_configed;
    bool sound_card_abnormal_state;
    int (*notify_set_bargein_route)(bool);
#endif
};

#endif /* __ODM_STHAL_DEFNITION_H__ */
