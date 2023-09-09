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
 * servicehandler.h
 *
 *  Created on: 2020. 7. 6.
 *      Author: sungwoo48.choi
 */

#ifndef __SERVICE_HANDLER_H__
#define __SERVICE_HANDLER_H__

///////////////////////////////////////////////
// macro for ServiceHandler
///////////////////////////////////////////////

#define DECLARE_CREATOR(theClassName) \
public:\
    static theClassName *Create(Service *service); \

#define IMPLEMENT_CREATOR(theClassName) \
theClassName *theClassName::Create(Service *service) \
{ \
    return new theClassName(service); \
} \

#define SERVICE_HANDLER_INSTANCE(theClassName, service) \
theClassName::Create(service);


class Message;
class Service;

/**
 * ServiceHandler
 */
class ServiceHandler {
public:
    virtual ~ServiceHandler() {}
    virtual bool OnHandleRequest(Message *msg)=0;
    virtual bool OnHandleSolicitedResponse(Message *msg)=0;
    virtual bool OnHandleUnsolicitedResponse(Message *msg)=0;
    virtual bool OnHandleRequestTimeout(Message *msg)=0;
    virtual bool OnHandleInternalMessage(Message *msg)=0;
    virtual const char *GetName()=0;
};

///////////////////////////////////////////////
// macro for ServiceHandlerFactory
///////////////////////////////////////////////
typedef ServiceHandler * (*ServiceHandlerLoader)(int, Service *);

#define DECLARE_SERVICE_HANDLER_FACTORY_INIT(theClassName) \
private: \
    static const char *TAG; \
    static theClassName instance; \
    /*
     static ServiceHandler *OnCallbackLoader(ServiceHandlerLoader loader, int messageId, Service *service) \
        { return (loader ? loader(messageId, service) : nullptr); } \
    */ \
public: \
    theClassName(); \
    virtual ~theClassName() {} \
    virtual const char *GetName() { return theClassName::TAG; } \
protected: \
    ServiceHandler *OnLoad(int messageId, Service *service); \

#define IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(theClassName) \
const char * theClassName::TAG = #theClassName; \
theClassName theClassName::instance; \
theClassName::theClassName() { OnCreate(); ServiceHandlerManager::Publish(this); } \

#define ON_LOAD_SERVICE_HANDLER(theClassLoader, messageId, service) \
(theClassLoader ? theClassLoader(messageId, service) : nullptr);

#define BEGIN_FACTORY_ENTRY(theClassName) \
ServiceHandler *theClassName::OnLoad(int messageId, Service *service) { \
    if (service == nullptr) return nullptr; \
    switch (messageId) { \

#define ADD_FACTORY_ENTRY(theClassName, messagId) \
case messagId: return theClassName::Create(service); \

#define ADD_FACTORY_ENTRY_BY_LOADER(theClassLoader, messagId) \
case messagId: return OnCallbackLoader(theClassLoader, messageId, service); \

#define END_FACTORY_ENTRY() \
    default: break; } \
    return nullptr; \
} \


/**
 * ServiceHandlerFactory
 */
class ServiceHandlerFactory {
public:
    virtual ~ServiceHandlerFactory() {}
    virtual const char *GetName()=0;
    ServiceHandler *GetHandler(int messageId, Service *service);
protected:
    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual ServiceHandler *OnPreload(int, Service *) { return nullptr; }
    virtual ServiceHandler *OnPostload(int, Service *) { return nullptr; }
    virtual ServiceHandler *OnLoad(int messageId, Service *service)=0;
};


///////////////////////////////////////////////
// macro for ServiceHandlerManager
///////////////////////////////////////////////

#define PUBLISH_SERVICE_HANDLER_FACTORY() \
ServiceHandlerManager::Publish(this); \

/**
 * ServiceHandlerManager
 */
class ServiceHandlerManager {
public:
    static void Publish(ServiceHandlerFactory *serviceHandlerFactory);
    static ServiceHandler *GetHandler(int messageId, Service *service);
};


/**
 * ServiceHandlerHolder
 *  - A proxy handler
 */
class ServiceHandlerHolder : public ServiceHandler {
private:
    ServiceHandler *mHandler;
public:
    ServiceHandlerHolder(ServiceHandler *handler);
    virtual ~ServiceHandlerHolder() { mHandler = nullptr; }
    bool OnHandleRequest(Message *msg);
    bool OnHandleSolicitedResponse(Message *msg);
    bool OnHandleUnsolicitedResponse(Message *msg);
    bool OnHandleRequestTimeout(Message *msg);
    bool OnHandleInternalMessage(Message *msg);
    const char *GetName();
};

#endif /* __SERVICE_HANDLER_H__ */
