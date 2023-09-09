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
 * textutils.cpp
 *
 *  Created on: 2015. 1. 5.
 *      Author: sungwoo48.choi
 */
#include <librilutils/textutils.h>

#ifdef SIZE_MAX
#undef SIZE_MAX
#define SIZE_MAX 65535
#endif

using namespace std;

namespace TextUtils {

bool IsEmpty(const char *str) {
    return (str == NULL || *str == 0);
}

bool Equals(const char *l, const char *r) {
    if (IsEmpty(l) && IsEmpty(r)) {
        return true;
    }

    if ((!IsEmpty(l) && r == NULL) || (l == NULL && !IsEmpty(r))) {
        return false;
    }

    return strncmp(l, r, SIZE_MAX) == 0;
}

bool IsEmpty(const string &str) {
    return str.empty();
}

bool Equals(const string &l, const string &r) {
    return l.compare(r) == 0;
}

bool IsDigitsOnly(const char *str) {
    if (IsEmpty(str)) {
        return false;
    }

    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        char ch = *(str + i);
        if (ch < '0' || ch > '9') {
            return false;
        }
    } // end for i ~
    return true;
}

bool IsDigitsOnly(const string &str) {
    return IsDigitsOnly(str.c_str());
}

bool DupString(char **dest, char *src, bool allowEmpty) {
     if (dest == NULL || (IsEmpty(src) && !allowEmpty)) {
         return false;
     }

     int len = (src == NULL)? 0 : strlen(src);
     *dest = new char[len + 1];
     if (*dest == NULL) {
         return false;
     }

     if(len > 0) strncpy(*dest, src, len);
     *(*dest + len) = 0;
     return true;
}

int ParseInt(const char *str) {
    int ret = INT_MAX;
    if (IsDigitsOnly(str)) {
        ret = strtol(str, NULL, 10);
    }
    return ret;
}

int ParseInt(const string &str) {
    return ParseInt(str.c_str());
}

}
