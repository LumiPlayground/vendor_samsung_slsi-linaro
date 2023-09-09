/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
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
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <testchecker.h>
#include <string>

#ifdef DLOG
#undef DLOG
#endif

#define ENABLE_DLOG 1

#if ENABLE_DLOG
#define DLOG(x, ...) printf(x"\n", ##__VA_ARGS__)
#else
#define DLOG(x, ...)
#endif

#define NUM_ELEMS(a)     (sizeof (a) / sizeof (a)[0])

int INTrand(bool timeseed);
int getRunningRadioHalVersion(std::string &out);

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

#endif
