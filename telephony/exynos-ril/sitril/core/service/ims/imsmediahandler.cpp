/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */

#include "protocolimsmediaadapter.h"
#include "protocolimsmediabuilder.h"
#include "imsmediahandler.h"
#include "rillog.h"
#include "service.h"

#include  <set>

#define TIMEOUT_IMS_AIMS_REQUEST    180000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                     RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return false; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                     RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                     RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

IMPLEMENT_MODULE_TAG(ImsMediaDefaultHandler, ImsMediaDefaultHandler)

std::set<int32_t> g_imsMediaSolicitedRequests({
                                                   MSG_IMS_MEDIA_TEST_RCM,
                                                   MSG_IMS_MEDIA_INIT,
                                                   MSG_IMS_MEDIA_OPEN_SESSION,
                                                   MSG_IMS_MEDIA_CLOSE_SESSION,
                                                   MSG_IMS_MEDIA_MODIFY_SESSION,
                                                   MSG_IMS_MEDIA_SEND_DTMF,
                                                   MSG_IMS_MEDIA_START_DTMF,
                                                   MSG_IMS_MEDIA_STOP_DTMF,
                                                   MSG_IMS_MEDIA_SEND_TEXT,
                                                   MSG_IMS_MEDIA_SEND_HEADER_EXTENSION,
                                                   MSG_IMS_MEDIA_SET_MEDIA_QUALITY_THRESHOLD,
                                                   MSG_IMS_MEDIA_GET_OPERATION_MODE,
                                                   MSG_IMS_SIP_MESSAGE,
                                                   MSG_IMS_MEDIA_WIFI_CONFIG_REQ,
                                                   MSG_IMS_MEDIA_QUERY_MAC_CE_INFO,
                                           });

std::set<int32_t> g_imsMediaUnsolicitedResponses({
                                                      MSG_IMS_IND_IMS_TEST_CMD,
                                                      MSG_IMS_IND_MEDIA_ON_DTMF_RECEIVED,
                                                      MSG_IMS_IND_MEDIA_ON_TEXT_RECEIVED,
                                                      MSG_IMS_IND_MEDIA_ON_HEADER_EXTENSION_RECEIVED,
                                                      MSG_IMS_IND_MEDIA_ON_FIRST_MEDIA_RECEIVED,
                                                      MSG_IMS_IND_MEDIA_NOTIFY_MEDIA,
                                                      MSG_IMS_IND_MEDIA_NOTIFY_PACKET_LOSS,
                                                      MSG_IMS_IND_MEDIA_NOTIFY_JITTER,
                                                      MSG_IMS_IND_MEDIA_ON_CALL_QUALITY_CHANGED,
                                                      MSG_IMS_IND_MEDIA_TRIGGER_AMBR_QUERY,
                                                      MSG_IMS_MEDIA_IND_MAC_CE_INFO,
                                              });

std::set<int32_t> g_imsMediaRequestsInd({});

ImsMediaDefaultHandler *ImsMediaDefaultHandler::Create(Service *service)
{
    return new ImsMediaDefaultHandler(service);
}

bool ImsMediaDefaultHandler::OnHandleRequest(Message *msg)
{
    CHECK_NULL_MSG(msg);

    int ret = -1;
    int messageId = msg->GetMsgId();
    if (CanHandleRequest(messageId)) {
        ret = OnRequest(msg);
    } else if (CanHandleRequestInd(messageId)) {
        ret = OnRequestInd(msg);
    }
    return (ret == 0);
}

bool ImsMediaDefaultHandler::OnHandleSolicitedResponse(Message *msg)
{
    CHECK_NULL_MSG(msg);

    int ret = -1;
    int messageId = msg->GetMsgId();
    if (CanHandleRequest(messageId)) {
        ret = OnResponse(msg);
    }
    return (ret == 0);
}

bool ImsMediaDefaultHandler::OnHandleUnsolicitedResponse(Message *msg)
{
    CHECK_NULL_MSG(msg);

    int ret = -1;
    int messageId = msg->GetMsgId();
    if (CanHandleIndication(messageId)) {
        ret = OnIndication(msg);
    }
    return (ret == 0);
}

int ImsMediaDefaultHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata =(RawRequestData *)(msg->GetRequestData());

    RilLogV("[%s] ImsMedia Request: msgId=%d requestId=%d parameter=0x%p parameter length=%d",
            mServiceName, msg->GetMsgId(), rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());

    ProtocolImsMediaBuilder builder;
    ModemData *pModemData = builder.BuildImsMediaPDU(rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());
    if (SendRequest(pModemData, TIMEOUT_IMS_AIMS_REQUEST, msg->GetMsgId()) < 0) {
        RilLogW("[%s] Failed to send ImsMedia PDU", mServiceName);
        return -1;
    }

    return 0;
}

int ImsMediaDefaultHandler::OnRequestInd(Message *msg)
{
    RilLogI("[%s] Process ImsMedia Request(Ind)", mServiceName);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata =(RawRequestData *)(msg->GetRequestData());

    RilLogV("[%s] ImsMedia Request(Ind): msgId=%d requestId=%d parameter=0x%p parameter length=%d",
            mServiceName, msg->GetMsgId(), rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());

    ProtocolImsMediaBuilder builder;
    ModemData *pModemData = builder.BuildImsMediaIndPDU(rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());

    if (pModemData != nullptr) {
        if (SendRequest(pModemData) < 0) {
            RilLogW("[%s] Failed to send ImsMedia Ind PDU", mServiceName);
        }
        delete pModemData;
    }
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

int ImsMediaDefaultHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    RilLogV("[%s] ImsMedia Response: msgId=%d errorCode=%d parameter=0x%p parameter length=%d",
            mServiceName, msg->GetMsgId(), errorCode, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *)adapter.GetParameter(), adapter.GetParameterLength());
    }
    else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

int ImsMediaDefaultHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ProtocolImsMediaIndAdapter adapter(msg->GetModemData());
    int resultId = adapter.GetResultId();
    RilLogV("[%s] ImsMedia Indication: msgId=%d resultId=%d parameter=0x%p parameter length=%d",
            mServiceName, msg->GetMsgId(), resultId, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());

    if (resultId != -1) {
        OnUnsolicitedResponse(resultId, adapter.GetParameter(), adapter.GetParameterLength());
    }
    else {
        RilLogW("[%s] Invalid or undefined protocolId=%d", mServiceName, adapter.GetId());
    }

    return 0;
}

bool ImsMediaDefaultHandler::CanHandleRequest(int messageId)
{
    return g_imsMediaSolicitedRequests.find(messageId) != g_imsMediaSolicitedRequests.end();
}

bool ImsMediaDefaultHandler::CanHandleRequestInd(int messageId)
{
    return g_imsMediaRequestsInd.find(messageId) != g_imsMediaRequestsInd.end();
}

bool ImsMediaDefaultHandler::CanHandleIndication(int messageId)
{
    return g_imsMediaUnsolicitedResponses.find(messageId) != g_imsMediaUnsolicitedResponses.end();
}
