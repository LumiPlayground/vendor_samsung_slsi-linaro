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
 * protocolmiscbuilderlegacy.h
 *
 *  Created on: 2014. 12. 2.
 *      Author: sungwoo48.choi
 */

#ifndef __PROTOCOL_MISC_BUILDER_LEGACY_H__
#define __PROTOCOL_MISC_BUILDER_LEGACY_H__

#include "protocolmiscbuilderinterface.h"
#include "protocolbuilder.h"

/**
 * ProtocolMiscBuilderLegacy
 */
class ProtocolMiscBuilderLegacy : public ProtocolBuilder, public ProtocolMiscBuilderInterface {
  public:
    ProtocolMiscBuilderLegacy() = default;
    virtual ~ProtocolMiscBuilderLegacy() = default;

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
 * ProtocolDeviceInfoBuilderLegacy
 */
class ProtocolDeviceInfoBuilderLegacy : public ProtocolBuilder, public ProtocolDeviceInfoBuilderInterface {
  public:
    ProtocolDeviceInfoBuilderLegacy() = default;
    virtual ~ProtocolDeviceInfoBuilderLegacy() = default;

  public:
    ModemData *GetBaseBandVersion(BYTE mask = 0xFF);
    ModemData *GetIMEI();
    ModemData *GetIMEISV();
    ModemData *GetDevID();
    ModemData *SetScreenState(INT32 state);
    ModemData *SetPinControl(BYTE signal, BYTE status);
    ModemData *SendSGCValue(const int TargetOp, const int Rvs1, const int Rvs2);
    ModemData *BuildGetHwConfig();
    ModemData *SendDeviceInfo(const char *model, const char *swVer, const char *productName, const char *OsVer);
    ModemData *BuildSetFeatureInfo(RilProperty &bundle);
    ModemData *GetSignalStrength();
    ModemData *BuildLceStart(int lceMode, int interval);
    ModemData *BuildLceStop();
    ModemData *BuildLcePullLceData();
    ModemData *BuildSetUnsolicitedResponseFilter(unsigned int bitMask);
    ModemData *BuildSetCarrierInfoImsiEncryption(char *pMcc, char *pMnc,
                                                 int keyLen, BYTE *pKey, int keyIdLen, char *pKeyId, LONG expTime, int keyType);
    ModemData *BuildSetOpenCarierInfo(unsigned int opc,
                                      const char *plmn);
    ModemData *BuildSetSignalReportCriteria(int ms, int db, int len,
                                            int *dbm, int accessNetwork, int measureType, bool isEnable);
    ModemData *BuildSetLinkCapaReportCriteria(int hMs, int hDlKbps,
                                              int hUlKbps, int tDlLen, int *tDlKbps, int tUlLen, int *tUlKbps,
                                              int accessNetwork);
};

/**
 * ProtocolMiscDebugBuilderLegacy
 */
class ProtocolMiscDebugBuilderLegacy : public ProtocolBuilder, public ProtocolMiscDebugBuilderInterface {
  public:
    ProtocolMiscDebugBuilderLegacy() = default;
    virtual ~ProtocolMiscDebugBuilderLegacy() = default;

  public:
    ModemData *SetEngMode(BYTE mode);
    ModemData *SetEngMode(BYTE mode, BYTE sub_mode);
    ModemData *SetScrLine(BYTE lineno);
    ModemData *SetEngStringInput(BYTE len, char *input);
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
 * ProtocolSensorBuilderLegacy
 */
class ProtocolSensorBuilderLegacy : public ProtocolBuilder, public ProtocolSensorBuilderInterface {
  public:
    ProtocolSensorBuilderLegacy() = default;
    virtual ~ProtocolSensorBuilderLegacy() = default;

  public:
    ModemData *GetRfDesenseMode();
    ModemData *SetRfDesenseMode(void *data, unsigned int datalen);
    ModemData *BuildPSensorStatus(int pSensorStatus);
    ModemData *BuildSetSarState(int sarState);
    ModemData *BuildGetSarState();
    ModemData *BuildScanRssi(int rat, int band, int rbw, int scanMode,
                             int startFreq, int endFreq, int step, int antenna, int sampling,
                             int tx1, int tx1Band, int tx1Bw, int tx1Freq, int tx1Power,
                             int tx1RbNum, int tx1RbOffset, int tx1Mcs, int tx2, int tx2Band,
                             int tx2Bw, int tx2Freq, int tx2Power, int tx2RbNum, int tx2RbOffset,
                             int tx2Mcs);
};

/**
 * ProtocolOemOmaDmBuilderLegacy
 */
class ProtocolOemOmaDmBuilderLegacy : public ProtocolBuilder, public ProtocolOemOmaDmBuilderInterface {
  public:
    ProtocolOemOmaDmBuilderLegacy() = default;
    virtual ~ProtocolOemOmaDmBuilderLegacy() = default;

  public:
    ModemData *BuildGetRadioNode(const char *path);
    ModemData *BuildSetRadioNode(const char *path, const char *value);
    ModemData *BuildGetVoLteProvisionUpdate();
    ModemData *BuildSetVoLteProvisionUpdate();
    ModemData *GetMslCode();
};

/**
 * ProtocolDtmfBuilderLegacy
 */
class ProtocolDtmfBuilderLegacy : public ProtocolBuilder, public ProtocolDtmfBuilderInterface {
  public:
    ProtocolDtmfBuilderLegacy() = default;
    virtual ~ProtocolDtmfBuilderLegacy() = default;

  public:
    ModemData *BuildDtmfStart(bool tone_type, BYTE digit);
    ModemData *BuildDtmf(int dtmf_length, char *dtmf);
    ModemData *BuildDtmfStop();
};

/**
 * ProtocolMiscNetworkBuilderLegacy
 */
class ProtocolMiscNetworkBuilderLegacy : public ProtocolBuilder, public ProtocolMiscNetworkBuilderInterface {
  public:
    ProtocolMiscNetworkBuilderLegacy() = default;
    virtual ~ProtocolMiscNetworkBuilderLegacy() = default;

    ModemData *GetManualBandMode();
    ModemData *SetManualBandMode(void *data, unsigned int datalen);
    ModemData *BuildCdmaSubscription();
    ModemData *BuildSetVoiceOperation(int mode);
    ModemData *BuildGetVoiceOperation();
    ModemData *BuildSetPreferredCallCapability(int mode);
    ModemData *BuildGetPreferredCallCapability();
    ModemData *BuildSetStatckStatus(int mode);
    ModemData *BuildGetStatckStatus();
    ModemData *BuildGetNitzTime();
};

/**
 * ProtocolCarrierConfigBuilderLegacy
 */
class ProtocolCarrierConfigBuilderLegacy : public ProtocolBuilder, public ProtocolCarrierConfigBuilderInterface {
  public:
    ProtocolCarrierConfigBuilderLegacy() = default;
    virtual ~ProtocolCarrierConfigBuilderLegacy() = default;

  public:
    ModemData *BuildSetCpCarrierConfig(const char *configDir, char *encodedManifest, int encodedManifestLen);
};

#endif /* __PROTOCOL_MISC_BUILDER_LEGACY_H__ */
