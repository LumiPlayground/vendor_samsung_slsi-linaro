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
#include "radioconfigproxy.h"

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_config = ::aidl::android::hardware::radio::config;

using namespace android::hardware::radio;
using ::android::hardware::Return;
using android::sp;

TEST(RadioConfigProxyHook, getNumOfLiveModems) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioConfigBase_1_1 {
        TestChecker checker;
        Return<void> getModemsConfig(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioConfigProxy> test = {static_cast<sp<V1_6::IRadio>>(nullptr), sp<TestRadio>::make()};
    test->getNumOfLiveModems(s);
}

TEST(RadioConfigProxyHook, getPhoneCapability) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioConfigBase_1_1 {
        TestChecker checker;
        Return<void> getPhoneCapability(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioConfigProxy> test = {static_cast<sp<V1_6::IRadio>>(nullptr), sp<TestRadio>::make()};
    test->getPhoneCapability(s);
}

TEST(RadioConfigProxyHook, setNumOfLiveModems) {
    static int32_t s = INTrand(true);
    static int8_t testVector = 2;
    struct TestRadio : public TestRadioConfigBase_1_1 {
        TestChecker checker;
        Return<void> setModemsConfig(int32_t serial,
                const config::V1_1::ModemsConfig& modemsConfig) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int8_t>(modemsConfig.numOfLiveModems) == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioConfigProxy> test = {static_cast<sp<V1_6::IRadio>>(nullptr), sp<TestRadio>::make()};
    test->setNumOfLiveModems(s, testVector);
}

TEST(RadioConfigProxyHook, setPreferredDataModem) {
    static int32_t s = INTrand(true);
    static int8_t testVector = 1;
    struct TestRadio : public TestRadioConfigBase_1_1 {
        TestChecker checker;
        Return<void> setPreferredDataModem(int32_t serial, uint8_t modemId) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(static_cast<int8_t>(modemId) == testVector);
            return {};
        }
    };
    RadioProxyTestSetup<RadioConfigProxy> test = {static_cast<sp<V1_6::IRadio>>(nullptr), sp<TestRadio>::make()};
    test->setPreferredDataModem(s, testVector);
}
