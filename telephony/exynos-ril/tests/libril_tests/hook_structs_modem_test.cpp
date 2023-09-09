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
#include "modem/hook/structs.h"
#include "commonStructs.h"
#include "collections.h"
#include <aidl/android/hardware/radio/RadioTechnology.h>

using namespace ::android::hardware::radio;
using namespace ::aidl::android::hardware::radio;
namespace aidl_modem = ::aidl::android::hardware::radio::modem;
using ::android::hardware::hidl_array;

TEST(HookStructs, NvWriteItem_toHidl) {
    aidl_modem::NvWriteItem testVector = {
        aidl_modem::NvItem::CDMA_MEID, "12345",
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.itemId == V1_0::NvItem{testVector.itemId});
    EXPECT_TRUE(test.value == testVector.value);
}

TEST(HookStructs, RadioCapability_toAidl) {
    V1_0::RadioCapability testVector = {
        1, V1_0::RadioCapabilityPhase::START, 0x1234F,
        "modem_uuid0", V1_0::RadioCapabilityStatus::SUCCESS,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.session == testVector.session);
    EXPECT_TRUE(test.phase == static_cast<int32_t>(testVector.phase));
    EXPECT_TRUE(test.raf == static_cast<int32_t>(testVector.raf));
    EXPECT_TRUE(test.logicalModemUuid == testVector.logicalModemUuid);
    EXPECT_TRUE(test.status == static_cast<int32_t>(testVector.status));
}

TEST(HookStructs, RadioCapability_toHidl) {
    aidl_modem::RadioCapability testVector = {
        2, 3, 0x1234F,
        "modem_uuid0", 1,
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.session == testVector.session);
    EXPECT_TRUE(static_cast<int32_t>(test.phase) == testVector.phase);
    EXPECT_TRUE(static_cast<int32_t>(test.raf) == testVector.raf);
    EXPECT_TRUE(test.logicalModemUuid == testVector.logicalModemUuid);
    EXPECT_TRUE(static_cast<int32_t>(test.status) == testVector.status);
}

TEST(HookStructs, HardwareConfig_toAidl) {
    V1_0::HardwareConfig testVector = {
        V1_0::HardwareConfigType::MODEM,
        "uuid-1234", V1_0::HardwareConfigState::ENABLED,
        {{0, 1, 2, 3, 4}},
        {{"modem_uuid0"}},
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.type == static_cast<int32_t>(testVector.type));
    EXPECT_TRUE(test.uuid == testVector.uuid);
    EXPECT_TRUE(test.state == static_cast<int32_t>(testVector.state));
    EXPECT_TRUE(test.modem == compat::toAidl(testVector.modem));
    EXPECT_TRUE(test.sim == compat::toAidl(testVector.sim));
}

TEST(HookStructs, HardwareConfigModem_toAidl) {
    V1_0::HardwareConfigModem testVector = {
        0, 1, 2, 3, 4,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.rilModel == testVector.rilModel);
    EXPECT_TRUE(test.rat == static_cast<RadioTechnology>(testVector.rat));
    EXPECT_TRUE(test.maxVoiceCalls == testVector.maxVoice);
    EXPECT_TRUE(test.maxDataCalls == testVector.maxData);
    EXPECT_TRUE(test.maxStandby == testVector.maxStandby);
}

TEST(HookStructs, HardwareConfigSim_toAidl) {
    V1_0::HardwareConfigSim testVector = {
        "modem_uuid0"
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.modemUuid == testVector.modemUuid);
}

TEST(HookStructs, ActivityStatsInfo_toAidl) {
    uint32_t txmModetimeMs[] = {1, 2, 3, 4, 5};
    V1_0::ActivityStatsInfo testVector = {
        1000, 2000, txmModetimeMs, 4000,
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.sleepModeTimeMs == static_cast<int32_t>(testVector.sleepModeTimeMs));
    EXPECT_TRUE(test.idleModeTimeMs == static_cast<int32_t>(testVector.idleModeTimeMs));

    std::vector<aidl_modem::ActivityStatsTechSpecificInfo> techSpecificInfo = {
        {
            .rat = AccessNetwork(AccessNetwork::UNKNOWN),
            .frequencyRange = static_cast<int32_t>(
                    aidl_modem::ActivityStatsTechSpecificInfo::FREQUENCY_RANGE_UNKNOWN),
            .txmModetimeMs = compat::toAidl(testVector.txmModetimeMs),
            .rxModeTimeMs = static_cast<int32_t>(testVector.rxModeTimeMs),
        },
    };
    EXPECT_TRUE(test.techSpecificInfo == techSpecificInfo);
}