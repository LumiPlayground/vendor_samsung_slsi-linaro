/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#ifndef __MEMLOG_SERVICE_IMPL_H__
#define __MEMLOG_SERVICE_IMPL_H__

#include <vendor/samsung_slsi/hardware/memlogservice/1.0/IMemlogService.h>
#include <vendor/samsung_slsi/hardware/memlogservice/1.0/IMemlogServiceCallback.h>
#include "memlogservice.h"
#include "memlog_internal.h"

using namespace vendor::samsung_slsi::hardware::memlogservice::V1_0;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using android::sp;
using android::wp;

struct MemlogServiceDeathRecipient : hidl_death_recipient {
    MemlogServiceDeathRecipient(const sp<IMemlogService> memlogServiceImpl) : mMemlogServiceImpl(memlogServiceImpl) {
    }

    virtual void serviceDied(uint64_t /*cookie*/, const wp<::android::hidl::base::V1_0::IBase>& /*who*/) {
        dlog("MemlogServiceDeathRecipient::serviceDied");
        mMemlogServiceImpl->close();
    }
    sp<IMemlogService> mMemlogServiceImpl;
};

struct MemlogServiceImpl : public IMemlogService {
    MemlogServiceImpl();
    sp<IMemlogServiceCallback> mCallback;
    MEMLOG_ServiceFunctions *mFunc;
    sp<MemlogServiceDeathRecipient> mDeathRecipient;

    Return<void> sendRequestRaw(int32_t type, int32_t id, const ::android::hardware::hidl_vec<uint8_t>& data);
    Return<void> setCallback(const ::android::sp<IMemlogServiceCallback>& callback);
    Return<void> close();
    void checkReturnStatus(Return<void>& ret);
};

class MemlogService {
private:
    sp<MemlogServiceImpl> mMemlogService;
    char mServiceName[MAX_SERVICE_NAMX + 1];
    MEMLOG_ServiceFunctions mFunc;

public:
    MemlogService(const char *serviceName);

public:
    const char *getServiceName() const { return mServiceName; }
    int registerService();
    void setServiceFunction(MEMLOG_ServiceFunctions *func);
    int onCallback(int type, int id, void *data, unsigned int datalen);
public:
    static MemlogService *makeInstance(const char *serviceName, MEMLOG_ServiceFunctions *func);
};


#endif // __MEMLOG_SERVICE_IMPL_H__
