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
#pragma once

#include <tinyalsa/asoundlib.h>
#include <system/sound_trigger.h>
#include <hardware/sound_trigger.h>
#include "soundtrigger_conf.h"

#define MAX_SOUND_MODELS 3
#define MAX_CNT 20
#define MAX_LEN 30

#define VTSDRV_MISC_MODEL_BIN_MAXSZ 0x10800
#define MODEL_MEM_OFFSET 0x1A6800
#define VTS_DEV "/dev/vts_fio_dev"

struct vts_model_bin_info {
    int64_t actual_sz;
    int64_t max_sz;
    int64_t offset;
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
    struct mixer *mixer;
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

    /* MIC Mixer controls for VTS */
    struct mic_ctrl_info mic_ctrl;

    /* for debugging */
    bool pcm_dump_enabled;
    bool debug_verify_mode;
};


