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
 * imshandlerfactory.cpp
 *
 *  Created on: 2020. 7. 8.
 *      Author: sungwoo48.choi
 */
#include "messageid.h"
#include "imshandler.h"
#include "imshandlerfactory.h"
#include "imsmediahandler.h"
#include "rillog.h"
#include "service.h"

#ifdef RilLogV
#undef RilLogV
#define RilLogV(format, ...) if (LOG_VDBG) CRilLog::Log(CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__)
#endif

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(ImsHandlerFactory)

//add Media string to key parameter
#define MEDIA "_Media"

void ImsHandlerFactory::OnCreate()
{
    RilLog("ImsHandlerFactory OnCreate");
}

void ImsHandlerFactory::OnDestroy()
{
}

BEGIN_FACTORY_ENTRY(ImsHandlerFactory)
END_FACTORY_ENTRY()

ServiceHandler *ImsHandlerFactory::OnPostload(int messageId, Service *service)
{
    //RilLogV("%s %s messageId=%d service=%s", GetName(), __FUNCTION__, messageId, service->GetServiceName());
    if (AimsDefaultHandler::CanHandleRequest(messageId) ||
        AimsDefaultHandler::CanHandleRequestInd(messageId) ||
        AimsDefaultHandler::CanHandleIndication(messageId)) {
        std::string key = service->GetServiceName();
        ServiceHandler *h = Find(key);
        if (h == NULL) {
            h = AimsDefaultHandler::Create(service);
            Put(key, std::shared_ptr<ServiceHandler>(h));
        }
        return new ServiceHandlerHolder(h);
    }
    else if (ImsMediaDefaultHandler::CanHandleRequest(messageId) ||
            ImsMediaDefaultHandler::CanHandleRequestInd(messageId) ||
            ImsMediaDefaultHandler::CanHandleIndication(messageId)) {
        std::string key = service->GetServiceName();
        key = key + MEDIA; // Change Key value to IMSSVC_Media to distinguish it from IMSSVC key
        ServiceHandler *h = Find(key);
        if (h == nullptr) {
            h = ImsMediaDefaultHandler::Create(service);
            Put(key, std::shared_ptr<ServiceHandler>(h));
        }
        return new ServiceHandlerHolder(h);
    }

    return NULL;
}

ServiceHandler *ImsHandlerFactory::Find(const std::string& key) {
    auto iter = mHandler.find(key);
    if (iter != mHandler.end()) {
        return iter->second.get();
    }
    return NULL;
}

bool ImsHandlerFactory::Contain(const std::string& key) const {
    auto iter = mHandler.find(key);
    return (iter != mHandler.end());
}

void ImsHandlerFactory::Put(const std::string& key, std::shared_ptr<ServiceHandler> h) {
    mHandler[key] = h;
}
