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
 * protocolnetadapterlegacy.h
 *
 *  Created on: 2014. 6. 25.
 *      Author: sungwoo48.choi
 */

#ifndef __PROTOCOL_NET_ADAPTER_LEGACY_H__
#define __PROTOCOL_NET_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "sitdef.h"
#include "protocolnetadapterinterface.h"
#include "netprotocolutils.h"

/**
 * ProtocolNetVoiceRegStateAdapterLegacy
 */
class ProtocolNetVoiceRegStateAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetVoiceRegStateAdapterInterface {
  public:
    ProtocolNetVoiceRegStateAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetVoiceRegStateAdapterLegacy() = default;

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
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetDataRegStateAdapterLegacy
 */
class ProtocolNetDataRegStateAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetDataRegStateAdapterInterface {
  public:
    ProtocolNetDataRegStateAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetDataRegStateAdapterLegacy() = default;

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
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetOperatorAdapterLegacy
 */
class ProtocolNetOperatorAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetOperatorAdapterInterface {
  private:
    char m_szPlmn[MAX_PLMN_LEN + 1];
    char m_szShortPlmn[MAX_SHORT_NAME_LEN + 1];
    char m_szLongPlmn[MAX_FULL_NAME_LEN + 1];
    int m_regState;
    int m_lac;

  public:
    ProtocolNetOperatorAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetOperatorAdapterLegacy() = default;

  private:
    void Init();

  public:
    const char *GetPlmn() const;
    const char *GetShortPlmn() const;
    const char *GetLongPlmn() const;
    int GetRegState() const;
    int GetLac() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetSelModeAdapterLegacy
 */
class ProtocolNetSelModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetSelModeAdapterInterface {
  public:
    ProtocolNetSelModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetSelModeAdapterLegacy() = default;

  public:
    int GetNetworkSelectionMode() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetRadioStateRespAdapterLegacy
 */
class ProtocolGetRadioStateRespAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetRadioStateRespAdapterInterface {
  public:
    ProtocolGetRadioStateRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolGetRadioStateRespAdapterLegacy() = default;

  public:
    int GetRadioState() const;
    int ConvertRilRadioState(int radioState) const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolRadioPowerAdapterLegacy
 */
class ProtocolRadioPowerAdapterLegacy : public ProtocolRespAdapter, public ProtocolRadioPowerAdapterInterface {
  public:
    ProtocolRadioPowerAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolRadioPowerAdapterLegacy() = default;

  public:
    virtual UINT GetErrorCode() const;
};

/**
 * ProtocolRadioStateAdapterLegacy
 */
class ProtocolRadioStateAdapterLegacy : public ProtocolIndAdapter, public ProtocolRadioStateAdapterInterface {
  public:
    ProtocolRadioStateAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolRadioStateAdapterLegacy() = default;

  public:
    int GetRadioState() const;
    int ConvertRilRadioState(int radioState) const;
};

/**
 * ProtocolNetPrefNetTypeAdapterLegacy
 */
class ProtocolNetPrefNetTypeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetPrefNetTypeAdapterInterface {
  public:
    ProtocolNetPrefNetTypeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetPrefNetTypeAdapterLegacy() = default;

  public:
    int GetPreferredNetworkType() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetBandModeAdapterLegacy
 */
class ProtocolNetBandModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetBandModeAdapterInterface {
  private:
    int m_bandMode[SIT_NET_BAND_MAX];
    int m_count;

  public:
    ProtocolNetBandModeAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetBandModeAdapterLegacy() = default;

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
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetAvailableNetworkAdapterLegacy
 */
class ProtocolNetAvailableNetworkAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetAvailableNetworkAdapterInterface {
  public:
    ProtocolNetAvailableNetworkAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetAvailableNetworkAdapterLegacy();

  public:
    int GetCount();
    bool GetNetwork(NetworkInfo &nwkInfo, int index, const char *simPlmn, char *simSpn);
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetPsServiceAdapterLegacy
 */
class ProtocolNetGetPsServiceAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetPsServiceAdapterInterface {
  public:
    ProtocolNetGetPsServiceAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetGetPsServiceAdapterLegacy() = default;

  public:
    int GetState();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetDuplexModeRespAdapterLegacy
 */
class ProtocolNetDuplexModeRespAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetDuplexModeRespAdapterInterface {
  public:
    ProtocolNetDuplexModeRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetDuplexModeRespAdapterLegacy() = default;

  public:
    int Get4gDuplexMode() const;
    int Get3gDuplexMode() const;
    int GetDuplexMode() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetEmergencyActInfoAdapterLegacy
 */
class ProtocolNetEmergencyActInfoAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetEmergencyActInfoAdapterInterface {
  public:
    ProtocolNetEmergencyActInfoAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetEmergencyActInfoAdapterLegacy() = default;

  public:
    int GetRat() const;
    int GetActStatus() const;
};

/**
 * ProtocolNetMcSrchRespAdapterLegacy
 */
class ProtocolNetMcSrchRespAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetMcSrchRespAdapterInterface {
  private:
    char m_szPlmn[MAX_PLMN_LEN + 1];

  public:
    ProtocolNetMcSrchRespAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetMcSrchRespAdapterLegacy() = default;

  public:
    int GetMcSrchResult() const;
    const char *GetMcSrchPlmn() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolSetNetworkRCRespAdapterLegacy
 */
class ProtocolSetNetworkRCRespAdapterLegacy : public ProtocolRespAdapter, public ProtocolSetNetworkRCRespAdapterInterface {
  public:
    ProtocolSetNetworkRCRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolSetNetworkRCRespAdapterLegacy() = default;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetNetworkRCRespAdapterLegacy
 */
class ProtocolGetNetworkRCRespAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetNetworkRCRespAdapterInterface {
  public:
    ProtocolGetNetworkRCRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolGetNetworkRCRespAdapterLegacy() = default;

  public:
    int GetVersion() const;
    int GetSession() const;
    int GetPhase() const;
    int GetRafType() const;
    BYTE *GetUuid() const;
    int GetStatus() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetworkRCIndAdapterLegacy
 */
class ProtocolNetworkRCIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetworkRCIndAdapterInterface {
  public:
    ProtocolNetworkRCIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolNetworkRCIndAdapterLegacy() = default;

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
 * ProtocolNetworkSgcBearerAllocIndAdapterLegacy
 */
class ProtocolNetworkSgcBearerAllocIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetworkSgcBearerAllocIndAdapterInterface {
  private:
    int mRat;
    int mConnectionStatus;

  public:
    ProtocolNetworkSgcBearerAllocIndAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetworkSgcBearerAllocIndAdapterLegacy() = default;

  public:
    int GetRat() const {
        return mRat;
    }
    int GetConnectionStatus() const {
        return mConnectionStatus;
    }
};

/**
 * ProtocolNetCdmaQueryRoamingTypeAdapterLegacy
 */
class ProtocolNetCdmaQueryRoamingTypeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetCdmaQueryRoamingTypeAdapterInterface {
  public:
    ProtocolNetCdmaQueryRoamingTypeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetCdmaQueryRoamingTypeAdapterLegacy() = default;

  public:
    int QueryRoamingType() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetCdmaHybridModeAdapterLegacy
 */
class ProtocolNetCdmaHybridModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetCdmaHybridModeAdapterInterface {
  public:
    ProtocolNetCdmaHybridModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetCdmaHybridModeAdapterLegacy() = default;

  public:
    int GetCdmaHybridMode() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetMccAdapterLegacy
 */
class ProtocolNetMccAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetMccAdapterInterface {
  private:
    char mMcc[4];

  public:
    ProtocolNetMccAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetMccAdapterLegacy() = default;

  public:
    int GetCurrentPrefNetworkMode() const;
    const char *GetMcc() const {
        return mMcc;
    }
};

/**
 * ProtocolNetCellInfoListAdapterLegacy
 */
class ProtocolNetCellInfoListAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetCellInfoListAdapterInterface {
  private:
    list<RIL_CellInfo_V1_6> mCellInfoList;

  public:
    ProtocolNetCellInfoListAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetCellInfoListAdapterLegacy() = default;

  public:
    list<RIL_CellInfo_V1_6> &GetCellInfoList(int cellInfoVer);
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetCellInfoListIndAdapterLegacy
 */
class ProtocolNetCellInfoListIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetCellInfoListIndAdapterInterface {
  private:
    list<RIL_CellInfo_V1_6> mCellInfoList;

  public:
    ProtocolNetCellInfoListIndAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetCellInfoListIndAdapterLegacy() = default;

  public:
    list<RIL_CellInfo_V1_6> &GetCellInfoList(int cellInfoVer);
};

/**
 * ProtocolNetScanResultAdapterLegacy
 */
class ProtocolNetScanResultAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetScanResultAdapterInterface {
  private:
    list<RIL_CellInfo_V1_6> mCellInfoList;

  public:
    ProtocolNetScanResultAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetScanResultAdapterLegacy() = default;

  public:
    int GetScanStatus() const;
    int GetScanResult() const;
    list<RIL_CellInfo_V1_6> &GetCellInfoList(int cellInfoVer);
};

/**
 * ProtocolNetSimFileInfoAdapterLegacy
 */
class ProtocolNetSimFileInfoAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetSimFileInfoAdapterInterface {
  private:
    int mSimFileId;
    int mRecordLen;
    int mNumRecords;
    BYTE **mppData;

  public:
    ProtocolNetSimFileInfoAdapterLegacy(const ModemData *pModemData);
    ProtocolNetSimFileInfoAdapterLegacy(const ProtocolNetSimFileInfoAdapterLegacy &) = delete;
    ProtocolNetSimFileInfoAdapterLegacy& operator=(ProtocolNetSimFileInfoAdapterLegacy const&) = delete;
    virtual ~ProtocolNetSimFileInfoAdapterLegacy();

  public:
    int GetSimFileId() const;
    int GetRecordLen() const;
    int GetNumOfRecords() const;
    BYTE **GetSimFileData() const;
};

/**
 * ProtocolNetPhysicalChannelConfigsLegacy
 */
class ProtocolNetPhysicalChannelConfigsLegacy : public ProtocolIndAdapter, public ProtocolNetPhysicalChannelConfigsInterface {
  private:
    list<PhysicalChannelConfigsData> mPcc;

  private:
    void Init();
    void FillPhysicalChannelConfig();
    void FillPhysicalChannelConfigV1_6();

  public:
    ProtocolNetPhysicalChannelConfigsLegacy(const ModemData *pModemData);
    std::list<PhysicalChannelConfigsData>& GetConfigList() { return mPcc; }
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolNetGetManualRatModeAdapterLegacy
 */
class ProtocolNetGetManualRatModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetManualRatModeAdapterInterface {
  private:
    int m_manual_rat_mode_set;
    int m_rat;

  public:
    ProtocolNetGetManualRatModeAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetGetManualRatModeAdapterLegacy() = default;

  public:
    void GetManualRatMode(void *data);
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetSetManualRatModeAdapterLegacy
 */
class ProtocolNetSetManualRatModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetSetManualRatModeAdapterInterface {
  public:
    ProtocolNetSetManualRatModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetSetManualRatModeAdapterLegacy() = default;

  public:
    int GetCause() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetFreqLockAdapterLegacy
 */
class ProtocolNetGetFreqLockAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetFreqLockAdapterInterface {
  private:
    int m_freq_mode_set;
    int m_rat;
    int m_lte_pcid;
    int m_lte_earfcn;
    int m_gsm_arfcn;
    int m_wcdma_psc;
    int m_wcdma_uarfcn;

  public:
    ProtocolNetGetFreqLockAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetGetFreqLockAdapterLegacy() = default;

  public:
    void GetFrequencyLock(void *data);
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetSetFreqLockAdapterLegacy
 */
class ProtocolNetSetFreqLockAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetSetFreqLockAdapterInterface {
  public:
    ProtocolNetSetFreqLockAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetSetFreqLockAdapterLegacy() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetEndcModeAdapterLegacy
 */
class ProtocolNetGetEndcModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetEndcModeAdapterInterface {
  public:
    ProtocolNetGetEndcModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetGetEndcModeAdapterLegacy() = default;

  public:
    int GetEndcMode() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetworkFrequencyInfoIndAdapterLegacy
 */
class ProtocolNetworkFrequencyInfoIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetworkFrequencyInfoIndAdapterInterface {
  public:
    ProtocolNetworkFrequencyInfoIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolNetworkFrequencyInfoIndAdapterLegacy() = default;

  public:
    int GetPrimaryRat() const;
    int GetPrimaryBand() const;
    int GetPrimaryFrequency() const;
    int GetSecondaryRat() const;
    int GetSecondaryBand() const;
    int GetSecondaryFrequency() const;
};

/**
 * ProtocolNetAcBarringInfoLegacy
 */
class ProtocolNetAcBarringInfoLegacy : public ProtocolIndAdapter, public ProtocolNetAcBarringInfoInterface {
  private:
    AC_BARRING_INFO mAcBarringInfo;

  public:
    ProtocolNetAcBarringInfoLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetAcBarringInfoLegacy() = default;

  public:
    void GetAcBarringInfo(void *data, unsigned int size);
};

/**
 * ProtocolNetGetFrequencyInfoAdapterLegacy
 */
class ProtocolNetGetFrequencyInfoAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetFrequencyInfoAdapterInterface {
  public:
    ProtocolNetGetFrequencyInfoAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetGetFrequencyInfoAdapterLegacy() = default;

  public:
    int GetPrimaryRat() const;
    int GetPrimaryBand() const;
    int GetPrimaryFrequency() const;
    int GetSecondaryRat() const;
    int GetSecondaryBand() const;
    int GetSecondaryFrequency() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetRrcInfoAdapterLegacy
 */
class ProtocolNetRrcInfoAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetRrcInfoAdapterInterface {
  public:
    ProtocolNetRrcInfoAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolNetRrcInfoAdapterLegacy() = default;

  public:
    int GetRat() const;
    int GetState() const;
};

/**
 * ProtocolNetGetNrModeAdapterLegacy
 */
class ProtocolNetGetNrModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetNrModeAdapterInterface {
  public:
    ProtocolNetGetNrModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetGetNrModeAdapterLegacy() = default;

  public:
    int GetNrMode() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetNrSilenceModeAdapterLegacy
 */
class ProtocolNetGetNrSilenceModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetNrSilenceModeAdapterInterface {
  public:
    ProtocolNetGetNrSilenceModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetGetNrSilenceModeAdapterLegacy() = default;

  public:
    bool GetNsaMode() const;
    bool GetSaMode() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolRegistrationFailedAdapterLegacy
 */
class ProtocolRegistrationFailedAdapterLegacy : public ProtocolIndAdapter, public ProtocolRegistrationFailedAdapterInterface {
  private:
    RIL_RegistrationFailed mRegFailedInfo;
    int mCellIdentityVer;

  public:
    ProtocolRegistrationFailedAdapterLegacy(const ModemData *pModemData, int cellIdentityVer = 3);
    virtual ~ProtocolRegistrationFailedAdapterLegacy();

  private:
    void Init();

  public:
    RIL_RegistrationFailed *GetRegistrationFailed() {
        return &mRegFailedInfo;
    }
};

/**
 * ProtocolBarringInfosLegacyLegacy
 */
class ProtocolBarringInfosLegacy {
  private:
    RIL_CellIdAndBarringInfo mCellIdAndBarringInfo;

  public:
    ProtocolBarringInfosLegacy();
    virtual ~ProtocolBarringInfosLegacy();

    void DecodingBarringInfos(BYTE *pBarringInfos, UINT len, int cellIdentityVer = 3);
    RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo();
};

/**
 * ProtocolGetBarringInfoRspAdapterLegacy
 */
class ProtocolGetBarringInfoRspAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetBarringInfoRspAdapterInterface, public ProtocolBarringInfosLegacy {
    int mCellIdentityVer;

  public:
    ProtocolGetBarringInfoRspAdapterLegacy(const ModemData *pModemData, int cellIdentityVer = 3);
    virtual ~ProtocolGetBarringInfoRspAdapterLegacy() = default;
    RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo();

  private:
    void Init();
    UINT GetErrorCode() const;
};

/**
 * ProtocolBarringInfoChangedAdapterLegacy
 */
class ProtocolBarringInfoChangedAdapterLegacy : public ProtocolIndAdapter, public ProtocolBarringInfoChangedAdapterInterface, public ProtocolBarringInfosLegacy {
    int mCellIdentityVer;

  public:
    ProtocolBarringInfoChangedAdapterLegacy(const ModemData *pModemData, int cellIdentityVer = 3);
    virtual ~ProtocolBarringInfoChangedAdapterLegacy() = default;
    RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo();

  private:
    void Init();
};

/**
 * ProtocolNetGetVonrCapaAdapterLegacy
 */
class ProtocolNetGetVonrCapaAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetVonrCapaAdapterInterface {
  public:
    ProtocolNetGetVonrCapaAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetGetVonrCapaAdapterLegacy() = default;

  public:
    int GetVonrCapa() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetGetAllowNetworkAdapterLegacy
 */
class ProtocolNetGetAllowNetworkAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetAllowNetworkAdapterInterface {
  public:
    ProtocolNetGetAllowNetworkAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetGetAllowNetworkAdapterLegacy() = default;

  public:
    UINT32 GetRat() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * CellInfoListAdapter
 */
class CellInfoListAdapter {
  public:
    int mCellInfoNum;
    char *mData;
    int mDataLen;
    list<RIL_CellInfo_V1_6> mCellInfoList;

  public:
    CellInfoListAdapter();
    CellInfoListAdapter(const CellInfoListAdapter&) = delete;
    CellInfoListAdapter& operator=(CellInfoListAdapter const&) = delete;
    void Init(void *data, int dataLen, int cellInfoNum);
    virtual list<RIL_CellInfo_V1_6> &GetCellInfoList() = 0;
    virtual ~CellInfoListAdapter() {
        if (mData != NULL) {
            delete[] mData;
        }
    }
};

class CellInfoListParserV1 : public CellInfoListAdapter {
  public:
    CellInfoListParserV1()
        : CellInfoListAdapter() {}
    virtual ~CellInfoListParserV1() {}
    list<RIL_CellInfo_V1_6> &GetCellInfoList();
    void FillCellInfo();
};

class CellInfoListParserV2 : public CellInfoListAdapter {
  public:
    CellInfoListParserV2()
        : CellInfoListAdapter() {}
    virtual ~CellInfoListParserV2() {}
    list<RIL_CellInfo_V1_6> &GetCellInfoList();
    void FillCellInfo();
};

class CellInfoListParserV3 : public CellInfoListAdapter {
  public:
    CellInfoListParserV3()
        : CellInfoListAdapter() {}
    virtual ~CellInfoListParserV3() {}
    list<RIL_CellInfo_V1_6> &GetCellInfoList();
    void FillCellInfo();
};

class CellInfoListParserV4 : public CellInfoListAdapter {
  public:
    CellInfoListParserV4()
        : CellInfoListAdapter() {}
    virtual ~CellInfoListParserV4() {}
    list<RIL_CellInfo_V1_6> &GetCellInfoList();
    void FillCellInfo();
};

class CellInfoAdapterFactory {
  private:
    CellInfoListParserV1 mCellInfoV1;
    CellInfoListParserV2 mCellInfoV2;
    CellInfoListParserV3 mCellInfoV3;
    CellInfoListParserV4 mCellInfoV4;

  public:
    CellInfoAdapterFactory() {}
    virtual ~CellInfoAdapterFactory() {}
    CellInfoListAdapter *GetCellInfoListAdapter(int cellInfoVersion);
};

/**
 * ProtocolNetSystemSelectionChannelsAdapterLegacy
 */
class ProtocolNetSystemSelectionChannelsAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetSystemSelectionChannelsAdapterInterface {
  public:
    ProtocolNetSystemSelectionChannelsAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolNetSystemSelectionChannelsAdapterLegacy() = default;

  private:
    int m_specifiersLength;
    RIL_RadioAccessSpecifier_V1_5 m_radioAccessSpecifiers[MAX_RADIO_ACCESS_NETWORKS];

  private:
    void Init();
    void FillRasSitSrcToRilDst(int sitNetworkType, sit_net_radio_access_specifier &sitSrcRas, RIL_RadioAccessSpecifier_V1_5 &rilDstRas);
    void FillRasSitSrcToRilDst(GERAN_TYPE, sit_net_radio_access_specifier &sitSrcRas, RIL_RadioAccessSpecifier_V1_5 &rilDstRas);
    void FillRasSitSrcToRilDst(UTRAN_TYPE, sit_net_radio_access_specifier &sitSrcRas, RIL_RadioAccessSpecifier_V1_5 &rilDstRas);
    void FillRasSitSrcToRilDst(EUTRAN_TYPE, sit_net_radio_access_specifier &sitSrcRas, RIL_RadioAccessSpecifier_V1_5 &rilDstRas);
    void FillRasSitSrcToRilDst(NGRAN_TYPE, sit_net_radio_access_specifier &sitSrcRas, RIL_RadioAccessSpecifier_V1_5 &rilDstRas);

  public:
    int GetSpecifiersLength() {
        return m_specifiersLength;
    }
    RIL_RadioAccessSpecifier_V1_5 *GetRadioAccessSpecifier() {
        return m_radioAccessSpecifiers;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetB1B2ConfigIndAdapterLegacy
 */
class ProtocolNetB1B2ConfigIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetB1B2ConfigIndAdapterInterface {
  public:
    ProtocolNetB1B2ConfigIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolNetB1B2ConfigIndAdapterLegacy() = default;

  public:
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolNetGetDeviceServiceAdapterLegacy
 */
class ProtocolNetGetDeviceServiceAdapterLegacy : public ProtocolRespAdapter, public ProtocolNetGetDeviceServiceAdapterInterface {
  public:
    ProtocolNetGetDeviceServiceAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolNetGetDeviceServiceAdapterLegacy() = default;

  public:
    int GetUsageSetting() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolNetDsdsStatusIndAdapterLegacy
 */
class ProtocolNetDsdsStatusIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolNetDsdsStatusIndAdapterInterface {
  public:
    ProtocolNetDsdsStatusIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolNetDsdsStatusIndAdapterLegacy() = default;
    int GetStatus() const;

  public:
    bool IsPsRestricted() const;
};

#endif /* __PROTOCOL_NET_ADAPTER_LEGACY_H__ */
