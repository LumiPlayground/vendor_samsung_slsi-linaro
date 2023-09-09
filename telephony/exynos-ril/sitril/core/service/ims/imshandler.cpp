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
 * imshandler.cpp
 *
 *  Created on: 2020. 7. 8.
 *      Author: sungwoo48.choi
 */

#include "imshandler.h"
#include "protocolimsadapter.h"
#include "protocolimsbuilder.h"
#include "rillog.h"
#include "service.h"

#include  <set>

#define TIMEOUT_IMS_AIMS_REQUEST    180000

#define CHECK_NULL_MSG(msg, ret) { if(msg==NULL) { \
                                     RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return ret; } }
#define CHECK_NULL_REQ(msg, ret) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                     RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return ret; } }
#define CHECK_NULL_RSP(msg, ret) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                     RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return ret; } }

IMPLEMENT_MODULE_TAG(AimsDefaultHandler, AimsDefaultHandler)

std::set<int32_t> g_solicitedRequests({
    // Handled by ImsService
    MSG_AIMS_DIAL,
    MSG_AIMS_DEREGISTRATION,
    MSG_AIMS_CALL_MANAGE,
    MSG_AIMS_CALL_MODIFY,
    MSG_AIMS_RESPONSE_CALL_MODIFY,
    MSG_AIMS_CONF_CALL_ADD_REMOVE_USER,
    MSG_AIMS_ENHANCED_CONF_CALL,
    MSG_AIMS_GET_CALL_FORWARD_STATUS,
    MSG_AIMS_SET_CALL_FORWARD_STATUS,
    MSG_AIMS_GET_CALL_WAITING,
    MSG_AIMS_GET_CALL_BARRING,
    MSG_AIMS_SET_CALL_BARRING,
    MSG_AIMS_CHG_BARRING_PWD,
    MSG_AIMS_SEND_USSD_INFO,
    MSG_AIMS_HANGUP,
    MSG_AIMS_HIDDEN_MENU,
    MSG_AIMS_SEND_DTMF,
    MSG_AIMS_SET_FRAME_TIME,
    MSG_AIMS_GET_FRAME_TIME,
    MSG_AIMS_TIME_INFO,
    MSG_AIMS_GET_PRESENTATION_SETTINGS,
    MSG_AIMS_SET_PRESENTATION_SETTINGS,
    MSG_AIMS_SET_SELF_CAPABILITY,
    MSG_AIMS_XCAPM_START_REQ,
    MSG_AIMS_XCAPM_STOP_REQ,
    MSG_AIMS_RTT_SEND_TEXT,
    MSG_AIMS_EXIT_EMERGENCY_CB_MODE,
    MSG_AIMS_SET_GEO_LOCATION_INFO,
    MSG_AIMS_CDMA_SEND_SMS,
    MSG_AIMS_RCS_MULTI_FRAME_REQ,
    MSG_AIMS_RCS_CHAT_REQ,
    MSG_AIMS_RCS_GROUP_CHAT_REQ,
    MSG_AIMS_RCS_OFFLINE_MODE_REQ,
    MSG_AIMS_RCS_FILE_TRANSFER_REQ,
    MSG_AIMS_RCS_COMMON_MESSAGE_REQ,
    MSG_AIMS_RCS_CONTENT_SHARE_REQ,
    MSG_AIMS_RCS_PRESENCE_REQ,
    MSG_AIMS_XCAP_MANAGE_REQ,
    MSG_AIMS_RCS_CONFIG_MANAGE_REQ,
    MSG_AIMS_RCS_TLS_MANAGE_REQ,
    MSG_AIMS_SET_PDN_EST_STATUS,
    MSG_AIMS_SET_RTP_RX_STATISTICS,
    MSG_WFC_MEDIA_CHANNEL_CONFIG,
    MSG_WFC_DTMF_START,
    MSG_WFC_SET_VOWIFI_HO_THRESHOLD,
    MSG_AIMS_AT_CMD_FORWARDING,
    MSG_AIMS_CALL_TRANSFER,
    MSG_AIMS_COMMON_IMS_SVC_INFO,
    MSG_AIMS_COMPOSER_MMTEL_REQ,
    MSG_AIMS_ENRICHED_CALLING,
    MSG_AIMS_CANCEL_COMPOSER_MMTEL,
    MSG_AIMS_CALL_DEFLECTION_REQ,
    MSG_AIMS_SET_RTP_HEADER_EXTENSION_TYPE,
    MSG_AIMS_SEND_RTP_HEADER_EXTENSION,
    MSG_AIMS_EMERGENCY_VOWIFI_CAPABILITY,

    MSG_AIMS_HO_TO_WIFI_READY,
    MSG_AIMS_HO_TO_3GPP,
    MSG_AIMS_DC_SESSION_UPDATE,
    MSG_AIMS_DC_SESSION_MANAGE,

    // Handled by MiscService
    MSG_AIMS_ADD_PDN_INFO,
    MSG_MISC_AIMS_SEND_SMS_ACK,
    MSG_MISC_AIMS_SEND_ACK_INCOMING_SMS,
    MSG_MISC_AIMS_SEND_ACK_INCOMING_CDMA_SMS,
    MSG_AIMS_DEL_PDN_INFO,
    MSG_AIMS_STACK_START_REQ,
    MSG_AIMS_STACK_STOP_REQ,
    MSG_AIMS_SET_HIDDEN_MENU_ITEM,
    MSG_AIMS_GET_HIDDEN_MENU_ITEM,
});

std::set<int32_t> g_indRequests({
    // Handled by ImsService
    MSG_AIMS_HO_TO_WIFI_CANCEL_IND,
    MSG_AIMS_PAYLOAD_INFO_IND,
    MSG_AIMS_MEDIA_STATE_IND,
    MSG_AIMS_SEND_UL_SIP_MSG_IND,
    MSG_AIMS_DL_SIP_MSG_RESULT_IND,
    MSG_AIMS_REGISTER_IND,
    MSG_AIMS_DC_SESSION_MODIFY_IND,
});

std::set<int32_t> g_unsolicitedResponses({
    // Handled by ImsService
    MSG_IND_AIMS_CALL_RING,
    MSG_IND_AIMS_CALL_STATUS,
    MSG_IND_AIMS_CALL_MODIFY,
    MSG_IND_AIMS_EXPIRE_INFO,
    MSG_IND_AIMS_FRAME_TIME,
    MSG_IND_AIMS_SUPP_SVC_NOTIFICATION,
    MSG_IND_AIMS_NEW_SMS,
    MSG_IND_AIMS_NEW_SMS_STATUS_REPORT,
    MSG_IND_AIMS_ON_USSD,
    MSG_IND_AIMS_CONFERENCE_CALL_EVENT,
    MSG_IND_AIMS_PAYLOAD_INFO,
    MSG_IND_AIMS_VOWIFI_HO_CALL_INFO,
    MSG_IND_AIMS_NEW_CDMA_SMS,
    MSG_IND_AIMS_RINGBACK_TONE,
    MSG_IND_AIMS_CALL_MANAGE,
    MSG_IND_AIMS_CONF_CALL_ADD_REMOVE_USER,
    MSG_IND_AIMS_ENHANCED_CONF_CALL,
    MSG_IND_AIMS_CALL_MODIFY_RSP,
    MSG_IND_AIMS_DTMF_EVENT,
    MSG_IND_AIMS_RTT_NEW_TEXT,
    MSG_IND_AIMS_RTT_FAIL_SENDING_TEXT,
    MSG_IND_AIMS_EXIT_EMERGENCY_CB_MODE,
    MSG_IND_AIMS_DIALOG_INFO,
    MSG_IND_AIMS_RCS_MULTI_FRAME,
    MSG_IND_AIMS_RCS_CHAT,
    MSG_IND_AIMS_RCS_GROUP_CHAT,
    MSG_IND_AIMS_RCS_OFFLINE_MODE,
    MSG_IND_AIMS_RCS_FILE_TRANSFER,
    MSG_IND_AIMS_RCS_COMMON_MESSAGE,
    MSG_IND_AIMS_RCS_CONTENT_SHARE,
    MSG_IND_AIMS_RCS_PRESENCE,
    MSG_IND_AIMS_RCS_XCAP_MANAGE,
    MSG_IND_AIMS_RCS_CONFIG_MANAGE,
    MSG_IND_AIMS_RCS_TLS_MANAGE,
    MSG_IND_AIMS_MEDIA_STATUS,
    MSG_IND_AIMS_SIP_MSG_INFO,
    MSG_IND_AIMS_VOICE_RTQ_QUALITY,
    MSG_IND_AIMS_RTP_RX_STATISTICS,
    MSG_IND_AIMS_AT_CMD_FORWARDING,
    MSG_IND_AIMS_FEAT_SUPP,
    MSG_IND_AIMS_CALL_TRANSFER_RSP,
    MSG_IND_AIMS_CALL_TRANSFER,
    MSG_IND_AIMS_MWI_INFO,
    MSG_IND_AIMS_COMPOSER_MMTEL,
    MSG_IND_AIMS_ENRICHED_CALLING,
    MSG_IND_AIMS_RTP_HEADER_EXTENSION_TYPE,
    MSG_IND_AIMS_RTP_HEADER_EXTENSION,
    MSG_IND_AIMS_SIP_CONFIG,
    MSG_IND_AIMS_UL_SIP_MSG_RESULT,
    MSG_IND_AIMS_SEND_DL_SIP_MSG,
    MSG_IND_WFC_RTP_RTCP_TIMEOUT,
    MSG_IND_WFC_FIRST_RTP,
    MSG_IND_WFC_RTCP_RX_SR,
    MSG_IND_WFC_RCV_DTMF_NOTI,
    MSG_IND_AIMS_DC_SESS_UPDATE,
    MSG_IND_AIMS_DC_SESS_MANAGE,
    MSG_IND_AIMS_DC_SESS_MODIFY,
    MSG_IND_AIMS_CODEC_INFO,
});

AimsDefaultHandler *AimsDefaultHandler::Create(Service *service)
{
    return new AimsDefaultHandler(service);
}

bool AimsDefaultHandler::OnHandleRequest(Message *msg)
{
    CHECK_NULL_MSG(msg, false);

    int ret = -1;
    int messageId = msg->GetMsgId();
    if (CanHandleRequest(messageId)) {
        ret = OnRequest(msg);
    } else if (CanHandleRequestInd(messageId)) {
        ret = OnRequestInd(msg);
    }
    return (ret == 0);
}

bool AimsDefaultHandler::OnHandleSolicitedResponse(Message *msg)
{
    CHECK_NULL_MSG(msg, false);

    int ret = -1;
    int messageId = msg->GetMsgId();
    if (CanHandleRequest(messageId)) {
        ret = OnResponse(msg);
    }
    return (ret == 0);
}

bool AimsDefaultHandler::OnHandleUnsolicitedResponse(Message *msg)
{
    CHECK_NULL_MSG(msg, false);

    int ret = -1;
    int messageId = msg->GetMsgId();
    if (CanHandleIndication(messageId)) {
        ret = OnIndication(msg);
    }
    return (ret == 0);
}

int AimsDefaultHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg, -1);

    RawRequestData *rildata =(RawRequestData *)(msg->GetRequestData());

    RilLogV("[%s] AIMS Request: msgId=%d requestId=%d parameter=0x%p parameter length=%d",
            mServiceName, msg->GetMsgId(), rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());

    UINT timeout = TIMEOUT_IMS_AIMS_REQUEST;
    // if timeout value need to be changed, set it here
    switch(msg->GetMsgId()) {
        case MSG_AIMS_HANGUP:
            timeout = 3000;
            break;
    }

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildAimsPDU(rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());
    if (SendRequest(pModemData, timeout, msg->GetMsgId()) < 0) {
        RilLogW("[%s] Failed to send AIMS PDU", mServiceName);
        return -1;
    }

    return 0;
}

int AimsDefaultHandler::OnRequestInd(Message *msg)
{
    RilLogV("[%s] Process AIMS Request(Ind)", mServiceName);
    CHECK_NULL_REQ(msg, -1);

    RawRequestData *rildata =(RawRequestData *)(msg->GetRequestData());

    RilLogV("[%s] AIMS Request(Ind): msgId=%d requestId=%d parameter=0x%p parameter length=%d",
            mServiceName, msg->GetMsgId(), rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildAimsIndPDU(rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());

    if (pModemData != NULL) {
        if (SendRequest(pModemData) < 0) {
            RilLogW("[%s] Failed to send AIMS Ind PDU", mServiceName);
        }
        delete pModemData;
    }
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

int AimsDefaultHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg, -1);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    RilLogV("[%s] AIMS Response: msgId=%d errorCode=%d parameter=0x%p parameter length=%d",
            mServiceName, msg->GetMsgId(), errorCode, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *)adapter.GetParameter(), adapter.GetParameterLength());
    }
    else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

int AimsDefaultHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg, -1);

    ProtocolAimsIndAdapter adapter(msg->GetModemData());
    int resultId = adapter.GetResultId();
    RilLogV("[%s] AIMS Indication: msgId=%d resultId=%d parameter=0x%p parameter length=%d",
            mServiceName, msg->GetMsgId(), resultId, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());

    if (resultId != -1) {
        OnUnsolicitedResponse(resultId, adapter.GetParameter(), adapter.GetParameterLength());
    }
    else {
        RilLogW("[%s] Invalid or undefined protocolId=%d", mServiceName, adapter.GetId());
    }

    return 0;
}

bool AimsDefaultHandler::CanHandleRequest(int messageId)
{
    return g_solicitedRequests.find(messageId) != g_solicitedRequests.end();
}

bool AimsDefaultHandler::CanHandleRequestInd(int messageId)
{
    return g_indRequests.find(messageId) != g_indRequests.end();
}

bool AimsDefaultHandler::CanHandleIndication(int messageId)
{
    return g_unsolicitedResponses.find(messageId) != g_unsolicitedResponses.end();
}
