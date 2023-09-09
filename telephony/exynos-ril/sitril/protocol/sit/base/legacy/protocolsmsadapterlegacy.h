/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_SMS_ADAPTER_LEGACY_H__
#define __PROTOCOL_SMS_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolsmsadapterinterface.h"

class BroadcastSmsConfigs;
class CCdmaBroadcastSmsConfigs;
class CCdmaSmsMessage;

class ProtocolSendSmsRespAdapterLegacy : public ProtocolSendSmsRespAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSendSmsRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolSendSmsRespAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSendSmsRespAdapterLegacy() = default;

  public:
    int GetRef() const;
    int GetSmsRspErrorCode() const;
    int GetPduSize() const;
    const char *GetPdu() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolWriteSmsToSimRespAdapterLegacy : public ProtocolWriteSmsToSimRespAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolWriteSmsToSimRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolWriteSmsToSimRespAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolWriteSmsToSimRespAdapterLegacy() = default;

  public:
    int GetIndex() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSmsCapacityOnSimRespAdapterLegacy : public ProtocolSmsCapacityOnSimRespAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSmsCapacityOnSimRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolSmsCapacityOnSimRespAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSmsCapacityOnSimRespAdapterLegacy() = default;

  public:
    int GetSimId() const;
    int GetTotalNum() const;
    int GetUsedNum() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSmscAddrRespAdapterLegacy : public ProtocolSmscAddrRespAdapterInterface, public ProtocolRespAdapter {
  private:
    char m_smsc[MAX_GSM_SMS_SERVICE_CENTER_ADDR * 2];
    int m_nSmscLen;

  public:
    ProtocolSmscAddrRespAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSmscAddrRespAdapterLegacy() = default;

  protected:
    void Init();

  public:
    const char *GetSmsc() const;
    int GetSmscLength() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolNewSmsIndAdapterLegacy : public ProtocolNewSmsIndAdapterInterface, public ProtocolIndAdapter {
  private:
    char m_tpdu[MAX_GSM_SMS_TPDU_SIZE * 2 + 1];
    int m_tpid;
    int m_len;

  public:
    ProtocolNewSmsIndAdapterLegacy(const ModemData *pModemData);
    ProtocolNewSmsIndAdapterLegacy(const ProtocolNewSmsIndAdapterLegacy&) = delete;
    ProtocolNewSmsIndAdapterLegacy& operator=(ProtocolNewSmsIndAdapterLegacy const&) = delete;
    virtual ~ProtocolNewSmsIndAdapterLegacy() = default;

  protected:
    void Init();

  public:
    int GetPduSize() const {
        return m_len;
    }
    const char *GetPdu() const;
    int GetTpid() const {
        return m_tpid;
    }
};

class ProtocolNewSmsOnSimIndAdapterLegacy : public ProtocolNewSmsOnSimIndAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolNewSmsOnSimIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolNewSmsOnSimIndAdapterInterface(), ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolNewSmsOnSimIndAdapterLegacy() = default;

  public:
    int GetIndex() const;
};

class ProtocolGetBcstSmsConfRespAdapterLegacy : public ProtocolGetBcstSmsConfRespAdapterInterface, public ProtocolRespAdapter {
  private:
    BroadcastSmsConfigs *m_pCBcsc;

  public:
    ProtocolGetBcstSmsConfRespAdapterLegacy(const ModemData *pModemData);
    ProtocolGetBcstSmsConfRespAdapterLegacy(const ProtocolGetBcstSmsConfRespAdapterLegacy&) = delete;
    ProtocolGetBcstSmsConfRespAdapterLegacy& operator=(ProtocolGetBcstSmsConfRespAdapterLegacy const&) = delete;
    virtual ~ProtocolGetBcstSmsConfRespAdapterLegacy();

    UINT8 GetConfigsNumber();
    RIL_GSM_BroadcastSmsConfigInfo **GetConfigsInfoPointers();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolNewBcstSmsAdapterLegacy : public ProtocolNewBcstSmsAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolNewBcstSmsAdapterLegacy(const ModemData *pModemData)
        : ProtocolNewBcstSmsAdapterInterface(), ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolNewBcstSmsAdapterLegacy() = default;

  public:
    BYTE *GetBcst() const;
    UINT16 GetBcstLen() const;
};

class ProtocolCdmaSendSmsRespAdapterLegacy : public ProtocolCdmaSendSmsRespAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolCdmaSendSmsRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolCdmaSendSmsRespAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolCdmaSendSmsRespAdapterLegacy() = default;

    int GetRef();
    int GetSmsRspErrorClass();
    int GetSmsRspCauseCode();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolCdmaNewSmsIndAdapterLegacy : public ProtocolCdmaNewSmsIndAdapterInterface, public ProtocolIndAdapter {
  private:
    CCdmaSmsMessage *m_pCCsm;
    int m_nTpId;

  public:
    ProtocolCdmaNewSmsIndAdapterLegacy(const ModemData *pModemData);
    ProtocolCdmaNewSmsIndAdapterLegacy(const ProtocolCdmaNewSmsIndAdapterLegacy&) = delete;
    ProtocolCdmaNewSmsIndAdapterLegacy& operator=(ProtocolCdmaNewSmsIndAdapterLegacy const&) = delete;
    virtual ~ProtocolCdmaNewSmsIndAdapterLegacy();

    int GetMessageLength() {
        return sizeof(RIL_CDMA_SMS_Message);
    }
    const RIL_CDMA_SMS_Message *GetRilCdmaSmsMsg() const;
    int GetTpid() const {
        return m_nTpId;
    }
};

class ProtocolCdmaWriteSmsToRuimRespAdapterLegacy : public ProtocolCdmaWriteSmsToRuimRespAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolCdmaWriteSmsToRuimRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolCdmaWriteSmsToRuimRespAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolCdmaWriteSmsToRuimRespAdapterLegacy() = default;

    int GetIndex();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolGetCdmaBcstSmsConfRespAdapterLegacy : public ProtocolGetCdmaBcstSmsConfRespAdapterInterface, public ProtocolRespAdapter {
  private:
    CCdmaBroadcastSmsConfigs *m_pCCbcsc;

  public:
    ProtocolGetCdmaBcstSmsConfRespAdapterLegacy(const ModemData *pModemData);
    ProtocolGetCdmaBcstSmsConfRespAdapterLegacy(const ProtocolGetCdmaBcstSmsConfRespAdapterLegacy&) = delete;
    ProtocolGetCdmaBcstSmsConfRespAdapterLegacy& operator=(ProtocolGetCdmaBcstSmsConfRespAdapterLegacy const&) = delete;
    virtual ~ProtocolGetCdmaBcstSmsConfRespAdapterLegacy();

    UINT8 GetConfigsNumber();
    RIL_CDMA_BroadcastSmsConfigInfo **GetConfigsInfoPointers();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy : public ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface, public ProtocolIndAdapter {
  private:
    RIL_CDMA_SMS_Message *m_ptRcsm;

  public:
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy(const ModemData *pModemData);
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy(const ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy&) = delete;
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy& operator=(ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy const&) = delete;
    virtual ~ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy();

    int GetMessageLength() {
        return sizeof(RIL_CDMA_SMS_Message);
    }
    const RIL_CDMA_SMS_Message *GetRilCdmaSmsMsg() const {
        return m_ptRcsm;
    }
};
#endif  // __PROTOCOL_SMS_ADAPTER_LEGACY_H__
