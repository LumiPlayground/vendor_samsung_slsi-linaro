/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "service.h"
#include "open_carrier.h"
#include "protocolutils.h"
#include "rillog.h"
#include "reset_util.h"
#include "rilapptoken.h"
#include "servicestate.h"
#include "util.h"
#include "cscservice.h"
#include "networkservice.h"
#include "rilapplicationcontext.h"
#include <unistd.h>

static bool debug = true;

#define SIGTIMER (SIGRTMAX)

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__);} while(0)
#undef RilLogV
#define RilLogV(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__);} while(0)
#undef RilLogW
#define RilLogW(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__);} while(0)
#undef RilLogE
#define RilLogE(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__);} while(0)

/**
 * RilDataResp
 */
class RilDataResp : public RilData {
private:
    char *m_resp;
    int m_respLen;
    int m_id;
    int m_result;

public:
    RilDataResp(const RilDataResp& other) = delete;
    RilDataResp& operator=(RilDataResp const&) = delete;
    RilDataResp(int id, int result, const void *resp, int respLen) : m_resp(0), m_respLen(0) {
        m_id = id;
        m_result = result;
        if (resp != NULL && respLen > 0) {
            m_resp = new char[respLen];
            m_respLen = respLen;
            memcpy(m_resp, resp, respLen);
        }
    }

    virtual ~RilDataResp() {
        if (m_resp != NULL) {
            delete[] m_resp;
        }
    }

public:
    int GetId() const { return m_id; }
    int GetResult() const { return m_result; }
    void *GetData() const { return m_resp; }
    unsigned int GetDataLength() const { return m_respLen; }
};

/**
 * RilResponseInternalListener
 */
class RilResponseInternalListener : public RilApplicationContext
{
private:
    Service *service;
    int requestId;
public:
    RilResponseInternalListener(Service *service, int requestId) {
        this->requestId = requestId;
        this->service = service;
    }
    virtual ~RilResponseInternalListener() {}

public:
    // response
    void OnRequestComplete(RIL_Token t, RIL_Errno e, void *response, unsigned int responselen) {
        RilLogV("OnRequestInternalComplete(id=%d result=%d data=%p length=%d)", requestId, e, response, responselen);
        if (service != NULL) {
            service->OnRequestInternalComplete(t, requestId, e, response, responselen);
        }
        else {
            RilLogW("No target to be called for response");
        }
    }
    void OnUnsolicitedResponse(int /* unsolResponse */, const void * /* data */, unsigned int /* datalen */) {}
    void OnUnsolicitedResponse(int /* unsolResponse */, const void * /* data */, unsigned int /* datalen */, RIL_SOCKET_ID) {}
    void OnRequestAck(RIL_Token) {}
    void OnUpdateReqModemData(RIL_Token, void * /* data */, unsigned int /* len */) {}

public:
    static RilResponseInternalListener *NewInstance(Service *service, int requestId) {
        if (requestId < 0) {
            return NULL;
        }

        if (service == NULL) {
            return NULL;
        }

        return new RilResponseInternalListener(service, requestId);
    }
};



IMPLEMENT_MODULE_TAG(Service, Service)

const char *Service::GetServiceName(const Service *pService)
{
    if (pService != NULL) {
        return ServiceInterface::GetServiceName(pService->GetServiceId());
    }
    return "RIL_SERVICE_UNKNOWN";
}

Service::Service(RilContext* pRilContext, UINT nServiceId/* = RIL_SERVICE_UNKNOWN*/)
    : ServiceInterface(pRilContext)
{
    SetServiceId(nServiceId);
    m_pPoll = NULL;
    m_pServiceMonitorRunnable = NULL;
    m_bIsInited = false;
    memset(&m_tvStart, 0, sizeof(m_tvStart));
    m_pMutex = NULL;

    // Priority Req Interface
    m_pAsyncMsgHistory = NULL;
    m_pAsyncTimerMutex = NULL;

}

Service::~Service()
{
    ClearMessageQueue();
    AsycMsgReqClear();

    if(m_pPoll)
    {
        m_pPoll->Stop();
        delete m_pPoll;
        m_pPoll = NULL;
    }

    if(m_pAsyncTimerMutex)
    {
        delete m_pAsyncTimerMutex;
        m_pAsyncTimerMutex = NULL;
    }
}

int Service::Init()
{
    RilLogI("[%s] %s++", m_szSvcName, __FUNCTION__);
    if (m_bIsInited) {
        RilLogW("[%s] %s already initialized", m_szSvcName, __FUNCTION__);
        return -1;
    }

    // callback OnCreate() method
    // all initialization code for each Service instance should be here.
    // every Service instance should override.
    if (OnCreate(m_pRilContext) < 0) {
        RilLogE("[%s] %s OnCreate error", m_szSvcName, __FUNCTION__);
        return -1;
    }
    m_nServiceState = Service::CREATED;

    m_pAsyncMsgHistory = new CAsyncMsgReqHistory();
    m_pAsyncTimerMutex = new CMutex();
    if(m_pAsyncTimerMutex == NULL) {
        RilLogE("[%s] %s Fail to create Async Mutex instance", m_szSvcName, __FUNCTION__);
        return -1;
    }

    m_pMutex = &mMutex;

    if (InitMessageQueue() < 0) {
        RilLogE("[%s] %s Fail to initialize message queue", m_szSvcName, __FUNCTION__);
        return -1;
    }

    if (OpenMessagePipe() < 0)
    {
        RilLogE("[%s] %s Fail to open message pipe", m_szSvcName, __FUNCTION__);
        return -1;
    }

    m_bIsInited = true;
    RilLogI("[%s] %s--", m_szSvcName, __FUNCTION__);
    return 0;
}

int Service::Start()
{
    if (m_pServiceMonitorRunnable == NULL) {
        m_pServiceMonitorRunnable = new ServiceMonitorRunnable(this);
    }

    m_pPoll = new Thread(m_pServiceMonitorRunnable);
    if (m_pPoll != NULL && m_pPoll->Start() < 0) {
        RilLogE("[%s] %s Fail to start ServiceMonitor thread.", m_szSvcName, __FUNCTION__);
        return -1;
    }
    m_nServiceState = Service::STARTED;

    return 0;
}

int Service::Finalize()
{
    RilLogI("++[%s] %s", m_szSvcName, __FUNCTION__);
    if (m_bIsInited == false) {
        RilLogW("Service(base) finalize error");
        RilLogW("[%s] %s Finalize but not initialized", m_szSvcName, __FUNCTION__);
        return -1;
    }

    if (m_pPoll != NULL) {

        if (m_pServiceMonitorRunnable != NULL) {
            m_pServiceMonitorRunnable->Stop();
            m_pPoll->Stop();
            delete m_pServiceMonitorRunnable;
            m_pServiceMonitorRunnable = NULL;
        }

        delete m_pPoll;
        m_pPoll = NULL;
    }

    if (m_nMsgPipeR > 0) {
        close(m_nMsgPipeR);
        m_nMsgPipeR = -1;
    }

    if (m_nMsgPipeW > 0) {
        close(m_nMsgPipeW);
        m_nMsgPipeW = -1;
    }

    ClearMessageQueue();
    AsycMsgReqClear();

    // callback OnDestroy()
    OnDestroy();
    m_nServiceState = Service::DESTROYED;

    m_bIsInited = false;
    RilLogI("--[%s] %s", m_szSvcName, __FUNCTION__);
    return 0;
}

void Service::SetServiceMonitorRunnable(ServiceMonitorRunnable *pServiceMonitorRunnable)
{
    if (m_bIsInited && m_nServiceState != Service::STARTED) {
        if (m_pServiceMonitorRunnable != NULL) {
            delete m_pServiceMonitorRunnable;
        }
        m_pServiceMonitorRunnable = pServiceMonitorRunnable;
    }
}

bool Service::PostponeRequestMessage(unsigned int millis)
{
    // PostponeRequestMessage can be called both
    // HandleRequest and HandleResponse
    Message *postponeMsg = m_pCurReqMsg;
    if (postponeMsg != NULL) {
        RilLogV("[%s] PostponeRequestMessage Postpone Message:{id=%d}", GetServiceName(), postponeMsg->GetMsgId());
        m_pCurReqMsg = NULL;
        postponeMsg->SetModemData(NULL);
        postponeMsg->SetTimeout(0);

        if (m_pRilContext != NULL) {
            // try to remove request history record if garbage token is remained
            unsigned int token = postponeMsg->GetToken();
            if (token != TOKEN_INVALID) {
                m_pRilContext->ClearGarbage(token);
            }
        }

        // Unset Transaction ID
        RilLogV("[%s] Unset transaction (%d)", GetServiceName(), m_nCurrentReqId);
        UnsetTransaction();

        // postpone
        SendMessageDelayed(postponeMsg, millis);
    }

    NotifyNextRequestMessage();

    return true;
}

int Service::SetRequestTimeout(UINT timeout)
{
    if (m_pCurReqMsg == NULL) {
        return -1;
    }

    m_pCurReqMsg->SetTimeout(timeout);
    return 0;
}

int Service::ResetRequestTimeout(UINT timeout)
{
    if (SetRequestTimeout(timeout) < 0) {
        return -1;
    }

    // reset started time
    StartRequestTimeout();

    return 0;
}

int Service::CalcNextTimeout(struct timeval *tv)
{
    if (tv == NULL || mTimerList.size() == 0) {
        // infinite
        return -1;
    }

    TimerEvent front = mTimerList.front();
    struct timeval now = TimerList::getNow();
    TimerList::sub(front.timeout, now);
    tv->tv_sec = front.timeout.tv_sec;
    tv->tv_usec = front.timeout.tv_usec;

    if (front.msg == NULL) {
        tv->tv_sec = tv->tv_usec = 0;
    }

    return 0;
}

int Service::SendRequest(ModemData *pModemData, UINT nTimeout, UINT nResult, Message *pMsg/* = NULL*/)
{
    bool isAsycMsg = ( pMsg != NULL && pMsg->GetAsyncMsgReqStatus() != ASYNC_MSG_STATUS_NONE);

    // for unit test
    RequestData* pReqData = NULL;
    RIL_Token token = NULL;

    if (pModemData == NULL) {
        RilLogE("%s::%s() : pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    if (m_pRilContext == NULL) {
        RilLogE("%s::%s() : m_pRilContext is NULL", m_szSvcName, __FUNCTION__);
        delete pModemData;
        return -1;
    }

    if (isAsycMsg == FALSE) {
        // normal sync msg case
        if (m_pCurReqMsg == NULL) {
            RilLogE("%s::%s() : m_pCurReqMsg is NULL", m_szSvcName, __FUNCTION__);
            delete pModemData;
            return -1;
        }
        // for unit test
        pReqData = m_pCurReqMsg->GetRequestData();
        if (pReqData != NULL) token = pReqData->GetToken();

        m_pCurReqMsg->SetModemData(pModemData);
        m_pCurReqMsg->SetTimeout(nTimeout);
        if (m_pRilContext->Send(pModemData, m_nServiceId, nResult, token) < 0) {
            RilLogE("%s::%s() : m_pRilContext->Send2Modem error", m_szSvcName, __FUNCTION__);
            return -1;
        }
    } else { // async msg case
        // for unit test
        pReqData = pMsg->GetRequestData();
        if (pReqData != NULL) token = pReqData->GetToken();

        pMsg->SetModemData(pModemData);
        pMsg->SetTimeout(nTimeout);

        // Due to different thread(send and receive), update status before sending.
        UINT priorityReqToken = pMsg->GetToken();
        pMsg->SetAsyncMsgReqStatus(ASYNC_MSG_STATUS_SENT2CP);
        pMsg->SetAsyncMsgReqStartTime();
        AsyncMsgReqInsertHistory(priorityReqToken, pMsg);
        // After getting earliest expiration time in priority request list, then update timer.
        AsyncMsgReqStartTimer(priorityReqToken, pMsg);

        if (m_pRilContext->Send(pModemData, m_nServiceId, nResult, token) < 0) {
            RilLogE("%s::%s() : m_pRilContext->Send2Modem error", m_szSvcName, __FUNCTION__);
            pMsg->SetAsyncMsgReqStatus(ASYNC_MSG_STATUS_PROCESSING);
            AsyncMsgReqRemoveHistory(priorityReqToken);
            AsyncMsgReqDeleteTimer(priorityReqToken, ASYNC_MSG_TIMER_DELETE_REASON_OTHERS);
            return -1;
        }

    }

    return 0;
}

int Service::SendRequest(ModemData *pModemData, UINT nTimeout)
{
    if (m_pCurReqMsg == NULL) {
        return -1;
    }
    return SendRequest(pModemData, nTimeout, m_pCurReqMsg->GetMsgId());
}

int Service::SendRequest(ModemData *pModemData)
{
    if (pModemData == NULL) {
        RilLogE("%s::%s() : pModemData is NULL", m_szSvcName, __FUNCTION__);
        return -1;
    }

    if (m_pRilContext != NULL && m_pRilContext->Send((void *)pModemData->GetRawData(), pModemData->GetLength()) < 0) {
        RilLogE("%s::%s() : m_pRilContext->Send2Modem error", m_szSvcName, __FUNCTION__);
        return -1;
    }
    return 0;
}

void Service::CheckCountsForTimeOut(ModemData *pModemData)
{
    if ( pModemData == NULL) return;

    if ( ++m_nr_count > MAX_NR_COUNT )
    {
        RilLogE("There is NO RESPONSE from CP more than %d times", MAX_NR_COUNT);

        const BOOL bModemCrash = TRUE;
        if (bModemCrash == TRUE)
        {
            RilLogE("Request to make CP Forced Crash");

            // Request CP Crash Dump
            int rcmId = pModemData->GetMessageId();
            RequestData *pData = RilParser::CreateRawData(RIL_REQUEST_OEM_MODEM_DUMP, 0, (char *)(&rcmId), sizeof(int));
            if (pData != NULL) {
                Message *msg = Message::ObtainMessage(pData, RIL_SERVICE_MISC, MSG_MISC_OEM_FCRASH_MNR_REQ);

                // Check Async message
                if (m_pRilContext->GetServiceManager()->IsAsyncMessage(RIL_SERVICE_MISC, MSG_MISC_OEM_FCRASH_MNR_REQ) == true) {
                    msg->SetDirection(ASYNC_REQUEST);
                    msg->SetAsyncMsgReqStatus(ASYNC_MSG_STATUS_INITIATED);
                }

                if (m_pRilContext->GetServiceManager()->SendMessage(msg) < 0) {
                    if (msg) {
                        delete msg;
                    }
                }
            }
        }
        else
        {
            RilErrorReset("IPC_TIMEOUT(MNR)");
        }
    }
}

unsigned int Service::GetOpenCarrierIndex()
{
    RilContext *pRilContext = GetRilContext();
    if ( pRilContext != NULL )
    {
        return pRilContext->GetOpenCarrierIndex();
    }

    RilLogE("[%s] %s() : Failed to get open carrier index", m_szSvcName, __FUNCTION__);
    return (unsigned int)OC_UNKNOWN;
}

void Service::SetOpenCarrierIndex(const char* mccmnc)
{
    RilContext *pRilContext = GetRilContext();
    if ( pRilContext != NULL )
    {
        return pRilContext->SetOpenCarrierIndex(mccmnc);
    }

    RilLogE("[%s] %s() : Failed to set open carrier index", m_szSvcName, __FUNCTION__);
}

std::string Service::GetSimOperatorNumeric()
{
    const int phoneId = GetRilSocketId();
    std::string numeric = GetSimOperatorNum(phoneId);
    return numeric;
}

void Service::OnRequestInternal(int request, void *data/* = NULL*/, unsigned int datalen/* = 0*/)
{
    RilLogV("[%d]%s(id=%d data=%p length=%d", GetRilSocketId(), __FUNCTION__, request, data, datalen);
    if (request > 0) {
        RilAppToken *tok = RilAppToken::NewInstance(RilResponseInternalListener::NewInstance(this, request));
        GetRilContext()->OnRequest(request, data, datalen, (RIL_Token)tok);
    }
}

void Service::OnRequestInternal(int request, void *data, unsigned int datalen, RIL_Token t)
{
    RilLogV("[%d]%s(id=%d data=%p length=%d)", GetRilSocketId(), __FUNCTION__, request, data, datalen);
    if (request > 0) {
        RilAppToken *tok = RilAppToken::NewInstance(RilResponseInternalListener::NewInstance(this, request), t);
        GetRilContext()->OnRequest(request, data, datalen, (RIL_Token)tok);
    }

}

int Service::OnRequestComplete(int result, void *data/* = NULL*/, int length/* = 0*/, Message *pMsg/* = NULL*/)
{
    bool isAsycMsg = ( pMsg != NULL && pMsg->GetAsyncMsgReqStatus() != ASYNC_MSG_STATUS_NONE);

    if (isAsycMsg == FALSE) {
        return ServiceInterface::OnRequestComplete(result, data, length, NULL);
    } else {
        // async msg case
        Message *pRequestMsg = NULL;
        RequestData *pRequest = NULL;
        int token = TOKEN_INVALID;
        AsyncMsgReqStatus asyncMsgReqStatus = pMsg->GetAsyncMsgReqStatus();

        if (pMsg->GetDirection() == ASYNC_REQUEST) {
            pRequestMsg = pMsg;
            pRequest = pRequestMsg->GetRequestData();

            if (asyncMsgReqStatus == ASYNC_MSG_STATUS_SENT2CP) {
                // case1: SIT time out case
                // it is called in OnHandleRequestTimeout(), pMsg is request message.
                token = pMsg->GetToken();
                if (m_pAsyncMsgHistory != NULL) m_pAsyncMsgHistory->Remove(token);
            } else if (asyncMsgReqStatus == ASYNC_MSG_STATUS_PROCESSING) {
                // case2: Cases that SIT is not sent to CP
                // OnRequestComplete() is called in request function
                RilLogV("[%s] Command(%d) not sent to the modem", m_szSvcName, pMsg->GetMsgId());
            } else {
                // unwanted scenario
                RilLogW("[%s] unwanted asyn for request msgId = %d ", m_szSvcName, pMsg->GetMsgId());
            }
        } else {
            if (asyncMsgReqStatus == ASYNC_MSG_STATUS_RESPONSE) {
                // case3: SIT respone is received. pMsg is response msg.
                token = pMsg->GetToken();
                if (m_pAsyncMsgHistory != NULL) {
                    pRequestMsg = m_pAsyncMsgHistory->Find(token);
                    if (pRequestMsg != NULL) pRequest = pRequestMsg->GetRequestData();
                    m_pAsyncMsgHistory->Remove(token);
                }
            } else {
                // unwanted scenario
                RilLogW("[%s] unwanted asyn for response msgId = %d ", m_szSvcName, pMsg->GetMsgId());
            }
        }

        RilLogV("[%s] %s() AsyncMsgReqStatus = %d, token = 0x%x", m_szSvcName, __FUNCTION__, asyncMsgReqStatus, token);
        if(m_pRilContext != NULL) {
            m_pRilContext->OnRequestComplete(pRequest, result, data, length);

            // try to remove request history record if garbage token is remained
            m_pRilContext->ClearGarbage(token);
        }

        // delete request message
        if (pRequestMsg != NULL) {
            delete pRequestMsg;
        }
    }

    return 0;
}

void Service::OnRequestInternalComplete(RIL_Token /* t */, int /* id */, int /* result */, void * /* data = NULL */, int /* datalen = 0 */)
{
    // TODO : override
}

void Service::AsycMsgReqClearHistory()
{
    UINT size, index, token;

    if (m_pAsyncMsgHistory == NULL) return;

    size = m_pAsyncMsgHistory->GetSize();
    for(index = 0; index < size; index++) {
        token = m_pAsyncMsgHistory->GetBegin();
        Message *pReqMsg = m_pAsyncMsgHistory->Find(token);
        if (pReqMsg != NULL) {
            RequestData *pRequest = pReqMsg->GetRequestData();
            if(m_pRilContext != NULL) {
                m_pRilContext->OnRequestTimeout(pRequest, token);
            }
        }
        AsyncMsgReqDeleteTimer(token, ASYNC_MSG_TIMER_DELETE_REASON_OTHERS);
        m_pAsyncMsgHistory->Remove(token);
        if(pReqMsg != NULL) delete pReqMsg;
    }
}

void Service::AsycMsgReqClear()
{
    AsycMsgReqClearHistory();

    // For double checking
    if (m_pAsyncMsgHistory != NULL)
    {
        m_pAsyncMsgHistory->Clear();
        delete m_pAsyncMsgHistory;
        m_pAsyncMsgHistory = NULL;
    }

    for (auto element : m_asyncMsgReqTimerMap) {
        delete element.second.second;
    }
    m_asyncMsgReqTimerMap.clear();
}

void Service::AsyncMsgReqPreProcessing(Message *pMsg)
{
    if(NULL == pMsg) {
        RilLogE("PreProcessing for priority fail: message is NULL");
        return;
    }

    //Enqueue message due to different direction
    switch (pMsg->GetDirection()) {
        case REQUEST:
        {
            break;
        }
        case RESPONSE:
        {
            int rspMsgId = -1;
            UINT token = pMsg->GetToken();
            rspMsgId = pMsg->GetMsgId();
            ModemData *pModemData = pMsg->GetModemData();

            if(token != TOKEN_INVALID && m_pAsyncMsgHistory != NULL && pModemData != NULL && pModemData->IsSolicitedResponse()) {
                Message *pReqMsg = m_pAsyncMsgHistory->Find(token);
                if(pReqMsg != NULL && pReqMsg->GetAsyncMsgReqStatus() == ASYNC_MSG_STATUS_SENT2CP) {
                    pMsg->SetAsyncMsgReqStatus(ASYNC_MSG_STATUS_RESPONSE);
                    AsyncMsgReqDeleteTimer(token, ASYNC_MSG_TIMER_DELETE_REASON_NORMAL);
                    RilLogV("%s::%s():RESPONSE token(0x%x), RSP msg id(%d)",
                            m_szSvcName, __FUNCTION__, token, rspMsgId);
                }
            }
            break;
        }
        case INTERNAL:
        {
            break;
        }
        case ASYNC_REQUEST:
        {
            break;
        }
        default:
        {
            //these value shouldn't be set, report error
            RilLogE("[%s] %s(): message direction error", m_szSvcName, __FUNCTION__);
        }
    } // end switch ~
}

void Service::AsyncMsgReqInsertHistory(UINT token, Message * pMsg) {
    if (m_pAsyncMsgHistory != NULL) m_pAsyncMsgHistory->Push(token, pMsg);
}

void Service::AsyncMsgReqRemoveHistory(UINT token) {
    if (m_pAsyncMsgHistory != NULL) m_pAsyncMsgHistory->Remove(token);
}

void Service::AsyncMsgReqStartTimer(UINT token, Message * pMsg)
{
    timer_t timerId;
    int relativeTimeOut;

    if (pMsg != NULL && token != TOKEN_INVALID) {
        relativeTimeOut = pMsg->GetTimeout();
        timerId = StartTimer(token, relativeTimeOut, Service::AsyncMsgReqTimerHandlerWrapper);
        RilLogV("%s::%s() : token(0x%x), timerID(0x%x), timeout(%d), total(%d)",
            m_szSvcName, __FUNCTION__, token, timerId, relativeTimeOut, m_asyncMsgReqTimerMap.size());
    }
}

int Service::HandleAsycMsgRequest()
{
    Message *pMsg = asycMsgReqDeQ();
    if (pMsg == NULL) {
        //RilLogW("asycMsgReqDeQ() : NULL");
        return -1;
    }

    RequestData *pReqData = pMsg->GetRequestData();
    if (pReqData == NULL) {
        RilLogE("%s::%s() : No RIL Request Data, msg(%p)", m_szSvcName, __FUNCTION__, pMsg);
        delete pMsg;
        return -1;
    }

    if (m_nOverallRadioState == RADIO_STATE_OFF)
    {
        if ( false == IsPossibleToPassInRadioOffState(pReqData->GetReqId()) )
        {
            RilLogV("[%s_%d] %s Current Radio State is RADIO_STATE_OFF : RIL Request ID=%d",
                m_szSvcName, m_pRilContext->GetRilSocketId(), __FUNCTION__, pReqData->GetReqId());
            if(m_pRilContext != NULL) m_pRilContext->OnRequestComplete(pReqData, RIL_E_RADIO_NOT_AVAILABLE);
            delete pMsg;
            //NotifyNewMessage(ASYNC_REQUEST);
            return 0;
        }
    }

    if(m_nOverallRadioState == RADIO_STATE_UNAVAILABLE)
    {
        if ( false == IsPossibleToPassInRadioUnavailableState(pReqData->GetReqId()) )
        {
            RilLogV("[%s_%d] %s Current Radio State is RADIO_STATE_UNAVAILABLE : RIL Request ID=%d",
                m_szSvcName, m_pRilContext->GetRilSocketId(),__FUNCTION__, pReqData->GetReqId());
            if(m_pRilContext != NULL) m_pRilContext->OnRequestComplete(pReqData, RIL_E_RADIO_NOT_AVAILABLE);
            delete pMsg;
            //NotifyNewMessage(ASYNC_REQUEST);
            return 0;
        }
    }

    RilLogV("Enter %s::%s()", m_szSvcName, __FUNCTION__);
    pMsg->SetAsyncMsgReqStatus(ASYNC_MSG_STATUS_PROCESSING);
    BOOL ret = OnHandleRequest(pMsg);
    RilLogV("Exit %s::%s() ret=%s, RIL Request ID=%d", m_szSvcName, __FUNCTION__, ret ? "TRUE" : "FALSE", pReqData->GetReqId());
    if (!ret) {
        RilLogE("%s::%s() OnRequestComplete(RIL_E_GENERIC_FAILURE) to the RILD", m_szSvcName, __FUNCTION__);
        if(m_pRilContext != NULL) m_pRilContext->OnRequestComplete(pReqData, RIL_E_GENERIC_FAILURE);
        delete pMsg;
        //NotifyNewMessage(ASYNC_REQUEST);
        return 0;
    }

    return 0;
}

void Service::AsyncMsgReqDeleteTimer(UINT token, int reason)
{
    timer_t t_id = (timer_t)-1;
    bool isTimerDeleteError = FALSE;

    m_pAsyncTimerMutex->lock();
    auto iter = m_asyncMsgReqTimerMap.find(token);
    if (iter != m_asyncMsgReqTimerMap.end()) {
        t_id = iter->second.first;

        if(timer_delete(t_id) == -1){
            isTimerDeleteError = TRUE;
        }
        delete iter->second.second;
        m_asyncMsgReqTimerMap.erase(token);
    }
    m_pAsyncTimerMutex->unlock();
    RilLogV("[%s] %s() : token(0x%x), isTimerDeleteError(%d) reason(%d)",
            m_szSvcName, __FUNCTION__, token, isTimerDeleteError, reason);
}

void Service::AsyncMsgReqTimerCallback(UINT token)
{
    AsyncMsgReqDeleteTimer(token, ASYNC_MSG_TIMER_DELETE_REASON_TIMEOUT);

    if (m_pAsyncMsgHistory == NULL) return;

    Message *pReqMsg = m_pAsyncMsgHistory->Find(token);
    if (pReqMsg != NULL) {
        bool isReqMsgTimeoutProcessed = false;
        RequestData *req = pReqMsg->GetRequestData();
        if (req != NULL) {
            // if requested message was already handled in OnHandleRequestTimeout method, MUST be returned true.
            // if it is true, pReqMsg is deleted and  m_pAsyncMsgHistory->Remove is called in OnHandleRequestTimeout.
            if (OnHandleRequestTimeout(pReqMsg)) {
                isReqMsgTimeoutProcessed = true;
            } else {
                CheckCountsForTimeOut(pReqMsg->GetModemData());
                m_pRilContext->OnRequestTimeout(req, token);
            }
        } else {
            RilLogE("[%s] %s() : No REQ data for token(0x%x) in the async msg req list", m_szSvcName, __FUNCTION__, token);
        }

        if (isReqMsgTimeoutProcessed == false) {
            m_pAsyncMsgHistory->Remove(token);
            delete pReqMsg;
        }
    }
    else {
        RilLogE("[%s] %s() : No MSG for token(0x%x) in the async msg req list", m_szSvcName, __FUNCTION__, token);
    }
}

void Service::AsyncMsgReqTimerHandlerWrapper(union sigval sv)
{
    async_msg_timer_param *object = (async_msg_timer_param *) sv.sival_ptr;

    RilLogV("%s() : token %d", __FUNCTION__, object->token);
    ((Service *) object->callback_handler)->AsyncMsgReqTimerCallback(object->token);

    // Do not access object after this, object is deleted when calling AsyncMsgReqTimerCallback()
}

timer_t Service::StartTimer(UINT token, int timeout, void (pAction)(union sigval))
{
    struct sigevent sigEvt;
    struct itimerspec val;
    timer_t temp_t_id;

    memset(&sigEvt, 0 , sizeof(struct sigevent));
    memset(&val, 0, sizeof(struct itimerspec));

    async_msg_timer_param * t_param = new(async_msg_timer_param);
    t_param->callback_handler = (void *) this;
    t_param->token = token;

    sigEvt.sigev_notify = SIGEV_THREAD; // SIGEV_SIGNAL
    sigEvt.sigev_notify_function = pAction;
    sigEvt.sigev_value.sival_ptr = (void *) t_param;

    if(timer_create(CLOCK_REALTIME, &sigEvt, &temp_t_id) == 0){
        //The unit for timeout value is ms
        val.it_value.tv_sec = (timeout/1000);
        val.it_value.tv_nsec = (long)(timeout % 1000) * (1000000L);

        if(timer_settime(temp_t_id, 0, &val, NULL) != 0){
            RilLogE("%s() timer_settimer() error", __FUNCTION__);
            if(timer_delete(temp_t_id) == -1){
                RilLogE("%s() timer_delete() error", __FUNCTION__);
            }
            delete t_param;
            return (timer_t)-1;
        }
        m_pMutex->lock();
        m_asyncMsgReqTimerMap.insert({token, std::make_pair(temp_t_id, t_param)});
        m_pMutex->unlock();
    }
    else{
        RilLogE("timer_create() error");
        delete t_param;
        return (timer_t)-1;
    }

    return temp_t_id;

}

BOOL Service::IsInTransaction(UINT nReqId)
{
    BOOL ret = FALSE;

    // Check Normal Msg
    ret = (m_nCurrentReqId != 0 && m_nCurrentReqId == nReqId);

    // Check Async Msg History
    if(ret == FALSE && m_pAsyncMsgHistory != NULL && m_pAsyncMsgHistory->GetSize() != 0) {
        ret = m_pAsyncMsgHistory->IsReqInQueue(nReqId);
    }

    return ret;
}

RequestData *Service::GetRequestData(Message *response)
{
    if (response == NULL) {
        RilLogW("[%s] Invalid a parameter.", GetServiceName());
        return NULL;
    }

    if (response->GetDirection() != RESPONSE) {
        RilLogW("[%s] Message is not a response.", GetServiceName());
        return NULL;
    }

    RequestData *ret = NULL;
    AsyncMsgReqStatus status = response->GetAsyncMsgReqStatus();
    switch (status) {
    case ASYNC_MSG_STATUS_NONE:
        ret = GetCurrentRequestData();
        break;
    case ASYNC_MSG_STATUS_RESPONSE: {
        int token = response->GetToken();
        if (m_pAsyncMsgHistory != NULL) {
            Message *request = m_pAsyncMsgHistory->Find(token);
            if (request != NULL) {
                ret = request->GetRequestData();
            }
            else {
                RilLogW("[%s] No-matched request message for this response", GetServiceName());
            }
        }
        break;
    }
    default:
        RilLogW("[%s] Warning. Unexpected AsyncMsgReqStatus(%d)", GetServiceName(), status);
        break;
    } // end switch ~

    if (ret == NULL) {
        RilLogW("[%s] Warning. Returned RequestData is null.", GetServiceName());
    }

    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////
CAsyncMsgReqHistory::CAsyncMsgReqHistory()
{
}

CAsyncMsgReqHistory::~CAsyncMsgReqHistory()
{
    Clear();
}

UINT CAsyncMsgReqHistory::GetSize()
{
    UINT ret;
    Lock();
    ret = (UINT)m_asyncMsgReqMap.size();
    Unlock();
    return ret;
}

bool CAsyncMsgReqHistory::Push(UINT token, Message *pMsg)
{
    UINT mapSize = 0;
    bool result = FALSE;
    pair<map<UINT, Message *>::iterator,bool> ret;

    if (token != TOKEN_INVALID && pMsg != NULL) {
        Lock();
        ret = m_asyncMsgReqMap.insert(pair<UINT, Message *>(token, pMsg));
        mapSize = (UINT)m_asyncMsgReqMap.size();
        Unlock();

        if (ret.second == TRUE) {
            RilLogV("CAsyncMsgReqHistory::%s() element 0x%x is pushed. size = %d", __FUNCTION__, token, mapSize);
            result = true;
        }
        else {
            RilLogE("CAsyncMsgReqHistory::%s() element 0x%x is already exist. size = %d", __FUNCTION__, token, mapSize);
        }
    }
    else {
        RilLogE("CAsyncMsgReqHistory::%s() token 0x%x is invalid or msg is null", __FUNCTION__, token);
    }

    return result;
}

UINT CAsyncMsgReqHistory::GetBegin()
{
    UINT token = TOKEN_INVALID;

    Lock();
    if ( m_asyncMsgReqMap.size() > 0 ) {
        map<UINT, Message *>::iterator iter = m_asyncMsgReqMap.begin();
        token = iter->first;
    }
    Unlock();
    return token;
}

void CAsyncMsgReqHistory::Remove(UINT token)
{
    Lock();
    if ( m_asyncMsgReqMap.size() > 0 && token != TOKEN_INVALID) {
        m_asyncMsgReqMap.erase(token);
    }
    Unlock();
}

Message *CAsyncMsgReqHistory::Find(UINT token)
{
    Message *pResultMsg = NULL;
    Lock();
    if ( m_asyncMsgReqMap.size() > 0 && token != TOKEN_INVALID ) {

        map<UINT, Message *>::iterator iter = m_asyncMsgReqMap.find(token);
        if (iter != m_asyncMsgReqMap.end()) {
             pResultMsg = iter->second;
        }
    }
    Unlock();

    return pResultMsg;
}

bool CAsyncMsgReqHistory::IsReqInQueue(UINT nReqId)
{
    bool ret = false;
    Message *pMessage = NULL;
    Lock();
    if ( m_asyncMsgReqMap.size() > 0 ) {
        RequestData *pReqData;
        for ( map<UINT, Message *>::iterator iter = m_asyncMsgReqMap.begin(); iter != m_asyncMsgReqMap.end(); ++iter) {
            pMessage = iter->second;
            if (pMessage != NULL) {
                pReqData = pMessage->GetRequestData();
                if(pReqData != NULL && (UINT)pReqData->GetReqId() == nReqId) {
                    ret = true;
                    break;
                }
            }
        }
    }
    Unlock();

    return ret;
}

Message *CAsyncMsgReqHistory::GetReqMsgForEarlistTimeOut(struct timeval *pTv, UINT *pToken)
{
    Message *pResultMsg = NULL;
    UINT earlistToken = TOKEN_INVALID;

    int64_t relativeTimeOut;
    struct timeval tv;
    struct timeval earlistTimeout;

    memset(&tv, 0, sizeof(tv));
    memset(&earlistTimeout, 0, sizeof(earlistTimeout));

    Lock();
    if ( m_asyncMsgReqMap.size() > 0 ) {
        for ( map<UINT, Message *>::iterator iter = m_asyncMsgReqMap.begin(); iter != m_asyncMsgReqMap.end(); ++iter) {
            Message *pMessage = iter->second;
            if (pMessage != NULL && pMessage->GetAsyncMsgReqStatus() == ASYNC_MSG_STATUS_SENT2CP ) {
                relativeTimeOut = pMessage->GetTimeout();
                tv = pMessage->GetAsyncMsgReqStartTime();
                tv.tv_sec = tv.tv_sec + relativeTimeOut /1000;
                tv.tv_usec = tv.tv_usec + ((relativeTimeOut % 1000) * 1000);

                if(pResultMsg == NULL) {
                    pResultMsg = pMessage;
                    earlistTimeout = tv;
                    earlistToken = iter->first;
                }
                else {
                    if(earlistTimeout.tv_sec > tv.tv_sec || (earlistTimeout.tv_sec == tv.tv_sec && earlistTimeout.tv_usec > tv.tv_usec)){
                        pResultMsg = pMessage;
                        earlistTimeout = tv;
                        earlistToken = iter->first;
                    }
                }
            }
        }
    }
    Unlock();

    if(pResultMsg != NULL) {
        pTv->tv_sec = earlistTimeout.tv_sec;
        pTv->tv_usec = earlistTimeout.tv_usec;
        *pToken = earlistToken;
    }

    return pResultMsg;
}

void CAsyncMsgReqHistory::Clear()
{
    Lock();
    if ( m_asyncMsgReqMap.size() > 0 ) {
        for ( map<UINT, Message *>::iterator iter = m_asyncMsgReqMap.begin(); iter != m_asyncMsgReqMap.end(); ++iter) {
            Message *pReqMsg = iter->second;
            if (pReqMsg != NULL) {
                delete pReqMsg;
            }
        }
    }

    m_asyncMsgReqMap.clear();
    Unlock();
}
