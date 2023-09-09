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
 * servicehandler.cpp
 *
 *  Created on: 2020. 7. 13.
 *      Author: sungwoo48.choi
 */

#include <list>
#include "rillog.h"
#include "service.h"
#include "servicehandler.h"
#include "servicehandlerbase.h"

#ifdef RilLogV
#undef RilLogV
#define RilLogV(format, ...) if (LOG_VDBG) CRilLog::Log(CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__)
#endif

/**
 * ServiceHandlerFactory
 */
ServiceHandler *ServiceHandlerFactory::GetHandler(int messageId, Service *service)
{
    if (messageId < 0 || service == NULL) {
        RilLogV("%s %s invalid parameter", GetName(), __FUNCTION__);
        return NULL;
    }

    ServiceHandler *h = NULL;
    // preload
    if ((h = OnPreload(messageId, service)) != NULL) {
        return h;
    }

    // sub classes must implement OnLoad()
    if ((h = OnLoad(messageId, service)) != NULL) {
        return h;
    }

    // postload
    if ((h = OnPostload(messageId, service)) != NULL) {
        return h;
    }
    return NULL;
}


/**
 * ServiceHandlerFactoryManagerImpl
 */
class ServiceHandlerFactoryManagerImpl : public ServiceHandlerManager
{
private:
    static ServiceHandlerFactoryManagerImpl *instance;
    std::list<ServiceHandlerFactory *> mFactory;
public:
    static ServiceHandlerFactoryManagerImpl *GetInstance();
    void Add(ServiceHandlerFactory *factory) {
        if (factory != NULL) {
            mFactory.push_back(factory);
        }
    }
    std::list<ServiceHandlerFactory *> &GetList() { return mFactory; }
};

ServiceHandlerFactoryManagerImpl *ServiceHandlerFactoryManagerImpl::instance = NULL;
ServiceHandlerFactoryManagerImpl *ServiceHandlerFactoryManagerImpl::GetInstance()
{
    if (instance == NULL) {
        instance = new ServiceHandlerFactoryManagerImpl();
    }
    return instance;
}

/**
 * ServiceHandlerManager
 */
void ServiceHandlerManager::Publish(ServiceHandlerFactory *factory)
{
    if (factory != NULL) {
        RilLogV("[ServiceHandlerManager]Publish %s(%p)", factory->GetName(), factory);
        ServiceHandlerFactoryManagerImpl::GetInstance()->Add(factory);
    }
}

ServiceHandler *ServiceHandlerManager::GetHandler(int messageId, Service *service)
{
    if (messageId < 0 || service == NULL) {
        return NULL;
    }

    std::list<ServiceHandlerFactory *> &factories =
            ServiceHandlerFactoryManagerImpl::GetInstance()->GetList();
    std::list<ServiceHandlerFactory *>::iterator iter = factories.begin();
    for (; iter != factories.end(); iter++) {
        ServiceHandlerFactory *f = *iter;
        if (f != NULL) {
            ServiceHandler *h = f->GetHandler(messageId, service);
            if (h != NULL) {
                RilLogV("[ServiceHandlerManager]ServiceHandler %s(%p) for messiageId=%d in %s",
                        h->GetName(), h, messageId, service->GetServiceName());
                return h;
            }
        }
    } // end for iter ~
    //RilLogV("[ServiceHandlerManager]ServiceHandler not found for messiageId=%d", messageId);
    return NULL;
}

/**
 * ServiceHandlerHolder
 */
ServiceHandlerHolder::ServiceHandlerHolder(ServiceHandler *handler)
    : mHandler(handler)
{
    RilLogV("[ServiceHandlerHolder] %s(%p)", GetName(), mHandler);
}

bool ServiceHandlerHolder::OnHandleRequest(Message *msg)
{
    if (mHandler != NULL) {
        RilLogV("[ServiceHandlerHolder]%s %s(%p)", __FUNCTION__, GetName(), mHandler);
        return mHandler->OnHandleRequest(msg);
    }
    return false;
}

bool ServiceHandlerHolder::OnHandleSolicitedResponse(Message *msg)
{
    if (mHandler != NULL) {
        RilLogV("[ServiceHandlerHolder]%s %s(%p)", __FUNCTION__, GetName(), mHandler);
        return mHandler->OnHandleSolicitedResponse(msg);
    }
    return false;
}

bool ServiceHandlerHolder::OnHandleUnsolicitedResponse(Message *msg)
{
    if (mHandler != NULL) {
        RilLogV("[ServiceHandlerHolder]%s %s(%p)", __FUNCTION__, GetName(), mHandler);
        return mHandler->OnHandleUnsolicitedResponse(msg);
    }
    return false;
}

bool ServiceHandlerHolder::OnHandleRequestTimeout(Message *msg)
{
    if (mHandler != NULL) {
        RilLogV("[ServiceHandlerHolder]%s %s(%p)", __FUNCTION__, GetName(), mHandler);
        return mHandler->OnHandleRequestTimeout(msg);
    }
    return false;
}

bool ServiceHandlerHolder::OnHandleInternalMessage(Message *msg)
{
    if (mHandler != NULL) {
        RilLogV("[ServiceHandlerHolder]%s %s(%p)", __FUNCTION__, GetName(), mHandler);
        return mHandler->OnHandleInternalMessage(msg);
    }
    return false;
}

const char *ServiceHandlerHolder::GetName()
{
    if (mHandler != NULL) {
        return mHandler->GetName();
    }
    return "ServiceHandler<unknown>";
}
