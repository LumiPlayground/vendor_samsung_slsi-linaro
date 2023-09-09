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

#include <hidl/1.6/hidlhalresponse_data.h>
#include <hidl/hidlhalresponse_data.h>
#include "radiodataresponse.h"
#include <telephony/ril.h>

namespace android::hardware::radio::impl::data {

struct BaseHidlDataCommand {
    ::android::sp<RadioDataResponseHook> mHook;
    BaseHidlDataCommand(const ::android::sp<RadioDataResponseHook> &hook)
        : mHook(hook) {}
};

struct setupDataCallResponse_1_5 : public BaseHidlDataCommand {
    using BaseHidlDataCommand::BaseHidlDataCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse(mHook).setupDataCallResponse_1_5(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct deactivateDataCallResponse : public BaseHidlDataCommand {
    using BaseHidlDataCommand::BaseHidlDataCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse(mHook).deactivateDataCallResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct setDataAllowedResponse : public BaseHidlDataCommand {
    using BaseHidlDataCommand::BaseHidlDataCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse(mHook).setDataAllowedResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct setInitialAttachApnResponse_1_5 : public BaseHidlDataCommand {
    using BaseHidlDataCommand::BaseHidlDataCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse(mHook).setInitialAttachApnResponse_1_5(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct startKeepaliveResponse : public BaseHidlDataCommand {
    using BaseHidlDataCommand::BaseHidlDataCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse(mHook).startKeepaliveResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct stopKeepaliveResponse : public BaseHidlDataCommand {
    using BaseHidlDataCommand::BaseHidlDataCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse(mHook).stopKeepaliveResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct setDataProfileResponse_1_5 : public BaseHidlDataCommand {
    using BaseHidlDataCommand::BaseHidlDataCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse(mHook).setDataProfileResponse_1_5(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct BaseDataCommand_1_6 {
    ::android::sp<RadioDataResponseHook> mHook;
    BaseDataCommand_1_6(const ::android::sp<RadioDataResponseHook> &hook)
        : mHook(hook) {}
};

struct allocatePduSessionIdResponse : public BaseDataCommand_1_6 {
    using BaseDataCommand_1_6::BaseDataCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse_1_6(mHook).allocatePduSessionIdResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct releasePduSessionIdResponse: public BaseDataCommand_1_6 {
    using BaseDataCommand_1_6::BaseDataCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse_1_6(mHook).releasePduSessionIdResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct startHandoverResponse : public BaseDataCommand_1_6 {
    using BaseDataCommand_1_6::BaseDataCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse_1_6(mHook).startHandoverResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct cancelHandoverResponse : public BaseDataCommand_1_6 {
    using BaseDataCommand_1_6::BaseDataCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse_1_6(mHook).cancelHandoverResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct getDataCallListResponse_1_6 : public BaseDataCommand_1_6 {
    using BaseDataCommand_1_6::BaseDataCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse_1_6(mHook).getDataCallListResponse_1_6(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct getSlicingConfigResponse_1_6 : public BaseDataCommand_1_6 {
    using BaseDataCommand_1_6::BaseDataCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse_1_6(mHook).getSlicingConfigResponse_1_6(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct setDataThrottlingResponse : public BaseDataCommand_1_6 {
    using BaseDataCommand_1_6::BaseDataCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse_1_6(mHook).setDataThrottlingResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct setupDataCallResponse_1_6 : public BaseDataCommand_1_6 {
    using BaseDataCommand_1_6::BaseDataCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalDataResponse_1_6(mHook).setupDataCallResponse_1_6(
            slotId, responseType, serial, e, response, responselen);
    }
};

}  // namespace android::hardware::radio::impl::data
