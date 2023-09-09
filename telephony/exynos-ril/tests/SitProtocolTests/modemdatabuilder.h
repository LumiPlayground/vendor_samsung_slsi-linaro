#ifndef __MODEM_DATA_BUILDER_H__
#define __MODEM_DATA_BUILDER_H__

#include "json/json.h"

class ModemData;

class ModemDataBuilder {
private:
    int mType;
    char *mData;
    size_t mDataLen;
    uint32_t mToken;
    int mId;
    int mErrorCode;
public:
    ModemDataBuilder();
    ~ModemDataBuilder();
    ModemDataBuilder& SetType(int type);
    ModemDataBuilder& SetId(int id);
    ModemDataBuilder& SetToken(uint32_t token);
    ModemDataBuilder& SetErrorCode(int errorCode);
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
