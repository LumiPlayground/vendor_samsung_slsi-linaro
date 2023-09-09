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
 * protocolsapadapterjson.h
 *
 *  Created on: 2021. 8. 3.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SAP_ADAPTER_JSON_H__
#define __PROTOCOL_SAP_ADAPTER_JSON_H__

#include "protocoljsonadapter.h"
#include "protocolsapadapterinterface.h"
#include "json/json.h"

class ProtocolSapConnectAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSapConnectAdapterInterface {
  public:
    ProtocolSapConnectAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapConnectAdapterJson() = default;

  public:
    int GetStatus() const;
    int GetMaxMsgSize() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapDisconnectAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSapDisconnectAdapterInterface {
  public:
    ProtocolSapDisconnectAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapDisconnectAdapterJson() = default;

  public:
    int GetStatus() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapTransferApduAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSapTransferApduAdapterInterface {
  private:
    char m_apdu[513];

  public:
    ProtocolSapTransferApduAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapTransferApduAdapterJson() = default;

  public:
    int GetResult() const;
    int GetApduLen() const;
    const char *GetApdu() const {
        return (m_apdu[0] == 0 ? NULL : m_apdu);
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void Init();
    void SetApduData();
};

class ProtocolSapTransferAtrAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSapTransferAtrAdapterInterface {
  private:
    char m_atr[34];

  public:
    ProtocolSapTransferAtrAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapTransferAtrAdapterJson() = default;

  public:
    int GetResult() const;
    int GetAtrLen() const;
    const char *GetAtr() const {
        return (m_atr[0] == 0 ? NULL : m_atr);
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void Init();
    void SetAtrData();
};

class ProtocolSapPowerSimOnOffAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSapPowerSimOnOffAdapterInterface {
  public:
    ProtocolSapPowerSimOnOffAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapPowerSimOnOffAdapterJson() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void Init();
};

class ProtocolSapResetSimAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSapResetSimAdapterInterface {
  public:
    ProtocolSapResetSimAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapResetSimAdapterJson() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapTransferCardReaderStatusAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSapTransferCardReaderStatusAdapterInterface {
  public:
    ProtocolSapTransferCardReaderStatusAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapTransferCardReaderStatusAdapterJson() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapSetTranportProtocolAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSapSetTranportProtocolAdapterInterface {
  public:
    ProtocolSapSetTranportProtocolAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapSetTranportProtocolAdapterJson() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSapDisconnectTypeAdapterJson : public ProtocolJsonIndAdapter, public ProtocolSapDisconnectTypeAdapterInterface {
  public:
    ProtocolSapDisconnectTypeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapDisconnectTypeAdapterJson() = default;

  public:
    int GetDisconStatus() const;
};

class ProtocolSapStatusAdapterJson : public ProtocolJsonIndAdapter, public ProtocolSapStatusAdapterInterface {
  public:
    ProtocolSapStatusAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSapStatusAdapterJson() = default;

  public:
    int GetCardStatus() const;
};

#endif /* __PROTOCOL_SAP_ADAPTER_JSON_H__ */
