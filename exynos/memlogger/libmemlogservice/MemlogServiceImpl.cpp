/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include "MemlogServiceImpl.h"

MemlogServiceImpl::MemlogServiceImpl() {
    mFunc = NULL;
    mCallback = NULL;
    mDeathRecipient = new MemlogServiceDeathRecipient(this);
}

Return<void> MemlogServiceImpl::sendRequestRaw(int32_t type, int32_t id, const ::android::hardware::hidl_vec<uint8_t>& rawBytes) {
    dlog("sendRequestRaw");
    if (mFunc != NULL) {
        const uint8_t *uData = rawBytes.data();
        mFunc->onRequest(type, id, (void *)uData, rawBytes.size());
    }
    return Void();
}

Return<void> MemlogServiceImpl::setCallback(const ::android::sp<IMemlogServiceCallback>& callback) {
    dlog("setCallback");

    mCallback = callback;
    if (mCallback != NULL) {
        dlog("setCallback: linkToDeath");
        mCallback->linkToDeath(mDeathRecipient, 0 /*cookie*/);
    }
    return Void();
}
Return<void> MemlogServiceImpl::close() {
    dlog("close");

    if (mCallback != NULL) {
        dlog("close: unlinkToDeath");
        mCallback->unlinkToDeath(mDeathRecipient);
    }
    mCallback = NULL;
    return Void();
}

void MemlogServiceImpl::checkReturnStatus(Return<void>& ret) {
    if (ret.isOk() == false) {
        dlog("checkReturnStatus: unable to call callback. Client may be died.");
        this->close();
    }
}

MemlogService *MemlogService::makeInstance(const char *serviceName, MEMLOG_ServiceFunctions *func) {
    if (serviceName == NULL || *serviceName == 0) {
        return NULL;
    }

    MemlogService *memlogService = new MemlogService(serviceName);
    if (memlogService != NULL) {
        memlogService->setServiceFunction(func);
    }
    return memlogService;
}

MemlogService::MemlogService(const char *serviceName) {
    memset(mServiceName, 0, sizeof(mServiceName));
    memset(&mFunc, 0, sizeof(mFunc));
    mMemlogService = NULL;
    if (serviceName != NULL) {
        strncpy(mServiceName, serviceName, MAX_SERVICE_NAMX);
        mMemlogService = new MemlogServiceImpl;
        mMemlogService->mFunc = NULL;
        mMemlogService->mCallback = NULL;
    }
}

int MemlogService::registerService() {
    android::status_t status = mMemlogService->registerAsService(mServiceName);
    dlog("[%s]registerService %d", mServiceName, status);
    return status;
}

void MemlogService::setServiceFunction(MEMLOG_ServiceFunctions *func) {
    dlog("[%s]setServiceFunction", mServiceName);
    if (func != NULL) {
        memcpy(&mFunc, func, sizeof(MEMLOG_ServiceFunctions));
        mMemlogService->mFunc = &mFunc;
    }
}

int MemlogService::onCallback(int type, int id, void *data, unsigned int datalen) {
    //dlog("[%s]onCallback", mServiceName);
    if (mMemlogService != NULL && mMemlogService->mCallback != NULL) {
        if (data == NULL || datalen == 0) {
            dlog("invalid data");
            return -1;
        }

        hidl_vec<uint8_t> rawBytes;
        rawBytes.setToExternal((uint8_t *) data, datalen);
        Return<void> retStatus = mMemlogService->mCallback->onCallback(type, id, rawBytes);
        mMemlogService->checkReturnStatus(retStatus);
    } else {
        if (mMemlogService == NULL)
            dlog("service not available");
        else if (mMemlogService->mCallback == NULL)
            dlog("callback not available");
    }

    return 0;
}
