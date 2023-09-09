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
 * protocolmiscadapter.h
 *
 *  Created on: 2021. 06. 21.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_MISC_ADAPTER_H__
#define __PROTOCOL_MISC_ADAPTER_H__

#include "protocolmiscadapterinterface.h"
#include "protocoladapter.h"

class ModemData;

/**
 * ProtocolMiscResponseAdapter
 */
class ProtocolMiscResponseAdapter : public ProtocolMiscResponseAdapterInterface {
private:
    ProtocolMiscResponseAdapterInterface *protocolMiscResponseAdapterInterface = NULL;

public:
    ProtocolMiscResponseAdapter(const ModemData *pModemData);
    ProtocolMiscResponseAdapter(const ProtocolMiscResponseAdapter&) = delete;
    ProtocolMiscResponseAdapter& operator=(ProtocolMiscResponseAdapter const&) = delete;
    virtual ~ProtocolMiscResponseAdapter();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscResponseAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscVersionAdapter
 */
class ProtocolMiscVersionAdapter : public ProtocolMiscVersionAdapterInterface {
private:
    ProtocolMiscVersionAdapterInterface *protocolMiscVersionAdapterInterface = NULL;

public:
    ProtocolMiscVersionAdapter(const ModemData *pModemData);
    ProtocolMiscVersionAdapter(const ProtocolMiscVersionAdapter&) = delete;
    ProtocolMiscVersionAdapter& operator=(ProtocolMiscVersionAdapter const&) = delete;
    virtual ~ProtocolMiscVersionAdapter();
public:
    int GetMask() const;
    const char * GetSwVer() const;
    const char * GetHwVer() const;
    const char * GetRfCalDate() const;
    const char * GetProdCode() const;
    const char * GetModelID() const;
    int GetPrlNamNum() const;
    const BYTE * GetPrlVersion() const;
    int GetEriNamNum() const;
    const BYTE * GetEriVersion() const;
    const BYTE * GetCPChipSet() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscVersionAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetTtyAdapter
 */
class ProtocolMiscGetTtyAdapter : public ProtocolMiscGetTtyAdapterInterface {
private:
    ProtocolMiscGetTtyAdapterInterface *protocolMiscGetTtyAdapterInterface = NULL;

public:
    ProtocolMiscGetTtyAdapter(const ModemData *pModemData);
    ProtocolMiscGetTtyAdapter(const ProtocolMiscGetTtyAdapter&) = delete;
    ProtocolMiscGetTtyAdapter& operator=(ProtocolMiscGetTtyAdapter const&) = delete;
    virtual ~ProtocolMiscGetTtyAdapter();
public:
    int GetTtyMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetTtyAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolSignalStrengthAdapter
 */
class ProtocolSignalStrengthAdapter : public ProtocolSignalStrengthAdapterInterface {
private:
    ProtocolSignalStrengthAdapterInterface *protocolSignalStrengthAdapterInterface = NULL;

public:
    ProtocolSignalStrengthAdapter(const ModemData *pModemData);
    ProtocolSignalStrengthAdapter(const ProtocolSignalStrengthAdapter&) = delete;
    ProtocolSignalStrengthAdapter& operator=(ProtocolSignalStrengthAdapter const&) = delete;
    virtual ~ProtocolSignalStrengthAdapter();
public:
    RIL_SignalStrength_V1_6& GetSignalStrength(int ssVer);

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolSignalStrengthAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolSignalStrengthIndAdapter
 */
class ProtocolSignalStrengthIndAdapter : public ProtocolSignalStrengthIndAdapterInterface {
private:
    ProtocolSignalStrengthIndAdapterInterface *protocolSignalStrengthIndAdapterInterface = NULL;

public:
    ProtocolSignalStrengthIndAdapter(const ModemData *pModemData);
    ProtocolSignalStrengthIndAdapter(const ProtocolSignalStrengthIndAdapter&) = delete;
    ProtocolSignalStrengthIndAdapter& operator=(ProtocolSignalStrengthIndAdapter const&) = delete;
    virtual ~ProtocolSignalStrengthIndAdapter();
public:
    RIL_SignalStrength_V1_6& GetSignalStrength(int ssVer);

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolSignalStrengthIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolSignalStrengthIndAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscNITZTimeAdapter
 */
class ProtocolMiscNITZTimeAdapter : public ProtocolMiscNITZTimeAdapterInterface {
private:
    ProtocolMiscNITZTimeAdapterInterface *protocolMiscNITZTimeAdapterInterface = NULL;

public:
    ProtocolMiscNITZTimeAdapter(const ModemData *pModemData);
    ProtocolMiscNITZTimeAdapter(const ProtocolMiscNITZTimeAdapter &) = delete;
    ProtocolMiscNITZTimeAdapter& operator=(ProtocolMiscNITZTimeAdapter const&) = delete;
    virtual ~ProtocolMiscNITZTimeAdapter();
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
    const BYTE * GetPLMN() const;
    int64_t age() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscNITZTimeAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscNITZTimeAdapterInterface->GetParameterLength(); }
};


/**
 * ProtocolMiscGetNITZTimeAdapter
 */
class ProtocolMiscGetNITZTimeAdapter : public ProtocolMiscGetNITZTimeAdapterInterface {
private:
    ProtocolMiscGetNITZTimeAdapterInterface *mProxy = NULL;
public:
    ProtocolMiscGetNITZTimeAdapter(const ModemData *pModemData);
    ProtocolMiscGetNITZTimeAdapter(const ProtocolMiscGetNITZTimeAdapter &) = delete;
    ProtocolMiscGetNITZTimeAdapter& operator=(ProtocolMiscGetNITZTimeAdapter const&) = delete;
    ~ProtocolMiscGetNITZTimeAdapter();
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
    const BYTE * GetPLMN() const;
    int64_t age() const;

    UINT GetErrorCode() const { return mProxy->GetErrorCode(); }
    const char *GetParameter() const { return mProxy->GetParameter(); }
    UINT GetParameterLength() const { return mProxy->GetParameterLength(); }
};

/**
 * ProtocolMiscIMEIAdapter
 */
class ProtocolMiscIMEIAdapter : public ProtocolMiscIMEIAdapterInterface {
private:
    ProtocolMiscIMEIAdapterInterface *protocolMiscIMEIAdapterInterface = NULL;

public:
    ProtocolMiscIMEIAdapter(const ModemData *pModemData);
    ProtocolMiscIMEIAdapter(const ProtocolMiscIMEIAdapter&) = delete;
    ProtocolMiscIMEIAdapter& operator=(ProtocolMiscIMEIAdapter const&) = delete;
    virtual ~ProtocolMiscIMEIAdapter();
public:
    int GetIMEILen() const;
    const BYTE * GetIMEI() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscIMEIAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscIMEISVAdapter
 */
class ProtocolMiscIMEISVAdapter : public ProtocolMiscIMEISVAdapterInterface {
private:
    ProtocolMiscIMEISVAdapterInterface *protocolMiscIMEISVAdapterInterface = NULL;

public:
    ProtocolMiscIMEISVAdapter(const ModemData *pModemData);
    ProtocolMiscIMEISVAdapter(const ProtocolMiscIMEISVAdapter&) = delete;
    ProtocolMiscIMEISVAdapter& operator=(ProtocolMiscIMEISVAdapter const&) = delete;
    virtual ~ProtocolMiscIMEISVAdapter();
public:
    int GetIMEISVLen() const;
    const BYTE * GetIMEISV() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscIMEISVAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscDeviceIDAdapter
 */
class ProtocolMiscDeviceIDAdapter : public ProtocolMiscDeviceIDAdapterInterface {
private:
    ProtocolMiscDeviceIDAdapterInterface *protocolMiscDeviceIDAdapterInterface = NULL;

public:
    ProtocolMiscDeviceIDAdapter(const ModemData *pModemData);
    ProtocolMiscDeviceIDAdapter(const ProtocolMiscDeviceIDAdapter&) = delete;
    ProtocolMiscDeviceIDAdapter& operator=(ProtocolMiscDeviceIDAdapter const&) = delete;
    virtual ~ProtocolMiscDeviceIDAdapter();
public:
    int GetIMEILen() const;
    const BYTE * GetIMEI() const;
    int GetIMEISVLen() const;
    const BYTE * GetIMEISV() const;
    int GetMEIDLen() const;
    const BYTE * GetMEID() const;
    int GetESNLen() const;
    const BYTE * GetESN() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscDeviceIDAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscOemDisplayEngAdapter
 */
class ProtocolMiscOemDisplayEngAdapter : public ProtocolMiscOemDisplayEngAdapterInterface {
private:
    ProtocolMiscOemDisplayEngAdapterInterface *protocolMiscOemDisplayEngAdapterInterface = NULL;

public:
    ProtocolMiscOemDisplayEngAdapter(const ModemData *pModemData);
    ProtocolMiscOemDisplayEngAdapter(const ProtocolMiscOemDisplayEngAdapter&) = delete;
    ProtocolMiscOemDisplayEngAdapter& operator=(ProtocolMiscOemDisplayEngAdapter const&) = delete;
    virtual ~ProtocolMiscOemDisplayEngAdapter();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscOemDisplayEngAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscOemDisplayEngAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscPhoneResetAdapter
 */
class ProtocolMiscPhoneResetAdapter : public ProtocolMiscPhoneResetAdapterInterface {
private:
    ProtocolMiscPhoneResetAdapterInterface *protocolMiscPhoneResetAdapterInterface = NULL;

public:
    ProtocolMiscPhoneResetAdapter(const ModemData *pModemData);
    ProtocolMiscPhoneResetAdapter(const ProtocolMiscPhoneResetAdapter&) = delete;
    ProtocolMiscPhoneResetAdapter& operator=(ProtocolMiscPhoneResetAdapter const&) = delete;
    virtual ~ProtocolMiscPhoneResetAdapter();
public:
    BYTE GetResetType();
    BYTE GetResetCause();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscPhoneResetAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscPhoneResetAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscDataStateChangeAdapter
 */
class ProtocolMiscDataStateChangeAdapter : public ProtocolMiscDataStateChangeAdapterInterface {
private:
    ProtocolMiscDataStateChangeAdapterInterface *protocolMiscDataStateChangeAdapterInterface = NULL;

public:
    ProtocolMiscDataStateChangeAdapter(const ModemData *pModemData);
    ProtocolMiscDataStateChangeAdapter(const ProtocolMiscDataStateChangeAdapter&) = delete;
    ProtocolMiscDataStateChangeAdapter& operator=(ProtocolMiscDataStateChangeAdapter const&) = delete;
    virtual ~ProtocolMiscDataStateChangeAdapter();
public:
    BYTE GetExpectedState();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscDataStateChangeAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscDataStateChangeAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscGetActivityInfoAdapter
 */
class ProtocolMiscGetActivityInfoAdapter : public ProtocolMiscGetActivityInfoAdapterInterface {
private:
    ProtocolMiscGetActivityInfoAdapterInterface *protocolMiscGetActivityInfoAdapterInterface = NULL;

public:
    ProtocolMiscGetActivityInfoAdapter(const ModemData *pModemData);
    ProtocolMiscGetActivityInfoAdapter(const ProtocolMiscGetActivityInfoAdapter&) = delete;
    ProtocolMiscGetActivityInfoAdapter& operator=(ProtocolMiscGetActivityInfoAdapter const&) = delete;
    virtual ~ProtocolMiscGetActivityInfoAdapter();
public:
    UINT32 GetSleepPeriod() const;
    UINT32 GetIdlePeriod() const;
    UINT32* GetTxPeriod() const;
    UINT32 GetRxPeriod() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetActivityInfoAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetActivityInfoRatSpecificAdapter
 */
class ProtocolMiscGetActivityInfoRatSpecificAdapter :
        public ProtocolMiscGetActivityInfoRatSpecificAdapterInterface {
private:
    std::shared_ptr<ProtocolMiscGetActivityInfoRatSpecificAdapterInterface> mProxy;
public:
    ProtocolMiscGetActivityInfoRatSpecificAdapter(const ModemData *pModemData);
    const android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsInfo& info() const {
        return mProxy->info();
    }
    UINT GetErrorCode() const { return mProxy->GetErrorCode(); }
};

/**
 * ProtocolMiscGetMslCodeAdapter
 */
class ProtocolMiscGetMslCodeAdapter : public ProtocolMiscGetMslCodeAdapterInterface {
private:
    ProtocolMiscGetMslCodeAdapterInterface *protocolMiscGetMslCodeAdapterInterface = NULL;

public:
    ProtocolMiscGetMslCodeAdapter(const ModemData *pModemData);
    ProtocolMiscGetMslCodeAdapter(const ProtocolMiscGetMslCodeAdapter&) = delete;
    ProtocolMiscGetMslCodeAdapter& operator=(ProtocolMiscGetMslCodeAdapter const&) = delete;
    virtual ~ProtocolMiscGetMslCodeAdapter();
public:
    const char *getMslCode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetMslCodeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscPinControlAdapter
 */
class ProtocolMiscPinControlAdapter : public ProtocolMiscPinControlAdapterInterface {
private:
    ProtocolMiscPinControlAdapterInterface *protocolMiscPinControlAdapterInterface = NULL;

public:
    ProtocolMiscPinControlAdapter(const ModemData *pModemData);
    ProtocolMiscPinControlAdapter(const ProtocolMiscPinControlAdapter&) = delete;
    ProtocolMiscPinControlAdapter& operator=(ProtocolMiscPinControlAdapter const&) = delete;
    virtual ~ProtocolMiscPinControlAdapter();
public:
    BYTE GetSignal();
    BYTE GetStatus();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscPinControlAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscPinControlAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolGetVoiceOperationAdapter
 */
class ProtocolGetVoiceOperationAdapter : public ProtocolGetVoiceOperationAdapterInterface {
private:
    ProtocolGetVoiceOperationAdapterInterface *protocolGetVoiceOperationAdapterInterface = NULL;

public:
    ProtocolGetVoiceOperationAdapter(const ModemData *pModemData);
    ProtocolGetVoiceOperationAdapter(const ProtocolGetVoiceOperationAdapter&) = delete;
    ProtocolGetVoiceOperationAdapter& operator=(ProtocolGetVoiceOperationAdapter const&) = delete;
    virtual ~ProtocolGetVoiceOperationAdapter();
public:
    int GetMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolGetVoiceOperationAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetPreferredCallCapabilityAdapter
 */
class ProtocolMiscGetPreferredCallCapability : public ProtocolMiscGetPreferredCallCapabilityAdapterInterface {
private:
    ProtocolMiscGetPreferredCallCapabilityAdapterInterface *protocolMiscGetPreferredCallCapabilityAdapterInterface = NULL;

public:
    ProtocolMiscGetPreferredCallCapability(const ModemData *pModemData);
    ProtocolMiscGetPreferredCallCapability(const ProtocolMiscGetPreferredCallCapability&) = delete;
    ProtocolMiscGetPreferredCallCapability& operator=(ProtocolMiscGetPreferredCallCapability const&) = delete;
    virtual ~ProtocolMiscGetPreferredCallCapability();
public:
    int GetMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetPreferredCallCapabilityAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetManualBandModeAdapter
 */
class ProtocolMiscSetManualBandModeAdapter : public ProtocolMiscSetManualBandModeAdapterInterface {
private:
    ProtocolMiscSetManualBandModeAdapterInterface *protocolMiscSetManualBandModeAdapterInterface = NULL;

public:
    ProtocolMiscSetManualBandModeAdapter(const ModemData *pModemData);
    ProtocolMiscSetManualBandModeAdapter(const ProtocolMiscSetManualBandModeAdapter&) = delete;
    ProtocolMiscSetManualBandModeAdapter& operator=(ProtocolMiscSetManualBandModeAdapter const&) = delete;
    virtual ~ProtocolMiscSetManualBandModeAdapter();
public:
    BYTE GetCause();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetManualBandModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetRfDesenseModeAdapter
 */
class ProtocolMiscSetRfDesenseModeAdapter : public ProtocolMiscSetRfDesenseModeAdapterInterface {
private:
    ProtocolMiscSetRfDesenseModeAdapterInterface *protocolMiscSetRfDesenseModeAdapterInterface = NULL;

public:
    ProtocolMiscSetRfDesenseModeAdapter(const ModemData *pModemData);
    ProtocolMiscSetRfDesenseModeAdapter(const ProtocolMiscSetRfDesenseModeAdapter&) = delete;
    ProtocolMiscSetRfDesenseModeAdapter& operator=(ProtocolMiscSetRfDesenseModeAdapter const&) = delete;
    virtual ~ProtocolMiscSetRfDesenseModeAdapter();
public:
    BYTE GetCause();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetRfDesenseModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetHwConfigAdapter
 */
class ProtocolMiscGetHwConfigAdapter : public ProtocolMiscGetHwConfigAdapterInterface {
private:
    ProtocolMiscGetHwConfigAdapterInterface *protocolMiscGetHwConfigAdapterInterface = NULL;

public:
    ProtocolMiscGetHwConfigAdapter(const ModemData *pModemData);
    ProtocolMiscGetHwConfigAdapter(const ProtocolMiscGetHwConfigAdapter&) = delete;
    ProtocolMiscGetHwConfigAdapter& operator=(ProtocolMiscGetHwConfigAdapter const&) = delete;
    virtual ~ProtocolMiscGetHwConfigAdapter();
public:
    int GetNum() const;
    int GetData(RIL_HardwareConfig *pRsp, int num) const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetHwConfigAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscHwConfigChangeAdapter
 */
class ProtocolMiscHwConfigChangeAdapter : public ProtocolMiscHwConfigChangeAdapterInterface {
private:
    ProtocolMiscHwConfigChangeAdapterInterface *protocolMiscHwConfigChangeAdapterInterface = NULL;

public:
    ProtocolMiscHwConfigChangeAdapter(const ModemData *pModemData);
    ProtocolMiscHwConfigChangeAdapter(const ProtocolMiscHwConfigChangeAdapter &) = delete;
    ProtocolMiscHwConfigChangeAdapter& operator=(ProtocolMiscHwConfigChangeAdapter const&) = delete;
    virtual ~ProtocolMiscHwConfigChangeAdapter();
public:
    int GetNum() const;
    int GetData(RIL_HardwareConfig *pRsp, int num) const;

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscHwConfigChangeAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscHwConfigChangeAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscCdmaPrlChangeAdapter
 */
class ProtocolMiscCdmaPrlChangeAdapter : public ProtocolMiscCdmaPrlChangeAdapterInterface {
private:
    ProtocolMiscCdmaPrlChangeAdapterInterface *protocolMiscCdmaPrlChangeAdapterInterface = NULL;

public:
    ProtocolMiscCdmaPrlChangeAdapter(const ModemData *pModemData);
    ProtocolMiscCdmaPrlChangeAdapter(const ProtocolMiscCdmaPrlChangeAdapter&) = delete;
    ProtocolMiscCdmaPrlChangeAdapter& operator=(ProtocolMiscCdmaPrlChangeAdapter const&) = delete;
    virtual ~ProtocolMiscCdmaPrlChangeAdapter();
public:
    int GetPrlVersion() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscCdmaPrlChangeAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscCdmaPrlChangeAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscLceAdapter
 */
class ProtocolMiscLceAdapter : public ProtocolMiscLceAdapterInterface {
private:
    ProtocolMiscLceAdapterInterface *protocolMiscLceAdapterInterface = NULL;

public:
    ProtocolMiscLceAdapter(const ModemData *pModemData);
    ProtocolMiscLceAdapter(const ProtocolMiscLceAdapter&) = delete;
    ProtocolMiscLceAdapter& operator=(ProtocolMiscLceAdapter const&) = delete;
    virtual ~ProtocolMiscLceAdapter();

public:
    int getLceStatus() { return protocolMiscLceAdapterInterface->getLceStatus(); }
    unsigned int getActualIntervalMs() { return protocolMiscLceAdapterInterface->getActualIntervalMs(); }
    unsigned int getDlCapacityKbps () { return protocolMiscLceAdapterInterface->getDlCapacityKbps(); }
    unsigned int getUlCapacityKbps () { return protocolMiscLceAdapterInterface->getUlCapacityKbps(); }
    int getConfidencelevel () { return protocolMiscLceAdapterInterface->getConfidencelevel(); }
    int getLceSuspended() { return protocolMiscLceAdapterInterface->getLceSuspended(); }
    RIL_Errno GetRilErrorCode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscLceAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscLceIndAdapter
 */
class ProtocolMiscLceIndAdapter : public ProtocolMiscLceIndAdapterInterface {
private:
    ProtocolMiscLceIndAdapterInterface *protocolMiscLceIndAdapterInterface = NULL;

public:
    ProtocolMiscLceIndAdapter(const ModemData *pModemData);
    ProtocolMiscLceIndAdapter(const ProtocolMiscLceIndAdapter&) = delete;
    ProtocolMiscLceIndAdapter& operator=(ProtocolMiscLceIndAdapter const&) = delete;
    virtual ~ProtocolMiscLceIndAdapter();

public:
    int GetDLLc() const;
    int GetULLc() const;
    int GetConfLevel() const;
    int GetIsSuspended() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscLceIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscLceIndAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolCdmaSubscriptionAdapter
 */
class ProtocolCdmaSubscriptionAdapter : public ProtocolCdmaSubscriptionAdapterInterface {
private:
    ProtocolCdmaSubscriptionAdapterInterface *protocolCdmaSubscriptionAdapterInterface = NULL;

public:
    ProtocolCdmaSubscriptionAdapter(const ModemData *pModemData);
    ProtocolCdmaSubscriptionAdapter(const ProtocolCdmaSubscriptionAdapter &) = delete;
    ProtocolCdmaSubscriptionAdapter& operator=(ProtocolCdmaSubscriptionAdapter const&) = delete;
    virtual ~ProtocolCdmaSubscriptionAdapter();

public:
    char *GetMdn() const { return protocolCdmaSubscriptionAdapterInterface->GetMdn(); }
    char *GetMin() const { return (char *) protocolCdmaSubscriptionAdapterInterface->GetMin(); }
    WORD GetSid() const { return protocolCdmaSubscriptionAdapterInterface->GetSid(); }
    WORD GetNid() const { return protocolCdmaSubscriptionAdapterInterface->GetNid(); }
    UINT GetPrlVersion() const { return protocolCdmaSubscriptionAdapterInterface->GetPrlVersion(); }

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolCdmaSubscriptionAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSarControlStateAdapter
 */
class ProtocolMiscSarControlStateAdapter : public ProtocolMiscSarControlStateAdapterInterface {
private:
    ProtocolMiscSarControlStateAdapterInterface *protocolMiscSarControlStateAdapterInterface = NULL;
public:
    ProtocolMiscSarControlStateAdapter(const ModemData *pModemData);
    ProtocolMiscSarControlStateAdapter(const ProtocolMiscSarControlStateAdapter&) = delete;
    ProtocolMiscSarControlStateAdapter& operator=(ProtocolMiscSarControlStateAdapter const&) = delete;
    virtual ~ProtocolMiscSarControlStateAdapter();
public:
    BYTE GetDeviceState();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscSarControlStateAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscSarControlStateAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscSarRfConnectionAdapter
 */
class ProtocolMiscSarRfConnectionAdapter : public ProtocolMiscSarRfConnectionAdapterInterface {
private:
    ProtocolMiscSarRfConnectionAdapterInterface *protocolMiscSarRfConnectionAdapterInterface = NULL;
public:
    ProtocolMiscSarRfConnectionAdapter(const ModemData *pModemData);
    ProtocolMiscSarRfConnectionAdapter(const ProtocolMiscSarRfConnectionAdapter&) = delete;
    ProtocolMiscSarRfConnectionAdapter& operator=(ProtocolMiscSarRfConnectionAdapter const&) = delete;
    virtual ~ProtocolMiscSarRfConnectionAdapter();
public:
    BYTE GetRfState();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscSarRfConnectionAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscSarRfConnectionAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscGetSarStateAdapter
 */
class ProtocolMiscGetSarStateAdapter : public ProtocolMiscGetSarStateAdapterInterface {
private:
    ProtocolMiscGetSarStateAdapterInterface *protocolMiscGetSarStateAdapterInterface = NULL;
public:
    ProtocolMiscGetSarStateAdapter(const ModemData *pModemData);
    ProtocolMiscGetSarStateAdapter(const ProtocolMiscGetSarStateAdapter&) = delete;
    ProtocolMiscGetSarStateAdapter& operator=(ProtocolMiscGetSarStateAdapter const&) = delete;
    virtual ~ProtocolMiscGetSarStateAdapter();
public:
    int GetSarState() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetSarStateAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscRssiScanResultAdapter
 */
class ProtocolMiscRssiScanResultAdapter : public ProtocolMiscRssiScanResultAdapterInterface {
private:
    ProtocolMiscRssiScanResultAdapterInterface *protocolMiscRssiScanResultAdapterInterface = NULL;
public:
    ProtocolMiscRssiScanResultAdapter(const ModemData *pModemData);
    ProtocolMiscRssiScanResultAdapter(const ProtocolMiscRssiScanResultAdapter&) = delete;
    ProtocolMiscRssiScanResultAdapter& operator=(ProtocolMiscRssiScanResultAdapter const&) = delete;
    virtual ~ProtocolMiscRssiScanResultAdapter();
public:
    int GetTotalPage();
    int GetCurrentPage();
    int GetStartFrequency();
    int GetEndFrequency();
    int GetStep();
    int GetScanResultSize();
    INT16* GetScanResult();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscRssiScanResultAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscRssiScanResultAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscATCommandAdapter
 */
class ProtocolMiscATCommandAdapter : public ProtocolMiscATCommandAdapterInterface {
private:
    ProtocolMiscATCommandAdapterInterface *protocolMiscATCommandAdapterInterface = NULL;
public:
    ProtocolMiscATCommandAdapter(const ModemData *pModemData);
    ProtocolMiscATCommandAdapter(const ProtocolMiscATCommandAdapter&) = delete;
    ProtocolMiscATCommandAdapter& operator=(ProtocolMiscATCommandAdapter const&) = delete;
    virtual ~ProtocolMiscATCommandAdapter();
public:
    const char *GetCommand() const { return protocolMiscATCommandAdapterInterface->GetCommand(); }
    unsigned int GetCommandLength() const { return protocolMiscATCommandAdapterInterface->GetCommandLength(); }

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscATCommandAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscATCommandAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscGetRadioNodeAdapter
 */
class ProtocolMiscGetRadioNodeAdapter : public ProtocolMiscGetRadioNodeAdapterInterface {
private:
    ProtocolMiscGetRadioNodeAdapterInterface *protocolMiscGetRadioNodeAdapterInterface = NULL;
public:
    ProtocolMiscGetRadioNodeAdapter(const ModemData *pModemData);
    ProtocolMiscGetRadioNodeAdapter(const ProtocolMiscGetRadioNodeAdapter&) = delete;
    ProtocolMiscGetRadioNodeAdapter& operator=(ProtocolMiscGetRadioNodeAdapter const&) = delete;
    virtual ~ProtocolMiscGetRadioNodeAdapter();
public:
    const char *GetValue() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetRadioNodeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetVoLteProvisionUpdateAdapter
 */
class ProtocolMiscGetVoLteProvisionUpdateAdapter : public ProtocolMiscGetVoLteProvisionUpdateAdapterInterface {
private:
    ProtocolMiscGetVoLteProvisionUpdateAdapterInterface *protocolMiscGetVoLteProvisionUpdateAdapterInterface = NULL;
public:
    ProtocolMiscGetVoLteProvisionUpdateAdapter(const ModemData *pModemData);
    ProtocolMiscGetVoLteProvisionUpdateAdapter(const ProtocolMiscGetVoLteProvisionUpdateAdapter&) = delete;
    ProtocolMiscGetVoLteProvisionUpdateAdapter& operator=(ProtocolMiscGetVoLteProvisionUpdateAdapter const&) = delete;
    virtual ~ProtocolMiscGetVoLteProvisionUpdateAdapter();
public:
    int GetStatus();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetVoLteProvisionUpdateAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetVoLteProvisionUpdateAdapter
 */
class ProtocolMiscSetVoLteProvisionUpdateAdapter : public ProtocolMiscSetVoLteProvisionUpdateAdapterInterface {
private:
    ProtocolMiscSetVoLteProvisionUpdateAdapterInterface *protocolMiscSetVoLteProvisionUpdateAdapterInterface = NULL;
public:
    ProtocolMiscSetVoLteProvisionUpdateAdapter(const ModemData *pModemData);
    ProtocolMiscSetVoLteProvisionUpdateAdapter(const ProtocolMiscSetVoLteProvisionUpdateAdapter&) = delete;
    ProtocolMiscSetVoLteProvisionUpdateAdapter& operator=(ProtocolMiscSetVoLteProvisionUpdateAdapter const&) = delete;
    virtual ~ProtocolMiscSetVoLteProvisionUpdateAdapter();
public:
    int GetResult();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetVoLteProvisionUpdateAdapterInterface->GetErrorCode(); }
};

/*
 * ProtocolMiscGetStackStatusAdapter
 */
class ProtocolMiscGetStackStatusAdapter : public ProtocolMiscGetStackStatusAdapterInterface {
private:
    ProtocolMiscGetStackStatusAdapterInterface *protocolMiscGetStackStatusAdapterInterface = NULL;
public:
    ProtocolMiscGetStackStatusAdapter(const ModemData *pModemData);
    ProtocolMiscGetStackStatusAdapter(const ProtocolMiscGetStackStatusAdapter&) = delete;
    ProtocolMiscGetStackStatusAdapter& operator=(ProtocolMiscGetStackStatusAdapter const&) = delete;
    virtual ~ProtocolMiscGetStackStatusAdapter();
public:
    int GetMode();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetStackStatusAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolOemModemInfoAdapter
 */
class ProtocolOemModemInfoAdapter : public ProtocolOemModemInfoAdapterInterface {
private:
    ProtocolOemModemInfoAdapterInterface *protocolOemModemInfoAdapterInterface = NULL;
public:
    ProtocolOemModemInfoAdapter(const ModemData *pModemData);
    ProtocolOemModemInfoAdapter(const ProtocolOemModemInfoAdapter&) = delete;
    ProtocolOemModemInfoAdapter& operator=(ProtocolOemModemInfoAdapter const&) = delete;
    virtual ~ProtocolOemModemInfoAdapter();
public:
    int GetCommandType() const;
    unsigned int GetSize() const;
    void *GetData();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolOemModemInfoAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolOemModemInfoIndAdapter
 */
class ProtocolOemModemInfoIndAdapter : public ProtocolOemModemInfoIndAdapterInterface {
private:
    ProtocolOemModemInfoIndAdapterInterface *protocolOemModemInfoIndAdapterInterface = NULL;
public:
    ProtocolOemModemInfoIndAdapter(const ModemData *pModemData);
    ProtocolOemModemInfoIndAdapter(const ProtocolOemModemInfoIndAdapter&) = delete;
    ProtocolOemModemInfoIndAdapter& operator=(ProtocolOemModemInfoIndAdapter const&) = delete;
    virtual ~ProtocolOemModemInfoIndAdapter();
public:
    int GetCommandType() const;
    unsigned int GetSize() const;
    void *GetData();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolOemModemInfoIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolOemModemInfoIndAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolOemSwitchModemFunctionAdapter
 */
class ProtocolOemSwitchModemFunctionAdapter : public ProtocolOemSwitchModemFunctionAdapterInterface {
private:
    ProtocolOemSwitchModemFunctionAdapterInterface *protocolOemSwitchModemFunctionAdapterInterface = NULL;
public:
    ProtocolOemSwitchModemFunctionAdapter(const ModemData *pModemData);
    ProtocolOemSwitchModemFunctionAdapter(const ProtocolOemSwitchModemFunctionAdapter&) = delete;
    ProtocolOemSwitchModemFunctionAdapter& operator=(ProtocolOemSwitchModemFunctionAdapter const&) = delete;
    virtual ~ProtocolOemSwitchModemFunctionAdapter();
public:
    BYTE GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolOemSwitchModemFunctionAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetSelflogAdapter
 */
class ProtocolMiscSetSelflogAdapter : public ProtocolMiscSetSelflogAdapterInterface {
private:
    ProtocolMiscSetSelflogAdapterInterface *protocolMiscSetSelflogAdapterInterface = NULL;
public:
    ProtocolMiscSetSelflogAdapter(const ModemData *pModemData);
    ProtocolMiscSetSelflogAdapter(const ProtocolMiscSetSelflogAdapter&) = delete;
    ProtocolMiscSetSelflogAdapter& operator=(ProtocolMiscSetSelflogAdapter const&) = delete;
    virtual ~ProtocolMiscSetSelflogAdapter();
public:
    int GetSelflogResult();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetSelflogAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetSelflogStatusAdapter
 */
class ProtocolMiscGetSelflogStatusAdapter : public ProtocolMiscGetSelflogStatusAdapterInterface {
private:
    ProtocolMiscGetSelflogStatusAdapterInterface *protocolMiscGetSelflogStatusAdapterInterface = NULL;
public:
    ProtocolMiscGetSelflogStatusAdapter(const ModemData *pModemData);
    ProtocolMiscGetSelflogStatusAdapter(const ProtocolMiscGetSelflogStatusAdapter&) = delete;
    ProtocolMiscGetSelflogStatusAdapter& operator=(ProtocolMiscGetSelflogStatusAdapter const&) = delete;
    virtual ~ProtocolMiscGetSelflogStatusAdapter();
public:
    int GetSelflogStatus();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetSelflogStatusAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSelflogStatusAdapter
 */
class ProtocolMiscSelflogStatusAdapter : public ProtocolMiscSelflogStatusAdapterInterface {
private:
    ProtocolMiscSelflogStatusAdapterInterface *protocolMiscSelflogStatusAdapterInterface = NULL;
public:
    ProtocolMiscSelflogStatusAdapter(const ModemData *pModemData);
    ProtocolMiscSelflogStatusAdapter(const ProtocolMiscSelflogStatusAdapter&) = delete;
    ProtocolMiscSelflogStatusAdapter& operator=(ProtocolMiscSelflogStatusAdapter const&) = delete;
    virtual ~ProtocolMiscSelflogStatusAdapter();
public:
    BYTE GetIndSelflogStatus();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscSelflogStatusAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscSelflogStatusAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolOemGetCqiInfoAdapter
 */
class ProtocolOemGetCqiInfoAdapter : public ProtocolOemGetCqiInfoAdapterInterface {
private:
    ProtocolOemGetCqiInfoAdapterInterface *protocolOemGetCqiInfoAdapterInterface = NULL;
public:
    ProtocolOemGetCqiInfoAdapter(const ModemData *pModemData);
    ProtocolOemGetCqiInfoAdapter(const ProtocolOemGetCqiInfoAdapter&) = delete;
    ProtocolOemGetCqiInfoAdapter& operator=(ProtocolOemGetCqiInfoAdapter const&) = delete;
    virtual ~ProtocolOemGetCqiInfoAdapter();
public:
    INT16 GetCqiType() const;
    INT16 GetCqiInfo0() const;
    INT16 GetCqiInfo1() const;
    INT16 GetRi() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolOemGetCqiInfoAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetTcsFciAdapter
 */
class ProtocolMiscSetTcsFciAdapter : public ProtocolMiscSetTcsFciAdapterInterface {
private:
    ProtocolMiscSetTcsFciAdapterInterface *protocolMiscSetTcsFciAdapterInterface = NULL;
public:
    ProtocolMiscSetTcsFciAdapter(const ModemData *pModemData);
    ProtocolMiscSetTcsFciAdapter(const ProtocolMiscSetTcsFciAdapter&) = delete;
    ProtocolMiscSetTcsFciAdapter& operator=(ProtocolMiscSetTcsFciAdapter const&) = delete;
    virtual ~ProtocolMiscSetTcsFciAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetTcsFciAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetTcsFciAdapter
 */
class ProtocolMiscGetTcsFciAdapter : public ProtocolMiscGetTcsFciAdapterInterface {
private:
    ProtocolMiscGetTcsFciAdapterInterface *protocolMiscGetTcsFciAdapterInterface = NULL;
public:
    ProtocolMiscGetTcsFciAdapter(const ModemData *pModemData);
    ProtocolMiscGetTcsFciAdapter(const ProtocolMiscGetTcsFciAdapter&) = delete;
    ProtocolMiscGetTcsFciAdapter& operator=(ProtocolMiscGetTcsFciAdapter const&) = delete;
    virtual ~ProtocolMiscGetTcsFciAdapter();
public:
    const char * GetFci() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetTcsFciAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolCaBandwidthFilterIndAdapter
 */
class ProtocolCaBandwidthFilterIndAdapter : public ProtocolCaBandwidthFilterIndAdapterInterface {
private:
    ProtocolCaBandwidthFilterIndAdapterInterface *protocolCaBandwidthFilterIndAdapterInterface = NULL;
public:
    ProtocolCaBandwidthFilterIndAdapter(const ModemData *pModemData);
    ProtocolCaBandwidthFilterIndAdapter(const ProtocolCaBandwidthFilterIndAdapter&) = delete;
    ProtocolCaBandwidthFilterIndAdapter& operator=(ProtocolCaBandwidthFilterIndAdapter const&) = delete;
    virtual ~ProtocolCaBandwidthFilterIndAdapter();
public:
    int GetCaConfig() const;
    int GetNRB() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolCaBandwidthFilterIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolCaBandwidthFilterIndAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscSetModemLogDumpAdapter
 */
class ProtocolMiscSetModemLogDumpAdapter : public ProtocolMiscSetModemLogDumpAdapterInterface {
private:
    ProtocolMiscSetModemLogDumpAdapterInterface *protocolMiscSetModemLogDumpAdapterInterface = NULL;
public:
    ProtocolMiscSetModemLogDumpAdapter(const ModemData *pModemData);
    ProtocolMiscSetModemLogDumpAdapter(const ProtocolMiscSetModemLogDumpAdapter&) = delete;
    ProtocolMiscSetModemLogDumpAdapter& operator=(ProtocolMiscSetModemLogDumpAdapter const&) = delete;
    virtual ~ProtocolMiscSetModemLogDumpAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetModemLogDumpAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscCurrentLinkCapacityEstimateAdapter
 */
class ProtocolMiscCurrentLinkCapacityEstimate : public ProtocolMiscCurrentLinkCapacityEstimateAdapterInterface {
private:
    ProtocolMiscCurrentLinkCapacityEstimateAdapterInterface *protocolMiscCurrentLinkCapacityEstimateAdapterInterface = NULL;
public:
    ProtocolMiscCurrentLinkCapacityEstimate(const ModemData *pModemData);
    ProtocolMiscCurrentLinkCapacityEstimate(const ProtocolMiscCurrentLinkCapacityEstimate&) = delete;
    ProtocolMiscCurrentLinkCapacityEstimate& operator=(ProtocolMiscCurrentLinkCapacityEstimate const&) = delete;
    virtual ~ProtocolMiscCurrentLinkCapacityEstimate();
public:
    int GetDlCapaKbps() const;
    int GetUlCapaKbps() const;
    int GetSecDlCapaKbps() const;
    int GetSecUlCapaKbps() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscCurrentLinkCapacityEstimateAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscCurrentLinkCapacityEstimateAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscEndcCapabilityIndAdapter
 */
class ProtocolMiscEndcCapabilityIndAdapter : public ProtocolMiscEndcCapabilityIndAdapterInterface {
private:
    ProtocolMiscEndcCapabilityIndAdapterInterface *protocolMiscEndcCapabilityIndAdapterInterface = NULL;
public:
    ProtocolMiscEndcCapabilityIndAdapter(const ModemData *pModemData);
    ProtocolMiscEndcCapabilityIndAdapter(const ProtocolMiscEndcCapabilityIndAdapter&) = delete;
    ProtocolMiscEndcCapabilityIndAdapter& operator=(ProtocolMiscEndcCapabilityIndAdapter const&) = delete;
    virtual ~ProtocolMiscEndcCapabilityIndAdapter();
public:
    int GetCapability() const;
    int GetCause() const;

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolMiscEndcCapabilityIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscEndcCapabilityIndAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscSetSelflogProfileAdapter
 */
class ProtocolMiscSetSelflogProfileAdapter : public ProtocolMiscSetSelflogProfileAdapterInterface {
private:
    ProtocolMiscSetSelflogProfileAdapterInterface *protocolMiscSetSelflogProfileAdapterInterface = NULL;
public:
    ProtocolMiscSetSelflogProfileAdapter(const ModemData *pModemData);
    ProtocolMiscSetSelflogProfileAdapter(const ProtocolMiscSetSelflogProfileAdapter&) = delete;
    ProtocolMiscSetSelflogProfileAdapter& operator=(ProtocolMiscSetSelflogProfileAdapter const&) = delete;
    virtual ~ProtocolMiscSetSelflogProfileAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetSelflogProfileAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetForbidLteCellAdapter
 */
class ProtocolMiscSetForbidLteCellAdapter : public ProtocolMiscSetForbidLteCellAdapterInterface {
private:
    ProtocolMiscSetForbidLteCellAdapterInterface *protocolMiscSetForbidLteCellAdapterInterface = NULL;

public:
    ProtocolMiscSetForbidLteCellAdapter(const ModemData *pModemData);
    ProtocolMiscSetForbidLteCellAdapter(const ProtocolMiscSetForbidLteCellAdapter&) = delete;
    ProtocolMiscSetForbidLteCellAdapter& operator=(ProtocolMiscSetForbidLteCellAdapter const&) = delete;
    virtual ~ProtocolMiscSetForbidLteCellAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetForbidLteCellAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetApSystemTimeAdapter
 */
class ProtocolMiscSetApSystemTimeAdapter : public ProtocolMiscSetApSystemTimeAdapterInterface {
private:
    ProtocolMiscSetApSystemTimeAdapterInterface *protocolMiscSetApSystemTimeAdapterInterface = NULL;

public:
    ProtocolMiscSetApSystemTimeAdapter(const ModemData *pModemData);
    ProtocolMiscSetApSystemTimeAdapter(const ProtocolMiscSetApSystemTimeAdapter&) = delete;
    ProtocolMiscSetApSystemTimeAdapter& operator=(ProtocolMiscSetApSystemTimeAdapter const&) = delete;
    virtual ~ProtocolMiscSetApSystemTimeAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetApSystemTimeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetForbidFrequencyAdapter
 */
class ProtocolMiscSetForbidFrequencyAdapter : public ProtocolMiscSetForbidFrequencyAdapterInterface {
private:
    ProtocolMiscSetForbidFrequencyAdapterInterface *protocolMiscSetForbidFrequencyAdapterInterface = NULL;

public:
    ProtocolMiscSetForbidFrequencyAdapter(const ModemData *pModemData);
    ProtocolMiscSetForbidFrequencyAdapter(const ProtocolMiscSetForbidFrequencyAdapter&) = delete;
    ProtocolMiscSetForbidFrequencyAdapter& operator=(ProtocolMiscSetForbidFrequencyAdapter const&) = delete;
    virtual ~ProtocolMiscSetForbidFrequencyAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetForbidFrequencyAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetForbidLteBandAdapter
 */
class ProtocolMiscSetForbidLteBandAdapter : public ProtocolMiscSetForbidLteBandAdapterInterface {
private:
    ProtocolMiscSetForbidLteBandAdapterInterface *protocolMiscSetForbidLteBandAdapterInterface = NULL;

public:
    ProtocolMiscSetForbidLteBandAdapter(const ModemData *pModemData);
    ProtocolMiscSetForbidLteBandAdapter(const ProtocolMiscSetForbidLteBandAdapter&) = delete;
    ProtocolMiscSetForbidLteBandAdapter& operator=(ProtocolMiscSetForbidLteBandAdapter const&) = delete;
    virtual ~ProtocolMiscSetForbidLteBandAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetForbidLteBandAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetForbidLteTacAdapter
 */
class ProtocolMiscSetForbidLteTacAdapter : public ProtocolMiscSetForbidLteTacAdapterInterface {
private:
    ProtocolMiscSetForbidLteTacAdapterInterface *protocolMiscSetForbidLteTacAdapterInterface = NULL;

public:
    ProtocolMiscSetForbidLteTacAdapter(const ModemData *pModemData);
    ProtocolMiscSetForbidLteTacAdapter(const ProtocolMiscSetForbidLteTacAdapter&) = delete;
    ProtocolMiscSetForbidLteTacAdapter& operator=(ProtocolMiscSetForbidLteTacAdapter const&) = delete;
    virtual ~ProtocolMiscSetForbidLteTacAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetForbidLteTacAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetAheadRreAdapter
 */
class ProtocolMiscSetAheadRreAdapter : public ProtocolMiscSetAheadRreAdapterInterface {
private:
    ProtocolMiscSetAheadRreAdapterInterface *protocolMiscSetAheadRreAdapterInterface = NULL;

public:
    ProtocolMiscSetAheadRreAdapter(const ModemData *pModemData);
    ProtocolMiscSetAheadRreAdapter(const ProtocolMiscSetAheadRreAdapter&) = delete;
    ProtocolMiscSetAheadRreAdapter& operator=(ProtocolMiscSetAheadRreAdapter const&) = delete;
    virtual ~ProtocolMiscSetAheadRreAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetAheadRreAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetForbidNrCellAdapter
 */
class ProtocolMiscSetForbidNrCellAdapter : public ProtocolMiscSetForbidNrCellAdapterInterface {
private:
    ProtocolMiscSetForbidNrCellAdapterInterface *protocolMiscSetForbidNrCellAdapterInterface = NULL;

public:
    ProtocolMiscSetForbidNrCellAdapter(const ModemData *pModemData);
    ProtocolMiscSetForbidNrCellAdapter(const ProtocolMiscSetForbidNrCellAdapter&) = delete;
    ProtocolMiscSetForbidNrCellAdapter& operator=(ProtocolMiscSetForbidNrCellAdapter const&) = delete;
    virtual ~ProtocolMiscSetForbidNrCellAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetForbidNrCellAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetBinaryVersionInfoAdapter
 */
class ProtocolMiscGetBinaryVersionInfoAdapter : public ProtocolMiscGetBinaryVersionInfoAdapterInterface {
private:
    ProtocolMiscGetBinaryVersionInfoAdapterInterface *protocolMiscGetBinaryVersionInfoAdapterInterface = nullptr;

public:
    ProtocolMiscGetBinaryVersionInfoAdapter(const ModemData *pModemData);
    ProtocolMiscGetBinaryVersionInfoAdapter(const ProtocolMiscGetBinaryVersionInfoAdapter&) = delete;
    ProtocolMiscGetBinaryVersionInfoAdapter& operator=(ProtocolMiscGetBinaryVersionInfoAdapter const&) = delete;
    virtual ~ProtocolMiscGetBinaryVersionInfoAdapter();
public:
    int GetVersionInfoLength() const;
    const char * GetData() const;

    UINT GetErrorCode() const { return protocolMiscGetBinaryVersionInfoAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolRfConnectionStateAdapter
 */
class ProtocolRfConnectionStateAdapter : public ProtocolRfConnectionStateAdapterInterface {
private:
    ProtocolRfConnectionStateAdapterInterface *protocolRfConnectionStateAdapterInterface = NULL;

public:
    ProtocolRfConnectionStateAdapter(const ModemData *pModemData);
    ProtocolRfConnectionStateAdapter(const ProtocolRfConnectionStateAdapter&) = delete;
    ProtocolRfConnectionStateAdapter& operator=(ProtocolRfConnectionStateAdapter const&) = delete;
    virtual ~ProtocolRfConnectionStateAdapter();
public:
    BYTE GetState();

    //For ProtocolIndAdapter
    const char *GetParameter() const { return protocolRfConnectionStateAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolRfConnectionStateAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscGetCdmaCapabilityInfoAdapter
 */
class ProtocolMiscGetCdmaCapabilityInfoAdapter : public ProtocolMiscGetCdmaCapabilityInfoAdapterInterface {
private:
    ProtocolMiscGetCdmaCapabilityInfoAdapterInterface *protocolMiscGetCdmaCapabilityInfoAdapterInterface = NULL;

public:
    ProtocolMiscGetCdmaCapabilityInfoAdapter(const ModemData *pModemData);
    ProtocolMiscGetCdmaCapabilityInfoAdapter(const ProtocolMiscGetCdmaCapabilityInfoAdapter&) = delete;
    ProtocolMiscGetCdmaCapabilityInfoAdapter& operator=(ProtocolMiscGetCdmaCapabilityInfoAdapter const&) = delete;
    virtual ~ProtocolMiscGetCdmaCapabilityInfoAdapter();
public:
    BYTE GetCapability() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscGetCdmaCapabilityInfoAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscSetDeviceInfoAdapter
 */
class ProtocolMiscSetDeviceInfoAdapter : public ProtocolMiscSetDeviceInfoAdapterInterface {
private:
    ProtocolMiscSetDeviceInfoAdapterInterface *protocolMiscSetDeviceInfoAdapterInterface = NULL;

public:
    ProtocolMiscSetDeviceInfoAdapter(const ModemData *pModemData);
    ProtocolMiscSetDeviceInfoAdapter(const ProtocolMiscSetDeviceInfoAdapter&) = delete;
    ProtocolMiscSetDeviceInfoAdapter& operator=(ProtocolMiscSetDeviceInfoAdapter const&) = delete;
    virtual ~ProtocolMiscSetDeviceInfoAdapter();
public:
    int GetVersionInfo(const char *input);

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetDeviceInfoAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscBaseStationIndAdapter
 */
class ProtocolMiscBaseStationIndAdapter : public ProtocolMiscBaseStationIndAdapterInterface {
private:
    ProtocolMiscBaseStationIndAdapterInterface *protocolMiscBaseStationIndAdapterInterface = NULL;

public:
    ProtocolMiscBaseStationIndAdapter(const ModemData *pModemData);
    ProtocolMiscBaseStationIndAdapter(const ProtocolMiscBaseStationIndAdapter&) = delete;
    ProtocolMiscBaseStationIndAdapter& operator=(ProtocolMiscBaseStationIndAdapter const&) = delete;
    virtual ~ProtocolMiscBaseStationIndAdapter();
public:
    const char *GetParameter() const { return protocolMiscBaseStationIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscBaseStationIndAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscSetActiveVsimAdapter
 */
class ProtocolMiscSetActiveVsimAdapter : public ProtocolMiscSetActvieVsimAdapterInterface {
private:
    ProtocolMiscSetActvieVsimAdapterInterface *protocolMiscSetActvieVsimAdapterInterface = NULL;

public:
    ProtocolMiscSetActiveVsimAdapter(const ModemData *pModemData);
    ProtocolMiscSetActiveVsimAdapter(const ProtocolMiscSetActiveVsimAdapter&) = delete;
    ProtocolMiscSetActiveVsimAdapter& operator=(ProtocolMiscSetActiveVsimAdapter const&) = delete;
    virtual ~ProtocolMiscSetActiveVsimAdapter();
public:
    int GetResult() const { return protocolMiscSetActvieVsimAdapterInterface->GetResult(); }

    //For ProtocolRespAdapter
    UINT GetErrorCode() const { return protocolMiscSetActvieVsimAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetModemActivityInfoAdapter
 */
class ProtocolMiscGetModemActivityInfoAdapter : public ProtocolMiscGetModemActivityInfoAdapterInterface {
private:
    ProtocolMiscGetModemActivityInfoAdapterInterface *protocolMiscGetModemActivityInfoAdapterInterface = NULL;

public:
    ProtocolMiscGetModemActivityInfoAdapter(const ModemData *pModemData);
    ProtocolMiscGetModemActivityInfoAdapter(const ProtocolMiscGetModemActivityInfoAdapter&) = delete;
    ProtocolMiscGetModemActivityInfoAdapter& operator=(ProtocolMiscGetModemActivityInfoAdapter const&) = delete;
    virtual ~ProtocolMiscGetModemActivityInfoAdapter();
public:
    const char *GetParameter() const { return protocolMiscGetModemActivityInfoAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscGetModemActivityInfoAdapterInterface->GetParameterLength(); }
    UINT GetErrorCode() const { return protocolMiscGetModemActivityInfoAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscDropMtCallInfoIndAdapter
 */
class ProtocolMiscDropMtCallInfoIndAdapter : public ProtocolMiscDropMtCallInfoIndAdapterInterface {
private:
    ProtocolMiscDropMtCallInfoIndAdapterInterface *protocolMiscDropMtCallInfoIndAdapterInterface = NULL;

public:
    ProtocolMiscDropMtCallInfoIndAdapter(const ModemData *pModemData);
    ProtocolMiscDropMtCallInfoIndAdapter(const ProtocolMiscDropMtCallInfoIndAdapter&) = delete;
    ProtocolMiscDropMtCallInfoIndAdapter& operator=(ProtocolMiscDropMtCallInfoIndAdapter const&) = delete;
    virtual ~ProtocolMiscDropMtCallInfoIndAdapter();
public:
    const char *GetParameter() const { return protocolMiscDropMtCallInfoIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscDropMtCallInfoIndAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscCarrierConfigSimInfoIndAdapter
 */
class ProtocolMiscCarrierConfigSimInfoIndAdapter : public ProtocolMiscCarrierConfigSimInfoIndAdapterInterface {
private:
    ProtocolMiscCarrierConfigSimInfoIndAdapterInterface *protocolMiscCarrierConfigSimInfoIndAdapterInterface = NULL;

public:
    ProtocolMiscCarrierConfigSimInfoIndAdapter(const ModemData *pModemData);
    ProtocolMiscCarrierConfigSimInfoIndAdapter(const ProtocolMiscCarrierConfigSimInfoIndAdapter&) = delete;
    ProtocolMiscCarrierConfigSimInfoIndAdapter& operator=(ProtocolMiscCarrierConfigSimInfoIndAdapter const&) = delete;
    virtual ~ProtocolMiscCarrierConfigSimInfoIndAdapter();

public:
    const std::string &getMccMnc() { return protocolMiscCarrierConfigSimInfoIndAdapterInterface->getMccMnc(); }
    const std::string &getIccid() { return protocolMiscCarrierConfigSimInfoIndAdapterInterface->getIccid(); }
    const std::string &getImsi() { return protocolMiscCarrierConfigSimInfoIndAdapterInterface->getImsi(); }
    const std::string &getSpn() { return protocolMiscCarrierConfigSimInfoIndAdapterInterface->getSpn(); }
    const std::string &getPnn() { return protocolMiscCarrierConfigSimInfoIndAdapterInterface->getPnn(); }
    const std::string &getGid1() { return protocolMiscCarrierConfigSimInfoIndAdapterInterface->getGid1(); }
    const std::string &getGid2() { return protocolMiscCarrierConfigSimInfoIndAdapterInterface->getGid2(); }
    const std::string &getPrivilegeAccessRules() { return protocolMiscCarrierConfigSimInfoIndAdapterInterface->getPrivilegeAccessRules(); }
};

/**
 * ProtocolMiscCarrierConfigStatusIndAdapter
 */
class ProtocolMiscCarrierConfigStatusIndAdapter : public ProtocolMiscCarrierConfigStatusIndAdapterInterface {
private:
    ProtocolMiscCarrierConfigStatusIndAdapterInterface *protocolMiscCarrierConfigStatusIndAdapterInterface = NULL;

public:
    ProtocolMiscCarrierConfigStatusIndAdapter(const ModemData *pModemData);
    ProtocolMiscCarrierConfigStatusIndAdapter(const ProtocolMiscCarrierConfigStatusIndAdapter&) = delete;
    ProtocolMiscCarrierConfigStatusIndAdapter& operator=(ProtocolMiscCarrierConfigStatusIndAdapter const&) = delete;
    virtual ~ProtocolMiscCarrierConfigStatusIndAdapter();

public:
    int GetStatus() const { return protocolMiscCarrierConfigStatusIndAdapterInterface->GetStatus(); }
    int GetCause() const { return protocolMiscCarrierConfigStatusIndAdapterInterface->GetCause(); }
    const char *GetParameter() const { return protocolMiscCarrierConfigStatusIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolMiscCarrierConfigStatusIndAdapterInterface->GetParameterLength(); }
};

/**
 * ProtocolMiscGetTraceDumpCfgAdapter
 */
class ProtocolMiscGetTraceDumpCfgAdapter : public ProtocolMiscGetTraceDumpCfgAdapterInterface {
private:
    ProtocolMiscGetTraceDumpCfgAdapterInterface *protocolMiscGetTraceDumpCfgAdapterInterface = NULL;

public:
    ProtocolMiscGetTraceDumpCfgAdapter(const ModemData *pModemData);
    ProtocolMiscGetTraceDumpCfgAdapter(const ProtocolMiscGetTraceDumpCfgAdapter&) = delete;
    ProtocolMiscGetTraceDumpCfgAdapter& operator=(ProtocolMiscGetTraceDumpCfgAdapter const&) = delete;
    virtual ~ProtocolMiscGetTraceDumpCfgAdapter();
public:
    BYTE GetConfig() const;
    UINT GetErrorCode() const { return protocolMiscGetTraceDumpCfgAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetUeOperationModeAdapter
 */
class ProtocolMiscGetUeOperationModeAdapter : public ProtocolMiscGetUeOperationModeAdapterInterface {
private:
    ProtocolMiscGetUeOperationModeAdapterInterface *protocolMiscGetUeOperationModeAdapterInterface = NULL;

public:
    ProtocolMiscGetUeOperationModeAdapter(const ModemData *pModemData);
    ProtocolMiscGetUeOperationModeAdapter(const ProtocolMiscGetUeOperationModeAdapter&) = delete;
    ProtocolMiscGetUeOperationModeAdapter& operator=(ProtocolMiscGetUeOperationModeAdapter const&) = delete;
    virtual ~ProtocolMiscGetUeOperationModeAdapter();
public:
    BYTE GetMode() const;
    UINT GetErrorCode() const { return protocolMiscGetUeOperationModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetNrrrmMinReqEnableAdapter
 */
class ProtocolMiscGetNrrrmMinReqEnableAdapter : public ProtocolMiscGetNrrrmMinReqEnableAdapterInterface {
    private:
        ProtocolMiscGetNrrrmMinReqEnableAdapterInterface *protocolMiscGetNrrrmMinReqEnableAdapterInterface = NULL;

    public:
        ProtocolMiscGetNrrrmMinReqEnableAdapter(const ModemData *pModemData);
        ProtocolMiscGetNrrrmMinReqEnableAdapter(const ProtocolMiscGetNrrrmMinReqEnableAdapter&) = delete;
        ProtocolMiscGetNrrrmMinReqEnableAdapter& operator=(ProtocolMiscGetNrrrmMinReqEnableAdapter const&) = delete;
        virtual ~ProtocolMiscGetNrrrmMinReqEnableAdapter();
    public:
        BYTE GetEnable() const;
        UINT GetErrorCode() const { return protocolMiscGetNrrrmMinReqEnableAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolMiscGetVcrtAdapter
 */
class ProtocolMiscGetVcrtAdapter : public ProtocolMiscGetVcrtAdapterInterface {
private:
    ProtocolMiscGetVcrtAdapterInterface *protocolMiscGetVcrtAdapterInterface = nullptr;

public:
    ProtocolMiscGetVcrtAdapter(const ModemData *pModemData);
    ProtocolMiscGetVcrtAdapter(const ProtocolMiscGetVcrtAdapter&) = delete;
    ProtocolMiscGetVcrtAdapter& operator=(ProtocolMiscGetVcrtAdapter const&) = delete;
    virtual ~ProtocolMiscGetVcrtAdapter();
public:
    BYTE GetVcrtMode() const;

    UINT GetErrorCode() const { return protocolMiscGetVcrtAdapterInterface->GetErrorCode(); }
};

#endif /* __PROTOCOL_MISC_ADAPTER_H__ */
