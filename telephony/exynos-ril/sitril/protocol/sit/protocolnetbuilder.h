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
 * protocolnetbuilder.h
 *
 *  Created on: 2021. 06. 21.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_NET_BUILDER_H__
#define __PROTOCOL_NET_BUILDER_H__

#include "protocolnetbuilderinterface.h"

class ProtocolNetworkRegistrationBuilder : public ProtocolNetworkRegistrationBuilderInterface {
private:
    ProtocolNetworkRegistrationBuilderInterface *protocolNetworkRegistrationBuilderInterface = NULL;

public:
    ProtocolNetworkRegistrationBuilder();
    ProtocolNetworkRegistrationBuilder(const ProtocolNetworkRegistrationBuilder&) = delete;
    ProtocolNetworkRegistrationBuilder& operator=(ProtocolNetworkRegistrationBuilder const&) = delete;
    virtual ~ProtocolNetworkRegistrationBuilder();

    ModemData *BuildNetworkRegistrationState(int domain);
    ModemData *BuildOperator();
};

class ProtocolRadioPowerBuilder : public ProtocolRadioPowerBuilderInterface {
private:
    ProtocolRadioPowerBuilderInterface *protocolRadioPowerBuilderInterface = NULL;

public:
    ProtocolRadioPowerBuilder();
    ProtocolRadioPowerBuilder(const ProtocolRadioPowerBuilder&) = delete;
    ProtocolRadioPowerBuilder& operator=(ProtocolRadioPowerBuilder const&) = delete;
    virtual ~ProtocolRadioPowerBuilder();

    ModemData* BuildRadioPower(int powerState, int forEmergencyCall,
            int preferredForEmergencyCall);
    ModemData* BuildShutdown();
    ModemData* BuildGetRadioState();
    ModemData *BuildRestartModem();
};

class ProtocolNetworkModeBuilder : public ProtocolNetworkModeBuilderInterface {
private:
    ProtocolNetworkModeBuilderInterface *protocolNetworkModeBuilderInterface = NULL;

public:
    ProtocolNetworkModeBuilder();
    ProtocolNetworkModeBuilder(const ProtocolNetworkModeBuilder&) = delete;
    ProtocolNetworkModeBuilder& operator=(ProtocolNetworkModeBuilder const&) = delete;
    virtual ~ProtocolNetworkModeBuilder();

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
    ModemData* BuildSetSysSelChannels(bool isSpecifyChannel,
            int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *pSpecifiers);
    ModemData* BuildGetFrequencyLock();
    ModemData* BuildSetFrequencyLock(int mode, int rat, int ltePci,
            int lteEarfcn, int gsmArfcn, int wcdmaPsc, int wcdmaUarfcn);
    ModemData *BuildSetEmergencyCallStatus(int status, int rat);
    ModemData *BuildSetMicroCellSearch(BYTE srch_mode);
};

class ProtocolNetworkScanBuilder : public ProtocolNetworkScanBuilderInterface {
private:
    ProtocolNetworkScanBuilderInterface *protocolNetworkScanBuilderInterface = NULL;

public:
    ProtocolNetworkScanBuilder();
    ProtocolNetworkScanBuilder(const ProtocolNetworkScanBuilder&) = delete;
    ProtocolNetworkScanBuilder& operator=(ProtocolNetworkScanBuilder const&) = delete;
    virtual ~ProtocolNetworkScanBuilder();

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

class ProtocolNetworkTypeBuilder : public ProtocolNetworkTypeBuilderInterface {
private:
    ProtocolNetworkTypeBuilderInterface *protocolNetworkTypeBuilderInterface = NULL;

public:
    ProtocolNetworkTypeBuilder();
    ProtocolNetworkTypeBuilder(const ProtocolNetworkTypeBuilder&) = delete;
    ProtocolNetworkTypeBuilder& operator=(ProtocolNetworkTypeBuilder const&) = delete;
    virtual ~ProtocolNetworkTypeBuilder();

    ModemData *BuildSetPreferredNetworkType(int netType);
    ModemData *BuildGetPreferredNetworkType();
    ModemData *BuildSetDSNetworkType(int netType);
    ModemData* BuildSetDualNetworkAndAllowData(int typeForPrimary,
            int typeForSecondary, int allowedForPrimary,
            int allowedForSecondary);
};

class ProtocolCdmaBuilder : public ProtocolCdmaBuilderInterface {
private:
    ProtocolCdmaBuilderInterface *protocolCdmaBuilderInterface = NULL;

public:
    ProtocolCdmaBuilder();
    ProtocolCdmaBuilder(const ProtocolCdmaBuilder&) = delete;
    ProtocolCdmaBuilder& operator=(ProtocolCdmaBuilder const&) = delete;
    virtual ~ProtocolCdmaBuilder();

    ModemData *BuildSetCdmaSetRoamingType(int cdmaRoamingType);
    ModemData *BuildQueryCdmaRoamingType();
    ModemData *BuildSetCdmaHybridMode(int hybridMode);
    ModemData *BuildGetCdmaHybridMode();
};

class ProtocolNetworkBuilder : public ProtocolNetworkBuilderInterface {
private:
    ProtocolNetworkBuilderInterface *protocolNetworkBuilderInterface = NULL;

public:
    ProtocolNetworkBuilder();
    ProtocolNetworkBuilder(const ProtocolNetworkBuilder&) = delete;
    ProtocolNetworkBuilder& operator=(ProtocolNetworkBuilder const&) = delete;
    virtual ~ProtocolNetworkBuilder();

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
    ModemData *BuildGetNitzTime();
    ModemData *BuildSetDeviceService(int setting);
    ModemData *BuildGetDeviceService();
};

#endif /* __PROTOCOL_NET_BUILDER_H__ */
