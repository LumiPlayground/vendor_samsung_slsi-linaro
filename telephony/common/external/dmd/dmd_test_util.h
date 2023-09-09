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
 * dmd_test_util.h
 *
 *  Created on: 2021. 9. 3.
 *      Author: sungwoo48.choi
 */

#ifndef __SDM_DOC_TEST_UTIL_H__
#define __SDM_DOC_TEST_UTIL_H__

#include <chrono>
#include <random>
#include <string>
#include "sdmstream.h"

#ifdef dlog
#undef dlog
#endif

#define ENABLE_DLOG 1

#if ENABLE_DLOG
#define dlog(x, ...) printf(x"\n", ##__VA_ARGS__)
#else
#define dlog(x, ...)
#endif

static inline int INTrand() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, INT_MAX);
    return dis(gen);
}

inline std::string toHexdump(const void *data, size_t datalen) {
    static char hex[] = "0123456789ABCDEF";
    std::string str = "";
    const char *p = (const char *)data;
    for (size_t i = 0; p && i < datalen; i++) {
        str += hex[(p[i] >> 4) & 0xF];
        str += hex[p[i] & 0xF];
        if ((i + 1) % 16 == 0) str += "\n";
    }
    return str;
}

struct TestVector {
    uint8_t *data;
    size_t datalen;
    TestVector() {
        datalen = INTrand() % (256 - 30) + 30;
        data = new uint8_t[datalen]{};
        if (data) {
            for (size_t i = 0; i < datalen; i++) {
                *(data + i) = INTrand() % 256;
            }
        }
    }
    ~TestVector() {
        if (data) delete data;
    }
};

#endif /* __SDM_DOC_TEST_UTIL_H__ */
