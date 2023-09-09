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
 * protocoljsonbuilder.cpp
 *
 *  Created on: 2021. 07. 22.
 *      Author: p1.bansal
 */

#include "protocoljsonbuilder.h"
#include "protocoljsonlogger.h"
#include "protocolutils.h"
#include "modemdata.h"
#include "rillog.h"
#include <string>

using std::string;

/**
 * ProtocolJsonBuilder
 */
ModemData *ProtocolJsonBuilder::GetJsonReqModemData(Json::Value &jsonReqData, int id) {
    Json::StreamWriterBuilder builder;
    /* Ensures printing of array values in same line while keeping the keys in next line
    builder["commentStyle"] = "None"; */
    builder["indentation"] = "";
    string jsonDataStream = Json::writeString(builder, jsonReqData);
    const char *requestData = jsonDataStream.c_str();
    if (requestData == NULL) {
        return NULL;
    }
    int requestDataLen = strlen(requestData);
    requestDataLen = requestDataLen + 1;  // 1 extra byte so that sent serialized string data includes NULL termination character also

    ProtocolJsonLogger::LogJsonReqData(id, requestData);

    int totalLen = requestDataLen + sizeof(RCM_HEADER);
    char *buf = new char[totalLen];
    InitRequestHeader((RCM_HEADER *)buf, id, totalLen);
    if (requestData != NULL && requestDataLen > 0) {
        memcpy(buf + sizeof(RCM_HEADER), requestData, requestDataLen);
    }
    ModemData *modemData = new ModemData(buf, totalLen);
    delete[] buf;
    return modemData;
}

ModemData *ProtocolJsonBuilder::GetJsonNullReqModemData(int id) {
    int length = sizeof(RCM_HEADER);
    char *buf = new char[length];
    InitRequestHeader((RCM_HEADER *)buf, id, length);
    ModemData *modemData = new ModemData(buf, length);
    delete[] buf;
    return modemData;
}
