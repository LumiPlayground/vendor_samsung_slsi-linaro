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
#include <testutils.h>
#include "commandsinfo.h"
#include "hookcommandstestsetup.h"
#include <slsi/radio_v2_0.h>
#include <ril_internal.h>
#include "radiocallbackadapter.h"

#include <utils/Log.h>

using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_messaging = ::aidl::android::hardware::radio::messaging;

static HookCommandsTestSetup<RadioMessagingResponse, RadioMessagingIndication> sTestStub;

TEST(MessagingCommandsIndHook, cdmaNewSms) {
    static RIL_CDMA_SMS_Message testCdmaSms = {
        .uTeleserviceID = 0x1002,
        .bIsServicePresent = 0x0,
        .uServicecategory = 0x0};

    struct TestIndicationCb : public RadioMessagingIndicationCallbackAdapter {
        TestChecker checker;
        void cdmaNewSms(aidl_common::RadioIndicationType /* type */, const aidl_messaging::CdmaSmsMessage& msg) override {
            checker.checked();
            EXPECT_EQ(testCdmaSms.uTeleserviceID, msg.teleserviceId);
            EXPECT_EQ(testCdmaSms.bIsServicePresent, msg.isServicePresent);
            EXPECT_EQ(testCdmaSms.uServicecategory, msg.serviceCategory);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new RadioMessagingResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioMessagingIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_CDMA_NEW_SMS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testCdmaSms, sizeof(testCdmaSms));
    sTestStub.teardown();
}

TEST(MessagingCommandsIndHook, cdmaRuimSmsStorageFull) {
    struct TestIndicationCb : public RadioMessagingIndicationCallbackAdapter {
        TestChecker checker;
        void cdmaRuimSmsStorageFull(aidl_common::RadioIndicationType /* type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new RadioMessagingResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioMessagingIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(MessagingCommandsIndHook, newBroadcastSms) {
    static int testBytePduSize = 1;
    static unsigned char testBytePdu[1] = {0x12};

    struct TestIndicationCb : public RadioMessagingIndicationCallbackAdapter {
        TestChecker checker;
        void newBroadcastSms(aidl_common::RadioIndicationType /* type */, const std::vector<uint8_t>& data) override {
            checker.checked();
            EXPECT_EQ(testBytePdu[0], data[0]);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new RadioMessagingResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioMessagingIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testBytePdu, testBytePduSize);
    sTestStub.teardown();
}

TEST(MessagingCommandsIndHook, newSms) {
    static int testBytePduSize = 1;
    static unsigned char testBytePdu[1] = {0x12};
    static int testCharPduSize = testBytePduSize * 2;
    static char testCharPdu[2] = {'1', '2'};

    struct TestIndicationCb : public RadioMessagingIndicationCallbackAdapter {
        TestChecker checker;
        void newSms(aidl_common::RadioIndicationType /* type */, const std::vector<uint8_t>& pdu) override {
            checker.checked();
            EXPECT_EQ(testBytePdu[0], pdu[0]);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new RadioMessagingResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioMessagingIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_NEW_SMS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testCharPdu, testCharPduSize);
    sTestStub.teardown();
}

TEST(MessagingCommandsIndHook, newSmsOnSim) {
    static int32_t testRecordNumber = 100;

    struct TestIndicationCb : public RadioMessagingIndicationCallbackAdapter {
        TestChecker checker;
        void newSmsOnSim(aidl_common::RadioIndicationType /* type */, int32_t recordNumber) override {
            checker.checked();
            EXPECT_EQ(testRecordNumber, recordNumber);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new RadioMessagingResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioMessagingIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testRecordNumber, sizeof(testRecordNumber));
    sTestStub.teardown();
}

TEST(MessagingCommandsIndHook, newSmsStatusReport) {
    static int testBytePduSize = 1;
    static unsigned char testBytePdu[1] = {0x12};
    static int testCharPduSize = testBytePduSize * 2;
    static char testCharPdu[2] = {'1', '2'};

    struct TestIndicationCb : public RadioMessagingIndicationCallbackAdapter {
        TestChecker checker;
        void newSmsStatusReport(aidl_common::RadioIndicationType /* type */, const std::vector<uint8_t>& pdu) override {
            checker.checked();
            EXPECT_EQ(testBytePdu[0], pdu[0]);
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new RadioMessagingResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioMessagingIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testCharPdu, testCharPduSize);
    sTestStub.teardown();
}

TEST(MessagingCommandsIndHook, simSmsStorageFull) {
    struct TestIndicationCb : public RadioMessagingIndicationCallbackAdapter {
        TestChecker checker;
        void simSmsStorageFull(aidl_common::RadioIndicationType /* type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioMessagingResponseCallback>(new RadioMessagingResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioMessagingIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_SIM_SMS_STORAGE_FULL);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}