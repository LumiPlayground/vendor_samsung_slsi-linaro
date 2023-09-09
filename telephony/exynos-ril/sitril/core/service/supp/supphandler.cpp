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
 * supphandler.cpp
 */

#include "supphandler.h"
#include <fw/callreqdata.h>
#include "calldata.h"
#include "protocoladapter.h"
#include "protocolcalladapter.h"
#include "protocolcallbuilder.h"
#include "rillog.h"
#include "service.h"
#include "suppserviceproxy.h"

#define SUPPLEMENTARY_DEFAULT_TIMEOUT     60000

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
    RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
    RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * GetCallWaitingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetCallWaitingHandler, MSG_SS_QUERY_CALL_WAITING)

int GetCallWaitingHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    IntsRequestData* pReq = (IntsRequestData*) msg->GetRequestData();
    RilLogV("[%s] %s SS Class:%d", mServiceName, __FUNCTION__, pReq->GetInt(0)/*Service Class*/);

    SsClassX serviceClass = (SsClassX)pReq->GetInt(0) == RIL_SS_CLASS_UNKNOWN
        ? RIL_SS_CLASS_VOICE :(SsClassX)pReq->GetInt(0);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildGetCallWaiting(serviceClass);
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetCallWaitingHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolGetCallWaitingAdapter adapter(pModemData);

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    int errorCode = serviceProxy.GetValidErrors(adapter.GetErrorCode());
    /*
     * Valid errors:
     * SUCCESS
     * RADIO_NOT_AVAILABLE (radio resetting)
     */
    if (errorCode == RIL_E_SUCCESS) {
        int resp[2];
        resp[0] = adapter.GetServiceStatus();
        RilLogV("[%s] %s Service Status : %d", mServiceName, __FUNCTION__, resp[0]);

        resp[1] = adapter.GetServiceClass();
        RilLogV("[%s] %s Service Class : %d", mServiceName, __FUNCTION__, resp[1]);
        OnRequestComplete(RIL_E_SUCCESS, resp, 2 * sizeof(int));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetCallWaitingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCallWaitingHandler, MSG_SS_SET_CALL_WAITING)

int SetCallWaitingHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    IntsRequestData* pReq = (IntsRequestData*) msg->GetRequestData();
    RilLogV("[%s] %s SS Mode : %d, SS Class:%d", mServiceName, __FUNCTION__,
            pReq->GetInt(0)/*Mode*/, pReq->GetInt(1)/*Service Class*/);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildSetCallWaiting((SsModeType)pReq->GetInt(0)/*Mode*/,
            (SsClassX)pReq->GetInt(1)/*Service Class*/);
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetCallWaitingHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    int errorCode = serviceProxy.GetValidErrors(adapter.GetErrorCode());
    /*
     * Valid errors:
     *  SUCCESS
     *  RADIO_NOT_AVAILABLE (radio resetting)
    */
    OnRequestComplete(errorCode);

    return 0;
}

/**
 * GetCallForwardingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetCallForwardingHandler, MSG_SS_QUERY_CALL_FORWARDING)

void GetCallForwardingHandler::OnCreate() {
    memset(mRespCallForward, 0, sizeof(mRespCallForward));
    memset(mRespCallForwardData, 0, sizeof(mRespCallForwardData));
}

int GetCallForwardingHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    CallForwardReqData* pReq = (CallForwardReqData*) msg->GetRequestData();
    RilLogV("[%s] %s Status: %d", mServiceName, __FUNCTION__, pReq->GetStatus());
    RilLogV("[%s] %s Reason: %d", mServiceName, __FUNCTION__, pReq->GetReason());
    RilLogV("[%s] %s classType: %d", mServiceName, __FUNCTION__, pReq->GetSsClassType());
    RilLogV("[%s] %s Toa: %d", mServiceName, __FUNCTION__, pReq->GetToa());
    RilLogV("[%s] %s Number: %s", mServiceName, __FUNCTION__, pReq->GetNumber());
    RilLogV("[%s] %s TimeSeconds: %d", mServiceName, __FUNCTION__, pReq->GetTimeSeconds());

    SsClassX serviceClass = pReq->GetSsClassType();

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    if (serviceProxy.IsOperatorUsingUnknownServiceClass() && serviceClass == RIL_SS_CLASS_VOICE) {
        serviceClass = RIL_SS_CLASS_UNKNOWN;
        RilLogV("[%s] %s classType: %d", mServiceName, __FUNCTION__, serviceClass);
    }

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildGetCallForwardingStatus((SsStatusType)pReq->GetStatus(),
            pReq->GetReason(), serviceClass, pReq->GetToa(), pReq->GetNumber(), pReq->GetTimeSeconds());
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetCallForwardingHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolGetCallForwardingStatusAdapter adapter(pModemData);

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    int errorCode = serviceProxy.GetValidErrors(adapter.GetErrorCode());
    /*
     * Valid errors:
     *  SUCCESS
     *  RADIO_NOT_AVAILABLE
    */
    if (adapter.HasValidLength() == false) {
        RilLogE("[%s] %s Get Query Call forwarding status fail invalid length", mServiceName, __FUNCTION__);
        errorCode = RIL_E_INTERNAL_ERR;
    }

    if (errorCode != RIL_E_SUCCESS) {
        RilLogE("[%s] %s Get Query Call forwarding status fail (error code: %d)", mServiceName, __FUNCTION__,
                adapter. GetErrorCode());
        OnRequestComplete(errorCode);
        return 0;
    }

    int nCfNum = adapter.GetCfNum();
    if (nCfNum >= MAX_CALL_FORWARD_STATUS_NUM) {
        RilLogE("[%s] %s Call forwarding status Count invalid (%d)", mServiceName, __FUNCTION__, nCfNum);
        OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }

    RilLogV("[%s] %s Call forwarding status Count: %d", mServiceName, __FUNCTION__, nCfNum);

    for (int i = 0; i < nCfNum; i++) {
        if (0 > adapter.GetCfInfo(&mRespCallForwardData[i], i)) {
            break;
        }
        RilLogV("< %d CallInfo >", i);
        adapter.DebugPrintCfInfo(&mRespCallForwardData[i]);

        mRespCallForward[i] = &mRespCallForwardData[i];
        if (serviceProxy.IsOperatorUsingUnknownServiceClass()
                && mRespCallForward[i]->serviceClass == (int)RIL_SS_CLASS_UNKNOWN) {
            mRespCallForward[i]->serviceClass = (int)RIL_SS_CLASS_VOICE;
            RilLogV("[%s] %s classType: %d", mServiceName, __FUNCTION__, mRespCallForward[i]->serviceClass);
        }
    }

    OnRequestComplete(RIL_E_SUCCESS, mRespCallForward, nCfNum * sizeof(RIL_CallForwardInfo *));
    return 0;
}

/**
 * SetCallForwardingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCallForwardingHandler, MSG_SS_SET_CALL_FORWARDING)

int SetCallForwardingHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    CallForwardReqData* pReq = (CallForwardReqData*) msg->GetRequestData();
    RilLogV("[%s] %s Status: %d", mServiceName, __FUNCTION__, pReq->GetStatus());
    RilLogV("[%s] %s Reason: %d", mServiceName, __FUNCTION__, pReq->GetReason());
    RilLogV("[%s] %s classType: %d", mServiceName, __FUNCTION__, pReq->GetSsClassType());
    RilLogV("[%s] %s Toa: %d", mServiceName, __FUNCTION__, pReq->GetToa());
    RilLogV("[%s] %s Number: %s", mServiceName, __FUNCTION__, pReq->GetNumber());
    RilLogV("[%s] %s TimeSeconds: %d", mServiceName, __FUNCTION__, pReq->GetTimeSeconds());

    SsClassX serviceClass = pReq->GetSsClassType();

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    if (serviceProxy.IsOperatorUsingUnknownServiceClass() && serviceClass == RIL_SS_CLASS_VOICE) {
        serviceClass = RIL_SS_CLASS_UNKNOWN;
        RilLogV("[%s] %s classType: %d", mServiceName, __FUNCTION__, serviceClass);
    }

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildSetCallForwarding(pReq->GetStatus(), pReq->GetReason(),
            serviceClass, pReq->GetToa(), pReq->GetNumber(), pReq->GetTimeSeconds());
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetCallForwardingHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    int errorCode = serviceProxy.GetValidErrors(adapter.GetErrorCode());
    /*
     * Valid errors:
     *  SUCCESS
     *  RADIO_NOT_AVAILABLE (radio resetting)
    */
    if (errorCode != RIL_E_SUCCESS) {
        RilLogE("[%s] %s Set Call forwarding status fail (error code: %d)", mServiceName,
                __FUNCTION__, errorCode);
        OnRequestComplete(errorCode);
        return 0;
    }
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

/**
 * ChangeCallBarringPwdHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ChangeCallBarringPwdHandler, MSG_SS_CALL_BARRING_PWD)

int ChangeCallBarringPwdHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    OemStringsRequestData *pReq = (OemStringsRequestData *)msg->GetRequestData();

    // If barring facility string code (eg. "AO" barr all outgoing calls) is needed,
    // use CBarringPwdReqData::GetFacilityStringCode() function.
    // If 3rd para is null, this is AOSP scheme.
    // If not, this is samsung scheme that request is sent even though the new password is mismatch.
    const char *str = pReq->GetString(3) != NULL ? pReq->GetString(3) : pReq->GetString(2);
    RilLogV("[%s] %s oldpw:%s, newpw:%s, newpwagain:%s", mServiceName, __FUNCTION__, pReq->GetString(1),
            pReq->GetString(2), str);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildChangeBarringPwd(pReq->GetString(1), pReq->GetString(2), str);
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int ChangeCallBarringPwdHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    /*
     * Allowed RadioError on CardState::ABSENT is
     *   RadioError::NONE
     *   RadioError::MODEM_ERR
     *   RadioError::INVALID_ARGUMENTS
     *   RadioError::FDN_CHECK_FAILURE
     *   GeneralErrors on VTS-HIDL
     *    RadioError::RADIO_NOT_AVAILABLE (radio resetting)
     *    RadioError::NO_MEMORY
     *    RadioError::INTERNAL_ERR
     *    RadioError::SYSTEM_ERR
     *    RadioError::REQUEST_NOT_SUPPORTED
     *    RadioError::CANCELLED
     * Valid errors returned:
     *   RadioError:NONE
     *   RadioError:RADIO_NOT_AVAILABLE
     *   RadioError:SS_MODIFIED_TO_DIAL
     *   RadioError:SS_MODIFIED_TO_USSD
     *   RadioError:SS_MODIFIED_TO_SS
     *   RadioError:INVALID_ARGUMENTS
     *   RadioError:NO_MEMORY
     *   RadioError:MODEM_ERR
     *   RadioError:INTERNAL_ERR
     *   RadioError:FDN_CHECK_FAILURE
     *   RadioError:REQUEST_NOT_SUPPORTED
     *   RadioError:NO_RESOURCES
     *   RadioError:CANCELLED
     */
    RilLogI("[%s] %s errorCode=%d", mServiceName, __FUNCTION__, errorCode);

    switch (errorCode) {
        // Expected Error Code
        case RIL_E_SUCCESS:
        case RIL_E_RADIO_NOT_AVAILABLE:
        case RIL_E_SS_MODIFIED_TO_DIAL:
        case RIL_E_SS_MODIFIED_TO_USSD:
        case RIL_E_SS_MODIFIED_TO_SS:
        case RIL_E_INVALID_ARGUMENTS:
        case RIL_E_NO_MEMORY:
        case RIL_E_MODEM_ERR:
        case RIL_E_INTERNAL_ERR:
        case RIL_E_SYSTEM_ERR:
        case RIL_E_FDN_CHECK_FAILURE:
        case RIL_E_REQUEST_NOT_SUPPORTED:
        case RIL_E_NO_RESOURCES:
        case RIL_E_CANCELLED:
            OnRequestComplete(errorCode);
            break;
        // These ErrorCodes are no more expected
        case RIL_E_PASSWORD_INCORRECT:
        case RIL_E_SUBSCRIPTION_NOT_AVAILABLE:
            OnRequestComplete(RIL_E_CANCELLED);
            break;
        default:
            OnRequestComplete(RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * GetClirHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetClirHandler, MSG_SS_GET_CLIR)

int GetClirHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildGetClir();
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetClirHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolGetClirAdapter adapter(pModemData);

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    int errorCode = adapter.GetErrorCode();
    /*
     * Allowed RadioError on CardState::ABSENT is
     *   RadioError::NONE
     *   RadioError::MODEM_ERR
     *   GeneralErrors on VTS-HIDL
     *    RadioError::RADIO_NOT_AVAILABLE (radio resetting)
     *    RadioError::NO_MEMORY
     *    RadioError::INTERNAL_ERR
     *    RadioError::SYSTEM_ERR
     *    RadioError::REQUEST_NOT_SUPPORTED
     *    RadioError::CANCELLED
     * Valid errors returned:
     *   RadioError:NONE
     *   RadioError:RADIO_NOT_AVAILABLE
     *   RadioError:SS_MODIFIED_TO_DIAL
     *   RadioError:SS_MODIFIED_TO_USSD
     *   RadioError:SS_MODIFIED_TO_SS
     *   RadioError:NO_MEMORY
     *   RadioError:MODEM_ERR
     *   RadioError:INTERNAL_ERR
     *   RadioError:FDN_CHECK_FAILURE
     *   RadioError:SYSTEM_ERR
     *   RadioError:REQUEST_NOT_SUPPORTED
     *   RadioError:INVALID_ARGUMENTS
     *   RadioError:NO_RESOURCES
     *   RadioError:CANCELLED
     */
    if (errorCode == RIL_E_SUCCESS) {
        ClirInfo *clirInfo = serviceProxy.GetClirInfo();
        int resp[2];
        switch (adapter.GetClirStatus()) {
            case CLIR_PROVISIONED:
                resp[0] = CLIR_INVOCATION;
                clirInfo->SetClirAoc(CLIR_INVOCATION, mService->GetRilSocketId());
                break;
            case CLIR_NOT_PROVISIONED:
            case CLIR_UNKNOWN:
                resp[0] = CLIR_DEFAULT;
                clirInfo->SetClirAoc(CLIR_DEFAULT, mService->GetRilSocketId());
                break;
            case CLIR_TEMP_RESTRICTED:
            case CLIR_TEMP_ALLOWED:
                resp[0] = clirInfo->GetClirAoc(mService->GetRilSocketId());
                break;
        }
        resp[1] = adapter.GetClirStatus();

        RilLogV("[%s] %s CLIR AOC : %d", mServiceName, __FUNCTION__, resp[0]);
        RilLogV("[%s] %s CLIR Status : %d", mServiceName, __FUNCTION__, resp[1]);

        OnRequestComplete(RIL_E_SUCCESS, resp, 2 * sizeof(int));
    } else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_MODEM_ERR);
    }
    return 0;
}

/**
 * SetClirHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetClirHandler, MSG_SS_SET_CLIR)

int SetClirHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    IntRequestData *pReq = (IntRequestData *) msg->GetRequestData();
    RilLogV("[%s] %s Clir Mode : %d", mServiceName, __FUNCTION__,
            pReq->GetInt());

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    ClirInfo *clirInfo = serviceProxy.GetClirInfo();
    clirInfo->SetClirAoc(pReq->GetInt(), mService->GetRilSocketId());

    // jhdaniel.kim 20140930 no meaning to send CLIR set command to CP.
    // CP just ignore
    // Any side can handle CLIR value and store CLIR previous set
    // After discussing with Venkaeswar(k.venkatesh), Vendor RIL implements CLIR handling
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

int SetClirHandler::OnResponse(Message * /* msg */) {
    return -1;
}

/**
 * GetClipHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetClipHandler, MSG_SS_GET_CLIP)

int GetClipHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildGetClip();
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetClipHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolGetClipAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    /* Allowed RadioError on CardState::ABSENT are
     *   RadioError::NONE
     *   RadioError::MODEM_ERR
     *   GeneralErrors
     *    RadioError::RADIO_NOT_AVAILABLE (radio resetting)
     *    RadioError::NO_MEMORY
     *    RadioError::INTERNAL_ERR
     *    RadioError::SYSTEM_ERR
     *    RadioError::REQUEST_NOT_SUPPORTED
     *    RadioError::CANCELLED
     * Valid errors returned:
     *   RadioError:NONE
     *   RadioError:RADIO_NOT_AVAILABLE
     *   RadioError:INVALID_ARGUMENTS
     *   RadioError:NO_MEMORY
     *   RadioError:SYSTEM_ERR
     *   RadioError:MODEM_ERR
     *   RadioError:INTERNAL_ERR
     *   RadioError:FDN_CHECK_FAILURE
     *   RadioError:REQUEST_NOT_SUPPORTED
     *   RadioError:NO_RESOURCES
     *   RadioError:CANCELLED
     */
    if (errorCode == RIL_E_SUCCESS) {
        int resp = adapter.GetClipStatus();
        RilLogV("[%s] %s CLIP Status : %d", mServiceName, __FUNCTION__, resp);
        OnRequestComplete(RIL_E_SUCCESS, &resp, sizeof(int));
    } else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_MODEM_ERR);
    }
    return 0;
}

/**
 * GetColrHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetColrHandler, MSG_SS_QUERY_COLR)

int GetColrHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildQueryColr();
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetColrHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolGetColrAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    /*
     * Valid errors:
     *  SUCCESS
     *  RADIO_NOT_AVAILABLE (radio resetting)
     *  GENERIC_FAILURE
     */
    if (errorCode == RIL_E_SUCCESS) {
        int resp = adapter.GetColrStatus();
        RilLogV("[%s] %s COLR Status : %d", mServiceName, __FUNCTION__, resp);
        OnRequestComplete(RIL_E_SUCCESS, &resp, sizeof(int));
    } else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE
                ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * GetColpHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetColpHandler, MSG_SS_QUERY_COLP)

int GetColpHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildQueryColp();
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetColpHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolGetColpAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    /*
     * Valid errors:
     *  SUCCESS
     *  RADIO_NOT_AVAILABLE (radio resetting)
     *  GENERIC_FAILURE
     */
    if (errorCode == RIL_E_SUCCESS) {
        int resp = adapter.GetColpStatus();
        RilLogV("[%s] %s COLP Status : %d", mServiceName, __FUNCTION__, resp);
        OnRequestComplete(RIL_E_SUCCESS, &resp, sizeof(int));
    } else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE
                ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * SendUssdHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SendUssdHandler, MSG_SS_SEND_USSD)

int SendUssdHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    StringRequestData*pReq = (StringRequestData*) msg->GetRequestData();

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildSendUssd(pReq->GetString(), serviceProxy.IsUssdUserInitiated());
    if (serviceProxy.IsUssdUserInitiated() == false) {
        serviceProxy.SetUssdUserInitiated(true);
    }
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SendUssdHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    /* Allowed RadioError on CardState::ABSENT are
     *   RadioError:INVALID_ARGUMENTS
     *   RadioError:INVALID_STATE
     *   RadioError:MODEM_ERR
     *   GeneralErrors
     *    RadioError::RADIO_NOT_AVAILABLE (radio resetting)
     *    RadioError::NO_MEMORY
     *    RadioError::INTERNAL_ERR
     *    RadioError::SYSTEM_ERR
     *    RadioError::REQUEST_NOT_SUPPORTED
     *    RadioError::CANCELLED
     * Valid errors returned:
     *   RadioError:NONE
     *   RadioError:RADIO_NOT_AVAILABLE
     *   RadioError:SS_MODIFIED_TO_DIAL
     *   RadioError:SS_MODIFIED_TO_USSD
     *   RadioError:SS_MODIFIED_TO_SS
     *   RadioError:SIM_BUSY
     *   RadioError:OPERATION_NOT_ALLOWED
     *   RadioError:INVALID_ARGUMENTS
     *   RadioError:NO_MEMORY
     *   RadioError:MODEM_ERR
     *   RadioError:INTERNAL_ERR
     *   RadioError:ABORTED
     *   RadioError:SYSTEM_ERR
     *   RadioError:INVALID_STATE
     *   RadioError:REQUEST_NOT_SUPPORTED
     *   RadioError:INVALID_MODEM_STATE
     *   RadioError:NO_RESOURCES
     *   RadioError:CANCELLED
     */
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    } else {
        RilLogE("[%s] %s errorCode=%d", mServiceName, __FUNCTION__, errorCode);
        switch (errorCode) {
            case RIL_E_FDN_CHECK_FAILURE:
            case RIL_E_RADIO_NOT_AVAILABLE:
                OnRequestComplete(errorCode);
                break;
            default:
                OnRequestComplete(RIL_E_MODEM_ERR);
        }
    }
    return 0;
}

/**
 * CancelUssdHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CancelUssdHandler, MSG_SS_CANCEL_USSD)

int CancelUssdHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildCancelUssd();
    if (serviceProxy.IsUssdUserInitiated() == false) {
        serviceProxy.SetUssdUserInitiated(true);
    }
    if (SendRequest(pModemData, SUPPLEMENTARY_DEFAULT_TIMEOUT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int CancelUssdHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    /*
     * Valid errors:
     *  SUCCESS
     *  RADIO_NOT_AVAILABLE (radio resetting)
     *  GENERIC_FAILURE
     */
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
        OnUnsolicitedResponse(RIL_UNSOL_USSD_CANCELED);
    } else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE
                ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * OnUssdHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_INIT(OnUssdHandler, MSG_SS_USSD_NTF)

OnUssdHandler *OnUssdHandler::Create(Service *service) {
    return new OnUssdHandler(service);
}

int OnUssdHandler::OnIndication(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    /*
     * "data" is const char **
     * ((const char **)data)[0] points to a type code, which is
     *  one of these string values:
     *      "0"   USSD-Notify -- text in ((const char **)data)[1]
     *      "1"   USSD-Request -- text in ((const char **)data)[1]
     *      "2"   Session terminated by network
     *      "3"   other local client (eg, SIM Toolkit) has responded
     *      "4"   Operation not supported
     *      "5"   Network timeout
     *
     * The USSD session is assumed to persist if the type code is "1", otherwise
     * the current session (if any) is assumed to have terminated.
     *
     * ((const char **)data)[1] points to a message string if applicable, which
     *  should always be in UTF-8.
     */
    ModemData *pModemData = msg->GetModemData();
    ProtocolUssdIndAdapter adapter(pModemData);
    RilLogV("[%s] %s ussd status : %d", mServiceName, __FUNCTION__, adapter.GetUssdStatus());

    int len;
    char status[4];
    int dcs_val = 0;
    char dcs[4];
    // Received ussd can be 182, if it is gsm7bit, it can be 182*8/7=208.
    char decodedUssd[MAX_USSD_DATA_LEN + 26];
    memset(decodedUssd, 0x00, sizeof(decodedUssd));
    char *respUssd[MAX_USSD_COUNT];

    SuppServiceProxy serviceProxy(mService);
    if (!serviceProxy.HasService()) {
        return -1;
    }

    if (adapter.GetUssdStatus() == RIL_USSD_REQUEST) {
        serviceProxy.SetUssdUserInitiated(false);
    } else {
        serviceProxy.SetUssdUserInitiated(true);
    }
    RilLogV("[%s] %s user initiated : %d", mServiceName, __FUNCTION__, serviceProxy.IsUssdUserInitiated());

    len = adapter.GetDecodedUssd(decodedUssd, sizeof(decodedUssd), dcs_val);
    snprintf(status, sizeof(status)-1, "%d", adapter.GetUssdStatus());
    memset(dcs, 0x00, sizeof(dcs));
    snprintf(dcs, sizeof(dcs)-1, "%x", dcs_val);
    respUssd[0] = status;
    respUssd[1] = decodedUssd;

#ifdef RIL_FEATURE_NO_EXTENTION_USSD_DCS    // original AOSP compatibility
    OnUnsolicitedResponse(RIL_UNSOL_ON_USSD, (char *)respUssd, 2 * sizeof(char*));
#else
    respUssd[2] = dcs;
    OnUnsolicitedResponse(RIL_UNSOL_ON_USSD_WITH_DCS, (char *)respUssd, 3 * sizeof(char*));
#endif  // RIL_FEATURE_NO_EXTENTION_USSD_DCS
    return 0;
}

/**
 * OnSsSvcHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(OnSsSvcHandler, MSG_SS_SVC_NTF)

int OnSsSvcHandler::OnIndication(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    /*
     * RIL_UNSOL_SUPP_SVC_NOTIFICATION
     *
     * Reports supplementary service related notification from the network.
     *
     * "data" is a const RIL_SuppSvcNotification *
     *
     */
    ModemData *pModemData = msg->GetModemData();
    ProtocolSsSvcIndAdapter adapter(pModemData);

    RIL_SuppSvcNotification respSuppSvcNoti;
    memset(&respSuppSvcNoti, 0x00, sizeof(RIL_SuppSvcNotification));
    respSuppSvcNoti.notificationType = adapter.GetNotificationType();
    respSuppSvcNoti.code = adapter.GetCode();
    respSuppSvcNoti.index = adapter.GetCugIndex();
    if (respSuppSvcNoti.notificationType == RIL_SSNOTI_TYPE_MT) {/* 1 = MT unsolicited result code */
        respSuppSvcNoti.type = adapter.GetSSType();
        respSuppSvcNoti.number = adapter.GetNumber();
    }

    RilLogV("[%s] %s notificationType : %s", mServiceName, __FUNCTION__,
            respSuppSvcNoti.notificationType == 0 ? "MO intermediate result" : "MT unsolicited result");
    RilLogV("[%s] %s code : %d", mServiceName, __FUNCTION__, respSuppSvcNoti.code);
    RilLogV("[%s] %s index : %d", mServiceName, __FUNCTION__, respSuppSvcNoti.index);
    RilLogV("[%s] %s type : %d", mServiceName, __FUNCTION__, respSuppSvcNoti.type);
    RilLogV("[%s] %s number : %s", mServiceName, __FUNCTION__, respSuppSvcNoti.number);

    OnUnsolicitedResponse(RIL_UNSOL_SUPP_SVC_NOTIFICATION, &respSuppSvcNoti, sizeof(RIL_SuppSvcNotification));
    return 0;
}

/**
 * OnUnsolOnSsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(OnUnsolOnSsHandler, MSG_SS_IND)

int OnUnsolOnSsHandler::OnIndication(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolUnsolOnSSAdapter adapter(pModemData);

    RIL_StkCcUnsolSsResponse rsp;
    memset(&rsp, 0x0, sizeof(RIL_StkCcUnsolSsResponse));
    rsp.serviceType = (RIL_SsServiceType)adapter.GetServiceType();
    rsp.requestType = (RIL_SsRequestType)adapter.GetRequestType();
    rsp.teleserviceType = (RIL_SsTeleserviceType)adapter.GetTeleServiceType();
    rsp.serviceClass = adapter.GetServiceClass();
    rsp.result = (RIL_Errno)adapter.GetResult();
    if (adapter.GetData(rsp.ssInfo)) {
        RilLogI("[%s] %s fail to get data", mServiceName, __FUNCTION__);
    }

    OnUnsolicitedResponse(RIL_UNSOL_ON_SS, &rsp, sizeof(RIL_StkCcUnsolSsResponse));
    return 0;
}
