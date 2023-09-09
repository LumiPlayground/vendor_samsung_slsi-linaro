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
 * protocolsmsadapterjson.cpp
 *
 *  Created on: 2021. 8. 27.
 *      Author: aryaman.2019
 */

#include "protocolsmsadapterjson.h"
#include "protocolutils.h"
#include "smsjsondef.h"
#include "rillog.h"
#include "util.h"
#include "smsdata.h"
#include "cdmasmsdata.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * ProtocolSendSmsRespAdapterJson
 */
ProtocolSendSmsRespAdapterJson::ProtocolSendSmsRespAdapterJson(const ModemData *pModemData)
    : ProtocolSendSmsRespAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSendSmsRespAdapterJson::Init() {
    m_nPduSize = 0;
    m_pPdu = NULL;
    ParseJsonPayload(SIT_SEND_SMS) || ParseJsonPayload(SIT_SEND_SMS_EXPECT_MORE);
    SetPdu();
}

void ProtocolSendSmsRespAdapterJson::SetPdu() {
    if (!m_bParsingSuccess) {
        return;
    }
    m_nPduSize = m_jsonData[ns_sms_send_sms_rsp::key_ack_pdu].size();

    ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SMS, __CLASS_FUNC__, m_nPduSize, MAX_GSM_SMS_TPDU_SIZE);

    m_pPdu = new char[m_nPduSize];
    for (unsigned int i = 0; i < m_nPduSize; i++) {
        m_pPdu[i] = m_jsonData[ns_sms_send_sms_rsp::key_ack_pdu][i].asInt();
    }
}

int ProtocolSendSmsRespAdapterJson::GetRef() const {
    int nMsgRef = -1;
    if (m_bParsingSuccess) {
        nMsgRef = m_jsonData[ns_sms_send_sms_rsp::key_msg_ref].asInt();
    }
    return nMsgRef;
}

int ProtocolSendSmsRespAdapterJson::GetSmsRspErrorCode() const {
    int nErrorCode = -1;
    if (m_bParsingSuccess) {
        nErrorCode = m_jsonData[ns_sms_send_sms_rsp::key_error_code].asInt();
    }
    return nErrorCode;
}

/**
 * ProtocolCdmaNewSmsIndAdapterJson
 */
ProtocolCdmaNewSmsIndAdapterJson::ProtocolCdmaNewSmsIndAdapterJson(const ModemData *pModemData)
    : ProtocolCdmaNewSmsIndAdapterInterface(), ProtocolJsonIndAdapter(pModemData)
{
    m_pCCdmaSmsMsg = NULL;
    ParseJsonPayload(SIT_IND_CDMA_NEW_SMS);
    SetCdmaSmsMsg();
}

ProtocolCdmaNewSmsIndAdapterJson::~ProtocolCdmaNewSmsIndAdapterJson() {
    if (m_pCCdmaSmsMsg) {
        delete m_pCCdmaSmsMsg;
        m_pCCdmaSmsMsg = NULL;
    }
}

void ProtocolCdmaNewSmsIndAdapterJson::SetCdmaSmsMsg() {
    if (!m_bParsingSuccess) return;
    unsigned int nMsgLen = m_jsonData[ns_sms_cdma_new_sms_ind::key_msg_data].size();

    ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SMS, __CLASS_FUNC__, nMsgLen, MAX_CDMA_SMS_MSG_SIZE);

    BYTE cdmaMsg[nMsgLen];
    for (unsigned int i = 0; i < nMsgLen; i++) {
        cdmaMsg[i] = (BYTE) m_jsonData[ns_sms_cdma_new_sms_ind::key_msg_data][i].asInt();
    }
    m_pCCdmaSmsMsg = new CCdmaSmsMessage(cdmaMsg, nMsgLen);
}

const RIL_CDMA_SMS_Message* ProtocolCdmaNewSmsIndAdapterJson::GetRilCdmaSmsMsg() const {
    if (m_pCCdmaSmsMsg) {
        return m_pCCdmaSmsMsg->GetRilCdmaSmsMsg();
    }
    return NULL;
}

int ProtocolCdmaNewSmsIndAdapterJson::GetTpid() const {
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_sms_cdma_new_sms_ind::key_msg_tpid].asInt();
}

/**
 * ProtocolWriteSmsToSimRespAdapterJson
 */
ProtocolWriteSmsToSimRespAdapterJson::ProtocolWriteSmsToSimRespAdapterJson(const ModemData *pModemData) :
    ProtocolWriteSmsToSimRespAdapterInterface(), ProtocolJsonRespAdapter(pModemData)
{
    if (ProtocolJsonRespAdapter::GetErrorCode() == RIL_E_SUCCESS) {
        ParseJsonPayload(SIT_WRITE_SMS_TO_SIM);
    }
}

int ProtocolWriteSmsToSimRespAdapterJson::GetIndex() const {
    if (m_bParsingSuccess) return m_jsonData[ns_sms_write_sms_to_sim_rsp::key_index].asInt();
    return -1;
}

/**
 * ProtocolGetBcstSmsConfRespAdapterJson
 */
ProtocolGetBcstSmsConfRespAdapterJson::ProtocolGetBcstSmsConfRespAdapterJson(const ModemData *pModemData)
        : ProtocolGetBcstSmsConfRespAdapterInterface(), ProtocolJsonRespAdapter(pModemData)
{
    m_pCBcsc = NULL;
    ParseJsonPayload(SIT_GET_BCST_SMS_CFG);
    SetBcstConfInfoList();
}

ProtocolGetBcstSmsConfRespAdapterJson::~ProtocolGetBcstSmsConfRespAdapterJson() {
    if (m_pCBcsc) {
        delete m_pCBcsc;
        m_pCBcsc = NULL;
    }
}

void ProtocolGetBcstSmsConfRespAdapterJson::SetBcstConfInfoList() {
    if (!m_bParsingSuccess) {
        return;
    }
    unsigned int nBcstInfoNum = m_jsonData[ns_sms_get_bcst_sms_cfg_rsp::key_bcst_conf_info_list].size();
    if (nBcstInfoNum == 0) {
        RilLogW("ProtocolGetBcstSmsConfRespAdapterJson::%s() Number of BCST is 0.", __FUNCTION__);
    } else {
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SMS, __CLASS_FUNC__, nBcstInfoNum, MAX_BCST_INFO_NUM);

        // store bcst_info_list
        // see ns_sms_bcst_conf_info
        Json::Value jsonRspBcstInfoList;
        jsonRspBcstInfoList = m_jsonData[ns_sms_get_bcst_sms_cfg_rsp::key_bcst_conf_info_list];
        RIL_GSM_BroadcastSmsConfigInfo *rgbsci;
        rgbsci = new RIL_GSM_BroadcastSmsConfigInfo[nBcstInfoNum];
        for (unsigned int i = 0; i < nBcstInfoNum; i++) {
            rgbsci[i].fromServiceId = jsonRspBcstInfoList[i][ns_sms_bcst_conf_info::key_from_svc_id].asInt();
            rgbsci[i].toServiceId = jsonRspBcstInfoList[i][ns_sms_bcst_conf_info::key_to_svc_id].asInt();
            rgbsci[i].fromCodeScheme = jsonRspBcstInfoList[i][ns_sms_bcst_conf_info::key_from_code_scheme].asInt();
            rgbsci[i].toCodeScheme = jsonRspBcstInfoList[i][ns_sms_bcst_conf_info::key_to_code_scheme].asInt();
            rgbsci[i].selected = jsonRspBcstInfoList[i][ns_sms_bcst_conf_info::key_selected].asInt();
        }
        m_pCBcsc = new BroadcastSmsConfigs(rgbsci, nBcstInfoNum);
        delete[] rgbsci;
    }
}

UINT8 ProtocolGetBcstSmsConfRespAdapterJson::GetConfigsNumber() {
    if (m_pCBcsc) {
        return m_pCBcsc->GetConfigsNumber();
    }
    return 0;
}

RIL_GSM_BroadcastSmsConfigInfo** ProtocolGetBcstSmsConfRespAdapterJson::GetConfigsInfoPointers() {
    if (m_pCBcsc) {
        return m_pCBcsc->GetConfigsInfoPointers();
    }
    return NULL;
}

/**
 * ProtocolSmsCapacityOnSimRespAdapterJson
 */
int ProtocolSmsCapacityOnSimRespAdapterJson::GetSimId() const {
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_sms_get_stored_sms_count_rsp::key_sim_type].asInt();
}

int ProtocolSmsCapacityOnSimRespAdapterJson::GetTotalNum() const {
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_sms_get_stored_sms_count_rsp::key_total_num].asInt();
}

int ProtocolSmsCapacityOnSimRespAdapterJson::GetUsedNum() const {
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_sms_get_stored_sms_count_rsp::key_used_num].asInt();
}

/**
 * ProtocolSmscAddrRespAdapterJson
 */
ProtocolSmscAddrRespAdapterJson::ProtocolSmscAddrRespAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolSmscAddrRespAdapterJson::Init() {
    m_nSmscLen = 0;
    memset(m_smsc, 0x0, sizeof(m_smsc));

    if (ProtocolJsonRespAdapter::GetErrorCode() == RIL_E_SUCCESS && GetParameterLength() > 0) {
        ParseJsonPayload(SIT_GET_SMSC_ADDR);
        SetSmsc();
    }
}

void ProtocolSmscAddrRespAdapterJson::SetSmsc() {
    if (!m_bParsingSuccess) {
        return;
    }

    m_nSmscLen = m_jsonData[ns_sms_get_smsc_addr_rsp::key_service_center_addr].size();

    ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SMS, __CLASS_FUNC__, m_nSmscLen, MAX_GSM_SMS_SERVICE_CENTER_ADDR);

    for (unsigned int i = 0; i < m_nSmscLen; i++) {
        m_smsc[i] = m_jsonData[ns_sms_get_smsc_addr_rsp::key_service_center_addr][i].asInt();
    }
}

/**
 * ProtocolNewSmsIndAdapterJson
 */
ProtocolNewSmsIndAdapterJson::ProtocolNewSmsIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolNewSmsIndAdapterJson::Init() {
    m_nTpduLen = 0;
    memset(m_tpdu, 0, sizeof(m_tpdu));
    ParseJsonPayload(SIT_IND_NEW_SMS) || ParseJsonPayload(SIT_IND_NEW_SMS_STATUS_REPORT);
    SetPdu();
}

void ProtocolNewSmsIndAdapterJson::SetPdu() {
    if (!m_bParsingSuccess) return;
    unsigned int nTpduLen = m_jsonData[ns_sms_new_sms_ind::key_tpdu].size();

    if (nTpduLen == 0) {
        RilLogW("ProtocolNewSmsIndAdapterJson::%s() TPDU len is 0!!", __FUNCTION__);
        return;
    }

    ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SMS, __CLASS_FUNC__, nTpduLen, MAX_GSM_SMS_TPDU_SIZE);

    BYTE tmpTpdu[nTpduLen];
    for (unsigned int i = 0; i < nTpduLen; i++) {
        tmpTpdu[i] = m_jsonData[ns_sms_new_sms_ind::key_tpdu][i].asInt();
    }
    m_nTpduLen = ConvertToHexString(tmpTpdu, nTpduLen, m_tpdu, sizeof(m_tpdu));
}

const char *ProtocolNewSmsIndAdapterJson::GetPdu() const {
    if (m_nTpduLen == 0 || *m_tpdu == 0)  return NULL;
    return m_tpdu;
}

int ProtocolNewSmsIndAdapterJson::GetTpid() const {
    int nTpid = -1;
    if (m_bParsingSuccess) {
        nTpid = m_jsonData[ns_sms_new_sms_ind::key_msg_tpid].asInt();
    }
    return nTpid;
}

/**
 * ProtocolNewBcstSmsAdapterJson
 */
ProtocolNewBcstSmsAdapterJson::ProtocolNewBcstSmsAdapterJson(const ModemData *pModemData)
    : ProtocolNewBcstSmsAdapterInterface(), ProtocolJsonIndAdapter(pModemData, SIT_IND_NEW_BCST_SMS) {
    m_pBcstMsg = NULL;
    m_nBcstMsgLen = 0;
    SetBcst();
}

ProtocolNewBcstSmsAdapterJson::~ProtocolNewBcstSmsAdapterJson() {
    if (m_pBcstMsg) {
        delete[] m_pBcstMsg;
        m_pBcstMsg = NULL;
    }
}

void ProtocolNewBcstSmsAdapterJson::SetBcst() {
    if (!m_bParsingSuccess)  return;
    m_nBcstMsgLen = m_jsonData[ns_sms_new_bcst_sms_ind::key_bcst_msg].size();

    ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SMS, __CLASS_FUNC__, m_nBcstMsgLen, MAX_BCST_MSG_LEN);

    m_pBcstMsg = new BYTE[m_nBcstMsgLen];
    for (unsigned int i = 0; i < m_nBcstMsgLen; i++) {
        m_pBcstMsg[i] = (BYTE) m_jsonData[ns_sms_new_bcst_sms_ind::key_bcst_msg][i].asInt();
    }
}

/**
 * ProtocolCdmaSendSmsRespAdapterJson
 */
ProtocolCdmaSendSmsRespAdapterJson::ProtocolCdmaSendSmsRespAdapterJson(const ModemData *pModemData)
    : ProtocolCdmaSendSmsRespAdapterInterface(), ProtocolJsonRespAdapter(pModemData) {
    Init();
}

void ProtocolCdmaSendSmsRespAdapterJson::Init() {
    ParseJsonPayload(SIT_CDMA_SEND_SMS) || ParseJsonPayload(SIT_CDMA_SEND_SMS_EXPECT_MORE);
}

int ProtocolCdmaSendSmsRespAdapterJson::GetRef() {
    int nMsgRef = -1;
    if (m_bParsingSuccess) {
        nMsgRef = m_jsonData[ns_sms_cdma_send_sms_rsp::key_msg_ref].asInt();
    }
    return nMsgRef;
}

int ProtocolCdmaSendSmsRespAdapterJson::GetSmsRspErrorClass() {
    int nErrorClass = -1;
    if (m_bParsingSuccess) {
        nErrorClass = m_jsonData[ns_sms_cdma_send_sms_rsp::key_error_class].asInt();
    }
    return nErrorClass;
}

int ProtocolCdmaSendSmsRespAdapterJson::GetSmsRspCauseCode() {
    int nErrorCode = -1;
    if (m_bParsingSuccess) {
        nErrorCode = m_jsonData[ns_sms_cdma_send_sms_rsp::key_error_code].asInt();
    }
    return nErrorCode;
}

/**
 * ProtocolCdmaWriteSmsToRuimRespAdapterJson
 */
int ProtocolCdmaWriteSmsToRuimRespAdapterJson::GetIndex()
{
    if (!m_bParsingSuccess) return -1;
    return m_jsonData[ns_sms_cdma_write_sms_to_ruim_rsp::key_index].asInt();
}

/**
 * ProtocolGetCdmaBcstSmsConfRespAdapterJson
 */
ProtocolGetCdmaBcstSmsConfRespAdapterJson::ProtocolGetCdmaBcstSmsConfRespAdapterJson(const ModemData *pModemData) :
    ProtocolGetCdmaBcstSmsConfRespAdapterInterface(), ProtocolJsonRespAdapter(pModemData)
{
    m_pCCbcsc = NULL;
    ParseJsonPayload(SIT_CDMA_GET_BCST_SMS_CFG);
    SetBcstConfInfoList();
}

ProtocolGetCdmaBcstSmsConfRespAdapterJson::~ProtocolGetCdmaBcstSmsConfRespAdapterJson() {
    if (m_pCCbcsc) {
        delete m_pCCbcsc;
        m_pCCbcsc = NULL;
    }
}

void ProtocolGetCdmaBcstSmsConfRespAdapterJson::SetBcstConfInfoList() {
    if (!m_bParsingSuccess)  return;
    unsigned int nBcstInfoNum = m_jsonData[ns_sms_cdma_get_bcst_sms_cfg_rsp::key_cdma_bcst_conf_info_list].size();
    if (nBcstInfoNum == 0) {
        RilLogW("ProtocolGetCdmaBcstSmsConfRespAdapterJson::%s() Number of BCST is 0.", __FUNCTION__);
    } else {
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_SMS, __CLASS_FUNC__, nBcstInfoNum, MAX_CDMA_BCST_INFO_NUM);

        // store bcst_info_list
        // see ns_sms_cdma_bcst_conf_info
        Json::Value jsonRspBcstInfoList;
        jsonRspBcstInfoList = m_jsonData[ns_sms_cdma_get_bcst_sms_cfg_rsp::key_cdma_bcst_conf_info_list];
        RIL_CDMA_BroadcastSmsConfigInfo *rcbsci;
        rcbsci = new RIL_CDMA_BroadcastSmsConfigInfo[nBcstInfoNum];
        for (unsigned int i = 0; i < nBcstInfoNum; i++) {
            rcbsci[i].service_category = jsonRspBcstInfoList[i][ns_sms_cdma_bcst_conf_info::key_service_category].asInt();
            rcbsci[i].language = jsonRspBcstInfoList[i][ns_sms_cdma_bcst_conf_info::key_language].asInt();
            rcbsci[i].selected = true;
        }
        m_pCCbcsc = new CCdmaBroadcastSmsConfigs(rcbsci, nBcstInfoNum);
        delete[] rcbsci;
    }
}

UINT8 ProtocolGetCdmaBcstSmsConfRespAdapterJson::GetConfigsNumber() {
    if (m_pCCbcsc) {
        return m_pCCbcsc->GetConfigsNumber();
    } else {
        return 0;
    }
}

RIL_CDMA_BroadcastSmsConfigInfo** ProtocolGetCdmaBcstSmsConfRespAdapterJson::GetConfigsInfoPointers() {
    if (m_pCCbcsc) {
        return m_pCCbcsc->GetConfigsInfoPointers();
    }
    return NULL;
}

/**
 * ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson
 */
ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson::ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson(const ModemData *pModemData)
        : ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface(), ProtocolJsonIndAdapter(pModemData)
{
    m_ptRcsm = NULL;
    ParseJsonPayload(SIT_IND_CDMA_VOICE_MSG_WAITING_INFO);
    SetCdmaSmsMsg();
}

ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson::~ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson() {
    if (m_ptRcsm) {
        delete m_ptRcsm;
        m_ptRcsm = NULL;
    }
}

void ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson::SetCdmaSmsMsg() {
    if (!m_bParsingSuccess) return;
    // Create RIL_CDMA_SMS_Message for MWI. Message Waiting Info Record defined in 3GPP2 C.S-0005, 3.7.5.6
    // It contains only an 8-bit number with the number of messages waiting.
    m_ptRcsm = new RIL_CDMA_SMS_Message;
    memset(m_ptRcsm, 0, sizeof(RIL_CDMA_SMS_Message));

    m_ptRcsm->uTeleserviceID = TELESERVICE_IDENTIFIER_MWI;
    m_ptRcsm->uBearerDataLen = 1;
    m_ptRcsm->aBearerData[0] = m_jsonData[ns_sms_cdma_voice_msg_waiting_info_ind::key_msg_count].asInt();
}
