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
#include <radiovoicecallback.h>

namespace android::hardware::radio::impl::voice {

struct BaseVoiceCommand : public BaseCommand {
    using CallbackType = android::hardware::radio::impl::RadioVoiceResponseCallback;
    std::shared_ptr<CallbackType>& mCb;

    BaseVoiceCommand(std::shared_ptr<CallbackType>& cb) : BaseCommand(), mCb(cb) {}

    BaseVoiceCommand(uint32_t slotId, std::shared_ptr<CallbackType>& cb)
            : BaseCommand(slotId), mCb(cb) {}

    BaseVoiceCommand(const VendorFunctions& vendorFunctions, std::shared_ptr<CallbackType>& cb)
            : BaseCommand(vendorFunctions), mCb(cb) {}
};

struct BaseVoiceIndication : public BaseIndication {
    using CallbackType = android::hardware::radio::impl::RadioVoiceIndicationCallback;
    std::shared_ptr<CallbackType>& mCb;

    BaseVoiceIndication(std::shared_ptr<CallbackType>& cb) : BaseVoiceIndication(0, cb) {}
    BaseVoiceIndication(uint32_t slotId, std::shared_ptr<CallbackType>& cb)
            : BaseIndication(slotId), mCb(cb) {}
};

// New commands
/**
  * IsVoNrEnabledCommand
  */
struct IsVoNrEnabledCommand : public BaseVoiceCommand {
    using BaseVoiceCommand::BaseVoiceCommand;

    const char *TAG = "IsVoNrEnabledCommand";
    void onRequest(int serial);
    int onResponse(int slotId,int responseType, int serial, RIL_Errno e, void *response, size_t responselen);

};

/**
  * SetVoNrEnabledCommand
  */
struct SetVoNrEnabledCommand : public BaseVoiceCommand {
    using BaseVoiceCommand::BaseVoiceCommand;

    const char *TAG = "SetVoNrEnabledCommand";
    void onRequest(int serial, bool enable);
    int onResponse(int slotId,int responseType, int serial, RIL_Errno e, void *response, size_t responselen);
};

}
