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
 * protocolsimadapterlegacy.h
 *
 *  Created on: 2014. 6. 28.
 *      Author: MOX
 */

#ifndef __PROTOCOL_SIM_ADAPTER_LEGACY_H__
#define __PROTOCOL_SIM_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolsimadapterinterface.h"
#include "sitdef.h"

class ProtocolSimResponseAdapterLegacy : public ProtocolSimResponseAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimResponseAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimResponseAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimResponseAdapterLegacy() = default;
    virtual const char *GetParameter() const {
        return ProtocolBaseAdapter::GetParameter();
    }
    virtual UINT GetParameterLength() const {
        return ProtocolBaseAdapter::GetParameterLength();
    }
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimStatusAdapterLegacy : public ProtocolSimStatusAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimStatusAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimStatusAdapterInterface(), ProtocolRespAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolSimStatusAdapterLegacy() = default;

  public:
    int GetCardState() const {
        return (int)m_tSimStatusRsp.card_state;
    }
    int GetUniversalPinState() const {
        return (int)m_tSimStatusRsp.universal_pin_state;
    }
    int GetApplicationCount() const {
        return (int)m_tSimStatusRsp.application_num;
    }
    // Applications Status Information
    int GetAppsType(int nIndex) const {
        return (int)m_tSimStatusRsp.apps_status_info[nIndex].apps_type;
    }
    int GetAppsState(int nIndex) const {
        return (int)m_tSimStatusRsp.apps_status_info[nIndex].apps_state;
    }
    int GetPersonalSubstate(int nIndex) const {
        return (int)m_tSimStatusRsp.apps_status_info[nIndex].perso_substate;
    }
    char *GetAID(int nIndex) const;
    int GetApplicationLabel(int nIndex, BYTE *pAppLabel) const;
    int GetPin1Replaced(int nIndex) const {
        return (int)m_tSimStatusRsp.apps_status_info[nIndex].pin1_replaced;
    }
    int GetPinState(int nIndex, int nPinIndex) const;
    int GetPinRemainCount(int nIndex, int nPinIndex) const;
    int GetPukRemainCount(int nIndex, int nPukIndex) const;
    // Extension (1.2)
    bool GetEsimNoProfile() const {
        return (m_tSimStatusRspExt.esim_no_profile == 1);
    }
    int GetPhysicalSlotId() const {
        return (int)m_tSimStatusRspExt.physical_slot_id;
    }
    int GetAtrLength() const {
        return (int)m_tSimStatusRspExt.atr_length;
    }
    char *GetAtr() const {
        return (char *)m_tSimStatusRspExt.atr;
    }
    int GetIccidLength() const {
        return (int)m_tSimStatusRspExt.iccid_length;
    }
    char *GetIccid() const {
        return (char *)m_tSimStatusRspExt.iccid;
    }

    // Extension (1.4)
    int GetEidLength() const {
        return (int)m_tSimStatusRspExt.eid_length;
    }
    char *GetEid() const {
        return (char *)m_tSimStatusRspExt.eid;
    }

    // Extension (2.0)
    int GetPortId() const {
        return (int) m_tSimStatusRspExt.port_id;
    }
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    sit_sim_get_sim_status_rsp m_tSimStatusRsp;
    sit_sim_get_sim_status_rsp_ext m_tSimStatusRspExt;

    virtual void Init();
};

class ProtocolSimVerifyPinAdapterLegacy : public ProtocolSimVerifyPinAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimVerifyPinAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimVerifyPinAdapterInterface(), ProtocolRespAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolSimVerifyPinAdapterLegacy() = default;

  private:
    int m_nPinIndex;
    int m_nRemainCount;

    virtual void Init();

  public:
    int GetPinIndex() const {
        return m_nPinIndex;
    }
    int GetRemainCount() const {
        return m_nRemainCount;
    }
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimVerifyPukAdapterLegacy : public ProtocolSimVerifyPukAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimVerifyPukAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimVerifyPukAdapterInterface(), ProtocolRespAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolSimVerifyPukAdapterLegacy() = default;

  private:
    int m_nPukIndex;
    int m_nRemainCount;

    virtual void Init();

  public:
    int GetPukIndex() const {
        return m_nPukIndex;
    }
    int GetRemainCount() const {
        return m_nRemainCount;
    }
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimVerifyNetLockAdapterLegacy : public ProtocolSimVerifyNetLockAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimVerifyNetLockAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimVerifyNetLockAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimVerifyNetLockAdapterLegacy() = default;

    int GetRemainCount() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimIOAdapterLegacy : public ProtocolSimIOAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimIOAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimIOAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimIOAdapterLegacy() = default;

  public:
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    BYTE *GetResponse() const;
    int GetResponseLength() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimGetFacilityLockAdapterLegacy : public ProtocolSimGetFacilityLockAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimGetFacilityLockAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimGetFacilityLockAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimGetFacilityLockAdapterLegacy() = default;

  public:
    int GetServiceClass() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimSetFacilityLockAdapterLegacy : public ProtocolSimSetFacilityLockAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimSetFacilityLockAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimSetFacilityLockAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimSetFacilityLockAdapterLegacy() = default;

  public:
    int GetRemainCount() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimGetSimAuthAdapterLegacy : public ProtocolSimGetSimAuthAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimGetSimAuthAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimGetSimAuthAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimGetSimAuthAdapterLegacy() = default;

  public:
    int GetPayloadLength() const;

    int GetAuthType() const;
    int GetAuthLength() const;
    BYTE *GetAuth() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
    virtual UINT GetParameterLength() const {
        return ProtocolBaseAdapter::GetParameterLength();
    }
};

class ProtocolSimTransmitApduBasicAdapterLegacy : public ProtocolSimTransmitApduBasicAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimTransmitApduBasicAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimTransmitApduBasicAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimTransmitApduBasicAdapterLegacy() = default;

  public:
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    int GetApduLength() const;
    BYTE *GetApdu() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimOpenChannelAdapterLegacy : public ProtocolSimOpenChannelAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimOpenChannelAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimOpenChannelAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimOpenChannelAdapterLegacy() = default;

  public:
    int GetSessionID() const;
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    BYTE *GetResponse() const;
    int GetResponseLength() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimTransmitApduChannelAdapterLegacy : public ProtocolSimTransmitApduChannelAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimTransmitApduChannelAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimTransmitApduChannelAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimTransmitApduChannelAdapterLegacy() = default;

  public:
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    int GetApduLength() const;
    BYTE *GetApdu() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
    virtual UINT GetParameterLength() const {
        return ProtocolBaseAdapter::GetParameterLength();
    }
    virtual const char *GetParameter() const {
        return ProtocolBaseAdapter::GetParameter();
    }
};

class ProtocolSimImsiAdapterLegacy : public ProtocolSimImsiAdapterInterface, public ProtocolRespAdapter {
  private:
    char m_imsi[MAX_IMSI_LEN + 1];

  public:
    ProtocolSimImsiAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSimImsiAdapterLegacy() = default;

  protected:
    void Init();

  public:
    const char *GetImsi() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimGetGbaAuthAdapterLegacy : public ProtocolSimGetGbaAuthAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimGetGbaAuthAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimGetGbaAuthAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimGetGbaAuthAdapterLegacy() = default;

  public:
    int GetGbaAuthLength() const;
    BYTE *GetGbaAuth() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimATRAdapterLegacy : public ProtocolSimATRAdapterInterface, public ProtocolRespAdapter {
  private:
    BYTE m_result;
    BYTE m_atrlen;
    char m_atr[MAX_ATR_LEN];

  public:
    ProtocolSimATRAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSimATRAdapterLegacy() = default;

  protected:
    void Init();

  public:
    BYTE GetResult() const;
    BYTE GetATRLength() const;
    const char *GetATR() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimReadPbEntryLegacy : public ProtocolSimReadPbEntryInterface, public ProtocolRespAdapter {
  private:
    int m_pbType;
    int m_recordId;
    int m_dataLen;
    char m_entryData[MAX_PB_ENTRY_LEN];

  public:
    ProtocolSimReadPbEntryLegacy(const ModemData *pModemData);
    virtual ~ProtocolSimReadPbEntryLegacy() = default;
    void Init();
    int GetRecordId() const {
        return m_recordId;
    }
    int GetPbType() const {
        return m_pbType;
    }
    int GetDataLength() const {
        return m_dataLen;
    }
    char *GetEntryData() {
        return m_entryData;
    }
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimUpdatePbEntryLegacy : public ProtocolSimUpdatePbEntryInterface, public ProtocolRespAdapter {
  private:
    BYTE m_mode;
    BYTE m_pbtype;
    UINT16 m_recordId;

  public:
    ProtocolSimUpdatePbEntryLegacy(const ModemData *pModemData);
    virtual ~ProtocolSimUpdatePbEntryLegacy() = default;
    void Init();
    BYTE GetMode() {
        return m_mode;
    }
    BYTE GetPbtype() {
        return m_pbtype;
    }
    UINT16 GetRecordId() {
        return m_recordId;
    }
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimPbStorageInfoAdapterLegacy : public ProtocolSimPbStorageInfoAdapterInterface, public ProtocolRespAdapter {
  private:
    int m_pbType;
    int m_totalCount;
    int m_usedCount;

  public:
    ProtocolSimPbStorageInfoAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSimPbStorageInfoAdapterLegacy() = default;
    void Init();
    int GetPbType() const {
        return m_pbType;
    }
    int GetTotalCount() const {
        return m_totalCount;
    }
    int GetUsedCount() const {
        return m_usedCount;
    }
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimPbCapaAdapterLegacy : public ProtocolSimPbCapaAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimPbCapaAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimPbCapaAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimPbCapaAdapterLegacy() = default;

  public:
    int GetEntryNum() const;
    bool GetPbCapa(int *pb, int entryNum);
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimPbReadyAdapterLegacy : public ProtocolSimPbReadyAdapterInterface, public ProtocolIndAdapter {
  private:
    int m_pbReady;

  public:
    ProtocolSimPbReadyAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSimPbReadyAdapterLegacy() = default;
    void Init();
    int GetPbReady() const {
        return m_pbReady;
    }
};

class ProtocolSimGetCarrierRestrictionsAdapterLegacy : public ProtocolSimGetCarrierRestrictionsAdapterInterface, public ProtocolRespAdapter {
  private:
    int m_nAllowedCarrierLength;
    int m_nExcludedCarrierLength;
    SIT_CARRIER_INFO *m_pAllowedCarriers;
    SIT_CARRIER_INFO *m_pExcludedCarriers;
    bool m_bAllowedCarriersPrioritized;
    INT32 m_nMultiSimPolicy;

  public:
    ProtocolSimGetCarrierRestrictionsAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSimGetCarrierRestrictionsAdapterLegacy() = default;
    void Init();
    int GetAllowedCarrierLength() const {
        return m_nAllowedCarrierLength;
    }
    int GetExcludedCarrierLength() const {
        return m_nExcludedCarrierLength;
    }
    int GetAllowedCarriers(RIL_Carrier *pCarriers, int nSize) const;
    int GetExcludedCarriers(RIL_Carrier *pCarriers, int nSize) const;
    bool GetAllowedCarriersPrioritized() const {
        return m_bAllowedCarriersPrioritized;
    }
    INT32 GetMultiSimPolicy() const {
        return m_nMultiSimPolicy;
    }
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolUiccSubStatusChangeAdapterLegacy : public ProtocolUiccSubStatusChangeAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolUiccSubStatusChangeAdapterLegacy(const ModemData *pModemData)
        : ProtocolUiccSubStatusChangeAdapterInterface(), ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolUiccSubStatusChangeAdapterLegacy() = default;
    int GetState() const;
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

class ProtocolSimLockInfoAdapterLegacy : public ProtocolSimLockInfoAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSimLockInfoAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimLockInfoAdapterInterface(), ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSimLockInfoAdapterLegacy() = default;

  public:
    int GetPolicy() const;
    int GetStatus() const;
    int GetLockType() const;
    int GetMaxRetryCount() const;
    int GetRemainCount() const;
    int GetLockCodeCount() const;
    const char *GetLockCode() const;
    int GetLockCodeSize() const;
    virtual UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

#ifdef AUTO_VERIFY_PIN_WITH_CP
class ProtocolSimEncryptedPinAdapterLegacy : public ProtocolSimEncryptedPinAdapterInterface, public ProtocolIndAdapter {
private:
    int m_nEncryptedPinLen;
    int m_nPinLen;
    char m_szEncryptedPin[(MAX_ENCRYPTED_PIN_LEN*2)+1];

public:
    ProtocolSimEncryptedPinAdapterLegacy(const ModemData *pModemData)
            : ProtocolSimEncryptedPinAdapterInterface(), ProtocolIndAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolSimEncryptedPinAdapterLegacy() = default;
    int GetEncryptedPinLen() const { return m_nEncryptedPinLen; }
    int GetPinLen() const { return m_nPinLen; }
    const char *GetEncryptedPin() const;

private:
    void Init();
};
#endif

class ProtocolSimTrayStatusChangeAdapterLegacy : public ProtocolSimTrayStatusChangeAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolSimTrayStatusChangeAdapterLegacy(const ModemData *pModemData)
        : ProtocolSimTrayStatusChangeAdapterInterface(), ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolSimTrayStatusChangeAdapterLegacy() = default;
    int GetState() const;
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

#endif /* __PROTOCOL_SIM_ADAPTER_LEGACY_H__ */
