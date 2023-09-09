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

#include <android/hardware/radio/1.6/types.h>
#include <slsi/radio_v1_6.h>
#include <ril_internal.h>

#include "commandsinfo.h"
#include "commands.h"
#include "unsolresponse.h"

using namespace android::hardware::radio::impl;
using ::android::sp;

void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioVoiceResponse>& response) {
    // HIDL response hook
    auto hidlResponse = response->hidl();
    android::updateCommandInfo(slot, { RIL_REQUEST_ANSWER, voice::AcceptCallResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CANCEL_USSD,
            voice::CancelPendingUssdResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CONFERENCE, voice::ConferenceResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_DIAL, voice::DialResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_EMERGENCY_DIAL, voice::EmergencyDialResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE,
            voice::ExitEmergencyCallbackModeResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_EXPLICIT_CALL_TRANSFER,
            voice::ExplicitCallTransferResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_QUERY_CALL_FORWARD_STATUS,
            voice::GetCallForwardStatusResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_QUERY_CALL_WAITING,
            voice::GetCallWaitingResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_QUERY_CLIP, voice::GetClipResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_CLIR, voice::GetClirResponse(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_GET_CURRENT_CALLS, HAL_VERSION_CODE(1,6)),
            voice::GetCurrentCallsResponse_1_6(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_LAST_CALL_FAIL_CAUSE,
            voice::GetLastCallFailCauseResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_MUTE, voice::GetMuteResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE,
            voice::GetPreferredVoicePrivacyResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_QUERY_TTY_MODE, voice::GetTTYModeResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM,
            voice::HandleStkCallSetupRequestFromSimResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_HANGUP, voice::HangupConnectionResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,
            voice::HangupForegroundResumeBackgroundResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,
            voice::HangupWaitingOrBackgroundResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_UDUB, voice::RejectCallResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_BURST_DTMF,
            voice::SendBurstDtmfResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_FLASH,
            voice::SendCDMAFeatureCodeResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_DTMF, voice::SendDtmfResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SEND_USSD, voice::SendUssdResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SEPARATE_CONNECTION,
            voice::SeparateConnectionResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_CALL_FORWARD,
            voice::SetCallForwardResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_CALL_WAITING,
            voice::SetCallWaitingResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_CLIR, voice::SetClirResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_MUTE, voice::SetMuteResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE,
            voice::SetPreferredVoicePrivacyResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_TTY_MODE, voice::SetTTYModeResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_DTMF_START, voice::StartDtmfResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_DTMF_STOP, voice::StopDtmfResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,
            voice::SwitchWaitingOrHoldingAndActiveResponse(hidlResponse) });
}

void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioVoiceIndication>& indication) {
    // HIDL response hook
    auto hidlResponse = indication->hidl();
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_CALL_RING,
            voice::CallRingInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
            voice::CallStateChangedInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_CDMA_CALL_WAITING,
            voice::CdmaCallWaitingInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_CDMA_INFO_REC,
            voice::CdmaInfoRecInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_CDMA_OTA_PROVISION_STATUS,
            voice::CdmaOtaProvisionStatusInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_EMERGENCY_NUMBER_LIST,
            voice::CurrentEmergencyNumberListInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE,
            voice::EnterEmergencyCallbackModeInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE,
            voice::ExitEmergencyCallbackModeInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RINGBACK_TONE,
            voice::IndicateRingbackToneInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_ON_SS,
            voice::OnSupplementaryServiceIndicationInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_ON_USSD,
            voice::OnUssdInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_ON_USSD_REQUEST,
            voice::OnUssdInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESEND_INCALL_MUTE,
            voice::ResendIncallMuteInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_SRVCC_STATE_NOTIFY,
            voice::SrvccStateNotifyInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_STK_CC_ALPHA_NOTIFY,
            voice::StkCallControlAlphaNotifyInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_STK_CALL_SETUP,
            voice::StkCallSetupInd(hidlResponse), android::WAKE_PARTIAL });
}
