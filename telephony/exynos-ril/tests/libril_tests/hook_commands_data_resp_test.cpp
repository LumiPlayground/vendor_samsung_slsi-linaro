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
#include "radiodataresponse.h"
#include "radiocallbackadapter.h"

using namespace android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace data = ::aidl::android::hardware::radio::data;

static HookCommandsTestSetup<RadioDataResponse, RadioDataIndication> sTestStub;

TEST(DataCommandsRespHook, setupDataCallResponse_1_5) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void setupDataCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            const ::aidl::android::hardware::radio::data::SetupDataCallResult /*dcResponse*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 5)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}


TEST(DataCommandsRespHook, setupDataCallResponse_1_6) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void setupDataCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            const ::aidl::android::hardware::radio::data::SetupDataCallResult /*dcResponse*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}


TEST(DataCommandsRespHook, setDataProfileResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void setDataProfileResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_DATA_PROFILE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}


TEST(DataCommandsRespHook, getSlicingConfigResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void getSlicingConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            const ::aidl::android::hardware::radio::data::SlicingConfig /*slicingConfig*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_SLICING_CONFIG, HAL_VERSION_CODE(1, 6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}


TEST(DataCommandsRespHook, deactivateDataCallResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void deactivateDataCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_DEACTIVATE_DATA_CALL);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}


TEST(DataCommandsRespHook, allocatePduSessionIdResponse) {
    static int testId = 11;
    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void allocatePduSessionIdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/, int id) override {
            checker.checked();
            EXPECT_TRUE(id == testId);
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ALLOCATE_PDU_SESSION_ID);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, &testId, sizeof(testId));
    sTestStub.teardown();
}

TEST(DataCommandsRespHook, releasePduSessionIdResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void releasePduSessionIdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_RELEASE_PDU_SESSION_ID);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(DataCommandsRespHook, startHandoverResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void startHandoverResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_START_HANDOVER);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(DataCommandsRespHook, cancelHandoverResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void cancelHandoverResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_CANCEL_HANDOVER);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}


TEST(DataCommandsRespHook, startKeepaliveResponse) {
    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void startKeepaliveResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
            const::aidl::android::hardware::radio::data::KeepaliveStatus /*status*/)  override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_START_KEEPALIVE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(DataCommandsRespHook, stopKeepaliveResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void stopKeepaliveResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_STOP_KEEPALIVE);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}


TEST(DataCommandsRespHook, setDataAllowedResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
      void setDataAllowedResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/)override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_ALLOW_DATA);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(DataCommandsRespHook, setInitialAttachApnResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
      void setInitialAttachApnResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/)override {
            checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_INITIAL_ATTACH_APN);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}
#if 0
TEST(DataCommandsRespHook, getDataCallListResponse_1_5) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void getDataCallListResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
             const std::vector<::aidl::android::hardware::radio::data::SetupDataCallResult> /*dcResponse*/) override {
             checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_DATA_CALL_LIST, HAL_VERSION_CODE(1, 5)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}
#endif
TEST(DataCommandsRespHook, getDataCallListResponse_1_6) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void getDataCallListResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/,
           const std::vector<::aidl::android::hardware::radio::data::SetupDataCallResult> /*dcResponse*/) override {
               checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, ENCODE_REQUEST(RIL_REQUEST_DATA_CALL_LIST, HAL_VERSION_CODE(1, 6)));
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}

TEST(DataCommandsRespHook, setDataThrottlingResponse) {

    struct TestResponseCb : public RadioDataResponseCallbackAdapter {
        TestChecker checker;
        void setDataThrottlingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& /*info*/) override {
             checker.checked();
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new TestResponseCb);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new RadioDataIndicationCallbackAdapter);

    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getCommandInfo(0, RIL_REQUEST_SET_DATA_THROTTLING);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_SOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}
