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
#include "radiosimproxy.h"

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_sim = ::aidl::android::hardware::radio::sim;

using namespace android::hardware::radio;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using android::sp;

TEST(RadioSimProxyHook, areUiccApplicationsEnabled) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> areUiccApplicationsEnabled(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->areUiccApplicationsEnabled(s);
}

TEST(RadioSimProxyHook, changeIccPin2ForApp) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string oldPin2;
        std::string newPin2;
        std::string aid;
    } testVector = {
        "oldPin2", "newPin2", "aid",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> changeIccPin2ForApp(int32_t serial, const hidl_string& oldPin2,
                const hidl_string& newPin2,const hidl_string& aid) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(oldPin2 == testVector.oldPin2);
            EXPECT_TRUE(newPin2 == testVector.newPin2);
            EXPECT_TRUE(aid == testVector.aid);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->changeIccPin2ForApp(s, testVector.oldPin2, testVector.newPin2, testVector.aid);
}

TEST(RadioSimProxyHook, changeIccPinForApp) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string oldPin;
        std::string newPin;
        std::string aid;
    } testVector = {
        "oldPin", "newPin", "aid",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> changeIccPinForApp(int32_t serial, const hidl_string& oldPin,
                const hidl_string& newPin,const hidl_string& aid) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(oldPin == testVector.oldPin);
            EXPECT_TRUE(newPin == testVector.newPin);
            EXPECT_TRUE(aid == testVector.aid);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->changeIccPinForApp(s, testVector.oldPin, testVector.newPin, testVector.aid);
}

TEST(RadioSimProxyHook, enableUiccApplications) {
    static int32_t s = INTrand(true);
    static bool testVector = true;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> enableUiccApplications(int32_t serial, bool enable) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(enable == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->enableUiccApplications(s, testVector);
}

TEST(RadioSimProxyHook, getAllowedCarriers) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getAllowedCarriers_1_4(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->getAllowedCarriers(s);
}

TEST(RadioSimProxyHook, getCdmaSubscription) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getCDMASubscription(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->getCdmaSubscription(s);
}

TEST(RadioSimProxyHook, getCdmaSubscriptionSource) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getCdmaSubscriptionSource(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->getCdmaSubscriptionSource(s);
}

TEST(RadioSimProxyHook, getFacilityLockForApp) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string facility;
        std::string password;
        int32_t serviceClass;
        std::string appId;
    } testVector = {
        "facility", "password", 0, "aid",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getFacilityLockForApp(int32_t serial, const hidl_string& facility,
                const hidl_string& password, int32_t serviceClass,
                const hidl_string& appId) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(facility == testVector.facility);
            EXPECT_TRUE(password == testVector.password);
            EXPECT_TRUE(serviceClass == testVector.serviceClass);
            EXPECT_TRUE(appId == testVector.appId);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->getFacilityLockForApp(s, testVector.facility, testVector.password,
            testVector.serviceClass, testVector.appId);
}

TEST(RadioSimProxyHook, getImsiForApp) {
    static int32_t s = INTrand(true);
    static std::string testVector = "aid";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getImsiForApp(int32_t serial, const hidl_string& aid) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(aid == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->getImsiForApp(s, testVector);
}

TEST(RadioSimProxyHook, getSimPhonebookCapacity) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getSimPhonebookCapacity(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->getSimPhonebookCapacity(s);
}

TEST(RadioSimProxyHook, getSimPhonebookRecords) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getSimPhonebookRecords(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->getSimPhonebookRecords(s);
}

TEST(RadioSimProxyHook, iccCloseLogicalChannel) {
    static int32_t s = INTrand(true);
    static int32_t testVector = 0;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> iccCloseLogicalChannel(int32_t serial, int32_t channelId) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(channelId == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->iccCloseLogicalChannel(s, testVector);
}

TEST(RadioSimProxyHook, iccIoForApp) {
    static int32_t s = INTrand(true);
    static aidl_sim::IccIo testVector = {
        .command = 0xb0,
        .fileId = 0x6f2f,
        .path = "3F00",
        .p1 = 0,
        .p2 = 0,
        .p3 = 4,
        .data = "data",
        .pin2 = "pin2",
        .aid = "aid"
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> iccIOForApp(int32_t serial, const V1_0::IccIo& iccIo) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(iccIo.command == testVector.command);
            EXPECT_TRUE(iccIo.fileId == testVector.fileId);
            EXPECT_TRUE(iccIo.path == testVector.path);
            EXPECT_TRUE(iccIo.p1 == testVector.p1);
            EXPECT_TRUE(iccIo.p2 == testVector.p2);
            EXPECT_TRUE(iccIo.p3 == testVector.p3);
            EXPECT_TRUE(iccIo.data == testVector.data);
            EXPECT_TRUE(iccIo.pin2 == testVector.pin2);
            EXPECT_TRUE(iccIo.aid == testVector.aid);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->iccIoForApp(s, testVector);
}

TEST(RadioSimProxyHook, iccOpenLogicalChannel) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string aid;
        int32_t p2;
    } testVector = {
        "aid", 0
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> iccOpenLogicalChannel(int32_t serial, const hidl_string& aid, int32_t p2)
                override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(aid == testVector.aid);
            EXPECT_TRUE(p2 == testVector.p2);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->iccOpenLogicalChannel(s, testVector.aid, testVector.p2);
}

TEST(RadioSimProxyHook, iccTransmitApduBasicChannel) {
    static int32_t s = INTrand(true);
    static aidl_sim::SimApdu testVector = {
        .sessionId = 1,
        .cla = 2,
        .instruction = 3,
        .p1 = 4,
        .p2 = 5,
        .p3 = 4,
        .data = "data",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> iccTransmitApduBasicChannel(int32_t serial, const V1_0::SimApdu& message)
                override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(message.sessionId == testVector.sessionId);
            EXPECT_TRUE(message.cla == testVector.cla);
            EXPECT_TRUE(message.instruction == testVector.instruction);
            EXPECT_TRUE(message.p1 == testVector.p1);
            EXPECT_TRUE(message.p2 == testVector.p2);
            EXPECT_TRUE(message.p3 == testVector.p3);
            EXPECT_TRUE(message.data == testVector.data);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->iccTransmitApduBasicChannel(s, testVector);
}

TEST(RadioSimProxyHook, iccTransmitApduLogicalChannel) {
    static int32_t s = INTrand(true);
    static aidl_sim::SimApdu testVector = {
        .sessionId = 1,
        .cla = 2,
        .instruction = 3,
        .p1 = 4,
        .p2 = 5,
        .p3 = 4,
        .data = "data",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> iccTransmitApduLogicalChannel(int32_t serial, const V1_0::SimApdu& message)
                override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(message.sessionId == testVector.sessionId);
            EXPECT_TRUE(message.cla == testVector.cla);
            EXPECT_TRUE(message.instruction == testVector.instruction);
            EXPECT_TRUE(message.p1 == testVector.p1);
            EXPECT_TRUE(message.p2 == testVector.p2);
            EXPECT_TRUE(message.p3 == testVector.p3);
            EXPECT_TRUE(message.data == testVector.data);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->iccTransmitApduLogicalChannel(s, testVector);
}

TEST(RadioSimProxyHook, reportStkServiceIsRunning) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> reportStkServiceIsRunning(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->reportStkServiceIsRunning(s);
}

TEST(RadioSimProxyHook, requestIccSimAuthentication) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        int32_t authContext;
        std::string authData;
        std::string aid;
    } testVector = {
        0, "authData", "aid"
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> requestIccSimAuthentication(int32_t serial, int32_t authContext,
                const hidl_string& authData, const hidl_string& aid) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(authContext == testVector.authContext);
            EXPECT_TRUE(authData == testVector.authData);
            EXPECT_TRUE(aid == testVector.aid);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->requestIccSimAuthentication(s, testVector.authContext, testVector.authData,
            testVector.aid);
}

TEST(RadioSimProxyHook, responseAcknowledgement) {
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> responseAcknowledgement() override {
            checker.checked();
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->responseAcknowledgement();
}

TEST(RadioSimProxyHook, sendEnvelope) {
    static int32_t s = INTrand(true);
    static std::string testVector = "command";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendEnvelope(int32_t serial, const hidl_string& command) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(command == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->sendEnvelope(s, testVector);
}

TEST(RadioSimProxyHook, sendEnvelopeWithStatus) {
    static int32_t s = INTrand(true);
    static std::string testVector = "contents";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendEnvelopeWithStatus(int32_t serial, const hidl_string& contents) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(contents == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->sendEnvelopeWithStatus(s, testVector);
}

TEST(RadioSimProxyHook, sendTerminalResponseToSim) {
    static int32_t s = INTrand(true);
    static std::string testVector = "commandResponse";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendTerminalResponseToSim(int32_t serial, const hidl_string& commandResponse)
                override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(commandResponse == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->sendTerminalResponseToSim(s, testVector);
}

TEST(RadioSimProxyHook, setAllowedCarriers) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        aidl_sim::CarrierRestrictions carriers;
        aidl_sim::SimLockMultiSimPolicy multiSimPolicy;
    } testVector = {
        {
            {
                { "450", "05", aidl_sim::Carrier::MATCH_TYPE_SPN, "spn" },
                { "001", "01", aidl_sim::Carrier::MATCH_TYPE_IMSI_PREFIX, "imsi" },
                { "310", "480", aidl_sim::Carrier::MATCH_TYPE_ALL, "" }
            },
            {
                { "450", "05", aidl_sim::Carrier::MATCH_TYPE_SPN, "spn" },
                { "001", "01", aidl_sim::Carrier::MATCH_TYPE_IMSI_PREFIX, "imsi" },
                { "310", "480", aidl_sim::Carrier::MATCH_TYPE_ALL, "" }
            },
            true
        },
        aidl_sim::SimLockMultiSimPolicy::NO_MULTISIM_POLICY
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setAllowedCarriers_1_4(int32_t serial,
                const V1_4::CarrierRestrictionsWithPriority& carriers,
                V1_4::SimLockMultiSimPolicy multiSimPolicy) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            int i = 0;
            for (auto& carrier : carriers.allowedCarriers) {
                aidl_sim::Carrier& testCarrier = testVector.carriers.allowedCarriers[i++];
                EXPECT_TRUE(carrier.mcc == testCarrier.mcc);
                EXPECT_TRUE(carrier.mnc == testCarrier.mnc);
                EXPECT_TRUE(carrier.matchType == V1_0::CarrierMatchType{testCarrier.matchType});
                EXPECT_TRUE(carrier.matchData == testCarrier.matchData);
            }
            i = 0;
            for (auto& carrier : carriers.excludedCarriers) {
                aidl_sim::Carrier& testCarrier = testVector.carriers.excludedCarriers[i++];
                EXPECT_TRUE(carrier.mcc == testCarrier.mcc);
                EXPECT_TRUE(carrier.mnc == testCarrier.mnc);
                EXPECT_TRUE(carrier.matchType == V1_0::CarrierMatchType{testCarrier.matchType});
                EXPECT_TRUE(carrier.matchData == testCarrier.matchData);
            }
            EXPECT_TRUE(carriers.allowedCarriersPrioritized ==
                    testVector.carriers.allowedCarriersPrioritized);
            EXPECT_TRUE(multiSimPolicy == V1_4::SimLockMultiSimPolicy{testVector.multiSimPolicy});
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->setAllowedCarriers(s, testVector.carriers, testVector.multiSimPolicy);
}

TEST(RadioSimProxyHook, setCarrierInfoForImsiEncryption) {
    static int32_t s = INTrand(true);
    static aidl_sim::ImsiEncryptionInfo testVector = {
        .mcc = "450",
        .mnc = "05",
        .carrierKey = {1, 2, 3},
        .keyIdentifier = "keyIdentifier",
        .expirationTime = 0L,
        .keyType = 0
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setCarrierInfoForImsiEncryption_1_6(int32_t serial,
                const V1_6::ImsiEncryptionInfo& imsiEncryptionInfo) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(imsiEncryptionInfo.base.mcc == testVector.mcc);
            EXPECT_TRUE(imsiEncryptionInfo.base.mnc == testVector.mnc);
            EXPECT_TRUE(imsiEncryptionInfo.base.carrierKey == testVector.carrierKey);
            EXPECT_TRUE(imsiEncryptionInfo.base.keyIdentifier == testVector.keyIdentifier);
            EXPECT_TRUE(imsiEncryptionInfo.base.expirationTime == testVector.expirationTime);
            EXPECT_TRUE(imsiEncryptionInfo.keyType == V1_6::PublicKeyType{testVector.keyType});
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->setCarrierInfoForImsiEncryption(s, testVector);
}

TEST(RadioSimProxyHook, setCdmaSubscriptionSource) {
    static int32_t s = INTrand(true);
    static aidl_sim::CdmaSubscriptionSource testVector = aidl_sim::CdmaSubscriptionSource::RUIM_SIM;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setCdmaSubscriptionSource(int32_t serial,
                V1_0::CdmaSubscriptionSource cdmaSub) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(cdmaSub == V1_0::CdmaSubscriptionSource{testVector});
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->setCdmaSubscriptionSource(s, testVector);
}

TEST(RadioSimProxyHook, setFacilityLockForApp) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string facility;
        bool lockState;
        std::string password;
        int32_t serviceClass;
        std::string appId;
    } testVector = {
        "facility", false, "password", 0, "aid",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setFacilityLockForApp(int32_t serial, const hidl_string& facility,
                bool lockState, const hidl_string& password, int32_t serviceClass,
                const hidl_string& appId) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(facility == testVector.facility);
            EXPECT_TRUE(lockState == testVector.lockState);
            EXPECT_TRUE(password == testVector.password);
            EXPECT_TRUE(serviceClass == testVector.serviceClass);
            EXPECT_TRUE(appId == testVector.appId);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->setFacilityLockForApp(s, testVector.facility, testVector.lockState, testVector.password,
            testVector.serviceClass, testVector.appId);
}

TEST(RadioSimProxyHook, setSimCardPower) {
    static int32_t s = INTrand(true);
    static aidl_sim::CardPowerState testVector = aidl_sim::CardPowerState::POWER_UP;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setSimCardPower_1_6(int32_t serial,
                const V1_1::CardPowerState powerUp) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(powerUp == V1_1::CardPowerState{testVector});
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->setSimCardPower(s, testVector);
}

TEST(RadioSimProxyHook, setUiccSubscription) {
    static int32_t s = INTrand(true);
    static aidl_sim::SelectUiccSub testVector = {
        .slot = 0,
        .appIndex = 1,
        .subType = 2,
        .actStatus = 3,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setUiccSubscription(int32_t serial, const V1_0::SelectUiccSub& uiccSub)
                override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(uiccSub.slot == testVector.slot);
            EXPECT_TRUE(uiccSub.appIndex == testVector.appIndex);
            EXPECT_TRUE(uiccSub.subType == V1_0::SubscriptionType{testVector.subType});
            EXPECT_TRUE(uiccSub.actStatus == V1_0::UiccSubActStatus{testVector.actStatus});
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->setUiccSubscription(s, testVector);
}

TEST(RadioSimProxyHook, supplyIccPin2ForApp) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string pin2;
        std::string aid;
    } testVector = {
        "pin2", "aid"
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> supplyIccPin2ForApp(int32_t serial, const hidl_string& pin2,
                const hidl_string& aid) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(pin2 == testVector.pin2);
            EXPECT_TRUE(aid == testVector.aid);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->supplyIccPin2ForApp(s, testVector.pin2, testVector.aid);
}

TEST(RadioSimProxyHook, supplyIccPinForApp) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string pin;
        std::string aid;
    } testVector = {
        "pin", "aid"
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> supplyIccPinForApp(int32_t serial, const hidl_string& pin,
                const hidl_string& aid) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(pin == testVector.pin);
            EXPECT_TRUE(aid == testVector.aid);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->supplyIccPinForApp(s, testVector.pin, testVector.aid);
}

TEST(RadioSimProxyHook, supplyIccPuk2ForApp) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string puk2;
        std::string pin2;
        std::string aid;
    } testVector = {
        "puk2", "pin2", "aid"
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> supplyIccPuk2ForApp(int32_t serial, const hidl_string& puk2,
                const hidl_string& pin2, const hidl_string& aid) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(puk2 == testVector.puk2);
            EXPECT_TRUE(pin2 == testVector.pin2);
            EXPECT_TRUE(aid == testVector.aid);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->supplyIccPuk2ForApp(s, testVector.puk2, testVector.pin2, testVector.aid);
}

TEST(RadioSimProxyHook, supplyIccPukForApp) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        std::string puk;
        std::string pin;
        std::string aid;
    } testVector = {
        "puk", "pin", "aid"
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> supplyIccPukForApp(int32_t serial, const hidl_string& puk,
                const hidl_string& pin, const hidl_string& aid) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(puk == testVector.puk);
            EXPECT_TRUE(pin == testVector.pin);
            EXPECT_TRUE(aid == testVector.aid);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->supplyIccPukForApp(s, testVector.puk, testVector.pin, testVector.aid);
}

TEST(RadioSimProxyHook, supplySimDepersonalization) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        aidl_sim::PersoSubstate persoType;
        std::string controlKey;
    } testVector = {
        aidl_sim::PersoSubstate::UNKNOWN, "controlKey"
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> supplySimDepersonalization(int32_t serial, V1_5::PersoSubstate persoType,
                const hidl_string& controlKey) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(persoType == V1_5::PersoSubstate{testVector.persoType});
            EXPECT_TRUE(controlKey == testVector.controlKey);
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->supplySimDepersonalization(s, testVector.persoType, testVector.controlKey);
}

TEST(RadioSimProxyHook, updateSimPhonebookRecords) {
    static int32_t s = INTrand(true);
    static aidl_sim::PhonebookRecordInfo testVector = {
        .recordId = 0,
        .name = "name",
        .number = "number",
        .emails = {"1@1.com", "2@2.com"},
        .additionalNumbers = {"1111"}
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> updateSimPhonebookRecords(int32_t serial,
                const V1_6::PhonebookRecordInfo& pbRecordInfo) override {
            checker.checked();
            EXPECT_TRUE(static_cast<int32_t>(serial) == s);
            EXPECT_TRUE(pbRecordInfo.recordId == static_cast<uint32_t>(testVector.recordId));
            EXPECT_TRUE(pbRecordInfo.name == testVector.name);
            EXPECT_TRUE(pbRecordInfo.number == testVector.number);
            int i = 0;
            for (auto& email : pbRecordInfo.emails) {
                EXPECT_TRUE(email == testVector.emails[i++]);
            }
            i = 0;
            for (auto& additionalNumber : pbRecordInfo.additionalNumbers) {
                EXPECT_TRUE(additionalNumber == testVector.additionalNumbers[i++]);
            }
            return {};
        }
    };
    RadioProxyTestSetup<RadioSimProxy> test = sp<TestRadio>::make();
    test->updateSimPhonebookRecords(s, testVector);
}
