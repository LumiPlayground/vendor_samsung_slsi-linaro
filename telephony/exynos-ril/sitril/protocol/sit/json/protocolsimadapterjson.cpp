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
 * protocolsimadapterjson.cpp
 *
 *  Created on: 2021. 7. 19.
 *      Author: aryaman.2019
 */

#include "protocolsimadapterjson.h"
#include "protocolutils.h"
#include "rillog.h"
#include "simjsondef.h"
#include "util.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * ProtocolSimStatusAdapterJson
 */
ProtocolSimStatusAdapterJson::ProtocolSimStatusAdapterJson(const ModemData *pModemData)
    : ProtocolSimStatusAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimStatusAdapterJson::Init() {
    m_nAtrLen = -1;
    m_atr = NULL;
    m_nIccidLen = -1;
    m_iccid = NULL;
    m_nEidLen = -1;
    m_eid = NULL;
    ParseJsonPayload(SIT_GET_SIM_STATUS);
    if(m_bParsingSuccess) {
        // store app_status_info data in m_jsonRspAppData
        m_jsonRspAppData = m_jsonData[ns_sim_get_sim_status_rsp::key_app_status_info_list];
        SetAtr();
        SetIccid();
        SetEid();
    }
    // RilLogI("[%s] Json_Data = %s",  __FUNCTION__, GetParameter());
}

void ProtocolSimStatusAdapterJson::SetAtr() {
    if(m_bParsingSuccess) {
        m_nAtrLen = m_jsonData[ns_sim_get_sim_status_rsp::key_atr].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, m_nAtrLen, MAX_ATR_LEN);
        m_atr = new char[m_nAtrLen];
        memset(m_atr, 0, m_nAtrLen);
        for(unsigned int i = 0; i < m_nAtrLen; i++) {
            m_atr[i] = (char) m_jsonData[ns_sim_get_sim_status_rsp::key_atr][i].asInt();
        }
    }
}

void ProtocolSimStatusAdapterJson::SetIccid() {
    if(m_bParsingSuccess) {
        m_nIccidLen = m_jsonData[ns_sim_get_sim_status_rsp::key_iccid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, m_nIccidLen, MAX_ICCID_LEN);
        m_iccid = new char[m_nIccidLen];
        memset(m_iccid, 0, m_nIccidLen);
        for(unsigned int i = 0; i < m_nIccidLen; i++) {
            m_iccid[i] = (char) m_jsonData[ns_sim_get_sim_status_rsp::key_iccid][i].asInt();
        }
    }
}

void ProtocolSimStatusAdapterJson::SetEid() {
    if(m_bParsingSuccess) {
        m_nEidLen = m_jsonData[ns_sim_get_sim_status_rsp::key_eid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, m_nIccidLen, MAX_EID_LEN);
        m_eid = new char[m_nEidLen];
        memset(m_eid, 0, m_nEidLen);
        for(unsigned int i = 0; i < m_nEidLen; i++) {
            m_eid[i] = (char) m_jsonData[ns_sim_get_sim_status_rsp::key_eid][i].asInt();
        }
    }
}

int ProtocolSimStatusAdapterJson::GetCardState() const {
    int nCardState = -1;
    if(m_bParsingSuccess) {
        nCardState = m_jsonData[ns_sim_get_sim_status_rsp::key_card_state].asInt();
    }
    return nCardState;
}

int ProtocolSimStatusAdapterJson::GetUniversalPinState() const {
    int nUnivPinState = -1;
    if(m_bParsingSuccess) {
        nUnivPinState = m_jsonData[ns_sim_get_sim_status_rsp::key_univ_pin_state].asInt();
    }
    return nUnivPinState;
}

int ProtocolSimStatusAdapterJson::GetApplicationCount() const {
    int nAppCount = -1;
    if(m_bParsingSuccess) {
        nAppCount = m_jsonData[ns_sim_get_sim_status_rsp::key_app_status_info_list].size();
    }
    return nAppCount;
}

int ProtocolSimStatusAdapterJson::GetAppsType(int nIndex) const {
    int nAppType = -1;
    if(m_bParsingSuccess) {
        nAppType = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_app_type].asInt();
    }
    return nAppType;
}

int ProtocolSimStatusAdapterJson::GetAppsState(int nIndex) const {
    int nAppState = -1;
    if(m_bParsingSuccess) {
        nAppState = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_app_state].asInt();
    }
    return nAppState;
}

int ProtocolSimStatusAdapterJson::GetPersonalSubstate(int nIndex) const {
    int nPersoSubState = -1;
    if(m_bParsingSuccess) {
        nPersoSubState = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_perso_substate].asInt();
    }
    return nPersoSubState;
}

int ProtocolSimStatusAdapterJson::GetApplicationLabel(int nIndex, BYTE *pAppLabel) const {
    int nAppLabelLen = -1;
    if(pAppLabel == NULL) {
        RilLogE("ProtocolSimStatusAdapterJson::%s() pAppLabel is NULL!!", __FUNCTION__);
        return -1;
    }
    if(m_bParsingSuccess) {
        nAppLabelLen = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_app_label].size();
        int nNumberOfApps = m_jsonData[ns_sim_get_sim_status_rsp::key_app_status_info_list].size();
        if(nIndex >= 0 && nIndex < nNumberOfApps && nAppLabelLen <= MAX_SIM_APP_LABEL_LEN) {
            for(int i = 0; i < nAppLabelLen; i++) {
                pAppLabel[i] = (BYTE) m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_app_label][i].asInt();
            }
        }
    }
    return nAppLabelLen;
}

int ProtocolSimStatusAdapterJson::GetPin1Replaced(int nIndex) const {
    int nPin1Replaced = -1;
    if(m_bParsingSuccess) {
        nPin1Replaced = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_pin1_replaced].asInt();
    }
    return nPin1Replaced;
}

int ProtocolSimStatusAdapterJson::GetPinState(int nIndex, int nPinIndex) const {
    int nPinState = 0;
    if(m_bParsingSuccess) {
        switch(nPinIndex)
        {
        case 1:
            nPinState = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_pin1_state].asInt();
            break;
        case 2:
            nPinState = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_pin2_state].asInt();
            break;
        }
    }
    return nPinState;
}

int ProtocolSimStatusAdapterJson::GetPinRemainCount(int nIndex, int nPinIndex) const {
    int nPinRemainCount = -1;
    if(m_bParsingSuccess) {
        switch(nPinIndex)
        {
        case 1:
            nPinRemainCount = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_pin1_remain_cnt].asInt();
            break;
        case 2:
            nPinRemainCount = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_pin2_remain_cnt].asInt();
            break;
        }
    }
    return nPinRemainCount;
}

int ProtocolSimStatusAdapterJson::GetPukRemainCount(int nIndex, int nPukIndex) const {
    int nPukRemainCount = -1;
    if(m_bParsingSuccess) {
        switch(nPukIndex)
        {
        case 1:
            nPukRemainCount = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_puk1_remain_cnt].asInt();
            break;
        case 2:
            nPukRemainCount = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_puk2_remain_cnt].asInt();
            break;
        }
    }
    return nPukRemainCount;
}

char *ProtocolSimStatusAdapterJson::GetAID(int nIndex) const {
    char *pAID = NULL;
    if(m_bParsingSuccess) {
        unsigned int aidLen = m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_aid].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, aidLen, MAX_SIM_AID_LEN);
        BYTE aid[aidLen];
        for(unsigned int i = 0; i < aidLen; i++) {
            aid[i] = (BYTE) m_jsonRspAppData[nIndex][ns_sim_apps_status_info::key_aid][i].asInt();
        }
        pAID = new char[(aidLen*2)+1];
        int ret = Value2HexString(pAID, aid, aidLen);
        if (ret == -1) {
            delete [] pAID;
            return NULL;
        }
    }
    return pAID;
}

bool ProtocolSimStatusAdapterJson::GetEsimNoProfile() const {
    int esimProf = 0;
    if(m_bParsingSuccess) {
        esimProf = m_jsonData[ns_sim_get_sim_status_rsp::key_esim_no_profile].asInt();
    }
    return esimProf == 1;
}

int ProtocolSimStatusAdapterJson::GetPhysicalSlotId() const {
    int slotId = -1;
    if(m_bParsingSuccess) {
        slotId = m_jsonData[ns_sim_get_sim_status_rsp::key_phy_slotid].asInt();
    }
    return slotId;
}

/**
 * ProtocolSimVerifyPinAdapterJson
 */
ProtocolSimVerifyPinAdapterJson::ProtocolSimVerifyPinAdapterJson(const ModemData *pModemData)
    : ProtocolSimVerifyPinAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimVerifyPinAdapterJson::Init() {
    m_nPinIndex = 0;
    m_nRemainCount = -1;

    ParseJsonPayload(SIT_VERIFY_SIM_PIN) || ParseJsonPayload(SIT_CHG_SIM_PIN)
        || ParseJsonPayload(SIT_VERIFY_SIM_PIN2) || ParseJsonPayload(SIT_CHG_SIM_PIN2);
    if(m_bParsingSuccess) {
        switch(GetId())
        {
            case SIT_VERIFY_SIM_PIN:
                m_nPinIndex = 1;
                m_nRemainCount = m_jsonData[ns_sim_verify_sim_pin_rsp::key_remain_cnt].asInt();
                break;
            case SIT_CHG_SIM_PIN:
                m_nPinIndex = 1;
                m_nRemainCount = m_jsonData[ns_sim_change_sim_pin_rsp::key_remain_cnt].asInt();
                break;
            case SIT_VERIFY_SIM_PIN2:
                m_nPinIndex = 2;
                m_nRemainCount = m_jsonData[ns_sim_verify_sim_pin2_rsp::key_remain_cnt].asInt();
                break;
            case SIT_CHG_SIM_PIN2:
                m_nPinIndex = 2;
                m_nRemainCount = m_jsonData[ns_sim_change_sim_pin2_rsp::key_remain_cnt].asInt();
                break;
        }
    }
}

/**
 * ProtocolSimVerifyPukAdapterJson
 */
ProtocolSimVerifyPukAdapterJson::ProtocolSimVerifyPukAdapterJson(const ModemData *pModemData)
    : ProtocolSimVerifyPukAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimVerifyPukAdapterJson::Init() {
    m_nPukIndex = 0;
    m_nRemainCount = -1;

    ParseJsonPayload(SIT_VERIFY_SIM_PUK) || ParseJsonPayload(SIT_VERIFY_SIM_PUK2);
    if(m_bParsingSuccess) {
        switch(GetId())
        {
            case SIT_VERIFY_SIM_PUK:
                m_nPukIndex = 1;
                m_nRemainCount = m_jsonData[ns_sim_verify_sim_puk_rsp::key_remain_cnt].asInt();;
                break;

            case SIT_VERIFY_SIM_PUK2:
                m_nPukIndex = 2;
                m_nRemainCount = m_jsonData[ns_sim_verify_sim_puk2_rsp::key_remain_cnt].asInt();;
                break;
        }
    }
}

/**
 * ProtocolSimVerifyNetLockAdapterJson
 */
ProtocolSimVerifyNetLockAdapterJson::ProtocolSimVerifyNetLockAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_VERIFY_NETWORK_LOCK);
}

int ProtocolSimVerifyNetLockAdapterJson::GetRemainCount() const {
    int nRemainCount = -1;
    if(m_bParsingSuccess) {
        nRemainCount = m_jsonData[ns_sim_verify_network_lock_rsp::key_remain_cnt].asInt();
    }

    return nRemainCount;
}

/**
 * ProtocolSimIOAdapterJson
 */
ProtocolSimIOAdapterJson::ProtocolSimIOAdapterJson(const ModemData *pModemData)
    : ProtocolSimIOAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimIOAdapterJson::Init() {
    m_pResponse = NULL;
    ParseJsonPayload(SIT_SIM_IO);
    SetResponseData();
}

void ProtocolSimIOAdapterJson::SetResponseData() {
    if(m_bParsingSuccess) {
        int responseLen = GetResponseLength();
        m_pResponse = new BYTE[responseLen];
        memset(m_pResponse, 0, responseLen);
        for (int i = 0; i < responseLen; i++) {
            m_pResponse[i] = (BYTE) m_jsonData[ns_sim_sim_io_rsp::key_rsp][i].asInt();
        }
    }
}

BYTE ProtocolSimIOAdapterJson::GetSw1() const {
    BYTE sw1 = 0;
    if(m_bParsingSuccess) {
        sw1 = (BYTE) m_jsonData[ns_sim_sim_io_rsp::key_sw1].asInt();
    }
    return sw1;
}

BYTE ProtocolSimIOAdapterJson::GetSw2() const {
    BYTE sw2 = 0;
    if(m_bParsingSuccess) {
        sw2 = (BYTE) m_jsonData[ns_sim_sim_io_rsp::key_sw2].asInt();
    }
    return sw2;
}

int ProtocolSimIOAdapterJson::GetResponseLength() const {
    unsigned int nResponseLength = 0;
    if(m_bParsingSuccess) {
        nResponseLength = m_jsonData[ns_sim_sim_io_rsp::key_rsp].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nResponseLength, MAX_SIM_IO_DATA_LEN);
    }
    return nResponseLength;
}

/**
 * ProtocolSimGetFacilityLockAdapterJson
 */
ProtocolSimGetFacilityLockAdapterJson::ProtocolSimGetFacilityLockAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_GET_FACILITY_LOCK);
}

int ProtocolSimGetFacilityLockAdapterJson::GetServiceClass() const {
    int nSvcClass = 0;
    if(m_bParsingSuccess) {
        nSvcClass = m_jsonData[ns_sim_get_facility_lock_rsp::key_fac_lock_status].asInt();
    }
    return nSvcClass;
}

/**
 * ProtocolSimSetFacilityLockAdapterJson
 */
ProtocolSimSetFacilityLockAdapterJson::ProtocolSimSetFacilityLockAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_SET_FACILITY_LOCK);
}

int ProtocolSimSetFacilityLockAdapterJson::GetRemainCount() const {
    int nRemainCount = -1;
    if(m_bParsingSuccess) {
        switch(GetErrorCode())
        {
            case RCM_E_SUCCESS:
            case RCM_E_GENERIC_FAILURE:
            case RCM_E_PASSWORD_INCORRECT:
                nRemainCount = m_jsonData[ns_sim_set_facility_lock_rsp::key_remain_cnt].asInt();
                break;
            case RCM_E_SIM_PUK2:
                nRemainCount = 0;
                break;
            case RCM_E_SIM_PIN2:
                nRemainCount = -1;    // Unknown
                break;
        }
    }
    return nRemainCount;
}

/* ProtocolSimGetSimAuthAdapterJson */
ProtocolSimGetSimAuthAdapterJson::ProtocolSimGetSimAuthAdapterJson(const ModemData *pModemData)
    : ProtocolSimGetSimAuthAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimGetSimAuthAdapterJson::Init() {
    m_pAuthData = NULL;

    ParseJsonPayload(SIT_GET_SIM_AUTH);
    SetAuthData();
}

void ProtocolSimGetSimAuthAdapterJson::SetAuthData() {
    if(m_pAuthData) {
        delete[] m_pAuthData;
        m_pAuthData = NULL;
    }
    if(m_bParsingSuccess) {
        int nAuthType = GetAuthType();
        int nIndex = 0;
        // see sit_sim_get_sim_auth_rsp
        int nAuthLen = GetAuthLength();
        m_pAuthData = new BYTE[nAuthLen + 2]; // add 2 for sw1 and sw2
        memset(m_pAuthData, 0, nAuthLen + 2);
        m_pAuthData[nIndex++] = (BYTE) m_jsonData[ns_sim_get_sim_auth_rsp::key_auth_result].asInt(); // auth_result
        int nResAutsLen = m_jsonData[ns_sim_get_sim_auth_rsp::key_res_auts].size();
        m_pAuthData[nIndex++] = (BYTE) nResAutsLen;
        for(int i = 0; i < nResAutsLen; i++) {
            m_pAuthData[nIndex++] = (BYTE) m_jsonData[ns_sim_get_sim_auth_rsp::key_res_auts][i].asInt(); // res_auts
        }
        if(nAuthType == SIT_SIM_AUTH_IMS || nAuthType == SIT_SIM_AUTH_3G) {
            // ck and ik data is sent only for SIT_SIM_AUTH_IMS and SIT_SIM_AUTH_3G
            int nCkLen = m_jsonData[ns_sim_get_sim_auth_rsp::key_ck].size();
            m_pAuthData[nIndex++] = (BYTE) nCkLen;
            for(int i = 0; i < nCkLen; i++) {
                m_pAuthData[nIndex++] = (BYTE) m_jsonData[ns_sim_get_sim_auth_rsp::key_ck][i].asInt(); // ck
            }
            int nIkLen = m_jsonData[ns_sim_get_sim_auth_rsp::key_ik].size();
            m_pAuthData[nIndex++] = (BYTE) nIkLen;
            for(int i = 0; i < nIkLen; i++) {
                m_pAuthData[nIndex++] = (BYTE) m_jsonData[ns_sim_get_sim_auth_rsp::key_ik][i].asInt(); // ik
            }
        }
        int nKcLen = m_jsonData[ns_sim_get_sim_auth_rsp::key_kc].size();
        m_pAuthData[nIndex++] = (BYTE) nKcLen;
        for(int i = 0; i < nKcLen; i++) {
            m_pAuthData[nIndex++] = (BYTE) m_jsonData[ns_sim_get_sim_auth_rsp::key_kc][i].asInt(); // kc
        }
        m_pAuthData[nIndex++] = (BYTE) m_jsonData[ns_sim_get_sim_auth_rsp::key_sw1].asInt(); // sw1
        m_pAuthData[nIndex] = (BYTE) m_jsonData[ns_sim_get_sim_auth_rsp::key_sw2].asInt();  // sw2
    }
}

int ProtocolSimGetSimAuthAdapterJson::GetAuthType() const {
    int nAuthType = 0;
    if(m_bParsingSuccess) {
        nAuthType = m_jsonData[ns_sim_get_sim_auth_rsp::key_auth_type].asInt();
    }
    return nAuthType;
}

int ProtocolSimGetSimAuthAdapterJson::GetAuthLength() const {
    int nAuthLen = 0;
    if(m_bParsingSuccess) {
        // see ns_sim_get_sim_auth_rsp
        nAuthLen += 1; // auth_result
        int nResAutsLen = m_jsonData[ns_sim_get_sim_auth_rsp::key_res_auts].size(); // res_auts
        nAuthLen += 1;  // store res_auts_len
        nAuthLen += nResAutsLen;
        if(GetAuthType() == SIT_SIM_AUTH_IMS || GetAuthType() == SIT_SIM_AUTH_3G) {
            // ck and ik data is sent only for SIT_SIM_AUTH_IMS and SIT_SIM_AUTH_3G
            int nCkLen = m_jsonData[ns_sim_get_sim_auth_rsp::key_ck].size();  // ck
            nAuthLen += 1; // store ck_len
            nAuthLen += nCkLen;
            int nIkLen = m_jsonData[ns_sim_get_sim_auth_rsp::key_ik].size(); // ik
            nAuthLen += 1; // store ik_len
            nAuthLen += nIkLen;
        }
        int nKcLen = m_jsonData[ns_sim_get_sim_auth_rsp::key_kc].size(); // kc
        nAuthLen += 1; // store kc_len
        nAuthLen += nKcLen;
    }
    return nAuthLen;
}

/**
 * ProtocolSimTransmitApduBasicAdapterJson
 */
ProtocolSimTransmitApduBasicAdapterJson::ProtocolSimTransmitApduBasicAdapterJson(const ModemData *pModemData)
    : ProtocolSimTransmitApduBasicAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimTransmitApduBasicAdapterJson::Init() {
    m_pData = NULL;
    ParseJsonPayload(SIT_TRANSMIT_SIM_APDU_BASIC);
    SetApduData();
}

void ProtocolSimTransmitApduBasicAdapterJson::SetApduData() {
    if (m_pData != NULL) {
        delete[] m_pData;
        m_pData = NULL;
    }
    if(m_bParsingSuccess) {
        int nLength = GetApduLength();
        m_pData = new BYTE[nLength];
        memset(m_pData, 0, nLength);
        for(int i = 0; i < nLength; i++) {
            m_pData[i] = (BYTE) m_jsonData[ns_sim_transmit_sim_apdu_basic_rsp::key_rsp][i].asInt();
        }
    }
}

BYTE ProtocolSimTransmitApduBasicAdapterJson::GetSw1() const {
    BYTE sw1 = 0;
    if(m_bParsingSuccess && GetApduLength() >= 2) {
        sw1 = (BYTE) m_jsonData[ns_sim_transmit_sim_apdu_basic_rsp::key_rsp][GetApduLength()-2].asInt();
    }
    return sw1;
}

BYTE ProtocolSimTransmitApduBasicAdapterJson::GetSw2() const {
    BYTE sw2 = 0;
    if(m_bParsingSuccess && GetApduLength() >= 2) {
        sw2 = (BYTE) m_jsonData[ns_sim_transmit_sim_apdu_basic_rsp::key_rsp][GetApduLength()-1].asInt();
    }
    return sw2;
}

int ProtocolSimTransmitApduBasicAdapterJson::GetApduLength() const {
    int nLength = 0;
    if(m_bParsingSuccess) {
        nLength =  m_jsonData[ns_sim_transmit_sim_apdu_basic_rsp::key_rsp].size();
        if(nLength > MAX_APDU_LEN) nLength = MAX_APDU_LEN;
    }
    return nLength;
}

/**
 * ProtocolSimOpenChannelAdapterJson
 */
ProtocolSimOpenChannelAdapterJson::ProtocolSimOpenChannelAdapterJson(const ModemData *pModemData)
    : ProtocolSimOpenChannelAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimOpenChannelAdapterJson::Init() {
    m_nSitId = GetId();
    ParseJsonPayload(SIT_OPEN_SIM_CHANNEL) || ParseJsonPayload(SIT_OPEN_SIM_CHANNEL_WITH_P2);
    SetResponseData();
}

void ProtocolSimOpenChannelAdapterJson::SetResponseData() {
    m_pResponse = NULL;
    if(m_bParsingSuccess) {
        int responseLen = GetResponseLength();
        m_pResponse = new BYTE[responseLen];
        memset(m_pResponse, 0, responseLen);
        if(m_nSitId == SIT_OPEN_SIM_CHANNEL) {
            for (int i = 0; i < responseLen; i++) {
                m_pResponse[i] = (BYTE) m_jsonData[ns_sim_open_sim_channel_rsp::key_rsp][i].asInt();
            }
        } else {
            for (int i = 0; i < responseLen; i++) {
                m_pResponse[i] = (BYTE) m_jsonData[ns_sim_open_sim_channel_with_p2_rsp::key_rsp][i].asInt();
            }
        }
    }
}

int ProtocolSimOpenChannelAdapterJson::GetSessionID() const {
    int nSessionID = 0;
    if(m_bParsingSuccess) {
        if(m_nSitId == SIT_OPEN_SIM_CHANNEL) {
            nSessionID = (int) m_jsonData[ns_sim_open_sim_channel_rsp::key_sid].asUInt();
        } else {
            nSessionID = (int) m_jsonData[ns_sim_open_sim_channel_with_p2_rsp::key_sid].asUInt();
        }
    }
    return nSessionID;
}

BYTE ProtocolSimOpenChannelAdapterJson::GetSw1() const {
    BYTE sw1 = 0;
    if(m_bParsingSuccess) {
        if(m_nSitId == SIT_OPEN_SIM_CHANNEL) {
            sw1 = (BYTE) m_jsonData[ns_sim_open_sim_channel_rsp::key_sw1].asInt();
        } else {
            sw1 = (BYTE) m_jsonData[ns_sim_open_sim_channel_with_p2_rsp::key_sw1].asInt();
        }
    }
    return sw1;
}

BYTE ProtocolSimOpenChannelAdapterJson::GetSw2() const {
    BYTE sw2 = 0;
    if(m_bParsingSuccess) {
        if(m_nSitId == SIT_OPEN_SIM_CHANNEL) {
            sw2 = (BYTE) m_jsonData[ns_sim_open_sim_channel_rsp::key_sw2].asInt();
        } else {
            sw2 = (BYTE) m_jsonData[ns_sim_open_sim_channel_with_p2_rsp::key_sw2].asInt();
        }
    }
    return sw2;
}

int ProtocolSimOpenChannelAdapterJson::GetResponseLength() const {
    unsigned int nResponseLength = 0;
    if(m_bParsingSuccess) {
        if(m_nSitId == SIT_OPEN_SIM_CHANNEL) {
            nResponseLength = m_jsonData[ns_sim_open_sim_channel_rsp::key_rsp].size();
        } else {
            nResponseLength = m_jsonData[ns_sim_open_sim_channel_with_p2_rsp::key_rsp].size();
        }
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, nResponseLength, MAX_OPEN_CHANNEL_RSP_LEN);
    }
    return nResponseLength;
}

/**
 * ProtocolSimTransmitApduChannelAdapterJson
 */
ProtocolSimTransmitApduChannelAdapterJson::ProtocolSimTransmitApduChannelAdapterJson(const ModemData *pModemData)
    : ProtocolSimTransmitApduChannelAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimTransmitApduChannelAdapterJson::Init() {
    m_pData = NULL;
    ParseJsonPayload(SIT_TRANSMIT_SIM_APDU_CHANNEL);
    SetApduData();
}

void ProtocolSimTransmitApduChannelAdapterJson::SetApduData() {
    if (m_pData != NULL) {
        delete[] m_pData;
        m_pData = NULL;
    }
    if(m_bParsingSuccess) {
        int nLength = GetApduLength();
        m_pData = new BYTE[nLength];
        memset(m_pData, 0, nLength);
        for(int i = 0; i < nLength; i++) {
            m_pData[i] = (BYTE) m_jsonData[ns_sim_transmit_sim_apdu_channel_rsp::key_rsp][i].asInt();
        }
    }
}

BYTE ProtocolSimTransmitApduChannelAdapterJson::GetSw1() const {
    BYTE sw1 = 0;
    if(m_bParsingSuccess) {
        sw1 = (BYTE) m_jsonData[ns_sim_transmit_sim_apdu_channel_rsp::key_sw1].asInt();
    }
    return sw1;
}

BYTE ProtocolSimTransmitApduChannelAdapterJson::GetSw2() const {
    BYTE sw2 = 0;
    if(m_bParsingSuccess) {
        sw2 = (BYTE) m_jsonData[ns_sim_transmit_sim_apdu_channel_rsp::key_sw2].asInt();
    }
    return sw2;
}

int ProtocolSimTransmitApduChannelAdapterJson::GetApduLength() const {
    int nLength = 0;
    if(m_bParsingSuccess) {
        nLength =  m_jsonData[ns_sim_transmit_sim_apdu_channel_rsp::key_rsp].size();
        if(nLength > MAX_APDU_LEN) nLength = MAX_APDU_LEN;
    }
    return nLength;
}

/* ProtocolSimImsiAdapterJson */
ProtocolSimImsiAdapterJson::ProtocolSimImsiAdapterJson(const ModemData *pModemData)
    : ProtocolSimImsiAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimImsiAdapterJson::Init() {
    m_pImsi = NULL;
    ParseJsonPayload(SIT_GET_IMSI);
    SetImsi();
}

void ProtocolSimImsiAdapterJson::SetImsi() {
    if (!m_bParsingSuccess) return;
    std::string imsi = m_jsonData[ns_sim_id_get_imsi_rsp::key_imsi].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_SIM, __CLASS_FUNC__, imsi, MAX_IMSI_LEN);
    m_pImsi = new char[imsi.length() + 1];
    memset(m_pImsi, 0, imsi.length() + 1);
    memcpy(m_pImsi, imsi.c_str(), imsi.length());
}

/* ProtocolSimGetGbaAuthAdapterJson */
ProtocolSimGetGbaAuthAdapterJson::ProtocolSimGetGbaAuthAdapterJson(const ModemData *pModemData)
    : ProtocolSimGetGbaAuthAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimGetGbaAuthAdapterJson::Init() {
    m_pData = NULL;
    ParseJsonPayload(SIT_GET_GBA_CONTEXT);
    SetGbaAuthData();
}

void ProtocolSimGetGbaAuthAdapterJson::SetGbaAuthData() {
    if (m_pData != NULL) {
        delete[] m_pData;
        m_pData = NULL;
    }
    int nLength = GetGbaAuthLength();
    if(m_bParsingSuccess && nLength > 0) {
        m_pData = new BYTE[nLength];
        memset(m_pData, 0, nLength);
        m_pData[0] = (BYTE) m_jsonData[ns_sim_get_sim_gba_auth_rsp::key_result].asInt();
        m_pData[1] = (BYTE) m_jsonData[ns_sim_get_sim_gba_auth_rsp::key_auth_type].asInt();
        m_pData[2] = (BYTE) m_jsonData[ns_sim_get_sim_gba_auth_rsp::key_gba_type].asInt();
        m_pData[3] = (BYTE) m_jsonData[ns_sim_get_sim_gba_auth_rsp::key_gba_tag].asInt();
        for(int i = 4; i < nLength; i++) {
            m_pData[i] = (BYTE) m_jsonData[ns_sim_get_sim_gba_auth_rsp::key_data1][i-4].asInt();
        }
    }
}

int ProtocolSimGetGbaAuthAdapterJson::GetGbaAuthLength() const {
    int nLength = 0;
    if(m_bParsingSuccess) {
        // lengthOf(data1) + (result + auth_type + gba_type + gba_tag)
        nLength =  m_jsonData[ns_sim_get_sim_gba_auth_rsp::key_data1].size() + 4;
        if(nLength > MAX_SIM_GBA_AUTH_RSP_LEN) nLength = MAX_SIM_GBA_AUTH_RSP_LEN;
    }
    return nLength;
}

/**
 *  ProtocolSimATRAdapterJson
 */
ProtocolSimATRAdapterJson::ProtocolSimATRAdapterJson(const ModemData *pModemData)
    : ProtocolSimATRAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimATRAdapterJson::Init() {
    memset(m_atr, 0, sizeof(m_atr));
    m_result = (BYTE)-1;
    m_nAtrLen = 0;
    if (ProtocolJsonRespAdapter::GetErrorCode() == RIL_E_SUCCESS) {
        ParseJsonPayload(SIT_GET_ATR);
        SetAtrData();
    }
}

void ProtocolSimATRAdapterJson::SetAtrData() {
    if(m_bParsingSuccess) {
        m_result = m_jsonData[ns_sim_id_get_atr_rsp::key_result].asInt();
        m_nAtrLen = m_jsonData[ns_sim_id_get_atr_rsp::key_atr].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, m_nAtrLen, MAX_ATR_LEN);
        for (unsigned int i = 0; i < m_nAtrLen; i++) {
            m_atr[i] = (char) m_jsonData[ns_sim_id_get_atr_rsp::key_atr][i].asInt();
        }
    }
}

BYTE ProtocolSimATRAdapterJson::GetResult() const {
    return m_result;
}

BYTE ProtocolSimATRAdapterJson::GetATRLength() const {
    return (BYTE) m_nAtrLen;
}

/* ProtocolSimReadPbEntryJson */
ProtocolSimReadPbEntryJson::ProtocolSimReadPbEntryJson(const ModemData *pModemData)
    : ProtocolSimReadPbEntryInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimReadPbEntryJson::Init() {
    m_nPbType = 0;
    m_nRecordId = 0;
    m_nDataLen = 0;
    memset(m_entryData, 0, MAX_PB_ENTRY_LEN);
    if (ProtocolJsonRespAdapter::GetErrorCode() == RIL_E_SUCCESS) {
        ParseJsonPayload(SIT_READ_PB_ENTRY);
        SetPbType();
        SetRecordId();
        SetDataLen();
    }
}

void ProtocolSimReadPbEntryJson::SetPbType() {
    if(m_bParsingSuccess) {
        m_nPbType = m_jsonData[ns_read_pb_resp::key_pb_type].asInt();
    }
}

void ProtocolSimReadPbEntryJson::SetRecordId() {
    if(m_bParsingSuccess) {
        m_nRecordId = m_jsonData[ns_read_pb_resp::key_record_id].asInt();
    }
}

void ProtocolSimReadPbEntryJson::SetDataLen() {
    if(m_bParsingSuccess) {
        m_nDataLen = m_jsonData[ns_read_pb_resp::key_entry_data].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SIM, __CLASS_FUNC__, m_nDataLen, MAX_PB_ENTRY_LEN);
    }
}

char *ProtocolSimReadPbEntryJson::GetEntryData() {
    if(m_bParsingSuccess && m_nDataLen > 0) {
        for(unsigned int i = 0; i < m_nDataLen; i++) {
            m_entryData[i] = (char)  m_jsonData[ns_read_pb_resp::key_entry_data][i].asInt();
        }
    }
    return m_entryData;
}

/* ProtocolSimUpdatePbEntryJson */
ProtocolSimUpdatePbEntryJson::ProtocolSimUpdatePbEntryJson(const ModemData *pModemData)
    : ProtocolSimUpdatePbEntryInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimUpdatePbEntryJson::Init() {
    m_mode = 0;
    m_pbtype = 0;
    m_recordId = 0;
    if(ProtocolJsonRespAdapter::GetErrorCode() == RIL_E_SUCCESS) {
        ParseJsonPayload(SIT_UPDATE_PB_ENTRY);
    }
}

BYTE ProtocolSimUpdatePbEntryJson::GetMode() {
    if(m_bParsingSuccess) {
        m_mode = (BYTE) m_jsonData[ns_update_pb_entry_resp::key_mode].asInt();
    }
    return m_mode;
}

BYTE ProtocolSimUpdatePbEntryJson::GetPbtype() {
    if(m_bParsingSuccess) {
        m_pbtype = (BYTE) m_jsonData[ns_update_pb_entry_resp::key_pb_type].asInt();
    }
    return m_pbtype;
}

UINT16 ProtocolSimUpdatePbEntryJson::GetRecordId() {
    if(m_bParsingSuccess) {
        m_recordId = m_jsonData[ns_update_pb_entry_resp::key_record_id].asInt();
    }
    return m_recordId;
}

/* ProtocolSimPbStorageInfoJson */
ProtocolSimPbStorageInfoAdapterJson::ProtocolSimPbStorageInfoAdapterJson(const ModemData *pModemData)
    : ProtocolSimPbStorageInfoAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimPbStorageInfoAdapterJson::Init()
{
    m_nPbType = 0;
    m_nTotalCount = 0;
    m_nUsedCount = 0;
    if (ProtocolJsonRespAdapter::GetErrorCode() == RIL_E_SUCCESS) {
        ParseJsonPayload(SIT_GET_PB_STORAGE_INFO);
        SetPbType();
        SetTotalCount();
        SetUsedCount();
    }
}

void ProtocolSimPbStorageInfoAdapterJson::SetPbType() {
    if(m_bParsingSuccess) {
        m_nPbType = m_jsonData[ns_sim_pb_storage_info_rsp::key_pb_type].asInt();
    }
}

void ProtocolSimPbStorageInfoAdapterJson::SetTotalCount() {
    if(m_bParsingSuccess) {
        m_nTotalCount = m_jsonData[ns_sim_pb_storage_info_rsp::key_total_cnt].asInt();
    }
}

void ProtocolSimPbStorageInfoAdapterJson::SetUsedCount() {
    if(m_bParsingSuccess) {
        m_nUsedCount = m_jsonData[ns_sim_pb_storage_info_rsp::key_used_cnt].asInt();
    }
}

/* ProtocolSim3GPBCapaAdapterJson */
ProtocolSimPbCapaAdapterJson::ProtocolSimPbCapaAdapterJson(const ModemData *pModemData)
    : ProtocolSimPbCapaAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSimPbCapaAdapterJson::Init() {
    ParseJsonPayload(SIT_GET_3G_PB_CAPA);
    if(m_bParsingSuccess) {
        m_jsonPbList = m_jsonData[ns_sim_pb_capa_rsp::key_pb_cap_list];
    }
}

bool ProtocolSimPbCapaAdapterJson::GetPbCapa(int *pb, int entryNum) {
    if(m_bParsingSuccess) {
        int j = 0;
        for(int i = 0; i < entryNum; i++) {
            pb[j] = m_jsonPbList[i][ns_pb_cap_list::key_pb_type].asInt();
            pb[j+1] = m_jsonPbList[i][ns_pb_cap_list::key_max_index].asInt();
            pb[j+2] = m_jsonPbList[i][ns_pb_cap_list::key_max_entry].asInt();
            pb[j+3] = m_jsonPbList[i][ns_pb_cap_list::key_used_cnt].asInt();
            j += 4;
        }
    }
    return true;
}

int ProtocolSimPbCapaAdapterJson::GetEntryNum() const {
    int nEntryNum = 0;
    if(m_bParsingSuccess) {
        nEntryNum = m_jsonData[ns_sim_pb_capa_rsp::key_pb_cap_list].size();
    }
    return nEntryNum;
}

/* ProtocolSimPbReadyAdapterJson */
ProtocolSimPbReadyAdapterJson::ProtocolSimPbReadyAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_SIM_PB_READY);
}

int ProtocolSimPbReadyAdapterJson::GetPbReady() const {
    int nPbReady = 0;
    if(m_bParsingSuccess) {
        nPbReady = m_jsonData[ns_sim_pb_ready_ind::key_pb_status].asInt();
    }
    return nPbReady;
}

/* ProtocolSimGetCarrierRestrictionsAdapterJson */
ProtocolSimGetCarrierRestrictionsAdapterJson::ProtocolSimGetCarrierRestrictionsAdapterJson(const ModemData *pModemData)
    : ProtocolSimGetCarrierRestrictionsAdapterInterface(), ProtocolJsonRespAdapter(pModemData, SIT_GET_CARRIER_RESTRICTIONS) {
    Init();
}

ProtocolSimGetCarrierRestrictionsAdapterJson::~ProtocolSimGetCarrierRestrictionsAdapterJson() {
    if (m_pAllowedCarriers)  {
        for (int i = 0; i < m_nAllowedCarrierLength; ++i) {
            if (m_pAllowedCarriers[i].mcc) delete [] m_pAllowedCarriers[i].mcc;
            if (m_pAllowedCarriers[i].mnc) delete [] m_pAllowedCarriers[i].mnc;
            if (m_pAllowedCarriers[i].match_data) delete [] m_pAllowedCarriers[i].match_data;
        }
        free(m_pAllowedCarriers);
        m_pAllowedCarriers = NULL;
    }

    if (m_pExcludedCarriers) {
        for (int i = 0; i < m_nExcludedCarrierLength; ++i) {
            if (m_pExcludedCarriers[i].mcc) delete [] m_pExcludedCarriers[i].mcc;
            if (m_pExcludedCarriers[i].mnc) delete [] m_pExcludedCarriers[i].mnc;
            if (m_pExcludedCarriers[i].match_data) delete [] m_pExcludedCarriers[i].match_data;
        }
        free(m_pExcludedCarriers);
        m_pExcludedCarriers = NULL;
    }
}

void ProtocolSimGetCarrierRestrictionsAdapterJson::FillMccMncFromString(RIL_Carrier &carrierInfo, std::string plmn) {
    if (plmn.length() > 0) {
        char *mcc = new char[MAX_MCC_LEN+1];
        char *mnc = new char[MAX_MNC_LEN+1];
        int mccLen = MIN(MAX_MCC_LEN, plmn.length());
        int mncLen = MIN(MAX_MNC_LEN, plmn.length() - mccLen);
        memcpy(mcc, plmn.c_str(), mccLen); mcc[mccLen] = 0;
        memcpy(mnc, plmn.substr(mccLen).c_str(), mncLen); mnc[mncLen] = 0;
        carrierInfo.mcc = mcc;
        carrierInfo.mnc = mnc;
    }
}

void ProtocolSimGetCarrierRestrictionsAdapterJson::FillMatchDataFromJson(RIL_Carrier &carrierInfo, Json::Value &jsonMatchData) {
    int sizeOfmatchData = jsonMatchData.size();
    if (sizeOfmatchData > 0 ) {
        char *dstData = new char[sizeOfmatchData + 1];
        for ( int i = 0; i < sizeOfmatchData; ++i) {
            dstData[i] = (BYTE)jsonMatchData[i].asInt();
        }
        dstData[sizeOfmatchData] = 0;
        carrierInfo.match_data = dstData;
    }
}

void ProtocolSimGetCarrierRestrictionsAdapterJson::Init() {
    m_nAllowedCarrierLength = 0;
    m_nExcludedCarrierLength = 0;
    m_pAllowedCarriers = NULL;
    m_pExcludedCarriers = NULL;
    m_bAllowedCarriersPrioritized = false;
    m_nMultiSimPolicy = 0;

    if (m_bParsingSuccess && ProtocolRespAdapter::GetErrorCode() == RIL_E_SUCCESS) {
        m_nAllowedCarrierLength = m_jsonData[ns_sim_net_getset_carrier_restrictions::key_allowed_list].size();
        m_nExcludedCarrierLength = m_jsonData[ns_sim_net_getset_carrier_restrictions::key_excluded_list].size();
        m_bAllowedCarriersPrioritized = (m_jsonData[ns_sim_net_getset_carrier_restrictions::key_allowed_priority].asInt() == 1);
        m_nMultiSimPolicy = m_jsonData[ns_sim_net_getset_carrier_restrictions::key_multi_simlock].asInt();

        if (m_nAllowedCarrierLength > 0 ) {
            m_pAllowedCarriers = (RIL_Carrier *)calloc(m_nAllowedCarrierLength, sizeof(RIL_Carrier));
            memset(m_pAllowedCarriers, 0, m_nAllowedCarrierLength * sizeof(RIL_Carrier));
            if (m_pAllowedCarriers != NULL) {
                for (int i = 0; i < m_nAllowedCarrierLength; i++) {
                    Json::Value jsonCarrierData = m_jsonData[ns_sim_net_getset_carrier_restrictions::key_allowed_list][i];
                    std::string strPlmn = jsonCarrierData[ns_sim_net_carrier_info::key_plmn].asString();
                    FillMccMncFromString(m_pAllowedCarriers[i], strPlmn);
                    m_pAllowedCarriers[i].match_type = (RIL_CarrierMatchType) jsonCarrierData[ns_sim_net_carrier_info::key_match_type].asInt();
                    FillMatchDataFromJson(m_pAllowedCarriers[i], jsonCarrierData[ns_sim_net_carrier_info::key_match_data_list]);
                }
            }
        }

        if (m_nExcludedCarrierLength > 0 ) {
            m_pExcludedCarriers = (RIL_Carrier *)calloc(m_nExcludedCarrierLength, sizeof(RIL_Carrier));
            memset(m_pExcludedCarriers, 0, m_nExcludedCarrierLength * sizeof(RIL_Carrier));
            if (m_pExcludedCarriers != NULL) {
                for (int i = 0; i < m_nExcludedCarrierLength; i++) {
                    Json::Value jsonCarrierData = m_jsonData[ns_sim_net_getset_carrier_restrictions::key_excluded_list][i];
                    std::string strPlmn = jsonCarrierData[ns_sim_net_carrier_info::key_plmn].asString();
                    FillMccMncFromString(m_pExcludedCarriers[i], strPlmn);
                    m_pExcludedCarriers[i].match_type = (RIL_CarrierMatchType) jsonCarrierData[ns_sim_net_carrier_info::key_match_type].asInt();
                    FillMatchDataFromJson(m_pExcludedCarriers[i], jsonCarrierData[ns_sim_net_carrier_info::key_match_data_list]);
                }
            }
        }
    }
}

int ProtocolSimGetCarrierRestrictionsAdapterJson::GetAllowedCarriers(RIL_Carrier *pCarriers, int nSize) const
{
    if (pCarriers == NULL || nSize == 0 || m_pAllowedCarriers == NULL
            || m_nAllowedCarrierLength == 0 || nSize < m_nAllowedCarrierLength) return 0;

    memset(pCarriers, 0, sizeof(RIL_Carrier) * nSize);
    for(int i = 0; i < m_nAllowedCarrierLength; i++) {
        pCarriers[i] = m_pAllowedCarriers[i];
    }

    return m_nAllowedCarrierLength;
}

int ProtocolSimGetCarrierRestrictionsAdapterJson::GetExcludedCarriers(RIL_Carrier *pCarriers, int nSize) const
{
    if (pCarriers == NULL || nSize == 0 || m_pExcludedCarriers == NULL
            || m_nExcludedCarrierLength == 0 || nSize < m_nExcludedCarrierLength) return 0;

    memset(pCarriers, 0, sizeof(RIL_Carrier) * nSize);
    for(int i = 0; i < m_nExcludedCarrierLength; i++) {
        pCarriers[i] = m_pExcludedCarriers[i];
    }

    return m_nExcludedCarrierLength;
}

/* ProtocolUiccSubStatusChangeAdapterJson */
ProtocolUiccSubStatusChangeAdapterJson::ProtocolUiccSubStatusChangeAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_UICC_SUBSCRIPTION_STATE_CHANGED);
}

int ProtocolUiccSubStatusChangeAdapterJson::GetState() const {
    int nState = 0;
    if(m_bParsingSuccess) {
        nState = m_jsonData[ns_sim_uicc_sub_state_changed_ind::key_state].asInt();
    }
    return nState;
}

/* ProtocolSimTrayStatusChangeAdapterJson */
ProtocolSimTrayStatusChangeAdapterJson::ProtocolSimTrayStatusChangeAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_SIM_TRAY_STATUS_CHANGED);
}

int ProtocolSimTrayStatusChangeAdapterJson::GetState() const {
    int nState = 0;
    if(m_bParsingSuccess) {
        nState = m_jsonData[ns_sim_tray_status_changed_ind::key_tray_status].asInt();
    }
    return nState;
}
