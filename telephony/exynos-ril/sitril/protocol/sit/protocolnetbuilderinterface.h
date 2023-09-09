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
 * protocolnetbuilderinterface.h
 *
 *  Created on: 2021. 06. 21.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_NET_BUILDER_INTERFACE_H__
#define __PROTOCOL_NET_BUILDER_INTERFACE_H__

#include "rildef.h"

class ModemData;

class ProtocolNetworkRegistrationBuilderInterface {
public:
    ProtocolNetworkRegistrationBuilderInterface() = default;
    virtual ~ProtocolNetworkRegistrationBuilderInterface() = default;

    virtual ModemData *BuildNetworkRegistrationState(int domain) = 0;
    virtual ModemData *BuildOperator() = 0;
};

class ProtocolRadioPowerBuilderInterface {
public:
    ProtocolRadioPowerBuilderInterface() = default;
    virtual ~ProtocolRadioPowerBuilderInterface() = default;

    virtual ModemData* BuildRadioPower(int powerState, int forEmergencyCall,
            int preferredForEmergencyCall) = 0;
    virtual ModemData* BuildShutdown() = 0;
    virtual ModemData* BuildGetRadioState() = 0;
    virtual ModemData *BuildRestartModem() = 0;
};

class ProtocolNetworkModeBuilderInterface {
public:
    ProtocolNetworkModeBuilderInterface() = default;
    virtual ~ProtocolNetworkModeBuilderInterface() = default;

    virtual ModemData *BuildQueryNetworkSelectionMode() = 0;
    virtual ModemData *BuildSetNetworkSelectionAuto() = 0;
    virtual ModemData *BuildSetNetworkSelectionManual(int rat, const char *plmn) = 0;
    virtual ModemData *BuildSetBandMode(int bandMode) = 0;
    virtual ModemData *BuildQueryAvailableBandMode() = 0;
    virtual ModemData *BuildSetEndcMode(int mode) = 0;
    virtual ModemData *BuildGetEndcMode() = 0;
    virtual ModemData *BuildGetFrequencyInfo() = 0;
    virtual ModemData *BuildSetNrMode(int mode) = 0;
    virtual ModemData *BuildGetNrMode() = 0;
    virtual ModemData *BuildSetNrSilenceMode(int nsa, int sa) = 0;
    virtual ModemData *BuildGetNrSilenceMode() = 0;
    virtual ModemData *BuildSetVonrCapa(int capa) = 0;
    virtual ModemData *BuildGetVonrCapa() = 0;
    virtual ModemData *BuildGetManualRatMode() = 0;
    virtual ModemData *BuildSetManualRatMode(int mode, int rat) = 0;
    virtual ModemData *BuildGetBarringInfo() = 0;
    virtual ModemData *BuildSetAllowData(int state) = 0;
    virtual ModemData *BuildGetPsService() = 0;
    virtual ModemData *BuildSetDuplexMode(BYTE mode_4g, BYTE mode_3g) = 0;
    virtual ModemData *BuildSetDuplexMode(int mode) = 0;
    virtual ModemData *BuildGetDuplexMode() = 0;
    virtual ModemData* BuildSetSysSelChannels(bool isSpecifyChannel,
            int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *pSpecifiers) = 0;
    virtual ModemData* BuildGetFrequencyLock() = 0;
    virtual ModemData* BuildSetFrequencyLock(int mode, int rat, int ltePci,
            int lteEarfcn, int gsmArfcn, int wcdmaPsc, int wcdmaUarfcn) = 0;
    virtual ModemData *BuildSetEmergencyCallStatus(int status, int rat) = 0;
    virtual ModemData *BuildSetMicroCellSearch(BYTE srch_mode) = 0;
};

class ProtocolNetworkScanBuilderInterface {
public:
    ProtocolNetworkScanBuilderInterface() = default;
    virtual ~ProtocolNetworkScanBuilderInterface() = default;

    virtual ModemData *BuildQueryAvailableNetwork(int ran) = 0;
    virtual ModemData *BuildCancelQueryAvailableNetwork() = 0;
    virtual ModemData *BuildStartNetworkScan(int scantype, int timeInterval, int lenSpecifiers,
                                        RIL_RadioAccessSpecifier_V1_5 *pSpecifiers) = 0;
    virtual ModemData *BuildStartNetworkScan(
            int scantype, int timeInterval, int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *specifiers,
            int maxSearchTime, bool incrementalResults, int incrementalResultsPeriodicity,
            int numOfMccMncs, char **mccMncs) = 0;
    virtual ModemData *BuildStopNetworkScan() = 0;
};

class ProtocolNetworkTypeBuilderInterface {
public:
    ProtocolNetworkTypeBuilderInterface() = default;
    virtual ~ProtocolNetworkTypeBuilderInterface() = default;

    virtual ModemData *BuildSetPreferredNetworkType(int netType) = 0;
    virtual ModemData *BuildGetPreferredNetworkType() = 0;
    virtual ModemData *BuildSetDSNetworkType(int netType) = 0;
    virtual ModemData* BuildSetDualNetworkAndAllowData(int typeForPrimary,
            int typeForSecondary, int allowedForPrimary,
            int allowedForSecondary) = 0;
};

class ProtocolCdmaBuilderInterface {
public:
    ProtocolCdmaBuilderInterface() = default;
    virtual ~ProtocolCdmaBuilderInterface() = default;

    virtual ModemData *BuildSetCdmaSetRoamingType(int cdmaRoamingType) = 0;
    virtual ModemData *BuildQueryCdmaRoamingType() = 0;
    virtual ModemData *BuildSetCdmaHybridMode(int hybridMode) = 0;
    virtual ModemData *BuildGetCdmaHybridMode() = 0;
};

class ProtocolNetworkBuilderInterface {
public:
    ProtocolNetworkBuilderInterface() = default;
    virtual ~ProtocolNetworkBuilderInterface() = default;

    virtual ModemData *BuildGetCellInfoList() = 0;
    virtual ModemData *BuildSetCellInfoListReportRate(int rate) = 0;
    virtual ModemData *BuildSetRCNetworkType(int rcVersion, int rcSession, int rcPhase, int rcRaf, char *pUuid, int rcStatus) = 0;
    virtual ModemData *BuildGetRCNetworkType() = 0;
    virtual ModemData *BuildSvNumber(const char *svn) = 0;
    virtual ModemData *BuildGetSystemSelectionChannels() = 0;
    virtual ModemData *BuildSetAllowedNetworkTypeBitmap(int networkTypeBitmap) = 0;
    virtual ModemData *BuildGetAllowedNetworkTypeBitmap() = 0;
    virtual ModemData *BuildSetPlmnList(int requestId, void *data, unsigned int datalen) = 0;
    virtual ModemData *BuildGetPlmnList() = 0;
    virtual ModemData *BuildSetMobileDataState(int mobileData, int roamingData) = 0;
    virtual ModemData *BuildGetNitzTime() = 0;
    virtual ModemData *BuildSetDeviceService(int setting) = 0;
    virtual ModemData *BuildGetDeviceService() = 0;
};

#endif /* __PROTOCOL_NET_BUILDER_INTERFACE_H__ */
