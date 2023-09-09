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
#ifndef __MODEM_DATA_BUILDER_H__
#define __MODEM_DATA_BUILDER_H__

#include "json/json.h"
#include <base/types.h>

class ModemData;

class ModemDataBuilder {
private:
    int mType;
    char *mData;
    size_t mDataLen;
    uint32_t mToken;
    uint16_t mId;
    BYTE mErrorCode;
public:
    ModemDataBuilder();
    ~ModemDataBuilder();
    ModemDataBuilder& SetType(int type);
    ModemDataBuilder& SetId(uint16_t id);
    ModemDataBuilder& SetToken(uint32_t token);
    ModemDataBuilder& SetErrorCode(uint8_t errorCode);
    ModemDataBuilder& SetParameter(void *parameter, size_t len);
    ModemData *Build();
    void Reset();

public:
    static ModemData *BuildRequest(uint16_t id, uint32_t token, void *data = nullptr, size_t dataLen = 0);
    static ModemData *BuildResponse(uint16_t id, uint32_t token, uint8_t errorCode = 0, void *data = nullptr, size_t dataLen = 0);
    static ModemData *BuildIndication(uint16_t id, void *data = nullptr, size_t dataLen = 0);

    // for JSON
    static ModemData *BuildJsonRsp(int sitId, int token, int rspError, Json::Value &jsonRspData,
            bool debug_print = false);
    static ModemData *BuildJsonInd(int sitId, Json::Value &jsonIndData, bool debug_print = false);
};

#endif // __MODEM_DATA_BUILDER_H__
