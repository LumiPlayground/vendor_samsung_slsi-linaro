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

#include <hidl/hidlhalresponse_modem.h>
#include <hidl/1.6/hidlhalresponse_modem.h>
#include "radiomodemresponse.h"
#include <telephony/ril.h>

namespace android::hardware::radio::impl::modem {

struct BaseHidlModemCommand {
    ::android::sp<RadioModemResponseHook> mHook;
    BaseHidlModemCommand(const ::android::sp<RadioModemResponseHook>& hook) : mHook(hook) {}
};

struct EnableModemResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).enableModemResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetBasebandVersionResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).getBasebandVersionResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetDeviceIdentityResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).getDeviceIdentityResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetHardwareConfigResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).getHardwareConfigResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetModemActivityInfoResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).getModemActivityInfoResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetModemStackStatusResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).getModemStackStatusResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct NvReadItemResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).nvReadItemResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct NvResetConfigResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).nvResetConfigResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct NvWriteCdmaPrlResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).nvWriteCdmaPrlResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct NvWriteItemResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).nvWriteItemResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct RequestShutdownResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).requestShutdownResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SendDeviceStateResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).sendDeviceStateResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetRadioCapabilityResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).getRadioCapabilityResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetRadioCapabilityResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).setRadioCapabilityResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetRadioPowerResponse : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).setRadioPowerResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetRadioPowerResponse_1_5 : public BaseHidlModemCommand {
    using BaseHidlModemCommand::BaseHidlModemCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse(mHook).setRadioPowerResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct BaseHidlModemCommand_1_6 {
    ::android::sp<RadioModemResponseHook> mHook;
    BaseHidlModemCommand_1_6(const ::android::sp<RadioModemResponseHook>& hook) : mHook(hook) {}
};

struct SetRadioPowerResponse_1_6 : public BaseHidlModemCommand_1_6 {
    using BaseHidlModemCommand_1_6::BaseHidlModemCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemResponse_1_6(mHook).setRadioPowerResponse_1_6(
                        slotId, responseType, serial, e, response, responselen);
    }
};

}