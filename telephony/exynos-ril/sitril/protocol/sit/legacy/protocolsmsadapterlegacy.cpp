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
 * protocolsmsadapterlegacy.cpp
 *
 *  Created on: 2014. 7. 3.
 *      Author: sungwoo48.choi
 */

#include "protocolsmsadapterlegacy.h"
#include "rillog.h"
#include "util.h"
#include "sitdef.h"
#include "modemdata.h"
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
 * ProtocolSendSmsRespAdapterLegacy
 */

int ProtocolSendSmsRespAdapterLegacy::GetRef() const
{
    if (m_pModemData != NULL) {
        sit_sms_send_sms_rsp *data = (sit_sms_send_sms_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_SEND_SMS || data->hdr.id == SIT_SEND_SMS_EXPECT_MORE)) {
            return (data->msg_ref & 0xFF);
        }
    }
    return -1;
}
int ProtocolSendSmsRespAdapterLegacy::GetSmsRspErrorCode() const
{
    if (m_pModemData != NULL) {
        sit_sms_send_sms_rsp *data = (sit_sms_send_sms_rsp *) m_pModemData->GetRawData();

        if (data != NULL && (data->hdr.id == SIT_SEND_SMS || data->hdr.id == SIT_SEND_SMS_EXPECT_MORE)) {
            return data->error_code;
        }
    }

    return -1;
}
int ProtocolSendSmsRespAdapterLegacy::GetPduSize() const
{
    if (m_pModemData != NULL) {
        sit_sms_send_sms_rsp *data = (sit_sms_send_sms_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_SEND_SMS || data->hdr.id == SIT_SEND_SMS_EXPECT_MORE)) {
            return (data->ack_pdu_len & 0xFF);
        }
    }
    return 0;
}
const char *ProtocolSendSmsRespAdapterLegacy::GetPdu() const
{
    char *pdu = NULL;
    if (m_pModemData != NULL) {
        sit_sms_send_sms_rsp *data = (sit_sms_send_sms_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_SEND_SMS || data->hdr.id == SIT_SEND_SMS_EXPECT_MORE)) {
            pdu = (char *)data->ack_pdu;
            if (data->ack_pdu_len == 0 || *data->ack_pdu == 0) {
                pdu = NULL;
            }
        }
    }
    return pdu;
}

int ProtocolWriteSmsToSimRespAdapterLegacy::GetIndex() const
{
    if (m_pModemData != NULL) {
        sit_sms_write_sms_to_sim_rsp *data = (sit_sms_write_sms_to_sim_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_WRITE_SMS_TO_SIM)) {
            return (data->index);
        }
    }
    return -1;
}

BYTE* ProtocolNewBcstSmsAdapterLegacy::GetBcst() const
{
    BYTE *msg = NULL;

    if (m_pModemData != NULL) {
        sit_sms_new_bcst_sms_ind *data = (sit_sms_new_bcst_sms_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_NEW_BCST_SMS)) {
            msg = data->bcst_msg;
            if (data->bcst_msg_len == 0) {
                msg = NULL;
            }
        }
    }
    return msg;
}

UINT16 ProtocolNewBcstSmsAdapterLegacy::GetBcstLen() const {
    if (m_pModemData != NULL) {
        sit_sms_new_bcst_sms_ind *data = (sit_sms_new_bcst_sms_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_NEW_BCST_SMS)) {
            return (data->bcst_msg_len);
        }
    }
    return 0;
}

/**
 * ProtocolSmscAddrRespAdapterLegacy
 */
ProtocolSmscAddrRespAdapterLegacy::ProtocolSmscAddrRespAdapterLegacy(const ModemData *pModemData)
    : ProtocolSmscAddrRespAdapterInterface(), ProtocolRespAdapter(pModemData)
{
    m_nSmscLen = 0;
    memset(m_smsc, 0x0, sizeof(m_smsc));

    Init();
}

void ProtocolSmscAddrRespAdapterLegacy::Init()
{
    if (m_pModemData != NULL) {
        sit_sms_get_smsc_addr_rsp *data = (sit_sms_get_smsc_addr_rsp *) m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_SMSC_ADDR && data->hdr.ext.rsp.error == RIL_E_SUCCESS && data->hdr.length > sizeof(RCM_HEADER))
        {
            m_nSmscLen = (int)(data->sca_len & 0xFF);
            if (m_nSmscLen > MAX_GSM_SMS_SERVICE_CENTER_ADDR) {
                m_nSmscLen = MAX_GSM_SMS_SERVICE_CENTER_ADDR;
            }
            memcpy(m_smsc, data->sca, m_nSmscLen);
        }
    }
}

const char *ProtocolSmscAddrRespAdapterLegacy::GetSmsc() const
{
    if (m_nSmscLen == 0) {
        return NULL;
    }
    return m_smsc;
}

int ProtocolSmscAddrRespAdapterLegacy::GetSmscLength() const
{
    return m_nSmscLen;
}

/**
 * ProtocolNewSmsIndAdapterLegacy
 */
ProtocolNewSmsIndAdapterLegacy::ProtocolNewSmsIndAdapterLegacy(const ModemData *pModemData)
    : ProtocolNewSmsIndAdapterInterface(), ProtocolIndAdapter(pModemData)
{
    Init();

    if (m_pModemData != NULL) {
        sit_sms_new_sms_ind *data = (sit_sms_new_sms_ind *) m_pModemData->GetRawData();
        if (data != NULL && ((data->hdr.id == SIT_IND_NEW_SMS) || (data->hdr.id == SIT_IND_NEW_SMS_STATUS_REPORT))) {
            m_len = ConvertToHexString(data->tpdu, data->tpdu_len, m_tpdu, sizeof(m_tpdu));
            if (m_len == 0) {
                RilLogW("TPDU_LEN is 0 or over MAX_GSM_SMS_TPDU_SIZE");
            }
            m_tpid = data->msg_tpid & 0xFF;
        }
    }
}

void ProtocolNewSmsIndAdapterLegacy::Init()
{
    m_len = 0;
    m_tpid = -1;
    memset(m_tpdu, 0, sizeof(m_tpdu));
}

const char *ProtocolNewSmsIndAdapterLegacy::GetPdu() const
{
    if (m_len == 0 || *m_tpdu == 0)
        return NULL;
    return m_tpdu;
}

/**
 * ProtocolNewSmsOnSimIndAdapterLegacy
 */
int ProtocolNewSmsOnSimIndAdapterLegacy::GetIndex() const {
    if (m_pModemData != NULL) {
        sit_sms_new_sms_on_sim_ind *data = (sit_sms_new_sms_on_sim_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_NEW_SMS_ON_SIM)) {
            return (data->index);
        }
    }
    return -1;
}

/**
 * ProtocolGetBcstSmsConfRespAdapterLegacy
 */
ProtocolGetBcstSmsConfRespAdapterLegacy::ProtocolGetBcstSmsConfRespAdapterLegacy(const ModemData *pModemData)
    : ProtocolGetBcstSmsConfRespAdapterInterface(), ProtocolRespAdapter(pModemData), m_pCBcsc(NULL)
{
    if (m_pModemData != NULL) {
        sit_sms_get_bcst_sms_cfg_rsp *data =
            (sit_sms_get_bcst_sms_cfg_rsp *) m_pModemData->GetRawData();
        if (data != NULL  && (data->hdr.id == SIT_GET_BCST_SMS_CFG)) {
            int num = data->bcst_info_num;
            if (num == 0) {
                RilLogW("Number of BCST is 0.");
            } else {
                if (num > MAX_BCST_INFO_NUM) {
                    RilLogW("Number of BCST(%d) is larger than MAX_BCST_INFO_NUM(%d).",\
                            num , MAX_BCST_INFO_NUM);
                    num = MAX_BCST_INFO_NUM;
                }
                RIL_GSM_BroadcastSmsConfigInfo *rgbsci;
                rgbsci = new RIL_GSM_BroadcastSmsConfigInfo[num];
                for (int i = 0; i < num; i++) {
                    rgbsci[i].fromServiceId = data->cfgitem[i].from_svc_id;
                    rgbsci[i].toServiceId = data->cfgitem[i].to_svc_id;
                    rgbsci[i].fromCodeScheme = data->cfgitem[i].from_code_scheme;
                    rgbsci[i].toCodeScheme = data->cfgitem[i].to_code_scheme;
                    rgbsci[i].selected = data->cfgitem[i].selected;
                }
                m_pCBcsc = new BroadcastSmsConfigs(rgbsci, num);
                delete[] rgbsci;
            }
        }
    }
}

ProtocolGetBcstSmsConfRespAdapterLegacy::~ProtocolGetBcstSmsConfRespAdapterLegacy()
{
    if (m_pCBcsc) {
        delete m_pCBcsc;
    }
}

UINT8 ProtocolGetBcstSmsConfRespAdapterLegacy::GetConfigsNumber()
{
    if (m_pCBcsc) {
        return m_pCBcsc->GetConfigsNumber();
    } else {
        return 0;
    }
}

RIL_GSM_BroadcastSmsConfigInfo** ProtocolGetBcstSmsConfRespAdapterLegacy::GetConfigsInfoPointers()
{
    if (m_pCBcsc) {
        return m_pCBcsc->GetConfigsInfoPointers();
    } else {
        return NULL;
    }
}

int ProtocolSmsCapacityOnSimRespAdapterLegacy::GetSimId() const
{
    if (m_pModemData != NULL) {
        sit_sms_get_stored_sms_count_rsp *data = (sit_sms_get_stored_sms_count_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_GET_STORED_SMS_COUNT)) {
            return (data->sim_id);
        }
    }
    // return invalid sim id
    return -1;
}

int ProtocolSmsCapacityOnSimRespAdapterLegacy::GetTotalNum() const
{
    if (m_pModemData != NULL) {
        sit_sms_get_stored_sms_count_rsp *data = (sit_sms_get_stored_sms_count_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_GET_STORED_SMS_COUNT)) {
            return (data->total_num);
        }
    }
    return 0;
}

int ProtocolSmsCapacityOnSimRespAdapterLegacy::GetUsedNum() const
{
    if (m_pModemData != NULL) {
        sit_sms_get_stored_sms_count_rsp *data = (sit_sms_get_stored_sms_count_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_GET_STORED_SMS_COUNT)) {
            return (data->used_num);
        }
    }
    return 0;
}

/**
 * ProtocolCdmaSendSmsRespAdapterLegacy
 */
int ProtocolCdmaSendSmsRespAdapterLegacy::GetRef()
{
    if (m_pModemData != NULL) {
        sit_sms_cdma_send_sms_rsp *data = (sit_sms_cdma_send_sms_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_CDMA_SEND_SMS || data->hdr.id == SIT_CDMA_SEND_SMS_EXPECT_MORE)) {
            return data->msg_ref;
        }
    }
    return -1;
}
int ProtocolCdmaSendSmsRespAdapterLegacy::GetSmsRspErrorClass()
{
    if (m_pModemData != NULL) {
        sit_sms_cdma_send_sms_rsp *data = (sit_sms_cdma_send_sms_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_CDMA_SEND_SMS || data->hdr.id == SIT_CDMA_SEND_SMS_EXPECT_MORE)) {
            return data->error_class;
        }
    }
    return -1;
}
int ProtocolCdmaSendSmsRespAdapterLegacy::GetSmsRspCauseCode()
{
    if (m_pModemData != NULL) {
        sit_sms_cdma_send_sms_rsp *data = (sit_sms_cdma_send_sms_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_CDMA_SEND_SMS || data->hdr.id == SIT_CDMA_SEND_SMS_EXPECT_MORE)) {
            return data->error_code;
        }
    }
    return -1;
}

/**
 * ProtocolCdmaNewSmsIndAdapterLegacy
 */
ProtocolCdmaNewSmsIndAdapterLegacy::ProtocolCdmaNewSmsIndAdapterLegacy(const ModemData *pModemData)
    : ProtocolCdmaNewSmsIndAdapterInterface(), ProtocolIndAdapter(pModemData), m_pCCsm(NULL), m_nTpId(-1)
{
    if (m_pModemData != NULL) {
        sit_sms_cdma_new_sms_ind *data = (sit_sms_cdma_new_sms_ind *) m_pModemData->GetRawData();
        if (data != NULL && data->msg_len > 0 && (data->hdr.id == SIT_IND_CDMA_NEW_SMS)) {
            m_pCCsm = new CCdmaSmsMessage((BYTE *)data->msg, data->msg_len);
            m_nTpId = data->msg_tpid;
        }
    }
}

ProtocolCdmaNewSmsIndAdapterLegacy::~ProtocolCdmaNewSmsIndAdapterLegacy()
{
    if (m_pCCsm) {
        delete m_pCCsm;
    }
}

const RIL_CDMA_SMS_Message* ProtocolCdmaNewSmsIndAdapterLegacy::GetRilCdmaSmsMsg() const
{
    if (m_pCCsm) {
        return m_pCCsm->GetRilCdmaSmsMsg();
    } else {
        return NULL;
    }
}

/**
 * ProtocolCdmaWriteSmsToRuimRespAdapterLegacy
 */
int ProtocolCdmaWriteSmsToRuimRespAdapterLegacy::GetIndex()
{
    if (m_pModemData != NULL) {
        sit_sms_cdma_write_sms_to_ruim_rsp *data =
            (sit_sms_cdma_write_sms_to_ruim_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_CDMA_WRITE_SMS_TO_RUIM)) {
            return data->index;
        }
    }
    return -1;
}

/**
 * ProtocolGetCdmaBcstSmsConfRespAdapterLegacy
 */
ProtocolGetCdmaBcstSmsConfRespAdapterLegacy::ProtocolGetCdmaBcstSmsConfRespAdapterLegacy(const ModemData *pModemData)
    : ProtocolGetCdmaBcstSmsConfRespAdapterInterface(), ProtocolRespAdapter(pModemData), m_pCCbcsc(NULL)
{
    if (m_pModemData != NULL) {
        sit_sms_cdma_get_bcst_sms_cfg_rsp *data =
            (sit_sms_cdma_get_bcst_sms_cfg_rsp *) m_pModemData->GetRawData();
        if (data != NULL  && (data->hdr.id == SIT_CDMA_GET_BCST_SMS_CFG)) {
            int num = data->bcst_info_num;
            if (num == 0) {
                RilLogW("Number of BCST is 0.");
            } else {
                if (num > MAX_CDMA_BCST_INFO_NUM) {
                    RilLogW("Number of BCST(%d) is larger than MAX_CDMA_BCST_INFO_NUM(%d).",\
                            num, MAX_CDMA_BCST_INFO_NUM);
                    num = MAX_CDMA_BCST_INFO_NUM;
                }
                RIL_CDMA_BroadcastSmsConfigInfo *rcbsci;
                rcbsci = new RIL_CDMA_BroadcastSmsConfigInfo[num];
                for (int i = 0; i < num; i++) {
                    rcbsci[i].service_category = data->cfgitem[i].svc_category;
                    rcbsci[i].language = data->cfgitem[i].language;
                    rcbsci[i].selected = true;
                }
                m_pCCbcsc = new CCdmaBroadcastSmsConfigs(rcbsci, num);
                delete[] rcbsci;
            }
        }
    }
}

ProtocolGetCdmaBcstSmsConfRespAdapterLegacy::~ProtocolGetCdmaBcstSmsConfRespAdapterLegacy()
{
    if (m_pCCbcsc) {
        delete m_pCCbcsc;
    }
}

UINT8 ProtocolGetCdmaBcstSmsConfRespAdapterLegacy::GetConfigsNumber()
{
    if (m_pCCbcsc) {
        return m_pCCbcsc->GetConfigsNumber();
    } else {
        return 0;
    }
}

RIL_CDMA_BroadcastSmsConfigInfo** ProtocolGetCdmaBcstSmsConfRespAdapterLegacy::GetConfigsInfoPointers()
{
    if (m_pCCbcsc) {
        return m_pCCbcsc->GetConfigsInfoPointers();
    } else {
        return NULL;
    }
}

/**
 * ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy
 */
ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy::ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy(const ModemData *pModemData)
    : ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface(), ProtocolIndAdapter(pModemData), m_ptRcsm(NULL)
{
    if (m_pModemData != NULL) {
        sit_sms_cdma_voice_msg_waiting_info_ind *data =
                (sit_sms_cdma_voice_msg_waiting_info_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_CDMA_VOICE_MSG_WAITING_INFO)) {
            // Create RIL_CDMA_SMS_Message for MWI. Message Waiting Info Record defined in 3GPP2 C.S-0005, 3.7.5.6
            // It contains only an 8-bit number with the number of messages waiting.
            m_ptRcsm = new RIL_CDMA_SMS_Message;
            memset(m_ptRcsm, 0, sizeof(RIL_CDMA_SMS_Message));

            m_ptRcsm->uTeleserviceID = TELESERVICE_IDENTIFIER_MWI;
            m_ptRcsm->uBearerDataLen = 1;
            m_ptRcsm->aBearerData[0] = data->msg_count;
        }
    }
}

ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy::~ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy()
{
    if (m_ptRcsm) {
        delete m_ptRcsm;
    }
}
