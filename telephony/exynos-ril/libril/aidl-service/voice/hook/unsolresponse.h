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

#include <hidl/hidlhalindication_voice.h>

namespace android::hardware::radio::impl::voice {

struct BaseVoiceUnsolResponse {
    ::android::sp<RadioVoiceIndicationHook> mHook;
    BaseVoiceUnsolResponse(const ::android::sp<RadioVoiceIndicationHook>& hook) : mHook(hook) {}
};

struct CallRingInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).callRingInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct CallStateChangedInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).callStateChangedInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct CdmaCallWaitingInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).cdmaCallWaitingInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct CdmaInfoRecInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).cdmaInfoRecInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct CdmaOtaProvisionStatusInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).cdmaOtaProvisionStatusInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct CurrentEmergencyNumberListInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).currentEmergencyNumberListInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct EnterEmergencyCallbackModeInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).enterEmergencyCallbackModeInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct ExitEmergencyCallbackModeInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).exitEmergencyCallbackModeInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct IndicateRingbackToneInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).indicateRingbackToneInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct OnSupplementaryServiceIndicationInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).onSupplementaryServiceIndicationInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct OnUssdInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).onUssdInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct ResendIncallMuteInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).resendIncallMuteInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct SrvccStateNotifyInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).srvccStateNotifyInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct StkCallControlAlphaNotifyInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).stkCallControlAlphaNotifyInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

struct StkCallSetupInd : public BaseVoiceUnsolResponse {
    using BaseVoiceUnsolResponse::BaseVoiceUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceIndication(mHook).stkCallSetupInd(
                slotId, indicationType, token, e, response, responselen);
    }
};

}
