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
 * memblock_test.cpp
 *
 *  Created on: 2021. 9. 8.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "memblock.h"

#include "dmd_test_util.h"

TEST(MemBlock, MemBlock) {
    srand(time(0));

    TestVector testVector[10];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        TestVector& t = testVector[i];
        MemBlock test(t.data, t.datalen);
        EXPECT_EQ(test.size(), t.datalen);
        EXPECT_TRUE(memcmp(test.ptr(), t.data, t.datalen) == 0);
    }
}

TEST(MemBlock, MemBlock_copy_assignment) {
    srand(time(0));

    TestVector testVector[10];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        TestVector& t = testVector[i];
        MemBlock test(t.data, t.datalen);
        // constructor copy
        MemBlock copy = test;
        EXPECT_TRUE(test == copy);

        // opeator= copy
        MemBlock copy2;
        copy2 = test;
        EXPECT_TRUE(test == copy2);
    }
}

MemBlock foo(void *data, size_t size) {
    return MemBlock(data, size);
}

TEST(MemBlock, MemBlock_move_assignment) {
    srand(time(0));

    TestVector testVector[10];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        TestVector& t = testVector[i];
        MemBlock test(t.data, t.datalen);
        void *p = test.ptr();
        size_t s = test.size();

        // constructor move
        MemBlock move = std::move(test);
        EXPECT_EQ(test.ptr(), nullptr);
        EXPECT_EQ(test.size(), 0);
        EXPECT_EQ(move.ptr(), p);
        EXPECT_EQ(move.size(), s);

        // operator= move
        MemBlock move2;
        move2 = std::move(move);
        EXPECT_EQ(move2.ptr(), p);
        EXPECT_EQ(move2.size(), s);

        // constructor. optimized by a compiler
        MemBlock move3 = foo(t.data, t.datalen);
        EXPECT_EQ(move3.size(), t.datalen);
        EXPECT_TRUE(memcmp(move3.ptr(), t.data, t.datalen) == 0);

        // constructor. optimized by a compiler
        MemBlock move4 = MemBlock(t.data, t.datalen);

        // constructor then move.
        MemBlock move5 = std::move(MemBlock(t.data, t.datalen));
    }
}

TEST(MemBlock, clear) {
    srand(time(0));
    TestVector testVector;
    MemBlock test(testVector.data, testVector.datalen);
    EXPECT_TRUE(memcmp(test.ptr(), testVector.data, testVector.datalen) == 0);
    EXPECT_TRUE(test.hasData());
    test.clear();
    EXPECT_EQ(test.ptr(), nullptr);
    EXPECT_EQ(test.size(), 0);
    EXPECT_FALSE(test.hasData());
}
