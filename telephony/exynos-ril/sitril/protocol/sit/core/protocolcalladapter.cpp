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
 * protocolcalladapter.cpp
 *
 *  Created on: 2021. 06. 08.
 *      Author: p1.bansal
 */

#include "protocolcalladapter.h"
#include "legacy/cs/protocolcalladapterlegacy.h"
#include "json/cs/protocolcalladapterjson.h"
#include "modemdata.h"

/**
 * ProtocolGetCurrentCallAdapter
 */
ProtocolGetCurrentCallAdapter::ProtocolGetCurrentCallAdapter(const ModemData *pModemData)
    : ProtocolGetCurrentCallAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetCurrentCallAdapterInterface = new ProtocolGetCurrentCallAdapterJson(pModemData);
    } else {
        protocolGetCurrentCallAdapterInterface = new ProtocolGetCurrentCallAdapterLegacy(pModemData);
    }
}

ProtocolGetCurrentCallAdapter::~ProtocolGetCurrentCallAdapter() {
    delete protocolGetCurrentCallAdapterInterface;
    protocolGetCurrentCallAdapterInterface = NULL;
}

int ProtocolGetCurrentCallAdapter::GetCallNum() {
    return protocolGetCurrentCallAdapterInterface->GetCallNum();
}

bool ProtocolGetCurrentCallAdapter::HasValidLength() {
    return protocolGetCurrentCallAdapterInterface->HasValidLength();
}

int ProtocolGetCurrentCallAdapter::GetCallInfo(CallInfo *pCallInfo, int index) {
    return protocolGetCurrentCallAdapterInterface->GetCallInfo(pCallInfo, index);
}

void ProtocolGetCurrentCallAdapter::DebugPrintCallInfo(CallInfo *pCallInfo) {
    return protocolGetCurrentCallAdapterInterface->DebugPrintCallInfo(pCallInfo);
}

/**
 * ProtocolGetLastCallFailCauseAdapter
 */
ProtocolGetLastCallFailCauseAdapter::ProtocolGetLastCallFailCauseAdapter(const ModemData *pModemData)
    : ProtocolGetLastCallFailCauseAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetLastCallFailCauseAdapterInterface = new ProtocolGetLastCallFailCauseAdapterJson(pModemData);
    } else {
        protocolGetLastCallFailCauseAdapterInterface = new ProtocolGetLastCallFailCauseAdapterLegacy(pModemData);
    }
}

ProtocolGetLastCallFailCauseAdapter::~ProtocolGetLastCallFailCauseAdapter() {
    delete protocolGetLastCallFailCauseAdapterInterface;
    protocolGetLastCallFailCauseAdapterInterface = NULL;
}

int ProtocolGetLastCallFailCauseAdapter::GetLastCallFailCause() {
    return protocolGetLastCallFailCauseAdapterInterface->GetLastCallFailCause();
}

/**
 * ProtocolGetClipAdapter
 */
ProtocolGetClipAdapter::ProtocolGetClipAdapter(const ModemData *pModemData)
    : ProtocolGetClipAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetClipAdapterInterface = new ProtocolGetClipAdapterJson(pModemData);
    } else {
        protocolGetClipAdapterInterface = new ProtocolGetClipAdapterLegacy(pModemData);
    }
}

ProtocolGetClipAdapter::~ProtocolGetClipAdapter() {
    delete protocolGetClipAdapterInterface;
    protocolGetClipAdapterInterface = NULL;
}

int ProtocolGetClipAdapter::GetClipStatus() {
    return protocolGetClipAdapterInterface->GetClipStatus();
}

/**
 * ProtocolGetClirAdapter
 */
ProtocolGetClirAdapter::ProtocolGetClirAdapter(const ModemData *pModemData)
    : ProtocolGetClirAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetClirAdapterInterface = new ProtocolGetClirAdapterJson(pModemData);
    } else {
        protocolGetClirAdapterInterface = new ProtocolGetClirAdapterLegacy(pModemData);
    }
}

ProtocolGetClirAdapter::~ProtocolGetClirAdapter() {
    delete protocolGetClirAdapterInterface;
    protocolGetClirAdapterInterface = NULL;
}

int ProtocolGetClirAdapter::GetClirStatus() {
    return protocolGetClirAdapterInterface->GetClirStatus();
}

/**
 * ProtocolGetCallForwardingStatusAdapter
 */
ProtocolGetCallForwardingStatusAdapter::ProtocolGetCallForwardingStatusAdapter(const ModemData *pModemData)
    : ProtocolGetCallForwardingStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetCallForwardingStatusAdapterInterface = new ProtocolGetCallForwardingStatusAdapterJson(pModemData);
    } else {
        protocolGetCallForwardingStatusAdapterInterface = new ProtocolGetCallForwardingStatusAdapterLegacy(pModemData);
    }
}

ProtocolGetCallForwardingStatusAdapter::~ProtocolGetCallForwardingStatusAdapter() {
    delete protocolGetCallForwardingStatusAdapterInterface;
    protocolGetCallForwardingStatusAdapterInterface = NULL;
}

int ProtocolGetCallForwardingStatusAdapter::GetCfNum() {
    return protocolGetCallForwardingStatusAdapterInterface->GetCfNum();
}

bool ProtocolGetCallForwardingStatusAdapter::HasValidLength() {
    return protocolGetCallForwardingStatusAdapterInterface->HasValidLength();
}

int ProtocolGetCallForwardingStatusAdapter::GetCfInfo(RIL_CallForwardInfo *pCfInfo, int index) {
    return protocolGetCallForwardingStatusAdapterInterface->GetCfInfo(pCfInfo, index);
}

void ProtocolGetCallForwardingStatusAdapter::DebugPrintCfInfo(RIL_CallForwardInfo *pCfInfo) {
    return protocolGetCallForwardingStatusAdapterInterface->DebugPrintCfInfo(pCfInfo);
}

/**
 * ProtocolGetCallWaitingAdapter
 */
ProtocolGetCallWaitingAdapter::ProtocolGetCallWaitingAdapter(const ModemData *pModemData)
    : ProtocolGetCallWaitingAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetCallWaitingAdapterInterface = new ProtocolGetCallWaitingAdapterJson(pModemData);
    } else {
        protocolGetCallWaitingAdapterInterface = new ProtocolGetCallWaitingAdapterLegacy(pModemData);
    }
}

ProtocolGetCallWaitingAdapter::~ProtocolGetCallWaitingAdapter() {
    delete protocolGetCallWaitingAdapterInterface;
    protocolGetCallWaitingAdapterInterface = NULL;
}

int ProtocolGetCallWaitingAdapter::GetServiceStatus() {
    return protocolGetCallWaitingAdapterInterface->GetServiceStatus();
}

int ProtocolGetCallWaitingAdapter::GetServiceClass() {
    return protocolGetCallWaitingAdapterInterface->GetServiceClass();
}

/**
 * ProtocolUssdIndAdapter
 */
ProtocolUssdIndAdapter::ProtocolUssdIndAdapter(const ModemData *pModemData)
    : ProtocolUssdIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolUssdIndAdapterInterface = new ProtocolUssdIndAdapterJson(pModemData);
    } else {
        protocolUssdIndAdapterInterface = new ProtocolUssdIndAdapterLegacy(pModemData);
    }
}

ProtocolUssdIndAdapter::~ProtocolUssdIndAdapter() {
    delete protocolUssdIndAdapterInterface;
    protocolUssdIndAdapterInterface = NULL;
}

int ProtocolUssdIndAdapter::GetDecodedUssd(char *decodedUssd, size_t buf_size, int &dcs) {
    return protocolUssdIndAdapterInterface->GetDecodedUssd(decodedUssd, buf_size, dcs);
}

int ProtocolUssdIndAdapter::GetUssdStatus() {
    return protocolUssdIndAdapterInterface->GetUssdStatus();
}

/**
 * ProtocolSsSvcIndAdapter
 */
ProtocolSsSvcIndAdapter::ProtocolSsSvcIndAdapter(const ModemData *pModemData)
    : ProtocolSsSvcIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSsSvcIndAdapterInterface = new ProtocolSsSvcIndAdapterJson(pModemData);
    } else {
        protocolSsSvcIndAdapterInterface = new ProtocolSsSvcIndAdapterLegacy(pModemData);
    }
}

ProtocolSsSvcIndAdapter::~ProtocolSsSvcIndAdapter() {
    delete protocolSsSvcIndAdapterInterface;
    protocolSsSvcIndAdapterInterface = NULL;
}

int ProtocolSsSvcIndAdapter::GetNotificationType() {
    return protocolSsSvcIndAdapterInterface->GetNotificationType();
}

int ProtocolSsSvcIndAdapter::GetCode() {
    return protocolSsSvcIndAdapterInterface->GetCode();
}

int ProtocolSsSvcIndAdapter::GetCugIndex() {
    return protocolSsSvcIndAdapterInterface->GetCugIndex();
}

UINT ProtocolSsSvcIndAdapter::GetSSType() {
    return protocolSsSvcIndAdapterInterface->GetSSType();
}

int ProtocolSsSvcIndAdapter::GetNumberLength() {
    return protocolSsSvcIndAdapterInterface->GetNumberLength();
}

char *ProtocolSsSvcIndAdapter::GetNumber() {
    return protocolSsSvcIndAdapterInterface->GetNumber();
}

/**
 * ProtocolGetColpAdapter
 */
ProtocolGetColpAdapter::ProtocolGetColpAdapter(const ModemData *pModemData)
    : ProtocolGetColpAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetColpAdapterInterface = new ProtocolGetColpAdapterJson(pModemData);
    } else {
        protocolGetColpAdapterInterface = new ProtocolGetColpAdapterLegacy(pModemData);
    }
}

ProtocolGetColpAdapter::~ProtocolGetColpAdapter() {
    delete protocolGetColpAdapterInterface;
    protocolGetColpAdapterInterface = NULL;
}
int ProtocolGetColpAdapter::GetColpStatus() {
    return protocolGetColpAdapterInterface->GetColpStatus();
}

/**
 * ProtocolGetColrAdapter
 */
ProtocolGetColrAdapter::ProtocolGetColrAdapter(const ModemData *pModemData)
    : ProtocolGetColrAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetColrAdapterInterface = new ProtocolGetColrAdapterJson(pModemData);
    } else {
        protocolGetColrAdapterInterface = new ProtocolGetColrAdapterLegacy(pModemData);
    }
}

ProtocolGetColrAdapter::~ProtocolGetColrAdapter() {
    delete protocolGetColrAdapterInterface;
    protocolGetColrAdapterInterface = NULL;
}
int ProtocolGetColrAdapter::GetColrStatus() {
    return protocolGetColrAdapterInterface->GetColrStatus();
}

/**
 * ProtocolSendCallConfirmRespAdapter
 */
ProtocolSendCallConfirmRespAdapter::ProtocolSendCallConfirmRespAdapter(const ModemData *pModemData)
    : ProtocolSendCallConfirmRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSendCallConfirmRespAdapterInterface = new ProtocolSendCallConfirmRespAdapterJson(pModemData);
    } else {
        protocolSendCallConfirmRespAdapterInterface = new ProtocolSendCallConfirmRespAdapterLegacy(pModemData);
    }
}

ProtocolSendCallConfirmRespAdapter::~ProtocolSendCallConfirmRespAdapter() {
    delete protocolSendCallConfirmRespAdapterInterface;
    protocolSendCallConfirmRespAdapterInterface = NULL;
}

int ProtocolSendCallConfirmRespAdapter::GetResult() const {
    return protocolSendCallConfirmRespAdapterInterface->GetResult();
}

/**
 * ProtocolEmergencyCallListIndAdapter
 */
ProtocolEmergencyCallListIndAdapter::ProtocolEmergencyCallListIndAdapter(const ModemData *pModemData)
    : ProtocolEmergencyCallListIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolEmergencyCallListIndAdapterInterface = new ProtocolEmergencyCallListIndAdapterJson(pModemData);
    } else {
        protocolEmergencyCallListIndAdapterInterface = new ProtocolEmergencyCallListIndAdapterLegacy(pModemData);
    }
}

ProtocolEmergencyCallListIndAdapter::~ProtocolEmergencyCallListIndAdapter() {
    delete protocolEmergencyCallListIndAdapterInterface;
    protocolEmergencyCallListIndAdapterInterface = NULL;
}

int ProtocolEmergencyCallListIndAdapter::GetVersion() const {
    return protocolEmergencyCallListIndAdapterInterface->GetVersion();
}

const char *ProtocolEmergencyCallListIndAdapter::GetMcc() const {
    return protocolEmergencyCallListIndAdapterInterface->GetMcc();
}

const char *ProtocolEmergencyCallListIndAdapter::GetMnc() const {
    return protocolEmergencyCallListIndAdapterInterface->GetMnc();
}

int ProtocolEmergencyCallListIndAdapter::GetNum() const {
    return protocolEmergencyCallListIndAdapterInterface->GetNum();
}

void ProtocolEmergencyCallListIndAdapter::GetEccInfo(EccInfo *eccInfo) {
    return protocolEmergencyCallListIndAdapterInterface->GetEccInfo(eccInfo);
}

/**
 * ProtocolGetPreferredVoicePrivacyModeAdapter
 */
ProtocolGetPreferredVoicePrivacyModeAdapter::ProtocolGetPreferredVoicePrivacyModeAdapter(const ModemData *pModemData)
    : ProtocolGetPreferredVoicePrivacyModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetPreferredVoicePrivacyModeAdapterInterface = new ProtocolGetPreferredVoicePrivacyModeAdapterJson(pModemData);
    } else {
        protocolGetPreferredVoicePrivacyModeAdapterInterface = new ProtocolGetPreferredVoicePrivacyModeAdapterLegacy(pModemData);
    }
}

ProtocolGetPreferredVoicePrivacyModeAdapter::~ProtocolGetPreferredVoicePrivacyModeAdapter() {
    delete protocolGetPreferredVoicePrivacyModeAdapterInterface;
    protocolGetPreferredVoicePrivacyModeAdapterInterface = NULL;
}

int ProtocolGetPreferredVoicePrivacyModeAdapter::GetPreferredVoicePrivacyMode() {
    return protocolGetPreferredVoicePrivacyModeAdapterInterface->GetPreferredVoicePrivacyMode();
}

/**
 * ProtocolCdmaCallWaitingIndAdapter
 */
ProtocolCdmaCallWaitingIndAdapter::ProtocolCdmaCallWaitingIndAdapter(const ModemData *pModemData)
    : ProtocolCdmaCallWaitingIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolCdmaCallWaitingIndAdapterInterface = new ProtocolCdmaCallWaitingIndAdapterJson(pModemData);
    } else {
        protocolCdmaCallWaitingIndAdapterInterface = new ProtocolCdmaCallWaitingIndAdapterLegacy(pModemData);
    }
}

ProtocolCdmaCallWaitingIndAdapter::~ProtocolCdmaCallWaitingIndAdapter() {
    delete protocolCdmaCallWaitingIndAdapterInterface;
    protocolCdmaCallWaitingIndAdapterInterface = NULL;
}

int ProtocolCdmaCallWaitingIndAdapter::GetCwInfo(RIL_CDMA_CallWaiting_v6 *pCwInfo) {
    return protocolCdmaCallWaitingIndAdapterInterface->GetCwInfo(pCwInfo);
}

/**
 * ProtocolCdmaInfoListIndAdapter
 */
ProtocolCdmaInfoListIndAdapter::ProtocolCdmaInfoListIndAdapter(const ModemData *pModemData)
    : ProtocolCdmaInfoListIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolCdmaInfoListIndAdapterInterface = new ProtocolCdmaInfoListIndAdapterJson(pModemData);
    } else {
        protocolCdmaInfoListIndAdapterInterface = new ProtocolCdmaInfoListIndAdapterLegacy(pModemData);
    }
}

ProtocolCdmaInfoListIndAdapter::~ProtocolCdmaInfoListIndAdapter() {
    delete protocolCdmaInfoListIndAdapterInterface;
    protocolCdmaInfoListIndAdapterInterface = NULL;
}

int ProtocolCdmaInfoListIndAdapter::GetNumberOfInfoRecs() const {
    return protocolCdmaInfoListIndAdapterInterface->GetNumberOfInfoRecs();
}

int ProtocolCdmaInfoListIndAdapter::GetCdmaInfo(RIL_CDMA_InformationRecord &cdmaInfo, int index) const {
    return protocolCdmaInfoListIndAdapterInterface->GetCdmaInfo(cdmaInfo, index);
}

/**
 * ProtocolCdmaOtaProvisionStatusIndAdapter
 */
ProtocolCdmaOtaProvisionStatusIndAdapter::ProtocolCdmaOtaProvisionStatusIndAdapter(const ModemData *pModemData)
    : ProtocolCdmaOtaProvisionStatusIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolCdmaOtaProvisionStatusIndAdapterInterface = new ProtocolCdmaOtaProvisionStatusIndAdapterJson(pModemData);
    } else {
        protocolCdmaOtaProvisionStatusIndAdapterInterface = new ProtocolCdmaOtaProvisionStatusIndAdapterLegacy(pModemData);
    }
}

ProtocolCdmaOtaProvisionStatusIndAdapter::~ProtocolCdmaOtaProvisionStatusIndAdapter() {
    delete protocolCdmaOtaProvisionStatusIndAdapterInterface;
    protocolCdmaOtaProvisionStatusIndAdapterInterface = NULL;
}

int ProtocolCdmaOtaProvisionStatusIndAdapter::GetOtaProvisionStatus() const {
    return protocolCdmaOtaProvisionStatusIndAdapterInterface->GetOtaProvisionStatus();
}

/**
 * ProtocolEmergencySupportRatModeIndAdapter
 */
ProtocolEmergencySupportRatModeIndAdapter::ProtocolEmergencySupportRatModeIndAdapter(const ModemData *pModemData)
    : ProtocolEmergencySupportRatModeIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolEmergencySupportRatModeIndAdapterInterface = new ProtocolEmergencySupportRatModeIndAdapterJson(pModemData);
    } else {
        protocolEmergencySupportRatModeIndAdapterInterface = new ProtocolEmergencySupportRatModeIndAdapterLegacy(pModemData);
    }
}

ProtocolEmergencySupportRatModeIndAdapter::~ProtocolEmergencySupportRatModeIndAdapter() {
    delete protocolEmergencySupportRatModeIndAdapterInterface;
    protocolEmergencySupportRatModeIndAdapterInterface = NULL;
}

int ProtocolEmergencySupportRatModeIndAdapter::GetSupportRatMode() const {
    return protocolEmergencySupportRatModeIndAdapterInterface->GetSupportRatMode();
}

/**
 * ProtocolExitEmergencyCbModeRespAdapter
 */
ProtocolExitEmergencyCbModeRespAdapter::ProtocolExitEmergencyCbModeRespAdapter(const ModemData *pModemData)
    : ProtocolExitEmergencyCbModeRespAdapterInterface() {
    protocolExitEmergencyCbModeRespAdapterInterface = new ProtocolExitEmergencyCbModeRespAdapterLegacy(pModemData);
}

ProtocolExitEmergencyCbModeRespAdapter::~ProtocolExitEmergencyCbModeRespAdapter() {
    delete protocolExitEmergencyCbModeRespAdapterInterface;
    protocolExitEmergencyCbModeRespAdapterInterface = NULL;
}

bool ProtocolExitEmergencyCbModeRespAdapter::GetResult() const {
    return protocolExitEmergencyCbModeRespAdapterInterface->GetResult();
}

RIL_Errno ProtocolExitEmergencyCbModeRespAdapter::GetRilErrorCode() const {
    return protocolExitEmergencyCbModeRespAdapterInterface->GetRilErrorCode();
}

/**
 * ProtocolUnsolOnSSAdapter
 */
ProtocolUnsolOnSSAdapter::ProtocolUnsolOnSSAdapter(const ModemData *pModemData)
    : ProtocolUnsolOnSSAdapterInterface() {
    protocolUnsolOnSSAdapterInterface = new ProtocolUnsolOnSSAdapterLegacy(pModemData);
}

ProtocolUnsolOnSSAdapter::~ProtocolUnsolOnSSAdapter() {
    delete protocolUnsolOnSSAdapterInterface;
    protocolUnsolOnSSAdapterInterface = NULL;
}

int ProtocolUnsolOnSSAdapter::GetServiceType() const {
    return protocolUnsolOnSSAdapterInterface->GetServiceType();
}

int ProtocolUnsolOnSSAdapter::GetRequestType() const {
    return protocolUnsolOnSSAdapterInterface->GetRequestType();
}

int ProtocolUnsolOnSSAdapter::GetTeleServiceType() const {
    return protocolUnsolOnSSAdapterInterface->GetTeleServiceType();
}

int ProtocolUnsolOnSSAdapter::GetServiceClass() const {
    return protocolUnsolOnSSAdapterInterface->GetServiceClass();
}

int ProtocolUnsolOnSSAdapter::GetResult() const {
    return protocolUnsolOnSSAdapterInterface->GetResult();
}

int ProtocolUnsolOnSSAdapter::GetDataType() const {
    return protocolUnsolOnSSAdapterInterface->GetDataType();
}

bool ProtocolUnsolOnSSAdapter::GetData(void *pData) const {
    return protocolUnsolOnSSAdapterInterface->GetData(pData);
}

/**
  * ProtocolSrvccHoIndAdapter
 */
ProtocolSrvccHoIndAdapter::ProtocolSrvccHoIndAdapter(const ModemData *pModemData)
    : ProtocolSrvccHoIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSrvccHoIndAdapterInterface = new ProtocolSrvccHoIndAdapterJson(pModemData);
    } else {
        protocolSrvccHoIndAdapterInterface = new ProtocolSrvccHoIndAdapterLegacy(pModemData);
    }
}

ProtocolSrvccHoIndAdapter::~ProtocolSrvccHoIndAdapter() {
    delete protocolSrvccHoIndAdapterInterface;
    protocolSrvccHoIndAdapterInterface = NULL;
}

BYTE ProtocolSrvccHoIndAdapter::GetSrvccHoType() const {
    return protocolSrvccHoIndAdapterInterface->GetSrvccHoType();
}

BYTE ProtocolSrvccHoIndAdapter::GetSrvccHoStatus() const {
    return protocolSrvccHoIndAdapterInterface->GetSrvccHoStatus();
}

BYTE ProtocolSrvccHoIndAdapter::GetSrvccHoFailCause() const {
    return protocolSrvccHoIndAdapterInterface->GetSrvccHoFailCause();
}
