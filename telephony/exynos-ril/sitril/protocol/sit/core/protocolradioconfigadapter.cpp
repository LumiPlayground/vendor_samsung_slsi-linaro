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
 * protocolradioconfigadapter.cpp
 *
 *  Created on: 2021. 8. 21.
 */
#include "protocolradioconfigadapter.h"
#include "legacy/protocolradioconfigadapterlegacy.h"
#include "json/protocolradioconfigadapterjson.h"
#include <string>

ProtocolPhoneCapabilityAdapter::ProtocolPhoneCapabilityAdapter(const ModemData *pModemData)
    : ProtocolPhoneCapabilityAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolPhoneCapabilityAdapterInterface = new ProtocolPhoneCapabilityAdapterJson(pModemData);
    } else {
        protocolPhoneCapabilityAdapterInterface = new ProtocolPhoneCapabilityAdapterLegacy(pModemData);
    }
}

ProtocolPhoneCapabilityAdapter::~ProtocolPhoneCapabilityAdapter() {
    delete protocolPhoneCapabilityAdapterInterface;
    protocolPhoneCapabilityAdapterInterface = NULL;
}

int ProtocolPhoneCapabilityAdapter::GetMaxActiveData() const {
    return protocolPhoneCapabilityAdapterInterface->GetMaxActiveData();
}

int ProtocolPhoneCapabilityAdapter::GetMaxActiveInternetData() const {
    return protocolPhoneCapabilityAdapterInterface->GetMaxActiveInternetData();
}

bool ProtocolPhoneCapabilityAdapter::IsInternetLingeringSupported() const {
    return protocolPhoneCapabilityAdapterInterface->IsInternetLingeringSupported();
}

int ProtocolPhoneCapabilityAdapter::GetLogicalModemListSize() const {
    return protocolPhoneCapabilityAdapterInterface->GetLogicalModemListSize();
}

int *ProtocolPhoneCapabilityAdapter::GetLogicalModemList() {
    return protocolPhoneCapabilityAdapterInterface->GetLogicalModemList();
}

/* Radio Config */
ProtocolSimSlotStatusAdapter::ProtocolSimSlotStatusAdapter(const ModemData *pModemData)
    : ProtocolSimSlotStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSimSlotStatusAdapterInterface = new ProtocolSimSlotStatusAdapterJson(pModemData);
    } else {
        protocolSimSlotStatusAdapterInterface = new ProtocolSimSlotStatusAdapterLegacy(pModemData);
    }
}

ProtocolSimSlotStatusAdapter::~ProtocolSimSlotStatusAdapter() {
    delete protocolSimSlotStatusAdapterInterface;
    protocolSimSlotStatusAdapterInterface = NULL;
}

bool ProtocolSimSlotStatusAdapter::IsLegacySlotStatus() const
{
    return protocolSimSlotStatusAdapterInterface->IsLegacySlotStatus();
}

int ProtocolSimSlotStatusAdapter::GetNumOfSlotStatus() const {
    return protocolSimSlotStatusAdapterInterface->GetNumOfSlotStatus();
}

int ProtocolSimSlotStatusAdapter::GetCardState(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetCardState(phy_slotId);
}

int ProtocolSimSlotStatusAdapter::GetSlotState(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetSlotState(phy_slotId);
}

int ProtocolSimSlotStatusAdapter::GetAtrSize(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetAtrSize(phy_slotId);
}

std::string ProtocolSimSlotStatusAdapter::GetAtr(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetAtr(phy_slotId);
}

int ProtocolSimSlotStatusAdapter::GetPortInfoSize(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetPortInfoSize(phy_slotId);
}

int ProtocolSimSlotStatusAdapter::GetLogicalSlotId(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetLogicalSlotId(phy_slotId);
}

int ProtocolSimSlotStatusAdapter::GetLogicalSlotId(int phy_slotId, int portId) const {
    return protocolSimSlotStatusAdapterInterface->GetLogicalSlotId(phy_slotId, portId);
}

int ProtocolSimSlotStatusAdapter::GetIccIdSize(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetIccIdSize(phy_slotId);
}

int ProtocolSimSlotStatusAdapter::GetIccIdSize(int phy_slotId, int portId) const {
    return protocolSimSlotStatusAdapterInterface->GetIccIdSize(phy_slotId, portId);
}

std::string ProtocolSimSlotStatusAdapter::GetIccId(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetIccId(phy_slotId);
}

std::string ProtocolSimSlotStatusAdapter::GetIccId(int phy_slotId, int portId) const {
    return protocolSimSlotStatusAdapterInterface->GetIccId(phy_slotId, portId);
}

int ProtocolSimSlotStatusAdapter::GetPortState(int phy_slotId, int portId) const {
    return protocolSimSlotStatusAdapterInterface->GetPortState(phy_slotId, portId);
}

int ProtocolSimSlotStatusAdapter::GetEidSize(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetEidSize(phy_slotId);
}

std::string ProtocolSimSlotStatusAdapter::GetEid(int phy_slotId) const {
    return protocolSimSlotStatusAdapterInterface->GetEid(phy_slotId);
}

int ProtocolSimSlotStatusAdapter::FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const {
    return protocolSimSlotStatusAdapterInterface->FillSimSlotStatusLegacy(simSlotStatus);
}

int ProtocolSimSlotStatusAdapter::FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const {
    return protocolSimSlotStatusAdapterInterface->FillSimSlotStatus(simSlotStatus);
}

// Slot status changed
ProtocolSlotStatusChangedAdapter::ProtocolSlotStatusChangedAdapter(const ModemData *pModemData)
    : ProtocolSlotStatusChangedAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSlotStatusChangedAdapterInterface = new ProtocolSlotStatusChangedAdapterJson(pModemData);
    } else {
        protocolSlotStatusChangedAdapterInterface = new ProtocolSlotStatusChangedAdapterLegacy(pModemData);
    }
}

ProtocolSlotStatusChangedAdapter::~ProtocolSlotStatusChangedAdapter() {
    delete protocolSlotStatusChangedAdapterInterface;
    protocolSlotStatusChangedAdapterInterface = NULL;
}

bool ProtocolSlotStatusChangedAdapter::IsLegacySlotStatus() const
{
    return protocolSlotStatusChangedAdapterInterface->IsLegacySlotStatus();
}

int ProtocolSlotStatusChangedAdapter::GetNumOfSlotStatus() const {
    return protocolSlotStatusChangedAdapterInterface->GetNumOfSlotStatus();
}

int ProtocolSlotStatusChangedAdapter::GetCardState(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetCardState(phy_slotId);
}

int ProtocolSlotStatusChangedAdapter::GetSlotState(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetSlotState(phy_slotId);
}

int ProtocolSlotStatusChangedAdapter::GetAtrSize(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetAtrSize(phy_slotId);
}

std::string ProtocolSlotStatusChangedAdapter::GetAtr(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetAtr(phy_slotId);
}

int ProtocolSlotStatusChangedAdapter::GetPortInfoSize(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetPortInfoSize(phy_slotId);
}

int ProtocolSlotStatusChangedAdapter::GetLogicalSlotId(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetLogicalSlotId(phy_slotId);
}

int ProtocolSlotStatusChangedAdapter::GetLogicalSlotId(int phy_slotId, int portId) const {
    return protocolSlotStatusChangedAdapterInterface->GetLogicalSlotId(phy_slotId, portId);
}

int ProtocolSlotStatusChangedAdapter::GetIccIdSize(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetIccIdSize(phy_slotId);
}

int ProtocolSlotStatusChangedAdapter::GetIccIdSize(int phy_slotId, int portId) const {
    return protocolSlotStatusChangedAdapterInterface->GetIccIdSize(phy_slotId, portId);
}

std::string ProtocolSlotStatusChangedAdapter::GetIccId(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetIccId(phy_slotId);
}

std::string ProtocolSlotStatusChangedAdapter::GetIccId(int phy_slotId, int portId) const {
    return protocolSlotStatusChangedAdapterInterface->GetIccId(phy_slotId, portId);
}

int ProtocolSlotStatusChangedAdapter::GetPortState(int phy_slotId, int portId) const {
    return protocolSlotStatusChangedAdapterInterface->GetPortState(phy_slotId, portId);
}

int ProtocolSlotStatusChangedAdapter::GetEidSize(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetEidSize(phy_slotId);
}

std::string ProtocolSlotStatusChangedAdapter::GetEid(int phy_slotId) const {
    return protocolSlotStatusChangedAdapterInterface->GetEid(phy_slotId);
}

int ProtocolSlotStatusChangedAdapter::FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const {
    return protocolSlotStatusChangedAdapterInterface->FillSimSlotStatusLegacy(simSlotStatus);
}

int ProtocolSlotStatusChangedAdapter::FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const {
    return protocolSlotStatusChangedAdapterInterface->FillSimSlotStatus(simSlotStatus);
}
