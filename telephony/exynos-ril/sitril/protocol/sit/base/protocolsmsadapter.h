/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef    __PROTOCOL_SMS_ADAPTER_H__
#define    __PROTOCOL_SMS_ADAPTER_H__

#include "protocolsmsadapterinterface.h"

class ModemData;

class ProtocolSendSmsRespAdapter : public ProtocolSendSmsRespAdapterInterface {
private:
    ProtocolSendSmsRespAdapterInterface *protocolSendSmsRespAdapterInterface = NULL;

public:
    ProtocolSendSmsRespAdapter(const ModemData *pModemData);
    ProtocolSendSmsRespAdapter(const ProtocolSendSmsRespAdapter&) = delete;
    ProtocolSendSmsRespAdapter& operator=(ProtocolSendSmsRespAdapter const&) = delete;
    virtual ~ProtocolSendSmsRespAdapter();

public:
    int GetRef() const;
    int GetSmsRspErrorCode() const;
    int GetPduSize() const;
    const char *GetPdu() const;
    UINT GetErrorCode() const { return protocolSendSmsRespAdapterInterface->GetErrorCode(); }
};

class ProtocolWriteSmsToSimRespAdapter : public ProtocolWriteSmsToSimRespAdapterInterface {
private:
    ProtocolWriteSmsToSimRespAdapterInterface *protocolWriteSmsToSimRespAdapterInterface = NULL;

public:
    ProtocolWriteSmsToSimRespAdapter(const ModemData *pModemData);
    ProtocolWriteSmsToSimRespAdapter(const ProtocolWriteSmsToSimRespAdapter&) = delete;
    ProtocolWriteSmsToSimRespAdapter& operator=(ProtocolWriteSmsToSimRespAdapter const&) = delete;
    virtual ~ProtocolWriteSmsToSimRespAdapter();

public:
    int GetIndex() const;
    UINT GetErrorCode() const { return protocolWriteSmsToSimRespAdapterInterface->GetErrorCode(); }
};

class ProtocolSmsCapacityOnSimRespAdapter : public ProtocolSmsCapacityOnSimRespAdapterInterface {
private:
    ProtocolSmsCapacityOnSimRespAdapterInterface *protocolSmsCapacityOnSimRespAdapterInterface = NULL;

public:
    ProtocolSmsCapacityOnSimRespAdapter(const ModemData *pModemData);
    ProtocolSmsCapacityOnSimRespAdapter(const ProtocolSmsCapacityOnSimRespAdapter&) = delete;
    ProtocolSmsCapacityOnSimRespAdapter& operator=(ProtocolSmsCapacityOnSimRespAdapter const&) = delete;
    virtual ~ProtocolSmsCapacityOnSimRespAdapter();

public:
    int GetSimId() const;
    int GetTotalNum() const;
    int GetUsedNum() const;
    UINT GetErrorCode() const { return protocolSmsCapacityOnSimRespAdapterInterface->GetErrorCode(); }
};

class ProtocolSmscAddrRespAdapter : public ProtocolSmscAddrRespAdapterInterface {
private:
    ProtocolSmscAddrRespAdapterInterface *protocolSmscAddrRespAdapterInterface = NULL;

public:
    ProtocolSmscAddrRespAdapter(const ModemData *pModemData);
    ProtocolSmscAddrRespAdapter(const ProtocolSmscAddrRespAdapter&) = delete;
    ProtocolSmscAddrRespAdapter& operator=(ProtocolSmscAddrRespAdapter const&) = delete;
    virtual ~ProtocolSmscAddrRespAdapter();

public:
    const char *GetSmsc() const;
    int GetSmscLength() const;
    UINT GetErrorCode() const { return protocolSmscAddrRespAdapterInterface->GetErrorCode(); }
};

class ProtocolNewSmsIndAdapter : public ProtocolNewSmsIndAdapterInterface {
private:
    ProtocolNewSmsIndAdapterInterface *protocolNewSmsIndAdapterInterface = NULL;

public:
    ProtocolNewSmsIndAdapter(const ModemData *pModemData);
    ProtocolNewSmsIndAdapter(const ProtocolNewSmsIndAdapter&) = delete;
    ProtocolNewSmsIndAdapter& operator=(ProtocolNewSmsIndAdapter const&) = delete;
    virtual ~ProtocolNewSmsIndAdapter();

public:
    int GetPduSize() const;
    const char *GetPdu() const;
    int GetTpid() const;
};

typedef ProtocolNewSmsIndAdapter ProtocolSmsStatusReportIndAdapter;

class ProtocolNewSmsOnSimIndAdapter : public ProtocolNewSmsOnSimIndAdapterInterface {
private:
    ProtocolNewSmsOnSimIndAdapterInterface *protocolNewSmsOnSimIndAdapterInterface = NULL;

public:
    ProtocolNewSmsOnSimIndAdapter(const ModemData *pModemData);
    ProtocolNewSmsOnSimIndAdapter(const ProtocolNewSmsOnSimIndAdapter&) = delete;
    ProtocolNewSmsOnSimIndAdapter& operator=(ProtocolNewSmsOnSimIndAdapter const&) = delete;
    virtual ~ProtocolNewSmsOnSimIndAdapter();

public:
    int GetIndex() const;
};

class ProtocolGetBcstSmsConfRespAdapter : public ProtocolGetBcstSmsConfRespAdapterInterface {
private:
    ProtocolGetBcstSmsConfRespAdapterInterface *protocolGetBcstSmsConfRespAdapterInterface = NULL;

public:
    ProtocolGetBcstSmsConfRespAdapter(const ModemData *pModemData);
    ProtocolGetBcstSmsConfRespAdapter(const ProtocolGetBcstSmsConfRespAdapter&) = delete;
    ProtocolGetBcstSmsConfRespAdapter& operator=(ProtocolGetBcstSmsConfRespAdapter const&) = delete;
    virtual ~ProtocolGetBcstSmsConfRespAdapter();

    UINT8 GetConfigsNumber();
    RIL_GSM_BroadcastSmsConfigInfo** GetConfigsInfoPointers();
    UINT GetErrorCode() const { return protocolGetBcstSmsConfRespAdapterInterface->GetErrorCode(); }
};

class ProtocolNewBcstSmsAdapter : public ProtocolNewBcstSmsAdapterInterface {
private:
    ProtocolNewBcstSmsAdapterInterface *protocolNewBcstSmsAdapterInterface = NULL;

public:
    ProtocolNewBcstSmsAdapter(const ModemData *pModemData);
    ProtocolNewBcstSmsAdapter(const ProtocolNewBcstSmsAdapter&) = delete;
    ProtocolNewBcstSmsAdapter& operator=(ProtocolNewBcstSmsAdapter const&) = delete;
    virtual ~ProtocolNewBcstSmsAdapter();

public:
    BYTE *GetBcst() const;
    UINT16 GetBcstLen() const;
};

class ProtocolCdmaSendSmsRespAdapter : public ProtocolCdmaSendSmsRespAdapterInterface {
private:
    ProtocolCdmaSendSmsRespAdapterInterface *protocolCdmaSendSmsRespAdapterInterface = NULL;

public:
    ProtocolCdmaSendSmsRespAdapter(const ModemData *pModemData);
    ProtocolCdmaSendSmsRespAdapter(const ProtocolCdmaSendSmsRespAdapter&) = delete;
    ProtocolCdmaSendSmsRespAdapter& operator=(ProtocolCdmaSendSmsRespAdapter const&) = delete;
    virtual ~ProtocolCdmaSendSmsRespAdapter();

    int GetRef();
    int GetSmsRspErrorClass();
    int GetSmsRspCauseCode();
    UINT GetErrorCode() const { return protocolCdmaSendSmsRespAdapterInterface->GetErrorCode(); }
};

class ProtocolCdmaNewSmsIndAdapter : public ProtocolCdmaNewSmsIndAdapterInterface {
private:
    ProtocolCdmaNewSmsIndAdapterInterface *protocolCdmaNewSmsIndAdapterInterface = NULL;

public:
    ProtocolCdmaNewSmsIndAdapter(const ModemData *pModemData);
    ProtocolCdmaNewSmsIndAdapter(const ProtocolCdmaNewSmsIndAdapter&) = delete;
    ProtocolCdmaNewSmsIndAdapter& operator=(ProtocolCdmaNewSmsIndAdapter const&) = delete;
    virtual ~ProtocolCdmaNewSmsIndAdapter();

    int GetMessageLength();
    const RIL_CDMA_SMS_Message* GetRilCdmaSmsMsg() const;
    int GetTpid() const;
};

class ProtocolCdmaWriteSmsToRuimRespAdapter : public ProtocolCdmaWriteSmsToRuimRespAdapterInterface {
private:
    ProtocolCdmaWriteSmsToRuimRespAdapterInterface *protocolCdmaWriteSmsToRuimRespAdapterInterface = NULL;

public:
    ProtocolCdmaWriteSmsToRuimRespAdapter(const ModemData *pModemData);
    ProtocolCdmaWriteSmsToRuimRespAdapter(const ProtocolCdmaWriteSmsToRuimRespAdapter&) = delete;
    ProtocolCdmaWriteSmsToRuimRespAdapter& operator=(ProtocolCdmaWriteSmsToRuimRespAdapter const&) = delete;
    virtual ~ProtocolCdmaWriteSmsToRuimRespAdapter();

    int GetIndex();
    UINT GetErrorCode() const { return protocolCdmaWriteSmsToRuimRespAdapterInterface->GetErrorCode(); }
};

class ProtocolGetCdmaBcstSmsConfRespAdapter : public ProtocolGetCdmaBcstSmsConfRespAdapterInterface {
private:
    ProtocolGetCdmaBcstSmsConfRespAdapterInterface *protocolGetCdmaBcstSmsConfRespAdapterInterface = NULL;

public:
    ProtocolGetCdmaBcstSmsConfRespAdapter(const ModemData *pModemData);
    ProtocolGetCdmaBcstSmsConfRespAdapter(const ProtocolGetCdmaBcstSmsConfRespAdapter&) = delete;
    ProtocolGetCdmaBcstSmsConfRespAdapter& operator=(ProtocolGetCdmaBcstSmsConfRespAdapter const&) = delete;
    virtual ~ProtocolGetCdmaBcstSmsConfRespAdapter();

    UINT8 GetConfigsNumber();
    RIL_CDMA_BroadcastSmsConfigInfo** GetConfigsInfoPointers();
    UINT GetErrorCode() const { return protocolGetCdmaBcstSmsConfRespAdapterInterface->GetErrorCode(); }
};

class ProtocolCdmaVoiceMsgWaitingInfoIndAdapter : public ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface {
private:
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface *protocolCdmaVoiceMsgWaitingInfoIndAdapterInterface = NULL;

public:
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapter(const ModemData *pModemData);
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapter(const ProtocolCdmaVoiceMsgWaitingInfoIndAdapter&) = delete;
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapter& operator=(ProtocolCdmaVoiceMsgWaitingInfoIndAdapter const&) = delete;
    virtual ~ProtocolCdmaVoiceMsgWaitingInfoIndAdapter();

    int GetMessageLength();
    const RIL_CDMA_SMS_Message* GetRilCdmaSmsMsg() const;
};
#endif // __PROTOCOL_SMS_ADAPTER_H__
