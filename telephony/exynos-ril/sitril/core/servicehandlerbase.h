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
 * servicehandlerbase.h
 *
 *  Created on: 2020. 7. 6.
 *      Author: sungwoo48.choi
 */

#ifndef __SERVICE_HANDLER_BASE_H__
#define __SERVICE_HANDLER_BASE_H__

#include "types.h"
#include "servicehandler.h"

#define DECLARE_TINY_SERVICE_HANDLER_INIT(theClassName) \
public: \
    static const char *TAG; \
    static theClassName *Create(Service *service); \
public: \
    theClassName(Service *service); \
    virtual ~theClassName(); \
    const char *GetName() { return theClassName::TAG; } \
protected: \
    int OnRequest(Message *msg); \
    int OnResponse(Message *msg); \

#define DECLARE_TINY_UNSOL_SERVICE_HANDLER_INIT(theClassName) \
public: \
    static const char *TAG; \
    static theClassName *Create(Service *service); \
public: \
    theClassName(Service *service); \
    virtual ~theClassName(); \
    const char *GetName() { return theClassName::TAG; } \
protected: \
    int OnIndication(Message *msg); \

#define IMPLEMENT_TINY_SERVICE_HANDLER_INIT(theClassName, messageId) \
const char *theClassName::TAG = #theClassName; \
theClassName::theClassName(Service *service) \
    : ServiceHandlerBase(messageId, service) {} \
theClassName::~theClassName() {} \

#define TINY_SERVICE_HANDLER_ON_REQUEST_BEGIN(theClassName) \
int theClassName::OnRequest(Message *msg) { \

#define TINY_SERVICE_HANDLER_ON_REQUEST_END() \
} \

#define TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(theClassName) \
int theClassName::OnResponse(Message *msg) { \

#define TINY_SERVICE_HANDLER_ON_RESPONSE_END() \
} \

#define IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE() \
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__); \
    if (msg == NULL) { \
        RilLogE("msg is NULL"); \
        return -1; \
    } \
    ModemData *pModemData = msg->GetModemData(); \
    if (pModemData == NULL) { \
        RilLogE("pModemData is NULL"); \
        return -1; \
    } \
    ProtocolRespAdapter adapter(pModemData); \
    int errorCode = adapter.GetErrorCode(); \
    if (errorCode == RIL_E_SUCCESS) { \
        OnRequestComplete(RIL_E_SUCCESS); \
    } else { \
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR); \
    } \
    return 0; \

#define IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2() \
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__); \
    if (msg == NULL) { \
        RilLogE("msg is NULL"); \
        return -1; \
    } \
    ModemData *pModemData = msg->GetModemData(); \
    if (pModemData == NULL) { \
        RilLogE("pModemData is NULL"); \
        return -1; \
    } \
    ProtocolRespAdapter adapter(pModemData); \
    int errorCode = adapter.GetErrorCode(); \
    RilLogV("RilErrorCode(%d)", errorCode); \
    OnRequestComplete(errorCode); \
    return 0; \

#define IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE3() \
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__); \
    if (msg == NULL) { \
        RilLogE("msg is NULL"); \
        return -1; \
    } \
    ModemData *pModemData = msg->GetModemData(); \
    if (pModemData == NULL) { \
        RilLogE("pModemData is NULL"); \
        return -1; \
    } \
    ProtocolRespAdapter adapter(pModemData); \
    int errorCode = adapter.GetErrorCode(); \
    if (errorCode == RIL_E_SUCCESS) { \
        OnRequestComplete(RIL_E_SUCCESS, (void *)adapter.GetParameter(), (int)adapter.GetParameterLength()); \
    } else { \
        OnRequestComplete(RIL_E_GENERIC_FAILURE); \
    } \
    return 0; \

#define IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ADAPTER_CLZ) \
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__); \
    if (msg == NULL) { \
        RilLogE("msg is NULL"); \
        return -1; \
    } \
    ModemData *pModemData = msg->GetModemData(); \
    if (pModemData == NULL) { \
        RilLogE("pModemData is NULL"); \
        return -1; \
    } \
    ADAPTER_CLZ adapter(pModemData); \
    int errorCode = adapter.GetErrorCode(); \
    if (errorCode == RIL_E_SUCCESS) { \
        int result = adapter.GetResult(); \
        RilLogV("[%s] %s(), result = %d", mServiceName, __FUNCTION__, result); \
        OnRequestComplete(errorCode, &result, sizeof(int)); \
    } else { \
        OnRequestComplete(errorCode); \
    } \
    return 0; \

#define IMPLEMENT_TINY_SERVICE_HANDLER_FOO_REQUEST_RESPONSE(theClassName) \
int theClassName::OnRequest(Message *msg) { return -1; } \
int theClassName::OnResponse(Message *msg) { return -1; } \

#define IMPLEMENT_TINY_SERVICE_HANDLER_FOO_INDICATION(theClassName) \
int theClassName::OnIndication(Message *msg) { return -1; } \

#define DECLARE_TINY_SERVICE_HANDLER_CLASS(theClassName) \
class theClassName : public ServiceHandlerBase { \
public: \
    theClassName(Service *service); \
    virtual ~theClassName(); \
    const char *GetName() { return theClassName::TAG; } \
protected: \
    int OnRequest(Message *msg); \
    int OnResponse(Message *msg); \
public:\
    static theClassName *Create(Service *service); \
    static const char *TAG; \
}; \

#define DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(theClassName) \
class theClassName : public ServiceHandlerBase { \
public: \
    theClassName(Service *service); \
    virtual ~theClassName(); \
    const char *GetName() { return theClassName::TAG; } \
protected: \
    virtual int OnIndication(Message *msg); \
public:\
    static theClassName *Create(Service *service); \
    static const char *TAG; \
}; \

#define IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(theClassName, messageId) \
theClassName::theClassName(Service *service) \
    : ServiceHandlerBase(messageId, service) \
{} \
theClassName::~theClassName() \
{} \
const char *theClassName::TAG = #theClassName; \
theClassName *theClassName::Create(Service *service) \
{ theClassName *h = new theClassName(service); \
  if (h) h->OnCreate(); \
  return h; } \

class ModemData;
class RilContext;
class Service;

/**
 * ServiceHandlerBase
 *   An adapter class which implements ServiceHandler class
 */
class ServiceHandlerBase : public ServiceHandler {
protected:
    int mMessageId;
    Service *mService;
    const char *mServiceName;
    RilContext *mRilContext;
    int mPhoneId;

public:
    ServiceHandlerBase(int messageId, Service *service);
    virtual ~ServiceHandlerBase();
    virtual bool OnHandleRequest(Message *msg);
    virtual bool OnHandleSolicitedResponse(Message *msg);
    virtual bool OnHandleUnsolicitedResponse(Message *msg);
    virtual bool OnHandleRequestTimeout(Message *msg);
    virtual bool OnHandleInternalMessage(Message *msg);
    int OnRequestComplete(int result, void *data = NULL, size_t length = 0);

  protected:
    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual int OnRequest(Message *msg);
    virtual int OnResponse(Message *msg);
    virtual int OnIndication(Message *msg);
    virtual int OnTimeout(Message *msg);
    int SendRequest(ModemData *modemData, uint32_t timeout, uint32_t result);
    int SendRequest(ModemData *modemData);
    int OnUnsolicitedResponse(int id, const void *data = NULL, size_t length = 0);
};

#endif /* __SERVICE_HANDLER_BASE_H__ */
