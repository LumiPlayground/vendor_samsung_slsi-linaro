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
#include "radioproxytestsetup.h"
#include "radiodataproxy.h"

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_data = ::aidl::android::hardware::radio::data;

using namespace android::hardware::radio;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using android::sp;

TEST(RadioDataProxyHook, responseAcknowledgement) {
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> responseAcknowledgement() override {
            checker.checked();
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->responseAcknowledgement();
}

TEST(RadioDataProxyHook, allocatePduSessionId) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> allocatePduSessionId(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->allocatePduSessionId(s);
}

TEST(RadioDataProxyHook, releasePduSessionId) {
    static int32_t s = INTrand(true);
    static int32_t testID = 11;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> releasePduSessionId(int32_t serial, int32_t id) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(testID == id);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->releasePduSessionId(s,testID);
}

TEST(RadioDataProxyHook, cancelHandover) {
    static int32_t s = INTrand(true);
    static int cid = 3;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> cancelHandover(int32_t serial, int32_t  callId){
            checker.checked();
            EXPECT_TRUE(callId == cid);
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->cancelHandover(s,cid);
}


TEST(RadioDataProxyHook, setDataAllowed) {
    static int32_t s = INTrand(true);
    const bool testAllowed = false;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setDataAllowed(int32_t serial, bool allow) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(allow == testAllowed);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->setDataAllowed(s,testAllowed);
}

TEST(RadioDataProxyHook, startHandover) {
    static int32_t s = INTrand(true);
    const int32_t testCid = 11;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> startHandover(int32_t serial, int32_t callId) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(callId == testCid);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->startHandover(s,testCid);
}

TEST(RadioDataProxyHook, startKeepalive) {
    static int32_t s = INTrand(true);
    const aidl_data::KeepaliveRequest testKr= {
                .type = 1,
		.sourceAddress ={1,2,3,4},
		.sourcePort = 8080,
		.destinationAddress = {127,0,0,0},
		.destinationPort = 8080,
		.maxKeepaliveIntervalMillis =1000,
		.cid = 11,
    	};
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> startKeepalive(int32_t  serial,
            const ::android::hardware::radio::V1_1::KeepaliveRequest& /*keepalive*/) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->startKeepalive(s,testKr);
}

TEST(RadioDataProxyHook, getDataCallList) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getDataCallList_1_6(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->getDataCallList(s);
}

TEST(RadioDataProxyHook, getSlicingConfig) {
    static int32_t s = INTrand(true);
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> getSlicingConfig(int32_t serial) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->getSlicingConfig(s);
}


TEST(RadioDataProxyHook, stopKeepalive) {
    static int32_t s = INTrand(true);
    static int32_t testSessionHandle = 11;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> stopKeepalive(int32_t serial, int32_t sessionHandle) override {
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(sessionHandle == testSessionHandle);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->stopKeepalive(s,testSessionHandle);
}

TEST(RadioDataProxyHook, setDataThrottling) {
    static int32_t s = INTrand(true);
    const aidl_data::DataThrottlingAction act = (aidl_data::DataThrottlingAction)0; //NO_DATA_THROTTLING
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> setDataThrottling(int32_t serial,
            ::android::hardware::radio::V1_6::DataThrottlingAction /*dataThrottlingAction */, int64_t completionDurationMillis) override{
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(completionDurationMillis == 1000);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->setDataThrottling(s,act,1000);
}

TEST(RadioDataProxyHook, deactivateDataCall) {
    static int32_t s = INTrand(true);
    static int32_t callId = 3;
    aidl_data::DataRequestReason rs = (aidl_data::DataRequestReason)1;
    struct TestRadio : public TestRadioBase_1_6 {
        TestChecker checker;
        Return<void> deactivateDataCall_1_2(int32_t serial, int32_t cid, ::android::hardware::radio::V1_2::DataRequestReason /* reasonRadioShutDown */) override{
            checker.checked();
            EXPECT_TRUE(serial == s);
            EXPECT_TRUE(cid == callId);
            return {};
        }
    };
    RadioProxyTestSetup<RadioDataProxy> test = sp<TestRadio>::make();
    test->deactivateDataCall(s,callId,rs);
}
