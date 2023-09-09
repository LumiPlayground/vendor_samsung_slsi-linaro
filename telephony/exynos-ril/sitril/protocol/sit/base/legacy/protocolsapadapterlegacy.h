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
 * protocolsapadapterlegacy.h
 *
 *  Created on: 2019. 3. 13.
 *      Author: ji_young.bae
 */

#ifndef __PROTOCOL_SAP_ADAPTER_LEGACY_H__
#define __PROTOCOL_SAP_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolsapadapterinterface.h"

class ProtocolSapConnectAdapterLegacy : public ProtocolRespAdapter, public ProtocolSapConnectAdapterInterface {
  private:
    int m_status;
    int m_maxMsgSize;

  public:
    ProtocolSapConnectAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSapConnectAdapterLegacy() = default;

  public:
    int GetStatus() const {
        return m_status;
    }
    int GetMaxMsgSize() const {
        return m_maxMsgSize;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapDisconnectAdapterLegacy : public ProtocolRespAdapter, public ProtocolSapDisconnectAdapterInterface {
  public:
    ProtocolSapDisconnectAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSapDisconnectAdapterLegacy() = default;

  public:
    int GetStatus() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

// ind
class ProtocolSapTransferApduAdapterLegacy : public ProtocolRespAdapter, public ProtocolSapTransferApduAdapterInterface {
  private:
    int m_result;
    int m_apduLen;
    char m_apdu[513];

  public:
    ProtocolSapTransferApduAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSapTransferApduAdapterLegacy() = default;

  public:
    int GetResult() const {
        return m_result;
    }
    int GetApduLen() const {
        return m_apduLen;
    }
    const char *GetApdu() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapTransferAtrAdapterLegacy : public ProtocolRespAdapter, public ProtocolSapTransferAtrAdapterInterface {
  private:
    int m_result;
    int m_atrLen;
    char m_atr[34];

  public:
    ProtocolSapTransferAtrAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSapTransferAtrAdapterLegacy() = default;

  public:
    int GetResult() const {
        return m_result;
    }
    int GetAtrLen() const {
        return m_atrLen;
    }
    const char *GetAtr() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapPowerSimOnOffAdapterLegacy : public ProtocolRespAdapter, public ProtocolSapPowerSimOnOffAdapterInterface {
  public:
    ProtocolSapPowerSimOnOffAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSapPowerSimOnOffAdapterLegacy() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapResetSimAdapterLegacy : public ProtocolRespAdapter, public ProtocolSapResetSimAdapterInterface {
  public:
    ProtocolSapResetSimAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSapResetSimAdapterLegacy() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapTransferCardReaderStatusAdapterLegacy : public ProtocolRespAdapter, public ProtocolSapTransferCardReaderStatusAdapterInterface {
  public:
    ProtocolSapTransferCardReaderStatusAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSapTransferCardReaderStatusAdapterLegacy() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapSetTranportProtocolAdapterLegacy : public ProtocolRespAdapter, public ProtocolSapSetTranportProtocolAdapterInterface {
  public:
    ProtocolSapSetTranportProtocolAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSapSetTranportProtocolAdapterLegacy() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapDisconnectTypeAdapterLegacy : public ProtocolIndAdapter, public ProtocolSapDisconnectTypeAdapterInterface {
  public:
    ProtocolSapDisconnectTypeAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolSapDisconnectTypeAdapterLegacy() = default;

  public:
    int GetDisconStatus() const;
};

class ProtocolSapStatusAdapterLegacy : public ProtocolIndAdapter, public ProtocolSapStatusAdapterInterface {
  public:
    ProtocolSapStatusAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolSapStatusAdapterLegacy() = default;

  public:
    int GetCardStatus() const;
};

#endif /* __PROTOCOL_SAP_ADAPTER_LEGACY_H__ */
