/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#pragma once

#include <basecommand.h>
#include <radiosimcallback.h>

namespace android::hardware::radio::impl::sim {
struct BaseSimCommand : public BaseCommand {
    using CallbackType = android::hardware::radio::impl::RadioSimResponseCallback;
    std::shared_ptr<CallbackType> &mCb;

    BaseSimCommand(std::shared_ptr<CallbackType> &cb) : BaseCommand(), mCb(cb) {}
    BaseSimCommand(uint32_t slotId, std::shared_ptr<CallbackType> &cb)
            : BaseCommand(slotId), mCb(cb) {}
    BaseSimCommand(const VendorFunctions &vendorFunctions,
            std::shared_ptr<CallbackType> &cb) : BaseCommand(vendorFunctions), mCb(cb) {}
};

struct BaseSimIndication : public BaseIndication {
    using CallbackType = android::hardware::radio::impl::RadioSimIndicationCallback;
    std::shared_ptr<CallbackType> &mCb;
    BaseSimIndication(std::shared_ptr<CallbackType> &cb) : BaseSimIndication(0, cb) {}
    BaseSimIndication(uint32_t slotId, std::shared_ptr<CallbackType> &cb)
            : BaseIndication(slotId), mCb(cb) {}
};


/**
 * GetIccCardStatusCommand
 */
struct GetIccCardStatusCommand : public BaseSimCommand {
    using BaseSimCommand::BaseSimCommand;

    const char *TAG = "GetIccCardStatusCommand";
    void onRequest(int32_t serial);
    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

}
