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
 * smsservice.cpp
 *
 *  Created on: 2014. 7. 21.
 *      Author: sungwoo48.choi
 */

#include "smsservice.h"
#include "rilcontext.h"
#include "rillog.h"
#include "protocoladapter.h"
#include "protocolsmsbuilder.h"
#include "protocolsmsadapter.h"
#include "protocolimsbuilder.h"
#include "cdmasmsdata.h"

BYTE g_nLastTpidNewSms = 0;

#define CDMA_SMS_DEFAULT_TIMEOUT    SMS_DEFAULT_TIMEOUT
#define CDMA_SMS_SEND_TIMEOUT       SMS_SEND_TIMEOUT

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

SmsService::SmsService(RilContext* pRilContext) : Service(pRilContext, RIL_SERVICE_SMS)
{
    memset(m_sca, 0, sizeof(m_sca));
    m_scaLen = 0;
    m_smsClass= 0;
}
SmsService::~SmsService()
{

}

bool SmsService::IsPossibleToPassInRadioOffState(int request_id)
{
    switch (request_id) {
        case RIL_REQUEST_OEM_AIMS_SEND_SMS:
        case RIL_REQUEST_OEM_AIMS_SEND_EXPECT_MORE:
        case RIL_REQUEST_GET_SMSC_ADDRESS:
        case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG:
        case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION:
        case RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG:
        case RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION:
            break;
        default:
            return false;
    }
    return true;
}

int SmsService::OnCreate(RilContext * /* pRilContext */)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    return 0;
}

BOOL SmsService::OnHandleRequest(Message* pMsg)
{
    int ret = -1;
    if (pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return FALSE;
    }

    switch (pMsg->GetMsgId()) {
    case MSG_SMS_CDMA_SEND:
        ret = DoSendCdmaSms(pMsg, FALSE);
        break;
    case MSG_SMS_CDMA_WRITE_SMS_TO_RUIM:
        ret = DoWriteCdmaSmsToRuim(pMsg);
        break;
    case MSG_SMS_CDMA_DELETE_SMS_ON_RUIM:
        ret = DoDeleteCdmaSmsOnRuim(pMsg);
        break;
    case MSG_SMS_CDMA_GET_BROADCAST_SMS_CONFIG:
        ret = DoGetCdmaBroadcastSmsConfig(pMsg);
        break;
    case MSG_SMS_CDMA_SET_BROADCAST_SMS_CONFIG:
        ret = DoSetCdmaBroadcastSmsConfig(pMsg);
        break;
    case MSG_SMS_CDMA_BROADCAST_ACTIVATION:
        ret = DoSmsCdmaBroadcastActivation(pMsg);
        break;
    case MSG_SMS_CDMA_SEND_EXPECT_MORE:
        ret = DoSendCdmaSms(pMsg, TRUE);
        break;
    case MSG_SMS_AIMS_SEND_SMS:
    case MSG_SMS_AIMS_SEND_EXPECT_MORE:
        ret = DoSendAimsSms(pMsg);
        break;
    default:
        return FALSE;
    }
    return (ret >= 0)? TRUE : FALSE;
}

BOOL SmsService::OnHandleSolicitedResponse(Message* pMsg)
{
    int ret = -1;
    if (pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return FALSE;
    }

    switch (pMsg->GetMsgId()) {
    case MSG_SMS_CDMA_SEND_DONE:
        ret = OnSendCdmaSmsDone(pMsg);
        break;
    case MSG_SMS_CDMA_WRITE_SMS_TO_RUIM_DONE:
        ret = OnWriteCdmaSmsToRuimDone(pMsg);
        break;
    case MSG_SMS_CDMA_DELETE_SMS_ON_RUIM_DONE:
        ret = OnDeleteCdmaSmsOnRuimDone(pMsg);
        break;
    case MSG_SMS_CDMA_GET_BROADCAST_SMS_CONFIG_DONE:
        ret = OnGetCdmaBroadcastSmsConfigDone(pMsg);
        break;
    case MSG_SMS_CDMA_SET_BROADCAST_SMS_CONFIG_DONE:
        ret = OnSetCdmaBroadcastSmsConfigDone(pMsg);
        break;
    case MSG_SMS_CDMA_BROADCAST_ACTIVATION_DONE:
        ret = OnSmsCdmaBroadcastActivationDone(pMsg);
        break;
    case MSG_SMS_AIMS_SEND_SMS_DONE:
    case MSG_SMS_AIMS_SEND_EXPECT_MORE_DONE:
        ret = OnSendAimsSmsDone(pMsg);
        break;
    default:
        return FALSE;
    }
    return (ret >= 0)? TRUE : FALSE;
}

BOOL SmsService::OnHandleUnsolicitedResponse(Message* pMsg)
{
    int ret = -1;
    if (pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return FALSE;
    }

    switch (pMsg->GetMsgId()) {
    case MSG_SMS_CDMA_INCOMING_NEW_SMS:
        ret = OnIncomingNewCdmaSms(pMsg);
        break;
    case MSG_SMS_CDMA_RUIM_SMS_STORAGE_FULL:
        ret = OnRuimSmsStorageFull(pMsg);
        break;
    case MSG_SMS_CDMA_VOICE_MSG_WAITING_INFO:
        ret = OnVoiceMsgWaitingInfo(pMsg);
        break;
    default:
        return FALSE;
    }
    return (ret >= 0)? TRUE : FALSE;
}

void SmsService::OnSimStatusChanged(int cardState, int appState)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if ((cardState == RIL_CARDSTATE_PRESENT) && (appState == RIL_APPSTATE_READY)) {
        RequestData *pData = new RequestData(RIL_REQUEST_GET_SMSC_ADDRESS, 0);

        if (pData != NULL) {
            Message *msg = Message::ObtainMessage(pData, RIL_SERVICE_SMS, MSG_SMS_GET_SMSC_ADDRESS);

            if (m_pRilContext->GetServiceManager()->SendMessage(msg) < 0) {
                if (msg) {
                    delete msg;
                }
            }
        }
    }
}

/*
    MMS configuration for SKT LTE
=============================================
    APN -------  lte.sktelecom.com
    MMSC ------ http://omms.nate.com:9082/oma_mms
    MMS Port ---  9093
=============================================
    MMS Proxy -- Not Set
    MCC ------- 450
    MNC ------- 05
=============================================
*/
int SmsService::DoSendCdmaSms(Message *pMsg, bool bExpectMore)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    CdmaSmsRequestData *rildata = (CdmaSmsRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] %s() rildata is NULL", m_szSvcName,  __FUNCTION__);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSendCdmaSms((const char *)rildata->GetMessage(),
            rildata->GetMessageLength(), bExpectMore);

    if (SendRequest(pModemData, CDMA_SMS_SEND_TIMEOUT, MSG_SMS_CDMA_SEND_DONE) < 0) {
        RilLogE("[%s] %s() Error", m_szSvcName, __FUNCTION__);
        return -1;
    }
    return 0;
}

int SmsService::OnSendCdmaSmsDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] %s() pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolCdmaSendSmsRespAdapter adapter(pModemData);
    int rilError = adapter.GetErrorCode();
    int errorClass = adapter.GetSmsRspErrorClass();
    int causeCode = adapter.GetSmsRspCauseCode();
    int messageRef = adapter.GetRef();

    RIL_SMS_Response resp;
    resp.ackPDU = NULL;
    resp.messageRef = messageRef;
    resp.errorCode = causeCode;

    if (rilError == RIL_E_SUCCESS) {
        RilLogV("[%s] %s() RCM_E_SUCCESS(Error class = %d, Cause code = %d)", m_szSvcName, __FUNCTION__,
                errorClass, causeCode);
        // TODO: Should set proper RIL Error based on Error Class.
        switch (errorClass) {
            case ERROR_CLASS_NO_ERROR:
                OnRequestComplete(RIL_E_SUCCESS, &resp, sizeof(RIL_SMS_Response));
                break;
            case ERROR_CLASS_TEMPORARY_ERROR:
                OnRequestComplete(RIL_E_NETWORK_NOT_READY, &resp, sizeof(RIL_SMS_Response));
                break;
            case ERROR_CLASS_PERMANENT_ERROR:
                OnRequestComplete(RIL_E_NETWORK_REJECT, &resp, sizeof(RIL_SMS_Response));
                break;
            default:
                OnRequestComplete(RIL_E_GENERIC_FAILURE);
                break;
        }
    } else {
        OnRequestComplete(rilError);
    }
    return 0;
}

int SmsService::OnIncomingNewCdmaSms(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] %s() pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolCdmaNewSmsIndAdapter adapter(pModemData);
    g_nLastTpidNewSms = adapter.GetTpid();

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_CDMA_NEW_SMS, adapter.GetRilCdmaSmsMsg(),
            adapter.GetMessageLength());
    return 0;
}

int SmsService::DoWriteCdmaSmsToRuim(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }
    CdmaSmsWriteToRuimRequestData *rildata = (CdmaSmsWriteToRuimRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] %s() rildata is NULL", m_szSvcName,  __FUNCTION__);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildWriteCdmaSmsToRuim(rildata->GetStatus(),
            (const char *)rildata->GetMessage(), rildata->GetMessageLength());

    if (SendRequest(pModemData, CDMA_SMS_DEFAULT_TIMEOUT, MSG_SMS_CDMA_WRITE_SMS_TO_RUIM_DONE) < 0) {
        RilLogE("[%s] %s() Error", m_szSvcName, __FUNCTION__);
        return -1;
    }
    return 0;
}

int SmsService::OnWriteCdmaSmsToRuimDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] %s() pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolCdmaWriteSmsToRuimRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int index = adapter.GetIndex();

    if (errorCode == RIL_E_SUCCESS) {
        RilLogV("[%s] %s() RCM_E_SUCCESS(Index=%d)", m_szSvcName, __FUNCTION__, index);
        OnRequestComplete(RIL_E_SUCCESS, &index, sizeof(index));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

int SmsService::DoDeleteCdmaSmsOnRuim(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    IntRequestData* rildata= (IntRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] %s() rildata is NULL", m_szSvcName,  __FUNCTION__);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildDeleteCdmaSmsOnRuim(rildata->GetInt());

    if (SendRequest(pModemData, CDMA_SMS_DEFAULT_TIMEOUT, MSG_SMS_CDMA_DELETE_SMS_ON_RUIM_DONE) < 0) {
        RilLogE("[%s] %s() Error", m_szSvcName, __FUNCTION__);
        return -1;
    }
    return 0;
}

int SmsService::OnDeleteCdmaSmsOnRuimDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] %s() pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    OnRequestComplete(adapter.GetErrorCode());
    return 0;
}

int SmsService::OnRuimSmsStorageFull(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    OnUnsolicitedResponse(RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL, NULL, 0);
    return 0;
}

int SmsService::DoGetCdmaBroadcastSmsConfig(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildGetCdmaBroadcastSmsConfig();
    if (SendRequest(pModemData, CDMA_SMS_DEFAULT_TIMEOUT, MSG_SMS_CDMA_GET_BROADCAST_SMS_CONFIG_DONE) < 0) {
        RilLogE("[%s] %s() Error", m_szSvcName, __FUNCTION__);
        return -1;
    }
    return 0;
}

int SmsService::OnGetCdmaBroadcastSmsConfigDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] %s() pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolGetCdmaBcstSmsConfRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, adapter.GetConfigsInfoPointers(),
                adapter.GetConfigsNumber() * sizeof(RIL_CDMA_BroadcastSmsConfigInfo *));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

int SmsService::DoSetCdmaBroadcastSmsConfig(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    CdmaBroadcastSmsConfigsRequestData *rildata = (CdmaBroadcastSmsConfigsRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] %s() rildata is NULL", m_szSvcName,  __FUNCTION__);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSetCdmaBroadcastSmsConfig(rildata->GetConfigsInfo(),
            rildata->GetConfigsNumber());
    if (SendRequest(pModemData, CDMA_SMS_DEFAULT_TIMEOUT, MSG_SMS_CDMA_SET_BROADCAST_SMS_CONFIG_DONE) < 0) {
        RilLogE("[%s] %s() Error", m_szSvcName, __FUNCTION__);
        return -1;
    }
    return 0;
}

int SmsService::OnSetCdmaBroadcastSmsConfigDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] %s() pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    OnRequestComplete(adapter.GetErrorCode());
    return 0;
}

int SmsService::DoSmsCdmaBroadcastActivation(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    IntRequestData* rildata= (IntRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] %s() rildata is NULL", m_szSvcName,  __FUNCTION__);
        return -1;
    }

    int act = rildata->GetInt();
    if (act != RIL_SMS_CDMA_BCST_ACT_ACTIVATE && act != RIL_SMS_CDMA_BCST_ACT_DEACTIVATE) {
        RilLogE("[%s] %s() Undefined activation code(%d)!!!", m_szSvcName, __FUNCTION__, act);
        return -1;
    }
    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildCdmaSmsBroadcastActivation(act);

    if (SendRequest(pModemData, CDMA_SMS_DEFAULT_TIMEOUT, MSG_SMS_CDMA_BROADCAST_ACTIVATION_DONE) < 0) {
        RilLogE("[%s] %s() Error", m_szSvcName, __FUNCTION__);
        return -1;
    }
    return 0;
}

int SmsService::OnSmsCdmaBroadcastActivationDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] %s() pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    OnRequestComplete(adapter.GetErrorCode());
    return 0;
}

int SmsService::OnVoiceMsgWaitingInfo(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] %s() pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolCdmaVoiceMsgWaitingInfoIndAdapter adapter(pModemData);

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_CDMA_NEW_SMS, adapter.GetRilCdmaSmsMsg(),
            adapter.GetMessageLength());
    return 0;
}

int SmsService::DoSendAimsSms(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    RawRequestData *rildata =(RawRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildAimsPDU(rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());

    if (SendRequest(pModemData, SMS_SEND_TIMEOUT, pMsg->GetMsgId() + 1) < 0) {
        RilLogE("[%s] %s() Error", m_szSvcName, __FUNCTION__);
        return -1;
    }

    return 0;
}

int SmsService::OnSendAimsSmsDone(Message *pMsg)
{
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("[%s] %s() pMsg is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] %s() pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());
    }
    else {
        OnRequestComplete(errorCode);
    }

    return 0;
}
