/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * rilparser_test.cpp
 *
 *  Created on: 2021. 8. 4.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include <testutils.h>
#include "rilparser.h"
#include <librilutils/textutils.h>
#include "callreqdata.h"
#include "datacallreqdata.h"
#include "miscdata.h"
#include "simdata.h"
#include "vsimdata.h"

TEST(RilParser, CreateInt) {
    int testVector[] = { 10 };
    int requestId = RIL_REQUEST_SET_SIM_CARD_POWER;
    RequestData *request = RilParser::CreateInt(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateInt(requestId, 0,
            (char *)testVector, sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateInts) {
    int testVector[] = { 10, 20 };
    int requestId = RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING;
    RequestData *request = RilParser::CreateInts(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateInts(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateString) {
    char testVector[] = "test string";
    int requestId = RIL_REQUEST_GET_IMSI;
    RequestData *request = RilParser::CreateString(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateStrings) {
    const char *testVector[] = { "test string1", "test string2" };
    int requestId = RIL_REQUEST_ENTER_SIM_PIN;
    RequestData *request = RilParser::CreateStrings(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateRawData) {
    char testVector[] = "test string";
    int requestId = RIL_REQUEST_OEM_SET_ENG_MODE;
    RequestData *request = RilParser::CreateRawData(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateHookRaw) {
    char testVector[] = { 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xA0, 0xB0 };
    int requestId = RIL_REQUEST_OEM_HOOK_RAW;
    RequestData *request = RilParser::CreateHookRaw(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateHookRaw(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateCallDial) {
    RIL_Dial testVector = {};
    int requestId = RIL_REQUEST_DIAL_WITH_CALL_TYPE;
    RequestData *request = RilParser::CreateCallDial(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateCallDial(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateCallEmergencyDial) {
    RIL_EmergencyDial testVector = {};
    int requestId = RIL_REQUEST_EMERGENCY_DIAL;
    RequestData *request = RilParser::CreateCallEmergencyDial(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateCallEmergencyDial(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateCallForward) {
    RIL_CallForwardInfo testVector = {};
    int requestId = RIL_REQUEST_QUERY_CALL_FORWARD_STATUS;
    RequestData *request = RilParser::CreateCallForward(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateGsmSms) {
    const char *testVector[2] = {};
    int requestId = RIL_REQUEST_SEND_SMS;
    RequestData *request = RilParser::CreateGsmSms(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateGsmSms(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSmsAck) {
    int testVector[2] = {};
    int requestId = RIL_REQUEST_SMS_ACKNOWLEDGE;
    RequestData *request = RilParser::CreateSmsAck(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSmsAck(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSmsAckPdu) {
    const char *testVector[2] = {};
    int requestId = RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU;
    RequestData *request = RilParser::CreateSmsAckPdu(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSmsAckPdu(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSetSmsConfig) {
    RIL_GSM_BroadcastSmsConfigInfo testConfig = {};
    RIL_GSM_BroadcastSmsConfigInfo *testVector[] = { &testConfig };
    int requestId = RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG;
    RequestData *request = RilParser::CreateSetSmsConfig(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSetSmsConfig(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSendDtmf) {
    RequestData *request = RilParser::CreateSendDtmf(RIL_REQUEST_DTMF, 0, 0, 0);
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateStartDtmf) {
    RequestData *request = RilParser::CreateStartDtmf(RIL_REQUEST_DTMF_START, 0, 0, 0);
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateStopDtmf) {
    RequestData *request = RilParser::CreateStopDtmf(RIL_REQUEST_DTMF_STOP, 0, 0, 0);
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSimIoData) {
    RIL_SIM_IO_v6 testVector = {};
    int requestId = RIL_REQUEST_SIM_IO;
    RequestData *request = RilParser::CreateSimIoData(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSimIoData(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSimUiccSubscription) {
    RIL_SelectUiccSub testVector = {};
    int requestId = RIL_REQUEST_SET_UICC_SUBSCRIPTION;
    RequestData *request = RilParser::CreateSimUiccSubscription(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSimUiccSubscription(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSimAuthentication) {
    RIL_SimAuthentication testVector = {};
    int requestId = RIL_REQUEST_SIM_AUTHENTICATION;
    RequestData *request = RilParser::CreateSimAuthentication(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSimAuthentication(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSimSmsData) {
    RIL_SMS_WriteArgs testVector = {};
    int requestId = RIL_REQUEST_WRITE_SMS_TO_SIM;
    RequestData *request = RilParser::CreateSimSmsData(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSimSmsData(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSetupDataCall) {
    RIL_SetupDataCallInfo_V1_5 testVector = {};
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 5));
    RequestData *request = RilParser::CreateSetupDataCall(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSetupDataCall(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateDeactivateDataCall) {
    const char *testVector[] = { "1", "0" };
    int requestId = RIL_REQUEST_DEACTIVATE_DATA_CALL_WITH_REASON;
    RequestData *request = RilParser::CreateDeactivateDataCall(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateDeactivateDataCall(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSetInitialAttachApn) {
    RIL_DataProfileInfo_V1_5 testVector = {};
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(1, 5));
    RequestData *request = RilParser::CreateSetInitialAttachApn(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSetInitialAttachApn(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSetDataProfile) {
    RIL_DataProfileInfo_V1_5 testDataProfile = {};
    RIL_DataProfileInfo_V1_5 *testVector[] = { &testDataProfile };
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(1, 5));
    RequestData *request = RilParser::CreateSetDataProfile(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSetDataProfile(requestId, 0, (char *)testVector, sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSimAPDU) {
    RIL_SIM_APDU testVector = {};
    int requestId = RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL;
    RequestData *request = RilParser::CreateSimAPDU(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSimAPDU(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSimOpenChannel) {
    RIL_OpenChannelParams testVector = {};
    int requestId = RIL_REQUEST_SIM_OPEN_CHANNEL;
    RequestData *request = RilParser::CreateSimOpenChannel(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSimOpenChannel(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateUpdatePbEntry) {
    RIL_UpdatePbEntry testVector = {};
    int requestId = RIL_REQUEST_UPDATE_SIM_PHONEBOOK_RECORDS;
    RequestData *request = RilParser::CreateUpdatePbEntry(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateUpdatePbEntry(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateNetRCData) {
    RIL_RadioCapability testVector = {};
    int requestId = RIL_REQUEST_SET_RADIO_CAPABILITY;
    RequestData *request = RilParser::CreateNetRCData(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateNetRCData(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateNvReadItem) {
    RIL_NV_ReadItem testVector = {};
    int requestId = RIL_REQUEST_NV_READ_ITEM;
    RequestData *request = RilParser::CreateNvReadItem(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateNvReadItem(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateNvWriteItem) {
    RIL_NV_WriteItem testVector = {};
    int requestId = RIL_REQUEST_NV_WRITE_ITEM;
    RequestData *request = RilParser::CreateNvWriteItem(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateNvWriteItem(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateVsimOperation) {
    RIL_VsimOperationEvent testVector = {};
    int requestId = RIL_LOCAL_REQUEST_VSIM_OPERATION;
    RequestData *request = RilParser::CreateVsimOperation(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateVsimOperation(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateVsimOperationExt) {
    int testVector[4] = {};
    int requestId = RIL_REQUEST_OEM_VSIM_OPERATION;
    RequestData *request = RilParser::CreateVsimOperationExt(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateVsimOperationExt(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateApnSettings) {
    char testVector[] = "AT+VZWAPNE?";
    int requestId = RIL_REQUEST_OEM_APN_SETTINGS;
    RequestData *request = RilParser::CreateApnSettings(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateApnSettings(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateCarrierRestrictions) {
    RIL_CarrierRestrictions_V1_4 testVector = {};
    int requestId = RIL_REQUEST_SET_CARRIER_RESTRICTIONS;
    RequestData *request = RilParser::CreateCarrierRestrictions(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateCarrierRestrictions(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateCarrierInfoForImsiEncryption) {
    RIL_CarrierInfoForImsiEncryption_1_6 testVector = {};
    int requestId = RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION;
    RequestData *request = RilParser::CreateCarrierInfoForImsiEncryption(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateCarrierInfoForImsiEncryption(requestId, 0,
            (char *)&testVector, sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateRequestKeepalive) {
    RIL_KeepaliveRequest testVector = {};
    int requestId = RIL_REQUEST_START_KEEPALIVE;
    RequestData *request = RilParser::CreateRequestKeepalive(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateRequestKeepalive(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateNetworkScanRequest) {
    RIL_NetworkScanRequest_V1_5 testVector = {};
    int requestId = ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1, 5));
    RequestData *request = RilParser::CreateNetworkScanRequest(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateNetworkScanRequest(requestId, 0, (char *)&testVector, sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSystemSelectionChannels) {
    RIL_SysSelChannelRequest_V1_5 testVector = {};
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, HAL_VERSION_CODE(1, 5));
    RequestData *request = RilParser::CreateSystemSelectionChannels(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSystemSelectionChannels(requestId, 0, (char *)&testVector, sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSetActivateVsim) {
    RIL_SetActivateVsim testVector = {};
    RequestData *request = RilParser::CreateSetActivateVsim(RIL_REQUEST_DATA_REGISTRATION_STATE, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSetActivateVsim(RIL_REQUEST_DATA_REGISTRATION_STATE, 0,
            (char *)&testVector, sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSignalStrengthReportingCriteria) {
    RIL_SignalStrengthReportingCriteria_V1_5 testVector = {};
    int requestId = RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA;
    RequestData *request = RilParser::CreateSignalStrengthReportingCriteria(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSignalStrengthReportingCriteria(requestId, 0,
            (char *)&testVector, sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateLinkCapacityReportingCriteria) {
    RIL_LinkCapacityReportingCriteria_V1_5 testVector = {};
    int requestId = RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA;
    RequestData *request = RilParser::CreateLinkCapacityReportingCriteria(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateLinkCapacityReportingCriteria(requestId, 0,
            (char *)&testVector, sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateEmbmsSessionData) {
    RIL_EmbmsSessionData testVector = {};
    int requestId = RIL_REQUEST_OEM_EMBMS_SET_SESSION;
    RequestData *request = RilParser::CreateEmbmsSessionData(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateEmbmsSessionData(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateSetUrspData) {
    UrspData testVector = {};
    int requestId = RIL_REQUEST_OEM_SET_URSP;
    RequestData *request = RilParser::CreateSetUrspData(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateSetUrspData(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateDataThrottling) {
    RIL_DataThrottling testVector = {};
    int requestId = RIL_REQUEST_SET_DATA_THROTTLING;
    RequestData *request = RilParser::CreateDataThrottling(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateDataThrottling(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateCdmaSms) {
    RIL_CDMA_SMS_Message testVector = {};
    int requestId = RIL_REQUEST_CDMA_SEND_SMS;
    RequestData *request = RilParser::CreateCdmaSms(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateCdmaSms(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateCdmaSmsAck) {
    char testVector[2] = {};
    int requestId = RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE;
    RequestData *request = RilParser::CreateCdmaSmsAck(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateCdmaSmsAck(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateCdmaSetBroadcastSmsConfig) {
    RIL_CDMA_BroadcastSmsConfigInfo testConfig = {};
    RIL_CDMA_BroadcastSmsConfigInfo *testVector[] = { &testConfig,  };
    int requestId = RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG;
    RequestData *request = RilParser::CreateCdmaSetBroadcastSmsConfig(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateCdmaSetBroadcastSmsConfig(requestId, 0, (char *)testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, CreateRuimSmsData) {
    RIL_CDMA_SMS_WriteArgs testVector = {};
    int requestId = RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM;
    RequestData *request = RilParser::CreateRuimSmsData(requestId, 0, 0, 0);
    EXPECT_EQ(request, nullptr);

    request = RilParser::CreateRuimSmsData(requestId, 0, (char *)&testVector,
            sizeof(testVector));
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

TEST(RilParser, OemCreateStrings) {
    int requestId = RIL_REQUEST_OEM_CHANGE_BARRING_PASSWORD_OVER_MMI;
    RequestData *request = RilParser::OemCreateStrings(requestId, 0, 0, 0);
    EXPECT_NE(request, nullptr);
    if (request) {
        delete request;
    }
}

#if 0 // Not implementation
TEST(RilParser, CreateOemSignalStrengthReportingCriteria) {
    using SignalThresholdInfo =
            android::hardware::radio::impl::network::V2_0::RIL_SignalThresholdInfo;
    SignalThresholdInfo testVector[] = {
        { 10, 20, 30, { 1, 2, 3 }, true, 4 },
        { 50, 60, 70, { -10, -20, -30 }, true, 6 },
        { 100, 200, 300, { 99, 99, 99, 99 }, false, 3 },
        { 3, 6, 7, {}, true, 2 },
    };
    size_t size = NUM_ELEMS(testVector);
    auto rildata = std::shared_ptr<RequestData>(
                        RilParser::CreateOemSignalStrengthReportingCriteria(
                                RIL_REQUEST_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, 0,
                                (char *)&testVector, sizeof(testVector)));
    ASSERT_NE(rildata, nullptr);
    auto test = static_cast<SignalStrengthReportingCriteria_V2_0 *>(rildata.get());
    EXPECT_EQ(test->GetSignalThresholdInfoNum(), (int)size);

    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(test->at(i) == testVector[i]);
    }
}
#endif
