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
 * protocolradioconfigadapterjson.cpp
 *
 *  Created on: 2021. 8. 6.
 */
#include "protocolradioconfigadapterjson.h"
#include "protocolutils.h"
#include "rillog.h"
#include "util.h"
#include "simjsondef.h"
#include "sitdef.h"

/**
 * ProtocolPhoneCapabilityAdapterjson
 */
ProtocolPhoneCapabilityAdapterJson::ProtocolPhoneCapabilityAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_PHONE_CAPABILITY), ProtocolPhoneCapabilityAdapterInterface() {
    // logicalModemList not supported by modem
    // generate logical modemId using index
    int size = sizeof(mLogicalModemList)/sizeof(mLogicalModemList[0]);
    for (int i = 0; i < size; i++) {
        mLogicalModemList[i] = i;
    }
}

int ProtocolPhoneCapabilityAdapterJson::GetMaxActiveData() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_sim_ps_get_phone_capability_rsp::key_max_simultaneous_data_stack].asInt();
    }
    return 1;
}

int ProtocolPhoneCapabilityAdapterJson::GetMaxActiveInternetData() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_sim_ps_get_phone_capability_rsp::key_max_simultaneous_internet_pdn].asInt();
    }
    return 1;
}

bool ProtocolPhoneCapabilityAdapterJson::IsInternetLingeringSupported() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_sim_ps_get_phone_capability_rsp::key_internet_lingering_support].asInt();
    }
    return false;
}

int ProtocolPhoneCapabilityAdapterJson::GetLogicalModemListSize() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_sim_ps_get_phone_capability_rsp::key_max_supported_stack].asInt();
    }

    return 1;
}

int *ProtocolPhoneCapabilityAdapterJson::GetLogicalModemList() {
    return mLogicalModemList;
}

UINT ProtocolPhoneCapabilityAdapterJson::GetErrorCode() const {
    return ProtocolRespAdapter::GetErrorCode();
}

/*
 * ProtocolSlotStatusAdapterJson
 */
ProtocolSimSlotStatusAdapterJson::ProtocolSimSlotStatusAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimSlotStatusAdapterJson::Init() {
    ParseJsonPayload(SIT_GET_SLOT_STATUS);
    if(m_bParsingSuccess) {
        // store slot_status_info data in m_jsonRspSlotStatusData
        m_jsonRspSlotStatusData = m_jsonData[ns_sim_get_slot_status_resp::key_slot_status_info_list];
    }
}

int ProtocolSimSlotStatusAdapterJson::GetNumOfSlotStatus() const {
    int numOfSlot = 0;
    if(m_bParsingSuccess) {
        numOfSlot =  m_jsonData[ns_sim_get_slot_status_resp::key_slot_status_info_list].size();
    }
    return numOfSlot;
}

int ProtocolSimSlotStatusAdapterJson::GetCardState(int phy_slotId) const {
    int nCardState = 0;
    if(m_bParsingSuccess) {
        nCardState = m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_card_state].asInt();
    }
    return nCardState;
}

int ProtocolSimSlotStatusAdapterJson::GetSlotState(int phy_slotId) const {
    int nSlotState = 0;
    if(m_bParsingSuccess) {
        nSlotState = m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_slot_state].asInt();
    }
    return nSlotState;
}

int ProtocolSimSlotStatusAdapterJson::GetAtrSize(int phy_slotId) const {
    unsigned int nAtrSize = 0;
    if(m_bParsingSuccess) {
        unsigned int nAtrValueSize = m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_atr].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nAtrValueSize, MAX_ATR_LEN);
        // ATR size in hex string
        if(nAtrValueSize > 0) nAtrSize = nAtrValueSize * 2 + 1;
    }
    return nAtrSize;
}

std::string ProtocolSimSlotStatusAdapterJson::GetAtr(int phy_slotId) const {
    std::string strAtr;
    char *pAtr = NULL;
    if(m_bParsingSuccess) {
        int nAtrSize = GetAtrSize(phy_slotId);
        pAtr = new char[nAtrSize];
        unsigned int nAtrValueSize = m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_atr].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nAtrValueSize, MAX_ATR_LEN);
        BYTE atr[nAtrValueSize];
        memset(atr, 0, nAtrValueSize);
        for(unsigned int i = 0; i < nAtrValueSize; i++) {
            atr[i] = (BYTE) m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_atr][i].asInt();
        }
        int ret = Value2HexString(pAtr, atr, nAtrValueSize);
        if (ret == -1) {
            delete [] pAtr;
            pAtr = NULL;
            return NULL;
        }

        strAtr = pAtr;
        delete[] pAtr;
        return strAtr;
    }
    return "";

}

int ProtocolSimSlotStatusAdapterJson::GetLogicalSlotId(int phy_slotId) const {
    int nLogicalSlotId = 0;
    if(m_bParsingSuccess) {
        nLogicalSlotId = m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_log_slot_id].asInt();
    }
    return nLogicalSlotId;
}

int ProtocolSimSlotStatusAdapterJson::GetIccIdSize(int phy_slotId) const {
    unsigned int nIccIdSize = 0;
    if(m_bParsingSuccess) {
        unsigned int iccIdValueSize = m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_iccid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, iccIdValueSize, MAX_ICCID_LEN);
        // iccid size in hex string
        if(iccIdValueSize > 0 ) nIccIdSize = iccIdValueSize * 2 + 1;
    }
    return nIccIdSize;
}

std::string ProtocolSimSlotStatusAdapterJson::GetIccId(int phy_slotId) const {
    std::string iccId = "";
    if(m_bParsingSuccess) {
        unsigned int nIccIdValueSize = m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_iccid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nIccIdValueSize, MAX_ICCID_LEN);
        BYTE iccIdValue[nIccIdValueSize];
        memset(iccIdValue, 0, nIccIdValueSize);
        for(unsigned int i = 0; i < nIccIdValueSize; i++) {
            iccIdValue[i] = (BYTE) m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_iccid][i].asInt();
        }
        iccId = bchToString(iccIdValue, nIccIdValueSize);
    }
    return iccId;
}

int ProtocolSimSlotStatusAdapterJson::GetEidSize(int phy_slotId) const {
    unsigned int nEidSize = 0;
    if(m_bParsingSuccess) {
        unsigned int nEidValueSize = m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_eid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nEidValueSize, MAX_EID_LEN);
        // eid size in hex string
        if(nEidValueSize > 0) nEidSize = nEidValueSize * 2 + 1;
    }
    return nEidSize;
}

std::string ProtocolSimSlotStatusAdapterJson::GetEid(int phy_slotId) const {
    std::string strEid;
    char *pEid = NULL;
    if(m_bParsingSuccess) {
        int nEidSize = GetEidSize(phy_slotId);
        pEid = new char[nEidSize];
        unsigned int nEidValueSize = m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_eid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nEidValueSize, MAX_EID_LEN);
        BYTE eid[nEidValueSize];
        memset(eid, 0, nEidValueSize);
        for(unsigned int i = 0; i < nEidValueSize; i++) {
            eid[i] = (BYTE) m_jsonRspSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_eid][i].asInt();
        }
        int ret = Value2HexString(pEid, eid, nEidValueSize);
        if (ret == -1) {
            delete [] pEid;
            pEid = NULL;
            return NULL;
        }
        strEid = pEid;
        delete[] pEid;
        return strEid;
    }

    return "";

}

int ProtocolSimSlotStatusAdapterJson::FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const {

    simSlotStatus.sstVer = 12;
    return sizeof(RIL_SimSlotStatusResult_1_2);
}

int ProtocolSimSlotStatusAdapterJson::FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const {

    simSlotStatus.sstVer = 20;
    return sizeof(configV2_0::RIL_SimSlotStatusResult);
}

/*
 * ProtocolSlotStatusChangedAdapterJson
 */
ProtocolSlotStatusChangedAdapterJson::ProtocolSlotStatusChangedAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolSlotStatusChangedAdapterJson::Init() {
    ParseJsonPayload(SIT_IND_SIM_SLOT_STATUS_CHANGED);
    if(m_bParsingSuccess) {
        // store slot_status_info data in m_jsonIndSlotStatusData
        m_jsonIndSlotStatusData = m_jsonData[ns_sim_slot_status_changed_ind::key_slot_status_info_list];
    }
}

int ProtocolSlotStatusChangedAdapterJson::GetNumOfSlotStatus() const {
    int numOfSlot = 0;
    if(m_bParsingSuccess) {
        numOfSlot =  m_jsonData[ns_sim_slot_status_changed_ind::key_slot_status_info_list].size();
    }
    return numOfSlot;
}

int ProtocolSlotStatusChangedAdapterJson::GetCardState(int phy_slotId) const {
    int nCardState = 0;
    if(m_bParsingSuccess) {
        nCardState = m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_card_state].asInt();
    }
    return nCardState;
}

int ProtocolSlotStatusChangedAdapterJson::GetSlotState(int phy_slotId) const {
    int nSlotState = 0;
    if(m_bParsingSuccess) {
        nSlotState = m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_slot_state].asInt();
    }
    return nSlotState;
}

int ProtocolSlotStatusChangedAdapterJson::GetAtrSize(int phy_slotId) const {
    unsigned int nAtrSize = 0;
    if(m_bParsingSuccess) {
        unsigned int nAtrValueSize = m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_atr].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nAtrValueSize, MAX_ATR_LEN);

        // ATR size in hex string
        if(nAtrValueSize > 0) nAtrSize = nAtrValueSize * 2 + 1;
    }
    return nAtrSize;
}

std::string ProtocolSlotStatusChangedAdapterJson::GetAtr(int phy_slotId) const {
    std::string strAtr;
    char *pAtr = NULL;
    if(m_bParsingSuccess) {
        int nAtrSize = GetAtrSize(phy_slotId);
        pAtr = new char[nAtrSize];
        unsigned int nAtrValueSize = m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_atr].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nAtrValueSize, MAX_ATR_LEN);
        BYTE atr[nAtrValueSize];
        memset(atr, 0, nAtrValueSize);
        for(unsigned int i = 0; i < nAtrValueSize; i++) {
            atr[i] = (BYTE) m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_atr][i].asInt();
        }
        int ret = Value2HexString(pAtr, atr, nAtrValueSize);
        if (ret == -1) {
            delete [] pAtr;
            pAtr = NULL;
            return NULL;
        }
        strAtr = pAtr;
        delete[] pAtr;
        return strAtr;
    }
    return "";
}

int ProtocolSlotStatusChangedAdapterJson::GetLogicalSlotId(int phy_slotId) const {
    int nLogicalSlotId = 0;
    if(m_bParsingSuccess) {
        nLogicalSlotId = m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_log_slot_id].asInt();
    }
    return nLogicalSlotId;
}

int ProtocolSlotStatusChangedAdapterJson::GetIccIdSize(int phy_slotId) const {
    int nIccIdSize = 0;
    if(m_bParsingSuccess) {
        unsigned int iccIdValueSize = m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_iccid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, iccIdValueSize, MAX_ICCID_LEN);
        // iccid size in hex string
        if(iccIdValueSize > 0 ) nIccIdSize = iccIdValueSize * 2 + 1;
    }
    return nIccIdSize;
}

string ProtocolSlotStatusChangedAdapterJson::GetIccId(int phy_slotId) const {
    string iccId = "";
    if(m_bParsingSuccess) {
        unsigned int nIccIdValueSize = m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_iccid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nIccIdValueSize, MAX_ICCID_LEN);
        BYTE iccIdValue[nIccIdValueSize];
        memset(iccIdValue, 0, nIccIdValueSize);
        for(unsigned int i = 0; i < nIccIdValueSize; i++) {
            iccIdValue[i] = (BYTE) m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_iccid][i].asInt();
        }
        iccId = bchToString(iccIdValue, nIccIdValueSize);
    }
    return iccId;
}

int ProtocolSlotStatusChangedAdapterJson::GetEidSize(int phy_slotId) const {
    int nEidSize = 0;
    if(m_bParsingSuccess) {
        unsigned int nEidValueSize = m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_eid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nEidValueSize, MAX_EID_LEN);
        // eid size in hex string
        if(nEidValueSize > 0) nEidSize = nEidValueSize * 2 + 1;
    }
    return nEidSize;
}

std::string ProtocolSlotStatusChangedAdapterJson::GetEid(int phy_slotId) const {
    std::string strEid;
    char *pEid = NULL;
    if(m_bParsingSuccess) {
        int nEidSize = GetEidSize(phy_slotId);
        pEid = new char[nEidSize];
        unsigned int nEidValueSize = m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_eid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nEidValueSize, MAX_EID_LEN);
        BYTE eid[nEidValueSize];
        memset(eid, 0, nEidValueSize);
        for(unsigned int i = 0; i < nEidValueSize; i++) {
            eid[i] = (BYTE) m_jsonIndSlotStatusData[phy_slotId][ns_sim_slot_status_info::key_eid][i].asInt();
        }
        int ret = Value2HexString(pEid, eid, nEidValueSize);
        if (ret == -1) {
            delete [] pEid;
            pEid = NULL;
            return NULL;
        }
        strEid = pEid;
        delete[] pEid;
        return strEid;
    }

    return "";
}
int ProtocolSlotStatusChangedAdapterJson::FillSimSlotStatusLegacy(RIL_SimSlotStatusResult_1_2 &simSlotStatus) const {

    simSlotStatus.sstVer = 12;
    return sizeof(RIL_SimSlotStatusResult_1_2);
}

int ProtocolSlotStatusChangedAdapterJson::FillSimSlotStatus(configV2_0::RIL_SimSlotStatusResult &simSlotStatus) const {

    simSlotStatus.sstVer = 20;
    return sizeof(configV2_0::RIL_SimSlotStatusResult);
}
