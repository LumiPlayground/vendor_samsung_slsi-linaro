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
 * sdmstream_test.cpp
 *
 *  Created on: 2021. 9. 3.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "sdmstream.h"
#include "dmd_test_util.h"

TEST(SdmStream, FileOutputStream) {
    std::string filepath = "/data/local/tmp/dmd_tests/FileOutputStream.sdm2";
    FileOutputStream test(filepath);

    srand(time(0));
    TestVector testVector[10];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        int ret = test.write(testVector[i].data, testVector[i].datalen);
        EXPECT_EQ(ret, (int)testVector[i].datalen);
    }
    test.close();

    FILE *fp = fopen(filepath.c_str(), "r");
    EXPECT_NE(fp, nullptr);
    if (fp) {
        uint8_t buf[1024];
        for (size_t i = 0; i < size; i++) {
            int ret = fread(buf, 1, testVector[i].datalen, fp);
            EXPECT_EQ(ret, (int)testVector[i].datalen);
            std::string testStr = ::toHexdump(buf, ret);
            std::string strData = ::toHexdump(testVector[i].data, testVector[i].datalen);
            EXPECT_TRUE(testStr.compare(strData) == 0);
        }
        fclose(fp);
    }

    // remove
    unlink(filepath.c_str());
}

TEST(SdmStream, FileInputStream) {
    const char *filepath = "/data/local/tmp/dmd_tests/FileInputStream.sdm2";
    srand(time(0));
    TestVector testVector[10];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    FILE *fp = fopen(filepath, "w");
    EXPECT_NE(fp, nullptr);
    if (fp) {
        for (size_t i = 0; i < size; i++) {
            int ret = fwrite(testVector[i].data, 1, testVector[i].datalen, fp);
            EXPECT_EQ(ret, (int)testVector[i].datalen);
        }
        fclose(fp);
    }

    FileInputStream test(filepath);
    char buf[1024];
    for (size_t i = 0; i < size; i++) {
        int ret = test.read(buf, testVector[i].datalen);
        EXPECT_EQ(ret, (int)testVector[i].datalen);
        std::string testStr = ::toHexdump(buf, ret);
        std::string strData = ::toHexdump(testVector[i].data, testVector[i].datalen);
        EXPECT_TRUE(testStr.compare(strData) == 0);
    }
    test.close();
    unlink(filepath);
}

TEST(SdmStream, MemoryBufferOutputStream) {
    srand(time(0));
    TestVector testVector[20];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    size_t total = 0;
    MemoryBufferOutputStream test;
    for (size_t i = 0; i < size; i++) {
        int ret = test.write(testVector[i].data, testVector[i].datalen);
        EXPECT_EQ(ret, testVector[i].datalen);
        total += testVector[i].datalen;
    }

    uint8_t *p = (uint8_t *)test.buf();
    for (size_t i = 0; i < size; i++) {
        TestVector& t = testVector[i];
        std::string testStr = ::toHexdump(p, t.datalen);
        std::string strData = ::toHexdump(t.data, t.datalen);
        EXPECT_TRUE(testStr.compare(strData) == 0);
        p += t.datalen;
    }
    EXPECT_EQ(total, test.size());

    // small size buffer
    MemoryBufferOutputStream test2(1024);
    for (size_t i = 0; i < size; i++) {
        int ret = test2.write(testVector[i].data, testVector[i].datalen);
        EXPECT_EQ(ret, testVector[i].datalen);
    }

    p = (uint8_t *)test2.buf();
    for (size_t i = 0; i < size; i++) {
        TestVector& t = testVector[i];
        std::string testStr = ::toHexdump(p, t.datalen);
        std::string strData = ::toHexdump(t.data, t.datalen);
        EXPECT_TRUE(testStr.compare(strData) == 0);
        p += t.datalen;
    }
    EXPECT_EQ(total, test2.size());

    // over than max size
    MemoryBufferOutputStream test3((2048 * 2048) + 1);
    for (size_t i = 0; i < size; i++) {
        int ret = test3.write(testVector[i].data, testVector[i].datalen);
        EXPECT_EQ(ret, testVector[i].datalen);
    }

    p = (uint8_t *)test3.buf();
    for (size_t i = 0; i < size; i++) {
        TestVector& t = testVector[i];
        std::string testStr = ::toHexdump(p, t.datalen);
        std::string strData = ::toHexdump(t.data, t.datalen);
        EXPECT_TRUE(testStr.compare(strData) == 0);
        p += t.datalen;
    }
    EXPECT_EQ(total, test3.size());
}

TEST(SdmStream, MemoryBufferInputStream) {
    char data[16 * 10];
    for (size_t i = 0; i < sizeof(data); i++) {
        data[i] = rand() % 256;
    }
    MemoryBufferInputStream test(data, sizeof(data));
    char buf[100];
    size_t size = 16;
    for (size_t i = 0; i < sizeof(data); i += size) {
        EXPECT_TRUE(test.read(buf, size) > 0);
        EXPECT_TRUE(memcmp(buf, data + i, size) == 0);
    }
}

TEST(SdmStream, BufferedOutputStream) {
    srand(time(0));
    const size_t maxSize = 1024;
    MemoryBufferOutputStream *mos = new MemoryBufferOutputStream();
    MemoryBufferOutputStream ans;
    BufferedOutputStream test(mos, maxSize);
    size_t size = 0;
    size_t bufSize = 0;
    for (size_t i = 0; i < 100; i++) {
        TestVector t;
        size += t.datalen;
        if ((bufSize / maxSize) < (size / maxSize)) {
            bufSize = size;
        }
        int ret = test.write(t.data, t.datalen);
        ans.write(t.data, t.datalen);
        EXPECT_EQ(ret, t.datalen);
        EXPECT_EQ(test.tell(), size);

//        EXPECT_EQ(mos->size(), bufSize);
//        EXPECT_TRUE(memcmp(ans.buf(), mos->buf(), bufSize) == 0);
    }
    test.close();
    EXPECT_EQ(ans.size(), size);
    EXPECT_EQ(mos->size(), ans.size());
    EXPECT_TRUE(memcmp(ans.buf(), mos->buf(), size) == 0);
}

TEST(SdmStream, RawFileOutputStream) {
    std::string filepath = "/data/local/tmp/dmd_tests/RawFileOutputStream.dummy";
    RawFileOutputStream test(filepath);

    srand(time(0));
    TestVector testVector[10];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    size_t totlaSize = 0;
    for (size_t i = 0; i < size; i++) {
        int ret = test.write(testVector[i].data, testVector[i].datalen);
        EXPECT_EQ(ret, (int)testVector[i].datalen);
        totlaSize += testVector[i].datalen;
    }
    EXPECT_EQ(test.tell(), totlaSize);
    test.close();
    EXPECT_EQ(test.tell(), 0);

    FILE *fp = fopen(filepath.c_str(), "r");
    EXPECT_NE(fp, nullptr);
    if (fp) {
        uint8_t buf[1024];
        for (size_t i = 0; i < size; i++) {
            int ret = fread(buf, 1, testVector[i].datalen, fp);
            EXPECT_EQ(ret, (int)testVector[i].datalen);
            std::string testStr = ::toHexdump(buf, ret);
            std::string strData = ::toHexdump(testVector[i].data, testVector[i].datalen);
            EXPECT_TRUE(testStr.compare(strData) == 0);
        }
        fclose(fp);
    }

    // remove
    unlink(filepath.c_str());
}