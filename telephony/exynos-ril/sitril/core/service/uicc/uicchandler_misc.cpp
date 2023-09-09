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
 * uicchandler.cpp
 *
 *  Created on: 2020. 7. 14.
 *      Author: sungwoo48.choi
 */

#include "uicchandler_misc.h"
#include "miscdata.h"
#include "miscservice.h"
#include "protocoladapter.h"
#include "protocolsimbuilder.h"
#include "protocolsimadapter.h"
#include "rilapplication.h"
#include "rillog.h"
#include "simdata.h"
#include "simdatabuilder.h"
#include "simservice.h"
#include "simserviceproxy.h"
#include "util.h"


#define TIMEOUT_UICC_DEFAULT    30000
#define TIMEOUT_UICC_ACT_DEACT  30000

/**
 * EnableUiccAppHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(EnableUiccAppHandler, MSG_SIM_ENABLE_UICC_APP)

int EnableUiccAppHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int nResult = -1;
    IntRequestData *pReq = (IntRequestData *) msg->GetRequestData();

    if (pReq == NULL) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        RilLogE("%s::%s() Invalid arguments", mServiceName, __FUNCTION__);
        return 0;
    }

    SimServiceProxy simProxy(mService);
    if(!simProxy.HasService()){
        return -1;
    }

    // need to implement IPC, the below is temperal code.
    if (mRilContext->GetUiccCardState() != RIL_CARDSTATE_PRESENT) {
        OnRequestComplete(RIL_E_SIM_ABSENT);
        return 0;
    }
    else {
        int reqUiccEnableState = pReq->GetInt();
        simProxy.SetReqUiccEnableState(reqUiccEnableState);
        ProtocolSimBuilder builder;
        ModemData *pModemData = builder.BuildSetUicc(reqUiccEnableState);
        if(pModemData!=NULL)
            nResult = SendRequest(pModemData, TIMEOUT_UICC_DEFAULT, mMessageId);
        else {
            OnRequestComplete(RIL_E_NO_MEMORY);
            return 0;
        }
    }
    return (nResult<0)? -1: 0;
}

int EnableUiccAppHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }
    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();

    SimServiceProxy simProxy(mService);
    if(!simProxy.HasService()){
        return -1;
    }

    int currentUiccEnableState = simProxy.GetCurrentUiccEnableState();
    int reqUiccEnableState = simProxy.GetReqUiccEnableState();

    if (uErrCode != RIL_E_SUCCESS) {
        uErrCode = RIL_E_MODEM_ERR;
        OnRequestComplete(uErrCode);
    }
    else {
        OnRequestComplete(uErrCode);
        if(currentUiccEnableState != reqUiccEnableState) {
            currentUiccEnableState = reqUiccEnableState;
            simProxy.SetCurrentUiccEnableState(reqUiccEnableState);
            OnUnsolicitedResponse(RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED,
                    &currentUiccEnableState, sizeof(currentUiccEnableState));
        }
    }
    return 0;
}

/**
 * GetUiccAppEnablementHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetUiccAppEnablementHandler, MSG_SIM_GET_UICC_APP_ENABLEMENT)

int GetUiccAppEnablementHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    SimServiceProxy simProxy(mService);
    if (!simProxy.HasService()) {
        return -1;
    }

    int currentUiccEnableState = simProxy.GetCurrentUiccEnableState();
    // need to implement IPC, the below is temperal code.
    if (mRilContext->GetUiccCardState() != RIL_CARDSTATE_PRESENT) {
        OnRequestComplete(RIL_E_SIM_ABSENT);
    } else {
        OnRequestComplete(RIL_E_SUCCESS, &currentUiccEnableState, sizeof(currentUiccEnableState));
    }
    return 0;
}

int GetUiccAppEnablementHandler::OnResponse(Message * /* msg */) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    // This function is never called.

    return 0;
}

/**
 * SetUiccHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetUiccHandler, MSG_SIM_SET_UICC)

int SetUiccHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    return 0;
}

int SetUiccHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    RilLogE("This should not happen");

    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }
    return 0;
}

/**
 * GetFacilityLockHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetFacilityLockHandler, MSG_SIM_GET_FACILITY_LOCK)

int GetFacilityLockHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }


    StringsRequestData *pReq = (StringsRequestData *) msg->GetRequestData();
    if (pReq == NULL) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        RilLogE("%s::%s() Invalid arguments", mServiceName, __FUNCTION__);
        return 0;
    }

    int nResult = -1;
    FacilityLock rildata;
    char **ppString = pReq->GetStringsContent();
    if (ppString == NULL || ppString[0] == NULL) { // facility is null
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        RilLogE("%s::%s() Invalid arguments", mServiceName, __FUNCTION__);
        nResult = 0;
    } else {
        if (rildata.Parse(*pReq) == FALSE) {
            OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
            RilLogE("%s::%s() Invalid arguments", mServiceName, __FUNCTION__);
            return 0;
        }

        ProtocolSimBuilder builder;
        ModemData *pModemData = builder.BuildSimGetFacilityLock((char *) rildata.GetCode(), rildata.GetPassword(),
                                                                         rildata.GetServiceClass(), rildata.GetAID());
        nResult = SendRequest(pModemData, TIMEOUT_UICC_DEFAULT, mMessageId);
    }
    return (nResult<0)? -1: 0;
}

int GetFacilityLockHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if(uErrCode==RIL_E_SUCCESS)
    {
        //int nLength = 0;
        ProtocolSimGetFacilityLockAdapter adapter(pModemData);
        RilLogV("%s::%s() Service Class:%d", mServiceName, __FUNCTION__, adapter.GetServiceClass());
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimGetFacilityLockResponse(adapter.GetServiceClass());
        if (pRilData != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
            delete pRilData;
        }
        else {
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }

    }
    else OnRequestComplete(uErrCode);
    return 0;
}

/**
 * GetIsimAuthHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetIsimAuthHandler, MSG_SIM_GET_ISIM_AUTH)

int GetIsimAuthHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    SimService *simService = SimService::AsService(mService);
    if (simService == NULL) {
        RilLogE("simService is NULL");
        return -1;
    }

    const V2_0::RIL_CardStatus rilCardStatus = simService->GetRilCardStatus();
    if (rilCardStatus.card_state == RIL_CARDSTATE_ABSENT) {
        OnRequestComplete(RIL_E_SIM_ABSENT);
        return -1;
    }

    int nResult = -1;
    IsimAuth rildata;
    if(rildata.Parse((StringRequestData *) msg->GetRequestData())==FALSE
            || rildata.GetLength()==0 || rildata.GetAuth()==NULL) {
        return -1;
    }

    ProtocolSimBuilder builder;
    int nAuthType = 0x00/*ISIM_AUTH_IMS*/;
    BOOL bExistIMS=FALSE, bExist3G=FALSE, bExistGSM=FALSE;
    for(int i = 0; i < rilCardStatus.num_applications; i++) {
        switch(rilCardStatus.applications[i].app_type) {
        case RIL_APPTYPE_SIM: bExistGSM = TRUE; break;
        case RIL_APPTYPE_USIM: bExist3G = TRUE; break;
        case RIL_APPTYPE_ISIM: bExistIMS = TRUE; break;
        default: break;
        }
    }

    RilLogV("%s::%s() IMS(%c), 3G(%c), GSM(%c)", mServiceName, __FUNCTION__, bExistIMS? 'O':'X', bExist3G? 'O':'X', bExistGSM? 'O':'X');

    if(bExistIMS==TRUE) nAuthType = 0x00/*ISIM_AUTH_IMS*/;
    else if(bExist3G==TRUE) nAuthType = 0x02/*ISIM_AUTH_3G*/;
    else if(bExistGSM==TRUE) nAuthType = 0x01/*ISIM_AUTH_GSM*/;

    ModemData *pModemData = builder.BuildSimGetIsimAuth(nAuthType, rildata.GetAuth(), rildata.GetLength());
    nResult = SendRequest(pModemData, TIMEOUT_UICC_DEFAULT, mMessageId);

    return (nResult < 0) ? -1 : 0;
}

int GetIsimAuthHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        ProtocolSimGetSimAuthAdapter adapter(pModemData);
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimGetIsimAuthResponse(adapter.GetAuthLength(), adapter.GetAuth());
        OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
        delete pRilData;
    }
    else OnRequestComplete(uErrCode);
    return 0;
}

/**
 * GetSimAuthHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetSimAuthHandler, MSG_SIM_GET_SIM_AUTH)

int GetSimAuthHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    SimService *simService = SimService::AsService(mService);
    if (simService == NULL) {
        RilLogE("simService is NULL");
        return -1;
    }

    int nResult = -1;
    SimAuthentication *rildata = (SimAuthentication *) msg->GetRequestData();
    const V2_0::RIL_CardStatus rilCardStatus = simService->GetRilCardStatus();

    int nAppType = RIL_APPTYPE_UNKNOWN;
    RilLogV("%s::%s() RIL AID:%s, App Num:%d", mServiceName, __FUNCTION__, rildata->GetAid()
            , rilCardStatus.num_applications);
    if(0 < strlen(rildata->GetAid())) {
        for (int i = 0; i < rilCardStatus.num_applications; i++) {
            if (rilCardStatus.applications[i].aid_ptr != NULL
                    && strcmp(rildata->GetAid(), rilCardStatus.applications[i].aid_ptr) == 0) {
                nAppType = rilCardStatus.applications[i].app_type;
                RilLogV("%s::%s() App Type:%d", mServiceName, __FUNCTION__, nAppType);
                break;
            }
        }
    }

    RilLogV("%s::%s() AuthContext:0x%08X", mServiceName, __FUNCTION__, rildata->GetAuthContext());

    BYTE *pAuthentication = NULL;
    int nAuthLength = 0;
    if (rildata->GetAuthentication() != NULL) {
        char *pEncodedAuth = rildata->GetAuthentication();
        if(strlen(pEncodedAuth) % 4 != 0) {
            RilLogV("%s::%s() not a multiple of 4 bytes", mServiceName, __FUNCTION__);
            OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
            return 0;
        }

        RilLogV("%s::%s() Authentication:%s", mServiceName, __FUNCTION__, rildata->GetAuthentication());
        nAuthLength = Base64_Decode((BYTE **) &pAuthentication, rildata->GetAuthentication());
    }

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimGetSimAuth(rildata->GetAuthContext(), pAuthentication
            , nAuthLength, nAppType);
    if (pModemData != NULL)
        nResult = SendRequest(pModemData, TIMEOUT_UICC_DEFAULT, mMessageId);
    else {
        OnRequestComplete(RIL_E_NO_MEMORY);
        return 0;
    }

    if (pAuthentication != NULL) delete [] pAuthentication;

    return (nResult < 0) ? -1 : 0;
}

int GetSimAuthHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        ProtocolSimGetSimAuthAdapter adapter(pModemData);
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimGetSimAuthResponse(adapter.GetAuthType()
                , adapter.GetParameterLength(), adapter.GetAuthLength(), adapter.GetAuth());
        if (pRilData != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
            delete pRilData;
        }
        else OnRequestComplete(RIL_E_INTERNAL_ERR);
    }
    else if(uErrCode==RIL_E_GENERIC_FAILURE) OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
    else OnRequestComplete(uErrCode);
    return 0;
}

/**
 * GetAtrHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetAtrHandler, MSG_SIM_GET_ATR)

int GetAtrHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int nResult = -1;
    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimGetATR();
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    return (nResult < 0) ? -1 : 0;
}

int GetAtrHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimATRAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        BYTE result = adapter.GetResult();
        if (result == TRUE) {
            BYTE length = adapter.GetATRLength();
            const char *atr = adapter.GetATR();

            SimDataBuilder builder;
            const RilData *rildata = builder.BuildGetATRResponse(atr, length);
            if (rildata != NULL) {
                OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
                delete rildata;
            }
        } else {
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
    }
    else OnRequestComplete(uErrCode);
    return 0;
}

/**
 * OpenSimChannelHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(OpenSimChannelHandler, MSG_SIM_OPEN_CHANNEL)

int OpenSimChannelHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int nResult = -1;
    SimOpenChannel *rildata = (SimOpenChannel *)msg->GetRequestData();
    const char *pszAID = rildata->GetAid();
    int p2 = rildata->GetP2();
    RilLog("AID=%s, P2=0x%02X", pszAID, p2);

    /* hardware/interfaces/radio/1.0/types.hal
    enum P2Constant : int32_t {
        NO_P2 = -1,             // No P2 value is provided
    };
    */

    ProtocolSimBuilder builder;
    ModemData *pModemData = NULL;
    if (p2 == -1) pModemData = builder.BuildSimOpenChannel(pszAID);
    else pModemData = builder.BuildSimOpenChannelWithP2(pszAID, p2);

    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    return (nResult<0)? -1: 0;
}

int OpenSimChannelHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimOpenChannelAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();

    RilLogV("Session ID : %d, uErrCode : %d", adapter.GetSessionID(), uErrCode);

    SimDataBuilder builder;
    const RilData *pRilData = builder.BuildSimOpenChannelResponse(adapter.GetSessionID()
            , adapter.GetSw1(), adapter.GetSw2(), adapter.GetResponseLength(), adapter.GetResponse());
    if (uErrCode == RIL_E_SUCCESS) {
        /* When error code is NO_SUCH_ELEMENT, need to close channel.
                so, sends session id to framework. */
        if (pRilData != NULL)
            OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
        else
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
    } else {
        if (pRilData != NULL) {
            if (uErrCode == RIL_E_GENERIC_FAILURE) {
                BYTE sw1 = adapter.GetSw1();
                BYTE sw2 = adapter.GetSw2();
                if (sw1 == 0x69 && (sw2 == 0x85 || sw2 == 0x99)) {
                    OnRequestComplete(RIL_E_NO_SUCH_ELEMENT, pRilData->GetData(), pRilData->GetDataLength());
                    RilLogV("%s() CHANGE ERROR CODE : RIL_E_GENERIC_FAILURE -> RIL_E_NO_SUCH_ELEMENT"
                            , __FUNCTION__);
                    if (sw2 == 0x99) {
                        RilLogV("%s() CRC2: NoSuchElementError", __FUNCTION__);
                        RilLogV("%s() if the AID on the SE is not available (or cannot be selected) or a logical channel is already open to a non-multiselectable applet.", __FUNCTION__);
                    } else if (sw2 == 0x85) {
                        RilLogV("%s() Conditions of use not satisfied", __FUNCTION__);
                    }
                    RilLogV("%s() sw1: 0x%X, sw2: 0x%X", __FUNCTION__, sw1, sw2);
                } else {
                    OnRequestComplete(uErrCode, pRilData->GetData(), pRilData->GetDataLength());
                }
            } else {
                OnRequestComplete(uErrCode, pRilData->GetData(), pRilData->GetDataLength());
            }
        } else {
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
    }

    if (pRilData != NULL)
        delete pRilData;
    return 0;
}

/**
 * CloseSimChannelHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CloseSimChannelHandler, MSG_SIM_CLOSE_CHANNEL)

int CloseSimChannelHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        return -1;
    }

    int nResult = -1;
    INT32 nSessionID = rildata->GetInt();
    RilLogV("session id : %d", nSessionID);

    if (nSessionID == 0) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        RilLogE("%s::%s() Invalid arguments", mServiceName, __FUNCTION__);
        nResult = 0;
    } else {
        ProtocolSimBuilder builder;
        ModemData *pModemData = builder.BuildSimCloseChannel(nSessionID);
        nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);
    }

    return (nResult<0)? -1: 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(CloseSimChannelHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * TransmitSimApduChannelHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(TransmitSimApduChannelHandler, MSG_SIM_TRANSMIT_APDU_CHANNEL)

int TransmitSimApduChannelHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    SimAPDU *rildata = (SimAPDU *)msg->GetRequestData();
    if (NULL == rildata) {
        return -1;
    }

    int nResult = -1;

    int sessionId = rildata->GetSessionId();
    // bits b1, b2 of the CLA should contain the logical channel value. from Table 10.3, ETSI 102 221 spec.
    // bit 7 should be applied from 5.1.1 Class byte and 5.1.1.2 Logical channels on ISO/IEC 7816-4.
    int cla = rildata->GetCla();
    if (sessionId >= 4) // further interindustry
        cla = (cla & 0xF0) | 0x40 | (0x0f & (sessionId-4));
    else // First interindustry
        cla = (cla & 0xFC) | (0x03 & sessionId);

    int instruction = rildata->GetInstruction();
    int p1 = rildata->GetP1();
    int p2 = rildata->GetP2();
    int p3 = rildata->GetP3();
    const char *pszApduData = rildata->GetDataLength()? (const char*) rildata->GetData() : NULL;

    // In case of READ_BINARY(0xb0), don't need data.
    // Refer 11.1.3 READ BINARY on ETSI 102 221
    if (instruction == 0xb0) pszApduData =  NULL;

    RilLogV("sessionId(%d), cla(%d), instruction(%d), p1(%d), p2(%d), p3(%d) dataLen(%d)",
                            sessionId, cla, instruction, p1, p2, p3, rildata->GetDataLength());

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimTransmitApduChannel(sessionId, cla, instruction, p1, p2, p3, pszApduData);
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    return (nResult<0)? -1: 0;
}

int TransmitSimApduChannelHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimResponseAdapter response(pModemData);
    UINT uErrCode = response.GetErrorCode();

    if (uErrCode == RIL_E_SUCCESS)
    {
        ProtocolSimTransmitApduChannelAdapter adapter(pModemData);

        SimDataBuilder builder;

        //RilLogV("%s::%s() sw1(%d), sw2(%d), length(%d), data(%s)", m_szSvcName, __FUNCTION__,
        //                            adapter.GetSw1(), adapter.GetSw2(), adapter.GetResponseLength());

        const RilData *pRilData = builder.BuildSimTransmitApduChannelResponse(adapter.GetSw1()
                , adapter.GetSw2(), adapter.GetApduLength(), adapter.GetApdu());
        if (pRilData != NULL) {
            // ETSI TS 102.201, 7.2.2.3.1
            // SW1 : '6C' : The terminal shall wait for a second procedure byte then immediately
            // repeat the previous command header to the UICC using a length of
            // 'XX', where 'XX' is the value of the second procedure byte (SW2).
            int nSW1 = adapter.GetSw1();
            int nSW2 = adapter.GetSw2();
            if(nSW1==0x6C)
            {
                RilLogV("SW1:0x%02X, Retransmission with P3(%d:0x%X)", nSW1, nSW2, nSW2);
                Message *pCurrMsg = mService->GetCurrentMsg();
                SimAPDU *rildata = (SimAPDU *)pCurrMsg->GetRequestData();
                rildata->SetP3(nSW2);
                if(OnRequest(pCurrMsg)==-1) OnRequestComplete(RIL_E_GENERIC_FAILURE);
            }
            else    // Not 0x6C
                OnRequestComplete(uErrCode, pRilData->GetData(), pRilData->GetDataLength());

            delete pRilData;
        }
        else
            OnRequestComplete(RIL_E_GENERIC_FAILURE);

    }
    else
        OnRequestComplete(uErrCode);
    return 0;
}

/**
 * TransmitSimApduBasicHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(TransmitSimApduBasicHandler, MSG_SIM_TRANSMIT_APDU_BASIC)

int TransmitSimApduBasicHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int nResult = -1;
    SimAPDU *rildata = (SimAPDU *)msg->GetRequestData();
    if (NULL == rildata) {
        return -1;
    }

    int sessionId = rildata->GetSessionId();
    // bits b1, b2 of the CLA should contain the logical channel value. from Table 10.3, ETSI 102 221 spec.
    int cla = rildata->GetCla() | (0x03 & sessionId);
    int instruction = rildata->GetInstruction();
    int p1 = rildata->GetP1();
    int p2 = rildata->GetP2();
    int p3 = rildata->GetP3();
    const char *pszApduData = rildata->GetDataLength()? (const char*) rildata->GetData() : NULL;

    // In case of READ_BINARY(0xb0), don't need data.
    // Refer 11.1.3 READ BINARY on ETSI 102 221
    if (instruction == 0xb0) pszApduData =  NULL;

    RilLogV("sessionId(%d), cla(%d), instruction(%d), p1(%d), p2(%d), p3(%d) dataLen(%d)",
                            sessionId, cla, instruction, p1, p2, p3, rildata->GetDataLength());

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimTransmitApduBasic(sessionId, cla, instruction, p1, p2
            , p3, pszApduData);
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    return (nResult<0)? -1: 0;
}

int TransmitSimApduBasicHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimTransmitApduBasicAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS)
    {
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimTransmitApduBasicResponse(adapter.GetApduLength()
                , adapter.GetApdu());
        if (pRilData != NULL) {
            // ETSI TS 102.201, 7.2.2.3.1
            // SW1 : '6C' : The terminal shall wait for a second procedure byte then immediately
            // repeat the previous command header to the UICC using a length of
            // 'XX', where 'XX' is the value of the second procedure byte (SW2).
            int nSW1 = adapter.GetSw1();
            int nSW2 = adapter.GetSw2();
            if(nSW1==0x6C)
            {
                RilLogV("SW1:0x%02X, Retransmission with P3(%d:0x%X)", nSW1, nSW2, nSW2);
                Message *pCurrMsg = mService->GetCurrentMsg();
                SimAPDU *rildata = (SimAPDU *)pCurrMsg->GetRequestData();
                rildata->SetP3(nSW2);
                if(OnRequest(pCurrMsg)==-1) OnRequestComplete(RIL_E_INTERNAL_ERR);
            }
            else    // Not 0x6C
                OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());

            delete pRilData;
        }

    }
    else
        OnRequestComplete(RIL_E_SIM_ERR);
    return 0;
}

/**
 * SetCarrierRestrictionsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCarrierRestrictionsHandler, MSG_SIM_SET_CARRIER_RESTRICTIONS)

int SetCarrierRestrictionsHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int halVer = RilApplication::RIL_HalVersionCode;
    if (mService->GetCurrentRequestData() != NULL) {
        halVer = mService->GetCurrentRequestData()->GetHalVersion();
    }

    if (halVer < HAL_VERSION_CODE(1, 4)) {
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    int nResult = -1;
    CarrierRestrictionsData *pRilData = (CarrierRestrictionsData *)msg->GetRequestData();
    int nAllowedLength = pRilData->GetAllowCarrierLength();
    int nExcludedLength = pRilData->GetExcludeCarrierLength();
    CarrierInfo *pAllowed = pRilData->GetAllowCarrier();
    CarrierInfo *pExcluded = pRilData->GetExcludeCarrier();
    bool bPrioritized = pRilData->GetAllowedCarriersPrioritized();
    int nMultiSimPolicy = pRilData->GetMultiSimLockPolicy();
    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSetCarrierRestrictions(nAllowedLength, pAllowed, nExcludedLength,
        pExcluded, bPrioritized, nMultiSimPolicy);
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    return (nResult<0)? -1: 0;
}

int SetCarrierRestrictionsHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    OnRequestComplete(uErrCode);

    return 0;
}

/**
 * GetCarrierRestrictionsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetCarrierRestrictionsHandler, MSG_SIM_GET_CARRIER_RESTRICTIONS)

int GetCarrierRestrictionsHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int halVer = RilApplication::RIL_HalVersionCode;
    if (mService->GetCurrentRequestData() != NULL) {
        halVer = mService->GetCurrentRequestData()->GetHalVersion();
    }

    if (halVer < HAL_VERSION_CODE(1, 4)) {
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    int nResult = -1;
    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildGetCarrierRestrictions();
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    return (nResult<0)? -1: 0;
}

int GetCarrierRestrictionsHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimGetCarrierRestrictionsAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {

        int nAllowedLength = adapter.GetAllowedCarrierLength();
        int nExcludedLength = adapter.GetExcludedCarrierLength();

        RIL_Carrier *pAllowed = NULL;
        RIL_Carrier *pExcluded = NULL;
        if (nAllowedLength > 0) {
            pAllowed = new RIL_Carrier[nAllowedLength];
            if (pAllowed == NULL) {
                RilLogE("OnGetCarrierRestrictionsDone: Memory allocation failed for request");
                nAllowedLength = 0;
            }

            nAllowedLength = adapter.GetAllowedCarriers(pAllowed, nAllowedLength);
        }

        if (nExcludedLength > 0) {
            pExcluded = new RIL_Carrier[nExcludedLength];
            if (pExcluded == NULL) {
                RilLogE("OnGetCarrierRestrictionsDone: Memory allocation failed for request");
                nExcludedLength = 0;
            }
            nExcludedLength = adapter.GetExcludedCarriers(pExcluded, nExcludedLength);
        }

        bool bPrioritized = adapter.GetAllowedCarriersPrioritized();
        int nMultiSimPolicy = adapter.GetMultiSimPolicy();
        RilLogV("bPrioritized : %d", bPrioritized);
        RilLogV("nMultiSimPolicy: %d", nMultiSimPolicy);

        SimDataBuilder builder;
        const RilData *rildata = builder.BuildGetCarrierRestrictionsResponse(nAllowedLength
                , pAllowed, nExcludedLength, pExcluded, bPrioritized, nMultiSimPolicy);
        if (rildata != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        } else {
            OnRequestComplete(RIL_E_INTERNAL_ERR);
        }

        if (pAllowed != NULL) {
            delete[] pAllowed->mcc;
            delete[] pAllowed->mnc;
            delete[] pAllowed->match_data;
            delete[] pAllowed;
        }
        if (pExcluded != NULL) {
            delete[] pExcluded->mcc;
            delete[] pExcluded->mnc;
            delete[] pExcluded->match_data;
            delete[] pExcluded;
        }

    } else {
        OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
    }

    return 0;
}

/**
 * SetSimCardPowerHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSimCardPowerHandler, MSG_SIM_SET_SIM_CARD_POWER)

int SetSimCardPowerHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    int nResult = -1;
    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        RilLogE("%s::%s() Invalid arguments", mServiceName, __FUNCTION__);
        return 0;
    }

    int halVer = RilApplication::RIL_HalVersionCode;
    if (mService->GetCurrentRequestData() != NULL) {
        halVer = mService->GetCurrentRequestData()->GetHalVersion();
    }

    int nSimCardPowerState = rildata->GetInt();
    if (halVer >= HAL_VERSION_CODE(1, 6)) {
        if (nSimCardPowerState == ((int) SimService::SIM_CARD_POWERSTATE_POWER_DOWN)) {
            RilLogI("nSimCardPowerState is changed for Radio1.6v");
            nSimCardPowerState = SimService::SIM_CARD_POWERSTATE_POWER_DOWN_PRESENT;
        }
    } else if (halVer == HAL_VERSION_CODE(1, 1)) {
        // HAL_VERSION_CODE(1, 1) nSimCardPowerState - POWER_DOWN / UP / PASS_THROUGH
        if (nSimCardPowerState > SimService::SIM_CARD_POWERSTATE_POWER_UP_PASS_THROUGH) {
            OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
            RilLogE("%s::%s() Invalid arguments", mServiceName, __FUNCTION__);
            return 0;
        }
    } else {
        // HAL_VERSION_CODE(1, 0) nSimCardPowerState - POWER_DOWN / UP
        if (nSimCardPowerState > ((int) SimService::SIM_CARD_POWERSTATE_POWER_UP)) {
            OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
            RilLogE("%s::%s() Invalid arguments", mServiceName, __FUNCTION__);
            return 0;
        }
    }

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSetSimCardPower(nSimCardPowerState);
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    return (nResult<0)? -1: 0;
}

int SetSimCardPowerHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    int halVer = RilApplication::RIL_HalVersionCode;
    if (mService->GetCurrentRequestData() != NULL) {
        halVer = mService->GetCurrentRequestData()->GetHalVersion();
    }
    ProtocolSimResponseAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();

    if (halVer >= HAL_VERSION_CODE(1,6)) {
        if (uErrCode == RIL_E_SUCCESS) {
            OnRequestComplete(RIL_E_SUCCESS);
        } else {
            OnRequestComplete(uErrCode == RIL_E_RADIO_NOT_AVAILABLE ?
            RIL_E_RADIO_NOT_AVAILABLE : RIL_E_SIM_ERR);
        }
    } else {
        if (uErrCode == RIL_E_SUCCESS) {
            OnRequestComplete(RIL_E_SUCCESS);
        } else {
            OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        }
    }
    return 0;
}
/**
 * UiccSubscriptionStatusChanged
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UiccSubscriptionStatusChanged, MSG_SIM_IND_UICC_SUBSCRIPTION_STATUS_CHANGED)

int UiccSubscriptionStatusChanged::OnIndication(Message *msg)
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

    ProtocolUiccSubStatusChangeAdapter adapter(pModemData);
    int state = adapter.GetState();
    OnUnsolicitedResponse(RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED, &state, sizeof(state));

    return 0;
}

/**
 * SimStatusChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SimStatusChangedHandler, MSG_SIM_STATUS_CHANGED)

int SimStatusChangedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED);
    return 0;
}

/**
 * SimDepersonalizationHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SimDepersonalizationHandler, MSG_SIM_DEPERSONALIZATION)

int SimDepersonalizationHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    StringsRequestData *pReq = (StringsRequestData *) msg->GetRequestData();
    if (pReq == NULL) {
        return -1;
    }

    SimService *simService = SimService::AsService(mService);
    if (simService == NULL) {
        RilLogE("simService is NULL");
        return -1;
    }

    int nResult = -1;
    int persoType = strtol(pReq->GetString(0), NULL, 10);
    const char *pControlKey = pReq->GetString(1);
    RilLogE("%s::%s() persoType = %d, controlKey = %s", mServiceName, __FUNCTION__, persoType, pControlKey);

    const V2_0::RIL_CardStatus rilCardStatus = simService->GetRilCardStatus();
    if (rilCardStatus.card_state != RIL_CARDSTATE_PRESENT) {
        OnRequestComplete(RIL_E_SIM_ABSENT);
        return 0;
    }
    else {
        /* test code for _SIM_DEPERSONALIZATION_TEST_
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimDepersonalizationResponse(persoType, 8);
        if ( pRilData == NULL )
        {
            return OnRequestComplete(RIL_E_NO_MEMORY);
        }
        OnRequestComplete(RIL_E_PASSWORD_INCORRECT, pRilData->GetData(), pRilData->GetDataLength());
        LEAVE_FUNC();
        return 0;
        */

        // check last saved sim status and decide FAC type.
        int nFac = -1;

        switch (persoType) {
            case RIL_PERSOSUBSTATE_SIM_NETWORK:
                nFac = FacilityLock::FAC_PN;
                break;
            case RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET:
                nFac = FacilityLock::FAC_PU;
                break;
            case RIL_PERSOSUBSTATE_SIM_CORPORATE: //5
                nFac = FacilityLock::FAC_PC;
                break;
            case RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER:
                nFac = FacilityLock::FAC_PP;
                break;
            case RIL_PERSOSUBSTATE_SIM_SIM:
                RilLogE("%s(): There is no matching", __FUNCTION__);
                break;
            case RIL_PERSOSUBSTATE_SIM_NETWORK_PUK:
            case RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK:
                nFac = FacilityLock::FAC_PN;
                break;
            case RIL_PERSOSUBSTATE_SIM_CORPORATE_PUK: // 10
                nFac = FacilityLock::FAC_PC;
                break;
            case RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK:
                nFac = FacilityLock::FAC_PP;
                break;
            case RIL_PERSOSUBSTATE_SIM_SIM_PUK:
                RilLogE("%s(): There is no matching", __FUNCTION__);
                break;
            case RIL_PERSOSUBSTATE_RUIM_NETWORK1:
            case RIL_PERSOSUBSTATE_RUIM_NETWORK2:
                nFac = FacilityLock::FAC_PN;
                break;
            case RIL_PERSOSUBSTATE_RUIM_HRPD: //15
                RilLogE("%s(): There is no matching", __FUNCTION__);
                break;
            case RIL_PERSOSUBSTATE_RUIM_CORPORATE:
                nFac = FacilityLock::FAC_PC;
                break;
            case RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER:
                nFac = FacilityLock::FAC_PP;
                break;
            case RIL_PERSOSUBSTATE_RUIM_RUIM:
                RilLogE("%s(): There is no matching", __FUNCTION__);
                break;
            case RIL_PERSOSUBSTATE_RUIM_NETWORK1_PUK:
            case RIL_PERSOSUBSTATE_RUIM_NETWORK2_PUK: //20
                nFac = FacilityLock::FAC_PN;
                break;
            case RIL_PERSOSUBSTATE_RUIM_HRPD_PUK:
                RilLogE("%s(): There is no matching", __FUNCTION__);
                break;
            case RIL_PERSOSUBSTATE_RUIM_CORPORATE_PUK:
                nFac = FacilityLock::FAC_PC;
                break;
            case RIL_PERSOSUBSTATE_RUIM_SERVICE_PROVIDER_PUK:
                nFac = FacilityLock::FAC_PP;
                break;
            case RIL_PERSOSUBSTATE_RUIM_RUIM_PUK:
                RilLogE("%s(): There is no matching", __FUNCTION__);
                break;
            case RIL_PERSOSUBSTATE_SIM_SPN: //25
            case RIL_PERSOSUBSTATE_SIM_SPN_PUK:
                nFac = FacilityLock::FAC_SIM_SPN;
                break;
            /** Equivalent Home PLMN */
            case RIL_PERSOSUBSTATE_SIM_SP_EHPLMN:
            case RIL_PERSOSUBSTATE_SIM_SP_EHPLMN_PUK:
                nFac = FacilityLock::FAC_SIM_SP_EHPLMN;
                break;
            case RIL_PERSOSUBSTATE_SIM_ICCID:
            case RIL_PERSOSUBSTATE_SIM_ICCID_PUK:
                nFac = FacilityLock::FAC_SIM_ICCID;
                break;
            case RIL_PERSOSUBSTATE_SIM_IMPI:
            case RIL_PERSOSUBSTATE_SIM_IMPI_PUK:
                nFac = FacilityLock::FAC_SIM_IMPI;
                break;
            /** Network subset service provider */
            case RIL_PERSOSUBSTATE_SIM_NS_SP:
            case RIL_PERSOSUBSTATE_SIM_NS_SP_PUK: //34
                nFac = FacilityLock::FAC_SIM_NS_SP;
                break;
            default:
                break;
        }

        if ( nFac == -1) {
            RilLogE("%s(): FAC type is invalid", __FUNCTION__);
            return -1;
        }

        VerifyNetLock *rildata = (VerifyNetLock *)msg->GetRequestData();
        const char *pszCode = rildata->GetDepersonalCode();
        if (pszCode != NULL && strlen(pszCode)>0) {
            ProtocolSimBuilder builder;
            //need to converting from AP persoType to CP perso type
            ModemData *pModemData = builder.BuildSimVerifyNetworkLock(nFac, pControlKey, 0, NULL);
            nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);
        }
    }

    return (nResult<0)? -1: 0;
}

int SimDepersonalizationHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    StringsRequestData *pReq = (StringsRequestData *) mService->GetCurrentRequestData();
    if (pReq == NULL) {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }

    int persoType = strtol(pReq->GetString(0), NULL, 10);
    SimDataBuilder builder;
    ProtocolSimVerifyNetLockAdapter adapterSimVerifyNetLock(pModemData);
    UINT uErrCode = adapterSimVerifyNetLock.GetErrorCode();
    const RilData *pRilData = builder.BuildSimDepersonalizationResponse(persoType
            , adapterSimVerifyNetLock.GetRemainCount());
    RilLogV("remain count : %d", adapterSimVerifyNetLock.GetRemainCount());
    if ( pRilData == NULL )
    {
        return OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    if (uErrCode==RIL_E_SUCCESS)
        OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
    else
        OnRequestComplete(uErrCode, pRilData->GetData(), pRilData->GetDataLength());

    if (pRilData != NULL)
        delete pRilData;

    return 0;
}

/**
 * SimLockInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SimLockInfoHandler, MSG_SIM_GET_SIM_LOCK_INFO)

int SimLockInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildGetSimLockInfo();
    if (SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SimLockInfoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    if(msg == NULL) {
        RilLogE("%s::%s() Message = NULL", mServiceName, __FUNCTION__);
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSimLockInfoAdapter adapter(pModemData);
    int policy = adapter.GetPolicy();
    int status = adapter.GetStatus();
    int lockType = adapter.GetLockType();
    int maxRetryCount = adapter.GetMaxRetryCount();
    int remainCount = adapter.GetRemainCount();
    int lockCodeCount = adapter.GetLockCodeCount();
    const char *lockCode = adapter.GetLockCode();

    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        SimDataBuilder builder;
        const RilData *rildata = builder.BuildGetSimLockInfoResponse(policy, status, lockType,
                maxRetryCount, remainCount, lockCodeCount, lockCode);
        if (rildata != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }
    else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * SimTrayStatusChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SimTrayStatusChangedHandler, MSG_IND_SIM_TRAY_STATUS)

int SimTrayStatusChangedHandler::OnIndication(Message *msg)
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

    ProtocolSimTrayStatusChangeAdapter adapter(pModemData);
    int state = adapter.GetState();
    RilLogV("[%s] %s %s() - state:%d", mServiceName, TAG, __FUNCTION__, state);

    OnUnsolicitedResponse(RIL_UNSOL_IND_SIM_TRAY_STATUS_CHANGED, &state, sizeof(state));
    return 0;
}
