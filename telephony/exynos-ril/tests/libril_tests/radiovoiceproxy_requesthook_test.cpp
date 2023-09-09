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
#include "radiovoiceproxy.h"

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_voice = ::aidl::android::hardware::radio::voice;

using namespace android::hardware::radio;
using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using android::sp;

TEST(RadioVoiceProxyHook, acceptCall) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> acceptCall(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->acceptCall(s);
}

TEST(RadioVoiceProxyHook, cancelPendingUssd) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> cancelPendingUssd(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->cancelPendingUssd(s);
}

TEST(RadioVoiceProxyHook, conference) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> conference(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->conference(s);
}

TEST(RadioVoiceProxyHook, dial) {
    static int32_t s = INTrand(true);
    static aidl_voice::Dial testVector = {
        "12345", aidl_voice::Dial::CLIR_DEFAULT, {},
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> dial(int32_t serial, const V1_0::Dial& dialInfo) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(dialInfo.address == testVector.address);
            EXPECT_TRUE(static_cast<int32_t>(dialInfo.clir) == static_cast<int32_t>(testVector.clir));
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->dial(s, testVector);
}

TEST(RadioVoiceProxyHook, emergencyDial) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        aidl_voice::Dial dialInfo;
        int32_t categories;
        std::vector<std::string> urns;
        aidl_voice::EmergencyCallRouting routing;
        bool hasKnownUserIntentEmergency;
        bool isTesting;
    } testVector = {
        {"112", aidl_voice::Dial::CLIR_DEFAULT, {}}, 1, {{}, {}},
        aidl_voice::EmergencyCallRouting::EMERGENCY, false, false,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> emergencyDial(int32_t serial, const V1_0::Dial& dialInfo,
                hidl_bitfield<V1_4::EmergencyServiceCategory> categories,
                const hidl_vec<hidl_string>& urns, V1_4::EmergencyCallRouting routing,
                bool hasKnownUserIntentEmergency, bool isTesting) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(dialInfo.address == testVector.dialInfo.address);
            EXPECT_TRUE(static_cast<int32_t>(dialInfo.clir) == static_cast<int32_t>(testVector.dialInfo.clir));
            EXPECT_TRUE(categories == testVector.categories);
            EXPECT_TRUE(urns.size() == testVector.urns.size());
            EXPECT_TRUE(static_cast<int32_t>(routing) == static_cast<int32_t>(testVector.routing));
            EXPECT_TRUE(hasKnownUserIntentEmergency == testVector.hasKnownUserIntentEmergency);
            EXPECT_TRUE(isTesting == testVector.isTesting);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->emergencyDial(s, testVector.dialInfo, testVector.categories, testVector.urns, testVector.routing,
            testVector.hasKnownUserIntentEmergency, testVector.isTesting);
}

TEST(RadioVoiceProxyHook, exitEmergencyCallbackMode) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> exitEmergencyCallbackMode(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->exitEmergencyCallbackMode(s);
}

TEST(RadioVoiceProxyHook, explicitCallTransfer) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> explicitCallTransfer(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->explicitCallTransfer(s);
}

TEST(RadioVoiceProxyHook, getCallForwardStatus) {
    static int32_t s = INTrand(true);
    static aidl_voice::CallForwardInfo testVector = {
        aidl_voice::CallForwardInfo::STATUS_ENABLE, 0, 1, 145, "12345", 1,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getCallForwardStatus(int32_t serial, const V1_0::CallForwardInfo& callInfo) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int32_t>(callInfo.status) == testVector.status);
            EXPECT_TRUE(callInfo.reason == testVector.reason);
            EXPECT_TRUE(callInfo.serviceClass == testVector.serviceClass);
            EXPECT_TRUE(callInfo.toa == testVector.toa);
            EXPECT_TRUE(callInfo.number == testVector.number);
            EXPECT_TRUE(callInfo.timeSeconds == testVector.timeSeconds);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->getCallForwardStatus(s, testVector);
}

TEST(RadioVoiceProxyHook, getCallWaiting) {
    static int32_t s = INTrand(true);
    static int32_t testVector = 1;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getCallWaiting(int32_t serial, int32_t serviceClass) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(serviceClass == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->getCallWaiting(s, testVector);
}

TEST(RadioVoiceProxyHook, getClip) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getClip(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->getClip(s);
}

TEST(RadioVoiceProxyHook, getClir) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getClir(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->getClir(s);
}

TEST(RadioVoiceProxyHook, getCurrentCalls) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getCurrentCalls_1_6(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->getCurrentCalls(s);
}

TEST(RadioVoiceProxyHook, getLastCallFailCause) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getLastCallFailCause(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->getLastCallFailCause(s);
}

TEST(RadioVoiceProxyHook, getMute) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getMute(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->getMute(s);
}

TEST(RadioVoiceProxyHook, getPreferredVoicePrivacy) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getPreferredVoicePrivacy(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->getPreferredVoicePrivacy(s);
}

TEST(RadioVoiceProxyHook, getTtyMode) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getTTYMode(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->getTtyMode(s);
}

TEST(RadioVoiceProxyHook, handleStkCallSetupRequestFromSim) {
    static int32_t s = INTrand(true);
    static bool testVector = true;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> handleStkCallSetupRequestFromSim(int32_t serial, bool accept) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(accept == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->handleStkCallSetupRequestFromSim(s, testVector);
}

TEST(RadioVoiceProxyHook, hangup) {
    static int32_t s = INTrand(true);
    static int32_t testVector = 1;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> hangup(int32_t serial, int32_t gsmIndex) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(gsmIndex == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->hangup(s, testVector);
}

TEST(RadioVoiceProxyHook, hangupForegroundResumeBackground) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> hangupForegroundResumeBackground(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->hangupForegroundResumeBackground(s);
}

TEST(RadioVoiceProxyHook, hangupWaitingOrBackground) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> hangupWaitingOrBackground(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->hangupWaitingOrBackground(s);
}

TEST(RadioVoiceProxyHook, rejectCall) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> rejectCall(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->rejectCall(s);
}

TEST(RadioVoiceProxyHook, responseAcknowledgement) {
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> responseAcknowledgement() override {
            checker.checked();
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->responseAcknowledgement();
}

TEST(RadioVoiceProxyHook, sendBurstDtmf) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        const std::string dtmf;
        int32_t on;
        int32_t off;
    } testVector = {
        "1", 0, 0,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendBurstDtmf(int32_t serial, const hidl_string& dtmf, int32_t on, int32_t off) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(dtmf == testVector.dtmf);
            EXPECT_TRUE(on == testVector.on);
            EXPECT_TRUE(off == testVector.off);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->sendBurstDtmf(s, testVector.dtmf, testVector.on, testVector.off);
}

TEST(RadioVoiceProxyHook, sendCdmaFeatureCode) {
    static int32_t s = INTrand(true);
    static std::string testVector = "";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendCDMAFeatureCode(int32_t serial, const hidl_string& featureCode) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(featureCode == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->sendCdmaFeatureCode(s, testVector);
}

TEST(RadioVoiceProxyHook, sendDtmf) {
    static int32_t s = INTrand(true);
    static std::string testVector = "1";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendDtmf(int32_t serial, const hidl_string& str) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(str == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->sendDtmf(s, testVector);
}

TEST(RadioVoiceProxyHook, sendUssd) {
    static int32_t s = INTrand(true);
    static std::string testVector = "1";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendUssd(int32_t serial, const hidl_string& ussd) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(ussd == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->sendUssd(s, testVector);
}

TEST(RadioVoiceProxyHook, separateConnection) {
    static int32_t s = INTrand(true);
    static int32_t testVector = 1;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> separateConnection(int32_t serial, int32_t gsmIndex) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(gsmIndex == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->separateConnection(s, testVector);
}

TEST(RadioVoiceProxyHook, setCallForward) {
    static int32_t s = INTrand(true);
    static aidl_voice::CallForwardInfo testVector = {
        aidl_voice::CallForwardInfo::STATUS_ENABLE, 0, 1, 145, "12345", 0,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setCallForward(int32_t serial, const V1_0::CallForwardInfo& callInfo) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int32_t>(callInfo.status) == testVector.status);
            EXPECT_TRUE(callInfo.reason == testVector.reason);
            EXPECT_TRUE(callInfo.serviceClass == testVector.serviceClass);
            EXPECT_TRUE(callInfo.toa == testVector.toa);
            EXPECT_TRUE(callInfo.number == testVector.number);
            EXPECT_TRUE(callInfo.timeSeconds == testVector.timeSeconds);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->setCallForward(s, testVector);
}

TEST(RadioVoiceProxyHook, setCallWaiting) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        bool enable;
        int32_t serviceClass;
    } testVector = {
        true, 1,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setCallWaiting(int32_t serial, bool enable, int32_t serviceClass) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(enable == testVector.enable);
            EXPECT_TRUE(serviceClass == testVector.serviceClass);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->setCallWaiting(s, testVector.enable, testVector.serviceClass);
}

TEST(RadioVoiceProxyHook, setClir) {
    static int32_t s = INTrand(true);
    static int32_t testVector = 0;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setClir(int32_t serial, int32_t status) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(status == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->setClir(s, testVector);
}

TEST(RadioVoiceProxyHook, setMute) {
    static int32_t s = INTrand(true);
    static bool testVector = false;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setMute(int32_t serial, bool enable) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(enable == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->setMute(s, testVector);
}

TEST(RadioVoiceProxyHook, setPreferredVoicePrivacy) {
    static int32_t s = INTrand(true);
    static bool testVector = false;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setPreferredVoicePrivacy(int32_t serial, bool enable) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(enable == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->setPreferredVoicePrivacy(s, testVector);
}

TEST(RadioVoiceProxyHook, setTtyMode) {
    static int32_t s = INTrand(true);
    static aidl_voice::TtyMode testVector = aidl_voice::TtyMode::OFF;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setTTYMode(int32_t serial, V1_0::TtyMode mode) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int32_t>(mode) == static_cast<int32_t>(testVector));
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->setTtyMode(s, testVector);
}

TEST(RadioVoiceProxyHook, startDtmf) {
    static int32_t s = INTrand(true);
    static std::string testVector = "0";
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> startDtmf(int32_t serial, const hidl_string& str) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(str == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->startDtmf(s, testVector);
}

TEST(RadioVoiceProxyHook, stopDtmf) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> stopDtmf(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->stopDtmf(s);
}

TEST(RadioVoiceProxyHook, switchWaitingOrHoldingAndActive) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> switchWaitingOrHoldingAndActive(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioVoiceProxy> test = sp<TestRadio>::make();
    test->switchWaitingOrHoldingAndActive(s);
}
