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
 * protocolradioconfigadapter.h
 *
 *  Created on: 2021. 8. 2.
 */

#ifndef __PROTOCOL_RADIO_CONFIG_ADAPTER_H__
#define __PROTOCOL_RADIO_CONFIG_ADAPTER_H__

#include "protocolradioconfigadapterinterface.h"

class ModemData;

class ProtocolPhoneCapabilityAdapter : public ProtocolPhoneCapabilityAdapterInterface {
private:
    ProtocolPhoneCapabilityAdapterInterface *protocolPhoneCapabilityAdapterInterface = NULL;
public:
    ProtocolPhoneCapabilityAdapter(const ModemData *pModemData);
    ProtocolPhoneCapabilityAdapter(const ProtocolPhoneCapabilityAdapter &) = delete;
    ProtocolPhoneCapabilityAdapter& operator=(ProtocolPhoneCapabilityAdapter const&) = delete;
    virtual ~ProtocolPhoneCapabilityAdapter();
public:
    int GetMaxActiveData() const;
    int GetMaxActiveInternetData() const;
    bool IsInternetLingeringSupported() const;
    int GetLogicalModemListSize() const;
    int *GetLogicalModemList();
    UINT GetErrorCode() const { return protocolPhoneCapabilityAdapterInterface->GetErrorCode(); }
};

class ProtocolSimSlotStatusAdapter : public ProtocolSimSlotStatusAdapterInterface {
private:
    ProtocolSimSlotStatusAdapterInterface *protocolSimSlotStatusAdapterInterface = NULL;
public:
    ProtocolSimSlotStatusAdapter(const ModemData *pModemData);
    ProtocolSimSlotStatusAdapter(const ProtocolSimSlotStatusAdapter &) = delete;
    ProtocolSimSlotStatusAdapter& operator=(ProtocolSimSlotStatusAdapter const&) = delete;
    virtual ~ProtocolSimSlotStatusAdapter();
public:
    bool IsLegacySlotStatus() const;
    int GetNumOfSlotStatus() const;
    int GetCardState(int phy_slotId) const;
    int GetSlotState(int phy_slotId) const;
    int GetLogicalSlotId(int phy_slotId) const;
    int GetIccIdSize(int phy_slotId) const;
    std::string GetIccId(int phy_slotId) const;
    int GetAtrSize(int phy_slotId) const;
    std::string GetAtr(int phy_slotId) const;
    int GetEidSize(int phy_slotId) const;
    std::string GetEid(int phy_slotId) const;
    int GetPortInfoSize(int phy_slotId) const;
    int GetLogicalSlotId(int phy_slotId, int portId) const;
    int GetIccIdSize(int phy_slotId, int portId) const;
    std::string GetIccId(int phy_slotId, int portId) const;
    int GetPortState(int phy_slotId, int portId) const;
    UINT GetErrorCode() const { return protocolSimSlotStatusAdapterInterface->GetErrorCode(); }

    int FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const;
    int FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const;
};

class ProtocolSlotStatusChangedAdapter : public ProtocolSlotStatusChangedAdapterInterface {
private:
    ProtocolSlotStatusChangedAdapterInterface *protocolSlotStatusChangedAdapterInterface = NULL;
public:
    ProtocolSlotStatusChangedAdapter(const ModemData *pModemData);
    ProtocolSlotStatusChangedAdapter(const ProtocolSlotStatusChangedAdapter &) = delete;
    ProtocolSlotStatusChangedAdapter& operator=(ProtocolSlotStatusChangedAdapter const&) = delete;
    virtual ~ProtocolSlotStatusChangedAdapter();
public:
    bool IsLegacySlotStatus() const;
    int GetNumOfSlotStatus() const;
    int GetCardState(int phy_slotId) const;
    int GetSlotState(int phy_slotId) const;
    int GetLogicalSlotId(int phy_slotId) const;
    int GetIccIdSize(int phy_slotId) const;
    std::string GetIccId(int phy_slotId) const;
    int GetAtrSize(int phy_slotId) const;
    std::string GetAtr(int phy_slotId) const;
    int GetEidSize(int phy_slotId) const;
    std::string GetEid(int phy_slotId) const;
    int GetPortInfoSize(int phy_slotId) const;
    int GetLogicalSlotId(int phy_slotId, int portId) const;
    int GetIccIdSize(int phy_slotId, int portId) const;
    std::string GetIccId(int phy_slotId, int portId) const;
    int GetPortState(int phy_slotId, int portId) const;

    int FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const;
    int FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const;
};

#endif /* __PROTOCOL_RADIO_CONFIG_ADAPTER_H__ */
