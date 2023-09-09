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
#include <ril_internal.h>
#include "radiomodemresponse.h"
#include "radiocallbackadapter.h"

using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_modem = ::aidl::android::hardware::radio::modem;

static HookCommandsTestSetup<RadioModemResponse, RadioModemIndication> sTestStub;

TEST(ModemCommandsIndHook, hardwareConfigChanged) {
    static RIL_HardwareConfig testVector[] = {
        {
            RIL_HardwareConfig_Type::RIL_HARDWARE_CONFIG_MODEM, "uuid0",
            RIL_HardwareConfig_State::RIL_HARDWARE_CONFIG_STATE_ENABLED,
            {.modem = {12, RADIO_TECH_LTE, 1, 1, 2}},
        },
    };
    struct TestResponseCb : public RadioModemIndicationCallbackAdapter {
        TestChecker checker;
        void hardwareConfigChanged(aidl_common::RadioIndicationType /* in_type */,
                const std::vector<aidl_modem::HardwareConfig>& config) override {
            checker.checked();
            EXPECT_TRUE(config.size() == NUM_ELEMS(testVector));
            for (size_t i = 0; i < config.size(); i++) {
                test(config[i], testVector[i]);
            }
        }
        void test(const aidl_modem::HardwareConfig& config, const RIL_HardwareConfig& rilConfig) {
            EXPECT_TRUE(config.type == static_cast<int32_t>(rilConfig.type));
            EXPECT_TRUE(config.uuid == rilConfig.uuid);
            EXPECT_TRUE(config.state == static_cast<int32_t>(rilConfig.state));
            if (config.modem.size() > 0) {
                auto& modem = config.modem[0];
                auto& rilModem = rilConfig.cfg.modem;
                EXPECT_TRUE(modem.rilModel == rilModem.rilModel);
                EXPECT_TRUE(static_cast<uint32_t>(modem.rat) == rilModem.rat);
                EXPECT_TRUE(modem.maxVoiceCalls == rilModem.maxVoice);
                EXPECT_TRUE(modem.maxDataCalls == rilModem.maxData);
                EXPECT_TRUE(modem.maxStandby == rilModem.maxStandby);
            }
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new RadioModemResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_HARDWARE_CONFIG_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsIndHook, modemReset) {
    static char testVector[] = "test_reset";
    struct TestResponseCb : public RadioModemIndicationCallbackAdapter {
        TestChecker checker;
        void modemReset(aidl_common::RadioIndicationType /* in_type */,
                const std::string& reason) override {
            checker.checked();
            EXPECT_TRUE(reason == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new RadioModemResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_MODEM_RESTART);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsIndHook, radioCapabilityIndication) {
    static RIL_RadioCapability testVector = {
        1, 2, RC_PHASE_START, RADIO_TECH_LTE, "logical_modem_uuid0", RC_STATUS_SUCCESS
    };
    struct TestResponseCb : public RadioModemIndicationCallbackAdapter {
        TestChecker checker;
        void radioCapabilityIndication(aidl_common::RadioIndicationType /* in_type */,
                const aidl_modem::RadioCapability& rc) override {
            checker.checked();
            EXPECT_TRUE(rc.session == testVector.session);
            EXPECT_TRUE(rc.phase == testVector.phase);
            EXPECT_TRUE(rc.raf == testVector.rat);
            EXPECT_TRUE(rc.logicalModemUuid == testVector.logicalModemUuid);
            EXPECT_TRUE(rc.status == testVector.status);
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new RadioModemResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RADIO_CAPABILITY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsIndHook, radioStateChanged) {
    struct TestResponseCb : public RadioModemIndicationCallbackAdapter {
        TestChecker checker;
        void radioStateChanged(aidl_common::RadioIndicationType /* in_type */,
                aidl_modem::RadioState radioState) override {
            checker.checked();
            EXPECT_TRUE(radioState == aidl_modem::RadioState::OFF ||
                        radioState == aidl_modem::RadioState::UNAVAILABLE ||
                        radioState == aidl_modem::RadioState::ON);
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new RadioModemResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ModemCommandsIndHook, rilConnected) {
    struct TestResponseCb : public RadioModemIndicationCallbackAdapter {
        TestChecker checker;
        void rilConnected(aidl_common::RadioIndicationType /* in_type */ ) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new RadioModemResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RIL_CONNECTED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}