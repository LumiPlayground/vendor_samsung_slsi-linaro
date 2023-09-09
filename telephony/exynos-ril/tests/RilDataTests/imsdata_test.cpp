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

#include <gtest/gtest.h>
#include "imsdata.h"
#include "librilutils/textutils.h"

TEST(ImsData, UpdateImsRegistrationInfoData) {
    RilImsV2_1::RIL_UpdateImsRegistration testVector = {
        RilImsV2_1::RIL_ImsRegState::NOT_REGISTERED,
        2, ACCESS_NETWORK_GERAN,
        RilImsV2_1::RIL_SuggestedAction::TRIGGER_PLMN_BLOCK_WITH_TIMEOUT};

    UpdateImsRegistrationInfoData test(RIL_REQUEST_OEM_IMS_UPDATE_REGISTRATION_INFO, 0);
    EXPECT_EQ(test.encode(nullptr, 0), -1);
    EXPECT_EQ(test.encode(nullptr, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode(reinterpret_cast<char *>(&testVector), sizeof(testVector)), 0);
    EXPECT_EQ(test.GetRegState(), static_cast<int32_t>(testVector.regState));
    EXPECT_EQ(test.GetCapabilities(), static_cast<int32_t>(testVector.capabilities));
    EXPECT_EQ(test.GetAccessNetwork(), static_cast<int32_t>(testVector.accessNetworkType));
    EXPECT_EQ(test.GetSuggestedAction(), static_cast<int32_t>(testVector.suggestedAction));
}

TEST(ImsData, UpdateImsRegistrationInfoData_oem) {
    int testVector[4] = { 0, 1, 2, 3 };

    UpdateImsRegistrationInfoData test(RIL_REQUEST_OEM_IMS_UPDATE_REGISTRATION_INFO, 0);
    EXPECT_EQ(test.encode(nullptr, 0), -1);
    EXPECT_EQ(test.encode(nullptr, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode(reinterpret_cast<char *>(&testVector), sizeof(testVector)), 0);
    EXPECT_EQ(test.GetRegState(), static_cast<int32_t>(testVector[0]));
    EXPECT_EQ(test.GetCapabilities(), static_cast<int32_t>(testVector[1]));
    EXPECT_EQ(test.GetAccessNetwork(), static_cast<int32_t>(testVector[2]));
    EXPECT_EQ(test.GetSuggestedAction(), static_cast<int32_t>(testVector[3]));
}

TEST(ImsData, StartImsTrafficData) {
    std::initializer_list<RilImsV2_1::RIL_StartImsTraffic> testVector = {
        {0, RilImsV2_1::RIL_ImsTrafficType::EMERGENCY, static_cast<int>(ACCESS_NETWORK_UNKNOWN), 0},
        {1, RilImsV2_1::RIL_ImsTrafficType::EMERGENCY_SMS, static_cast<int>(ACCESS_NETWORK_GERAN), 1},
        {1, RilImsV2_1::RIL_ImsTrafficType::VOICE, static_cast<int>(ACCESS_NETWORK_EUTRAN), 0},
    };

    for (auto t : testVector) {
        StartImsTrafficData test(RIL_REQUEST_OEM_IMS_START_IMS_TRAFFIC, 0);
        EXPECT_EQ(test.encode(nullptr, 0), -1);
        EXPECT_EQ(test.encode(nullptr, sizeof(t)), -1);
        EXPECT_EQ(test.encode(reinterpret_cast<char *>(&t), sizeof(t)), 0);
        EXPECT_EQ(test.GetToken(), static_cast<int32_t>(t.token));
        EXPECT_EQ(test.GetImsTrafficType(), static_cast<int32_t>(t.imsTrafficType));
        EXPECT_EQ(test.GetAccessNetwork(), static_cast<int32_t>(t.accessNetworks));
        EXPECT_EQ(test.GetDirection(), static_cast<int32_t>(t.direction));
    }
}

TEST(ImsData, CreateTriggerEpsFallbackData) {
    std::initializer_list<int32_t> testVector = {1, 2};

    for (auto t : testVector) {
        CreateTriggerEpsFallbackData test(RIL_REQUEST_OEM_IMS_TRIGGER_EPS_FALLBACK, 0);
        EXPECT_EQ(test.encode(nullptr, 0), -1);
        EXPECT_EQ(test.encode(nullptr, sizeof(testVector)), -1);
        EXPECT_EQ(test.encode(reinterpret_cast<char *>(&t), sizeof(t)), 0);
        EXPECT_EQ(test.GetReason(), static_cast<int32_t>(t));
    }
}

TEST(ImsData, CreateTriggerEpsFallbackData_oem) {
    std::initializer_list<uint8_t> testVector = {1, 2};

    for (auto t : testVector) {
        CreateTriggerEpsFallbackData test(RIL_REQUEST_OEM_IMS_TRIGGER_EPS_FALLBACK, 0);
        EXPECT_EQ(test.encode(nullptr, 0), -1);
        EXPECT_EQ(test.encode(nullptr, sizeof(testVector)), -1);
        EXPECT_EQ(test.encode(reinterpret_cast<char *>(&t), sizeof(t)), 0);
        EXPECT_EQ(test.GetReason(), static_cast<int32_t>(t));
    }
}


TEST(ImsData, SrvccCallInfoData) {
    std::vector<RilImsV2_1::RIL_SrvccCall> testVector = {
        {0, RilImsV2_1::RIL_CallType::NORMAL, 0, RilImsV2_1::RIL_CallSubState::NONE,
            RilImsV2_1::RIL_ToneType::LOCAL, true, false, "01012341234", 0, "AAA", 0},
        {1, RilImsV2_1::RIL_CallType::EMERGENCY, 1, RilImsV2_1::RIL_CallSubState::PREALERTING,
            RilImsV2_1::RIL_ToneType::NETWORK, false, true, "01043214321", 1, "BBB", 1},
        {2, RilImsV2_1::RIL_CallType::NORMAL, 2, RilImsV2_1::RIL_CallSubState::NONE,
            RilImsV2_1::RIL_ToneType::NONE, true, true, "01011112222", 2, "CCC", 2},
        {3, RilImsV2_1::RIL_CallType::EMERGENCY, 3, RilImsV2_1::RIL_CallSubState::PREALERTING,
            RilImsV2_1::RIL_ToneType::NONE, false, false, "01033334444", 3, "DDD", 3},
    };

    // This is for RIL_REQUEST_SET_SRVCC_CALL_INFO.
    SrvccCallInfoData test(RIL_REQUEST_OEM_IMS_SET_SRVCC_CALL_INFO, 0);
    EXPECT_EQ(test.encode(nullptr, 0), -1);
    EXPECT_EQ(test.encode(nullptr, sizeof(RilImsV2_1::RIL_SrvccCall) * testVector.size()), -1);
    EXPECT_EQ(test.encode(reinterpret_cast<char *>(testVector.data()), sizeof(RilImsV2_1::RIL_SrvccCall) * testVector.size()), 0);
    const std::vector<RilImsV2_1::RIL_SrvccCall> result = test.GetSrvccCallInfo();
    int idx = 0;
    EXPECT_EQ(static_cast<int>(testVector.size()), static_cast<int>(result.size()));
    for (auto iter = result.begin(); iter != result.end(); iter++) {
        EXPECT_EQ(iter->index, testVector[idx].index);
        EXPECT_EQ(iter->callType, testVector[idx].callType);
        EXPECT_EQ(iter->callState, testVector[idx].callState);
        EXPECT_EQ(iter->callSubstate, testVector[idx].callSubstate);
        EXPECT_EQ(iter->ringbackToneType, testVector[idx].ringbackToneType);
        EXPECT_EQ(iter->isMpty, testVector[idx].isMpty);
        EXPECT_EQ(iter->isMT, testVector[idx].isMT);
        EXPECT_EQ(iter->number, testVector[idx].number);
        EXPECT_EQ(iter->numPresentation, testVector[idx].numPresentation);
        EXPECT_EQ(iter->name, testVector[idx].name);
        EXPECT_EQ(iter->namePresentation, testVector[idx].namePresentation);
        idx++;
    }
}

TEST(ImsData, OemSrvccCallInfoData) {
    RilImsV2_1::RIL_SrvccCallListOem testVector = {
        sizeof(testVector.callList) / sizeof(testVector.callList[0]),
        { { 0, static_cast<uint8_t>(0), static_cast<uint8_t>(0), static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), true, false, static_cast<uint8_t>(11), "01012341234",
            0, static_cast<uint8_t>(3), "AAA", 0 },
        { 1, static_cast<uint8_t>(1), static_cast<uint8_t>(1), static_cast<uint8_t>(1),
            static_cast<uint8_t>(1), false, true, static_cast<uint8_t>(11), "01043214321",
            1, static_cast<uint8_t>(3), "BBB", 1 },
        { 2, static_cast<uint8_t>(0), static_cast<uint8_t>(2), static_cast<uint8_t>(0),
            static_cast<uint8_t>(2), true, true, static_cast<uint8_t>(11), "01011112222",
            2, static_cast<uint8_t>(3), "CCC", 2 },
        { 3, static_cast<uint8_t>(1), static_cast<uint8_t>(3), static_cast<uint8_t>(1),
            static_cast<uint8_t>(0), false, false, static_cast<uint8_t>(11), "01033334444",
            3, static_cast<uint8_t>(3), "DDD", 3 }, }
    };
    int32_t size = sizeof(testVector.callList) / sizeof(testVector.callList[0]);

    OemSrvccCallInfoData test(RIL_REQUEST_OEM_IMS_SET_SRVCC_CALL_INFO, 0);
    EXPECT_EQ(test.encode(nullptr, 0), -1);
    EXPECT_EQ(test.encode(nullptr, sizeof(RilImsV2_1::RIL_SrvccCallListOem::callListNum) +
        (sizeof(RilImsV2_1::RIL_OemSrvccCall) * size)), -1);
    EXPECT_EQ(test.encode(reinterpret_cast<char *>(&testVector),
        sizeof(RilImsV2_1::RIL_SrvccCallListOem::callListNum) +
        (sizeof(RilImsV2_1::RIL_OemSrvccCall) * size)), 0);
    const std::vector<RilImsV2_1::RIL_SrvccCall> result = test.GetSrvccCallInfo();
    int idx = 0;
    EXPECT_EQ(testVector.callListNum, static_cast<int32_t>(result.size()));
    for (auto iter = result.begin(); iter != result.end(); iter++) {
        EXPECT_EQ(iter->index, testVector.callList[idx].index);
        EXPECT_EQ(iter->callType, static_cast<RilImsV2_1::RIL_CallType>(testVector.callList[idx].callType));
        EXPECT_EQ(iter->callState, static_cast<int32_t>(testVector.callList[idx].callState));
        EXPECT_EQ(iter->callSubstate, static_cast<RilImsV2_1::RIL_CallSubState>(testVector.callList[idx].callSubstate));
        EXPECT_EQ(iter->ringbackToneType, static_cast<RilImsV2_1::RIL_ToneType>(testVector.callList[idx].ringbackToneType));
        EXPECT_EQ(iter->isMpty, testVector.callList[idx].isMpty);
        EXPECT_EQ(iter->isMT, testVector.callList[idx].isMT);
        EXPECT_EQ(iter->number, std::string(testVector.callList[idx].number));
        EXPECT_EQ(iter->numPresentation, testVector.callList[idx].numPresentation);
        EXPECT_EQ(iter->name, std::string(testVector.callList[idx].name));
        EXPECT_EQ(iter->namePresentation, testVector.callList[idx].namePresentation);
        idx++;
    }
}