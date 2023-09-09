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

#include "json/protocolsoundbuilderjson.h"
#include "protocoljsontestadapter.h"
#include "soundjsondef.h"
#include "modemdata.h"

TEST(ProtocolSoundBuilderJson, BuildGetMute) {
    ProtocolSoundBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetMute();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_MUTE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderJson, BuildSetMute) {
    int testVector[] = {0, 1};

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSoundBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetMute(testVector[i]);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_MUTE);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) {
            delete modemData;
            modemData = nullptr;
        }
        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i], jsonReqData[ns_snd_set_mute_req::key_mute_mode].asInt());

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }
}

TEST(ProtocolSoundBuilderJson, BuildSetVolume) {
    int testVector[] = {0, 1, 2, 3, 4, 5};

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSoundBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetVolume(testVector[i]);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_VOLUME);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) {
            delete modemData;
            modemData = nullptr;
        }
        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i], jsonReqData[ns_snd_set_volume_req::key_volume_level].asInt());

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }
}

TEST(ProtocolSoundBuilderJson, BuildGetVolume) {
    ProtocolSoundBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetVolume();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_VOLUME);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderJson, BuildSetAudioPath) {
    ProtocolSoundBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetAudioPath(SITRIL_AUDIO_PATH_HANDSET);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_AUDIO_PATH);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) {
        delete modemData;
        modemData = nullptr;
    }
    ASSERT_NE(false, parsed);
    EXPECT_EQ(SITRIL_AUDIO_PATH_HANDSET, jsonReqData[ns_snd_set_audio_path_req::key_audio_path].asInt());

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderJson, BuildGetAudioPath) {
    ProtocolSoundBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetAudioPath();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_AUDIO_PATH);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderJson, BuildSetMultiMic) {
    ProtocolSoundBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetMultiMic(SITRIL_MULTIMIC_ONE);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_MULTI_MIC);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) {
        delete modemData;
        modemData = nullptr;
    }
    ASSERT_NE(false, parsed);
    EXPECT_EQ(SITRIL_MULTIMIC_ONE, jsonReqData[ns_snd_set_multi_mic_req::key_multi_mic_on_off].asInt());

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderJson, BuildGetMultiMic) {
    ProtocolSoundBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetMultiMic();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_MULTI_MIC);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderJson, BuildSetAudioClock) {
    ProtocolSoundBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetAudioClock(5);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_AUDIO_CLK);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) {
        delete modemData;
        modemData = nullptr;
    }
    ASSERT_NE(false, parsed);
    EXPECT_EQ(5, jsonReqData[ns_snd_set_audio_clock_req::key_clock_mode].asInt());


    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderJson, BuildSetAudioLoopback) {
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
        ProtocolSoundBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetAudioLoopback(testVector[i].onoff, testVector[i].path);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_AUDIO_LOOPBACK);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) {
            delete modemData;
            modemData = nullptr;
        }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i].onoff,jsonReqData[ns_snd_set_audio_loopback_req::key_on_off].asInt());
        EXPECT_EQ(testVector[i].expectedPath,jsonReqData[ns_snd_set_audio_loopback_req::key_audio_path].asInt());

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundBuilderJson, BuildSetWbAmrCapability) {
    ProtocolSoundBuilderJson testBuilder;
    EXPECT_EQ(testBuilder.BuildSetWbAmrCapability(-1), nullptr);
    EXPECT_EQ(testBuilder.BuildSetWbAmrCapability(SITRIL_AUDIO_WMAMR_MAX), nullptr);

    ModemData *modemData =
            testBuilder.BuildSetWbAmrCapability(SITRIL_AUDIO_WMAMR_3GWIDE_2GNARROW);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_WB_CAPABILITY);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) {
       delete modemData;
       modemData = nullptr;
    }

    ASSERT_NE(false, parsed);
    EXPECT_EQ(SITRIL_AUDIO_WMAMR_3GWIDE_2GNARROW,jsonReqData[ns_snd_set_wb_amr_3g_req::key_wbamr].asInt());

    if(modemData){
       delete modemData;
       modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderJson, BuildGetWbAmrCapability) {
    ProtocolSoundBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetWbAmrCapability();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_WB_CAPABILITY);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSoundBuilderJson, BuildSwitchVoiceCallAudio) {
    ProtocolSoundBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSwitchVoiceCallAudio(0);
    EXPECT_EQ(modemData, nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}
