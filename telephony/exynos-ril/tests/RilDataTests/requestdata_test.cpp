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
 * requestdata_test.cpp
 *
 *  Created on: 2021. 8. 3.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "requestdata.h"
#include "librilutils/textutils.h"
#include "librilutils/vendor.h"
#include "slsi/radio_v1_5.h"

TEST(RequestData, RequestData) {
    RequestData test;
    EXPECT_EQ(test.GetHalVersion(), HAL_VERSION_CODE(1, 0));

    int t;
    test = RequestData(RIL_REQUEST_DATA_REGISTRATION_STATE, (RIL_Token)&t);
    EXPECT_EQ(test.GetReqId(), RIL_REQUEST_DATA_REGISTRATION_STATE);
    EXPECT_EQ((void *)test.GetToken(), (void *)&t);

    test = RequestData(
            ENCODE_REQUEST(RIL_REQUEST_DATA_REGISTRATION_STATE, HAL_VERSION_CODE(1, 5)),
            (RIL_Token)&t);
    EXPECT_EQ(test.GetReqId(), RIL_REQUEST_DATA_REGISTRATION_STATE);
    EXPECT_EQ(test.GetHalVersion(), HAL_VERSION_CODE(1, 5));

    RequestData *clone = test.Clone();
    ASSERT_NE(clone, nullptr);
    if (clone) {
        EXPECT_EQ(clone->GetReqId(), test.GetReqId());
        EXPECT_EQ((void *)clone->GetToken(), (void *)test.GetToken());
        delete clone;
    }
}

TEST(RequestData, IntRequestData) {
    int testVector[] = { 10 };
    IntRequestData test(RIL_REQUEST_DATA_REGISTRATION_STATE, 0);
    EXPECT_EQ(test.encode((char *)0, 0), -1);
    EXPECT_EQ(test.encode((char *)testVector, 0), -1);
    EXPECT_EQ(test.encode((char *)testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetInt(), testVector[0]);

    IntRequestData *clone = test.Clone();
    ASSERT_NE(clone, nullptr);
    if (clone) {
        EXPECT_EQ(clone->GetInt(), test.GetInt());
        delete clone;
    }
}

TEST(RequestData, IntsRequestData) {
    int testVector[] = { 10, 20 };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    IntsRequestData test(RIL_REQUEST_DATA_REGISTRATION_STATE, 0);
    EXPECT_EQ(test.encode((char *)0, 0), -1);
    EXPECT_EQ(test.encode((char *)testVector, 0), -1);
    EXPECT_EQ(test.encode((char *)testVector, sizeof(testVector) + 1), -1);
    EXPECT_EQ(test.encode((char *)testVector, sizeof(testVector)), 0);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetSize(), size);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(test.GetInt(i), testVector[i]);
    }

    IntsRequestData *clone = test.Clone();
    ASSERT_NE(clone, nullptr);
    if (clone) {
        EXPECT_EQ(clone->GetSize(), test.GetSize());
        for (size_t i = 0; i < size; i++) {
            EXPECT_EQ(clone->GetInt(i), test.GetInt(i));
        }
        delete clone;
    }
}

TEST(RequestData, StringRequestData) {
    char testVector[] = "test string";
    StringRequestData test(RIL_REQUEST_DATA_REGISTRATION_STATE, 0);
    EXPECT_EQ(test.encode((char *)0, 0), 0);
    EXPECT_EQ(test.encode((char *)0, sizeof(testVector)), 0);
    EXPECT_EQ(test.encode((char *)"", sizeof(testVector)), 0);
    EXPECT_EQ(test.encode(testVector, sizeof(testVector)), 0);
    EXPECT_TRUE(TextUtils::Equals(test.GetString(), testVector));

    StringRequestData *clone = test.Clone();
    ASSERT_NE(clone, nullptr);
    if (clone) {
        EXPECT_TRUE(!TextUtils::IsEmpty(test.GetString()));
        EXPECT_TRUE(!TextUtils::IsEmpty(clone->GetString()));
        EXPECT_TRUE(TextUtils::Equals(clone->GetString(), test.GetString()));
        delete clone;
    }
}

TEST(RequestData, StringsRequestData) {
    char *testVector[] = {
        (char *)"test string1",
        (char *)"test string2",
        (char *)"test string3",
        nullptr,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    StringsRequestData test(RIL_REQUEST_DATA_REGISTRATION_STATE, 0);
    EXPECT_EQ(test.encode((char *)0, 0), 0);
    EXPECT_EQ(test.encode((char *)0, sizeof(testVector)), 0);
    EXPECT_EQ(test.encode((char *)testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetString(-1), nullptr);
    EXPECT_EQ((unsigned long)/* NEED TO CHECK */ test.GetStringCount(), size);
    char **contents = test.GetStringsContent();
    ASSERT_NE(contents, nullptr);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(TextUtils::Equals(test.GetString(i), testVector[i]));
        EXPECT_TRUE(TextUtils::Equals(contents[i], testVector[i]));
    }

    StringsRequestData *clone = test.Clone();
    ASSERT_NE(clone, nullptr);
    if (clone) {
        EXPECT_EQ(clone->GetStringCount(), test.GetStringCount());
        for (size_t i = 0; i < size; i++) {
            EXPECT_TRUE(TextUtils::Equals(clone->GetString(i), test.GetString(i)));
        }
        delete clone;
    }
}

TEST(RequestData, RawRequestData) {
    char testVector[100];
    RawRequestData test(RIL_REQUEST_DATA_REGISTRATION_STATE, 0);
    EXPECT_EQ(test.encode((char *)0, 0), 0);
    EXPECT_EQ(test.encode(testVector, sizeof(testVector)), 0);
    EXPECT_NE(test.GetRawData(), nullptr);
    EXPECT_EQ((unsigned long)/* NEED To CHECK */ test.GetSize(), sizeof(testVector));
    if (test.GetRawData()) {
        EXPECT_TRUE(memcmp(test.GetRawData(), testVector, sizeof(testVector)) == 0);
    }

    RawRequestData *clone = test.Clone();
    ASSERT_NE(clone, nullptr);
    if (clone) {
        EXPECT_EQ(clone->GetSize(), test.GetSize());
        EXPECT_TRUE(memcmp(clone->GetRawData(), test.GetRawData(), sizeof(test.GetSize())) == 0);
        delete clone;
    }
}

TEST(RequestData, OemStringsRequestData) {
    const char *testVector[] = {
        "test string1",
        "test string2",
        "test string3",
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    char buf[100];
    char *p = buf;
    *((int *)p) = size; // 3 strings
    p += sizeof(int);
    for (size_t i = 0; i < size; i++) {
        size_t len = strlen(testVector[i]);
        *((int *)p) = len;
        p += sizeof(int);
        memcpy(p, testVector[i], len);
        p += len;
    }

    OemStringsRequestData test(RIL_REQUEST_DATA_REGISTRATION_STATE, 0);
    EXPECT_EQ(test.encode((char *)0, 0), 0);
    EXPECT_EQ(test.encode((char *)0, sizeof(testVector)), 0);
    EXPECT_EQ(test.encode(buf, p - buf), 0);
    EXPECT_EQ(test.GetString(-1), nullptr);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(TextUtils::Equals(test.GetString(i), testVector[i]));
    }

    OemStringsRequestData *clone = test.Clone();
    ASSERT_NE(clone, nullptr);
    if (clone) {
        for (size_t i = 0; i < size; i++) {
            EXPECT_TRUE(TextUtils::Equals(clone->GetString(i), test.GetString(i)));
        }
        delete clone;
    }
}
