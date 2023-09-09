/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef    __PROTOCOL_SMS_ADAPTER_INTERFACE_H__
#define    __PROTOCOL_SMS_ADAPTER_INTERFACE_H__

#include <telephony/ril.h>
#include <base/types.h>

class ProtocolSendSmsRespAdapterInterface {
public:
    ProtocolSendSmsRespAdapterInterface() {}
    virtual ~ProtocolSendSmsRespAdapterInterface() = default;

public:
    virtual int GetRef() const = 0;
    virtual int GetSmsRspErrorCode() const = 0;
    virtual int GetPduSize() const = 0;
    virtual const char *GetPdu() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolWriteSmsToSimRespAdapterInterface {
public:
    ProtocolWriteSmsToSimRespAdapterInterface() {}
    virtual ~ProtocolWriteSmsToSimRespAdapterInterface() = default;

public:
    virtual int GetIndex() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSmsCapacityOnSimRespAdapterInterface {
public:
    ProtocolSmsCapacityOnSimRespAdapterInterface() {}
    virtual ~ProtocolSmsCapacityOnSimRespAdapterInterface() = default;

public:
    virtual int GetSimId() const = 0;
    virtual int GetTotalNum() const = 0;
    virtual int GetUsedNum() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSmscAddrRespAdapterInterface {
public:
    ProtocolSmscAddrRespAdapterInterface() {}
    virtual ~ProtocolSmscAddrRespAdapterInterface() = default;

public:
    virtual const char *GetSmsc() const = 0;
    virtual int GetSmscLength() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolNewSmsIndAdapterInterface {
public:
    ProtocolNewSmsIndAdapterInterface() {}
    virtual ~ProtocolNewSmsIndAdapterInterface() = default;

public:
    virtual int GetPduSize() const = 0;
    virtual const char *GetPdu() const = 0;
    virtual int GetTpid() const = 0;
};

class ProtocolNewSmsOnSimIndAdapterInterface {
public:
    ProtocolNewSmsOnSimIndAdapterInterface() {}
    virtual ~ProtocolNewSmsOnSimIndAdapterInterface() = default;

public:
    virtual int GetIndex() const = 0;
};

class ProtocolGetBcstSmsConfRespAdapterInterface {
public:
    ProtocolGetBcstSmsConfRespAdapterInterface() {}
    virtual ~ProtocolGetBcstSmsConfRespAdapterInterface() = default;

    virtual UINT8 GetConfigsNumber() = 0;
    virtual RIL_GSM_BroadcastSmsConfigInfo** GetConfigsInfoPointers() = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolNewBcstSmsAdapterInterface {
public:
    ProtocolNewBcstSmsAdapterInterface() {}
    virtual ~ProtocolNewBcstSmsAdapterInterface() = default;

public:
    virtual BYTE *GetBcst() const = 0;
    virtual UINT16 GetBcstLen() const = 0;
};

class ProtocolCdmaSendSmsRespAdapterInterface {
public:
    ProtocolCdmaSendSmsRespAdapterInterface() {}
    virtual ~ProtocolCdmaSendSmsRespAdapterInterface() = default;

    virtual int GetRef() = 0;
    virtual int GetSmsRspErrorClass() = 0;
    virtual int GetSmsRspCauseCode() = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolCdmaNewSmsIndAdapterInterface {
public:
    ProtocolCdmaNewSmsIndAdapterInterface() {}
    virtual ~ProtocolCdmaNewSmsIndAdapterInterface() = default;

    virtual int GetMessageLength() = 0;
    virtual const RIL_CDMA_SMS_Message* GetRilCdmaSmsMsg() const = 0;
    virtual int GetTpid() const = 0;
};

class ProtocolCdmaWriteSmsToRuimRespAdapterInterface {
public:
    ProtocolCdmaWriteSmsToRuimRespAdapterInterface() {}
    virtual ~ProtocolCdmaWriteSmsToRuimRespAdapterInterface() = default;

    virtual int GetIndex() = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolGetCdmaBcstSmsConfRespAdapterInterface {
public:
    ProtocolGetCdmaBcstSmsConfRespAdapterInterface() {}
    virtual ~ProtocolGetCdmaBcstSmsConfRespAdapterInterface() = default;

    virtual UINT8 GetConfigsNumber() = 0;
    virtual RIL_CDMA_BroadcastSmsConfigInfo** GetConfigsInfoPointers() = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface {
public:
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface() {}
    virtual ~ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface() = default;

    virtual int GetMessageLength() = 0;
    virtual const RIL_CDMA_SMS_Message* GetRilCdmaSmsMsg() const = 0;
};
#endif // __PROTOCOL_SMS_ADAPTER_INTERFACE_H__
