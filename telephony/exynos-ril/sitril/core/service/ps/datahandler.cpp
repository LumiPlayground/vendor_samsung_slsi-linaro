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
* datahandler.cpp
*/

#include "pshandler.h"
#include "datacallreqdata.h"
#include "protocolpsbuilder.h"
#include "protocolpsadapter.h"
#include "protocoladapter.h"
#include "rilapplication.h"
#include "rillog.h"
#include "service.h"
#include "psserviceproxy.h"

#define TIMEOUT_DATA_DEFAULT        5000

/**
 * UnthrottleApnHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnthrottleApnHandler, MSG_PS_IND_UNTHROTTLE_APN)

int UnthrottleApnHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("pMsg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    PsServiceProxy psProxy(mService);
    if (!psProxy.HasService())
    {
        return -1;
    }

    ProtocolPsUnthrottleApn adapter(pModemData);
    const char *apn = adapter.GetApn();

    int halVer = RilApplication::RIL_HalVersionCode;

    if (halVer == HAL_VERSION_CODE(1, 6)) {
        OnUnsolicitedResponse(RIL_UNSOL_UNTHROTTLE_APN, apn, (strlen(apn) + 1));
    } else {
        DataV2_0::RIL_DataProfileInfo *dataProfileInfo = psProxy.findDataProfile(apn);
        OnUnsolicitedResponse(RIL_UNSOL_UNTHROTTLE_APN, dataProfileInfo, sizeof(DataV2_0::RIL_DataProfileInfo));
    }
    return 0;
}

/**
 * AllocatePduSessionIdHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(AllocatePduSessionIdHandler, MSG_PS_ALLOC_PDU_SESSION)
int AllocatePduSessionIdHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildAllocatePduSessionId();

    if (pModemData == NULL) {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }

    if (SendRequest(pModemData, TIMEOUT_DATA_DEFAULT, mMessageId) < 0)
    {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int AllocatePduSessionIdHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolPsAllocatePduSessionId adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int sessionId = adapter.GetSessionId();
        RilLogV("sessionId = %d", sessionId);
        OnRequestComplete(errorCode, &sessionId, sizeof(int));
    }
    else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * ReleasePduSessionIdHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ReleasePduSessionIdHandler, MSG_PS_REL_PDU_SESSION)
int ReleasePduSessionIdHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int sessionId = rildata->GetInt();

    RilLogI("[%s] sessionId = %d", mServiceName, sessionId);

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildReleasePduSessionId(sessionId);
    if (SendRequest(pModemData, TIMEOUT_DATA_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int ReleasePduSessionIdHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}

/**
 * StartHandoverHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(StartHandoverHandler, MSG_PS_START_HANDOVER)
int StartHandoverHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int callId = rildata->GetInt();

    RilLogI("[%s] callId = %d", mServiceName, callId);

    // SIT_START_HANDOVER is deprecated.
    // https://partnerissuetracker.corp.google.com/issues/172071583
    OnRequestComplete(RIL_E_SUCCESS);
/*
    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildStartHandover(callId);
    if (SendRequest(pModemData, TIMEOUT_DATA_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
*/
    return 0;
}

int StartHandoverHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}

/**
 * CancelHandoverHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CancelHandoverHandler, MSG_PS_CANCEL_HANDOVER)
int CancelHandoverHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

   int callId = rildata->GetInt();

    RilLogI("[%s] sessionId = %d", mServiceName, callId);

    // SIT_START_HANDOVER is deprecated.
    // https://partnerissuetracker.corp.google.com/issues/172071583
    OnRequestComplete(RIL_E_SUCCESS);

/*
    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildCancelHandover(callId);
    if (SendRequest(pModemData, TIMEOUT_DATA_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
*/
    return 0;
}

int CancelHandoverHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}

/**
 * PsDataOffStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(PsDataOffStatusHandler, MSG_IND_PS_DATA_OFF_STATUS)

int PsDataOffStatusHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("pMsg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolPsDataOffStatusAdapter adapter(pModemData);

    uint8_t result[3] = { (uint8_t)adapter.GetPsDataOffAvailableStatus(),
        (uint8_t)adapter.GetExemptListForHome(), (uint8_t)adapter.GetExemptListForRoam() };
    OnUnsolicitedResponse(RIL_UNSOL_OEM_PS_DATA_OFF_STATUS, result, sizeof(result));

    return 0;
}

/**
 * StartKeepaliveHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(StartKeepaliveHandler, MSG_PS_START_KEEPALIVE)
int StartKeepaliveHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    KeepaliveRequestData *rildata = (KeepaliveRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildStartKeepAlive(rildata->m_keepAliveReq);

    if (pModemData == NULL) {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }

    if (SendRequest(pModemData, TIMEOUT_DATA_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int StartKeepaliveHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolPsStartKeepAliveAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        RIL_KeepaliveStatus result;
        result.sessionHandle = adapter.getSessionHandle();
        result.code = (RIL_KeepaliveStatusCode)adapter.getCode();
        OnRequestComplete(RIL_E_SUCCESS, &result, sizeof(result));
    } else {
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    }

    return 0;
}

/**
 * StopKeepaliveHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(StopKeepaliveHandler, MSG_PS_STOP_KEEPALIVE)
int StopKeepaliveHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildStopKeepAlive(rildata->GetInt());

    if (pModemData == NULL) {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }

    if (SendRequest(pModemData, TIMEOUT_DATA_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int StopKeepaliveHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    } else {
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    }

    return 0;
}

/**
 * KeepaliveStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(KeepaliveStatusHandler, MSG_PS_IND_KEEPALIVE_STATUS)

int KeepaliveStatusHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("pMsg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolPsKeepAliveStatusAdapter adapter(pModemData);
    RIL_KeepaliveStatus result;
    result.sessionHandle = adapter.getSessionHandle();
    result.code = (RIL_KeepaliveStatusCode)adapter.getCode();
    OnUnsolicitedResponse(RIL_UNSOL_KEEPALIVE_STATUS, &result, sizeof(result));

    return 0;
}

/**
 * CurrentRrcInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CurrentRrcInfoHandler, MSG_PS_RRC_INFO)

int CurrentRrcInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    PsServiceProxy psProxy(mService);
    if (!psProxy.HasService()) {
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    bool isRrcCon = (rildata->GetInt() ? true : false);

    psProxy.SetCurRrcState(isRrcCon);
    psProxy.OnNotifyDataCallList();
    OnRequestComplete(RIL_E_SUCCESS);

    return 0;
}

int CurrentRrcInfoHandler::OnResponse(Message * /* msg */) {
    return -1;
}

