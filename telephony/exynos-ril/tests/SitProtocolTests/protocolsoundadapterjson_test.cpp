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
#include "json/protocolsoundadapterjson.h"
#include "soundjsondef.h"
#include "rildef.h"
#include "sitdef.h"

TEST(ProtocolSoundAdapterJson, ProtocolSoundGetMuteRespAdapterJson) {
    int testVector[] = {
        SIT_SND_MUTE_MODE_DISABLE,
        SIT_SND_MUTE_MODE_ENABLE,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_GET_MUTE;
        Json::Value jsonRspData;
        jsonRspData[ns_snd_get_mute_rsp::key_mute_mode] = testVector[i];
        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_MUTE, 0, RCM_E_SUCCESS,
        jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSoundGetMuteRespAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetMuteState(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapterJson, ProtocolSoundRingbackToneIndAdapterJson) {
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
        Json::Value jsonIndData;
        jsonIndData[ns_snd_ringback_tone_ind::key_flag] = testVector[i].data[0];
        jsonIndData[ns_snd_ringback_tone_ind::key_ringback_state] = testVector[i].expected;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(id,jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSoundRingbackToneIndAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetRingbackToneState(), testVector[i].expected);
        EXPECT_EQ(testAdapter.GetFlag(), testVector[i].data[0]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapterJson, ProtocolSoundGetVolumeRespAdapterJson) {
    for (int i = 0; i < 7; i++) {
        int volume = i;
        unsigned int id = SIT_GET_VOLUME;
        Json::Value jsonRspData;
        jsonRspData[ns_snd_get_volume_rsp::key_volume_level] = i;
        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(id, 0, RCM_E_SUCCESS,jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSoundGetVolumeRespAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetVolume(), volume);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapterJson, ProtocolSoundGetAudiopathRespAdapterJson) {
    for (int i = 0; i < 10; i++) {
        int path = i;
        unsigned int id = SIT_GET_AUDIO_PATH;

        Json::Value jsonRspData;
        jsonRspData[ns_snd_get_audio_path_rsp::key_audio_path] = path;
        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(id, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSoundGetAudiopathRespAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetAudiopath(), path);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapterJson, ProtocolSoundGetMultiMICRespAdapterJson) {
    for (int i = 0; i < 10; i++) {
        int mode = i;
        unsigned int id = SIT_GET_MULTI_MIC;
        Json::Value jsonRspData;
        jsonRspData[ns_snd_get_multi_mic_rsp::key_multi_mic_on_off] = mode;
        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(id, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSoundGetMultiMICRespAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetMultimicmode(), mode);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapterJson, ProtocolSoundWBAMRReportAdapterJson) {
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
        Json::Value jsonIndData;
        jsonIndData[ns_snd_wb_amr_report_ind::key_status] = testVector[i][0];
        jsonIndData[ns_snd_wb_amr_report_ind::key_rat] = testVector[i][1];
        ModemData *modemData = ModemDataBuilder::BuildJsonInd(id, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSoundWBAMRReportAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetStatus(), testVector[i][0]);
        EXPECT_EQ(testAdapter.GetCallType(), testVector[i][1]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSoundAdapterJson, ProtocolSoundGetWBAMRCapabilityAdapterJson) {
    for (int i = 0; i < 10; i++) {

        int data = i;
        unsigned int id = SIT_GET_WB_CAPABILITY;
        Json::Value jsonRspData;
        jsonRspData[ns_snd_get_wb_capability_rsp::key_wbamr] = data;
        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(id, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSoundGetWBAMRCapabilityAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetWbAmr(), data);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}
