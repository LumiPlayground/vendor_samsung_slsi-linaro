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
 * protocolsimadapterlegacy.cpp
 *
 *  Created on: 2014. 6. 28.
 *      Author: mox
 */

#include "uicc/protocolsimadapterlegacy.h"
#include "rillog.h"
#include "modemdata.h"
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
 * ProtocolSimStatusAdapterLegacy
 */
void ProtocolSimStatusAdapterLegacy::Init()
{
    memset(&m_tSimStatusRsp, 0, sizeof(m_tSimStatusRsp));
    memset(&m_tSimStatusRspExt, 0, sizeof(m_tSimStatusRspExt));

    if (m_pModemData != NULL) {
        sit_sim_get_sim_status_rsp *pData = (sit_sim_get_sim_status_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_GET_SIM_STATUS) {
            // for Radio 1.0
            UINT nSimStatusLength = sizeof(sit_sim_get_sim_status_rsp) - (sizeof(sit_sim_apps_status_info)*MAX_SIM_APPS_INFO_COUNT);
            nSimStatusLength += (sizeof(sit_sim_apps_status_info)*pData->application_num);
            if (sizeof(m_tSimStatusRsp) >= nSimStatusLength) {
                memcpy(&m_tSimStatusRsp, pData , nSimStatusLength);
            } else {
                RilLogE("%s sizeof(m_tSimStatusRsp) %d is smaller than nSimStatusLength %d.",
                        __FUNCTION__, (int)sizeof(m_tSimStatusRsp), (int)nSimStatusLength);
            }
            // Convert from SIT CardState to RIL CardState
            if (pData->card_state == SIT_SIM_CARD_STATE_DETECTED) {
                m_tSimStatusRsp.card_state = RIL_CARDSTATE_PRESENT;
                m_tSimStatusRsp.universal_pin_state = RIL_PINSTATE_UNKNOWN;
                m_tSimStatusRsp.application_num = 0;
            } else if (pData->card_state == SIT_SIM_CARD_STATE_RESTRICTED) {
                m_tSimStatusRsp.card_state = RIL_CARDSTATE_RESTRICTED;
            }

            // for Radio 1.2
            if (m_pModemData->GetLength() > nSimStatusLength) {
                sit_sim_get_sim_status_rsp_ext *pData_ext = (sit_sim_get_sim_status_rsp_ext *) (m_pModemData->GetRawData() + nSimStatusLength);
                int sizefor_1_2 = sizeof(sit_sim_get_sim_status_rsp_ext) - (MAX_EID_LEN + 1 + 1);
                memcpy(&m_tSimStatusRspExt, pData_ext, sizefor_1_2);
                nSimStatusLength += sizefor_1_2;

                // for Radio 1.4 (eID)
                if (m_pModemData->GetLength() > nSimStatusLength) {
                    int sizefor_1_4 = MAX_EID_LEN + 1;
                    m_tSimStatusRspExt.eid_length = pData_ext->eid_length;
                    memcpy(&m_tSimStatusRspExt.eid, pData_ext->eid, pData_ext->eid_length);
                    nSimStatusLength += sizefor_1_4;

                    // for Radio 2.0 (portId)
                    if (m_pModemData->GetLength() > nSimStatusLength) {
                        int sizefor_2_0 = 1; // port_id
                        m_tSimStatusRspExt.port_id = pData_ext->port_id;
                        nSimStatusLength += sizefor_2_0;
                    }
                }
            }
        } else {
            RilLogE("%s GetRawData() is NULL", __FUNCTION__);
        }
    } else {
        RilLogE("%s m_pModemData is NULL", __FUNCTION__);
    }
}

char *ProtocolSimStatusAdapterLegacy::GetAID(int nIndex) const
{
    char *pAID = NULL;
    if(nIndex>=0 && nIndex<m_tSimStatusRsp.application_num)
    {
        int size = m_tSimStatusRsp.apps_status_info[nIndex].aid_len * 2 + 1;
        pAID = new char[size];
        int ret = Value2HexString(pAID, m_tSimStatusRsp.apps_status_info[nIndex].AID, m_tSimStatusRsp.apps_status_info[nIndex].aid_len);
        if (ret == -1) {
            delete [] pAID;
            return NULL;
        }
    }

    return pAID;
}

int ProtocolSimStatusAdapterLegacy::GetApplicationLabel(int nIndex, BYTE *pAppLabel) const
{
    int nAppLabelLen = 0;
    if((nIndex>=0 && nIndex<m_tSimStatusRsp.application_num)
            && m_tSimStatusRsp.apps_status_info[nIndex].app_label_len<=MAX_SIM_APP_LABEL_LEN && pAppLabel!=NULL) {
        nAppLabelLen = (int) m_tSimStatusRsp.apps_status_info[nIndex].app_label_len;
        memcpy(pAppLabel, m_tSimStatusRsp.apps_status_info[nIndex].app_label, nAppLabelLen);
    }

    return nAppLabelLen;
}

int ProtocolSimStatusAdapterLegacy::GetPinState(int nIndex, int nPinIndex) const
{
    int nPinState = 0;
    if(nIndex>=0 && nIndex<m_tSimStatusRsp.application_num)
    {
        switch(nPinIndex)
        {
        case 1: nPinState = (int) m_tSimStatusRsp.apps_status_info[nIndex].pin1_state; break;
        case 2: nPinState = (int) m_tSimStatusRsp.apps_status_info[nIndex].pin2_state; break;
        }
    }

    return nPinState;
}

int ProtocolSimStatusAdapterLegacy::GetPinRemainCount(int nIndex, int nPinIndex) const
{
    int nPinRemainCount = -1;
    if(nIndex>=0 && nIndex<m_tSimStatusRsp.application_num)
    {
        switch(nPinIndex)
        {
        case 1: nPinRemainCount = (int) m_tSimStatusRsp.apps_status_info[nIndex].pin1_remain_count; break;
        case 2: nPinRemainCount = (int) m_tSimStatusRsp.apps_status_info[nIndex].pin2_remain_count; break;
        }
    }

    return nPinRemainCount;
}

int ProtocolSimStatusAdapterLegacy::GetPukRemainCount(int nIndex, int nPukIndex) const
{
    int nPukRemainCount = -1;
    if(nIndex>=0 && nIndex<m_tSimStatusRsp.application_num)
    {
        switch(nPukIndex)
        {
        case 1: nPukRemainCount = (int) m_tSimStatusRsp.apps_status_info[nIndex].puk1_remain_count; break;
        case 2: nPukRemainCount = (int) m_tSimStatusRsp.apps_status_info[nIndex].puk2_remain_count; break;
        }
    }

    return nPukRemainCount;
}

/**
 * ProtocolSimVerifyPinAdapterLegacy
 */
void ProtocolSimVerifyPinAdapterLegacy::Init()
{
    m_nPinIndex = 0;
    m_nRemainCount = -1;

    if (m_pModemData != NULL) {
        sit_sim_verify_sim_pin_rsp *pData = (sit_sim_verify_sim_pin_rsp *) m_pModemData->GetRawData();
        if (pData != NULL) {
            switch(pData->hdr.id)
            {
            case SIT_VERIFY_SIM_PIN:
            case SIT_CHG_SIM_PIN:
                m_nPinIndex = 1;
                m_nRemainCount = pData->remain_count;
                break;

            case SIT_VERIFY_SIM_PIN2:
            case SIT_CHG_SIM_PIN2:
                m_nPinIndex = 2;
                m_nRemainCount = pData->remain_count;
                break;
            }
        }
    }
}

/**
 * ProtocolSimVerifyPukAdapterLegacy
 */
void ProtocolSimVerifyPukAdapterLegacy::Init()
{
    m_nPukIndex = 0;
    m_nRemainCount = -1;

    if (m_pModemData != NULL) {
        sit_sim_verify_sim_puk_rsp *pData = (sit_sim_verify_sim_puk_rsp *) m_pModemData->GetRawData();
        if (pData != NULL) {
            switch(pData->hdr.id)
            {
            case SIT_VERIFY_SIM_PUK:
                m_nPukIndex = 1;
                m_nRemainCount = pData->remain_count;
                break;
            case SIT_VERIFY_SIM_PUK2:
                m_nPukIndex = 2;
                m_nRemainCount = pData->remain_count;
                break;
            }
        }
    }
}

/**
 * ProtocolSimVerifyNetLockAdapterLegacy
 */
int ProtocolSimVerifyNetLockAdapterLegacy::GetRemainCount() const
{
    int nRemainCount = -1;
    if (m_pModemData != NULL) {
        sit_sim_verify_network_lock_rsp *pData = (sit_sim_verify_network_lock_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_VERIFY_NETWORK_LOCK) {
            nRemainCount = pData->remain_count;
        }
    }

    return nRemainCount;
}

/**
 * ProtocolSimIOAdapterLegacy
 */
BYTE ProtocolSimIOAdapterLegacy::GetSw1() const
{
    BYTE sw1 = 0;
    if (m_pModemData != NULL) {
        sit_sim_sim_io_rsp *pData = (sit_sim_sim_io_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && (pData->hdr.id == SIT_SIM_IO
                    || pData->hdr.id == SIT_TRANSMIT_SIM_APDU_CHANNEL
                    || pData->hdr.id == SIT_TRANSMIT_SIM_APDU_BASIC)) {
            sw1 = pData->sw1;
        }
    }

    return sw1;
}

BYTE ProtocolSimIOAdapterLegacy::GetSw2() const
{
    BYTE sw2 = 0;
    if (m_pModemData != NULL) {
        sit_sim_sim_io_rsp *pData = (sit_sim_sim_io_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && (pData->hdr.id == SIT_SIM_IO
                    || pData->hdr.id == SIT_TRANSMIT_SIM_APDU_CHANNEL
                    || pData->hdr.id == SIT_TRANSMIT_SIM_APDU_BASIC)) {
            sw2 = pData->sw2;
        }
    }

    return sw2;
}

BYTE *ProtocolSimIOAdapterLegacy::GetResponse() const
{
    BYTE *pResponse = NULL;
    if (m_pModemData != NULL) {
        sit_sim_sim_io_rsp *pData = (sit_sim_sim_io_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && (pData->hdr.id == SIT_SIM_IO
                    || pData->hdr.id == SIT_TRANSMIT_SIM_APDU_CHANNEL
                    || pData->hdr.id == SIT_TRANSMIT_SIM_APDU_BASIC)) {
            pResponse = pData->response;
        }
    }

    return pResponse;
}

int ProtocolSimIOAdapterLegacy::GetResponseLength() const
{
    int nResponseLength = 0;
    if (m_pModemData != NULL) {
        sit_sim_sim_io_rsp *pData = (sit_sim_sim_io_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && (pData->hdr.id == SIT_SIM_IO
                    || pData->hdr.id == SIT_TRANSMIT_SIM_APDU_CHANNEL
                    || pData->hdr.id == SIT_TRANSMIT_SIM_APDU_BASIC)) {
            nResponseLength =  pData->response_len;
        }
    }

    return (nResponseLength>MAX_SIM_IO_DATA_LEN)? MAX_SIM_IO_DATA_LEN : nResponseLength;
}

/**
 * ProtocolSimGetFacilityLockAdapterLegacy
 */
int ProtocolSimGetFacilityLockAdapterLegacy::GetServiceClass() const
{
    int nSvcClass = 0;
    if (m_pModemData != NULL) {
        sit_sim_get_facility_lock_rsp *pData = (sit_sim_get_facility_lock_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_GET_FACILITY_LOCK) {
            nSvcClass = (int) pData->service_class;
        }
    }

    return nSvcClass;
}

/**
 * ProtocolSimSetFacilityLockAdapterLegacy
 */
int ProtocolSimSetFacilityLockAdapterLegacy::GetRemainCount() const
{
    int nRemainCount = -1;
    if (m_pModemData != NULL) {
        sit_sim_set_facility_lock_rsp *pData = (sit_sim_set_facility_lock_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_SET_FACILITY_LOCK) {
            switch(pData->hdr.ext.rsp.error)
            {
            case RCM_E_SUCCESS:
            case RCM_E_GENERIC_FAILURE:
            case RCM_E_PASSWORD_INCORRECT:
                nRemainCount = pData->remain_count;
                break;

            //case RCM_E_SIM_PUK:
            //    nRemainCount = 0;
            //    break;
            case RCM_E_SIM_PUK2:
                nRemainCount = 0;
                break;
            //case RCM_E_SIM_PIN:
            case RCM_E_SIM_PIN2:
            //case RCM_E_PERM_BLOCKED:
            //case RCM_E_PERM_BLOCKED2:
                nRemainCount = -1;    // Unknown
                break;
            }
        }
    }

    return nRemainCount;
}

/**
 * ProtocolSimGetSimAuthAdapterLegacy
 */
int ProtocolSimGetSimAuthAdapterLegacy::GetPayloadLength() const
{
    if (m_pModemData != NULL) {
        sit_sim_get_sim_auth_rsp *pData = (sit_sim_get_sim_auth_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id==SIT_GET_SIM_AUTH) {
            return (int) (pData->hdr.length - sizeof(RCM_HEADER));
        }
    }

    return 0;
}

int ProtocolSimGetSimAuthAdapterLegacy::GetAuthType() const
{
    if (m_pModemData != NULL) {
        sit_sim_get_sim_auth_rsp *pData = (sit_sim_get_sim_auth_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id==SIT_GET_SIM_AUTH) {
            return (int) pData->auth_type;
        }
    }

    return 0;
}

int ProtocolSimGetSimAuthAdapterLegacy::GetAuthLength() const
{
    if (m_pModemData != NULL) {
        sit_sim_get_sim_auth_rsp *pData = (sit_sim_get_sim_auth_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id==SIT_GET_SIM_AUTH) {
            return (int) pData->auth_len;
        }
    }

    return 0;
}

BYTE *ProtocolSimGetSimAuthAdapterLegacy::GetAuth() const
{
    if (m_pModemData != NULL) {
        sit_sim_get_sim_auth_rsp *pData = (sit_sim_get_sim_auth_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id==SIT_GET_SIM_AUTH) {
            return pData->auth;
        }
    }

    return NULL;
}

/**
 * ProtocolSimTransmitApduBasicAdapterLegacy
 */
BYTE ProtocolSimTransmitApduBasicAdapterLegacy::GetSw1() const
{
    BYTE sw1 = 0;
    if (m_pModemData != NULL) {
        sit_sim_transmit_sim_apdu_basic_rsp *pData = (sit_sim_transmit_sim_apdu_basic_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_TRANSMIT_SIM_APDU_BASIC) {
            sw1 = (GetApduLength()>=2)? pData->apdu[GetApduLength()-2] : 0;
        }
    }

    return sw1;
}

BYTE ProtocolSimTransmitApduBasicAdapterLegacy::GetSw2() const
{
    BYTE sw2 = 0;
    if (m_pModemData != NULL) {
        sit_sim_transmit_sim_apdu_basic_rsp *pData = (sit_sim_transmit_sim_apdu_basic_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_TRANSMIT_SIM_APDU_BASIC) {
            sw2 = (GetApduLength()>=2)? pData->apdu[GetApduLength()-1] : 0;
        }
    }

    return sw2;
}

int ProtocolSimTransmitApduBasicAdapterLegacy::GetApduLength() const
{
    int nLength = 0;
    if (m_pModemData != NULL) {
        sit_sim_transmit_sim_apdu_basic_rsp *pData = (sit_sim_transmit_sim_apdu_basic_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_TRANSMIT_SIM_APDU_BASIC) {
            nLength =  pData->apdu_len;
            if(nLength>MAX_APDU_LEN) nLength = MAX_APDU_LEN;
            if(nLength>=(int)(GetParameterLength()-2)) nLength = GetParameterLength() - 2;        // 2 is for APDU length(WORD)
        }
    }

    return nLength;
}

BYTE *ProtocolSimTransmitApduBasicAdapterLegacy::GetApdu() const
{
    BYTE *pApdu = NULL;
    if (m_pModemData != NULL) {
        sit_sim_transmit_sim_apdu_basic_rsp *pData = (sit_sim_transmit_sim_apdu_basic_rsp*) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_TRANSMIT_SIM_APDU_BASIC) {
            pApdu = pData->apdu;
        }
    }

    return pApdu;
}

/**
 * ProtocolSimOpenChannelAdapterLegacy
 */
int ProtocolSimOpenChannelAdapterLegacy::GetSessionID() const
{
    int nSessionID = 0;
    if (m_pModemData != NULL) {
        sit_sim_open_channel_rsp *pData = (sit_sim_open_channel_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && (pData->hdr.id == SIT_OPEN_SIM_CHANNEL || pData->hdr.id == SIT_OPEN_SIM_CHANNEL_WITH_P2)) {
            nSessionID = (int) pData->session_id;
        }
    }

    return nSessionID;
}

BYTE ProtocolSimOpenChannelAdapterLegacy::GetSw1() const
{
    BYTE sw1 = 0;
    if (m_pModemData != NULL) {
        sit_sim_open_channel_rsp *pData = (sit_sim_open_channel_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && (pData->hdr.id == SIT_OPEN_SIM_CHANNEL || pData->hdr.id == SIT_OPEN_SIM_CHANNEL_WITH_P2)) {
            sw1 = pData->sw1;
        }
    }

    return sw1;
}

BYTE ProtocolSimOpenChannelAdapterLegacy::GetSw2() const
{
    BYTE sw2 = 0;
    if (m_pModemData != NULL) {
        sit_sim_open_channel_rsp *pData = (sit_sim_open_channel_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && (pData->hdr.id == SIT_OPEN_SIM_CHANNEL || pData->hdr.id == SIT_OPEN_SIM_CHANNEL_WITH_P2)) {
            sw2 = pData->sw2;
        }
    }

    return sw2;
}

BYTE *ProtocolSimOpenChannelAdapterLegacy::GetResponse() const
{
    BYTE *pResponse = NULL;
    if (m_pModemData != NULL) {
        sit_sim_open_channel_rsp *pData = (sit_sim_open_channel_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && (pData->hdr.id == SIT_OPEN_SIM_CHANNEL || pData->hdr.id == SIT_OPEN_SIM_CHANNEL_WITH_P2)) {
            pResponse = pData->response;
        }
    }

    return pResponse;
}

int ProtocolSimOpenChannelAdapterLegacy::GetResponseLength() const
{
    int nResponseLength = 0;
    if (m_pModemData != NULL) {
        sit_sim_open_channel_rsp *pData = (sit_sim_open_channel_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && (pData->hdr.id == SIT_OPEN_SIM_CHANNEL || pData->hdr.id == SIT_OPEN_SIM_CHANNEL_WITH_P2)) {
            nResponseLength =  pData->response_len;
            if(nResponseLength>MAX_OPEN_CHANNEL_RSP_LEN) nResponseLength = MAX_OPEN_CHANNEL_RSP_LEN;
            if(nResponseLength>(int)(GetParameterLength()-2)) nResponseLength = GetParameterLength() - 2;        // 2 is for APDU length(WORD)
        }
    }

    return nResponseLength;
}

/**
 * ProtocolSimTransmitApduChannelAdapterLegacy
 */
BYTE ProtocolSimTransmitApduChannelAdapterLegacy::GetSw1() const
{
    BYTE sw1 = 0;
    if (m_pModemData != NULL) {
        sit_sim_transmit_sim_apdu_channel_rsp *pData = (sit_sim_transmit_sim_apdu_channel_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_TRANSMIT_SIM_APDU_CHANNEL) {
            sw1 = pData->sw1;
        }
    }

    return sw1;
}

BYTE ProtocolSimTransmitApduChannelAdapterLegacy::GetSw2() const
{
    BYTE sw2 = 0;
    if (m_pModemData != NULL) {
        sit_sim_transmit_sim_apdu_channel_rsp *pData = (sit_sim_transmit_sim_apdu_channel_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_TRANSMIT_SIM_APDU_CHANNEL) {
            sw2 = pData->sw2;
        }
    }

    return sw2;
}

int ProtocolSimTransmitApduChannelAdapterLegacy::GetApduLength() const
{
    int nLength = 0;
    if (m_pModemData != NULL) {
        sit_sim_transmit_sim_apdu_channel_rsp *pData = (sit_sim_transmit_sim_apdu_channel_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_TRANSMIT_SIM_APDU_CHANNEL) {
            nLength = pData->response_len;
            if(nLength>MAX_APDU_LEN) nLength = MAX_APDU_LEN;
            if(nLength>(int)(GetParameterLength()-2)) nLength = GetParameterLength() - 2;        // 2 is for APDU length(WORD)
        }
    }

    return nLength;
}

BYTE *ProtocolSimTransmitApduChannelAdapterLegacy::GetApdu() const
{
    BYTE *pApdu = NULL;
    if (m_pModemData != NULL) {
        sit_sim_transmit_sim_apdu_channel_rsp *pData = (sit_sim_transmit_sim_apdu_channel_rsp*) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id == SIT_TRANSMIT_SIM_APDU_CHANNEL) {
            pApdu = pData->response;
        }
    }

    return pApdu;
}

/* ProtocolSimImsiAdapterLegacy */
ProtocolSimImsiAdapterLegacy::ProtocolSimImsiAdapterLegacy(const ModemData *pModemData)
    : ProtocolSimImsiAdapterInterface(), ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolSimImsiAdapterLegacy::Init()
{
    memset(m_imsi, 0, sizeof(m_imsi));
    if (m_pModemData != NULL) {
        sit_id_get_imsi_rp *data = (sit_id_get_imsi_rp *)m_pModemData->GetRawData();
        if (GetErrorCode() == RIL_E_SUCCESS && data != NULL && data->hdr.id == SIT_GET_IMSI) {
            if (data->imsi_len <= MAX_IMSI_LEN) {
                memcpy(m_imsi, data->imsi, data->imsi_len);
            } else {
                RilLogE("%s data->imsi_len %d is invalid.", __FUNCTION__, (int)data->imsi_len);
            }
        }
    }
}

const char *ProtocolSimImsiAdapterLegacy::GetImsi() const
{
    if (strlen(m_imsi) > 0)
        return m_imsi;
    return NULL;
}

int ProtocolSimGetGbaAuthAdapterLegacy::GetGbaAuthLength() const
{
    if (m_pModemData != NULL) {
        sit_sim_get_sim_gba_auth_rsp *pData = (sit_sim_get_sim_gba_auth_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id==SIT_GET_GBA_CONTEXT) {
            return sizeof(pData->auth);
        }
    }
    return 0;
}

BYTE *ProtocolSimGetGbaAuthAdapterLegacy::GetGbaAuth() const
{
    if (m_pModemData != NULL) {
        sit_sim_get_sim_gba_auth_rsp *pData = (sit_sim_get_sim_gba_auth_rsp *) m_pModemData->GetRawData();
        if (pData != NULL && pData->hdr.id==SIT_GET_GBA_CONTEXT) {
            return pData->auth;
        }
    }
    return 0;
}

/* ProtocolSimATRAdapterLegacy */
ProtocolSimATRAdapterLegacy::ProtocolSimATRAdapterLegacy(const ModemData *pModemData)
    : ProtocolSimATRAdapterInterface(), ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolSimATRAdapterLegacy::Init()
{
    memset(m_atr, 0, sizeof(m_atr));
    if (m_pModemData != NULL) {
        sit_id_get_atr_rsp *data = (sit_id_get_atr_rsp *)m_pModemData->GetRawData();
        if (GetErrorCode() == RIL_E_SUCCESS && data != NULL && data->hdr.id == SIT_GET_ATR) {
            m_result = data->result;
            m_atrlen = data->atr_len;
            if (sizeof(m_atr) >= m_atrlen) {
                memcpy(m_atr, data->atr, m_atrlen);
            } else {
                RilLogE("%s m_atrlen %d is invalid.", __FUNCTION__, (int)m_atrlen);
            }
        }
    }
}

BYTE ProtocolSimATRAdapterLegacy::GetResult() const
{
    return m_result;
}

BYTE ProtocolSimATRAdapterLegacy::GetATRLength() const
{
    return m_atrlen;
}

const char *ProtocolSimATRAdapterLegacy::GetATR() const
{
    if (m_atrlen > 0)
        return m_atr;
    return NULL;
}

/* ProtocolSimReadPbEntryLegacy */
ProtocolSimReadPbEntryLegacy::ProtocolSimReadPbEntryLegacy(const ModemData *pModemData)
    : ProtocolSimReadPbEntryInterface(), ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolSimReadPbEntryLegacy::Init()
{
    m_pbType = 0;
    m_recordId = 0;
    m_dataLen = 0;

    if (m_pModemData != NULL && GetErrorCode() == RIL_E_SUCCESS) {
        sit_read_pb_resp *data = (sit_read_pb_resp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_READ_PB_ENTRY) {
            m_pbType = data->pb_type;
            m_recordId = data->recordId;
            m_dataLen = data->data_len;
            if (m_dataLen > 0 && (int)sizeof(m_entryData) >= m_dataLen) {
                memcpy(m_entryData, data->entry_data, m_dataLen);
            } else {
                RilLogE("%s m_dataLen %d is invalid.", __FUNCTION__, m_dataLen);
            }
        }
    }
}

/* ProtocolSimUpdatePbEntryLegacy */
ProtocolSimUpdatePbEntryLegacy::ProtocolSimUpdatePbEntryLegacy(const ModemData *pModemData)
    : ProtocolSimUpdatePbEntryInterface(), ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolSimUpdatePbEntryLegacy::Init()
{
    m_mode = 0;
    m_pbtype = 0;
    m_recordId = 0;

    if (m_pModemData != NULL && GetErrorCode() == RIL_E_SUCCESS) {
        sit_update_pb_entry_resp *data = (sit_update_pb_entry_resp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_UPDATE_PB_ENTRY) {
            m_mode = data->mode;
            m_pbtype = data->pb_type;
            m_recordId = data->recordId;
        }
    }
}

/* ProtocolSimPbStorageInfoLegacy */
ProtocolSimPbStorageInfoAdapterLegacy::ProtocolSimPbStorageInfoAdapterLegacy(const ModemData *pModemData)
    : ProtocolSimPbStorageInfoAdapterInterface(), ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolSimPbStorageInfoAdapterLegacy::Init()
{
    m_pbType = 0;
    m_totalCount = 0;
    m_usedCount = 0;

    if (m_pModemData != NULL && GetErrorCode() == RIL_E_SUCCESS) {
        sit_sim_pb_storage_info_rsp *data = (sit_sim_pb_storage_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PB_STORAGE_INFO) {
            m_pbType = data->pb_type;
            m_totalCount = data->total_count;
            m_usedCount = data->used_count;
        }
    }
}

/* ProtocolSim3GPBCapaAdapterLegacy */
bool ProtocolSimPbCapaAdapterLegacy::GetPbCapa(int *pb, int entryNum)
{
    if (m_pModemData != NULL) {
        sit_sim_pb_capa_rsp *data = (sit_sim_pb_capa_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_3G_PB_CAPA) {
            int j = 0;
            for (int i=0; i < entryNum; i++) {
                pb[j] = data->pb_list[i].pb_type;
                pb[j+1] = data->pb_list[i].index_max;
                pb[j+2] = data->pb_list[i].entry_max;
                pb[j+3] = data->pb_list[i].used_count;
                j += 4;
            }
        }
    }

    return true;
}

int ProtocolSimPbCapaAdapterLegacy::GetEntryNum() const
{
    if (m_pModemData != NULL) {
        sit_sim_pb_capa_rsp *data = (sit_sim_pb_capa_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_3G_PB_CAPA)
            return data->entry_num;
    }

    return 0;
}

/* ProtocolSimPbReadyAdapterLegacy */
ProtocolSimPbReadyAdapterLegacy::ProtocolSimPbReadyAdapterLegacy(const ModemData *pModemData)
    : ProtocolSimPbReadyAdapterInterface(), ProtocolIndAdapter(pModemData) {
    Init();
}

void ProtocolSimPbReadyAdapterLegacy::Init()
{
    m_pbReady = -1;

    if (m_pModemData != NULL) {
        sit_sim_pb_ready_ind *data = (sit_sim_pb_ready_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_SIM_PB_READY) {
            m_pbReady = data->pb_ready;
        }
    }
}

/* ProtocolSimGetCarrierRestrictionsAdapterLegacy */
ProtocolSimGetCarrierRestrictionsAdapterLegacy::ProtocolSimGetCarrierRestrictionsAdapterLegacy(const ModemData *pModemData)
    : ProtocolSimGetCarrierRestrictionsAdapterInterface(), ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolSimGetCarrierRestrictionsAdapterLegacy::Init()
{
    m_nAllowedCarrierLength = 0;
    m_nExcludedCarrierLength = 0;
    m_pAllowedCarriers = NULL;
    m_pExcludedCarriers = NULL;
    m_bAllowedCarriersPrioritized = false;
    m_nMultiSimPolicy = 0;

    if (m_pModemData != NULL) {
        sit_sim_get_carrier_restriction_rsp *pRsp =
                (sit_sim_get_carrier_restriction_rsp *)m_pModemData->GetRawData();
        if (pRsp != NULL && pRsp->hdr.id == SIT_GET_CARRIER_RESTRICTIONS
                && GetErrorCode() == RIL_E_SUCCESS) {
            const int SIZE_TO_EXCLUDE = (sizeof(INT32) * 2 + sizeof(BYTE) * 2);
            int CarrierDataLen = (GetParameterLength() - SIZE_TO_EXCLUDE);
            int allCarrierLength =
                    (CarrierDataLen > 0) ? (CarrierDataLen / sizeof(SIT_CARRIER_INFO)) : 0;

            //First, fill m_pAllowedCarriers and m_pExcludedCarriers
            m_nAllowedCarrierLength =
                    (pRsp->allowed_carriers_len > 0) ? pRsp->allowed_carriers_len : 0;
            m_nExcludedCarrierLength =
                    (pRsp->excluded_carriers_len > 0) ? pRsp->excluded_carriers_len : 0;
            SIT_CARRIER_INFO *pCarrierList = &pRsp->carrier_list[0];
            if (m_nAllowedCarrierLength <= allCarrierLength) {
                m_pAllowedCarriers = pCarrierList;
            } else {
                RilLogE("%s Carrier length is wrong (Allowed:%d, All%d)", __FUNCTION__,
                        m_nAllowedCarrierLength, allCarrierLength);
                return;
            }

            if (m_nExcludedCarrierLength <= (allCarrierLength - m_nAllowedCarrierLength)) {
                m_pExcludedCarriers = &pCarrierList[m_nAllowedCarrierLength];
            } else {
                RilLogE("%s Carrier length is wrong (Allowed:%d, Excluded:%d, All:%d)",
                        __FUNCTION__, m_nAllowedCarrierLength, m_nExcludedCarrierLength,
                        allCarrierLength);
                return;
            }

            // Second, fill m_bAllowedCarriersPrioritized and m_nMultiSimPolicy
            if (2 <= m_pModemData->GetLength()) {
                BYTE *pData = (BYTE *) pRsp;
                m_bAllowedCarriersPrioritized = pData[m_pModemData->GetLength() - 2];
                m_nMultiSimPolicy = static_cast<int> (pData[m_pModemData->GetLength() - 1]);
            }
        }
    }
}

int ProtocolSimGetCarrierRestrictionsAdapterLegacy::GetAllowedCarriers(RIL_Carrier *pCarriers, int nSize) const
{
    if (pCarriers == NULL || nSize == 0 || m_pAllowedCarriers == NULL
            || m_nAllowedCarrierLength == 0 || nSize < m_nAllowedCarrierLength) return 0;

    memset(pCarriers, 0, sizeof(RIL_Carrier) * nSize);
    for(int i = 0; i < m_nAllowedCarrierLength; i++) {
        pCarriers[i].mcc = new char[MAX_MCC_LEN + 1] {};
        pCarriers[i].mnc = new char[MAX_MNC_LEN + 1] {};
        pCarriers[i].match_data = new char[MAX_CR_MATCH_DATA_SIZE + 1] {};

        memcpy((void*)pCarriers[i].mcc, (const void*)m_pAllowedCarriers[i].mcc, MAX_MCC_LEN);
        memcpy((void*)pCarriers[i].mnc, (const void*)m_pAllowedCarriers[i].mnc, MAX_MNC_LEN);
        memcpy((void*)pCarriers[i].match_data, (const void*)m_pAllowedCarriers[i].match_data,
                MAX_CR_MATCH_DATA_SIZE);
        pCarriers[i].match_type = (RIL_CarrierMatchType) m_pAllowedCarriers[i].match_type;
    }

    return m_nAllowedCarrierLength;
}

int ProtocolSimGetCarrierRestrictionsAdapterLegacy::GetExcludedCarriers(RIL_Carrier *pCarriers, int nSize) const
{
    if (pCarriers == NULL || nSize == 0 || m_pExcludedCarriers == NULL
            || m_nExcludedCarrierLength == 0 || nSize < m_nExcludedCarrierLength) return 0;

    memset(pCarriers, 0, sizeof(RIL_Carrier) * nSize);
    for(int i = 0; i < m_nExcludedCarrierLength; i++) {
        pCarriers[i].mcc = new char[MAX_MCC_LEN + 1] {};
        pCarriers[i].mnc = new char[MAX_MNC_LEN + 1] {};
        pCarriers[i].match_data = new char[MAX_CR_MATCH_DATA_SIZE + 1] {};

        memcpy((void*)pCarriers[i].mcc, (const void*)m_pExcludedCarriers[i].mcc, MAX_MCC_LEN);
        memcpy((void*)pCarriers[i].mnc, (const void*)m_pExcludedCarriers[i].mnc, MAX_MNC_LEN);
        memcpy((void*)pCarriers[i].match_data, (const void*)m_pExcludedCarriers[i].match_data,
                MAX_CR_MATCH_DATA_SIZE);
        pCarriers[i].match_type = (RIL_CarrierMatchType) m_pExcludedCarriers[i].match_type;
    }

    return m_nExcludedCarrierLength;
}

/* ProtocolUiccSubStatusChangeAdapterLegacy */
int ProtocolUiccSubStatusChangeAdapterLegacy::GetState() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_sim_uicc_sub_state_changed_ind *data = (sit_sim_uicc_sub_state_changed_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_UICC_SUBSCRIPTION_STATE_CHANGED)
            ret = data->state;
    }

    return ret;
}

/* ProtocolSimLockInfoAdapterLegacy */
int ProtocolSimLockInfoAdapterLegacy::GetPolicy() const {
    if (m_pModemData != NULL) {
        sit_sim_get_sim_lock_info_rsp *data = (sit_sim_get_sim_lock_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_SIM_LOCK_INFO) {
            return data->policy & 0xF;
        }
    }
    return -1;
}

int ProtocolSimLockInfoAdapterLegacy::GetStatus() const {
    if (m_pModemData != NULL) {
        sit_sim_get_sim_lock_info_rsp *data = (sit_sim_get_sim_lock_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_SIM_LOCK_INFO) {
            return data->status & 0xF;
        }
    }
    return -1;
}

int ProtocolSimLockInfoAdapterLegacy::GetLockType() const {
    if (m_pModemData != NULL) {
        sit_sim_get_sim_lock_info_rsp *data = (sit_sim_get_sim_lock_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_SIM_LOCK_INFO) {
            return data->lockType & 0xF;
        }
    }
    return -1;
}

int ProtocolSimLockInfoAdapterLegacy::GetMaxRetryCount() const {
    if (m_pModemData != NULL) {
        sit_sim_get_sim_lock_info_rsp *data = (sit_sim_get_sim_lock_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_SIM_LOCK_INFO) {
            return data->maxRetryCount & 0xFF;
        }
    }
    return 0;
}

int ProtocolSimLockInfoAdapterLegacy::GetRemainCount() const {
    if (m_pModemData != NULL) {
        sit_sim_get_sim_lock_info_rsp *data = (sit_sim_get_sim_lock_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_SIM_LOCK_INFO) {
            return data->reaminCount & 0xFF;
        }
    }
    return 0;
}

int ProtocolSimLockInfoAdapterLegacy::GetLockCodeCount() const {
    if (m_pModemData != NULL) {
        sit_sim_get_sim_lock_info_rsp *data = (sit_sim_get_sim_lock_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_SIM_LOCK_INFO) {
            return data->lockCodeCount & 0xFFFF;
        }
    }
    return 0;
}

const char *ProtocolSimLockInfoAdapterLegacy::GetLockCode() const {
    if (m_pModemData != NULL) {
        sit_sim_get_sim_lock_info_rsp *data = (sit_sim_get_sim_lock_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_SIM_LOCK_INFO) {
            return data->lockCode;
        }
    }
    return NULL;
}

int ProtocolSimLockInfoAdapterLegacy::GetLockCodeSize() const {
    int size = 0;
    int lockType = GetLockType();
    int entryCount = GetLockCodeCount();
    if (lockType != SIT_LOCK_TYPE_UNKNOWN && entryCount > 0 && GetLockCode() != NULL) {
        if (lockType == SIT_LOCK_TYPE_PN) {
            size = 6 * entryCount;
        }
        else {
            size = 2 * entryCount;
        }
    }
    return size;
}

#ifdef AUTO_VERIFY_PIN_WITH_CP
/* ProtocolSimEncryptedPinAdapterLegacy */
void ProtocolSimEncryptedPinAdapterLegacy::Init()
{
    m_nEncryptedPinLen = -1;
    m_nPinLen = -1;

    memset(m_szEncryptedPin, 0x00, sizeof(m_szEncryptedPin));
    if (m_pModemData != NULL) {
        sit_sim_encrypted_pin_ind *data = (sit_sim_encrypted_pin_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_IND_SIM_PIN_ENC) {
            m_nEncryptedPinLen = data->encrypted_pin_len;
            m_nPinLen = data->pin_len;
            if (0 < m_nEncryptedPinLen) {
                memset(m_szEncryptedPin, 0x00, sizeof(m_szEncryptedPin));
                Value2HexString(m_szEncryptedPin, data->encrypted_pin, m_nEncryptedPinLen);
            }
        }
    }
}

const char *ProtocolSimEncryptedPinAdapterLegacy::GetEncryptedPin() const
{
    if (0 < m_nEncryptedPinLen) {
        return m_szEncryptedPin;
    }
    return NULL;
}
#endif

/* ProtocolSimTrayStatusChangeAdapterLegacy */
int ProtocolSimTrayStatusChangeAdapterLegacy::GetState() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_sim_tray_status_changed_ind *data = (sit_sim_tray_status_changed_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_SIM_TRAY_STATUS_CHANGED)
            ret = data->state;
    }

    return ret;
}
