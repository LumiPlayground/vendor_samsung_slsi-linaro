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
 * protocolradioconfigadapterlegacy.cpp
 *
 *  Created on: 2019. 8. 13.
 */
#include "protocolradioconfigadapterlegacy.h"
#include "rillog.h"
#include "modemdata.h"
#include "util.h"

ProtocolPhoneCapabilityAdapterLegacy::ProtocolPhoneCapabilityAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData)
{
    // logicalModemList not supported by modem
    // generate logical modemId using index
    int size = sizeof(mLogicalModemList)/sizeof(mLogicalModemList[0]);
    for (int i = 0; i < size; i++) {
        mLogicalModemList[i] = i;
    }
}

int ProtocolPhoneCapabilityAdapterLegacy::GetMaxActiveData() const
{
    if (m_pModemData != NULL) {
        sit_pdp_get_phone_capability_rsp *data = (sit_pdp_get_phone_capability_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PHONE_CAPABILITY) {
            return data->max_simultaneous_data_stack & 0xFF;
        }
    }
    return 1;
}

int ProtocolPhoneCapabilityAdapterLegacy::GetMaxActiveInternetData() const
{
    if (m_pModemData != NULL) {
        sit_pdp_get_phone_capability_rsp *data = (sit_pdp_get_phone_capability_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PHONE_CAPABILITY) {
            return data->max_simultaneous_internet_pdn & 0xFF;
        }
    }
    return 1;
}

bool ProtocolPhoneCapabilityAdapterLegacy::IsInternetLingeringSupported() const
{
    if (m_pModemData != NULL) {
        sit_pdp_get_phone_capability_rsp *data = (sit_pdp_get_phone_capability_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PHONE_CAPABILITY) {
            return data->internet_lingering_support & 0xFF;
        }
    }
    return false;
}

int ProtocolPhoneCapabilityAdapterLegacy::GetLogicalModemListSize() const
{
    if (m_pModemData != NULL) {
        sit_pdp_get_phone_capability_rsp *data = (sit_pdp_get_phone_capability_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PHONE_CAPABILITY) {
            return data->max_supported_stack & 0xFF;
        }
    }
    return 1;
}

int *ProtocolPhoneCapabilityAdapterLegacy::GetLogicalModemList()
{
    return mLogicalModemList;
}

/* Radio Config */
void ProtocolSimSlotStatusAdapterLegacy::Init()
{
    if (m_pModemData != NULL) {
        UINT modemDataLength = m_pModemData->GetLength();
        if (modemDataLength <= sizeof(RCM_HEADER)) {
            RilLogE("%s() ProtocolSimSlotStatusAdapterLegacy modemDataLength is wrong",
                    __FUNCTION__);
            return;
        }
        if (modemDataLength > sizeof(sit_sim_get_slot_status_rsp_v1_2)) {
            sit_sim_get_slot_status_rsp *pData
                    = (sit_sim_get_slot_status_rsp *) m_pModemData->GetRawData();
            if (pData != NULL && pData->hdr.id == SIT_GET_SLOT_STATUS
                    && pData->num_of_info > 0 && pData->num_of_info <= MAX_SLOT_NUM) {
                m_tSlotStatusInfo = new sit_sim_get_slot_status_info[pData->num_of_info] {};
                sit_sim_get_slot_status_info *pData_info
                        = (sit_sim_get_slot_status_info *) pData->info;

                for (int slotId = 0; slotId < pData->num_of_info; slotId++) {
                    memcpy(&(m_tSlotStatusInfo[slotId]), &(pData_info[slotId]),
                            sizeof(sit_sim_get_slot_status_info));
                }
            }
        } else {
            sit_sim_get_slot_status_rsp_v1_2 *pData
                    = (sit_sim_get_slot_status_rsp_v1_2 *) m_pModemData->GetRawData();
            if (pData != NULL && pData->hdr.id == SIT_GET_SLOT_STATUS
                    && pData->num_of_info > 0 && pData->num_of_info <= MAX_SLOT_NUM) {
                m_tSlotStatusInfoLegacy =
                        new sit_sim_get_slot_status_info_v1_2[pData->num_of_info] {};
                sit_sim_get_slot_status_info_v1_2 *pData_info
                        = (sit_sim_get_slot_status_info_v1_2 *) pData->info;
                unsigned int sizefor_1_0 =
                        sizeof(sit_sim_get_slot_status_info_v1_2) - (MAX_EID_LEN + 1);
                mIsLegacy = true;

                for (int slotId = 0; slotId < pData->num_of_info; slotId++) {
                    memcpy(&(m_tSlotStatusInfoLegacy[slotId]), &(pData_info[slotId]), sizefor_1_0);

                    // for RadioConfig v1.2
                    if (modemDataLength > ((sizefor_1_0 * pData->num_of_info) + 1)) {
                        m_tSlotStatusInfoLegacy[slotId].eid_len = pData_info[slotId].eid_len;
                        if (pData_info[slotId].eid_len <= MAX_EID_LEN) {
                            memcpy(&(m_tSlotStatusInfoLegacy[slotId].eid),
                                    &(pData_info[slotId].eid), pData_info[slotId].eid_len);
                        } else {
                            RilLogI("%s() ProtocolSimSlotStatusAdapter eid_len(%) is wrong",
                                    __FUNCTION__, pData_info[slotId].eid_len);
                            pData_info[slotId].eid_len = 0;
                        }
                    }
                }
            }
        }
        RilLogI("%s() ProtocolSimSlotStatusAdapter m_tSlotStatusInfo is initialized", __FUNCTION__);
    }
}

void ProtocolSimSlotStatusAdapterLegacy::Dispose()
{
    RilLogI("%s() ProtocolSimSlotStatusAdapter is disposed", __FUNCTION__);
    if (m_tSlotStatusInfo != NULL) {
        delete[] m_tSlotStatusInfo;
    }

    if (m_tSlotStatusInfoLegacy != NULL) {
        delete[] m_tSlotStatusInfoLegacy;
    }
    m_tSlotStatusInfo = NULL;
    m_tSlotStatusInfoLegacy = NULL;
}


bool ProtocolSimSlotStatusAdapterLegacy::IsLegacySlotStatus() const
{
    return mIsLegacy;
}

int ProtocolSimSlotStatusAdapterLegacy::GetNumOfSlotStatus() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        if (m_tSlotStatusInfo != NULL) {
            sit_sim_get_slot_status_rsp *data = (sit_sim_get_slot_status_rsp *)m_pModemData->GetRawData();
            ret = (int)(data->num_of_info);
        } else {
            sit_sim_get_slot_status_rsp_v1_2 *data = (sit_sim_get_slot_status_rsp_v1_2 *)m_pModemData->GetRawData();
            ret = (int)(data->num_of_info);
        }
    }
    return ret;
}

int ProtocolSimSlotStatusAdapterLegacy::GetCardState(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL) {
        ret = (int)(m_tSlotStatusInfo[phy_slotId].card_state);
    } else if (m_tSlotStatusInfoLegacy != NULL) {
        ret = (int)(m_tSlotStatusInfoLegacy[phy_slotId].card_state);
    }
    return ret;
}

int ProtocolSimSlotStatusAdapterLegacy::GetSlotState(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfoLegacy != NULL) {
        ret = (int)(m_tSlotStatusInfoLegacy[phy_slotId].slot_State);
    }
    return ret;
}

int ProtocolSimSlotStatusAdapterLegacy::GetAtrSize(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL && m_tSlotStatusInfo[phy_slotId].atr_len > 0) {
        ret = ((int)(m_tSlotStatusInfo[phy_slotId].atr_len)) * 2 + 1;
    } else if (m_tSlotStatusInfoLegacy != NULL && m_tSlotStatusInfoLegacy[phy_slotId].atr_len > 0) {
        ret = ((int)(m_tSlotStatusInfoLegacy[phy_slotId].atr_len)) * 2 + 1;
    }
    return ret;
}

std::string ProtocolSimSlotStatusAdapterLegacy::GetAtr(int phy_slotId) const
{
    std::string strAtr;
    char *pAtr = NULL;
    if(m_tSlotStatusInfo != NULL) {
        int size = ((int)(m_tSlotStatusInfo[phy_slotId].atr_len)) * 2 + 1;
        pAtr = new char[size];
        int ret = Value2HexString(pAtr, m_tSlotStatusInfo[phy_slotId].atr,
                (int)(m_tSlotStatusInfo[phy_slotId].atr_len));
        if (ret == -1) {
            delete [] pAtr;
            return NULL;
        }

        strAtr = pAtr;
        delete[] pAtr;
        return strAtr;
    } else if (m_tSlotStatusInfoLegacy != NULL) {
        int size = ((int)(m_tSlotStatusInfoLegacy[phy_slotId].atr_len)) * 2 + 1;
        pAtr = new char[size];
        int ret = Value2HexString(pAtr, m_tSlotStatusInfoLegacy[phy_slotId].atr,
                (int)(m_tSlotStatusInfoLegacy[phy_slotId].atr_len));
        if (ret == -1) {
            delete [] pAtr;
            return NULL;
        }

        strAtr = pAtr;
        delete[] pAtr;
        return strAtr;
    }

    return "";
}

int ProtocolSimSlotStatusAdapterLegacy::GetPortInfoSize(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL) {
        ret = (int) (m_tSlotStatusInfo[phy_slotId].port_len);
    }
    return ret;
}

int ProtocolSimSlotStatusAdapterLegacy::GetLogicalSlotId(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfoLegacy != NULL) {
        ret = (int) (m_tSlotStatusInfoLegacy[phy_slotId].logicalSlotId);
    }
    return ret;
}

int ProtocolSimSlotStatusAdapterLegacy::GetLogicalSlotId(int phy_slotId, int portId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL) {
        ret = (int) (m_tSlotStatusInfo[phy_slotId].port_info[portId].logicalSlotId);
    }
    return ret;
}

int ProtocolSimSlotStatusAdapterLegacy::GetIccIdSize(int phy_slotId) const
{
    if (m_tSlotStatusInfoLegacy != NULL && m_tSlotStatusInfoLegacy[phy_slotId].iccid_len > 0) {
        return (int)(m_tSlotStatusInfoLegacy[phy_slotId].iccid_len * 2 + 1);
    }
    return 0;
}

int ProtocolSimSlotStatusAdapterLegacy::GetIccIdSize(int phy_slotId, int portId) const
{
    if (m_tSlotStatusInfo != NULL && m_tSlotStatusInfo[phy_slotId].port_info[portId].iccid_len > 0) {
        return (int)(m_tSlotStatusInfo[phy_slotId].port_info[portId].iccid_len * 2 + 1);
    }
    return 0;
}

std::string ProtocolSimSlotStatusAdapterLegacy::GetIccId(int phy_slotId) const
{
    if(m_tSlotStatusInfoLegacy != NULL) {
        return bchToString(m_tSlotStatusInfoLegacy[phy_slotId].iccid,
                (int)(m_tSlotStatusInfoLegacy[phy_slotId].iccid_len));
    }
    return NULL;
}

std::string ProtocolSimSlotStatusAdapterLegacy::GetIccId(int phy_slotId, int portId) const
{
    if(m_tSlotStatusInfo != NULL) {
        return bchToString(m_tSlotStatusInfo[phy_slotId].port_info[portId].iccid,
                (int)(m_tSlotStatusInfo[phy_slotId].port_info[portId].iccid_len));
    }
    return NULL;
}

int ProtocolSimSlotStatusAdapterLegacy::GetPortState(int phy_slotId, int portId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL) {
        ret = (int) (m_tSlotStatusInfo[phy_slotId].port_info[portId].port_state);
    }
    return ret;
}

int ProtocolSimSlotStatusAdapterLegacy::GetEidSize(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL && m_tSlotStatusInfo[phy_slotId].eid_len > 0) {
        ret = ((int)(m_tSlotStatusInfo[phy_slotId].eid_len)) * 2 + 1;
    } else if (m_tSlotStatusInfoLegacy != NULL && m_tSlotStatusInfoLegacy[phy_slotId].eid_len > 0) {
        ret = ((int)(m_tSlotStatusInfoLegacy[phy_slotId].eid_len)) * 2 + 1;
    }
    return ret;
}

std::string ProtocolSimSlotStatusAdapterLegacy::GetEid(int phy_slotId) const
{
    std::string strEid;
    char *pEid = NULL;
    if(m_tSlotStatusInfo != NULL) {
        int size = ((int)(m_tSlotStatusInfo[phy_slotId].eid_len)) * 2 + 1;
        pEid = new char[size];
        int ret = Value2HexString(pEid, m_tSlotStatusInfo[phy_slotId].eid,
                (int)(m_tSlotStatusInfo[phy_slotId].eid_len));
        if (ret == -1) {
            delete [] pEid;
            return NULL;
        }

        strEid = pEid;
        delete[] pEid;
        return strEid;
    } else if (m_tSlotStatusInfoLegacy != NULL) {
        int size = ((int)(m_tSlotStatusInfoLegacy[phy_slotId].eid_len)) * 2 + 1;
        pEid = new char[size];
        int ret = Value2HexString(pEid, m_tSlotStatusInfoLegacy[phy_slotId].eid,
                (int)(m_tSlotStatusInfoLegacy[phy_slotId].eid_len));
        if (ret == -1) {
            delete [] pEid;
            return NULL;
        }

        strEid = pEid;
        delete[] pEid;
        return strEid;
    }

    return "";
}


int ProtocolSimSlotStatusAdapterLegacy::FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const {
    int num_slots = GetNumOfSlotStatus();
    RilLogV("FillSimSlotStatusLegacy status size: %d", num_slots);
    simSlotStatus.sstVer = 12;

    for (int i = 0; i < num_slots; i++) {
        RIL_SimSlotStatus_1_2 sst = {};
        sst.cardState = (RIL_CardState) GetCardState(i);
        sst.slotState = (RIL_SlotState) GetSlotState(i);
        sst.logicalSlotId = GetLogicalSlotId(i);
        if (sst.logicalSlotId == INVALID_SLOT_ID) {
            sst.logicalSlotId = -1;
        }

        if (sst.cardState==(RIL_CardState) RIL_CARDSTATE_PRESENT ||
                sst.cardState ==(RIL_CardState) RIL_CARDSTATE_RESTRICTED) {
            int atr_size = GetAtrSize(i);
            if (atr_size > 0) {
                sst.atr = GetAtr(i);
            } else {
                sst.atr = "";
                RilLogE("FillSimSlotStatusLegacy atr for slot %d parsing failed", i);
            }

            int iccid_size = GetIccIdSize(i);
            if (iccid_size > 0) {
                sst.iccid = GetIccId(i);
            } else {
                sst.iccid = "";
                RilLogE("FillSimSlotStatusLegacy iccid for slot %d parsing failed", i);
            }

            int eid_size = GetEidSize(i);
            if (eid_size > 0) {
                sst.eid = GetEid(i);
            } else {
                sst.eid = "";
                RilLogE("FillSimSlotStatusLegacy eid for slot %d parsing failed", i);
            }
        }

        simSlotStatus.mSimSlotStatus.push_back(sst);
    }

    return sizeof(RIL_SimSlotStatusResult_1_2);
}

int ProtocolSimSlotStatusAdapterLegacy::FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const {
    int num_slots = GetNumOfSlotStatus();
    RilLogV("FillSimSlotStatus status size: %d", num_slots);
    simSlotStatus.sstVer = 20;

    for (int i = 0; i < num_slots; i++) {
        configV2_0::RIL_SimSlotStatus sst = {};
        sst.cardState = (RIL_CardState) GetCardState(i);
        int atr_size = GetAtrSize(i);
        if (atr_size > 0) {
            sst.atr = GetAtr(i);
        } else {
            sst.atr = "";
            RilLogE("FillSimSlotStatus atr for slot %d parsing failed", i);
        }

        int eid_size = GetEidSize(i);
        if (eid_size > 0) {
            sst.eid = GetEid(i);
        } else {
            sst.eid = "";
            RilLogE("FillSimSlotStatus eid for slot %d parsing failed", i);
        }

        for (int j = 0; j < GetPortInfoSize(i); j++) {
            configV2_0::RIL_PORT_INFO pi = {};
            pi.logicalSlotId =
                    GetLogicalSlotId(i, j);
            if (pi.logicalSlotId == INVALID_SLOT_ID) {
                pi.logicalSlotId = -1;
            }

            int portState = GetPortState(i, j);
            pi.port_state =
                    (portState == configV2_0::PORT_ACTIVE ? true : false);
            int iccid_size = GetIccIdSize(i, j);
            if (iccid_size > 0) {
                pi.iccid = GetIccId(i, j);
            } else {
                pi.iccid = "";
                RilLogE("FillSimSlotStatus iccid for slot %d parsing failed", i);
            }

            sst.port_info.push_back(pi);
        }

        simSlotStatus.mSimSlotStatus.push_back(sst);
    }

    return sizeof(configV2_0::RIL_SimSlotStatusResult);
}

// Slot status changed
void ProtocolSlotStatusChangedAdapterLegacy::Init()
{
    if (m_pModemData != NULL) {
        UINT modemDataLength = m_pModemData->GetLength();
        if (modemDataLength <= sizeof(RCM_HEADER)) {
            RilLogE("%s() ProtocolSlotStatusChangedAdapterLegacy modemDataLength is wrong",
                    __FUNCTION__);
            return;
        }
        if (modemDataLength > sizeof(sit_sim_slot_status_changed_ind_v1_2)) {
            sit_sim_slot_status_changed_ind *pData
                    = (sit_sim_slot_status_changed_ind *) m_pModemData->GetRawData();
            if (pData != NULL && pData->hdr.id == SIT_IND_SIM_SLOT_STATUS_CHANGED
                    && pData->num_of_info > 0 && pData->num_of_info <= MAX_SLOT_NUM) {
                m_tSlotStatusInfo = new sit_sim_get_slot_status_info[pData->num_of_info];
                sit_sim_get_slot_status_info *pData_info
                        = (sit_sim_get_slot_status_info *) pData->info;

                for (int slotId = 0; slotId < pData->num_of_info; slotId++) {
                    memcpy(&(m_tSlotStatusInfo[slotId]), &(pData_info[slotId]),
                            sizeof(sit_sim_get_slot_status_info));
                }
                RilLogI("%s() ProtocolSlotStatusChangedAdapter m_tSlotStatusInfo is initialized",
                        __FUNCTION__);
            }
        } else {
            sit_sim_slot_status_changed_ind_v1_2 *pData
                    = (sit_sim_slot_status_changed_ind_v1_2 *) m_pModemData->GetRawData();
            if (pData != NULL && pData->hdr.id == SIT_IND_SIM_SLOT_STATUS_CHANGED
                    && pData->num_of_info > 0 && pData->num_of_info <= MAX_SLOT_NUM) {
                m_tSlotStatusInfoLegacy = new sit_sim_get_slot_status_info_v1_2[pData->num_of_info];
                sit_sim_get_slot_status_info_v1_2 *pData_info
                        = (sit_sim_get_slot_status_info_v1_2 *) pData->info;
                unsigned int sizefor_1_0 =
                        sizeof(sit_sim_get_slot_status_info_v1_2) - (MAX_EID_LEN + 1);
                mIsLegacy = true;

                for (int slotId = 0; slotId < pData->num_of_info; slotId++) {
                    memcpy(&(m_tSlotStatusInfoLegacy[slotId]), &(pData_info[slotId]), sizefor_1_0);

                    // for RadioConfig v1.2
                    if (modemDataLength > (sizefor_1_0 * pData->num_of_info + 1)) {
                        m_tSlotStatusInfoLegacy[slotId].eid_len = pData_info[slotId].eid_len;
                        if (pData_info[slotId].eid_len <= MAX_EID_LEN) {
                            memcpy(&(m_tSlotStatusInfoLegacy[slotId].eid),
                                    &(pData_info[slotId].eid), pData_info[slotId].eid_len);
                        } else {
                            RilLogI("%s() ProtocolSlotStatusChangedAdapter eid_len(%d) is wrong",
                                    __FUNCTION__, m_tSlotStatusInfoLegacy[slotId].eid_len);
                            m_tSlotStatusInfoLegacy[slotId].eid_len = 0;
                        }
                    }
                }
                RilLogI("%s() ProtocolSlotStatusChangedAdapter m_tSlotStatusInfoLegacy is initialized",
                        __FUNCTION__);
            }
        }
    }
}

void ProtocolSlotStatusChangedAdapterLegacy::Dispose()
{
    RilLogI("%s() ProtocolSlotStatusChangedAdapterLegacy is disposed", __FUNCTION__);
    if (m_tSlotStatusInfo != NULL) {
        delete[] m_tSlotStatusInfo;
    }

    if (m_tSlotStatusInfoLegacy != NULL) {
        delete[] m_tSlotStatusInfoLegacy;
    }
    m_tSlotStatusInfo = NULL;
    m_tSlotStatusInfoLegacy = NULL;
}

bool ProtocolSlotStatusChangedAdapterLegacy::IsLegacySlotStatus() const
{
    return mIsLegacy;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetNumOfSlotStatus() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        if (m_tSlotStatusInfo != NULL) {
            sit_sim_slot_status_changed_ind *data = (sit_sim_slot_status_changed_ind *)m_pModemData->GetRawData();
            ret = (int)(data->num_of_info);
        } else {
            sit_sim_slot_status_changed_ind_v1_2 *data = (sit_sim_slot_status_changed_ind_v1_2 *)m_pModemData->GetRawData();
            ret = (int)(data->num_of_info);
        }
    }
    return ret;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetCardState(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL) {
        ret = (int)(m_tSlotStatusInfo[phy_slotId].card_state);
    } else if (m_tSlotStatusInfoLegacy != NULL) {
        ret = (int)(m_tSlotStatusInfoLegacy[phy_slotId].card_state);
    }
    return ret;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetSlotState(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfoLegacy != NULL) {
        ret = (int)(m_tSlotStatusInfoLegacy[phy_slotId].slot_State);
    }
    return ret;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetAtrSize(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL && m_tSlotStatusInfo[phy_slotId].atr_len > 0) {
        ret = ((int)(m_tSlotStatusInfo[phy_slotId].atr_len)) * 2 + 1;
    } else if (m_tSlotStatusInfoLegacy != NULL && m_tSlotStatusInfoLegacy[phy_slotId].atr_len > 0) {
        ret = ((int)(m_tSlotStatusInfoLegacy[phy_slotId].atr_len)) * 2 + 1;
    }
    return ret;
}

std::string ProtocolSlotStatusChangedAdapterLegacy::GetAtr(int phy_slotId) const
{
    std::string strAtr;
    char *pAtr = NULL;
    if(m_tSlotStatusInfo != NULL) {
        int size = ((int)(m_tSlotStatusInfo[phy_slotId].atr_len)) * 2 + 1;
        pAtr = new char[size];
        int ret = Value2HexString(pAtr, m_tSlotStatusInfo[phy_slotId].atr,
                (int)(m_tSlotStatusInfo[phy_slotId].atr_len));
        if (ret == -1) {
            delete [] pAtr;
            return NULL;
        }

        strAtr = pAtr;
        delete[] pAtr;
        return strAtr;
    } else if (m_tSlotStatusInfoLegacy != NULL) {
        int size = ((int)(m_tSlotStatusInfoLegacy[phy_slotId].atr_len)) * 2 + 1;
        pAtr = new char[size];
        int ret = Value2HexString(pAtr, m_tSlotStatusInfoLegacy[phy_slotId].atr,
                (int)(m_tSlotStatusInfoLegacy[phy_slotId].atr_len));
        if (ret == -1) {
            delete [] pAtr;
            return NULL;
        }

        strAtr = pAtr;
        delete[] pAtr;
        return strAtr;
    }
    return "";
}

int ProtocolSlotStatusChangedAdapterLegacy::GetPortInfoSize(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL) {
        ret = (int) (m_tSlotStatusInfo[phy_slotId].port_len);
    }
    return ret;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetLogicalSlotId(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfoLegacy != NULL) {
        ret = (int) (m_tSlotStatusInfoLegacy[phy_slotId].logicalSlotId);
    }
    return ret;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetLogicalSlotId(int phy_slotId, int portId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL) {
        ret = (int) (m_tSlotStatusInfo[phy_slotId].port_info[portId].logicalSlotId);
    }
    return ret;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetIccIdSize(int phy_slotId) const
{
    if (m_tSlotStatusInfoLegacy != NULL && m_tSlotStatusInfoLegacy[phy_slotId].iccid_len > 0) {
        return (int)(m_tSlotStatusInfoLegacy[phy_slotId].iccid_len * 2 + 1);
    }
    return 0;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetIccIdSize(int phy_slotId, int portId) const
{
    if (m_tSlotStatusInfo != NULL && m_tSlotStatusInfo[phy_slotId].port_info[portId].iccid_len) {
        return (int)(m_tSlotStatusInfo[phy_slotId].port_info[portId].iccid_len * 2 + 1);
    }
    return 0;
}

std::string ProtocolSlotStatusChangedAdapterLegacy::GetIccId(int phy_slotId) const
{
    if(m_tSlotStatusInfoLegacy != NULL) {
        return bchToString(m_tSlotStatusInfoLegacy[phy_slotId].iccid,
                (int)(m_tSlotStatusInfoLegacy[phy_slotId].iccid_len));
    }
    return NULL;
}

std::string ProtocolSlotStatusChangedAdapterLegacy::GetIccId(int phy_slotId, int portId) const
{
    if(m_tSlotStatusInfo != NULL) {
        return bchToString(m_tSlotStatusInfo[phy_slotId].port_info[portId].iccid,
                (int)(m_tSlotStatusInfo[phy_slotId].port_info[portId].iccid_len));
    }
    return NULL;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetPortState(int phy_slotId, int portId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL) {
        ret = (int) (m_tSlotStatusInfo[phy_slotId].port_info[portId].port_state);
    }
    return ret;
}

int ProtocolSlotStatusChangedAdapterLegacy::GetEidSize(int phy_slotId) const
{
    int ret = 0;
    if (m_tSlotStatusInfo != NULL && m_tSlotStatusInfo[phy_slotId].eid_len > 0) {
        ret = ((int)(m_tSlotStatusInfo[phy_slotId].eid_len)) * 2 + 1;
    } else if (m_tSlotStatusInfoLegacy != NULL && m_tSlotStatusInfoLegacy[phy_slotId].eid_len > 0) {
        ret = ((int)(m_tSlotStatusInfoLegacy[phy_slotId].eid_len)) * 2 + 1;
    }
    return ret;
}

std::string ProtocolSlotStatusChangedAdapterLegacy::GetEid(int phy_slotId) const
{
    std::string strEid;
    char *pEid = NULL;
    if(m_tSlotStatusInfo != NULL) {
        int size = ((int)(m_tSlotStatusInfo[phy_slotId].eid_len)) * 2 + 1;
        pEid = new char[size];
        int ret = Value2HexString(pEid, m_tSlotStatusInfo[phy_slotId].eid,
                (int)(m_tSlotStatusInfo[phy_slotId].eid_len));
        if (ret == -1) {
            delete [] pEid;
            return NULL;
        }

        strEid = pEid;
        delete[] pEid;
        return strEid;
    } else if (m_tSlotStatusInfoLegacy != NULL) {
        int size = ((int)(m_tSlotStatusInfoLegacy[phy_slotId].eid_len)) * 2 + 1;
        pEid = new char[size];
        int ret = Value2HexString(pEid, m_tSlotStatusInfoLegacy[phy_slotId].eid,
                (int)(m_tSlotStatusInfoLegacy[phy_slotId].eid_len));
        if (ret == -1) {
            delete [] pEid;
            return NULL;
        }

        strEid = pEid;
        delete[] pEid;
        return strEid;
    }
    return "";
}

int ProtocolSlotStatusChangedAdapterLegacy::FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const {
    int num_slots = GetNumOfSlotStatus();
    RilLogV("FillSimSlotStatusLegacy status size: %d", num_slots);
    simSlotStatus.sstVer = 12;

    for (int i = 0; i < num_slots; i++) {
        RIL_SimSlotStatus_1_2 sst = {};
        sst.cardState = (RIL_CardState) GetCardState(i);
        sst.slotState = (RIL_SlotState) GetSlotState(i);
        sst.logicalSlotId = GetLogicalSlotId(i);
        if (sst.logicalSlotId == INVALID_SLOT_ID) {
            sst.logicalSlotId = -1;
        }

        if (sst.cardState==(RIL_CardState) RIL_CARDSTATE_PRESENT ||
                sst.cardState ==(RIL_CardState) RIL_CARDSTATE_RESTRICTED) {
            int atr_size = GetAtrSize(i);
            if (atr_size > 0) {
                sst.atr = GetAtr(i);
            } else {
                sst.atr = "";
                RilLogE("FillSimSlotStatusLegacy atr for slot %d parsing failed", i);
            }

            int iccid_size = GetIccIdSize(i);
            if (iccid_size > 0) {
                sst.iccid = GetIccId(i);
            } else {
                sst.iccid = "";
                RilLogE("FillSimSlotStatusLegacy iccid for slot %d parsing failed", i);
            }

            int eid_size = GetEidSize(i);
            if (eid_size > 0) {
                sst.eid = GetEid(i);
            } else {
                sst.eid = "";
                RilLogE("FillSimSlotStatusLegacy eid for slot %d parsing failed", i);
            }
        }

        simSlotStatus.mSimSlotStatus.push_back(sst);
    }

    return sizeof(RIL_SimSlotStatusResult_1_2);
}

int ProtocolSlotStatusChangedAdapterLegacy::FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const {
    int num_slots = GetNumOfSlotStatus();
    RilLogV("FillSimSlotStatus status size: %d", num_slots);
    simSlotStatus.sstVer = 20;

    for (int i = 0; i < num_slots; i++) {
        configV2_0::RIL_SimSlotStatus sst = {};
        sst.cardState = (RIL_CardState) GetCardState(i);
        int atr_size = GetAtrSize(i);
        if (atr_size > 0) {
            sst.atr = GetAtr(i);
        } else {
            sst.atr = "";
            RilLogE("FillSimSlotStatus atr for slot %d parsing failed", i);
        }

        int eid_size = GetEidSize(i);
        if (eid_size > 0) {
            sst.eid = GetEid(i);
        } else {
            sst.eid = "";
            RilLogE("FillSimSlotStatus eid for slot %d parsing failed", i);
        }

        for (int j = 0; j < GetPortInfoSize(i); j++) {
            configV2_0::RIL_PORT_INFO pi = {};
            pi.logicalSlotId =
                    GetLogicalSlotId(i, j);
            if (pi.logicalSlotId == INVALID_SLOT_ID) {
                pi.logicalSlotId = -1;
            }

            int portState = GetPortState(i, j);
            pi.port_state =
                    (portState == configV2_0::PORT_ACTIVE ? true : false);
            int iccid_size = GetIccIdSize(i, j);
            if (iccid_size > 0) {
                pi.iccid = GetIccId(i, j);
            } else {
                pi.iccid = "";
                RilLogE("FillSimSlotStatus iccid for slot %d parsing failed", i);
            }

            sst.port_info.push_back(pi);
        }

        simSlotStatus.mSimSlotStatus.push_back(sst);
    }

    return sizeof(configV2_0::RIL_SimSlotStatusResult);
}
