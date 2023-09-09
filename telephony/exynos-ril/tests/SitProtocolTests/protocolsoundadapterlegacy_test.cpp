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
#include "legacy/protocolsoundadapterlegacy.h"
#include "rildef.h"
#include "sitdef.h"

TEST(ProtocolSoundAdapter, ProtocolSoundGetMuteRespAdapterLegacy) {
    int testVector[] = {
        SIT_SND_MUTE_MODE_DISABLE,
        SIT_SND_MUTE_MODE_ENABLE,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_GET_MUTE;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                SIT_GET_MUTE, 0, RCM_E_SUCCESS,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSoundGetMuteRespAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetMuteState(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapter, ProtocolSoundRingbackToneIndAdapterLegacy) {
    struct {
        int data[2];
        int expected;
    } testVector[] = {
        { { SIT_SND_RINGBACK_STATE_STOP, 0x05 }, RIL_SND_RINGBACK_TONE_END },
        { { SIT_SND_RINGBACK_STATE_START, 0x0A }, RIL_SND_RINGBACK_TONE_START },
        { { SIT_SND_RINGBACK_STATE_START + 3, 0x0F }, RIL_SND_RINGBACK_TONE_START }, // default
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_IND_RINGBACK_TONE;
        ModemData *modemData = ModemDataBuilder::BuildIndication(
                id, testVector[i].data, sizeof(testVector[i].data));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSoundRingbackToneIndAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i].data));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetRingbackToneState(), testVector[i].expected);
        EXPECT_EQ(testAdapter.GetFlag(), testVector[i].data[1]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapter, ProtocolSoundGetVolumeRespAdapterLegacy) {
    for (int i = 0; i < 7; i++) {
        int volume = i;
        unsigned int id = SIT_GET_VOLUME;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &volume, sizeof(volume));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSoundGetVolumeRespAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(volume));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetVolume(), volume);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapter, ProtocolSoundGetAudiopathRespAdapterLegacy) {
    for (int i = 0; i < 10; i++) {
        int path = i;
        unsigned int id = SIT_GET_AUDIO_PATH;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &path, sizeof(path));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSoundGetAudiopathRespAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(path));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetAudiopath(), path);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapter, ProtocolSoundGetMultiMICRespAdapterLegacy) {
    for (int i = 0; i < 10; i++) {
        int mode = i;
        unsigned int id = SIT_GET_MULTI_MIC;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &mode, sizeof(mode));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSoundGetMultiMICRespAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(mode));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetMultimicmode(), mode);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapter, ProtocolSoundWBAMRReportAdapterLegacy) {
    uint8_t testVector[][2] = {
        { SITRIL_AUDIO_WB_AMR_REPORT_STATUS_OFF, SIT_AUDIO_CALL_TYPE_UNKNOWN },
        { SITRIL_AUDIO_WB_AMR_REPORT_STATUS_ON, SIT_AUDIO_CALL_TYPE_GSM },
        { SITRIL_AUDIO_WB_AMR_REPORT_STATUS_NB, SIT_AUDIO_CALL_TYPE_CDMA },
        { SITRIL_AUDIO_WB_AMR_REPORT_STATUS_WB, SIT_AUDIO_CALL_TYPE_IMS },
        { SITRIL_AUDIO_WB_AMR_REPORT_STATUS_SWB, SIT_AUDIO_CALL_TYPE_OTHERS },
        { SITRIL_AUDIO_WB_AMR_REPORT_STATUS_FB, SIT_AUDIO_CALL_TYPE_IMS },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_IND_WB_AMR_REPORT;
        ModemData *modemData = ModemDataBuilder::BuildIndication(
                id, &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSoundWBAMRReportAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetStatus(), testVector[i][0]);
        EXPECT_EQ(testAdapter.GetCallType(), testVector[i][1]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapter, ProtocolSoundGetWBAMRCapabilityAdapterLegacy) {
    for (int i = 0; i < 10; i++) {
        int data = i;
        unsigned int id = SIT_GET_WB_CAPABILITY;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS, &data, sizeof(data));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSoundGetWBAMRCapabilityAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(data));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetWbAmr(), data);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}
