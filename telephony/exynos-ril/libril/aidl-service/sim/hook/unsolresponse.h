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

#include <hidl/1.6/hidlhalindication_sim.h>
#include <hidl/hidlhalindication_sim.h>

namespace android::hardware::radio::impl::sim {

struct BaseSimUnsolResponse {
    ::android::sp<RadioSimIndicationHook> mHook;
    BaseSimUnsolResponse(const ::android::sp<RadioSimIndicationHook>& hook)
            : mHook(hook) {}
};

struct carrierInfoForImsiEncryption : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication(mHook).carrierInfoForImsiEncryption(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct cdmaSubscriptionSourceChanged : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication(mHook).cdmaSubscriptionSourceChanged(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct simRefresh : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication(mHook).simRefresh(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct simStatusChanged : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication(mHook).simStatusChanged(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct stkEventNotify : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication(mHook).stkEventNotify(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct stkProactiveCommand : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication(mHook).stkProactiveCommand(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct stkSessionEnd : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication(mHook).stkSessionEnd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct subscriptionStatusChanged : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication(mHook).subscriptionStatusChanged(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct uiccApplicationsEnablementChanged : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication(mHook).uiccApplicationsEnablementChanged(
                        slotId, indicationType, token, e, response, responselen);
    }
};

//Ver 1.6
struct simPhonebookChanged : public BaseSimUnsolResponse {
        using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication_1_6(mHook).simPhonebookChanged(
                   slotId, indicationType, token, e, response, responselen);
    }
};

struct simPhonebookRecordsReceived : public BaseSimUnsolResponse {
    using BaseSimUnsolResponse::BaseSimUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen) {
        return HidlHalSimIndication_1_6(mHook).simPhonebookRecordsReceived(
                        slotId, indicationType, token, e, response, responselen);
    }
};


}
