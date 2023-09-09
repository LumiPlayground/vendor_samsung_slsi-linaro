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
#include <functional>
#include "postaction.h"
#include "dmd_test_util.h"
#include "timeutils.h"

TEST(PostAction, PostAction) {

    bool criteria1 = false;
    bool criteria2 = false;
    bool criteria3 = false;

    PostAction<std::function<void()>> test;
    test.add([&criteria1]() { criteria1 = true; });
    test.add([&criteria2]() { criteria2 = true; });
    test.add([&criteria3]() { criteria3 = true; });
    test.doAction();
    EXPECT_TRUE(criteria1);
    EXPECT_TRUE(criteria2);
    EXPECT_TRUE(criteria3);
}

TEST(PostAction, PostAction_UTC) {
    static uint64_t timestamp = timeutils::getTimestamp();
    struct TestStub {
        void Write(const void *data, size_t /* datalen */) {
            uint8_t buf[8] {};
            memcpy(buf, (uint8_t *)data + 15, sizeof(buf));
            uint64_t result = *(uint64_t *)buf;
            EXPECT_TRUE(result == timestamp);
        }
    };
    TestStub testStub;
    auto postActions = PostAction<std::function<void()>>();
    postActions.add([modemProxy = &testStub]() {
        uint8_t packet[] = {
            0x7F,
            0x1C, 0x00, 0x81, 0x19, 0x00, 0x00, 0x00,
            0xA0, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // UTC
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Reserved
            0x7E
        };
        mempcpy(packet + 15, &timestamp, sizeof(uint64_t));
        if (modemProxy) {
            modemProxy->Write(packet, sizeof(packet));
        }
    });
    postActions.doAction();
}