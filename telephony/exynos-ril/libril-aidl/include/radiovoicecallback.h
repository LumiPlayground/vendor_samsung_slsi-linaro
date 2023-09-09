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

#include <aidl/android/hardware/radio/voice/BnRadioVoice.h>

namespace android::hardware::radio::impl {

struct RadioVoiceResponseCallback;
struct RadioVoiceIndicationCallback;

struct RadioVoiceCallback {
    virtual ~RadioVoiceCallback() = default;
    virtual void setResponseFunctions(
                const std::shared_ptr<RadioVoiceResponseCallback>& responseCb,
                const std::shared_ptr<RadioVoiceIndicationCallback>& indicationCb) = 0;
    virtual void acceptCall(int32_t serial) = 0;
    virtual void cancelPendingUssd(int32_t serial) = 0;
    virtual void conference(int32_t serial) = 0;
    virtual void dial(int32_t serial,
                const ::aidl::android::hardware::radio::voice::Dial& dialInfo) = 0;
    virtual void emergencyDial(int32_t serial,
                const ::aidl::android::hardware::radio::voice::Dial& dialInfo,
                int32_t categories,
                const std::vector<std::string>& urns,
                ::aidl::android::hardware::radio::voice::EmergencyCallRouting routing,
                bool hasKnownUserIntentEmergency, bool isTesting) = 0;
    virtual void exitEmergencyCallbackMode(int32_t serial) = 0;
    virtual void explicitCallTransfer(int32_t serial) = 0;
    virtual void getCallForwardStatus(int32_t serial,
                const ::aidl::android::hardware::radio::voice::CallForwardInfo& callInfo) = 0;
    virtual void getCallWaiting(int32_t serial, int32_t serviceClass) = 0;
    virtual void getClip(int32_t serial) = 0;
    virtual void getClir(int32_t serial) = 0;
    virtual void getCurrentCalls(int32_t serial) = 0;
    virtual void getLastCallFailCause(int32_t serial) = 0;
    virtual void getMute(int32_t serial) = 0;
    virtual void getPreferredVoicePrivacy(int32_t serial) = 0;
    virtual void getTtyMode(int32_t serial) = 0;
    virtual void handleStkCallSetupRequestFromSim(int32_t serial, bool accept) = 0;
    virtual void hangup(int32_t serial, int32_t gsmIndex) = 0;
    virtual void hangupForegroundResumeBackground(int32_t serial) = 0;
    virtual void hangupWaitingOrBackground(int32_t serial) = 0;
    virtual void isVoNrEnabled(int32_t serial) = 0;
    virtual void rejectCall(int32_t serial) = 0;
    virtual void responseAcknowledgement() = 0;
    virtual void sendBurstDtmf(int32_t serial, const std::string& dtmf, int32_t on,
                                       int32_t off) = 0;
    virtual void sendCdmaFeatureCode(int32_t serial, const std::string& fcode) = 0;
    virtual void sendDtmf(int32_t serial, const std::string& s) = 0;
    virtual void sendUssd(int32_t serial, const std::string& ussd) = 0;
    virtual void separateConnection(int32_t serial, int32_t gsmIndex) = 0;
    virtual void setCallForward(int32_t serial,
                const ::aidl::android::hardware::radio::voice::CallForwardInfo& callInfo) = 0;
    virtual void setCallWaiting(int32_t serial, bool enable, int32_t serviceClass) = 0;
    virtual void setClir(int32_t serial, int32_t status) = 0;
    virtual void setMute(int32_t serial, bool enable) = 0;
    virtual void setPreferredVoicePrivacy(int32_t serial, bool enable) = 0;
    virtual void setTtyMode(int32_t serial,
                                    ::aidl::android::hardware::radio::voice::TtyMode mode) = 0;
    virtual void setVoNrEnabled(int32_t serial, bool enable) = 0;
    virtual void startDtmf(int32_t serial, const std::string& s) = 0;
    virtual void stopDtmf(int32_t serial) = 0;
    virtual void switchWaitingOrHoldingAndActive(int32_t serial) = 0;
};

struct RadioVoiceResponseCallback {
    virtual ~RadioVoiceResponseCallback() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceResponse>&
            radioVoiceResponse)=0;
    virtual void acceptCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void acknowledgeRequest(int32_t serial) = 0;
    virtual void cancelPendingUssdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void conferenceResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void dialResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void emergencyDialResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void exitEmergencyCallbackModeResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void explicitCallTransferResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)
        = 0;
    virtual void getCallForwardStatusResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::vector<::aidl::android::hardware::radio::voice::CallForwardInfo>& callForwardInfos) = 0;
    virtual void getCallWaitingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            bool enable, int32_t serviceClass) = 0;
    virtual void getClipResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            ::aidl::android::hardware::radio::voice::ClipStatus status) = 0;
    virtual void getClirResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info, int32_t n,
            int32_t m) = 0;
    virtual void getCurrentCallsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::vector<::aidl::android::hardware::radio::voice::Call>& calls) = 0;
    virtual void getLastCallFailCauseResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::voice::LastCallFailCauseInfo& failCauseinfo) = 0;
    virtual void getMuteResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enable)
        = 0;
    virtual void getPreferredVoicePrivacyResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enable) = 0;
    virtual void getTtyModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            ::aidl::android::hardware::radio::voice::TtyMode mode) = 0;
    virtual void handleStkCallSetupRequestFromSimResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void hangupConnectionResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void hangupForegroundResumeBackgroundResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void hangupWaitingOrBackgroundResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void isVoNrEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            bool enable) = 0;
    virtual void rejectCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void sendBurstDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void sendCdmaFeatureCodeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)
        = 0;
    virtual void sendDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void sendUssdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void separateConnectionResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setCallForwardResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setCallWaitingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setClirResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setMuteResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setPreferredVoicePrivacyResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setTtyModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setVoNrEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void startDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void stopDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void switchWaitingOrHoldingAndActiveResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
};

struct RadioVoiceIndicationCallback {
    virtual ~RadioVoiceIndicationCallback() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceIndication>&
            radioVoiceIndication)=0;
    virtual void callRing(::aidl::android::hardware::radio::RadioIndicationType type, bool isGsm,
            const ::aidl::android::hardware::radio::voice::CdmaSignalInfoRecord& record) = 0;
    virtual void callStateChanged(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void cdmaCallWaiting(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::voice::CdmaCallWaiting& callWaitingRecord) = 0;
    virtual void cdmaInfoRec(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::vector<::aidl::android::hardware::radio::voice::CdmaInformationRecord>& records) = 0;
    virtual void cdmaOtaProvisionStatus(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::voice::CdmaOtaProvisionStatus status) = 0;
    virtual void currentEmergencyNumberList(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::vector<::aidl::android::hardware::radio::voice::EmergencyNumber>& emergencyNumberList)
        = 0;
    virtual void enterEmergencyCallbackMode(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void exitEmergencyCallbackMode(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void indicateRingbackTone(::aidl::android::hardware::radio::RadioIndicationType type, bool start)
        = 0;
    virtual void onSupplementaryServiceIndication(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::voice::StkCcUnsolSsResult& ss) = 0;
    virtual void onUssd(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::voice::UssdModeType modeType, const std::string& msg) = 0;
    virtual void resendIncallMute(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void srvccStateNotify(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::voice::SrvccState state) = 0;
    virtual void stkCallControlAlphaNotify(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::string& alpha) = 0;
    virtual void stkCallSetup(::aidl::android::hardware::radio::RadioIndicationType type, int64_t timeout) = 0;
};

}  // namespace android::hardware::radio::impl {
