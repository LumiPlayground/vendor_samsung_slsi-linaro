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
#include <aidl-service/sim/commands.h>
#include <aidl-service/sim/vendorstructs.h>
#include <vendorfunctions.h>

#include <slsi/radio_v2_0.h>
#include <testutils.h>
#include "commands_test_base.h"

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::sim;
using libriltest::GetRandomSerialNumber;
using libriltest::commands::setupCommand;
using libriltest::commands::setupVendorFunctions;

TEST(SimCommands, GetIccCardStatusCommand_onRequest) {
    static int request = ENCODE_REQUEST(RIL_REQUEST_GET_SIM_STATUS, HAL_VERSION_CODE(2, 0));
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
    std::shared_ptr<RadioSimResponseCallback> cb = std::make_shared<RadioSimResponseCallbackAdapter>();
    auto test = setupCommand<GetIccCardStatusCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber());
    EXPECT_TRUE(testDone);
}

TEST(SimCommands, GetIccCardStatusCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    static V2_0::RIL_CardStatus testVector = {
        {
            .card_state = RIL_CARDSTATE_PRESENT,
            .universal_pin_state = RIL_PINSTATE_DISABLED,
            .gsm_umts_subscription_app_index = 0,
            .cdma_subscription_app_index = -1,
            .ims_subscription_app_index = -1,
            .num_applications = 2,
            .applications = {
                {
                    RIL_APPTYPE_USIM,
                    RIL_APPSTATE_READY,
                    RIL_PERSOSUBSTATE_UNKNOWN,
                    (char*)"aid1",
                    (char*)"app_label1",
                    0,
                    RIL_PINSTATE_DISABLED,
                    RIL_PINSTATE_DISABLED,
                },
                {
                    RIL_APPTYPE_CSIM,
                    RIL_APPSTATE_READY,
                    RIL_PERSOSUBSTATE_UNKNOWN,
                    (char*)"aid2",
                    (char*)"app_label2",
                    0,
                    RIL_PINSTATE_DISABLED,
                    RIL_PINSTATE_DISABLED,
                }
            },
            .physicalSlotId = 0,
            .atr = (char*)"atr",
            .iccid = (char*)"iccid",
            .eid = (char*)"eid",
        },
        .portId = 0
    };

    static bool testDone = false;
    struct CallbackImpl : public RadioSimResponseCallbackAdapter {
        void getIccCardStatusResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::CardStatus& cardStatus) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE ||
                        info.error == aidl::RadioError::INVALID_RESPONSE);
            if (info.error == aidl::RadioError::NONE) {
                EXPECT_TRUE(cardStatus == toAidl(testVector));
            }
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioSimResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_REQUEST_GET_SIM_STATUS, HAL_VERSION_CODE(2, 0));
    auto test = setupCommand<GetIccCardStatusCommand>(func, cb, request);

    int slotId = (int)func.mSlotId;
    int32_t serial = GetRandomSerialNumber();

    // invalid response
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS,
                    nullptr, 0);
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS,
                    nullptr, sizeof(testVector));
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS,
                    &testVector, 0);
    // success
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS,
                    &testVector, sizeof(testVector));
    EXPECT_TRUE(testDone);
}
