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
 * protocolmiscadapter.cpp
 *
 *  Created on: 2021. 06. 21.
 *      Author: p1.bansal
 */
#include "protocolmiscadapter.h"
#include "legacy/misc/protocolmiscadapterlegacy.h"
#include "json/misc/protocolmiscadapterjson.h"
#include "modemdata.h"

/**
 * ProtocolMiscResponseAdapter
 */
ProtocolMiscResponseAdapter::ProtocolMiscResponseAdapter(const ModemData *pModemData)
    : ProtocolMiscResponseAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscResponseAdapterInterface = new ProtocolMiscResponseAdapterJson(pModemData);
    else
        protocolMiscResponseAdapterInterface = new ProtocolMiscResponseAdapterLegacy(pModemData);
}

ProtocolMiscResponseAdapter::~ProtocolMiscResponseAdapter() {
    delete protocolMiscResponseAdapterInterface;
    protocolMiscResponseAdapterInterface = NULL;
}

/**
 * ProtocolMiscVersionAdapter
 */
ProtocolMiscVersionAdapter::ProtocolMiscVersionAdapter(const ModemData *pModemData)
    : ProtocolMiscVersionAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscVersionAdapterInterface = new ProtocolMiscVersionAdapterJson(pModemData);
    else
        protocolMiscVersionAdapterInterface = new ProtocolMiscVersionAdapterLegacy(pModemData);
}

ProtocolMiscVersionAdapter::~ProtocolMiscVersionAdapter() {
    delete protocolMiscVersionAdapterInterface;
    protocolMiscVersionAdapterInterface = NULL;
}

int ProtocolMiscVersionAdapter::GetMask() const {
    return protocolMiscVersionAdapterInterface->GetMask();
}

const char *ProtocolMiscVersionAdapter::GetSwVer() const {
    return protocolMiscVersionAdapterInterface->GetSwVer();
}

const char *ProtocolMiscVersionAdapter::GetHwVer() const {
    return protocolMiscVersionAdapterInterface->GetHwVer();
}

const char *ProtocolMiscVersionAdapter::GetRfCalDate() const {
    return protocolMiscVersionAdapterInterface->GetRfCalDate();
}

const char *ProtocolMiscVersionAdapter::GetProdCode() const {
    return protocolMiscVersionAdapterInterface->GetProdCode();
}

const char *ProtocolMiscVersionAdapter::GetModelID() const {
    return protocolMiscVersionAdapterInterface->GetModelID();
}

int ProtocolMiscVersionAdapter::GetPrlNamNum() const {
    return protocolMiscVersionAdapterInterface->GetPrlNamNum();
}

const BYTE *ProtocolMiscVersionAdapter::GetPrlVersion() const {
    return protocolMiscVersionAdapterInterface->GetPrlVersion();
}

int ProtocolMiscVersionAdapter::GetEriNamNum() const {
    return protocolMiscVersionAdapterInterface->GetEriNamNum();
}

const BYTE *ProtocolMiscVersionAdapter::GetEriVersion() const {
    return protocolMiscVersionAdapterInterface->GetEriVersion();
}

const BYTE *ProtocolMiscVersionAdapter::GetCPChipSet() const {
    return protocolMiscVersionAdapterInterface->GetCPChipSet();
}

/**
 * ProtocolMiscGetTtyAdapter
 */
ProtocolMiscGetTtyAdapter::ProtocolMiscGetTtyAdapter(const ModemData *pModemData)
    : ProtocolMiscGetTtyAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscGetTtyAdapterInterface = new ProtocolMiscGetTtyAdapterJson(pModemData);
    else
        protocolMiscGetTtyAdapterInterface = new ProtocolMiscGetTtyAdapterLegacy(pModemData);
}

ProtocolMiscGetTtyAdapter::~ProtocolMiscGetTtyAdapter() {
    delete protocolMiscGetTtyAdapterInterface;
    protocolMiscGetTtyAdapterInterface = NULL;
}

int ProtocolMiscGetTtyAdapter::GetTtyMode() const {
    return protocolMiscGetTtyAdapterInterface->GetTtyMode();
}

/**
 * ProtocolSignalStrengthAdapter
 */
ProtocolSignalStrengthAdapter::ProtocolSignalStrengthAdapter(const ModemData *pModemData)
    : ProtocolSignalStrengthAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSignalStrengthAdapterInterface = new ProtocolSignalStrengthAdapterJson(pModemData);
    } else {
        protocolSignalStrengthAdapterInterface = new ProtocolSignalStrengthAdapterLegacy(pModemData);
    }
}

ProtocolSignalStrengthAdapter::~ProtocolSignalStrengthAdapter() {
    delete protocolSignalStrengthAdapterInterface;
    protocolSignalStrengthAdapterInterface = NULL;
}

RIL_SignalStrength_V1_6 &ProtocolSignalStrengthAdapter::GetSignalStrength(int ssVer) {
    return protocolSignalStrengthAdapterInterface->GetSignalStrength(ssVer);
}

/**
 * ProtocolSignalStrengthIndAdapter
 */
ProtocolSignalStrengthIndAdapter::ProtocolSignalStrengthIndAdapter(const ModemData *pModemData)
    : ProtocolSignalStrengthIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSignalStrengthIndAdapterInterface = new ProtocolSignalStrengthIndAdapterJson(pModemData);
    } else {
        protocolSignalStrengthIndAdapterInterface = new ProtocolSignalStrengthIndAdapterLegacy(pModemData);
    }
}

ProtocolSignalStrengthIndAdapter::~ProtocolSignalStrengthIndAdapter() {
    delete protocolSignalStrengthIndAdapterInterface;
    protocolSignalStrengthIndAdapterInterface = NULL;
}

RIL_SignalStrength_V1_6 &ProtocolSignalStrengthIndAdapter::GetSignalStrength(int ssVer) {
    return protocolSignalStrengthIndAdapterInterface->GetSignalStrength(ssVer);
}

/**
 * ProtocolMiscNITZTimeAdapter
 */
ProtocolMiscNITZTimeAdapter::ProtocolMiscNITZTimeAdapter(const ModemData *pModemData)
    : ProtocolMiscNITZTimeAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscNITZTimeAdapterInterface = new ProtocolMiscNITZTimeAdapterJson(pModemData);
    else
        protocolMiscNITZTimeAdapterInterface = new ProtocolMiscNITZTimeAdapterLegacy(pModemData);
}

ProtocolMiscNITZTimeAdapter::~ProtocolMiscNITZTimeAdapter() {
    delete protocolMiscNITZTimeAdapterInterface;
    protocolMiscNITZTimeAdapterInterface = NULL;
}

int ProtocolMiscNITZTimeAdapter::TimeInfoType() const {
    return protocolMiscNITZTimeAdapterInterface->TimeInfoType();
}

int ProtocolMiscNITZTimeAdapter::DayLightValid() const {
    return protocolMiscNITZTimeAdapterInterface->DayLightValid();
}

int ProtocolMiscNITZTimeAdapter::Year() const {
    return protocolMiscNITZTimeAdapterInterface->Year();
}

int ProtocolMiscNITZTimeAdapter::Month() const {
    return protocolMiscNITZTimeAdapterInterface->Month();
}

int ProtocolMiscNITZTimeAdapter::Day() const {
    return protocolMiscNITZTimeAdapterInterface->Day();
}

int ProtocolMiscNITZTimeAdapter::Hour() const {
    return protocolMiscNITZTimeAdapterInterface->Hour();
}

int ProtocolMiscNITZTimeAdapter::Minute() const {
    return protocolMiscNITZTimeAdapterInterface->Minute();
}

int ProtocolMiscNITZTimeAdapter::Second() const {
    return protocolMiscNITZTimeAdapterInterface->Second();
}

int ProtocolMiscNITZTimeAdapter::TimeZone() const {
    return protocolMiscNITZTimeAdapterInterface->TimeZone();
}
int ProtocolMiscNITZTimeAdapter::DayLightAdjust() const {
    return protocolMiscNITZTimeAdapterInterface->DayLightAdjust();
}

int ProtocolMiscNITZTimeAdapter::DayofWeek() const {
    return protocolMiscNITZTimeAdapterInterface->DayofWeek();
}

int ProtocolMiscNITZTimeAdapter::GetMMInfo() const {
    return protocolMiscNITZTimeAdapterInterface->GetMMInfo();
}

const BYTE *ProtocolMiscNITZTimeAdapter::GetPLMN() const {
    return protocolMiscNITZTimeAdapterInterface->GetPLMN();
}

int64_t ProtocolMiscNITZTimeAdapter::age() const {
    return protocolMiscNITZTimeAdapterInterface->age();
}

/**
 * ProtocolMiscGetNITZTimeAdapter
 */
ProtocolMiscGetNITZTimeAdapter::ProtocolMiscGetNITZTimeAdapter(const ModemData *pModemData)
    : ProtocolMiscGetNITZTimeAdapterInterface() {
    mProxy = new ProtocolMiscGetNITZTimeAdapterLegacy(pModemData);
}

ProtocolMiscGetNITZTimeAdapter::~ProtocolMiscGetNITZTimeAdapter() {
    delete mProxy;
}

int ProtocolMiscGetNITZTimeAdapter::TimeInfoType() const {
    return mProxy->TimeInfoType();
}

int ProtocolMiscGetNITZTimeAdapter::DayLightValid() const {
    return mProxy->DayLightValid();
}

int ProtocolMiscGetNITZTimeAdapter::Year() const {
    return mProxy->Year();
}

int ProtocolMiscGetNITZTimeAdapter::Month() const {
    return mProxy->Month();
}

int ProtocolMiscGetNITZTimeAdapter::Day() const {
    return mProxy->Day();
}

int ProtocolMiscGetNITZTimeAdapter::Hour() const {
    return mProxy->Hour();
}

int ProtocolMiscGetNITZTimeAdapter::Minute() const {
    return mProxy->Minute();
}

int ProtocolMiscGetNITZTimeAdapter::Second() const {
    return mProxy->Second();
}

int ProtocolMiscGetNITZTimeAdapter::TimeZone() const {
    return mProxy->TimeZone();
}
int ProtocolMiscGetNITZTimeAdapter::DayLightAdjust() const {
    return mProxy->DayLightAdjust();
}

int ProtocolMiscGetNITZTimeAdapter::DayofWeek() const {
    return mProxy->DayofWeek();
}

int ProtocolMiscGetNITZTimeAdapter::GetMMInfo() const {
    return mProxy->GetMMInfo();
}

const BYTE *ProtocolMiscGetNITZTimeAdapter::GetPLMN() const {
    return mProxy->GetPLMN();
}

int64_t ProtocolMiscGetNITZTimeAdapter::age() const {
    return mProxy->age();
}

/**
 * ProtocolMiscIMEIAdapter
 */
ProtocolMiscIMEIAdapter::ProtocolMiscIMEIAdapter(const ModemData *pModemData)
    : ProtocolMiscIMEIAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscIMEIAdapterInterface = new ProtocolMiscIMEIAdapterJson(pModemData);
    } else {
        protocolMiscIMEIAdapterInterface = new ProtocolMiscIMEIAdapterLegacy(pModemData);
    }
}

ProtocolMiscIMEIAdapter::~ProtocolMiscIMEIAdapter() {
    delete protocolMiscIMEIAdapterInterface;
    protocolMiscIMEIAdapterInterface = NULL;
}

int ProtocolMiscIMEIAdapter::GetIMEILen() const {
    return protocolMiscIMEIAdapterInterface->GetIMEILen();
}

const BYTE *ProtocolMiscIMEIAdapter::GetIMEI() const {
    return protocolMiscIMEIAdapterInterface->GetIMEI();
}

/**
 * ProtocolMiscIMEISVAdapter
 */
ProtocolMiscIMEISVAdapter::ProtocolMiscIMEISVAdapter(const ModemData *pModemData)
    : ProtocolMiscIMEISVAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscIMEISVAdapterInterface = new ProtocolMiscIMEISVAdapterJson(pModemData);
    } else {
        protocolMiscIMEISVAdapterInterface = new ProtocolMiscIMEISVAdapterLegacy(pModemData);
    }
}

ProtocolMiscIMEISVAdapter::~ProtocolMiscIMEISVAdapter() {
    delete protocolMiscIMEISVAdapterInterface;
    protocolMiscIMEISVAdapterInterface = NULL;
}

int ProtocolMiscIMEISVAdapter::GetIMEISVLen() const {
    return protocolMiscIMEISVAdapterInterface->GetIMEISVLen();
}

const BYTE *ProtocolMiscIMEISVAdapter::GetIMEISV() const {
    return protocolMiscIMEISVAdapterInterface->GetIMEISV();
}

/**
 * ProtocolMiscDeviceIDAdapter
 */
ProtocolMiscDeviceIDAdapter::ProtocolMiscDeviceIDAdapter(const ModemData *pModemData)
    : ProtocolMiscDeviceIDAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscDeviceIDAdapterInterface = new ProtocolMiscDeviceIDAdapterJson(pModemData);
    } else {
        protocolMiscDeviceIDAdapterInterface = new ProtocolMiscDeviceIDAdapterLegacy(pModemData);
    }
}

ProtocolMiscDeviceIDAdapter::~ProtocolMiscDeviceIDAdapter() {
    delete protocolMiscDeviceIDAdapterInterface;
    protocolMiscDeviceIDAdapterInterface = NULL;
}

int ProtocolMiscDeviceIDAdapter::GetIMEILen() const {
    return protocolMiscDeviceIDAdapterInterface->GetIMEILen();
}

const BYTE *ProtocolMiscDeviceIDAdapter::GetIMEI() const {
    return protocolMiscDeviceIDAdapterInterface->GetIMEI();
}

int ProtocolMiscDeviceIDAdapter::GetIMEISVLen() const {
    return protocolMiscDeviceIDAdapterInterface->GetIMEISVLen();
}

const BYTE *ProtocolMiscDeviceIDAdapter::GetIMEISV() const {
    return protocolMiscDeviceIDAdapterInterface->GetIMEISV();
}

int ProtocolMiscDeviceIDAdapter::GetMEIDLen() const {
    return protocolMiscDeviceIDAdapterInterface->GetMEIDLen();
}

const BYTE *ProtocolMiscDeviceIDAdapter::GetMEID() const {
    return protocolMiscDeviceIDAdapterInterface->GetMEID();
}

int ProtocolMiscDeviceIDAdapter::GetESNLen() const {
    return protocolMiscDeviceIDAdapterInterface->GetESNLen();
}

const BYTE *ProtocolMiscDeviceIDAdapter::GetESN() const {
    return protocolMiscDeviceIDAdapterInterface->GetESN();
}

/**
 * ProtocolMiscOemDisplayEngAdapter
 */
ProtocolMiscOemDisplayEngAdapter::ProtocolMiscOemDisplayEngAdapter(const ModemData *pModemData)
    : ProtocolMiscOemDisplayEngAdapterInterface() {
    protocolMiscOemDisplayEngAdapterInterface = new ProtocolMiscOemDisplayEngAdapterLegacy(pModemData);
}

ProtocolMiscOemDisplayEngAdapter::~ProtocolMiscOemDisplayEngAdapter() {
    delete protocolMiscOemDisplayEngAdapterInterface;
    protocolMiscOemDisplayEngAdapterInterface = NULL;
}

/**
 * ProtocolMiscPhoneResetAdapter
 */
ProtocolMiscPhoneResetAdapter::ProtocolMiscPhoneResetAdapter(const ModemData *pModemData)
    : ProtocolMiscPhoneResetAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscPhoneResetAdapterInterface = new ProtocolMiscPhoneResetAdapterJson(pModemData);
    } else {
        protocolMiscPhoneResetAdapterInterface = new ProtocolMiscPhoneResetAdapterLegacy(pModemData);
    }
}

ProtocolMiscPhoneResetAdapter::~ProtocolMiscPhoneResetAdapter() {
    delete protocolMiscPhoneResetAdapterInterface;
    protocolMiscPhoneResetAdapterInterface = NULL;
}

BYTE ProtocolMiscPhoneResetAdapter::GetResetType() {
    return protocolMiscPhoneResetAdapterInterface->GetResetType();
}

BYTE ProtocolMiscPhoneResetAdapter::GetResetCause() {
    return protocolMiscPhoneResetAdapterInterface->GetResetCause();
}

/**
 * ProtocolMiscDataStateChangeAdapter
 */
ProtocolMiscDataStateChangeAdapter::ProtocolMiscDataStateChangeAdapter(const ModemData *pModemData)
    : ProtocolMiscDataStateChangeAdapterInterface() {
    protocolMiscDataStateChangeAdapterInterface = new ProtocolMiscDataStateChangeAdapterLegacy(pModemData);
}

ProtocolMiscDataStateChangeAdapter::~ProtocolMiscDataStateChangeAdapter() {
    delete protocolMiscDataStateChangeAdapterInterface;
    protocolMiscDataStateChangeAdapterInterface = NULL;
}

BYTE ProtocolMiscDataStateChangeAdapter::GetExpectedState() {
    return protocolMiscDataStateChangeAdapterInterface->GetExpectedState();
}

/**
 * ProtocolMiscGetActivityInfoAdapter
 */
ProtocolMiscGetActivityInfoAdapter::ProtocolMiscGetActivityInfoAdapter(const ModemData *pModemData)
    : ProtocolMiscGetActivityInfoAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscGetActivityInfoAdapterInterface = new ProtocolMiscGetActivityInfoAdapterJson(pModemData);
    else
        protocolMiscGetActivityInfoAdapterInterface = new ProtocolMiscGetActivityInfoAdapterLegacy(pModemData);
}

ProtocolMiscGetActivityInfoAdapter::~ProtocolMiscGetActivityInfoAdapter() {
    delete protocolMiscGetActivityInfoAdapterInterface;
    protocolMiscGetActivityInfoAdapterInterface = NULL;
}

UINT32 ProtocolMiscGetActivityInfoAdapter::GetSleepPeriod() const {
    return protocolMiscGetActivityInfoAdapterInterface->GetSleepPeriod();
}

UINT32 ProtocolMiscGetActivityInfoAdapter::GetIdlePeriod() const {
    return protocolMiscGetActivityInfoAdapterInterface->GetIdlePeriod();
}

UINT32 *ProtocolMiscGetActivityInfoAdapter::GetTxPeriod() const {
    return protocolMiscGetActivityInfoAdapterInterface->GetTxPeriod();
}

UINT32 ProtocolMiscGetActivityInfoAdapter::GetRxPeriod() const {
    return protocolMiscGetActivityInfoAdapterInterface->GetRxPeriod();
}

/**
 * ProtocolMiscGetActivityInfoRatSpecificAdapter
 */
ProtocolMiscGetActivityInfoRatSpecificAdapter::ProtocolMiscGetActivityInfoRatSpecificAdapter(
        const ModemData *pModemData) {
    mProxy = std::make_shared<ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy>(pModemData);
}

/**
 * ProtocolMiscGetMslCodeAdapter
 */
ProtocolMiscGetMslCodeAdapter::ProtocolMiscGetMslCodeAdapter(const ModemData *pModemData)
    : ProtocolMiscGetMslCodeAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscGetMslCodeAdapterInterface = new ProtocolMiscGetMslCodeAdapterJson(pModemData);
    else
        protocolMiscGetMslCodeAdapterInterface = new ProtocolMiscGetMslCodeAdapterLegacy(pModemData);
}

ProtocolMiscGetMslCodeAdapter::~ProtocolMiscGetMslCodeAdapter() {
    delete protocolMiscGetMslCodeAdapterInterface;
    protocolMiscGetMslCodeAdapterInterface = NULL;
}

const char *ProtocolMiscGetMslCodeAdapter::getMslCode() const {
    return protocolMiscGetMslCodeAdapterInterface->getMslCode();
}

/**
 * ProtocolMiscPinControlAdapter
 */
ProtocolMiscPinControlAdapter::ProtocolMiscPinControlAdapter(const ModemData *pModemData)
    : ProtocolMiscPinControlAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscPinControlAdapterInterface = new ProtocolMiscPinControlAdapterJson(pModemData);
    else
        protocolMiscPinControlAdapterInterface = new ProtocolMiscPinControlAdapterLegacy(pModemData);
}

ProtocolMiscPinControlAdapter::~ProtocolMiscPinControlAdapter() {
    delete protocolMiscPinControlAdapterInterface;
    protocolMiscPinControlAdapterInterface = NULL;
}

BYTE ProtocolMiscPinControlAdapter::GetSignal() {
    return protocolMiscPinControlAdapterInterface->GetSignal();
}

BYTE ProtocolMiscPinControlAdapter::GetStatus() {
    return protocolMiscPinControlAdapterInterface->GetStatus();
}

/**
 * ProtocolGetVoiceOperationAdapter
 */
ProtocolGetVoiceOperationAdapter::ProtocolGetVoiceOperationAdapter(const ModemData *pModemData)
    : ProtocolGetVoiceOperationAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolGetVoiceOperationAdapterInterface = new ProtocolGetVoiceOperationAdapterJson(pModemData);
    else
        protocolGetVoiceOperationAdapterInterface = new ProtocolGetVoiceOperationAdapterLegacy(pModemData);
}

ProtocolGetVoiceOperationAdapter::~ProtocolGetVoiceOperationAdapter() {
    delete protocolGetVoiceOperationAdapterInterface;
    protocolGetVoiceOperationAdapterInterface = NULL;
}

int ProtocolGetVoiceOperationAdapter::GetMode() const {
    return protocolGetVoiceOperationAdapterInterface->GetMode();
}

/**
 * ProtocolMiscGetPreferredCallCapability
 */
ProtocolMiscGetPreferredCallCapability::ProtocolMiscGetPreferredCallCapability(const ModemData *pModemData)
    : ProtocolMiscGetPreferredCallCapabilityAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscGetPreferredCallCapabilityAdapterInterface = new ProtocolMiscGetPreferredCallCapabilityAdapterJson(pModemData);
    else
        protocolMiscGetPreferredCallCapabilityAdapterInterface = new ProtocolMiscGetPreferredCallCapabilityAdapterLegacy(pModemData);
}

ProtocolMiscGetPreferredCallCapability::~ProtocolMiscGetPreferredCallCapability() {
    delete protocolMiscGetPreferredCallCapabilityAdapterInterface;
    protocolMiscGetPreferredCallCapabilityAdapterInterface = NULL;
}

int ProtocolMiscGetPreferredCallCapability::GetMode() const {
    return protocolMiscGetPreferredCallCapabilityAdapterInterface->GetMode();
}

/**
 * ProtocolMiscSetManualBandModeAdapter
 */
ProtocolMiscSetManualBandModeAdapter::ProtocolMiscSetManualBandModeAdapter(const ModemData *pModemData)
    : ProtocolMiscSetManualBandModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscSetManualBandModeAdapterInterface = new ProtocolMiscSetManualBandModeAdapterJson(pModemData);
    } else {
        protocolMiscSetManualBandModeAdapterInterface = new ProtocolMiscSetManualBandModeAdapterLegacy(pModemData);
    }
}

ProtocolMiscSetManualBandModeAdapter::~ProtocolMiscSetManualBandModeAdapter() {
    delete protocolMiscSetManualBandModeAdapterInterface;
    protocolMiscSetManualBandModeAdapterInterface = NULL;
}

BYTE ProtocolMiscSetManualBandModeAdapter::GetCause() {
    return protocolMiscSetManualBandModeAdapterInterface->GetCause();
}

/**
 * ProtocolMiscSetRfDesenseModeAdapter
 */
ProtocolMiscSetRfDesenseModeAdapter::ProtocolMiscSetRfDesenseModeAdapter(const ModemData *pModemData)
    : ProtocolMiscSetRfDesenseModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscSetRfDesenseModeAdapterInterface = new ProtocolMiscSetRfDesenseModeAdapterJson(pModemData);
    } else {
        protocolMiscSetRfDesenseModeAdapterInterface = new ProtocolMiscSetRfDesenseModeAdapterLegacy(pModemData);
    }
}

ProtocolMiscSetRfDesenseModeAdapter::~ProtocolMiscSetRfDesenseModeAdapter() {
    delete protocolMiscSetRfDesenseModeAdapterInterface;
    protocolMiscSetRfDesenseModeAdapterInterface = NULL;
}

BYTE ProtocolMiscSetRfDesenseModeAdapter::GetCause() {
    return protocolMiscSetRfDesenseModeAdapterInterface->GetCause();
}

/**
 * ProtocolMiscGetHwConfigAdapter
 */
ProtocolMiscGetHwConfigAdapter::ProtocolMiscGetHwConfigAdapter(const ModemData *pModemData)
    : ProtocolMiscGetHwConfigAdapterInterface() {
    protocolMiscGetHwConfigAdapterInterface = new ProtocolMiscGetHwConfigAdapterLegacy(pModemData);
}

ProtocolMiscGetHwConfigAdapter::~ProtocolMiscGetHwConfigAdapter() {
    delete protocolMiscGetHwConfigAdapterInterface;
    protocolMiscGetHwConfigAdapterInterface = NULL;
}

int ProtocolMiscGetHwConfigAdapter::GetNum() const {
    return protocolMiscGetHwConfigAdapterInterface->GetNum();
}

int ProtocolMiscGetHwConfigAdapter::GetData(RIL_HardwareConfig *pRsp, int num) const {
    return protocolMiscGetHwConfigAdapterInterface->GetData(pRsp, num);
}

/**
 * ProtocolMiscHwConfigChangeAdapter
 */
ProtocolMiscHwConfigChangeAdapter::ProtocolMiscHwConfigChangeAdapter(const ModemData *pModemData)
    : ProtocolMiscHwConfigChangeAdapterInterface() {
    protocolMiscHwConfigChangeAdapterInterface = new ProtocolMiscHwConfigChangeAdapterLegacy(pModemData);
}

ProtocolMiscHwConfigChangeAdapter::~ProtocolMiscHwConfigChangeAdapter() {
    delete protocolMiscHwConfigChangeAdapterInterface;
    protocolMiscHwConfigChangeAdapterInterface = NULL;
}

int ProtocolMiscHwConfigChangeAdapter::GetNum() const {
    return protocolMiscHwConfigChangeAdapterInterface->GetNum();
}

int ProtocolMiscHwConfigChangeAdapter::GetData(RIL_HardwareConfig *pRsp, int num) const {
    return protocolMiscHwConfigChangeAdapterInterface->GetData(pRsp, num);
}

/**
 * ProtocolMiscCdmaPrlChangeAdapter
 */
ProtocolMiscCdmaPrlChangeAdapter::ProtocolMiscCdmaPrlChangeAdapter(const ModemData *pModemData)
    : ProtocolMiscCdmaPrlChangeAdapterInterface() {
    protocolMiscCdmaPrlChangeAdapterInterface = new ProtocolMiscCdmaPrlChangeAdapterLegacy(pModemData);
}

ProtocolMiscCdmaPrlChangeAdapter::~ProtocolMiscCdmaPrlChangeAdapter() {
    delete protocolMiscCdmaPrlChangeAdapterInterface;
    protocolMiscCdmaPrlChangeAdapterInterface = NULL;
}

int ProtocolMiscCdmaPrlChangeAdapter::GetPrlVersion() const {
    return protocolMiscCdmaPrlChangeAdapterInterface->GetPrlVersion();
}

/**
 * ProtocolMiscLceAdapter
 */
ProtocolMiscLceAdapter::ProtocolMiscLceAdapter(const ModemData *pModemData)
    : ProtocolMiscLceAdapterInterface() {
    protocolMiscLceAdapterInterface = new ProtocolMiscLceAdapterLegacy(pModemData);
}

ProtocolMiscLceAdapter::~ProtocolMiscLceAdapter() {
    delete protocolMiscLceAdapterInterface;
    protocolMiscLceAdapterInterface = NULL;
}

RIL_Errno ProtocolMiscLceAdapter::GetRilErrorCode() const {
    return protocolMiscLceAdapterInterface->GetRilErrorCode();
}

/**
 * ProtocolMiscLceIndAdapter
 */
ProtocolMiscLceIndAdapter::ProtocolMiscLceIndAdapter(const ModemData *pModemData)
    : ProtocolMiscLceIndAdapterInterface() {
    protocolMiscLceIndAdapterInterface = new ProtocolMiscLceIndAdapterLegacy(pModemData);
}

ProtocolMiscLceIndAdapter::~ProtocolMiscLceIndAdapter() {
    delete protocolMiscLceIndAdapterInterface;
    protocolMiscLceIndAdapterInterface = NULL;
}

int ProtocolMiscLceIndAdapter::GetDLLc() const {
    return protocolMiscLceIndAdapterInterface->GetDLLc();
}

int ProtocolMiscLceIndAdapter::GetULLc() const {
    return protocolMiscLceIndAdapterInterface->GetULLc();
}

int ProtocolMiscLceIndAdapter::GetConfLevel() const {
    return protocolMiscLceIndAdapterInterface->GetConfLevel();
}

int ProtocolMiscLceIndAdapter::GetIsSuspended() const {
    return protocolMiscLceIndAdapterInterface->GetIsSuspended();
}

/**
 * ProtocolCdmaSubscriptionAdapter
 */
ProtocolCdmaSubscriptionAdapter::ProtocolCdmaSubscriptionAdapter(const ModemData *pModemData)
    : ProtocolCdmaSubscriptionAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolCdmaSubscriptionAdapterInterface = new ProtocolCdmaSubscriptionAdapterJson(pModemData);
    else
        protocolCdmaSubscriptionAdapterInterface = new ProtocolCdmaSubscriptionAdapterLegacy(pModemData);
}

ProtocolCdmaSubscriptionAdapter::~ProtocolCdmaSubscriptionAdapter() {
    delete protocolCdmaSubscriptionAdapterInterface;
    protocolCdmaSubscriptionAdapterInterface = NULL;
}

/**
 * ProtocolMiscSarControlStateAdapter
 */
ProtocolMiscSarControlStateAdapter::ProtocolMiscSarControlStateAdapter(const ModemData *pModemData)
    : ProtocolMiscSarControlStateAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscSarControlStateAdapterInterface = new ProtocolMiscSarControlStateAdapterJson(pModemData);
    else
        protocolMiscSarControlStateAdapterInterface = new ProtocolMiscSarControlStateAdapterLegacy(pModemData);
}

ProtocolMiscSarControlStateAdapter::~ProtocolMiscSarControlStateAdapter() {
    delete protocolMiscSarControlStateAdapterInterface;
    protocolMiscSarControlStateAdapterInterface = NULL;
}

BYTE ProtocolMiscSarControlStateAdapter::GetDeviceState() {
    return protocolMiscSarControlStateAdapterInterface->GetDeviceState();
}

/**
 * ProtocolMiscSarRfConnectionAdapter
 */
ProtocolMiscSarRfConnectionAdapter::ProtocolMiscSarRfConnectionAdapter(const ModemData *pModemData)
    : ProtocolMiscSarRfConnectionAdapterInterface() {
    protocolMiscSarRfConnectionAdapterInterface = new ProtocolMiscSarRfConnectionAdapterLegacy(pModemData);
}

ProtocolMiscSarRfConnectionAdapter::~ProtocolMiscSarRfConnectionAdapter() {
    delete protocolMiscSarRfConnectionAdapterInterface;
    protocolMiscSarRfConnectionAdapterInterface = NULL;
}

BYTE ProtocolMiscSarRfConnectionAdapter::GetRfState() {
    return protocolMiscSarRfConnectionAdapterInterface->GetRfState();
}

/**
 * ProtocolMiscGetSarStateAdapter
 */
ProtocolMiscGetSarStateAdapter::ProtocolMiscGetSarStateAdapter(const ModemData *pModemData)
    : ProtocolMiscGetSarStateAdapterInterface() {
    protocolMiscGetSarStateAdapterInterface = new ProtocolMiscGetSarStateAdapterLegacy(pModemData);
}

ProtocolMiscGetSarStateAdapter::~ProtocolMiscGetSarStateAdapter() {
    delete protocolMiscGetSarStateAdapterInterface;
    protocolMiscGetSarStateAdapterInterface = NULL;
}

int ProtocolMiscGetSarStateAdapter::GetSarState() const {
    return protocolMiscGetSarStateAdapterInterface->GetSarState();
}

/**
 * ProtocolMiscRssiScanResultAdapter
 */
ProtocolMiscRssiScanResultAdapter::ProtocolMiscRssiScanResultAdapter(const ModemData *pModemData)
    : ProtocolMiscRssiScanResultAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscRssiScanResultAdapterInterface = new ProtocolMiscRssiScanResultAdapterJson(pModemData);
    else
        protocolMiscRssiScanResultAdapterInterface = new ProtocolMiscRssiScanResultAdapterLegacy(pModemData);
}

ProtocolMiscRssiScanResultAdapter::~ProtocolMiscRssiScanResultAdapter() {
    delete protocolMiscRssiScanResultAdapterInterface;
    protocolMiscRssiScanResultAdapterInterface = NULL;
}

int ProtocolMiscRssiScanResultAdapter::GetTotalPage() {
    return protocolMiscRssiScanResultAdapterInterface->GetTotalPage();
}

int ProtocolMiscRssiScanResultAdapter::GetCurrentPage() {
    return protocolMiscRssiScanResultAdapterInterface->GetCurrentPage();
}

int ProtocolMiscRssiScanResultAdapter::GetStartFrequency() {
    return protocolMiscRssiScanResultAdapterInterface->GetStartFrequency();
}

int ProtocolMiscRssiScanResultAdapter::GetEndFrequency() {
    return protocolMiscRssiScanResultAdapterInterface->GetEndFrequency();
}

int ProtocolMiscRssiScanResultAdapter::GetStep() {
    return protocolMiscRssiScanResultAdapterInterface->GetStep();
}

int ProtocolMiscRssiScanResultAdapter::GetScanResultSize() {
    return protocolMiscRssiScanResultAdapterInterface->GetScanResultSize();
}

INT16 *ProtocolMiscRssiScanResultAdapter::GetScanResult() {
    return protocolMiscRssiScanResultAdapterInterface->GetScanResult();
}

/**
 * ProtocolMiscATCommandAdapter
 */
ProtocolMiscATCommandAdapter::ProtocolMiscATCommandAdapter(const ModemData *pModemData)
    : ProtocolMiscATCommandAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscATCommandAdapterInterface = new ProtocolMiscATCommandAdapterJson(pModemData);
    else
        protocolMiscATCommandAdapterInterface = new ProtocolMiscATCommandAdapterLegacy(pModemData);
}

ProtocolMiscATCommandAdapter::~ProtocolMiscATCommandAdapter() {
    delete protocolMiscATCommandAdapterInterface;
    protocolMiscATCommandAdapterInterface = NULL;
}

/**
 * ProtocolMiscGetRadioNodeAdapter
 */
ProtocolMiscGetRadioNodeAdapter::ProtocolMiscGetRadioNodeAdapter(const ModemData *pModemData)
    : ProtocolMiscGetRadioNodeAdapterInterface() {
    protocolMiscGetRadioNodeAdapterInterface = new ProtocolMiscGetRadioNodeAdapterLegacy(pModemData);
}

ProtocolMiscGetRadioNodeAdapter::~ProtocolMiscGetRadioNodeAdapter() {
    delete protocolMiscGetRadioNodeAdapterInterface;
    protocolMiscGetRadioNodeAdapterInterface = NULL;
}

const char *ProtocolMiscGetRadioNodeAdapter::GetValue() const {
    return protocolMiscGetRadioNodeAdapterInterface->GetValue();
}

/**
 * ProtocolMiscGetVoLteProvisionUpdateAdapter
 */
ProtocolMiscGetVoLteProvisionUpdateAdapter::ProtocolMiscGetVoLteProvisionUpdateAdapter(const ModemData *pModemData)
    : ProtocolMiscGetVoLteProvisionUpdateAdapterInterface() {
    protocolMiscGetVoLteProvisionUpdateAdapterInterface = new ProtocolMiscGetVoLteProvisionUpdateAdapterLegacy(pModemData);
}

ProtocolMiscGetVoLteProvisionUpdateAdapter::~ProtocolMiscGetVoLteProvisionUpdateAdapter() {
    delete protocolMiscGetVoLteProvisionUpdateAdapterInterface;
    protocolMiscGetVoLteProvisionUpdateAdapterInterface = NULL;
}

int ProtocolMiscGetVoLteProvisionUpdateAdapter::GetStatus() {
    return protocolMiscGetVoLteProvisionUpdateAdapterInterface->GetStatus();
}

/**
 * ProtocolMiscSetVoLteProvisionUpdateAdapter
 */
ProtocolMiscSetVoLteProvisionUpdateAdapter::ProtocolMiscSetVoLteProvisionUpdateAdapter(const ModemData *pModemData)
    : ProtocolMiscSetVoLteProvisionUpdateAdapterInterface() {
    protocolMiscSetVoLteProvisionUpdateAdapterInterface = new ProtocolMiscSetVoLteProvisionUpdateAdapterLegacy(pModemData);
}

ProtocolMiscSetVoLteProvisionUpdateAdapter::~ProtocolMiscSetVoLteProvisionUpdateAdapter() {
    delete protocolMiscSetVoLteProvisionUpdateAdapterInterface;
    protocolMiscSetVoLteProvisionUpdateAdapterInterface = NULL;
}

int ProtocolMiscSetVoLteProvisionUpdateAdapter::GetResult() {
    return protocolMiscSetVoLteProvisionUpdateAdapterInterface->GetResult();
}

/*
 * ProtocolMiscGetStackStatusAdapter
 */
ProtocolMiscGetStackStatusAdapter::ProtocolMiscGetStackStatusAdapter(const ModemData *pModemData)
    : ProtocolMiscGetStackStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscGetStackStatusAdapterInterface = new ProtocolMiscGetStackStatusAdapterJson(pModemData);
    } else {
        protocolMiscGetStackStatusAdapterInterface = new ProtocolMiscGetStackStatusAdapterLegacy(pModemData);
    }
}

ProtocolMiscGetStackStatusAdapter::~ProtocolMiscGetStackStatusAdapter() {
    delete protocolMiscGetStackStatusAdapterInterface;
    protocolMiscGetStackStatusAdapterInterface = NULL;
}

int ProtocolMiscGetStackStatusAdapter::GetMode() {
    return protocolMiscGetStackStatusAdapterInterface->GetMode();
}

/**
 * ProtocolOemModemInfoAdapter
 */
ProtocolOemModemInfoAdapter::ProtocolOemModemInfoAdapter(const ModemData *pModemData)
    : ProtocolOemModemInfoAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolOemModemInfoAdapterInterface = new ProtocolOemModemInfoAdapterJson(pModemData);
    } else {
        protocolOemModemInfoAdapterInterface = new ProtocolOemModemInfoAdapterLegacy(pModemData);
    }
}

ProtocolOemModemInfoAdapter::~ProtocolOemModemInfoAdapter() {
    delete protocolOemModemInfoAdapterInterface;
    protocolOemModemInfoAdapterInterface = NULL;
}

int ProtocolOemModemInfoAdapter::GetCommandType() const {
    return protocolOemModemInfoAdapterInterface->GetCommandType();
}

unsigned int ProtocolOemModemInfoAdapter::GetSize() const {
    return protocolOemModemInfoAdapterInterface->GetSize();
}

void *ProtocolOemModemInfoAdapter::GetData() {
    return protocolOemModemInfoAdapterInterface->GetData();
}

/**
 * ProtocolOemModemInfoIndAdapter
 */
ProtocolOemModemInfoIndAdapter::ProtocolOemModemInfoIndAdapter(const ModemData *pModemData)
    : ProtocolOemModemInfoIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolOemModemInfoIndAdapterInterface = new ProtocolOemModemInfoIndAdapterJson(pModemData);
    } else {
        protocolOemModemInfoIndAdapterInterface = new ProtocolOemModemInfoIndAdapterLegacy(pModemData);
    }
}

ProtocolOemModemInfoIndAdapter::~ProtocolOemModemInfoIndAdapter() {
    delete protocolOemModemInfoIndAdapterInterface;
    protocolOemModemInfoIndAdapterInterface = NULL;
}

int ProtocolOemModemInfoIndAdapter::GetCommandType() const {
    return protocolOemModemInfoIndAdapterInterface->GetCommandType();
}

unsigned int ProtocolOemModemInfoIndAdapter::GetSize() const {
    return protocolOemModemInfoIndAdapterInterface->GetSize();
}

void *ProtocolOemModemInfoIndAdapter::GetData() {
    return protocolOemModemInfoIndAdapterInterface->GetData();
}

/**
 * ProtocolOemSwitchModemFunctionAdapter
 */
ProtocolOemSwitchModemFunctionAdapter::ProtocolOemSwitchModemFunctionAdapter(const ModemData *pModemData)
    : ProtocolOemSwitchModemFunctionAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolOemSwitchModemFunctionAdapterInterface = new ProtocolOemSwitchModemFunctionAdapterJson(pModemData);
    } else {
        protocolOemSwitchModemFunctionAdapterInterface = new ProtocolOemSwitchModemFunctionAdapterLegacy(pModemData);
    }
}

ProtocolOemSwitchModemFunctionAdapter::~ProtocolOemSwitchModemFunctionAdapter() {
    delete protocolOemSwitchModemFunctionAdapterInterface;
    protocolOemSwitchModemFunctionAdapterInterface = NULL;
}

BYTE ProtocolOemSwitchModemFunctionAdapter::GetResult() const {
    return protocolOemSwitchModemFunctionAdapterInterface->GetResult();
}

/**
 * ProtocolMiscSetSelflogAdapter
 */
ProtocolMiscSetSelflogAdapter::ProtocolMiscSetSelflogAdapter(const ModemData *pModemData)
    : ProtocolMiscSetSelflogAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscSetSelflogAdapterInterface = new ProtocolMiscSetSelflogAdapterJson(pModemData);
    else
        protocolMiscSetSelflogAdapterInterface = new ProtocolMiscSetSelflogAdapterLegacy(pModemData);
}

ProtocolMiscSetSelflogAdapter::~ProtocolMiscSetSelflogAdapter() {
    delete protocolMiscSetSelflogAdapterInterface;
    protocolMiscSetSelflogAdapterInterface = NULL;
}

int ProtocolMiscSetSelflogAdapter::GetSelflogResult() {
    return protocolMiscSetSelflogAdapterInterface->GetSelflogResult();
}

/**
 * ProtocolMiscGetSelflogStatusAdapter
 */
ProtocolMiscGetSelflogStatusAdapter::ProtocolMiscGetSelflogStatusAdapter(const ModemData *pModemData)
    : ProtocolMiscGetSelflogStatusAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscGetSelflogStatusAdapterInterface = new ProtocolMiscGetSelflogStatusAdapterJson(pModemData);
    else
        protocolMiscGetSelflogStatusAdapterInterface = new ProtocolMiscGetSelflogStatusAdapterLegacy(pModemData);
}

ProtocolMiscGetSelflogStatusAdapter::~ProtocolMiscGetSelflogStatusAdapter() {
    delete protocolMiscGetSelflogStatusAdapterInterface;
    protocolMiscGetSelflogStatusAdapterInterface = NULL;
}

int ProtocolMiscGetSelflogStatusAdapter::GetSelflogStatus() {
    return protocolMiscGetSelflogStatusAdapterInterface->GetSelflogStatus();
}

/**
 * ProtocolMiscSelflogStatusAdapter
 */
ProtocolMiscSelflogStatusAdapter::ProtocolMiscSelflogStatusAdapter(const ModemData *pModemData)
    : ProtocolMiscSelflogStatusAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscSelflogStatusAdapterInterface = new ProtocolMiscSelflogStatusAdapterJson(pModemData);
    else
        protocolMiscSelflogStatusAdapterInterface = new ProtocolMiscSelflogStatusAdapterLegacy(pModemData);
}

ProtocolMiscSelflogStatusAdapter::~ProtocolMiscSelflogStatusAdapter() {
    delete protocolMiscSelflogStatusAdapterInterface;
    protocolMiscSelflogStatusAdapterInterface = NULL;
}

BYTE ProtocolMiscSelflogStatusAdapter::GetIndSelflogStatus() {
    return protocolMiscSelflogStatusAdapterInterface->GetIndSelflogStatus();
}

/**
 * ProtocolOemGetCqiInfoAdapter
 */
ProtocolOemGetCqiInfoAdapter::ProtocolOemGetCqiInfoAdapter(const ModemData *pModemData)
    : ProtocolOemGetCqiInfoAdapterInterface() {
    protocolOemGetCqiInfoAdapterInterface = new ProtocolOemGetCqiInfoAdapterLegacy(pModemData);
}

ProtocolOemGetCqiInfoAdapter::~ProtocolOemGetCqiInfoAdapter() {
    delete protocolOemGetCqiInfoAdapterInterface;
    protocolOemGetCqiInfoAdapterInterface = NULL;
}

INT16 ProtocolOemGetCqiInfoAdapter::GetCqiType() const {
    return protocolOemGetCqiInfoAdapterInterface->GetCqiType();
}
INT16 ProtocolOemGetCqiInfoAdapter::GetCqiInfo0() const {
    return protocolOemGetCqiInfoAdapterInterface->GetCqiInfo0();
}
INT16 ProtocolOemGetCqiInfoAdapter::GetCqiInfo1() const {
    return protocolOemGetCqiInfoAdapterInterface->GetCqiInfo1();
}
INT16 ProtocolOemGetCqiInfoAdapter::GetRi() const {
    return protocolOemGetCqiInfoAdapterInterface->GetRi();
}

/**
 * ProtocolMiscSetTcsFciAdapter
 */
ProtocolMiscSetTcsFciAdapter::ProtocolMiscSetTcsFciAdapter(const ModemData *pModemData)
    : ProtocolMiscSetTcsFciAdapterInterface() {
    protocolMiscSetTcsFciAdapterInterface = new ProtocolMiscSetTcsFciAdapterLegacy(pModemData);
}

ProtocolMiscSetTcsFciAdapter::~ProtocolMiscSetTcsFciAdapter() {
    delete protocolMiscSetTcsFciAdapterInterface;
    protocolMiscSetTcsFciAdapterInterface = NULL;
}

int ProtocolMiscSetTcsFciAdapter::GetResult() const {
    return protocolMiscSetTcsFciAdapterInterface->GetResult();
}

/**
 * ProtocolMiscGetTcsFciAdapter
 */
ProtocolMiscGetTcsFciAdapter::ProtocolMiscGetTcsFciAdapter(const ModemData *pModemData)
    : ProtocolMiscGetTcsFciAdapterInterface() {
    protocolMiscGetTcsFciAdapterInterface = new ProtocolMiscGetTcsFciAdapterLegacy(pModemData);
}

ProtocolMiscGetTcsFciAdapter::~ProtocolMiscGetTcsFciAdapter() {
    delete protocolMiscGetTcsFciAdapterInterface;
    protocolMiscGetTcsFciAdapterInterface = NULL;
}

const char *ProtocolMiscGetTcsFciAdapter::GetFci() const {
    return protocolMiscGetTcsFciAdapterInterface->GetFci();
}

/**
 * ProtocolCaBandwidthFilterIndAdapter
 */
ProtocolCaBandwidthFilterIndAdapter::ProtocolCaBandwidthFilterIndAdapter(const ModemData *pModemData)
    : ProtocolCaBandwidthFilterIndAdapterInterface() {
    protocolCaBandwidthFilterIndAdapterInterface = new ProtocolCaBandwidthFilterIndAdapterLegacy(pModemData);
}

ProtocolCaBandwidthFilterIndAdapter::~ProtocolCaBandwidthFilterIndAdapter() {
    delete protocolCaBandwidthFilterIndAdapterInterface;
    protocolCaBandwidthFilterIndAdapterInterface = NULL;
}

int ProtocolCaBandwidthFilterIndAdapter::GetCaConfig() const {
    return protocolCaBandwidthFilterIndAdapterInterface->GetCaConfig();
}

int ProtocolCaBandwidthFilterIndAdapter::GetNRB() const {
    return protocolCaBandwidthFilterIndAdapterInterface->GetNRB();
}

/**
 * ProtocolMiscSetModemLogDumpAdapter
 */
ProtocolMiscSetModemLogDumpAdapter::ProtocolMiscSetModemLogDumpAdapter(const ModemData *pModemData)
    : ProtocolMiscSetModemLogDumpAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscSetModemLogDumpAdapterInterface = new ProtocolMiscSetModemLogDumpAdapterJson(pModemData);
    else
        protocolMiscSetModemLogDumpAdapterInterface = new ProtocolMiscSetModemLogDumpAdapterLegacy(pModemData);
}

ProtocolMiscSetModemLogDumpAdapter::~ProtocolMiscSetModemLogDumpAdapter() {
    delete protocolMiscSetModemLogDumpAdapterInterface;
    protocolMiscSetModemLogDumpAdapterInterface = NULL;
}

int ProtocolMiscSetModemLogDumpAdapter::GetResult() const {
    return protocolMiscSetModemLogDumpAdapterInterface->GetResult();
}

/*
 * ProtocolMiscCurrentLinkCapacityEstimate
 */
ProtocolMiscCurrentLinkCapacityEstimate::ProtocolMiscCurrentLinkCapacityEstimate(const ModemData *pModemData)
    : ProtocolMiscCurrentLinkCapacityEstimateAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscCurrentLinkCapacityEstimateAdapterInterface = new ProtocolMiscCurrentLinkCapacityEstimateAdapterJson(pModemData);
    else
        protocolMiscCurrentLinkCapacityEstimateAdapterInterface = new ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy(pModemData);
}

ProtocolMiscCurrentLinkCapacityEstimate::~ProtocolMiscCurrentLinkCapacityEstimate() {
    delete protocolMiscCurrentLinkCapacityEstimateAdapterInterface;
    protocolMiscCurrentLinkCapacityEstimateAdapterInterface = NULL;
}

int ProtocolMiscCurrentLinkCapacityEstimate::GetDlCapaKbps() const {
    return protocolMiscCurrentLinkCapacityEstimateAdapterInterface->GetDlCapaKbps();
}

int ProtocolMiscCurrentLinkCapacityEstimate::GetUlCapaKbps() const {
    return protocolMiscCurrentLinkCapacityEstimateAdapterInterface->GetUlCapaKbps();
}

int ProtocolMiscCurrentLinkCapacityEstimate::GetSecDlCapaKbps() const {
    return protocolMiscCurrentLinkCapacityEstimateAdapterInterface->GetSecDlCapaKbps();
}

int ProtocolMiscCurrentLinkCapacityEstimate::GetSecUlCapaKbps() const {
    return protocolMiscCurrentLinkCapacityEstimateAdapterInterface->GetSecUlCapaKbps();
}

/*
 * ProtocolMiscEndcCapabilityIndAdapter
 */
ProtocolMiscEndcCapabilityIndAdapter::ProtocolMiscEndcCapabilityIndAdapter(const ModemData *pModemData)
    : ProtocolMiscEndcCapabilityIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscEndcCapabilityIndAdapterInterface = new ProtocolMiscEndcCapabilityIndAdapterJson(pModemData);
    } else {
        protocolMiscEndcCapabilityIndAdapterInterface = new ProtocolMiscEndcCapabilityIndAdapterLegacy(pModemData);
    }
}

ProtocolMiscEndcCapabilityIndAdapter::~ProtocolMiscEndcCapabilityIndAdapter() {
    delete protocolMiscEndcCapabilityIndAdapterInterface;
    protocolMiscEndcCapabilityIndAdapterInterface = NULL;
}

int ProtocolMiscEndcCapabilityIndAdapter::GetCapability() const {
    return protocolMiscEndcCapabilityIndAdapterInterface->GetCapability();
}

int ProtocolMiscEndcCapabilityIndAdapter::GetCause() const {
    return protocolMiscEndcCapabilityIndAdapterInterface->GetCause();
}

/**
 * ProtocolMiscSetSelflogProfileAdapter
 */
ProtocolMiscSetSelflogProfileAdapter::ProtocolMiscSetSelflogProfileAdapter(const ModemData *pModemData)
    : ProtocolMiscSetSelflogProfileAdapterInterface() {
    protocolMiscSetSelflogProfileAdapterInterface = new ProtocolMiscSetSelflogProfileAdapterLegacy(pModemData);
}

ProtocolMiscSetSelflogProfileAdapter::~ProtocolMiscSetSelflogProfileAdapter() {
    delete protocolMiscSetSelflogProfileAdapterInterface;
    protocolMiscSetSelflogProfileAdapterInterface = NULL;
}

int ProtocolMiscSetSelflogProfileAdapter::GetResult() const {
    return protocolMiscSetSelflogProfileAdapterInterface->GetResult();
}

/**
 * ProtocolMiscSetForbidLteCellAdapter
 */
ProtocolMiscSetForbidLteCellAdapter::ProtocolMiscSetForbidLteCellAdapter(const ModemData *pModemData)
    : ProtocolMiscSetForbidLteCellAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscSetForbidLteCellAdapterInterface = new ProtocolMiscSetForbidLteCellAdapterJson(pModemData);
    } else {
        protocolMiscSetForbidLteCellAdapterInterface = new ProtocolMiscSetForbidLteCellAdapterLegacy(pModemData);
    }
}

ProtocolMiscSetForbidLteCellAdapter::~ProtocolMiscSetForbidLteCellAdapter() {
    delete protocolMiscSetForbidLteCellAdapterInterface;
    protocolMiscSetForbidLteCellAdapterInterface = NULL;
}

int ProtocolMiscSetForbidLteCellAdapter::GetResult() const {
    return protocolMiscSetForbidLteCellAdapterInterface->GetResult();
}

/**
 * ProtocolMiscSetApSystemTimeAdapter
 */
ProtocolMiscSetApSystemTimeAdapter::ProtocolMiscSetApSystemTimeAdapter(const ModemData *pModemData)
    : ProtocolMiscSetApSystemTimeAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscSetApSystemTimeAdapterInterface = new ProtocolMiscSetApSystemTimeAdapterJson(pModemData);
    else
        protocolMiscSetApSystemTimeAdapterInterface = new ProtocolMiscSetApSystemTimeAdapterLegacy(pModemData);
}

ProtocolMiscSetApSystemTimeAdapter::~ProtocolMiscSetApSystemTimeAdapter() {
    delete protocolMiscSetApSystemTimeAdapterInterface;
    protocolMiscSetApSystemTimeAdapterInterface = NULL;
}

int ProtocolMiscSetApSystemTimeAdapter::GetResult() const {
    return protocolMiscSetApSystemTimeAdapterInterface->GetResult();
}

/**
* ProtocolMiscSetForbidFrequencyAdapter
*/
ProtocolMiscSetForbidFrequencyAdapter::ProtocolMiscSetForbidFrequencyAdapter(const ModemData *pModemData)
    : ProtocolMiscSetForbidFrequencyAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscSetForbidFrequencyAdapterInterface = new ProtocolMiscSetForbidFrequencyAdapterJson(pModemData);
    } else {
        protocolMiscSetForbidFrequencyAdapterInterface = new ProtocolMiscSetForbidFrequencyAdapterLegacy(pModemData);
    }
}

ProtocolMiscSetForbidFrequencyAdapter::~ProtocolMiscSetForbidFrequencyAdapter() {
    delete protocolMiscSetForbidFrequencyAdapterInterface;
    protocolMiscSetForbidFrequencyAdapterInterface = NULL;
}

int ProtocolMiscSetForbidFrequencyAdapter::GetResult() const {
    return protocolMiscSetForbidFrequencyAdapterInterface->GetResult();
}

/**
 * ProtocolMiscSetForbidLteBandAdapter
 */
ProtocolMiscSetForbidLteBandAdapter::ProtocolMiscSetForbidLteBandAdapter(const ModemData *pModemData)
    : ProtocolMiscSetForbidLteBandAdapterInterface() {
    protocolMiscSetForbidLteBandAdapterInterface = new ProtocolMiscSetForbidLteBandAdapterLegacy(pModemData);
}

ProtocolMiscSetForbidLteBandAdapter::~ProtocolMiscSetForbidLteBandAdapter() {
    delete protocolMiscSetForbidLteBandAdapterInterface;
    protocolMiscSetForbidLteBandAdapterInterface = NULL;
}

int ProtocolMiscSetForbidLteBandAdapter::GetResult() const {
    return protocolMiscSetForbidLteBandAdapterInterface->GetResult();
}

/**
 * ProtocolMiscSetForbidLteTacAdapter
 */
ProtocolMiscSetForbidLteTacAdapter::ProtocolMiscSetForbidLteTacAdapter(const ModemData *pModemData)
    : ProtocolMiscSetForbidLteTacAdapterInterface() {
    protocolMiscSetForbidLteTacAdapterInterface = new ProtocolMiscSetForbidLteTacAdapterLegacy(pModemData);
}

ProtocolMiscSetForbidLteTacAdapter::~ProtocolMiscSetForbidLteTacAdapter() {
    delete protocolMiscSetForbidLteTacAdapterInterface;
    protocolMiscSetForbidLteTacAdapterInterface = NULL;
}

int ProtocolMiscSetForbidLteTacAdapter::GetResult() const {
    return protocolMiscSetForbidLteTacAdapterInterface->GetResult();
}

/**
 * ProtocolMiscSetAheadRreAdapter
 */
ProtocolMiscSetAheadRreAdapter::ProtocolMiscSetAheadRreAdapter(const ModemData *pModemData)
    : ProtocolMiscSetAheadRreAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscSetAheadRreAdapterInterface = new ProtocolMiscSetAheadRreAdapterJson(pModemData);
    } else {
        protocolMiscSetAheadRreAdapterInterface = new ProtocolMiscSetAheadRreAdapterLegacy(pModemData);
    }
}

ProtocolMiscSetAheadRreAdapter::~ProtocolMiscSetAheadRreAdapter() {
    delete protocolMiscSetAheadRreAdapterInterface;
    protocolMiscSetAheadRreAdapterInterface = NULL;
}

int ProtocolMiscSetAheadRreAdapter::GetResult() const {
    return protocolMiscSetAheadRreAdapterInterface->GetResult();
}

/**
 * ProtocolMiscSetForbidNrCellAdapter
 */
ProtocolMiscSetForbidNrCellAdapter::ProtocolMiscSetForbidNrCellAdapter(const ModemData *pModemData)
    : ProtocolMiscSetForbidNrCellAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscSetForbidNrCellAdapterInterface = new ProtocolMiscSetForbidNrCellAdapterJson(pModemData);
    } else {
        protocolMiscSetForbidNrCellAdapterInterface = new ProtocolMiscSetForbidNrCellAdapterLegacy(pModemData);
    }
}

ProtocolMiscSetForbidNrCellAdapter::~ProtocolMiscSetForbidNrCellAdapter() {
    delete protocolMiscSetForbidNrCellAdapterInterface;
    protocolMiscSetForbidNrCellAdapterInterface = NULL;
}

int ProtocolMiscSetForbidNrCellAdapter::GetResult() const {
    return protocolMiscSetForbidNrCellAdapterInterface->GetResult();
}

/**
 * ProtocolRfConnectionStateAdapter
 */
ProtocolRfConnectionStateAdapter::ProtocolRfConnectionStateAdapter(const ModemData *pModemData)
    : ProtocolRfConnectionStateAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolRfConnectionStateAdapterInterface = new ProtocolRfConnectionStateAdapterJson(pModemData);
    else
        protocolRfConnectionStateAdapterInterface = new ProtocolRfConnectionStateAdapterLegacy(pModemData);
}

ProtocolRfConnectionStateAdapter::~ProtocolRfConnectionStateAdapter() {
    delete protocolRfConnectionStateAdapterInterface;
    protocolRfConnectionStateAdapterInterface = NULL;
}

BYTE ProtocolRfConnectionStateAdapter::GetState() {
    return protocolRfConnectionStateAdapterInterface->GetState();
}

/**
 * ProtocolMiscGetCdmaCapabilityInfoAdapter
 */
ProtocolMiscGetCdmaCapabilityInfoAdapter::ProtocolMiscGetCdmaCapabilityInfoAdapter(const ModemData *pModemData)
    : ProtocolMiscGetCdmaCapabilityInfoAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscGetCdmaCapabilityInfoAdapterInterface = new ProtocolMiscGetCdmaCapabilityInfoAdapterJson(pModemData);
    } else {
        protocolMiscGetCdmaCapabilityInfoAdapterInterface = new ProtocolMiscGetCdmaCapabilityInfoAdapterLegacy(pModemData);
    }
}

ProtocolMiscGetCdmaCapabilityInfoAdapter::~ProtocolMiscGetCdmaCapabilityInfoAdapter() {
    delete protocolMiscGetCdmaCapabilityInfoAdapterInterface;
    protocolMiscGetCdmaCapabilityInfoAdapterInterface = NULL;
}

BYTE ProtocolMiscGetCdmaCapabilityInfoAdapter::GetCapability() const {
    return protocolMiscGetCdmaCapabilityInfoAdapterInterface->GetCapability();
}

/**
 * ProtocolMiscSetDeviceInfoAdapter
 */
ProtocolMiscSetDeviceInfoAdapter::ProtocolMiscSetDeviceInfoAdapter(const ModemData *pModemData)
    : ProtocolMiscSetDeviceInfoAdapterInterface() {
    if (IsPayloadTypeJson())
        protocolMiscSetDeviceInfoAdapterInterface = new ProtocolMiscSetDeviceInfoAdapterJson(pModemData);
    else
        protocolMiscSetDeviceInfoAdapterInterface = new ProtocolMiscSetDeviceInfoAdapterLegacy(pModemData);
}

ProtocolMiscSetDeviceInfoAdapter::~ProtocolMiscSetDeviceInfoAdapter() {
    delete protocolMiscSetDeviceInfoAdapterInterface;
    protocolMiscSetDeviceInfoAdapterInterface = NULL;
}

int ProtocolMiscSetDeviceInfoAdapter::GetVersionInfo(const char *input) {
    return protocolMiscSetDeviceInfoAdapterInterface->GetVersionInfo(input);
}

/**
 * ProtocolMiscBaseStationIndAdapter
 */
ProtocolMiscBaseStationIndAdapter::ProtocolMiscBaseStationIndAdapter(const ModemData *pModemData) {
    if (IsPayloadTypeJson()) {
        protocolMiscBaseStationIndAdapterInterface = new ProtocolMiscBaseStationIndAdapterJson(pModemData);
    } else {
        protocolMiscBaseStationIndAdapterInterface = new ProtocolMiscBaseStationIndAdapterLegacy(pModemData);
    }
}

ProtocolMiscBaseStationIndAdapter::~ProtocolMiscBaseStationIndAdapter() {
    delete protocolMiscBaseStationIndAdapterInterface;
    protocolMiscBaseStationIndAdapterInterface = NULL;
}

/**
 * ProtocolMiscSetActiveVsimAdapter
 */
ProtocolMiscSetActiveVsimAdapter::ProtocolMiscSetActiveVsimAdapter(const ModemData *pModemData) {
    if (IsPayloadTypeJson()) {
        protocolMiscSetActvieVsimAdapterInterface = new ProtocolMiscSetActiveVsimAdapterJson(pModemData);
    } else {
        protocolMiscSetActvieVsimAdapterInterface = new ProtocolMiscSetActiveVsimAdapterLegacy(pModemData);
    }
}

ProtocolMiscSetActiveVsimAdapter::~ProtocolMiscSetActiveVsimAdapter() {
    delete protocolMiscSetActvieVsimAdapterInterface;
    protocolMiscSetActvieVsimAdapterInterface = NULL;
}

/**
 * ProtocolMiscGetModemActivityInfoAdapter
 */
ProtocolMiscGetModemActivityInfoAdapter::ProtocolMiscGetModemActivityInfoAdapter(const ModemData *pModemData) {
    if (IsPayloadTypeJson()) {
        protocolMiscGetModemActivityInfoAdapterInterface = new ProtocolMiscGetModemActivityInfoAdapterJson(pModemData);
    } else {
        protocolMiscGetModemActivityInfoAdapterInterface = new ProtocolMiscGetModemActivityInfoAdapterLegacy(pModemData);
    }
}

ProtocolMiscGetModemActivityInfoAdapter::~ProtocolMiscGetModemActivityInfoAdapter() {
    delete protocolMiscGetModemActivityInfoAdapterInterface;
    protocolMiscGetModemActivityInfoAdapterInterface = NULL;
}

/**
 * ProtocolMiscDropMtCallInfoIndAdapterLegacy
 */
ProtocolMiscDropMtCallInfoIndAdapter::ProtocolMiscDropMtCallInfoIndAdapter(const ModemData *pModemData) {
    if (IsPayloadTypeJson()) {
        protocolMiscDropMtCallInfoIndAdapterInterface = new ProtocolMiscDropMtCallInfoIndAdapterJson(pModemData);
    } else {
        protocolMiscDropMtCallInfoIndAdapterInterface = new ProtocolMiscDropMtCallInfoIndAdapterLegacy(pModemData);
    }
}

ProtocolMiscDropMtCallInfoIndAdapter::~ProtocolMiscDropMtCallInfoIndAdapter() {
    delete protocolMiscDropMtCallInfoIndAdapterInterface;
    protocolMiscDropMtCallInfoIndAdapterInterface = NULL;
}

/**
 * ProtocolMiscCarrierConfigSimInfoIndAdapter
 */
ProtocolMiscCarrierConfigSimInfoIndAdapter::ProtocolMiscCarrierConfigSimInfoIndAdapter(const ModemData *pModemData)
    : ProtocolMiscCarrierConfigSimInfoIndAdapterInterface() {
    protocolMiscCarrierConfigSimInfoIndAdapterInterface = new ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy(pModemData);
}

ProtocolMiscCarrierConfigSimInfoIndAdapter::~ProtocolMiscCarrierConfigSimInfoIndAdapter() {
    delete protocolMiscCarrierConfigSimInfoIndAdapterInterface;
    protocolMiscCarrierConfigSimInfoIndAdapterInterface = NULL;
}

/**
 * ProtocolMiscCarrierConfigStatusIndAdapter
ProtocolMiscCarrierConfigStatusIndAdapterInterface */
ProtocolMiscCarrierConfigStatusIndAdapter::ProtocolMiscCarrierConfigStatusIndAdapter(const ModemData *pModemData)
    : ProtocolMiscCarrierConfigStatusIndAdapterInterface() {
    protocolMiscCarrierConfigStatusIndAdapterInterface = new ProtocolMiscCarrierConfigStatusIndAdapterLegacy(pModemData);
}

ProtocolMiscCarrierConfigStatusIndAdapter::~ProtocolMiscCarrierConfigStatusIndAdapter() {
    delete protocolMiscCarrierConfigStatusIndAdapterInterface;
    protocolMiscCarrierConfigStatusIndAdapterInterface = NULL;
}

/**
 * ProtocolMiscGetTraceDumpCfgAdapter
 */
ProtocolMiscGetTraceDumpCfgAdapter::ProtocolMiscGetTraceDumpCfgAdapter(const ModemData *pModemData)
    : ProtocolMiscGetTraceDumpCfgAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolMiscGetTraceDumpCfgAdapterInterface = new ProtocolMiscGetTraceDumpCfgAdapterJson(pModemData);
    } else {
        protocolMiscGetTraceDumpCfgAdapterInterface = new ProtocolMiscGetTraceDumpCfgAdapterLegacy(pModemData);
    }
}

ProtocolMiscGetTraceDumpCfgAdapter::~ProtocolMiscGetTraceDumpCfgAdapter() {
    delete protocolMiscGetTraceDumpCfgAdapterInterface;
    protocolMiscGetTraceDumpCfgAdapterInterface = NULL;
}

BYTE ProtocolMiscGetTraceDumpCfgAdapter::GetConfig() const {
    return protocolMiscGetTraceDumpCfgAdapterInterface->GetConfig();
}

/**
 * ProtocolMiscGetUeOperationModeAdapter
 */
ProtocolMiscGetUeOperationModeAdapter::ProtocolMiscGetUeOperationModeAdapter(const ModemData *pModemData)
    : ProtocolMiscGetUeOperationModeAdapterInterface() {
    protocolMiscGetUeOperationModeAdapterInterface = new ProtocolMiscGetUeOperationModeAdapterLegacy(pModemData);
}

ProtocolMiscGetUeOperationModeAdapter::~ProtocolMiscGetUeOperationModeAdapter() {
    delete protocolMiscGetUeOperationModeAdapterInterface;
    protocolMiscGetUeOperationModeAdapterInterface = nullptr;
}

BYTE ProtocolMiscGetUeOperationModeAdapter::GetMode() const {
    return protocolMiscGetUeOperationModeAdapterInterface->GetMode();
}

/**
 * ProtocolMiscGetNrrrmMinReqEnableAdapter
 */
ProtocolMiscGetNrrrmMinReqEnableAdapter::ProtocolMiscGetNrrrmMinReqEnableAdapter(const ModemData *pModemData)
    : ProtocolMiscGetNrrrmMinReqEnableAdapterInterface() {
    protocolMiscGetNrrrmMinReqEnableAdapterInterface = new ProtocolMiscGetNrrrmMinReqEnableAdapterLegacy(pModemData);
}

ProtocolMiscGetNrrrmMinReqEnableAdapter::~ProtocolMiscGetNrrrmMinReqEnableAdapter() {
    delete protocolMiscGetNrrrmMinReqEnableAdapterInterface;
    protocolMiscGetNrrrmMinReqEnableAdapterInterface = nullptr;
}

BYTE ProtocolMiscGetNrrrmMinReqEnableAdapter::GetEnable() const {
    return protocolMiscGetNrrrmMinReqEnableAdapterInterface->GetEnable();
}

/**
 * ProtocolMiscGetBinaryVersionInfoAdapter
 */
ProtocolMiscGetBinaryVersionInfoAdapter::ProtocolMiscGetBinaryVersionInfoAdapter(const ModemData *pModemData)
    : ProtocolMiscGetBinaryVersionInfoAdapterInterface() {
    protocolMiscGetBinaryVersionInfoAdapterInterface = new ProtocolMiscGetBinaryVersionInfoAdapterLegacy(pModemData);
}

ProtocolMiscGetBinaryVersionInfoAdapter::~ProtocolMiscGetBinaryVersionInfoAdapter() {
    delete protocolMiscGetBinaryVersionInfoAdapterInterface;
    protocolMiscGetBinaryVersionInfoAdapterInterface = nullptr;
}

int ProtocolMiscGetBinaryVersionInfoAdapter::GetVersionInfoLength() const {
    return protocolMiscGetBinaryVersionInfoAdapterInterface->GetVersionInfoLength();
}

const char * ProtocolMiscGetBinaryVersionInfoAdapter::GetData() const {
    return protocolMiscGetBinaryVersionInfoAdapterInterface->GetData();
}

/**
 * ProtocolMiscGetVcrtAdapter
 */
ProtocolMiscGetVcrtAdapter::ProtocolMiscGetVcrtAdapter(const ModemData *pModemData)
    : ProtocolMiscGetVcrtAdapterInterface() {
    protocolMiscGetVcrtAdapterInterface = new ProtocolMiscGetVcrtAdapterLegacy(pModemData);
}

ProtocolMiscGetVcrtAdapter::~ProtocolMiscGetVcrtAdapter() {
    delete protocolMiscGetVcrtAdapterInterface;
    protocolMiscGetVcrtAdapterInterface = nullptr;
}

BYTE ProtocolMiscGetVcrtAdapter::GetVcrtMode() const {
    return protocolMiscGetVcrtAdapterInterface->GetVcrtMode();
}
