/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * ril_service_1_6_response_test.cpp
 *
 *  Created on: 2021. 8. 11.
 */
#include <gtest/gtest.h>
#include "ril_service_1_6.h"
#include "ril_service_common.h"
#include "ril_service_test.h"
#include <librilutils/radio_conv_utils.h>
#include "test_radio_hidl_utils_1_5.h"
#include "test_radio_hidl_utils_1_6.h"
#include <sys/types.h>
#include <sys/wait.h>

using namespace android::hardware::radio;
using android::hardware::Status;

using libriltest::GetRandomSerialNumber;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
static VendorFunctionExcutor sVendorFunctionExcutor;

#if defined(ANDROID_MULTI_SIM)
static void onTestRequest(int request, void *data, size_t datalen, RIL_Token t,
        RIL_SOCKET_ID socket_id) {
    sVendorFunctionExcutor.onRequest(request, data, datalen, t, socket_id);
}
static RIL_RadioState onTestStateRequest(RIL_SOCKET_ID socket_id) {
    return sVendorFunctionExcutor.onStateRequest(socket_id);
}
#else
static void onTestRequest(int request, void *data, size_t datalen, RIL_Token t) {
    sVendorFunctionExcutor.onRequest(request, data, datalen, t, RIL_SOCKET_1);
}
static RIL_RadioState onTestStateRequest() {
    return sVendorFunctionExcutor.onStateRequest(RIL_SOCKET_1);
}
#endif

static RIL_RadioFunctions sTestVendorFunctions = {
    RIL_VERSION,
    onTestRequest,
    onTestStateRequest,
    nullptr,
    nullptr,
    nullptr,
};

static sp<V1_6::IRadio> setupRadio(OnRequestListener *listener = nullptr) {
    radio::impl::v1_6::initRadios();
    sp<V1_0::IRadio> radio = radio::impl::v1_6::getRadio(RIL_SOCKET_1);
    EXPECT_FALSE(radio.get() == nullptr);
    sp<V1_6::IRadio> radioV16 = V1_6::IRadio::castFrom(radio).withDefault(nullptr);
    EXPECT_FALSE(radioV16.get() == nullptr);

    s_vendorFunctions = &sTestVendorFunctions;
    sVendorFunctionExcutor.setOnRequestListener(listener);

    return radioV16;
}

static void tearDown(sp<V1_6::IRadio>& radio) {
    radio->setResponseFunctions(nullptr, nullptr);
    radio = nullptr;
    sVendorFunctionExcutor.setOnRequestListener(nullptr);
}

TEST(ril_service_1_6_rsp_tests, setupDataCallResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setupDataCallResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SetupDataCallResult_V1_6 testVector = {
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> setupDataCallResponse_1_6(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const android::hardware::radio::V1_6::SetupDataCallResult& dcResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_6::RadioError::NONE ||
                    info.error == V1_6::RadioError::INVALID_RESPONSE);
            if (info.error == V1_6::RadioError::NONE) {
                test(dcResponse, testVector);
            }
            return Void();
        }

        void test(const V1_6::SetupDataCallResult& /*test*/,
                const RIL_SetupDataCallResult_V1_6& /*dc*/) {

        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::setupDataCallResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    // test
    radio::setupDataCallResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getDataCallListResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataCallListResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SetupDataCallResult_V1_6 testVector[] = {
        {}, {}
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getDataCallListResponse_1_6(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const hidl_vec<::android::hardware::radio::V1_6::SetupDataCallResult>& dcResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_6::RadioError::NONE ||
                    info.error == V1_6::RadioError::INVALID_RESPONSE);
            if (info.error == V1_6::RadioError::NONE) {
                EXPECT_EQ(dcResponse.size(), size);
                for (size_t i = 0; i < dcResponse.size(); i++) {
                    test(dcResponse[i], testVector[i]);
                }
            }
            return Void();
        }

        void test(const V1_6::SetupDataCallResult& /*test*/,
                const RIL_SetupDataCallResult_V1_6& /*dc*/) {

        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getDataCallListResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    // test
    radio::getDataCallListResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, setNrDualConnectivityStateResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setNrDualConnectivityStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> setNrDualConnectivityStateResponse(const V1_6::RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setNrDualConnectivityStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, isNrDualConnectivityEnabledResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::isNrDualConnectivityEnabledResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> isNrDualConnectivityEnabledResponse(
                const V1_6::RadioResponseInfo& info, bool isEnabled) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(isEnabled, testVector[0]);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::isNrDualConnectivityEnabledResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, setRadioPowerResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setRadioPowerResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> setRadioPowerResponse_1_6(const V1_6::RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setRadioPowerResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, sendSmsResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendSmsResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SMS_Response testVector = {
        200, (char *)"", 0
    };
    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> sendSmsResponse_1_6(
                const V1_6::RadioResponseInfo& info,
                const SendSmsResult& sms) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(sms.messageRef, testVector.messageRef);
            EXPECT_TRUE(sms.ackPDU == testVector.ackPDU);
            EXPECT_EQ(sms.errorCode, testVector.errorCode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendSmsResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, sendSmsExpectMoreResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendSmsExpectMoreResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SMS_Response testVector = {
        200, (char *)"", 0
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> sendSmsExpectMoreResponse_1_6(
                const V1_6::RadioResponseInfo& info,
                const SendSmsResult& sms) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(sms.messageRef, testVector.messageRef);
            EXPECT_TRUE(sms.ackPDU == testVector.ackPDU);
            EXPECT_EQ(sms.errorCode, testVector.errorCode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendSmsExpectMoreResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, sendCdmaSmsResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendCdmaSmsResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SMS_Response testVector = {
        200, (char *)"", 0
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> sendCdmaSmsResponse_1_6(
                const V1_6::RadioResponseInfo& info,
                const SendSmsResult& sms) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(sms.messageRef, testVector.messageRef);
            EXPECT_TRUE(sms.ackPDU == testVector.ackPDU);
            EXPECT_EQ(sms.errorCode, testVector.errorCode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendCdmaSmsResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, sendCdmaSmsExpectMoreResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendCdmaSmsExpectMoreResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SMS_Response testVector = {
        200, (char *)"", 0
    };
    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> sendCdmaSmsExpectMoreResponse_1_6(
                const V1_6::RadioResponseInfo& info,
                const SendSmsResult& sms) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(sms.messageRef, testVector.messageRef);
            EXPECT_TRUE(sms.ackPDU == testVector.ackPDU);
            EXPECT_EQ(sms.errorCode, testVector.errorCode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendCdmaSmsExpectMoreResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, setSimCardPowerResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setSimCardPowerResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> setSimCardPowerResponse_1_6(const V1_6::RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setSimCardPowerResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, allocatePduSessionIdResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::allocatePduSessionIdResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> allocatePduSessionIdResponse(
                const V1_6::RadioResponseInfo& info, int32_t id) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(id, testVector[0]);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::allocatePduSessionIdResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, releasePduSessionIdResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::releasePduSessionIdResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> releasePduSessionIdResponse(const V1_6::RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::releasePduSessionIdResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, startHandoverResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::startHandoverResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> startHandoverResponse(const V1_6::RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::startHandoverResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, cancelHandoverResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::cancelHandoverResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> cancelHandoverResponse(const V1_6::RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::cancelHandoverResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, setAllowedNetworkTypesBitmapResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setAllowedNetworkTypesBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> setAllowedNetworkTypesBitmapResponse(const V1_6::RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setAllowedNetworkTypesBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, setDataThrottlingResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setDataThrottlingResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> setDataThrottlingResponse(const V1_6::RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setDataThrottlingResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getSystemSelectionChannelsResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSystemSelectionChannelsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RadioAccessSpecifier_V1_5 testVector[] = {
        {}, {}
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);
    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getSystemSelectionChannelsResponse(
                const V1_6::RadioResponseInfo& info,
                const hidl_vec<V1_5::RadioAccessSpecifier>& specifier) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_6::RadioError::NONE ||
                    info.error == V1_6::RadioError::INVALID_RESPONSE);
            if (info.error == V1_6::RadioError::NONE) {
                EXPECT_EQ(specifier.size(), size);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getSystemSelectionChannelsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 1);

    radio::getSystemSelectionChannelsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getCurrentCallsResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCurrentCallsResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_Call_V1_6 *testVector[] = {
        new RIL_Call_V1_6 {
            RIL_CallState::RIL_CALL_ACTIVE,
            1,
            149,
            0,
            1,
            1,
            1,
            0,
            (char *)"0123456",
            0,
            (char *)"",
            0,
            0,
            RIL_AudioQuality::AMR_WB,
            0
        },
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getCurrentCallsResponse_1_6(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::Call>& calls) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_6::RadioError::NONE ||
                    info.error == V1_6::RadioError::INVALID_RESPONSE);
            if (info.error == V1_6::RadioError::NONE) {
                EXPECT_EQ(calls.size(), size);
                for (size_t i = 0; i < calls.size(); i++) {
                    test(calls[i], *testVector[i]);
                }
            }
            return Void();
        }

        void test(const V1_6::Call& /*test*/, const RIL_Call_V1_6& /*call*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getCurrentCallsResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getCurrentCallsResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);

    for (size_t i = 0; i < size; i++) {
        if (testVector[i]) {
            delete testVector[i];
        }
    }
}

TEST(ril_service_1_6_rsp_tests, getVoiceRegistrationStateResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getVoiceRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RegStateResult_V1_6 testVector = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        RIL_RegistrationFailCause::REG_FAIL_CAUSE_NONE,
        {},
        {},
        {},
    };
    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getVoiceRegistrationStateResponse_1_6(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const ::android::hardware::radio::V1_6::RegStateResult& regResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_6::RadioError::NONE ||
                    info.error == V1_6::RadioError::INVALID_RESPONSE);
            if (info.error == V1_6::RadioError::NONE) {
                test(regResponse, testVector);
            }
            return Void();
        }

        void test(const V1_6::RegStateResult& test,
                const RIL_RegStateResult_V1_6& voiceRegResponse) {
            EXPECT_EQ((int)test.regState, (int)voiceRegResponse.regState);
            EXPECT_EQ((int)test.rat, (int)voiceRegResponse.rat);
            EXPECT_EQ((int)test.reasonForDenial, (int)voiceRegResponse.reasonForDenial);
            EXPECT_TRUE(test.registeredPlmn == voiceRegResponse.registeredPlmn);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getVoiceRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getVoiceRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getVoiceRegistrationStateResponse_1_6_fallback) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getVoiceRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RegStateResult_V1_6 testVector = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        RIL_RegistrationFailCause::REG_FAIL_CAUSE_NONE,
        {},
        {},
        {},
    };
    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getVoiceRegistrationStateResponse_1_5(
                const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                const ::android::hardware::radio::V1_5::RegStateResult& regResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_0::RadioError::NONE);
            if (info.error == V1_0::RadioError::NONE) {
                RIL_RegStateResult voiceRegResult {};
                RadioConvUtils::convertVoiceRegistrationStateResult(voiceRegResult, testVector);
                test(regResponse, voiceRegResult);
            }
            return Void();
        }

        void test(const V1_5::RegStateResult& test,
                const RIL_RegStateResult& voiceRegResponse) {
            EXPECT_EQ((int)test.regState, (int)voiceRegResponse.regState);
            EXPECT_EQ((int)test.rat, (int)voiceRegResponse.rat);
            EXPECT_EQ((int)test.reasonForDenial, (int)voiceRegResponse.reasonForDenial);
            EXPECT_TRUE(test.registeredPlmn == voiceRegResponse.registeredPlmn);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getVoiceRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getDataRegistrationStateResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RegStateResult_V1_6 testVector = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        RIL_RegistrationFailCause::REG_FAIL_CAUSE_NONE,
        {},
        {},
        {},
    };
    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getDataRegistrationStateResponse_1_6(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const ::android::hardware::radio::V1_6::RegStateResult& regResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_6::RadioError::NONE ||
                    info.error == V1_6::RadioError::INVALID_RESPONSE);
            if (info.error == V1_6::RadioError::NONE) {
                test(regResponse, testVector);
            }
            return Void();
        }

        void test(const V1_6::RegStateResult& test,
                const RIL_RegStateResult_V1_6& dataRegResponse) {
            EXPECT_EQ((int)test.regState, (int)dataRegResponse.regState);
            EXPECT_EQ((int)test.rat, (int)dataRegResponse.rat);
            EXPECT_EQ((int)test.reasonForDenial, (int)dataRegResponse.reasonForDenial);
            EXPECT_TRUE(test.registeredPlmn == dataRegResponse.registeredPlmn);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getDataRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getDataRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getDataRegistrationStateResponse_1_6_fallback) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RegStateResult_V1_6 testVector = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        RIL_RegistrationFailCause::REG_FAIL_CAUSE_NONE,
        {},
        {},
        {},
    };
    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getDataRegistrationStateResponse_1_5(
                const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                const ::android::hardware::radio::V1_5::RegStateResult& regResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_0::RadioError::NONE);
            if (info.error == V1_0::RadioError::NONE) {
                RIL_RegStateResult dataRegResult {};
                RadioConvUtils::convertVoiceRegistrationStateResult(dataRegResult, testVector);
                test(regResponse, dataRegResult);
            }
            return Void();
        }

        void test(const V1_5::RegStateResult& test,
                const RIL_RegStateResult& dataRegResponse) {
            EXPECT_EQ((int)test.regState, (int)dataRegResponse.regState);
            EXPECT_EQ((int)test.rat, (int)dataRegResponse.rat);
            EXPECT_EQ((int)test.reasonForDenial, (int)dataRegResponse.reasonForDenial);
            EXPECT_TRUE(test.registeredPlmn == dataRegResponse.registeredPlmn);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getDataRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getDataRegistrationStateResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getAllowedNetworkTypesBitmapResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getAllowedNetworkTypesBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 0x7FFFF };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getAllowedNetworkTypesBitmapResponse(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const ::android::hardware::hidl_bitfield<
                        ::android::hardware::radio::V1_4::RadioAccessFamily>
                        networkTypeBitmap) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ((int)networkTypeBitmap, testVector[0]);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getAllowedNetworkTypesBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getSignalStrengthResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSignalStrengthResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SignalStrength_V1_6 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { { 30, 50, 10, -100, 3, 0 }, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
        {}, // NR
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getSignalStrengthResponse_1_6(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const ::android::hardware::radio::V1_6::SignalStrength& signalStrength) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_6::RadioError::NONE ||
                    info.error == V1_6::RadioError::INVALID_RESPONSE);
            if (info.error == V1_6::RadioError::NONE) {
                test(signalStrength.lte.base,
                        static_cast<RIL_LTE_SignalStrength_v8&>(testVector.LTE_SignalStrength));
            }
            return Void();
        }

        void test(const V1_0::LteSignalStrength& test, const RIL_LTE_SignalStrength_v8& lte) {
            EXPECT_EQ(test.signalStrength, (unsigned int)/* NEED TO CHECK */ lte.signalStrength);
            EXPECT_EQ(test.rsrp, (unsigned int)/* NEED TO CHECK */ lte.rsrp);
            EXPECT_EQ(test.rsrq, (unsigned int)/* NEED TO CHECK */ lte.rsrq);
            EXPECT_EQ(test.rssnr, lte.rssnr);
            EXPECT_EQ(test.cqi, (unsigned int)/* NEED TO CHECK */ lte.cqi);
            EXPECT_EQ(test.timingAdvance, (unsigned int)/* NEED TO CHECK */ lte.timingAdvance);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getSignalStrengthResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::getSignalStrengthResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getSignalStrengthResponse_1_6_fallback) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSignalStrengthResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SignalStrength_V1_6 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { { 30, 50, 10, -100, 3, 0 }, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
        {}, // NR
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getSignalStrengthResponse_1_4(
                const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                const ::android::hardware::radio::V1_4::SignalStrength& signalStrength) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_0::RadioError::NONE);
            if (info.error == V1_0::RadioError::NONE) {
                test(signalStrength.lte,
                        static_cast<RIL_LTE_SignalStrength_v8&>(testVector.LTE_SignalStrength));
            }
            return Void();
        }

        void test(const V1_0::LteSignalStrength& test, const RIL_LTE_SignalStrength_v8& lte) {
            EXPECT_EQ(test.signalStrength, (unsigned int)/* NEED TO CHECK */ lte.signalStrength);
            EXPECT_EQ(test.rsrp, (unsigned int)/* NEED TO CHECK */ lte.rsrp);
            EXPECT_EQ(test.rsrq, (unsigned int)/* NEED TO CHECK */ lte.rsrq);
            EXPECT_EQ(test.rssnr, lte.rssnr);
            EXPECT_EQ(test.cqi, (unsigned int)/* NEED TO CHECK */ lte.cqi);
            EXPECT_EQ(test.timingAdvance, (unsigned int)/* NEED TO CHECK */ lte.timingAdvance);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getSignalStrengthResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getSlicingConfigResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSlicingConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SlicingConfig testVector = {
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getSlicingConfigResponse(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const ::android::hardware::radio::V1_6::SlicingConfig& /*slicingConfig*/) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_6::RadioError::NONE ||
                    info.error == V1_6::RadioError::INVALID_RESPONSE ||
                    info.error == V1_6::RadioError::MODEM_ERR);
            if (info.error == V1_6::RadioError::NONE) {
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getSlicingConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    radio::getSlicingConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_MODEM_ERR, 0, 0);

    radio::getSlicingConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getCellInfoListResponse_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCellInfoListResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_6 testVector[] = {
        // TODO
        {},
        {},
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getCellInfoListResponse_1_6(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>&
                        cellInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_6::RadioError::NONE ||
                      info.error == V1_6::RadioError::INVALID_RESPONSE);
              if (info.error == V1_6::RadioError::NONE) {
                for (size_t i = 0; i < cellInfo.size(); i++) {
                    EXPECT_EQ(cellInfo.size(), size);
                    test(cellInfo[i], testVector[i]);
                }
            }
            return Void();
        }

        void test(const V1_5::CellInfo& /*test*/, const RIL_CellInfo_V1_5& /*cellInfo*/) {
            // TODO
        }

        void test(const V1_6::CellInfo& /*test*/, const RIL_CellInfo_V1_6& /*cellInfo*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getCellInfoListResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(RIL_CellInfo_V1_5));

    radio::getCellInfoListResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getCellInfoListResponse_1_6_fallback) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCellInfoListResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_6 testVector[] = {
        // TODO
        {},
        {},
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCellInfoListResponse_1_5(
                const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                        cellInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == V1_0::RadioError::NONE);
              if (info.error == V1_0::RadioError::NONE) {
                for (size_t i = 0; i < cellInfo.size(); i++) {
                    EXPECT_EQ(cellInfo.size(), size);
                    RIL_CellInfo_V1_5 ci {};
                    RadioConvUtils::convertCellInfo(ci, testVector[i]);
                    test(cellInfo[i], ci);
                }
            }
            return Void();
        }

        void test(const V1_5::CellInfo& /*test*/, const RIL_CellInfo_V1_5& /*cellInfo*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getCellInfoListResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(RIL_CellInfo_V1_5));

    radio::getCellInfoListResponse_1_6(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getSimPhonebookRecordsResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSimPhonebookRecordsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getSimPhonebookRecordsResponse(const V1_6::RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getSimPhonebookRecordsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, getSimPhonebookCapacityResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSimPhonebookCapacityResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_PhonebookCapacity testVector = {
        100, 200, 300, 400, 100, 200, 300, 400, 100, 200
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> getSimPhonebookCapacityResponse(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                const ::android::hardware::radio::V1_6::PhonebookCapacity& capacity) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(capacity.maxAdnRecords, testVector.maxAdnRecords);
            EXPECT_EQ(capacity.usedAdnRecords, testVector.usedAdnRecords);
            EXPECT_EQ(capacity.maxEmailRecords, testVector.maxEmailRecords);
            EXPECT_EQ(capacity.usedEmailRecords, testVector.usedEmailRecords);
            EXPECT_EQ(capacity.maxAdditionalNumberRecords, testVector.maxAdditionalNumberRecords);
            EXPECT_EQ(capacity.usedAdditionalNumberRecords, testVector.usedAdditionalNumberRecords);
            EXPECT_EQ(capacity.maxNameLen, testVector.maxNameLen);
            EXPECT_EQ(capacity.maxNumberLen, testVector.maxNumberLen);
            EXPECT_EQ(capacity.maxEmailLen, testVector.maxEmailLen);
            EXPECT_EQ(capacity.maxAdditionalNumberLen, testVector.maxAdditionalNumberLen);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getSimPhonebookCapacityResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, updateSimPhonebookRecordsResponse) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::updateSimPhonebookRecordsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_UpdatePbRsp testVector = { .recordId = 10 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        Return<void> updateSimPhonebookRecordsResponse(
                const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
                int32_t updatedRecordIndex) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(updatedRecordIndex, testVector.recordId);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::updateSimPhonebookRecordsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, remote_died) {
    sp<V1_6::IRadio> radio = setupRadio();

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        bool called = false;
        Return<void> isNrDualConnectivityEnabledResponse(
                const V1_6::RadioResponseInfo& info, bool isEnabled) {
            called = true;
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(isEnabled, testVector[0]);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;

    pid_t pid = fork();
    if (pid == 0) {
        dlog("child pid=%d", getpid());
        radio->setResponseFunctions(responseCallback, indicationCallback);
        exit(1);
    } else {
        dlog("wait for child pid=%d is died", pid);
        wait(0);
        dlog("child pid=%d is died", pid);
    }
    // no response callback after a remote process is died.
    radio::isNrDualConnectivityEnabledResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    EXPECT_FALSE(responseCallback->called);

    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::isNrDualConnectivityEnabledResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    EXPECT_TRUE(responseCallback->called);
    tearDown(radio);
}

TEST(ril_service_1_6_rsp_tests, checkReturnStatus_not_okay) {
    sp<V1_6::IRadio> radio = setupRadio();

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    struct TestRadioResponseImpl : public TestRadioResponseBase_1_6 {
        bool called = false;
        Return<void> isNrDualConnectivityEnabledResponse(
                const V1_6::RadioResponseInfo& info, bool isEnabled) {
            called = true;
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, V1_6::RadioError::NONE);
            EXPECT_EQ(isEnabled, testVector[0]);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_6::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_6;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::isNrDualConnectivityEnabledResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    EXPECT_TRUE(responseCallback->called);

    // Return<void> is not okay.
    responseCallback->called = false;
    Return<void> ret(Status::fromExceptionCode(Status::EX_TRANSACTION_FAILED));
    radio::impl::v1_6::checkReturnStatus(RIL_SOCKET_1, ret);

    radio::isNrDualConnectivityEnabledResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    EXPECT_TRUE(responseCallback->called);
    tearDown(radio);
}
