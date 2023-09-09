/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef    __PROTOCOL_ADAPTER_H__
#define    __PROTOCOL_ADAPTER_H__

#include <base/types.h>
#include "constdef.h"

class ModemData;

class ProtocolBaseAdapter {
protected:
    const ModemData *m_pModemData;

public:
    ProtocolBaseAdapter(const ModemData *pModemData) : m_pModemData(pModemData) {}
    virtual ~ProtocolBaseAdapter() {}

public:
    UINT GetType() const;
    UINT GetId() const;
    UINT GetLength() const;

    UINT GetToken() const;
    const char *GetParameter() const;
    UINT GetParameterLength() const;

    bool IsRequest() const;
    bool IsResponse() const;
    bool IsUnsolicitedResponse() const;
    bool IsSolicitedResponse() const;
};

class ProtocolReqAdapter : public ProtocolBaseAdapter {
public:
    ProtocolReqAdapter(const ModemData *pModemData) : ProtocolBaseAdapter(pModemData) {}
    virtual ~ProtocolReqAdapter() {}
};

class ProtocolRespAdapter : public ProtocolBaseAdapter {
public:
    ProtocolRespAdapter(const ModemData *pModemData) : ProtocolBaseAdapter(pModemData) {}
    virtual ~ProtocolRespAdapter() {}

public:
    UINT GetErrorCode() const;
};

class ProtocolIndAdapter : public ProtocolBaseAdapter {
public:
    ProtocolIndAdapter(const ModemData *pModemData) : ProtocolBaseAdapter(pModemData) {}
    virtual ~ProtocolIndAdapter() {}

public:
    const char *GetParameter() const;
    UINT GetParameterLength() const;
    UINT GetToken() const { return TOKEN_INVALID; }
};

#endif // __PROTOCOL_ADAPTER_H__
