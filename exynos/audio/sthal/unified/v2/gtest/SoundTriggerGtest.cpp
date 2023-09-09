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

#include <gtest/gtest.h>

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "SoundTriggerGtest.h"
#include "../vts.h"
#include "st_xml_parser.h"

TEST(VtsDev, openFdTest) {
    int fd = open(VTS_DEV, O_RDWR);
    EXPECT_LE(0, fd);
    close(fd);
}

TEST(LoadSoundModel, actualSizeOverFlowTest) {
    int fd = open(VTS_DEV, O_RDWR);
    struct vts_model_bin_info sm_config;
    sm_config.actual_sz = VTSDRV_MISC_MODEL_BIN_MAXSZ + 1;
    sm_config.max_sz = VTSDRV_MISC_MODEL_BIN_MAXSZ;
    sm_config.offset = MODEL_MEM_OFFSET;

    int err = ioctl(fd, VTSDRV_MISC_IOCTL_LOAD_SOUND_MODEL, &sm_config);
    EXPECT_GT(0, err);
    close(fd);
}

TEST(LoadSoundModel, actualSizeUnderFlowTest) {
    int fd = open(VTS_DEV, O_RDWR);
    struct vts_model_bin_info sm_config;
    sm_config.actual_sz = -1;
    sm_config.max_sz = VTSDRV_MISC_MODEL_BIN_MAXSZ;
    sm_config.offset = MODEL_MEM_OFFSET;

    int err = ioctl(fd, VTSDRV_MISC_IOCTL_LOAD_SOUND_MODEL, &sm_config);
    EXPECT_GT(0, err);
    close(fd);
}

TEST(XMLTest, configValidationTest) {
    sound_trigger_device stdev;
    st_xml_parser(&stdev);

    EXPECT_GT(stdev.num_sound_model, 0);
    EXPECT_GE(stdev.num_supported_mic, 1);
    EXPECT_GT(stdev.sysclk_div, 0);
}

TEST(MixerTest, mixerOpenTest) {
    sound_trigger_device stdev;
    stdev.mixer = NULL;
    stdev.mixer = mixer_open(VTS_MIXER_CARD);

    EXPECT_TRUE(stdev.mixer);

    mixer_close(stdev.mixer);
}
