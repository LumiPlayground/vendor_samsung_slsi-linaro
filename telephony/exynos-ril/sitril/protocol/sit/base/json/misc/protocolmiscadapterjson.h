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
 * protocolmiscadapterjson.h
 *
 *  Created on: 2021. 08. 16.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_MISC_ADAPTER_JSON_H__
#define __PROTOCOL_MISC_ADAPTER_JSON_H__

#include "protocolmiscadapterinterface.h"
#include "protocoljsonadapter.h"
#include "rilproperty.h"
#include <string>
#include "sitdef.h"

/**
 * ProtocolMiscSetManualBandModeAdapterJson
 */
class ProtocolMiscSetManualBandModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetManualBandModeAdapterInterface {
  public:
    ProtocolMiscSetManualBandModeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_SET_MANUAL_BAND_MODE) {}
    virtual ~ProtocolMiscSetManualBandModeAdapterJson() = default;

  public:
    BYTE GetCause();

    //For ProtocolJsonRespAdatper
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetRfDesenseModeAdapterJson
 */
class ProtocolMiscSetRfDesenseModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetRfDesenseModeAdapterInterface {
  public:
    ProtocolMiscSetRfDesenseModeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_SET_RF_DESENSE_MODE) {}
    virtual ~ProtocolMiscSetRfDesenseModeAdapterJson() = default;

  public:
    BYTE GetCause();

    //For ProtocolJsonRespAdatper
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscResponseAdapterJson
 */
class ProtocolMiscResponseAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscResponseAdapterInterface {
  public:
    ProtocolMiscResponseAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData) {}
    virtual ~ProtocolMiscResponseAdapterJson() = default;

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscVersionAdapterJson
 */
class ProtocolMiscVersionAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscVersionAdapterInterface {
  private:
    std::string m_swVer;
    std::string m_hwVer;
    std::string m_rfCalDate;
    std::string m_productCode;
    std::string m_modelId;
    std::string m_prlVer;
    std::string m_eriVer;
    std::string m_cpChipset;

  private:
    void Init();

  public:
    ProtocolMiscVersionAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscVersionAdapterJson() = default;

  public:
    int GetMask() const;
    const char *GetSwVer() const;
    const char *GetHwVer() const;
    const char *GetRfCalDate() const;
    const char *GetProdCode() const;
    const char *GetModelID() const;
    int GetPrlNamNum() const;
    const BYTE *GetPrlVersion() const;
    int GetEriNamNum() const;
    const BYTE *GetEriVersion() const;
    const BYTE *GetCPChipSet() const;

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetTtyAdapterJson
 */
class ProtocolMiscGetTtyAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscGetTtyAdapterInterface {
  public:
    ProtocolMiscGetTtyAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscGetTtyAdapterJson() = default;

  public:
    int GetTtyMode() const;

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscNITZTimeAdapterJson
 */
class ProtocolMiscNITZTimeAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscNITZTimeAdapterInterface {
  public:
    ProtocolMiscNITZTimeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscNITZTimeAdapterJson() = default;

  public:
    int TimeInfoType() const;
    int DayLightValid() const;
    int Year() const;
    int Month() const;
    int Day() const;
    int Hour() const;
    int Minute() const;
    int Second() const;
    int TimeZone() const;
    int DayLightAdjust() const;
    int DayofWeek() const;
    int GetMMInfo() const;
    const BYTE *GetPLMN() const;
    int64_t age() const { return 0; }

    //For ProtocolJsonIndAdapter
    const char *GetParameter() const {
        return ProtocolJsonIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolJsonIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolOemModemInfoAdapterJson
 */
class ProtocolOemModemInfoAdapterJson : public ProtocolJsonRespAdapter, public ProtocolOemModemInfoAdapterInterface {
  private:
    int m_commandType;
    int m_size;
    std::string m_data;

  private:
    void Init();

  public:
    ProtocolOemModemInfoAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolOemModemInfoAdapterJson() = default;

  public:
    int GetCommandType() const;
    unsigned int GetSize() const;
    void *GetData();

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolOemSwitchModemFunctionAdapterJson
 */
class ProtocolOemSwitchModemFunctionAdapterJson : public ProtocolJsonRespAdapter, public ProtocolOemSwitchModemFunctionAdapterInterface {
  public:
    ProtocolOemSwitchModemFunctionAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolOemSwitchModemFunctionAdapterJson() = default;

  public:
    BYTE GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolOemGetCqiInfoAdapterJson
 */
class ProtocolOemGetCqiInfoAdapterJson : public ProtocolJsonRespAdapter, public ProtocolOemGetCqiInfoAdapterInterface {
  public:
    ProtocolOemGetCqiInfoAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolOemGetCqiInfoAdapterJson() = default;

  public:
    INT16 GetCqiType() const;
    INT16 GetCqiInfo0() const;
    INT16 GetCqiInfo1() const;
    INT16 GetRi() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetTcsFciAdapterJson
 */
class ProtocolMiscSetTcsFciAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetTcsFciAdapterInterface {
  public:
    ProtocolMiscSetTcsFciAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSetTcsFciAdapterJson() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetTcsFciAdapterJson
 */
class ProtocolMiscGetTcsFciAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscGetTcsFciAdapterInterface {
  private:
    std::string m_fci;

  private:
    void Init();

  public:
    ProtocolMiscGetTcsFciAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscGetTcsFciAdapterJson() = default;

  public:
    const char *GetFci() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscOemDisplayEngAdapterJson
 */
class ProtocolMiscOemDisplayEngAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscOemDisplayEngAdapterInterface {
  public:
    ProtocolMiscOemDisplayEngAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData) {}
    virtual ~ProtocolMiscOemDisplayEngAdapterJson() = default;

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolOemModemInfoIndAdapterJson
 */
class ProtocolOemModemInfoIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolOemModemInfoIndAdapterInterface {
  private:
    std::string m_data;
    int m_commandType;
    int m_size;

  private:
    void Init();

  public:
    ProtocolOemModemInfoIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolOemModemInfoIndAdapterJson() = default;

  public:
    int GetCommandType() const;
    unsigned int GetSize() const;
    void *GetData();

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscBaseStationIndAdapterJson
 */
class ProtocolMiscBaseStationIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscBaseStationIndAdapterInterface {
  private:
    char *m_legacyData;
    int m_dataLength;

  private:
    void Init();

  public:
    ProtocolMiscBaseStationIndAdapterJson(const ModemData *pModemData);
    ProtocolMiscBaseStationIndAdapterJson(const ProtocolMiscBaseStationIndAdapterJson &) = delete;
    ProtocolMiscBaseStationIndAdapterJson& operator=(ProtocolMiscBaseStationIndAdapterJson const&) = delete;
    virtual ~ProtocolMiscBaseStationIndAdapterJson();

  public:
    const char *GetParameter() const;
    UINT GetParameterLength() const;
};

/**
 * ProtocolMiscGetCdmaCapabilityInfoAdapterJson
 */
class ProtocolMiscGetCdmaCapabilityInfoAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscGetCdmaCapabilityInfoAdapterInterface {
  public:
    ProtocolMiscGetCdmaCapabilityInfoAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscGetCdmaCapabilityInfoAdapterJson() = default;

  public:
    BYTE GetCapability() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetActiveVsimAdapterJson
 */
class ProtocolMiscSetActiveVsimAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetActvieVsimAdapterInterface {
  public:
    ProtocolMiscSetActiveVsimAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSetActiveVsimAdapterJson() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetForbidNrCellAdapterJson
 */
class ProtocolMiscSetForbidNrCellAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetForbidNrCellAdapterInterface {
  public:
    ProtocolMiscSetForbidNrCellAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSetForbidNrCellAdapterJson() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscEndcCapabilityIndAdapterJson
 */
class ProtocolMiscEndcCapabilityIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscEndcCapabilityIndAdapterInterface {
  public:
    ProtocolMiscEndcCapabilityIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscEndcCapabilityIndAdapterJson() = default;

  public:
    int GetCapability() const;
    int GetCause() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscGetModemActivityInfoAdapterJson
 */
class ProtocolMiscGetModemActivityInfoAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscGetModemActivityInfoAdapterInterface {
  private:
    char *m_legacyData;  //SIT format
    int m_dataLength;

  protected:
    void Init();

  public:
    ProtocolMiscGetModemActivityInfoAdapterJson(const ModemData *pModemData);
    ProtocolMiscGetModemActivityInfoAdapterJson(const ProtocolMiscGetModemActivityInfoAdapterJson &) = delete;
    ProtocolMiscGetModemActivityInfoAdapterJson& operator=(ProtocolMiscGetModemActivityInfoAdapterJson const&) = delete;
    virtual ~ProtocolMiscGetModemActivityInfoAdapterJson();

  public:
    UINT GetErrorCode() const;
    const char *GetParameter() const;
    UINT GetParameterLength() const;
};

/**
 * ProtocolMiscDropMtCallInfoIndAdapterJson
 */
class ProtocolMiscDropMtCallInfoIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscDropMtCallInfoIndAdapterInterface {
  private:
    char *m_legacyData;  //SIT format
    int m_dataLength;

  private:
    void Init();

  public:
    ProtocolMiscDropMtCallInfoIndAdapterJson(const ModemData *pModemData);
    ProtocolMiscDropMtCallInfoIndAdapterJson(const ProtocolMiscDropMtCallInfoIndAdapterJson &) = delete;
    ProtocolMiscDropMtCallInfoIndAdapterJson& operator=(ProtocolMiscDropMtCallInfoIndAdapterJson const&) = delete;
    virtual ~ProtocolMiscDropMtCallInfoIndAdapterJson();

  public:
    const char *GetParameter() const;
    UINT GetParameterLength() const;
};

/**
 * ProtocolMiscSetAheadRreAdapterJson
 */
class ProtocolMiscSetAheadRreAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetAheadRreAdapterInterface {
  public:
    ProtocolMiscSetAheadRreAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSetAheadRreAdapterJson() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const;
};

/**
 * ProtocolMiscGetActivityInfoAdapterJson
 */
class ProtocolMiscGetActivityInfoAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscGetActivityInfoAdapterInterface {
  private:
    UINT32 txModeTime[RIL_NUM_TX_POWER_LEVELS] = {0};

  public:
    ProtocolMiscGetActivityInfoAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscGetActivityInfoAdapterJson() = default;

  public:
    UINT32 GetSleepPeriod() const;
    UINT32 GetIdlePeriod() const;
    UINT32 *GetTxPeriod() const;
    UINT32 GetRxPeriod() const;

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetForbidLteCellAdapterJson
 */
class ProtocolMiscSetForbidLteCellAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetForbidLteCellAdapterInterface {
  public:
    ProtocolMiscSetForbidLteCellAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSetForbidLteCellAdapterJson() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetForbidFrequencyAdapterJson
 */
class ProtocolMiscSetForbidFrequencyAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetForbidFrequencyAdapterInterface {
  public:
    ProtocolMiscSetForbidFrequencyAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSetForbidFrequencyAdapterJson() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolCdmaSubscriptionAdapterJson
 */
class ProtocolCdmaSubscriptionAdapterJson : public ProtocolJsonRespAdapter, public ProtocolCdmaSubscriptionAdapterInterface {
  private:
    char m_szMdn[MAX_CDMA_MDN_LEN + 1];
    char m_szMin[MAX_CDMA_MIN_LEN + 1];
    WORD m_wSid;
    WORD m_wNid;
    UINT m_uPrlVersion;

  protected:
    void Init();

  public:
    ProtocolCdmaSubscriptionAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolCdmaSubscriptionAdapterJson() = default;
    char *GetMdn() const {
        return (char *)m_szMdn;
    }
    char *GetMin() const {
        return (char *)m_szMin;
    }
    WORD GetSid() const {
        return m_wSid;
    }
    WORD GetNid() const {
        return m_wNid;
    }
    UINT GetPrlVersion() const {
        return m_uPrlVersion;
    }

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetMslCodeAdapterJson
 */
class ProtocolMiscGetMslCodeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscGetMslCodeAdapterInterface {
  public:
    ProtocolMiscGetMslCodeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscGetMslCodeAdapterJson() = default;

  public:
    const char *getMslCode() const;

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetVoiceOperationAdapterJson
 */
class ProtocolGetVoiceOperationAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetVoiceOperationAdapterInterface {
  public:
    ProtocolGetVoiceOperationAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolGetVoiceOperationAdapterJson() = default;

  public:
    int GetMode() const;

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscPinControlAdapterJson
 */
class ProtocolMiscPinControlAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscPinControlAdapterInterface {
  private:
    BYTE m_Signal;
    BYTE m_Status;

  public:
    ProtocolMiscPinControlAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscPinControlAdapterJson() = default;

  public:
    BYTE GetSignal();
    BYTE GetStatus();

    //For ProtocolJsonIndAdapter
    const char *GetParameter() const {
        return ProtocolJsonIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolJsonIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscSarControlStateAdapterJson
 */
class ProtocolMiscSarControlStateAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscSarControlStateAdapterInterface {
  private:
    BYTE m_DeviceState;

  public:
    ProtocolMiscSarControlStateAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSarControlStateAdapterJson() = default;

  public:
    BYTE GetDeviceState();

    //For ProtocolJsonIndAdapter
    const char *GetParameter() const {
        return ProtocolJsonIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolJsonIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscGetPreferredCallCapabilityAdapterJson
 */
class ProtocolMiscGetPreferredCallCapabilityAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscGetPreferredCallCapabilityAdapterInterface {
  public:
    ProtocolMiscGetPreferredCallCapabilityAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscGetPreferredCallCapabilityAdapterJson() = default;

  public:
    int GetMode() const;

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscATCommandAdapterJson
 */
class ProtocolMiscATCommandAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscATCommandAdapterInterface {
  private:
    char *mCommand;
    unsigned int mCommandLength;

  private:
    void Init();

  public:
    ProtocolMiscATCommandAdapterJson(const ModemData *pModemData);
    ProtocolMiscATCommandAdapterJson(const ProtocolMiscATCommandAdapterJson&) = delete;
    ProtocolMiscATCommandAdapterJson& operator=(ProtocolMiscATCommandAdapterJson const&) = delete;
    virtual ~ProtocolMiscATCommandAdapterJson();

  public:
    const char *GetCommand() const {
        return mCommand;
    }
    unsigned int GetCommandLength() const {
        return mCommandLength;
    }

    //For ProtocolJsonIndAdapter
    const char *GetParameter() const {
        return ProtocolJsonIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolJsonIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscSetSelflogAdapterJson
 */
class ProtocolMiscSetSelflogAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetSelflogAdapterInterface {
  public:
    ProtocolMiscSetSelflogAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSetSelflogAdapterJson() = default;

  public:
    int GetSelflogResult();

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetSelflogStatusAdapterJson
 */
class ProtocolMiscGetSelflogStatusAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscGetSelflogStatusAdapterInterface {
  public:
    ProtocolMiscGetSelflogStatusAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscGetSelflogStatusAdapterJson() = default;

  public:
    int GetSelflogStatus();

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSelflogStatusAdapterJson
 */
class ProtocolMiscSelflogStatusAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscSelflogStatusAdapterInterface {
  public:
    ProtocolMiscSelflogStatusAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSelflogStatusAdapterJson() = default;

  public:
    BYTE GetIndSelflogStatus();

    //For ProtocolJsonIndAdapter
    const char *GetParameter() const {
        return ProtocolJsonIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolJsonIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscSetModemLogDumpAdapterJson
 */
class ProtocolMiscSetModemLogDumpAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetModemLogDumpAdapterInterface {
  public:
    ProtocolMiscSetModemLogDumpAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSetModemLogDumpAdapterJson() = default;

  public:
    int GetResult() const;

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscCurrentLinkCapacityEstimateAdapterJson
 */
class ProtocolMiscCurrentLinkCapacityEstimateAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscCurrentLinkCapacityEstimateAdapterInterface {
  private:
    CURRENT_LINK_CAPA_ESTIMATE mCurLinkCapaEstimate;

  private:
    void InitLinkCapaEstimate();

  public:
    ProtocolMiscCurrentLinkCapacityEstimateAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscCurrentLinkCapacityEstimateAdapterJson() = default;

  public:
    int GetDlCapaKbps() const;
    int GetUlCapaKbps() const;
    int GetSecDlCapaKbps() const;
    int GetSecUlCapaKbps() const;

    //For ProtocolJsonIndAdapter
    const char *GetParameter() const {
        return ProtocolJsonIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolJsonIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolRfConnectionStateAdapterJson
 */
class ProtocolRfConnectionStateAdapterJson : public ProtocolJsonIndAdapter, public ProtocolRfConnectionStateAdapterInterface {
  public:
    ProtocolRfConnectionStateAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolRfConnectionStateAdapterJson() = default;

  public:
    BYTE GetState();

    //For ProtocolJsonIndAdapter
    const char *GetParameter() const {
        return ProtocolJsonIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolJsonIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscRssiScanResultAdapterJson
 */
class ProtocolMiscRssiScanResultAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscRssiScanResultAdapterInterface {
  private:
    INT16 m_scanResult[MAX_RSSI_SCAN_RESULT];

  public:
    enum { DEFAULT_RSSI = -2040,
           INVALID_RSSI = 0x7FFFFFFF };

  public:
    ProtocolMiscRssiScanResultAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscRssiScanResultAdapterJson() = default;

  public:
    int GetTotalPage();
    int GetCurrentPage();
    int GetStartFrequency();
    int GetEndFrequency();
    int GetStep();
    int GetScanResultSize();
    INT16 *GetScanResult();

    //For ProtocolJsonIndAdapter
    const char *GetParameter() const {
        return ProtocolJsonIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolJsonIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolSignalStrengthAdapterJson
 */
class ProtocolSignalStrengthAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSignalStrengthAdapterInterface {
  private:
    RIL_SignalStrength_V1_6 mSignalStrength;

  public:
    ProtocolSignalStrengthAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSignalStrengthAdapterJson() = default;

  public:
    RIL_SignalStrength_V1_6 &GetSignalStrength(int ssVer);

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * SignalStrengthAdapterJson
 */
class SignalStrengthAdapterJson {
  public:
    RIL_SignalStrength_V1_6 mSignalStrength;

  public:
    SignalStrengthAdapterJson();
    virtual ~SignalStrengthAdapterJson() = default;
    virtual RIL_SignalStrength_V1_6 &GetSignalStrength(Json::Value &jsonSSData) = 0;

  private:
    void FillDefaultSignalStrength();

    virtual void FillSignalStrength(Json::Value &JsonSSData) = 0;
    virtual void FillGsmSignalStrength(Json::Value &jsonGsmSS) = 0;
    virtual void FillWcdmaSignalStrength(Json::Value &jsonWcdmaSS) = 0;
    virtual void FillCdmaSignalStrength(Json::Value &jsonCdmaSS) = 0;
    virtual void FillEvdoSignalStrength(Json::Value &jsonEvdoSS) = 0;
    virtual void FillLteSignalStrength(Json::Value &jsonLteSS) = 0;
    virtual void FillTdscdmaSignalStrength(Json::Value &jsonTdscdmaSS) = 0;
    virtual void FillNrSignalStrength(Json::Value &jsonNrSS) = 0;
};

class SignalStrengthParserV4Json : public SignalStrengthAdapterJson {
  public:
    SignalStrengthParserV4Json()
        : SignalStrengthAdapterJson() {}
    virtual ~SignalStrengthParserV4Json() = default;
    RIL_SignalStrength_V1_6 &GetSignalStrength(Json::Value &jsonSSData);

  private:
    void FillSignalStrength(Json::Value &jsonSSData);
    void FillGsmSignalStrength(Json::Value &jsonGsmSS);
    void FillWcdmaSignalStrength(Json::Value &jsonWcdmaSS);
    void FillCdmaSignalStrength(Json::Value &jsonCdmaSS);
    void FillEvdoSignalStrength(Json::Value &jsonEvdoSS);
    void FillLteSignalStrength(Json::Value &jsonLteSS);
    void FillTdscdmaSignalStrength(Json::Value &jsonTdscdmaSS);
    void FillNrSignalStrength(Json::Value &jsonNrSS);
};

class SignalStrengthAdapterFactoryJson {
  private:
    SignalStrengthParserV4Json mSignalStrengthV4;

  public:
    SignalStrengthAdapterFactoryJson() = default;
    virtual ~SignalStrengthAdapterFactoryJson() = default;
    SignalStrengthAdapterJson *GetSignalStrengthJsonAdapter(int ssVer);
};

/**
 * ProtocolSignalStrengthIndAdapterJson
 */
class ProtocolSignalStrengthIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolSignalStrengthIndAdapterInterface {
  private:
    RIL_SignalStrength_V1_6 mSignalStrength;

  public:
    ProtocolSignalStrengthIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSignalStrengthIndAdapterJson() = default;

  public:
    RIL_SignalStrength_V1_6 &GetSignalStrength(int ssVer);

    //For ProtocolJsonIndAdapter
    const char *GetParameter() const {
        return ProtocolJsonIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolJsonIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscSetDeviceInfoAdapterJson
 */
class ProtocolMiscSetDeviceInfoAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetDeviceInfoAdapterInterface {
  private:
    char *mDeviceInfo;
    RilProperty mRilProp;

  private:
    void Init();

  public:
    ProtocolMiscSetDeviceInfoAdapterJson(const ModemData *pModemData);
    ProtocolMiscSetDeviceInfoAdapterJson(const ProtocolMiscSetDeviceInfoAdapterJson &) = delete;
    ProtocolMiscSetDeviceInfoAdapterJson& operator=(ProtocolMiscSetDeviceInfoAdapterJson const&) = delete;
    virtual ~ProtocolMiscSetDeviceInfoAdapterJson();

  public:
    int GetVersionInfo(const char *input);

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetApSystemTimeAdapterJson
 */
class ProtocolMiscSetApSystemTimeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscSetApSystemTimeAdapterInterface {
  public:
    ProtocolMiscSetApSystemTimeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscSetApSystemTimeAdapterJson() = default;

  public:
    int GetResult() const;

    //For ProtocolJsonRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscIMEIAdapterJson
 */
class ProtocolMiscIMEIAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscIMEIAdapterInterface {
  public:
    ProtocolMiscIMEIAdapterJson(const ModemData *pModemData);
    ProtocolMiscIMEIAdapterJson(const ProtocolMiscIMEIAdapterJson &) = delete;
    ProtocolMiscIMEIAdapterJson& operator=(ProtocolMiscIMEIAdapterJson const&) = delete;
    virtual ~ProtocolMiscIMEIAdapterJson();

  public:
    int GetIMEILen() const;
    const BYTE *GetIMEI() const;
    // For ProtocolRespAdatper
    UINT GetErrorCode() const;

  private:
    int m_nImeiLen;
    BYTE *m_pImei;

    void Init();
    void SetIMEI();
};

/**
 * ProtocolMiscIMEISVAdapterJson
 */
class ProtocolMiscIMEISVAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscIMEISVAdapterInterface {
  public:
    ProtocolMiscIMEISVAdapterJson(const ModemData *pModemData);
    ProtocolMiscIMEISVAdapterJson(const ProtocolMiscIMEISVAdapterJson &) = delete;
    ProtocolMiscIMEISVAdapterJson& operator=(ProtocolMiscIMEISVAdapterJson const&) = delete;
    virtual ~ProtocolMiscIMEISVAdapterJson();

  public:
    int GetIMEISVLen() const;
    const BYTE *GetIMEISV() const;
    // For ProtocolRespAdatper
    UINT GetErrorCode() const;

  private:
    int m_nImeisvLen;
    BYTE *m_pImeisv;

    void Init();
    void SetIMEISV();
};

/**
 * ProtocolMiscDeviceIDAdapterJson
 */
class ProtocolMiscDeviceIDAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscDeviceIDAdapterInterface {
  public:
    ProtocolMiscDeviceIDAdapterJson(const ModemData *pModemData);
    ProtocolMiscDeviceIDAdapterJson(const ProtocolMiscDeviceIDAdapterJson &) = delete;
    ProtocolMiscDeviceIDAdapterJson& operator=(ProtocolMiscDeviceIDAdapterJson const&) = delete;
    virtual ~ProtocolMiscDeviceIDAdapterJson();

  public:
    int GetIMEILen() const {
        return m_nImeiLen;
    }
    const BYTE *GetIMEI() const {
        return m_pImei;
    }
    int GetIMEISVLen() const {
        return m_nImeisvLen;
    }
    const BYTE *GetIMEISV() const {
        return m_pImeisv;
    }
    int GetMEIDLen() const {
        return m_nMeidLen;
    }
    const BYTE *GetMEID() const {
        return m_pMeid;
    }
    int GetESNLen() const {
        return m_nEsnLen;
    }
    const BYTE *GetESN() const {
        return m_pEsn;
    }

    // For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void Init();
    void SetDeviceId();

    // imei
    BYTE *m_pImei;
    int m_nImeiLen;

    // imeisv
    BYTE *m_pImeisv;
    int m_nImeisvLen;

    // esn
    BYTE *m_pEsn;
    int m_nEsnLen;

    // meid
    BYTE *m_pMeid;
    int m_nMeidLen;
};

/*
 * ProtocolMiscGetStackStatusAdapterJson
 */
class ProtocolMiscGetStackStatusAdapterJson : public ProtocolJsonRespAdapter,
                                              public ProtocolMiscGetStackStatusAdapterInterface {
  public:
    ProtocolMiscGetStackStatusAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_STACK_STATUS) {}
    virtual ~ProtocolMiscGetStackStatusAdapterJson() = default;

  public:
    int GetMode();

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscPhoneResetAdapterJson
 */
class ProtocolMiscPhoneResetAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscPhoneResetAdapterInterface {
  public:
    ProtocolMiscPhoneResetAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData, SIT_IND_PHONE_RESET) {}
    virtual ~ProtocolMiscPhoneResetAdapterJson() = default;

  public:
    BYTE GetResetType();
    BYTE GetResetCause();

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscDataStateChangeAdapterJson
 */
class ProtocolMiscDataStateChangeAdapterJson : public ProtocolJsonIndAdapter, public ProtocolMiscDataStateChangeAdapterInterface {
  private:
    BYTE m_ExpectedState;

  public:
    ProtocolMiscDataStateChangeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscDataStateChangeAdapterJson() = default;

  public:
    BYTE GetExpectedState();

    //For ProtocolIndAdapter
    const char *GetParameter() const;
    UINT GetParameterLength() const;
};

/**
 * ProtocolMiscGetTraceDumpCfgAdapterJson
 */
class ProtocolMiscGetTraceDumpCfgAdapterJson : public ProtocolJsonRespAdapter, public ProtocolMiscGetTraceDumpCfgAdapterInterface {
  public:
    ProtocolMiscGetTraceDumpCfgAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolMiscGetTraceDumpCfgAdapterJson() = default;

  public:
    BYTE GetConfig() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

#endif /* __PROTOCOL_MISC_ADAPTER_JSON_H__ */
