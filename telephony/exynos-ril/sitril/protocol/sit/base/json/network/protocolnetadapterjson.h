/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_NET_ADAPTER_JSON_H__
#define __PROTOCOL_NET_ADAPTER_JSON_H__

#include "protocoljsonadapter.h"
#include "protocolnetadapterinterface.h"
#include "sitdef.h"

/**
 * CellInfoListAdapter
 */
class CellInfoListAdapterJson {
  public:
    int mCellInfoNum;
    list<RIL_CellInfo_V1_6> mCellInfoList;

  public:
    CellInfoListAdapterJson() {
        mCellInfoNum = 0;
        mCellInfoList.resize(0);
    }
    virtual ~CellInfoListAdapterJson() = default;  // The deleting of mCellInfoList is caller responsibility

    list<RIL_CellInfo_V1_6> &GetCellInfoList() {
        return mCellInfoList;
    }

    virtual void FillCellInfo(int cellInfoNum, Json::Value &jsonData) = 0;

    virtual void FillCellIdentityGsm(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) = 0;
    virtual void FillCellIdentityCdma(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) = 0;
    virtual void FillCellIdentityLte(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) = 0;
    virtual void FillCellIdentityWcdma(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) = 0;
    virtual void FillCellIdentityTdscdma(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) = 0;
    virtual void FillCellIdentityNr(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) = 0;
};

/**
 * CellInfoListParserV4Json
 */
class CellInfoListParserV4Json : public CellInfoListAdapterJson {
  public:
    CellInfoListParserV4Json()
        : CellInfoListAdapterJson() {}
    virtual ~CellInfoListParserV4Json() = default;

    void FillCellInfo(int cellInfoNum, Json::Value &jsonData);
    void FillCellIdentityGsm(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillCellIdentityCdma(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillCellIdentityLte(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillCellIdentityWcdma(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillCellIdentityTdscdma(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillCellIdentityNr(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);

  private:
    void FillGsmCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillCdmaCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillLteCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillWcdmaCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillTdscdmaCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);
    void FillNrCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);

    void FillCellCommon(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData);

    void FillAlphaName(CellIdentityOperatorNames &dst, string alphaShort, string alphaLong);
    void FillCellIdentityCsgInfo(RIL_OptionalCsgInfo &dst, Json::Value &jsonData);
    char **FillGetAdditionalPlmns(Json::Value &jsonData);
};

class CellInfoAdapterFactoryJson {
  private:
    CellInfoListParserV4Json mCellInfoV4;

  public:
    CellInfoAdapterFactoryJson() = default;
    virtual ~CellInfoAdapterFactoryJson() = default;
    CellInfoListAdapterJson *GetCellInfoListAdapterJson(int cellInfoVersion);
};

/**
 * ProtocolNetVoiceRegStateAdapterJson
 */
class ProtocolNetVoiceRegStateAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetVoiceRegStateAdapterInterface {
  private:
    int m_rat;

  public:
    ProtocolNetVoiceRegStateAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetVoiceRegStateAdapterJson() = default;

  protected:
    void Init();

  public:
    int GetRegState() const;
    int GetRejectCause() const;
    int GetRadioTech() const;
    int GetLAC() const;
    int GetCellId() const;
    int GetPSC() const;
    int GetTAC() const;
    int GetPCID() const;
    int GetECI() const;
    int GetStationId() const;
    int GetStationLat() const;
    int GetStationLong() const;
    int GetConCurrent() const;
    int GetSystemId() const;
    int GetNetworkId() const;
    int GetRoamingInd() const;
    int GetRegPrl() const;
    int GetRoamingIndPrl() const;
    int getChannelNumber() const;
    long int getNrCid() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetDataRegStateAdapterJson
 */
class ProtocolNetDataRegStateAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetDataRegStateAdapterInterface {
  private:
    int m_rat;

  public:
    ProtocolNetDataRegStateAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetDataRegStateAdapterJson() = default;

  protected:
    void Init();

  public:
    int GetRegState() const;
    int GetRejectCause() const;
    int GetMaxSDC() const;
    int GetRadioTech() const;
    int GetLAC() const;
    int GetCellId() const;
    int GetPSC() const;
    int GetTAC() const;
    int GetPCID() const;
    int GetECI() const;
    int GetCSGID() const;
    int GetTADV() const;
    int GetImsVops() const;
    int GetEmcService() const;
    int getChannelNumber() const;
    bool IsEndcAvailable() const;
    bool IsDcNrRestricted() const;
    bool IsNrAvailable() const;
    long int getNrCid() const;
    int GetEmf() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetOperatorAdapterJson
 */
class ProtocolNetOperatorAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetOperatorAdapterInterface {
  private:
    char m_szPlmn[MAX_PLMN_LEN + 1];
    char m_szShortPlmn[MAX_SHORT_NAME_LEN + 1];
    char m_szLongPlmn[MAX_FULL_NAME_LEN + 1];
    int m_regState;
    int m_lac;

  public:
    ProtocolNetOperatorAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetOperatorAdapterJson() = default;

  protected:
    void Init();

  public:
    const char *GetPlmn() const;
    const char *GetShortPlmn() const;
    const char *GetLongPlmn() const;
    int GetRegState() const;
    int GetLac() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetSelModeAdapterJson
 */
class ProtocolNetSelModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetSelModeAdapterInterface {
  public:
    ProtocolNetSelModeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_NTW_MODE) {}
    virtual ~ProtocolNetSelModeAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetNetworkSelectionMode() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolRadioPowerAdapterJson
 */
class ProtocolRadioPowerAdapterJson : public ProtocolJsonRespAdapter, public ProtocolRadioPowerAdapterInterface {
  public:
    ProtocolRadioPowerAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_SET_RADIO_POWER, false) {}
    virtual ~ProtocolRadioPowerAdapterJson() = default;

  protected:
    //void Init();
  public:
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    };
};

/**
 * ProtocolGetRadioStateRespAdapterJson
 */
class ProtocolGetRadioStateRespAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetRadioStateRespAdapterInterface {
  public:
    ProtocolGetRadioStateRespAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_RADIO_POWER) {}
    virtual ~ProtocolGetRadioStateRespAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetRadioState() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }

  private:
    int ConvertRilRadioState(int radioState) const;
};

/**
 * ProtocolRadioStateAdapterJson
 */
class ProtocolRadioStateAdapterJson : public ProtocolJsonIndAdapter, public ProtocolRadioStateAdapterInterface {
  public:
    ProtocolRadioStateAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData, SIT_IND_RADIO_STATE_CHANGED) {}
    virtual ~ProtocolRadioStateAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetRadioState() const;

  private:
    int ConvertRilRadioState(int radioState) const;
};

/**
 * ProtocolNetPrefNetTypeAdapterJson
 */
class ProtocolNetPrefNetTypeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetPrefNetTypeAdapterInterface {
  public:
    ProtocolNetPrefNetTypeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_PREFERRED_NTW_TYPE) {}
    virtual ~ProtocolNetPrefNetTypeAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetPreferredNetworkType() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetBandModeAdapterJson
 */
class ProtocolNetBandModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetBandModeAdapterInterface {
  private:
    int m_bandMode[SIT_NET_BAND_MAX];
    unsigned int m_count;

  public:
    ProtocolNetBandModeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetBandModeAdapterJson() = default;

  protected:
    void Init();

  public:
    int GetCount() const {
        return m_count;
    }
    const int *GetAvialableBandMode() const {
        return (m_count == 0) ? NULL : m_bandMode;
    }
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetAvailableNetworkAdapterJson
 */
class ProtocolNetAvailableNetworkAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetAvailableNetworkAdapterInterface {
  public:
    ProtocolNetAvailableNetworkAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_AVAILABLE_NETWORKS) {}
    virtual ~ProtocolNetAvailableNetworkAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetCount();
    bool GetNetwork(NetworkInfo &nwkInfo, int index, const char *simPlmn, char *simSpn);
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetPsServiceAdapterJson
 */
class ProtocolNetGetPsServiceAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetPsServiceAdapterInterface {
  public:
    ProtocolNetGetPsServiceAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_PS_SERVICE) {}
    virtual ~ProtocolNetGetPsServiceAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetState();
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetDuplexModeRespAdapterJson
 */
class ProtocolNetDuplexModeRespAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetDuplexModeRespAdapterInterface {
  public:
    ProtocolNetDuplexModeRespAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_DUPLEX_MODE) {}
    virtual ~ProtocolNetDuplexModeRespAdapterJson() = default;

  protected:
    //void Init();
  public:
    int Get4gDuplexMode() const;
    int Get3gDuplexMode() const;
    int GetDuplexMode() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetEmergencyActInfoAdapterJson
 */
class ProtocolNetEmergencyActInfoAdapterJson : public ProtocolJsonIndAdapter, public ProtocolNetEmergencyActInfoAdapterInterface {
  public:
    ProtocolNetEmergencyActInfoAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData, SIT_IND_EMERGENCY_ACT_INFO) {}
    virtual ~ProtocolNetEmergencyActInfoAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetRat() const;
    int GetActStatus() const;
};

/**
 * ProtocolNetMcSrchRespAdapterJson
 */
class ProtocolNetMcSrchRespAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetMcSrchRespAdapterInterface {
  private:
    char m_szPlmn[MAX_PLMN_LEN + 1];

  public:
    ProtocolNetMcSrchRespAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetMcSrchRespAdapterJson() = default;

  protected:
    void Init();

  public:
    int GetMcSrchResult() const;
    const char *GetMcSrchPlmn() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolSetNetworkRCRespAdapterJson
 */
class ProtocolSetNetworkRCRespAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSetNetworkRCRespAdapterInterface {
  public:
    ProtocolSetNetworkRCRespAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_SET_RADIO_CAPABILITY, false) {}
    virtual ~ProtocolSetNetworkRCRespAdapterJson() = default;

  protected:
    //void Init();
  public:
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetNetworkRCRespAdapterJson
 */
class ProtocolGetNetworkRCRespAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetNetworkRCRespAdapterInterface {
  public:
    ProtocolGetNetworkRCRespAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_RADIO_CAPABILITY) {}
    virtual ~ProtocolGetNetworkRCRespAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetVersion() const;
    int GetSession() const;
    int GetPhase() const;
    int GetRafType() const;
    BYTE *GetUuid() const;
    int GetStatus() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetworkRCIndAdapterJson
 */
class ProtocolNetworkRCIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolNetworkRCIndAdapterInterface {
  public:
    ProtocolNetworkRCIndAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData, SIT_IND_RADIO_CAPABILITY) {}
    virtual ~ProtocolNetworkRCIndAdapterJson() = default;

  protected:
    //void Init();
  public:
    //need to implement based on SIT definition when it is available
    int GetVersion() const;
    int GetSession() const;
    int GetPhase() const;
    int GetRafType() const;
    BYTE *GetUuid() const;
    int GetStatus() const;
};

/**
 * ProtocolNetCdmaQueryRoamingTypeAdapterJson
 */
class ProtocolNetCdmaQueryRoamingTypeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetCdmaQueryRoamingTypeAdapterInterface {
  public:
    ProtocolNetCdmaQueryRoamingTypeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_CDMA_ROAMING_PREFERENCE) {}
    virtual ~ProtocolNetCdmaQueryRoamingTypeAdapterJson() = default;

  protected:
    //void Init();
  public:
    int QueryRoamingType() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetCdmaHybridModeAdapterJson
 */
class ProtocolNetCdmaHybridModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetCdmaHybridModeAdapterInterface {
  public:
    ProtocolNetCdmaHybridModeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_CDMA_HYBRID_MODE) {}
    virtual ~ProtocolNetCdmaHybridModeAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetCdmaHybridMode() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetMccAdapterJson
 */
class ProtocolNetMccAdapterJson : public ProtocolJsonIndAdapter, public ProtocolNetMccAdapterInterface {
  private:
    char mMcc[4];

  public:
    ProtocolNetMccAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetMccAdapterJson() = default;

  protected:
    void Init();

  public:
    int GetCurrentPrefNetworkMode() const;
    const char *GetMcc() const {
        return mMcc;
    }
};

/**
 * ProtocolNetCellInfoListAdapterJson
 */
class ProtocolNetCellInfoListAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetCellInfoListAdapterInterface {
  private:
    list<RIL_CellInfo_V1_6> mCellInfoList;

  public:
    ProtocolNetCellInfoListAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_CELL_INFO_LIST) {}
    virtual ~ProtocolNetCellInfoListAdapterJson();

  protected:
    //void Init();
  public:
    list<RIL_CellInfo_V1_6> &GetCellInfoList(int cellInfoVer);
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetCellInfoListIndAdapterJson
 */
class ProtocolNetCellInfoListIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolNetCellInfoListIndAdapterInterface {
  private:
    list<RIL_CellInfo_V1_6> mCellInfoList;

  public:
    ProtocolNetCellInfoListIndAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData, SIT_IND_CELL_INFO_LIST) {}
    virtual ~ProtocolNetCellInfoListIndAdapterJson();

  protected:
    //void Init();
  public:
    list<RIL_CellInfo_V1_6> &GetCellInfoList(int cellInfoVer);
};

/**
 * ProtocolNetScanResultAdapterJson
 */
class ProtocolNetScanResultAdapterJson : public ProtocolJsonIndAdapter, public ProtocolNetScanResultAdapterInterface {
  private:
    list<RIL_CellInfo_V1_6> mCellInfoList;

  public:
    ProtocolNetScanResultAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetScanResultAdapterJson();

  protected:
    void Init();

  public:
    int GetScanStatus() const;
    int GetScanResult() const;
    list<RIL_CellInfo_V1_6> &GetCellInfoList(int cellInfoVer);
};

/**
 * ProtocolNetSimFileInfoAdapterJson
 */
class ProtocolNetSimFileInfoAdapterJson : public ProtocolJsonIndAdapter, public ProtocolNetSimFileInfoAdapterInterface {
  private:
    BYTE **m_ppData;
    int m_nNumOfRecords;

  public:
    ProtocolNetSimFileInfoAdapterJson(const ModemData *pModemData);
    ProtocolNetSimFileInfoAdapterJson(const ProtocolNetSimFileInfoAdapterJson&) = delete;
    ProtocolNetSimFileInfoAdapterJson& operator=(ProtocolNetSimFileInfoAdapterJson const&) = delete;
    virtual ~ProtocolNetSimFileInfoAdapterJson();

  protected:
    void Init();

  public:
    int GetSimFileId() const;
    int GetRecordLen() const;
    int GetNumOfRecords() const {
        return m_nNumOfRecords;
    }
    BYTE **GetSimFileData() const {
        return m_ppData;
    }

  private:
    void SetRecordsData();
};

/**
 * ProtocolNetPhysicalChannelConfigsJson
 */
class ProtocolNetPhysicalChannelConfigsJson : public ProtocolJsonIndAdapter, public ProtocolNetPhysicalChannelConfigsInterface {
  private:
    list<PhysicalChannelConfigsData> mPcc;
    unsigned int mSize;

  public:
    ProtocolNetPhysicalChannelConfigsJson(const ModemData *pModemData);
    virtual ~ProtocolNetPhysicalChannelConfigsJson() = default;

  protected:
    void Init();

  public:
    list<PhysicalChannelConfigsData> &GetConfigList() {
        return mPcc;
    }
    const char *GetParameter() const;
    UINT GetParameterLength() const;
};

/**
 * ProtocolNetGetManualRatModeAdapterJson
 */
class ProtocolNetGetManualRatModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetManualRatModeAdapterInterface {
  private:
    int m_manual_rat_mode_set;
    int m_rat;

  public:
    ProtocolNetGetManualRatModeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetGetManualRatModeAdapterJson() = default;

  protected:
    void Init();

  public:
    void GetManualRatMode(void *data);
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetSetManualRatModeAdapterJson
 */
class ProtocolNetSetManualRatModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetSetManualRatModeAdapterInterface {
  public:
    ProtocolNetSetManualRatModeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_SET_MANUAL_RAT_MODE) {}
    virtual ~ProtocolNetSetManualRatModeAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetCause() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetFreqLockAdapterJson
 */
class ProtocolNetGetFreqLockAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetFreqLockAdapterInterface {
  private:
    int m_freq_mode_set;
    int m_rat;
    int m_lte_pcid;
    int m_lte_earfcn;
    int m_gsm_arfcn;
    int m_wcdma_psc;
    int m_wcdma_uarfcn;

  public:
    ProtocolNetGetFreqLockAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetGetFreqLockAdapterJson() = default;

  protected:
    void Init();

  public:
    void GetFrequencyLock(void *data);
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetSetFreqLockAdapterJson
 */
class ProtocolNetSetFreqLockAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetSetFreqLockAdapterInterface {
  public:
    ProtocolNetSetFreqLockAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_SET_FREQUENCY_LOCK) {}
    virtual ~ProtocolNetSetFreqLockAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetEndcModeAdapterJson
 */
class ProtocolNetGetEndcModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetEndcModeAdapterInterface {
  public:
    ProtocolNetGetEndcModeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_ENDC_MODE) {}
    virtual ~ProtocolNetGetEndcModeAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetEndcMode() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetFrequencyInfoAdapterJson
 */
class ProtocolNetGetFrequencyInfoAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetFrequencyInfoAdapterInterface {
  private:
    int m_indexPrimary;
    int m_indexSecondary;

  public:
    ProtocolNetGetFrequencyInfoAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetGetFrequencyInfoAdapterJson() = default;

  protected:
    void Init();

  public:
    int GetPrimaryRat() const;
    int GetPrimaryBand() const;
    int GetPrimaryFrequency() const;
    int GetSecondaryRat() const;
    int GetSecondaryBand() const;
    int GetSecondaryFrequency() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetworkFrequencyInfoIndAdapterJson
 */
class ProtocolNetworkFrequencyInfoIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolNetworkFrequencyInfoIndAdapterInterface {
  private:
    int m_indexPrimary;
    int m_indexSecondary;

  public:
    ProtocolNetworkFrequencyInfoIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetworkFrequencyInfoIndAdapterJson() = default;

  protected:
    void Init();

  public:
    int GetPrimaryRat() const;
    int GetPrimaryBand() const;
    int GetPrimaryFrequency() const;
    int GetSecondaryRat() const;
    int GetSecondaryBand() const;
    int GetSecondaryFrequency() const;
};

/**
 * ProtocolNetAcBarringInfoJson
 */
class ProtocolNetAcBarringInfoJson : public ProtocolJsonIndAdapter, public ProtocolNetAcBarringInfoInterface {
  private:
    AC_BARRING_INFO mAcBarringInfo;

  public:
    ProtocolNetAcBarringInfoJson(const ModemData *pModemData);
    virtual ~ProtocolNetAcBarringInfoJson() = default;

  protected:
    void Init();

  public:
    void GetAcBarringInfo(void *data, unsigned int size);
};

/**
 * ProtocolNetRrcInfoAdapterJson
 */
class ProtocolNetRrcInfoAdapterJson : public ProtocolJsonIndAdapter, public ProtocolNetRrcInfoAdapterInterface {
  public:
    ProtocolNetRrcInfoAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData, SIT_IND_RRC_INFO) {}
    virtual ~ProtocolNetRrcInfoAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetRat() const;
    int GetState() const;
};

/**
 * ProtocolNetGetNrModeAdapterJson
 */
class ProtocolNetGetNrModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetNrModeAdapterInterface {
  public:
    ProtocolNetGetNrModeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_NR_MODE) {}
    virtual ~ProtocolNetGetNrModeAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetNrMode() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetNrSilenceModeAdapterJson
 */
class ProtocolNetGetNrSilenceModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetNrSilenceModeAdapterInterface {
  public:
    ProtocolNetGetNrSilenceModeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_NR_SILENCE_MODE) {}
    virtual ~ProtocolNetGetNrSilenceModeAdapterJson() = default;

  protected:
    //void Init();
  public:
    bool GetNsaMode() const;
    bool GetSaMode() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolRegistrationFailedAdapterJson
 */
class ProtocolRegistrationFailedAdapterJson : public ProtocolJsonIndAdapter, public ProtocolRegistrationFailedAdapterInterface {
  private:
    RIL_CellInfo_V1_6 mRilCellInfo;
    RIL_RegistrationFailed mRegFailedInfo;
    int mCellIdentityVer;

  public:
    ProtocolRegistrationFailedAdapterJson(const ModemData *pModemData, int cellIdentityVer);
    virtual ~ProtocolRegistrationFailedAdapterJson();

  protected:
    void Init();

  public:
    RIL_RegistrationFailed *GetRegistrationFailed() {
        return &mRegFailedInfo;
    }
};

/**
 * ProtocolBarringInfosJson
 */
class ProtocolBarringInfosJson {
  private:
    RIL_CellInfo_V1_6 mRilCellInfo;
    RIL_CellIdAndBarringInfo mCellIdAndBarringInfo;

  public:
    ProtocolBarringInfosJson();
    virtual ~ProtocolBarringInfosJson();

    void UpdateCellIdentityInfo(Json::Value &jsonData, int cellIdentityVer);
    void UpdateBarringInfos(Json::Value &jsonData);
    RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo();
};

/**
 * ProtocolGetBarringInfoRspAdapterJson
 */
class ProtocolGetBarringInfoRspAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetBarringInfoRspAdapterInterface, public ProtocolBarringInfosJson {
  private:
    int mCellIdentityVer;

  public:
    ProtocolGetBarringInfoRspAdapterJson(const ModemData *pModemData, int cellIdentityVer);
    virtual ~ProtocolGetBarringInfoRspAdapterJson() = default;

  protected:
    void Init();

  public:
    RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo();
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolBarringInfoChangedAdapterJson
 */
class ProtocolBarringInfoChangedAdapterJson : public ProtocolJsonIndAdapter, public ProtocolBarringInfoChangedAdapterInterface, public ProtocolBarringInfosJson {
  private:
    int mCellIdentityVer;

  public:
    ProtocolBarringInfoChangedAdapterJson(const ModemData *pModemData, int cellIdentityVer);
    virtual ~ProtocolBarringInfoChangedAdapterJson() = default;

  protected:
    void Init();

  public:
    RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo();
};

/**
 * ProtocolNetGetVonrCapaAdapterJson
 */
class ProtocolNetGetVonrCapaAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetVonrCapaAdapterInterface {
  public:
    ProtocolNetGetVonrCapaAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_VONR_CAPABILITY) {}
    virtual ~ProtocolNetGetVonrCapaAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetVonrCapa() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetAllowNetworkAdapterJson
 */
class ProtocolNetGetAllowNetworkAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetAllowNetworkAdapterInterface {
  public:
    ProtocolNetGetAllowNetworkAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_ALLOW_NETWORK) {}
    virtual ~ProtocolNetGetAllowNetworkAdapterJson() = default;

  protected:
    //void Init();
  public:
    UINT32 GetRat() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetSystemSelectionChannelsAdapterJson
 */
class ProtocolNetSystemSelectionChannelsAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetSystemSelectionChannelsAdapterInterface {
  private:
    int m_specifiersLength;
    RIL_RadioAccessSpecifier_V1_5 m_radioAccessSpecifiers[MAX_RADIO_ACCESS_NETWORKS];

  public:
    ProtocolNetSystemSelectionChannelsAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNetSystemSelectionChannelsAdapterJson() = default;

  protected:
    void Init();

  public:
    int GetSpecifiersLength() {
        return m_specifiersLength;
    }
    RIL_RadioAccessSpecifier_V1_5 *GetRadioAccessSpecifier() {
        return m_radioAccessSpecifiers;
    }
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetB1B2ConfigIndAdapterJson
 */
class ProtocolNetB1B2ConfigIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolNetB1B2ConfigIndAdapterInterface {
  private:
    char *m_legacyData;  //SIT format data
    int m_legacyDataLength;

  private:
    void Init();

  public:
    ProtocolNetB1B2ConfigIndAdapterJson(const ModemData *pModemData);
    ProtocolNetB1B2ConfigIndAdapterJson(const ProtocolNetB1B2ConfigIndAdapterJson &) = delete;
    ProtocolNetB1B2ConfigIndAdapterJson& operator=(ProtocolNetB1B2ConfigIndAdapterJson const&) = delete;
    virtual ~ProtocolNetB1B2ConfigIndAdapterJson();

  public:
    const char *GetParameter() const;
    UINT GetParameterLength() const;
};

/**
 * ProtocolNetGetDeviceServiceAdapterJson
 */
class ProtocolNetGetDeviceServiceAdapterJson : public ProtocolJsonRespAdapter, public ProtocolNetGetDeviceServiceAdapterInterface {
  public:
    ProtocolNetGetDeviceServiceAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_DEVICE_SERVICE ) {}
    virtual ~ProtocolNetGetDeviceServiceAdapterJson() = default;

  protected:
    //void Init();
  public:
    int GetUsageSetting() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

#endif /* __PROTOCOL_NET_ADAPTER_JSON_H__ */
