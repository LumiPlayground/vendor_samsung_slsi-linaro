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
#include <aidl-service/modem/commands.h>
#include <aidl-service/modem/vendorstructs.h>
#include <vendorfunctions.h>

#include <slsi/radio_v2_0.h>
#include <testutils.h>
#include "commands_test_base.h"

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::modem;
using libriltest::GetRandomSerialNumber;
using libriltest::commands::setupCommand;
using libriltest::commands::setupVendorFunctions;

TEST(ModemCommands, ModemActivityInfoCommand_onRequest) {
    static int request = ENCODE_REQUEST(RIL_REQUEST_GET_ACTIVITY_INFO, HAL_VERSION_CODE(2, 0));
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
    std::shared_ptr<RadioModemResponseCallback> cb = std::make_shared<RadioModemResponseCallbackAdapter>();
    auto test = setupCommand<ModemActivityInfoCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber());
    EXPECT_TRUE(testDone);
}

TEST(ModemCommands, ModemActivityInfoCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    static V2_0::RIL_ActivityStatsInfo testVector = {
        10, // sleepModeTimeMs
        20, // idleModeTimeMs
        // techSpecificInfo
        {
            // GERAN
            { RIL_RAN_GERAN, FREQUENCY_RANGE_UNKNOWN, { 1, 2, 3, 4, 5 }, 1 },
            // UTRAN
            { RIL_RAN_UTRAN, FREQUENCY_RANGE_UNKNOWN, { 1, 2, 3, 4, 5 }, 2 },
            // EUTRAN
            { RIL_RAN_EUTRAN, FREQUENCY_RANGE_UNKNOWN, { 1, 2, 3, 4, 5 }, 3 },
            // NGRAN - sub6
            { (RIL_RadioAccessNetworks)RIL_RAN_NGRAN, FREQUENCY_RANGE_HIGH, { 1, 2, 3, 4, 5 }, 4 },
            // NGRAN - mmWave
            { (RIL_RadioAccessNetworks)RIL_RAN_NGRAN, FREQUENCY_RANGE_MMWAVE, { 1, 2, 3, 4, 5 }, 5 },
        },
    };
    static bool testDone = false;
    struct CallbackImpl : public RadioModemResponseCallbackAdapter {
        void getModemActivityInfoResponse(const aidl::RadioResponseInfo& info,
                                        const aidl::modem::ActivityStatsInfo& activityInfo) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE ||
                        info.error == aidl::RadioError::INVALID_RESPONSE);
            if (info.error == aidl::RadioError::NONE) {
                EXPECT_TRUE(activityInfo == toAidl(testVector));
            }
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioModemResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_REQUEST_GET_ACTIVITY_INFO, HAL_VERSION_CODE(2, 0));
    auto test = setupCommand<ModemActivityInfoCommand>(func, cb, request);

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
