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
 * protocolmiscadapterinterface.h
 *
 *  Created on: 2021. 06. 18.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_MISC_ADAPTER_INTERFACE_H__
#define __PROTOCOL_MISC_ADAPTER_INTERFACE_H__

#include <slsi/radio_v2_0.h>
#include <base/types.h>

/**
 * ProtocolMiscResponseAdapterInterface
 */
class ProtocolMiscResponseAdapterInterface {
public:
    ProtocolMiscResponseAdapterInterface() = default;
    virtual ~ProtocolMiscResponseAdapterInterface() = default;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscVersionAdapterInterface
 */
class ProtocolMiscVersionAdapterInterface {
public:
    ProtocolMiscVersionAdapterInterface() = default;
    virtual ~ProtocolMiscVersionAdapterInterface() = default;
public:
    virtual int GetMask() const = 0;
    virtual const char * GetSwVer() const = 0;
    virtual const char * GetHwVer() const = 0;
    virtual const char * GetRfCalDate() const = 0;
    virtual const char * GetProdCode() const = 0;
    virtual const char * GetModelID() const = 0;
    virtual int GetPrlNamNum() const = 0;
    virtual const BYTE * GetPrlVersion() const = 0;
    virtual int GetEriNamNum() const = 0;
    virtual const BYTE * GetEriVersion() const = 0;
    virtual const BYTE * GetCPChipSet() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetTtyAdapterInterface
 */
class ProtocolMiscGetTtyAdapterInterface {
public:
    ProtocolMiscGetTtyAdapterInterface() = default;
    virtual ~ProtocolMiscGetTtyAdapterInterface() = default;
public:
    virtual int GetTtyMode() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolSignalStrengthAdapterInterface
 */
class ProtocolSignalStrengthAdapterInterface {
public:
    ProtocolSignalStrengthAdapterInterface() = default;
    virtual ~ProtocolSignalStrengthAdapterInterface() = default;
public:
    virtual RIL_SignalStrength_V1_6& GetSignalStrength(int ssVer) = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolSignalStrengthIndAdapterInterface
 */
class ProtocolSignalStrengthIndAdapterInterface {
public:
    ProtocolSignalStrengthIndAdapterInterface() = default;
    virtual ~ProtocolSignalStrengthIndAdapterInterface() = default;
public:
    virtual RIL_SignalStrength_V1_6& GetSignalStrength(int ssVer) = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscNITZTimeAdapterInterface
 */
class ProtocolMiscNITZTimeAdapterInterface {
public:
    ProtocolMiscNITZTimeAdapterInterface() = default;
    virtual ~ProtocolMiscNITZTimeAdapterInterface() = default;
public:
    virtual int TimeInfoType() const = 0;
    virtual int DayLightValid() const = 0;
    virtual int Year() const = 0;
    virtual int Month() const = 0;
    virtual int Day() const = 0;
    virtual int Hour() const = 0;
    virtual int Minute() const = 0;
    virtual int Second() const = 0;
    virtual int TimeZone() const = 0;
    virtual int DayLightAdjust() const = 0;
    virtual int DayofWeek() const = 0;
    virtual int GetMMInfo() const = 0;
    virtual const BYTE * GetPLMN() const = 0;
    virtual int64_t age() const = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscGetNITZTimeAdapterInterface
 */
class ProtocolMiscGetNITZTimeAdapterInterface {
public:
    ProtocolMiscGetNITZTimeAdapterInterface() = default;
    virtual ~ProtocolMiscGetNITZTimeAdapterInterface() = default;
public:
    virtual int TimeInfoType() const = 0;
    virtual int DayLightValid() const = 0;
    virtual int Year() const = 0;
    virtual int Month() const = 0;
    virtual int Day() const = 0;
    virtual int Hour() const = 0;
    virtual int Minute() const = 0;
    virtual int Second() const = 0;
    virtual int TimeZone() const = 0;
    virtual int DayLightAdjust() const = 0;
    virtual int DayofWeek() const = 0;
    virtual int GetMMInfo() const = 0;
    virtual const BYTE * GetPLMN() const = 0;
    virtual int64_t age() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscIMEIAdapterInterface
 */
class ProtocolMiscIMEIAdapterInterface {
public:
    ProtocolMiscIMEIAdapterInterface() = default;
    virtual ~ProtocolMiscIMEIAdapterInterface() = default;
public:
    virtual int GetIMEILen() const = 0;
    virtual const BYTE * GetIMEI() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscIMEISVAdapterInterface
 */
class ProtocolMiscIMEISVAdapterInterface {
public:
    ProtocolMiscIMEISVAdapterInterface() = default;
    virtual ~ProtocolMiscIMEISVAdapterInterface() = default;
public:
    virtual int GetIMEISVLen() const = 0;
    virtual const BYTE * GetIMEISV() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscDeviceIDAdapterInterface
 */
class ProtocolMiscDeviceIDAdapterInterface {
public:
    ProtocolMiscDeviceIDAdapterInterface() = default;
    virtual ~ProtocolMiscDeviceIDAdapterInterface() = default;
public:
    virtual int GetIMEILen() const = 0;
    virtual const BYTE * GetIMEI() const = 0;
    virtual int GetIMEISVLen() const = 0;
    virtual const BYTE * GetIMEISV() const = 0;
    virtual int GetMEIDLen() const = 0;
    virtual const BYTE * GetMEID() const = 0;
    virtual int GetESNLen() const = 0;
    virtual const BYTE * GetESN() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscOemDisplayEngAdapterInterface
 */
class ProtocolMiscOemDisplayEngAdapterInterface {
public:
    ProtocolMiscOemDisplayEngAdapterInterface() = default;
    virtual ~ProtocolMiscOemDisplayEngAdapterInterface() = default;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscPhoneResetAdapterInterface
 */
class ProtocolMiscPhoneResetAdapterInterface {
public:
    ProtocolMiscPhoneResetAdapterInterface() = default;
    virtual ~ProtocolMiscPhoneResetAdapterInterface() = default;
public:
    virtual BYTE GetResetType() = 0;
    virtual BYTE GetResetCause() = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscDataStateChangeAdapterInterface
 */
class ProtocolMiscDataStateChangeAdapterInterface {
public:
    ProtocolMiscDataStateChangeAdapterInterface() = default;
    virtual ~ProtocolMiscDataStateChangeAdapterInterface() = default;
public:
    virtual BYTE GetExpectedState() = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscGetActivityInfoAdapterInterface
 */
class ProtocolMiscGetActivityInfoAdapterInterface {
public:
    ProtocolMiscGetActivityInfoAdapterInterface() = default;
    virtual ~ProtocolMiscGetActivityInfoAdapterInterface() = default;
public:
    virtual UINT32 GetSleepPeriod() const = 0;
    virtual UINT32 GetIdlePeriod() const = 0;
    virtual UINT32* GetTxPeriod() const = 0;
    virtual UINT32 GetRxPeriod() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetActivityInfoRatSpecificAdapterInterface
 */
class ProtocolMiscGetActivityInfoRatSpecificAdapterInterface {
public:
    virtual ~ProtocolMiscGetActivityInfoRatSpecificAdapterInterface()=default;
    virtual const android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsInfo& info() const=0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetMslCodeAdapterInterface
 */
class ProtocolMiscGetMslCodeAdapterInterface {
public:
    ProtocolMiscGetMslCodeAdapterInterface() = default;
    virtual ~ProtocolMiscGetMslCodeAdapterInterface() = default;
public:
    virtual const char *getMslCode() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscPinControlAdapterInterface
 */
class ProtocolMiscPinControlAdapterInterface {
public:
    ProtocolMiscPinControlAdapterInterface() = default;
    virtual ~ProtocolMiscPinControlAdapterInterface() = default;
public:
    virtual BYTE GetSignal() = 0;
    virtual BYTE GetStatus() = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolGetVoiceOperationAdapterInterface
 */
class ProtocolGetVoiceOperationAdapterInterface {
public:
    ProtocolGetVoiceOperationAdapterInterface() = default;
    virtual ~ProtocolGetVoiceOperationAdapterInterface() = default;
public:
    virtual int GetMode() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetPreferredCallCapabilityAdapterInterface
 */
class ProtocolMiscGetPreferredCallCapabilityAdapterInterface {
public:
    ProtocolMiscGetPreferredCallCapabilityAdapterInterface() = default;
    virtual ~ProtocolMiscGetPreferredCallCapabilityAdapterInterface() = default;
public:
    virtual int GetMode() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetManualBandModeAdapterInterface
 */
class ProtocolMiscSetManualBandModeAdapterInterface {
public:
    ProtocolMiscSetManualBandModeAdapterInterface() = default;
    virtual ~ProtocolMiscSetManualBandModeAdapterInterface() = default;
public:
    virtual BYTE GetCause() = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetRfDesenseModeAdapterInterface
 */
class ProtocolMiscSetRfDesenseModeAdapterInterface {
public:
    ProtocolMiscSetRfDesenseModeAdapterInterface() = default;
    virtual ~ProtocolMiscSetRfDesenseModeAdapterInterface() = default;
public:
    virtual BYTE GetCause() = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetHwConfigAdapterInterface
 */
class ProtocolMiscGetHwConfigAdapterInterface {
public:
    ProtocolMiscGetHwConfigAdapterInterface() = default;
    virtual ~ProtocolMiscGetHwConfigAdapterInterface() = default;
public:
    virtual int GetNum() const = 0;
    virtual int GetData(RIL_HardwareConfig *pRsp, int num) const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscHwConfigChangeAdapterInterface
 */
class ProtocolMiscHwConfigChangeAdapterInterface {
public:
    ProtocolMiscHwConfigChangeAdapterInterface() = default;
    virtual ~ProtocolMiscHwConfigChangeAdapterInterface() = default;
public:
    virtual int GetNum() const = 0;
    virtual int GetData(RIL_HardwareConfig *pRsp, int num) const = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscCdmaPrlChangeAdapterInterface
 */
class ProtocolMiscCdmaPrlChangeAdapterInterface {
public:
    ProtocolMiscCdmaPrlChangeAdapterInterface() = default;
    virtual ~ProtocolMiscCdmaPrlChangeAdapterInterface() = default;
public:
    virtual int GetPrlVersion() const = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscLceAdapterInterface
 */
class ProtocolMiscLceAdapterInterface {
public:
    ProtocolMiscLceAdapterInterface() = default;
    virtual ~ProtocolMiscLceAdapterInterface() = default;
public:
    virtual int getLceStatus() = 0;
    virtual unsigned int getActualIntervalMs() = 0;
    virtual unsigned int getDlCapacityKbps () = 0;
    virtual unsigned int getUlCapacityKbps () = 0;
    virtual int getConfidencelevel () = 0;
    virtual int getLceSuspended() = 0;
    virtual RIL_Errno GetRilErrorCode() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscLceIndAdapterInterface
 */
class ProtocolMiscLceIndAdapterInterface {
public:
    ProtocolMiscLceIndAdapterInterface() = default;
    virtual ~ProtocolMiscLceIndAdapterInterface() = default;
public:
    virtual int GetDLLc() const = 0;
    virtual int GetULLc() const = 0;
    virtual int GetConfLevel() const = 0;
    virtual int GetIsSuspended() const = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolCdmaSubscriptionAdapterInterface
 */
class ProtocolCdmaSubscriptionAdapterInterface {
public:
    ProtocolCdmaSubscriptionAdapterInterface() = default;
    virtual ~ProtocolCdmaSubscriptionAdapterInterface() = default;

public:
    virtual char *GetMdn() const = 0;
    virtual char *GetMin() const = 0;
    virtual WORD GetSid() const = 0;
    virtual WORD GetNid() const = 0;
    virtual UINT GetPrlVersion() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSarControlStateAdapterInterface
 */
class ProtocolMiscSarControlStateAdapterInterface {
public:
    ProtocolMiscSarControlStateAdapterInterface() = default;
    virtual ~ProtocolMiscSarControlStateAdapterInterface() = default;
public:
    virtual BYTE GetDeviceState() = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscSarRfConnectionAdapterInterface
 */
class ProtocolMiscSarRfConnectionAdapterInterface {
public:
    ProtocolMiscSarRfConnectionAdapterInterface() = default;
    virtual ~ProtocolMiscSarRfConnectionAdapterInterface() = default;
public:
    virtual BYTE GetRfState() = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscGetSarStateAdapterInterface
 */
class ProtocolMiscGetSarStateAdapterInterface {
public:
    ProtocolMiscGetSarStateAdapterInterface() = default;
    virtual ~ProtocolMiscGetSarStateAdapterInterface() = default;
public:
    virtual int GetSarState() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscRssiScanResultAdapterInterface
 */
class ProtocolMiscRssiScanResultAdapterInterface {
public:
    ProtocolMiscRssiScanResultAdapterInterface() = default;
    virtual ~ProtocolMiscRssiScanResultAdapterInterface() = default;
public:
    virtual int GetTotalPage() = 0;
    virtual int GetCurrentPage() = 0;
    virtual int GetStartFrequency() = 0;
    virtual int GetEndFrequency() = 0;
    virtual int GetStep() = 0;
    virtual int GetScanResultSize() = 0;
    virtual INT16* GetScanResult() = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscATCommandAdapterInterface
 */
class ProtocolMiscATCommandAdapterInterface {
public:
    ProtocolMiscATCommandAdapterInterface() = default;
    virtual ~ProtocolMiscATCommandAdapterInterface() = default;
public:
    virtual const char *GetCommand() const = 0;
    virtual unsigned int GetCommandLength() const = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscGetRadioNodeAdapterInterface
 */
class ProtocolMiscGetRadioNodeAdapterInterface {
public:
    ProtocolMiscGetRadioNodeAdapterInterface() = default;
    virtual ~ProtocolMiscGetRadioNodeAdapterInterface() = default;
public:
    virtual const char *GetValue() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetVoLteProvisionUpdateAdapterInterface
 */
class ProtocolMiscGetVoLteProvisionUpdateAdapterInterface {
public:
    ProtocolMiscGetVoLteProvisionUpdateAdapterInterface() = default;
    virtual ~ProtocolMiscGetVoLteProvisionUpdateAdapterInterface() = default;
public:
    virtual int GetStatus() = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetVoLteProvisionUpdateAdapterInterface
 */
class ProtocolMiscSetVoLteProvisionUpdateAdapterInterface {
public:
    ProtocolMiscSetVoLteProvisionUpdateAdapterInterface() = default;
    virtual ~ProtocolMiscSetVoLteProvisionUpdateAdapterInterface() = default;
public:
    virtual int GetResult() = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/*
 * ProtocolMiscGetStackStatusAdapterInterface
 */
class ProtocolMiscGetStackStatusAdapterInterface {
public:
    ProtocolMiscGetStackStatusAdapterInterface() = default;
    virtual ~ProtocolMiscGetStackStatusAdapterInterface() = default;
public:
    virtual int GetMode() = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolOemModemInfoAdapterInterface
 */
class ProtocolOemModemInfoAdapterInterface {
public:
    ProtocolOemModemInfoAdapterInterface() = default;
    virtual ~ProtocolOemModemInfoAdapterInterface() = default;
public:
    virtual int GetCommandType() const = 0;
    virtual unsigned int GetSize() const = 0;
    virtual void *GetData() = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolOemModemInfoIndAdapterInterface
 */
class ProtocolOemModemInfoIndAdapterInterface {
public:
    ProtocolOemModemInfoIndAdapterInterface() = default;
    virtual ~ProtocolOemModemInfoIndAdapterInterface() = default;
public:
    virtual int GetCommandType() const = 0;
    virtual unsigned int GetSize() const = 0;
    virtual void *GetData() = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolOemSwitchModemFunctionAdapterInterface
 */
class ProtocolOemSwitchModemFunctionAdapterInterface {
public:
    ProtocolOemSwitchModemFunctionAdapterInterface() = default;
    virtual ~ProtocolOemSwitchModemFunctionAdapterInterface() = default;
public:
    virtual BYTE GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetSelflogAdapterInterface
 */
class ProtocolMiscSetSelflogAdapterInterface {
public:
    ProtocolMiscSetSelflogAdapterInterface() = default;
    virtual ~ProtocolMiscSetSelflogAdapterInterface() = default;
public:
    virtual int GetSelflogResult() = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetSelflogStatusAdapterInterface
 */
class ProtocolMiscGetSelflogStatusAdapterInterface {
public:
    ProtocolMiscGetSelflogStatusAdapterInterface() = default;
    virtual ~ProtocolMiscGetSelflogStatusAdapterInterface() = default;
public:
    virtual int GetSelflogStatus() = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSelflogStatusAdapterInterface
 */
class ProtocolMiscSelflogStatusAdapterInterface {
public:
    ProtocolMiscSelflogStatusAdapterInterface() = default;
    virtual ~ProtocolMiscSelflogStatusAdapterInterface() = default;
public:
    virtual BYTE GetIndSelflogStatus() = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolOemGetCqiInfoAdapterInterface
 */
class ProtocolOemGetCqiInfoAdapterInterface {
public:
    ProtocolOemGetCqiInfoAdapterInterface() = default;
    virtual ~ProtocolOemGetCqiInfoAdapterInterface() = default;
public:
    virtual INT16 GetCqiType() const = 0;
    virtual INT16 GetCqiInfo0() const = 0;
    virtual INT16 GetCqiInfo1() const = 0;
    virtual INT16 GetRi() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetTcsFciAdapterInterface
 */
class ProtocolMiscSetTcsFciAdapterInterface {
public:
    ProtocolMiscSetTcsFciAdapterInterface() = default;
    virtual ~ProtocolMiscSetTcsFciAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetTcsFciAdapterInterface
 */
class ProtocolMiscGetTcsFciAdapterInterface {
public:
    ProtocolMiscGetTcsFciAdapterInterface() = default;
    virtual ~ProtocolMiscGetTcsFciAdapterInterface() = default;
public:
    virtual const char * GetFci() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolCaBandwidthFilterIndAdapterInterface
 */
class ProtocolCaBandwidthFilterIndAdapterInterface {
public:
    ProtocolCaBandwidthFilterIndAdapterInterface() = default;
    virtual ~ProtocolCaBandwidthFilterIndAdapterInterface() = default;
public:
    virtual int GetCaConfig() const = 0;
    virtual int GetNRB() const = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscSetModemLogDumpAdapterInterface
 */
class ProtocolMiscSetModemLogDumpAdapterInterface {
public:
    ProtocolMiscSetModemLogDumpAdapterInterface() = default;
    virtual ~ProtocolMiscSetModemLogDumpAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscCurrentLinkCapacityEstimateAdapterInterface
 */
class ProtocolMiscCurrentLinkCapacityEstimateAdapterInterface {
public:
    ProtocolMiscCurrentLinkCapacityEstimateAdapterInterface() = default;
    virtual ~ProtocolMiscCurrentLinkCapacityEstimateAdapterInterface() = default;
public:
    virtual int GetDlCapaKbps() const = 0;
    virtual int GetUlCapaKbps() const = 0;
    virtual int GetSecDlCapaKbps() const = 0;
    virtual int GetSecUlCapaKbps() const = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscEndcCapabilityIndAdapterInterface
 */
class ProtocolMiscEndcCapabilityIndAdapterInterface {
public:
    ProtocolMiscEndcCapabilityIndAdapterInterface() = default;
    virtual ~ProtocolMiscEndcCapabilityIndAdapterInterface() = default;
public:
    virtual int GetCapability() const = 0;
    virtual int GetCause() const = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscSetSelflogProfileAdapterInterface
 */
class ProtocolMiscSetSelflogProfileAdapterInterface {
public:
    ProtocolMiscSetSelflogProfileAdapterInterface() = default;
    virtual ~ProtocolMiscSetSelflogProfileAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetForbidLteCellAdapterInterface
 */
class ProtocolMiscSetForbidLteCellAdapterInterface {
public:
    ProtocolMiscSetForbidLteCellAdapterInterface() = default;
    virtual ~ProtocolMiscSetForbidLteCellAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetApSystemTimeAdapterInterface
 */
class ProtocolMiscSetApSystemTimeAdapterInterface {
public:
    ProtocolMiscSetApSystemTimeAdapterInterface() = default;
    virtual ~ProtocolMiscSetApSystemTimeAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetForbidFrequencyAdapterInterface
 */
class ProtocolMiscSetForbidFrequencyAdapterInterface {
public:
    ProtocolMiscSetForbidFrequencyAdapterInterface() = default;
    virtual ~ProtocolMiscSetForbidFrequencyAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetForbidLteBandAdapterInterface
 */
class ProtocolMiscSetForbidLteBandAdapterInterface {
public:
    ProtocolMiscSetForbidLteBandAdapterInterface() = default;
    virtual ~ProtocolMiscSetForbidLteBandAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetForbidLteTacAdapterInterface
 */
class ProtocolMiscSetForbidLteTacAdapterInterface {
public:
    ProtocolMiscSetForbidLteTacAdapterInterface() = default;
    virtual ~ProtocolMiscSetForbidLteTacAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetAheadRreAdapterInterface
 */
class ProtocolMiscSetAheadRreAdapterInterface {
public:
    ProtocolMiscSetAheadRreAdapterInterface() = default;
    virtual ~ProtocolMiscSetAheadRreAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetForbidNrCellAdapterInterface
 */
class ProtocolMiscSetForbidNrCellAdapterInterface {
public:
    ProtocolMiscSetForbidNrCellAdapterInterface() = default;
    virtual ~ProtocolMiscSetForbidNrCellAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetBinaryVersionInfoAdapterInterface
 */
class ProtocolMiscGetBinaryVersionInfoAdapterInterface {
public:
    ProtocolMiscGetBinaryVersionInfoAdapterInterface() = default;
    virtual ~ProtocolMiscGetBinaryVersionInfoAdapterInterface() = default;
public:
    virtual int GetVersionInfoLength() const = 0;
    virtual const char * GetData() const = 0;

    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolRfConnectionStateAdapterInterface
 */
class ProtocolRfConnectionStateAdapterInterface {
public:
    ProtocolRfConnectionStateAdapterInterface() = default;
    virtual ~ProtocolRfConnectionStateAdapterInterface() = default;
public:
    virtual BYTE GetState() = 0;

    //For ProtocolIndAdapter
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscGetCdmaCapabilityInfoAdapterInterface
 */
class ProtocolMiscGetCdmaCapabilityInfoAdapterInterface {
public:
    ProtocolMiscGetCdmaCapabilityInfoAdapterInterface() = default;
    virtual ~ProtocolMiscGetCdmaCapabilityInfoAdapterInterface() = default;
public:
    virtual BYTE GetCapability() const = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscSetDeviceInfoAdapterInterface
 */
class ProtocolMiscSetDeviceInfoAdapterInterface {
public:
    ProtocolMiscSetDeviceInfoAdapterInterface() = default;
    virtual ~ProtocolMiscSetDeviceInfoAdapterInterface() = default;
public:
    virtual int GetVersionInfo(const char *input) = 0;

    //For ProtocolRespAdapter
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscBaseStationIndAdapterInterface
 */
class ProtocolMiscBaseStationIndAdapterInterface {
public:
    ProtocolMiscBaseStationIndAdapterInterface() = default;
    virtual ~ProtocolMiscBaseStationIndAdapterInterface() = default;
public:
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscSetActvieVsimAdapterInterface
 */
class ProtocolMiscSetActvieVsimAdapterInterface {
public:
    ProtocolMiscSetActvieVsimAdapterInterface() = default;
    virtual ~ProtocolMiscSetActvieVsimAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetModemActivityInfoAdapterInterface
 */
class ProtocolMiscGetModemActivityInfoAdapterInterface {
public:
    ProtocolMiscGetModemActivityInfoAdapterInterface() = default;
    virtual ~ProtocolMiscGetModemActivityInfoAdapterInterface() = default;
public:
    virtual UINT GetErrorCode() const = 0;
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscDropMtCallInfoIndAdapterInterface
 */
class ProtocolMiscDropMtCallInfoIndAdapterInterface {
public:
    ProtocolMiscDropMtCallInfoIndAdapterInterface() = default;
    virtual ~ProtocolMiscDropMtCallInfoIndAdapterInterface() = default;
public:
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscCarrierConfigSimInfoIndAdapterInterface
 */
class ProtocolMiscCarrierConfigSimInfoIndAdapterInterface {
public:
    ProtocolMiscCarrierConfigSimInfoIndAdapterInterface() = default;
    virtual ~ProtocolMiscCarrierConfigSimInfoIndAdapterInterface() = default;
public:
    virtual const std::string &getMccMnc() = 0;
    virtual const std::string &getIccid() = 0;
    virtual const std::string &getImsi() = 0;
    virtual const std::string &getSpn() = 0;
    virtual const std::string &getPnn() = 0;
    virtual const std::string &getGid1() = 0;
    virtual const std::string &getGid2() = 0;
    virtual const std::string &getPrivilegeAccessRules() = 0;
};

/**
 * ProtocolMiscCarrierConfigStatusIndAdapterInterface
 */
class ProtocolMiscCarrierConfigStatusIndAdapterInterface {
public:
    ProtocolMiscCarrierConfigStatusIndAdapterInterface() = default;
    virtual ~ProtocolMiscCarrierConfigStatusIndAdapterInterface() = default;
public:
    virtual int GetStatus() const = 0;
    virtual int GetCause() const = 0;
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolMiscGetTraceDumpCfgAdapterInterface
 */
class ProtocolMiscGetTraceDumpCfgAdapterInterface {
public:
    ProtocolMiscGetTraceDumpCfgAdapterInterface() = default;
    virtual ~ProtocolMiscGetTraceDumpCfgAdapterInterface() = default;
public:
    virtual BYTE GetConfig() const = 0;
    virtual UINT GetErrorCode() const = 0;
};


/**
 * ProtocolMiscGetUeOperationModeAdapterInterface
 */
class ProtocolMiscGetUeOperationModeAdapterInterface {
public:
    ProtocolMiscGetUeOperationModeAdapterInterface() = default;
    virtual ~ProtocolMiscGetUeOperationModeAdapterInterface() = default;
public:
    virtual BYTE GetMode() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetNrrrmMinReqEnableAdapterInterface
 */
class ProtocolMiscGetNrrrmMinReqEnableAdapterInterface {
public:
    ProtocolMiscGetNrrrmMinReqEnableAdapterInterface() = default;
    virtual ~ProtocolMiscGetNrrrmMinReqEnableAdapterInterface() = default;
public:
    virtual BYTE GetEnable() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolMiscGetVcrtAdapterInterface
 */
class ProtocolMiscGetVcrtAdapterInterface {
public:
    ProtocolMiscGetVcrtAdapterInterface() = default;
    virtual ~ProtocolMiscGetVcrtAdapterInterface() = default;
public:
    virtual BYTE GetVcrtMode() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

#endif /* __PROTOCOL_MISC_ADAPTER_INTERFACE_H__ */
