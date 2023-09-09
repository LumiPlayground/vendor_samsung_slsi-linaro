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

#define LOG_TAG "st_pcm_dump"
//#define LOG_NDEBUG 0

#include <stdlib.h>
#include <errno.h>
#include <log/log.h>
#include <time.h>

#include "sound_trigger_hw.h"
#include "st_pcm_dump.h"

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164
#define FORMAT_PCM 1

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

static FILE *fp_pcm = NULL;
static FILE *fp_wav = NULL;
static int total_size = 0;
static struct wav_header header;

void st_pcm_dump_init(struct pcm_config config, char *file_name)
{
    char pcm_file_path[100];
    char wav_file_path[100];
    time_t raw_time;
    struct tm* time_info;

    /* initialize the Wav Header information */
    header.riff_id = ID_RIFF;
    header.riff_sz = 0;
    header.riff_fmt = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;
    header.audio_format = FORMAT_PCM;
    header.num_channels = config.channels;
    header.sample_rate = config.rate;

    header.bits_per_sample = pcm_format_to_bits(PCM_FORMAT_S16_LE);
    header.byte_rate = (header.bits_per_sample / 8) * config.channels * config.rate;
    header.block_align = config.channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;

    total_size = 0;

    raw_time = time(NULL);
    time_info = localtime(&raw_time);

    sprintf(pcm_file_path, ST_PCM_DUMP_PATH"%d-%02d-%02d_%02dh_%02dm_%02ds_%s.pcm",
        time_info->tm_year + 1900, time_info->tm_mon + 1,
        time_info->tm_mday, time_info->tm_hour,
        time_info->tm_min, time_info->tm_sec, file_name);

    sprintf(wav_file_path, ST_PCM_DUMP_PATH"%d-%02d-%02d_%02dh_%02dm_%02ds_%s.wav",
        time_info->tm_year + 1900, time_info->tm_mon + 1,
        time_info->tm_mday, time_info->tm_hour,
        time_info->tm_min, time_info->tm_sec, file_name);

    if (fp_pcm)
        fclose(fp_pcm);

    if (fp_wav)
        fclose(fp_wav);

    fp_pcm = fopen(pcm_file_path, "w+");
    fp_wav = fopen(wav_file_path, "w+");

    if (!fp_pcm || !fp_wav) {
        ALOGE("%s: Failed to open, pcm_path:%s wav_path:%s",
                __func__, pcm_file_path, wav_file_path);
        return;
    }

    fseek(fp_wav, sizeof(struct wav_header), SEEK_SET);

    ALOGD("%s: init success, pcm_path:%s wav_path:%s",
            __func__, pcm_file_path, wav_file_path);
}

void st_pcm_dump_write(void *buffer, unsigned int size)
{
    if (fp_pcm)
        fwrite((void*)buffer, size, 1, fp_pcm);

    if (fp_wav)
        fwrite((void*)buffer, size, 1, fp_wav);

    total_size += size;
}

void st_pcm_dump_close(struct pcm *pcm)
{
    unsigned int frames = 0;

    if (fp_pcm) {
        fclose(fp_pcm);
        fp_pcm = NULL;
    }

    if (fp_wav) {
        if (pcm) {
            frames = pcm_bytes_to_frames(pcm, total_size);
        } else {
            ALOGE("%s: PCM node is NULL", __func__);
        }

        header.data_sz = frames * header.block_align;
        header.riff_sz = (uint32_t)(header.data_sz + sizeof(header) - 8);
        fseek(fp_wav, 0, SEEK_SET);
        fwrite(&header, sizeof(struct wav_header), 1, fp_wav);

        fclose(fp_wav);
        fp_wav = NULL;
    }

    total_size = 0;
}

