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

#include <hidl/config/hidlhalresponse_config.h>
#include "radioconfigresponse.h"
#include <telephony/ril.h>

namespace android::hardware::radio::impl::config {

struct BaseHidlConfigCommand {
    ::android::sp<RadioConfigResponseHook> mHook;
    BaseHidlConfigCommand(const ::android::sp<RadioConfigResponseHook>& hook) : mHook(hook) {}
};

struct GetModemsConfigResponse : public BaseHidlConfigCommand {
    using BaseHidlConfigCommand::BaseHidlConfigCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalRadioConfigResponse(mHook).getModemsConfigResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetPhoneCapabilityResponse : public BaseHidlConfigCommand {
    using BaseHidlConfigCommand::BaseHidlConfigCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalRadioConfigResponse(mHook).getPhoneCapabilityResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetModemsConfigResponse : public BaseHidlConfigCommand {
    using BaseHidlConfigCommand::BaseHidlConfigCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalRadioConfigResponse(mHook).setModemsConfigResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetPreferredDataModemResponse : public BaseHidlConfigCommand {
    using BaseHidlConfigCommand::BaseHidlConfigCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalRadioConfigResponse(mHook).setPreferredDataModemResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

}