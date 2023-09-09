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
 *
 */

#pragma once

#include <hidl/hidlhalresponse_voice.h>
#include <hidl/1.6/hidlhalresponse_voice.h>
#include "radiovoiceresponse.h"
#include <telephony/ril.h>

namespace android::hardware::radio::impl::voice {

struct BaseHidlVoiceCommand {
    ::android::sp<RadioVoiceResponseHook> mHook;
    BaseHidlVoiceCommand(const ::android::sp<RadioVoiceResponseHook>& hook) : mHook(hook) {}
};

struct AcceptCallResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).acceptCallResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct CancelPendingUssdResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).cancelPendingUssdResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct ConferenceResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).conferenceResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct DialResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).dialResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct EmergencyDialResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).emergencyDialResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct ExitEmergencyCallbackModeResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).exitEmergencyCallbackModeResponse(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct ExplicitCallTransferResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).explicitCallTransferResponse(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct GetCallForwardStatusResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).getCallForwardStatusResponse(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct GetCallWaitingResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).getCallWaitingResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct GetClipResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).getClipResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct GetClirResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).getClirResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct BaseHidlVoiceCommand_1_6 {
    ::android::sp<RadioVoiceResponseHook> mHook;
    BaseHidlVoiceCommand_1_6(const ::android::sp<RadioVoiceResponseHook>& hook) : mHook(hook) {}
};

struct GetCurrentCallsResponse_1_6 : public BaseHidlVoiceCommand_1_6 {
    using BaseHidlVoiceCommand_1_6::BaseHidlVoiceCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse_1_6(mHook).getCurrentCallsResponse_1_6(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct GetLastCallFailCauseResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).getLastCallFailCauseResponse(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct GetMuteResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).getMuteResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct GetPreferredVoicePrivacyResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).getPreferredVoicePrivacyResponse(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct GetTTYModeResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).getTTYModeResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct HandleStkCallSetupRequestFromSimResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).handleStkCallSetupRequestFromSimResponse(slotId, responseType,
                serial, e, response, responselen);
    }
};

struct HangupConnectionResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).hangupConnectionResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct HangupForegroundResumeBackgroundResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).hangupForegroundResumeBackgroundResponse(slotId, responseType,
                serial, e, response, responselen);
    }
};

struct HangupWaitingOrBackgroundResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).hangupWaitingOrBackgroundResponse(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct RejectCallResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).rejectCallResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct SendBurstDtmfResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).sendBurstDtmfResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct SendCDMAFeatureCodeResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).sendCDMAFeatureCodeResponse(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct SendDtmfResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).sendDtmfResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct SendUssdResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).sendUssdResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct SeparateConnectionResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).separateConnectionResponse(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct SetCallForwardResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).setCallForwardResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct SetCallWaitingResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).setCallWaitingResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct SetClirResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).setClirResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct SetMuteResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).setMuteResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct SetPreferredVoicePrivacyResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).setPreferredVoicePrivacyResponse(slotId, responseType, serial, e,
                response, responselen);
    }
};

struct SetTTYModeResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).setTTYModeResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct StartDtmfResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).startDtmfResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct StopDtmfResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).stopDtmfResponse(slotId, responseType, serial, e, response,
                responselen);
    }
};

struct SwitchWaitingOrHoldingAndActiveResponse : public BaseHidlVoiceCommand {
    using BaseHidlVoiceCommand::BaseHidlVoiceCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen) {
        return HidlHalVoiceResponse(mHook).switchWaitingOrHoldingAndActiveResponse(slotId, responseType,
                serial, e, response, responselen);
    }
};

}
