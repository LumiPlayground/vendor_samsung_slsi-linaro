/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "imsservice.h"
#include <librilutils/systemproperty.h>
#include "rillog.h"
#include "protocolimsbuilder.h"
#include "protocolimsadapter.h"
#include "protocolcalladapter.h" // TODO: remove dependency on this when IMS service is supporting JSON
#include "protocolbuilder.h"
#include "imsdatabuilder.h"

#include "simdata.h"
#include "simdatabuilder.h"
#include "protocolsimbuilder.h"
#include "protocolsimadapter.h"
#include "protocolcallbuilder.h"
#include "networkservice.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_IMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define PARAM_NULL(msg, retValue) { if(msg==NULL) { RilLogE("%s::%s() Parameter = NULL", m_szSvcName, __FUNCTION__); return retValue; } }
#define NULL_REQ(msg)             { if(msg==NULL || msg->GetRequestData()==NULL) { RilLogE("%s::%s() RequestData = NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_RSP(msg)             { if(msg==NULL || msg->GetModemData()==NULL) { RilLogE("%s::%s() ModemData = NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define ENTER_FUNC()        { RilLogI("%s::%s() [<-- ", m_szSvcName, __FUNCTION__); }
#define LEAVE_FUNC()        { RilLogI("%s::%s() [--> ", m_szSvcName, __FUNCTION__); }
#define NOT_IMPLEMENT()     { RilLogE("%s::%s() Not Implemented", m_szSvcName, __FUNCTION__); }

// #### Definition for Debugging Logs ####
//#define ENABLE_LOGS_FUNC_ENTER_EXIT
#define ENABLE_ANDROID_LOG

#define LogE    RilLogE
#define LogW    RilLogW
#define LogN    RilLogI
#ifdef ENABLE_ANDROID_LOG
#define LogI    RilLogI
#define LogV    RilLogV
#endif // end of ENABLE_ANDROID_LOG

#define PROPERTY_IMS_APP_TYPE "vendor.ril.context.sim.ims_app_type"
#define PROPERTY_CALL_WAITING_FOR_SYNC "persist.vendor.radio.call_waiting_for_sync"
#define IMS_AIMS_REQUEST_TIMEOUT   180000
#define UNSUPPORT_CALL_WAITING_SYNC 0
#define SUPPORT_CALL_WAITING_SYNC   1

// #### Internal Done Functions ####
INT32 ImsService::DoXXXDone(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    int nResult = -1;

    do
    {
        // Parsing Params from Modem
        ModemData *pModemData = pMsg->GetModemData();
        ProtocolRespAdapter adapter(pModemData);
        INT32 errorCode = adapter.GetErrorCode();
        if(errorCode != RIL_E_SUCCESS)
        {
            RilLogE("%s::%s() !! ERROR !!, errorCode(0x%x) in GetErrorCode()",m_szSvcName, __FUNCTION__,errorCode);

            // Complete Request for Error
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            nResult = -3;
            break;
        }

        // Complete Request for Success
        OnRequestComplete( RIL_E_SUCCESS);

        RilLogV("%s::%s() Done ...Success",m_szSvcName, __FUNCTION__);
        nResult= 0;
    }while(0);

    LEAVE_FUNC();
    return nResult;
}


INT32 ImsService::DoSimAuth(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = -1;
    RawRequestData *pRawRildata = (RawRequestData *)pMsg->GetRequestData();
    int len = pRawRildata->GetSize();
    if (len <= 0) return -1;
    auto pCharArr = std::unique_ptr<char[]>(new char[len + 1] {});
    if (pRawRildata->GetRawData() == NULL) return -1;
    memcpy(pCharArr.get(), pRawRildata->GetRawData(), len);

    IsimAuth rildata;
    if(rildata.ParseWithAuthType((const char*)pCharArr.get()) == FALSE || rildata.GetLength()==0 ||
            rildata.GetAuth()==NULL) {
        LEAVE_FUNC();
        return -1;
    }

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimGetIsimAuth(rildata.GetAuthType(), rildata.GetAuth(),
            rildata.GetLength());
    nResult = SendRequest(pModemData, IMS_DEFAULT_TIMEOUT, MSG_IMS_SIM_AUTH_DONE);

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

INT32 ImsService::OnSimAuthDone(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolRespAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    if(uErrCode==RIL_E_SUCCESS)
    {
        ProtocolSimGetSimAuthAdapter adapter(pMsg->GetModemData());
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimGetIsimAuthResponse(adapter.GetAuthLength(), adapter.GetAuth());
        OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
        delete pRilData;
    }
    else
    {
        OnRequestComplete(uErrCode== RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_GENERIC_FAILURE);
    }

    LEAVE_FUNC();
    return 0;
}

INT32 ImsService::DoGetGbaAuth(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    int nResult = -1;

    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    const char *GbaAuthData = (char *)rildata->GetRawData();
    int GbaAuthDataSize = rildata->GetSize();
    if (GbaAuthData != NULL && GbaAuthDataSize > 0)
    {
        ProtocolSimBuilder builder;
        ModemData *pModemData = builder.BuildSimGetGbaAuth(GbaAuthData, GbaAuthDataSize);
        if (pModemData == nullptr) {
            RilLogE("Cannot build modem data for GetGbaAuth");
            LEAVE_FUNC();
            return -1;
        }

        nResult = SendRequest(pModemData, IMS_DEFAULT_TIMEOUT, MSG_IMS_GET_GBA_AUTH_DONE);
    }

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

INT32 ImsService::OnGetGbaAuthDone(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolRespAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();

    if(uErrCode==RIL_E_SUCCESS)
    {
        ProtocolSimGetGbaAuthAdapter adapter(pMsg->GetModemData());
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildSimGetGbaAuthResponse(adapter.GetGbaAuthLength(), adapter.GetGbaAuth());
        OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
        delete pRilData;
    }
    else
    {
        OnRequestComplete(uErrCode== RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_GENERIC_FAILURE);
    }

    LEAVE_FUNC();
    return 0;
}

INT32 ImsService::DoSetTempEmergencyNumber(Message *pMsg) {
    NULL_REQ(pMsg);

    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    typedef struct {
        BYTE operation;
        BYTE number_len;
        char number[255];
        BYTE category;
    } ECC_DATA;

    const char *rawData = (char *)rildata->GetRawData();
    int len = rildata->GetSize();
    if (len != sizeof(ECC_DATA)) return -1;

    if (rawData != NULL) {
        ECC_DATA *data = (ECC_DATA *)rawData;
        NetworkService* pNetworkService = (NetworkService*)GetCurrentService(RIL_SERVICE_NETWORK);
        if (pNetworkService != NULL) {
            pNetworkService->SetTempEmergencyNumber(data->operation, data->number_len,
                                                    data->number, data->category);
        }
    }
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

INT32 ImsService::OnImsSetSrvccInfo(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    BYTE SrvccInfo[3];
    ProtocolSrvccHoIndAdapter adapter(pMsg->GetModemData());
    SrvccInfo[0] = adapter.GetSrvccHoType();
    SrvccInfo[1] = adapter.GetSrvccHoStatus();
    SrvccInfo[2] = adapter.GetSrvccHoFailCause();

    OnUnsolicitedResponse(RIL_UNSOL_OEM_IMS_SET_SRVCC_INFO, (void *)SrvccInfo, sizeof(SrvccInfo));

    //Send to Framework
    {
        INT32 SrvccResult =  SrvccInfo[1] < 2 ? 0x00000011 & SrvccInfo[1] : 2;
        OnUnsolicitedResponse(RIL_UNSOL_SRVCC_STATE_NOTIFY, (int *)&SrvccResult, sizeof(INT32));
        RilLogV("%s() RIL_UNSOL_SRVCC_STATE_NOTIFY: %d ##", __FUNCTION__, SrvccResult);

        m_srvccState = (RIL_SrvccState)SrvccResult;
    }

    switch(m_srvccState)
    {
    case HANDOVER_STARTED:
        RilLogI("[%s] SRVCC : HO Started", GetServiceName());
        break;
    case HANDOVER_COMPLETED:
        {
            RilLogI("[%s] SRVCC : HO Completed", GetServiceName());
            OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED);

            // send non-VoLTE command instead of VoLTE request
            if ( m_pCurReqMsg != NULL ) {
                if ( m_pCurReqMsg->GetMsgId() == MSG_AIMS_ANSWER ) {
                    RilLogI("[%s] Change AIMS answer to CS answer", GetServiceName());
                    ProtocolCallBuilder builder;
                    ModemData *pModemData = builder.BuildAnswer();
                    if ( SendRequest(pModemData, IMS_AIMS_REQUEST_TIMEOUT, MSG_AIMS_ANSWER_DONE) < 0 ) {
                        OnRequestComplete(RIL_E_GENERIC_FAILURE);
                    }
                }
            }
        }
        break;
    default://failed
        {
            RilLogI("[%s] SRVCC : HO Failed or cancelled", GetServiceName());

            if ( m_pCurReqMsg != NULL ) {
                if ( m_pCurReqMsg->GetMsgId() == MSG_AIMS_ANSWER ) {
                    RilLogI("[%s] Send original AIMS command", GetServiceName());
                    if ( DoAimsDefaultRequestHandler(m_pCurReqMsg) < 0 ) {
                        OnRequestComplete(RIL_E_GENERIC_FAILURE);
                    }
                }
            }
        }
        break;
    }

    LEAVE_FUNC();

    return 0;
}

//AIMS support start ---------------------
INT32 ImsService::Do_AIMS_SET_CALL_WAITING(Message *pMsg)
{
    ENTER_FUNC();
    NULL_REQ(pMsg);

    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    const char *ImsData = (char *)rildata->GetRawData();

    RilLogI("[%s] %s ", m_szSvcName, __FUNCTION__ );

    if ( ImsData != NULL )
    {
        ProtocolImsAimsCallWaitingAdapter adapter(ImsData, rildata->GetSize());
        BOOL bEnable = adapter.IsEnable();
        int nServiceClass = adapter.GetServiceClass();
        RilLogV("[%s] set cw --> enable : %d, SS Class:%d, socketid:%d", __FUNCTION__, bEnable, nServiceClass, GetRilSocketId());
        char szProp[64] = {0, };
        sprintf(szProp, "%s_%d", PROPERTY_CALL_WAITING_FOR_SYNC, GetRilSocketId());
        int needSyncWithCs = SystemProperty::GetInt(szProp, UNSUPPORT_CALL_WAITING_SYNC);
        if (needSyncWithCs == SUPPORT_CALL_WAITING_SYNC) {
            RilLogV("[%s] needSyncWithCs ", __FUNCTION__);
            int pInts[2] = {bEnable, nServiceClass};
            OnRequest(RIL_REQUEST_SET_CALL_WAITING, pInts, sizeof(pInts));
        }
        return DoAimsDefaultRequestHandler(pMsg);
    }

    LEAVE_FUNC();
    return -1;

}

INT32 ImsService::On_AIMS_SET_CALL_WAITINGDone(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolRespAdapter adapter(pMsg->GetModemData());
    int errorCode = adapter.GetErrorCode();

    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *)adapter.GetParameter(), adapter.GetParameterLength());
    } else {
        OnRequestComplete(errorCode);
    }

    LEAVE_FUNC();
    return 0;
}

INT32 ImsService::Do_AIMS_GET_REGISTRATION(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg, -1);

    INT32 RespInts[2];
    RespInts[0] = RIL_IMS_NOT_REGISTERED;
    RespInts[1] = RADIO_TECH_3GPP;

    OnRequestComplete(RIL_E_GENERIC_FAILURE);

    LEAVE_FUNC();
    return 0;
}

INT32 ImsService::OnUNSOL_AIMS_REGISTRATION(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolImsRegIndAdapter adapter(pMsg->GetModemData());
    OnUnsolicitedResponse(RIL_UNSOL_OEM_AIMS_REGISTRATION, adapter.GetParameter(), adapter.GetParameterLength());

    int preImsRegState = mImsRegState;
    int imsReg = adapter.GetRegState();
    if (imsReg != -1)
        mImsRegState = imsReg;

    RilProperty *property = GetRilContextProperty();
    if (property != NULL) {
        property->Put(RIL_CONTEXT_IMS_REGISTRATION, mImsRegState);
    }

    RilLogV("[%s::%s] Ims Registration : %s", m_szSvcName,__FUNCTION__, mImsRegState==RIL_IMS_REGISTERED?"TRUE":"FALSE");
    // OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED);

    if (preImsRegState != mImsRegState) {
        OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);

        NetworkService* pNetworkService = (NetworkService*)GetCurrentService(RIL_SERVICE_NETWORK);
        if (pNetworkService != NULL) {
            pNetworkService->UpdateDbEccInfo();
        }
    }

    LEAVE_FUNC();

    return 0;
}

/**
 * DoAimsDefaultRequestHandler
 * @desc default AIMS Request handler
 */
int ImsService::DoAimsDefaultRequestHandler(Message *pMsg)
{
    UINT timeout = IMS_AIMS_REQUEST_TIMEOUT;

    RilLogI("[%s] Process AIMS Request", GetServiceName());
    NULL_REQ(pMsg);

    RawRequestData *rildata =(RawRequestData *)(pMsg->GetRequestData());

    RilLogV("[%s] AIMS Request: msgId=%d requestId=%d parameter=0x%p parameter length=%d",
            GetServiceName(), pMsg->GetMsgId(), rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());

    ProtocolImsBuilder builder;
    ModemData *pModemData = builder.BuildAimsPDU(rildata->GetReqId(), rildata->GetRawData(), rildata->GetSize());

    // if timeout value need to be changed, set it here
    switch( pMsg->GetMsgId() ) {
        case MSG_AIMS_HANGUP:
            timeout = 3000;
            break;
    }

    if (SendRequest(pModemData, timeout, pMsg->GetMsgId() + 1) < 0) {
        RilLogW("[%s] Failed to send AIMS PDU", GetServiceName());
        return -1;
    }

    return 0;
}

/**
 * OnAimsDefaultResponseHandler
 * @desc default AIMS Response handler
 */
int ImsService::OnAimsDefaultResponseHandler(Message *pMsg)
{
    RilLogI("[%s] Process AIMS Response", GetServiceName());
    NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    RilLogV("[%s] AIMS Response: msgId=%d errorCode=%d parameter=0x%p parameter length=%d",
            GetServiceName(), pMsg->GetMsgId(), errorCode, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength());
    }
    else {
        OnRequestComplete(errorCode);
    }

    return 0;
}


// #### Mandantory Functions ####

ImsService::ImsService(RilContext* pRilContext)
    : Service(pRilContext, RIL_SERVICE_IMS)
{
    mImsRegState = RIL_IMS_NOT_REGISTERED;
    int preImsRegState = mImsRegState;
    RilProperty *property = GetRilContextProperty();
    if (property != NULL) {
        property->Put(RIL_CONTEXT_IMS_REGISTRATION, mImsRegState);
    }

    m_srvccState = HANDOVER_COMPLETED;

    if (preImsRegState != mImsRegState)
        OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
}

ImsService::~ImsService()
{
}

int ImsService::OnCreate(RilContext * /* pRilContext */)
{
    ENTER_FUNC();

    //INT32 nRet = -1;

    LEAVE_FUNC();
    return 0;
}

void ImsService::OnDestroy()
{
    INT32 nRet = -1;

    ENTER_FUNC();

    do
    {
        nRet = 0;
    }while(FALSE);


    LEAVE_FUNC();
}

BOOL ImsService::OnHandleRequest(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg, false);

    INT32 nRet = -1;
    INT32 nMsgId = 0;


    do
    {
        nMsgId = pMsg->GetMsgId();

        // Processing Received Message(pMsg)
        RilLogV("%s::%s() nMsgId=%d",m_szSvcName,__FUNCTION__,nMsgId);
        nRet = 0;
        switch (nMsgId)
        {
            case MSG_IMS_SIM_AUTH:
                nRet = DoSimAuth(pMsg);
                break;
            case MSG_IMS_GET_GBA_AUTH:
                nRet = DoGetGbaAuth(pMsg);
                break;
            case MSG_AIMS_GET_REGISTRATION:
                nRet = Do_AIMS_GET_REGISTRATION(pMsg);
                break;

            // AIMS custom handler
            case MSG_AIMS_SET_CALL_WAITING:
                nRet = Do_AIMS_SET_CALL_WAITING(pMsg);
                break;

            // handle aims command during srvcc
            case MSG_AIMS_ANSWER:
                if ( m_srvccState == HANDOVER_STARTED ) {
                    // command will be decided after SRVCC HO complete
                    RilLogI("[%s::%s] Command is pending during SRVCC is processing", GetServiceName(), __FUNCTION__ );
                    nRet = 0;
                }
                else {
                    nRet = DoAimsDefaultRequestHandler(pMsg);
                }
                break;

            case MSG_AIMS_SET_TEMP_EMERGENCY_NUMBER:
                nRet = DoSetTempEmergencyNumber(pMsg);
                break;

            default:
            {
                RilLogE("%s::%s() !! ERROR !!, Unknown nMsgId = %d",m_szSvcName,__FUNCTION__,nMsgId);
                nRet = -3;
                break;
            }
        }
    }while(FALSE);


    LEAVE_FUNC();

    if(0 == nRet)
        return TRUE;
    else
        return FALSE;
}

BOOL ImsService::OnHandleSolicitedResponse(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg, false);

    INT32 nRet = -1;
    INT32 nMsgId = 0;

    do
    {
        nMsgId = pMsg->GetMsgId();

        // Processing Received Message(pMsg)
        RilLogV("%s::%s() nMsgId=%d",m_szSvcName,__FUNCTION__,nMsgId);
        nRet = 0;
        switch (nMsgId)
        {
            case MSG_IMS_SIM_AUTH_DONE:
                nRet = OnSimAuthDone(pMsg);
                break;
            case MSG_IMS_GET_GBA_AUTH_DONE:
                nRet = OnGetGbaAuthDone(pMsg);
                break;
            case MSG_AIMS_ANSWER_DONE:
                nRet = OnAimsDefaultResponseHandler(pMsg);
                break;
            case MSG_AIMS_SET_CALL_WAITING_DONE:
                nRet = On_AIMS_SET_CALL_WAITINGDone(pMsg);
                break;

            default:
            {
                RilLogE("%s::%s() !! ERROR !!, Unknown nMsgId = %d",m_szSvcName,__FUNCTION__,nMsgId);
                nRet = -3;
                break;
            }
        }
    }while(FALSE);


    LEAVE_FUNC();

    if(0 == nRet)
        return TRUE;
    else
        return FALSE;
}

BOOL ImsService::OnHandleUnsolicitedResponse(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg, false);

    INT32 nRet = -1;
    INT32 nMsgId = 0;

    do
    {
        nMsgId = pMsg->GetMsgId();

        RilLogI("%s::%s() nMsgId=%d",m_szSvcName,__FUNCTION__,nMsgId);
        nRet = 0;
        switch (nMsgId)
        {
            case MSG_IMS_SET_SRVCC_INFO:
                nRet = OnImsSetSrvccInfo(pMsg);
                break;
            //AIMS custom handler
            case MSG_IND_AIMS_REGISTRATION:
                nRet = OnUNSOL_AIMS_REGISTRATION(pMsg);
                break;
            //AIMS support end ---------------------

            default:
            {
                nRet = -3;
                RilLogE("%s::%s() !! ERROR !!, Unknown nMsgId = %d",m_szSvcName,__FUNCTION__,nMsgId);
                break;
            }
        }
    }while(FALSE);


    LEAVE_FUNC();

    if(0 == nRet)
        return TRUE;
    else
        return FALSE;
}

bool ImsService::IsPossibleToPassInRadioOffState(int /* request_id */)
{
    return true;
}

INT32 ImsService::GetImsRegState() {
    return mImsRegState;
}
