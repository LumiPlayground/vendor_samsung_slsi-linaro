/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_UTILS_H__
#define __PROTOCOL_UTILS_H__

#include "rillog.h"

#undef RilLogWarning
#define RilLogWarning(rilLogCat, format, ...) CRilLog::Log(rilLogCat, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)

inline std::string GetClassAndFuncName(const std::string& prettyFunction) {
    size_t colons = prettyFunction.find("::");
    size_t begin = prettyFunction.substr(0, colons).rfind(" ") + 1;
    size_t end = prettyFunction.rfind("(") - begin;

    return prettyFunction.substr(begin,end) + "()";
}

#define __CLASS_FUNC__ GetClassAndFuncName(__PRETTY_FUNCTION__).c_str()

namespace ProtocolUtils {

void LimitStringToMaxLength(int rilLogCat, const char *className, std::string &s, int maxLength);

template <typename T>
void LimitLengthToMax(int rilLogCat, const char* className, T &length, T maxLength) {
    if (length > maxLength) {
        RilLogWarning(rilLogCat, "%s length %d is more than max length %d", className, length, maxLength);
        length = maxLength;
    }
}

const char * rcmMsgToString(int rcmId);
const char * rcmErrorToString(int rspErr);
}  // ProtocolUtils

#endif // __PROTOCOL_UTILS_H__
