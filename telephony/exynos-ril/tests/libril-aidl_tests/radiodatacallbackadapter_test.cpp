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
using ::aidl::android::hardware::radio::AccessNetwork;
using ::aidl::android::hardware::radio::data::DataRequestReason;
using ::aidl::android::hardware::radio::data::DataThrottlingAction;

TEST(RadioCallbackAdapter, RadioDataCallbackAdapter) {
    TestChecker checker;
    auto radio = std::make_shared<RadioDataCallbackAdapter>();
    radio->setResponseFunctions(nullptr, nullptr);
    radio->allocatePduSessionId(0);
    radio->cancelHandover(0, 1);
    radio->deactivateDataCall(0, 1, DataRequestReason::NORMAL);
    radio->getDataCallList(0);
    radio->getSlicingConfig(0);
    radio->releasePduSessionId(0, 0);
    radio->responseAcknowledgement();
    radio->setDataAllowed(0, true);
    radio->setDataProfile(0, {});
    radio->setDataThrottling(0, DataThrottlingAction::NO_DATA_THROTTLING, 1000);
    radio->setInitialAttachApn(0, {});
    radio->setupDataCall(0, AccessNetwork::UNKNOWN, {}, false,
                        DataRequestReason::NORMAL, {}, {}, -1, {}, false);
    radio->startHandover(0, 1);
    radio->startKeepalive(0, {});
    radio->stopKeepalive(0, 1);
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioDataResponseCallbackAdapter) {
    TestChecker checker;
    auto response = std::make_shared<RadioDataResponseCallbackAdapter>();
    response->acknowledgeRequest(0);
    response->setResponseFunction(nullptr);
    response->allocatePduSessionIdResponse({}, 1);
    response->cancelHandoverResponse({});
    response->deactivateDataCallResponse({});
    response->getDataCallListResponse({}, {});
    response->getSlicingConfigResponse({}, {});
    response->releasePduSessionIdResponse({});
    response->setDataAllowedResponse({});
    response->setDataThrottlingResponse({});
    response->setInitialAttachApnResponse({});
    response->setDataProfileResponse({});
    response->setupDataCallResponse({}, {});
    response->startHandoverResponse({});
    response->startKeepaliveResponse({}, {});
    response->stopKeepaliveResponse({});
    checker.set(true);
}
TEST(RadioCallbackAdapter, RadioDataIndicationCallbackAdapter) {
    TestChecker checker;
    auto indication = std::make_shared<RadioDataIndicationCallbackAdapter>();
    indication->setResponseFunction(nullptr);
    indication->dataCallListChanged({}, {});
    indication->keepaliveStatus({}, {});
    indication->pcoData({}, {});
    indication->unthrottleApn({}, {});
    checker.set(true);
}