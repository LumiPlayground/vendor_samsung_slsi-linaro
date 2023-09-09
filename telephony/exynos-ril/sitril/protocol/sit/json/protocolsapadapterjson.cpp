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
 * protocolsapadapterjson.cpp
 *
 *  Created on: 2021. 8. 3.
 *      Author: aryaman.2019
 */
#include "protocolsapadapterjson.h"
#include "protocolutils.h"
#include "rillog.h"
#include "simjsondef.h"
#include "sitdef.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SAP, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

ProtocolSapConnectAdapterJson::ProtocolSapConnectAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_SAP_CONNECT);
}

int ProtocolSapConnectAdapterJson::GetStatus() const {
    int nStatus = 0;
    if(m_bParsingSuccess) {
        nStatus = m_jsonData[ns_sap_connect_rsp::key_status].asInt();
    }
    return nStatus;
}

int ProtocolSapConnectAdapterJson::GetMaxMsgSize() const {
    int nMaxMsgSize = 0;
    if(m_bParsingSuccess) {
        nMaxMsgSize = m_jsonData[ns_sap_connect_rsp::key_max_msg_size].asInt();
    }
    return nMaxMsgSize;
}

ProtocolSapDisconnectAdapterJson::ProtocolSapDisconnectAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_SAP_DISCONNECT);
}

int ProtocolSapDisconnectAdapterJson::GetStatus() const {
    int nStatus = -1;
    if(m_bParsingSuccess) {
        nStatus = m_jsonData[ns_sap_disconnect_rsp::key_status].asInt();
    }
    return nStatus;
}

ProtocolSapTransferApduAdapterJson::ProtocolSapTransferApduAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSapTransferApduAdapterJson::Init() {
    memset(m_apdu, 0, sizeof(m_apdu));
    if (ProtocolJsonRespAdapter::GetErrorCode() == RCM_E_SUCCESS) {
        ParseJsonPayload(SIT_SAP_TRANSFER_APDU);
        SetApduData();
    }
}

void ProtocolSapTransferApduAdapterJson::SetApduData() {
    if(m_bParsingSuccess) {
        int nApduLen = GetApduLen();
        for(int i = 0; i < nApduLen; i++) {
            m_apdu[i] = m_jsonData[ns_sap_transfer_apdu_rsp::key_rsp_apdu][i].asInt();
        }
    }
}

int ProtocolSapTransferApduAdapterJson::GetResult() const {
    int nResult = -1;
    if(m_bParsingSuccess) {
        nResult = m_jsonData[ns_sap_transfer_apdu_rsp::key_result].asInt();
    }
    return nResult;
}

int ProtocolSapTransferApduAdapterJson::GetApduLen() const {
    unsigned int nApduLen = 0;
    if(m_bParsingSuccess) {
        nApduLen = m_jsonData[ns_sap_transfer_apdu_rsp::key_rsp_apdu].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SAP, __CLASS_FUNC__, nApduLen, MAX_SAP_TRANSFER_APDU_LEN);
    }
    return nApduLen;
}

ProtocolSapTransferAtrAdapterJson::ProtocolSapTransferAtrAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSapTransferAtrAdapterJson::Init() {
    memset(m_atr, 0, sizeof(m_atr));
    if (ProtocolJsonRespAdapter::GetErrorCode() == RCM_E_SUCCESS) {
        ParseJsonPayload(SIT_SAP_TRANSFER_ATR);
        SetAtrData();
    }
}

void ProtocolSapTransferAtrAdapterJson::SetAtrData() {
    if(m_bParsingSuccess) {
        int nAtrLen = GetAtrLen();
        for(int i = 0; i < nAtrLen; i++) {
            m_atr[i] = m_jsonData[ns_sap_transfer_atr_rsp::key_atr][i].asInt();
        }
    }
}

int ProtocolSapTransferAtrAdapterJson::GetResult() const {
    int nResult = -1;
    if(m_bParsingSuccess) {
        nResult = m_jsonData[ns_sap_transfer_atr_rsp::key_result].asInt();
    }
    return nResult;
}

int ProtocolSapTransferAtrAdapterJson::GetAtrLen() const {
    unsigned int nAtrLen = 0;
    if(m_bParsingSuccess) {
        nAtrLen = m_jsonData[ns_sap_transfer_atr_rsp::key_atr].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SAP, __CLASS_FUNC__, nAtrLen, MAX_SAP_TRANSFER_ATR_LEN);
    }
    return nAtrLen;
}

ProtocolSapPowerSimOnOffAdapterJson::ProtocolSapPowerSimOnOffAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSapPowerSimOnOffAdapterJson::Init() {
    ParseJsonPayload(SIT_SAP_POWER_SIM_OFF) || ParseJsonPayload(SIT_SAP_POWER_SIM_ON);
}

int ProtocolSapPowerSimOnOffAdapterJson::GetResult() const {
    int nResult = -1;
    if(m_bParsingSuccess) {
        nResult = m_jsonData[ns_sap_power_sim_onoff_rsp::key_result].asInt();
    }
    return nResult;
}

ProtocolSapResetSimAdapterJson::ProtocolSapResetSimAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_SAP_RESET_SIM);
}

int ProtocolSapResetSimAdapterJson::GetResult() const {
    int nResult = -1;
    if(m_bParsingSuccess) {
        nResult = m_jsonData[ns_sap_reset_sim_rsp::key_result].asInt();
    }
    return nResult;
}

ProtocolSapTransferCardReaderStatusAdapterJson::ProtocolSapTransferCardReaderStatusAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_SAP_TRANSFER_CARD_READER_STATUS);
}

int ProtocolSapTransferCardReaderStatusAdapterJson::GetResult() const {
    int nResult = -1;
    if(m_bParsingSuccess) {
        nResult = m_jsonData[ns_sap_transfer_card_reader_status_rsp::key_result].asInt();
    }
    return nResult;
}

ProtocolSapSetTranportProtocolAdapterJson::ProtocolSapSetTranportProtocolAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_SAP_SET_TRANPORT_PROTOCOL);
}

int ProtocolSapSetTranportProtocolAdapterJson::GetResult() const {
    int nResult = -1;
    if(m_bParsingSuccess) {
        nResult = m_jsonData[ns_sap_tranport_protocol_rsp::key_result].asInt();
    }
    return nResult;

}

ProtocolSapDisconnectTypeAdapterJson::ProtocolSapDisconnectTypeAdapterJson(const ModemData *pModemData) : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_SAP_DISCONNECT);
}

int ProtocolSapDisconnectTypeAdapterJson::GetDisconStatus() const {
    int nStatus = -1;
    if(m_bParsingSuccess) {
        nStatus = m_jsonData[ns_sap_disconnect_ind::key_disconnect_type].asInt();
    }
    return nStatus;
}

ProtocolSapStatusAdapterJson::ProtocolSapStatusAdapterJson(const ModemData *pModemData) : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_SAP_STATUS);
}

int ProtocolSapStatusAdapterJson::GetCardStatus() const {
    int nCardStatus = -1;
    if(m_bParsingSuccess) {
        nCardStatus = m_jsonData[ns_sap_status_ind::key_card_status].asInt();
    }
    return nCardStatus;
}
