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
#include <aidl-log.h>
#include "radiovoice.h"
#include "radiovoiceindication.h"
#include "radiovoiceresponse.h"

#define TAG "RadioVoice"

namespace android::hardware::radio::impl {

using namespace std::string_literals;
using ::ndk::ScopedAStatus;
namespace aidl = ::aidl::android::hardware::radio::voice;
constexpr auto ok = &ScopedAStatus::ok;

RadioVoice::RadioVoice(std::shared_ptr<RadioVoiceCallback> radio, uint32_t slot)
    : mRadio(radio), mSlot(slot) {
    mCallbacks = {
        mRadio,
        std::make_shared<RadioVoiceResponse>(),
        std::make_shared<RadioVoiceIndication>()
    };
}


ScopedAStatus RadioVoice::setResponseFunctions(
        const std::shared_ptr<aidl::IRadioVoiceResponse>& voiceResponse,
        const std::shared_ptr<aidl::IRadioVoiceIndication>& voiceIndication) {
#if VDBG
    RLOGD("%s::%s voiceResponse=%p voiceIndication=%p", TAG, __FUNCTION__,
            voiceResponse.get(), voiceIndication.get());
#endif
    mCallbacks.setResponseFunctions(voiceResponse, voiceIndication);
    return ok();
}

ScopedAStatus RadioVoice::acceptCall(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->acceptCall(serial);
    return ok();
}

ScopedAStatus RadioVoice::cancelPendingUssd(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->cancelPendingUssd(serial);
    return ok();
}

ScopedAStatus RadioVoice::conference(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->conference(serial);
    return ok();
}

ScopedAStatus RadioVoice::dial(int32_t serial, const aidl::Dial& dialInfo) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->dial(serial, dialInfo);
    return ok();
}

ScopedAStatus RadioVoice::emergencyDial(int32_t serial,
        const aidl::Dial& dialInfo, int32_t categories,
        const std::vector<std::string>& urns,
        aidl::EmergencyCallRouting routing,
        bool hasKnownUserIntentEmergency, bool isTesting) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->emergencyDial(serial, dialInfo, categories, urns, routing, hasKnownUserIntentEmergency, isTesting);
    return ok();
}

ScopedAStatus RadioVoice::exitEmergencyCallbackMode(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->exitEmergencyCallbackMode(serial);
    return ok();
}

ScopedAStatus RadioVoice::explicitCallTransfer(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->explicitCallTransfer(serial);
    return ok();
}

ScopedAStatus RadioVoice::getCallForwardStatus(int32_t serial,
        const aidl::CallForwardInfo& callInfo) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getCallForwardStatus(serial, callInfo);
    return ok();
}

ScopedAStatus RadioVoice::getCallWaiting(int32_t serial, int32_t serviceClass) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getCallWaiting(serial, serviceClass);
    return ok();
}

ScopedAStatus RadioVoice::getClip(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getClip(serial);
    return ok();
}

ScopedAStatus RadioVoice::getClir(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getClir(serial);
    return ok();
}

ScopedAStatus RadioVoice::getCurrentCalls(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getCurrentCalls(serial);
    return ok();
}

ScopedAStatus RadioVoice::getLastCallFailCause(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getLastCallFailCause(serial);
    return ok();
}

ScopedAStatus RadioVoice::getMute(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getMute(serial);
    return ok();
}

ScopedAStatus RadioVoice::getPreferredVoicePrivacy(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getPreferredVoicePrivacy(serial);
    return ok();
}

ScopedAStatus RadioVoice::getTtyMode(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getTtyMode(serial);
    return ok();
}

ScopedAStatus RadioVoice::handleStkCallSetupRequestFromSim(int32_t serial, bool accept) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->handleStkCallSetupRequestFromSim(serial, accept);
    return ok();
}

ScopedAStatus RadioVoice::hangup(int32_t serial, int32_t gsmIndex) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->hangup(serial, gsmIndex);
    return ok();
}

ScopedAStatus RadioVoice::hangupForegroundResumeBackground(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->hangupForegroundResumeBackground(serial);
    return ok();
}

ScopedAStatus RadioVoice::hangupWaitingOrBackground(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->hangupWaitingOrBackground(serial);
    return ok();
}

ScopedAStatus RadioVoice::isVoNrEnabled(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->isVoNrEnabled(serial);
    return ok();
}

ScopedAStatus RadioVoice::rejectCall(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->rejectCall(serial);
    return ok();
}

ScopedAStatus RadioVoice::responseAcknowledgement() {
    return ok();
}

ScopedAStatus RadioVoice::sendBurstDtmf(int32_t serial, const std::string& dtmf, int32_t on, int32_t off) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendBurstDtmf(serial, dtmf, on, off);
    return ok();
}

ScopedAStatus RadioVoice::sendCdmaFeatureCode(int32_t serial, const std::string& fcode) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendCdmaFeatureCode(serial, fcode);
    return ok();
}

ScopedAStatus RadioVoice::sendDtmf(int32_t serial, const std::string& s) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendDtmf(serial, s);
    return ok();
}

ScopedAStatus RadioVoice::sendUssd(int32_t serial, const std::string& ussd) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendUssd(serial, ussd);
    return ok();
}

ScopedAStatus RadioVoice::separateConnection(int32_t serial, int32_t gsmIndex) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->separateConnection(serial, gsmIndex);
    return ok();
}

ScopedAStatus RadioVoice::setCallForward(int32_t serial,
        const aidl::CallForwardInfo& callInfo) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setCallForward(serial, callInfo);
    return ok();
}

ScopedAStatus RadioVoice::setCallWaiting(int32_t serial, bool enable, int32_t serviceClass) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setCallWaiting(serial, enable, serviceClass);
    return ok();
}

ScopedAStatus RadioVoice::setClir(int32_t serial, int32_t status) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setClir(serial, status);
    return ok();
}

ScopedAStatus RadioVoice::setMute(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setMute(serial, enable);
    return ok();
}

ScopedAStatus RadioVoice::setPreferredVoicePrivacy(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setPreferredVoicePrivacy(serial, enable);
    return ok();
}

ScopedAStatus RadioVoice::setTtyMode(int32_t serial, aidl::TtyMode mode) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setTtyMode(serial, mode);
    return ok();
}

ScopedAStatus RadioVoice::setVoNrEnabled(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setVoNrEnabled(serial, enable);
    return ok();
}

ScopedAStatus RadioVoice::startDtmf(int32_t serial, const std::string& s) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->startDtmf(serial, s);
    return ok();
}

ScopedAStatus RadioVoice::stopDtmf(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->stopDtmf(serial);
    return ok();
}

ScopedAStatus RadioVoice::switchWaitingOrHoldingAndActive(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->switchWaitingOrHoldingAndActive(serial);
    return ok();
}

std::shared_ptr<RadioVoice> RadioVoice::make(std::shared_ptr<RadioVoiceCallback> radio, uint32_t slot) {
#if VDBG
    RLOGD("%s::%s slot %u", TAG, __FUNCTION__, slot);
#endif
    return ndk::SharedRefBase::make<RadioVoice>(radio, slot);
}

}
