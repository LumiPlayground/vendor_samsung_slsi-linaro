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
 * protocolsimadapterjson.h
 *
 *  Created on: 2021. 7. 19.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SIM_ADAPTER_JSON_H__
#define __PROTOCOL_SIM_ADAPTER_JSON_H__

#include "protocoljsonadapter.h"
#include "protocolsimadapterinterface.h"
#include "json/json.h"
#include "sitdef.h"
#include <string>

class ProtocolSimStatusAdapterJson : public ProtocolSimStatusAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimStatusAdapterJson(const ModemData *pModemData);
    ProtocolSimStatusAdapterJson(const ProtocolSimStatusAdapterJson&) = delete;
    ProtocolSimStatusAdapterJson& operator=(ProtocolSimStatusAdapterJson const&) = delete;
    virtual ~ProtocolSimStatusAdapterJson() {
        if (m_atr) {
            delete[] m_atr;
            m_atr = NULL;
        }
        if (m_iccid) {
            delete[] m_iccid;
            m_iccid = NULL;
        }
        if (m_eid) {
            delete[] m_eid;
            m_eid = NULL;
        }
    }

  public:
    int GetCardState() const;
    int GetUniversalPinState() const;
    int GetApplicationCount() const;
    // Applications Status Information
    int GetAppsType(int nIndex) const;
    int GetAppsState(int nIndex) const;
    int GetPersonalSubstate(int nIndex) const;
    char *GetAID(int nIndex) const;
    int GetApplicationLabel(int nIndex, BYTE *pAppLabel) const;
    int GetPin1Replaced(int nIndex) const;
    int GetPinState(int nIndex, int nPinIndex) const;
    int GetPinRemainCount(int nIndex, int nPinIndex) const;
    int GetPukRemainCount(int nIndex, int nPukIndex) const;
    // Extension (1.2)
    bool GetEsimNoProfile() const;
    int GetPhysicalSlotId() const;
    int GetAtrLength() const {
        return m_nAtrLen;
    }
    char *GetAtr() const {
        return m_atr;
    }
    int GetIccidLength() const {
        return m_nIccidLen;
    }
    char *GetIccid() const {
        return m_iccid;
    }

    // Extension (1.4)
    int GetEidLength() const {
        return m_nEidLen;
    }
    char *GetEid() const {
        return m_eid;
    }

    // Extension (2.0)
    int GetPortId() const {return 0;} // to do implement
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    // member variable to store app_status_info data
    // see ns_sim_apps_status_info
    Json::Value m_jsonRspAppData;
    unsigned int m_nAtrLen;
    char *m_atr;
    unsigned int m_nIccidLen;
    char *m_iccid;
    unsigned int m_nEidLen;
    char *m_eid;

    void Init();
    void SetAtr();
    void SetIccid();
    void SetEid();
};

class ProtocolSimVerifyPinAdapterJson : public ProtocolSimVerifyPinAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimVerifyPinAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimVerifyPinAdapterJson() = default;

  private:
    int m_nPinIndex;
    int m_nRemainCount;

    void Init();

  public:
    int GetPinIndex() const {
        return m_nPinIndex;
    }
    int GetRemainCount() const {
        return m_nRemainCount;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimVerifyPukAdapterJson : public ProtocolSimVerifyPukAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimVerifyPukAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimVerifyPukAdapterJson() = default;

  private:
    int m_nPukIndex;
    int m_nRemainCount;

    void Init();

  public:
    int GetPukIndex() const {
        return m_nPukIndex;
    }
    int GetRemainCount() const {
        return m_nRemainCount;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimVerifyNetLockAdapterJson : public ProtocolSimVerifyNetLockAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimVerifyNetLockAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimVerifyNetLockAdapterJson() = default;

    int GetRemainCount() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimIOAdapterJson : public ProtocolSimIOAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimIOAdapterJson(const ModemData *pModemData);
    ProtocolSimIOAdapterJson(const ProtocolSimIOAdapterJson&) = delete;
    ProtocolSimIOAdapterJson& operator=(ProtocolSimIOAdapterJson const&) = delete;
    virtual ~ProtocolSimIOAdapterJson() {
        if (m_pResponse) {
            delete[] m_pResponse;
            m_pResponse = NULL;
        }
    }

  public:
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    BYTE *GetResponse() const {
        return m_pResponse;
    }
    int GetResponseLength() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void Init();
    void SetResponseData();

  private:
    BYTE *m_pResponse;
};

class ProtocolSimGetFacilityLockAdapterJson : public ProtocolSimGetFacilityLockAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimGetFacilityLockAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimGetFacilityLockAdapterJson() = default;

  public:
    int GetServiceClass() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimSetFacilityLockAdapterJson : public ProtocolSimSetFacilityLockAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimSetFacilityLockAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimSetFacilityLockAdapterJson() = default;

  public:
    int GetRemainCount() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimGetSimAuthAdapterJson : public ProtocolSimGetSimAuthAdapterInterface, public ProtocolJsonRespAdapter {
  private:
    BYTE *m_pAuthData;

  public:
    ProtocolSimGetSimAuthAdapterJson(const ModemData *pModemData);
    ProtocolSimGetSimAuthAdapterJson(const ProtocolSimGetSimAuthAdapterJson&) = delete;
    ProtocolSimGetSimAuthAdapterJson& operator=(ProtocolSimGetSimAuthAdapterJson const&) = delete;
    virtual ~ProtocolSimGetSimAuthAdapterJson() {
        if (m_pAuthData) {
            delete[] m_pAuthData;
            m_pAuthData = NULL;
        }
    }

  public:
    int GetAuthType() const;
    int GetAuthLength() const;
    BYTE *GetAuth() const {
        return m_pAuthData;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
    UINT GetParameterLength() const {
        return ProtocolBaseAdapter::GetParameterLength();
    }

  private:
    void Init();
    void SetAuthData();
};

class ProtocolSimTransmitApduBasicAdapterJson : public ProtocolSimTransmitApduBasicAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimTransmitApduBasicAdapterJson(const ModemData *pModemData);
    ProtocolSimTransmitApduBasicAdapterJson(const ProtocolSimTransmitApduBasicAdapterJson&) = delete;
    ProtocolSimTransmitApduBasicAdapterJson& operator=(ProtocolSimTransmitApduBasicAdapterJson const&) = delete;
    virtual ~ProtocolSimTransmitApduBasicAdapterJson() {
        if (m_pData) {
            delete[] m_pData;
            m_pData = NULL;
        }
    }

  public:
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    int GetApduLength() const;
    BYTE *GetApdu() const {
        return m_pData;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    BYTE *m_pData;

    void Init();
    void SetApduData();
};

class ProtocolSimOpenChannelAdapterJson : public ProtocolSimOpenChannelAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimOpenChannelAdapterJson(const ModemData *pModemData);
    ProtocolSimOpenChannelAdapterJson(const ProtocolSimOpenChannelAdapterJson&) = delete;
    ProtocolSimOpenChannelAdapterJson& operator=(ProtocolSimOpenChannelAdapterJson const&) = delete;
    virtual ~ProtocolSimOpenChannelAdapterJson() {
        if (m_pResponse) {
            delete[] m_pResponse;
            m_pResponse = NULL;
        }
    }

  public:
    int GetSessionID() const;
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    BYTE *GetResponse() const {
        return m_pResponse;
    }
    int GetResponseLength() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void Init();
    void SetResponseData();

  private:
    BYTE *m_pResponse;
    int m_nSitId;
};

class ProtocolSimTransmitApduChannelAdapterJson : public ProtocolSimTransmitApduChannelAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimTransmitApduChannelAdapterJson(const ModemData *pModemData);
    ProtocolSimTransmitApduChannelAdapterJson(const ProtocolSimTransmitApduChannelAdapterJson&) = delete;
    ProtocolSimTransmitApduChannelAdapterJson& operator=(ProtocolSimTransmitApduChannelAdapterJson const&) = delete;
    virtual ~ProtocolSimTransmitApduChannelAdapterJson() {
        if (m_pData) {
            delete[] m_pData;
            m_pData = NULL;
        }
    }

  public:
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    int GetApduLength() const;
    BYTE *GetApdu() const {
        return m_pData;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
    UINT GetParameterLength() const {
        return ProtocolBaseAdapter::GetParameterLength();
    }
    const char *GetParameter() const {
        return ProtocolBaseAdapter::GetParameter();
    }

  private:
    BYTE *m_pData;

    void Init();
    void SetApduData();
};

class ProtocolSimImsiAdapterJson : public ProtocolSimImsiAdapterInterface, public ProtocolJsonRespAdapter {
  private:
    char *m_pImsi;

    void Init();
    void SetImsi();

  public:
    ProtocolSimImsiAdapterJson(const ModemData *pModemData);
    ProtocolSimImsiAdapterJson(const ProtocolSimImsiAdapterJson &) = delete;
    ProtocolSimImsiAdapterJson operator=(ProtocolSimImsiAdapterJson const&) = delete;
    virtual ~ProtocolSimImsiAdapterJson() {
        if (m_pImsi) {
            delete[] m_pImsi;
            m_pImsi = NULL;
        }
    }

  public:
    const char *GetImsi() const {
        return m_pImsi;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimGetGbaAuthAdapterJson : public ProtocolSimGetGbaAuthAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimGetGbaAuthAdapterJson(const ModemData *pModemData);
    ProtocolSimGetGbaAuthAdapterJson(const ProtocolSimGetGbaAuthAdapterJson&) = delete;
    ProtocolSimGetGbaAuthAdapterJson& operator=(ProtocolSimGetGbaAuthAdapterJson const&) = delete;
    virtual ~ProtocolSimGetGbaAuthAdapterJson() {
        if (m_pData) {
            delete[] m_pData;
            m_pData = NULL;
        }
    }

  public:
    int GetGbaAuthLength() const;
    BYTE *GetGbaAuth() const {
        return m_pData;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    BYTE *m_pData;

    void Init();
    void SetGbaAuthData();
};

class ProtocolSimATRAdapterJson : public ProtocolSimATRAdapterInterface, public ProtocolJsonRespAdapter {
  private:
    BYTE m_result;
    unsigned int m_nAtrLen;
    char m_atr[MAX_ATR_LEN];

  public:
    ProtocolSimATRAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimATRAdapterJson() = default;

  private:
    void Init();
    void SetAtrData();

  public:
    BYTE GetResult() const;
    BYTE GetATRLength() const;
    const char *GetATR() const {
        return m_atr;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimReadPbEntryJson : public ProtocolSimReadPbEntryInterface, public ProtocolJsonRespAdapter {
  private:
    int m_nPbType;
    int m_nRecordId;
    unsigned int m_nDataLen;
    char m_entryData[MAX_PB_ENTRY_LEN];

  public:
    ProtocolSimReadPbEntryJson(const ModemData *pModemData);
    virtual ~ProtocolSimReadPbEntryJson() = default;
    void Init();
    int GetRecordId() const {
        return m_nRecordId;
    }
    int GetPbType() const {
        return m_nPbType;
    }
    int GetDataLength() const {
        return m_nDataLen;
    }
    char *GetEntryData();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void SetPbType();
    void SetRecordId();
    void SetDataLen();
};

class ProtocolSimUpdatePbEntryJson : public ProtocolSimUpdatePbEntryInterface, public ProtocolJsonRespAdapter {
  private:
    BYTE m_mode;
    BYTE m_pbtype;
    UINT16 m_recordId;

  public:
    ProtocolSimUpdatePbEntryJson(const ModemData *pModemData);
    virtual ~ProtocolSimUpdatePbEntryJson() = default;
    BYTE GetMode();
    BYTE GetPbtype();
    UINT16 GetRecordId();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void Init();
};

class ProtocolSimPbStorageInfoAdapterJson : public ProtocolSimPbStorageInfoAdapterInterface, public ProtocolJsonRespAdapter {
  private:
    int m_nPbType;
    int m_nTotalCount;
    int m_nUsedCount;

  public:
    ProtocolSimPbStorageInfoAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimPbStorageInfoAdapterJson() = default;
    void Init();
    int GetPbType() const {
        return m_nPbType;
    }
    int GetTotalCount() const {
        return m_nTotalCount;
    }
    int GetUsedCount() const {
        return m_nUsedCount;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void SetPbType();
    void SetTotalCount();
    void SetUsedCount();
};

class ProtocolSimPbCapaAdapterJson : public ProtocolSimPbCapaAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSimPbCapaAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimPbCapaAdapterJson() = default;

  public:
    int GetEntryNum() const;
    bool GetPbCapa(int *pb, int entryNum);
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    // member variable to store pb_list
    // see namespace ns_pb_cap_list
    Json::Value m_jsonPbList;

    void Init();
};

class ProtocolSimPbReadyAdapterJson : public ProtocolSimPbReadyAdapterInterface, public ProtocolJsonIndAdapter {
  public:
    ProtocolSimPbReadyAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimPbReadyAdapterJson() = default;
    int GetPbReady() const;
};

class ProtocolSimGetCarrierRestrictionsAdapterJson : public ProtocolSimGetCarrierRestrictionsAdapterInterface, public ProtocolJsonRespAdapter {
  private:
    int m_nAllowedCarrierLength;
    int m_nExcludedCarrierLength;
    RIL_Carrier *m_pAllowedCarriers;
    RIL_Carrier *m_pExcludedCarriers;
    bool m_bAllowedCarriersPrioritized;
    INT32 m_nMultiSimPolicy;

    void FillMccMncFromString(RIL_Carrier &carrierInfo, std::string plmn);
    void FillMatchDataFromJson(RIL_Carrier &carrierInfo, Json::Value &jsonMatchData);

  public:
    ProtocolSimGetCarrierRestrictionsAdapterJson(const ModemData *pModemData);
    ProtocolSimGetCarrierRestrictionsAdapterJson(const ProtocolSimGetCarrierRestrictionsAdapterJson&) = delete;
    ProtocolSimGetCarrierRestrictionsAdapterJson& operator=(ProtocolSimGetCarrierRestrictionsAdapterJson const&) = delete;
    virtual ~ProtocolSimGetCarrierRestrictionsAdapterJson();

  protected:
    void Init();

  public:
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
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolUiccSubStatusChangeAdapterJson : public ProtocolUiccSubStatusChangeAdapterInterface, public ProtocolJsonIndAdapter {
  public:
    ProtocolUiccSubStatusChangeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolUiccSubStatusChangeAdapterJson() = default;
    int GetState() const;
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

class ProtocolSimTrayStatusChangeAdapterJson : public ProtocolSimTrayStatusChangeAdapterInterface, public ProtocolJsonIndAdapter {
  public:
    ProtocolSimTrayStatusChangeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimTrayStatusChangeAdapterJson() = default;
    int GetState() const;
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};
#endif /* __PROTOCOL_SIM_ADAPTER_JSON_H__ */
