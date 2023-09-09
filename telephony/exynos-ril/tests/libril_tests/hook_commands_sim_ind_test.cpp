/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
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
#include "radiosimresponse.h"
#include "radiocallbackadapter.h"


using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_sim = ::aidl::android::hardware::radio::sim;

static HookCommandsTestSetup<RadioSimResponse, RadioSimIndication> sTestStub;

TEST(SimCommandsIndHook, carrierInfoForImsiEncryption) {
    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void carrierInfoForImsiEncryption(aidl_common::RadioIndicationType /* type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, cdmaSubscriptionSourceChanged) {
    static int32_t testVector = 0;
    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void cdmaSubscriptionSourceChanged(aidl_common::RadioIndicationType /* type */,
                aidl_sim::CdmaSubscriptionSource cdmaSource) override {
            checker.checked();
            EXPECT_TRUE(cdmaSource == aidl_sim::CdmaSubscriptionSource(testVector));
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, simRefresh) {
    static RIL_SimRefreshResponse_v7 testVector = {
        .result = RIL_SimRefreshResult::SIM_FILE_UPDATE,
        .ef_id = 0x4f00,
        .aid = (char *)"aid"
    };

    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void simRefresh(aidl_common::RadioIndicationType /* type */,
                const aidl_sim::SimRefreshResult& refreshResult) override {
            checker.checked();
            EXPECT_TRUE(refreshResult.type == testVector.result);
            EXPECT_TRUE(refreshResult.efId == testVector.ef_id);
            EXPECT_TRUE(refreshResult.aid == testVector.aid);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_SIM_REFRESH);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, simStatusChanged) {
    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void simStatusChanged(aidl_common::RadioIndicationType /* type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, stkEventNotify) {
    static char testVector[] = "stkEventNotify";
    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void stkEventNotify(aidl_common::RadioIndicationType /* type */,
                const std::string& cmd) override {
            checker.checked();
            EXPECT_TRUE(cmd == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_STK_EVENT_NOTIFY);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, stkProactiveCommand) {
    static char testVector[] = "stkProactiveCommand";
    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void stkProactiveCommand(aidl_common::RadioIndicationType /* type */,
                const std::string& cmd) override {
            checker.checked();
            EXPECT_TRUE(cmd == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_STK_PROACTIVE_COMMAND);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, stkSessionEnd) {
    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void stkSessionEnd(aidl_common::RadioIndicationType /* type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_STK_SESSION_END);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, subscriptionStatusChanged) {
    static int testVector = 1;
    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void subscriptionStatusChanged(aidl_common::RadioIndicationType /* type */,
                bool activate) override {
            checker.checked();
            EXPECT_TRUE((int) activate == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, uiccApplicationsEnablementChanged) {
    static int testVector = 1;
    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void uiccApplicationsEnablementChanged(aidl_common::RadioIndicationType /* type */,
                bool enabled) override {
            checker.checked();
            EXPECT_TRUE((int) enabled == testVector);
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, simPhonebookChanged) {
    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void simPhonebookChanged(aidl_common::RadioIndicationType /* type */) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, NULL, 0);
    sTestStub.teardown();
}

TEST(SimCommandsIndHook, simPhonebookRecordsReceived) {
    static int32_t receivedStatus = 0;
    static RIL_PhonebookRecordInfo Records[2] = {};
    Records[0].recordId = 0;
    Records[0].emailSetCount = 1;
    Records[0].anrSetCount = 1;
    Records[0].anrType = 0;
    strncpy(Records[0].name, "name0", strlen("name0"));
    strncpy(Records[0].number, "number0", strlen("number0"));
    strncpy(Records[0].emails[0], "emails0", strlen("emails0"));
    strncpy(Records[0].additionalNumbers[0], "1111", strlen("1111"));
    Records[1].recordId = 1;
    Records[1].emailSetCount = 2;
    Records[1].anrSetCount = 1;
    Records[1].anrType = 0;
    strncpy(Records[1].name, "name1", strlen("name1"));
    strncpy(Records[1].number, "number1", strlen("number1"));
    strncpy(Records[1].emails[0], "emails10", strlen("emails10"));
    strncpy(Records[1].emails[1], "emails11", strlen("emails11"));
    strncpy(Records[1].additionalNumbers[0], "2222", strlen("2222"));
    static RIL_PhonebookRecInfoResult testVector = {
            .loadedCount = 2,
            .receivedStatus = receivedStatus,
            .mPbRec = Records
    };

    struct TestIndicationCb : public RadioSimIndicationCallbackAdapter {
        TestChecker checker;
        void simPhonebookRecordsReceived(aidl_common::RadioIndicationType /* type */,
                aidl_sim::PbReceivedStatus status,
                const std::vector<aidl_sim::PhonebookRecordInfo>& records) override {
            checker.checked();
            EXPECT_EQ(status, aidl_sim::PbReceivedStatus(receivedStatus));
            int index = 0;
            for(auto& record : records) {
                EXPECT_EQ(record.recordId, testVector.mPbRec[index].recordId);
                EXPECT_EQ(record.name, testVector.mPbRec[index].name);
                EXPECT_EQ(record.number, testVector.mPbRec[index].number);
                int i = 0;
                for(auto& email : record.emails) {
                    EXPECT_EQ(email, testVector.mPbRec[index].emails[i++]);
                }
                i = 0;
                for(auto& additionalNumber : record.additionalNumbers) {
                    EXPECT_EQ(additionalNumber, testVector.mPbRec[index++].additionalNumbers[i++]);
                }
            }
        }
    };
    auto responseCb = std::shared_ptr<RadioSimResponseCallback>(new RadioSimResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioSimIndicationCallback>(new TestIndicationCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_RECORDS_RECEIVED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector,
            sizeof(testVector));
    sTestStub.teardown();
}
