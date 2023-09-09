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
#include <radiomodemcallback.h>

namespace android::hardware::radio::impl::modem {

struct BaseModemCommand : public BaseCommand {
    using CallbackType = android::hardware::radio::impl::RadioModemResponseCallback;
    std::shared_ptr<CallbackType>& mCb;

    BaseModemCommand(std::shared_ptr<CallbackType>& cb) : BaseCommand(), mCb(cb) {}

    BaseModemCommand(uint32_t slotId, std::shared_ptr<CallbackType>& cb)
            : BaseCommand(slotId), mCb(cb) {}

    BaseModemCommand(const VendorFunctions& vendorFunctions, std::shared_ptr<CallbackType>& cb)
            : BaseCommand(vendorFunctions), mCb(cb) {}
};

struct BaseModemIndication : public BaseIndication {
    using CallbackType = android::hardware::radio::impl::RadioModemIndicationCallback;
    std::shared_ptr<CallbackType>& mCb;

    BaseModemIndication(std::shared_ptr<CallbackType>& cb) : BaseModemIndication(0, cb) {}
    BaseModemIndication(uint32_t slotId, std::shared_ptr<CallbackType>& cb)
            : BaseIndication(slotId), mCb(cb) {}
};

// New commands
/**
 * ModemActivityInfoCommand
 */
struct ModemActivityInfoCommand : public BaseModemCommand {
    using BaseModemCommand::BaseModemCommand;

    const char *TAG = "ModemActivityInfoCommand";
    void onRequest(int serial);
    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * SetRadioPowerCommand
 */
struct SetRadioPowerCommand : public BaseModemCommand {
    using BaseModemCommand::BaseModemCommand;

    const char *TAG = "SetRadioPowerCommand";
    void onRequest(int32_t serial, bool powerOn, bool forEmergencyCall,
                               bool preferredForEmergencyCall);

    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * RadioStateChangedInd
 */
struct RadioStateChangedInd : public BaseModemIndication {
    using BaseModemIndication::BaseModemIndication;

    const char *TAG = "RadioStateChangedInd";
    int onIndication(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen);
};

}
