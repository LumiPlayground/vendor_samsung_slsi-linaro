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
#include <librilutils/osalutils.h>
#include <radiocallbackadapter.h>
#include <aidl-service/network/commands.h>
#include <aidl-service/network/vendorstructs.h>
#include <vendorfunctions.h>

#include <slsi/radio_v2_0.h>
#include <slsi/ril_oem.h>
#include "commands_test_base.h"

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::network;
using libriltest::GetRandomSerialNumber;
using libriltest::commands::setupCommand;
using libriltest::commands::setupIndCommand;
using libriltest::commands::setupVendorFunctions;

TEST(NetworkCommand, NitzTimeReceivedInd_onIndication) {
    static V2_0::RIL_NitzTime testVector = {
        "some nitz-formatted string",
        100,                                        // age, msec
        OsalUtils::elapsedRealtime() - 150000000L,  // timestamp, nsec, current - 150ms
    };
    static bool testDone = false;
    struct CallbackImpl : public RadioNetworkIndicationCallbackAdapter {
        void nitzTimeReceived(::aidl::android::hardware::radio::RadioIndicationType /* type */,
               const std::string& nitzTime, long receivedTimeMs, long ageMs) {
            testDone = true;
            EXPECT_TRUE(nitzTime == testVector.nitzTime);
            EXPECT_TRUE(ageMs = testVector.age + (receivedTimeMs - testVector.timestamp / 1000000));
        }
    };
    int slotId = 0;
    std::shared_ptr<RadioNetworkIndicationCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_UNSOL_NITZ_TIME_RECEIVED, HAL_VERSION_CODE(2, 0));
    auto test = setupIndCommand<NitzTimeReceivedInd>(slotId, cb, request);

    // invalid response
    test->onIndication(slotId, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS,
                    nullptr, 0);
    test->onIndication(slotId, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS,
                    nullptr, sizeof(testVector));
    test->onIndication(slotId, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS,
                    &testVector, 0);
    // normal
    test->onIndication(slotId, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS,
                    &testVector, sizeof(testVector));
    EXPECT_TRUE(testDone);
}

TEST(NetworkCommand, SetSignalStrengthReportingCriteriaCommand_onRequest) {
    namespace aidl = ::aidl::android::hardware::radio;
    static int request = RIL_REQUEST_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA;
    static std::vector<aidl::network::SignalThresholdInfo> testVector {
        { 0, 1, 2, { 1, 2, 3 }, true, (aidl::AccessNetwork) 1 },
        { 3, 4, 5, { 2, 2 }, false, (aidl::AccessNetwork) 0 },
        { 1, 1, 1, { 5, 5, 5 }, true, (aidl::AccessNetwork) 2 },
    };

    static bool testDone = false;
    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            EXPECT_EQ(request, request);
            V2_0::RIL_SignalThresholdInfo *infoPtrs = (V2_0::RIL_SignalThresholdInfo *)data;
            size_t length = datalen / sizeof(V2_0::RIL_SignalThresholdInfo);
            for (size_t i = 0; i < length; i++) {
                V2_0::RIL_SignalThresholdInfo info = infoPtrs[i];
                EXPECT_EQ(info.signalMeasurement, testVector[i].signalMeasurement);
                EXPECT_EQ(info.hysteresisMs, testVector[i].hysteresisMs);
                EXPECT_EQ(info.hysteresisDb, testVector[i].hysteresisDb);
                EXPECT_EQ(info.isEnabled, testVector[i].isEnabled);
                EXPECT_EQ(info.ran, (int32_t) testVector[i].ran);
                for (size_t j = 0; j < info.thresholds.size(); j++) {
                    EXPECT_EQ(info.thresholds[j], testVector[i].thresholds[j]);
                }
            }
            EXPECT_EQ(length, testVector.size());
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioNetworkResponseCallback> cb = std::make_shared<RadioNetworkResponseCallbackAdapter>();
    auto test = setupCommand<SetSignalStrengthReportingCriteriaCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber(), testVector);
    EXPECT_TRUE(testDone);
}

TEST(NetworkCommand, SetSignalStrengthReportingCriteriaCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    static bool testDone = false;
    struct CallbackImpl : public RadioNetworkResponseCallbackAdapter {
        void setSignalStrengthReportingCriteriaResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE ||
                        info.error == aidl::RadioError::INVALID_RESPONSE);
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioNetworkResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = RIL_REQUEST_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA;
    auto test = setupCommand<SetSignalStrengthReportingCriteriaCommand>(func, cb, request);

    // success
    int slotId = (int)func.mSlotId;
    int32_t serial = GetRandomSerialNumber();

    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS, nullptr, 0);
    EXPECT_TRUE(testDone);
}

TEST(NetworkCommand, SetUsageSettingCommand_onRequest) {
    namespace aidl = ::aidl::android::hardware::radio;
    static int request = RIL_REQUEST_SET_USAGE_SETTING;
    static aidl::network::UsageSetting testVector = (aidl::network::UsageSetting) 1;

    static bool testDone = false;
    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t /* datalen */, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            EXPECT_EQ(request, request);
            aidl::network::UsageSetting setting = *(aidl::network::UsageSetting *)data;
            EXPECT_EQ(setting, testVector);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioNetworkResponseCallback> cb = std::make_shared<RadioNetworkResponseCallbackAdapter>();
    auto test = setupCommand<SetUsageSettingCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber(), testVector);
    EXPECT_TRUE(testDone);
}

TEST(NetworkCommand, SetUsageSettingCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    static bool testDone = false;
    struct CallbackImpl : public RadioNetworkResponseCallbackAdapter {
        void setUsageSettingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE ||
                        info.error == aidl::RadioError::INVALID_RESPONSE);
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioNetworkResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = RIL_REQUEST_SET_USAGE_SETTING;
    auto test = setupCommand<SetUsageSettingCommand>(func, cb, request);

    // success
    int slotId = (int)func.mSlotId;
    int32_t serial = GetRandomSerialNumber();

    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS, nullptr, 0);
    EXPECT_TRUE(testDone);
}

TEST(NetworkCommand, GetUsageSettingCommand_onRequest) {
    static int request = RIL_REQUEST_GET_USAGE_SETTING;
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
    std::shared_ptr<RadioNetworkResponseCallback> cb = std::make_shared<RadioNetworkResponseCallbackAdapter>();
    auto test = setupCommand<GetUsageSettingCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber());
    EXPECT_TRUE(testDone);
}

TEST(NetworkCommand, GetUsageSettingCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    static aidl::network::UsageSetting testVector = (aidl::network::UsageSetting) 1;
    static bool testDone = false;
    struct CallbackImpl : public RadioNetworkResponseCallbackAdapter {
        void getUsageSettingResponse(const aidl::RadioResponseInfo& info,
                                        const aidl::network::UsageSetting usageSetting) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE ||
                        info.error == aidl::RadioError::INVALID_RESPONSE);
            if (info.error == aidl::RadioError::NONE) {
                EXPECT_EQ(usageSetting, testVector);
            }
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioNetworkResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = RIL_REQUEST_GET_USAGE_SETTING;
    auto test = setupCommand<GetUsageSettingCommand>(func, cb, request);

    // success
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
