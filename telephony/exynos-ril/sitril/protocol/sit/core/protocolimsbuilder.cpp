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
 * protocolimsbuilder.cpp
 *
 *  Created on: 2014. 11. 18.
 *      Author: Martin
 */

#include "protocolimsbuilder.h"
#include <memory.h>
#include <string.h>
#include "modemdata.h"
#include "rillog.h"
#include <slsi/ril_oem.h>

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_INFO_LOG, format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_WARNING_LOG, format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_CRITICAL_LOG, format, ##__VA_ARGS__)

ModemData *ProtocolImsBuilder::BuildEmergencyCallStatus(BYTE nStatus, BYTE nRat) {
    sit_net_set_emergency_call_status_req req;
    int length = sizeof(req);

    memset(&req, 0, sizeof(sit_net_set_emergency_call_status_req));
    req.status = nStatus;
    req.rat = nRat;

    InitRequestHeader(&req.hdr, SIT_SET_EMERGENCY_CALL_STATUS, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolImsBuilder::BuildAimsPDU(int requestId, void *data, unsigned int datalen) {
    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    static const int messageMap[][2] = {
        {RIL_REQUEST_OEM_AIMS_DIAL, SIT_AIMS_DIAL},
        {RIL_REQUEST_OEM_AIMS_ANSWER, SIT_AIMS_ANSWER},
        {RIL_REQUEST_OEM_AIMS_HANGUP, SIT_AIMS_HANGUP},
        {RIL_REQUEST_OEM_AIMS_DEREGISTRATION, SIT_AIMS_DEREGISTRATION},
        {RIL_REQUEST_OEM_AIMS_HIDDEN_MENU, SIT_AIMS_HIDDEN_MENU},
        {RIL_REQUEST_OEM_AIMS_ADD_PDN_INFO, SIT_AIMS_ADD_PDN_INFO},
        {RIL_REQUEST_OEM_AIMS_CALL_MANAGE, SIT_AIMS_CALL_MANAGE},
        {RIL_REQUEST_OEM_AIMS_SEND_DTMF, SIT_AIMS_SEND_DTMF},
        {RIL_REQUEST_OEM_AIMS_SET_FRAME_TIME, SIT_AIMS_SET_FRAME_TIME},
        {RIL_REQUEST_OEM_AIMS_GET_FRAME_TIME, SIT_AIMS_GET_FRAME_TIME},
        {RIL_REQUEST_OEM_AIMS_CALL_MODIFY, SIT_AIMS_CALL_MODIFY},
        {RIL_REQUEST_OEM_AIMS_RESPONSE_CALL_MODIFY, SIT_AIMS_RESPONSE_CALL_MODIFY},
        {RIL_REQUEST_OEM_AIMS_CONF_CALL_ADD_REMOVE_USER, SIT_AIMS_CONF_CALL_ADD_REMOVE_USER},
        {RIL_REQUEST_OEM_AIMS_ENHANCED_CONF_CALL, SIT_AIMS_ENHANCED_CONF_CALL},
        {RIL_REQUEST_OEM_AIMS_GET_CALL_FORWARD_STATUS, SIT_AIMS_GET_CALL_FORWARD_STATUS},
        {RIL_REQUEST_OEM_AIMS_SET_CALL_FORWARD_STATUS, SIT_AIMS_SET_CALL_FORWARD_STATUS},
        {RIL_REQUEST_OEM_AIMS_GET_CALL_WAITING, SIT_AIMS_GET_CALL_WAITING},
        {RIL_REQUEST_OEM_AIMS_SET_CALL_WAITING, SIT_AIMS_SET_CALL_WAITING},
        {RIL_REQUEST_OEM_AIMS_GET_CALL_BARRING, SIT_AIMS_GET_CALL_BARRING},
        {RIL_REQUEST_OEM_AIMS_SET_CALL_BARRING, SIT_AIMS_SET_CALL_BARRING},
        {RIL_REQUEST_OEM_AIMS_SEND_SMS, SIT_AIMS_SEND_SMS},
        {RIL_REQUEST_OEM_AIMS_SEND_EXPECT_MORE, SIT_AIMS_SEND_EXPECT_MORE},
        {RIL_REQUEST_OEM_AIMS_SEND_SMS_ACK, SIT_AIMS_SEND_SMS_ACK},
        {RIL_REQUEST_OEM_AIMS_SEND_ACK_INCOMING_SMS, SIT_AIMS_SEND_ACK_INCOMING_SMS},
        {RIL_REQUEST_OEM_AIMS_CHG_BARRING_PWD, SIT_AIMS_CHG_BARRING_PWD},
        {RIL_REQUEST_OEM_AIMS_SEND_USSD_INFO, SIT_AIMS_SEND_USSD_INFO},
        {RIL_REQUEST_OEM_AIMS_GET_PRESENTATION_SETTINGS, SIT_AIMS_GET_PRESENTATION_SETTINGS},
        {RIL_REQUEST_OEM_AIMS_SET_PRESENTATION_SETTINGS, SIT_AIMS_SET_PRESENTATION_SETTINGS},
        {RIL_REQUEST_OEM_AIMS_SET_SELF_CAPABILITY, SIT_AIMS_SET_SELF_CAPABILITY},
        {RIL_REQUEST_OEM_AIMS_HO_TO_WIFI_READY, SIT_AIMS_HO_TO_WIFI_READY_REQ},
        {RIL_REQUEST_OEM_AIMS_HO_TO_3GPP, SIT_AIMS_HO_TO_3GPP_REQ},
        {RIL_REQUEST_OEM_AIMS_SEND_ACK_INCOMING_CDMA_SMS, SIT_AIMS_SEND_ACK_INCOMING_CDMA_SMS},
        {RIL_REQUEST_OEM_AIMS_DEL_PDN_INFO, SIT_AIMS_DEL_PDN_INFO},
        {RIL_REQUEST_OEM_AIMS_STACK_START_REQ, SIT_AIMS_STACK_START_REQ},
        {RIL_REQUEST_OEM_AIMS_STACK_STOP_REQ, SIT_AIMS_STACK_STOP_REQ},
        {RIL_REQUEST_OEM_AIMS_XCAPM_START_REQ, SIT_AIMS_XCAPM_START_REQ},
        {RIL_REQUEST_OEM_AIMS_XCAPM_STOP_REQ, SIT_AIMS_XCAPM_STOP_REQ},
        {RIL_REQUEST_OEM_AIMS_RTT_SEND_TEXT, SIT_AIMS_RTT_SEND_TEXT},
        {RIL_REQUEST_OEM_AIMS_EXIT_EMERGENCY_CB_MODE, SIT_AIMS_EXIT_EMERGENCY_CB_MODE},
        {RIL_REQUEST_OEM_AIMS_SET_GEO_LOCATION_INFO, SIT_AIMS_SET_GEO_LOCATION_INFO},
        {RIL_REQUEST_OEM_AIMS_CDMA_SEND_SMS, SIT_AIMS_CDMA_SEND_SMS},
        {RIL_REQUEST_OEM_AIMS_RCS_MULTI_FRAME, SIT_AIMS_RCS_MULTI_FRAME},
        {RIL_REQUEST_OEM_AIMS_RCS_CHAT, SIT_AIMS_RCS_CHAT},
        {RIL_REQUEST_OEM_AIMS_RCS_GROUP_CHAT, SIT_AIMS_RCS_GROUP_CHAT},
        {RIL_REQUEST_OEM_AIMS_RCS_OFFLINE_MODE, SIT_AIMS_RCS_OFFLINE_MODE},
        {RIL_REQUEST_OEM_AIMS_RCS_FILE_TRANSFER, SIT_AIMS_RCS_FILE_TRANSFER},
        {RIL_REQUEST_OEM_AIMS_RCS_COMMON_MESSAGE, SIT_AIMS_RCS_COMMON_MESSAGE},
        {RIL_REQUEST_OEM_AIMS_RCS_CONTENT_SHARE, SIT_AIMS_RCS_CONTENT_SHARE},
        {RIL_REQUEST_OEM_AIMS_RCS_PRESENCE, SIT_AIMS_RCS_PRESENCE},
        {RIL_REQUEST_OEM_AIMS_XCAP_MANAGE, SIT_AIMS_RCS_XCAP_MANAGE},
        {RIL_REQUEST_OEM_AIMS_RCS_CONFIG_MANAGE, SIT_AIMS_RCS_CONFIG_MANAGE},
        {RIL_REQUEST_OEM_AIMS_RCS_TLS_MANAGE, SIT_AIMS_RCS_TLS_MANAGE},
        {RIL_REQUEST_OEM_AIMS_SET_PDN_EST_STATUS, SIT_AIMS_SET_PDN_EST_STATUS},
        {RIL_REQUEST_OEM_AIMS_SET_HIDDEN_MENU_ITEM, SIT_AIMS_SET_HIDDEN_MENU_ITEM},
        {RIL_REQUEST_OEM_AIMS_GET_HIDDEN_MENU_ITEM, SIT_AIMS_GET_HIDDEN_MENU_ITEM},
        {RIL_REQUEST_OEM_AIMS_SET_RTP_RX_STATISTICS, SIT_AIMS_SET_RTP_RX_STATISTICS},
        {RIL_REQUEST_OEM_AIMS_AT_CMD_FORWARDING, SIT_REQ_AIMS_AT_CMD_FORWARDING},
        {RIL_REQUEST_OEM_AIMS_CALL_TRANSFER, SIT_REQ_AIMS_CALL_TRANSFER},
        {RIL_REQUEST_OEM_AIMS_COMMON_IMS_SVC_INFO, SIT_REQ_AIMS_COMMON_IMS_SVC_INFO},
        {RIL_REQUEST_OEM_WFC_MEDIA_CHANNEL_CONFIG, SIT_SET_WFC_MEDIA_CONFIGURATION},
        {RIL_REQUEST_OEM_WFC_DTMF_START, SIT_WFC_DTMF_START},
        {RIL_REQUEST_OEM_WFC_SET_VOWIFI_HO_THRESHOLD, SIT_SET_VOWIFI_HO_THRESHOLD},
        {RIL_REQUEST_OEM_AIMS_COMPOSER_MMTEL, SIT_AIMS_COMPOSER_MMTEL},
        {RIL_REQUEST_OEM_AIMS_ENRICHED_CALLING, SIT_AIMS_ENRICHED_CALLING},
        {RIL_REQUEST_OEM_AIMS_CANCEL_COMPOSER_MMTEL, SIT_AIMS_CANCEL_COMPOSER_MMTEL},
        {RIL_REQUEST_OEM_AIMS_CALL_DEFLECTION_REQ, SIT_AIMS_CALL_DEFLECTION_REQ},
        {RIL_REQUEST_OEM_AIMS_SET_RTP_HEADER_EXTENSION_TYPE, SIT_AIMS_SET_RTP_HEADER_EXTENSION_TYPE},
        {RIL_REQUEST_OEM_AIMS_SEND_RTP_HEADER_EXTENSION, SIT_AIMS_SEND_RTP_HEADER_EXTENSION},
        {RIL_REQUEST_OEM_AIMS_DC_SESSION_UPDATE, SIT_REQ_AIMS_DC_SESS_UPDATE},
        {RIL_REQUEST_OEM_AIMS_DC_SESSION_MANAGE, SIT_REQ_AIMS_DC_SESS_MANAGE},
        {RIL_REQUEST_OEM_AIMS_EMERGENCY_VOWIFI_CAPABILITY, SIT_AIMS_EMERGENCY_VOWIFI_CAPABILITY},
    };

    if (requestId < 0) {
        return NULL;
    }

    // TODO check maximum RCM PDU size
    if (data == NULL && datalen > 0) {
        datalen = 0;
    }
    if (datalen > MAX_IMS_RCM_SIZE) {
        // exceed allocated buffer size
        RilLogW("%s: Too large data(%u) for requestId=%d", __FUNCTION__, datalen, requestId);
        return NULL;
    }

    int protocolId = -1;
    int size = sizeof(messageMap) / sizeof(messageMap[0]);
    for (int i = 0; i < size; i++) {
        if (messageMap[i][0] == requestId) {
            protocolId = messageMap[i][1];
            break;
        }
    }  // end for i ~

    if (protocolId < 0) {
        return NULL;
    }

    int length = sizeof(RCM_HEADER) + datalen;
    char *buf = new char[length];
    InitRequestHeader((RCM_HEADER *)buf, protocolId, length);

    if (data != NULL && datalen > 0) {
        memcpy(buf + sizeof(RCM_HEADER), data, datalen);
    }

    ModemData *modemData = new ModemData(buf, length);
    delete[] buf;
    return modemData;
}

ModemData *ProtocolImsBuilder::BuildAimsIndPDU(int requestId, void *data, unsigned int datalen) {
    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    static const int messageMap[][2] = {
        {RIL_REQUEST_OEM_AIMS_HO_TO_WIFI_CANCEL_IND, SIT_AIMS_HO_TO_WIFI_CANCEL_IND},
        {RIL_REQUEST_OEM_AIMS_HO_PAYLOAD_IND, SIT_IND_AIMS_PAYLOAD_INFO_IND},
        {RIL_REQUEST_OEM_AIMS_MEDIA_STATE_IND, SIT_AIMS_MEDIA_STATE_IND},
        {RIL_REQUEST_OEM_AIMS_SEND_UL_SIP_MSG_IND, SIT_AIMS_SEND_UL_SIP_MSG_IND},
        {RIL_REQUEST_OEM_AIMS_DL_SIP_MSG_RESULT_IND, SIT_AIMS_DL_SIP_MSG_RESULT_IND},
        {RIL_REQUEST_OEM_AIMS_REGISTER_IND, SIT_AIMS_REGISTER_IND},
        {RIL_REQUEST_OEM_AIMS_DC_SESSION_MODIFY_IND, SIT_AIMS_DC_SESS_MODIFY_IND},
    };

    if (requestId < 0) {
        return NULL;
    }

    // TODO check maximum RCM PDU size
    if (data == NULL && datalen > 0) {
        datalen = 0;
    }
    if (datalen > MAX_IMS_RCM_SIZE) {
        // exceed allocated buffer size
        RilLogW("%s: Too large data(%u) for requestId=%d", __FUNCTION__, datalen, requestId);
        return NULL;
    }

    int protocolId = -1;
    int size = sizeof(messageMap) / sizeof(messageMap[0]);
    for (int i = 0; i < size; i++) {
        if (messageMap[i][0] == requestId) {
            protocolId = messageMap[i][1];
            break;
        }
    }  // end for i ~

    if (protocolId < 0) {
        return NULL;
    }

    int length = sizeof(RCM_IND_HEADER) + datalen;
    char *buf = new char[length];
    InitIndRequestHeader((RCM_IND_HEADER *)buf, protocolId, length);

    if (data != NULL && datalen > 0) {
        memcpy(buf + sizeof(RCM_IND_HEADER), data, datalen);
    }

    ModemData *modemData = new ModemData(buf, length);
    delete[] buf;
    return modemData;
}

ModemData *ProtocolImsBuilder::BuildImsRegistrationInfo(
        int regState, int accessNetwork, int suggestedAction, int capabilities) {
    sit_ims_set_registration_status_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_IMS_SET_REGISTRATION_STATUS, length);
    req.regStatus = regState;
    req.capabilites = capabilities;
    req.accessNetwork = accessNetwork;
    req.suggestedAction = suggestedAction;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolImsBuilder::BuildImsStartImsTraffic(
        int token, int trafficType, int accessNetwork, int direction) {
    sit_ims_start_ims_traffic_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_IMS_START_TRAFFIC, length);
    req.token = token;
    req.trafficType = trafficType;
    req.accessNetwork = accessNetwork;
    req.direction = direction;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolImsBuilder::BuildImsStopImsTraffic(int token) {
    sit_ims_stop_ims_traffic_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_IMS_STOP_TRAFFIC, length);
    req.token = token;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolImsBuilder::BuildImsTriggerFallback(int reason) {
    sit_set_trigger_epsfb_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_SET_TRIGGER_EPSFB, length);
    req.epsFallbackReason = static_cast<uint8_t>(reason);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolImsBuilder::BuildImsSetSrvccCallInfo(const std::vector<RilImsV2_1::RIL_SrvccCall>& infos) {
    int num = infos.size();
    int length = sizeof(sit_ims_set_srvcc_call_list_req::hdr) +
                sizeof(sit_ims_set_srvcc_call_list_req::callListNum) +
                (num * sizeof(current_call_list));

    auto pData = std::make_unique<BYTE []>(length);

    sit_ims_set_srvcc_call_list_req *pReq = reinterpret_cast<sit_ims_set_srvcc_call_list_req *>(pData.get());
    InitRequestHeader(&pReq->hdr, SIT_SET_SRVCC_CALL_LIST, length);

    pReq->callListNum = num;

    int idx = 0;
    for (auto info : infos) {
        pReq->callList[idx].index = info.index;
        pReq->callList[idx].callType = static_cast<uint8_t>(info.callType);
        pReq->callList[idx].state = static_cast<uint8_t>(info.callState);
        pReq->callList[idx].subState = static_cast<uint8_t>(info.callSubstate);
        pReq->callList[idx].ringBackTone = static_cast<uint8_t>(info.ringbackToneType);
        pReq->callList[idx].isMpty = info.isMpty;
        pReq->callList[idx].isMt = info.isMT;
        pReq->callList[idx].numberLen = MIN(info.number.length(), MAX_DIAL_NUM);
        memcpy(pReq->callList[idx].number, info.number.c_str(), pReq->callList[idx].numberLen);
        pReq->callList[idx].numberPresentation = info.numPresentation;
        pReq->callList[idx].nameLen = MIN(info.name.length(), MAX_DIAL_NAME);
        memcpy(pReq->callList[idx].name, info.name.c_str(), pReq->callList[idx].nameLen);
        pReq->callList[idx].namePresentation = info.namePresentation;
        idx++;
    }

    return new ModemData((char *)pReq, length);
}

ModemData *ProtocolImsBuilder::BuildImsMtCallGuardtmrStatus(uint8_t status) {
    sit_ims_mt_call_guardtmr_status_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_IND_MT_CALL_GUARDTMR_STATUS, length);
    req.status = status;

    return new ModemData((char *)&req, length);
}
