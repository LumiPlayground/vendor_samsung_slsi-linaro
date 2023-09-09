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
 * protocolmiscbuilder.cpp
 *
 *  Created on: 2021. 06. 17.
 *      Author: p1.bansal
 */

#include "protocolmiscbuilder.h"
#include "legacy/misc/protocolmiscbuilderlegacy.h"
#include "json/misc/protocolmiscbuilderjson.h"

/**
 * ProtocolMiscBuilder
 */
ProtocolMiscBuilder::ProtocolMiscBuilder()
    : ProtocolMiscBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolMiscBuilderInterface = new ProtocolMiscBuilderJson();
    else
        protocolMiscBuilderInterface = new ProtocolMiscBuilderLegacy();
}

ProtocolMiscBuilder::~ProtocolMiscBuilder() {
    delete protocolMiscBuilderInterface;
    protocolMiscBuilderInterface = NULL;
}

ModemData *ProtocolMiscBuilder::GetTtyMode() {
    return protocolMiscBuilderInterface->GetTtyMode();
}

ModemData *ProtocolMiscBuilder::SetTtyMode(int mode) {
    return protocolMiscBuilderInterface->SetTtyMode(mode);
}

ModemData *ProtocolMiscBuilder::BuildNvReadItem(int nvItemId) {
    return protocolMiscBuilderInterface->BuildNvReadItem(nvItemId);
}

ModemData *ProtocolMiscBuilder::BuildNvWriteItem(int nvItemId, const char *value) {
    return protocolMiscBuilderInterface->BuildNvWriteItem(nvItemId, value);
}

ModemData *ProtocolMiscBuilder::GetModemActivityInfo() {
    return protocolMiscBuilderInterface->GetModemActivityInfo();
}

ModemData *ProtocolMiscBuilder::BuildSetSuppSvcNotification(int enable) {
    return protocolMiscBuilderInterface->BuildSetSuppSvcNotification(enable);
}

ModemData *ProtocolMiscBuilder::BuildModemInfo(int type, char *data, unsigned int size) {
    return protocolMiscBuilderInterface->BuildModemInfo(type, data, size);
}

ModemData *ProtocolMiscBuilder::BuildSwitchModemFunction(int feature, BYTE enable) {
    return protocolMiscBuilderInterface->BuildSwitchModemFunction(feature, enable);
}

ModemData *ProtocolMiscBuilder::BuildSetPdcpDiscardTimer(int discardTimer) {
    return protocolMiscBuilderInterface->BuildSetPdcpDiscardTimer(discardTimer);
}

ModemData *ProtocolMiscBuilder::BuildSetActivateVsim(int slot, int iccidLen, const char *pIccid,
                                                     int imsiLen, const char *pImsi, const char *pHplmn,
                                                     int vsimState, int vsimCardType) {
    return protocolMiscBuilderInterface->BuildSetActivateVsim(slot, iccidLen, pIccid,
                                                              imsiLen, pImsi, pHplmn,
                                                              vsimState, vsimCardType);
}

ModemData *ProtocolMiscBuilder::BuildGetCqiInfo() {
    return protocolMiscBuilderInterface->BuildGetCqiInfo();
}

ModemData *ProtocolMiscBuilder::BuildSetSarSetting(int dsi) {
    return protocolMiscBuilderInterface->BuildSetSarSetting(dsi);
}

ModemData *ProtocolMiscBuilder::BuildSetImsTestMode(int mode) {
    return protocolMiscBuilderInterface->BuildSetImsTestMode(mode);
}

ModemData *ProtocolMiscBuilder::BuildSetGmoSwitch(int feature) {
    return protocolMiscBuilderInterface->BuildSetGmoSwitch(feature);
}

ModemData *ProtocolMiscBuilder::BuildSetTcsFci(int state, int len, char *fci) {
    return protocolMiscBuilderInterface->BuildSetTcsFci(state, len, fci);
}

ModemData *ProtocolMiscBuilder::BuildGetTcsFci() {
    return protocolMiscBuilderInterface->BuildGetTcsFci();
}

ModemData *ProtocolMiscBuilder::BuildSetCABandwidthFilter(int enable) {
    return protocolMiscBuilderInterface->BuildSetCABandwidthFilter(enable);
}

ModemData *ProtocolMiscBuilder::BuildSetElevatorSensor(int enable) {
    return protocolMiscBuilderInterface->BuildSetElevatorSensor(enable);
}

ModemData *ProtocolMiscBuilder::BuildSetLocationUpdates(int enable) {
    return protocolMiscBuilderInterface->BuildSetLocationUpdates(enable);
}

ModemData *ProtocolMiscBuilder::BuildSetForbidLteCell(int mode, int cellId, int forbiddenTimer, char *plmn) {
    return protocolMiscBuilderInterface->BuildSetForbidLteCell(mode, cellId, forbiddenTimer, plmn);
}

ModemData *ProtocolMiscBuilder::BuildGetModemActivityInfo() {
    return protocolMiscBuilderInterface->BuildGetModemActivityInfo();
}

ModemData *ProtocolMiscBuilder::BuildOemSetSvn(char *svn) {
    return protocolMiscBuilderInterface->BuildOemSetSvn(svn);
}

ModemData *ProtocolMiscBuilder::BuildSetForbidFrequency(int mode, int frequency, int forbiddenTimer) {
    return protocolMiscBuilderInterface->BuildSetForbidFrequency(mode, frequency, forbiddenTimer);
}

ModemData *ProtocolMiscBuilder::BuildSetForbidLetBand(int numBands, int *band) {
    return protocolMiscBuilderInterface->BuildSetForbidLetBand(numBands, band);
}

ModemData *ProtocolMiscBuilder::BuildSetForbidLteTac(int mode, int tac) {
    return protocolMiscBuilderInterface->BuildSetForbidLteTac(mode, tac);
}

ModemData *ProtocolMiscBuilder::BuildSetAllDivSetting() {
    return protocolMiscBuilderInterface->BuildSetAllDivSetting();
}

ModemData *ProtocolMiscBuilder::BuildSetBsrPara(int switchStatus, int packetPeriod, int packetSize) {
    return protocolMiscBuilderInterface->BuildSetBsrPara(switchStatus, packetPeriod, packetSize);
}

ModemData *ProtocolMiscBuilder::BuildSetAheadRre(int searchType, int earfcn, int globalCellId) {
    return protocolMiscBuilderInterface->BuildSetAheadRre(searchType, earfcn, globalCellId);
}

ModemData *ProtocolMiscBuilder::BuildSetDrdv(int flag) {
    return protocolMiscBuilderInterface->BuildSetDrdv(flag);
}

ModemData *ProtocolMiscBuilder::BuildSetLabTestConfig(int lab_test_config, BYTE value) {
    return protocolMiscBuilderInterface->BuildSetLabTestConfig(lab_test_config, value);
}

ModemData *ProtocolMiscBuilder::BuildSetForbidNrCell(int mode, ULONG *cellId, int forbiddenTimer, char *plmn) {
    return protocolMiscBuilderInterface->BuildSetForbidNrCell(mode, cellId, forbiddenTimer, plmn);
}

ModemData *ProtocolMiscBuilder::BuildGetCdmaCapabilityInfo() {
    return protocolMiscBuilderInterface->BuildGetCdmaCapabilityInfo();
}

ModemData *ProtocolMiscBuilder::BuildSetDataThrottling(int dataThrottlingAction, int64_t completionDurationMillis) {
    return protocolMiscBuilderInterface->BuildSetDataThrottling(dataThrottlingAction, completionDurationMillis);
}

ModemData *ProtocolMiscBuilder::BuildSetSrsPwrLimit(int level) {
    return protocolMiscBuilderInterface->BuildSetSrsPwrLimit(level);
}

ModemData *ProtocolMiscBuilder::BuildSetTraceDumpCfg(BYTE config) {
    return protocolMiscBuilderInterface->BuildSetTraceDumpCfg(config);
}

ModemData *ProtocolMiscBuilder::BuildGetTraceDumpCfg() {
    return protocolMiscBuilderInterface->BuildGetTraceDumpCfg();
}

ModemData *ProtocolMiscBuilder::BuildSetUeOperationMode(int mode) {
    return protocolMiscBuilderInterface->BuildSetUeOperationMode(mode);
}

ModemData *ProtocolMiscBuilder::BuildGetUeOperationMode() {
    return protocolMiscBuilderInterface->BuildGetUeOperationMode();
}

ModemData *ProtocolMiscBuilder::BuildSetNrrrmMinReqEnable(int enable) {
    return protocolMiscBuilderInterface->BuildSetNrrrmMinReqEnable(enable);
}

ModemData *ProtocolMiscBuilder::BuildGetNrrrmMinReqEnable() {
    return protocolMiscBuilderInterface->BuildGetNrrrmMinReqEnable();
}

ModemData *ProtocolMiscBuilder::BuildSetPwrDetectThr(int txPwrDetectThr, int txPwrDifferenceThr) {
    return protocolMiscBuilderInterface->BuildSetPwrDetectThr(txPwrDetectThr, txPwrDifferenceThr);
}

ModemData *ProtocolMiscBuilder::BuildSetEnableN28(int enable) {
    return protocolMiscBuilderInterface->BuildSetEnableN28(enable);
}

ModemData *ProtocolMiscBuilder::BuildSetUlMimo(int disable) {
    return protocolMiscBuilderInterface->BuildSetUlMimo(disable);
}

ModemData *ProtocolMiscBuilder::BuildGetBinaryVersionInfo() {
    return protocolMiscBuilderInterface->BuildGetBinaryVersionInfo();
}

ModemData *ProtocolMiscBuilder::BuildRadioConfigReset(int type) {
    return protocolMiscBuilderInterface->BuildRadioConfigReset(type);
}

ModemData *ProtocolMiscBuilder::BuildSetVcrt(int vcrtMode) {
    return protocolMiscBuilderInterface->BuildSetVcrt(vcrtMode);
}

ModemData *ProtocolMiscBuilder::BuildGetVcrt() {
    return protocolMiscBuilderInterface->BuildGetVcrt();
}

ModemData *ProtocolMiscBuilder::BuildSetNrDisableBand(void *data, unsigned int datalen) {
    return protocolMiscBuilderInterface->BuildSetNrDisableBand(data, datalen);
}

/**
 * ProtocolDeviceInfoBuilder
 */
ProtocolDeviceInfoBuilder::ProtocolDeviceInfoBuilder()
    : ProtocolDeviceInfoBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolDeviceInfoBuilderInterface = new ProtocolDeviceInfoBuilderJson();
    else
        protocolDeviceInfoBuilderInterface = new ProtocolDeviceInfoBuilderLegacy();
}

ProtocolDeviceInfoBuilder::~ProtocolDeviceInfoBuilder() {
    delete protocolDeviceInfoBuilderInterface;
    protocolDeviceInfoBuilderInterface = NULL;
}

ModemData *ProtocolDeviceInfoBuilder::GetBaseBandVersion(BYTE mask /* = 0xFF*/) {
    return protocolDeviceInfoBuilderInterface->GetBaseBandVersion(mask /* = 0xFF*/);
}

ModemData *ProtocolDeviceInfoBuilder::GetIMEI() {
    return protocolDeviceInfoBuilderInterface->GetIMEI();
}

ModemData *ProtocolDeviceInfoBuilder::GetIMEISV() {
    return protocolDeviceInfoBuilderInterface->GetIMEISV();
}

ModemData *ProtocolDeviceInfoBuilder::GetDevID() {
    return protocolDeviceInfoBuilderInterface->GetDevID();
}

ModemData *ProtocolDeviceInfoBuilder::SetScreenState(int state) {
    return protocolDeviceInfoBuilderInterface->SetScreenState(state);
}

ModemData *ProtocolDeviceInfoBuilder::SetPinControl(BYTE signal, BYTE status) {
    return protocolDeviceInfoBuilderInterface->SetPinControl(signal, status);
}

ModemData *ProtocolDeviceInfoBuilder::SendSGCValue(const int TargetOp, const int Rsv1, const int Rsv2) {
    return protocolDeviceInfoBuilderInterface->SendSGCValue(TargetOp, Rsv1, Rsv2);
}

ModemData *ProtocolDeviceInfoBuilder::BuildGetHwConfig() {
    return protocolDeviceInfoBuilderInterface->BuildGetHwConfig();
}

ModemData *ProtocolDeviceInfoBuilder::SendDeviceInfo(const char *model, const char *swVer, const char *productName, const char *OsVer) {
    return protocolDeviceInfoBuilderInterface->SendDeviceInfo(model, swVer, productName, OsVer);
}

ModemData *ProtocolDeviceInfoBuilder::BuildSetFeatureInfo(RilProperty &bundle) {
    return protocolDeviceInfoBuilderInterface->BuildSetFeatureInfo(bundle);
}

ModemData *ProtocolDeviceInfoBuilder::GetSignalStrength() {
    return protocolDeviceInfoBuilderInterface->GetSignalStrength();
}

ModemData *ProtocolDeviceInfoBuilder::BuildLceStart(int lceMode, int interval) {
    return protocolDeviceInfoBuilderInterface->BuildLceStart(lceMode, interval);
}

ModemData *ProtocolDeviceInfoBuilder::BuildLceStop() {
    return protocolDeviceInfoBuilderInterface->BuildLceStop();
}

ModemData *ProtocolDeviceInfoBuilder::BuildLcePullLceData() {
    return protocolDeviceInfoBuilderInterface->BuildLcePullLceData();
}

ModemData *ProtocolDeviceInfoBuilder::BuildSetUnsolicitedResponseFilter(unsigned int bitMask) {
    return protocolDeviceInfoBuilderInterface->BuildSetUnsolicitedResponseFilter(bitMask);
}

ModemData *ProtocolDeviceInfoBuilder::BuildSetCarrierInfoImsiEncryption(char *pMcc, char *pMnc, int keyLen, BYTE *pKey, int keyIdLen, char *pKeyId, LONG expTime, int keyType) {
    return protocolDeviceInfoBuilderInterface->BuildSetCarrierInfoImsiEncryption(pMcc, pMnc, keyLen, pKey, keyIdLen, pKeyId, expTime, keyType);
}

ModemData *ProtocolDeviceInfoBuilder::BuildSetOpenCarierInfo(unsigned int opc, const char *plmn) {
    return protocolDeviceInfoBuilderInterface->BuildSetOpenCarierInfo(opc, plmn);
}

ModemData *ProtocolDeviceInfoBuilder::BuildSetSignalReportCriteria(int ms, int db, int len, int *dbm, int accessNetwork, int measureType, bool isEnable) {
    return protocolDeviceInfoBuilderInterface->BuildSetSignalReportCriteria(ms, db, len, dbm, accessNetwork, measureType, isEnable);
}

ModemData *ProtocolDeviceInfoBuilder::BuildSetLinkCapaReportCriteria(int hMs, int hDlKbps, int hUlKbps, int tDlLen, int *tDlKbps, int tUlLen, int *tUlKbps, int accessNetwork) {
    return protocolDeviceInfoBuilderInterface->BuildSetLinkCapaReportCriteria(hMs, hDlKbps, hUlKbps, tDlLen, tDlKbps, tUlLen, tUlKbps, accessNetwork);
}

/**
 * ProtocolMiscDebugBuilder
 */
ProtocolMiscDebugBuilder::ProtocolMiscDebugBuilder()
    : ProtocolMiscDebugBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolMiscDebugBuilderInterface = new ProtocolMiscDebugBuilderJson();
    else
        protocolMiscDebugBuilderInterface = new ProtocolMiscDebugBuilderLegacy();
}

ProtocolMiscDebugBuilder::~ProtocolMiscDebugBuilder() {
    delete protocolMiscDebugBuilderInterface;
    protocolMiscDebugBuilderInterface = NULL;
}

ModemData *ProtocolMiscDebugBuilder::SetEngMode(BYTE mode) {
    return protocolMiscDebugBuilderInterface->SetEngMode(mode);
}

ModemData *ProtocolMiscDebugBuilder::SetEngMode(BYTE mode, BYTE sub_mode) {
    return protocolMiscDebugBuilderInterface->SetEngMode(mode, sub_mode);
}

ModemData *ProtocolMiscDebugBuilder::SetScrLine(BYTE lineno) {
    return protocolMiscDebugBuilderInterface->SetScrLine(lineno);
}

ModemData *ProtocolMiscDebugBuilder::SetEngStringInput(BYTE len, char *input) {
    return protocolMiscDebugBuilderInterface->SetEngStringInput(len, input);
}

ModemData *ProtocolMiscDebugBuilder::SetDebugTrace(BYTE value) {
    return protocolMiscDebugBuilderInterface->SetDebugTrace(value);
}

ModemData *ProtocolMiscDebugBuilder::BuildATCommand(const char *command) {
    return protocolMiscDebugBuilderInterface->BuildATCommand(command);
}

ModemData *ProtocolMiscDebugBuilder::BuildSetSelflog(int mode, int size) {
    return protocolMiscDebugBuilderInterface->BuildSetSelflog(mode, size);
}

ModemData *ProtocolMiscDebugBuilder::BuildGetSelflogStatus() {
    return protocolMiscDebugBuilderInterface->BuildGetSelflogStatus();
}

ModemData *ProtocolMiscDebugBuilder::BuildSetSelflogProfile() {
    return protocolMiscDebugBuilderInterface->BuildSetSelflogProfile();
}

ModemData *ProtocolMiscDebugBuilder::BuildSetSelflogTcpDumpMode(int tcpDumpEnable, int headerDumpEnable) {
    return protocolMiscDebugBuilderInterface->BuildSetSelflogTcpDumpMode(tcpDumpEnable, headerDumpEnable);
}

ModemData *ProtocolMiscDebugBuilder::BuildSetModemLogDump() {
    return protocolMiscDebugBuilderInterface->BuildSetModemLogDump();
}

ModemData *ProtocolMiscDebugBuilder::BuildSetApSystemTime() {
    return protocolMiscDebugBuilderInterface->BuildSetApSystemTime();
}

ModemData *ProtocolMiscDebugBuilder::StoreAdbSerialNumber(void *data, unsigned int datalen) {
    return protocolMiscDebugBuilderInterface->StoreAdbSerialNumber(data, datalen);
}

ModemData *ProtocolMiscDebugBuilder::ReadAdbSerialNumber() {
    return protocolMiscDebugBuilderInterface->ReadAdbSerialNumber();
}

ModemData *ProtocolMiscDebugBuilder::BuildSetBtlBufMaxSize(int size) {
    return protocolMiscDebugBuilderInterface->BuildSetBtlBufMaxSize(size);
}

/**
 * ProtocolSensorBuilder
 */
ProtocolSensorBuilder::ProtocolSensorBuilder()
    : ProtocolSensorBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolSensorBuilderInterface = new ProtocolSensorBuilderJson();
    else
        protocolSensorBuilderInterface = new ProtocolSensorBuilderLegacy();
}

ProtocolSensorBuilder::~ProtocolSensorBuilder() {
    delete protocolSensorBuilderInterface;
    protocolSensorBuilderInterface = NULL;
}

ModemData *ProtocolSensorBuilder::GetRfDesenseMode() {
    return protocolSensorBuilderInterface->GetRfDesenseMode();
}

ModemData *ProtocolSensorBuilder::SetRfDesenseMode(void *data, unsigned int datalen) {
    return protocolSensorBuilderInterface->SetRfDesenseMode(data, datalen);
}

ModemData *ProtocolSensorBuilder::BuildPSensorStatus(int pSensorStatus) {
    return protocolSensorBuilderInterface->BuildPSensorStatus(pSensorStatus);
}

ModemData *ProtocolSensorBuilder::BuildSetSarState(int sarState) {
    return protocolSensorBuilderInterface->BuildSetSarState(sarState);
}

ModemData *ProtocolSensorBuilder::BuildGetSarState() {
    return protocolSensorBuilderInterface->BuildGetSarState();
}

ModemData *ProtocolSensorBuilder::BuildScanRssi(int rat, int band, int rbw, int scanMode, int startFreq, int endFreq, int step, int antenna, int sampling,
                                                int tx1, int tx1Band, int tx1Bw, int tx1Freq, int tx1Power, int tx1RbNum, int tx1RbOffset, int tx1Mcs,
                                                int tx2, int tx2Band, int tx2Bw, int tx2Freq, int tx2Power, int tx2RbNum, int tx2RbOffset, int tx2Mcs) {
    return protocolSensorBuilderInterface->BuildScanRssi(rat, band, rbw, scanMode, startFreq, endFreq, step, antenna, sampling,
                                                         tx1, tx1Band, tx1Bw, tx1Freq, tx1Power, tx1RbNum, tx1RbOffset, tx1Mcs,
                                                         tx2, tx2Band, tx2Bw, tx2Freq, tx2Power, tx2RbNum, tx2RbOffset, tx2Mcs);
}

/**
 * ProtocolOemOmaDmBuilder
 */
ProtocolOemOmaDmBuilder::ProtocolOemOmaDmBuilder()
    : ProtocolOemOmaDmBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolOemOmaDmBuilderInterface = new ProtocolOemOmaDmBuilderJson();
    else
        protocolOemOmaDmBuilderInterface = new ProtocolOemOmaDmBuilderLegacy();
}

ProtocolOemOmaDmBuilder::~ProtocolOemOmaDmBuilder() {
    delete protocolOemOmaDmBuilderInterface;
    protocolOemOmaDmBuilderInterface = NULL;
}

ModemData *ProtocolOemOmaDmBuilder::BuildGetRadioNode(const char *path) {
    return protocolOemOmaDmBuilderInterface->BuildGetRadioNode(path);
}

ModemData *ProtocolOemOmaDmBuilder::BuildSetRadioNode(const char *path, const char *value) {
    return protocolOemOmaDmBuilderInterface->BuildSetRadioNode(path, value);
}

ModemData *ProtocolOemOmaDmBuilder::BuildGetVoLteProvisionUpdate() {
    return protocolOemOmaDmBuilderInterface->BuildGetVoLteProvisionUpdate();
}

ModemData *ProtocolOemOmaDmBuilder::BuildSetVoLteProvisionUpdate() {
    return protocolOemOmaDmBuilderInterface->BuildSetVoLteProvisionUpdate();
}

ModemData *ProtocolOemOmaDmBuilder::GetMslCode() {
    return protocolOemOmaDmBuilderInterface->GetMslCode();
}

/**
 * ProtocolDtmfBuilder
 */
ProtocolDtmfBuilder::ProtocolDtmfBuilder()
    : ProtocolDtmfBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolDtmfBuilderInterface = new ProtocolDtmfBuilderJson();
    else
        protocolDtmfBuilderInterface = new ProtocolDtmfBuilderLegacy();
}
ProtocolDtmfBuilder::~ProtocolDtmfBuilder() {
    delete protocolDtmfBuilderInterface;
    protocolDtmfBuilderInterface = NULL;
}
ModemData *ProtocolDtmfBuilder::BuildDtmf(int dtmf_length, char *dtmf) {
    return protocolDtmfBuilderInterface->BuildDtmf(dtmf_length, dtmf);
}

ModemData *ProtocolDtmfBuilder::BuildDtmfStart(bool tone_type, BYTE digit) {
    return protocolDtmfBuilderInterface->BuildDtmfStart(tone_type, digit);
}

ModemData *ProtocolDtmfBuilder::BuildDtmfStop() {
    return protocolDtmfBuilderInterface->BuildDtmfStop();
}

/**
 * ProtocolMiscNetworkBuilder
 */
ProtocolMiscNetworkBuilder::ProtocolMiscNetworkBuilder()
    : ProtocolMiscNetworkBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolMiscNetworkBuilderInterface = new ProtocolMiscNetworkBuilderJson();
    else
        protocolMiscNetworkBuilderInterface = new ProtocolMiscNetworkBuilderLegacy();
}

ProtocolMiscNetworkBuilder::~ProtocolMiscNetworkBuilder() {
    delete protocolMiscNetworkBuilderInterface;
    protocolMiscNetworkBuilderInterface = NULL;
}

ModemData *ProtocolMiscNetworkBuilder::GetManualBandMode() {
    return protocolMiscNetworkBuilderInterface->GetManualBandMode();
}

ModemData *ProtocolMiscNetworkBuilder::SetManualBandMode(void *data, unsigned int datalen) {
    return protocolMiscNetworkBuilderInterface->SetManualBandMode(data, datalen);
}

ModemData *ProtocolMiscNetworkBuilder::BuildCdmaSubscription() {
    return protocolMiscNetworkBuilderInterface->BuildCdmaSubscription();
}

ModemData *ProtocolMiscNetworkBuilder::BuildSetVoiceOperation(int mode) {
    return protocolMiscNetworkBuilderInterface->BuildSetVoiceOperation(mode);
}

ModemData *ProtocolMiscNetworkBuilder::BuildGetVoiceOperation() {
    return protocolMiscNetworkBuilderInterface->BuildGetVoiceOperation();
}

ModemData *ProtocolMiscNetworkBuilder::BuildSetPreferredCallCapability(int mode) {
    return protocolMiscNetworkBuilderInterface->BuildSetPreferredCallCapability(mode);
}

ModemData *ProtocolMiscNetworkBuilder::BuildGetPreferredCallCapability() {
    return protocolMiscNetworkBuilderInterface->BuildGetPreferredCallCapability();
}

ModemData *ProtocolMiscNetworkBuilder::BuildSetStatckStatus(int mode) {
    return protocolMiscNetworkBuilderInterface->BuildSetStatckStatus(mode);
}

ModemData *ProtocolMiscNetworkBuilder::BuildGetStatckStatus() {
    return protocolMiscNetworkBuilderInterface->BuildGetStatckStatus();
}

/**
 * ProtocolCarrierConfigBuilder
 */
ProtocolCarrierConfigBuilder::ProtocolCarrierConfigBuilder()
    : ProtocolCarrierConfigBuilderInterface() {
    protocolCarrierConfigBuilderInterface = new ProtocolCarrierConfigBuilderLegacy();
}

ProtocolCarrierConfigBuilder::~ProtocolCarrierConfigBuilder() {
    delete protocolCarrierConfigBuilderInterface;
    protocolCarrierConfigBuilderInterface = NULL;
}

ModemData *ProtocolCarrierConfigBuilder::BuildSetCpCarrierConfig(const char *configDir, char *encodedManifest, int encodedManifestLen) {
    return protocolCarrierConfigBuilderInterface->BuildSetCpCarrierConfig(configDir, encodedManifest, encodedManifestLen);
}
