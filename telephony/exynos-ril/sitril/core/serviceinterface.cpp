/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include "message.h"
#include "modemstatemonitor.h"
#include "protocolutils.h"
#include "requestdata.h"
#include "rillog.h"
#include "serviceinterface.h"
#include "simcardstatus.h"
#include "rilcontext.h"
#include "servicestate.h"
#include <unistd.h>
#include <fcntl.h>

static bool DBG = true;
static bool VDBG = false;

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...)                                                                \
    do {                                                                                    \
        if (DBG)                                                                            \
            CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG, format, ##__VA_ARGS__); \
    } while (0)
#undef RilLogV
#define RilLogV(format, ...)                                                                   \
    do {                                                                                       \
        if (VDBG)                                                                              \
            CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__); \
    } while (0)
#undef RilLogW
#define RilLogW(format, ...)                                                               \
    do {                                                                                   \
        CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_WARNING_LOG, format, ##__VA_ARGS__); \
    } while (0)
#undef RilLogE
#define RilLogE(format, ...)                                                                \
    do {                                                                                    \
        CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_CRITICAL_LOG, format, ##__VA_ARGS__); \
    } while (0)

ServiceInterface::ServiceInterface(RilContext *rilContext)
    : m_pRilContext(rilContext), m_nOverallRadioState(RADIO_STATE_UNAVAILABLE), mModemState(MS_OFFLINE), m_nVoiceNetworkState(NOT_REGISTERED), m_nDataNetworkState(NOT_REGISTERED), m_nServiceId(RIL_SERVICE_UNKNOWN), m_nServiceState(UNAVAILABLE), m_nMsgPipeR(-1), m_nMsgPipeW(-1), m_pCurReqMsg(nullptr), m_nr_count(0), m_nCurrentReqId(0) {
    memset(m_szSvcName, 0, sizeof(m_szSvcName));

    // register for RadioState callback
    addRadioStateListener(this);

    // register for ModemState callback
    addModemStateListener(this);

    // register for NetworkState callback
    addNetworkStateListener(this);

    // register for SIM status callback
    addSimStatusListener(this);
}

ServiceInterface::~ServiceInterface() {
    if (-1 != m_nMsgPipeR) {
        RilLogW("Stop error: m_nMsgPipeR is -1");
        close(m_nMsgPipeR);
        m_nMsgPipeR = -1;
    }

    if (-1 != m_nMsgPipeW) {
        RilLogW("Stop error: m_nMsgPipeW is -1");
        close(m_nMsgPipeW);
        m_nMsgPipeW = -1;
    }

    for (auto &[id, handler] : mHandlerMap) {
        if (handler) {
            RilLogI("[%s] release handler %d", m_szSvcName, id);
            delete handler;
        }
    }
}

void ServiceInterface::SetServiceId(UINT nServiceId) {
    m_nServiceId = nServiceId;
    if (m_pRilContext != NULL) {
        snprintf(m_szSvcName, sizeof(m_szSvcName) - 1, "%s_%d",
                 ServiceInterface::GetServiceName(nServiceId),
                 m_pRilContext->GetRilSocketId());
    } else {
        snprintf(m_szSvcName, sizeof(m_szSvcName) - 1, "%s",
                 ServiceInterface::GetServiceName(nServiceId));
    }
}

/**
 * An initializer of Service instance.
 * call-back by Init() method in Service
 *
 * @param pRilContext - a pointer of RilContext instance
 * @return return 0 if success, negative value if error
 */
int ServiceInterface::OnCreate(RilContext *) {
    return 0;
}

void ServiceInterface::OnStart() {}
/**
 * An end point of Service instance.
 * call-back by Finalize() method in Service
 *
 * @param pRilContext - a pointer of RilContext instance
 */
void ServiceInterface::OnDestroy() {}

int ServiceInterface::OpenMessagePipe() {
    if (m_nMsgPipeR != -1) {
        RilLogV("Open command pipe again(m_nMsgPipeR)");
        close(m_nMsgPipeR);
        m_nMsgPipeR = -1;
    }

    if (m_nMsgPipeW != -1) {
        RilLogV("Open command pipe again(m_nMsgPipeW)");
        close(m_nMsgPipeW);
        m_nMsgPipeW = -1;
    }

    int fds[2];
    int n = pipe2(fds, O_CLOEXEC);
    if (n < 0) {
        RilLogE("Command pipe create fail");
        return -1;
    }

    m_nMsgPipeR = fds[0];
    m_nMsgPipeW = fds[1];
    return 0;
}

int ServiceInterface::NotifyNewMessage(MsgDirection direction) {
    if (m_nMsgPipeW != -1) {
        mMutex.lock();
        INT8 bVal = (INT8)direction;
        if (write(m_nMsgPipeW, &bVal, 1) > 0) {
            mMutex.unlock();
            return 0;
        }
        mMutex.unlock();
    }

    RilLogE("Notify new message fail: m_nMsgPipeW is -1");
    return -1;
}

int ServiceInterface::NotifyNewMessage(Message *pMsg) {
    return NotifyNewMessage(pMsg->GetDirection());
}

void ServiceInterface::NotifyNextRequestMessage() {
    //Before notify, delete current message
    if (m_pCurReqMsg != NULL) {
        delete m_pCurReqMsg;
        m_pCurReqMsg = NULL;
    }

    NotifyNewMessage(REQUEST);

    return;
}

ServiceHandler *ServiceInterface::FindServiceHandler(int messageId) {
    ServiceHandler *handler = NULL;
    map<int, ServiceHandler *>::iterator iter = mHandlerMap.find(messageId);
    if (iter != mHandlerMap.end()) {
        handler = (ServiceHandler *)iter->second;
    }
    return handler;
}

ServiceHandler *ServiceInterface::GetServiceHandler(Message *msg, bool legacy /* = false*/) {
    // if legacy is true, return a legacy handler
    // this flag is used to set true for Async RIL requests.
    ServiceHandler *handler = NULL;
    if (msg != NULL && !legacy) {
        int messageId = msg->GetMsgId();
        handler = FindServiceHandler(messageId);
        if (handler != NULL) {
            if (VDBG)
                RilLogV("%s ServiceHandler for %d found", m_szSvcName, messageId);
        }
    }
    if (handler == NULL) {
        handler = this;
        if (legacy) {
            RilLog("[%s] Use legacy handler", m_szSvcName);
        }
    }
    return handler;
}

void ServiceInterface::RegisterServiceHandler(int messageId, ServiceHandler *handler) {
    if (m_nServiceState == ServiceInterface::CREATED) {
        ServiceHandler *h = FindServiceHandler(messageId);
        if (h != NULL) {
            delete h;
        }
        mHandlerMap[messageId] = handler;
    } else {
        RilLogW("[%s] Not allowed to register ServiceHandler", m_szSvcName);
    }
}

int ServiceInterface::StartRequestTimeout() {
    if (m_pCurReqMsg != NULL) {
        if (m_pCurReqMsg->GetTimeout() <= 0) {
            m_pCurReqMsg->SetTimeout(60000);  // 1 min.
        }

        // timeout for the current transaction
        // m_pCurReqMsg MUST not be NULL.
        int elapse = m_pCurReqMsg->GetTimeout();
        mTimerList.put(m_pCurReqMsg, elapse);
    }
    return 0;
}

RequestData *ServiceInterface::GetCurrentRequestData() {
    return m_pCurReqMsg != NULL ? m_pCurReqMsg->GetRequestData() : NULL;
}

int32_t ServiceInterface::GetCurrentRequestHalVersionCode() {
    return GetCurrentRequestData() ?
                GetCurrentRequestData()->GetHalVersion() : HAL_VERSION_CODE(1, 0);
}

int ServiceInterface::HandleRequest() {
    if (m_pCurReqMsg == NULL) {
        Message *msg = reqDeQ();
        if (msg == NULL) {
            //RilLogW("reqDeQ() : NULL");
            return -1;
        }
        RequestData *pReqData = msg->GetRequestData();

        if (pReqData == NULL) {
            RilLogE("%s::%s() : No RIL Request Data, msg(%p)", m_szSvcName, __FUNCTION__, msg);
            RilLogE("%s : before delete (%p)", __FUNCTION__, msg);
            delete msg;
            return -1;
        }

        int requestId = pReqData->GetReqId();
        bool allowedRadioUnavailable = IsPossibleToPassInRadioUnavailableState(requestId);
        bool allowedRadioOff = IsPossibleToPassInRadioOffState(requestId);
        if (m_nOverallRadioState == RADIO_STATE_UNAVAILABLE) {
            if (!allowedRadioUnavailable) {
                RilLogV("[%s_%d] %s Current Radio State is RADIO_STATE_UNAVAILABLE : RIL Request ID=%d",
                        m_szSvcName, m_pRilContext->GetRilSocketId(), __FUNCTION__, pReqData->GetReqId());
                m_pRilContext->OnRequestComplete(pReqData, RIL_E_RADIO_NOT_AVAILABLE);
                NotifyNextRequestMessage();
                return 0;
            }
        }

        if (m_nOverallRadioState == RADIO_STATE_OFF) {
            if (!(allowedRadioUnavailable || allowedRadioOff)) {
                RilLogV("[%s_%d] %s Current Radio State is RADIO_STATE_OFF : RIL Request ID=%d",
                        m_szSvcName, m_pRilContext->GetRilSocketId(), __FUNCTION__, pReqData->GetReqId());
                m_pRilContext->OnRequestComplete(pReqData, RIL_E_RADIO_NOT_AVAILABLE);
                NotifyNextRequestMessage();
                return 0;
            }
        }

        // Set Transaction ID
        if (msg != NULL) {
            RequestData *pReqData = msg->GetRequestData();
            RilLogV("[%s] Set transaction (%d)", m_szSvcName, pReqData->GetReqId());
            if (pReqData != NULL) {
                SetTransaction((UINT)(pReqData->GetReqId()));
            }
        }

        // m_pCurReqMsg is only set by OnHandleRequest inside
        m_pCurReqMsg = msg;
        RilLogV("Enter %s::HandleReq()", m_szSvcName);
        bool ret = false;
        ServiceHandler *handler = GetServiceHandler(msg);
        if (handler != NULL) {
            ret = handler->OnHandleRequest(msg);
        }
        RilLogV("Exit %s::HandleReq() : ret=%s", m_szSvcName, ret ? "TRUE" : "FALSE");
        if (!ret) {
            RilLogE("HandleReq(msg) error : OnRequestComplete(RIL_E_INTERNAL_ERR) to the RILD");
            m_pRilContext->OnRequestComplete(pReqData, RIL_E_INTERNAL_ERR);

            // Unset Transaction ID
            RilLogV("[%s] Unset transaction (%d)", m_szSvcName, m_nCurrentReqId);
            UnsetTransaction();

            NotifyNextRequestMessage();
            return 0;
        }
        StartRequestTimeout();
    } else {
        if (DBG) {
            RilLogV("[%s] Now on being IPC transaction : Message ID=%04d", m_szSvcName, m_pCurReqMsg->GetMsgId());
        }
    }

    return 0;
}

int ServiceInterface::HandleResponse() {
    m_nr_count = 0;  //reset no response count

    Message *msg = respDeQ();
    if (msg == NULL) {
        RilLogW("[%s] %s() : respDeQ() is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    ModemData *modemData = NULL;
    if ((modemData = msg->GetModemData()) == NULL) {
        RilLogE("[%s] %s() : No Modem Data", m_szSvcName, __FUNCTION__);
        delete msg;
        return -1;
    }

    if (modemData->IsSolicitedResponse()) {
        bool isReqRspMatched = FALSE;
        bool useLegacyHandler = false;
        AsyncMsgReqStatus asyncStatus = msg->GetAsyncMsgReqStatus();
        // sync msg check
        if (m_pCurReqMsg != NULL && m_pCurReqMsg->GetToken() == msg->GetToken()) {
            isReqRspMatched = TRUE;
        }

        // async msg check
        if (isReqRspMatched == FALSE && asyncStatus == ASYNC_MSG_STATUS_RESPONSE) {
            isReqRspMatched = TRUE;
            // Async requests is not supported ServiceHandler yet.
            // Must use a legacy handler which handled by Service instance itself.
            useLegacyHandler = true;
        }

        if (isReqRspMatched == FALSE) {
            RilLogE("[%s] %s() : No Requested message. Drop response message.", m_szSvcName, __FUNCTION__);
            delete msg;
            return -1;
        }

        RilLogV("Enter %s::%s()", m_szSvcName, __FUNCTION__);
        ServiceHandler *handler = GetServiceHandler(m_pCurReqMsg, useLegacyHandler);
        if (handler != NULL) {
            bool handled = false;
            handled = handler->OnHandleSolicitedResponse(msg);
            RilLogV("Exit %s::%s()", m_szSvcName, __FUNCTION__);
            if (!handled) {
                OnRequestComplete(RIL_E_GENERIC_FAILURE, NULL, 0, msg);
            }
        } else {
            RilLogW("[%s::%s]Invalid ServiceHandler", m_szSvcName, __FUNCTION__);
        }
    } else if (modemData->IsUnsolicitedResponse()) {
        RilLogV("Enter %s::%s()", m_szSvcName, __FUNCTION__);
        ServiceHandler *handler = GetServiceHandler(msg);
        if (handler != NULL) {
            handler->OnHandleUnsolicitedResponse(msg);
        } else {
            RilLogW("[%s::%s]Invalid ServiceHandler", m_szSvcName, __FUNCTION__);
        }
        RilLogV("Exit %s::%s()", m_szSvcName, __FUNCTION__);
    }

    if (msg)
        delete msg;

    return 0;
}

int ServiceInterface::HandleRequestTimeout() {
    TimerEvent te = mTimerList.pop();
    if (te.msg != NULL) {
        if (te.msg == m_pCurReqMsg) {
            RequestData *req = m_pCurReqMsg->GetRequestData();
            if (req != NULL) {
                RilLogE("[%s] %s RIL Request(%d) TIMEOUT", m_szSvcName, __FUNCTION__, req->GetReqId());

                // keep token to be removed
                unsigned int token = m_pCurReqMsg->GetToken();

                ModemData *pModemData = m_pCurReqMsg->GetModemData();
                if (pModemData != NULL) {
                    RilLogE("[%s] TIMEOUT IPC Info : Id=0x%04x(%s), token=0x%04x",
                            m_szSvcName, pModemData->GetMessageId(),
                            ProtocolUtils::rcmMsgToString(pModemData->GetMessageId()),
                            pModemData->GetToken());
                }

                // if requested message was already handled in OnHandleRequestTimeout method, MUST be returned true.
                ServiceHandler *handler = GetServiceHandler(m_pCurReqMsg);
                if (handler != NULL) {
                    if (handler->OnHandleRequestTimeout(m_pCurReqMsg)) {
                        return 0;
                    }
                } else {
                    RilLogW("[%s::%s]Invalid ServiceHandler", m_szSvcName, __FUNCTION__);
                }

                CheckCountsForTimeOut(pModemData);

                m_pRilContext->OnRequestTimeout(req, token);
                NotifyNextRequestMessage();
            }
        } else {
            // handle a postponed or delayed messages
            // push at the end of request queue
            Message *delayedMsg = te.msg;
            RilLogI("[%s] %s Handle a postponed or delayed Message:{id=%d}", m_szSvcName, __FUNCTION__, delayedMsg->GetMsgId());
            EnQueue(delayedMsg);
            NotifyNewMessage(REQUEST);
        }
    } else {
        RilLogW("[%s] %s TimerEvent has NULL message.", m_szSvcName, __FUNCTION__);
    }
    return 0;
}

int ServiceInterface::HandleInternalMessage() {
    Message *msg = internalDeQ();
    if (msg == NULL) {
        //RilLogW("internalDeQ() : NULL");
        return -1;
    }

    switch (msg->GetMsgId()) {
    case MSG_SYSTEM_RADIO_STATE_CHANGED: {
        if (VDBG)
            RilLogV("[%s] OnRadioStateChanged()", m_szSvcName);
        int radioState = RADIO_STATE_UNAVAILABLE;
        RilDataInts *rildata = (RilDataInts *)msg->GetUserData();
        if (rildata != NULL) {
            radioState = rildata->GetInt(0);
        } else {
            radioState = m_pRilContext->GetCurrentRadioState();
        }
        SetRadioState(radioState, true);

        if ((m_nOverallRadioState == RADIO_STATE_OFF) && (m_pCurReqMsg != NULL)) {
            RequestData *pRequest = m_pCurReqMsg->GetRequestData();
            if (!IsPossibleToPassInRadioOffState(pRequest->GetReqId())) {
                RilLogV("[%s_%d] Pending Request Complete in RADIO_STATE_OFF. req=%d", m_szSvcName, m_pRilContext->GetRilSocketId(), pRequest->GetReqId());
                OnRequestComplete(RIL_E_RADIO_NOT_AVAILABLE);
            }
        } else if ((m_nOverallRadioState == RADIO_STATE_UNAVAILABLE) && (m_pCurReqMsg != NULL)) {
            RequestData *pRequest = m_pCurReqMsg->GetRequestData();
            if (!IsPossibleToPassInRadioUnavailableState(pRequest->GetReqId())) {
                RilLogV("[%s_%d] Pending Request Complete in RADIO_STATE_UNAVAILABLE. req=%d", m_szSvcName, m_pRilContext->GetRilSocketId(), pRequest->GetReqId());
                OnRequestComplete(RIL_E_RADIO_NOT_AVAILABLE);
            }
        }
        break;
    }
    case MSG_SYSTEM_SIM_STATUS_CHANGED: {
        if (VDBG)
            RilLogV("[%s] OnSimStatusChanged()", m_szSvcName);
        RilDataInts *rildata = (RilDataInts *)msg->GetUserData();
        if (rildata != NULL) {
            mSimStatusListener.notifySimStatusChanged(rildata->GetInt(0), rildata->GetInt(1));
        }
        break;
    }
    case MSG_SYSTEM_SIM_STATUS_CHANGED_2: {
        if (VDBG)
            RilLogV("[%s] OnSimStatusChanged_2()", m_szSvcName);
        auto rildata =
            static_cast<RilDataTemplate<const SimCardStatus &> *>(msg->GetUserData());
        if (rildata != NULL) {
            mSimStatusListener.notifySimStatusChanged(rildata->Get());
        }
        break;
    }
    case MSG_SYSTEM_IMSI_UPDATED: {
        // MSG_SYSTEM_IMSI_UPDATED is only broadcast when reading IMSI of (U)SIM subscription is done.
        // No IMSI erase event when howswap is occurred. (query from SIM card only)
        if (VDBG)
            RilLogV("[%s] OnImsiUpdated()", m_szSvcName);
        RilDataStrings *rildata = (RilDataStrings *)msg->GetUserData();
        if (rildata != NULL) {
            const char *aid = rildata->GetString(0);
            const char *imsi = rildata->GetString(1);
            // legacy. IMSI only
            OnImsiUpdated(imsi);
            // IMSI with AID
            OnImsiUpdated(aid, imsi);
        }
        break;
    }
    case MSG_SYSTEM_RESET:
        if (VDBG)
            RilLogV("[%s] OnReset()", m_szSvcName);
        OnReset();
        break;
    case MSG_SYSTEM_VOICE_REGISTRTION_STATE_CHANGED: {
        if (VDBG)
            RilLogV("[%s] OnVoiceRegistrationStateChanged()", m_szSvcName);
        RilDataInts *rildata = (RilDataInts *)msg->GetUserData();
        if (rildata != NULL) {
            m_nVoiceNetworkState = rildata->GetInt(0);
            mNetworkStateListener.notifyVoiceRegistrationStateChanged(m_nVoiceNetworkState);
        }
        break;
    }
    case MSG_SYSTEM_DATA_REGISTRTION_STATE_CHANGED: {
        if (VDBG)
            RilLogV("[%s] OnDataRegistrationStateChanged()", m_szSvcName);
        RilDataInts *rildata = (RilDataInts *)msg->GetUserData();
        if (rildata != NULL) {
            m_nDataNetworkState = rildata->GetInt(0);
            mNetworkStateListener.notifyDataRegistrationStateChanged(m_nDataNetworkState);
        }
        break;
    }
    case MSG_SYSTEM_SERVICE_STATE_CHANGED: {
        if (VDBG)
            RilLogV("[%s] OnServiceStateChanged()", m_szSvcName);
        if (GetRilContext() != NULL) {
            const ServiceState &serviceState = GetRilContext()->GetRadioServiceState();
            mNetworkStateListener.notifyServiceStateChanged(serviceState);
        }
        break;
    }
    case MSG_SYSTEM_DATA_CALL_STATE_CHANGED: {
        if (VDBG)
            RilLogV("[%s] OnDataCallStateChanged()", m_szSvcName);
        RilDataInts *rildata = (RilDataInts *)msg->GetUserData();
        if (rildata != NULL) {
            int nCid = rildata->GetInt(0);
            bool bActive = rildata->GetInt(1) ? true : false;
            OnDataCallStateChanged(nCid, bActive);
        }
        break;
    }

    case MSG_SYSTEM_MODEM_STATE_CHANGED: {
        if (VDBG)
            RilLogV("[%s] OnModemStateChanged()", m_szSvcName);
        RilDataInts *rildata = (RilDataInts *)msg->GetUserData();
        if (rildata != NULL) {
            int state = rildata->GetInt(0);
            int oldModemState = mModemState;
            mModemState = state;

            // callback ModemState to all listeners
            mModemStateListener.notifyModemStateChanged(oldModemState, mModemState);
        }
        break;
    }
    case MSG_SYSTEM_IMS_REGISTRATION_STATE_CHANGED: {
        if (VDBG)
            RilLogV("[%s] OnModemStateChanged()", m_szSvcName);
        RilDataInts *rildata = (RilDataInts *)msg->GetUserData();
        if (rildata != NULL) {
            int state = rildata->GetInt(0);
            OnImsRegistrationStateChanged(state);
        }
        break;
    }
    default:
        OnHandleInternalMessage(msg);
        break;
    }  // end switch ~

    if (msg)
        delete msg;

    return 0;
}

/**
 * Put into the timer list and wake up to calculate new timeout
 */
int ServiceInterface::SendMessageDelayed(Message *pMsg, long delayMillis) {
    if (pMsg != NULL) {
        if (pMsg->GetDirection() != REQUEST || delayMillis <= 0) {
            EnQueue(pMsg);
            NotifyNewMessage(pMsg);
        } else {
            // delayed message only if synchronous REQUEST
            mTimerList.put(pMsg, delayMillis);
            NotifyNewMessage(WAKEUP);
        }
        return 0;
    }
    return -1;
}

void ServiceInterface::OnRequest(int request, void *data, unsigned int datalen) {
    OnRequest(request, data, datalen, 0);
}

void ServiceInterface::OnRequest(int request, void *data, unsigned int datalen, long delayedMillis) {
    if (GetRilContext() != NULL) {
        GetRilContext()->OnRequest(request, data, datalen, 0, delayedMillis);
    } else {
        RilLogE("[%s] %s GetRilContext() is NULL!", m_szSvcName, __FUNCTION__);
    }
}

int ServiceInterface::OnRequestComplete(int result, void *data /* = NULL*/, int length /* = 0*/, Message * /* pMsg = NULL */) {
    // normal sync msg case
    if (m_pCurReqMsg != NULL) {
        RequestData *pRequest = m_pCurReqMsg->GetRequestData();
        int token = m_pCurReqMsg->GetToken();
        if (m_pRilContext != NULL) {
            RilLogV("[%s] %s() MSG_ID=%d", m_szSvcName, __FUNCTION__, m_pCurReqMsg->GetMsgId());
            m_pRilContext->OnRequestComplete(pRequest, result, data, length);

            // try to remove request history record if garbage token is remained
            m_pRilContext->ClearGarbage(token);
        }

        // remove timer
        mTimerList.erase(m_pCurReqMsg);
    }

    // Unset Transaction ID
    RilLogV("[%s] Unset transaction (%d)", m_szSvcName, m_nCurrentReqId);
    UnsetTransaction();
    NotifyNextRequestMessage();
    return 0;
}

int ServiceInterface::OnUnsolicitedResponse(int id, const void *data /* = NULL*/, int length /* = 0*/) {
    if (m_pRilContext != NULL) {
        m_pRilContext->OnUnsolicitedResponse(id, data, length);
    }
    return 0;
}

int ServiceInterface::OnRequestAck() {
    if (m_pCurReqMsg != NULL) {
        RequestData *pRequest = m_pCurReqMsg->GetRequestData();

        if (m_pRilContext != NULL) {
            m_pRilContext->OnRequestAck(pRequest->GetToken());
        }
    }
    return 0;
}

bool ServiceInterface::OnHandleRequest(Message *) {
    return false;
}
bool ServiceInterface::OnHandleSolicitedResponse(Message *) {
    return false;
}
bool ServiceInterface::OnHandleUnsolicitedResponse(Message *) {
    return false;
}
bool ServiceInterface::OnHandleInternalMessage(Message *) {
    return false;
}
bool ServiceInterface::OnHandleRequestTimeout(Message *) {
    return false;
}
const char *ServiceInterface::GetName() {
    return m_szSvcName;
}

int ServiceInterface::InitMessageQueue() {
    return 0;
}

void ServiceInterface::ClearMessageQueue() {
    /*
    queue<Message *> m_requestQueue;
    queue<Message *> m_responseQueue;
    queue<Message *> m_internalQueue;
    */
    Message *message = NULL;
    while (!m_requestQueue.empty()) {
        message = m_requestQueue.front();
        if (message != NULL) {
            delete message;
        }
        m_requestQueue.pop();
    }  // end while ~
    while (!m_responseQueue.empty()) {
        message = m_responseQueue.front();
        if (message != NULL) {
            delete message;
        }
        m_responseQueue.pop();
    }  // end while ~
    while (!m_internalQueue.empty()) {
        message = m_internalQueue.front();
        if (message != NULL) {
            delete message;
        }
        m_internalQueue.pop();
    }  // end while ~
    while (!m_asyncMsgReqQueue.empty()) {
        message = m_asyncMsgReqQueue.front();
        if (message != NULL) {
            delete message;
        }
        m_asyncMsgReqQueue.pop();
    }
}

Message *ServiceInterface::reqDeQ() {
    mMutex.lock();
    if (m_requestQueue.empty()) {
        mMutex.unlock();
        return NULL;
    }
    Message *message = m_requestQueue.front();
    m_requestQueue.pop();
    mMutex.unlock();
    return message;
}

Message *ServiceInterface::respDeQ() {
    mMutex.lock();
    if (m_responseQueue.empty()) {
        mMutex.unlock();
        return NULL;
    }
    Message *message = m_responseQueue.front();
    m_responseQueue.pop();
    mMutex.unlock();
    return message;
}

/* function: internalDeQ
    description: de-queue a message from internal event queue

    change log:
    13-06-05 songqiao.yin initial version
*/
Message *ServiceInterface::internalDeQ() {
    mMutex.lock();
    if (m_internalQueue.empty()) {
        mMutex.unlock();
        return NULL;
    }
    Message *message = m_internalQueue.front();
    m_internalQueue.pop();
    mMutex.unlock();
    return message;
}

Message *ServiceInterface::asycMsgReqDeQ() {
    mMutex.lock();
    if (m_asyncMsgReqQueue.empty()) {
        mMutex.unlock();
        return NULL;
    }
    Message *message = m_asyncMsgReqQueue.front();
    m_asyncMsgReqQueue.pop();
    mMutex.unlock();
    return message;
}

int ServiceInterface::EnQueue(Message *pMsg) {
    if (NULL == pMsg) {
        RilLogE("Message enqueue fail: message is NULL");
        return -1;
    }

    //Enqueue message due to different direction
    mMutex.lock();
    switch (pMsg->GetDirection()) {
    case REQUEST:
        m_requestQueue.push(pMsg);
        break;
    case RESPONSE:
        m_responseQueue.push(pMsg);
        break;
    case INTERNAL:
        m_internalQueue.push(pMsg);
        break;
    case ASYNC_REQUEST:
        m_asyncMsgReqQueue.push(pMsg);
        break;
    default:
        //these value shouldn't be set, report error
        RilLogE("[%s] %s(): message direction error", GetName(), __FUNCTION__);
        break;
    }  // end switch ~
    mMutex.unlock();

    return 0;
}

void ServiceInterface::SetRadioState(int radioState, bool forceNotify) {
    int oldRadioState = m_nOverallRadioState;
    m_nOverallRadioState = radioState;

    if (radioState == RADIO_STATE_UNAVAILABLE) {
        SetRadioStateUnavailable();
    }

    // callback RadioState to all listeners
    mRadioStateListener.notifyRadioStateChanged(oldRadioState, m_nOverallRadioState, forceNotify);
}

void ServiceInterface::SetRadioStateUnavailable() {
    m_nOverallRadioState = RADIO_STATE_UNAVAILABLE;
    m_nVoiceNetworkState = NOT_REGISTERED;
    m_nDataNetworkState = NOT_REGISTERED;
}

bool ServiceInterface::IsRadioNotAvailable() const {
    return (m_nOverallRadioState == RADIO_STATE_UNAVAILABLE);
}

bool ServiceInterface::IsRadioAvailable() const {
    return (m_nOverallRadioState != RADIO_STATE_UNAVAILABLE);
}

bool ServiceInterface::IsRadioOn() const {
    return (m_nOverallRadioState == RADIO_STATE_ON);
}

bool ServiceInterface::IsRadioOffOrNotAvailable() const {
    return (m_nOverallRadioState != RADIO_STATE_ON);
}

RilContext *ServiceInterface::GetRilContext() {
    if (m_pRilContext == NULL) {
        RilLogW("%s Warning no valid RilContext instance!!!", __FUNCTION__);
    }
    return m_pRilContext;
}

RIL_SOCKET_ID ServiceInterface::GetRilSocketId() {
    RilContext *pRilContext = GetRilContext();
    if (pRilContext != NULL) {
        return pRilContext->GetRilSocketId();
    }
    return RIL_SOCKET_1;
}

RilProperty *ServiceInterface::GetRilContextProperty() {
    RilContext *pRilContext = GetRilContext();
    if (pRilContext != NULL) {
        return pRilContext->GetProperty();
    }
    return NULL;
}

RilProperty *ServiceInterface::GetRilApplicationProperty() {
    RilContext *pRilContext = GetRilContext();
    if (pRilContext != NULL) {
        return pRilContext->GetApplicationProperty();
    }
    return NULL;
}

int ServiceInterface::GetCurrentRequestDataHalVersion(int halVer) {
    if (GetCurrentRequestData() != NULL) {
        halVer = GetCurrentRequestData()->GetHalVersion();
    }
    return halVer;
}

ServiceState ServiceInterface::GetRadioServiceState() {
    ServiceState ss;
    if (GetRilContext() != NULL) {
        ss = GetRilContext()->GetRadioServiceState();
    }
    return ss;
}

Service *ServiceInterface::GetOppositeService(int nServiceId) {
    RilContext *pRilContext = GetRilContext();
    if (pRilContext != NULL) {
        RilContext *pTargetRilContext = pRilContext->GetOppositeRilContext();
        if (pTargetRilContext != NULL) {
            if (VDBG)
                RilLogV("%s Get Service(%d) in RilContext(%d)", __FUNCTION__, nServiceId, (UINT)(pTargetRilContext->GetRilSocketId()));
            return pTargetRilContext->GetService(nServiceId);
        }
    }

    RilLogV("[%s] %s() : Failed to find opposite service(%d)", m_szSvcName, __FUNCTION__, nServiceId);
    return NULL;
}

Service *ServiceInterface::GetCurrentService(int nServiceId) {
    RilContext *pRilContext = GetRilContext();
    if (pRilContext != NULL) {
        if (VDBG)
            RilLogV("%s Get Service(%d) in RilContext(%d)", __FUNCTION__, nServiceId, (UINT)GetRilSocketId());
        return pRilContext->GetService(nServiceId);
    }

    RilLogE("[%s] %s() : Failed to find current service(%d)", m_szSvcName, __FUNCTION__, nServiceId);
    return NULL;
}

const char *ServiceInterface::GetServiceName(UINT nServiceId) {
    switch (nServiceId) {
    case RIL_SERVICE_UNKNOWN:
        return "Service";
    case RIL_SERVICE_CSC:
        return "CsService";
    case RIL_SERVICE_PS:
        return "PsService";
    case RIL_SERVICE_SIM:
        return "SimService";
    case RIL_SERVICE_MISC:
        return "MiscService";
    case RIL_SERVICE_NETWORK:
        return "NetworkService";
    case RIL_SERVICE_SMS:
        return "SmsService";
    case RIL_SERVICE_AUDIO:
        return "AudioService";
    case RIL_SERVICE_IMS:
        return "ImsService";
    case RIL_SERVICE_GPS:
        return "GpsService";
    case RIL_SERVICE_VSIM:
        return "VSimService";
    case RIL_SERVICE_STK:
        return "StkService";
    case RIL_SERVICE_SUPPLEMENTARY:
        return "SupplementaryService";
    case RIL_SERVICE_EMBMS:
        return "EmbmsService";
    case RIL_SERVICE_SIM_IO:
        return "SimIoService";
    case RIL_SERVICE_SAP:
        return "SapService";
    default:
        return "SERVICE_UNSUPPORTED";
    }  // end switch ~
}

// listener interfaces
void ServiceInterface::addRadioStateListener(ServiceCallback::RadioStateListener *listener) {
    if (listener) {
        mRadioStateListener.addListener(listener);
    }
}

void ServiceInterface::removeRadioStateListener(ServiceCallback::RadioStateListener *listener) {
    if (listener) {
        mRadioStateListener.removeListener(listener);
    }
}

void ServiceInterface::addModemStateListener(ServiceCallback::ModemStateListener *listener) {
    if (listener) {
        mModemStateListener.addListener(listener);
    }
}

void ServiceInterface::removeModemStateListener(ServiceCallback::ModemStateListener *listener) {
    if (listener) {
        mModemStateListener.removeListener(listener);
    }
}

void ServiceInterface::addNetworkStateListener(ServiceCallback::NetworkStateListener *listener) {
    if (listener) {
        mNetworkStateListener.addListener(listener);
    }
}

void ServiceInterface::removeNetworkStateListener(ServiceCallback::NetworkStateListener *listener) {
    if (listener) {
        mNetworkStateListener.removeListener(listener);
    }
}

void ServiceInterface::addSimStatusListener(ServiceCallback::SimStatusListener *listener) {
    if (listener) {
        mSimStatusListener.addListener(listener);
    }
}

void ServiceInterface::removeSimStatusListener(ServiceCallback::SimStatusListener *listener) {
    if (listener) {
        mSimStatusListener.removeListener(listener);
    }
}
