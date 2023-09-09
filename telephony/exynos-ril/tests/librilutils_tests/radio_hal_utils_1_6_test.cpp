/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * radio_hal_utils_test.cpp
 *
 *  Created on: 2021. 7. 14.
 */

#include <gtest/gtest.h>

#include <android/hardware/radio/1.6/types.h>

#include <librilutils/radio_hal_utils_1_6.h>
#include <slsi/radio_v1_6.h>

#include <string>
#include <vector>

using namespace android::hardware::radio;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;

TEST(Radio2HalUtils_1_6, convertRilPhysicalChannelConfigToHal) {
    V1_6::PhysicalChannelConfig pcc {};
    RIL_PhysicalChannelConfig_V1_6 testVector = {

    };

    // an invalid parameter test
    Radio2HalUtils::convertRilPhysicalChannelConfigToHal(nullptr, 0, pcc);
    Radio2HalUtils::convertRilPhysicalChannelConfigToHal(&testVector, sizeof(testVector) + 1, pcc);

    // TODO
}

TEST(Radio2HalUtils_1_6, convertVector_hidl_string) {
    hidl_vec<hidl_string> out;
    std::vector<std::string> testVector;
    testVector.push_back("test1");
    testVector.push_back("test2");
    testVector.push_back("test3");
    testVector.push_back("test4");
    testVector.push_back("test5");

    Radio2HalUtils::convertVector(out, testVector);
    EXPECT_EQ(out.size(), testVector.size());
    for (size_t i = 0; i < testVector.size(); i++) {
        EXPECT_TRUE(out[i] == hidl_string(testVector[i]));
    }
}

TEST(Radio2HalUtils_1_6, convertRilRegStateToHal) {
    V1_6::RegStateResult result;
    RIL_RegStateResult_V1_6 testVector {};

    // an invalid parameter test
    Radio2HalUtils::convertRilRegStateToHal(nullptr, sizeof(testVector), result);
    Radio2HalUtils::convertRilRegStateToHal(&testVector, sizeof(testVector) + 1, result);

    // TODO
}

TEST(Radio2HalUtils_1_6, convertRilSignalStrengthToHal) {
    V1_6::SignalStrength ss {};
    RIL_SignalStrength_V1_6 testVector {};

    // an invalid parameter test
    Radio2HalUtils::convertRilSignalStrengthToHal(nullptr, sizeof(testVector), ss);
    Radio2HalUtils::convertRilSignalStrengthToHal(&testVector, sizeof(testVector) + 1, ss);

    // TODO
}

TEST(Radio2HalUtils_1_6, convertRilCellInfoLteToHal) {
    V1_6::CellInfoLte cellinfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoLteToHal(nullptr, cellinfo);

    // TODO
}

TEST(Radio2HalUtils_1_6, convertRilCellInfoNrToHal) {
    V1_6::CellInfoNr cellinfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoNrToHal(nullptr, cellinfo);

    // TODO
}

TEST(Radio2HalUtils_1_6, convertRilCellInfoListToHal) {
    hidl_vec<V1_6::CellInfo> cellinfos;
    RIL_CellInfo_V1_6 testVector[] = {
        {},
        {},
        {},
    };

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoListToHal(nullptr, sizeof(testVector), cellinfos);

    // TODO
}

TEST(Radio2HalUtils_1_6, convertLinkAddress) {
    hidl_vec<V1_5::LinkAddress> linkAddresses;

    // an invalid parameter test
    Radio2HalUtils::convertLinkAddress(linkAddresses, nullptr);

    // TODO
}

TEST(Radio2HalUtils_1_6, convertRilDataCallListToHal) {
    hidl_vec<V1_6::SetupDataCallResult> dcResultList;
    RIL_SetupDataCallResult_V1_6 testVector[] = {
        {},
        {},
        {},
    };

    // an invalid parameter test
    Radio2HalUtils::convertRilDataCallListToHal(nullptr, sizeof(testVector), dcResultList);

    // TODO
}

TEST(Radio2HalUtils_1_6, convertRilCallToHal) {
    V1_6::Call call {};
    RIL_Call_V1_6 testVector {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCallToHal(nullptr, sizeof(testVector), call);
    Radio2HalUtils::convertRilCallToHal(&testVector, sizeof(testVector) + 1, call);

    // TODO
}
