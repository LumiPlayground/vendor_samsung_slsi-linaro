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

#include <hidl/hidlhalindication_modem.h>

namespace android::hardware::radio::impl::modem {

struct BaseModemUnsolResponse {
    ::android::sp<RadioModemIndicationHook> mHook;
    BaseModemUnsolResponse(const ::android::sp<RadioModemIndicationHook>& hook) : mHook(hook) {}
};

struct HardwareConfigChangedInd : public BaseModemUnsolResponse {
    using BaseModemUnsolResponse::BaseModemUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemIndication(mHook).hardwareConfigChangedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct ModemResetInd : public BaseModemUnsolResponse {
    using BaseModemUnsolResponse::BaseModemUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemIndication(mHook).modemResetInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct RadioCapabilityIndicationInd : public BaseModemUnsolResponse {
    using BaseModemUnsolResponse::BaseModemUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemIndication(mHook).radioCapabilityIndicationInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct RadioStateChangedInd : public BaseModemUnsolResponse {
    using BaseModemUnsolResponse::BaseModemUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemIndication(mHook).radioStateChangedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct RilConnectedInd : public BaseModemUnsolResponse {
    using BaseModemUnsolResponse::BaseModemUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalModemIndication(mHook).rilConnectedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

}