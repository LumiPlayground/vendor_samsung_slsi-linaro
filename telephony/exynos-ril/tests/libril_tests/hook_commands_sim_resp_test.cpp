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
#include "radiosimresponse.h"
#include "radiocallbackadapter.h"

using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_sim = ::aidl::android::hardware::radio::sim;

static HookCommandsTestSetup<RadioSimResponse, RadioSimIndication> sTestStub;

TEST(SimCommandsRespHook, areUiccApplicationsEnabledResponse) {
    static int32_t testVector = 1;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void areUiccApplicationsEnabledResponse(const aidl_common::RadioResponseInfo& /* info */,
                bool enabled) override {
            checker.checked();
            EXPECT_TRUE(enabled == static_cast<bool>(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, changeIccPin2ForAppResponse) {
    static int32_t testVector = 3;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void changeIccPin2ForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                int remainingRetries) override {
            checker.checked();
            EXPECT_TRUE(remainingRetries == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CHANGE_SIM_PIN2);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, changeIccPinForAppResponse) {
    static int32_t testVector = 3;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void changeIccPinForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                int remainingRetries) override {
            checker.checked();
            EXPECT_TRUE(remainingRetries == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CHANGE_SIM_PIN);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, enableUiccApplicationsResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void enableUiccApplicationsResponse(const aidl_common::RadioResponseInfo& /* info */)
                override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ENABLE_UICC_APPLICATIONS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, getAllowedCarriersResponse) {
    static RIL_Carrier allowed[2] = {
            {"001", "01", RIL_MATCH_ALL, "ALL"}, {"002", "02", RIL_MATCH_ALL, "ALL"}
    };
    static RIL_Carrier excluded[2] = {
            {"001", "01", RIL_MATCH_SPN, "SPN"}, {"002", "02", RIL_MATCH_IMSI_PREFIX, "IMSI"}
    };
    static RIL_CarrierRestrictions_V1_4 testVector = {
            .carriers.len_allowed_carriers = 2,
            .carriers.len_excluded_carriers = 2,
            .carriers.allowed_carriers = allowed,
            .carriers.excluded_carriers = excluded,
            .carriers.allowedCarriersPrioritized = false,
            .multiSimPolicy = NO_MULTISIM_POLICY
    };
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void getAllowedCarriersResponse(const aidl_common::RadioResponseInfo& /* info */,
                const aidl_sim::CarrierRestrictions& carriers,
                const aidl_sim::SimLockMultiSimPolicy& multiSimPolicy) override {
            checker.checked();
            int i = 0;
            for (auto& carrier : carriers.allowedCarriers) {
                EXPECT_TRUE(carrier.mcc == testVector.carriers.allowed_carriers[i].mcc);
                EXPECT_TRUE(carrier.mnc == testVector.carriers.allowed_carriers[i].mnc);
                EXPECT_TRUE(carrier.matchType ==
                        static_cast<int32_t>(testVector.carriers.allowed_carriers[i].match_type));
                EXPECT_TRUE(carrier.matchData ==
                        testVector.carriers.allowed_carriers[i++].match_data);
            }
            i = 0;
            for (auto& carrier : carriers.excludedCarriers) {
                EXPECT_TRUE(carrier.mcc == testVector.carriers.excluded_carriers[i].mcc);
                EXPECT_TRUE(carrier.mnc == testVector.carriers.excluded_carriers[i].mnc);
                EXPECT_TRUE(carrier.matchType ==
                        static_cast<int32_t>(testVector.carriers.excluded_carriers[i].match_type));
                EXPECT_TRUE(carrier.matchData ==
                        testVector.carriers.excluded_carriers[i++].match_data);
            }
            EXPECT_TRUE(multiSimPolicy ==
                    aidl_sim::SimLockMultiSimPolicy{testVector.multiSimPolicy});
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_GET_CARRIER_RESTRICTIONS,
            HAL_VERSION_CODE(1, 4)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, getCdmaSubscriptionResponse) {
    static char* testVector[] = {
            (char*)"mdn", (char*)"hSid", (char*)"hNid", (char*)"min", (char*)"prl"
    };
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void getCdmaSubscriptionResponse(const aidl_common::RadioResponseInfo& /* info */,
                const std::string& mdn, const std::string& hSid, const std::string& hNid,
                const std::string& min, const std::string& prl) override {
            checker.checked();
            EXPECT_TRUE(mdn == testVector[0]);
            EXPECT_TRUE(hSid == testVector[1]);
            EXPECT_TRUE(hNid == testVector[2]);
            EXPECT_TRUE(min == testVector[3]);
            EXPECT_TRUE(prl == testVector[4]);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_SUBSCRIPTION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, getCdmaSubscriptionSourceResponse) {
    static int32_t testVector = 1;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void getCdmaSubscriptionSourceResponse(const aidl_common::RadioResponseInfo& /* info */,
                aidl_sim::CdmaSubscriptionSource source) override {
            checker.checked();
            EXPECT_TRUE(source == aidl_sim::CdmaSubscriptionSource(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, getFacilityLockForAppResponse) {
    static int testVector = 0;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void getFacilityLockForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                int response) override {
            checker.checked();
            EXPECT_TRUE(response == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_QUERY_FACILITY_LOCK);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, getIMSIForAppResponse) {
    static char* testVector = (char*)"imsi";
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void getIMSIForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                const std::string& imsi) override {
            checker.checked();
            EXPECT_TRUE(imsi == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_IMSI);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, getSimPhonebookCapacityResponse) {
    static int32_t testVector[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void getSimPhonebookCapacityResponse(const aidl_common::RadioResponseInfo& /* info */,
                const aidl_sim::PhonebookCapacity& capacity) override {
            checker.checked();
            EXPECT_TRUE(capacity.maxAdnRecords == testVector[0]);
            EXPECT_TRUE(capacity.usedAdnRecords == testVector[1]);
            EXPECT_TRUE(capacity.maxEmailRecords == testVector[2]);
            EXPECT_TRUE(capacity.usedEmailRecords == testVector[3]);
            EXPECT_TRUE(capacity.maxAdditionalNumberRecords == testVector[4]);
            EXPECT_TRUE(capacity.usedAdditionalNumberRecords == testVector[5]);
            EXPECT_TRUE(capacity.maxNameLen == testVector[6]);
            EXPECT_TRUE(capacity.maxNumberLen == testVector[7]);
            EXPECT_TRUE(capacity.maxEmailLen == testVector[8]);
            EXPECT_TRUE(capacity.maxAdditionalNumberLen == testVector[9]);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_SIM_PHONEBOOK_CAPACITY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, getSimPhonebookRecordsResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void getSimPhonebookRecordsResponse(const aidl_common::RadioResponseInfo& /* info */)
                override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_GET_SIM_PHONEBOOK_RECORDS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, iccCloseLogicalChannelResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void iccCloseLogicalChannelResponse(const aidl_common::RadioResponseInfo& /* info */)
                override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SIM_CLOSE_CHANNEL);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, iccIoForAppResponse) {
    static RIL_SIM_IO_Response testVector = {
        0x90, 0x00, (char*)"simResponse"
    };
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void iccIoForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                const aidl_sim::IccIoResult& iccIo) override {
            checker.checked();
            EXPECT_TRUE(iccIo.sw1 == testVector.sw1);
            EXPECT_TRUE(iccIo.sw2 == testVector.sw2);
            EXPECT_TRUE(iccIo.simResponse == testVector.simResponse);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SIM_IO);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, iccOpenLogicalChannelResponse) {
    static int32_t testVector[] = {
        1,  // channelId
        2, 3, 4, 5, 6, 7, 8, 9, 10  // selectResponse
    };
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void iccOpenLogicalChannelResponse(const aidl_common::RadioResponseInfo& /* info */,
                int channelId, const std::vector<uint8_t>& selectResponse) override {
            checker.checked();
            EXPECT_TRUE(channelId == testVector[0]);
            int i = 1;
            for(auto& response : selectResponse) {
                EXPECT_TRUE(response == static_cast<uint8_t>(testVector[i++]));
            }
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SIM_OPEN_CHANNEL);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, iccTransmitApduBasicChannelResponse) {
    static RIL_SIM_IO_Response testVector = {
        0x90, 0x00, (char*)"simResponse"
    };
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void iccTransmitApduBasicChannelResponse(const aidl_common::RadioResponseInfo& /* info */,
                const aidl_sim::IccIoResult& result) override {
            checker.checked();
            EXPECT_TRUE(result.sw1 == testVector.sw1);
            EXPECT_TRUE(result.sw2 == testVector.sw2);
            EXPECT_TRUE(result.simResponse == testVector.simResponse);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, iccTransmitApduLogicalChannelResponse) {
    static RIL_SIM_IO_Response testVector = {
        0x90, 0x00, (char*)"simResponse"
    };
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void iccTransmitApduLogicalChannelResponse(const aidl_common::RadioResponseInfo& /* info */,
                const aidl_sim::IccIoResult& result) override {
            checker.checked();
            EXPECT_TRUE(result.sw1 == testVector.sw1);
            EXPECT_TRUE(result.sw2 == testVector.sw2);
            EXPECT_TRUE(result.simResponse == testVector.simResponse);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, reportStkServiceIsRunningResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void reportStkServiceIsRunningResponse(const aidl_common::RadioResponseInfo& /* info */)
                override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, requestIccSimAuthenticationResponse) {
    static RIL_SIM_IO_Response testVector = {
        0x90, 0x00, (char*)"simResponse"
    };
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void requestIccSimAuthenticationResponse(const aidl_common::RadioResponseInfo& /* info */,
                const aidl_sim::IccIoResult& result) override {
            checker.checked();
            EXPECT_TRUE(result.sw1 == testVector.sw1);
            EXPECT_TRUE(result.sw2 == testVector.sw2);
            EXPECT_TRUE(result.simResponse == testVector.simResponse);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SIM_AUTHENTICATION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, sendEnvelopeResponse) {
    static char* testVector = (char*)"commandResponse";
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void sendEnvelopeResponse(const aidl_common::RadioResponseInfo& /* info */,
                const std::string& commandResponse) override {
            checker.checked();
            EXPECT_TRUE(commandResponse == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, sendEnvelopeWithStatusResponse) {
    static RIL_SIM_IO_Response testVector = {
        0x90, 0x00, (char*)"simResponse"
    };
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void sendEnvelopeWithStatusResponse(const aidl_common::RadioResponseInfo& /* info */,
                const aidl_sim::IccIoResult& iccIo) override {
            checker.checked();
            EXPECT_TRUE(iccIo.sw1 == testVector.sw1);
            EXPECT_TRUE(iccIo.sw2 == testVector.sw2);
            EXPECT_TRUE(iccIo.simResponse == testVector.simResponse);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, sendTerminalResponseToSimResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void sendTerminalResponseToSimResponse(const aidl_common::RadioResponseInfo& /* info */)
                override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, setAllowedCarriersResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void setAllowedCarriersResponse(const aidl_common::RadioResponseInfo& /* info */)
                override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0,
            ENCODE_REQUEST(RIL_REQUEST_SET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1, 4)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, setCarrierInfoForImsiEncryptionResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void setCarrierInfoForImsiEncryptionResponse(const aidl_common::RadioResponseInfo& /*info*/)
                override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, setCdmaSubscriptionSourceResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void setCdmaSubscriptionSourceResponse(const aidl_common::RadioResponseInfo& /*info*/)
                override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, setFacilityLockForAppResponse) {
    static int32_t testVector = 1;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void setFacilityLockForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                int32_t retry) override {
            checker.checked();
            EXPECT_TRUE(retry == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_FACILITY_LOCK);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, setUiccSubscriptionResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void setUiccSubscriptionResponse(const aidl_common::RadioResponseInfo& /*info*/)
                override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_UICC_SUBSCRIPTION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, supplyIccPin2ForAppResponse) {
    static int32_t testVector = 3;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void supplyIccPin2ForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                int32_t remainingRetries) override {
            checker.checked();
            EXPECT_TRUE(remainingRetries == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ENTER_SIM_PIN2);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, supplyIccPinForAppResponse) {
    static int32_t testVector = 3;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void supplyIccPinForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                int32_t remainingRetries) override {
            checker.checked();
            EXPECT_TRUE(remainingRetries == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ENTER_SIM_PIN);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, supplyIccPuk2ForAppResponse) {
    static int32_t testVector = 10;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void supplyIccPuk2ForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                int32_t remainingRetries) override {
            checker.checked();
            EXPECT_TRUE(remainingRetries == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ENTER_SIM_PUK2);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, supplyIccPukForAppResponse) {
    static int32_t testVector = 10;
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void supplyIccPukForAppResponse(const aidl_common::RadioResponseInfo& /* info */,
                int32_t remainingRetries) override {
            checker.checked();
            EXPECT_TRUE(remainingRetries == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ENTER_SIM_PUK);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, supplySimDepersonalizationResponse) {
    static int32_t testVector[] = {1, 3};
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void supplySimDepersonalizationResponse(const aidl_common::RadioResponseInfo& /* info */,
                const aidl_sim::PersoSubstate persoType, int32_t remainingRetries) override {
            checker.checked();
            EXPECT_TRUE(persoType == aidl_sim::PersoSubstate(testVector[0]));
            EXPECT_TRUE(remainingRetries == testVector[1]);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, updateSimPhonebookRecordsResponse) {
    static RIL_UpdatePbRsp testVector = {1, 2, 3};
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void updateSimPhonebookRecordsResponse(const aidl_common::RadioResponseInfo& /* info */,
                int32_t updatedRecordIndex) override {
            checker.checked();
            printf("updatedRecordIndex : %d, testVector : %d\n", updatedRecordIndex, testVector.recordId);
            EXPECT_TRUE(updatedRecordIndex == testVector.recordId);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_UPDATE_SIM_PHONEBOOK_RECORDS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsRespHook, setSimCardPowerResponse) {
    struct TestResponseCb : public RadioSimResponseCallbackAdapter {
        TestChecker checker;
        void setSimCardPowerResponse(const aidl_common::RadioResponseInfo& /* info */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new TestResponseCb);
    auto indicationCb =
            std::shared_ptr<RadioSimIndicationCallback>(new RadioSimIndicationCallbackAdapter);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0,
            ENCODE_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, HAL_VERSION_CODE(1, 6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}
