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

#ifndef __ERD_STHAL_DEFNITION_H__
#define __ERD_STHAL_DEFNITION_H__

#include "soundtrigger_conf.h"
#include "ringbuf.h"
#include "sound_trigger_hw.h"

typedef enum {
    ODM_OFF_MODE            = 0,
    ODM_TRIGGER_MODE        = 1,
} ODM_VOICE_MODEL_MODE;

#define VTS_ONESHOT_ADD_DB 6 // db
#define VTS_BARGEIN_ONESHOT_ADD_DB 30 // db

#define DEINIT_RBUF(q)    if (q != NULL) {q->deinitRbuf(q); free(q); q = NULL;}

#define MAX_PCM_PATH_LEN 256

static void *streaming_thread_loop(void *context);
int odm_open_streaming(struct sound_trigger_device *stdev);
size_t odm_read_samples(struct sound_trigger_device *stdev, void *buffer,size_t  buffer_len);
void odm_close_streaming(struct sound_trigger_device *stdev);
int streaming_start(struct sound_trigger_device *stdev);
void streaming_stop(struct sound_trigger_device *stdev);
static int trigger_capture_open(void);
static int trigger_capture_close(void);
static int read_data_from_ringbuf(struct sound_trigger_device *stdev, unsigned char *buff, int size);
void failure_callback(struct sound_trigger_device* stdev);
struct sound_trigger_phrase_recognition_event *odm_sound_trigger_event_alloc(struct sound_trigger_device *stdev, int index);
struct sound_trigger_generic_recognition_event *odm_sound_trigger_generic_event_alloc(struct sound_trigger_device *stdev, int index);


void get_vts_keyword_length(struct sound_trigger_device *stdev);

#ifdef SUPPORT_BARGEIN_MODE
void set_bargein_keyword_type(struct sound_trigger_device *stdev, char *sm_data);
void get_bargein_keyword_length(struct sound_trigger_device *stdev);
void enable_bargein(struct sound_trigger_device *stdev);
void disable_bargein(struct sound_trigger_device *stdev);
void reset_bargein(struct sound_trigger_device *stdev);
#endif

/* Sound model binaries */
#define SOUND_MODEL_OKGOOGLE_BINARY "/data/voice_dva_okgoogle.bin"
#define VTS_HAL_DUMP  "/data/vendor/audio_dump/"

#define BARGEIN_KWDTYPE_CTL_NAME       "ABOX VIVO BI KWD Type"
#define BARGEIN_KWDLENGTH_CTL_NAME     "ABOX VIVO BI KWD Length"
#define VTS_KWDLENGTH_CTL_NAME         "VTS Keyword Length"

#define VTSDRV_MISC_ODM_MODEL_BIN_MAXSZ (0x8000 + 0x300)
#define VTS_IRQ_AP_SET_MODE (22)

#endif /* __ODM_STHAL_DEFNITION_H__ */
