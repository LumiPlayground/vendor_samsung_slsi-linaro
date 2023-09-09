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
 * registrant_test.cpp
 *
 *  Created on: 2021. 9. 27.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "registrant.h"

TEST(RegistrantList, RegistrantList_value) {
    struct TestHolder {
        int i;
    };

    RegistrantList<TestHolder> test;
    const size_t size = 10;
    TestHolder testVector[size];
    for (size_t i = 0; i < size; i++) {
        testVector[i].i = i;
        test.add(testVector[i]);
    }

    struct LocalExecutor : public Executor<TestHolder> {
        int result;
        bool called[size];
        LocalExecutor() {
            result = 0;
            for (size_t i = 0; i < size; i++) {
                called[i] = false;
            }
        }
        void run(TestHolder&& holder) {
            result++;
            called[holder.i] = true;
        }
    } executor;
    test.notifyAll(&executor);
    EXPECT_EQ((unsigned long)/* NEED TO CHECK */ executor.result, size);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(executor.called[i]);
    }

    test.clear();
    executor = LocalExecutor();
    EXPECT_EQ(executor.result, 0);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(executor.called[i]);
    }
}

TEST(RegistrantList, RegistrantList_pointer) {
    struct TestHolder {
        int i;
    };

    using T = TestHolder *;
    RegistrantList<T> test;
    const size_t size = 10;
    TestHolder testVector[size];
    for (size_t i = 0; i < size; i++) {
        testVector[i].i = i;
        test.add(&testVector[i]);
    }

    struct LocalExecutor : public Executor<T> {
        int result;
        bool called[size];
        LocalExecutor() {
            result = 0;
            for (size_t i = 0; i < size; i++) {
                called[i] = false;
            }
        }
        void run(T&& holder) {
            result++;
            called[holder->i] = true;
        }
    } executor;
    test.notifyAll(&executor);
    EXPECT_EQ((unsigned long)/* NEED TO CHECK */ executor.result, size);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(executor.called[i]);
    }

    executor = LocalExecutor();
    test.clear();
    EXPECT_EQ(executor.result, 0);
    for (size_t i = 0; i < size; i++) {
        EXPECT_FALSE(executor.called[i]);
    }
}
