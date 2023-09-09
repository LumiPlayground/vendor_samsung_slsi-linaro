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
 * radioconfig_service_test.cpp
 *
 *  Created on: 2021. 7. 20.
 */
#include <android/hardware/radio/config/1.1/IRadioConfig.h>
#include <android/hardware/radio/config/1.2/types.h>
#include <gtest/gtest.h>
#include "ril_service_legacy.h"
#include "ril_service_common.h"
#include "radioconfig_service.h"
#include "ril_service_test.h"
#include <librilutils/textutils.h>

using namespace android::hardware::radio::config;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Void;
using android::sp;

using libriltest::GetRandomSerialNumber;

typedef int (*RadioResponseHandler)(int, int, int, RIL_Errno, void *, size_t);
typedef int (*RadioIndicationHandler)(int, int, int, RIL_Errno, void *, size_t);

TEST(radioconfig_service, radioconfig_response) {
    radioconfig::initRadioConfig();

    RadioResponseHandler testVector[] = {
        radioconfig::getSimSlotsStatusResponse,
        radioconfig::setSimSlotsMappingResponse,
        radioconfig::getPhoneCapabilityResponse,
        radioconfig::setPreferredDataModemResponse,
        radioconfig::setModemsConfigResponse,
        radioconfig::setModemsConfigResponse,
        radioconfig::getSimSlotsStatusResponse_1_2,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioResponseHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_SOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(radioconfig_service, radioconfig_indication) {
    radioconfig::initRadioConfig();

    RadioIndicationHandler testVector[] = {
        radioconfig::simSlotsStatusChanged,
        radioconfig::simSlotsStatusChanged_1_2,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        RadioIndicationHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_UNSOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
extern RIL_RadioFunctions *s_radioconfig_vendorFunctions;
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

sp<V1_1::IRadioConfig> setupRadioConfig(OnRequestListener *listener = nullptr) {
    radioconfig::initRadioConfig();
    sp<V1_0::IRadioConfig> radioConfig = radioconfig::getRadioConfig();
    EXPECT_FALSE(radioConfig.get() == nullptr);
    sp<V1_1::IRadioConfig> radioConfigV11 = V1_1::IRadioConfig::castFrom(radioConfig).withDefault(nullptr);
    EXPECT_FALSE(radioConfigV11.get() == nullptr);

    s_radioconfig_vendorFunctions = &sTestVendorFunctions;
    sVendorFunctionExcutor.setOnRequestListener(listener);

    return radioConfigV11;
}

void tearDown(sp<V1_1::IRadioConfig>& radioConfig) {
    radioConfig = nullptr;
    sVendorFunctionExcutor.setOnRequestListener(nullptr);
}

TEST(radioconfig_service, getSimSlotsStatus) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadioConfig::getSimSlotsStatus");
            EXPECT_EQ(request, RIL_REQUEST_GET_SLOT_STATUS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IRadioConfig> radioConfig = setupRadioConfig(&callback);
    radioConfig->getSimSlotsStatus(serial);
    tearDown(radioConfig);
}

TEST(radioconfig_service, setSimSlotsMapping) {
    static hidl_vec<uint32_t> slotMap = { 0, 1 };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadioConfig::setSimSlotsMapping");
            EXPECT_EQ(request, RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(uint32_t) * slotMap.size());
            for (size_t i = 0; i < slotMap.size(); i++) {
                EXPECT_EQ(slotMap[i], ((unsigned /* NEED TO CHECK */ int *)data)[i]);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IRadioConfig> radioConfig = setupRadioConfig(&callback);
    radioConfig->setSimSlotsMapping(serial, slotMap);
    tearDown(radioConfig);
}

TEST(radioconfig_service, getPhoneCapability) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IRadioConfig::getPhoneCapability");
            EXPECT_EQ(request, RIL_REQUEST_GET_PHONE_CAPABILITY);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IRadioConfig> radioConfig = setupRadioConfig(&callback);
    radioConfig->getPhoneCapability(serial);
    tearDown(radioConfig);
}

TEST(radioconfig_service, setPreferredDataModem) {
    static uint8_t modemId = 0;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IRadioConfig::setPreferredDataModem");
            EXPECT_EQ(request, RIL_REQUEST_SET_PREFERRED_DATA_MODEM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(modemId, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IRadioConfig> radioConfig = setupRadioConfig(&callback);
    radioConfig->setPreferredDataModem(serial, modemId);
    tearDown(radioConfig);
}

TEST(radioconfig_service, setModemsConfig) {
    static V1_1::ModemsConfig modemsConfig = { 2 };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IRadioConfig::setModemsConfig");
            EXPECT_EQ(request, RIL_REQUEST_SET_MODEMS_CONFIG);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(modemsConfig.numOfLiveModems, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IRadioConfig> radioConfig = setupRadioConfig(&callback);
    radioConfig->setModemsConfig(serial, modemsConfig);
    tearDown(radioConfig);
}

TEST(radioconfig_service, getModemsConfig) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IRadioConfig::getModemsConfig");
            EXPECT_EQ(request, RIL_REQUEST_GET_MODEMS_CONFIG);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IRadioConfig> radioConfig = setupRadioConfig(&callback);
    radioConfig->getModemsConfig(serial);
    tearDown(radioConfig);
}
