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

TEST(ModemCommandsRespHook, enableModemResponse) {
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void enableModemResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ENABLE_MODEM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, getBasebandVersionResponse) {
    static char testVector[] = "test_baseband01";
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void getBasebandVersionResponse(const aidl_common::RadioResponseInfo& /*info*/,
                const std::string& version) override {
            checker.checked();
            EXPECT_TRUE(version == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_BASEBAND_VERSION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, getDeviceIdentityResponse) {
    static const char *testVector[] = {
        "0123456789", "0123456789", "0123456789", "0123456789",
    };
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void getDeviceIdentityResponse(const aidl_common::RadioResponseInfo& /*info*/,
                const std::string& imei, const std::string& imeisv,
                const std::string& esn, const std::string& meid) override {
            checker.checked();
            EXPECT_TRUE(imei == testVector[0]);
            EXPECT_TRUE(imeisv == testVector[1]);
            EXPECT_TRUE(esn == testVector[2]);
            EXPECT_TRUE(meid == testVector[3]);
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_DEVICE_IDENTITY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, getHardwareConfigResponse) {
    static RIL_HardwareConfig testVector[] = {
        {
            RIL_HardwareConfig_Type::RIL_HARDWARE_CONFIG_MODEM, "uuid0",
            RIL_HardwareConfig_State::RIL_HARDWARE_CONFIG_STATE_ENABLED,
            {.modem = {12, RADIO_TECH_LTE, 1, 1, 2}},
        },
    };
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void getHardwareConfigResponse(const aidl_common::RadioResponseInfo& /*info*/,
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
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_HARDWARE_CONFIG);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, getModemActivityInfoResponse) {
    static RIL_ActivityStatsInfo testVector = {
        1000, 2000, {1, 2, 3, 4, 5}, 4000,
    };
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void getModemActivityInfoResponse(const aidl_common::RadioResponseInfo& /*info*/,
                const aidl_modem::ActivityStatsInfo& activityInfo) override {
            checker.checked();
            EXPECT_TRUE(activityInfo.sleepModeTimeMs == static_cast<int32_t>(testVector.sleep_mode_time_ms));
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_ACTIVITY_INFO);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, getModemStackStatusResponse) {
    static int testVector = true;
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void getModemStackStatusResponse(const aidl_common::RadioResponseInfo& /*info*/,
                bool isEnabled) override {
            checker.checked();
            EXPECT_TRUE(isEnabled == static_cast<bool>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_MODEM_STATUS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, getRadioCapabilityResponse) {
    static RIL_RadioCapability testVector = {
        1, 2, RC_PHASE_START, RADIO_TECH_LTE, "logical_modem_uuid0", RC_STATUS_SUCCESS
    };
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void getRadioCapabilityResponse(const aidl_common::RadioResponseInfo& /*info*/,
                const aidl_modem::RadioCapability& rc) override {
            checker.checked();
            EXPECT_TRUE(rc.session == testVector.session);
            EXPECT_TRUE(rc.phase == testVector.phase);
            EXPECT_TRUE(rc.raf == testVector.rat);
            EXPECT_TRUE(rc.logicalModemUuid == testVector.logicalModemUuid);
            EXPECT_TRUE(rc.status == testVector.status);
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_RADIO_CAPABILITY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, nvReadItemResponse) {
    static char testVector[] = "nv_itme_value";
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void nvReadItemResponse(const aidl_common::RadioResponseInfo& /*info*/,
                const std::string& result) override {
            checker.checked();
            EXPECT_TRUE(result == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_NV_READ_ITEM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, nvResetConfigResponse) {
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void nvResetConfigResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_NV_RESET_CONFIG);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, nvWriteCdmaPrlResponse) {
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void nvWriteCdmaPrlResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_NV_WRITE_CDMA_PRL);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, nvWriteItemResponse) {
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void nvWriteItemResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_NV_WRITE_ITEM);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, requestShutdownResponse) {
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void requestShutdownResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SHUTDOWN);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, sendDeviceStateResponse) {
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void sendDeviceStateResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SEND_DEVICE_STATE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, setRadioCapabilityResponse) {
    static RIL_RadioCapability testVector = {
        1, 2, RC_PHASE_START, RADIO_TECH_LTE, "logical_modem_uuid0", RC_STATUS_SUCCESS
    };
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void setRadioCapabilityResponse(const aidl_common::RadioResponseInfo& /*info*/,
                const aidl_modem::RadioCapability& rc) override {
            checker.checked();
            EXPECT_TRUE(rc.session == testVector.session);
            EXPECT_TRUE(rc.phase == testVector.phase);
            EXPECT_TRUE(rc.raf == testVector.rat);
            EXPECT_TRUE(rc.logicalModemUuid == testVector.logicalModemUuid);
            EXPECT_TRUE(rc.status == testVector.status);
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_RADIO_CAPABILITY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, setRadioPowerResponse) {
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void setRadioPowerResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_RADIO_POWER);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, setRadioPowerResponse_1_5) {
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void setRadioPowerResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_RADIO_POWER, HAL_VERSION_CODE(1,5)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(ModemCommandsRespHook, setRadioPowerResponse_1_6) {
    struct TestResponseCb : public RadioModemResponseCallbackAdapter {
        TestChecker checker;
        void setRadioPowerResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioModemResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioModemIndicationCallback>(new RadioModemIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_RADIO_POWER, HAL_VERSION_CODE(1,6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}