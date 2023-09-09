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
#include <radiocallbackmanager.h>
#include <radiovoicecallback.h>

namespace android::hardware::radio::impl {
struct RadioVoice : public aidl::android::hardware::radio::voice::BnRadioVoice {
    std::shared_ptr<RadioVoiceCallback> mRadio;
    uint32_t mSlot;
    RadioCallbackManager<RadioVoiceCallback, RadioVoiceResponseCallback,
                RadioVoiceIndicationCallback> mCallbacks;

    RadioVoice(std::shared_ptr<RadioVoiceCallback> radio, uint32_t slot);

    ::ndk::ScopedAStatus acceptCall(int32_t serial) override;
    ::ndk::ScopedAStatus cancelPendingUssd(int32_t serial) override;
    ::ndk::ScopedAStatus conference(int32_t serial) override;
    ::ndk::ScopedAStatus dial(
            int32_t serial, const ::aidl::android::hardware::radio::voice::Dial& dialInfo) override;
    ::ndk::ScopedAStatus emergencyDial(
            int32_t serial, const ::aidl::android::hardware::radio::voice::Dial& dialInfo,
            int32_t categories, const std::vector<std::string>& urns,
            ::aidl::android::hardware::radio::voice::EmergencyCallRouting routing,
            bool hasKnownUserIntentEmergency, bool isTesting) override;
    ::ndk::ScopedAStatus exitEmergencyCallbackMode(int32_t serial) override;
    ::ndk::ScopedAStatus explicitCallTransfer(int32_t serial) override;
    ::ndk::ScopedAStatus getCallForwardStatus(
            int32_t serial,
            const ::aidl::android::hardware::radio::voice::CallForwardInfo& callInfo) override;
    ::ndk::ScopedAStatus getCallWaiting(int32_t serial, int32_t serviceClass) override;
    ::ndk::ScopedAStatus getClip(int32_t serial) override;
    ::ndk::ScopedAStatus getClir(int32_t serial) override;
    ::ndk::ScopedAStatus getCurrentCalls(int32_t serial) override;
    ::ndk::ScopedAStatus getLastCallFailCause(int32_t serial) override;
    ::ndk::ScopedAStatus getMute(int32_t serial) override;
    ::ndk::ScopedAStatus getPreferredVoicePrivacy(int32_t serial) override;
    ::ndk::ScopedAStatus getTtyMode(int32_t serial) override;
    ::ndk::ScopedAStatus handleStkCallSetupRequestFromSim(int32_t serial, bool accept) override;
    ::ndk::ScopedAStatus hangup(int32_t serial, int32_t gsmIndex) override;
    ::ndk::ScopedAStatus hangupForegroundResumeBackground(int32_t serial) override;
    ::ndk::ScopedAStatus hangupWaitingOrBackground(int32_t serial) override;
    ::ndk::ScopedAStatus isVoNrEnabled(int32_t serial) override;
    ::ndk::ScopedAStatus rejectCall(int32_t serial) override;
    ::ndk::ScopedAStatus responseAcknowledgement() override;
    ::ndk::ScopedAStatus sendBurstDtmf(int32_t serial, const std::string& dtmf, int32_t on,
                                       int32_t off) override;
    ::ndk::ScopedAStatus sendCdmaFeatureCode(int32_t serial, const std::string& fcode) override;
    ::ndk::ScopedAStatus sendDtmf(int32_t serial, const std::string& s) override;
    ::ndk::ScopedAStatus sendUssd(int32_t serial, const std::string& ussd) override;
    ::ndk::ScopedAStatus separateConnection(int32_t serial, int32_t gsmIndex) override;
    ::ndk::ScopedAStatus setCallForward(
            int32_t serial,
            const ::aidl::android::hardware::radio::voice::CallForwardInfo& callInfo) override;
    ::ndk::ScopedAStatus setCallWaiting(int32_t serial, bool enable, int32_t serviceClass) override;
    ::ndk::ScopedAStatus setClir(int32_t serial, int32_t status) override;
    ::ndk::ScopedAStatus setMute(int32_t serial, bool enable) override;
    ::ndk::ScopedAStatus setPreferredVoicePrivacy(int32_t serial, bool enable) override;
    ::ndk::ScopedAStatus setResponseFunctions(
            const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceResponse>&
                    radioVoiceResponse,
            const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceIndication>&
                    radioVoiceIndication) override;
    ::ndk::ScopedAStatus setTtyMode(int32_t serial,
                                    ::aidl::android::hardware::radio::voice::TtyMode mode) override;
    ::ndk::ScopedAStatus setVoNrEnabled(int32_t serial, bool enable) override;
    ::ndk::ScopedAStatus startDtmf(int32_t serial, const std::string& s) override;
    ::ndk::ScopedAStatus stopDtmf(int32_t serial) override;
    ::ndk::ScopedAStatus switchWaitingOrHoldingAndActive(int32_t serial) override;

    static std::shared_ptr<RadioVoice> make(std::shared_ptr<RadioVoiceCallback> radio,
            uint32_t slot);
};

}
