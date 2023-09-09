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
 * protocolmiscbuilderinterface.h
 *
 *  Created on: 2021. 06. 17.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_MISC_BUILDER_INTERFACE_H__
#define __PROTOCOL_MISC_BUILDER_INTERFACE_H__

#include <base/types.h>

class ModemData;
class RilProperty;

/**
 * ProtocolMiscBuilderInterface
 */
class ProtocolMiscBuilderInterface {
public:
    ProtocolMiscBuilderInterface() = default;
    virtual ~ProtocolMiscBuilderInterface() = default;
public:
    virtual ModemData *GetTtyMode() = 0;
    virtual ModemData *SetTtyMode(INT32 mode) = 0;
    virtual ModemData *BuildNvReadItem(int nvItemId) = 0;
    virtual ModemData *BuildNvWriteItem(int nvItemId, const char *value) = 0;
    virtual ModemData *GetModemActivityInfo() = 0;
    virtual ModemData *BuildSetSuppSvcNotification(int enable) = 0;
    virtual ModemData *BuildModemInfo(int type, char *data, unsigned int size) = 0;
    virtual ModemData *BuildSwitchModemFunction(int feature, BYTE enable) = 0;
    virtual ModemData *BuildSetPdcpDiscardTimer(int discardTimer) = 0;
    virtual ModemData *BuildSetActivateVsim(int slot, int iccidLen, const char *pIccid,
            int imsiLen, const char *pImsi, const char *pHplmn, int vsimState, int vsimCardType) = 0;
    virtual ModemData *BuildGetCqiInfo() = 0;
    virtual ModemData *BuildSetSarSetting(int dsi) = 0;
    virtual ModemData *BuildSetImsTestMode(int mode) = 0;
    virtual ModemData *BuildSetGmoSwitch(int feature) = 0;
    virtual ModemData *BuildSetTcsFci(int state, int len, char *fci) = 0;
    virtual ModemData *BuildGetTcsFci() = 0;
    virtual ModemData *BuildSetCABandwidthFilter(int enable) = 0;
    virtual ModemData *BuildSetElevatorSensor(int enable) = 0;
    virtual ModemData *BuildSetLocationUpdates(int enable) = 0;
    virtual ModemData *BuildSetForbidLteCell(int mode, int cellId, int forbiddenTimer, char *plmn) = 0;
    virtual ModemData *BuildGetModemActivityInfo() = 0;
    virtual ModemData *BuildOemSetSvn(char *svn) = 0;
    virtual ModemData *BuildSetForbidFrequency(int mode, int cellId, int forbiddenTimer) = 0;
    virtual ModemData *BuildSetForbidLetBand(int numBands, int *band) = 0;
    virtual ModemData *BuildSetForbidLteTac(int mode, int tac) = 0;
    virtual ModemData *BuildSetAllDivSetting() = 0;
    virtual ModemData *BuildSetBsrPara(int switchStatus, int packetPeriod, int packetSize) = 0;
    virtual ModemData *BuildSetAheadRre(int searchType, int earfcn, int gobalCellId) = 0;
    virtual ModemData *BuildSetDrdv(int flag) = 0;
    virtual ModemData *BuildSetLabTestConfig(int lab_test_config, BYTE value) = 0;
    virtual ModemData *BuildSetForbidNrCell(int mode, ULONG *cellId, int forbiddenTimer, char *plmn) = 0;
    virtual ModemData *BuildGetCdmaCapabilityInfo() = 0;
    virtual ModemData *BuildSetDataThrottling(int dataThrottlingAction, int64_t completionDurationMillis) = 0;
    virtual ModemData *BuildSetSrsPwrLimit(int level) = 0;
    virtual ModemData *BuildSetTraceDumpCfg(BYTE config) = 0;
    virtual ModemData *BuildGetTraceDumpCfg() = 0;
    virtual ModemData *BuildSetUeOperationMode(int mode) = 0;
    virtual ModemData *BuildGetUeOperationMode() = 0;
    virtual ModemData *BuildSetNrrrmMinReqEnable(int enable) = 0;
    virtual ModemData *BuildGetNrrrmMinReqEnable() = 0;
    virtual ModemData *BuildSetPwrDetectThr(int txPwrDetectThr, int txPwrDifferenceThr) = 0;
    virtual ModemData *BuildSetEnableN28(int enable) = 0;
    virtual ModemData *BuildSetUlMimo(int disable) = 0;
    virtual ModemData *BuildGetBinaryVersionInfo() = 0;
    virtual ModemData *BuildRadioConfigReset(int type) = 0;
    virtual ModemData *BuildSetVcrt(int vcrtMode) = 0;
    virtual ModemData *BuildGetVcrt() = 0;
    virtual ModemData *BuildSetNrDisableBand(void *data, unsigned int datalen) = 0;
};

/**
 * ProtocolDeviceInfoBuilderInterface
 */
class ProtocolDeviceInfoBuilderInterface {
public:
    ProtocolDeviceInfoBuilderInterface() = default;
    virtual ~ProtocolDeviceInfoBuilderInterface() = default;

    virtual ModemData *GetBaseBandVersion(BYTE mask = 0xFF) = 0;
    virtual ModemData *GetIMEI() = 0;
    virtual ModemData *GetIMEISV() = 0;
    virtual ModemData *GetDevID() = 0;
    virtual ModemData *SetScreenState(INT32 state) = 0;
    virtual ModemData *SetPinControl(BYTE signal, BYTE status) = 0;
    virtual ModemData *SendSGCValue(const int TargetOp, const int Rvs1, const int Rvs2) = 0;
    virtual ModemData *BuildGetHwConfig() = 0;
    virtual ModemData *SendDeviceInfo(const char* model, const char* swVer, const char* productName, const char* OsVer) = 0;
    virtual ModemData *BuildSetFeatureInfo(RilProperty &bundle) = 0;
    virtual ModemData *GetSignalStrength() = 0;
    virtual ModemData *BuildLceStart(int lceMode, int interval) = 0;
    virtual ModemData *BuildLceStop() = 0;
    virtual ModemData *BuildLcePullLceData() = 0;
    virtual ModemData *BuildSetUnsolicitedResponseFilter(unsigned int bitMask) = 0;
    virtual ModemData *BuildSetCarrierInfoImsiEncryption(char *pMcc, char *pMnc,
            int keyLen, BYTE *pKey, int keyIdLen, char *pKeyId, LONG expTime, int keyType) = 0;
    virtual ModemData* BuildSetOpenCarierInfo(unsigned int opc,
            const char *plmn) = 0;
    virtual ModemData* BuildSetSignalReportCriteria(int ms, int db, int len,
            int *dbm, int accessNetwork, int measureType, bool isEnable) = 0;
    virtual ModemData* BuildSetLinkCapaReportCriteria(int hMs, int hDlKbps,
            int hUlKbps, int tDlLen, int *tDlKbps, int tUlLen, int *tUlKbps,
            int accessNetwork) = 0;
};

/**
 * ProtocolMiscDebugBuilderInterface
 */
class ProtocolMiscDebugBuilderInterface {
public:
    ProtocolMiscDebugBuilderInterface() = default;
    virtual ~ProtocolMiscDebugBuilderInterface() = default;

    virtual ModemData *SetEngMode(BYTE mode) = 0;
    virtual ModemData *SetEngMode(BYTE mode, BYTE sub_mode) = 0;
    virtual ModemData *SetScrLine(BYTE lineno) = 0;
    virtual ModemData *SetEngStringInput(BYTE len, char* input) = 0;
    virtual ModemData *SetDebugTrace(BYTE value) = 0;
    virtual ModemData *BuildATCommand(const char *command) = 0;
    virtual ModemData *BuildSetSelflog(int mode, int size) = 0;
    virtual ModemData *BuildGetSelflogStatus() = 0;
    virtual ModemData *BuildSetSelflogProfile() = 0;
    virtual ModemData *BuildSetSelflogTcpDumpMode(int tcpDumpEnable, int headerDumpEnable) = 0;
    virtual ModemData *BuildSetModemLogDump() = 0;
    virtual ModemData *BuildSetApSystemTime() = 0;
    virtual ModemData *StoreAdbSerialNumber(void *data, unsigned int datalen) = 0;
    virtual ModemData *ReadAdbSerialNumber() = 0;
    virtual ModemData *BuildSetBtlBufMaxSize(int size) = 0;
};

/**
 * ProtocolSensorBuilderInterface
 */
class ProtocolSensorBuilderInterface {
public:
    ProtocolSensorBuilderInterface() = default;
    virtual ~ProtocolSensorBuilderInterface() = default;

    virtual ModemData *GetRfDesenseMode() = 0;
    virtual ModemData *SetRfDesenseMode(void *data, unsigned int datalen) = 0;
    virtual ModemData *BuildPSensorStatus(int pSensorStatus) = 0;
    virtual ModemData *BuildSetSarState(int sarState) = 0;
    virtual ModemData *BuildGetSarState() = 0;
    virtual ModemData* BuildScanRssi(int rat, int band, int rbw, int scanMode,
            int startFreq, int endFreq, int step, int antenna, int sampling,
            int tx1, int tx1Band, int tx1Bw, int tx1Freq, int tx1Power,
            int tx1RbNum, int tx1RbOffset, int tx1Mcs, int tx2, int tx2Band,
            int tx2Bw, int tx2Freq, int tx2Power, int tx2RbNum, int tx2RbOffset,
            int tx2Mcs) = 0;
};

/**
 * ProtocolOemOmaDmBuilderInterface
 */
class ProtocolOemOmaDmBuilderInterface {
public:
    ProtocolOemOmaDmBuilderInterface() = default;
    virtual ~ProtocolOemOmaDmBuilderInterface() = default;

    virtual ModemData *BuildGetRadioNode(const char *path) = 0;
    virtual ModemData *BuildSetRadioNode(const char *path, const char *value) = 0;
    virtual ModemData *BuildGetVoLteProvisionUpdate() = 0;
    virtual ModemData *BuildSetVoLteProvisionUpdate() = 0;
    virtual ModemData *GetMslCode() = 0;
};

/**
 * ProtocolDtmfBuilderInterface
 */
class ProtocolDtmfBuilderInterface {
public:
    ProtocolDtmfBuilderInterface() = default;
    virtual ~ProtocolDtmfBuilderInterface() = default;

    virtual ModemData *BuildDtmfStart(bool tone_type, BYTE digit) = 0;
    virtual ModemData *BuildDtmf(int dtmf_length, char* dtmf) = 0;
    virtual ModemData *BuildDtmfStop() = 0;
};

/**
 * ProtocolMiscNetworkBuilderInterface
 */
class ProtocolMiscNetworkBuilderInterface {
public:
    ProtocolMiscNetworkBuilderInterface() = default;
    virtual ~ProtocolMiscNetworkBuilderInterface() = default;

    virtual ModemData *GetManualBandMode() = 0;
    virtual ModemData *SetManualBandMode(void *data, unsigned int datalen) = 0;
    virtual ModemData *BuildCdmaSubscription() = 0;
    virtual ModemData *BuildSetVoiceOperation(int mode) = 0;
    virtual ModemData *BuildGetVoiceOperation() = 0;
    virtual ModemData *BuildSetPreferredCallCapability(int mode) = 0;
    virtual ModemData *BuildGetPreferredCallCapability() = 0;
    virtual ModemData *BuildSetStatckStatus(int mode) = 0;
    virtual ModemData *BuildGetStatckStatus() = 0;
};

/**
 * ProtocolCarrierConfigBuilderInterface
 */
class ProtocolCarrierConfigBuilderInterface {
public:
    ProtocolCarrierConfigBuilderInterface() = default;
    virtual ~ProtocolCarrierConfigBuilderInterface() = default;

    virtual ModemData *BuildSetCpCarrierConfig(const char *configDir, char *encodedManifest, int encodedManifestLen) = 0;
};

#endif /* __PROTOCOL_MISC_BUILDER_INTERFACE_H__ */
