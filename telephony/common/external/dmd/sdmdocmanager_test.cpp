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
#include "sdmdocenv.h"
#include "sdmdocmanager.h"

TEST(SdmDocManager, getFilename) {
    struct TestItem {
        std::string prefix;
        uint64_t date;
        std::string filename;
    };

    std::initializer_list<TestItem> testVector = {
        { "modem", 19820219112233UL, "modem_19820219_112233" },
        { "radio", 20220920144627UL, "radio_20220920_144627" },
        { "", 20220920144627UL, "sbuff_20220920144627" },
        { "modem", 20220921001523UL, "modem_20220921_001523" },
        { "", 20220921001523UL, "sbuff_20220921001523" },
    };

    for (auto& t : testVector) {
        EXPECT_TRUE(SdmDocManager::getFilename(t.prefix, t.date) == t.filename);
    }
}

TEST(SdmDocManager, getNewFilename) {
    std::initializer_list<std::string> testVector = {
        "hi", "test", "radio", "QB12345", "",
    };

    SdmDocManager test;
    for (auto& t : testVector) {
        SdmDocEnv::setFilenamePrefix(t);
        auto sdmDoc = std::shared_ptr<SdmDoc>(test.newDocument());
        ASSERT_TRUE(sdmDoc);
        auto prefix = !t.empty() ? t : "sbuff";
        auto filepath = sdmDoc->getFilepath();
        auto pos = filepath.find(prefix);
        EXPECT_TRUE(pos != std::string::npos);
        EXPECT_TRUE(filepath.substr(pos, prefix.size()) == prefix);
        std::remove(filepath.c_str());
    }
    SdmDocEnv::setFilenamePrefix("");
}