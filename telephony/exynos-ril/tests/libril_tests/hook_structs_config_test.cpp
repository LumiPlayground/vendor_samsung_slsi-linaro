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
#include "config/hook/structs.h"
#include "commonStructs.h"
#include "collections.h"

using namespace ::android::hardware::radio;
namespace aidl_config = ::aidl::android::hardware::radio::config;

TEST(HookStructs, SlotPortMapping_toHidl) {
    aidl_config::SlotPortMapping testVector = {0, 1};
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test == static_cast<uint32_t>(testVector.physicalSlotId));
}

TEST(HookStructs, SimSlotStatus_V1_0_toAidl) {
    config::V1_0::SimSlotStatus testVector = {
        V1_0::CardState::PRESENT, config::V1_0::SlotState::ACTIVE, "0123456789", 0, "777777777",
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.cardState == static_cast<int32_t>(testVector.cardState));
    EXPECT_TRUE(test.atr == testVector.atr);
    EXPECT_TRUE(test.eid == "");

    aidl_config::SimPortInfo portInfo = {
            .iccId = testVector.iccid,
            .logicalSlotId = static_cast<int32_t>(testVector.logicalSlotId),
            .portActive = testVector.slotState == config::V1_0::SlotState::ACTIVE,
    };
    EXPECT_TRUE(test.portInfo == std::vector<decltype(portInfo)>{portInfo});
}

TEST(HookStructs, SimSlotStatus_V1_2_toAidl) {
    config::V1_2::SimSlotStatus testVector = {
        {V1_0::CardState::PRESENT, config::V1_0::SlotState::ACTIVE, "0123456789", 0, "777777777"},
        "8888888"
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.cardState == static_cast<int32_t>(testVector.base.cardState));
    EXPECT_TRUE(test.atr == testVector.base.atr);
    EXPECT_TRUE(test.eid == testVector.eid);

    aidl_config::SimPortInfo portInfo = {
            .iccId = testVector.base.iccid,
            .logicalSlotId = static_cast<int32_t>(testVector.base.logicalSlotId),
            .portActive = testVector.base.slotState == config::V1_0::SlotState::ACTIVE,
    };
    EXPECT_TRUE(test.portInfo == std::vector<decltype(portInfo)>{portInfo});
}

TEST(HookStructs, ModemInfo_toAidl) {
    config::V1_1::ModemInfo testVector = { 0 };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test == testVector.modemId);
}

TEST(HookStructs, PhoneCapability_toAidl) {
    config::V1_1::PhoneCapability testVector = { 1, 1, true, {{}, {}} };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.maxActiveData == static_cast<int8_t>(testVector.maxActiveData));
    EXPECT_TRUE(test.maxActiveInternetData == static_cast<int8_t>(testVector.maxActiveInternetData));
    EXPECT_TRUE(test.isInternetLingeringSupported == testVector.isInternetLingeringSupported);
    EXPECT_TRUE(test.logicalModemIds == compat::toAidl(testVector.logicalModemList));
}