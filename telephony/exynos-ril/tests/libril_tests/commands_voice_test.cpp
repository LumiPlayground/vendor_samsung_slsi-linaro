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
#include <gtest/gtest.h>
// libril
#include <radiocallbackadapter.h>
#include <aidl-service/voice/commands.h>
#include <aidl-service/voice/vendorstructs.h>
#include <vendorfunctions.h>

#include <slsi/radio_v2_0.h>
#include "commands_test_base.h"

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::voice;
using libriltest::GetRandomSerialNumber;
using libriltest::commands::setupCommand;
using libriltest::commands::setupIndCommand;
using libriltest::commands::setupVendorFunctions;

TEST(VoiceCommands, IsVoNrEnabledCommand_onRequest) {
    static int request = ENCODE_REQUEST(RIL_REQUEST_IS_VONR_ENABLED, HAL_VERSION_CODE(2, 0));
    static bool testDone = false;
    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            EXPECT_EQ(request, request);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, (size_t)0);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioVoiceResponseCallback> cb = std::make_shared<RadioVoiceResponseCallbackAdapter>();
    auto test = setupCommand<IsVoNrEnabledCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber());
    EXPECT_TRUE(testDone);
}

TEST(VoiceCommands, IsVoNrEnabledCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    static bool testDone = false;
    struct CallbackImpl : public RadioVoiceResponseCallbackAdapter {
        void isVoNrEnabledResponse(const aidl::RadioResponseInfo& info, bool enable) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE ||
                        info.error == aidl::RadioError::INVALID_RESPONSE);
            if (info.error == aidl::RadioError::NONE) {
                EXPECT_TRUE(enable == true || enable == false);
            }
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioVoiceResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_REQUEST_IS_VONR_ENABLED, HAL_VERSION_CODE(2, 0));
    auto test = setupCommand<IsVoNrEnabledCommand>(func, cb, request);

    int slotId = (int)func.mSlotId;
    int32_t serial = GetRandomSerialNumber();

    bool ret = true;
    // invalid response
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS,
                    nullptr, 0);
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS,
                    nullptr, sizeof(bool));
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS,
                    &ret, 0);
    // success
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS,
                    &ret, sizeof(bool));
    ret = false;
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS,
                    &ret, sizeof(bool));
    EXPECT_TRUE(testDone);
}

TEST(VoiceCommand, setVoNrEnabledCommand_onRequest) {
    static int request = ENCODE_REQUEST(RIL_REQUEST_ENABLE_VONR, HAL_VERSION_CODE(2, 0));
    static bool testDone = false;
    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t /*datalen*/, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            EXPECT_EQ(request, request);
            int enabled = *(int *)data;
            EXPECT_EQ(enabled, 1);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioVoiceResponseCallback> cb = std::make_shared<RadioVoiceResponseCallbackAdapter>();
    auto test = setupCommand<SetVoNrEnabledCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber(), true);
    EXPECT_TRUE(testDone);
}

TEST(VoiceCommand, setVoNrEnabledCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    static bool testDone = false;
    struct CallbackImpl : public RadioVoiceResponseCallbackAdapter {
        void setVoNrEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE ||
                    info.error == aidl::RadioError::INVALID_RESPONSE);
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioVoiceResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_REQUEST_ENABLE_VONR, HAL_VERSION_CODE(2, 0));
    auto test = setupCommand<SetVoNrEnabledCommand>(func, cb, request);

    // success
    int slotId = (int)func.mSlotId;
    int32_t serial = GetRandomSerialNumber();

    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS, nullptr, 0);
    EXPECT_TRUE(testDone);
}

