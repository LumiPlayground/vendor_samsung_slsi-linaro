/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "apimshandler.h"
#include "imsservice.h"
#include "imsdata.h"
#include "imsdatabuilder.h"
#include "protocolimsbuilder.h"
#include "protocolimsadapter.h"
#include "rilapplication.h"
#include "rillog.h"
#include "service.h"
#include <librilutils/systemproperty.h>

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * UpdateImsRegistrationInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UpdateImsRegistrationInfoHandler, MSG_IMS_UPDATE_IMS_REGISTRATION_INFO)

int UpdateImsRegistrationInfoHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    UpdateImsRegistrationInfoData *rildata =
        static_cast<UpdateImsRegistrationInfoData *>(msg->GetRequestData());

    int regState = rildata->GetRegState();
    int capabilities = rildata->GetCapabilities();
    int accessNetwork = rildata->GetAccessNetwork();
    int suggestedAction = rildata->GetSuggestedAction();

    RilLogI("[%s] %s %s() regState=%d, capabilities=%d, accessNetwork=%d, suggestedAction=%d",
            mServiceName, TAG, __FUNCTION__, regState, capabilities, accessNetwork, suggestedAction);

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildImsRegistrationInfo(regState, accessNetwork, suggestedAction, capabilities);

    if (SendRequest(pModemData, IMS_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int UpdateImsRegistrationInfoHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();

    ProtocolRespAdapter adapter(pModemData);
    OnRequestComplete(adapter.GetErrorCode());

    return 0;
}

/**
 * StartImsTrafficHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(StartImsTrafficHandler, MSG_IMS_START_IMS_TRAFFIC)

int StartImsTrafficHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    StartImsTrafficData *rildata =
        static_cast<StartImsTrafficData *>(msg->GetRequestData());

    int token = rildata->GetToken();
    int trafficType = rildata->GetImsTrafficType();
    int accessNetwork = rildata->GetAccessNetwork();
    int direction = rildata->GetDirection();

    RilLogV("[%s] %s %s() token=%d, trafficType=%d, accessNetwork=%d, direction=%d",
            mServiceName, TAG, __FUNCTION__, token, trafficType, accessNetwork, direction);

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildImsStartImsTraffic(token, trafficType, accessNetwork, direction);

    if (SendRequest(pModemData, IMS_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int StartImsTrafficHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolImsStartImsTrafficRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    if (errorCode == RIL_E_SUCCESS) {
        int reason = adapter.GetConnectionFailureReason();
        int causeCode = adapter.GetCauseCode();
        int waitTimeMillis = adapter.GetWaitTimeMillis();

        RilLogV("[%s] %s %s() connectionFailureReason=%d, causeCode=%d, waitTimeMillis=%d",
                mServiceName, TAG, __FUNCTION__, reason, causeCode, waitTimeMillis);
        ImsDataStartImsTrafficBuilder builder;
        auto rilData = std::unique_ptr<const RilData>(builder.BuildImsDataStartImsTrafficResponse(reason, causeCode, waitTimeMillis));
        if (rilData != nullptr) {
            OnRequestComplete(RIL_E_SUCCESS, rilData->GetData(), rilData->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_INTERNAL_ERR);
        }
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * StopImsTrafficHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(StopImsTrafficHandler, MSG_IMS_STOP_IMS_TRAFFIC)

int StopImsTrafficHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = static_cast<IntRequestData *>(msg->GetRequestData());
    int token = rildata->GetInt();

    RilLogV("[%s] %s %s() token=%d", mServiceName, TAG, __FUNCTION__, token);

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildImsStopImsTraffic(token);

    if (SendRequest(pModemData, IMS_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int StopImsTrafficHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();

    ProtocolRespAdapter adapter(pModemData);
    OnRequestComplete(adapter.GetErrorCode());

    return 0;
}

/**
 * TriggerEpsFallbackHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(TriggerEpsFallbackHandler, MSG_IMS_TRIGGER_EPS_FALLBACK)

int TriggerEpsFallbackHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    CreateTriggerEpsFallbackData *rildata = static_cast<CreateTriggerEpsFallbackData *>(msg->GetRequestData());
    int reason = rildata->GetReason();

    RilLogV("[%s] %s %s() reason=%d", mServiceName, TAG, __FUNCTION__, reason);

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildImsTriggerFallback(reason);

    if (SendRequest(pModemData, IMS_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int TriggerEpsFallbackHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();

    ProtocolRespAdapter adapter(pModemData);
    OnRequestComplete(adapter.GetErrorCode());

    return 0;
}

/**
 * SetSrvccCallInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSrvccCallInfoHandler, MSG_IMS_SET_SRVCC_CALL_INFO)

int SetSrvccCallInfoHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    SrvccCallInfoData *rildata = static_cast<SrvccCallInfoData *>(msg->GetRequestData());
    int callInfoNum = rildata->GetSrvccCallInfoNum();
    std::vector<RilImsV2_1::RIL_SrvccCall> infos(rildata->GetSrvccCallInfo());

    RilLogV("[%s] %s %s() callInfoNum=%d", mServiceName, TAG, __FUNCTION__, callInfoNum);

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildImsSetSrvccCallInfo(infos);

    if (SendRequest(pModemData, IMS_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetSrvccCallInfoHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/*
 * TriggerImsDeregistrationHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(TriggerImsDeregistrationHandler, MSG_IMS_UNSOL_TRIGGER_IMS_DEREGISTRATION)

int TriggerImsDeregistrationHandler::OnIndication(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolImsTriggerImsDeregistrationIndAdapter adapter(pModemData);
    int reason = adapter.GetReason();

    RilLogV("[%s] %s %s() reason=%d", mServiceName, TAG, __FUNCTION__, reason);
    OnUnsolicitedResponse(RIL_UNSOL_TRIGGER_IMS_DEREGISTRATION, &reason, sizeof(int));

    return 0;
}

/**
 * OnConnectionSetupFailureHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(OnConnectionSetupFailureHandler, MSG_IMS_UNSOL_CONNECTION_SETUP_FAILURE)

int OnConnectionSetupFailureHandler::OnIndication(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolImsConnectionSetupFailureIndAdapter adapter(pModemData);
    int token = adapter.GetFailureToken();
    int reason = adapter.GetConnectionFailureReason();
    int causeCode = adapter.GetCauseCode();
    int waitTimeMillis = adapter.GetWaitTimeMillis();

    RilLogV("[%s] %s %s() token=%d, connectionFailureReason=%d, causeCode=%d, waitTimeMillis=%d",
            mServiceName, TAG, __FUNCTION__, token, reason, causeCode, waitTimeMillis);

    ImsDataConnectionSetupFailureBuilder builder;
    auto rilData = std::unique_ptr<const RilData>(builder.BuildImsDataConnectionSetupFailure(token, reason, causeCode, waitTimeMillis));
    OnUnsolicitedResponse(RIL_UNSOL_CONNECTION_SETUP_FAILURE, rilData->GetData(), rilData->GetDataLength());

    return 0;
}

/**
 * IndTriggerEpsFallbackHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(IndTriggerEpsFallbackHandler, MSG_IMS_IND_TRIGGER_EPS_FALLBACK)

int IndTriggerEpsFallbackHandler::OnIndication(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolImsTriggerEpsFallbackIndAdapter adapter(pModemData);
    int result = adapter.GetResult();
    int rat = adapter.GetRat();

    RilLogV("[%s] %s %s() result=%d, rat=%d", mServiceName, TAG, __FUNCTION__, result, rat);

    ImsDataTriggerEpsFallbackIndBuilder builder;
    auto rilData = std::unique_ptr<const RilData>(builder.BuildImsDataTriggerEpsFallbackInd(result, rat));
    OnUnsolicitedResponse(RIL_UNSOL_OEM_IMS_TRIGGER_EPS_FALLBACK, rilData->GetData(), rilData->GetDataLength());

    return 0;
}

/**
 * MtCallGuardtmrStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(MtCallGuardtmrStatusHandler, MSG_IMS_MT_CALL_GUARDTMR_STATUS_IND)

int MtCallGuardtmrStatusHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = static_cast<RawRequestData *>(msg->GetRequestData());
    uint8_t status = *(static_cast<uint8_t *>(rildata->GetRawData()));

    RilLogV("[%s] %s %s() status=%d", mServiceName, TAG, __FUNCTION__, status);

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildImsMtCallGuardtmrStatus(status);

    if (SendRequest(pModemData) < 0) {
        RilLogW("[%s] Failed to send MT call Guardtmr Status", mServiceName);
    }
    delete pModemData;

    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

int MtCallGuardtmrStatusHandler::OnResponse(Message * /* msg */) {
    return 0;
}

/**
 * MtCsfbHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(MtCsfbHandler, MSG_IMS_IND_MT_CSFB)

int MtCsfbHandler::OnIndication(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    OnUnsolicitedResponse(RIL_UNSOL_OEM_IMS_MT_CSFB);

    return 0;
}