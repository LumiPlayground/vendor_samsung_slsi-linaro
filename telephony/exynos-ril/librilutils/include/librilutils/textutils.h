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
#pragma once

#include <string>

namespace TextUtils {

bool IsEmpty(const char *str);
bool Equals(const char *l, const char *r);
bool IsEmpty(const std::string &str);
bool Equals(const std::string &l, const std::string &r);
bool IsDigitsOnly(const char *str);
bool IsDigitsOnly(const std::string &str);
bool DupString(char **dest, char *src, bool allowEmpty = true);
int ParseInt(const char *str);
int ParseInt(const std::string &str);

template<typename... Args>
inline std::string string_format(const std::string& format, Args ... args) {
    size_t n = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    if (n > 0) {
        std::unique_ptr<char[]> str(new char[n]);
        std::snprintf(str.get(), n, format.c_str(), args...);
        return str.get();
    }
    return "";
}

}
