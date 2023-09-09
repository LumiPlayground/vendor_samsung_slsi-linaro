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
namespace aidl_data = ::aidl::android::hardware::radio::data;

static HookCommandsTestSetup<RadioDataResponse, RadioDataIndication> sTestStub;


TEST(DataCommandsIndHook, dataCallListChanged) {
    struct TestResponseCb : public RadioDataIndicationCallbackAdapter {
        TestChecker checker;
        void dataCallListChanged( const ::aidl::android::hardware::radio::RadioIndicationType /* type*/,
              const std::vector< ::aidl::android::hardware::radio::data::SetupDataCallResult> /*dcList*/) override {
            checker.checked();

        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new RadioDataResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_DATA_CALL_LIST_CHANGED);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    sTestStub.teardown();
}


TEST(DataCommandsIndHook, pcoData) {
    using namespace std;
    std::string testBR ("IPV4V6");
    std::string testCT("");

    static RIL_PCO_Data testVector = {
       .cid = 1,
       .bearer_proto = (char*)testBR.c_str(),
       .pco_id=3,
       .contents_length = 3,
       .contents =(char*)testCT.c_str()
    };

    struct TestResponseCb : public RadioDataIndicationCallbackAdapter {
        TestChecker checker;
        void pcoData(const ::aidl::android::hardware::radio::RadioIndicationType /*type*/,
                                      const ::aidl::android::hardware::radio::data::PcoDataInfo pco)  override {
            checker.checked();
            EXPECT_TRUE(pco.cid == static_cast<int32_t>(testVector.cid));
            EXPECT_TRUE(pco.pcoId == static_cast<int32_t>(testVector.pco_id));
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new RadioDataResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_PCO_DATA);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}


TEST(DataCommandsIndHook, keepaliveStatus) {
    static RIL_KeepaliveStatus testVector = {0xC, KEEPALIVE_INACTIVE};
    struct TestResponseCb : public RadioDataIndicationCallbackAdapter {
        TestChecker checker;
        void keepaliveStatus(const ::aidl::android::hardware::radio::RadioIndicationType /*type*/,
        const ::aidl::android::hardware::radio::data::KeepaliveStatus status) override {
            checker.checked();
            EXPECT_TRUE(status.sessionHandle == static_cast<int32_t>(testVector.sessionHandle));
        }
    };
    auto responseCb = std::shared_ptr<RadioDataResponseCallback>(new RadioDataResponseCallbackAdapter);
    auto indicationCb = std::shared_ptr<RadioDataIndicationCallback>(new TestResponseCb);
    sTestStub.setup(responseCb, indicationCb);
    auto test = android::getUnsolRespInfo(0, RIL_UNSOL_KEEPALIVE_STATUS);
    ASSERT_NE(test, nullptr);
    ASSERT_NE(test->responseFunction, nullptr);
    test->responseFunction(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, &testVector, sizeof(testVector));
    sTestStub.teardown();
}
