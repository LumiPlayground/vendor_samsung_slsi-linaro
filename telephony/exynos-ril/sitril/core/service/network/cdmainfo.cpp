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
 * cdmainfo.cpp
 *
 *  Created on: 2020. 7. 13.
 *      Author: sungwoo48.choi
 */

#include "cdmainfo.h"
#include "miscservice.h"
#include "miscdatabuilder.h"
#include "protocoladapter.h"
#include "protocolcalladapter.h"
#include "protocolcallbuilder.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "protocolnetadapter.h"
#include "protocolnetbuilder.h"
#include "rillog.h"

#define TIMEOUT_CDMA_DEFAULT        5000
#define TIMEOUT_CDMA_SUPPLEMENTARY  60000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * GetCdmaSubscriptionSourceHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetCdmaSubscriptionSourceHandler, MSG_MISC_CDMA_GET_SUBSCRIPT_SOURCE)

int GetCdmaSubscriptionSourceHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    MiscService *misc = MiscService::AsService(mService);
    if (misc == NULL) {
        return -1;
    }
    int subscriptionSource = misc->GetCdmaSubscriptionSource();
    MiscDataBuilder builder;
    const RilData *pRilData = builder.BuildCdmaSubscriptionSource(subscriptionSource);
    if ( pRilData == NULL ) {
        return OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    else {
        OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
        delete pRilData;
    }

    return 0;
}

int GetCdmaSubscriptionSourceHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return ServiceHandlerBase::OnRequest(msg);
}

/**
 * SetCdmaSubscriptionSourceHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCdmaSubscriptionSourceHandler, MSG_MISC_CDMA_SET_SUBSCRIPT_SOURCE)

int SetCdmaSubscriptionSourceHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    INT32 newSubscriptionSource = rildata->GetInt();
    RilLog("[%s] %s() Set Subscription: %d", mServiceName, __FUNCTION__, newSubscriptionSource);

    if(newSubscriptionSource==CDMA_SUBSCRIPTION_SOURCE_NV)
    {
        RilLog("[%s] %s() CDMA_SUBSCRIPTION_SOURCE_NV", mServiceName, __FUNCTION__);
    }
    else
    {
        RilLog("[%s] %s() CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM", mServiceName, __FUNCTION__);
    }
    OnRequestComplete(RIL_E_SUCCESS);

    MiscService *misc = MiscService::AsService(mService);
    if (misc == NULL) {
        return -1;
    }
    int subscriptionSource = misc->GetCdmaSubscriptionSource();
    if(subscriptionSource != newSubscriptionSource) {
        misc->SetCdmaSubscriptionSource(newSubscriptionSource);
        MiscDataBuilder builder;
        const RilData *pRilData = builder.BuildCdmaSubscriptionSource(newSubscriptionSource);
        if (pRilData != NULL) {
            OnUnsolicitedResponse(RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED, pRilData->GetData(), pRilData->GetDataLength());
            delete pRilData;
        }
    }

    return 0;
}

int SetCdmaSubscriptionSourceHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return ServiceHandlerBase::OnRequest(msg);
}

/**
 * CdmaSubscriptionHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CdmaSubscriptionHandler, MSG_MISC_GET_CDMA_SUBSCRIPTION)

int CdmaSubscriptionHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscNetworkBuilder builder;
    ModemData *pModemData = builder.BuildCdmaSubscription();
    if (SendRequest(pModemData, TIMEOUT_CDMA_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int CdmaSubscriptionHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolCdmaSubscriptionAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();

    if (uErrCode == RIL_E_SUCCESS) {
        MiscDataBuilder builder;
        const RilData *pRilData = builder.BuildCdmaSubscription(adapter.GetMdn(),
                adapter.GetSid(), adapter.GetNid(), adapter.GetMin(), adapter.GetPrlVersion());
        if ( pRilData == NULL ) {
            return OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
        else {
            OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
            delete pRilData;
        }
    }
    else OnRequestComplete(RIL_E_SUBSCRIPTION_NOT_AVAILABLE);

    return 0;
}

/**
 * CdmaPrlChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CdmaPrlChangedHandler, MSG_MISC_IND_CDMA_PRL_CHANGED)

int CdmaPrlChangedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscCdmaPrlChangeAdapter adapter(pModemData);
    int prlVersion = adapter.GetPrlVersion();
    OnUnsolicitedResponse(RIL_UNSOL_CDMA_PRL_CHANGED, &prlVersion, sizeof(prlVersion));

    return 0;
}

/**
 * SetCdmaRoamingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCdmaRoamingHandler, MSG_NET_CDMA_SET_ROAMING)

int SetCdmaRoamingHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int cdmaRoamingType = rildata->GetInt();
    RilLogV("CDMA Roaming Type: req=%d", cdmaRoamingType);
    ProtocolCdmaBuilder builder;
    ModemData *pModemData = builder.BuildSetCdmaSetRoamingType(cdmaRoamingType);
    if (SendRequest(pModemData, TIMEOUT_CDMA_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetCdmaRoamingHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    return 0;
}

/**
 * QueryCdmaRoamingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(QueryCdmaRoamingHandler, MSG_NET_CDMA_QUERY_ROAMING)

int QueryCdmaRoamingHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolCdmaBuilder builder;
    ModemData *pModemData = builder.BuildQueryCdmaRoamingType();
    if (SendRequest(pModemData, TIMEOUT_CDMA_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int QueryCdmaRoamingHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetCdmaQueryRoamingTypeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int cdmaRoamingType = adapter.QueryRoamingType();
        OnRequestComplete(RIL_E_SUCCESS, &cdmaRoamingType, sizeof(int));
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetCdmaHybridModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCdmaHybridModeHandler, MSG_NET_SET_CDMA_HYBRID_MODE)

int SetCdmaHybridModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int hybridMode = rildata->GetInt();
    RilLog("Hybrid mode : %d", hybridMode);

    ProtocolCdmaBuilder builder;
    ModemData *pModemData = builder.BuildSetCdmaHybridMode(hybridMode);
    if (SendRequest(pModemData, TIMEOUT_CDMA_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetCdmaHybridModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * GetCdmaHybridModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetCdmaHybridModeHandler, MSG_NET_GET_CDMA_HYBRID_MODE)

int GetCdmaHybridModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolCdmaBuilder builder;
    ModemData *pModemData = builder.BuildGetCdmaHybridMode();
    if (SendRequest(pModemData, TIMEOUT_CDMA_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetCdmaHybridModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetCdmaHybridModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int hybridMode = adapter.GetCdmaHybridMode();
        OnRequestComplete(RIL_E_SUCCESS, &hybridMode, sizeof(int));
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * CdmaBurstDtmfHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CdmaBurstDtmfHandler, MSG_CS_CDMA_BURST_DTMF)

int CdmaBurstDtmfHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    // On Sim absent, requesting command into CP cause IPC TIMEOUT
    // So return early,
    int cardState = mRilContext->GetUiccCardState();
    if(cardState != RIL_CARDSTATE_PRESENT) {
        RilLogI("[%s] %s mCardState(%d) is not Present(%d)",
                mServiceName, __FUNCTION__, cardState, RIL_CARDSTATE_PRESENT);
        OnRequestComplete(RIL_E_INVALID_STATE);
        return 0;
    }

    StringsRequestData* pReq = (StringsRequestData*) msg->GetRequestData();
    ProtocolCallBuilder builder;
    RilLogV("[%s] %s DTMF string:%s, on_length:%s, off_length:%s",
            mServiceName, __FUNCTION__, pReq->GetString(0), pReq->GetString(1), pReq->GetString(2));
    ModemData *pModemData = builder.BuildCdmaBurstDtmf(strlen(pReq->GetString(0)), pReq->GetString(0),
            strlen(pReq->GetString(1)), pReq->GetString(1), strlen(pReq->GetString(2)), pReq->GetString(2));
    if (SendRequest(pModemData, TIMEOUT_CDMA_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int CdmaBurstDtmfHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * CdmaSetPreferredVoicePrivacyModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CdmaSetPreferredVoicePrivacyModeHandler, MSG_CS_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE)

int CdmaSetPreferredVoicePrivacyModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData* pReq = (IntRequestData*) msg->GetRequestData();
    ProtocolCallBuilder builder;
    RilLogV("[%s] %s VPMode state: %s", mServiceName, __FUNCTION__,
            pReq->GetInt() == RIL_CALL_CDMA_VOICEPRIVACY_INACTIVE ? "Inactive" : "Active");
    ModemData *pModemData = builder.BuildCdmaSetPreferredVoicePrivacyMode(pReq->GetInt());
    if (SendRequest(pModemData, TIMEOUT_CDMA_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int CdmaSetPreferredVoicePrivacyModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * CdmaQueryPreferredVoicePrivacyModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CdmaQueryPreferredVoicePrivacyModeHandler, MSG_CS_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE)

int CdmaQueryPreferredVoicePrivacyModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildCdmaGetPreferredVoicePrivacyMode();
    if (SendRequest(pModemData, TIMEOUT_CDMA_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int CdmaQueryPreferredVoicePrivacyModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolGetPreferredVoicePrivacyModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if(errorCode == RIL_E_SUCCESS) {
        int mode = adapter.GetPreferredVoicePrivacyMode();
        RilLogV("[%s] %s VoicePrivacyMode : %d", mServiceName, __FUNCTION__, mode);
        OnRequestComplete(RIL_E_SUCCESS, &mode, sizeof(int));
    }
    else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * UnsolCdmaInfoRecHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolCdmaInfoRecHandler, MSG_CS_IND_CDMA_INFO_REC)

int UnsolCdmaInfoRecHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolCdmaInfoListIndAdapter adapter(pModemData);
    RIL_CDMA_InformationRecords cdmaInfoRecs;
    memset(&cdmaInfoRecs, 0, sizeof(cdmaInfoRecs));
    cdmaInfoRecs.numberOfInfoRecs = adapter.GetNumberOfInfoRecs();
    if(cdmaInfoRecs.numberOfInfoRecs > MAX_NUMBER_OF_INFO_RECS) {
        return -1;
    }

    RilLogV("[%s] %s CdmaInfoRecs Num: %d", mServiceName, __FUNCTION__, cdmaInfoRecs.numberOfInfoRecs);

    for(int i = 0; i<cdmaInfoRecs.numberOfInfoRecs; i++) {
        memset(&cdmaInfoRecs.infoRec[i], 0x00, sizeof(cdmaInfoRecs.infoRec[i]));
        adapter.GetCdmaInfo(cdmaInfoRecs.infoRec[i], i);
    }

    OnUnsolicitedResponse(RIL_UNSOL_CDMA_INFO_REC, &cdmaInfoRecs, sizeof(RIL_CDMA_InformationRecords));

    return 0;
}

/**
 * UnsolCdmaCallWaitingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolCdmaCallWaitingHandler, MSG_CS_SS_IND_CDMA_CALL_WAITING)

int UnsolCdmaCallWaitingHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    RIL_CDMA_CallWaiting_v6 callWaitingNoti;
    ProtocolCdmaCallWaitingIndAdapter adapter(pModemData);
    memset(&callWaitingNoti, 0x00, sizeof(callWaitingNoti));
    adapter.GetCwInfo(&callWaitingNoti);

    RilLogV("[%s] %s number : %s", mServiceName, __FUNCTION__, callWaitingNoti.number);

    OnUnsolicitedResponse(RIL_UNSOL_CDMA_CALL_WAITING, &callWaitingNoti, sizeof(RIL_CDMA_CallWaiting_v6));

    return 0;
}

/**
 * CdmaFlashHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CdmaFlashHandler, MSG_CS_SS_CDMA_FLASH)

int CdmaFlashHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    StringRequestData* pReq = (StringRequestData*) msg->GetRequestData();
    const char *flash = pReq->GetString();
    ProtocolCallBuilder builder;

    RilLogV("[%s] %s FLASH string:%s", mServiceName, __FUNCTION__, flash);
    ModemData *pModemData = builder.BuildCdmaFlash(flash);
    if (SendRequest(pModemData, TIMEOUT_CDMA_SUPPLEMENTARY, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int CdmaFlashHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * UnsolCdmaOtaProvisionStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolCdmaOtaProvisionStatusHandler, MSG_CS_IND_CDMA_OTA_PROVISION_STATUS)

int UnsolCdmaOtaProvisionStatusHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolCdmaOtaProvisionStatusIndAdapter adapter(pModemData);
    int provisionStatus = adapter.GetOtaProvisionStatus();
    if (provisionStatus != -1) {
        OnUnsolicitedResponse(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS, &provisionStatus, sizeof(provisionStatus));
    } else {
        RilLogW("[%s] %s provisionStatus is error", mServiceName, __FUNCTION__);
    }

    return 0;
}
