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
 * vendor_test.cpp
 *
 *  Created on: 2021. 7. 14.
 */
#include <gtest/gtest.h>
#include <librilutils/vendor.h>

TEST(Vendor, getCurrentHalVersionCode) {
    int halVer = vendor::radio::getCurrentHalVersionCode();
    EXPECT_TRUE(halVer == HAL_VERSION_CODE(1, 5) || halVer == HAL_VERSION_CODE(1, 6) || halVer == HAL_VERSION_CODE(2, 0));
}

TEST(Vendor, getMaxHalVersionCode) {
    int halVer = vendor::radio::getMaxHalVersionCode();
    EXPECT_TRUE(halVer == HAL_VERSION_CODE(1, 5) || halVer == HAL_VERSION_CODE(1, 6) || halVer == HAL_VERSION_CODE(2, 0));
}

TEST(Vendor, halVersionCodeToString) {
    int halVer = vendor::radio::getCurrentHalVersionCode();
    std::string versionCode = vendor::radio::halVersionCodeToString(halVer);
    EXPECT_TRUE(versionCode.length() > 0);
}
