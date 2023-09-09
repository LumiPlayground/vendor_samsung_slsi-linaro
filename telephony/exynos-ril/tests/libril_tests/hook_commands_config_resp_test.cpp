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
#include "commandsinfo.h"
#include "hookcommandstestsetup.h"
#include <slsi/radio_v2_0.h>
#include <slsi/radioconfig_v1_1.h>
#include <ril_internal.h>
#include "radioconfigresponse.h"
#include "radiocallbackadapter.h"

using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_config = ::aidl::android::hardware::radio::config;

static HookCommandsTestSetup<RadioConfigResponse, RadioConfigIndication> sTestStub;

TEST(ConfigCommandsRespHook, getModemsConfigResponse) {
    struct TestResponseCb : public RadioConfigResponseCallbackAdapter {
        TestChecker checker;
        void getNumOfLiveModemsResponse(const aidl_common::RadioResponseInfo& /* info*/,
                int8_t /* numOfLiveModems */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioConfigResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioConfigIndicationCallback>(new RadioConfigIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_MODEMS_CONFIG);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ConfigCommandsRespHook, getPhoneCapabilityResponse) {
    static RIL_PhoneCapability testVector = {
        2, 1, true, 2, {{0}, {1}}
    };
    struct TestResponseCb : public RadioConfigResponseCallbackAdapter {
        TestChecker checker;
        void getPhoneCapabilityResponse(const aidl_common::RadioResponseInfo& /* info*/,
                const aidl_config::PhoneCapability& phoneCapability) override {
            checker.checked();
            EXPECT_TRUE(phoneCapability.maxActiveData == static_cast<int8_t>(testVector.maxActiveData));
            EXPECT_TRUE(phoneCapability.maxActiveInternetData == static_cast<int8_t>(testVector.maxActiveInternetData));
            EXPECT_TRUE(phoneCapability.isInternetLingeringSupported == testVector.isInternetLingeringSupported);
            EXPECT_TRUE(phoneCapability.logicalModemIds.size() == testVector.len_logicalModemList);
            std::vector<uint8_t> testLogicalModemIds;
            for (size_t i = 0; i < testVector.len_logicalModemList; i++) {
                testLogicalModemIds.push_back(testVector.logicalModemList[i].modemId);
            }
            EXPECT_TRUE(phoneCapability.logicalModemIds == testLogicalModemIds);
        }
    };
    auto responseCb = std::shared_ptr<RadioConfigResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioConfigIndicationCallback>(new RadioConfigIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_PHONE_CAPABILITY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ConfigCommandsRespHook, setModemsConfigResponse) {
    struct TestResponseCb : public RadioConfigResponseCallbackAdapter {
        TestChecker checker;
        void setNumOfLiveModemsResponse(const aidl_common::RadioResponseInfo& /* info*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioConfigResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioConfigIndicationCallback>(new RadioConfigIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_MODEMS_CONFIG);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ConfigCommandsRespHook, setPreferredDataModemResponse) {
    struct TestResponseCb : public RadioConfigResponseCallbackAdapter {
        TestChecker checker;
        void setPreferredDataModemResponse(const aidl_common::RadioResponseInfo& /* info*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioConfigResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioConfigIndicationCallback>(new RadioConfigIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_PREFERRED_DATA_MODEM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}