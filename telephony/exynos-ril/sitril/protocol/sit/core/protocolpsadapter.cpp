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
 * protocolpsadapter.cpp
 *
 *  Created on: 2021. 10. 18.
 *      Author: juneho.kwon
 */
#include "protocolpsadapter.h"
#include "legacy/ps/protocolpsadapterlegacy.h"
#include "json/ps/protocolpsadapterjson.h"

/**
 * ProtocolPsSetupDataCallAdapter
 */
ProtocolPsSetupDataCallAdapter::ProtocolPsSetupDataCallAdapter(const ModemData *pModemData) : ProtocolPsSetupDataCallAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsSetupDataCallAdapterInterface = new ProtocolPsSetupDataCallAdapterjson(pModemData);
    } else {
        protocolPsSetupDataCallAdapterInterface = new ProtocolPsSetupDataCallAdapterLegacy(pModemData);
    }
}

ProtocolPsSetupDataCallAdapter::~ProtocolPsSetupDataCallAdapter() {
    delete protocolPsSetupDataCallAdapterInterface;
    protocolPsSetupDataCallAdapterInterface = NULL;
}

int ProtocolPsSetupDataCallAdapter::GetStatus() const
{
    return protocolPsSetupDataCallAdapterInterface->GetStatus();
}

int ProtocolPsSetupDataCallAdapter::GetCid() const
{
    return protocolPsSetupDataCallAdapterInterface->GetCid();
}

int ProtocolPsSetupDataCallAdapter::GetActiveStatus() const
{
    return protocolPsSetupDataCallAdapterInterface->GetActiveStatus();
}

int ProtocolPsSetupDataCallAdapter::GetPdpType() const
{
    return protocolPsSetupDataCallAdapterInterface->GetPdpType();
}

int ProtocolPsSetupDataCallAdapter::GetAddrInfo(PDP_ADDR *pAddr, int pdpType) const
{
    return protocolPsSetupDataCallAdapterInterface->GetAddrInfo(pAddr, pdpType);
}

int ProtocolPsSetupDataCallAdapter::GetAddrInfo(PDP_ADDR *pAddr) const
{
    return protocolPsSetupDataCallAdapterInterface->GetAddrInfo(pAddr);
}

int ProtocolPsSetupDataCallAdapter::GetMTUv4() const
{
    return protocolPsSetupDataCallAdapterInterface->GetMTUv4();
}

int ProtocolPsSetupDataCallAdapter::GetMTUv6() const
{
    return protocolPsSetupDataCallAdapterInterface->GetMTUv6();
}

int ProtocolPsSetupDataCallAdapter::GetPCO() const
{
    return protocolPsSetupDataCallAdapterInterface->GetPCO();
}

int64_t ProtocolPsSetupDataCallAdapter::GetSuggestedRetryTime() const
{
    return protocolPsSetupDataCallAdapterInterface->GetSuggestedRetryTime();
}

const DataCall * ProtocolPsSetupDataCallAdapter::GetDataCall() const
{
    return protocolPsSetupDataCallAdapterInterface->GetDataCall();
}

/**
 * ProtocolPsDataCallListAdapter
 */
ProtocolPsDataCallListAdapter::ProtocolPsDataCallListAdapter(const ModemData *pModemData) : ProtocolPsDataCallListAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsDataCallListAdapterInterface = new ProtocolPsDataCallListAdapterjson(pModemData);
    } else {
        protocolPsDataCallListAdapterInterface = new ProtocolPsDataCallListAdapterLegacy(pModemData);
    }
}

ProtocolPsDataCallListAdapter::~ProtocolPsDataCallListAdapter() {
    delete protocolPsDataCallListAdapterInterface;
    protocolPsDataCallListAdapterInterface = NULL;
}

unsigned int ProtocolPsDataCallListAdapter::GetDataCallNum() const
{
    return protocolPsDataCallListAdapterInterface->GetDataCallNum();
}
const DataCall *ProtocolPsDataCallListAdapter::GetDataCallList() const
{
    return protocolPsDataCallListAdapterInterface->GetDataCallList();
}

const DataCall *ProtocolPsDataCallListAdapter::GetDataCallByIndex(unsigned int index) const
{
    return protocolPsDataCallListAdapterInterface->GetDataCallByIndex(index);
}

const DataCall *ProtocolPsDataCallListAdapter::GetDataCallByCid(int cid) const
{
    return protocolPsDataCallListAdapterInterface->GetDataCallByCid(cid);
}

/**
 * ProtocolPsDataCallListChangedAdapter
 */
ProtocolPsDataCallListChangedAdapter::ProtocolPsDataCallListChangedAdapter(const ModemData *pModemData) : ProtocolPsDataCallListChangedAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsDataCallListChangedAdapterInterface = new ProtocolPsDataCallListChangedAdapterjson(pModemData);
    } else {
        protocolPsDataCallListChangedAdapterInterface = new ProtocolPsDataCallListChangedAdapterLegacy(pModemData);
    }
}

ProtocolPsDataCallListChangedAdapter::~ProtocolPsDataCallListChangedAdapter() {
    delete protocolPsDataCallListChangedAdapterInterface;
    protocolPsDataCallListChangedAdapterInterface = NULL;
}

unsigned int ProtocolPsDataCallListChangedAdapter::GetDataCallNum() const
{
    return protocolPsDataCallListChangedAdapterInterface->GetDataCallNum();
}

const DataCall *ProtocolPsDataCallListChangedAdapter::GetDataCallList() const
{
    return protocolPsDataCallListChangedAdapterInterface->GetDataCallList();
}

const DataCall *ProtocolPsDataCallListChangedAdapter::GetDataCallByIndex(unsigned int index) const
{
    return protocolPsDataCallListChangedAdapterInterface->GetDataCallByIndex(index);
}

const DataCall *ProtocolPsDataCallListChangedAdapter::GetDataCallByCid(int cid) const
{
    return protocolPsDataCallListChangedAdapterInterface->GetDataCallByCid(cid);
}

/**
 * ProtocolPsDedicatedBearInfoAdapter
 */
ProtocolPsDedicatedBearInfoAdapter::ProtocolPsDedicatedBearInfoAdapter(const ModemData *pModemData) : ProtocolPsDedicatedBearInfoAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsDedicatedBearInfoAdapterInterface = new ProtocolPsDedicatedBearInfoAdapterjson(pModemData);
    } else {
        protocolPsDedicatedBearInfoAdapterInterface = new ProtocolPsDedicatedBearInfoAdapterLegacy(pModemData);
    }
}

ProtocolPsDedicatedBearInfoAdapter::~ProtocolPsDedicatedBearInfoAdapter() {
    delete protocolPsDedicatedBearInfoAdapterInterface;
    protocolPsDedicatedBearInfoAdapterInterface = NULL;
}

const DedicatedBearerInfo * ProtocolPsDedicatedBearInfoAdapter::GetDedicatedBearerInfo()
{
    return protocolPsDedicatedBearInfoAdapterInterface->GetDedicatedBearerInfo();
}

int ProtocolPsDedicatedBearInfoAdapter::GetCid() const
{
    return protocolPsDedicatedBearInfoAdapterInterface->GetCid();
}

int ProtocolPsDedicatedBearInfoAdapter::GetStatus() const
{
    return protocolPsDedicatedBearInfoAdapterInterface->GetStatus();
}

const QosSession_t &ProtocolPsDedicatedBearInfoAdapter::GetQosSessionInfo() const
{
    return protocolPsDedicatedBearInfoAdapterInterface->GetQosSessionInfo();
}

/**
 * ProtocolPsNasTimerStatusAdapter
 */
ProtocolPsNasTimerStatusAdapter::ProtocolPsNasTimerStatusAdapter(const ModemData *pModemData) : ProtocolPsNasTimerStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsNasTimerStatusAdapterInterface = new ProtocolPsNasTimerStatusAdapterjson(pModemData);
    } else {
        protocolPsNasTimerStatusAdapterInterface = new ProtocolPsNasTimerStatusAdapterLegacy(pModemData);
    }
}

ProtocolPsNasTimerStatusAdapter::~ProtocolPsNasTimerStatusAdapter() {
    delete protocolPsNasTimerStatusAdapterInterface;
    protocolPsNasTimerStatusAdapterInterface = NULL;
}

const SitNasTimerStatus *ProtocolPsNasTimerStatusAdapter::GetNasTimerStatus() {
    return protocolPsNasTimerStatusAdapterInterface->GetNasTimerStatus();
}

const char *ProtocolPsNasTimerStatusAdapter::GetParameter() const {
    return protocolPsNasTimerStatusAdapterInterface->GetParameter();
}

UINT ProtocolPsNasTimerStatusAdapter::GetParameterLength() const {
    return protocolPsNasTimerStatusAdapterInterface->GetParameterLength();
}

/**
 * ProtocolPsStartKeepAliveAdapter
 */
ProtocolPsStartKeepAliveAdapter::ProtocolPsStartKeepAliveAdapter(const ModemData *pModemData) : ProtocolPsStartKeepAliveAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsStartKeepAliveAdapterInterface = new ProtocolPsStartKeepAliveAdapterjson(pModemData);
    } else {
        protocolPsStartKeepAliveAdapterInterface = new ProtocolPsStartKeepAliveAdapterLegacy(pModemData);
    }
}

ProtocolPsStartKeepAliveAdapter::~ProtocolPsStartKeepAliveAdapter() {
    delete protocolPsStartKeepAliveAdapterInterface;
    protocolPsStartKeepAliveAdapterInterface = NULL;
}

unsigned int ProtocolPsStartKeepAliveAdapter::getSessionHandle() const
{
    return protocolPsStartKeepAliveAdapterInterface->getSessionHandle();
}

int ProtocolPsStartKeepAliveAdapter::getCode() const
{
    return protocolPsStartKeepAliveAdapterInterface->getCode();
}

/**
 * ProtocolPsKeepAliveStatusAdapter
 */
ProtocolPsKeepAliveStatusAdapter::ProtocolPsKeepAliveStatusAdapter(const ModemData *pModemData) : ProtocolPsKeepAliveStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsKeepAliveStatusAdapterInterface = new ProtocolPsKeepAliveStatusAdapterjson(pModemData);
    } else {
        protocolPsKeepAliveStatusAdapterInterface = new ProtocolPsKeepAliveStatusAdapterLegacy(pModemData);
    }
}

ProtocolPsKeepAliveStatusAdapter::~ProtocolPsKeepAliveStatusAdapter() {
    delete protocolPsKeepAliveStatusAdapterInterface;
    protocolPsKeepAliveStatusAdapterInterface = NULL;
}

unsigned int ProtocolPsKeepAliveStatusAdapter::getSessionHandle() const
{
    return protocolPsKeepAliveStatusAdapterInterface->getSessionHandle();
}

int ProtocolPsKeepAliveStatusAdapter::getCode() const
{
    return protocolPsKeepAliveStatusAdapterInterface->getCode();
}

/**
 * ProtocolPsPcoDataAdapter
 */
ProtocolPsPcoDataAdapter::ProtocolPsPcoDataAdapter(const ModemData *pModemData) : ProtocolPsPcoDataAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsPcoDataAdapterInterface = new ProtocolPsPcoDataAdapterjson(pModemData);
    } else {
        protocolPsPcoDataAdapterInterface = new ProtocolPsPcoDataAdapterLegacy(pModemData);
    }
}

ProtocolPsPcoDataAdapter::~ProtocolPsPcoDataAdapter() {
    delete protocolPsPcoDataAdapterInterface;
    protocolPsPcoDataAdapterInterface = NULL;
}

int ProtocolPsPcoDataAdapter::GetCid() const
{
    return protocolPsPcoDataAdapterInterface->GetCid();
}

int ProtocolPsPcoDataAdapter::GetPdpType() const
{
    return protocolPsPcoDataAdapterInterface->GetPdpType();
}

int ProtocolPsPcoDataAdapter::GetPcoNum() const
{
    return protocolPsPcoDataAdapterInterface->GetPcoNum();
}

int ProtocolPsPcoDataAdapter::GetPcoData(sit_pdp_pco_data_entry &pco_data)
{
    return protocolPsPcoDataAdapterInterface->GetPcoData(pco_data);
}

/**
 * ProtocolPsDataOffStatusAdapter
 */
ProtocolPsDataOffStatusAdapter::ProtocolPsDataOffStatusAdapter(const ModemData *pModemData) : ProtocolPsDataOffStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsDataOffStatusAdapterInterface = std::make_unique<ProtocolPsDataOffStatusAdapterjson>(pModemData);
    } else {
        protocolPsDataOffStatusAdapterInterface = std::make_unique<ProtocolPsDataOffStatusAdapterLegacy>(pModemData);
    }
}

ProtocolPsDataOffStatusAdapter::~ProtocolPsDataOffStatusAdapter() {
}

int ProtocolPsDataOffStatusAdapter::GetPsDataOffAvailableStatus() const
{
    return protocolPsDataOffStatusAdapterInterface->GetPsDataOffAvailableStatus();
}

int ProtocolPsDataOffStatusAdapter::GetExemptListForHome() const
{
    return protocolPsDataOffStatusAdapterInterface->GetExemptListForHome();
}

int ProtocolPsDataOffStatusAdapter::GetExemptListForRoam() const
{
    return protocolPsDataOffStatusAdapterInterface->GetExemptListForRoam();
}

/**
 * ProtocolPsIpAddrStatusAdapter
 */
ProtocolPsIpAddrStatusAdapter::ProtocolPsIpAddrStatusAdapter(const ModemData *pModemData) : ProtocolPsIpAddrStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsIpAddrStatusAdapterInterface = new ProtocolPsIpAddrStatusAdapterjson(pModemData);
    } else {
        protocolPsIpAddrStatusAdapterInterface = new ProtocolPsIpAddrStatusAdapterLegacy(pModemData);
    }
}

ProtocolPsIpAddrStatusAdapter::~ProtocolPsIpAddrStatusAdapter() {
    delete protocolPsIpAddrStatusAdapterInterface;
    protocolPsIpAddrStatusAdapterInterface = NULL;
}

int ProtocolPsIpAddrStatusAdapter::GetCid() const
{
    return protocolPsIpAddrStatusAdapterInterface->GetCid();
}

int ProtocolPsIpAddrStatusAdapter::GetStatus() const
{
    return protocolPsIpAddrStatusAdapterInterface->GetStatus();
}

void ProtocolPsIpAddrStatusAdapter::GetAddrInfo(char (&ipv4addr)[MAX_IPV4_ADDR_LEN], char (&ipv6addr)[MAX_IPV6_ADDR_LEN]) const
{
    return protocolPsIpAddrStatusAdapterInterface->GetAddrInfo(ipv4addr, ipv6addr);
}

/**
 * ProtocolPsUnthrottleApn
 */
ProtocolPsUnthrottleApn::ProtocolPsUnthrottleApn(const ModemData *pModemData) : ProtocolPsUnthrottleApnInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsUnthrottleApnInterface = new ProtocolPsUnthrottleApnjson(pModemData);
    } else {
        protocolPsUnthrottleApnInterface = new ProtocolPsUnthrottleApnLegacy(pModemData);
    }
}

ProtocolPsUnthrottleApn::~ProtocolPsUnthrottleApn() {
    delete protocolPsUnthrottleApnInterface;
    protocolPsUnthrottleApnInterface = NULL;
}

const char * ProtocolPsUnthrottleApn::GetApn() const
{
    return protocolPsUnthrottleApnInterface->GetApn();
}

/**
 * ProtocolPsAllocatePduSessionId
 */
ProtocolPsAllocatePduSessionId::ProtocolPsAllocatePduSessionId(const ModemData *pModemData) : ProtocolPsAllocatePduSessionIdInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsAllocatePduSessionIdInterface = new ProtocolPsAllocatePduSessionIdjson(pModemData);
    } else {
        protocolPsAllocatePduSessionIdInterface = new ProtocolPsAllocatePduSessionIdLegacy(pModemData);
    }
}

ProtocolPsAllocatePduSessionId::~ProtocolPsAllocatePduSessionId() {
    delete protocolPsAllocatePduSessionIdInterface;
    protocolPsAllocatePduSessionIdInterface = NULL;
}

int ProtocolPsAllocatePduSessionId::GetSessionId() const
{
    return protocolPsAllocatePduSessionIdInterface->GetSessionId();
}

/**
 * ProtocolPsSlicingConfig
 */
ProtocolPsSlicingConfig::ProtocolPsSlicingConfig(const ModemData *pModemData) : ProtocolPsSlicingConfigInterface() {
    if (IsPayloadTypeJson()) {
        protocolPsSlicingConfigInterface = new ProtocolPsSlicingConfigjson(pModemData);
    } else {
        protocolPsSlicingConfigInterface = new ProtocolPsSlicingConfigLegacy(pModemData);
    }
}

ProtocolPsSlicingConfig::~ProtocolPsSlicingConfig() {
    delete protocolPsSlicingConfigInterface;
    protocolPsSlicingConfigInterface = NULL;
}

DataV2_0::RIL_SlicingConfig ProtocolPsSlicingConfig::GetSlicingConfig() const
{
    return protocolPsSlicingConfigInterface->GetSlicingConfig();
}

int ProtocolPsSlicingConfig::GetPayloadSize() const
{
    return protocolPsSlicingConfigInterface->GetPayloadSize();
}

/**
 * ProtocolPsSlicingConfigUpdated
 */
ProtocolPsSlicingConfigUpdated::ProtocolPsSlicingConfigUpdated(const ModemData *pModemData) : ProtocolPsSlicingConfigUpdatedInterface() {
    if (IsPayloadTypeJson()) {
        // Not yet implemented
        // protocolPsSlicingConfigUpdatedInterface = new ProtocolPsSlicingConfigUpdatedjson(pModemData);
    } else {
        protocolPsSlicingConfigUpdatedInterface = new ProtocolPsSlicingConfigUpdatedLegacy(pModemData);
    }
}

ProtocolPsSlicingConfigUpdated::~ProtocolPsSlicingConfigUpdated() {
    delete protocolPsSlicingConfigUpdatedInterface;
    protocolPsSlicingConfigUpdatedInterface = NULL;
}

DataV2_0::RIL_SlicingConfig ProtocolPsSlicingConfigUpdated::GetSlicingConfig() const
{
    return protocolPsSlicingConfigUpdatedInterface->GetSlicingConfig();
}

int ProtocolPsSlicingConfigUpdated::GetPayloadSize() const
{
    return protocolPsSlicingConfigUpdatedInterface->GetPayloadSize();
}
