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
 * protocolmiscbuilder.h
 *
 *  Created on: 2021. 06. 17.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_MISC_BUILDER_H__
#define __PROTOCOL_MISC_BUILDER_H__

#include "protocolmiscbuilderinterface.h"

class ProtocolMiscBuilderJson;
class ProtocolMiscBuilderLegacy;

/**
 * ProtocolMiscBuilder
 */
class ProtocolMiscBuilder : public ProtocolMiscBuilderInterface
{
private:
    ProtocolMiscBuilderInterface *protocolMiscBuilderInterface = NULL;

public:
    ProtocolMiscBuilder();
    ProtocolMiscBuilder(const ProtocolMiscBuilder &) = delete;
    ProtocolMiscBuilder& operator=(ProtocolMiscBuilder const&) = delete;
    virtual ~ProtocolMiscBuilder();
public:
    ModemData *GetTtyMode();
    ModemData *SetTtyMode(INT32 mode);
    ModemData *BuildNvReadItem(int nvItemId);
    ModemData *BuildNvWriteItem(int nvItemId, const char *value);
    ModemData *GetModemActivityInfo();
    ModemData *BuildSetSuppSvcNotification(int enable);
    ModemData *BuildModemInfo(int type, char *data, unsigned int size);
    ModemData *BuildSwitchModemFunction(int feature, BYTE enable);
    ModemData *BuildSetPdcpDiscardTimer(int discardTimer);
    ModemData *BuildSetActivateVsim(int slot, int iccidLen, const char *pIccid,
            int imsiLen, const char *pImsi, const char *pHplmn, int vsimState, int vsimCardType);
    ModemData *BuildGetCqiInfo();
    ModemData *BuildSetSarSetting(int dsi);
    ModemData *BuildSetImsTestMode(int mode);
    ModemData *BuildSetGmoSwitch(int feature);
    ModemData *BuildSetTcsFci(int state, int len, char *fci);
    ModemData *BuildGetTcsFci();
    ModemData *BuildSetCABandwidthFilter(int enable);
    ModemData *BuildSetElevatorSensor(int enable);
    ModemData *BuildSetLocationUpdates(int enable);
    ModemData *BuildSetForbidLteCell(int mode, int cellId, int forbiddenTimer, char *plmn);
    ModemData *BuildGetModemActivityInfo();
    ModemData *BuildOemSetSvn(char *svn);
    ModemData *BuildSetForbidFrequency(int mode, int cellId, int forbiddenTimer);
    ModemData *BuildSetForbidLetBand(int numBands, int *band);
    ModemData *BuildSetForbidLteTac(int mode, int tac);
    ModemData *BuildSetAllDivSetting();
    ModemData *BuildSetBsrPara(int switchStatus, int packetPeriod, int packetSize);
    ModemData *BuildSetAheadRre(int searchType, int earfcn, int gobalCellId);
    ModemData *BuildSetDrdv(int flag);
    ModemData *BuildSetLabTestConfig(int lab_test_config, BYTE value);
    ModemData *BuildSetForbidNrCell(int mode, ULONG *cellId, int forbiddenTimer, char *plmn);
    ModemData *BuildGetCdmaCapabilityInfo();
    ModemData *BuildSetDataThrottling(int dataThrottlingAction, int64_t completionDurationMillis);
    ModemData *BuildSetSrsPwrLimit(int level);
    ModemData *BuildSetTraceDumpCfg(BYTE config);
    ModemData *BuildGetTraceDumpCfg();
    ModemData *BuildSetUeOperationMode(int mode);
    ModemData *BuildGetUeOperationMode();
    ModemData *BuildSetNrrrmMinReqEnable(int enable);
    ModemData *BuildGetNrrrmMinReqEnable();
    ModemData *BuildSetPwrDetectThr(int txPwrDetectThr, int txPwrDifferenceThr);
    ModemData *BuildSetEnableN28(int enable);
    ModemData *BuildSetUlMimo(int disable);
    ModemData *BuildGetBinaryVersionInfo();
    ModemData *BuildRadioConfigReset(int type);
    ModemData *BuildSetVcrt(int vcrtMode);
    ModemData *BuildGetVcrt();
    ModemData *BuildSetNrDisableBand(void *data, unsigned int datalen);
};

/**
 * ProtocolDeviceInfoBuilder
 */
class ProtocolDeviceInfoBuilder : public ProtocolDeviceInfoBuilderInterface {
private:
    ProtocolDeviceInfoBuilderInterface *protocolDeviceInfoBuilderInterface = NULL;

public:
    ProtocolDeviceInfoBuilder();
    ProtocolDeviceInfoBuilder(const ProtocolDeviceInfoBuilder &) = delete;
    ProtocolDeviceInfoBuilder& operator=(ProtocolDeviceInfoBuilder const&) = delete;
    virtual ~ProtocolDeviceInfoBuilder();

    ModemData *GetBaseBandVersion(BYTE mask = 0xFF);
    ModemData *GetIMEI();
    ModemData *GetIMEISV();
    ModemData *GetDevID();
    ModemData *SetScreenState(INT32 state);
    ModemData *SetPinControl(BYTE signal, BYTE status);
    ModemData *SendSGCValue(const int TargetOp, const int Rvs1, const int Rvs2);
    ModemData *BuildGetHwConfig();
    ModemData *SendDeviceInfo(const char* model, const char* swVer, const char* productName, const char* OsVer);
    ModemData *BuildSetFeatureInfo(RilProperty &bundle);
    ModemData *GetSignalStrength();
    ModemData *BuildLceStart(int lceMode, int interval);
    ModemData *BuildLceStop();
    ModemData *BuildLcePullLceData();
    ModemData *BuildSetUnsolicitedResponseFilter(unsigned int bitMask);
    ModemData *BuildSetCarrierInfoImsiEncryption(char *pMcc, char *pMnc,
            int keyLen, BYTE *pKey, int keyIdLen, char *pKeyId, LONG expTime, int keyType);
    ModemData* BuildSetOpenCarierInfo(unsigned int opc,
            const char *plmn);
    ModemData* BuildSetSignalReportCriteria(int ms, int db, int len,
            int *dbm, int accessNetwork, int measureType, bool isEnable);
    ModemData* BuildSetLinkCapaReportCriteria(int hMs, int hDlKbps,
            int hUlKbps, int tDlLen, int *tDlKbps, int tUlLen, int *tUlKbps,
            int accessNetwork);
};

/**
 * ProtocolMiscDebugBuilder
 */
class ProtocolMiscDebugBuilder : public ProtocolMiscDebugBuilderInterface {
private:
    ProtocolMiscDebugBuilderInterface *protocolMiscDebugBuilderInterface = NULL;

public:
    ProtocolMiscDebugBuilder();
    ProtocolMiscDebugBuilder(const ProtocolMiscDebugBuilder &) = delete;
    ProtocolMiscDebugBuilder& operator=(ProtocolMiscDebugBuilder const&) = delete;
    virtual ~ProtocolMiscDebugBuilder();

    ModemData *SetEngMode(BYTE mode);
    ModemData *SetEngMode(BYTE mode, BYTE sub_mode);
    ModemData *SetScrLine(BYTE lineno);
    ModemData *SetEngStringInput(BYTE len, char* input);
    ModemData *SetDebugTrace(BYTE value);
    ModemData *BuildATCommand(const char *command);
    ModemData *BuildSetSelflog(int mode, int size);
    ModemData *BuildGetSelflogStatus();
    ModemData *BuildSetSelflogProfile();
    ModemData *BuildSetSelflogTcpDumpMode(int tcpDumpEnable, int headerDumpEnable);
    ModemData *BuildSetModemLogDump();
    ModemData *BuildSetApSystemTime();
    ModemData *StoreAdbSerialNumber(void *data, unsigned int datalen);
    ModemData *ReadAdbSerialNumber();
    ModemData *BuildSetBtlBufMaxSize(int size);
};

/**
 * ProtocolSensorBuilder
 */
class ProtocolSensorBuilder : public ProtocolSensorBuilderInterface {
private:
    ProtocolSensorBuilderInterface *protocolSensorBuilderInterface = NULL;

public:
    ProtocolSensorBuilder();
    ProtocolSensorBuilder(const ProtocolSensorBuilder &) = delete;
    ProtocolSensorBuilder& operator=(ProtocolSensorBuilder const&) = delete;
    virtual ~ProtocolSensorBuilder();

    ModemData *GetRfDesenseMode();
    ModemData *SetRfDesenseMode(void *data, unsigned int datalen);
    ModemData *BuildPSensorStatus(int pSensorStatus);
    ModemData *BuildSetSarState(int sarState);
    ModemData *BuildGetSarState();
    ModemData* BuildScanRssi(int rat, int band, int rbw, int scanMode,
            int startFreq, int endFreq, int step, int antenna, int sampling,
            int tx1, int tx1Band, int tx1Bw, int tx1Freq, int tx1Power,
            int tx1RbNum, int tx1RbOffset, int tx1Mcs, int tx2, int tx2Band,
            int tx2Bw, int tx2Freq, int tx2Power, int tx2RbNum, int tx2RbOffset,
            int tx2Mcs);
};

/**
 * ProtocolOemOmaDmBuilder
 */
class ProtocolOemOmaDmBuilder : public ProtocolOemOmaDmBuilderInterface {
private:
    ProtocolOemOmaDmBuilderInterface *protocolOemOmaDmBuilderInterface = NULL;

public:
    ProtocolOemOmaDmBuilder();
    ProtocolOemOmaDmBuilder(const ProtocolOemOmaDmBuilder &) = delete;
    ProtocolOemOmaDmBuilder& operator=(ProtocolOemOmaDmBuilder const&) = delete;
    virtual ~ProtocolOemOmaDmBuilder();

    ModemData *BuildGetRadioNode(const char *path);
    ModemData *BuildSetRadioNode(const char *path, const char *value);
    ModemData *BuildGetVoLteProvisionUpdate();
    ModemData *BuildSetVoLteProvisionUpdate();
    ModemData *GetMslCode();
};

/**
 * ProtocolDtmfBuilder
 */
class ProtocolDtmfBuilder : public ProtocolDtmfBuilderInterface {
private:
    ProtocolDtmfBuilderInterface *protocolDtmfBuilderInterface = NULL;

public:
    ProtocolDtmfBuilder();
    ProtocolDtmfBuilder(const ProtocolDtmfBuilder &) = delete;
    ProtocolDtmfBuilder& operator=(ProtocolDtmfBuilder const&) = delete;
    virtual ~ProtocolDtmfBuilder();

    ModemData *BuildDtmfStart(bool tone_type, BYTE digit);
    ModemData *BuildDtmf(int dtmf_length, char* dtmf);
    ModemData *BuildDtmfStop();
};

/**
 * ProtocolMiscNetworkBuilder
 */
class ProtocolMiscNetworkBuilder : public ProtocolMiscNetworkBuilderInterface {
private:
    ProtocolMiscNetworkBuilderInterface *protocolMiscNetworkBuilderInterface = NULL;

public:
    ProtocolMiscNetworkBuilder();
    ProtocolMiscNetworkBuilder(const ProtocolMiscNetworkBuilder &) = delete;
    ProtocolMiscNetworkBuilder& operator=(ProtocolMiscNetworkBuilder const&) = delete;
    virtual ~ProtocolMiscNetworkBuilder();

    ModemData *GetManualBandMode();
    ModemData *SetManualBandMode(void *data, unsigned int datalen);
    ModemData *BuildCdmaSubscription();
    ModemData *BuildSetVoiceOperation(int mode);
    ModemData *BuildGetVoiceOperation();
    ModemData *BuildSetPreferredCallCapability(int mode);
    ModemData *BuildGetPreferredCallCapability();
    ModemData *BuildSetStatckStatus(int mode);
    ModemData *BuildGetStatckStatus();
};

/**
 * ProtocolCarrierConfigBuilder
 */
class ProtocolCarrierConfigBuilder : public ProtocolCarrierConfigBuilderInterface {
private:
    ProtocolCarrierConfigBuilderInterface *protocolCarrierConfigBuilderInterface = NULL;

public:
    ProtocolCarrierConfigBuilder();
    ProtocolCarrierConfigBuilder(const ProtocolCarrierConfigBuilder &) = delete;
    ProtocolCarrierConfigBuilder& operator=(ProtocolCarrierConfigBuilder const&) = delete;
    virtual ~ProtocolCarrierConfigBuilder();

    ModemData *BuildSetCpCarrierConfig(const char *configDir, char *encodedManifest, int encodedManifestLen);
};

#endif /* __PROTOCOL_MISC_BUILDER_H__ */
