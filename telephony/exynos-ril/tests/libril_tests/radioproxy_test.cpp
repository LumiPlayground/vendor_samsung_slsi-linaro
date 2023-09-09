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
#include <radioproxy.h>
#include <radioresponse.h>

TEST(RadioProxy, RadioProxy) {
    using namespace android::hardware::radio::impl;
    auto modem = std::static_pointer_cast<RadioModemCallback>(
                        std::make_shared<RadioModemCallbackAdapter>());
    auto data = std::static_pointer_cast<RadioDataCallback>(
                        std::make_shared<RadioDataCallbackAdapter>());
    auto messaging = std::static_pointer_cast<RadioMessagingCallback>(
                        std::make_shared<RadioMessagingCallbackAdapter>());
    auto network = std::static_pointer_cast<RadioNetworkCallback>(
                        std::make_shared<RadioNetworkCallbackAdapter>());
    auto sim = std::static_pointer_cast<RadioSimCallback>(
                        std::make_shared<RadioSimCallbackAdapter>());
    auto config = std::static_pointer_cast<RadioConfigCallback>(
                        std::make_shared<RadioConfigCallbackAdapter>());
    auto voice = std::static_pointer_cast<RadioVoiceCallback>(
                        std::make_shared<RadioVoiceCallbackAdapter>());
    uint32_t slot = 0;
    auto response = RadioResponse::make(slot);
    auto indication = RadioIndication::make(slot);
    auto test = RadioProxy::make(slot, nullptr, nullptr);
    test->setResponseFunctions(response, indication);
    test->modem(modem);
    test->data(data);
    test->messaging(messaging);
    test->network(network);
    test->sim(sim);
    test->config(config);
    test->voice(voice);
    EXPECT_TRUE(test->response() == response);
    EXPECT_TRUE(test->indication() == indication);
    EXPECT_TRUE(test->modem() == modem);
    EXPECT_TRUE(test->data() == data);
    EXPECT_TRUE(test->messaging() == messaging);
    EXPECT_TRUE(test->network() == network);
    EXPECT_TRUE(test->sim() == sim);
    EXPECT_TRUE(test->config() == config);
    EXPECT_TRUE(test->voice() == voice);
}