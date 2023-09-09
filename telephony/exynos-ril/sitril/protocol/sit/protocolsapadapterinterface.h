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
 * protocolsapadapterinterface.h
 *
 *  Created on: 2021. 8. 2.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SAP_ADAPTER_INTERFACE_H__
#define __PROTOCOL_SAP_ADAPTER_INTERFACE_H__

#include <base/types.h>

class ProtocolSapConnectAdapterInterface {
public:
    ProtocolSapConnectAdapterInterface() = default;
    virtual ~ProtocolSapConnectAdapterInterface() = default;
public:
    virtual int GetStatus() const = 0;
    virtual int GetMaxMsgSize() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSapDisconnectAdapterInterface {
public:
    ProtocolSapDisconnectAdapterInterface() = default;
    virtual ~ProtocolSapDisconnectAdapterInterface() = default;
public:
    virtual int GetStatus() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

// ind
class ProtocolSapTransferApduAdapterInterface {
public:
    ProtocolSapTransferApduAdapterInterface() = default;
    virtual ~ProtocolSapTransferApduAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;
    virtual int GetApduLen() const = 0;
    virtual const char *GetApdu() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSapTransferAtrAdapterInterface {
public:
    ProtocolSapTransferAtrAdapterInterface() = default;
    virtual ~ProtocolSapTransferAtrAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;
    virtual int GetAtrLen() const = 0;
    virtual const char *GetAtr() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSapPowerSimOnOffAdapterInterface {
public:
    ProtocolSapPowerSimOnOffAdapterInterface() = default;
    virtual ~ProtocolSapPowerSimOnOffAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSapResetSimAdapterInterface {
public:
    ProtocolSapResetSimAdapterInterface() = default;
    virtual ~ProtocolSapResetSimAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSapTransferCardReaderStatusAdapterInterface {
public:
    ProtocolSapTransferCardReaderStatusAdapterInterface() = default;
    virtual ~ProtocolSapTransferCardReaderStatusAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSapSetTranportProtocolAdapterInterface {
public:
    ProtocolSapSetTranportProtocolAdapterInterface() = default;
    virtual ~ProtocolSapSetTranportProtocolAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSapDisconnectTypeAdapterInterface {
public:
    ProtocolSapDisconnectTypeAdapterInterface() = default;
    virtual ~ProtocolSapDisconnectTypeAdapterInterface() = default;
public:
    virtual int GetDisconStatus() const = 0;
};

class ProtocolSapStatusAdapterInterface {
public:
    ProtocolSapStatusAdapterInterface() = default;
    virtual ~ProtocolSapStatusAdapterInterface() = default;
public:
    virtual int GetCardStatus() const = 0;
};

#endif /* __PROTOCOL_SAP_ADAPTER_INTERFACE_H__ */
