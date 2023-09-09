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
 * protocolradioconfigadapterlegacy.h
 *
 *  Created on: 2019. 8. 13.
 */

#ifndef __PROTOCOL_RADIO_CONFIG_ADAPTER_LEGACY_H__
#define __PROTOCOL_RADIO_CONFIG_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolradioconfigadapterinterface.h"
#include "sitdef.h"

class ProtocolPhoneCapabilityAdapterLegacy : public ProtocolRespAdapter, public ProtocolPhoneCapabilityAdapterInterface {
  private:
    int mLogicalModemList[4];

  public:
    ProtocolPhoneCapabilityAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPhoneCapabilityAdapterLegacy() {}

  public:
    int GetMaxActiveData() const;
    int GetMaxActiveInternetData() const;
    bool IsInternetLingeringSupported() const;
    int GetLogicalModemListSize() const;
    int *GetLogicalModemList();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSimSlotStatusAdapterLegacy : public ProtocolRespAdapter, public ProtocolSimSlotStatusAdapterInterface {
  public:
    ProtocolSimSlotStatusAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData),
          m_tSlotStatusInfo(nullptr) {
        Init();
    }
    ProtocolSimSlotStatusAdapterLegacy(const ProtocolSimSlotStatusAdapterLegacy&) = delete;
    ProtocolSimSlotStatusAdapterLegacy& operator=(ProtocolSimSlotStatusAdapterLegacy const&) = delete;
    virtual ~ProtocolSimSlotStatusAdapterLegacy() {
        Dispose();
    }

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
    UINT GetErrorCode() const { return ProtocolRespAdapter::GetErrorCode(); }

    int FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const;
    int FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const;

  private:
    sit_sim_get_slot_status_info *m_tSlotStatusInfo = NULL;
    sit_sim_get_slot_status_info_v1_2 *m_tSlotStatusInfoLegacy = NULL;
    bool mIsLegacy = false;

    virtual void Dispose();
    virtual void Init();
};

class ProtocolSlotStatusChangedAdapterLegacy : public ProtocolIndAdapter, public ProtocolSlotStatusChangedAdapterInterface {
  public:
    ProtocolSlotStatusChangedAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData),
          m_tSlotStatusInfo(nullptr) {
        Init();
    }
    ProtocolSlotStatusChangedAdapterLegacy(const ProtocolSlotStatusChangedAdapterLegacy&) = delete;
    ProtocolSlotStatusChangedAdapterLegacy& operator=(ProtocolSlotStatusChangedAdapterLegacy const&) = delete;
    virtual ~ProtocolSlotStatusChangedAdapterLegacy() {
        Dispose();
    }

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

  private:
    sit_sim_get_slot_status_info *m_tSlotStatusInfo = NULL;
    sit_sim_get_slot_status_info_v1_2 *m_tSlotStatusInfoLegacy = NULL;
    bool mIsLegacy = false;

    virtual void Dispose();
    virtual void Init();
};

#endif /* __PROTOCOL_RADIO_CONFIG_ADAPTER_LEGACY_H__ */
