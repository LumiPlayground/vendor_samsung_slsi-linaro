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
#include "dmd_test_util.h"
#include "sdmdoc.h"
#include "sdmdocenv.h"
#include "systemproperty.h"

TEST(SdmDocEnv, getBaseDirDefault) {
    std::string test = SdmDocEnv::getBaseDirDefault();
    EXPECT_TRUE(test.compare("/data/vendor/slog/") == 0);
}

TEST(SdmDocEnv, getBaseDir) {
    std::string ans = SdmDocEnv::getBaseDirDefault();
    std::string test = SdmDocEnv::getBaseDir();
    EXPECT_TRUE(test.compare(ans) == 0);

    const char *key = "vendor.sys.exynos.slog.path";
    ans = "/data/vendor/slog";
    SystemProperty::Set(key, ans);
    // "/" must be appended
    ans += "/";
    test = SdmDocEnv::getBaseDir();
    EXPECT_TRUE(test.compare(ans) == 0);

    ans = "/data/vendor/local/tmp/";
    SystemProperty::Set(key, ans);
    test = SdmDocEnv::getBaseDir();
    EXPECT_TRUE(test.compare(ans) == 0);

    SystemProperty::Set(key, "");
    ans = SdmDocEnv::getBaseDirDefault();
    test = SdmDocEnv::getBaseDir();
    EXPECT_TRUE(test.compare(ans) == 0);
}

TEST(SdmDocEnv, getSdmType) {
    const char *key = "persist.vendor.config.sdm_type2";
    SystemProperty::Set(key, "");
    EXPECT_EQ(SdmDocEnv::getSdmType(), SDM_TYPE1);

    SystemProperty::Set(key, "1");
    EXPECT_EQ(SdmDocEnv::getSdmType(), SDM_TYPE2);

    SystemProperty::Set(key, "0");
    EXPECT_EQ(SdmDocEnv::getSdmType(), SDM_TYPE1);
}

TEST(SdmDocEnv, SdmTypeXExt) {
    EXPECT_TRUE(SdmDocEnv::getSdmType1Ext().compare(".sdm") == 0);
    EXPECT_TRUE(SdmDocEnv::getSdmType2Ext().compare(".sdm2") == 0);
}

TEST(SdmDocEnv, MaxDocSize) {
    const size_t defSize = (100*1024*1024);
    const size_t minSize = (2*1024*1024);
    const size_t maxSize = (200*1024*1024);

    SdmDocEnv::setMaxDocSize(minSize - 1);
    EXPECT_EQ(SdmDocEnv::getMaxDocSize(), minSize);

    SdmDocEnv::setMaxDocSize(maxSize + 1);
    EXPECT_EQ(SdmDocEnv::getMaxDocSize(), maxSize);

    SdmDocEnv::setMaxDocSize(defSize + 15);
    EXPECT_EQ(SdmDocEnv::getMaxDocSize(), defSize + 15);

    SdmDocEnv::setMaxDocSize(defSize);
    EXPECT_EQ(SdmDocEnv::getMaxDocSize(), defSize);
}

TEST(SdmDocEnv, isZipArachiveRequiredByConfig) {
    const char *key = "persist.vendor.sys.dm.zip";
    SystemProperty::Set(key, "1");
    EXPECT_TRUE(SdmDocEnv::isZipArachiveRequiredByConfig());

    SystemProperty::Set(key, "0");
    EXPECT_FALSE(SdmDocEnv::isZipArachiveRequiredByConfig());

    SystemProperty::Set(key, "");
    EXPECT_TRUE(SdmDocEnv::isZipArachiveRequiredByConfig());
}

TEST(SdmDocEnv, isZipArachiveRequired) {
    const char *key = "persist.vendor.sys.dm.zip";
    const char *type1 = "test.sdm";
    const char *type2 = "test.sdm2";

    SystemProperty::Set(key, "1");
    EXPECT_TRUE(SdmDocEnv::isZipArachiveRequired(type1));
    EXPECT_FALSE(SdmDocEnv::isZipArachiveRequired(type2));

    SystemProperty::Set(key, "0");
    EXPECT_FALSE(SdmDocEnv::isZipArachiveRequired(type1));
    EXPECT_FALSE(SdmDocEnv::isZipArachiveRequired(type2));


    SystemProperty::Set(key, "");
    EXPECT_TRUE(SdmDocEnv::isZipArachiveRequired(type1));
    EXPECT_FALSE(SdmDocEnv::isZipArachiveRequired(type2));
}

TEST(SdmDocEnv, getFilenamePrefix) {
    std::initializer_list<std::string> testVector = {
        "sdm_prefix",
        "sbuff",
        "sbuff_123456789012345678901234567890",
    };

    const std::string key = "vendor.sys.exynos.slog.prefix";
    for (auto& t : testVector) {
        SystemProperty::Set(key, t);
        EXPECT_TRUE(SdmDocEnv::getFilenamePrefix() == t.substr(0, 32));
    }
    SystemProperty::Set(key, "");
}

TEST(SdmDocEnv, setFilenamePrefix) {
    std::initializer_list<std::string> testVector = {
        "sdm_prefix",
        "sbuff",
        "sbuff_123456789012345678901234567890",
    };

    const std::string key = "vendor.sys.exynos.slog.prefix";
    for (auto& t : testVector) {
        SdmDocEnv::setFilenamePrefix(t);
        EXPECT_TRUE(SystemProperty::Get(key, "") == t.substr(0, 32));
    }
    SystemProperty::Set(key, "");
}