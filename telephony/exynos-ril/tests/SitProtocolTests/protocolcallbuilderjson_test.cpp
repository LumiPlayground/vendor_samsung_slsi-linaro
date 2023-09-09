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

#include <modemdata.h>
#include <fw/callreqdata.h>
#include "protocoljsontestadapter.h"
#include "json/cs/protocolcallbuilderjson.h"
#include "calljsondef.h"
#include "tokengen.h"
#include <android/log.h>

using namespace std;

TEST(ProtocolCallBuilderJson, BuildGetCallList) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetCallList();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CURRENT_CALLS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildDial) {
    TokenGen::Init();

    struct {
        char number[MAX_DIAL_NUM + 1] = {0};
        int cpNumberType;
        UusInfo uusInfo; // This is not in use
        ClirType dialClirType;
        int cpClirType;
        CallType cType;
        UINT eccCat;
    } testVector [] = {
        { "+919876543210", SIT_CALL_NUMBER_TYPE_INTERNATIONAL, UusInfo(), CLIR_DEFAULT, SIT_CALL_CLIR_DEFAULT, CALL_TYPE_VOICE, 0},
        { "9876543210", SIT_CALL_NUMBER_TYPE_NATIONAL, UusInfo(), CLIR_DEFAULT, SIT_CALL_CLIR_DEFAULT, CALL_TYPE_VOICE, 0},
    };

    ProtocolCallBuilderJson testBuilder;

    for(auto td : testVector){
        ModemData *modemData = testBuilder.BuildDial(td.number, td.dialClirType,
                td.uusInfo, td.cType, td.eccCat);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_DIAL);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        sit_call_call_type_e_type cpCallType = (sit_call_call_type_e_type)jsonReqData[ns_call_dial_req::key_call_type].asInt();
        EXPECT_EQ(cpCallType, SIT_CALL_CALL_TYPE_VOICE);

        sit_call_emergency_call_subtype_e_type cpEmcCallType = (sit_call_emergency_call_subtype_e_type)jsonReqData[ns_call_dial_req::key_emc_call_sub_type].asInt();
        EXPECT_EQ(cpEmcCallType, SIT_CALL_EMERGENCY_CALL_SUBTYPE_DEFAULT_EMERGENCY_CENTER); //Service is always setting EMC call type as default

        string cpStringNumber = jsonReqData[ns_call_dial_req::key_number].asString();
        const char *cpNumber = cpStringNumber.c_str();
        EXPECT_EQ(strcmp(cpNumber, td.number), 0);

        sit_call_number_type_e_type cpNumberType = (sit_call_number_type_e_type)jsonReqData[ns_call_dial_req::key_number_type].asInt();
        EXPECT_EQ(cpNumberType, td.cpNumberType);

        sit_call_number_plan_e_type cpNumberPlan = (sit_call_number_plan_e_type)jsonReqData[ns_call_dial_req::key_number_plan].asInt();
        EXPECT_EQ(cpNumberPlan, SIT_CALL_NUMBER_PLAN_ISDN); // We are always setting is as ISDN

        sit_call_clir_e_type cpClirType = (sit_call_clir_e_type)jsonReqData[ns_call_dial_req::key_clir].asInt();
        EXPECT_EQ(cpClirType, td.cpClirType);

        sit_call_cug_call_e_type cpCugCallType = (sit_call_cug_call_e_type)jsonReqData[ns_call_dial_req::key_cug_call].asInt();
        EXPECT_EQ(cpCugCallType, SIT_CALL_CUG_CALL_DISABLED); // We are always setting is disabled

        int cpCugInfoIndex = jsonReqData[ns_call_dial_req::key_cug_info_index].asInt();
        EXPECT_EQ(cpCugInfoIndex, 0); // We are always setting it 0

        sit_call_cug_info_e_type cpCugInfo = (sit_call_cug_info_e_type)jsonReqData[ns_call_dial_req::key_cug_info].asInt();
        EXPECT_EQ(cpCugInfo, SIT_CALL_CUG_INFO_NONE); // We are always setting it as NONE

        delete modemData;
        modemData = nullptr;
    }

}

TEST(ProtocolCallBuilderJson, BuildLastCallFailCause) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildLastCallFailCause();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_LAST_CALL_FAIL_CAUSE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildAnswer) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildAnswer();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_ANSWER);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildExplicitCallTransfer) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildExplicitCallTransfer();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SEND_EXPLICIT_CALL_TRANSFER);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildHangup) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildHangup(1 /*call index*/);
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_HANGUP);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) { delete modemData; modemData = nullptr; }

    ASSERT_NE(false, parsed);

    int cpCallIndex = jsonReqData[ns_call_hangup_req::key_call_index].asInt();
    EXPECT_EQ(cpCallIndex, 1);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildHangupMulti) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildHangupMulti(1 /*call index*/);
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_HANGUP);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) { delete modemData; modemData = nullptr; }

    ASSERT_NE(false, parsed);

    int cpCallIndex = jsonReqData[ns_call_hangup_req::key_call_index].asInt();
    EXPECT_EQ(cpCallIndex, 1);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildSendCallConfirm) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSendCallConfirm();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_CALL_CONFIRM);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildCdmaSetPreferredVoicePrivacyMode) {
    TokenGen::Init();

    struct {
        int rilVPMode;
        int cpVPMode;
    } testVector[] = {
        { 0x00, 0x00 }, // Standard privacy mode (public long code mask)
        { 0x01, 0x01 }, // Enhanced privary mode (private long code mask)
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolCallBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildCdmaSetPreferredVoicePrivacyMode(testVector[i].rilVPMode);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_CDMA_VOICE_PRIVACY_MODE);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(parsed, false);

        EXPECT_EQ(jsonReqData[ns_call_set_cdma_voice_privacy_mode_req::key_vp_mode], testVector[i].cpVPMode);

        delete modemData;
        modemData = nullptr;
    }  // end for i ~
}

TEST(ProtocolCallBuilderJson, BuildCdmaGetPreferredVoicePrivacyMode) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildCdmaGetPreferredVoicePrivacyMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CDMA_VOICE_PRIVACY_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildCdmaBurstDtmf) {
    TokenGen::Init();

    struct {
        char dtmfDigits[MAX_BURST_DTMF_LEN + 1] = {0};
        char dtmfOnLength[MAX_BURST_DTMF_ON_OFF_LEN + 1] = {0};
        char dtmfOffLength[MAX_BURST_DTMF_ON_OFF_LEN + 1] = {0};
    } testVector[] = {
        { "*123#", "123", "1"},
        { "*1232345692#", "", "123654"},
        { "#", "1ddf3", "1255"},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolCallBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildCdmaBurstDtmf(
                strlen(testVector[i].dtmfDigits), testVector[i].dtmfDigits,
                strlen(testVector[i].dtmfOnLength), testVector[i].dtmfOnLength,
                strlen(testVector[i].dtmfOffLength), testVector[i].dtmfOffLength);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_CDMA_BURST_DTMF);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(parsed, false);

        EXPECT_EQ(jsonReqData[ns_call_cdma_burst_dtmf_req::key_dtmf_digits], testVector[i].dtmfDigits);
        EXPECT_EQ(jsonReqData[ns_call_cdma_burst_dtmf_req::key_dtmf_on_length], testVector[i].dtmfOnLength);
        EXPECT_EQ(jsonReqData[ns_call_cdma_burst_dtmf_req::key_dtmf_off_length], testVector[i].dtmfOffLength);

        delete modemData;
        modemData = nullptr;
    }  // end for i ~
}

TEST(ProtocolCallBuilderJson, BuildHangupWaitingOrBackground) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildHangupWaitingOrBackground();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_HANGUP_WAITING_OR_BACKGROUND);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildHangupForegroundResumeBackground) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildHangupForegroundResumeBackground();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_HANGUP_FORGROUND_RESUME_BACKGROUND);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildSwitchWaitingOrHoldingAndActive) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSwitchWaitingOrHoldingAndActive();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildConference) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildConference();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_CONFERENCE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildUdub) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildUdub();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_UDUB);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildSendUssd) {
    TokenGen::Init();

    struct {
        const char* ussd;
        unsigned int encoded_ussd_len;
        int encoded_ussd[MAX_USSD_DATA_LEN];
        bool user_initiated;
        int ussd_type;
    } testVector[] = {
        { "*141#", 5, { 0xaa, 0x18, 0x2d, 0x36, 0x02}, 1, SIT_SS_USSD_USER_INITIATED },
        { "*123#", 5, { 0xaa, 0x98, 0x6c, 0x36, 0x02}, 1, SIT_SS_USSD_USER_INITIATED },
        { "1", 1, { 0x31 }, 0, SIT_SS_USSD_USER_RESPONSE },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolCallBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSendUssd(testVector[i].ussd, testVector[i].user_initiated);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_USSD);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i].ussd_type, jsonReqData[ns_ss_send_ussd_req::key_ussd_type].asInt());
        EXPECT_EQ(0x0f, jsonReqData[ns_ss_send_ussd_req::key_dcs_type].asInt());
        EXPECT_EQ(testVector[i].encoded_ussd_len, jsonReqData[ns_ss_send_ussd_req::key_ussd_data].size());
        for (unsigned int j = 0; j < testVector[i].encoded_ussd_len; j++) {
            EXPECT_EQ(testVector[i].encoded_ussd[j], jsonReqData[ns_ss_send_ussd_req::key_ussd_data][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildSendEncodedUssd) {
    TokenGen::Init();

    struct {
        BYTE dcs;
        const char* encoded_ussd;  // in hex string format
        unsigned int encoded_ussd_data_len;
        int encoded_ussd_data[MAX_USSD_DATA_LEN];
        bool user_initiated;
        int ussd_type;
    } testVector[] = {
        { 0x0f, "aa986c3602", 5, { 0xaa, 0x98, 0x6c, 0x36, 0x02 }, 1, SIT_SS_USSD_USER_INITIATED },
        { 0x0f, "aa182d3602", 5, { 0xaa, 0x18, 0x2d, 0x36, 0x02 }, 1, SIT_SS_USSD_USER_INITIATED },
        { 0x0f, "31", 1, { 0x31 }, 0, SIT_SS_USSD_USER_RESPONSE },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolCallBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSendEncodedUssd(testVector[i].dcs,
                testVector[i].encoded_ussd, testVector[i].user_initiated);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_USSD);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i].ussd_type, jsonReqData[ns_ss_send_ussd_req::key_ussd_type].asInt());
        EXPECT_EQ(testVector[i].dcs, jsonReqData[ns_ss_send_ussd_req::key_dcs_type].asInt());
        EXPECT_EQ(testVector[i].encoded_ussd_data_len, jsonReqData[ns_ss_send_ussd_req::key_ussd_data].size());
        for (unsigned int j = 0; j < testVector[i].encoded_ussd_data_len; j++) {
            EXPECT_EQ(testVector[i].encoded_ussd_data[j], jsonReqData[ns_ss_send_ussd_req::key_ussd_data][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildCancelUssd) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildCancelUssd();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_CANCEL_USSD);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildGetClir) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetClir();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CLIR);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildQueryColp) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildQueryColp();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_COLP);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildSetCallWaiting) {
    TokenGen::Init();

    struct {
        int status;
        int sit_status;
        int service_class;
    } testVector[] = {
        { RIL_SS_MODE_DISABLE, SIT_SS_CALL_FARWARD_STATUS_DISABLE, RIL_SS_CLASS_VOICE },
        { RIL_SS_MODE_ENABLE, SIT_SS_CALL_FARWARD_STATUS_ENABLE, RIL_SS_CLASS_DATA },
        { RIL_SS_MODE_DISABLE, SIT_SS_CALL_FARWARD_STATUS_DISABLE, RIL_SS_CLASS_ALL_DEDICATED_PAD_ACCESS },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolCallBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetCallWaiting((SsModeType) testVector[i].status,
                (SsClassX) testVector[i].service_class);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_CALL_WAITING);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i].sit_status, jsonReqData[ns_ss_set_call_waiting_req::key_service_status].asInt());
        EXPECT_EQ(testVector[i].service_class, jsonReqData[ns_ss_set_call_waiting_req::key_service_class].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildGetCallWaiting) {
    TokenGen::Init();

    struct {
        int service_class;
    } testVector[] = {
        { RIL_SS_CLASS_VOICE },
        { RIL_SS_CLASS_DATA },
        { RIL_SS_CLASS_ALL_DEDICATED_PAD_ACCESS },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolCallBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildGetCallWaiting((SsClassX) testVector[i].service_class);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_CALL_WAITING);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i].service_class, jsonReqData[ns_ss_get_call_waiting_req::key_service_class].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildChangeBarringPwd) {
    TokenGen::Init();

    struct {
        const char* old_pswrd;
        const char* new_pswrd;
        const char* new_pswrd_again;
        bool isLenInvalid;  // case when password length is more than 4
    } testVector[] = {
        { "1234", "2344", "2344", false },
        { "0000", "9999", "9999", false },
        { "123456", "234412", "234412", true },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolCallBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildChangeBarringPwd(testVector[i].old_pswrd,
                testVector[i].new_pswrd, testVector[i].new_pswrd_again);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_CHG_BARRING_PWD);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        string pswrdStr = jsonReqData[ns_ss_change_barring_pwd_req::key_password].asString();
        const char* pswrd = pswrdStr.c_str();
        string newPswrdStr = jsonReqData[ns_ss_change_barring_pwd_req::key_new_password].asString();
        const char* newPswrd = newPswrdStr.c_str();
        string newPswrdAgainStr = jsonReqData[ns_ss_change_barring_pwd_req::key_new_password_again].asString();
        const char* newPswrdAgain = newPswrdAgainStr.c_str();
        // case when password length is more than 4
        if (testVector[i].isLenInvalid) {
            EXPECT_EQ(strcmp("1234", pswrd), 0);
            EXPECT_EQ(strcmp("2344", newPswrd), 0);
            EXPECT_EQ(strcmp("2344", newPswrdAgain), 0);
        } else {
            EXPECT_EQ(strcmp(testVector[i].old_pswrd, pswrd), 0);
            EXPECT_EQ(strcmp(testVector[i].new_pswrd, newPswrd), 0);
            EXPECT_EQ(strcmp(testVector[i].new_pswrd_again, newPswrdAgain), 0);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildSeparateConnection) {
    TokenGen::Init();

    int testVector[] = { 0, 1, 10, 210, 255 };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolCallBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSeparateConnection(testVector[i]);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEPARATE_CONNECTION);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i], jsonReqData[ns_ss_separate_connection_req::key_index].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildGetClip) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetClip();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CLIP);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildQueryColr) {
    TokenGen::Init();

    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildQueryColr();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_COLR);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallBuilderJson, BuildCdmaFlash) {
    TokenGen::Init();

    struct {
        const char* flash;
        bool isLenValid;  // handle case when flash length is more than MAX_FLASH_LEN
    } testVector[] = {
        { "18515947922", true },
        { "879281720101221", true },
        { "2637451289876345362109872345627182763", false },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolCallBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildCdmaFlash(testVector[i].flash);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_CDMA_FLASH_INFO);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        string flashStr = jsonReqData[ns_ss_set_cdma_flash_info_req::key_flash].asString();
        const char* flash = flashStr.c_str();
        // case when flash length is more than MAX_FLASH_LEN
        if (!testVector[i].isLenValid) {
            EXPECT_EQ(strcmp("26374512898763453621098723456271827", flash), 0);
        } else {
            EXPECT_EQ(strcmp(testVector[i].flash, flash), 0);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildSetCallForwarding) {
    TokenGen::Init();

    struct {
        SsModeType status;
        SsCfReason reason;
        SsClassX srvc_class;
        int toa;
        char number[MAX_SS_NUM_LEN + 2];
        int timeseconds;
        bool isLenAndTimeSecsValid;  // handle case when num len is more than MAX_SS_NUM_LEN and timeseconds is default
    } testVector[] = {
        { RIL_SS_MODE_ENABLE, RIL_SS_CF_REASON_MOBILE_BUSY, RIL_SS_CLASS_VOICE, RIL_TOA_INTERNATIONAL,
            "+867566122779", 21, true },
        { RIL_SS_MODE_DISABLE, RIL_SS_CF_REASON_NO_REPLY, RIL_SS_CLASS_DATA, RIL_TOA_UNKNOWN,
            "9999912345", 5, true },
        { RIL_SS_MODE_ENABLE, RIL_SS_CF_REASON_MOBILE_BUSY, RIL_SS_CLASS_VOICE, RIL_TOA_UNKNOWN,
            "999991234567890111112345698765123", 0, false },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolCallBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetCallForwarding(testVector[i].status,
                testVector[i].reason, testVector[i].srvc_class, testVector[i].toa,
                testVector[i].number, testVector[i].timeseconds);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_CALL_FORWARD);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i].status, jsonReqData[ns_ss_set_call_forward_req::key_status].asInt());
        EXPECT_EQ(testVector[i].reason, jsonReqData[ns_ss_set_call_forward_req::key_reason].asInt());
        EXPECT_EQ(testVector[i].srvc_class, jsonReqData[ns_ss_set_call_forward_req::key_srvc_class].asInt());
        EXPECT_EQ(testVector[i].toa, jsonReqData[ns_ss_set_call_forward_req::key_toa].asInt());

        string numberStr = jsonReqData[ns_ss_set_call_forward_req::key_number].asString();
        EXPECT_TRUE(strlen(numberStr.c_str()) <= MAX_SS_NUM_LEN);
        if (!testVector[i].isLenAndTimeSecsValid) {
            EXPECT_EQ(20, jsonReqData[ns_ss_set_call_forward_req::key_time_secs].asInt());
            EXPECT_EQ(strcmp("99999123456789011111234569876512", numberStr.c_str()), 0);
        } else {
            EXPECT_EQ(testVector[i].timeseconds,
                    jsonReqData[ns_ss_set_call_forward_req::key_time_secs].asInt());
            EXPECT_EQ(strcmp(testVector[i].number, numberStr.c_str()), 0);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildGetCallForwardingStatus) {
    TokenGen::Init();

    struct {
        SsStatusType status;
        int sit_status;
        SsCfReason reason;
        SsClassX srvc_class;
    } testVector[] = {
        { RIL_SS_STATUS_NOT_ACTIVE, SIT_SS_CALL_FARWARD_STATUS_INTERROGATE,
            RIL_SS_CF_REASON_UNCONDITIONAL, RIL_SS_CLASS_VOICE },
        { RIL_SS_STATUS_ACTIVE, SIT_SS_CALL_FARWARD_STATUS_INTERROGATE,
            RIL_SS_CF_REASON_MOBILE_BUSY, RIL_SS_CLASS_DATA },
        { RIL_SS_STATUS_ACTIVE, SIT_SS_CALL_FARWARD_STATUS_INTERROGATE,
            RIL_SS_CF_REASON_NOT_REACHABLE, RIL_SS_CLASS_SMS },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolCallBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildGetCallForwardingStatus(testVector[i].status,
                testVector[i].reason, testVector[i].srvc_class, 0, NULL, 0);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_CALL_FORWARD_STATUS);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i].sit_status, jsonReqData[ns_ss_get_call_forward_status_req::key_status].asInt());
        EXPECT_EQ(testVector[i].reason, jsonReqData[ns_ss_get_call_forward_status_req::key_reason].asInt());
        EXPECT_EQ(testVector[i].srvc_class, jsonReqData[ns_ss_get_call_forward_status_req::key_srvc_class].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallBuilderJson, BuildExitEmergencyCbMode) {
    ProtocolCallBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildExitEmergencyCbMode();
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}
