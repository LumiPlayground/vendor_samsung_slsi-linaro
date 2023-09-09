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
 * protocolradioconfigadapterinterface.h
 *
 *  Created on: 2021. 8. 2.
 */

#ifndef __PROTOCOL_RADIO_CONFIG_ADAPTER_INTERFACE_H__
#define __PROTOCOL_RADIO_CONFIG_ADAPTER_INTERFACE_H__

#include <string>
#include <base/types.h>

#include "sitdef.h"
#include "rildef.h"

namespace configV2_0 = android::hardware::radio::config::V2_0;

class ProtocolPhoneCapabilityAdapterInterface {
public:
    ProtocolPhoneCapabilityAdapterInterface() {}
    virtual ~ProtocolPhoneCapabilityAdapterInterface() {}
public:
    virtual int GetMaxActiveData() const = 0;
    virtual int GetMaxActiveInternetData() const = 0;
    virtual bool IsInternetLingeringSupported() const = 0;
    virtual int GetLogicalModemListSize() const = 0;
    virtual int *GetLogicalModemList() = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimSlotStatusAdapterInterface {
public:
    ProtocolSimSlotStatusAdapterInterface() {}
    virtual ~ProtocolSimSlotStatusAdapterInterface() {}

public:
    virtual bool IsLegacySlotStatus() const = 0;
    virtual int GetNumOfSlotStatus() const = 0;
    virtual int GetCardState(int phy_slotId) const = 0;
    virtual int GetSlotState(int phy_slotId) const = 0;
    virtual int GetLogicalSlotId(int phy_slotId) const = 0;
    virtual int GetIccIdSize(int phy_slotId) const = 0;
    virtual std::string GetIccId(int phy_slotId) const = 0;
    virtual int GetAtrSize(int phy_slotId) const = 0;
    virtual std::string GetAtr(int phy_slotId) const = 0;
    virtual int GetEidSize(int phy_slotId) const = 0;
    virtual std::string GetEid(int phy_slotId) const = 0;
    virtual int GetPortInfoSize(int phy_slotId) const = 0;
    virtual int GetLogicalSlotId(int phy_slotId, int portId) const = 0;
    virtual int GetIccIdSize(int phy_slotId, int portId) const = 0;
    virtual std::string GetIccId(int phy_slotId, int portId) const = 0;
    virtual int GetPortState(int phy_slotId, int portId) const = 0;
    virtual UINT GetErrorCode() const = 0;
    virtual int FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const = 0;
    virtual int FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const = 0;
};

class ProtocolSlotStatusChangedAdapterInterface {
public:
    ProtocolSlotStatusChangedAdapterInterface() {}
    virtual ~ProtocolSlotStatusChangedAdapterInterface() {}

public:
    virtual bool IsLegacySlotStatus() const = 0;
    virtual int GetNumOfSlotStatus() const = 0;
    virtual int GetCardState(int phy_slotId) const = 0;
    virtual int GetSlotState(int phy_slotId) const = 0;
    virtual int GetLogicalSlotId(int phy_slotId) const = 0;
    virtual int GetIccIdSize(int phy_slotId) const = 0;
    virtual std::string GetIccId(int phy_slotId) const = 0;
    virtual int GetAtrSize(int phy_slotId) const = 0;
    virtual std::string GetAtr(int phy_slotId) const = 0;
    virtual int GetEidSize(int phy_slotId) const = 0;
    virtual std::string GetEid(int phy_slotId) const = 0;
    virtual int GetPortInfoSize(int phy_slotId) const = 0;
    virtual int GetLogicalSlotId(int phy_slotId, int portId) const = 0;
    virtual int GetIccIdSize(int phy_slotId, int portId) const = 0;
    virtual std::string GetIccId(int phy_slotId, int portId) const = 0;
    virtual int GetPortState(int phy_slotId, int portId) const = 0;
    virtual int FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const = 0;
    virtual int FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const = 0;

};

#endif /* __PROTOCOL_RADIO_CONFIG_ADAPTER_INTERFACE_H__ */
