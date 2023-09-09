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
 * protocolnetadapter.cpp
 *
 *  Created on: 2021. 6. 24.
 *      Author: aryaman.2019
 */

#include "protocolnetadapter.h"
#include "legacy/network/protocolnetadapterlegacy.h"
#include "json/network/protocolnetadapterjson.h"

/**
 * ProtocolNetVoiceRegStateAdapter
 */
ProtocolNetVoiceRegStateAdapter::ProtocolNetVoiceRegStateAdapter(const ModemData *pModemData)
    : ProtocolNetVoiceRegStateAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetVoiceRegStateAdapterInterface = new ProtocolNetVoiceRegStateAdapterJson(pModemData);
    } else {
        protocolNetVoiceRegStateAdapterInterface = new ProtocolNetVoiceRegStateAdapterLegacy(pModemData);
    }
}

ProtocolNetVoiceRegStateAdapter::~ProtocolNetVoiceRegStateAdapter() {
    delete protocolNetVoiceRegStateAdapterInterface;
    protocolNetVoiceRegStateAdapterInterface = NULL;
}

int ProtocolNetVoiceRegStateAdapter::GetRegState() const {
    return protocolNetVoiceRegStateAdapterInterface->GetRegState();
}

int ProtocolNetVoiceRegStateAdapter::GetRejectCause() const {
    return protocolNetVoiceRegStateAdapterInterface->GetRejectCause();
}

int ProtocolNetVoiceRegStateAdapter::GetRadioTech() const {
    return protocolNetVoiceRegStateAdapterInterface->GetRadioTech();
}

int ProtocolNetVoiceRegStateAdapter::GetLAC() const {
    return protocolNetVoiceRegStateAdapterInterface->GetLAC();
}

int ProtocolNetVoiceRegStateAdapter::GetCellId() const {
    return protocolNetVoiceRegStateAdapterInterface->GetCellId();
}

int ProtocolNetVoiceRegStateAdapter::GetPSC() const {
    return protocolNetVoiceRegStateAdapterInterface->GetPSC();
}

int ProtocolNetVoiceRegStateAdapter::GetTAC() const {
    return protocolNetVoiceRegStateAdapterInterface->GetTAC();
}

int ProtocolNetVoiceRegStateAdapter::GetPCID() const {
    return protocolNetVoiceRegStateAdapterInterface->GetPCID();
}

int ProtocolNetVoiceRegStateAdapter::GetECI() const {
    return protocolNetVoiceRegStateAdapterInterface->GetECI();
}

int ProtocolNetVoiceRegStateAdapter::GetStationId() const {
    return protocolNetVoiceRegStateAdapterInterface->GetStationId();
}

int ProtocolNetVoiceRegStateAdapter::GetStationLat() const {
    return protocolNetVoiceRegStateAdapterInterface->GetStationLat();
}

int ProtocolNetVoiceRegStateAdapter::GetStationLong() const {
    return protocolNetVoiceRegStateAdapterInterface->GetStationLong();
}

int ProtocolNetVoiceRegStateAdapter::GetConCurrent() const {
    return protocolNetVoiceRegStateAdapterInterface->GetConCurrent();
}

int ProtocolNetVoiceRegStateAdapter::GetSystemId() const {
    return protocolNetVoiceRegStateAdapterInterface->GetSystemId();
}

int ProtocolNetVoiceRegStateAdapter::GetNetworkId() const {
    return protocolNetVoiceRegStateAdapterInterface->GetNetworkId();
}

int ProtocolNetVoiceRegStateAdapter::GetRoamingInd() const {
    return protocolNetVoiceRegStateAdapterInterface->GetRoamingInd();
}

int ProtocolNetVoiceRegStateAdapter::GetRegPrl() const {
    return protocolNetVoiceRegStateAdapterInterface->GetRegPrl();
}

int ProtocolNetVoiceRegStateAdapter::GetRoamingIndPrl() const {
    return protocolNetVoiceRegStateAdapterInterface->GetRoamingIndPrl();
}

int ProtocolNetVoiceRegStateAdapter::getChannelNumber() const {
    return protocolNetVoiceRegStateAdapterInterface->getChannelNumber();
}

long int ProtocolNetVoiceRegStateAdapter::getNrCid() const {
    return protocolNetVoiceRegStateAdapterInterface->getNrCid();
}

/**
 * ProtocolNetDataRegStateAdapter
 */
ProtocolNetDataRegStateAdapter::ProtocolNetDataRegStateAdapter(const ModemData *pModemData)
    : ProtocolNetDataRegStateAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetDataRegStateAdapterInterface = new ProtocolNetDataRegStateAdapterJson(pModemData);
    } else {
        protocolNetDataRegStateAdapterInterface = new ProtocolNetDataRegStateAdapterLegacy(pModemData);
    }
}

ProtocolNetDataRegStateAdapter::~ProtocolNetDataRegStateAdapter() {
    delete protocolNetDataRegStateAdapterInterface;
    protocolNetDataRegStateAdapterInterface = NULL;
}

int ProtocolNetDataRegStateAdapter::GetRegState() const {
    return protocolNetDataRegStateAdapterInterface->GetRegState();
}

int ProtocolNetDataRegStateAdapter::GetRejectCause() const {
    return protocolNetDataRegStateAdapterInterface->GetRejectCause();
}

int ProtocolNetDataRegStateAdapter::GetMaxSDC() const {
    return protocolNetDataRegStateAdapterInterface->GetMaxSDC();
}

int ProtocolNetDataRegStateAdapter::GetRadioTech() const {
    return protocolNetDataRegStateAdapterInterface->GetRadioTech();
}

int ProtocolNetDataRegStateAdapter::GetLAC() const {
    return protocolNetDataRegStateAdapterInterface->GetLAC();
}

int ProtocolNetDataRegStateAdapter::GetCellId() const {
    return protocolNetDataRegStateAdapterInterface->GetCellId();
}

int ProtocolNetDataRegStateAdapter::GetPSC() const {
    return protocolNetDataRegStateAdapterInterface->GetPSC();
}

int ProtocolNetDataRegStateAdapter::GetTAC() const {
    return protocolNetDataRegStateAdapterInterface->GetTAC();
}

int ProtocolNetDataRegStateAdapter::GetPCID() const {
    return protocolNetDataRegStateAdapterInterface->GetPCID();
}

int ProtocolNetDataRegStateAdapter::GetECI() const {
    return protocolNetDataRegStateAdapterInterface->GetECI();
}

int ProtocolNetDataRegStateAdapter::GetCSGID() const {
    return protocolNetDataRegStateAdapterInterface->GetCSGID();
}

int ProtocolNetDataRegStateAdapter::GetTADV() const {
    return protocolNetDataRegStateAdapterInterface->GetTADV();
}

int ProtocolNetDataRegStateAdapter::GetImsVops() const {
    return protocolNetDataRegStateAdapterInterface->GetImsVops();
}

int ProtocolNetDataRegStateAdapter::GetEmcService() const {
    return protocolNetDataRegStateAdapterInterface->GetEmcService();
}

int ProtocolNetDataRegStateAdapter::getChannelNumber() const {
    return protocolNetDataRegStateAdapterInterface->getChannelNumber();
}

bool ProtocolNetDataRegStateAdapter::IsEndcAvailable() const {
    return protocolNetDataRegStateAdapterInterface->IsEndcAvailable();
}

bool ProtocolNetDataRegStateAdapter::IsDcNrRestricted() const {
    return protocolNetDataRegStateAdapterInterface->IsDcNrRestricted();
}

bool ProtocolNetDataRegStateAdapter::IsNrAvailable() const {
    return protocolNetDataRegStateAdapterInterface->IsNrAvailable();
}

long int ProtocolNetDataRegStateAdapter::getNrCid() const {
    return protocolNetDataRegStateAdapterInterface->getNrCid();
}

int ProtocolNetDataRegStateAdapter::GetEmf() const {
    return protocolNetDataRegStateAdapterInterface->GetEmf();
}

/**
 * ProtocolNetOperatorAdapter
 */
ProtocolNetOperatorAdapter::ProtocolNetOperatorAdapter(const ModemData *pModemData)
    : ProtocolNetOperatorAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetOperatorAdapterInterface = new ProtocolNetOperatorAdapterJson(pModemData);
    } else {
        protocolNetOperatorAdapterInterface = new ProtocolNetOperatorAdapterLegacy(pModemData);
    }
}

ProtocolNetOperatorAdapter::~ProtocolNetOperatorAdapter() {
    delete protocolNetOperatorAdapterInterface;
    protocolNetOperatorAdapterInterface = NULL;
}

const char *ProtocolNetOperatorAdapter::GetPlmn() const {
    return protocolNetOperatorAdapterInterface->GetPlmn();
}
const char *ProtocolNetOperatorAdapter::GetShortPlmn() const {
    return protocolNetOperatorAdapterInterface->GetShortPlmn();
}
const char *ProtocolNetOperatorAdapter::GetLongPlmn() const {
    return protocolNetOperatorAdapterInterface->GetLongPlmn();
}
int ProtocolNetOperatorAdapter::GetRegState() const {
    return protocolNetOperatorAdapterInterface->GetRegState();
}
int ProtocolNetOperatorAdapter::GetLac() const {
    return protocolNetOperatorAdapterInterface->GetLac();
}

/**
 * ProtocolNetSelModeAdapter
 */
ProtocolNetSelModeAdapter::ProtocolNetSelModeAdapter(const ModemData *pModemData)
    : ProtocolNetSelModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetSelModeAdapterInterface = new ProtocolNetSelModeAdapterJson(pModemData);
    } else {
        protocolNetSelModeAdapterInterface = new ProtocolNetSelModeAdapterLegacy(pModemData);
    }
}

ProtocolNetSelModeAdapter::~ProtocolNetSelModeAdapter() {
    delete protocolNetSelModeAdapterInterface;
    protocolNetSelModeAdapterInterface = NULL;
}

int ProtocolNetSelModeAdapter::GetNetworkSelectionMode() const {
    return protocolNetSelModeAdapterInterface->GetNetworkSelectionMode();
}

/**
 * ProtocolGetRadioStateRespAdapter
 */
ProtocolGetRadioStateRespAdapter::ProtocolGetRadioStateRespAdapter(const ModemData *pModemData)
    : ProtocolGetRadioStateRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetRadioStateRespAdapterInterface = new ProtocolGetRadioStateRespAdapterJson(pModemData);
    } else {
        protocolGetRadioStateRespAdapterInterface = new ProtocolGetRadioStateRespAdapterLegacy(pModemData);
    }
}

ProtocolGetRadioStateRespAdapter::~ProtocolGetRadioStateRespAdapter() {
    delete protocolGetRadioStateRespAdapterInterface;
    protocolGetRadioStateRespAdapterInterface = NULL;
}

int ProtocolGetRadioStateRespAdapter::GetRadioState() const {
    return protocolGetRadioStateRespAdapterInterface->GetRadioState();
}

/**
 * ProtocolRadioPowerAdapter
 */
ProtocolRadioPowerAdapter::ProtocolRadioPowerAdapter(const ModemData *pModemData)
    : ProtocolRadioPowerAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolRadioPowerAdapterInterface = new ProtocolRadioPowerAdapterJson(pModemData);
    } else {
        protocolRadioPowerAdapterInterface = new ProtocolRadioPowerAdapterLegacy(pModemData);
    }
}

ProtocolRadioPowerAdapter::~ProtocolRadioPowerAdapter() {
    delete protocolRadioPowerAdapterInterface;
    protocolRadioPowerAdapterInterface = NULL;
}

UINT ProtocolRadioPowerAdapter::GetErrorCode() const {
    return protocolRadioPowerAdapterInterface->GetErrorCode();
}

/**
 * ProtocolRadioStateAdapter
 */
ProtocolRadioStateAdapter::ProtocolRadioStateAdapter(const ModemData *pModemData)
    : ProtocolRadioStateAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolRadioStateAdapterInterface = new ProtocolRadioStateAdapterJson(pModemData);
    } else {
        protocolRadioStateAdapterInterface = new ProtocolRadioStateAdapterLegacy(pModemData);
    }
}

ProtocolRadioStateAdapter::~ProtocolRadioStateAdapter() {
    delete protocolRadioStateAdapterInterface;
    protocolRadioStateAdapterInterface = NULL;
}

int ProtocolRadioStateAdapter::GetRadioState() const {
    return protocolRadioStateAdapterInterface->GetRadioState();
}

/**
 * ProtocolNetPrefNetTypeAdapter
 */
ProtocolNetPrefNetTypeAdapter::ProtocolNetPrefNetTypeAdapter(const ModemData *pModemData)
    : ProtocolNetPrefNetTypeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetPrefNetTypeAdapterInterface = new ProtocolNetPrefNetTypeAdapterJson(pModemData);
    } else {
        protocolNetPrefNetTypeAdapterInterface = new ProtocolNetPrefNetTypeAdapterLegacy(pModemData);
    }
}

ProtocolNetPrefNetTypeAdapter::~ProtocolNetPrefNetTypeAdapter() {
    delete protocolNetPrefNetTypeAdapterInterface;
    protocolNetPrefNetTypeAdapterInterface = NULL;
}

int ProtocolNetPrefNetTypeAdapter::GetPreferredNetworkType() const {
    return protocolNetPrefNetTypeAdapterInterface->GetPreferredNetworkType();
}

/**
 * ProtocolNetBandModeAdapter
 */
ProtocolNetBandModeAdapter::ProtocolNetBandModeAdapter(const ModemData *pModemData)
    : ProtocolNetBandModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetBandModeAdapterInterface = new ProtocolNetBandModeAdapterJson(pModemData);
    } else {
        protocolNetBandModeAdapterInterface = new ProtocolNetBandModeAdapterLegacy(pModemData);
    }
}

ProtocolNetBandModeAdapter::~ProtocolNetBandModeAdapter() {
    delete protocolNetBandModeAdapterInterface;
    protocolNetBandModeAdapterInterface = NULL;
}

int ProtocolNetBandModeAdapter::GetCount() const {
    return protocolNetBandModeAdapterInterface->GetCount();
}

const int *ProtocolNetBandModeAdapter::GetAvialableBandMode() const {
    return protocolNetBandModeAdapterInterface->GetAvialableBandMode();
}

/**
 * ProtocolNetAvailableNetworkAdapter
 */
ProtocolNetAvailableNetworkAdapter::ProtocolNetAvailableNetworkAdapter(const ModemData *pModemData)
    : ProtocolNetAvailableNetworkAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetAvailableNetworkAdapterInterface = new ProtocolNetAvailableNetworkAdapterJson(pModemData);
    } else {
        protocolNetAvailableNetworkAdapterInterface = new ProtocolNetAvailableNetworkAdapterLegacy(pModemData);
    }
}

ProtocolNetAvailableNetworkAdapter::~ProtocolNetAvailableNetworkAdapter() {
    delete protocolNetAvailableNetworkAdapterInterface;
    protocolNetAvailableNetworkAdapterInterface = NULL;
}

int ProtocolNetAvailableNetworkAdapter::GetCount() {
    return protocolNetAvailableNetworkAdapterInterface->GetCount();
}

bool ProtocolNetAvailableNetworkAdapter::GetNetwork(NetworkInfo &nwkInfo, int index, const char *simPlmn, char *simSpn) {
    return protocolNetAvailableNetworkAdapterInterface->GetNetwork(nwkInfo, index, simPlmn, simSpn);
}

/**
 * ProtocolNetGetPsServiceAdapter
 */
ProtocolNetGetPsServiceAdapter::ProtocolNetGetPsServiceAdapter(const ModemData *pModemData)
    : ProtocolNetGetPsServiceAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetPsServiceAdapterInterface = new ProtocolNetGetPsServiceAdapterJson(pModemData);
    } else {
        protocolNetGetPsServiceAdapterInterface = new ProtocolNetGetPsServiceAdapterLegacy(pModemData);
    }
}

ProtocolNetGetPsServiceAdapter::~ProtocolNetGetPsServiceAdapter() {
    delete protocolNetGetPsServiceAdapterInterface;
    protocolNetGetPsServiceAdapterInterface = NULL;
}

int ProtocolNetGetPsServiceAdapter::GetState() {
    return protocolNetGetPsServiceAdapterInterface->GetState();
}

/**
 * ProtocolNetDuplexModeRespAdapter
 */
ProtocolNetDuplexModeRespAdapter::ProtocolNetDuplexModeRespAdapter(const ModemData *pModemData)
    : ProtocolNetDuplexModeRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetDuplexModeRespAdapterInterface = new ProtocolNetDuplexModeRespAdapterJson(pModemData);
    } else {
        protocolNetDuplexModeRespAdapterInterface = new ProtocolNetDuplexModeRespAdapterLegacy(pModemData);
    }
}

ProtocolNetDuplexModeRespAdapter::~ProtocolNetDuplexModeRespAdapter() {
    delete protocolNetDuplexModeRespAdapterInterface;
    protocolNetDuplexModeRespAdapterInterface = NULL;
}

int ProtocolNetDuplexModeRespAdapter::Get4gDuplexMode() const {
    return protocolNetDuplexModeRespAdapterInterface->Get4gDuplexMode();
}

int ProtocolNetDuplexModeRespAdapter::Get3gDuplexMode() const {
    return protocolNetDuplexModeRespAdapterInterface->Get3gDuplexMode();
}

int ProtocolNetDuplexModeRespAdapter::GetDuplexMode() const {
    return protocolNetDuplexModeRespAdapterInterface->GetDuplexMode();
}

/**
 * ProtocolNetEmergencyActInfoAdapter
 */
ProtocolNetEmergencyActInfoAdapter::ProtocolNetEmergencyActInfoAdapter(const ModemData *pModemData)
    : ProtocolNetEmergencyActInfoAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetEmergencyActInfoAdapterInterface = new ProtocolNetEmergencyActInfoAdapterJson(pModemData);
    } else {
        protocolNetEmergencyActInfoAdapterInterface = new ProtocolNetEmergencyActInfoAdapterLegacy(pModemData);
    }
}

ProtocolNetEmergencyActInfoAdapter::~ProtocolNetEmergencyActInfoAdapter() {
    delete protocolNetEmergencyActInfoAdapterInterface;
    protocolNetEmergencyActInfoAdapterInterface = NULL;
}

int ProtocolNetEmergencyActInfoAdapter::GetRat() const {
    return protocolNetEmergencyActInfoAdapterInterface->GetRat();
}

int ProtocolNetEmergencyActInfoAdapter::GetActStatus() const {
    return protocolNetEmergencyActInfoAdapterInterface->GetActStatus();
}

/**
 * ProtocolNetMcSrchRespAdapter
 */
ProtocolNetMcSrchRespAdapter::ProtocolNetMcSrchRespAdapter(const ModemData *pModemData)
    : ProtocolNetMcSrchRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetMcSrchRespAdapterInterface = new ProtocolNetMcSrchRespAdapterJson(pModemData);
    } else {
        protocolNetMcSrchRespAdapterInterface = new ProtocolNetMcSrchRespAdapterLegacy(pModemData);
    }
}

ProtocolNetMcSrchRespAdapter::~ProtocolNetMcSrchRespAdapter() {
    delete protocolNetMcSrchRespAdapterInterface;
    protocolNetMcSrchRespAdapterInterface = NULL;
}

int ProtocolNetMcSrchRespAdapter::GetMcSrchResult() const {
    return protocolNetMcSrchRespAdapterInterface->GetMcSrchResult();
}

const char *ProtocolNetMcSrchRespAdapter::GetMcSrchPlmn() const {
    return protocolNetMcSrchRespAdapterInterface->GetMcSrchPlmn();
}

/**
 * ProtocolSetNetworkRCRespAdapter
 */
ProtocolSetNetworkRCRespAdapter::ProtocolSetNetworkRCRespAdapter(const ModemData *pModemData)
    : ProtocolSetNetworkRCRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSetNetworkRCRespAdapterInterface = new ProtocolSetNetworkRCRespAdapterJson(pModemData);
    } else {
        protocolSetNetworkRCRespAdapterInterface = new ProtocolSetNetworkRCRespAdapterLegacy(pModemData);
    }
}

ProtocolSetNetworkRCRespAdapter::~ProtocolSetNetworkRCRespAdapter() {
    delete protocolSetNetworkRCRespAdapterInterface;
    protocolSetNetworkRCRespAdapterInterface = NULL;
}

/**
 * ProtocolGetNetworkRCRespAdapter
 */
ProtocolGetNetworkRCRespAdapter::ProtocolGetNetworkRCRespAdapter(const ModemData *pModemData)
    : ProtocolGetNetworkRCRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetNetworkRCRespAdapterInterface = new ProtocolGetNetworkRCRespAdapterJson(pModemData);
    } else {
        protocolGetNetworkRCRespAdapterInterface = new ProtocolGetNetworkRCRespAdapterLegacy(pModemData);
    }
}

ProtocolGetNetworkRCRespAdapter::~ProtocolGetNetworkRCRespAdapter() {
    delete protocolGetNetworkRCRespAdapterInterface;
    protocolGetNetworkRCRespAdapterInterface = NULL;
}

int ProtocolGetNetworkRCRespAdapter::GetVersion() const {
    return protocolGetNetworkRCRespAdapterInterface->GetVersion();
}

int ProtocolGetNetworkRCRespAdapter::GetSession() const {
    return protocolGetNetworkRCRespAdapterInterface->GetSession();
}

int ProtocolGetNetworkRCRespAdapter::GetPhase() const {
    return protocolGetNetworkRCRespAdapterInterface->GetPhase();
}

int ProtocolGetNetworkRCRespAdapter::GetRafType() const {
    return protocolGetNetworkRCRespAdapterInterface->GetRafType();
}

BYTE *ProtocolGetNetworkRCRespAdapter::GetUuid() const {
    return protocolGetNetworkRCRespAdapterInterface->GetUuid();
}

int ProtocolGetNetworkRCRespAdapter::GetStatus() const {
    return protocolGetNetworkRCRespAdapterInterface->GetStatus();
}

/**
 * ProtocolNetworkRCIndAdapter
 */
ProtocolNetworkRCIndAdapter::ProtocolNetworkRCIndAdapter(const ModemData *pModemData)
    : ProtocolNetworkRCIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetworkRCIndAdapterInterface = new ProtocolNetworkRCIndAdapterJson(pModemData);
    } else {
        protocolNetworkRCIndAdapterInterface = new ProtocolNetworkRCIndAdapterLegacy(pModemData);
    }
}

ProtocolNetworkRCIndAdapter::~ProtocolNetworkRCIndAdapter() {
    delete protocolNetworkRCIndAdapterInterface;
    protocolNetworkRCIndAdapterInterface = NULL;
}

int ProtocolNetworkRCIndAdapter::GetVersion() const {
    return protocolNetworkRCIndAdapterInterface->GetVersion();
}

int ProtocolNetworkRCIndAdapter::GetSession() const {
    return protocolNetworkRCIndAdapterInterface->GetSession();
}

int ProtocolNetworkRCIndAdapter::GetPhase() const {
    return protocolNetworkRCIndAdapterInterface->GetPhase();
}

int ProtocolNetworkRCIndAdapter::GetRafType() const {
    return protocolNetworkRCIndAdapterInterface->GetRafType();
}

BYTE *ProtocolNetworkRCIndAdapter::GetUuid() const {
    return protocolNetworkRCIndAdapterInterface->GetUuid();
}

int ProtocolNetworkRCIndAdapter::GetStatus() const {
    return protocolNetworkRCIndAdapterInterface->GetStatus();
}

/*
 * ProtocolNetCdmaQueryRoamingTypeAdapter
 */
ProtocolNetCdmaQueryRoamingTypeAdapter::ProtocolNetCdmaQueryRoamingTypeAdapter(const ModemData *pModemData)
    : ProtocolNetCdmaQueryRoamingTypeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetCdmaQueryRoamingTypeAdapterInterface = new ProtocolNetCdmaQueryRoamingTypeAdapterJson(pModemData);
    } else {
        protocolNetCdmaQueryRoamingTypeAdapterInterface = new ProtocolNetCdmaQueryRoamingTypeAdapterLegacy(pModemData);
    }
}

ProtocolNetCdmaQueryRoamingTypeAdapter::~ProtocolNetCdmaQueryRoamingTypeAdapter() {
    delete protocolNetCdmaQueryRoamingTypeAdapterInterface;
    protocolNetCdmaQueryRoamingTypeAdapterInterface = NULL;
}

int ProtocolNetCdmaQueryRoamingTypeAdapter::QueryRoamingType() const {
    return protocolNetCdmaQueryRoamingTypeAdapterInterface->QueryRoamingType();
}

/*
 * ProtocolNetCdmaHybridModeAdapter
 */
ProtocolNetCdmaHybridModeAdapter::ProtocolNetCdmaHybridModeAdapter(const ModemData *pModemData)
    : ProtocolNetCdmaHybridModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetCdmaHybridModeAdapterInterface = new ProtocolNetCdmaHybridModeAdapterJson(pModemData);
    } else {
        protocolNetCdmaHybridModeAdapterInterface = new ProtocolNetCdmaHybridModeAdapterLegacy(pModemData);
    }
}

ProtocolNetCdmaHybridModeAdapter::~ProtocolNetCdmaHybridModeAdapter() {
    delete protocolNetCdmaHybridModeAdapterInterface;
    protocolNetCdmaHybridModeAdapterInterface = NULL;
}

int ProtocolNetCdmaHybridModeAdapter::GetCdmaHybridMode() const {
    return protocolNetCdmaHybridModeAdapterInterface->GetCdmaHybridMode();
}

/*
 * ProtocolNetMccAdapter
 */
ProtocolNetMccAdapter::ProtocolNetMccAdapter(const ModemData *pModemData)
    : ProtocolNetMccAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetMccAdapterInterface = new ProtocolNetMccAdapterJson(pModemData);
    } else {
        protocolNetMccAdapterInterface = new ProtocolNetMccAdapterLegacy(pModemData);
    }
}

ProtocolNetMccAdapter::~ProtocolNetMccAdapter() {
    delete protocolNetMccAdapterInterface;
    protocolNetMccAdapterInterface = NULL;
}

int ProtocolNetMccAdapter::GetCurrentPrefNetworkMode() const {
    return protocolNetMccAdapterInterface->GetCurrentPrefNetworkMode();
}

const char *ProtocolNetMccAdapter::GetMcc() const {
    return protocolNetMccAdapterInterface->GetMcc();
}

/**
 * ProtocolNetCellInfoListAdapter
 */
ProtocolNetCellInfoListAdapter::ProtocolNetCellInfoListAdapter(const ModemData *pModemData)
    : ProtocolNetCellInfoListAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetCellInfoListAdapterInterface = new ProtocolNetCellInfoListAdapterJson(pModemData);
    } else {
        protocolNetCellInfoListAdapterInterface = new ProtocolNetCellInfoListAdapterLegacy(pModemData);
    }
}

ProtocolNetCellInfoListAdapter::~ProtocolNetCellInfoListAdapter() {
    delete protocolNetCellInfoListAdapterInterface;
    protocolNetCellInfoListAdapterInterface = NULL;
}

list<RIL_CellInfo_V1_6> &ProtocolNetCellInfoListAdapter::GetCellInfoList(int cellInfoVer) {
    return protocolNetCellInfoListAdapterInterface->GetCellInfoList(cellInfoVer);
}

/**
 * ProtocolNetCellInfoListIndAdapter
 */
ProtocolNetCellInfoListIndAdapter::ProtocolNetCellInfoListIndAdapter(const ModemData *pModemData)
    : ProtocolNetCellInfoListIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetCellInfoListIndAdapterInterface = new ProtocolNetCellInfoListIndAdapterJson(pModemData);
    } else {
        protocolNetCellInfoListIndAdapterInterface = new ProtocolNetCellInfoListIndAdapterLegacy(pModemData);
    }
}

ProtocolNetCellInfoListIndAdapter::~ProtocolNetCellInfoListIndAdapter() {
    delete protocolNetCellInfoListIndAdapterInterface;
    protocolNetCellInfoListIndAdapterInterface = NULL;
}

list<RIL_CellInfo_V1_6> &ProtocolNetCellInfoListIndAdapter::GetCellInfoList(int cellInfoVer) {
    return protocolNetCellInfoListIndAdapterInterface->GetCellInfoList(cellInfoVer);
}

/**
 * ProtocolNetScanResultAdapter
 */
ProtocolNetScanResultAdapter::ProtocolNetScanResultAdapter(const ModemData *pModemData)
    : ProtocolNetScanResultAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetScanResultAdapterInterface = new ProtocolNetScanResultAdapterJson(pModemData);
    } else {
        protocolNetScanResultAdapterInterface = new ProtocolNetScanResultAdapterLegacy(pModemData);
    }
}

ProtocolNetScanResultAdapter::~ProtocolNetScanResultAdapter() {
    delete protocolNetScanResultAdapterInterface;
    protocolNetScanResultAdapterInterface = NULL;
}

int ProtocolNetScanResultAdapter::GetScanStatus() const {
    return protocolNetScanResultAdapterInterface->GetScanStatus();
}

int ProtocolNetScanResultAdapter::GetScanResult() const {
    return protocolNetScanResultAdapterInterface->GetScanResult();
}

list<RIL_CellInfo_V1_6> &ProtocolNetScanResultAdapter::GetCellInfoList(int cellInfoVer) {
    return protocolNetScanResultAdapterInterface->GetCellInfoList(cellInfoVer);
}

/**
 * ProtocolNetSimFileInfoAdapter
 */
ProtocolNetSimFileInfoAdapter::ProtocolNetSimFileInfoAdapter(const ModemData *pModemData)
    : ProtocolNetSimFileInfoAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetSimFileInfoAdapterInterface = new ProtocolNetSimFileInfoAdapterJson(pModemData);
    } else {
        protocolNetSimFileInfoAdapterInterface = new ProtocolNetSimFileInfoAdapterLegacy(pModemData);
    }
}

ProtocolNetSimFileInfoAdapter::~ProtocolNetSimFileInfoAdapter() {
    delete protocolNetSimFileInfoAdapterInterface;
    protocolNetSimFileInfoAdapterInterface = NULL;
}

int ProtocolNetSimFileInfoAdapter::GetSimFileId() const {
    return protocolNetSimFileInfoAdapterInterface->GetSimFileId();
}

int ProtocolNetSimFileInfoAdapter::GetRecordLen() const {
    return protocolNetSimFileInfoAdapterInterface->GetRecordLen();
}

int ProtocolNetSimFileInfoAdapter::GetNumOfRecords() const {
    return protocolNetSimFileInfoAdapterInterface->GetNumOfRecords();
}

BYTE **ProtocolNetSimFileInfoAdapter::GetSimFileData() const {
    return protocolNetSimFileInfoAdapterInterface->GetSimFileData();
}

/**
 * ProtocolNetPhysicalChannelConfigs
 */
ProtocolNetPhysicalChannelConfigs::ProtocolNetPhysicalChannelConfigs(const ModemData *pModemData)
    : ProtocolNetPhysicalChannelConfigsInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetPhysicalChannelConfigsInterface = new ProtocolNetPhysicalChannelConfigsJson(pModemData);
    } else {
        protocolNetPhysicalChannelConfigsInterface = new ProtocolNetPhysicalChannelConfigsLegacy(pModemData);
    }
}

ProtocolNetPhysicalChannelConfigs::~ProtocolNetPhysicalChannelConfigs() {
    delete protocolNetPhysicalChannelConfigsInterface;
    protocolNetPhysicalChannelConfigsInterface = NULL;
}

list<PhysicalChannelConfigsData> &ProtocolNetPhysicalChannelConfigs::GetConfigList() {
    return protocolNetPhysicalChannelConfigsInterface->GetConfigList();
}

const char *ProtocolNetPhysicalChannelConfigs::GetParameter() const {
    return protocolNetPhysicalChannelConfigsInterface->GetParameter();
}

UINT ProtocolNetPhysicalChannelConfigs::GetParameterLength() const {
    return protocolNetPhysicalChannelConfigsInterface->GetParameterLength();
}

/**
 * ProtocolNetGetManualRatModeAdapter
 */
ProtocolNetGetManualRatModeAdapter::ProtocolNetGetManualRatModeAdapter(const ModemData *pModemData)
    : ProtocolNetGetManualRatModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetManualRatModeAdapterInterface = new ProtocolNetGetManualRatModeAdapterJson(pModemData);
    } else {
        protocolNetGetManualRatModeAdapterInterface = new ProtocolNetGetManualRatModeAdapterLegacy(pModemData);
    }
}

ProtocolNetGetManualRatModeAdapter::~ProtocolNetGetManualRatModeAdapter() {
    delete protocolNetGetManualRatModeAdapterInterface;
    protocolNetGetManualRatModeAdapterInterface = NULL;
}

void ProtocolNetGetManualRatModeAdapter::GetManualRatMode(void *data) {
    return protocolNetGetManualRatModeAdapterInterface->GetManualRatMode(data);
}

/**
 * ProtocolNetSetManualRatModeAdapter
 */
ProtocolNetSetManualRatModeAdapter::ProtocolNetSetManualRatModeAdapter(const ModemData *pModemData)
    : ProtocolNetSetManualRatModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetSetManualRatModeAdapterInterface = new ProtocolNetSetManualRatModeAdapterJson(pModemData);
    } else {
        protocolNetSetManualRatModeAdapterInterface = new ProtocolNetSetManualRatModeAdapterLegacy(pModemData);
    }
}

ProtocolNetSetManualRatModeAdapter::~ProtocolNetSetManualRatModeAdapter() {
    delete protocolNetSetManualRatModeAdapterInterface;
    protocolNetSetManualRatModeAdapterInterface = NULL;
}

int ProtocolNetSetManualRatModeAdapter::GetCause() const {
    return protocolNetSetManualRatModeAdapterInterface->GetCause();
}

/**
 * ProtocolNetGetFreqLockAdapter
 */
ProtocolNetGetFreqLockAdapter::ProtocolNetGetFreqLockAdapter(const ModemData *pModemData)
    : ProtocolNetGetFreqLockAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetFreqLockAdapterInterface = new ProtocolNetGetFreqLockAdapterJson(pModemData);
    } else {
        protocolNetGetFreqLockAdapterInterface = new ProtocolNetGetFreqLockAdapterLegacy(pModemData);
    }
}

ProtocolNetGetFreqLockAdapter::~ProtocolNetGetFreqLockAdapter() {
    delete protocolNetGetFreqLockAdapterInterface;
    protocolNetGetFreqLockAdapterInterface = NULL;
}

void ProtocolNetGetFreqLockAdapter::GetFrequencyLock(void *data) {
    return protocolNetGetFreqLockAdapterInterface->GetFrequencyLock(data);
}

/**
 * ProtocolNetSetFreqLockAdapter
 */
ProtocolNetSetFreqLockAdapter::ProtocolNetSetFreqLockAdapter(const ModemData *pModemData)
    : ProtocolNetSetFreqLockAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetSetFreqLockAdapterInterface = new ProtocolNetSetFreqLockAdapterJson(pModemData);
    } else {
        protocolNetSetFreqLockAdapterInterface = new ProtocolNetSetFreqLockAdapterLegacy(pModemData);
    }
}

ProtocolNetSetFreqLockAdapter::~ProtocolNetSetFreqLockAdapter() {
    delete protocolNetSetFreqLockAdapterInterface;
    protocolNetSetFreqLockAdapterInterface = NULL;
}

int ProtocolNetSetFreqLockAdapter::GetResult() const {
    return protocolNetSetFreqLockAdapterInterface->GetResult();
}

/**
 * ProtocolNetGetEndcModeAdapter
 */
ProtocolNetGetEndcModeAdapter::ProtocolNetGetEndcModeAdapter(const ModemData *pModemData)
    : ProtocolNetGetEndcModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetEndcModeAdapterInterface = new ProtocolNetGetEndcModeAdapterJson(pModemData);
    } else {
        protocolNetGetEndcModeAdapterInterface = new ProtocolNetGetEndcModeAdapterLegacy(pModemData);
    }
}

ProtocolNetGetEndcModeAdapter::~ProtocolNetGetEndcModeAdapter() {
    delete protocolNetGetEndcModeAdapterInterface;
    protocolNetGetEndcModeAdapterInterface = NULL;
}

int ProtocolNetGetEndcModeAdapter::GetEndcMode() const {
    return protocolNetGetEndcModeAdapterInterface->GetEndcMode();
}

/**
 * ProtocolNetworkFrequencyInfoIndAdapter
 */
ProtocolNetworkFrequencyInfoIndAdapter::ProtocolNetworkFrequencyInfoIndAdapter(const ModemData *pModemData)
    : ProtocolNetworkFrequencyInfoIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetworkFrequencyInfoIndAdapterInterface = new ProtocolNetworkFrequencyInfoIndAdapterJson(pModemData);
    } else {
        protocolNetworkFrequencyInfoIndAdapterInterface = new ProtocolNetworkFrequencyInfoIndAdapterLegacy(pModemData);
    }
}

ProtocolNetworkFrequencyInfoIndAdapter::~ProtocolNetworkFrequencyInfoIndAdapter() {
    delete protocolNetworkFrequencyInfoIndAdapterInterface;
    protocolNetworkFrequencyInfoIndAdapterInterface = NULL;
}

int ProtocolNetworkFrequencyInfoIndAdapter::GetPrimaryRat() const {
    return protocolNetworkFrequencyInfoIndAdapterInterface->GetPrimaryRat();
}

int ProtocolNetworkFrequencyInfoIndAdapter::GetPrimaryBand() const {
    return protocolNetworkFrequencyInfoIndAdapterInterface->GetPrimaryBand();
}

int ProtocolNetworkFrequencyInfoIndAdapter::GetPrimaryFrequency() const {
    return protocolNetworkFrequencyInfoIndAdapterInterface->GetPrimaryFrequency();
}

int ProtocolNetworkFrequencyInfoIndAdapter::GetSecondaryRat() const {
    return protocolNetworkFrequencyInfoIndAdapterInterface->GetSecondaryRat();
}

int ProtocolNetworkFrequencyInfoIndAdapter::GetSecondaryBand() const {
    return protocolNetworkFrequencyInfoIndAdapterInterface->GetSecondaryBand();
}

int ProtocolNetworkFrequencyInfoIndAdapter::GetSecondaryFrequency() const {
    return protocolNetworkFrequencyInfoIndAdapterInterface->GetSecondaryFrequency();
}

/*
 * ProtocolNetAcBarringInfo
 */
ProtocolNetAcBarringInfo::ProtocolNetAcBarringInfo(const ModemData *pModemData)
    : ProtocolNetAcBarringInfoInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetAcBarringInfoInterface = new ProtocolNetAcBarringInfoJson(pModemData);
    } else {
        protocolNetAcBarringInfoInterface = new ProtocolNetAcBarringInfoLegacy(pModemData);
    }
}

ProtocolNetAcBarringInfo::~ProtocolNetAcBarringInfo() {
    delete protocolNetAcBarringInfoInterface;
    protocolNetAcBarringInfoInterface = NULL;
}

void ProtocolNetAcBarringInfo::GetAcBarringInfo(void *data, unsigned int size) {
    return protocolNetAcBarringInfoInterface->GetAcBarringInfo(data, size);
}

/*
 * ProtocolNetGetFrequencyInfoAdapter
 */
ProtocolNetGetFrequencyInfoAdapter::ProtocolNetGetFrequencyInfoAdapter(const ModemData *pModemData)
    : ProtocolNetGetFrequencyInfoAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetFrequencyInfoAdapterInterface = new ProtocolNetGetFrequencyInfoAdapterJson(pModemData);
    } else {
        protocolNetGetFrequencyInfoAdapterInterface = new ProtocolNetGetFrequencyInfoAdapterLegacy(pModemData);
    }
}

ProtocolNetGetFrequencyInfoAdapter::~ProtocolNetGetFrequencyInfoAdapter() {
    delete protocolNetGetFrequencyInfoAdapterInterface;
    protocolNetGetFrequencyInfoAdapterInterface = NULL;
}

int ProtocolNetGetFrequencyInfoAdapter::GetPrimaryRat() const {
    return protocolNetGetFrequencyInfoAdapterInterface->GetPrimaryRat();
}

int ProtocolNetGetFrequencyInfoAdapter::GetPrimaryBand() const {
    return protocolNetGetFrequencyInfoAdapterInterface->GetPrimaryBand();
}

int ProtocolNetGetFrequencyInfoAdapter::GetPrimaryFrequency() const {
    return protocolNetGetFrequencyInfoAdapterInterface->GetPrimaryFrequency();
}

int ProtocolNetGetFrequencyInfoAdapter::GetSecondaryRat() const {
    return protocolNetGetFrequencyInfoAdapterInterface->GetSecondaryRat();
}

int ProtocolNetGetFrequencyInfoAdapter::GetSecondaryBand() const {
    return protocolNetGetFrequencyInfoAdapterInterface->GetSecondaryBand();
}

int ProtocolNetGetFrequencyInfoAdapter::GetSecondaryFrequency() const {
    return protocolNetGetFrequencyInfoAdapterInterface->GetSecondaryFrequency();
}

/*
 * ProtocolNetRrcInfoAdapter
 */
ProtocolNetRrcInfoAdapter::ProtocolNetRrcInfoAdapter(const ModemData *pModemData)
    : ProtocolNetRrcInfoAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetRrcInfoAdapterInterface = new ProtocolNetRrcInfoAdapterJson(pModemData);
    } else {
        protocolNetRrcInfoAdapterInterface = new ProtocolNetRrcInfoAdapterLegacy(pModemData);
    }
}

ProtocolNetRrcInfoAdapter::~ProtocolNetRrcInfoAdapter() {
    delete protocolNetRrcInfoAdapterInterface;
    protocolNetRrcInfoAdapterInterface = NULL;
}

int ProtocolNetRrcInfoAdapter::GetRat() const {
    return protocolNetRrcInfoAdapterInterface->GetRat();
}

int ProtocolNetRrcInfoAdapter::GetState() const {
    return protocolNetRrcInfoAdapterInterface->GetState();
}

/**
 * ProtocolNetGetNrModeAdapter
 */
ProtocolNetGetNrModeAdapter::ProtocolNetGetNrModeAdapter(const ModemData *pModemData)
    : ProtocolNetGetNrModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetNrModeAdapterInterface = new ProtocolNetGetNrModeAdapterJson(pModemData);
    } else {
        protocolNetGetNrModeAdapterInterface = new ProtocolNetGetNrModeAdapterLegacy(pModemData);
    }
}

ProtocolNetGetNrModeAdapter::~ProtocolNetGetNrModeAdapter() {
    delete protocolNetGetNrModeAdapterInterface;
    protocolNetGetNrModeAdapterInterface = NULL;
}

int ProtocolNetGetNrModeAdapter::GetNrMode() const {
    return protocolNetGetNrModeAdapterInterface->GetNrMode();
}

/**
 * ProtocolNetGetNrSilenceModeAdapter
 */
ProtocolNetGetNrSilenceModeAdapter::ProtocolNetGetNrSilenceModeAdapter(const ModemData *pModemData)
    : ProtocolNetGetNrSilenceModeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetNrSilenceModeAdapterInterface = new ProtocolNetGetNrSilenceModeAdapterJson(pModemData);
    } else {
        protocolNetGetNrSilenceModeAdapterInterface = new ProtocolNetGetNrSilenceModeAdapterLegacy(pModemData);
    }
}

ProtocolNetGetNrSilenceModeAdapter::~ProtocolNetGetNrSilenceModeAdapter() {
    delete protocolNetGetNrSilenceModeAdapterInterface;
    protocolNetGetNrSilenceModeAdapterInterface = NULL;
}

bool ProtocolNetGetNrSilenceModeAdapter::GetNsaMode() const {
    return protocolNetGetNrSilenceModeAdapterInterface->GetNsaMode();
}

bool ProtocolNetGetNrSilenceModeAdapter::GetSaMode() const {
    return protocolNetGetNrSilenceModeAdapterInterface->GetSaMode();
}

/*
 * ProtocolRegistrationFailedAdapter
 */
ProtocolRegistrationFailedAdapter::ProtocolRegistrationFailedAdapter(const ModemData *pModemData, int cellIdentityVer = 3)
    : ProtocolRegistrationFailedAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolRegistrationFailedAdapterInterface =
            new ProtocolRegistrationFailedAdapterJson(pModemData, cellIdentityVer);
    } else {
        protocolRegistrationFailedAdapterInterface =
            new ProtocolRegistrationFailedAdapterLegacy(pModemData, cellIdentityVer);
    }
}

ProtocolRegistrationFailedAdapter::~ProtocolRegistrationFailedAdapter() {
    delete protocolRegistrationFailedAdapterInterface;
    protocolRegistrationFailedAdapterInterface = NULL;
}

RIL_RegistrationFailed *ProtocolRegistrationFailedAdapter::GetRegistrationFailed() {
    return protocolRegistrationFailedAdapterInterface->GetRegistrationFailed();
}

/**
 * ProtocolGetBarringInfoRspAdapter
 */
ProtocolGetBarringInfoRspAdapter::ProtocolGetBarringInfoRspAdapter(const ModemData *pModemData, int cellIdentityVer = 3)
    : ProtocolGetBarringInfoRspAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolGetBarringInfoRspAdapterInterface = new ProtocolGetBarringInfoRspAdapterJson(pModemData, cellIdentityVer);
    } else {
        protocolGetBarringInfoRspAdapterInterface = new ProtocolGetBarringInfoRspAdapterLegacy(pModemData, cellIdentityVer);
    }
}

ProtocolGetBarringInfoRspAdapter::~ProtocolGetBarringInfoRspAdapter() {
    delete protocolGetBarringInfoRspAdapterInterface;
    protocolGetBarringInfoRspAdapterInterface = NULL;
}

RIL_CellIdAndBarringInfo *ProtocolGetBarringInfoRspAdapter::GetCellIdAndBarringInfo() {
    return protocolGetBarringInfoRspAdapterInterface->GetCellIdAndBarringInfo();
}

/**
  * ProtocolBarringInfoChangedAdapter
  */
ProtocolBarringInfoChangedAdapter::ProtocolBarringInfoChangedAdapter(const ModemData *pModemData, int cellIdentityVer = 3)
    : ProtocolBarringInfoChangedAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolBarringInfoChangedAdapterInterface = new ProtocolBarringInfoChangedAdapterJson(pModemData, cellIdentityVer);
    } else {
        protocolBarringInfoChangedAdapterInterface = new ProtocolBarringInfoChangedAdapterLegacy(pModemData, cellIdentityVer);
    }
}

ProtocolBarringInfoChangedAdapter::~ProtocolBarringInfoChangedAdapter() {
    delete protocolBarringInfoChangedAdapterInterface;
    protocolBarringInfoChangedAdapterInterface = NULL;
}

RIL_CellIdAndBarringInfo *ProtocolBarringInfoChangedAdapter::GetCellIdAndBarringInfo() {
    return protocolBarringInfoChangedAdapterInterface->GetCellIdAndBarringInfo();
}

/**
 * ProtocolNetGetVonrCapaAdapter
 */
ProtocolNetGetVonrCapaAdapter::ProtocolNetGetVonrCapaAdapter(const ModemData *pModemData)
    : ProtocolNetGetVonrCapaAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetVonrCapaAdapterInterface = new ProtocolNetGetVonrCapaAdapterJson(pModemData);
    } else {
        protocolNetGetVonrCapaAdapterInterface = new ProtocolNetGetVonrCapaAdapterLegacy(pModemData);
    }
}

ProtocolNetGetVonrCapaAdapter::~ProtocolNetGetVonrCapaAdapter() {
    delete protocolNetGetVonrCapaAdapterInterface;
    protocolNetGetVonrCapaAdapterInterface = NULL;
}

int ProtocolNetGetVonrCapaAdapter::GetVonrCapa() const {
    return protocolNetGetVonrCapaAdapterInterface->GetVonrCapa();
}

/*
 * ProtocolNetGetAllowNetworkAdapter
 */
ProtocolNetGetAllowNetworkAdapter::ProtocolNetGetAllowNetworkAdapter(const ModemData *pModemData)
    : ProtocolNetGetAllowNetworkAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetAllowNetworkAdapterInterface = new ProtocolNetGetAllowNetworkAdapterJson(pModemData);
    } else {
        protocolNetGetAllowNetworkAdapterInterface = new ProtocolNetGetAllowNetworkAdapterLegacy(pModemData);
    }
}

ProtocolNetGetAllowNetworkAdapter::~ProtocolNetGetAllowNetworkAdapter() {
    delete protocolNetGetAllowNetworkAdapterInterface;
    protocolNetGetAllowNetworkAdapterInterface = NULL;
}

UINT32 ProtocolNetGetAllowNetworkAdapter::GetRat() const {
    return protocolNetGetAllowNetworkAdapterInterface->GetRat();
}

/**
 * ProtocolNetSystemSelectionChannelsAdapter
 */
ProtocolNetSystemSelectionChannelsAdapter::ProtocolNetSystemSelectionChannelsAdapter(const ModemData *pModemData)
    : ProtocolNetSystemSelectionChannelsAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetSystemSelectionChannelsAdapterInterface = new ProtocolNetSystemSelectionChannelsAdapterJson(pModemData);
    } else {
        protocolNetSystemSelectionChannelsAdapterInterface = new ProtocolNetSystemSelectionChannelsAdapterLegacy(pModemData);
    }
}

ProtocolNetSystemSelectionChannelsAdapter::~ProtocolNetSystemSelectionChannelsAdapter() {
    delete protocolNetSystemSelectionChannelsAdapterInterface;
    protocolNetSystemSelectionChannelsAdapterInterface = NULL;
}

int ProtocolNetSystemSelectionChannelsAdapter::GetSpecifiersLength() {
    return protocolNetSystemSelectionChannelsAdapterInterface->GetSpecifiersLength();
}

RIL_RadioAccessSpecifier_V1_5 *ProtocolNetSystemSelectionChannelsAdapter::GetRadioAccessSpecifier() {
    return protocolNetSystemSelectionChannelsAdapterInterface->GetRadioAccessSpecifier();
}

/**
 * ProtocolNetB1B2ConfigIndAdapter
 */
ProtocolNetB1B2ConfigIndAdapter::ProtocolNetB1B2ConfigIndAdapter(const ModemData *pModemData) {
    if (IsPayloadTypeJson()) {
        protocolNetB1B2ConfigIndAdapterInterface = new ProtocolNetB1B2ConfigIndAdapterJson(pModemData);
    } else {
        protocolNetB1B2ConfigIndAdapterInterface = new ProtocolNetB1B2ConfigIndAdapterLegacy(pModemData);
    }
}

ProtocolNetB1B2ConfigIndAdapter::~ProtocolNetB1B2ConfigIndAdapter() {
    delete protocolNetB1B2ConfigIndAdapterInterface;
    protocolNetB1B2ConfigIndAdapterInterface = NULL;
}


/**
 * ProtocolNetGetDeviceServiceAdapter
 */
ProtocolNetGetDeviceServiceAdapter::ProtocolNetGetDeviceServiceAdapter(const ModemData *pModemData)
    : ProtocolNetGetDeviceServiceAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolNetGetDeviceServiceAdapterInterface = new ProtocolNetGetDeviceServiceAdapterJson(pModemData);
    } else {
        protocolNetGetDeviceServiceAdapterInterface = new ProtocolNetGetDeviceServiceAdapterLegacy(pModemData);
    }
}

ProtocolNetGetDeviceServiceAdapter::~ProtocolNetGetDeviceServiceAdapter() {
    delete protocolNetGetDeviceServiceAdapterInterface;
    protocolNetGetDeviceServiceAdapterInterface = NULL;
}

int ProtocolNetGetDeviceServiceAdapter::GetUsageSetting() const {
    return protocolNetGetDeviceServiceAdapterInterface->GetUsageSetting();
}

/**
 * ProtocolNetDsdsStatusIndAdapter
 */
ProtocolNetDsdsStatusIndAdapter::ProtocolNetDsdsStatusIndAdapter(const ModemData *pModemData)
    : ProtocolNetDsdsStatusIndAdapterInterface() {
    /*
    if (IsPayloadTypeJson()) {
        protocolNetDsdsStatusIndAdapterInterface = new ProtocolNetDsdsStatusIndAdapterJson(pModemData);
    } else*/ {
        protocolNetDsdsStatusIndAdapterInterface = new ProtocolNetDsdsStatusIndAdapterLegacy(pModemData);
    }
}

ProtocolNetDsdsStatusIndAdapter::~ProtocolNetDsdsStatusIndAdapter() {
    delete protocolNetDsdsStatusIndAdapterInterface;
    protocolNetDsdsStatusIndAdapterInterface = nullptr;
}

bool ProtocolNetDsdsStatusIndAdapter::IsPsRestricted() const {
    return protocolNetDsdsStatusIndAdapterInterface->IsPsRestricted();
}
