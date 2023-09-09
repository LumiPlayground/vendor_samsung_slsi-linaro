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
#include "radioproxytestsetup.h"
#include "radiomessagingproxy.h"

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_messaging = ::aidl::android::hardware::radio::messaging;

using namespace android::hardware::radio;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using android::sp;

TEST(RadioMessagingProxyHook, acknowledgeIncomingGsmSmsWithPdu) {
    static int32_t s = INTrand(true);
    static bool testSuccess = true;
    static std::string testAckPdu = "test ackPdu string";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success, const hidl_string& ackPdu) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testSuccess, success);
            EXPECT_EQ(testAckPdu, ackPdu);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->acknowledgeIncomingGsmSmsWithPdu(s, testSuccess, testAckPdu);
}

TEST(RadioMessagingProxyHook, acknowledgeLastIncomingCdmaSms) {
    static int32_t s = INTrand(true);
    static aidl_messaging::CdmaSmsAck testCdmaSmsAck = {
        .errorClass = false,
        .smsCauseCode = 0
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> acknowledgeLastIncomingCdmaSms(int32_t serial, const V1_0::CdmaSmsAck &smsAck) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ((testCdmaSmsAck.errorClass ? V1_0::CdmaSmsErrorClass::ERROR : V1_0::CdmaSmsErrorClass::NO_ERROR), smsAck.errorClass);
            EXPECT_EQ(testCdmaSmsAck.smsCauseCode, smsAck.smsCauseCode);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->acknowledgeLastIncomingCdmaSms(s, testCdmaSmsAck);
}

TEST(RadioMessagingProxyHook, acknowledgeLastIncomingGsmSms) {
    static int32_t s = INTrand(true);
    static bool testSuccess = true;
    static aidl_messaging::SmsAcknowledgeFailCause testCause = aidl_messaging::SmsAcknowledgeFailCause::UNSPECIFIED_ERROR;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> acknowledgeLastIncomingGsmSms(int32_t serial, bool success, const V1_0::SmsAcknowledgeFailCause cause) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testSuccess, success);
            EXPECT_EQ(V1_0::SmsAcknowledgeFailCause{testCause}, cause);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->acknowledgeLastIncomingGsmSms(s, testSuccess, testCause);
}

TEST(RadioMessagingProxyHook, deleteSmsOnRuim) {
    static int32_t s = INTrand(true);
    static int32_t testIndex = 0;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> deleteSmsOnRuim(int32_t serial, int32_t index) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testIndex, index);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->deleteSmsOnRuim(s, testIndex);
}

TEST(RadioMessagingProxyHook, deleteSmsOnSim) {
    static int32_t s = INTrand(true);
    static int32_t testIndex = 0;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> deleteSmsOnSim(int32_t serial, int32_t index) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testIndex, index);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->deleteSmsOnSim(s, testIndex);
}

TEST(RadioMessagingProxyHook, getCdmaBroadcastConfig) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getCdmaBroadcastConfig(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->getCdmaBroadcastConfig(s);
}

TEST(RadioMessagingProxyHook, getGsmBroadcastConfig) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getGsmBroadcastConfig(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->getGsmBroadcastConfig(s);
}

TEST(RadioMessagingProxyHook, getSmscAddress) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getSmscAddress(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->getSmscAddress(s);
}

TEST(RadioMessagingProxyHook, reportSmsMemoryStatus) {
    static int32_t s = INTrand(true);
    static bool testAvailable = true;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> reportSmsMemoryStatus(int32_t serial, bool available) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testAvailable, available);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->reportSmsMemoryStatus(s, testAvailable);
}

TEST(RadioMessagingProxyHook, responseAcknowledgement) {
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> responseAcknowledgement() override {
            checker.checked();
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->responseAcknowledgement();
}

TEST(RadioMessagingProxyHook, sendCdmaSms) {
    static int32_t s = INTrand(true);
    static aidl_messaging::CdmaSmsMessage testSms {
        .teleserviceId = 0,
        .isServicePresent = true,
        .serviceCategory = 0,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendCdmaSms_1_6(int32_t serial, const V1_0::CdmaSmsMessage &sms) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testSms.teleserviceId, sms.teleserviceId);
            EXPECT_EQ(testSms.isServicePresent, sms.isServicePresent);
            EXPECT_EQ(testSms.serviceCategory, sms.serviceCategory);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->sendCdmaSms(s, testSms);
}

TEST(RadioMessagingProxyHook, sendCdmaSmsExpectMore) {
    static int32_t s = INTrand(true);
    static aidl_messaging::CdmaSmsMessage testSms {
        .teleserviceId = 0,
        .isServicePresent = true,
        .serviceCategory = 0,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendCdmaSmsExpectMore_1_6(int32_t serial, const V1_0::CdmaSmsMessage &sms) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testSms.teleserviceId, sms.teleserviceId);
            EXPECT_EQ(testSms.isServicePresent, sms.isServicePresent);
            EXPECT_EQ(testSms.serviceCategory, sms.serviceCategory);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->sendCdmaSmsExpectMore(s, testSms);
}

TEST(RadioMessagingProxyHook, sendImsSms) {
    static int32_t s = INTrand(true);
    static aidl_messaging::ImsSmsMessage testMessage {
        .retry = false,
        .messageRef = 0,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendImsSms(int32_t serial, const V1_0::ImsSmsMessage &message) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testMessage.retry, message.retry);
            EXPECT_EQ(testMessage.messageRef, message.messageRef);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->sendImsSms(s, testMessage);
}

TEST(RadioMessagingProxyHook, sendSms) {
    static int32_t s = INTrand(true);
    static aidl_messaging::GsmSmsMessage testMessage {
        .smscPdu = "smscPdu",
        .pdu = "pdu",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendSms_1_6(int32_t serial, const V1_0::GsmSmsMessage &message) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testMessage.smscPdu, message.smscPdu);
            EXPECT_EQ(testMessage.pdu, message.pdu);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->sendSms(s, testMessage);
}

TEST(RadioMessagingProxyHook, sendSmsExpectMore) {
    static int32_t s = INTrand(true);
    static aidl_messaging::GsmSmsMessage testMessage {
        .smscPdu = "smscPdu",
        .pdu = "pdu",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendSmsExpectMore_1_6(int32_t serial, const V1_0::GsmSmsMessage &message) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testMessage.smscPdu, message.smscPdu);
            EXPECT_EQ(testMessage.pdu, message.pdu);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->sendSmsExpectMore(s, testMessage);
}

TEST(RadioMessagingProxyHook, setCdmaBroadcastActivation) {
    static int32_t s = INTrand(true);
    static bool testActivate = true;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setCdmaBroadcastActivation(int32_t serial, bool activate) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testActivate, activate);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->setCdmaBroadcastActivation(s, testActivate);
}

TEST(RadioMessagingProxyHook, setCdmaBroadcastConfig) {
    static int32_t s = INTrand(true);
    static const std::vector<aidl_messaging::CdmaBroadcastSmsConfigInfo> testConfigInfo = {
        {
            .serviceCategory = 0,
            .language = 0,
            .selected = true
        }
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setCdmaBroadcastConfig(int32_t serial, const hidl_vec<V1_0::CdmaBroadcastSmsConfigInfo> &configInfo) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testConfigInfo[0].serviceCategory, configInfo[0].serviceCategory);
            EXPECT_EQ(testConfigInfo[0].language, configInfo[0].language);
            EXPECT_EQ(testConfigInfo[0].selected, configInfo[0].selected);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->setCdmaBroadcastConfig(s, testConfigInfo);
}

TEST(RadioMessagingProxyHook, setGsmBroadcastActivation) {
    static int32_t s = INTrand(true);
    static bool testActivate = true;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setGsmBroadcastActivation(int32_t serial, bool activate) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testActivate, activate);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->setGsmBroadcastActivation(s, testActivate);
}

TEST(RadioMessagingProxyHook, setGsmBroadcastConfig) {
    static int32_t s = INTrand(true);
    static const std::vector<aidl_messaging::GsmBroadcastSmsConfigInfo> testConfigInfo = {
        {
            .fromServiceId = 0,
            .toServiceId = 0,
            .fromCodeScheme = 0,
            .toCodeScheme = 0,
            .selected = true
        }
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setGsmBroadcastConfig(int32_t serial, const hidl_vec<V1_0::GsmBroadcastSmsConfigInfo> &configInfo) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testConfigInfo[0].fromServiceId, configInfo[0].fromServiceId);
            EXPECT_EQ(testConfigInfo[0].toServiceId, configInfo[0].toServiceId);
            EXPECT_EQ(testConfigInfo[0].fromCodeScheme, configInfo[0].fromCodeScheme);
            EXPECT_EQ(testConfigInfo[0].toCodeScheme, configInfo[0].toCodeScheme);
            EXPECT_EQ(testConfigInfo[0].selected, configInfo[0].selected);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->setGsmBroadcastConfig(s, testConfigInfo);
}

TEST(RadioMessagingProxyHook, setSmscAddress) {
    static int32_t s = INTrand(true);
    static std::string testSmsc = "smsc";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setSmscAddress(int32_t serial, const hidl_string& smsc) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(testSmsc, smsc);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->setSmscAddress(s, testSmsc);
}

TEST(RadioMessagingProxyHook, writeSmsToRuim) {
    static int32_t s = INTrand(true);
    static aidl_messaging::CdmaSmsWriteArgs testCdmaSms = {
        .status = aidl_messaging::CdmaSmsWriteArgs::STATUS_REC_UNREAD,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> writeSmsToRuim(int32_t serial, const V1_0::CdmaSmsWriteArgs &cdmaSms) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(V1_0::CdmaSmsWriteArgsStatus{testCdmaSms.status}, cdmaSms.status);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->writeSmsToRuim(s, testCdmaSms);
}

TEST(RadioMessagingProxyHook, writeSmsToSim) {
    static int32_t s = INTrand(true);
    static aidl_messaging::SmsWriteArgs testSmsWriteArgs = {
        .status = aidl_messaging::SmsWriteArgs::STATUS_REC_UNREAD,
        .pdu = "pdu",
        .smsc = "smsc"
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> writeSmsToSim(int32_t serial, const V1_0::SmsWriteArgs &smsWriteArgs) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_EQ(V1_0::SmsWriteArgsStatus{testSmsWriteArgs.status}, smsWriteArgs.status);
            EXPECT_EQ(testSmsWriteArgs.pdu, smsWriteArgs.pdu);
            EXPECT_EQ(testSmsWriteArgs.smsc, smsWriteArgs.smsc);
            return {};
        }
    };
    RadioProxyTestSetup<RadioMessagingProxy> test = sp<TestRadio>::make();
    test->writeSmsToSim(s, testSmsWriteArgs);
}