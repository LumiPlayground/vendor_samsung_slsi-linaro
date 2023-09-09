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

#include <radiovoicecallback.h>
#include <radiocallbackadapter.h>
#include <radiointerface.h>

struct RadioVoiceProxy : public RadioInterface,
    public android::hardware::radio::impl::RadioVoiceCallbackAdapter {
        using RadioInterface::RadioInterface;

        std::shared_ptr<android::hardware::radio::impl::RadioVoiceResponseCallback> mResponseCb;
        std::shared_ptr<android::hardware::radio::impl::RadioVoiceIndicationCallback> mIndicationCb;

        template<typename T, typename... Args>
        void onRequest(int request, Args&&... args) {
            auto command = getCommand<T>(request);
            if (command == nullptr) {
                command = std::make_shared<T>(mSlot, mResponseCb);
                RadioInterface::updateCommand(request, command);
            }
            RadioInterface::onRequest(command, args...);
        }

        template<typename T>
        void listen(int indication) {
            RadioInterface::updateIndication(indication, std::make_shared<T>(mSlot, mIndicationCb));
        }

        void setResponseFunctions(
                const std::shared_ptr<android::hardware::radio::impl::RadioVoiceResponseCallback>& responseCb,
                const std::shared_ptr<android::hardware::radio::impl::RadioVoiceIndicationCallback>&
                indicationCb);

        void acceptCall(int32_t serial) override;
        void cancelPendingUssd(int32_t serial) override;
        void conference(int32_t serial) override;
        void dial(int32_t serial, const ::aidl::android::hardware::radio::voice::Dial& dialInfo) override;
        void emergencyDial(int32_t serial, const ::aidl::android::hardware::radio::voice::Dial& dialInfo,
                int32_t categories, const std::vector<std::string>& urns,
                ::aidl::android::hardware::radio::voice::EmergencyCallRouting routing,
                bool hasKnownUserIntentEmergency, bool isTesting) override;
        void exitEmergencyCallbackMode(int32_t serial) override;
        void explicitCallTransfer(int32_t serial) override;
        void getCallForwardStatus(int32_t serial,
                const ::aidl::android::hardware::radio::voice::CallForwardInfo& callInfo) override;
        void getCallWaiting(int32_t serial, int32_t serviceClass) override;
        void getClip(int32_t serial) override;
        void getClir(int32_t serial) override;
        void getCurrentCalls(int32_t serial) override;
        void getLastCallFailCause(int32_t serial) override;
        void getMute(int32_t serial) override;
        void getPreferredVoicePrivacy(int32_t serial) override;
        void getTtyMode(int32_t serial) override;
        void handleStkCallSetupRequestFromSim(int32_t serial, bool accept) override;
        void hangup(int32_t serial, int32_t gsmIndex) override;
        void hangupForegroundResumeBackground(int32_t serial) override;
        void hangupWaitingOrBackground(int32_t serial) override;
        void isVoNrEnabled(int32_t serial) override;
        void rejectCall(int32_t serial) override;
        void responseAcknowledgement() override;
        void sendBurstDtmf(int32_t serial, const std::string& dtmf, int32_t on, int32_t off) override;
        void sendCdmaFeatureCode(int32_t serial, const std::string& fcode) override;
        void sendDtmf(int32_t serial, const std::string& s) override;
        void sendUssd(int32_t serial, const std::string& ussd) override;
        void separateConnection(int32_t serial, int32_t gsmIndex) override;
        void setCallForward(int32_t serial,
                const ::aidl::android::hardware::radio::voice::CallForwardInfo& callInfo) override;
        void setCallWaiting(int32_t serial, bool enable, int32_t serviceClass) override;
        void setClir(int32_t serial, int32_t status) override;
        void setMute(int32_t serial, bool enable) override;
        void setPreferredVoicePrivacy(int32_t serial, bool enable) override;
        void setTtyMode(int32_t serial, ::aidl::android::hardware::radio::voice::TtyMode mode) override;
        void setVoNrEnabled(int32_t serial, bool enable) override;
        void startDtmf(int32_t serial, const std::string& s) override;
        void stopDtmf(int32_t serial) override;
        void switchWaitingOrHoldingAndActive(int32_t serial) override;

        static auto make(uint32_t slot, const std::shared_ptr<RadioProxy>& radio) {
            return std::make_shared<RadioVoiceProxy>(slot, radio);
        }
    };
