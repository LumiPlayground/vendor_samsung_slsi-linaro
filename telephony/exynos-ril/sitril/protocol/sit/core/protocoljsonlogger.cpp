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
 * protocoljsonlogger.cpp
 *
 *  Created on: 2022. 03. 31.
 *      Author: p1.bansal
 */

#include "protocoljsonlogger.h"
#include "protocolutils.h"
#include "rillog.h"
#include <string>

using std::string;

/**
 * ProtocolJsonLogger
 */
void ProtocolJsonLogger::LogJsonRespData(UINT rcmId, const char* jsonString) {
    char format[MAX_JSON_LOG_FORMAT_LEN] = {0, };
    sprintf(format, "[%s] (%s) JsonRespData = ", __FUNCTION__, ProtocolUtils::rcmMsgToString(rcmId));
    LogJsonData(format, jsonString);
}

void ProtocolJsonLogger::LogJsonIndData(UINT rcmId, const char* jsonString) {
    char format[MAX_JSON_LOG_FORMAT_LEN] = {0, };
    sprintf(format, "[%s] (%s) JsonIndData = ", __FUNCTION__, ProtocolUtils::rcmMsgToString(rcmId));
    LogJsonData(format, jsonString);
}

void ProtocolJsonLogger::LogJsonReqData(UINT rcmId, const char* jsonString) {
    char format[MAX_JSON_LOG_FORMAT_LEN] = {0, };
    sprintf(format, "[%s] (%s) JsonReqData = ", __FUNCTION__, ProtocolUtils::rcmMsgToString(rcmId));
    LogJsonData(format, jsonString);
}

void ProtocolJsonLogger::LogJsonData(char *format, const char* jsonString) {

    string responseString = string(format) + string(jsonString);
    string currentSubStr;
    int prevIndex = 0;
    size_t commaIndex = responseString.find(",");

    while(commaIndex != string::npos) {
        int lenOfSubStr = (commaIndex-prevIndex+1); // Length of sub string starting from prevIndex to commaIndex
        if(lenOfSubStr > MAX_JSON_LOG_PRINT_LEN)
        {
            currentSubStr = responseString.substr(prevIndex, lenOfSubStr);
            RilLogI("%s", currentSubStr.c_str());
            prevIndex = commaIndex + 1;
        }
        commaIndex = responseString.find(",", commaIndex+1);
    }
    currentSubStr = responseString.substr(prevIndex, responseString.length());
    RilLogI("%s", currentSubStr.c_str());

}
