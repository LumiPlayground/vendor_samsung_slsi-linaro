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
#include "dmagentcallbackimpl.h"

TEST(FileOperationCallbackImpl, onSetFilenamePrefix) {
    std::initializer_list<std::string> testVector = {
        "sdm_prefix",
        "sbuff",
        "sbuff_123456789012345678901234567890",
    };

    FileOperationCallbackImpl test(nullptr);
    for (auto& t : testVector) {
        test.onSetFilenamePrefix(t);
        EXPECT_TRUE(SdmDocEnv::getFilenamePrefix() == t.substr(0, 32));
    }
    test.onSetFilenamePrefix("");
}