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
#include <string>
#include <iostream>

#include "modemdatabuilder.h"
#include "base/types.h"
#include "modemdata.h"
#include "sitdef.h"

using std::string;

ModemDataBuilder::ModemDataBuilder()
    : mType(-1), mData(0), mDataLen(0)
    , mToken((uint32_t)-1), mId(0), mErrorCode(0) {
}

ModemDataBuilder::~ModemDataBuilder() {
    if (mData) {
        delete[] mData;
    }
}

ModemDataBuilder& ModemDataBuilder::SetType(int type) {
    switch (type) {
    case RCM_TYPE_REQUEST:
    case RCM_TYPE_RESPONSE:
    case RCM_TYPE_INDICATION:
        mType = type;
        break;
    default:
        break;
    }
    return *this;
}

ModemDataBuilder& ModemDataBuilder::SetId(uint16_t id) {
    if (mType >= 0) {
        mId = id;
    }
    return *this;
}

ModemDataBuilder& ModemDataBuilder::SetToken(uint32_t token) {
    if (mType == RCM_TYPE_REQUEST || mType == RCM_TYPE_RESPONSE) {
        mToken = token;
    }
    return *this;
}

ModemDataBuilder& ModemDataBuilder::SetErrorCode(BYTE errorCode) {
    if (mType == RCM_TYPE_RESPONSE) {
        mErrorCode = errorCode;
    }
    return *this;
}

ModemDataBuilder& ModemDataBuilder::SetParameter(void *data, size_t dataLen) {
    if (data && dataLen > 0) {
        mData = new char[dataLen];
        if (mData) {
            memcpy(mData, data, dataLen);
            mDataLen = dataLen;
        }
    }
    return *this;
}

ModemData *ModemDataBuilder::Build() {
    if (mType < RCM_TYPE_REQUEST || mType > RCM_TYPE_INDICATION) {
        return nullptr;
    }

    union _HEADER{
        RCM_HEADER sol;
        RCM_IND_HEADER unsol;
        RCM_GEN_HEADER hdr;
    } header;
    memset(&header, 0, sizeof(header));

    size_t totalSize = 0, headerSize = 0;
    if (mType == RCM_TYPE_INDICATION) {
        headerSize = sizeof(RCM_IND_HEADER);
    } else {
        headerSize = sizeof(RCM_HEADER);
    }
    totalSize = headerSize + mDataLen;

    char *p = new char[totalSize];
    ModemData *ret = nullptr;
    if (p) {
        // fill data
        header.hdr.type = mType;
        header.hdr.id = mId;
        header.hdr.length = totalSize;

        if (mType == RCM_TYPE_REQUEST) {
            header.sol.ext.req.token = mToken;
        } else if (mType == RCM_TYPE_RESPONSE) {
            header.sol.ext.rsp.token = mToken;
            header.sol.ext.rsp.error = mErrorCode;
        }

        // copy header
        memcpy(p, &header, headerSize);

        // copy parameters
        if (mData) {
            memcpy(p + headerSize, mData, mDataLen);
        }

        ret = new ModemData(p, totalSize);
        delete[] p;
    }
    return ret;
}


void ModemDataBuilder::Reset() {
    mType = -1;
    mId = 0;
    mToken = (uint32_t)-1;
    mErrorCode = 0;
    if (mData) {
        delete[] mData;
    }
    mDataLen = 0;
}

ModemData *ModemDataBuilder::BuildRequest(uint16_t id, uint32_t token, void *data, size_t dataLen) {
    ModemDataBuilder builder;
    return builder.SetType(RCM_TYPE_REQUEST)
            .SetId(id)
            .SetToken(token)
            .SetParameter(data, dataLen).Build();
}

ModemData *ModemDataBuilder::BuildResponse(uint16_t id, uint32_t token, BYTE errorCode, void *data, size_t dataLen) {
    ModemDataBuilder builder;
    return builder.SetType(RCM_TYPE_RESPONSE)
            .SetId(id)
            .SetToken(token)
            .SetErrorCode(errorCode)
            .SetParameter(data, dataLen).Build();
}

ModemData *ModemDataBuilder::BuildIndication(uint16_t id, void *data, size_t dataLen) {
    ModemDataBuilder builder;
    return builder.SetType(RCM_TYPE_INDICATION)
        .SetId(id)
        .SetParameter(data, dataLen).Build();
}

ModemData *ModemDataBuilder::BuildJsonRsp(int sitId, int token, int rspError, Json::Value &jsonRspData,
        bool debug_print) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    string jsonRspString = Json::writeString(builder, jsonRspData);
    const char *rspData = jsonRspString.c_str();
    int rspDataLen = strlen(rspData) + 1;
    if (debug_print) printf("[0x%x] json rsp data = %s\n", sitId, rspData);

    ModemData *modemData = ModemDataBuilder::BuildResponse(
            sitId, token, (BYTE)rspError, (void *)rspData, rspDataLen);

    return modemData;
}

ModemData *ModemDataBuilder::BuildJsonInd(int sitId, Json::Value &jsonIndData, bool debug_print) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    string jsonIndString = Json::writeString(builder, jsonIndData);
    const char *indData = jsonIndString.c_str();
    int indDataLen = strlen(indData) + 1;
    if (debug_print) printf("[0x%x] json ind data = %s\n", sitId, indData);

    ModemData *modemData = ModemDataBuilder::BuildIndication(
            sitId, (void *)indData, indDataLen);

    return modemData;
}
