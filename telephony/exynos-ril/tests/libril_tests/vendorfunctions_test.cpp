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
#include <telephony/ril.h>
#include <slsi/ril_oem.h>
#include "vendorfunctions.h"
#include "ril_internal.h"
#include "vendorfunctionexecutor.h"

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

static RIL_RadioFunctions testVendorFunctionsDefault = {
    .onRequest = onTestRequest,
    .onStateRequest = onTestStateRequest,
};

static VendorFunctions setupVendorFunctions(OnRequestListener *listener = nullptr) {
    sVendorFunctionExcutor.setOnRequestListener(listener);
    return VendorFunctions(&testVendorFunctionsDefault);
}

TEST(VendorFunctions, constructor1) {
    extern RIL_RadioFunctions *s_vendorFunctions;
    VendorFunctions test;
    if (s_vendorFunctions) {
        EXPECT_TRUE(test.mVendorFunctions.onRequest == s_vendorFunctions->onRequest);
        EXPECT_TRUE(test.mVendorFunctions.onStateRequest == s_vendorFunctions->onStateRequest);
    }
}

TEST(VendorFunctions, constructor2) {
    VendorFunctions test(RIL_SOCKET_1);
    EXPECT_TRUE(test.mSlotId == RIL_SOCKET_1);
}

TEST(VendorFunctions, onRadioStateRequest) {
    VendorFunctions test = setupVendorFunctions();
    EXPECT_TRUE(test.onRadioStateRequest() == RADIO_STATE_ON);
}

TEST(VendorFunctions, onRadioStateRequest_invalid) {
    RIL_RadioFunctions functions = {};
    VendorFunctions test(&functions);
    EXPECT_TRUE(test.onRadioStateRequest() == RADIO_STATE_UNAVAILABLE);
}

TEST(VendorFunctions, dispatchVoid) {
    static int32_t id = RIL_REQUEST_DATA_REGISTRATION_STATE;
    struct OnRequestListenerImpl : public OnRequestListener {
        TestChecker checker;
        void onRequest(int request, void */* data */, size_t /* datalen */, RIL_Token t, RIL_SOCKET_ID) {
            checker.checked();
            EXPECT_TRUE(request == id);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    VendorFunctions test = setupVendorFunctions(&callback);
    EXPECT_TRUE(test.dispatchVoid(INTrand(true), id));
}

TEST(VendorFunctions, dispatchVoid_invalid) {
    VendorFunctions test = setupVendorFunctions();
    EXPECT_FALSE(test.dispatchVoid(INTrand(true), RIL_OEM_REQUEST_BASE));
    EXPECT_FALSE(test.dispatchVoid(nullptr));
}

TEST(VendorFunctions, dispatchInts) {
    static int32_t id = RIL_REQUEST_SEND_DEVICE_STATE;
    static std::vector<int32_t> params = { RIL_DeviceStateType::RIL_DST_POWER_SAVE_MODE, true, };
    struct OnRequestListenerImpl : public OnRequestListener {
        TestChecker checker;
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            checker.checked();
            EXPECT_TRUE(request == id);
            EXPECT_TRUE(datalen == sizeof(int32_t) * params.size());
            for (size_t i = 0; i < params.size(); i++) {
                EXPECT_TRUE(((int *)data)[i] == params[i]);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    VendorFunctions test = setupVendorFunctions(&callback);
    EXPECT_TRUE(test.dispatchInts(INTrand(true), id, params));
}

TEST(VendorFunctions, dispatchInts_invalid) {
    VendorFunctions test = setupVendorFunctions();
    EXPECT_FALSE(test.dispatchInts(INTrand(true), RIL_OEM_REQUEST_BASE, {1, 2, 3}));
    EXPECT_FALSE(test.dispatchInts(nullptr, {1, 2, 3}));
}

TEST(VendorFunctions, dispatchString) {
    static int32_t id = RIL_REQUEST_SET_SMSC_ADDRESS;
    static std::string params = "009110132123458";
    struct OnRequestListenerImpl : public OnRequestListener {
        TestChecker checker;
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            checker.checked();
            EXPECT_TRUE(request == id);
            EXPECT_TRUE(datalen == params.size());
            EXPECT_TRUE(std::string((const char *)data) == params);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    VendorFunctions test = setupVendorFunctions(&callback);
    EXPECT_TRUE(test.dispatchString(INTrand(true), id, params));
}

TEST(VendorFunctions, dispatchString_invalid) {
    VendorFunctions test = setupVendorFunctions();
    EXPECT_FALSE(test.dispatchString(INTrand(true), RIL_OEM_REQUEST_BASE, "12345"));
    EXPECT_FALSE(test.dispatchString(nullptr, "12345"));
}

TEST(VendorFunctions, dispatchStrings) {
    static int32_t id = RIL_REQUEST_ENTER_SIM_PIN;
    static std::vector<std::string> params = {"3456", "123456789"};
    struct OnRequestListenerImpl : public OnRequestListener {
        TestChecker checker;
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            checker.checked();
            EXPECT_TRUE(request == id);
            EXPECT_TRUE(datalen == sizeof(char *) * params.size());
            for (size_t i = 0; i < params.size(); i++) {
                EXPECT_TRUE(std::string(((const char **)data)[i]) == params[i]);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    VendorFunctions test = setupVendorFunctions(&callback);
    EXPECT_TRUE(test.dispatchStrings(INTrand(true), id, params));
}

TEST(VendorFunctions, dispatchStrings_invalid) {
    VendorFunctions test = setupVendorFunctions();
    EXPECT_FALSE(test.dispatchStrings(INTrand(true), RIL_OEM_REQUEST_BASE, {"1", "2", "3"}));
    EXPECT_FALSE(test.dispatchStrings(nullptr, {"1", "2", "3"}));
}

TEST(VendorFunctions, dispatchRaw) {
    static int32_t id = RIL_REQUEST_ENTER_SIM_PIN;
    static std::vector<uint8_t> params = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    struct OnRequestListenerImpl : public OnRequestListener {
        TestChecker checker;
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            checker.checked();
            EXPECT_TRUE(request == id);
            EXPECT_TRUE(datalen == params.size());
            std::vector<uint8_t> rawBytes(datalen);
            memcpy(rawBytes.data(), data, datalen);
            EXPECT_TRUE(rawBytes == params);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    VendorFunctions test = setupVendorFunctions(&callback);
    EXPECT_TRUE(test.dispatchRaw(INTrand(true), id, params));
}

TEST(VendorFunctions, dispatchRaw_invalid) {
    VendorFunctions test = setupVendorFunctions();
    EXPECT_FALSE(test.dispatchRaw(INTrand(true), RIL_OEM_REQUEST_BASE, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
    EXPECT_FALSE(test.dispatchRaw(nullptr, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
}