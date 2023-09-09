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

#include <hidl/hidlhalindication_messaging.h>

namespace android::hardware::radio::impl::messaging {

struct BaseMessagingUnsolResponse {
    ::android::sp<RadioMessagingIndicationHook> mHook;
    BaseMessagingUnsolResponse(const ::android::sp<RadioMessagingIndicationHook> &hook)
        : mHook(hook) {}
};

struct CdmaNewSmsInd : public BaseMessagingUnsolResponse {
    using BaseMessagingUnsolResponse::BaseMessagingUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingIndication(mHook).cdmaNewSmsInd(
            slotId, indicationType, token, e, response, responselen);
    }
};

struct CdmaRuimSmsStorageFullInd : public BaseMessagingUnsolResponse {
    using BaseMessagingUnsolResponse::BaseMessagingUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingIndication(mHook).cdmaRuimSmsStorageFullInd(
            slotId, indicationType, token, e, response, responselen);
    }
};

struct NewBroadcastSmsInd : public BaseMessagingUnsolResponse {
    using BaseMessagingUnsolResponse::BaseMessagingUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingIndication(mHook).newBroadcastSmsInd(
            slotId, indicationType, token, e, response, responselen);
    }
};

struct NewSmsInd : public BaseMessagingUnsolResponse {
    using BaseMessagingUnsolResponse::BaseMessagingUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingIndication(mHook).newSmsInd(
            slotId, indicationType, token, e, response, responselen);
    }
};

struct NewSmsOnSimInd : public BaseMessagingUnsolResponse {
    using BaseMessagingUnsolResponse::BaseMessagingUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingIndication(mHook).newSmsOnSimInd(
            slotId, indicationType, token, e, response, responselen);
    }
};

struct NewSmsStatusReportInd : public BaseMessagingUnsolResponse {
    using BaseMessagingUnsolResponse::BaseMessagingUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingIndication(mHook).newSmsStatusReportInd(
            slotId, indicationType, token, e, response, responselen);
    }
};

struct SimSmsStorageFullInd : public BaseMessagingUnsolResponse {
    using BaseMessagingUnsolResponse::BaseMessagingUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingIndication(mHook).simSmsStorageFullInd(
            slotId, indicationType, token, e, response, responselen);
    }
};

} // namespace android::hardware::radio::impl::messaging
