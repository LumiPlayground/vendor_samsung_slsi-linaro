/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __SERVICE_INTERFACE_H__
#define __SERVICE_INTERFACE_H__

#include "mutex.h"
#include "rilproperty.h"
#include "servicecallback.h"
#include "servicecallbacknotifier.h"
#include "servicehandler.h"
#include "timerlist.h"

#include <map>
#include <queue>
#include <vector>

class RilContext;

typedef enum {
    RIL_SERVICE_UNKNOWN,
    RIL_SERVICE_CSC,            // 1
    RIL_SERVICE_PS,             // 2
    RIL_SERVICE_SIM,            // 3
    RIL_SERVICE_MISC,           // 4
    RIL_SERVICE_NETWORK,        // 5
    RIL_SERVICE_SMS,            // 6
    RIL_SERVICE_AUDIO,          // 7
    RIL_SERVICE_IMS,            // 8
    RIL_SERVICE_GPS,            // 9
    RIL_SERVICE_VSIM,           // 10
    RIL_SERVICE_STK,            // 11
    RIL_SERVICE_SUPPLEMENTARY,  // 12
    RIL_SERVICE_EMBMS,          // 13
    RIL_SERVICE_SIM_IO,         // 14
    RIL_SERVICE_SAP,            // 15
    RIL_SERVICE_UNSUPPORT,
    RIL_SERVICE_MAX,
} RIL_SERVICE_TYPE;

class ServiceInterface : public ServiceHandler,
                         ServiceCallback::RadioStateListener,
                         ServiceCallback::ModemStateListener,
                         ServiceCallback::NetworkStateListener,
                         ServiceCallback::ImsRegistrationStateListener,
                         ServiceCallback::SimStatusListener {
  public:
    enum { UNAVAILABLE,
           CREATED,
           STARTED,
           DESTROYED };

  protected:
    RilContext *m_pRilContext;
    int m_nOverallRadioState;
    int mModemState;
    int m_nVoiceNetworkState;
    int m_nDataNetworkState;
    uint32_t m_nServiceId;
    char m_szSvcName[32];
    uint32_t m_nServiceState;
    RilProperty mServiceProperty;
    CMutex mMutex;
    int m_nMsgPipeR;
    int m_nMsgPipeW;
    Message *m_pCurReqMsg;
    // handling timeout and postpone messages
    TimerList mTimerList;
    // 2015.06.20 : currently, no response is checked only inside of each service.
    // Need to be considered to switch to check NR regardless of service category
    static const unsigned int MAX_NR_COUNT = 3;
    unsigned int m_nr_count;
    UINT m_nCurrentReqId;

    // ServiceHandler map
    std::map<int, ServiceHandler *> mHandlerMap;

    // for RadioStateListener
    ServiceCallback::RadioStateNotifier mRadioStateListener;
    ServiceCallback::ModemStateNotifier mModemStateListener;
    ServiceCallback::NetworkStateNotifier mNetworkStateListener;
    ServiceCallback::SimStatusNotifier mSimStatusListener;

  private:
    // STD queue
    std::queue<Message *> m_requestQueue;
    std::queue<Message *> m_responseQueue;
    std::queue<Message *> m_internalQueue;
    // Async Message Req Interface
    std::queue<Message *> m_asyncMsgReqQueue;

  public:
    ServiceInterface(RilContext *rilContext);
    ServiceInterface(const ServiceInterface&) = delete;
    ServiceInterface& operator=(const ServiceInterface&) = delete;
    virtual ~ServiceInterface();

  protected:
    virtual int OnCreate(RilContext *pRilContext);
    virtual void OnStart();
    virtual void OnDestroy();

    int OpenMessagePipe();
    int GetReadPipe() {
        return m_nMsgPipeR;
    }
    int NotifyNewMessage(Message *pMsg);
    int NotifyNewMessage(MsgDirection direction);
    void NotifyNextRequestMessage();

    void SetTransaction(UINT nReqId) {
        m_nCurrentReqId = nReqId;
    }
    void UnsetTransaction() {
        m_nCurrentReqId = 0;
    }

    ServiceHandler *FindServiceHandler(int messageId);
    ServiceHandler *GetServiceHandler(Message *msg, bool legacy = false);
    void RegisterServiceHandler(int messageId, ServiceHandler *handler);
    int HandleRequest();
    int HandleResponse();
    int HandleRequestTimeout();
    int HandleInternalMessage();
    virtual void CheckCountsForTimeOut(ModemData *) {}
    int SendMessageDelayed(Message *pMsg, long delayMillis);

    virtual bool OnHandleRequest(Message *msg);
    virtual bool OnHandleSolicitedResponse(Message *msg);
    virtual bool OnHandleUnsolicitedResponse(Message *msg);
    virtual bool OnHandleInternalMessage(Message *msg);
    virtual bool OnHandleRequestTimeout(Message *msg);
    const char *GetName();

    // System Broadcast Message Handler
    virtual void OnSimStatusChanged(int, int) {}
    virtual void OnImsiUpdated(const char *) {}
    virtual void OnImsiUpdated(const char *, const char *) {}
    virtual void OnReset() {}
    virtual void OnDataCallStateChanged(int, bool) {}
    virtual bool IsPossibleToPassInRadioOffState(int) {
        return false;
    }
    virtual bool IsPossibleToPassInRadioUnavailableState(int) {
        return false;
    }

    void SetServiceId(UINT nServiceId);
    void SetRadioState(int radioState, bool forceNotify);
    void SetRadioStateUnavailable();
    Service *GetOppositeService(int nServiceId);
    Service *GetCurrentService(int nServiceId);

    int InitMessageQueue();
    void ClearMessageQueue();

  public:
    Message *reqDeQ();
    Message *respDeQ();
    Message *internalDeQ();
    Message *asycMsgReqDeQ();
    int EnQueue(Message *pMsg);
    Message *GetCurrentMsg() {
        return m_pCurReqMsg;
    }
    RequestData *GetCurrentRequestData();
    int32_t GetCurrentRequestHalVersionCode();
    int StartRequestTimeout();

    uint32_t GetServiceId() const {
        return m_nServiceId;
    }
    uint32_t GetServiceState() const {
        return m_nServiceState;
    };
    bool IsRadioNotAvailable() const;
    bool IsRadioAvailable() const;
    bool IsRadioOn() const;
    bool IsRadioOffOrNotAvailable() const;
    int GetRadioState() const {
        return m_nOverallRadioState;
    }
    int GetModemState() {
        return mModemState;
    }
    int GetVoiceNetworkState() const {
        return m_nVoiceNetworkState;
    }
    int GetDataNetworkState() const {
        return m_nDataNetworkState;
    }
    bool IsVoiceNetworkAvailable() const {
        return (m_nVoiceNetworkState == REGISTERED_HOME ||
                m_nVoiceNetworkState == REGISTERED_ROAMING);
    }
    bool IsDataNetworkAvailable() const {
        return (m_nDataNetworkState == REGISTERED_HOME ||
                m_nDataNetworkState == REGISTERED_ROAMING);
    }

    RilContext *GetRilContext();
    RIL_SOCKET_ID GetRilSocketId();
    RilProperty *GetRilContextProperty();
    RilProperty *GetRilApplicationProperty();
    RilProperty *GetServiceProperty() {
        return &mServiceProperty;
    }
    int GetCurrentRequestDataHalVersion(int halVer);
    ServiceState GetRadioServiceState();

    void OnRequest(int request, void *data = NULL, unsigned int datalen = 0);
    void OnRequest(int request, void *data, unsigned int datalen, long delayedMillis);
    virtual int OnRequestComplete(int result, void *data = NULL, int length = 0, Message *pMsg = NULL);
    virtual int OnUnsolicitedResponse(int id, const void *data = NULL, int length = 0);
    int OnRequestAck();

    // listener interfaces
    void addRadioStateListener(ServiceCallback::RadioStateListener *listener);
    void removeRadioStateListener(ServiceCallback::RadioStateListener *listener);
    void addModemStateListener(ServiceCallback::ModemStateListener *listener);
    void removeModemStateListener(ServiceCallback::ModemStateListener *listener);
    void addNetworkStateListener(ServiceCallback::NetworkStateListener *listener);
    void removeNetworkStateListener(ServiceCallback::NetworkStateListener *listener);
    void addSimStatusListener(ServiceCallback::SimStatusListener *listener);
    void removeSimStatusListener(ServiceCallback::SimStatusListener *listener);

  public:
    static const char *GetServiceName(uint32_t nServiceId);
};

#endif  // __SERVICE_INTERFACE_H__
