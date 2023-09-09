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
 * protocolsmsbuilderjson.cpp
 *
 *  Created on: 2021. 8. 27.
 *      Author: aryaman.2019
 */
#include "protocolsmsbuilderjson.h"
#include "protocolutils.h"
#include "smsjsondef.h"
#include "rillog.h"

#include <librilutils/systemproperty.h>

#define PROPERTY_SMS_DOMAIN   "vendor.radio.smsdomain"

#define CS_PREF 0
#define PS_PREF 1
#define CS_ONLY 2
#define PS_ONLY 3

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

ModemData *ProtocolSmsBuilderJson::BuildSendSms(const char *smsc, int smscLen, const char *pdu, int pduSize, bool bExpectMore) {
    if (pdu == NULL || *pdu == 0) {
        RilLogE("Wrong pdu");
        return NULL;
    }

    if (pduSize <= 0 || pduSize > MAX_GSM_SMS_TPDU_SIZE) {
        RilLogE("pduSize = %d, MAX_TPDU_SIZE = %d", pduSize, MAX_GSM_SMS_TPDU_SIZE);
        return NULL;
    }

    Json::Value jsonReqData;
    unsigned int domain = stoi(SystemProperty::Get(PROPERTY_SMS_DOMAIN, "0" /* default : CS pref */));
    jsonReqData[ns_sms_send_sms_req::key_sms_domain] = domain;
    if (smscLen <= 0 || smscLen > MAX_GSM_SMS_SERVICE_CENTER_ADDR || smsc == NULL) {
        // SMSC is not valid, we should set SMSC value as 0x00 and SMSC length will be 1.
        jsonReqData[ns_sms_send_sms_req::key_smsc].append(0);
        RilLogI("SMSC is not valid. Sending SMSC with value 0x00");
    } else {
        for (int i = 0; i < smscLen; i++) {
            jsonReqData[ns_sms_send_sms_req::key_smsc].append((int) smsc[i]);
        }
    }

    for (int i = 0; i < pduSize; i++) {
        jsonReqData[ns_sms_send_sms_req::key_sms_data].append((int) pdu[i]);
    }
    return GetJsonReqModemData(jsonReqData, bExpectMore ? SIT_SEND_SMS_EXPECT_MORE : SIT_SEND_SMS);
}

ModemData *ProtocolSmsBuilderJson::BuildSmsAck(int result, int tpid, int error) {
    if (result < 0 || tpid < 0 )  return NULL;

    Json::Value jsonReqData;
    jsonReqData[ns_sms_send_sms_ack_req::key_result] = ((result & 0xFF) == 0x00 ? 0x00 : 0x01);
    jsonReqData[ns_sms_send_sms_ack_req::key_msg_tpid] = (tpid & 0xFF);
    jsonReqData[ns_sms_send_sms_ack_req::key_error_code] = error;

    return GetJsonReqModemData(jsonReqData, SIT_SEND_SMS_ACK);
}

ModemData *ProtocolSmsBuilderJson::BuildSmsAck(int result, int tpid, const char *pdu, int pduSize) {
    if (result < 0 || tpid < 0)  return NULL;

    ProtocolUtils::LimitLengthToMax(RIL_LOG_CAT_SMS, __CLASS_FUNC__, pduSize, MAX_GSM_SMS_TPDU_SIZE);

    Json::Value jsonReqData;
    jsonReqData[ns_sms_send_ack_incoming_sms_req::key_result] = ((result & 0xFF) == 0x00 ? 0x00 : 0x01);
    jsonReqData[ns_sms_send_ack_incoming_sms_req::key_msg_tpid] = (tpid & 0xFF);
    jsonReqData[ns_sms_send_ack_incoming_sms_req::key_tpdu] = Json::arrayValue;
    for (int i = 0; i < pduSize; i++) {
        jsonReqData[ns_sms_send_ack_incoming_sms_req::key_tpdu].append(pdu[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SEND_ACK_INCOMING_SMS);
}

ModemData *ProtocolSmsBuilderJson::BuildSmscAddress() {
    return GetJsonNullReqModemData(SIT_GET_SMSC_ADDR);
}

ModemData *ProtocolSmsBuilderJson::BuildSmscAddress(int sca_len, const char *sca) {
    if (sca_len > MAX_GSM_SMS_SERVICE_CENTER_ADDR)  sca_len = MAX_GSM_SMS_SERVICE_CENTER_ADDR;

    Json::Value jsonReqData;
    jsonReqData[ns_sms_set_smsc_addr_req::key_service_center_addr] = Json::arrayValue;
    for (int i = 0; i < sca_len; i++) {
        jsonReqData[ns_sms_set_smsc_addr_req::key_service_center_addr].append(sca[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SET_SMSC_ADDR);
}

ModemData *ProtocolSmsBuilderJson::BuildSmsMemoryStatus(int status) {
    Json::Value jsonReqData;

    if (status == 0x00) {
        jsonReqData[ns_sms_send_sms_mem_status_req::key_mem_status] = MEMORY_CAPACITY_EXCEEDED;
    } else {
        jsonReqData[ns_sms_send_sms_mem_status_req::key_mem_status] = MEMORY_AVAILABLE;
    }

    return GetJsonReqModemData(jsonReqData, SIT_SEND_SMS_MEM_STATUS);
}

ModemData *ProtocolSmsBuilderJson::BuildWriteSmsToSim(int status, int index, int pduSize, const char *pdu) {
    if (pduSize <= 0 || pduSize > MAX_GSM_SMS_TPDU_SIZE || pdu == NULL)  return NULL;

    Json::Value jsonReqData;
    switch (status) {
        case 0x0:
            jsonReqData[ns_sms_write_sms_to_sim_req::key_msg_status] = SIT_SIM_STATUS_RECEIVED_UNREAD;
            break;
        case 0x1:
            jsonReqData[ns_sms_write_sms_to_sim_req::key_msg_status] = SIT_SIM_STATUS_RECEIVED_READ;
            break;
        case 0x2:
            jsonReqData[ns_sms_write_sms_to_sim_req::key_msg_status] = SIT_SIM_STATUS_STORED_UNSENT;
            break;
        case 0x3:
            jsonReqData[ns_sms_write_sms_to_sim_req::key_msg_status] = SIT_SIM_STATUS_STORED_SENT;
            break;
        default:
            RilLogE("Staus is not a proper value.");
            return NULL;
    }
    jsonReqData[ns_sms_write_sms_to_sim_req::key_index] = index; // 0xFFFF: Default value all the time, expect for a test
    for (int i = 0; i < pduSize; i++) {
        jsonReqData[ns_sms_write_sms_to_sim_req::key_pdu_data].append(pdu[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_WRITE_SMS_TO_SIM);
}

ModemData *ProtocolSmsBuilderJson::BuildDeleteSmsOnSim(int index) {
    Json::Value jsonReqData;
    jsonReqData[ns_sms_delete_sms_on_sim_req::key_index] = index;

    return GetJsonReqModemData(jsonReqData, SIT_DELETE_SMS_ON_SIM);
}

ModemData *ProtocolSmsBuilderJson::BuildGetBroadcastSmsConfig() {
    return GetJsonNullReqModemData(SIT_GET_BCST_SMS_CFG);
}

ModemData *ProtocolSmsBuilderJson::BuildSetBroadcastSmsConfig(
   const RIL_GSM_BroadcastSmsConfigInfo *rgbsci, int num) {
    if (rgbsci == NULL || num <= 0) {
        RilLogE("ProtocolSmsBuilderJson::%s() Wrong Broadcast SMS Configs Info!!!", __FUNCTION__);
        return NULL;
    }

    ProtocolUtils::LimitLengthToMax(RIL_LOG_CAT_SMS, __CLASS_FUNC__, num, MAX_BCST_INFO_NUM);

    Json::Value jsonReqData;
    // store bcst_info_list
    // see ns_sms_bcst_conf_info
    Json::Value jsonBcstInfoList;

    for (int i = 0; i < num; i++) {
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_from_svc_id] = rgbsci[i].fromServiceId;
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_to_svc_id] = rgbsci[i].toServiceId;
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_from_code_scheme] = rgbsci[i].fromCodeScheme;
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_to_code_scheme] = rgbsci[i].toCodeScheme;
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_selected] = rgbsci[i].selected;
    }
    jsonReqData[ns_sms_set_bcst_sms_cfg_req::key_bcst_conf_info_list] = jsonBcstInfoList;

    return GetJsonReqModemData(jsonReqData, SIT_SET_BCST_SMS_CFG);
}

ModemData *ProtocolSmsBuilderJson::BuildSmsBroadcastActivation(int bcst_act) {
    Json::Value jsonReqData;
    jsonReqData[ns_sms_act_bcst_sms_req::key_bcst_act] = bcst_act;

    return GetJsonReqModemData(jsonReqData, SIT_ACT_BCST_SMS);
}

ModemData *ProtocolSmsBuilderJson::BuildGetStoredSmsCount(int sim_id) {
    Json::Value jsonReqData;

    if (sim_id == 0x00) {
        jsonReqData[ns_sms_get_stored_sms_count_req::key_sim_type] = STORED_SIM;
    } else {
        jsonReqData[ns_sms_get_stored_sms_count_req::key_sim_type] = STORED_RUIM;
    }

    return GetJsonReqModemData(jsonReqData, SIT_GET_STORED_SMS_COUNT);
}

// need to implement bExpectMore feature.
ModemData *ProtocolSmsBuilderJson::BuildSendCdmaSms(const char *msg, int msgLen, bool bExpectMore) {
    if (msg == NULL || msgLen <= 0) {
        RilLogE("Wrong message data!!!");
        return NULL;
    }

    if (msgLen > MAX_CDMA_SMS_MSG_SIZE) {
        RilLogE("Message length = %d, MAX_CDMA_SMS_MSG_SIZE = %d", msgLen, MAX_CDMA_SMS_MSG_SIZE);
        return NULL;
    }

    Json::Value jsonReqData;
    for (int i = 0; i < msgLen; i++) {
        jsonReqData[ns_sms_cdma_send_sms_req::key_msg_data].append(msg[i]);
    }

    return GetJsonReqModemData(jsonReqData, bExpectMore ? SIT_CDMA_SEND_SMS_EXPECT_MORE : SIT_CDMA_SEND_SMS);
}

ModemData *ProtocolSmsBuilderJson::BuildSendCdmaSmsAck(int tpid, int errClass, int errCode) {
    Json::Value jsonReqData;
    jsonReqData[ns_sms_cdma_send_sms_ack_req::key_msg_tpid] = tpid;
    jsonReqData[ns_sms_cdma_send_sms_ack_req::key_error_class] = errClass;
    jsonReqData[ns_sms_cdma_send_sms_ack_req::key_error_code] = errCode;

    return GetJsonReqModemData(jsonReqData, SIT_CDMA_SEND_SMS_ACK);
}

ModemData *ProtocolSmsBuilderJson::BuildWriteCdmaSmsToRuim(int status, const char *msg, int msgLen) {
    if (msg == NULL || msgLen <= 0) {
        RilLogE("Wrong message data!!!");
        return NULL;
    }

    if (msgLen > MAX_CDMA_SMS_RUIM_MSG_SIZE) {
        RilLogE("Message length = %d, MAX_CDMA_SMS_RUIM_MSG_SIZE = %d", msgLen,
                MAX_CDMA_SMS_RUIM_MSG_SIZE);
        return NULL;
    }

    Json::Value jsonReqData;
    switch(status) {
        case RIL_RUIM_STATUS_RECEIVED_UNREAD:
            jsonReqData[ns_sms_cdma_write_sms_to_ruim_req::key_msg_status] = SIT_RUIM_STATUS_RECEIVED_UNREAD;
            break;
        case RIL_RUIM_STATUS_RECEIVED_READ:
            jsonReqData[ns_sms_cdma_write_sms_to_ruim_req::key_msg_status] = SIT_RUIM_STATUS_RECEIVED_READ;
            break;
        case RIL_RUIM_STATUS_STORED_UNSENT:
            jsonReqData[ns_sms_cdma_write_sms_to_ruim_req::key_msg_status] = SIT_RUIM_STATUS_STORED_UNSENT;
            break;
        case RIL_RUIM_STATUS_STORED_SENT:
            jsonReqData[ns_sms_cdma_write_sms_to_ruim_req::key_msg_status] = SIT_RUIM_STATUS_STORED_SENT;
            break;
        default:
            jsonReqData[ns_sms_cdma_write_sms_to_ruim_req::key_msg_status] = SIT_RUIM_STATUS_RECEIVED_UNREAD;
            RilLogE("SMS status(%d) is wrong!!! : set default(unread) value", status);
            break;
    }
    for (int i = 0; i < msgLen; i++) {
        jsonReqData[ns_sms_cdma_write_sms_to_ruim_req::key_msg_data].append(msg[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_CDMA_WRITE_SMS_TO_RUIM);
}

ModemData *ProtocolSmsBuilderJson::BuildDeleteCdmaSmsOnRuim(int index) {
    Json::Value jsonReqData;
    jsonReqData[ns_sms_cdma_delete_sms_on_ruim_req::key_index] = index;

    return GetJsonReqModemData(jsonReqData, SIT_CDMA_DELETE_SMS_ON_RUIM);
}

ModemData *ProtocolSmsBuilderJson::BuildGetCdmaBroadcastSmsConfig() {
    return GetJsonNullReqModemData(SIT_CDMA_GET_BCST_SMS_CFG);
}

ModemData *ProtocolSmsBuilderJson::BuildSetCdmaBroadcastSmsConfig(
        const RIL_CDMA_BroadcastSmsConfigInfo *rcbsci, int num) {
    if (rcbsci == NULL || num <= 0) {
        RilLogE("ProtocolSmsBuilderJson::%s() Wrong Broadcast SMS Configs Info!!!", __FUNCTION__);
        return NULL;
    }

    ProtocolUtils::LimitLengthToMax(RIL_LOG_CAT_SMS, __CLASS_FUNC__, num, MAX_CDMA_BCST_INFO_NUM);

    Json::Value jsonReqData;
    // store bcst_info_list
    // see ns_sms_cdma_bcst_conf_info
    Json::Value jsonBcstInfoList;

    for (int i = 0; i < num; i++) {
        jsonBcstInfoList[i][ns_sms_cdma_bcst_conf_info::key_service_category] = rcbsci[i].service_category;
        jsonBcstInfoList[i][ns_sms_cdma_bcst_conf_info::key_language] = rcbsci[i].language;
    }
    jsonReqData[ns_sms_cdma_set_bcst_sms_cfg_req::key_cdma_bcst_conf_info_list] = jsonBcstInfoList;

    return GetJsonReqModemData(jsonReqData, SIT_CDMA_SET_BCST_SMS_CFG);
}

ModemData *ProtocolSmsBuilderJson::BuildCdmaSmsBroadcastActivation(int act) {
    Json::Value jsonReqData;
    switch(act) {
        case RIL_SMS_CDMA_BCST_ACT_ACTIVATE:
            jsonReqData[ns_sms_cdma_act_bcst_sms_req::key_cdma_bcst_act] = SIT_SMS_CDMA_BCST_ACT_ACTIVATE;
            break;
        case RIL_SMS_CDMA_BCST_ACT_DEACTIVATE:
            jsonReqData[ns_sms_cdma_act_bcst_sms_req::key_cdma_bcst_act] = SIT_SMS_CDMA_BCST_ACT_DEACTIVATE;
            break;
        default:
            jsonReqData[ns_sms_cdma_act_bcst_sms_req::key_cdma_bcst_act] = SIT_SMS_CDMA_BCST_ACT_DEACTIVATE;
            RilLogE("Undefined activation code(%d)!!!: set default(turn off) value", act);
            break;
    }

    return GetJsonReqModemData(jsonReqData, SIT_CDMA_ACT_BCST_SMS);
}
