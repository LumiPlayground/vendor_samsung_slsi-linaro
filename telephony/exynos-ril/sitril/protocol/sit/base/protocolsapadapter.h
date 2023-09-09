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
 * protocolsapadapter.h
 *
 *  Created on: 2021. 08. 2.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SAP_ADAPTER_H__
#define __PROTOCOL_SAP_ADAPTER_H__

#include "protocolsapadapterinterface.h"

class ModemData;

class ProtocolSapConnectAdapter : public ProtocolSapConnectAdapterInterface {
private:
    ProtocolSapConnectAdapterInterface *protocolSapConnectAdapterInterface = NULL;
public:
    ProtocolSapConnectAdapter(const ModemData *pModemData);
    ProtocolSapConnectAdapter(const ProtocolSapConnectAdapter&) = delete;
    ProtocolSapConnectAdapter& operator=(ProtocolSapConnectAdapter const&) = delete;
    virtual ~ProtocolSapConnectAdapter();
public:
    int GetStatus() const;
    int GetMaxMsgSize() const;
    UINT GetErrorCode() const { return protocolSapConnectAdapterInterface->GetErrorCode(); }
};

class ProtocolSapDisconnectAdapter : public ProtocolSapDisconnectAdapterInterface {
private:
    ProtocolSapDisconnectAdapterInterface *protocolSapDisconnectAdapterInterface = NULL;
public:
    ProtocolSapDisconnectAdapter(const ModemData *pModemData);
    ProtocolSapDisconnectAdapter(const ProtocolSapDisconnectAdapter&) = delete;
    ProtocolSapDisconnectAdapter& operator=(ProtocolSapDisconnectAdapter const&) = delete;
    virtual ~ProtocolSapDisconnectAdapter();
public:
    int GetStatus() const;
    UINT GetErrorCode() const { return protocolSapDisconnectAdapterInterface->GetErrorCode(); }
};

class ProtocolSapTransferApduAdapter : public ProtocolSapTransferApduAdapterInterface {
private:
    ProtocolSapTransferApduAdapterInterface *protocolSapTransferApduAdapterInterface = NULL;
public:
    ProtocolSapTransferApduAdapter(const ModemData *pModemData);
    ProtocolSapTransferApduAdapter(const ProtocolSapTransferApduAdapter&) = delete;
    ProtocolSapTransferApduAdapter& operator=(ProtocolSapTransferApduAdapter const&) = delete;
    virtual ~ProtocolSapTransferApduAdapter();
public:
    int GetResult() const;
    int GetApduLen() const;
    const char *GetApdu() const;
    UINT GetErrorCode() const { return protocolSapTransferApduAdapterInterface->GetErrorCode(); }
};

class ProtocolSapTransferAtrAdapter : public ProtocolSapTransferAtrAdapterInterface {
private:
    ProtocolSapTransferAtrAdapterInterface *protocolSapTransferAtrAdapterInterface = NULL;
public:
    ProtocolSapTransferAtrAdapter(const ModemData *pModemData);
    ProtocolSapTransferAtrAdapter(const ProtocolSapTransferAtrAdapter&) = delete;
    ProtocolSapTransferAtrAdapter& operator=(ProtocolSapTransferAtrAdapter const&) = delete;
    virtual ~ProtocolSapTransferAtrAdapter();
public:
    int GetResult() const;
    int GetAtrLen() const;
    const char *GetAtr() const;
    UINT GetErrorCode() const { return protocolSapTransferAtrAdapterInterface->GetErrorCode(); }
};

class ProtocolSapPowerSimOnOffAdapter : public ProtocolSapPowerSimOnOffAdapterInterface {
private:
    ProtocolSapPowerSimOnOffAdapterInterface *protocolSapPowerSimOnOffAdapterInterface = NULL;
public:
    ProtocolSapPowerSimOnOffAdapter(const ModemData *pModemData);
    ProtocolSapPowerSimOnOffAdapter(const ProtocolSapPowerSimOnOffAdapter&) = delete;
    ProtocolSapPowerSimOnOffAdapter& operator=(ProtocolSapPowerSimOnOffAdapter const&) = delete;
    virtual ~ProtocolSapPowerSimOnOffAdapter();
public:
    int GetResult() const;
    UINT GetErrorCode() const { return protocolSapPowerSimOnOffAdapterInterface->GetErrorCode(); }
};

class ProtocolSapResetSimAdapter : public ProtocolSapResetSimAdapterInterface {
private:
    ProtocolSapResetSimAdapterInterface *protocolSapResetSimAdapterInterface = NULL;
public:
    ProtocolSapResetSimAdapter(const ModemData *pModemData);
    ProtocolSapResetSimAdapter(const ProtocolSapResetSimAdapter&) = delete;
    ProtocolSapResetSimAdapter& operator=(ProtocolSapResetSimAdapter const&) = delete;
    virtual ~ProtocolSapResetSimAdapter();
public:
    int GetResult() const;
    UINT GetErrorCode() const { return protocolSapResetSimAdapterInterface->GetErrorCode(); }
};

class ProtocolSapTransferCardReaderStatusAdapter : public ProtocolSapTransferCardReaderStatusAdapterInterface {
private:
    ProtocolSapTransferCardReaderStatusAdapterInterface *protocolSapTransferCardReaderStatusAdapterInterface = NULL;
public:
    ProtocolSapTransferCardReaderStatusAdapter(const ModemData *pModemData);
    ProtocolSapTransferCardReaderStatusAdapter(const ProtocolSapTransferCardReaderStatusAdapter&) = delete;
    ProtocolSapTransferCardReaderStatusAdapter& operator=(ProtocolSapTransferCardReaderStatusAdapter const&) = delete;
    virtual ~ProtocolSapTransferCardReaderStatusAdapter();
public:
    int GetResult() const;
    UINT GetErrorCode() const { return protocolSapTransferCardReaderStatusAdapterInterface->GetErrorCode(); }
};

class ProtocolSapSetTranportProtocolAdapter : public ProtocolSapSetTranportProtocolAdapterInterface {
private:
    ProtocolSapSetTranportProtocolAdapterInterface *protocolSapSetTranportProtocolAdapterInterface = NULL;
public:
    ProtocolSapSetTranportProtocolAdapter(const ModemData *pModemData);
    ProtocolSapSetTranportProtocolAdapter(const ProtocolSapSetTranportProtocolAdapter&) = delete;
    ProtocolSapSetTranportProtocolAdapter& operator=(ProtocolSapSetTranportProtocolAdapter const&) = delete;
    virtual ~ProtocolSapSetTranportProtocolAdapter();
public:
    int GetResult() const;
    UINT GetErrorCode() const { return protocolSapSetTranportProtocolAdapterInterface->GetErrorCode(); }
};

class ProtocolSapDisconnectTypeAdapter : public ProtocolSapDisconnectTypeAdapterInterface {
private:
    ProtocolSapDisconnectTypeAdapterInterface *protocolSapDisconnectTypeAdapterInterface = NULL;
public:
    ProtocolSapDisconnectTypeAdapter(const ModemData *pModemData);
    ProtocolSapDisconnectTypeAdapter(const ProtocolSapDisconnectTypeAdapter&) = delete;
    ProtocolSapDisconnectTypeAdapter& operator=(ProtocolSapDisconnectTypeAdapter const&) = delete;
    virtual ~ProtocolSapDisconnectTypeAdapter();
public:
    int GetDisconStatus() const;
};

class ProtocolSapStatusAdapter : public ProtocolSapStatusAdapterInterface {
private:
    ProtocolSapStatusAdapterInterface *protocolSapStatusAdapterInterface = NULL;
public:
    ProtocolSapStatusAdapter(const ModemData *pModemData);
    ProtocolSapStatusAdapter(const ProtocolSapStatusAdapter&) = delete;
    ProtocolSapStatusAdapter& operator=(ProtocolSapStatusAdapter const&) = delete;
    virtual ~ProtocolSapStatusAdapter();
public:
    int GetCardStatus() const;
};

#endif /* __PROTOCOL_SAP_ADAPTER_H__ */
