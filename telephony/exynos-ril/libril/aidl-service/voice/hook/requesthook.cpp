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

#include "radiovoiceproxy.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
namespace impl = android::hardware::radio::impl;
namespace aidl_voice = ::aidl::android::hardware::radio::voice;
using ::android::sp;

void RadioVoiceProxy::acceptCall(int32_t serial) {
    mHidlRadio->acceptCall(serial);
}

void RadioVoiceProxy::cancelPendingUssd(int32_t serial) {
    mHidlRadio->cancelPendingUssd(serial);
}

void RadioVoiceProxy::conference(int32_t serial) {
    mHidlRadio->conference(serial);
}

void RadioVoiceProxy::dial(int32_t serial,
        const ::aidl::android::hardware::radio::voice::Dial& dialInfo) {
    mHidlRadio->dial(serial, compat::toHidl(dialInfo));
}

void RadioVoiceProxy::emergencyDial(int32_t serial,
        const ::aidl::android::hardware::radio::voice::Dial& dialInfo,
        int32_t categories, const std::vector<std::string>& urns,
        ::aidl::android::hardware::radio::voice::EmergencyCallRouting routing,
        bool hasKnownUserIntentEmergency, bool isTesting) {

    mHidlRadio->emergencyDial(serial,
            compat::toHidl(dialInfo),
            compat::toHidlBitfield<V1_4::EmergencyServiceCategory>(categories),
            compat::toHidl(urns),
            V1_4::EmergencyCallRouting(routing),
            hasKnownUserIntentEmergency,
            isTesting);
}

void RadioVoiceProxy::exitEmergencyCallbackMode(int32_t serial) {
    mHidlRadio->exitEmergencyCallbackMode(serial);
}

void RadioVoiceProxy::explicitCallTransfer(int32_t serial) {
    mHidlRadio->explicitCallTransfer(serial);
}

void RadioVoiceProxy::getCallForwardStatus(int32_t serial,
        const ::aidl::android::hardware::radio::voice::CallForwardInfo& callInfo) {
    mHidlRadio->getCallForwardStatus(serial, compat::toHidl(callInfo));
}

void RadioVoiceProxy::getCallWaiting(int32_t serial, int32_t serviceClass) {
    mHidlRadio->getCallWaiting(serial, serviceClass);
}

void RadioVoiceProxy::getClip(int32_t serial) {
    mHidlRadio->getClip(serial);
}

void RadioVoiceProxy::getClir(int32_t serial) {
    mHidlRadio->getClir(serial);
}

void RadioVoiceProxy::getCurrentCalls(int32_t serial) {
    mHidlRadio->getCurrentCalls_1_6(serial);
}

void RadioVoiceProxy::getLastCallFailCause(int32_t serial) {
    mHidlRadio->getLastCallFailCause(serial);
}

void RadioVoiceProxy::getMute(int32_t serial) {
    mHidlRadio->getMute(serial);
}

void RadioVoiceProxy::getPreferredVoicePrivacy(int32_t serial) {
    mHidlRadio->getPreferredVoicePrivacy(serial);
}

void RadioVoiceProxy::getTtyMode(int32_t serial) {
    mHidlRadio->getTTYMode(serial);
}

void RadioVoiceProxy::handleStkCallSetupRequestFromSim(int32_t serial, bool accept) {
    mHidlRadio->handleStkCallSetupRequestFromSim(serial, accept);
}

void RadioVoiceProxy::hangup(int32_t serial, int32_t gsmIndex) {
    mHidlRadio->hangup(serial, gsmIndex);
}

void RadioVoiceProxy::hangupForegroundResumeBackground(int32_t serial) {
    mHidlRadio->hangupForegroundResumeBackground(serial);
}

void RadioVoiceProxy::hangupWaitingOrBackground(int32_t serial) {
    mHidlRadio->hangupWaitingOrBackground(serial);
}


void RadioVoiceProxy::rejectCall(int32_t serial) {
    mHidlRadio->rejectCall(serial);
}

void RadioVoiceProxy::responseAcknowledgement() {
    mHidlRadio->responseAcknowledgement();
}

void RadioVoiceProxy::sendBurstDtmf(int32_t serial, const std::string& dtmf, int32_t on,
        int32_t off) {
    mHidlRadio->sendBurstDtmf(serial, dtmf, on, off);
}

void RadioVoiceProxy::sendCdmaFeatureCode(int32_t serial, const std::string& fcode) {
    mHidlRadio->sendCDMAFeatureCode(serial, fcode);
}

void RadioVoiceProxy::sendDtmf(int32_t serial, const std::string& s) {
    mHidlRadio->sendDtmf(serial, s);
}

void RadioVoiceProxy::sendUssd(int32_t serial, const std::string& ussd) {
    mHidlRadio->sendUssd(serial, ussd);
}

void RadioVoiceProxy::separateConnection(int32_t serial, int32_t gsmIndex) {
    mHidlRadio->separateConnection(serial, gsmIndex);
}

void RadioVoiceProxy::setCallForward(int32_t serial,
        const ::aidl::android::hardware::radio::voice::CallForwardInfo& callInfo) {
    mHidlRadio->setCallForward(serial, compat::toHidl(callInfo));
}

void RadioVoiceProxy::setCallWaiting(int32_t serial, bool enable, int32_t serviceClass) {
    mHidlRadio->setCallWaiting(serial, enable, serviceClass);
}

void RadioVoiceProxy::setClir(int32_t serial, int32_t status) {
    mHidlRadio->setClir(serial, status);
}

void RadioVoiceProxy::setMute(int32_t serial, bool enable) {
    mHidlRadio->setMute(serial, enable);
}

void RadioVoiceProxy::setPreferredVoicePrivacy(int32_t serial, bool enable) {
    mHidlRadio->setPreferredVoicePrivacy(serial, enable);
}

void RadioVoiceProxy::setTtyMode(int32_t serial,
        ::aidl::android::hardware::radio::voice::TtyMode mode) {
    mHidlRadio->setTTYMode(serial, V1_0::TtyMode(mode));
}

void RadioVoiceProxy::startDtmf(int32_t serial, const std::string& s) {
    mHidlRadio->startDtmf(serial, s);
}

void RadioVoiceProxy::stopDtmf(int32_t serial) {
    mHidlRadio->stopDtmf(serial);
}

void RadioVoiceProxy::switchWaitingOrHoldingAndActive(int32_t serial) {
    mHidlRadio->switchWaitingOrHoldingAndActive(serial);
}
