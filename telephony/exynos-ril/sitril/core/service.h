/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "message.h"
#include "modemstatemonitor.h"
#include "mutex.h"
#include "requestdata.h"
#include "rilcontext.h"
#include "rilparser.h"
#include "rilproperty.h"
#include "serviceinterface.h"
#include "servicemgr.h"
#include "servicemonitorrunnable.h"

#include <map>

#define    __WR_USING_STL__

#define    TIMEOUT_NO_WAIT         0
#define    TIMEOUT_INFINITE        0xFFFFFFFF
#define    ASYNC_MSG_TIMER_DELETE_REASON_NORMAL    (0)
#define    ASYNC_MSG_TIMER_DELETE_REASON_TIMEOUT   (1)
#define    ASYNC_MSG_TIMER_DELETE_REASON_OTHERS    (2)
#define    OCCUPY_RF_STATUS_PLMN_SEARCH    (1)
#define    OCCUPY_RF_STATUS_DIAL           (2)

#define INTERFACE_AS_SERVICE(theClassName, servieId) \
public: \
    static theClassName *AsService(Service *service) { \
        if (service != NULL && service->GetServiceId() == servieId) \
            return (theClassName *)service; \
        return NULL; } \

class ServiceState;
class Thread;

class CAsyncMsgReqHistory
{
private:
    CMutex m_lock;
    std::map<UINT, Message *> m_asyncMsgReqMap;

  public:
    CAsyncMsgReqHistory();
    virtual ~CAsyncMsgReqHistory();

public:
    UINT GetSize();
    bool Push(UINT token, Message *pMsg);
    UINT GetBegin();
    void Remove(UINT token);
    Message *Find(UINT token);
    bool IsReqInQueue(UINT nReqId);
    Message *GetReqMsgForEarlistTimeOut(struct timeval *pTv, UINT *pToken);
    void Clear();

protected:
    inline void Lock() { m_lock.lock(); }
    inline void Unlock() { m_lock.unlock(); }
};

class Service : public ServiceInterface
{
    DECLARE_MODULE_TAG()

    typedef struct _async_msg_timer_param
    {
        void *callback_handler;
        UINT token;
    } async_msg_timer_param;

private:
    friend class ServiceMonitorRunnable;
    friend class ServiceMgr;

    ServiceMonitorRunnable *m_pServiceMonitorRunnable;
    Thread* m_pPoll;
    bool m_bIsInited;
    CAsyncMsgReqHistory *m_pAsyncMsgHistory;
    CMutex *m_pAsyncTimerMutex;
    std::map<UINT, std::pair<timer_t, async_msg_timer_param *>> m_asyncMsgReqTimerMap;

  protected:
    CMutex *m_pMutex;
    struct timeval m_tvStart;   //

private:
    /**
     * constructor
     */
public:
    Service(RilContext* pRilContext, UINT nServiceId = RIL_SERVICE_UNKNOWN);
    Service(const Service &) = delete;
    Service& operator=(Service const&) = delete;
    virtual ~Service();

protected:
    int Init();
    int Start();
    int Finalize();

    virtual const char *GetName() { return GetServiceName(); }
    // Async Msg Req Interface
    int HandleAsycMsgRequest();
    BOOL IsInTransaction(UINT nReqId);
    void AsyncMsgReqPreProcessing(Message *pMsg);
    void AsyncMsgReqInsertHistory(UINT token, Message * pMsg);
    void AsyncMsgReqRemoveHistory(UINT token);
    void AsyncMsgReqStartTimer(UINT token, Message * pMsg);
    void AsyncMsgReqDeleteTimer(UINT token, int reason);
    void AsyncMsgReqTimerCallback(UINT token);
    timer_t StartTimer(UINT token, int timeout, void (pAction)(union sigval));
    void AsycMsgReqClearHistory();
    void AsycMsgReqClear();

    unsigned int GetOpenCarrierIndex();
    void SetOpenCarrierIndex(const char* mccmnc);
    std::string GetSimOperatorNumeric();
    virtual void CheckCountsForTimeOut(ModemData *pModemData);
public:
    const char *GetServiceName() { return m_szSvcName; }
    int SetRequestTimeout(UINT timeout);
    int SetRequestTimeout(Message *msg, UINT timeout);
    int ResetRequestTimeout(UINT timeout);
    int CalculateTimeout(struct timeval *pTv);
    int CalcNextTimeout(struct timeval *tv);
    RequestData *GetRequestData(Message *response);
    bool PostponeRequestMessage(unsigned int millis);
    void SetServiceMonitorRunnable(ServiceMonitorRunnable *pServiceMonitorRunnable);
    virtual int SendRequest(ModemData *pModemData, UINT timeout);
    virtual int SendRequest(ModemData *pModemData, UINT timeout, UINT nResult, Message *pMsg = NULL);
    virtual int SendRequest(ModemData *pModemData);
    void OnRequestInternal(int request, void *data = NULL, unsigned int datalen = 0);
    void OnRequestInternal(int request, void *data, unsigned int datalen, RIL_Token t);
    virtual void OnRequestInternalComplete(RIL_Token t, int id, int result, void *data = NULL, int datalen = 0);

    virtual int OnRequestComplete(int result, void *data = NULL, int length = 0, Message *pMsg = NULL);
    int OnAsyncRequestComplete(int result, void *data = NULL, int length = 0, Message *pMsg = NULL);

    // static
public:
    static const char *GetServiceName(const Service *pService);
    static void AsyncMsgReqTimerHandlerWrapper(union sigval sv);
};

#endif
