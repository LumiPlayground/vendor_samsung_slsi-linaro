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
 * datautils_test.cpp
 *
 *  Created on: 2021. 9. 6.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "datautils.h"

#include "dmd_test_util.h"

TEST(DataUtils, DataWriter_MemoryBufferOutputStream) {
    srand(time(0));
    struct LocalTestVector {
        uint8_t *data;
        size_t datalen;
        LocalTestVector() {
            datalen = (1 + 1 + 2 + 2 + 4 + 4 + 8 + 8 + 16);
            data = new uint8_t[datalen]{};
            if (data) {
                for (size_t i = 0; i < datalen; i++) {
                    *(data + i) = rand() % 256;
                }
            }
        }
        ~LocalTestVector() {
            if (data) delete data;
        }
    };

    LocalTestVector testVector[5];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        MemoryBufferOutputStream out;
        DataWriter test(&out);
        LocalTestVector& t = testVector[i];
        uint8_t *p = t.data;

        test.append(*p);
        p++;
        test.append((int8_t)*p);
        p++;
        test.append(*(uint16_t *)p);
        p += sizeof(uint16_t);
        test.append(*(int16_t *)p);
        p += sizeof(int16_t);
        test.append(*(uint32_t *)p);
        p += sizeof(uint32_t);
        test.append(*(int32_t *)p);
        p += sizeof(int32_t);
        uint64_t val1 = 0;
        for (int i = 0; i < 8; i++) {
            val1 |= (p[i] & 0xFFFFFFFFFFFFFFFFUL) << (i * 8);
        }
        test.append(val1);
        p += sizeof(uint64_t);
        int64_t val2 = 0;
        for (int i = 0; i < 8; i++) {
            val2 |= (p[i] & 0xFFFFFFFFFFFFFFFFL) << (i * 8);
        }
        test.append(val2);
        p += sizeof(int64_t);
        test.append(p, 16);
        p += 16;

        EXPECT_EQ(p - t.data, t.datalen);
        EXPECT_EQ(out.size(), t.datalen);
        EXPECT_TRUE(memcmp(out.buf(), t.data, t.datalen) == 0);
    }
}

TEST(DataUtils, DataReader_MemoryBufferInputStream) {
    srand(time(0));

    struct LocalTestVector {
        uint8_t *data;
        size_t datalen;
        LocalTestVector() {
            datalen = (1 + 1 + 2 + 2 + 4 + 4 + 8 + 8 + 16);
            data = new uint8_t[datalen]{};
            if (data) {
                for (size_t i = 0; i < datalen; i++) {
                    *(data + i) = rand() % 256;
                }
            }
        }
        ~LocalTestVector() {
            if (data) delete data;
        }
    };
    LocalTestVector testVector[5];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        LocalTestVector& t = testVector[i];
        uint8_t *p = t.data;
        MemoryBufferInputStream in(t.data, t.datalen);
        DataReader dr(&in);
        uint8_t b1;
        EXPECT_EQ(dr.read(b1), sizeof(b1));
        EXPECT_TRUE(memcmp(&b1, p, sizeof(b1)) == 0);
        p += sizeof(b1);

        int8_t b2;
        EXPECT_EQ(dr.read(b2), sizeof(b2));
        EXPECT_TRUE(memcmp(&b2, p, sizeof(b2)) == 0);
        p += sizeof(b2);

        uint16_t s1;
        EXPECT_EQ(dr.read(s1), sizeof(s1));
        EXPECT_TRUE(memcmp(&s1, p, sizeof(s1)) == 0);
        p += sizeof(s1);

        int16_t s2;
        EXPECT_EQ(dr.read(s2), sizeof(s2));
        EXPECT_TRUE(memcmp(&s2, p, sizeof(s2)) == 0);
        p += sizeof(s2);

        uint32_t val1;
        EXPECT_EQ(dr.read(val1), sizeof(val1));
        EXPECT_TRUE(memcmp(&val1, p, sizeof(val1)) == 0);
        p += sizeof(val1);

        int32_t val2;
        EXPECT_EQ(dr.read(val2), sizeof(val2));
        EXPECT_TRUE(memcmp(&val2, p, sizeof(val2)) == 0);
        p += sizeof(val2);

        uint64_t val3;
        EXPECT_EQ(dr.read(val3), sizeof(val3));
        EXPECT_TRUE(memcmp(&val3, p, sizeof(val3)) == 0);
        p += sizeof(val3);

        int64_t val4;
        EXPECT_EQ(dr.read(val4), sizeof(val4));
        EXPECT_TRUE(memcmp(&val4, p, sizeof(val4)) == 0);
        p += sizeof(val4);

        char tmp[16] = {};
        EXPECT_EQ(dr.read(tmp, sizeof(tmp)), sizeof(tmp));
        EXPECT_TRUE(memcmp(&tmp, p, sizeof(val4)) == 0);
    }
}

TEST(DataUtils, DataWriter_FileOutputStream) {
    srand(time(0));
    struct LocalTestVector {
        uint8_t *data;
        size_t datalen;
        LocalTestVector() {
            datalen = (1 + 1 + 2 + 2 + 4 + 4 + 8 + 8 + 16);
            data = new uint8_t[datalen]{};
            if (data) {
                for (size_t i = 0; i < datalen; i++) {
                    *(data + i) = rand() % 256;
                }
            }
        }
        ~LocalTestVector() {
            if (data) delete data;
        }
    };

    LocalTestVector testVector[5];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    const char *filepath = "/data/local/tmp/dmd_tests/DataReader_FileInputStream";
    for (size_t i = 0; i < size; i++) {
        FileOutputStream out(filepath);
        DataWriter test(&out);
        LocalTestVector& t = testVector[i];
        uint8_t *p = t.data;

        test.append(*p);
        p++;
        test.append((int8_t)*p);
        p++;
        test.append(*(uint16_t *)p);
        p += sizeof(uint16_t);
        test.append(*(int16_t *)p);
        p += sizeof(int16_t);
        test.append(*(uint32_t *)p);
        p += sizeof(uint32_t);
        test.append(*(int32_t *)p);
        p += sizeof(int32_t);
        uint64_t val1 = 0;
        for (int i = 0; i < 8; i++) {
            val1 |= (p[i] & 0xFFFFFFFFFFFFFFFFUL) << (i * 8);
        }
        test.append(val1);
        p += sizeof(uint64_t);
        int64_t val2 = 0;
        for (int i = 0; i < 8; i++) {
            val2 |= (p[i] & 0xFFFFFFFFFFFFFFFFL) << (i * 8);
        }
        test.append(val2);
        p += sizeof(int64_t);
        test.append(p, 16);
        p += 16;
        out.close();

        FILE *fp = fopen(filepath, "r");
        EXPECT_NE(fp, nullptr);
        if (fp) {
            uint8_t *buf = new uint8_t[t.datalen]{};
            int ret = fread(buf, 1, t.datalen, fp);
            EXPECT_EQ(ret, t.datalen);
            EXPECT_TRUE(memcmp(buf, t.data, t.datalen) == 0);
            delete[] buf;
            fclose(fp);
        }
        unlink(filepath);
    }
}

TEST(DataUtils, DataReader_FileInputStream) {
    srand(time(0));

    struct LocalTestVector {
        uint8_t *data;
        size_t datalen;
        LocalTestVector() {
            datalen = (1 + 1 + 2 + 2 + 4 + 4 + 8 + 8 + 16);
            data = new uint8_t[datalen]{};
            if (data) {
                for (size_t i = 0; i < datalen; i++) {
                    *(data + i) = rand() % 256;
                }
            }
        }
        ~LocalTestVector() {
            if (data) delete data;
        }
    };
    LocalTestVector testVector[5];
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    const char *filepath = "/data/local/tmp/dmd_tests/DataReader_FileInputStream";
    for (size_t i = 0; i < size; i++) {
        FILE *fp = fopen(filepath, "w");
        if (fp) {
            EXPECT_NE(fp, nullptr);
            LocalTestVector& t = testVector[i];
            int ret = fwrite(t.data, 1, t.datalen, fp);
            EXPECT_EQ(ret, (int)t.datalen);
            fclose(fp);
            fp = nullptr;

            uint8_t *p = t.data;
            FileInputStream in(filepath);
            DataReader dr(&in);
            int8_t b2;
            EXPECT_EQ(dr.read(b2), sizeof(b2));
            EXPECT_TRUE(memcmp(&b2, p, sizeof(b2)) == 0);
            p += sizeof(b2);

            uint16_t s1;
            EXPECT_EQ(dr.read(s1), sizeof(s1));
            EXPECT_TRUE(memcmp(&s1, p, sizeof(s1)) == 0);
            p += sizeof(s1);

            int16_t s2;
            EXPECT_EQ(dr.read(s2), sizeof(s2));
            EXPECT_TRUE(memcmp(&s2, p, sizeof(s2)) == 0);
            p += sizeof(s2);

            uint32_t val1;
            EXPECT_EQ(dr.read(val1), sizeof(val1));
            EXPECT_TRUE(memcmp(&val1, p, sizeof(val1)) == 0);
            p += sizeof(val1);

            int32_t val2;
            EXPECT_EQ(dr.read(val2), sizeof(val2));
            EXPECT_TRUE(memcmp(&val2, p, sizeof(val2)) == 0);
            p += sizeof(val2);

            uint64_t val3;
            EXPECT_EQ(dr.read(val3), sizeof(val3));
            EXPECT_TRUE(memcmp(&val3, p, sizeof(val3)) == 0);
            p += sizeof(val3);

            int64_t val4;
            EXPECT_EQ(dr.read(val4), sizeof(val4));
            EXPECT_TRUE(memcmp(&val4, p, sizeof(val4)) == 0);
            p += sizeof(val4);

            char tmp[16] = {};
            EXPECT_EQ(dr.read(tmp, sizeof(tmp)), sizeof(tmp));
            EXPECT_TRUE(memcmp(&tmp, p, sizeof(val4)) == 0);
        }
    }

    unlink(filepath);
}
