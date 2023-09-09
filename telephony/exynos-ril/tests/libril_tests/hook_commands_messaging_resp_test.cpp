/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <testutils.h>
#include "commandsinfo.h"
#include "hookcommandstestsetup.h"
#include <slsi/radio_v2_0.h>
#include "radiomessagingresponse.h"
#include "radiocallbackadapter.h"

using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_messaging = ::aidl::android::hardware::radio::messaging;

static HookCommandsTestSetup<RadioMessagingResponse, RadioMessagingIndication> sTestStub;
extern unsigned char *SMS_UT_SMSC_BYTE;

TEST(MessagingCommandsRespHook, acknowledgeIncomingGsmSmsWithPduResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void acknowledgeIncomingGsmSmsWithPduResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, acknowledgeLastIncomingCdmaSmsResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void acknowledgeLastIncomingCdmaSmsResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, acknowledgeLastIncomingGsmSmsResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void acknowledgeLastIncomingGsmSmsResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SMS_ACKNOWLEDGE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, deleteSmsOnRuimResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void deleteSmsOnRuimResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, deleteSmsOnSimResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void deleteSmsOnSimResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_DELETE_SMS_ON_SIM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, getCdmaBroadcastConfigResponse) {
    static int testCdmaBcstSmsConfigInfoNum = 1;
    static RIL_CDMA_BroadcastSmsConfigInfo testCdmaBcstSmsConfigInfo[1] = {{
        .service_category = 0,
        .language = 0,
        .selected = 1 /*true*/}};
    static RIL_CDMA_BroadcastSmsConfigInfo* testCdmaBcstSmsConfigInfoPtr[1] = {&testCdmaBcstSmsConfigInfo[0]};

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void getCdmaBroadcastConfigResponse(const aidl_common::RadioResponseInfo& /* info */, const std::vector<aidl_messaging::CdmaBroadcastSmsConfigInfo>& configs) override {
            checker.checked();
            EXPECT_EQ(testCdmaBcstSmsConfigInfoPtr[0]->service_category, configs[0].serviceCategory);
            EXPECT_EQ(testCdmaBcstSmsConfigInfoPtr[0]->language, configs[0].language);
            EXPECT_EQ(testCdmaBcstSmsConfigInfoPtr[0]->selected, configs[0].selected);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testCdmaBcstSmsConfigInfoPtr, testCdmaBcstSmsConfigInfoNum * sizeof(RIL_CDMA_BroadcastSmsConfigInfo*));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, getGsmBroadcastConfigResponse) {
    static int testGsmBcstSmsConfigInfoNum = 1;
    static RIL_GSM_BroadcastSmsConfigInfo testGsmBcstSmsConfigInfo[1] = {
        {
            .fromServiceId = 0,
            .toServiceId = 0,
            .fromCodeScheme = 0,
            .toCodeScheme = 0,
            .selected = 1 /*true*/
        }
    };
    static RIL_GSM_BroadcastSmsConfigInfo* testGsmBcstSmsConfigInfoPtr[1] = {&testGsmBcstSmsConfigInfo[0]};

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void getGsmBroadcastConfigResponse(const aidl_common::RadioResponseInfo& /* info */, const std::vector<aidl_messaging::GsmBroadcastSmsConfigInfo>& configs) override {
            checker.checked();
            EXPECT_EQ(testGsmBcstSmsConfigInfoPtr[0]->fromServiceId, configs[0].fromServiceId );
            EXPECT_EQ(testGsmBcstSmsConfigInfoPtr[0]->toServiceId, configs[0].toServiceId);
            EXPECT_EQ(testGsmBcstSmsConfigInfoPtr[0]->fromCodeScheme, configs[0].fromCodeScheme);
            EXPECT_EQ(testGsmBcstSmsConfigInfoPtr[0]->toCodeScheme, configs[0].toCodeScheme);
            EXPECT_EQ(testGsmBcstSmsConfigInfoPtr[0]->selected, configs[0].selected);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testGsmBcstSmsConfigInfoPtr, testGsmBcstSmsConfigInfoNum * sizeof(RIL_GSM_BroadcastSmsConfigInfo*));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, getSmscAddressResponse) {
    static int testCharPduSize = 2;
    static char testCharPdu[3] = {'1', '2', '\0'};
    static std::string testStringPdu = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void getSmscAddressResponse(const aidl_common::RadioResponseInfo& /* info */, const std::string& smsc) override {
            checker.checked();
            EXPECT_EQ(testStringPdu, smsc);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_SMSC_ADDRESS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testCharPdu, testCharPduSize);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, reportSmsMemoryStatusResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void reportSmsMemoryStatusResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_REPORT_SMS_MEMORY_STATUS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, sendCdmaSmsExpectMoreResponse) {
    static char testAckPdu[3] = {'1', '2', '\0'};
    static RIL_SMS_Response testSmsResp = {
        .messageRef = 0,
        .ackPDU = testAckPdu,
        .errorCode = 0
    };
    static std::string testAckPduString = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void sendCdmaSmsExpectMoreResponse(const aidl_common::RadioResponseInfo& /* info */, const aidl_messaging::SendSmsResult& sms) override {
            checker.checked();
            EXPECT_EQ(testSmsResp.messageRef, sms.messageRef);
            EXPECT_EQ(testAckPduString, sms.ackPDU);
            EXPECT_EQ(testSmsResp.errorCode, sms.errorCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testSmsResp, sizeof(RIL_SMS_Response));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, sendCdmaSmsExpectMoreResponse_1_6) {
    static char testAckPdu[3] = {'1', '2', '\0'};
    static RIL_SMS_Response testSmsResp = {
        .messageRef = 0,
        .ackPDU = testAckPdu,
        .errorCode = 0
    };
    static std::string testAckPduString = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void sendCdmaSmsExpectMoreResponse(const aidl_common::RadioResponseInfo& /* info */, const aidl_messaging::SendSmsResult& sms) override {
            checker.checked();
            EXPECT_EQ(testSmsResp.messageRef, sms.messageRef);
            EXPECT_EQ(testAckPduString, sms.ackPDU);
            EXPECT_EQ(testSmsResp.errorCode, sms.errorCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE, HAL_VERSION_CODE(1, 6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testSmsResp, sizeof(RIL_SMS_Response));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, sendCdmaSmsResponse) {
    static char testAckPdu[3] = {'1', '2', '\0'};
    static RIL_SMS_Response testSmsResp = {
        .messageRef = 0,
        .ackPDU = testAckPdu,
        .errorCode = 0
    };
    static std::string testAckPduString = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void sendCdmaSmsResponse(const aidl_common::RadioResponseInfo& /* info */, const aidl_messaging::SendSmsResult& sms) override {
            checker.checked();
            EXPECT_EQ(testSmsResp.messageRef, sms.messageRef);
            EXPECT_EQ(testAckPduString, sms.ackPDU);
            EXPECT_EQ(testSmsResp.errorCode, sms.errorCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_SEND_SMS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testSmsResp, sizeof(RIL_SMS_Response));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, sendCdmaSmsResponse_1_6) {
    static char testAckPdu[3] = {'1', '2', '\0'};
    static RIL_SMS_Response testSmsResp = {
        .messageRef = 0,
        .ackPDU = testAckPdu,
        .errorCode = 0
    };
    static std::string testAckPduString = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void sendCdmaSmsResponse(const aidl_common::RadioResponseInfo& /* info */, const aidl_messaging::SendSmsResult& sms) override {
            checker.checked();
            EXPECT_EQ(testSmsResp.messageRef, sms.messageRef);
            EXPECT_EQ(testAckPduString, sms.ackPDU);
            EXPECT_EQ(testSmsResp.errorCode, sms.errorCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_CDMA_SEND_SMS, HAL_VERSION_CODE(1, 6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testSmsResp, sizeof(RIL_SMS_Response));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, sendImsSmsResponse) {
    static char testAckPdu[3] = {'1', '2', '\0'};
    static RIL_SMS_Response testSmsResp = {
        .messageRef = 0,
        .ackPDU = testAckPdu,
        .errorCode = 0
    };
    static std::string testAckPduString = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void sendImsSmsResponse(const aidl_common::RadioResponseInfo& /* info */, const aidl_messaging::SendSmsResult& sms) override {
            checker.checked();
            EXPECT_EQ(testSmsResp.messageRef, sms.messageRef);
            EXPECT_EQ(testAckPduString, sms.ackPDU);
            EXPECT_EQ(testSmsResp.errorCode, sms.errorCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_IMS_SEND_SMS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testSmsResp, sizeof(RIL_SMS_Response));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, sendSmsExpectMoreResponse) {
    static char testAckPdu[3] = {'1', '2', '\0'};
    static RIL_SMS_Response testSmsResp = {
        .messageRef = 0,
        .ackPDU = testAckPdu,
        .errorCode = 0
    };
    static std::string testAckPduString = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void sendSmsExpectMoreResponse(const aidl_common::RadioResponseInfo& /* info */, const aidl_messaging::SendSmsResult& sms) override {
            checker.checked();
            EXPECT_EQ(testSmsResp.messageRef, sms.messageRef);
            EXPECT_EQ(testAckPduString, sms.ackPDU);
            EXPECT_EQ(testSmsResp.errorCode, sms.errorCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SEND_SMS_EXPECT_MORE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testSmsResp, sizeof(RIL_SMS_Response));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, sendSmsExpectMoreResponse_1_6) {
    static char testAckPdu[3] = {'1', '2', '\0'};
    static RIL_SMS_Response testSmsResp = {
        .messageRef = 0,
        .ackPDU = testAckPdu,
        .errorCode = 0
    };
    static std::string testAckPduString = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void sendSmsExpectMoreResponse(const aidl_common::RadioResponseInfo& /* info */, const aidl_messaging::SendSmsResult& sms) override {
            checker.checked();
            EXPECT_EQ(testSmsResp.messageRef, sms.messageRef);
            EXPECT_EQ(testAckPduString, sms.ackPDU);
            EXPECT_EQ(testSmsResp.errorCode, sms.errorCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SEND_SMS_EXPECT_MORE, HAL_VERSION_CODE(1, 6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testSmsResp, sizeof(RIL_SMS_Response));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, sendSmsResponse) {
    static char testAckPdu[3] = {'1', '2', '\0'};
    static RIL_SMS_Response testSmsResp = {
        .messageRef = 0,
        .ackPDU = testAckPdu,
        .errorCode = 0
    };
    static std::string testAckPduString = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void sendSmsResponse(const aidl_common::RadioResponseInfo& /* info */, const aidl_messaging::SendSmsResult& sms) override {
            checker.checked();
            EXPECT_EQ(testSmsResp.messageRef, sms.messageRef);
            EXPECT_EQ(testAckPduString, sms.ackPDU);
            EXPECT_EQ(testSmsResp.errorCode, sms.errorCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SEND_SMS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testSmsResp, sizeof(RIL_SMS_Response));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, sendSmsResponse_1_6) {
    static char testAckPdu[3] = {'1', '2', '\0'};
    static RIL_SMS_Response testSmsResp = {
        .messageRef = 0,
        .ackPDU = testAckPdu,
        .errorCode = 0
    };
    static std::string testAckPduString = "12";

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void sendSmsResponse(const aidl_common::RadioResponseInfo& /* info */, const aidl_messaging::SendSmsResult& sms) override {
            checker.checked();
            EXPECT_EQ(testSmsResp.messageRef, sms.messageRef);
            EXPECT_EQ(testAckPduString, sms.ackPDU);
            EXPECT_EQ(testSmsResp.errorCode, sms.errorCode);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SEND_SMS, HAL_VERSION_CODE(1, 6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testSmsResp, sizeof(RIL_SMS_Response));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, setCdmaBroadcastActivationResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void setCdmaBroadcastActivationResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, setCdmaBroadcastConfigResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void setCdmaBroadcastConfigResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, setGsmBroadcastActivationResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void setGsmBroadcastActivationResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, setGsmBroadcastConfigResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void setGsmBroadcastConfigResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, setSmscAddressResponse) {
    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void setSmscAddressResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_SMSC_ADDRESS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, writeSmsToRuimResponse) {
    static int32_t testIndex = 0;

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void writeSmsToRuimResponse(const aidl_common::RadioResponseInfo& /* info */, const int32_t index) override {
            checker.checked();
            EXPECT_EQ(testIndex, index);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testIndex, sizeof(int32_t));
    sTestStub.teardown();
}

TEST(MessagingCommandsRespHook, writeSmsToSimResponse) {
    static int32_t testIndex = 0;

    struct TestResponseCb : public RadioMessagingResponseCallbackAdapter {
        TestChecker checker;
        void writeSmsToSimResponse(const aidl_common::RadioResponseInfo& /* info */, const int32_t index) override {
            checker.checked();
            EXPECT_EQ(testIndex, index);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioMessagingIndicationCallback>(new RadioMessagingIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_WRITE_SMS_TO_SIM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testIndex, sizeof(int32_t));
    sTestStub.teardown();
}