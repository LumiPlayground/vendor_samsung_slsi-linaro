/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_NET_BUILDER_JSON_H__
#define __PROTOCOL_NET_BUILDER_JSON_H__

#include "protocoljsonbuilder.h"
#include "protocolnetbuilderinterface.h"

class ProtocolNetworkRegistrationBuilderJson : public ProtocolNetworkRegistrationBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolNetworkRegistrationBuilderJson() = default;
    virtual ~ProtocolNetworkRegistrationBuilderJson() = default;

    ModemData *BuildNetworkRegistrationState(int domain);
    ModemData *BuildOperator();
};

class ProtocolRadioPowerBuilderJson : public ProtocolRadioPowerBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolRadioPowerBuilderJson() = default;
    virtual ~ProtocolRadioPowerBuilderJson() = default;

    ModemData *BuildRadioPower(int powerState, int forEmergencyCall,
                               int preferredForEmergencyCall);
    ModemData *BuildShutdown();
    ModemData *BuildGetRadioState();
    ModemData *BuildRestartModem();
};

class ProtocolNetworkModeBuilderJson : public ProtocolNetworkModeBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolNetworkModeBuilderJson() = default;
    virtual ~ProtocolNetworkModeBuilderJson() = default;

    ModemData *BuildQueryNetworkSelectionMode();
    ModemData *BuildSetNetworkSelectionAuto();
    ModemData *BuildSetNetworkSelectionManual(int rat, const char *plmn);
    ModemData *BuildSetBandMode(int bandMode);
    ModemData *BuildQueryAvailableBandMode();
    ModemData *BuildSetEndcMode(int mode);
    ModemData *BuildGetEndcMode();
    ModemData *BuildGetFrequencyInfo();
    ModemData *BuildSetNrMode(int mode);
    ModemData *BuildGetNrMode();
    ModemData *BuildSetNrSilenceMode(int nsa, int sa);
    ModemData *BuildGetNrSilenceMode();
    ModemData *BuildSetVonrCapa(int capa);
    ModemData *BuildGetVonrCapa();
    ModemData *BuildGetManualRatMode();
    ModemData *BuildSetManualRatMode(int mode, int rat);
    ModemData *BuildGetBarringInfo();
    ModemData *BuildSetAllowData(int state);
    ModemData *BuildGetPsService();
    ModemData *BuildSetDuplexMode(BYTE mode_4g, BYTE mode_3g);
    ModemData *BuildSetDuplexMode(int mode);
    ModemData *BuildGetDuplexMode();
    ModemData *BuildSetSysSelChannels(bool isSpecifyChannel,
                                      int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *pSpecifiers);
    ModemData *BuildGetFrequencyLock();
    ModemData *BuildSetFrequencyLock(int mode, int rat, int ltePci,
                                     int lteEarfcn, int gsmArfcn, int wcdmaPsc, int wcdmaUarfcn);
    ModemData *BuildSetEmergencyCallStatus(int status, int rat);
    ModemData *BuildSetMicroCellSearch(BYTE srch_mode);
};

class ProtocolNetworkScanBuilderJson : public ProtocolNetworkScanBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolNetworkScanBuilderJson() = default;
    virtual ~ProtocolNetworkScanBuilderJson() = default;

    ModemData *BuildQueryAvailableNetwork(int ran);
    ModemData *BuildCancelQueryAvailableNetwork();
    ModemData *BuildStartNetworkScan(int scantype, int timeInterval, int lenSpecifiers,
                                     RIL_RadioAccessSpecifier_V1_5 *pSpecifiers);
    ModemData *BuildStartNetworkScan(
        int scantype, int timeInterval, int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *specifiers,
        int maxSearchTime, bool incrementalResults, int incrementalResultsPeriodicity,
        int numOfMccMncs, char **mccMncs);
    ModemData *BuildStopNetworkScan();
};

class ProtocolNetworkTypeBuilderJson : public ProtocolNetworkTypeBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolNetworkTypeBuilderJson() = default;
    virtual ~ProtocolNetworkTypeBuilderJson() = default;

    ModemData *BuildSetPreferredNetworkType(int netType);
    ModemData *BuildGetPreferredNetworkType();
    ModemData *BuildSetDSNetworkType(int netType);
    ModemData *BuildSetDualNetworkAndAllowData(int typeForPrimary,
                                               int typeForSecondary, int allowedForPrimary,
                                               int allowedForSecondary);
};

class ProtocolCdmaBuilderJson : public ProtocolCdmaBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolCdmaBuilderJson() = default;
    virtual ~ProtocolCdmaBuilderJson() = default;

    ModemData *BuildSetCdmaSetRoamingType(int cdmaRoamingType);
    ModemData *BuildQueryCdmaRoamingType();
    ModemData *BuildSetCdmaHybridMode(int hybridMode);
    ModemData *BuildGetCdmaHybridMode();
};

class ProtocolNetworkBuilderJson : public ProtocolNetworkBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolNetworkBuilderJson() = default;
    virtual ~ProtocolNetworkBuilderJson() = default;

    ModemData *BuildGetCellInfoList();
    ModemData *BuildSetCellInfoListReportRate(int rate);
    ModemData *BuildSetRCNetworkType(int rcVersion, int rcSession, int rcPhase, int rcRaf, char *pUuid, int rcStatus);
    ModemData *BuildGetRCNetworkType();
    ModemData *BuildSvNumber(const char *svn);
    ModemData *BuildGetSystemSelectionChannels();
    ModemData *BuildSetAllowedNetworkTypeBitmap(int networkTypeBitmap);
    ModemData *BuildGetAllowedNetworkTypeBitmap();
    ModemData *BuildSetPlmnList(int requestId, void *data, unsigned int datalen);
    ModemData *BuildGetPlmnList();
    ModemData *BuildSetMobileDataState(int mobileData, int roamingData);
    ModemData *BuildGetNitzTime() { return nullptr; }
    ModemData *BuildSetDeviceService(int setting);
    ModemData *BuildGetDeviceService();
};

#endif /* __PROTOCOL_NET_BUILDER_JSON_H__ */
