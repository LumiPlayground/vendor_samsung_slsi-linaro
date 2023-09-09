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
 * protocolmiscadapterlegacy.h
 *
 *  Created on: 2014. 6. 30.
 *      Author: m.afzal
 */

#ifndef __PROTOCOL_MISC_ADAPTER_LEGACY_H__
#define __PROTOCOL_MISC_ADAPTER_LEGACY_H__

#include "protocolmiscadapterinterface.h"
#include "protocoladapter.h"
#include "sitdef.h"
#include "rilproperty.h"

/**
 * ProtocolMiscResponseAdapterLegacy
 */
class ProtocolMiscResponseAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscResponseAdapterInterface {
  public:
    ProtocolMiscResponseAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscResponseAdapterLegacy() = default;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscVersionAdapterLegacy
 */
class ProtocolMiscVersionAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscVersionAdapterInterface {
  public:
    ProtocolMiscVersionAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscVersionAdapterLegacy() = default;

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

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetTtyAdapterLegacy
 */
class ProtocolMiscGetTtyAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetTtyAdapterInterface {
  public:
    ProtocolMiscGetTtyAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetTtyAdapterLegacy() = default;

  public:
    int GetTtyMode() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolSignalStrengthAdapterLegacy
 */
class ProtocolSignalStrengthAdapterLegacy : public ProtocolRespAdapter, public ProtocolSignalStrengthAdapterInterface {
  private:
    RIL_SignalStrength_V1_6 mSignalStrength;

  public:
    ProtocolSignalStrengthAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSignalStrengthAdapterLegacy() = default;

  public:
    RIL_SignalStrength_V1_6 &GetSignalStrength(int ssVer);

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * SignalStrengthAdapter
 */
class SignalStrengthAdapter {
  public:
    RIL_SignalStrength_V1_6 mSignalStrength;
    void *mData;
    int mDataLen;
    int mMask;

  public:
    SignalStrengthAdapter();
    SignalStrengthAdapter(const SignalStrengthAdapter &) = delete;
    SignalStrengthAdapter& operator=(SignalStrengthAdapter const&) = delete;
    virtual ~SignalStrengthAdapter();
    void Init(void *data, int dataLen, int mask);
    virtual RIL_SignalStrength_V1_6 &GetSignalStrength() = 0;

    static void FillDefaultSignalStrength(RIL_SignalStrength_V1_6 &out);
    static void FillGwSignalStrength(RIL_GSM_SignalStrength_v12 &out, GW_SIGNAL_STRENGTH &currentGWSignalStrength);
    static void FillGwSignalStrength(RIL_WCDMA_SignalStrength_V1_2 &out, GW_SIGNAL_STRENGTH &currentGWSignalStrength);
    static void FillGsmSignalStrength(RIL_GSM_SignalStrength_v12 &out, GSM_SIGNAL_STRENGTH &currentGsmSignalStrength);
    static void FillWcdmaSignalStrength(RIL_WCDMA_SignalStrength_V1_2 &out, WCDMA_SIGNAL_STRENGTH &currentWcdmaSignalStrength);
    static void FillCdmaSignalStrength(RIL_CDMA_SignalStrength &out, CDMA_SIGNAL_STRENGTH &currentCdmaSignalStrength);
    static void FillEvdoSignalStrength(RIL_EVDO_SignalStrength &out, EVDO_SIGNAL_STRENGTH &currentEvdoSignalStrength);
    static void FillLteSignalStrength(RIL_LTE_SignalStrength_V1_6 &out, LTE_SIGNAL_STRENGTH &currentLteSignalStrength);
    static void FillLteSignalStrengthV4(RIL_LTE_SignalStrength_V1_6 &out, LTE_SIGNAL_STRENGTH_V4 &currentLteSignalStrength);
    static void FillTdscdmaSignalStrength(RIL_TD_SCDMA_SignalStrength_V1_2 &out, TD_SCDMA_SIGNAL_STRENGTH &currentTdscdmaSignalStrength);
    static void FillTdscdmaSignalStrengthV2(RIL_TD_SCDMA_SignalStrength_V1_2 &out, TD_SCDMA_SIGNAL_STRENGTH_V2 &currentTdscdmaSignalStrength);
    static void FillNrSignalStrengthV2(RIL_NR_SignalStrength_V1_6 &out, NR_SIGNAL_STRENGTH &currentNrSignalStrength);
    static void FillNrSignalStrengthV4(RIL_NR_SignalStrength_V1_6 &out, NR_SIGNAL_STRENGTH_V4 &currentNrSignalStrength);

  protected:
    void FillDefaultSignalStrength();
    void FillGwSignalStrength(GW_SIGNAL_STRENGTH &currentGWSignalStrength);
    void FillGsmSignalStrength(GSM_SIGNAL_STRENGTH &currentGsmSignalStrength);
    void FillWcdmaSignalStrength(WCDMA_SIGNAL_STRENGTH &currentWcdmaSignalStrength);
    void FillCdmaSignalStrength(CDMA_SIGNAL_STRENGTH &currentCdmaSignalStrength);
    void FillEvdoSignalStrength(EVDO_SIGNAL_STRENGTH &currentEvdoSignalStrength);
    void FillLteSignalStrength(LTE_SIGNAL_STRENGTH &currentLteSignalStrength);
    void FillLteSignalStrengthV4(LTE_SIGNAL_STRENGTH_V4 &currentLteSignalStrength);
    void FillTdscdmaSignalStrength(TD_SCDMA_SIGNAL_STRENGTH &currentTdscdmaSignalStrength);
    void FillTdscdmaSignalStrengthV2(TD_SCDMA_SIGNAL_STRENGTH_V2 &currentTdscdmaSignalStrength);
    void FillNrSignalStrengthV2(NR_SIGNAL_STRENGTH &currentNrSignalStrength);
    void FillNrSignalStrengthV4(NR_SIGNAL_STRENGTH_V4 &currentNrSignalStrength);
};

class SignalStrengthParserV1 : public SignalStrengthAdapter {
  public:
    SignalStrengthParserV1()
        : SignalStrengthAdapter() {}
    RIL_SignalStrength_V1_6 &GetSignalStrength();
    void FillSignalStrength();
};

class SignalStrengthParserV2 : public SignalStrengthAdapter {
  public:
    SignalStrengthParserV2()
        : SignalStrengthAdapter() {}
    RIL_SignalStrength_V1_6 &GetSignalStrength();
    void FillSignalStrength();
};

class SignalStrengthParserV4 : public SignalStrengthAdapter {
  public:
    SignalStrengthParserV4()
        : SignalStrengthAdapter() {}
    RIL_SignalStrength_V1_6 &GetSignalStrength();
    void FillSignalStrength();
};

class SignalStrengthAdapterFactory {
  private:
    SignalStrengthParserV1 mSignalStrengthV1;
    SignalStrengthParserV2 mSignalStrengthV2;
    SignalStrengthParserV4 mSignalStrengthV4;

  public:
    SignalStrengthAdapterFactory() {}
    virtual ~SignalStrengthAdapterFactory() {}
    SignalStrengthAdapter *GetSignalStrengthAdapter(int ssVer);
};

/**
 * ProtocolSignalStrengthIndAdapterLegacy
 */
class ProtocolSignalStrengthIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolSignalStrengthIndAdapterInterface {
  private:
    RIL_SignalStrength_V1_6 mSignalStrength;

  public:
    ProtocolSignalStrengthIndAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSignalStrengthIndAdapterLegacy() = default;

  public:
    RIL_SignalStrength_V1_6 &GetSignalStrength(int ssVer);

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

// A helper functions to get details of NITZ time
struct ProtocolNitzData {
    const void *data = 0;
    size_t size = 0;
    ProtocolNitzData()=default;
    void setNitzData(const void *data, size_t size) {
        this->data = data;
        this->size = size;
    }
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
    int64_t age() const;
};

/**
 * ProtocolMiscNITZTimeAdapterLegacy
 */
class ProtocolMiscNITZTimeAdapterLegacy : public ProtocolIndAdapter,
        public ProtocolMiscNITZTimeAdapterInterface {
private:
    ProtocolNitzData mNitzData;
public:
    ProtocolMiscNITZTimeAdapterLegacy(const ModemData *pModemData);
    ~ProtocolMiscNITZTimeAdapterLegacy() = default;

    int TimeInfoType() const { return mNitzData.TimeInfoType(); }
    int DayLightValid() const { return mNitzData.DayLightValid(); }
    int Year() const { return mNitzData.Year(); }
    int Month() const { return mNitzData.Month(); }
    int Day() const { return mNitzData.Day(); }
    int Hour() const { return mNitzData.Hour(); }
    int Minute() const { return mNitzData.Minute(); }
    int Second() const { return mNitzData.Second(); }
    int TimeZone() const { return mNitzData.TimeZone(); }
    int DayLightAdjust() const { return mNitzData.DayLightAdjust(); }
    int DayofWeek() const { return mNitzData.DayofWeek(); }
    int GetMMInfo() const { return mNitzData.GetMMInfo(); }
    const BYTE *GetPLMN() const { return mNitzData.GetPLMN(); }
    int64_t age() const { return mNitzData.age(); }

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscGetNITZTimeAdapterLegacy
 */
class ProtocolMiscGetNITZTimeAdapterLegacy : public ProtocolRespAdapter,
        public ProtocolMiscGetNITZTimeAdapterInterface {
private:
    ProtocolNitzData mNitzData;
public:
    ProtocolMiscGetNITZTimeAdapterLegacy(const ModemData *pModemData);
    ~ProtocolMiscGetNITZTimeAdapterLegacy() = default;

    int TimeInfoType() const { return mNitzData.TimeInfoType(); }
    int DayLightValid() const { return mNitzData.DayLightValid(); }
    int Year() const { return mNitzData.Year(); }
    int Month() const { return mNitzData.Month(); }
    int Day() const { return mNitzData.Day(); }
    int Hour() const { return mNitzData.Hour(); }
    int Minute() const { return mNitzData.Minute(); }
    int Second() const { return mNitzData.Second(); }
    int TimeZone() const { return mNitzData.TimeZone(); }
    int DayLightAdjust() const { return mNitzData.DayLightAdjust(); }
    int DayofWeek() const { return mNitzData.DayofWeek(); }
    int GetMMInfo() const { return mNitzData.GetMMInfo(); }
    const BYTE *GetPLMN() const { return mNitzData.GetPLMN(); }
    int64_t age() const { return mNitzData.age(); }

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
    const char *GetParameter() const {
        return ProtocolRespAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolRespAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscIMEIAdapterLegacy
 */
class ProtocolMiscIMEIAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscIMEIAdapterInterface {
  public:
    ProtocolMiscIMEIAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscIMEIAdapterLegacy() = default;

  public:
    int GetIMEILen() const;
    const BYTE *GetIMEI() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscIMEISVAdapterLegacy
 */
class ProtocolMiscIMEISVAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscIMEISVAdapterInterface {
  public:
    ProtocolMiscIMEISVAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscIMEISVAdapterLegacy() = default;

  public:
    int GetIMEISVLen() const;
    const BYTE *GetIMEISV() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscDeviceIDAdapterLegacy
 */
class ProtocolMiscDeviceIDAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscDeviceIDAdapterInterface {
  public:
    ProtocolMiscDeviceIDAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscDeviceIDAdapterLegacy() = default;

  public:
    int GetIMEILen() const;
    const BYTE *GetIMEI() const;
    int GetIMEISVLen() const;
    const BYTE *GetIMEISV() const;
    int GetMEIDLen() const;
    const BYTE *GetMEID() const;
    int GetESNLen() const;
    const BYTE *GetESN() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscOemDisplayEngAdapterLegacy
 */
class ProtocolMiscOemDisplayEngAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscOemDisplayEngAdapterInterface {
  public:
    ProtocolMiscOemDisplayEngAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolMiscOemDisplayEngAdapterLegacy() = default;

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscPhoneResetAdapterLegacy
 */
class ProtocolMiscPhoneResetAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscPhoneResetAdapterInterface {
  private:
    BYTE m_ResetType;
    BYTE m_ResetCause;

  public:
    ProtocolMiscPhoneResetAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolMiscPhoneResetAdapterLegacy() = default;

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
 * ProtocolMiscDataStateChangeAdapterLegacy
 */
class ProtocolMiscDataStateChangeAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscDataStateChangeAdapterInterface {
  private:
    BYTE m_ExpectedState;

  public:
    ProtocolMiscDataStateChangeAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolMiscDataStateChangeAdapterLegacy() = default;

  public:
    BYTE GetExpectedState();

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscGetActivityInfoAdapterLegacy
 */
class ProtocolMiscGetActivityInfoAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetActivityInfoAdapterInterface {
  public:
    ProtocolMiscGetActivityInfoAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetActivityInfoAdapterLegacy() = default;

  public:
    UINT32 GetSleepPeriod() const;
    UINT32 GetIdlePeriod() const;
    UINT32 *GetTxPeriod() const;
    UINT32 GetRxPeriod() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy
 */
class ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy : public ProtocolRespAdapter,
        public ProtocolMiscGetActivityInfoRatSpecificAdapterInterface {
private:
    android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsInfo mInfo;
public:
    ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy(const ModemData *pModemData);
public:
    const android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsInfo& info() const { return mInfo; }

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
private:
    void init();
};

/**
 * ProtocolMiscGetMslCodeAdapterLegacy
 */
class ProtocolMiscGetMslCodeAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetMslCodeAdapterInterface {
  public:
    ProtocolMiscGetMslCodeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetMslCodeAdapterLegacy() = default;

  public:
    const char *getMslCode() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscPinControlAdapterLegacy
 */
class ProtocolMiscPinControlAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscPinControlAdapterInterface {
  private:
    BYTE m_Signal;
    BYTE m_Status;

  public:
    ProtocolMiscPinControlAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolMiscPinControlAdapterLegacy() = default;

  public:
    BYTE GetSignal();
    BYTE GetStatus();

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolGetVoiceOperationAdapterLegacy
 */
class ProtocolGetVoiceOperationAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetVoiceOperationAdapterInterface {
  public:
    ProtocolGetVoiceOperationAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolGetVoiceOperationAdapterLegacy() = default;

  public:
    int GetMode() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetPreferredCallCapabilityAdapterLegacy
 */
class ProtocolMiscGetPreferredCallCapabilityAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetPreferredCallCapabilityAdapterInterface {
  public:
    ProtocolMiscGetPreferredCallCapabilityAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetPreferredCallCapabilityAdapterLegacy() = default;

  public:
    int GetMode() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetManualBandModeAdapterLegacy
 */
class ProtocolMiscSetManualBandModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetManualBandModeAdapterInterface {
  public:
    ProtocolMiscSetManualBandModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetManualBandModeAdapterLegacy() = default;

  public:
    BYTE GetCause();

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetRfDesenseModeAdapterLegacy
 */
class ProtocolMiscSetRfDesenseModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetRfDesenseModeAdapterInterface {
  public:
    ProtocolMiscSetRfDesenseModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetRfDesenseModeAdapterLegacy() = default;

  public:
    BYTE GetCause();

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetHwConfigAdapterLegacy
 */
class ProtocolMiscGetHwConfigAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetHwConfigAdapterInterface {
  public:
    ProtocolMiscGetHwConfigAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetHwConfigAdapterLegacy() = default;

  public:
    int GetNum() const;
    int GetData(RIL_HardwareConfig *pRsp, int num) const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscHwConfigChangeAdapterLegacy
 */
class ProtocolMiscHwConfigChangeAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscHwConfigChangeAdapterInterface {
  public:
    ProtocolMiscHwConfigChangeAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolMiscHwConfigChangeAdapterLegacy() = default;

  public:
    int GetNum() const;
    int GetData(RIL_HardwareConfig *pRsp, int num) const;

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscCdmaPrlChangeAdapterLegacy
 */
class ProtocolMiscCdmaPrlChangeAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscCdmaPrlChangeAdapterInterface {
  public:
    ProtocolMiscCdmaPrlChangeAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolMiscCdmaPrlChangeAdapterLegacy() = default;

  public:
    int GetPrlVersion() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscLceAdapterLegacy
 */
class ProtocolMiscLceAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscLceAdapterInterface {
  private:
    int m_lceStatus;
    unsigned int m_actualIntervalMs;
    unsigned int m_dlCapacityKbps;
    unsigned int m_ulCapacityKbps;
    int m_confidenceLevel;
    int m_lceSuspended;

  public:
    ProtocolMiscLceAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolMiscLceAdapterLegacy() = default;
    void Init();
    int getLceStatus() {
        return m_lceStatus;
    }
    unsigned int getActualIntervalMs() {
        return m_actualIntervalMs;
    }
    unsigned int getDlCapacityKbps() {
        return m_dlCapacityKbps;
    }
    unsigned int getUlCapacityKbps() {
        return m_ulCapacityKbps;
    }
    int getConfidencelevel() {
        return m_confidenceLevel;
    }
    int getLceSuspended() {
        return m_lceSuspended;
    }
    RIL_Errno GetRilErrorCode() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscLceIndAdapterLegacy
 */
class ProtocolMiscLceIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscLceIndAdapterInterface {
  public:
    ProtocolMiscLceIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolMiscLceIndAdapterLegacy() = default;

  public:
    int GetDLLc() const;
    int GetULLc() const;
    int GetConfLevel() const;
    int GetIsSuspended() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolCdmaSubscriptionAdapterLegacy
 */
class ProtocolCdmaSubscriptionAdapterLegacy : public ProtocolRespAdapter, public ProtocolCdmaSubscriptionAdapterInterface {
  private:
    char m_szMdn[MAX_CDMA_MDN_LEN + 1];
    char m_szMin[MAX_CDMA_MIN_LEN + 1];
    WORD m_wSid;
    WORD m_wNid;
    UINT m_uPrlVersion;

  public:
    ProtocolCdmaSubscriptionAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolCdmaSubscriptionAdapterLegacy() = default;
    void Init();
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

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSarControlStateAdapterLegacy
 */
class ProtocolMiscSarControlStateAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscSarControlStateAdapterInterface {
  private:
    BYTE m_DeviceState;

  public:
    ProtocolMiscSarControlStateAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolMiscSarControlStateAdapterLegacy() = default;

  public:
    BYTE GetDeviceState();

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscSarRfConnectionAdapterLegacy
 */
class ProtocolMiscSarRfConnectionAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscSarRfConnectionAdapterInterface {
  private:
    BYTE m_RfState;

  public:
    ProtocolMiscSarRfConnectionAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolMiscSarRfConnectionAdapterLegacy() = default;

  public:
    BYTE GetRfState();

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscGetSarStateAdapterLegacy
 */
class ProtocolMiscGetSarStateAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetSarStateAdapterInterface {
  public:
    ProtocolMiscGetSarStateAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetSarStateAdapterLegacy() = default;

  public:
    int GetSarState() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscRssiScanResultAdapterLegacy
 */
class ProtocolMiscRssiScanResultAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscRssiScanResultAdapterInterface {
  public:
    enum { DEFAULT_RSSI = -2040,
           INVALID_RSSI = 0x7FFFFFFF };

  public:
    ProtocolMiscRssiScanResultAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolMiscRssiScanResultAdapterLegacy() = default;

  public:
    int GetTotalPage();
    int GetCurrentPage();
    int GetStartFrequency();
    int GetEndFrequency();
    int GetStep();
    int GetScanResultSize();
    INT16 *GetScanResult();

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscATCommandAdapterLegacy
 */
class ProtocolMiscATCommandAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscATCommandAdapterInterface {
  private:
    char *mCommand;
    unsigned int mCommandLength;

  private:
    void Init();

  public:
    ProtocolMiscATCommandAdapterLegacy(const ModemData *pModemData);
    ProtocolMiscATCommandAdapterLegacy(const ProtocolMiscATCommandAdapterLegacy &) = delete;
    ProtocolMiscATCommandAdapterLegacy& operator=(ProtocolMiscATCommandAdapterLegacy const&) = delete;
    virtual ~ProtocolMiscATCommandAdapterLegacy();

  public:
    const char *GetCommand() const {
        return mCommand;
    }
    unsigned int GetCommandLength() const {
        return mCommandLength;
    }

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscGetRadioNodeAdapterLegacy
 */
class ProtocolMiscGetRadioNodeAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetRadioNodeAdapterInterface {
  public:
    ProtocolMiscGetRadioNodeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetRadioNodeAdapterLegacy() = default;

  public:
    const char *GetValue() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetVoLteProvisionUpdateAdapterLegacy
 */
class ProtocolMiscGetVoLteProvisionUpdateAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetVoLteProvisionUpdateAdapterInterface {
  public:
    ProtocolMiscGetVoLteProvisionUpdateAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetVoLteProvisionUpdateAdapterLegacy() = default;

  public:
    int GetStatus();

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetVoLteProvisionUpdateAdapterLegacy
 */
class ProtocolMiscSetVoLteProvisionUpdateAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetVoLteProvisionUpdateAdapterInterface {
  public:
    ProtocolMiscSetVoLteProvisionUpdateAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetVoLteProvisionUpdateAdapterLegacy() = default;

  public:
    int GetResult();

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/*
 * ProtocolMiscGetStackStatusAdapterLegacy
 */
class ProtocolMiscGetStackStatusAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetStackStatusAdapterInterface {
  public:
    ProtocolMiscGetStackStatusAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetStackStatusAdapterLegacy() = default;

  public:
    int GetMode();

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolOemModemInfoAdapterLegacy
 */
class ProtocolOemModemInfoAdapterLegacy : public ProtocolRespAdapter, public ProtocolOemModemInfoAdapterInterface {
  public:
    ProtocolOemModemInfoAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolOemModemInfoAdapterLegacy() = default;

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
 * ProtocolOemModemInfoIndAdapterLegacy
 */
class ProtocolOemModemInfoIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolOemModemInfoIndAdapterInterface {
  public:
    ProtocolOemModemInfoIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolOemModemInfoIndAdapterLegacy() = default;

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
 * ProtocolOemSwitchModemFunctionAdapterLegacy
 */
class ProtocolOemSwitchModemFunctionAdapterLegacy : public ProtocolRespAdapter, public ProtocolOemSwitchModemFunctionAdapterInterface {
  public:
    ProtocolOemSwitchModemFunctionAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolOemSwitchModemFunctionAdapterLegacy() = default;

  public:
    BYTE GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetSelflogAdapterLegacy
 */
class ProtocolMiscSetSelflogAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetSelflogAdapterInterface {
  public:
    ProtocolMiscSetSelflogAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetSelflogAdapterLegacy() = default;

  public:
    int GetSelflogResult();

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetSelflogStatusAdapterLegacy
 */
class ProtocolMiscGetSelflogStatusAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetSelflogStatusAdapterInterface {
  public:
    ProtocolMiscGetSelflogStatusAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetSelflogStatusAdapterLegacy() = default;

  public:
    int GetSelflogStatus();

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSelflogStatusAdapterLegacy
 */
class ProtocolMiscSelflogStatusAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscSelflogStatusAdapterInterface {
  public:
    ProtocolMiscSelflogStatusAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolMiscSelflogStatusAdapterLegacy() = default;

  public:
    BYTE GetIndSelflogStatus();

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolOemGetCqiInfoAdapterLegacy
 */
class ProtocolOemGetCqiInfoAdapterLegacy : public ProtocolRespAdapter, public ProtocolOemGetCqiInfoAdapterInterface {
  public:
    ProtocolOemGetCqiInfoAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolOemGetCqiInfoAdapterLegacy() = default;

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
 * ProtocolMiscSetTcsFciAdapterLegacy
 */
class ProtocolMiscSetTcsFciAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetTcsFciAdapterInterface {
  public:
    ProtocolMiscSetTcsFciAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetTcsFciAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetTcsFciAdapterLegacy
 */
class ProtocolMiscGetTcsFciAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetTcsFciAdapterInterface {
  public:
    ProtocolMiscGetTcsFciAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetTcsFciAdapterLegacy() = default;

  public:
    const char *GetFci() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolCaBandwidthFilterIndAdapterLegacy
 */
class ProtocolCaBandwidthFilterIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolCaBandwidthFilterIndAdapterInterface {
  public:
    ProtocolCaBandwidthFilterIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolCaBandwidthFilterIndAdapterLegacy() = default;

  public:
    int GetCaConfig() const;
    int GetNRB() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscSetModemLogDumpAdapterLegacy
 */
class ProtocolMiscSetModemLogDumpAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetModemLogDumpAdapterInterface {
  public:
    ProtocolMiscSetModemLogDumpAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetModemLogDumpAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy
 */
class ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscCurrentLinkCapacityEstimateAdapterInterface {
  private:
    CURRENT_LINK_CAPA_ESTIMATE mCurLinkCapaEstimate;

  private:
    void InitLinkCapaEstimate();

  public:
    ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy() = default;

  public:
    int GetDlCapaKbps() const;
    int GetUlCapaKbps() const;
    int GetSecDlCapaKbps() const;
    int GetSecUlCapaKbps() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscEndcCapabilityIndAdapterLegacy
 */
class ProtocolMiscEndcCapabilityIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscEndcCapabilityIndAdapterInterface {
  public:
    ProtocolMiscEndcCapabilityIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolMiscEndcCapabilityIndAdapterLegacy() = default;

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
 * ProtocolMiscSetSelflogProfileAdapterLegacy
 */
class ProtocolMiscSetSelflogProfileAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetSelflogProfileAdapterInterface {
  public:
    ProtocolMiscSetSelflogProfileAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetSelflogProfileAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetForbidLteCellAdapterLegacy
 */
class ProtocolMiscSetForbidLteCellAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetForbidLteCellAdapterInterface {
  public:
    ProtocolMiscSetForbidLteCellAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetForbidLteCellAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetApSystemTimeAdapterLegacy
 */
class ProtocolMiscSetApSystemTimeAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetApSystemTimeAdapterInterface {
  public:
    ProtocolMiscSetApSystemTimeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetApSystemTimeAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetForbidFrequencyAdapterLegacy
 */
class ProtocolMiscSetForbidFrequencyAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetForbidFrequencyAdapterInterface {
  public:
    ProtocolMiscSetForbidFrequencyAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetForbidFrequencyAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetForbidLteBandAdapterLegacy
 */
class ProtocolMiscSetForbidLteBandAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetForbidLteBandAdapterInterface {
  public:
    ProtocolMiscSetForbidLteBandAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetForbidLteBandAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetForbidLteTacAdapterLegacy
 */
class ProtocolMiscSetForbidLteTacAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetForbidLteTacAdapterInterface {
  public:
    ProtocolMiscSetForbidLteTacAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetForbidLteTacAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetAheadRreAdapterLegacy
 */
class ProtocolMiscSetAheadRreAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetAheadRreAdapterInterface {
  public:
    ProtocolMiscSetAheadRreAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetAheadRreAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetForbidNrCellAdapterLegacy
 */
class ProtocolMiscSetForbidNrCellAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetForbidNrCellAdapterInterface {
  public:
    ProtocolMiscSetForbidNrCellAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetForbidNrCellAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetBinaryVersionInfoAdapterLegacy
 */
class ProtocolMiscGetBinaryVersionInfoAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetBinaryVersionInfoAdapterInterface {
  public:
    ProtocolMiscGetBinaryVersionInfoAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetBinaryVersionInfoAdapterLegacy() = default;

  public:
    int GetVersionInfoLength() const;
    const char * GetData() const;

    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolRfConnectionStateAdapterLegacy
 */
class ProtocolRfConnectionStateAdapterLegacy : public ProtocolIndAdapter, public ProtocolRfConnectionStateAdapterInterface {
  public:
    ProtocolRfConnectionStateAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolRfConnectionStateAdapterLegacy() = default;

  public:
    BYTE GetState();

    //For ProtocolIndAdapter
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscGetCdmaCapabilityInfoAdapterLegacy
 */
class ProtocolMiscGetCdmaCapabilityInfoAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetCdmaCapabilityInfoAdapterInterface {
  public:
    ProtocolMiscGetCdmaCapabilityInfoAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetCdmaCapabilityInfoAdapterLegacy() = default;

  public:
    BYTE GetCapability() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscSetDeviceInfoAdapterLegacy
 */
class ProtocolMiscSetDeviceInfoAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetDeviceInfoAdapterInterface {
  private:
    char *mDeviceInfo;
    RilProperty mRilProp;

  private:
    void Init();

  public:
    ProtocolMiscSetDeviceInfoAdapterLegacy(const ModemData *pModemData);
    ProtocolMiscSetDeviceInfoAdapterLegacy(const ProtocolMiscSetDeviceInfoAdapterLegacy &) = delete;
    ProtocolMiscSetDeviceInfoAdapterLegacy& operator=(ProtocolMiscSetDeviceInfoAdapterLegacy const&) = delete;
    virtual ~ProtocolMiscSetDeviceInfoAdapterLegacy();

  public:
    int GetVersionInfo(const char *input);

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscBaseStationIndAdapterLegacy
 */
class ProtocolMiscBaseStationIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscBaseStationIndAdapterInterface {
  public:
    ProtocolMiscBaseStationIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolMiscBaseStationIndAdapterLegacy() = default;

  public:
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscSetActiveVsimAdapterLegacy
 */
class ProtocolMiscSetActiveVsimAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscSetActvieVsimAdapterInterface {
  public:
    ProtocolMiscSetActiveVsimAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscSetActiveVsimAdapterLegacy() = default;

  public:
    int GetResult() const;

    //For ProtocolRespAdatper
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetModemActivityInfoAdapterLegacy
 */
class ProtocolMiscGetModemActivityInfoAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetModemActivityInfoAdapterInterface {
  public:
    ProtocolMiscGetModemActivityInfoAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetModemActivityInfoAdapterLegacy() = default;

  public:
    const char *GetParameter() const {
        return ProtocolRespAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolRespAdapter::GetParameterLength();
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscDropMtCallInfoIndAdapterLegacy
 */
class ProtocolMiscDropMtCallInfoIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscDropMtCallInfoIndAdapterInterface {
  public:
    ProtocolMiscDropMtCallInfoIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolMiscDropMtCallInfoIndAdapterLegacy() = default;

  public:
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy
 */
class ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscCarrierConfigSimInfoIndAdapterInterface {
public:
    ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy() = default;

    const std::string &getMccMnc();
    const std::string &getIccid();
    const std::string &getImsi();
    const std::string &getSpn();
    const std::string &getPnn();
    const std::string &getGid1();
    const std::string &getGid2();
    const std::string &getPrivilegeAccessRules();

private:
    std::string mMccMnc;
    std::string mIccid;
    std::string mImsi;
    std::string mSpn;
    std::string mPnn;
    std::string mGid1;
    std::string mGid2;
    std::string mPrivilegeAccessRules;
};

/**
 * ProtocolMiscCarrierConfigStatusIndAdapterLegacy
 */
class ProtocolMiscCarrierConfigStatusIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolMiscCarrierConfigStatusIndAdapterInterface {
public:
    ProtocolMiscCarrierConfigStatusIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}

public:
    int GetStatus() const;
    int GetCause() const;
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolMiscGetTraceDumpCfgAdapterLegacy
 */
class ProtocolMiscGetTraceDumpCfgAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetTraceDumpCfgAdapterInterface {
public:
    ProtocolMiscGetTraceDumpCfgAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetTraceDumpCfgAdapterLegacy() = default;

public:
    BYTE GetConfig() const;
    const char *GetParameter() const {
        return ProtocolRespAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolRespAdapter::GetParameterLength();
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetUeOperationModeAdapterLegacy
 */
class ProtocolMiscGetUeOperationModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetUeOperationModeAdapterInterface {
  public:
    ProtocolMiscGetUeOperationModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetUeOperationModeAdapterLegacy() = default;

  public:
    BYTE GetMode() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetNrrrmMinReqEnableAdapterLegacy
 */
class ProtocolMiscGetNrrrmMinReqEnableAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetNrrrmMinReqEnableAdapterInterface {
  public:
    ProtocolMiscGetNrrrmMinReqEnableAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetNrrrmMinReqEnableAdapterLegacy() = default;

  public:
    BYTE GetEnable() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolMiscGetVcrtAdapterLegacy
 */
class ProtocolMiscGetVcrtAdapterLegacy : public ProtocolRespAdapter, public ProtocolMiscGetVcrtAdapterInterface {
  public:
    ProtocolMiscGetVcrtAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolMiscGetVcrtAdapterLegacy() = default;

  public:
    BYTE GetVcrtMode() const;

    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

#endif /* __PROTOCOL_MISC_ADAPTER_LEGACY_H__ */
