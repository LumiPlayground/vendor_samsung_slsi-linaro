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
using ::aidl::android::hardware::radio::sim::CardPowerState;
using ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource;
using ::aidl::android::hardware::radio::sim::PersoSubstate;
using ::aidl::android::hardware::radio::sim::SimLockMultiSimPolicy;

TEST(RadioCallbackAdapter, RadioSimCallbackAdapter) {
    TestChecker checker;
    auto radio = std::make_shared<RadioSimCallbackAdapter>();
    radio->setResponseFunctions(nullptr, nullptr);
    radio->areUiccApplicationsEnabled(0);
    radio->changeIccPin2ForApp(0, "", "", "");
    radio->changeIccPinForApp(0, "", "", "");
    radio->enableUiccApplications(0, true);
    radio->getAllowedCarriers(0);
    radio->getCdmaSubscription(0);
    radio->getCdmaSubscriptionSource(0);
    radio->getFacilityLockForApp(0, "", "", 0, "");
    radio->getIccCardStatus(0);
    radio->getImsiForApp(0, "");
    radio->getSimPhonebookCapacity(0);
    radio->getSimPhonebookRecords(0);
    radio->iccCloseLogicalChannel(0, 0);
    radio->iccIoForApp(0, {});
    radio->iccOpenLogicalChannel(0, "", 0);
    radio->iccTransmitApduBasicChannel(0, {});
    radio->iccTransmitApduLogicalChannel(0, {});
    radio->reportStkServiceIsRunning(0);
    radio->requestIccSimAuthentication(0, 0, "", "");
    radio->responseAcknowledgement();
    radio->sendEnvelope(0, "");
    radio->sendEnvelopeWithStatus(0, "");
    radio->sendTerminalResponseToSim(0, "");
    radio->setAllowedCarriers(0, {}, SimLockMultiSimPolicy(0));
    radio->setCarrierInfoForImsiEncryption(0, {});
    radio->setCdmaSubscriptionSource(0, CdmaSubscriptionSource(0));
    radio->setFacilityLockForApp(0, "", true, "", 0, "");
    radio->setSimCardPower(0, CardPowerState(0));
    radio->setUiccSubscription(0, {});
    radio->supplyIccPin2ForApp(0, "", "");
    radio->supplyIccPinForApp(0, "", "");
    radio->supplyIccPuk2ForApp(0, "", "", "");
    radio->supplyIccPukForApp(0, "", "", "");
    radio->supplySimDepersonalization(0, PersoSubstate(0), "");
    radio->updateSimPhonebookRecords(0, {});
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioSimResponseCallbackAdapter) {
    TestChecker checker;
    auto response = std::make_shared<RadioSimResponseCallbackAdapter>();
    response->setResponseFunction(nullptr);
    response->acknowledgeRequest({});
    response->areUiccApplicationsEnabledResponse({}, {});
    response->changeIccPin2ForAppResponse({}, {});
    response->changeIccPinForAppResponse({}, {});
    response->enableUiccApplicationsResponse({});
    response->getAllowedCarriersResponse({}, {}, {});
    response->getCdmaSubscriptionResponse({}, {}, {}, {}, {}, {});
    response->getCdmaSubscriptionSourceResponse({}, {});
    response->getFacilityLockForAppResponse({}, {});
    response->getIccCardStatusResponse({}, {});
    response->getIMSIForAppResponse({}, {});
    response->getSimPhonebookCapacityResponse({}, {});
    response->getSimPhonebookRecordsResponse({});
    response->iccCloseLogicalChannelResponse({});
    response->iccIoForAppResponse({}, {});
    response->iccOpenLogicalChannelResponse({}, {}, {});
    response->iccTransmitApduBasicChannelResponse({}, {});
    response->iccTransmitApduLogicalChannelResponse({}, {});
    response->reportStkServiceIsRunningResponse({});
    response->requestIccSimAuthenticationResponse({}, {});
    response->sendEnvelopeResponse({}, {});
    response->sendEnvelopeWithStatusResponse({}, {});
    response->sendTerminalResponseToSimResponse({});
    response->setAllowedCarriersResponse({});
    response->setCarrierInfoForImsiEncryptionResponse({});
    response->setFacilityLockForAppResponse({}, {});
    response->setUiccSubscriptionResponse({});
    response->supplyIccPin2ForAppResponse({}, {});
    response->supplyIccPinForAppResponse({}, {});
    response->supplyIccPuk2ForAppResponse({}, {});
    response->supplyIccPukForAppResponse({}, {});
    response->supplySimDepersonalizationResponse({}, {}, {});
    response->updateSimPhonebookRecordsResponse({}, {});
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioSimIndicationCallbackAdapter) {
    TestChecker checker;
    auto indication = std::make_shared<RadioSimIndicationCallbackAdapter>();
    indication->setResponseFunction(nullptr);
    indication->carrierInfoForImsiEncryption({});
    indication->cdmaSubscriptionSourceChanged({}, {});
    indication->simPhonebookChanged({});
    indication->simPhonebookRecordsReceived({}, {}, {});
    indication->simRefresh({}, {});
    indication->simStatusChanged({});
    indication->stkEventNotify({}, {});
    indication->stkProactiveCommand({}, {});
    indication->stkSessionEnd({});
    indication->subscriptionStatusChanged({}, {});
    indication->uiccApplicationsEnablementChanged({}, {});
    checker.set(true);
}