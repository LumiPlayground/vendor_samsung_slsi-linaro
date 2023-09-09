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
#include "protocolimsbuilder.h"
#include "sitdef.h"
#include "rildef.h"
#include "slsi/ril_oem.h"
#include "tokengen.h"
#include "testutils.h"

#include <stdlib.h>

#define MAX_TEST_PAYLOAD 512
#define MAX_IMS_RCM_SIZE (64 * 1024)

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildAimsPDU) {
    TokenGen::Init();

    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    unsigned int testVector[][2] = {
        {RIL_REQUEST_OEM_AIMS_DIAL, SIT_AIMS_DIAL},
        {RIL_REQUEST_OEM_AIMS_ANSWER, SIT_AIMS_ANSWER},
        {RIL_REQUEST_OEM_AIMS_HANGUP, SIT_AIMS_HANGUP},
        {RIL_REQUEST_OEM_AIMS_DEREGISTRATION, SIT_AIMS_DEREGISTRATION},
        {RIL_REQUEST_OEM_AIMS_HIDDEN_MENU, SIT_AIMS_HIDDEN_MENU},
        {RIL_REQUEST_OEM_AIMS_ADD_PDN_INFO, SIT_AIMS_ADD_PDN_INFO},
        {RIL_REQUEST_OEM_AIMS_CALL_MANAGE, SIT_AIMS_CALL_MANAGE},
        {RIL_REQUEST_OEM_AIMS_SEND_DTMF, SIT_AIMS_SEND_DTMF},
        {RIL_REQUEST_OEM_AIMS_SET_FRAME_TIME, SIT_AIMS_SET_FRAME_TIME},
        {RIL_REQUEST_OEM_AIMS_GET_FRAME_TIME, SIT_AIMS_GET_FRAME_TIME},
        {RIL_REQUEST_OEM_AIMS_CALL_MODIFY, SIT_AIMS_CALL_MODIFY},
        {RIL_REQUEST_OEM_AIMS_RESPONSE_CALL_MODIFY, SIT_AIMS_RESPONSE_CALL_MODIFY},
        {RIL_REQUEST_OEM_AIMS_CONF_CALL_ADD_REMOVE_USER, SIT_AIMS_CONF_CALL_ADD_REMOVE_USER},
        {RIL_REQUEST_OEM_AIMS_ENHANCED_CONF_CALL, SIT_AIMS_ENHANCED_CONF_CALL},
        {RIL_REQUEST_OEM_AIMS_GET_CALL_FORWARD_STATUS, SIT_AIMS_GET_CALL_FORWARD_STATUS},
        {RIL_REQUEST_OEM_AIMS_SET_CALL_FORWARD_STATUS, SIT_AIMS_SET_CALL_FORWARD_STATUS},
        {RIL_REQUEST_OEM_AIMS_GET_CALL_WAITING, SIT_AIMS_GET_CALL_WAITING},
        {RIL_REQUEST_OEM_AIMS_SET_CALL_WAITING, SIT_AIMS_SET_CALL_WAITING},
        {RIL_REQUEST_OEM_AIMS_GET_CALL_BARRING, SIT_AIMS_GET_CALL_BARRING},
        {RIL_REQUEST_OEM_AIMS_SET_CALL_BARRING, SIT_AIMS_SET_CALL_BARRING},
        {RIL_REQUEST_OEM_AIMS_SEND_SMS, SIT_AIMS_SEND_SMS},
        {RIL_REQUEST_OEM_AIMS_SEND_EXPECT_MORE, SIT_AIMS_SEND_EXPECT_MORE},
        {RIL_REQUEST_OEM_AIMS_SEND_SMS_ACK, SIT_AIMS_SEND_SMS_ACK},
        {RIL_REQUEST_OEM_AIMS_SEND_ACK_INCOMING_SMS, SIT_AIMS_SEND_ACK_INCOMING_SMS},
        {RIL_REQUEST_OEM_AIMS_CHG_BARRING_PWD, SIT_AIMS_CHG_BARRING_PWD},
        {RIL_REQUEST_OEM_AIMS_SEND_USSD_INFO, SIT_AIMS_SEND_USSD_INFO},
        {RIL_REQUEST_OEM_AIMS_GET_PRESENTATION_SETTINGS, SIT_AIMS_GET_PRESENTATION_SETTINGS},
        {RIL_REQUEST_OEM_AIMS_SET_PRESENTATION_SETTINGS, SIT_AIMS_SET_PRESENTATION_SETTINGS},
        {RIL_REQUEST_OEM_AIMS_SET_SELF_CAPABILITY, SIT_AIMS_SET_SELF_CAPABILITY},
        {RIL_REQUEST_OEM_AIMS_HO_TO_WIFI_READY, SIT_AIMS_HO_TO_WIFI_READY_REQ},
        {RIL_REQUEST_OEM_AIMS_HO_TO_3GPP, SIT_AIMS_HO_TO_3GPP_REQ},
        {RIL_REQUEST_OEM_AIMS_SEND_ACK_INCOMING_CDMA_SMS, SIT_AIMS_SEND_ACK_INCOMING_CDMA_SMS},
        {RIL_REQUEST_OEM_AIMS_DEL_PDN_INFO, SIT_AIMS_DEL_PDN_INFO},
        {RIL_REQUEST_OEM_AIMS_STACK_START_REQ, SIT_AIMS_STACK_START_REQ},
        {RIL_REQUEST_OEM_AIMS_STACK_STOP_REQ, SIT_AIMS_STACK_STOP_REQ},
        {RIL_REQUEST_OEM_AIMS_XCAPM_START_REQ, SIT_AIMS_XCAPM_START_REQ},
        {RIL_REQUEST_OEM_AIMS_XCAPM_STOP_REQ, SIT_AIMS_XCAPM_STOP_REQ},
        {RIL_REQUEST_OEM_AIMS_RTT_SEND_TEXT, SIT_AIMS_RTT_SEND_TEXT},
        {RIL_REQUEST_OEM_AIMS_EXIT_EMERGENCY_CB_MODE, SIT_AIMS_EXIT_EMERGENCY_CB_MODE},
        {RIL_REQUEST_OEM_AIMS_SET_GEO_LOCATION_INFO, SIT_AIMS_SET_GEO_LOCATION_INFO},
        {RIL_REQUEST_OEM_AIMS_CDMA_SEND_SMS, SIT_AIMS_CDMA_SEND_SMS},
        {RIL_REQUEST_OEM_AIMS_RCS_MULTI_FRAME, SIT_AIMS_RCS_MULTI_FRAME},
        {RIL_REQUEST_OEM_AIMS_RCS_CHAT, SIT_AIMS_RCS_CHAT},
        {RIL_REQUEST_OEM_AIMS_RCS_GROUP_CHAT, SIT_AIMS_RCS_GROUP_CHAT},
        {RIL_REQUEST_OEM_AIMS_RCS_OFFLINE_MODE, SIT_AIMS_RCS_OFFLINE_MODE},
        {RIL_REQUEST_OEM_AIMS_RCS_FILE_TRANSFER, SIT_AIMS_RCS_FILE_TRANSFER},
        {RIL_REQUEST_OEM_AIMS_RCS_COMMON_MESSAGE, SIT_AIMS_RCS_COMMON_MESSAGE},
        {RIL_REQUEST_OEM_AIMS_RCS_CONTENT_SHARE, SIT_AIMS_RCS_CONTENT_SHARE},
        {RIL_REQUEST_OEM_AIMS_RCS_PRESENCE, SIT_AIMS_RCS_PRESENCE},
        {RIL_REQUEST_OEM_AIMS_XCAP_MANAGE, SIT_AIMS_RCS_XCAP_MANAGE},
        {RIL_REQUEST_OEM_AIMS_RCS_CONFIG_MANAGE, SIT_AIMS_RCS_CONFIG_MANAGE},
        {RIL_REQUEST_OEM_AIMS_RCS_TLS_MANAGE, SIT_AIMS_RCS_TLS_MANAGE},
        {RIL_REQUEST_OEM_AIMS_SET_PDN_EST_STATUS, SIT_AIMS_SET_PDN_EST_STATUS},
        {RIL_REQUEST_OEM_AIMS_SET_HIDDEN_MENU_ITEM, SIT_AIMS_SET_HIDDEN_MENU_ITEM},
        {RIL_REQUEST_OEM_AIMS_GET_HIDDEN_MENU_ITEM, SIT_AIMS_GET_HIDDEN_MENU_ITEM},
        {RIL_REQUEST_OEM_AIMS_SET_RTP_RX_STATISTICS, SIT_AIMS_SET_RTP_RX_STATISTICS},
        {RIL_REQUEST_OEM_AIMS_AT_CMD_FORWARDING, SIT_REQ_AIMS_AT_CMD_FORWARDING},
        {RIL_REQUEST_OEM_AIMS_CALL_TRANSFER, SIT_REQ_AIMS_CALL_TRANSFER},
        {RIL_REQUEST_OEM_WFC_MEDIA_CHANNEL_CONFIG, SIT_SET_WFC_MEDIA_CONFIGURATION},
        {RIL_REQUEST_OEM_WFC_DTMF_START, SIT_WFC_DTMF_START},
        {RIL_REQUEST_OEM_WFC_SET_VOWIFI_HO_THRESHOLD, SIT_SET_VOWIFI_HO_THRESHOLD},
        {RIL_REQUEST_OEM_AIMS_CALL_DEFLECTION_REQ, SIT_AIMS_CALL_DEFLECTION_REQ},
        {RIL_REQUEST_OEM_AIMS_COMPOSER_MMTEL, SIT_AIMS_COMPOSER_MMTEL},
        {RIL_REQUEST_OEM_AIMS_CANCEL_COMPOSER_MMTEL, SIT_AIMS_CANCEL_COMPOSER_MMTEL},
        {RIL_REQUEST_OEM_AIMS_COMMON_IMS_SVC_INFO, SIT_REQ_AIMS_COMMON_IMS_SVC_INFO},
        {RIL_REQUEST_OEM_AIMS_SET_RTP_HEADER_EXTENSION_TYPE, SIT_AIMS_SET_RTP_HEADER_EXTENSION_TYPE},
        {RIL_REQUEST_OEM_AIMS_SEND_RTP_HEADER_EXTENSION, SIT_AIMS_SEND_RTP_HEADER_EXTENSION},
        {RIL_REQUEST_OEM_AIMS_DC_SESSION_UPDATE, SIT_REQ_AIMS_DC_SESS_UPDATE},
        {RIL_REQUEST_OEM_AIMS_DC_SESSION_MANAGE, SIT_REQ_AIMS_DC_SESS_MANAGE},
        {RIL_REQUEST_OEM_AIMS_EMERGENCY_VOWIFI_CAPABILITY, SIT_AIMS_EMERGENCY_VOWIFI_CAPABILITY},
    };

    // not to initialize a local buffer to use random packets
    char payload[MAX_TEST_PAYLOAD];

    ProtocolImsBuilder testBuilder;
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        size_t len = INTrand(false) % (MAX_TEST_PAYLOAD + 1);
        ModemData *modemData = testBuilder.BuildAimsPDU(testVector[i][0], payload, len);
        ASSERT_TRUE(modemData != nullptr);
        EXPECT_TRUE(modemData != nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testVector[i][1]);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + len);
        EXPECT_EQ(testAdapter.GetParameterLength(), len);
        if (len > 0) {
            EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
            EXPECT_TRUE(memcmp(testAdapter.GetParameter(), payload, len) == 0);
        } else {
            EXPECT_TRUE(testAdapter.GetParameter() == nullptr);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildAimsPDU_invalid) {
    TokenGen::Init();

    // wrong ID
    int testVector[] = {
        -1,
        RIL_REQUEST_SETUP_DATA_CALL,
        RIL_REQUEST_OEM_SET_ENG_MODE,
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolImsBuilder testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildAimsPDU(testVector[i], nullptr, 0);
        EXPECT_TRUE(modemData == nullptr);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildAimsPDU_invalid_payload) {
    TokenGen::Init();

    // not to initialize a local buffer to use random packets
    char payload[MAX_TEST_PAYLOAD];

    ProtocolImsBuilder testBuilder;
    ModemData *modemData = testBuilder.BuildAimsPDU(RIL_REQUEST_OEM_AIMS_DIAL, payload, MAX_IMS_RCM_SIZE + 1);
    EXPECT_TRUE(modemData == nullptr);
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildAimsIndPDU) {
    TokenGen::Init();

    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    static const unsigned int testVector[][2] = {
        {RIL_REQUEST_OEM_AIMS_HO_TO_WIFI_CANCEL_IND, SIT_AIMS_HO_TO_WIFI_CANCEL_IND},
        {RIL_REQUEST_OEM_AIMS_HO_PAYLOAD_IND, SIT_IND_AIMS_PAYLOAD_INFO_IND},
        {RIL_REQUEST_OEM_AIMS_MEDIA_STATE_IND, SIT_AIMS_MEDIA_STATE_IND},
        {RIL_REQUEST_OEM_AIMS_SEND_UL_SIP_MSG_IND, SIT_AIMS_SEND_UL_SIP_MSG_IND},
        {RIL_REQUEST_OEM_AIMS_DL_SIP_MSG_RESULT_IND, SIT_AIMS_DL_SIP_MSG_RESULT_IND},
        {RIL_REQUEST_OEM_AIMS_REGISTER_IND, SIT_AIMS_REGISTER_IND},
        {RIL_REQUEST_OEM_AIMS_DC_SESSION_MODIFY_IND, SIT_AIMS_DC_SESS_MODIFY_IND},
    };

    // not to initialize a local buffer to use random packets
    char payload[MAX_TEST_PAYLOAD];

    ProtocolImsBuilder testBuilder;
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        size_t len = INTrand(false) % (MAX_TEST_PAYLOAD + 1);
        ModemData *modemData = testBuilder.BuildAimsIndPDU(testVector[i][0], payload, len);
        ASSERT_TRUE(modemData != nullptr);
        EXPECT_TRUE(modemData != nullptr);

        ProtocolIndAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), testVector[i][1]);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_IND_HEADER) + len);
        EXPECT_EQ(testAdapter.GetParameterLength(), len);
        if (len > 0) {
            EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
            EXPECT_TRUE(memcmp(testAdapter.GetParameter(), payload, len) == 0);
        } else {
            EXPECT_TRUE(testAdapter.GetParameter() == nullptr);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildAimsIndPDU_invalid) {
    TokenGen::Init();

    // wrong ID
    int testVector[] = {
        -1,
        RIL_UNSOL_NETWORK_SCAN_RESULT,
        RIL_UNSOL_OEM_DISPLAY_ENG_MODE,
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolImsBuilder testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildAimsIndPDU(testVector[i], nullptr, 0);
        EXPECT_TRUE(modemData == nullptr);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildAimsIndPDU_invalid_payload) {
    TokenGen::Init();

    // not to initialize a local buffer to use random packets
    char payload[MAX_TEST_PAYLOAD];

    ProtocolImsBuilder testBuilder;
    ModemData *modemData = testBuilder.BuildAimsIndPDU(RIL_REQUEST_OEM_AIMS_HO_PAYLOAD_IND, payload, MAX_IMS_RCM_SIZE + 1);
    EXPECT_TRUE(modemData == nullptr);
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildEmergencyCallStatus) {
    TokenGen::Init();

    ProtocolImsBuilder testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildEmergencyCallStatus(0, 1));
    ASSERT_TRUE(modemData != nullptr);
    EXPECT_TRUE(modemData != nullptr);
    ProtocolReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_EMERGENCY_CALL_STATUS);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_emergency_call_status_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 2U);
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_EQ(((char *)testAdapter.GetParameter())[0], 0);
    EXPECT_EQ(((char *)testAdapter.GetParameter())[1], 1);
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildImsRegistrationInfo) {
    const int testVector[][4] = {
        {0, 0, 0, 0}, {0, 1, 2, 3}, {1, 1, 1, 1}, {2, 2, 2, 2}
    };

    for (auto testElement : testVector) {
        ProtocolImsBuilder builder;
        auto modemData = std::unique_ptr<ModemData>(builder.BuildImsRegistrationInfo(
                    testElement[0], testElement[1], testElement[2], testElement[3]));
        ASSERT_NE(modemData, nullptr);
        ProtocolReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_IMS_SET_REGISTRATION_STATUS);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_ims_set_registration_status_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(sit_ims_set_registration_status_req) - sizeof(RCM_HEADER));
    }
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildImsStartImsTraffic) {
    const int testVector[][4] = {
        {0, 1, 2, 0}, {1, 2, 3, 1}, {0, 0, 0, 0}, {1, 1, 1, 1}
    };

    for (auto testElement : testVector) {
        ProtocolImsBuilder builder;
        auto modemData = std::unique_ptr<ModemData>(builder.BuildImsStartImsTraffic(
                    testElement[0], testElement[1], testElement[2], testElement[3]));
        ASSERT_NE(modemData, nullptr);
        ProtocolReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_IMS_START_TRAFFIC);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_ims_start_ims_traffic_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(sit_ims_start_ims_traffic_req) - sizeof(RCM_HEADER));
    }
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildImsStopImsTraffic) {
    const int testVector[] = { 0, 1, 2, 3 };

    for (auto testElement : testVector) {
        ProtocolImsBuilder builder;
        auto modemData = std::unique_ptr<ModemData>(builder.BuildImsStopImsTraffic(testElement));
        ASSERT_NE(modemData, nullptr);
        ProtocolReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_IMS_STOP_TRAFFIC);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_ims_stop_ims_traffic_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(sit_ims_stop_ims_traffic_req) - sizeof(RCM_HEADER));
    }
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildImsTriggerFallback) {
    const int testVector[] = { 1, 2 };

    for (auto testElement : testVector) {
        ProtocolImsBuilder builder;
        auto modemData = std::unique_ptr<ModemData>(builder.BuildImsTriggerFallback(testElement));
        ASSERT_NE(modemData, nullptr);
        ProtocolReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_TRIGGER_EPSFB);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_set_trigger_epsfb_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(sit_set_trigger_epsfb_req) - sizeof(RCM_HEADER));
        EXPECT_EQ((*(reinterpret_cast<const unsigned char *>(testAdapter.GetParameter()))), static_cast<unsigned char>(testElement));
    }
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildImsSetSrvccCallInfo) {
    std::vector<RilImsV2_1::RIL_SrvccCall> testVector = {
        {0, RilImsV2_1::RIL_CallType::NORMAL, 0, RilImsV2_1::RIL_CallSubState::NONE,
            RilImsV2_1::RIL_ToneType::LOCAL, true, false, "01012341234", 0, "AAA", 0},
        {1, RilImsV2_1::RIL_CallType::EMERGENCY, 1, RilImsV2_1::RIL_CallSubState::PREALERTING,
            RilImsV2_1::RIL_ToneType::NETWORK, false, true, "01043214321", 1, "BBB", 1},
        {2, RilImsV2_1::RIL_CallType::NORMAL, 2, RilImsV2_1::RIL_CallSubState::NONE,
            RilImsV2_1::RIL_ToneType::NONE, true, true, "01011112222", 2, "CCC", 2},
        {3, RilImsV2_1::RIL_CallType::EMERGENCY, 3, RilImsV2_1::RIL_CallSubState::PREALERTING,
            RilImsV2_1::RIL_ToneType::NONE, false, false, "01033334444", 3, "DDD", 3},
    };

    ProtocolImsBuilder builder;
    auto modemData = std::unique_ptr<ModemData>(builder.BuildImsSetSrvccCallInfo(testVector));
    ASSERT_NE(modemData, nullptr);
    ProtocolReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_SRVCC_CALL_LIST);
    EXPECT_EQ(testAdapter.GetLength(), (sizeof(RCM_HEADER) + sizeof(sit_ims_set_srvcc_call_list_req::callListNum) +
        (sizeof(current_call_list) * testVector.size())));
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(sit_ims_set_srvcc_call_list_req::callListNum) +
        (sizeof(current_call_list) * testVector.size()));

    const sit_ims_set_srvcc_call_list_req result =
        *(reinterpret_cast<const sit_ims_set_srvcc_call_list_req *>(modemData->GetRawData()));
    EXPECT_EQ(static_cast<int32_t>(testVector.size()), result.callListNum);

    for (int i = 0; i < result.callListNum; i++) {
        const current_call_list callList =
            *(reinterpret_cast<const current_call_list *>(modemData->GetRawData() +
            sizeof(RCM_HEADER) + sizeof(sit_ims_set_srvcc_call_list_req::callListNum) +
            (sizeof(current_call_list) * i)));

        EXPECT_EQ(callList.index, testVector[i].index);
        EXPECT_EQ(callList.callType, static_cast<uint8_t>(testVector[i].callType));
        EXPECT_EQ(callList.state, static_cast<uint8_t>(testVector[i].callState));
        EXPECT_EQ(callList.subState, static_cast<uint8_t>(testVector[i].callSubstate));
        EXPECT_EQ(callList.ringBackTone, static_cast<uint8_t>(testVector[i].ringbackToneType));
        EXPECT_EQ(callList.isMpty, testVector[i].isMpty);
        EXPECT_EQ(callList.isMt, testVector[i].isMT);
        EXPECT_EQ(callList.numberLen, static_cast<uint8_t>(testVector[i].number.length()));
        EXPECT_STREQ(callList.number, testVector[i].number.c_str());
        EXPECT_EQ(callList.numberPresentation, testVector[i].numPresentation);
        EXPECT_EQ(callList.nameLen, static_cast<uint8_t>(testVector[i].name.length()));
        EXPECT_STREQ(callList.name, testVector[i].name.c_str());
        EXPECT_EQ(callList.namePresentation, testVector[i].namePresentation);
    }
}

TEST(ProtocolImsBuilder, ProtocolImsBuilder_BuildImsMtCallGuardtmrStatus) {
    const uint8_t testVector[] = { 0, 1, 2, 3 };

    for (auto testElement : testVector) {
        ProtocolImsBuilder builder;
        auto modemData = std::unique_ptr<ModemData>(builder.BuildImsMtCallGuardtmrStatus(testElement));
        ASSERT_NE(modemData, nullptr);
        ProtocolReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_MT_CALL_GUARDTMR_STATUS);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_ims_mt_call_guardtmr_status_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(sit_ims_mt_call_guardtmr_status_req) - sizeof(RCM_HEADER));

        const sit_ims_mt_call_guardtmr_status_req result =
        *(reinterpret_cast<const sit_ims_mt_call_guardtmr_status_req *>(modemData->GetRawData()));
        EXPECT_EQ(result.status, testElement);
    }
}

