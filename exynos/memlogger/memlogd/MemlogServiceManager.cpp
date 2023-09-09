/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "MemlogServiceManager.h"
#include <dlfcn.h>
#include <string.h>

#define LOG_TAG     "MEMLOGGER"
#include <utils/Log.h>

#define LIB_PATH_MEMLOGSERVICE    "libmemlogservice.so"

#define dlog(x...) ALOGD( x )


MemlogServiceManager::MemlogServiceManager() : mHandle(NULL), mLib(NULL) {
    memset(&mFunc, 0, sizeof(Memlog_ServiceFunctions));
    m_pfnRegisterService = NULL;
    m_pFnReleaseService = NULL;
    m_pFnNotifyCallback = NULL;
}

MemlogServiceManager::~MemlogServiceManager() {
    release();
}

bool MemlogServiceManager::init() {
    mLib = dlopen(LIB_PATH_MEMLOGSERVICE, RTLD_NOW);
    if (mLib == NULL) {
        dlog("failed to dlopen for %s. error %s", LIB_PATH_MEMLOGSERVICE, dlerror());
        return false;
    }

    m_pfnRegisterService = (Memlog_RegisterService)dlsym(mLib, "registerService");
    m_pFnReleaseService = (Memlog_ReleaseService)dlsym(mLib, "releaseService");
    m_pFnNotifyCallback = (Memlog_NotifyCallback)dlsym(mLib, "notifyCallback");

    if (m_pfnRegisterService == NULL || m_pFnReleaseService == NULL || m_pFnNotifyCallback == NULL) {
        dlog("failed to load functions");
        release();
        return false;
    }

    dlog("m_pfnRegisterService=0x%p m_pFnReleaseService=0x%p m_pFnNotifyCallback=0x%p",
            m_pfnRegisterService, m_pFnReleaseService, m_pFnNotifyCallback);
    return true;
}

void MemlogServiceManager::release() {
    dlog("%s", __FUNCTION__);
    if (mLib != NULL) {
        dlclose(mLib);
        mLib = NULL;
    }

    m_pfnRegisterService = NULL;
    m_pFnReleaseService = NULL;
    m_pFnNotifyCallback = NULL;

    memset(&mFunc, 0, sizeof(Memlog_ServiceFunctions));
}

int MemlogServiceManager::registerService(const char *serviceName, Memlog_ServiceFunctions *func) {
    if (mLib == NULL) {
        dlog("lib(%s) is not opened yet.", LIB_PATH_MEMLOGSERVICE);
        return -1;
    }

    if (m_pfnRegisterService == NULL) {
        dlog("function is not loaded yet.");
        return -1;
    }

    if (serviceName == NULL || *serviceName == 0) {
        dlog("invalid serviceName");
        return -1;
    }

    if (func == NULL) {
        dlog("invalid Memlog_SerivceFunctions");
        return -1;
    }

    mHandle = m_pfnRegisterService(serviceName, func);
    if (mHandle == NULL) {
        dlog("failed to register service as %s", serviceName);
        return -1;
    }

    memcpy(&mFunc, func, sizeof(Memlog_ServiceFunctions));

    return 0;
}

void MemlogServiceManager::releaseService() {
    if (mLib == NULL) {
        dlog("lib(%s) is not opened yet.", LIB_PATH_MEMLOGSERVICE);
        return ;
    }

    if (m_pFnReleaseService == NULL) {
        dlog("function is not loaded yet.");
        return ;
    }

    m_pFnReleaseService(mHandle);
}

int MemlogServiceManager::notifyCallback(int type, int id, void *data, unsigned int datalen) {
    if (mLib == NULL) {
        dlog("lib(%s) is not opened yet.", LIB_PATH_MEMLOGSERVICE);
        return -1;
    }

    if (m_pFnNotifyCallback == NULL) {
        dlog("function is not loaded yet.");
        return -1;
    }

    m_pFnNotifyCallback(mHandle, type, id, data, datalen);
    return 0;
}
