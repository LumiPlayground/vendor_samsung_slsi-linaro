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
 * protocolnetbuilder.cpp
 *
 *  Created on: 2021. 6. 21.
 *      Author: aryaman.2019
 */

#include "protocolnetbuilder.h"
#include "legacy/network/protocolnetbuilderlegacy.h"
#include "json/network/protocolnetbuilderjson.h"

/**
 * ProtocolNetworkRegistrationBuilder
 */
ProtocolNetworkRegistrationBuilder::ProtocolNetworkRegistrationBuilder()
    : ProtocolNetworkRegistrationBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolNetworkRegistrationBuilderInterface = new ProtocolNetworkRegistrationBuilderJson();
    else
        protocolNetworkRegistrationBuilderInterface = new ProtocolNetworkRegistrationBuilderLegacy();
}

ProtocolNetworkRegistrationBuilder::~ProtocolNetworkRegistrationBuilder() {
    delete protocolNetworkRegistrationBuilderInterface;
    protocolNetworkRegistrationBuilderInterface = NULL;
}

ModemData *ProtocolNetworkRegistrationBuilder::BuildNetworkRegistrationState(int domain) {
    return protocolNetworkRegistrationBuilderInterface->BuildNetworkRegistrationState(domain);
}

ModemData *ProtocolNetworkRegistrationBuilder::BuildOperator() {
    return protocolNetworkRegistrationBuilderInterface->BuildOperator();
}

/**
 * ProtocolRadioPowerBuilder
 */
ProtocolRadioPowerBuilder::ProtocolRadioPowerBuilder()
    : ProtocolRadioPowerBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolRadioPowerBuilderInterface = new ProtocolRadioPowerBuilderJson();
    else
        protocolRadioPowerBuilderInterface = new ProtocolRadioPowerBuilderLegacy();
}

ProtocolRadioPowerBuilder::~ProtocolRadioPowerBuilder() {
    delete protocolRadioPowerBuilderInterface;
    protocolRadioPowerBuilderInterface = NULL;
}

ModemData *ProtocolRadioPowerBuilder::BuildRadioPower(int powerState, int forEmergencyCall, int preferredForEmergencyCall) {
    return protocolRadioPowerBuilderInterface->BuildRadioPower(powerState, forEmergencyCall, preferredForEmergencyCall);
}

ModemData *ProtocolRadioPowerBuilder::BuildRestartModem() {
    return protocolRadioPowerBuilderInterface->BuildRestartModem();
}

ModemData *ProtocolRadioPowerBuilder::BuildShutdown() {
    return protocolRadioPowerBuilderInterface->BuildShutdown();
}

ModemData *ProtocolRadioPowerBuilder::BuildGetRadioState() {
    return protocolRadioPowerBuilderInterface->BuildGetRadioState();
}

/**
 * ProtocolNetworkModeBuilder
 */
ProtocolNetworkModeBuilder::ProtocolNetworkModeBuilder()
    : ProtocolNetworkModeBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolNetworkModeBuilderInterface = new ProtocolNetworkModeBuilderJson();
    else
        protocolNetworkModeBuilderInterface = new ProtocolNetworkModeBuilderLegacy();
}

ProtocolNetworkModeBuilder::~ProtocolNetworkModeBuilder() {
    delete protocolNetworkModeBuilderInterface;
    protocolNetworkModeBuilderInterface = NULL;
}

ModemData *ProtocolNetworkModeBuilder::BuildQueryNetworkSelectionMode() {
    return protocolNetworkModeBuilderInterface->BuildQueryNetworkSelectionMode();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetNetworkSelectionAuto() {
    return protocolNetworkModeBuilderInterface->BuildSetNetworkSelectionAuto();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetBandMode(int bandMode) {
    return protocolNetworkModeBuilderInterface->BuildSetBandMode(bandMode);
}

ModemData *ProtocolNetworkModeBuilder::BuildQueryAvailableBandMode() {
    return protocolNetworkModeBuilderInterface->BuildQueryAvailableBandMode();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetNetworkSelectionManual(int rat, const char *plmn) {
    return protocolNetworkModeBuilderInterface->BuildSetNetworkSelectionManual(rat, plmn);
}

ModemData *ProtocolNetworkModeBuilder::BuildSetEndcMode(int mode) {
    return protocolNetworkModeBuilderInterface->BuildSetEndcMode(mode);
}

ModemData *ProtocolNetworkModeBuilder::BuildGetEndcMode() {
    return protocolNetworkModeBuilderInterface->BuildGetEndcMode();
}

ModemData *ProtocolNetworkModeBuilder::BuildGetFrequencyInfo() {
    return protocolNetworkModeBuilderInterface->BuildGetFrequencyInfo();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetNrMode(int mode) {
    return protocolNetworkModeBuilderInterface->BuildSetNrMode(mode);
}

ModemData *ProtocolNetworkModeBuilder::BuildGetNrMode() {
    return protocolNetworkModeBuilderInterface->BuildGetNrMode();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetNrSilenceMode(int nsa, int sa) {
    return protocolNetworkModeBuilderInterface->BuildSetNrSilenceMode(nsa, sa);
}

ModemData *ProtocolNetworkModeBuilder::BuildGetNrSilenceMode() {
    return protocolNetworkModeBuilderInterface->BuildGetNrSilenceMode();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetVonrCapa(int capa) {
    return protocolNetworkModeBuilderInterface->BuildSetVonrCapa(capa);
}

ModemData *ProtocolNetworkModeBuilder::BuildGetVonrCapa() {
    return protocolNetworkModeBuilderInterface->BuildGetVonrCapa();
}

ModemData *ProtocolNetworkModeBuilder::BuildGetManualRatMode() {
    return protocolNetworkModeBuilderInterface->BuildGetManualRatMode();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetManualRatMode(int mode, int rat) {
    return protocolNetworkModeBuilderInterface->BuildSetManualRatMode(mode, rat);
}

ModemData *ProtocolNetworkModeBuilder::BuildGetFrequencyLock() {
    return protocolNetworkModeBuilderInterface->BuildGetFrequencyLock();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetFrequencyLock(int mode, int rat,
                                                             int ltePci, int lteEarfcn, int gsmArfcn, int wcdmaPsc,
                                                             int wcdmaUarfcn) {
    return protocolNetworkModeBuilderInterface->BuildSetFrequencyLock(mode, rat,
                                                                      ltePci, lteEarfcn, gsmArfcn, wcdmaPsc, wcdmaUarfcn);
}

ModemData *ProtocolNetworkModeBuilder::BuildSetDuplexMode(BYTE mode_4g, BYTE mode_3g) {
    return protocolNetworkModeBuilderInterface->BuildSetDuplexMode(mode_4g, mode_3g);
}

ModemData *ProtocolNetworkModeBuilder::BuildSetDuplexMode(int mode) {
    return protocolNetworkModeBuilderInterface->BuildSetDuplexMode(mode);
}

ModemData *ProtocolNetworkModeBuilder::BuildGetDuplexMode() {
    return protocolNetworkModeBuilderInterface->BuildGetDuplexMode();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetEmergencyCallStatus(int status, int rat) {
    return protocolNetworkModeBuilderInterface->BuildSetEmergencyCallStatus(status, rat);
}

ModemData *ProtocolNetworkModeBuilder::BuildGetBarringInfo() {
    return protocolNetworkModeBuilderInterface->BuildGetBarringInfo();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetAllowData(int state) {
    return protocolNetworkModeBuilderInterface->BuildSetAllowData(state);
}

ModemData *ProtocolNetworkModeBuilder::BuildGetPsService() {
    return protocolNetworkModeBuilderInterface->BuildGetPsService();
}

ModemData *ProtocolNetworkModeBuilder::BuildSetMicroCellSearch(BYTE srch_mode) {
    return protocolNetworkModeBuilderInterface->BuildSetMicroCellSearch(srch_mode);
}

ModemData *ProtocolNetworkModeBuilder::BuildSetSysSelChannels(bool isSpecifyChannel,
                                                              int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *pSpecifiers) {
    return protocolNetworkModeBuilderInterface->BuildSetSysSelChannels(isSpecifyChannel,
                                                                       specifiersLength, pSpecifiers);
}

/**
 * ProtocolNetworkTypeBuilder
 */
ProtocolNetworkTypeBuilder::ProtocolNetworkTypeBuilder() {
    if (IsPayloadTypeJson())
        protocolNetworkTypeBuilderInterface = new ProtocolNetworkTypeBuilderJson();
    else
        protocolNetworkTypeBuilderInterface = new ProtocolNetworkTypeBuilderLegacy();
}

ProtocolNetworkTypeBuilder::~ProtocolNetworkTypeBuilder() {
    delete protocolNetworkTypeBuilderInterface;
    protocolNetworkTypeBuilderInterface = NULL;
}

ModemData *ProtocolNetworkTypeBuilder::BuildSetPreferredNetworkType(int netType) {
    return protocolNetworkTypeBuilderInterface->BuildSetPreferredNetworkType(netType);
}

ModemData *ProtocolNetworkTypeBuilder::BuildGetPreferredNetworkType() {
    return protocolNetworkTypeBuilderInterface->BuildGetPreferredNetworkType();
}

ModemData *ProtocolNetworkTypeBuilder::BuildSetDSNetworkType(int netType) {
    return protocolNetworkTypeBuilderInterface->BuildSetDSNetworkType(netType);
}

ModemData *ProtocolNetworkTypeBuilder::BuildSetDualNetworkAndAllowData(
    int typeForPrimary, int typeForSecondary, int allowedForPrimary,
    int allowedForSecondary) {
    return protocolNetworkTypeBuilderInterface->BuildSetDualNetworkAndAllowData(
        typeForPrimary, typeForSecondary, allowedForPrimary, allowedForSecondary);
}

/**
 * ProtocolNetworkScanBiulder
 */
ProtocolNetworkScanBuilder::ProtocolNetworkScanBuilder()
    : ProtocolNetworkScanBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolNetworkScanBuilderInterface = new ProtocolNetworkScanBuilderJson();
    else
        protocolNetworkScanBuilderInterface = new ProtocolNetworkScanBuilderLegacy();
}

ProtocolNetworkScanBuilder::~ProtocolNetworkScanBuilder() {
    delete protocolNetworkScanBuilderInterface;
    protocolNetworkScanBuilderInterface = NULL;
}

ModemData *ProtocolNetworkScanBuilder::BuildQueryAvailableNetwork(int ran) {
    return protocolNetworkScanBuilderInterface->BuildQueryAvailableNetwork(ran);
}

ModemData *ProtocolNetworkScanBuilder::BuildCancelQueryAvailableNetwork() {
    return protocolNetworkScanBuilderInterface->BuildCancelQueryAvailableNetwork();
}

ModemData *ProtocolNetworkScanBuilder::BuildStartNetworkScan(int scantype, int timeInterval, int lenSpecifiers,
                                                             RIL_RadioAccessSpecifier_V1_5 *pSpecifiers) {
    return protocolNetworkScanBuilderInterface->BuildStartNetworkScan(scantype, timeInterval,
                                                                      lenSpecifiers, pSpecifiers);
}

ModemData *ProtocolNetworkScanBuilder::BuildStartNetworkScan(
    int scantype, int timeInterval, int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *specifiers,
    int maxSearchTime, bool incrementalResults, int incrementalResultsPeriodicity,
    int numOfMccMncs, char **mccMncs) {
    return protocolNetworkScanBuilderInterface->BuildStartNetworkScan(scantype, timeInterval,
                                                                      specifiersLength, specifiers, maxSearchTime, incrementalResults, incrementalResultsPeriodicity,
                                                                      numOfMccMncs, mccMncs);
}

ModemData *ProtocolNetworkScanBuilder::BuildStopNetworkScan() {
    return protocolNetworkScanBuilderInterface->BuildStopNetworkScan();
}

/**
 * ProtocolCdmaBuilder
 */
ProtocolCdmaBuilder::ProtocolCdmaBuilder()
    : ProtocolCdmaBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolCdmaBuilderInterface = new ProtocolCdmaBuilderJson();
    else
        protocolCdmaBuilderInterface = new ProtocolCdmaBuilderLegacy();
}

ProtocolCdmaBuilder::~ProtocolCdmaBuilder() {
    delete protocolCdmaBuilderInterface;
    protocolCdmaBuilderInterface = NULL;
}

ModemData *ProtocolCdmaBuilder::BuildSetCdmaSetRoamingType(int cdmaRoamingType) {
    return protocolCdmaBuilderInterface->BuildSetCdmaSetRoamingType(cdmaRoamingType);
}

ModemData *ProtocolCdmaBuilder::BuildQueryCdmaRoamingType() {
    return protocolCdmaBuilderInterface->BuildQueryCdmaRoamingType();
}

ModemData *ProtocolCdmaBuilder::BuildSetCdmaHybridMode(int hybridMode) {
    return protocolCdmaBuilderInterface->BuildSetCdmaHybridMode(hybridMode);
}

ModemData *ProtocolCdmaBuilder::BuildGetCdmaHybridMode() {
    return protocolCdmaBuilderInterface->BuildGetCdmaHybridMode();
}

/**
 * ProtocolNetworkBuilder
 */
ProtocolNetworkBuilder::ProtocolNetworkBuilder()
    : ProtocolNetworkBuilderInterface() {
    if (IsPayloadTypeJson())
        protocolNetworkBuilderInterface = new ProtocolNetworkBuilderJson();
    else
        protocolNetworkBuilderInterface = new ProtocolNetworkBuilderLegacy();
}

ProtocolNetworkBuilder::~ProtocolNetworkBuilder() {
    delete protocolNetworkBuilderInterface;
    protocolNetworkBuilderInterface = NULL;
}
ModemData *ProtocolNetworkBuilder::BuildGetCellInfoList() {
    return protocolNetworkBuilderInterface->BuildGetCellInfoList();
}

ModemData *ProtocolNetworkBuilder::BuildSetCellInfoListReportRate(int rate) {
    return protocolNetworkBuilderInterface->BuildSetCellInfoListReportRate(rate);
}

ModemData *ProtocolNetworkBuilder::BuildSetRCNetworkType(int rcVersion, int rcSession, int rcPhase, int rcRaf, char *pUuid, int rcStatus) {
    return protocolNetworkBuilderInterface->BuildSetRCNetworkType(rcVersion, rcSession, rcPhase, rcRaf, pUuid, rcStatus);
}

ModemData *ProtocolNetworkBuilder::BuildGetRCNetworkType() {
    return protocolNetworkBuilderInterface->BuildGetRCNetworkType();
}

ModemData *ProtocolNetworkBuilder::BuildSvNumber(const char *svn) {
    return protocolNetworkBuilderInterface->BuildSvNumber(svn);
}

ModemData *ProtocolNetworkBuilder::BuildGetSystemSelectionChannels() {
    return protocolNetworkBuilderInterface->BuildGetSystemSelectionChannels();
}

ModemData *ProtocolNetworkBuilder::BuildSetAllowedNetworkTypeBitmap(int networkTypeBitmap) {
    return protocolNetworkBuilderInterface->BuildSetAllowedNetworkTypeBitmap(networkTypeBitmap);
}

ModemData *ProtocolNetworkBuilder::BuildGetAllowedNetworkTypeBitmap() {
    return protocolNetworkBuilderInterface->BuildGetAllowedNetworkTypeBitmap();
}

ModemData *ProtocolNetworkBuilder::BuildSetPlmnList(int requestId, void *data, unsigned int datalen) {
    return protocolNetworkBuilderInterface->BuildSetPlmnList(requestId, data, datalen);
}

ModemData *ProtocolNetworkBuilder::BuildGetPlmnList() {
    return protocolNetworkBuilderInterface->BuildGetPlmnList();
}

ModemData *ProtocolNetworkBuilder::BuildSetMobileDataState(int mobileData, int roamingData) {
    return protocolNetworkBuilderInterface->BuildSetMobileDataState(mobileData, roamingData);
}

ModemData *ProtocolNetworkBuilder::BuildGetNitzTime() {
    return protocolNetworkBuilderInterface->BuildGetNitzTime();
}

ModemData *ProtocolNetworkBuilder::BuildSetDeviceService(int setting) {
    return protocolNetworkBuilderInterface->BuildSetDeviceService(setting);
}

ModemData *ProtocolNetworkBuilder::BuildGetDeviceService() {
    return protocolNetworkBuilderInterface->BuildGetDeviceService();
}
