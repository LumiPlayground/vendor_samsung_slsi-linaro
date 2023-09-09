/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "memlogservice.h"
#include "MemlogServiceImpl.h"
#include <string>
#include <map>

using namespace std;
static map<string, MemlogService *> sMemlogServiceList;

HANDLE registerService(const char *serviceName, MEMLOG_ServiceFunctions *func) {
    dlog("registerService");

    if (serviceName == NULL || *serviceName == 0) {
        dlog("invalid service name");
        return NULL;
    }

    if (func == NULL) {
        dlog("invalid param. MEMLOG_SerivceFunctions is NULL.");
        return NULL;
    }

    string key = serviceName;
    map<string, MemlogService *>::iterator iter = sMemlogServiceList.find(key);
    if (iter != sMemlogServiceList.end()) {
        dlog("serviceName %s already existed.", serviceName);
        return iter->second;
    }

    MemlogService *newMemlogService = MemlogService::makeInstance(serviceName, func);
    if (newMemlogService != NULL) {
        if (newMemlogService->registerService() == android::OK) {
            sMemlogServiceList[key] = newMemlogService;
            dlog("registerService as \"%s\", MEMLOG_SerivceFunctions=0x%p", serviceName, func);
        }
        else {
            dlog("failed to registerService as \"%s\"", serviceName);
            delete newMemlogService;
            return NULL;
        }
    }
    else {
        dlog("memory allocation fail or invalid parameter");
    }

    return (HANDLE)newMemlogService;
}

void releaseService(HANDLE h) {
    dlog("releaseService");
    MemlogService *memlogService = (MemlogService *)h;
    if (memlogService != NULL) {
        const char *serviceName = memlogService->getServiceName();
        dlog("HANDLE=0x%p serviceName=%s", memlogService, serviceName);
        map<string, MemlogService *>::iterator iter = sMemlogServiceList.find(string(serviceName));
        if (iter != sMemlogServiceList.end()) {
            sMemlogServiceList.erase(iter);
        }
        else {
            dlog("unregistered service instance, serviceName=%s", serviceName);
        }

        delete memlogService;
        memlogService = NULL;
    }
    else {
        dlog("invalid HANDLE");
    }
}

void notifyCallback(HANDLE h, int type, int id, void *data, unsigned int datalen) {
    //dlog("notifyCallback");
    MemlogService *memlogService = (MemlogService *)h;
    if (memlogService != NULL) {
        const char *serviceName = memlogService->getServiceName();
        map<string, MemlogService *>::iterator iter = sMemlogServiceList.find(string(serviceName));
        if (iter != sMemlogServiceList.end()) {
            //dlog("(%s)notifyCallback", memlogService->getServiceName());
            memlogService->onCallback(type, id, data, datalen);
        }
        else {
            dlog("unregistered service instance, serviceName=%s", serviceName);
        }
    }
}
