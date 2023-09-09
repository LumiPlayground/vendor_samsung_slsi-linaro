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

TEST(RadioCallbackAdapter, RadioConfigCallbackAdapter) {
    TestChecker checker;
    auto radio = std::make_shared<RadioConfigCallbackAdapter>();
    radio->setResponseFunctions(nullptr, nullptr);
    radio->getHalDeviceCapabilities(0);
    radio->getNumOfLiveModems(0);
    radio->getPhoneCapability(0);
    radio->getSimSlotsStatus(0);
    radio->setNumOfLiveModems(0, 2);
    radio->setPreferredDataModem(0, 0);
    radio->setSimSlotsMapping(0, {});
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioConfigResponseCallbackAdapter) {
    TestChecker checker;
    auto response = std::make_shared<RadioConfigResponseCallbackAdapter>();
    response->setResponseFunction(nullptr);
    response->getHalDeviceCapabilitiesResponse({}, true);
    response->getNumOfLiveModemsResponse({}, 2);
    response->getPhoneCapabilityResponse({}, {});
    response->getSimSlotsStatusResponse({}, {});
    response->setNumOfLiveModemsResponse({});
    response->setPreferredDataModemResponse({});
    response->setSimSlotsMappingResponse({});
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioConfigIndicationCallbackAdapter) {
    TestChecker checker;
    auto indication = std::make_shared<RadioConfigIndicationCallbackAdapter>();
    indication->setResponseFunction(nullptr);
    indication->simSlotsStatusChanged({}, {});
    checker.set(true);
}