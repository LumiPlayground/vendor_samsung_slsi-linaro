/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "servicefactory.h"
#include "rillog.h"
#include "defaultservicefactory.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

ServiceFactory::ServiceFactory()
{
    mInit = false;
    ClearAvailableService();
}

void ServiceFactory::AddAvailableService(UINT serviceId)
{
    mAvailableServiceList.push_back(serviceId);
}

void ServiceFactory::ClearAvailableService()
{
    mAvailableServiceList.clear();
}

void ServiceFactory::Init() {
    RilLogI("DefaultServiceFactory: Init");
    if (mInit) {
        RilLogI("already initialized.");
        return ;
    }

    OnInitialize();
    mInit = true;
}

Service *ServiceFactory::CreateService(RilContext * /* pRilContext */, UINT nServiceId) {
    RilLogW("Need to call DefaultSerivceFactory : service ID=%d", nServiceId);
    return NULL;
}

void ServiceFactory::CreateService(RilContext *pRilContext, ServiceMgr *pServiceMgr)
{
    RilLogI("[ServiceFactory] %s", __FUNCTION__);
    if (pRilContext == NULL || pServiceMgr == NULL) {
        RilLogE("Invalid parameter : pRilContext=%p pServiceMgr=%p", pRilContext, pServiceMgr);
        return ;
    }

    list<UINT>::iterator iter;
    for (iter = mAvailableServiceList.begin(); iter != mAvailableServiceList.end(); ++iter) {
        UINT serviceId = *iter;
        Service* pService = CreateService(pRilContext, serviceId);
        if (pServiceMgr->AddService(serviceId, pService) < 0) {
            RilLogW("Failed to add new service instance : service ID=%d", serviceId);
        }
    } // end for iter ~
}

/**
 * static
 */
static DefaultServiceFactory sDefaultServiceFactory;
static bool sInit = []() { sDefaultServiceFactory.Init(); return true; }();
ServiceFactory *ServiceFactory::GetDefaultServiceFactory() {
    return &sDefaultServiceFactory;
}
