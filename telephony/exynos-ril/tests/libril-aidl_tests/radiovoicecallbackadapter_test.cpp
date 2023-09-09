/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <testutils.h>
#include <radiocallbackadapter.h>

using namespace android::hardware::radio::impl;
using ::aidl::android::hardware::radio::RadioIndicationType;
using ::aidl::android::hardware::radio::voice::CdmaOtaProvisionStatus;
using ::aidl::android::hardware::radio::voice::ClipStatus;
using ::aidl::android::hardware::radio::voice::EmergencyCallRouting;
using ::aidl::android::hardware::radio::voice::SrvccState;
using ::aidl::android::hardware::radio::voice::TtyMode;
using ::aidl::android::hardware::radio::voice::UssdModeType;

TEST(RadioCallbackAdapter, RadioVoiceCallbackAdapter) {
    TestChecker checker;
    auto radio = std::make_shared<RadioVoiceCallbackAdapter>();
    radio->setResponseFunctions(nullptr, nullptr);
    radio->acceptCall(0);
    radio->cancelPendingUssd(0);
    radio->conference(0);
    radio->dial(0, {});
    radio->emergencyDial(0, {}, 0, {}, EmergencyCallRouting(0), true, false);
    radio->exitEmergencyCallbackMode(0);
    radio->explicitCallTransfer(0);
    radio->getCallForwardStatus(0, {});
    radio->getCallWaiting(0, 0);
    radio->getClip(0);
    radio->getClir(0);
    radio->getCurrentCalls(0);
    radio->getLastCallFailCause(0);
    radio->getMute(0);
    radio->getPreferredVoicePrivacy(0);
    radio->getTtyMode(0);
    radio->handleStkCallSetupRequestFromSim(0, true);
    radio->hangup(0, 1);
    radio->hangupForegroundResumeBackground(0);
    radio->hangupWaitingOrBackground(0);
    radio->isVoNrEnabled(0);
    radio->rejectCall(0);
    radio->responseAcknowledgement();
    radio->sendBurstDtmf(0, "", 0, 0);
    radio->sendCdmaFeatureCode(0, "");
    radio->sendDtmf(0, "");
    radio->sendUssd(0, "");
    radio->separateConnection(0, 1);
    radio->setCallForward(0, {});
    radio->setCallWaiting(0, true, 0);
    radio->setClir(0, 0);
    radio->setMute(0, true);
    radio->setPreferredVoicePrivacy(0, true);
    radio->setTtyMode(0, TtyMode(0));
    radio->setVoNrEnabled(0, true);
    radio->startDtmf(0, "");
    radio->stopDtmf(0);
    radio->switchWaitingOrHoldingAndActive(0);
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioVoiceResponseCallbackAdapter) {
    TestChecker checker;
    auto response = std::make_shared<RadioVoiceResponseCallbackAdapter>();
    response->setResponseFunction(nullptr);
    response->acceptCallResponse({});
    response->acknowledgeRequest(0);
    response->cancelPendingUssdResponse({});
    response->conferenceResponse({});
    response->dialResponse({});
    response->emergencyDialResponse({});
    response->exitEmergencyCallbackModeResponse({});
    response->explicitCallTransferResponse({});
    response->getCallForwardStatusResponse({}, {});
    response->getCallWaitingResponse({}, true, 0);
    response->getClipResponse({}, ClipStatus(0));
    response->getClirResponse({}, 0, 0);
    response->getCurrentCallsResponse({}, {});
    response->getLastCallFailCauseResponse({}, {});
    response->getMuteResponse({}, true);
    response->getPreferredVoicePrivacyResponse({}, true);
    response->getTtyModeResponse({}, TtyMode(0));
    response->handleStkCallSetupRequestFromSimResponse({});
    response->hangupConnectionResponse({});
    response->hangupForegroundResumeBackgroundResponse({});
    response->hangupWaitingOrBackgroundResponse({});
    response->isVoNrEnabledResponse({}, true);
    response->rejectCallResponse({});
    response->sendBurstDtmfResponse({});
    response->sendCdmaFeatureCodeResponse({});
    response->sendDtmfResponse({});
    response->sendUssdResponse({});
    response->separateConnectionResponse({});
    response->setCallForwardResponse({});
    response->setCallWaitingResponse({});
    response->setClirResponse({});
    response->setMuteResponse({});
    response->setPreferredVoicePrivacyResponse({});
    response->setTtyModeResponse({});
    response->setVoNrEnabledResponse({});
    response->startDtmfResponse({});
    response->stopDtmfResponse({});
    response->switchWaitingOrHoldingAndActiveResponse({});
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioVoiceIndicationCallbackAdapter) {
    TestChecker checker;
    auto indication = std::make_shared<RadioVoiceIndicationCallbackAdapter>();
    indication->setResponseFunction(nullptr);
    indication->callRing(RadioIndicationType::UNSOLICITED, true, {});
    indication->callStateChanged(RadioIndicationType::UNSOLICITED);
    indication->cdmaCallWaiting(RadioIndicationType::UNSOLICITED, {});
    indication->cdmaInfoRec(RadioIndicationType::UNSOLICITED, {});
    indication->cdmaOtaProvisionStatus(RadioIndicationType::UNSOLICITED, CdmaOtaProvisionStatus(0));
    indication->currentEmergencyNumberList(RadioIndicationType::UNSOLICITED, {});
    indication->enterEmergencyCallbackMode(RadioIndicationType::UNSOLICITED);
    indication->exitEmergencyCallbackMode(RadioIndicationType::UNSOLICITED);
    indication->indicateRingbackTone(RadioIndicationType::UNSOLICITED, true);
    indication->onSupplementaryServiceIndication(RadioIndicationType::UNSOLICITED, {});
    indication->onUssd(RadioIndicationType::UNSOLICITED, UssdModeType(0), "");
    indication->resendIncallMute(RadioIndicationType::UNSOLICITED);
    indication->srvccStateNotify(RadioIndicationType::UNSOLICITED, SrvccState(0));
    indication->stkCallControlAlphaNotify(RadioIndicationType::UNSOLICITED, "");
    indication->stkCallSetup(RadioIndicationType::UNSOLICITED, 0);
    checker.set(true);
}