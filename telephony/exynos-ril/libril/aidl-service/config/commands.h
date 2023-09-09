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
#include <radioconfigcallback.h>

namespace android::hardware::radio::impl::config {

struct BaseConfigCommand : public BaseCommand {
    using CallbackType = android::hardware::radio::impl::RadioConfigResponseCallback;
    std::shared_ptr<CallbackType>& mCb;

    BaseConfigCommand(std::shared_ptr<CallbackType>& cb) : BaseCommand(), mCb(cb) {}

    BaseConfigCommand(uint32_t slotId, std::shared_ptr<CallbackType>& cb)
            : BaseCommand(slotId), mCb(cb) {}

    BaseConfigCommand(const VendorFunctions& vendorFunctions, std::shared_ptr<CallbackType>& cb)
            : BaseCommand(vendorFunctions), mCb(cb) {}
};

struct BaseConfigIndication : public BaseIndication {
    using CallbackType = android::hardware::radio::impl::RadioConfigIndicationCallback;
    std::shared_ptr<CallbackType>& mCb;

    BaseConfigIndication(std::shared_ptr<CallbackType>& cb) : BaseConfigIndication(0, cb) {}
    BaseConfigIndication(uint32_t slotId, std::shared_ptr<CallbackType>& cb)
            : BaseIndication(slotId), mCb(cb) {}
};

// New commands
/**
 * GetHalDeviceCapabilitiesCommand
 */
struct GetHalDeviceCapabilitiesCommand : public BaseConfigCommand {
    using BaseConfigCommand::BaseConfigCommand;

    const char *TAG = "GetHalDeviceCapabilitiesCommand";
    void onRequest(int serial);
    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * getSimSlotsStatus
 */
struct GetSimSlotsStatusCommand : public BaseConfigCommand {
    using BaseConfigCommand::BaseConfigCommand;

    const char *TAG = "GetSimSlotsStatusCommand";
    void onRequest(int serial);
    int onResponse(int slotId, int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * SetLogicalToPhysicalSlotMapping
 */
struct SetLogicalToPhysicalSlotPortMapping : public BaseConfigCommand {
    using BaseConfigCommand::BaseConfigCommand;

    const char *TAG = "SetLogicalToPhysicalSlotPortMapping";
    void onRequest(int serial,
            const std::vector<::aidl::android::hardware::radio::config::SlotPortMapping>& slotMap);
    int onResponse(int slotId, int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * simSlotsStatusChanged
 */
struct SimSlotsStatusChangedIndication : public BaseConfigIndication {
    using BaseConfigIndication::BaseConfigIndication;

    const char *TAG = "SimSlotsStatusChangedIndication";
    int onIndication(int slotId, int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen);
};

}

