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
 * protocolsimadapter.cpp
 *
 *  Created on: 2021. 06. 09.
 *      Author: aryaman.2019
 */

#include "protocolsimadapter.h"
#include "legacy/uicc/protocolsimadapterlegacy.h"
#include "json/protocolsimadapterjson.h"
/**
 * ProtocolSimResponseAdapter
 */
ProtocolSimResponseAdapter::ProtocolSimResponseAdapter(const ModemData *pModemData)
    : ProtocolSimResponseAdapterInterface() {
    protocolSimResponseAdapterInterface = new ProtocolSimResponseAdapterLegacy(pModemData);
}

ProtocolSimResponseAdapter::~ProtocolSimResponseAdapter() {
    delete protocolSimResponseAdapterInterface;
    protocolSimResponseAdapterInterface = NULL;
}

/**
 * ProtocolSimStatusAdapter
 */
ProtocolSimStatusAdapter::ProtocolSimStatusAdapter(const ModemData *pModemData)
    : ProtocolSimStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimStatusAdapterInterface = new ProtocolSimStatusAdapterJson(pModemData);
    } else {
        protocolSimStatusAdapterInterface = new ProtocolSimStatusAdapterLegacy(pModemData);
    }
}

ProtocolSimStatusAdapter::~ProtocolSimStatusAdapter() {
    delete protocolSimStatusAdapterInterface;
    protocolSimStatusAdapterInterface = NULL;
}

int ProtocolSimStatusAdapter::GetCardState() const {
    return protocolSimStatusAdapterInterface->GetCardState();
}

int ProtocolSimStatusAdapter::GetUniversalPinState() const {
    return protocolSimStatusAdapterInterface->GetUniversalPinState();
}

int ProtocolSimStatusAdapter::GetApplicationCount() const {
    return protocolSimStatusAdapterInterface->GetApplicationCount();
}

int ProtocolSimStatusAdapter::GetAppsType(int nIndex) const {
    return protocolSimStatusAdapterInterface->GetAppsType(nIndex);
}

int ProtocolSimStatusAdapter::GetAppsState(int nIndex) const {
    return protocolSimStatusAdapterInterface->GetAppsState(nIndex);
}

int ProtocolSimStatusAdapter::GetPersonalSubstate(int nIndex) const {
    return protocolSimStatusAdapterInterface->GetPersonalSubstate(nIndex);
}

char *ProtocolSimStatusAdapter::GetAID(int nIndex) const {
    return protocolSimStatusAdapterInterface->GetAID(nIndex);
}

int ProtocolSimStatusAdapter::GetApplicationLabel(int nIndex, BYTE *pAppLabel) const {
    return protocolSimStatusAdapterInterface->GetApplicationLabel(nIndex, pAppLabel);
}

int ProtocolSimStatusAdapter::GetPin1Replaced(int nIndex) const {
    return protocolSimStatusAdapterInterface->GetPin1Replaced(nIndex);
}

int ProtocolSimStatusAdapter::GetPinState(int nIndex, int nPinIndex) const {
    return protocolSimStatusAdapterInterface->GetPinState(nIndex, nPinIndex);
}

int ProtocolSimStatusAdapter::GetPinRemainCount(int nIndex, int nPinIndex) const {
    return protocolSimStatusAdapterInterface->GetPinRemainCount(nIndex, nPinIndex);
}

int ProtocolSimStatusAdapter::GetPukRemainCount(int nIndex, int nPukIndex) const {
    return protocolSimStatusAdapterInterface->GetPukRemainCount(nIndex, nPukIndex);
}

bool ProtocolSimStatusAdapter::GetEsimNoProfile() const {
    return protocolSimStatusAdapterInterface->GetEsimNoProfile();
}

int ProtocolSimStatusAdapter::GetPhysicalSlotId() const {
    return protocolSimStatusAdapterInterface->GetPhysicalSlotId();
}

int ProtocolSimStatusAdapter::GetAtrLength() const {
    return protocolSimStatusAdapterInterface->GetAtrLength();
}

char *ProtocolSimStatusAdapter::GetAtr() const {
    return protocolSimStatusAdapterInterface->GetAtr();
}

int ProtocolSimStatusAdapter::GetIccidLength() const {
    return protocolSimStatusAdapterInterface->GetIccidLength();
}

char *ProtocolSimStatusAdapter::GetIccid() const {
    return protocolSimStatusAdapterInterface->GetIccid();
}

int ProtocolSimStatusAdapter::GetEidLength() const {
    return protocolSimStatusAdapterInterface->GetEidLength();
}

char *ProtocolSimStatusAdapter::GetEid() const {
    return protocolSimStatusAdapterInterface->GetEid();
}

/**
 * ProtocolSimVerifyPinAdapter
 */
ProtocolSimVerifyPinAdapter::ProtocolSimVerifyPinAdapter(const ModemData *pModemData)
    : ProtocolSimVerifyPinAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimVerifyPinAdapterInterface = new ProtocolSimVerifyPinAdapterJson(pModemData);
    } else {
        protocolSimVerifyPinAdapterInterface = new ProtocolSimVerifyPinAdapterLegacy(pModemData);
    }
}

ProtocolSimVerifyPinAdapter::~ProtocolSimVerifyPinAdapter() {
    delete protocolSimVerifyPinAdapterInterface;
    protocolSimVerifyPinAdapterInterface = NULL;
}

int ProtocolSimVerifyPinAdapter::GetPinIndex() const {
    return protocolSimVerifyPinAdapterInterface->GetPinIndex();
}

int ProtocolSimVerifyPinAdapter::GetRemainCount() const {
    return protocolSimVerifyPinAdapterInterface->GetRemainCount();
}

/**
 * ProtocolSimVerifyPukAdapter
 */
ProtocolSimVerifyPukAdapter::ProtocolSimVerifyPukAdapter(const ModemData *pModemData)
    : ProtocolSimVerifyPukAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimVerifyPukAdapterInterface = new ProtocolSimVerifyPukAdapterJson(pModemData);
    } else {
        protocolSimVerifyPukAdapterInterface = new ProtocolSimVerifyPukAdapterLegacy(pModemData);
    }
}

ProtocolSimVerifyPukAdapter::~ProtocolSimVerifyPukAdapter() {
    delete protocolSimVerifyPukAdapterInterface;
    protocolSimVerifyPukAdapterInterface = NULL;
}

int ProtocolSimVerifyPukAdapter::GetPukIndex() const {
    return protocolSimVerifyPukAdapterInterface->GetPukIndex();
}

int ProtocolSimVerifyPukAdapter::GetRemainCount() const {
    return protocolSimVerifyPukAdapterInterface->GetRemainCount();
}

/**
 * ProtocolSimVerifyNetLockAdapter
 */
ProtocolSimVerifyNetLockAdapter::ProtocolSimVerifyNetLockAdapter(const ModemData *pModemData)
    : ProtocolSimVerifyNetLockAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimVerifyNetLockAdapterInterface = new ProtocolSimVerifyNetLockAdapterJson(pModemData);
    } else {
        protocolSimVerifyNetLockAdapterInterface = new ProtocolSimVerifyNetLockAdapterLegacy(pModemData);
    }
}

ProtocolSimVerifyNetLockAdapter::~ProtocolSimVerifyNetLockAdapter() {
    delete protocolSimVerifyNetLockAdapterInterface;
    protocolSimVerifyNetLockAdapterInterface = NULL;
}

int ProtocolSimVerifyNetLockAdapter::GetRemainCount() const {
    return protocolSimVerifyNetLockAdapterInterface->GetRemainCount();
}

/**
 * ProtocolSimIOAdapter
 */
ProtocolSimIOAdapter::ProtocolSimIOAdapter(const ModemData *pModemData)
    : ProtocolSimIOAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimIOAdapterInterface = new ProtocolSimIOAdapterJson(pModemData);
    } else {
        protocolSimIOAdapterInterface = new ProtocolSimIOAdapterLegacy(pModemData);
    }
}

ProtocolSimIOAdapter::~ProtocolSimIOAdapter() {
    delete protocolSimIOAdapterInterface;
    protocolSimIOAdapterInterface = NULL;
}

BYTE ProtocolSimIOAdapter::GetSw1() const {
    return protocolSimIOAdapterInterface->GetSw1();
}

BYTE ProtocolSimIOAdapter::GetSw2() const {
    return protocolSimIOAdapterInterface->GetSw2();
}

BYTE *ProtocolSimIOAdapter::GetResponse() const {
    return protocolSimIOAdapterInterface->GetResponse();
}

int ProtocolSimIOAdapter::GetResponseLength() const {
    return protocolSimIOAdapterInterface->GetResponseLength();
}

/**
 * ProtocolSimGetFacilityLockAdapter
 */
ProtocolSimGetFacilityLockAdapter::ProtocolSimGetFacilityLockAdapter(const ModemData *pModemData)
    : ProtocolSimGetFacilityLockAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimGetFacilityLockAdapterInterface = new ProtocolSimGetFacilityLockAdapterJson(pModemData);
    } else {
        protocolSimGetFacilityLockAdapterInterface = new ProtocolSimGetFacilityLockAdapterLegacy(pModemData);
    }
}

ProtocolSimGetFacilityLockAdapter::~ProtocolSimGetFacilityLockAdapter() {
    delete protocolSimGetFacilityLockAdapterInterface;
    protocolSimGetFacilityLockAdapterInterface = NULL;
}

int ProtocolSimGetFacilityLockAdapter::GetServiceClass() const {
    return protocolSimGetFacilityLockAdapterInterface->GetServiceClass();
}

/**
 * ProtocolSimSetFacilityLockAdapter
 */
ProtocolSimSetFacilityLockAdapter::ProtocolSimSetFacilityLockAdapter(const ModemData *pModemData)
    : ProtocolSimSetFacilityLockAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimSetFacilityLockAdapterInterface = new ProtocolSimSetFacilityLockAdapterJson(pModemData);
    } else {
        protocolSimSetFacilityLockAdapterInterface = new ProtocolSimSetFacilityLockAdapterLegacy(pModemData);
    }
}

ProtocolSimSetFacilityLockAdapter::~ProtocolSimSetFacilityLockAdapter() {
    delete protocolSimSetFacilityLockAdapterInterface;
    protocolSimSetFacilityLockAdapterInterface = NULL;
}

int ProtocolSimSetFacilityLockAdapter::GetRemainCount() const {
    return protocolSimSetFacilityLockAdapterInterface->GetRemainCount();
}

/**
 * ProtocolSimGetSimAuthAdapter
 */
ProtocolSimGetSimAuthAdapter::ProtocolSimGetSimAuthAdapter(const ModemData *pModemData)
    : ProtocolSimGetSimAuthAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimGetSimAuthAdapterInterface = new ProtocolSimGetSimAuthAdapterJson(pModemData);
    } else {
        protocolSimGetSimAuthAdapterInterface = new ProtocolSimGetSimAuthAdapterLegacy(pModemData);
    }
}

ProtocolSimGetSimAuthAdapter::~ProtocolSimGetSimAuthAdapter() {
    delete protocolSimGetSimAuthAdapterInterface;
    protocolSimGetSimAuthAdapterInterface = NULL;
}

int ProtocolSimGetSimAuthAdapter::GetAuthType() const {
    return protocolSimGetSimAuthAdapterInterface->GetAuthType();
}

int ProtocolSimGetSimAuthAdapter::GetAuthLength() const {
    return protocolSimGetSimAuthAdapterInterface->GetAuthLength();
}

BYTE *ProtocolSimGetSimAuthAdapter::GetAuth() const {
    return protocolSimGetSimAuthAdapterInterface->GetAuth();
}

/**
 * ProtocolSimTransmitApduBasicAdapter
 */
ProtocolSimTransmitApduBasicAdapter::ProtocolSimTransmitApduBasicAdapter(const ModemData *pModemData)
    : ProtocolSimTransmitApduBasicAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimTransmitApduBasicAdapterInterface = new ProtocolSimTransmitApduBasicAdapterJson(pModemData);
    } else {
        protocolSimTransmitApduBasicAdapterInterface = new ProtocolSimTransmitApduBasicAdapterLegacy(pModemData);
    }
}

ProtocolSimTransmitApduBasicAdapter::~ProtocolSimTransmitApduBasicAdapter() {
    delete protocolSimTransmitApduBasicAdapterInterface;
    protocolSimTransmitApduBasicAdapterInterface = NULL;
}

BYTE ProtocolSimTransmitApduBasicAdapter::GetSw1() const {
    return protocolSimTransmitApduBasicAdapterInterface->GetSw1();
}

BYTE ProtocolSimTransmitApduBasicAdapter::GetSw2() const {
    return protocolSimTransmitApduBasicAdapterInterface->GetSw2();
}

int ProtocolSimTransmitApduBasicAdapter::GetApduLength() const {
    return protocolSimTransmitApduBasicAdapterInterface->GetApduLength();
}

BYTE *ProtocolSimTransmitApduBasicAdapter::GetApdu() const {
    return protocolSimTransmitApduBasicAdapterInterface->GetApdu();
}

/**
 * ProtocolSimOpenChannelAdapter
 */
ProtocolSimOpenChannelAdapter::ProtocolSimOpenChannelAdapter(const ModemData *pModemData)
    : ProtocolSimOpenChannelAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimOpenChannelAdapterInterface = new ProtocolSimOpenChannelAdapterJson(pModemData);
    } else {
        protocolSimOpenChannelAdapterInterface = new ProtocolSimOpenChannelAdapterLegacy(pModemData);
    }
}

ProtocolSimOpenChannelAdapter::~ProtocolSimOpenChannelAdapter() {
    delete protocolSimOpenChannelAdapterInterface;
    protocolSimOpenChannelAdapterInterface = NULL;
}

int ProtocolSimOpenChannelAdapter::GetSessionID() const {
    return protocolSimOpenChannelAdapterInterface->GetSessionID();
}

BYTE ProtocolSimOpenChannelAdapter::GetSw1() const {
    return protocolSimOpenChannelAdapterInterface->GetSw1();
}

BYTE ProtocolSimOpenChannelAdapter::GetSw2() const {
    return protocolSimOpenChannelAdapterInterface->GetSw2();
}

BYTE *ProtocolSimOpenChannelAdapter::GetResponse() const {
    return protocolSimOpenChannelAdapterInterface->GetResponse();
}

int ProtocolSimOpenChannelAdapter::GetResponseLength() const {
    return protocolSimOpenChannelAdapterInterface->GetResponseLength();
}

/**
 * ProtocolSimTransmitApduChannelAdapter
 */
ProtocolSimTransmitApduChannelAdapter::ProtocolSimTransmitApduChannelAdapter(const ModemData *pModemData)
    : ProtocolSimTransmitApduChannelAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimTransmitApduChannelAdapterInterface = new ProtocolSimTransmitApduChannelAdapterJson(pModemData);
    } else {
        protocolSimTransmitApduChannelAdapterInterface = new ProtocolSimTransmitApduChannelAdapterLegacy(pModemData);
    }
}

ProtocolSimTransmitApduChannelAdapter::~ProtocolSimTransmitApduChannelAdapter() {
    delete protocolSimTransmitApduChannelAdapterInterface;
    protocolSimTransmitApduChannelAdapterInterface = NULL;
}

BYTE ProtocolSimTransmitApduChannelAdapter::GetSw1() const {
    return protocolSimTransmitApduChannelAdapterInterface->GetSw1();
}

BYTE ProtocolSimTransmitApduChannelAdapter::GetSw2() const {
    return protocolSimTransmitApduChannelAdapterInterface->GetSw2();
}

int ProtocolSimTransmitApduChannelAdapter::GetApduLength() const {
    return protocolSimTransmitApduChannelAdapterInterface->GetApduLength();
}

BYTE *ProtocolSimTransmitApduChannelAdapter::GetApdu() const {
    return protocolSimTransmitApduChannelAdapterInterface->GetApdu();
}

/**
 * ProtocolSimImsiAdapter
 */
ProtocolSimImsiAdapter::ProtocolSimImsiAdapter(const ModemData *pModemData)
    : ProtocolSimImsiAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimImsiAdapterInterface = new ProtocolSimImsiAdapterJson(pModemData);
    } else {
        protocolSimImsiAdapterInterface = new ProtocolSimImsiAdapterLegacy(pModemData);
    }
}

ProtocolSimImsiAdapter::~ProtocolSimImsiAdapter() {
    delete protocolSimImsiAdapterInterface;
    protocolSimImsiAdapterInterface = NULL;
}

const char *ProtocolSimImsiAdapter::GetImsi() const {
    return protocolSimImsiAdapterInterface->GetImsi();
}

/**
 * ProtocolSimGetGbaAuthAdapter
 */
ProtocolSimGetGbaAuthAdapter::ProtocolSimGetGbaAuthAdapter(const ModemData *pModemData)
    : ProtocolSimGetGbaAuthAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimGetGbaAuthAdapterInterface = new ProtocolSimGetGbaAuthAdapterJson(pModemData);
    } else {
        protocolSimGetGbaAuthAdapterInterface = new ProtocolSimGetGbaAuthAdapterLegacy(pModemData);
    }
}

ProtocolSimGetGbaAuthAdapter::~ProtocolSimGetGbaAuthAdapter() {
    delete protocolSimGetGbaAuthAdapterInterface;
    protocolSimGetGbaAuthAdapterInterface = NULL;
}

int ProtocolSimGetGbaAuthAdapter::GetGbaAuthLength() const {
    return protocolSimGetGbaAuthAdapterInterface->GetGbaAuthLength();
}

BYTE *ProtocolSimGetGbaAuthAdapter::GetGbaAuth() const {
    return protocolSimGetGbaAuthAdapterInterface->GetGbaAuth();
}

/**
 * ProtocolSimATRAdapter
 */
ProtocolSimATRAdapter::ProtocolSimATRAdapter(const ModemData *pModemData)
    : ProtocolSimATRAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimATRAdapterInterface = new ProtocolSimATRAdapterJson(pModemData);
    } else {
        protocolSimATRAdapterInterface = new ProtocolSimATRAdapterLegacy(pModemData);
    }
}

ProtocolSimATRAdapter::~ProtocolSimATRAdapter() {
    delete protocolSimATRAdapterInterface;
    protocolSimATRAdapterInterface = NULL;
}

BYTE ProtocolSimATRAdapter::GetResult() const {
    return protocolSimATRAdapterInterface->GetResult();
}

BYTE ProtocolSimATRAdapter::GetATRLength() const {
    return protocolSimATRAdapterInterface->GetATRLength();
}

const char *ProtocolSimATRAdapter::GetATR() const {
    return protocolSimATRAdapterInterface->GetATR();
}

/* ProtocolSimReadPbEntry */
ProtocolSimReadPbEntry::ProtocolSimReadPbEntry(const ModemData *pModemData)
    : ProtocolSimReadPbEntryInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimReadPbEntryInterface = new ProtocolSimReadPbEntryJson(pModemData);
    } else {
        protocolSimReadPbEntryInterface = new ProtocolSimReadPbEntryLegacy(pModemData);
    }
}

ProtocolSimReadPbEntry::~ProtocolSimReadPbEntry() {
    delete protocolSimReadPbEntryInterface;
    protocolSimReadPbEntryInterface = NULL;
}

int ProtocolSimReadPbEntry::GetRecordId() const {
    return protocolSimReadPbEntryInterface->GetRecordId();
}

int ProtocolSimReadPbEntry::GetPbType() const {
    return protocolSimReadPbEntryInterface->GetPbType();
}

int ProtocolSimReadPbEntry::GetDataLength() const {
    return protocolSimReadPbEntryInterface->GetDataLength();
}

char *ProtocolSimReadPbEntry::GetEntryData() {
    return protocolSimReadPbEntryInterface->GetEntryData();
}

/* ProtocolSimUpdatePbEntry */
ProtocolSimUpdatePbEntry::ProtocolSimUpdatePbEntry(const ModemData *pModemData)
    : ProtocolSimUpdatePbEntryInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimUpdatePbEntryInterface = new ProtocolSimUpdatePbEntryJson(pModemData);
    } else {
        protocolSimUpdatePbEntryInterface = new ProtocolSimUpdatePbEntryLegacy(pModemData);
    }
}

ProtocolSimUpdatePbEntry::~ProtocolSimUpdatePbEntry() {
    delete protocolSimUpdatePbEntryInterface;
    protocolSimUpdatePbEntryInterface = NULL;
}

BYTE ProtocolSimUpdatePbEntry::GetMode() {
    return protocolSimUpdatePbEntryInterface->GetMode();
}

BYTE ProtocolSimUpdatePbEntry::GetPbtype() {
    return protocolSimUpdatePbEntryInterface->GetPbtype();
}

UINT16 ProtocolSimUpdatePbEntry::GetRecordId() {
    return protocolSimUpdatePbEntryInterface->GetRecordId();
}

/* ProtocolSimPbStorageInfo */
ProtocolSimPbStorageInfoAdapter::ProtocolSimPbStorageInfoAdapter(const ModemData *pModemData)
    : ProtocolSimPbStorageInfoAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimPbStorageInfoAdapterInterface = new ProtocolSimPbStorageInfoAdapterJson(pModemData);
    } else {
        protocolSimPbStorageInfoAdapterInterface = new ProtocolSimPbStorageInfoAdapterLegacy(pModemData);
    }
}

ProtocolSimPbStorageInfoAdapter::~ProtocolSimPbStorageInfoAdapter() {
    delete protocolSimPbStorageInfoAdapterInterface;
    protocolSimPbStorageInfoAdapterInterface = NULL;
}

int ProtocolSimPbStorageInfoAdapter::GetPbType() const {
    return protocolSimPbStorageInfoAdapterInterface->GetPbType();
}

int ProtocolSimPbStorageInfoAdapter::GetTotalCount() const {
    return protocolSimPbStorageInfoAdapterInterface->GetTotalCount();
}

int ProtocolSimPbStorageInfoAdapter::GetUsedCount() const {
    return protocolSimPbStorageInfoAdapterInterface->GetUsedCount();
}

/* ProtocolSimPbCapaAdapter */
ProtocolSimPbCapaAdapter::ProtocolSimPbCapaAdapter(const ModemData *pModemData)
    : ProtocolSimPbCapaAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimPbCapaAdapterInterface = new ProtocolSimPbCapaAdapterJson(pModemData);
    } else {
        protocolSimPbCapaAdapterInterface = new ProtocolSimPbCapaAdapterLegacy(pModemData);
    }
}

ProtocolSimPbCapaAdapter::~ProtocolSimPbCapaAdapter() {
    delete protocolSimPbCapaAdapterInterface;
    protocolSimPbCapaAdapterInterface = NULL;
}

bool ProtocolSimPbCapaAdapter::GetPbCapa(int *pb, int entryNum) {
    return protocolSimPbCapaAdapterInterface->GetPbCapa(pb, entryNum);
}

int ProtocolSimPbCapaAdapter::GetEntryNum() const {
    return protocolSimPbCapaAdapterInterface->GetEntryNum();
}

/* ProtocolSimPbReadyAdapter */
ProtocolSimPbReadyAdapter::ProtocolSimPbReadyAdapter(const ModemData *pModemData)
    : ProtocolSimPbReadyAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimPbReadyAdapterInterface = new ProtocolSimPbReadyAdapterJson(pModemData);
    } else {
        protocolSimPbReadyAdapterInterface = new ProtocolSimPbReadyAdapterLegacy(pModemData);
    }
}

ProtocolSimPbReadyAdapter::~ProtocolSimPbReadyAdapter() {
    delete protocolSimPbReadyAdapterInterface;
    protocolSimPbReadyAdapterInterface = NULL;
}

int ProtocolSimPbReadyAdapter::GetPbReady() const {
    return protocolSimPbReadyAdapterInterface->GetPbReady();
}

/* ProtocolSimGetCarrierRestrictionsAdapter */
ProtocolSimGetCarrierRestrictionsAdapter::ProtocolSimGetCarrierRestrictionsAdapter(const ModemData *pModemData)
    : ProtocolSimGetCarrierRestrictionsAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimGetCarrierRestrictionsAdapterInterface = new ProtocolSimGetCarrierRestrictionsAdapterJson(pModemData);
    } else {
        protocolSimGetCarrierRestrictionsAdapterInterface = new ProtocolSimGetCarrierRestrictionsAdapterLegacy(pModemData);
    }
}

ProtocolSimGetCarrierRestrictionsAdapter::~ProtocolSimGetCarrierRestrictionsAdapter() {
    delete protocolSimGetCarrierRestrictionsAdapterInterface;
    protocolSimGetCarrierRestrictionsAdapterInterface = NULL;
}

int ProtocolSimGetCarrierRestrictionsAdapter::GetAllowedCarrierLength() const {
    return protocolSimGetCarrierRestrictionsAdapterInterface->GetAllowedCarrierLength();
}

int ProtocolSimGetCarrierRestrictionsAdapter::GetExcludedCarrierLength() const {
    return protocolSimGetCarrierRestrictionsAdapterInterface->GetExcludedCarrierLength();
}

int ProtocolSimGetCarrierRestrictionsAdapter::GetAllowedCarriers(RIL_Carrier *pCarriers, int nSize) const {
    return protocolSimGetCarrierRestrictionsAdapterInterface->GetAllowedCarriers(pCarriers, nSize);
}

int ProtocolSimGetCarrierRestrictionsAdapter::GetExcludedCarriers(RIL_Carrier *pCarriers, int nSize) const {
    return protocolSimGetCarrierRestrictionsAdapterInterface->GetExcludedCarriers(pCarriers, nSize);
}

bool ProtocolSimGetCarrierRestrictionsAdapter::GetAllowedCarriersPrioritized() const {
    return protocolSimGetCarrierRestrictionsAdapterInterface->GetAllowedCarriersPrioritized();
}

INT32 ProtocolSimGetCarrierRestrictionsAdapter::GetMultiSimPolicy() const {
    return protocolSimGetCarrierRestrictionsAdapterInterface->GetMultiSimPolicy();
}

/* ProtocolUiccSubStatusChangeAdapter */
ProtocolUiccSubStatusChangeAdapter::ProtocolUiccSubStatusChangeAdapter(const ModemData *pModemData)
    : ProtocolUiccSubStatusChangeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolUiccSubStatusChangeAdapterInterface = new ProtocolUiccSubStatusChangeAdapterJson(pModemData);
    } else {
        protocolUiccSubStatusChangeAdapterInterface = new ProtocolUiccSubStatusChangeAdapterLegacy(pModemData);
    }
}

ProtocolUiccSubStatusChangeAdapter::~ProtocolUiccSubStatusChangeAdapter() {
    delete protocolUiccSubStatusChangeAdapterInterface;
    protocolUiccSubStatusChangeAdapterInterface = NULL;
}

int ProtocolUiccSubStatusChangeAdapter::GetState() const {
    return protocolUiccSubStatusChangeAdapterInterface->GetState();
}

/* ProtocolSimLockInfoAdapter */
ProtocolSimLockInfoAdapter::ProtocolSimLockInfoAdapter(const ModemData *pModemData)
    : ProtocolSimLockInfoAdapterInterface() {
    protocolSimLockInfoAdapterInterface = new ProtocolSimLockInfoAdapterLegacy(pModemData);
}

ProtocolSimLockInfoAdapter::~ProtocolSimLockInfoAdapter() {
    delete protocolSimLockInfoAdapterInterface;
    protocolSimLockInfoAdapterInterface = NULL;
}

int ProtocolSimLockInfoAdapter::GetPolicy() const {
    return protocolSimLockInfoAdapterInterface->GetPolicy();
}

int ProtocolSimLockInfoAdapter::GetStatus() const {
    return protocolSimLockInfoAdapterInterface->GetStatus();
}

int ProtocolSimLockInfoAdapter::GetLockType() const {
    return protocolSimLockInfoAdapterInterface->GetLockType();
}

int ProtocolSimLockInfoAdapter::GetMaxRetryCount() const {
    return protocolSimLockInfoAdapterInterface->GetMaxRetryCount();
}

int ProtocolSimLockInfoAdapter::GetRemainCount() const {
    return protocolSimLockInfoAdapterInterface->GetRemainCount();
}

int ProtocolSimLockInfoAdapter::GetLockCodeCount() const {
    return protocolSimLockInfoAdapterInterface->GetLockCodeCount();
}

const char *ProtocolSimLockInfoAdapter::GetLockCode() const {
    return protocolSimLockInfoAdapterInterface->GetLockCode();
}

int ProtocolSimLockInfoAdapter::GetLockCodeSize() const {
    return protocolSimLockInfoAdapterInterface->GetLockCodeSize();
}

/* ProtocolSimTrayStatusChangeAdapter */
ProtocolSimTrayStatusChangeAdapter::ProtocolSimTrayStatusChangeAdapter(const ModemData *pModemData)
    : ProtocolSimTrayStatusChangeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimTrayStatusChangeAdapterInterface = new ProtocolSimTrayStatusChangeAdapterJson(pModemData);
    } else {
        protocolSimTrayStatusChangeAdapterInterface = new ProtocolSimTrayStatusChangeAdapterLegacy(pModemData);
    }
}

ProtocolSimTrayStatusChangeAdapter::~ProtocolSimTrayStatusChangeAdapter() {
    delete protocolSimTrayStatusChangeAdapterInterface;
    protocolSimTrayStatusChangeAdapterInterface = NULL;
}

int ProtocolSimTrayStatusChangeAdapter::GetState() const {
    return protocolSimTrayStatusChangeAdapterInterface->GetState();
}

#ifdef AUTO_VERIFY_PIN_WITH_CP
ProtocolSimEncryptedPinAdapter::ProtocolSimEncryptedPinAdapter(const ModemData *pModemData)
    : ProtocolSimEncryptedPinAdapterInterface() {
	protocolSimEncryptedPinAdapterInterface = new ProtocolSimEncryptedPinAdapterLegacy(pModemData);
}

ProtocolSimEncryptedPinAdapter::~ProtocolSimEncryptedPinAdapter() {
    delete protocolSimEncryptedPinAdapterInterface;
    protocolSimEncryptedPinAdapterInterface = NULL;
}

const char *ProtocolSimEncryptedPinAdapter::GetEncryptedPin() const {
    return protocolSimEncryptedPinAdapterInterface->GetEncryptedPin();
}

#endif
