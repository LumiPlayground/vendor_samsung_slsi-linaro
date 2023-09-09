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
 * rilcontextwrapper.cpp
 *
 *  Created on: 2014. 11. 12.
 *      Author: sungwoo48.choi
 */
#include <telephony/librilutils.h>

#include "rilapplication.h"
#include "rilcontextwrapper.h"
#include "productfactory.h"
#include "servicefactory.h"
#include "rilparser.h"
#include "modemstatemonitor.h"
#include "servicemgr.h"
#include "iochannel.h"
#include "rcmmgr.h"
#include "modemcontrol.h"
#include "requestdata.h"
#include "rildata.h"
#include "rillog.h"
#include "service.h"
#include "open_carrier.h"
#include "cscservice.h"
#include "networkutils.h"
#include "rilcontextwrapperhandler.h"
#include <librilutils/textutils.h>
#include <librilutils/systemproperty.h>

static bool VDBG = true;

class ModemData;

IMPLEMENT_MODULE_TAG(RilContextWrapper, RilContext)

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG, format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_WARNING_LOG, format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_CRITICAL_LOG, format, ##__VA_ARGS__)

#define MAX_IO_FAILURE_TOLERANCE_TIME (60 * 1000000000LL)  // 1 MIN.
#define MAX_IO_FAILURE_TOLERANCE_COUNT 10
#define INVALID_TIMESTAMP 0x7FFFFFFFFFFFFFFF

#define CLEAN_POINTER_VAR(POINTER_VAR) \
    if (POINTER_VAR != NULL) {         \
        delete POINTER_VAR;            \
        POINTER_VAR = NULL;            \
    }

RilContextWrapper::RilContextWrapper(RilApplication *rilApp, const RilContextParam *param)
    : m_pRilApp(rilApp), m_pIoChannel(NULL), mIoFailureCount(0), mIoFailureTimestamp(INVALID_TIMESTAMP),
      m_pRcmMgr(NULL), m_pRilParser(NULL), m_pServiceMgr(NULL),
      m_RadioState(RADIO_STATE_UNAVAILABLE), mModemState(MS_OFFLINE), m_imsRegState(0) {
    memset(&m_Param, 0, sizeof(RilContextParam));
    m_rilSocketId = (RIL_SOCKET_ID)-1;
    if (param != NULL) {
        memcpy(&m_Param, param, sizeof(RilContextParam));
        m_rilSocketId = m_Param.socket_id;
    }

    m_nOpenCarrierIndex = (unsigned int)OC_UNKNOWN;

    m_pRilContextWrapperHandler = new RilContextWrapperHandler(this);
}

RilContextWrapper::~RilContextWrapper() {
    OnDestroy();
}

int RilContextWrapper::OnCreate() {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    if (m_pRilApp == NULL) {
        return -1;
    }

    RilLogV("[%s_%d] %s RilContextParam{socket_id=%d,iochannel_name=%s,ifprefix=%s,ifstart=%d,ifmaxsize=%d,productFactory=%p,serviceFactory=%p}",
            TAG, m_rilSocketId, __FUNCTION__,
            m_Param.socket_id, m_Param.iochannel_name,
            m_Param.ifprefix, m_Param.ifstart, m_Param.ifmaxsize,
            m_Param.productFactory, m_Param.serviceFactory);

    // check parameter
    if (*m_Param.iochannel_name == 0)
        return -1;

    if (*m_Param.ifprefix == 0)
        return -1;

    if (m_Param.ifmaxsize == 0)
        return -1;

    RilLogV("[%s_%d] %s Get ProductFactory", TAG, m_rilSocketId, __FUNCTION__);
    ProductFactory *pProductFactory = m_Param.productFactory;
    if (pProductFactory == NULL) {
        // use default product factory
        RilLogV("[%s_%d] %s Use Default ProductFactory", TAG, m_rilSocketId, __FUNCTION__);
        pProductFactory = ProductFactory::GetDefaultProductFactory(this);
    }

    RilLogV("[%s_%d] %s Get RIL Request Parser", TAG, m_rilSocketId, __FUNCTION__);
    // initialize RIL Request Parser instance
    m_pRilParser = pProductFactory->GetRequestParser(this);
    if (m_pRilParser == NULL) {
        RilLogE("[%s_%d] %s Fail to init RIL Request Parser", TAG, m_rilSocketId, __FUNCTION__);
        return -1;
    }

    // initialize IoChannel instance
    RilLogV("[%s_%d] %s Init IoChannel name=%s", TAG, m_rilSocketId, __FUNCTION__, m_Param.iochannel_name);
    m_pIoChannel = pProductFactory->GetIoChannel(this, m_Param.iochannel_name);
    if (m_pIoChannel == NULL) {
        RilLogE("[%s_%d] %s Fail to init IoChannel", TAG, m_rilSocketId, __FUNCTION__);
        return -1;
    }

    RilLogV("[%s_%d] %s Get ServiceFactory", TAG, m_rilSocketId, __FUNCTION__);
    // initialize ServiceMgr and Service instances
    ServiceFactory *pServiceFactory = m_Param.serviceFactory;
    if (pServiceFactory == NULL) {
        RilLogW("[%s_%d] %s Use Default ServiceFactory", TAG, m_rilSocketId, __FUNCTION__);
        pServiceFactory = ServiceFactory::GetDefaultServiceFactory();
    }

    m_pServiceMgr = pProductFactory->GetServiceManager(this);
    if (m_pServiceMgr == NULL || m_pServiceMgr->Init(pServiceFactory) < 0) {
        RilLogE("[%s_%d] %s Fail to init ServiceManager", TAG, m_rilSocketId, __FUNCTION__);
        return -1;
    }

    m_RequestWaitList.Clear();

    return 0;
}

int RilContextWrapper::OnStart() {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    if (m_pServiceMgr != NULL && m_pServiceMgr->StartServices() < 0) {
        RilLogE("[%s_%d] %s Fail to start all service instance", TAG, m_rilSocketId, __FUNCTION__);
        return -1;
    }

    if (m_pRilApp->GetUnitTestMode() == 0) {  // skip in unit Test mode
        m_pRcmMgr = new RadioControlMessageMgr(this, m_pIoChannel);
        if (m_pRcmMgr == NULL || m_pRcmMgr->Start() < 0) {
            RilLogE("[%s_%d] %s Fail to start RadioControlMessageMgr", TAG, m_rilSocketId, __FUNCTION__);
            return -1;
        }
    }

    return 0;
}

void RilContextWrapper::OnDestroy() {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    CLEAN_POINTER_VAR(m_pRcmMgr);
    CLEAN_POINTER_VAR(m_pIoChannel);
    CLEAN_POINTER_VAR(m_pServiceMgr);
    CLEAN_POINTER_VAR(m_pRilParser);
    CLEAN_POINTER_VAR(m_pRilContextWrapperHandler);
}

void RilContextWrapper::OnRequest(int request, void *data, unsigned int datalen, RIL_Token t) {
    OnRequest(request, data, datalen, t, 0);
}

void RilContextWrapper::OnRequest(int request, void *data, unsigned int datalen, RIL_Token t, long delayedMillis) {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    if (request < 0) {
        RilLogE("Invalid RIL Request ID");
        OnRequestComplete(t, request, RIL_E_REQUEST_NOT_SUPPORTED);
        return;
    }

    RequestData *pReqData = m_pRilParser->GetRequestData(request, t, data, datalen);
    if (pReqData == NULL || SendMessageDelayed(pReqData, delayedMillis) < 0) {
        RilLogE("[%s_%d] %s Send message failed", TAG, m_rilSocketId, __FUNCTION__);
        OnRequestComplete(t, request, RIL_E_REQUEST_NOT_SUPPORTED);
        if (pReqData)
            delete pReqData;
    }
}

RIL_RadioState RilContextWrapper::OnRadioStateRequest() {
    return GetCurrentRadioState();
}

void RilContextWrapper::OnRequestComplete(RIL_Token t, int request, int result, void *response /* = NULL*/, unsigned int responselen /* = 0*/) {
    RilLogI("[%s_%d] %s Request ID=%d", TAG, m_rilSocketId, __FUNCTION__, request);
    // TODO pre-process solicited response
    if (result == RIL_E_SUCCESS) {
        ProcessSolicitedResponse(request, response, responselen);
    }

    // notify the result
    if (m_pRilApp != NULL) {
        m_pRilApp->OnRequestComplete(this, t, (RIL_Errno)result, response, responselen);
    }
}

void RilContextWrapper::OnUnsolicitedResponse(int unsolResponse, const void *data /* = NULL*/, unsigned int datalen /* = 0*/) {
    RilLogI("[%s_%d] %s Unsol Rsp ID=%d", TAG, m_rilSocketId, __FUNCTION__, unsolResponse);

    // pre-process unsolicited response
    ProcessUnsolicitedResponse(unsolResponse, data, datalen);

    // notify the result
    if (m_pRilApp != NULL) {
        m_pRilApp->OnUnsolicitedResponse(this, unsolResponse, data, datalen);
    }
}

void RilContextWrapper::OnRequestAck(RIL_Token t) {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);

    if (m_pRilApp != NULL) {
        m_pRilApp->OnRequestAck(t);
    }
}

void RilContextWrapper::OnRequestComplete(RequestData *req, int result, void *response /* = NULL*/, unsigned int responselen /* = 0*/) {
    if (req != NULL) {
        OnRequestComplete(req->GetToken(), req->GetReqId(), result, response, responselen);
    }
}

void RilContextWrapper::OnRequestTimeout(RequestData *req, unsigned int token) {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);

    MessageHistoryRecord record = m_RequestWaitList.Find(token);
    if (record.IsValid()) {
        RemoveRequestHistory(token);
    }

    if (req != NULL) {
        OnRequestComplete(req, RIL_E_INTERNAL_ERR);
    }
}

RIL_SOCKET_ID RilContextWrapper::GetRilSocketId() {
    return m_rilSocketId;
}

RIL_CardState RilContextWrapper::GetUiccCardState() {
    RIL_CardState cardState = RIL_CARDSTATE_ABSENT;
    // RilApplication property
    RilProperty *pRilAppProperty = GetApplicationProperty();
    if (pRilAppProperty != NULL) {
        // overallCardState
        // bit 0 : SIM1 card state
        // bit 1 : SIM2 card state
        int overallCardState = pRilAppProperty->GetInt(RIL_SIM_OVERALL_CARDSTATE, 0);
        if (overallCardState & (0x01 << GetRilSocketId())) {
            cardState = RIL_CARDSTATE_PRESENT;
        }
    }
    return cardState;
}

ServiceMgr *RilContextWrapper::GetServiceManager() {
    return m_pServiceMgr;
}

RilProperty *RilContextWrapper::GetApplicationProperty() {
    if (m_pRilApp != NULL)
        return m_pRilApp->GetProperty();
    return NULL;
}

void RilContextWrapper::OnModemStateChanged(int state) {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    int oldState = mModemState;
    mModemState = state;

    // state not changed
    if (oldState == state)
        return;

    if (state == MS_CRASH_EXIT) {
        if (m_pRcmMgr != NULL) {
            m_pRcmMgr->StopRcmMonitoring();
        }
    }

    RilDataInts *rildata = new RilDataInts(1);
    if (rildata != NULL) {
        RilLogW("[%d] Broadcast new modem state(%d)", GetRilSocketId(), state);
        rildata->SetInt(0, state);
        BroadcastSystemMessage(MSG_SYSTEM_MODEM_STATE_CHANGED, rildata);
    }

    if (oldState == MS_ONLINE && state != MS_ONLINE) {
        RilLogW("[%d] Broadcast system reset", GetRilSocketId());
        BroadcastSystemMessage(MSG_SYSTEM_RESET);
    }
}

int RilContextWrapper::Send(ModemData *pModemData, UINT nDstServiceId, UINT nResult, RIL_Token token /* = NULL*/) {
    //RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    int nRet = -1;
    if (pModemData != NULL) {
        // TODO record Tx history
        RecordRequestHistory(nDstServiceId, nDstServiceId, pModemData->GetToken(), nResult);
        nRet = this->Send((char *)pModemData->GetRawData(), pModemData->GetLength(), token);

        if (nRet < 0) {
            RemoveRequestHistory(pModemData->GetToken());
        }
    }

    return nRet;
}

int RilContextWrapper::Send(void *data, unsigned int datalen, RIL_Token token /* = NULL*/) {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);

    if (m_pRilApp->GetUnitTestMode() > 0) {  // unit Test mode
        int ret = m_pRilApp->UpdateReqModemData(token, data, datalen);
        if (ret == -1) {
            RilLogI("[%s_%d] %s, Sending modem data to test unit fails because it is not expected by test unit",
                    TAG, m_rilSocketId, __FUNCTION__);
        }
        return ret;
    }

    if (m_pRcmMgr->Write((char *)data, datalen) != 0) {
        RilLogE("Fail to write to the IO Channel.");
        if (m_RadioState == RADIO_STATE_OFF || m_RadioState == RADIO_STATE_ON) {
            if (mIoFailureCount == 0) {
                mIoFailureTimestamp = ril_nano_time();
            }
            mIoFailureCount++;
            uint64_t current = ril_nano_time();
            uint64_t diff = current - mIoFailureTimestamp;
            RilLogE("[%d] ####### Warning! I/O failure(s) are %zu!!!(%llu) #######",
                    GetRilSocketId(), mIoFailureCount, diff);
            if (mIoFailureCount >= MAX_IO_FAILURE_TOLERANCE_COUNT) {
                if (diff > MAX_IO_FAILURE_TOLERANCE_TIME) {
                    mIoFailureCount = 0;
                    mIoFailureTimestamp = INVALID_TIMESTAMP;
                    // Force CP crash
                    RilLogE("###################################################################################");
                    RilLogE("####### Force CP due to continuing I/O failures. Need to check by CP or CPIF! #######");
                    RilLogE("###################################################################################");
                    ModemControl::CrashModem("CP crash due to continuing I/O failures");
                }
            }
        }
        return -1;
    }

    // clear I/O failure status
    mIoFailureCount = 0;
    mIoFailureTimestamp = INVALID_TIMESTAMP;
    return 0;
}

UINT RilContextWrapper::RecordRequestHistory(UINT nSrcServiceId, UINT nDestServiceId, UINT nToken, UINT nResult) {
    RilLogV("[%s_%d] %s token=0x%04X", TAG, m_rilSocketId, __FUNCTION__, nToken);
    MessageHistoryRecord record = MessageHistoryRecord::newInstance(nSrcServiceId, nDestServiceId, nToken, nResult);
    if (record.IsValid()) {
        m_RequestWaitList.Push(record);
    }
    return 0;
}

void RilContextWrapper::RemoveRequestHistory(UINT nToken) {
    RilLogV("[%s_%d] %s  token=0x%04X", TAG, m_rilSocketId, __FUNCTION__, nToken);
    m_RequestWaitList.Remove(nToken);
}

int RilContextWrapper::ProcessModemData(void *data, unsigned int datalen) {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    if (data != NULL && datalen > 0) {
        ModemData *pModemData = new ModemData((char *)data, datalen, false /*tx*/);
        if (SendMessage(pModemData) < 0) {
            delete pModemData;
            pModemData = NULL;
        }
    }
    return 0;
}

void RilContextWrapper::ClearGarbage(UINT nToken) {
    if (nToken != TOKEN_INVALID) {
        RemoveRequestHistory(nToken);
    }
}

bool RilContextWrapper::IsInRequestWaitHistory(UINT nToken) {
    bool result = FALSE;
    MessageHistoryRecord record = m_RequestWaitList.Find(nToken);

    if (record.IsValid()) {
        result = TRUE;
    }

    return result;
}

int RilContextWrapper::SendMessage(RequestData *pRequestData) {
    return SendMessageDelayed(pRequestData, 0);
}

int RilContextWrapper::SendMessageDelayed(RequestData *pRequestData, long delayMillis) {
    //RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    int nServiceId = -1;
    int nMessageId = 0;

    if (pRequestData == NULL) {
        RilLogE("[%s_%d] SendMessage Invalid parameter : pRequestData", TAG, m_rilSocketId);
        return -1;
    }

    if (m_pServiceMgr == NULL || m_pServiceMgr->RouteRequest(pRequestData, nServiceId, nMessageId) < 0) {
        RilLogE("[%s_%d] SendMessage RIL Request Routing error - %d", TAG, m_rilSocketId, pRequestData->GetReqId());
        return -1;
    }

    RilLogV("[%s_%d] SendMessage RIL Request Routing : RCM ID=%d Service ID=%d Message ID=%04d delayMillis=%ld",
            TAG, m_rilSocketId, pRequestData->GetReqId(), nServiceId, nMessageId, delayMillis);
    Message *pMsg = Message::ObtainMessage(pRequestData, nServiceId, nMessageId);

    // Check Async message
    if (m_pServiceMgr->IsAsyncMessage(nServiceId, nMessageId) == true) {
        pMsg->SetDirection(ASYNC_REQUEST);
        pMsg->SetAsyncMsgReqStatus(ASYNC_MSG_STATUS_INITIATED);
    }

    if (m_pServiceMgr->SendMessageDelayed(pMsg, delayMillis) < 0) {
        RilLogE("[%s_%d] SendMessage SendMessage error", TAG, m_rilSocketId);
        if (pMsg) {
            delete pMsg;
        }
        OnRequestComplete(pRequestData, RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    return 0;
}

int RilContextWrapper::SendMessage(ModemData *pModemData) {
    //RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);

    if (pModemData == NULL) {
        RilLogE("Invalid parameter : pModemData");
        return -1;
    }

    int nServiceId = -1;
    int nMessageId = 0;
    if (pModemData->IsSolicitedResponse()) {
        MessageHistoryRecord record = m_RequestWaitList.Find(pModemData->GetToken());
        if (record.IsValid()) {
            nServiceId = record.GetDestServiceId();
            nMessageId = record.GetResult();
            m_RequestWaitList.Remove(pModemData->GetToken());
        } else {
            // drop the solicited response because timeout or negligible
            return -1;
        }
    } else if (pModemData->IsUnsolicitedResponse()) {
        if (m_pServiceMgr->RouteProtocolInd(pModemData->GetMessageId(), nServiceId, nMessageId)) {
            RilLogV("[%s_%d] %s Indication: no route info for Message ID=%04d", TAG, m_rilSocketId,
                    __FUNCTION__, pModemData->GetMessageId());
            return -1;
        }
    } else {
        RilLogE("[%s_%d] %s RCM Type : Unsupported(%d)", TAG, m_rilSocketId, __FUNCTION__, pModemData->GetType());
        return -1;
    }

    RilLogV("[%s_%d] %s RCM Response Routing : Type=%04d, RCM ID=0x%04X Service ID=%d Message ID=%04d",
            TAG, m_rilSocketId, __FUNCTION__, pModemData->GetType(),
            pModemData->GetMessageId(), nServiceId, nMessageId);
    Message *pMsg = Message::ObtainMessage(pModemData, nServiceId, nMessageId);
    if (m_pServiceMgr->SendMessage(pMsg) < 0) {
        RilLogE("[%s_%d] %s SendMessage error", TAG, m_rilSocketId, __FUNCTION__);
        if (pMsg)
            delete pMsg;

        return 0;
    }
    return 0;
}

RIL_RadioState RilContextWrapper::GetCurrentRadioState() const {
    return m_RadioState;
}

void RilContextWrapper::SetRadioState(RIL_RadioState radioState) {
    RilLogI("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    switch (radioState) {
    case RADIO_STATE_UNAVAILABLE:
        mServiceState.setNullState();
        SetNoSim();
        break;
    case RADIO_STATE_OFF:
        mServiceState.setNullState();
        break;
    case RADIO_STATE_ON:
        break;
    default:
        RilLogV("Unsupported Radio State %d", radioState);
        return;
    }  // end switch ~

    RilLogV("[%s_%d] %s Overall Radio State changed from %d to %d", TAG, m_rilSocketId, __FUNCTION__, m_RadioState, radioState);
    m_RadioState = radioState;
}

void RilContextWrapper::ProcessSolicitedResponse(int id, const void *data, unsigned int length) {
    //RilLogI("[%s_%d] %s Request ID=%d", TAG, m_rilSocketId, __FUNCTION__, id);
    if (data == NULL || length <= 0) {
        return;
    }

    int requestId = DECODE_REQUEST(id);
    //int halVer = DECODE_HAL(id);

    switch (requestId) {
    case RIL_REQUEST_OPERATOR:
    case RIL_REQUEST_DATA_REGISTRATION_STATE:
    case RIL_REQUEST_VOICE_REGISTRATION_STATE:
    case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:
        ProcessNetworkServiceState(requestId, data, length);
        break;
    case RIL_REQUEST_GET_SIM_STATUS:
        m_pRilContextWrapperHandler->ProcessIccCardStatus(data, length);
        break;
    case RIL_REQUEST_GET_IMSI:
        m_pRilContextWrapperHandler->ProcessImsi(data, length);
        break;
    case RIL_REQUEST_DEVICE_IDENTITY:
        m_pRilContextWrapperHandler->ProcessDeviceIdentity(data, length);
        break;
    default:
        break;
    }
}

void RilContextWrapper::ProcessUnsolicitedResponse(int id, const void *data, unsigned int length) {
    //RilLogI("[%s_%d] %s Unsol Rsp ID=%d", TAG, m_rilSocketId, __FUNCTION__, id);
    //Message *message = NULL;

    if (id == RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED) {
        RilLogV("[%s_%d] %s Process : RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED", TAG, m_rilSocketId, __FUNCTION__);
        if (data != NULL && length == sizeof(int)) {
            RIL_RadioState radioState = (RIL_RadioState) * (int *)data;
            if (m_RadioState != radioState) {  //radio state sync with f/w (not the real RIL state)
                SetRadioState(radioState);
            }
        }
    } else if (id == RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED) {
        RilLogV("[%s_%d] %s Process : RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED", TAG, m_rilSocketId, __FUNCTION__);
        int regState = m_RilContextProperty.GetInt(RIL_CONTEXT_IMS_REGISTRATION);
        if (m_imsRegState != regState) {
            m_imsRegState = regState;
            RilDataInts *rildata = new RilDataInts(1);
            if (rildata != NULL) {
                rildata->SetInt(0, m_imsRegState);
                BroadcastSystemMessage(MSG_SYSTEM_IMS_REGISTRATION_STATE_CHANGED, rildata);
                RilLogV("[%s_%d] %s Broadcast System Message : "
                        "MSG_SYSTEM_IMS_REGISTRATION_STATE_CHANGED",
                        TAG, m_rilSocketId, __FUNCTION__);
            }
        }
    }
}

int RilContextWrapper::BroadcastSystemMessage(int messageId, RilData *data /* = NULL*/) {
    RilLogI("[%s_%d] %s messageId:%d", TAG, m_rilSocketId, __FUNCTION__, messageId);
    if (m_pServiceMgr != NULL) {
        m_pServiceMgr->BroadcastSystemMessage(messageId, data);
    } else {
        delete data;
    }
    return 0;
}

RilContext *RilContextWrapper::GetRilContext(RIL_SOCKET_ID socket_id) {
    RilLogV("[%s_%d] %s socket_id:%d", TAG, m_rilSocketId, __FUNCTION__, socket_id);
    if (socket_id == m_rilSocketId) {
        RilLogV("[%s_%d] %s return myself", TAG, m_rilSocketId, __FUNCTION__);
        return this;
    } else {
        if (m_pRilApp != NULL) {
            RilContext *pRilContext = m_pRilApp->GetRilContext(socket_id);
            RilLogV("[%s_%d] %s return socketid(%d)'s RilContext", TAG, m_rilSocketId, __FUNCTION__, socket_id);
            return pRilContext;
        } else {
            RilLogV("[%s_%d] %s: RilApplication == NULL ", TAG, m_rilSocketId, __FUNCTION__);
        }
    }
    return NULL;
}

RilContext *RilContextWrapper::GetOppositeRilContext(void) {
    RIL_SOCKET_ID TargetSocketId = (RIL_SOCKET_ID)((int)RIL_SOCKET_NUM - 1);
    if (m_rilSocketId == RIL_SOCKET_NUM - 1)
        TargetSocketId = RIL_SOCKET_1;

    if (TargetSocketId == m_rilSocketId) {
        RilLogW("%s Target socket ID(%d) is same as mine. Compile option could be for Single SIM solution.", __FUNCTION__, TargetSocketId);
        return NULL;
    }

    RilLogV("%s Get RilContext(%d)", __FUNCTION__, TargetSocketId);
    return GetRilContext(TargetSocketId);
}

Service *RilContextWrapper::GetService(int nServiceId) {
    //RilLogV("[%s_%d] %s ", TAG, m_rilSocketId, __FUNCTION__);
    if (m_pServiceMgr != NULL) {
        //RilLogV("[%s_%d] %s return service(%d)", TAG, m_rilSocketId, __FUNCTION__, nServiceId);
        return m_pServiceMgr->FindService(nServiceId);
    }
    RilLogV("[%s_%d] %s Cannot find service (%d)", TAG, m_rilSocketId, __FUNCTION__, nServiceId);
    return NULL;
}

unsigned int RilContextWrapper::GetOpenCarrierIndex() {
    RilLogV("[%s_%d] %s return open carrier Index : 0x%x", TAG, m_rilSocketId, __FUNCTION__, m_nOpenCarrierIndex);
    return m_nOpenCarrierIndex;
}

void RilContextWrapper::SetOpenCarrierIndex(const char *mccmnc) {
    char buf[128] = {
        0,
    };
    m_nOpenCarrierIndex = GetOcNameByMccMnc(mccmnc);
    snprintf(buf, sizeof(buf), "%u", m_nOpenCarrierIndex);

    switch (m_rilSocketId) {
    case RIL_SOCKET_1:
        SystemProperty::Set(RIL_SIM1_OPEN_CARRIER_ID, buf);
        break;
#if (SIM_COUNT >= 2)
    case RIL_SOCKET_2:
        SystemProperty::Set(RIL_SIM2_OPEN_CARRIER_ID, buf);
        break;
#endif
    default:
        RilLogE("[%s_%d] %s invaild ril socket ID, open carrier index(0x%x)", TAG, m_rilSocketId, __FUNCTION__, m_nOpenCarrierIndex);
        break;
    }

    RilLogV("[%s_%d] %s set open carrier index : 0x%x", TAG, m_rilSocketId, __FUNCTION__, m_nOpenCarrierIndex);
}

// set card status into RilContextProperty
void RilContextWrapper::SetCardStatus(RIL_CardStatus_v6 *cardStatus) {
    if (cardStatus != NULL) {
        // RilContext property
        if (cardStatus->card_state != RIL_CARDSTATE_PRESENT) {
            m_RilContextProperty.Put("gsm_umts_subscription_app_index", -1);
            m_RilContextProperty.Put("cdma_subscription_app_index", -1);
            m_RilContextProperty.Put("ims_subscription_app_index", -1);
            RilLog("[%d] %s CardState ABSENT or ERROR", GetRilSocketId(), __FUNCTION__);
        } else {
            m_RilContextProperty.Put("gsm_umts_subscription_app_index", cardStatus->gsm_umts_subscription_app_index);
            m_RilContextProperty.Put("cdma_subscription_app_index", cardStatus->cdma_subscription_app_index);
            m_RilContextProperty.Put("ims_subscription_app_index", cardStatus->ims_subscription_app_index);
            RilLog("[%d] %s SIM/USIM: %s RUIM/CSIM: %s ISIM: %s", GetRilSocketId(), __FUNCTION__,
                   cardStatus->gsm_umts_subscription_app_index >= 0 ? "exist" : "not exist",
                   cardStatus->cdma_subscription_app_index >= 0 ? "exist" : "not exist",
                   cardStatus->ims_subscription_app_index >= 0 ? "exist" : "not exist");
        }

        // RilApplication property
        if (GetApplicationProperty() != NULL) {
            RilProperty *pRilAppProperty = GetApplicationProperty();
            // overallCardState
            // bit 0 : SIM1 card state
            // bit 1 : SIM2 card state
            int overallCardState = pRilAppProperty->GetInt(RIL_SIM_OVERALL_CARDSTATE, 0);
            if (cardStatus->card_state == RIL_CARDSTATE_PRESENT) {
                overallCardState |= (0x01 << GetRilSocketId());
            } else {
                overallCardState &= ~(0x01 << GetRilSocketId());
            }
            pRilAppProperty->Put(RIL_SIM_OVERALL_CARDSTATE, overallCardState);
        }
    }
}

void RilContextWrapper::SetCardStatus(const SimCardStatus &cardStatus) {
    if (mCardStatus != cardStatus) {
        mCardStatus = cardStatus;
        auto rildata = new RilDataTemplate<const SimCardStatus &>(
            [=]() -> const SimCardStatus & { return mCardStatus; });
        BroadcastSystemMessage(MSG_SYSTEM_SIM_STATUS_CHANGED_2, rildata);
    }
}

void RilContextWrapper::SetNoSim() {
    RIL_CardStatus_v6 dummyCardStatus;
    memset(&dummyCardStatus, 0, sizeof(dummyCardStatus));
    dummyCardStatus.card_state = RIL_CARDSTATE_ABSENT;
    dummyCardStatus.gsm_umts_subscription_app_index = -1;
    dummyCardStatus.cdma_subscription_app_index = -1;
    dummyCardStatus.ims_subscription_app_index = -1;
    SetCardStatus(&dummyCardStatus);

    // update SimCardStatus
    {
        SimCardStatus dummpyCardStatus = mCardStatus;
        dummpyCardStatus.setNoSim();
        SetCardStatus(dummpyCardStatus);
    }
}

/**
 * static
 */
RilContext *RilContextWrapper::NewInstance(RilApplication *rilApp, const RilContextParam *param) {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    RilContextWrapper *rilcontext = new RilContextWrapper(rilApp, param);
    return rilcontext;
}

void RilContextWrapper::ProcessNetworkServiceState(int requestId, const void *data, unsigned int length) {
    bool hasOperatorNumericChanged = false;
    bool hasVoiceRegStateChanged = false;
    bool hasVoiceRadioTechChanged = false;
    bool hasDataRegStateChanged = false;
    bool hasDataRadioTechChanged = false;
    bool hasLteVopsStatusChanged = false;
    bool hasNrStatusChanged = false;

    ServiceState newState = mServiceState;
    ServiceState &state = mServiceState;
    switch (requestId) {
    case RIL_REQUEST_OPERATOR: {
        ProcessOperatorInfo(newState, data, length);
        hasOperatorNumericChanged = (!TextUtils::Equals(state.getOperatorNumeric(), newState.getOperatorNumeric())) || (!TextUtils::Equals(state.getOperatorNumericRaw(), newState.getOperatorNumericRaw()));
        break;
    }
    case RIL_REQUEST_DATA_REGISTRATION_STATE: {
        ProcessNetworkRegStateResult(newState, NETWORK_DOMAIN_PS, data, length);
        hasDataRegStateChanged = (state.getDataRegState() != newState.getDataRegState());
        hasDataRadioTechChanged = (state.getDataRadioTechnology() != newState.getDataRadioTechnology()) ||
                                  (state.isUsingCarrierAggregation() != newState.isUsingCarrierAggregation());
        hasLteVopsStatusChanged = (state.getLteVopsSupport() != newState.getLteVopsSupport()) ||
                                  (state.getLteEmcBearerSupport() != newState.getLteEmcBearerSupport());
        hasNrStatusChanged = (state.getNrStatus() != newState.getNrStatus());
        break;
    }
    case RIL_REQUEST_VOICE_REGISTRATION_STATE: {
        ProcessNetworkRegStateResult(newState, NETWORK_DOMAIN_CS, data, length);
        hasVoiceRegStateChanged = (state.getVoiceRegState() != newState.getVoiceRegState());
        hasVoiceRadioTechChanged = (state.getVoiceRadioTechnology() != newState.getVoiceRadioTechnology());
        break;
    }
    case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: {
        ProcessNetworkSelectionMode(newState, data, length);
        break;
    }
    default:
        return;
    }

    // support legacy. Service::OnVoiceRegistrationStateChanged()
    if (hasVoiceRegStateChanged) {
        RilDataInts *rildata = new RilDataInts(1);
        if (rildata != NULL) {
            rildata->SetInt(0, newState.getVoiceRegState());
            BroadcastSystemMessage(MSG_SYSTEM_VOICE_REGISTRTION_STATE_CHANGED, rildata);
            RilLogV("[%s_%d] %s Broadcast System Message : MSG_SYSTEM_VOICE_REGISTRTION_STATE_CHANGED", TAG, m_rilSocketId, __FUNCTION__);
        }
    }

    // support legacy. Service::OnDataRegistrationStateChanged()
    if (hasDataRegStateChanged) {
        RilDataInts *rildata = new RilDataInts(1);
        if (rildata != NULL) {
            rildata->SetInt(0, newState.getDataRegState());
            BroadcastSystemMessage(MSG_SYSTEM_DATA_REGISTRTION_STATE_CHANGED, rildata);
            RilLogV("[%s_%d] %s Broadcast System Message : MSG_SYSTEM_DATA_REGISTRTION_STATE_CHANGED", TAG, m_rilSocketId, __FUNCTION__);
        }
    }

    bool hasChanged = (hasVoiceRegStateChanged || hasVoiceRadioTechChanged ||
                       hasDataRegStateChanged || hasDataRadioTechChanged ||
                       hasLteVopsStatusChanged || hasNrStatusChanged ||
                       hasOperatorNumericChanged);
    if (hasChanged) {
        RilLog("[%d] hasChanged(%s)\n"
               "    hasOperatorNumericChanged:%d\n"
               "    hasVoiceRegStateChanged:%d\n"
               "    hasVoiceRadioTechChanged:%d\n"
               "    hasDataRegStateChanged:%d\n"
               "    hasLteVopsStatusChanged:%d\n"
               "    hasNrStatusChanged:%d",
               GetRilSocketId(), hasChanged ? "true" : "false",
               hasOperatorNumericChanged,
               hasVoiceRegStateChanged, hasVoiceRadioTechChanged,
               hasDataRegStateChanged, hasLteVopsStatusChanged, hasNrStatusChanged);
        if (VDBG) {
            RilLogV("[%d] OldState=%s \n      NewState=%s",
                    GetRilSocketId(), state.toString().c_str(), newState.toString().c_str());
        }
        mServiceState = newState;
        BroadcastSystemMessage(MSG_SYSTEM_SERVICE_STATE_CHANGED);
    }
}

void RilContextWrapper::ProcessOperatorInfo(ServiceState &ss, const void *data, unsigned int datalen) {
    if (data != NULL && datalen % sizeof(char *) == 0) {
        size_t size = datalen / sizeof(char *);
        if (size >= 3) {
            string alphaLong = NetworkUtils::removeLteEnhancedAvailableFromAlphaTag((((char **)data)[0] != NULL) ? ((char **)data)[0] : "");
            string alphaShort = NetworkUtils::removeLteEnhancedAvailableFromAlphaTag((((char **)data)[1] != NULL) ? ((char **)data)[1] : "");
            string numeric = (((char **)data)[2] != NULL) ? ((char **)data)[2] : "";
            string alphaLongRaw = (((char **)data)[3] != NULL) ? ((char **)data)[3] : "";
            string alphaShortRaw = (((char **)data)[4] != NULL) ? ((char **)data)[4] : "";
            string numericRaw = (((char **)data)[5] != NULL) ? ((char **)data)[5] : "";
            ss.setOperatorNumeric(numeric);
            ss.setOperatorName(alphaLong, alphaShort);
            ss.setOperatorNumericRaw(numericRaw);
            ss.setOperatorNameRaw(alphaLongRaw, alphaShortRaw);
        }
    }
}

void RilContextWrapper::ProcessNetworkSelectionMode(ServiceState &ss, const void *data, unsigned int datalen) {
    if (data != NULL && datalen == sizeof(int)) {
        bool isManualNetworkSelection = (((int *)data)[0] == 1);
        ss.setIsManualSelection(isManualNetworkSelection);
    }
}

void RilContextWrapper::ProcessNetworkRegStateResult(ServiceState &ss, int domain, const void *networkStateResult, unsigned int datalen) {
    if (networkStateResult == NULL || datalen == 0)
        return;

    if (domain == NETWORK_DOMAIN_CS) {
        // radio 1.0 ~ 1.2
        if (datalen == sizeof(RIL_VoiceRegistrationStateResponse)) {
            ProcessVoiceRegStateResult(ss, (RIL_VoiceRegistrationStateResponse *)networkStateResult);
        } else if (datalen == sizeof(RIL_VoiceRegistrationStateResponse_V1_2)) {
            ProcessVoiceRegStateResult(ss, (RIL_VoiceRegistrationStateResponse_V1_2 *)networkStateResult);
        } else if (datalen == sizeof(RIL_RegStateResult)) {
            ProcessVoiceRegStateResult(ss, (RIL_RegStateResult *)networkStateResult);
        } else if (datalen == sizeof(RIL_RegStateResult_V1_6)) {
            ProcessVoiceRegStateResult(ss, (RIL_RegStateResult_V1_6 *)networkStateResult);
        }
    } else if (domain == NETWORK_DOMAIN_PS) {
        // radio 1.0 ~ 1.4
        if (datalen == sizeof(RIL_DataRegistrationStateResponse)) {
            ProcessDataRegStateResult(ss, (RIL_DataRegistrationStateResponse *)networkStateResult);
        } else if (datalen == sizeof(RIL_DataRegistrationStateResponse_V1_2)) {
            ProcessDataRegStateResult(ss, (RIL_DataRegistrationStateResponse_V1_2 *)networkStateResult);
        } else if (datalen == sizeof(RIL_DataRegistrationStateResponse_V1_4)) {
            ProcessDataRegStateResult(ss, (RIL_DataRegistrationStateResponse_V1_4 *)networkStateResult);
        } else if (datalen == sizeof(RIL_RegStateResult)) {
            ProcessDataRegStateResult(ss, (RIL_RegStateResult *)networkStateResult);
        } else if (datalen == sizeof(RIL_RegStateResult_V1_6)) {
            ProcessDataRegStateResult(ss, (RIL_RegStateResult_V1_6 *)networkStateResult);
        }
    }
}

void RilContextWrapper::ProcessVoiceRegStateResult(ServiceState &ss, RIL_VoiceRegistrationStateResponse *voiceRegResult) {
    //RilLogV("Process : RIL_REQUEST_VOICE_REGISTRATION_STATE");
    if (voiceRegResult != NULL) {
        ss.setVoiceRegState((int)voiceRegResult->regState);
        ss.setVoiceRadioTechnology((int)voiceRegResult->rat);

        GetProperty()->Put(RIL_CONTEXT_NET_VOICE_REGISTRATION_STATE, ss.getVoiceRegState());
        GetProperty()->Put(RIL_CONTEXT_NET_VOICE_RADIO_TECH, ss.getVoiceRadioTechnology());
    }
}

void RilContextWrapper::ProcessVoiceRegStateResult(ServiceState &ss, RIL_VoiceRegistrationStateResponse_V1_2 *voiceRegResult) {
    // enough casting a pointer type of RIL_VoiceRegistrationStateResponse.
    return ProcessVoiceRegStateResult(ss, (RIL_VoiceRegistrationStateResponse *)voiceRegResult);
}

void RilContextWrapper::ProcessVoiceRegStateResult(ServiceState &ss, RIL_RegStateResult *voiceRegResult) {
    if (voiceRegResult != NULL) {
        ss.setVoiceRegState((int)voiceRegResult->regState);
        ss.setVoiceRadioTechnology((int)voiceRegResult->rat);

        GetProperty()->Put(RIL_CONTEXT_NET_VOICE_REGISTRATION_STATE, ss.getVoiceRegState());
        GetProperty()->Put(RIL_CONTEXT_NET_VOICE_RADIO_TECH, ss.getVoiceRadioTechnology());
    }
}

void RilContextWrapper::ProcessVoiceRegStateResult(ServiceState &ss, RIL_RegStateResult_V1_6 *voiceRegResult) {
    if (voiceRegResult != NULL) {
        ProcessVoiceRegStateResult(ss, (RIL_RegStateResult *)voiceRegResult);
    }
}

void RilContextWrapper::ProcessDataRegStateResult(ServiceState &ss, RIL_DataRegistrationStateResponse *dataRegResult) {
    //RilLogV("Process : RIL_REQUEST_DATA_REGISTRATION_STATE");
    if (dataRegResult != NULL) {
        ss.setDataRegState((int)dataRegResult->regState);
        ss.setDataRadioTechnology((int)dataRegResult->rat);
        GetProperty()->Put(RIL_CONTEXT_NET_DATA_REGISTRATION_STATE, ss.getDataRegState());
        GetProperty()->Put(RIL_CONTEXT_NET_DATA_RADIO_TECH, ss.getDataRadioTechnology());
    }
}

void RilContextWrapper::ProcessDataRegStateResult(ServiceState &ss, RIL_DataRegistrationStateResponse_V1_2 *dataRegResult) {
    ProcessDataRegStateResult(ss, (RIL_DataRegistrationStateResponse *)dataRegResult);
}

void RilContextWrapper::ProcessDataRegStateResult(ServiceState &ss, RIL_DataRegistrationStateResponse_V1_4 *dataRegResult) {
    if (dataRegResult != NULL) {
        // pre-precessing for legacy
        ProcessDataRegStateResult(ss, (RIL_DataRegistrationStateResponse *)dataRegResult);

        // update LTE VoPS and emergency available
        ss.setLteVopsSupport(dataRegResult->lteVopsInfo.isVopsSupported);
        ss.setLteEmcBearerSupport(dataRegResult->lteVopsInfo.isEmcBearerSupported);

        // update NR indicator state
        ss.updateNrStatus(dataRegResult->nrIndicators.isEndcAvailable,
                          dataRegResult->nrIndicators.isDcNrRestricted,
                          dataRegResult->nrIndicators.isNrAvailable);
    }
}

void RilContextWrapper::ProcessDataRegStateResult(ServiceState &ss, RIL_RegStateResult *dataRegResult) {
    if (dataRegResult != NULL) {
        // pre-precessing for legacy
        ss.setDataRegState((int)dataRegResult->regState);
        ss.setDataRadioTechnology((int)dataRegResult->rat);
        GetProperty()->Put(RIL_CONTEXT_NET_DATA_REGISTRATION_STATE, ss.getDataRegState());
        GetProperty()->Put(RIL_CONTEXT_NET_DATA_RADIO_TECH, ss.getDataRadioTechnology());

        int ran = NetworkUtils::getRadioTechnologyToAccessNetworkType((int)dataRegResult->regState);
        if (ACCESS_NETWORK_EUTRAN == ran || ACCESS_NETWORK_NGRAN == ran) {
            // update LTE VoPS and emergency available
            ss.setLteVopsSupport(dataRegResult->accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isVopsSupported);
            ss.setLteEmcBearerSupport(dataRegResult->accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isEmcBearerSupported);

            // update NR indicator state
            ss.updateNrStatus(dataRegResult->accessTechnologySpecificInfo.eutranInfo.nrIndicators.isEndcAvailable,
                              dataRegResult->accessTechnologySpecificInfo.eutranInfo.nrIndicators.isDcNrRestricted,
                              dataRegResult->accessTechnologySpecificInfo.eutranInfo.nrIndicators.isNrAvailable);
        }
    }
}

void RilContextWrapper::ProcessDataRegStateResult(ServiceState &ss, RIL_RegStateResult_V1_6 *dataRegResult) {
    if (dataRegResult != NULL) {
        // pre-precessing for legacy
        ProcessDataRegStateResult(ss, (RIL_RegStateResult *)dataRegResult);

        // TODO: need to check nr vops, emc, emf
    }
}

void RilContextWrapper::ResetModem(const char *reason) {
    if (m_pRilApp != NULL) {
        m_pRilApp->ResetModem(reason);
    }
}
