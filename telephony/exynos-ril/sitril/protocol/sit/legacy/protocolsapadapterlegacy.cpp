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
 * protocolsapadapterlegacy.cpp
 *
 *  Created on: 2019. 3. 13.
 *      Author: ji_young.bae
 */
#include "protocolsapadapterlegacy.h"
#include "rillog.h"
#include "sitdef.h"
#include "modemdata.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

ProtocolSapConnectAdapterLegacy::ProtocolSapConnectAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), m_status(0), m_maxMsgSize(0)
{
    if (m_pModemData != NULL) {
        sit_sap_connect_rsp *data = (sit_sap_connect_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SAP_CONNECT) {
            m_status = (int)data->status;
            m_maxMsgSize = (int)data->max_msg_size;
        }
    }
}

int ProtocolSapDisconnectAdapterLegacy::GetStatus() const
{
    if (m_pModemData != NULL) {
        sit_sap_disconnect_rsp *data = (sit_sap_disconnect_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SAP_DISCONNECT) {
            return (int)data->status;
        }
    }
    return -1;
}

//ind

ProtocolSapTransferApduAdapterLegacy::ProtocolSapTransferApduAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), m_result(0), m_apduLen(0)
{
    memset(m_apdu, 0, sizeof(m_apdu));

    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        sit_sap_transfer_apdu_rsp *data = (sit_sap_transfer_apdu_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SAP_TRANSFER_APDU) {
            m_result = (int)data->result;
            m_apduLen = (int)data->apdu_len;
            if (m_apduLen > 0 && (int)sizeof(m_apdu) >= m_apduLen) {
                strncpy(m_apdu, data->apdu, m_apduLen);
            } else {
                RilLogE("%s m_apduLen %d is invalid", __FUNCTION__, m_apduLen);
            }
        }
    }
}

const char *ProtocolSapTransferApduAdapterLegacy::GetApdu() const
{
    return (m_apduLen == 0 ? NULL : m_apdu);
}

ProtocolSapTransferAtrAdapterLegacy::ProtocolSapTransferAtrAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), m_result(0), m_atrLen(0)
{
    memset(m_atr, 0, sizeof(m_atr));

    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        sit_sap_transfer_atr_rsp *data = (sit_sap_transfer_atr_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SAP_TRANSFER_ATR) {
            m_result = (int)data->result;
            m_atrLen = (int)data->atr_len;
            if (m_atrLen > 0 && (int)sizeof(m_atr) >= m_atrLen) {
                strncpy(m_atr, data->atr, m_atrLen);
            } else {
                RilLogE("%s m_atrLen %d is invalid", __FUNCTION__, m_atrLen);
            }
        }
    }
}

const char *ProtocolSapTransferAtrAdapterLegacy::GetAtr() const
{
    return (m_atrLen == 0 ? NULL : m_atr);
}

int ProtocolSapPowerSimOnOffAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_sap_power_sim_onoff_rsp *data = (sit_sap_power_sim_onoff_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_SAP_POWER_SIM_OFF
                    || data->hdr.id == SIT_SAP_POWER_SIM_ON)) {
            return (int)data->result;
        }
    }
    return -1;
}

int ProtocolSapResetSimAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_sap_reset_sim_rsp *data = (sit_sap_reset_sim_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SAP_RESET_SIM) {
            return (int)data->result;
        }
    }
    return -1;
}

int ProtocolSapTransferCardReaderStatusAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_sap_transfer_card_reader_status_rsp *data = (sit_sap_transfer_card_reader_status_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SAP_TRANSFER_CARD_READER_STATUS) {
            return (int)data->result;
        }
    }
    return -1;
}

int ProtocolSapSetTranportProtocolAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_sap_tranport_protocol_rsp *data = (sit_sap_tranport_protocol_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SAP_SET_TRANPORT_PROTOCOL) {
            return (int)data->result;
        }
    }
    return -1;
}

int ProtocolSapDisconnectTypeAdapterLegacy::GetDisconStatus() const
{
    if (m_pModemData != NULL) {
        sit_sap_disconnect_ind *data = (sit_sap_disconnect_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_SAP_DISCONNECT) {
            return (int)data->status;
        }
    }
    return -1;
}

int ProtocolSapStatusAdapterLegacy::GetCardStatus() const
{
    if (m_pModemData != NULL) {
        sit_sap_status_ind *data = (sit_sap_status_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_SAP_STATUS) {
            return (int)data->card_status;
        }
    }
    return -1;
}
