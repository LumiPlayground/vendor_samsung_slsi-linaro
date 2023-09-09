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
#include <radioresponse.h>

TEST(RadioResponse, RadioResponse) {
    using namespace android::hardware::radio::impl;
    uint32_t slot = 0;
    auto test = RadioResponse::make(slot);
    EXPECT_TRUE(test->modem());
    EXPECT_TRUE(test->data());
    EXPECT_TRUE(test->messaging());
    EXPECT_TRUE(test->network());
    EXPECT_TRUE(test->sim());
    EXPECT_TRUE(test->config());
    EXPECT_TRUE(test->voice());

    test->modem(nullptr);
    test->data(nullptr);
    test->messaging(nullptr);
    test->network(nullptr);
    test->sim(nullptr);
    test->voice(nullptr);
    EXPECT_FALSE(test->modem());
    EXPECT_FALSE(test->data());
    EXPECT_FALSE(test->messaging());
    EXPECT_FALSE(test->network());
    EXPECT_FALSE(test->sim());
    EXPECT_FALSE(test->voice());
}

TEST(RadioResponse, RadioIndication) {
    using namespace android::hardware::radio::impl;
    uint32_t slot = 0;
    auto test = RadioIndication::make(slot);
    EXPECT_TRUE(test->modem());
    EXPECT_TRUE(test->data());
    EXPECT_TRUE(test->messaging());
    EXPECT_TRUE(test->network());
    EXPECT_TRUE(test->sim());
    EXPECT_TRUE(test->config());
    EXPECT_TRUE(test->voice());

    test->modem(nullptr);
    test->data(nullptr);
    test->messaging(nullptr);
    test->network(nullptr);
    test->sim(nullptr);
    test->voice(nullptr);
    EXPECT_FALSE(test->modem());
    EXPECT_FALSE(test->data());
    EXPECT_FALSE(test->messaging());
    EXPECT_FALSE(test->network());
    EXPECT_FALSE(test->sim());
    EXPECT_FALSE(test->voice());
}
