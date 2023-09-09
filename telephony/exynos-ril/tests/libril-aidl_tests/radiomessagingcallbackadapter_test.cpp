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
using ::aidl::android::hardware::radio::messaging::SmsAcknowledgeFailCause;

TEST(RadioCallbackAdapter, RadioMessagingCallbackAdapter) {
    TestChecker checker;
    auto radio = std::make_shared<RadioMessagingCallbackAdapter>();
    radio->setResponseFunctions(nullptr, nullptr);
    radio->acknowledgeIncomingGsmSmsWithPdu(0, true, "");
    radio->acknowledgeLastIncomingCdmaSms(0, {});
    radio->acknowledgeLastIncomingGsmSms(0, true, SmsAcknowledgeFailCause(0));
    radio->deleteSmsOnRuim(0, 0);
    radio->deleteSmsOnSim(0, 0);
    radio->getCdmaBroadcastConfig(0);
    radio->getGsmBroadcastConfig(0);
    radio->getSmscAddress(0);
    radio->reportSmsMemoryStatus(0, true);
    radio->responseAcknowledgement();
    radio->sendCdmaSms(0, {});
    radio->sendCdmaSmsExpectMore(0, {});
    radio->sendImsSms(0, {});
    radio->sendSms(0, {});
    radio->sendSmsExpectMore(0, {});
    radio->setCdmaBroadcastActivation(0, true);
    radio->setCdmaBroadcastConfig(0, {});
    radio->setGsmBroadcastActivation(0, true);
    radio->setGsmBroadcastConfig(0, {});
    radio->setSmscAddress(0, "");
    radio->writeSmsToRuim(0, {});
    radio->writeSmsToSim(0, {});
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioMessagingResponseCallbackAdapter) {
    TestChecker checker;
    auto response = std::make_shared<RadioMessagingResponseCallbackAdapter>();
    response->setResponseFunction(nullptr);
    response->acknowledgeIncomingGsmSmsWithPduResponse({});
    response->acknowledgeLastIncomingCdmaSmsResponse({});
    response->acknowledgeLastIncomingGsmSmsResponse({});
    response->acknowledgeRequest(0);
    response->deleteSmsOnRuimResponse({});
    response->deleteSmsOnSimResponse({});
    response->getCdmaBroadcastConfigResponse({}, {});
    response->getGsmBroadcastConfigResponse({}, {});
    response->getSmscAddressResponse({}, {});
    response->reportSmsMemoryStatusResponse({});
    response->sendCdmaSmsExpectMoreResponse({}, {});
    response->sendCdmaSmsResponse({}, {});
    response->sendImsSmsResponse({}, {});
    response->sendSmsExpectMoreResponse({}, {});
    response->sendSmsResponse({}, {});
    response->setCdmaBroadcastActivationResponse({});
    response->setCdmaBroadcastConfigResponse({});
    response->setGsmBroadcastActivationResponse({});
    response->setGsmBroadcastActivationResponse({});
    response->setGsmBroadcastConfigResponse({});
    response->setSmscAddressResponse({});
    response->writeSmsToRuimResponse({}, {});
    response->writeSmsToSimResponse({}, {});
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioMessagingIndicationCallbackAdapter) {
    TestChecker checker;
    auto indication = std::make_shared<RadioMessagingIndicationCallbackAdapter>();
    indication->setResponseFunction(nullptr);
    indication->cdmaNewSms({}, {});
    indication->cdmaRuimSmsStorageFull({});
    indication->newBroadcastSms({}, {});
    indication->newSms({}, {});
    indication->newSmsOnSim({}, {});
    indication->newSmsStatusReport({}, {});
    indication->simSmsStorageFull({});
    checker.set(true);
}