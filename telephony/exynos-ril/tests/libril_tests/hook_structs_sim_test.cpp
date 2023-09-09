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
#include "sim/hook/structs.h"
#include "commonStructs.h"
#include "collections.h"

using namespace ::android::hardware::radio;
using namespace ::aidl::android::hardware::radio;
namespace aidl_sim = ::aidl::android::hardware::radio::sim;
namespace aidl_config = ::aidl::android::hardware::radio::config;
using ::android::hardware::hidl_array;

TEST(HookStruct, IccIo_toHidl) {
    aidl_sim::IccIo testVector = {
        0xb0, 0x6F3B, "0x3F00", 0x00, 0x00, 0x00, "", "1234", "1234567890"
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.command == testVector.command);
    EXPECT_TRUE(test.fileId == testVector.fileId);
    EXPECT_TRUE(test.path == testVector.path);
    EXPECT_TRUE(test.p1 == testVector.p1);
    EXPECT_TRUE(test.p2 == testVector.p2);
    EXPECT_TRUE(test.p3 == testVector.p3);
    EXPECT_TRUE(test.data == testVector.data);
    EXPECT_TRUE(test.pin2 == testVector.pin2);
    EXPECT_TRUE(test.aid == testVector.aid);
}

TEST(HookStruct, SimApdu_toHidl) {
    aidl_sim::SimApdu testVector = {
        1, 2, 3, 4, 5, 6, "7890"
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.sessionId == testVector.sessionId);
    EXPECT_TRUE(test.cla == testVector.cla);
    EXPECT_TRUE(test.instruction == testVector.instruction);
    EXPECT_TRUE(test.p1 == testVector.p1);
    EXPECT_TRUE(test.p2 == testVector.p2);
    EXPECT_TRUE(test.p3 == testVector.p3);
    EXPECT_TRUE(test.data == testVector.data);
}

TEST(HookStruct, Carrier_toAidl) {
//    V1_0::CarrierMatchType matchType = RIL_CarrierMatchType::RIL_MATCH_ALL;
    V1_0::Carrier testVector = {
        "001", "01", V1_0::CarrierMatchType::ALL, "0A1B"
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.mcc == testVector.mcc);
    EXPECT_TRUE(test.mnc == testVector.mnc);
    EXPECT_TRUE(test.matchType == static_cast<int32_t>(testVector.matchType));
    EXPECT_TRUE(test.matchData == testVector.matchData);
}

TEST(HookStruct, Carrier_toHidl) {
    aidl_sim::Carrier testVector = {
        "001", "01", aidl_sim::Carrier::MATCH_TYPE_ALL, "0A1B"
    };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.mcc == testVector.mcc);
    EXPECT_TRUE(test.mnc == testVector.mnc);
    EXPECT_TRUE(test.matchType == V1_0::CarrierMatchType{testVector.matchType});
    EXPECT_TRUE(test.matchData == testVector.matchData);
}

TEST(HookStruct, CarrierRestrictions_toAidl) {
    static V1_0::CarrierRestrictions testVector = {
        {
            { "450", "05", V1_0::CarrierMatchType::SPN, "spn" },
            { "001", "01", V1_0::CarrierMatchType::IMSI_PREFIX, "imsi" },
            { "310", "480", V1_0::CarrierMatchType::ALL, "" }
        },
        {
            { "450", "05", V1_0::CarrierMatchType::SPN, "spn" },
            { "001", "01", V1_0::CarrierMatchType::IMSI_PREFIX, "imsi" },
            { "310", "480", V1_0::CarrierMatchType::ALL, "" }
        },
    };

    int i = 0;
    auto test = compat::toAidl(testVector);
    for (auto& carrier : test.allowedCarriers) {
        V1_0::Carrier& testCarrier = testVector.allowedCarriers[i++];
        EXPECT_TRUE(carrier.mcc == testCarrier.mcc);
        EXPECT_TRUE(carrier.mnc == testCarrier.mnc);
        EXPECT_TRUE(carrier.matchType == static_cast<int32_t>(testCarrier.matchType));
        EXPECT_TRUE(carrier.matchData == testCarrier.matchData);
    }

    i = 0;
    for (auto& carrier : test.excludedCarriers) {
        V1_0::Carrier& testCarrier = testVector.excludedCarriers[i++];
        EXPECT_TRUE(carrier.mcc == testCarrier.mcc);
        EXPECT_TRUE(carrier.mnc == testCarrier.mnc);
        EXPECT_TRUE(carrier.matchType == static_cast<int32_t>(testCarrier.matchType));
        EXPECT_TRUE(carrier.matchData == testCarrier.matchData);
    }
}

TEST(HookStruct, CarrierRestrictionsWithPriority_toAidl) {
    static V1_4::CarrierRestrictionsWithPriority testVector = {
        {
            { "450", "05", V1_0::CarrierMatchType::SPN, "spn" },
            { "001", "01", V1_0::CarrierMatchType::IMSI_PREFIX, "imsi" },
            { "310", "480", V1_0::CarrierMatchType::ALL, "" }
        },
        {
            { "450", "05", V1_0::CarrierMatchType::SPN, "spn" },
            { "001", "01", V1_0::CarrierMatchType::IMSI_PREFIX, "imsi" },
            { "310", "480", V1_0::CarrierMatchType::ALL, "" }
        },
        true,
    };

    int i = 0;
    auto test = compat::toAidl(testVector);
    for (auto& carrier : test.allowedCarriers) {
        V1_0::Carrier& testCarrier = testVector.allowedCarriers[i++];
        EXPECT_TRUE(carrier.mcc == testCarrier.mcc);
        EXPECT_TRUE(carrier.mnc == testCarrier.mnc);
        EXPECT_TRUE(carrier.matchType == static_cast<int32_t>(testCarrier.matchType));
        EXPECT_TRUE(carrier.matchData == testCarrier.matchData);
    }

    i = 0;
    for (auto& carrier : test.excludedCarriers) {
        V1_0::Carrier& testCarrier = testVector.excludedCarriers[i++];
        EXPECT_TRUE(carrier.mcc == testCarrier.mcc);
        EXPECT_TRUE(carrier.mnc == testCarrier.mnc);
        EXPECT_TRUE(carrier.matchType == static_cast<int32_t>(testCarrier.matchType));
        EXPECT_TRUE(carrier.matchData == testCarrier.matchData);
    }

    EXPECT_TRUE(test.allowedCarriersPrioritized == testVector.allowedCarriersPrioritized);
}

TEST(HookStruct, CarrierRestrictionsWithPriority_toHidl) {
    static aidl_sim::CarrierRestrictions testVector = {
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
        true,
    };

    int i = 0;
    auto test = compat::toHidl(testVector);
    for (auto& carrier : test.allowedCarriers) {
        aidl_sim::Carrier& testCarrier = testVector.allowedCarriers[i++];
        EXPECT_TRUE(carrier.mcc == testCarrier.mcc);
        EXPECT_TRUE(carrier.mnc == testCarrier.mnc);
        EXPECT_TRUE(carrier.matchType == V1_0::CarrierMatchType{testCarrier.matchType});
        EXPECT_TRUE(carrier.matchData == testCarrier.matchData);
    }

    i = 0;
    for (auto& carrier : test.excludedCarriers) {
        aidl_sim::Carrier& testCarrier = testVector.excludedCarriers[i++];
        EXPECT_TRUE(carrier.mcc == testCarrier.mcc);
        EXPECT_TRUE(carrier.mnc == testCarrier.mnc);
        EXPECT_TRUE(carrier.matchType == V1_0::CarrierMatchType{testCarrier.matchType});
        EXPECT_TRUE(carrier.matchData == testCarrier.matchData);
    }

    EXPECT_TRUE(test.allowedCarriersPrioritized == testVector.allowedCarriersPrioritized);
}

TEST(HookStruct, ImsiEncryptionInfo_toHidl) {
    aidl_sim::ImsiEncryptionInfo testVector = {
        "450", "05",
        {0x1, 0x2, 0x3, 0x4, 0x5},
        "keyIdentifier",
        (int64_t)7777,
        0
    };

    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.mcc == testVector.mcc);
    EXPECT_TRUE(test.mnc == testVector.mnc);
    EXPECT_TRUE(test.carrierKey == testVector.carrierKey);
    EXPECT_TRUE(test.keyIdentifier == testVector.keyIdentifier);
    EXPECT_TRUE(test.expirationTime == testVector.expirationTime);
}

TEST(HookStruct, ImsiEncryptionInfo_toHidl_1_6) {
    aidl_sim::ImsiEncryptionInfo testVector = {
        "450", "05",
        {0x1, 0x2, 0x3, 0x4, 0x5},
        "keyIdentifier",
        (int64_t)7777,
        0
    };

    auto test = compat::toHidl_1_6(testVector);
    EXPECT_TRUE(test.base.mcc == testVector.mcc);
    EXPECT_TRUE(test.base.mnc == testVector.mnc);
    EXPECT_TRUE(test.base.carrierKey == testVector.carrierKey);
    EXPECT_TRUE(test.base.keyIdentifier == testVector.keyIdentifier);
    EXPECT_TRUE(test.base.expirationTime == testVector.expirationTime);
    EXPECT_TRUE(test.keyType == V1_6::PublicKeyType{testVector.keyType});
}

TEST(HookStruct, SelectUiccSub_toHidl) {
    aidl_sim::SelectUiccSub testVector = {
        0, 0,
        aidl_sim::SelectUiccSub::SUBSCRIPTION_TYPE_1,
        aidl_sim::SelectUiccSub::ACT_STATUS_ACTIVATE
    };

    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.slot == testVector.slot);
    EXPECT_TRUE(test.appIndex == testVector.appIndex);
    EXPECT_TRUE(test.subType == V1_0::SubscriptionType{testVector.subType});
    EXPECT_TRUE(test.actStatus == V1_0::UiccSubActStatus{testVector.actStatus});
}

TEST(HookStruct, PhonebookRecordInfo_toAidl) {
    V1_6::PhonebookRecordInfo testVector = {
        1,
        "test",
        "0100000000",
        {"1@1.com"},
        {"0000"}
    };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.recordId == static_cast<int32_t>(testVector.recordId));
    EXPECT_TRUE(test.name == testVector.name);
    EXPECT_TRUE(test.number == testVector.number);
    EXPECT_TRUE(test.emails == compat::toAidl(testVector.emails));
    EXPECT_TRUE(test.additionalNumbers == compat::toAidl(testVector.additionalNumbers));
}

TEST(HookStruct, PhonebookRecordInfo_toHidl) {
    aidl_sim::PhonebookRecordInfo testVector = {
        1,
        "test",
        "0100000000",
        {"1@1.com"},
        {"0000"}
    };

    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.recordId == static_cast<uint32_t>(testVector.recordId));
    EXPECT_TRUE(test.name == testVector.name);
    EXPECT_TRUE(test.number == testVector.number);
    EXPECT_TRUE(test.emails == compat::toHidl(testVector.emails));
    EXPECT_TRUE(test.additionalNumbers == compat::toHidl(testVector.additionalNumbers));
}

TEST(HookStruct, SimRefreshResult_toAidl) {
    V1_0::SimRefreshResult testVector = {
        V1_0::SimRefreshType::SIM_FILE_UPDATE, 0x4f00, "aid"
    };

    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.type == static_cast<int32_t>(testVector.type));
    EXPECT_TRUE(test.efId == testVector.efId);
    EXPECT_TRUE(test.aid == testVector.aid);
}

TEST(HookStruct, AppStatus_toAidl) {
    V1_0::AppStatus testVector = {
        V1_0::AppType::USIM,
        V1_0::AppState::READY,
        V1_0::PersoSubstate::READY,
        "aidPtr",
        "appLabel",
        0,
        V1_0::PinState::DISABLED,
        V1_0::PinState::DISABLED
    };

    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.appType == static_cast<int32_t>(testVector.appType));
    EXPECT_TRUE(test.appState == static_cast<int32_t>(testVector.appState));
    EXPECT_TRUE(test.persoSubstate == aidl_sim::PersoSubstate{testVector.persoSubstate});
    EXPECT_TRUE(test.aidPtr == testVector.aidPtr);
    EXPECT_TRUE(test.appLabelPtr == testVector.appLabelPtr);
    EXPECT_TRUE(test.pin1Replaced == static_cast<int32_t>(testVector.pin1Replaced));
    EXPECT_TRUE(test.pin1 == aidl_sim::PinState{testVector.pin1});
    EXPECT_TRUE(test.pin2 == aidl_sim::PinState{testVector.pin2});
}

TEST(HookStruct, AppStatus_V1_5_toAidl) {
    V1_5::AppStatus testVector = {
        {
            V1_0::AppType::USIM,
            V1_0::AppState::READY,
            V1_0::PersoSubstate::READY,
            "aidPtr",
            "appLabel",
            0,
            V1_0::PinState::DISABLED,
            V1_0::PinState::DISABLED
        },
        V1_5::PersoSubstate::READY
    };

    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.appType == static_cast<int32_t>(testVector.base.appType));
    EXPECT_TRUE(test.appState == static_cast<int32_t>(testVector.base.appState));
    EXPECT_TRUE(test.persoSubstate == aidl_sim::PersoSubstate{testVector.base.persoSubstate});
    EXPECT_TRUE(test.aidPtr == testVector.base.aidPtr);
    EXPECT_TRUE(test.appLabelPtr == testVector.base.appLabelPtr);
    EXPECT_TRUE(test.pin1Replaced == static_cast<int32_t>(testVector.base.pin1Replaced));
    EXPECT_TRUE(test.pin1 == aidl_sim::PinState{testVector.base.pin1});
    EXPECT_TRUE(test.pin2 == aidl_sim::PinState{testVector.base.pin2});
    EXPECT_TRUE(test.persoSubstate == aidl_sim::PersoSubstate{testVector.persoSubstate});
}

TEST(HookStruct, PhonebookCapacity_toAidl) {
    V1_6::PhonebookCapacity testVector = {
        255, 10, 255, 10, 255, 10, 255, 255, 255, 255
    };

    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.maxAdnRecords == testVector.maxAdnRecords);
    EXPECT_TRUE(test.usedAdnRecords == testVector.usedAdnRecords);
    EXPECT_TRUE(test.maxEmailRecords == testVector.maxEmailRecords);
    EXPECT_TRUE(test.usedEmailRecords == testVector.usedEmailRecords);
    EXPECT_TRUE(test.maxAdditionalNumberRecords == testVector.maxAdditionalNumberRecords);
    EXPECT_TRUE(test.usedAdditionalNumberRecords == testVector.usedAdditionalNumberRecords);
    EXPECT_TRUE(test.maxNameLen == testVector.maxNameLen);
    EXPECT_TRUE(test.maxNumberLen == testVector.maxNumberLen);
    EXPECT_TRUE(test.maxEmailLen == testVector.maxEmailLen);
    EXPECT_TRUE(test.maxAdditionalNumberLen == testVector.maxAdditionalNumberLen);
}

TEST(HookStruct, IccIoResult_toAidl) {
    V1_0::IccIoResult testVector = {
        0x90, 0x00, "1234"
    };

    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.sw1 == testVector.sw1);
    EXPECT_TRUE(test.sw2 == testVector.sw2);
    EXPECT_TRUE(test.simResponse == testVector.simResponse);
}
