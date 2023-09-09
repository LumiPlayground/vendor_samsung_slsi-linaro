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
 *  Created on: 2021. 7. 5.
 */

#include <gtest/gtest.h>

#include <android/hardware/radio/1.5/types.h>

#include <librilutils/radio_hal_utils.h>
#include <slsi/radio_v1_5.h>

using namespace android::hardware::radio;
using ::android::hardware::hidl_string;

TEST(Radio2HalUtils, convertRilCsgInfoToHal_V1_5) {

    RIL_ClosedSubscriberGroupInfo testVector[] = {
        { true, "cgs12345", 1 },
        { true, "cgs12345", 0x7FFFFFF },
        { false, "cgs67890", 1 },
        { false, "cgs67890", 0x7FFFFFF },
        { false, "", INT_MAX },
    };
    size_t size = sizeof(testVector) / sizeof(testVector);

    for (size_t i = 0; i < size; i++) {
        V1_5::ClosedSubscriberGroupInfo halCsgInfo {};
        Radio2HalUtils::convertRilCsgInfoToHal(halCsgInfo, testVector[i]);
        EXPECT_EQ(halCsgInfo.csgIndication, testVector[i].csgIndication);
        EXPECT_TRUE(halCsgInfo.homeNodebName == hidl_string(testVector[i].homeNodebName));
        EXPECT_EQ(halCsgInfo.csgIdentity, testVector[i].csgIdentity);
    }
}

TEST(Radio2HalUtils, convertRilOptionalCsgInfoToHal_V1_5) {
    RIL_OptionalCsgInfo testVector[] = {
        {{ true, "cgs12345", 1 }},
        {{ true, "cgs12345", 0x7FFFFFF }},
        {{ false, "cgs67890", 1 }},
        {{ false, "cgs67890", 0x7FFFFFF }},
    };
    size_t size = sizeof(testVector) / sizeof(testVector);

    for (size_t i = 0; i < size; i++) {
        V1_5::OptionalCsgInfo halOptionalCsgInfo {};
        Radio2HalUtils::convertRilOptionalCsgInfoToHal(halOptionalCsgInfo, testVector[i]);

        EXPECT_TRUE(halOptionalCsgInfo.getDiscriminator() == V1_5::OptionalCsgInfo::hidl_discriminator::csgInfo);
        const V1_5::ClosedSubscriberGroupInfo& halCsgInfo = halOptionalCsgInfo.csgInfo();
        EXPECT_EQ(halCsgInfo.csgIndication, testVector[i].csgInfo.csgIndication);
        EXPECT_TRUE(halCsgInfo.homeNodebName == hidl_string(testVector[i].csgInfo.homeNodebName));
        EXPECT_EQ(halCsgInfo.csgIdentity, testVector[i].csgInfo.csgIdentity);
    }

    RIL_OptionalCsgInfo testVectorNoinit = {{ false, "", INT_MAX }};
    {
        V1_5::OptionalCsgInfo halOptionalCsgInfo {};
        Radio2HalUtils::convertRilOptionalCsgInfoToHal(halOptionalCsgInfo, testVectorNoinit);
        EXPECT_TRUE(halOptionalCsgInfo.getDiscriminator() == V1_5::OptionalCsgInfo::hidl_discriminator::noinit);
    }
}

TEST(Radio2HalUtils, convertRilCellIdentityToHal_V1_5_cell_info_type) {
    struct {
        uint32_t rilCellInfoType;
        V1_5::CellIdentity::hidl_discriminator halCellInfoType;
    } testVector[] = {
        { RIL_CELL_INFO_TYPE_NONE, V1_5::CellIdentity::hidl_discriminator::noinit },
        { RIL_CELL_INFO_TYPE_GSM, V1_5::CellIdentity::hidl_discriminator::gsm },
        { RIL_CELL_INFO_TYPE_WCDMA, V1_5::CellIdentity::hidl_discriminator::wcdma },
        { RIL_CELL_INFO_TYPE_TD_SCDMA, V1_5::CellIdentity::hidl_discriminator::tdscdma },
        { RIL_CELL_INFO_TYPE_CDMA, V1_5::CellIdentity::hidl_discriminator::cdma },
        { RIL_CELL_INFO_TYPE_LTE, V1_5::CellIdentity::hidl_discriminator::lte },
        { RIL_CELL_INFO_TYPE_GSM, V1_5::CellIdentity::hidl_discriminator::nr },
    };
    size_t size = sizeof(testVector) / sizeof(testVector);
    for (size_t i = 0; i < size; i++) {
        RIL_CellIdentity_V1_5 rilCellIdentity = { .cellInfoType = (RIL_CellInfoType)testVector[i].rilCellInfoType,  };
        V1_5::CellIdentity halCellIdentity = {};
        Radio2HalUtils::convertRilCellIdentityToHal(halCellIdentity, rilCellIdentity);
        EXPECT_TRUE(halCellIdentity.getDiscriminator() == testVector[i].halCellInfoType);
    }
}
