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
 * protocolradioconfigadapterjson.h
 *
 *  Created on: 2021. 8. 6.
 */

#ifndef __PROTOCOL_RADIO_CONFIG_ADAPTER_JSON_H__
#define __PROTOCOL_RADIO_CONFIG_ADAPTER_JSON_H__

#include "protocoljsonadapter.h"
#include "protocolradioconfigadapterinterface.h"
#include "json/json.h"
#include <string>

class ProtocolPhoneCapabilityAdapterJson : public ProtocolJsonRespAdapter, public ProtocolPhoneCapabilityAdapterInterface {
  private:
    int mLogicalModemList[4];

  public:
    ProtocolPhoneCapabilityAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolPhoneCapabilityAdapterJson() {}

  public:
    int GetMaxActiveData() const;
    int GetMaxActiveInternetData() const;
    bool IsInternetLingeringSupported() const;
    int GetLogicalModemListSize() const;
    int *GetLogicalModemList();
    UINT GetErrorCode() const;
};

class ProtocolSimSlotStatusAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSimSlotStatusAdapterInterface {
  private:
    // member variable to store slot_status_info data
    // see ns_sim_slot_status_info
    Json::Value m_jsonRspSlotStatusData;

    void Init();

  public:
    ProtocolSimSlotStatusAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSimSlotStatusAdapterJson() {}

  public:
    bool IsLegacySlotStatus() const {return false;}  // to do implement
    int GetNumOfSlotStatus() const;
    int GetCardState(int phy_slotId) const;
    int GetSlotState(int phy_slotId) const;
    int GetAtrSize(int phy_slotId) const;
    std::string GetAtr(int phy_slotId) const;
    int GetLogicalSlotId(int phy_slotId) const;
    int GetIccIdSize(int phy_slotId) const;
    std::string GetIccId(int phy_slotId) const;
    int GetEidSize(int phy_slotId) const;
    std::string GetEid(int phy_slotId) const;

    int GetPortInfoSize(int /*phy_slotId*/) const {return 0;} // to do implement
    UINT GetErrorCode() const { return ProtocolRespAdapter::GetErrorCode(); }
    int GetLogicalSlotId(int /*phy_slotId*/, int /*portId*/) const {return 0;} // to do implement
    int GetIccIdSize(int /*phy_slotId*/, int /*portId*/) const {return 0;} // to do implement
    std::string GetIccId(int /*phy_slotId*/, int /*portId*/) const {return "";} // to do implement
    int GetPortState(int /*phy_slotId*/, int /*portId*/) const {return 0; } // to do implement

    int FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const; // to do implement
    int FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const; // to do implement
};

class ProtocolSlotStatusChangedAdapterJson : public ProtocolJsonIndAdapter, public ProtocolSlotStatusChangedAdapterInterface {
  private:
    // member variable to store slot_status_info data
    // see ns_sim_slot_status_info
    Json::Value m_jsonIndSlotStatusData;

    void Init();

  public:
    ProtocolSlotStatusChangedAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSlotStatusChangedAdapterJson() {}

  public:
    bool IsLegacySlotStatus() const {return false;}  // to do implement
    int GetNumOfSlotStatus() const;
    int GetCardState(int phy_slotId) const;
    int GetSlotState(int phy_slotId) const;
    int GetAtrSize(int phy_slotId) const;
    std::string GetAtr(int phy_slotId) const;
    int GetLogicalSlotId(int phy_slotId) const;
    int GetIccIdSize(int phy_slotId) const;
    std::string GetIccId(int phy_slotId) const;
    int GetEidSize(int phy_slotId) const;
    std::string GetEid(int phy_slotId) const;
    int GetPortInfoSize(int /*phy_slotId*/) const {return 0;} // to do implement
    int GetLogicalSlotId(int /*phy_slotId*/, int /*portId*/) const {return 0;} // to do implement
    int GetIccIdSize(int /*phy_slotId*/, int /*portId*/) const {return 0;} // to do implement
    std::string GetIccId(int /*phy_slotId*/, int /*portId*/) const {return "";} // to do implement
    int GetPortState(int /*phy_slotId*/, int /*portId*/) const {return 0; } // to do implement

    int FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const; // to do implement
    int FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const; // to do implement
};
#endif /* __PROTOCOL_RADIO_CONFIG_ADAPTER_JSON_H__ */
