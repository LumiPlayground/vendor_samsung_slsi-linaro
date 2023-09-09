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
#include "radiomodemproxy.h"

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_modem = ::aidl::android::hardware::radio::modem;

using namespace android::hardware::radio;
using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using android::sp;

TEST(RadioModemProxyHook, enableModem) {
    static int32_t s = INTrand(true);
    static bool testVector = true;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> enableModem(int32_t serial, bool on) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(on == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->enableModem(s, testVector);
}

TEST(RadioModemProxyHook, getBasebandVersion) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getBasebandVersion(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->getBasebandVersion(s);
}

TEST(RadioModemProxyHook, getDeviceIdentity) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getDeviceIdentity(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->getDeviceIdentity(s);
}

TEST(RadioModemProxyHook, getHardwareConfig) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getHardwareConfig(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->getHardwareConfig(s);
}

TEST(RadioModemProxyHook, getModemStackStatus) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getModemStackStatus(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->getModemStackStatus(s);
}

TEST(RadioModemProxyHook, getRadioCapability) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getRadioCapability(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->getRadioCapability(s);
}

TEST(RadioModemProxyHook, nvReadItem) {
    static int32_t s = INTrand(true);
    static aidl_modem::NvItem testVector = aidl_modem::NvItem(3);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> nvReadItem(int32_t serial, V1_0::NvItem itemId) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int32_t>(itemId) == static_cast<int32_t>(testVector));
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->nvReadItem(s, testVector);
}

TEST(RadioModemProxyHook, nvResetConfig) {
    static int32_t s = INTrand(true);
    static aidl_modem::ResetNvType testVector = aidl_modem::ResetNvType(3);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> nvResetConfig(int32_t serial, V1_0::ResetNvType resetType) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int32_t>(resetType) == static_cast<int32_t>(testVector));
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->nvResetConfig(s, testVector);
}

TEST(RadioModemProxyHook, nvWriteCdmaPrl) {
    static int32_t s = INTrand(true);
    static std::vector<uint8_t> testVector = {1, 2, 3, 4, 5};
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> nvWriteCdmaPrl(int32_t serial, const hidl_vec<uint8_t>& prl) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(prl == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->nvWriteCdmaPrl(s, testVector);
}

TEST(RadioModemProxyHook, nvWriteItem) {
    static int32_t s = INTrand(true);
    static aidl_modem::NvWriteItem testVector = {
        aidl_modem::NvItem::CDMA_MEID, "12345",
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> nvWriteItem(int32_t serial, const V1_0::NvWriteItem& item) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(item.itemId == V1_0::NvItem{testVector.itemId});
            EXPECT_TRUE(item.value == testVector.value);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->nvWriteItem(s, testVector);
}

TEST(RadioModemProxyHook, requestShutdown) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> requestShutdown(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->requestShutdown(s);
}

TEST(RadioModemProxyHook, responseAcknowledgement) {
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> responseAcknowledgement() override {
            checker.checked();
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->responseAcknowledgement();
}

TEST(RadioModemProxyHook, sendDeviceState) {
    static int32_t s = INTrand(true);
    static struct TestArgs {
        aidl_modem::DeviceStateType deviceStateType;
        bool state;
    } testVector = {
        aidl_modem::DeviceStateType(3), true,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> sendDeviceState(int32_t serial, V1_0::DeviceStateType deviceStateType, bool state) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int32_t>(deviceStateType) == static_cast<int32_t>(testVector.deviceStateType));
            EXPECT_TRUE(state == testVector.state);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->sendDeviceState(s, testVector.deviceStateType, testVector.state);
}

TEST(RadioModemProxyHook, setRadioCapability) {
    static int32_t s = INTrand(true);
    static aidl_modem::RadioCapability testVector = {
        2, 3, 0x1234F,
        "modem_uuid0", 1,
    };
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setRadioCapability(int32_t serial, const V1_0::RadioCapability& rc) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(rc.session == testVector.session);
            EXPECT_TRUE(static_cast<int32_t>(rc.phase) == testVector.phase);
            EXPECT_TRUE(static_cast<int32_t>(rc.raf) == testVector.raf);
            EXPECT_TRUE(rc.logicalModemUuid == testVector.logicalModemUuid);
            EXPECT_TRUE(static_cast<int32_t>(rc.status) == testVector.status);
            return {};
        }
    };
    RadioProxyTestSetup<RadioModemProxy> test = sp<TestRadio>::make();
    test->setRadioCapability(s, testVector);
}