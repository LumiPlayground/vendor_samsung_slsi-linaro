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
#include <radiomessagingcallback.h>

namespace android::hardware::radio::impl::messaging {

struct BaseMessagingCommand : public BaseCommand {
    using CallbackType = android::hardware::radio::impl::RadioMessagingResponseCallback;
    std::shared_ptr<CallbackType> &mCb;

    BaseMessagingCommand(std::shared_ptr<CallbackType> &cb)
        : BaseCommand(), mCb(cb) {}

    BaseMessagingCommand(uint32_t slotId, std::shared_ptr<CallbackType> &cb)
        : BaseCommand(slotId), mCb(cb) {}

    BaseMessagingCommand(const VendorFunctions &vendorFunctions, std::shared_ptr<CallbackType> &cb)
        : BaseCommand(vendorFunctions), mCb(cb) {}
};

struct BaseMessagingIndication : public BaseIndication {
    using CallbackType = android::hardware::radio::impl::RadioMessagingIndicationCallback;
    std::shared_ptr<CallbackType> &mCb;

    BaseMessagingIndication(std::shared_ptr<CallbackType> &cb)
        : BaseMessagingIndication(0, cb) {}
    BaseMessagingIndication(uint32_t slotId, std::shared_ptr<CallbackType> &cb)
        : BaseIndication(slotId), mCb(cb) {}
};

}  // namespace android::hardware::radio::impl::messaging