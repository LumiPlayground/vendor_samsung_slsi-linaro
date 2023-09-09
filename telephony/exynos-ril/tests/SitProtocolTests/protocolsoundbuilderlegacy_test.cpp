/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include <gtest/gtest.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "protocoladapter.h"
#include "legacy/protocolsoundbuilderlegacy.h"

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildGetMute) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetMute();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_MUTE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildSetMute) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetMute(1);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_MUTE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_snd_set_mute_req));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(int32_t));
    EXPECT_EQ(((sit_snd_set_mute_req *)modemData->GetRawData())->mute_mode, 1);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildGetVolume) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetVolume();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_VOLUME);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildSetAudioPath) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetAudioPath(SITRIL_AUDIO_PATH_HANDSET);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_AUDIO_PATH);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_snd_set_audiopath_req));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(int32_t));
    EXPECT_EQ(((sit_snd_set_audiopath_req *)modemData->GetRawData())->audiopath,
            SITRIL_AUDIO_PATH_HANDSET);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildGetAudioPath) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetAudioPath();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_AUDIO_PATH);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildSetMultiMic) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetMultiMic(SITRIL_MULTIMIC_ONE);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_MULTI_MIC);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_snd_set_multimic_req));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(int32_t));
    EXPECT_EQ(((sit_snd_set_multimic_req *)modemData->GetRawData())->multiMICmode,
            SITRIL_MULTIMIC_ONE);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildGetMultiMic) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetMultiMic();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_MULTI_MIC);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildSetAudioClock) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetAudioClock(5);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_AUDIO_CLK);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_snd_set_clock_mode_req));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(uint8_t));
    EXPECT_EQ(((sit_snd_set_clock_mode_req *)modemData->GetRawData())->clock_mode, 5);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildSetAudioLoopback) {
    struct {
        int onoff;
        int path;
        int expectedPath;
    } testVector[] = {
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_NA - 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_NA, SITRIL_AUDIO_LOOPBACK_PATH_NA },
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_LINEOUT, SITRIL_AUDIO_LOOPBACK_PATH_LINEOUT },
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_LINEOUT + 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSET - 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSET, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSET },
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_LINEOUT, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_LINEOUT },
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_LINEOUT + 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC1 - 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC1, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC1 },
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC3, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC3 },
            { 0, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC3 + 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET }, // invalid
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_NA - 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_NA, SITRIL_AUDIO_LOOPBACK_PATH_NA },
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_LINEOUT, SITRIL_AUDIO_LOOPBACK_PATH_LINEOUT },
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_LINEOUT + 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSET - 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSET, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSET },
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_LINEOUT, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_LINEOUT },
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_LINEOUT + 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC1 - 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET },  // invalid
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC1, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC1 },
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC3, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC3 },
            { 1, SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC3 + 1, SITRIL_AUDIO_LOOPBACK_PATH_HANDSET }, // invalid
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSoundBuilderLegacy testBuilder;
        ModemData *modemData = testBuilder.BuildSetAudioLoopback(testVector[i].onoff, testVector[i].path);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_AUDIO_LOOPBACK);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_snd_set_loopback_req));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(uint32_t) * 2);
        sit_snd_set_loopback_req *p = (sit_snd_set_loopback_req *)modemData->GetRawData();
        EXPECT_EQ(p->on_off, testVector[i].onoff);
        EXPECT_EQ(p->audio_path, testVector[i].expectedPath);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildSwitchVoiceCallAudio) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSwitchVoiceCallAudio(1);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SWITCH_VOICE_CALL_AUDIO);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_snd_switch_voice_call_audio_req));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(uint8_t));
    EXPECT_EQ(((sit_snd_set_clock_mode_req *)modemData->GetRawData())->clock_mode, 1);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildSetWbAmrCapability) {
    ProtocolSoundBuilderLegacy testBuilder;
    EXPECT_EQ(testBuilder.BuildSetWbAmrCapability(-1), nullptr);
    EXPECT_EQ(testBuilder.BuildSetWbAmrCapability(SITRIL_AUDIO_WMAMR_MAX), nullptr);

    auto modemData = std::unique_ptr<ModemData>(
                testBuilder.BuildSetWbAmrCapability(SITRIL_AUDIO_WMAMR_3GWIDE_2GNARROW));
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_WB_CAPABILITY);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_snd_set_wbmar_capability_req));
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(uint32_t));
    EXPECT_EQ(((sit_snd_set_wbmar_capability_req *)modemData->GetRawData())->wbamr,
            SITRIL_AUDIO_WMAMR_3GWIDE_2GNARROW);
}

TEST(ProtocolSoundBuilderLegacy, ProtocolSoundBuilder_BuildGetWbAmrCapability) {
    ProtocolSoundBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetWbAmrCapability();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_WB_CAPABILITY);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_snd_get_wbmar_capability_req));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}
