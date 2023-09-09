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
#include "protocolimsadapter.h"
#include "sitdef.h"
#include "rildef.h"
#include "slsi/ril_oem.h"


TEST(ProtocolImsAdapter, ProtocolAimsIndAdapter) {

    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    int testVector[][2] = {
        { RIL_UNSOL_OEM_AIMS_CALL_RING, SIT_IND_AIMS_CALL_RING },
        { RIL_UNSOL_OEM_AIMS_CALL_STATUS, SIT_IND_AIMS_CALL_STATUS  },
        { RIL_UNSOL_OEM_AIMS_REGISTRATION, SIT_IND_AIMS_REGISTRATION  },
        { RIL_UNSOL_OEM_AIMS_CALL_MODIFY, SIT_IND_AIMS_CALL_MODIFY  },
        { RIL_UNSOL_OEM_AIMS_EXPIRE_INFO, SIT_IND_AIMS_EXPIRE_INFO },
        { RIL_UNSOL_OEM_AIMS_FRAME_TIME, SIT_IND_AIMS_FRAME_TIME },
        { RIL_UNSOL_OEM_AIMS_SUPP_SVC_NOTIFICATION, SIT_IND_AIMS_SUPP_SVC_NOTIFICATION },
        { RIL_UNSOL_OEM_AIMS_NEW_SMS, SIT_IND_AIMS_NEW_SMS },
        { RIL_UNSOL_OEM_AIMS_NEW_SMS_STATUS_REPORT, SIT_IND_AIMS_NEW_SMS_STATUS_REPORT },
        { RIL_UNSOL_OEM_AIMS_ON_USSD, SIT_IND_AIMS_ON_USSD },
        { RIL_UNSOL_OEM_AIMS_CONFERENCE_CALL_EVENT, SIT_IND_AIMS_CONFERENCE_CALL_EVENT },
        { RIL_UNSOL_OEM_AIMS_PAYLOAD_INFO, SIT_IND_AIMS_PAYLOAD_INFO_IND },
        { RIL_UNSOL_OEM_AIMS_VOWIFI_HO_CALL_INFO, SIT_IND_AIMS_VOWIFI_HO_CALL_INFO },
        { RIL_UNSOL_OEM_AIMS_NEW_CDMA_SMS, SIT_IND_AIMS_NEW_CDMA_SMS },
        { RIL_UNSOL_OEM_AIMS_RINGBACK_TONE, SIT_IND_AIMS_RINGBACK_TONE },
        { RIL_UNSOL_OEM_AIMS_CALL_MANAGE, SIT_IND_AIMS_CALL_MANAGE },
        { RIL_UNSOL_OEM_AIMS_CONF_CALL_ADD_REMOVE_USER, SIT_IND_AIMS_CONF_CALL_ADD_REMOVE_USER },
        { RIL_UNSOL_OEM_AIMS_ENHANCED_CONF_CALL, SIT_IND_AIMS_ENHANCED_CONF_CALL },
        { RIL_UNSOL_OEM_AIMS_CALL_MODIFY_RSP, SIT_IND_AIMS_CALL_MODIFY_RSP },
        { RIL_UNSOL_OEM_AIMS_DTMF_EVENT, SIT_IND_AIMS_DTMF_EVENT },
        { RIL_UNSOL_OEM_AIMS_RTT_NEW_TEXT, SIT_IND_AIMS_RTT_NEW_TEXT },
        { RIL_UNSOL_OEM_AIMS_RTT_FAIL_SENDING_TEXT, SIT_IND_AIMS_RTT_FAIL_SENDING_TEXT },
        { RIL_UNSOL_OEM_AIMS_EXIT_EMERGENCY_CB_MODE, SIT_IND_AIMS_EXIT_EMERGENCY_CB_MODE },
        { RIL_UNSOL_OEM_AIMS_DIALOG_INFO, SIT_IND_AIMS_DIALOG_INFO },
        { RIL_UNSOL_OEM_AIMS_RCS_MULTI_FRAME, SIT_IND_AIMS_RCS_MULTI_FRAME},
        { RIL_UNSOL_OEM_AIMS_RCS_CHAT, SIT_IND_AIMS_RCS_CHAT},
        { RIL_UNSOL_OEM_AIMS_RCS_GROUP_CHAT, SIT_IND_AIMS_RCS_GROUP_CHAT},
        { RIL_UNSOL_OEM_AIMS_RCS_OFFLINE_MODE, SIT_IND_AIMS_RCS_OFFLINE_MODE},
        { RIL_UNSOL_OEM_AIMS_RCS_FILE_TRANSFER, SIT_IND_AIMS_RCS_FILE_TRANSFER},
        { RIL_UNSOL_OEM_AIMS_RCS_COMMON_MESSAGE, SIT_IND_AIMS_RCS_COMMON_MESSAGE},
        { RIL_UNSOL_OEM_AIMS_RCS_CONTENT_SHARE, SIT_IND_AIMS_RCS_CONTENT_SHARE},
        { RIL_UNSOL_OEM_AIMS_RCS_PRESENCE, SIT_IND_AIMS_RCS_PRESENCE},
        { RIL_UNSOL_OEM_AIMS_RCS_XCAP_MANAGE, SIT_IND_AIMS_RCS_XCAP_MANAGE},
        { RIL_UNSOL_OEM_AIMS_RCS_CONFIG_MANAGE, SIT_IND_AIMS_RCS_CONFIG_MANAGE},
        { RIL_UNSOL_OEM_AIMS_RCS_TLS_MANAGE, SIT_IND_AIMS_RCS_TLS_MANAGE},
        { RIL_UNSOL_OEM_WFC_RTP_RTCP_TIMEOUT, SIT_IND_WFC_RTP_RTCP_TIMEOUT},
        { RIL_UNSOL_OEM_WFC_FIRST_RTP, SIT_IND_WFC_FIRST_RTP},
        { RIL_UNSOL_OEM_WFC_RTCP_RX_SR, SIT_IND_WFC_RTCP_RX_SR},
        { RIL_UNSOL_OEM_WFC_RCV_DTMF_NOTI, SIT_IND_WFC_RCV_DTMF_NOTI},
        { RIL_UNSOL_OEM_AIMS_MEDIA_STATUS, SIT_IND_AIMS_MEDIA_STATUS},
        { RIL_UNSOL_OEM_AIMS_SIP_MSG_INFO, SIT_IND_AIMS_SIP_MSG_INFO},
        { RIL_UNSOL_OEM_AIMS_VOICE_RTP_QUALITY, SIT_IND_AIMS_VOICE_RTP_QUALITY},
        { RIL_UNSOL_OEM_AIMS_RTP_RX_STATISTICS, SIT_AIMS_IND_RTP_RX_STATISTICS},
        { RIL_UNSOL_OEM_AIMS_AT_CMD_FORWARDING, SIT_IND_AIMS_AT_CMD_FORWARDING},
        { RIL_UNSOL_OEM_AIMS_FEAT_SUPP, SIT_IND_AIMS_FEAT_SUPP},
        { RIL_UNSOL_OEM_AIMS_CALL_TRANSFER_RSP, SIT_IND_AIMS_CALL_TRANSFER_RSP},
        { RIL_UNSOL_OEM_AIMS_CALL_TRANSFER, SIT_IND_AIMS_CALL_TRANSFER},
        { RIL_UNSOL_OEM_AIMS_COMPOSER_MMTEL, SIT_IND_AIMS_COMPOSER_MMTEL},
        { RIL_UNSOL_OEM_AIMS_MWI_INFO, SIT_IND_AIMS_MWI_INFO},
        { RIL_UNSOL_OEM_AIMS_DC_SESS_UPDATE, SIT_IND_AIMS_DC_SESS_UPDATE},
        { RIL_UNSOL_OEM_AIMS_DC_SESSION_MANAGE, SIT_IND_AIMS_DC_SESSION_MANAGE},
        { RIL_UNSOL_OEM_AIMS_DC_SESS_MODIFY, SIT_IND_AIMS_DC_SESS_MODIFY},
        // { -1, -1},  // Impossible TC, UINT type can not care negative invalid case.
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ModemData *modemData = ModemDataBuilder::BuildIndication(testVector[i][1]);
        ASSERT_NE(modemData, nullptr);

        ProtocolAimsIndAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetResultId(), testVector[i][0]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolImsAdapter, ProtocolImsRegIndAdapter) {
    struct {
        char data[3];
        int state;
    } testVector[] = {
            { .data = { SITRIL_AIMS_IMSREG_STATE_REGISTERED, 0, 0 }, .state = RIL_IMS_REGISTERED },
            { .data = { SITRIL_AIMS_IMSREG_STATE_NOT_REGISTERED, 0, 0 }, .state = RIL_IMS_NOT_REGISTERED },
            { .data = { SITRIL_AIMS_IMSREG_STATE_LIMITED_REGISTERED, 0, 0 }, .state = -1 },
            { .data = { SITRIL_AIMS_IMSREG_STATE_NOT_REGISTERED_E911, 0, 0 }, .state = -1 },
            { .data = { SITRIL_AIMS_IMSREG_STATE_REGISTERED_E911, 0, 0 }, .state = -1 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        ModemData *modemData = ModemDataBuilder::BuildIndication(
                SIT_IND_AIMS_REGISTRATION, testVector[i].data, sizeof(testVector[i].data));
        ASSERT_NE(modemData, nullptr);

        ProtocolImsRegIndAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_AIMS_REGISTRATION);
        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].state);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolImsAdapter, ProtocolImsTriggerImsDeregistrationIndAdapter) {
    int32_t data[4] = { 1, 2, 3 };
    for (auto in : data) {
        auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildIndication(
                    SIT_IND_IMS_DEREGISTERTAION, &in, sizeof(in)));
        ASSERT_NE(modemData, nullptr);

        ProtocolImsTriggerImsDeregistrationIndAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_IMS_DEREGISTERTAION);
        EXPECT_EQ(testAdapter.GetReason(), in);
    }
}

TEST(ProtocolImsAdapter, ProtocolImsStartImsTrafficRespAdapter) {
    int testVector[][3] = {
        {0, 1, 0}, {2, 3, 4}, {-1, 0, 1},
    };

    for (auto testElement : testVector) {
        ModemDataBuilder builder;
        auto modemData = std::unique_ptr<ModemData>(builder.BuildResponse(
                SIT_IMS_START_TRAFFIC, 0, RCM_E_SUCCESS, testElement, sizeof(int)*3));
        ASSERT_NE(modemData, nullptr);

        ProtocolImsStartImsTrafficRespAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_IMS_START_TRAFFIC);
        EXPECT_EQ(testAdapter.GetConnectionFailureReason(), testElement[0]);
        EXPECT_EQ(testAdapter.GetCauseCode(), testElement[1]);
        EXPECT_EQ(testAdapter.GetWaitTimeMillis(), testElement[2]);
    }
}

TEST(ProtocolImsAdapter, ProtocolImsConnectionSetupFailureIndAdapter) {
    int testVector[][4] = {
        {0, 0, 0, 0}, {0, 1, 2, 3}, {1, 1, 1, 1}, {3, 2, 1, 0}
    };

    for (auto testElement : testVector) {
        ModemDataBuilder builder;
        auto modemData = std::unique_ptr<ModemData>(builder.BuildIndication(
                SIT_IND_IMS_RRC_CONNECTION_FAILURE, testElement, sizeof(int)*4));
        ASSERT_NE(modemData, nullptr);

        ProtocolImsConnectionSetupFailureIndAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_IMS_RRC_CONNECTION_FAILURE);
        EXPECT_EQ(testAdapter.GetFailureToken(), testElement[0]);
        EXPECT_EQ(testAdapter.GetConnectionFailureReason(), testElement[1]);
        EXPECT_EQ(testAdapter.GetCauseCode(), testElement[2]);
        EXPECT_EQ(testAdapter.GetWaitTimeMillis(), testElement[3]);
    }
}

TEST(ProtocolImsAdapter, ProtocolImsTriggerEpsFallbackIndAdapter) {
    uint8_t testVector[][2] = {
        {0, 0}, {0, 1}, {1, 0}, {1, 1}
    };

    for (auto testElement : testVector) {
        ModemDataBuilder builder;
        auto modemData = std::unique_ptr<ModemData>(builder.BuildIndication(
                SIT_SET_TRIGGER_EPSFB, testElement, sizeof(uint8_t)*2));
        ASSERT_NE(modemData, nullptr);

        ProtocolImsTriggerEpsFallbackIndAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_TRIGGER_EPSFB);
        EXPECT_EQ(testAdapter.GetResult(), static_cast<int32_t>(testElement[0]));
        EXPECT_EQ(testAdapter.GetRat(), static_cast<int32_t>(testElement[1]));
    }
}
