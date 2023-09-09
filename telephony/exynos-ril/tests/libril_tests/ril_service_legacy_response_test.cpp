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
 * ril_service_legacy_response_test.cpp
 *
 *  Created on: 2021. 7. 22.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "ril_service_legacy.h"
#include "ril_service_common.h"
#include "ril_service_test.h"
#include <librilutils/radio_conv_utils.h>
#include "test_radio_hidl_utils_1_0.h"
#include "test_radio_hidl_utils_1_1.h"
#include "test_radio_hidl_utils_1_2.h"
#include "test_radio_hidl_utils_1_4.h"
#include "test_radio_hidl_utils_1_5.h"

using namespace android::hardware::radio;
using android::hardware::Status;

using libriltest::GetRandomSerialNumber;

typedef int (*RadioResponseHandler)(int, int, int, RIL_Errno, void *, size_t);

TEST(ril_service_legacy, radio_1_0_response) {
    radio::impl::legacy::initRadios();

    RadioResponseHandler testVector[] = {
        radio::getIccCardStatusResponse,
        radio::supplyIccPinForAppResponse,
        radio::supplyIccPukForAppResponse,
        radio::supplyIccPin2ForAppResponse,
        radio::supplyIccPuk2ForAppResponse,
        radio::changeIccPinForAppResponse,
        radio::changeIccPin2ForAppResponse,
        radio::supplyNetworkDepersonalizationResponse,
        radio::getCurrentCallsResponse,
        radio::dialResponse,
        radio::getIMSIForAppResponse,
        radio::hangupConnectionResponse,
        radio::hangupWaitingOrBackgroundResponse,
        radio::hangupForegroundResumeBackgroundResponse,
        radio::switchWaitingOrHoldingAndActiveResponse,
        radio::conferenceResponse,
        radio::rejectCallResponse,
        radio::getLastCallFailCauseResponse,
        radio::getSignalStrengthResponse,
        radio::getVoiceRegistrationStateResponse,
        radio::getDataRegistrationStateResponse,
        radio::getOperatorResponse,
        radio::setRadioPowerResponse,
        radio::sendDtmfResponse,
        radio::sendSmsResponse,
        radio::sendSMSExpectMoreResponse,
        radio::setupDataCallResponse,
        radio::iccIOForAppResponse,
        radio::sendUssdResponse,
        radio::cancelPendingUssdResponse,
        radio::getClirResponse,
        radio::setClirResponse,
        radio::getCallForwardStatusResponse,
        radio::setCallForwardResponse,
        radio::getCallWaitingResponse,
        radio::setCallWaitingResponse,
        radio::acknowledgeLastIncomingGsmSmsResponse,
        radio::acceptCallResponse,
        radio::deactivateDataCallResponse,
        radio::getFacilityLockForAppResponse,
        radio::setFacilityLockForAppResponse,
        radio::setBarringPasswordResponse,
        radio::getNetworkSelectionModeResponse,
        radio::setNetworkSelectionModeAutomaticResponse,
        radio::setNetworkSelectionModeManualResponse,
        radio::getAvailableNetworksResponse,
        radio::startNetworkScanResponse,
        radio::stopNetworkScanResponse,
        radio::startDtmfResponse,
        radio::stopDtmfResponse,
        radio::getBasebandVersionResponse,
        radio::separateConnectionResponse,
        radio::setMuteResponse,
        radio::getMuteResponse,
        radio::getClipResponse,
        radio::getDataCallListResponse,
        radio::setSuppServiceNotificationsResponse,
        radio::writeSmsToSimResponse,
        radio::deleteSmsOnSimResponse,
        radio::setBandModeResponse,
        radio::getAvailableBandModesResponse,
        radio::sendEnvelopeResponse,
        radio::sendTerminalResponseToSimResponse,
        radio::handleStkCallSetupRequestFromSimResponse,
        radio::explicitCallTransferResponse,
        radio::setPreferredNetworkTypeResponse,
        radio::getPreferredNetworkTypeResponse,
        radio::getNeighboringCidsResponse,
        radio::setLocationUpdatesResponse,
        radio::setCdmaSubscriptionSourceResponse,
        radio::setCdmaRoamingPreferenceResponse,
        radio::getCdmaRoamingPreferenceResponse,
        radio::setTTYModeResponse,
        radio::getTTYModeResponse,
        radio::setPreferredVoicePrivacyResponse,
        radio::getPreferredVoicePrivacyResponse,
        radio::sendCDMAFeatureCodeResponse,
        radio::sendBurstDtmfResponse,
        radio::sendCdmaSmsResponse,
        radio::acknowledgeLastIncomingCdmaSmsResponse,
        radio::getGsmBroadcastConfigResponse,
        radio::setGsmBroadcastConfigResponse,
        radio::setGsmBroadcastActivationResponse,
        radio::getCdmaBroadcastConfigResponse,
        radio::setCdmaBroadcastConfigResponse,
        radio::setCdmaBroadcastActivationResponse,
        radio::getCDMASubscriptionResponse,
        radio::writeSmsToRuimResponse,
        radio::deleteSmsOnRuimResponse,
        radio::getDeviceIdentityResponse,
        radio::exitEmergencyCallbackModeResponse,
        radio::getSmscAddressResponse,
        radio::setCdmaBroadcastActivationResponse,
        radio::setSmscAddressResponse,
        radio::reportSmsMemoryStatusResponse,
        radio::reportStkServiceIsRunningResponse,
        radio::getCdmaSubscriptionSourceResponse,
        radio::requestIsimAuthenticationResponse,
        radio::acknowledgeIncomingGsmSmsWithPduResponse,
        radio::sendEnvelopeWithStatusResponse,
        radio::getVoiceRadioTechnologyResponse,
        radio::getCellInfoListResponse,
        radio::setCellInfoListRateResponse,
        radio::setInitialAttachApnResponse,
        radio::getImsRegistrationStateResponse,
        radio::sendImsSmsResponse,
        radio::iccTransmitApduBasicChannelResponse,
        radio::iccOpenLogicalChannelResponse,
        radio::iccCloseLogicalChannelResponse,
        radio::iccTransmitApduLogicalChannelResponse,
        radio::nvReadItemResponse,
        radio::nvWriteItemResponse,
        radio::nvWriteCdmaPrlResponse,
        radio::nvResetConfigResponse,
        radio::setUiccSubscriptionResponse,
        radio::setDataAllowedResponse,
        radio::getHardwareConfigResponse,
        radio::requestIccSimAuthenticationResponse,
        radio::setDataProfileResponse,
        radio::requestShutdownResponse,
        radio::getRadioCapabilityResponse,
        radio::setRadioCapabilityResponse,
        radio::startLceServiceResponse,
        radio::stopLceServiceResponse,
        radio::pullLceDataResponse,
        radio::getModemActivityInfoResponse,
        radio::setAllowedCarriersResponse,
        radio::getAllowedCarriersResponse,
        radio::sendDeviceStateResponse,
        radio::setIndicationFilterResponse,
        radio::setSimCardPowerResponse,
        radio::startKeepaliveResponse,
        radio::stopKeepaliveResponse,
        radio::setSignalStrengthReportingCriteriaResponse,
        radio::setLinkCapacityReportingCriteriaResponse,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioResponseHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_SOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_service_legacy, radio_1_2_response) {
    radio::impl::legacy::initRadios();

    RadioResponseHandler testVector[] = {
        radio::getIccCardStatusResponse_1_2,
        radio::getCurrentCallsResponse_1_2,
        radio::getVoiceRegistrationStateResponse_1_2,
        radio::getDataRegistrationStateResponse_1_2,
        radio::getSignalStrengthResponse_1_2,
        radio::getCellInfoListResponse_1_2,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioResponseHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_SOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_service_legacy, radio_1_3_response) {
    radio::impl::legacy::initRadios();

    RadioResponseHandler testVector[] = {
        radio::setSystemSelectionChannelsResponse,
        radio::enableModemResponse,
        radio::getModemStackStatusResponse,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioResponseHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_SOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_service_legacy, radio_1_4_response) {
    radio::impl::legacy::initRadios();

    RadioResponseHandler testVector[] = {
        radio::getIccCardStatusResponse_1_4,
        radio::getDataRegistrationStateResponse_1_4,
        radio::getSignalStrengthResponse_1_4,
        radio::setupDataCallResponse_1_4,
        radio::getDataCallListResponse_1_4,
        radio::setPreferredNetworkTypeBitmapResponse,
        radio::getPreferredNetworkTypeBitmapResponse,
        radio::getCellInfoListResponse_1_4,
        radio::setAllowedCarriersResponse_1_4,
        radio::getAllowedCarriersResponse_1_4,
        radio::startNetworkScanResponse_1_4,
        radio::emergencyDialResponse,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioResponseHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_SOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_service_legacy, radio_1_5_response) {
    radio::impl::legacy::initRadios();

    RadioResponseHandler testVector[] = {
        radio::setSignalStrengthReportingCriteriaResponse_1_5,
        radio::enableUiccApplicationsResponse,
        radio::areUiccApplicationsEnabledResponse,
        radio::setSystemSelectionChannelsResponse_1_5,
        radio::startNetworkScanResponse_1_5,
        radio::setupDataCallResponse_1_5,
        radio::setInitialAttachApnResponse_1_5,
        radio::setDataProfileResponse_1_5,
        radio::setRadioPowerResponse_1_5,
        radio::setIndicationFilterResponse_1_5,
        radio::getBarringInfoResponse,
        radio::getVoiceRegistrationStateResponse_1_5,
        radio::getDataRegistrationStateResponse_1_5,
        radio::setNetworkSelectionModeManualResponse_1_5,
        radio::sendCdmaSmsExpectMoreResponse,
        radio::supplySimDepersonalizationResponse,
        radio::setLinkCapacityReportingCriteriaResponse_1_5,
        radio::getCellInfoListResponse_1_5,
        radio::getDataCallListResponse_1_5,
        radio::getIccCardStatusResponse_1_5,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioResponseHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_SOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

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

static sp<V1_5::IRadio> setupRadio(OnRequestListener *listener = nullptr) {
    radio::impl::legacy::initRadios();
    sp<V1_0::IRadio> radio = radio::impl::legacy::getRadio(RIL_SOCKET_1);
    EXPECT_FALSE(radio.get() == nullptr);
    sp<V1_5::IRadio> radioV15 = V1_5::IRadio::castFrom(radio).withDefault(nullptr);
    EXPECT_FALSE(radioV15.get() == nullptr);

    s_vendorFunctions = &sTestVendorFunctions;
    sVendorFunctionExcutor.setOnRequestListener(listener);

    return radioV15;
}

static void tearDown(sp<V1_5::IRadio>& radio) {
    radio->setResponseFunctions(nullptr, nullptr);
    radio = nullptr;
    sVendorFunctionExcutor.setOnRequestListener(nullptr);
}

TEST(ril_service_legacy_rsp_tests, getIccCardStatusResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getIccCardStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CardStatus_v6 testVector = {
        RIL_CardState::RIL_CARDSTATE_PRESENT,
        RIL_PinState::RIL_PINSTATE_ENABLED_VERIFIED,
        0, -1, -1, 1,
        {
            {
                RIL_AppType::RIL_APPTYPE_USIM,
                RIL_AppState::RIL_APPSTATE_READY,
                RIL_PersoSubstate::RIL_PERSOSUBSTATE_UNKNOWN,
                (char *)"aid1234",
                (char *)"",
                0,
                RIL_PinState::RIL_PINSTATE_DISABLED,
                RIL_PinState::RIL_PINSTATE_DISABLED
            }
        }
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getIccCardStatusResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_0::CardStatus& cardStatus) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.card_state, (int)cardStatus.cardState);
                EXPECT_EQ(testVector.universal_pin_state, (int)cardStatus.universalPinState);
                EXPECT_EQ(testVector.gsm_umts_subscription_app_index, cardStatus.gsmUmtsSubscriptionAppIndex);
                EXPECT_EQ(testVector.cdma_subscription_app_index, cardStatus.cdmaSubscriptionAppIndex);
                EXPECT_EQ(testVector.ims_subscription_app_index, cardStatus.imsSubscriptionAppIndex);
                if (cardStatus.applications.size() > 0) {
                    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.num_applications, cardStatus.applications.size());
                    for (size_t i = 0; i < cardStatus.applications.size(); i++) {
                        test(testVector.applications[i], cardStatus.applications[i]);
                    }
                }
            }
            return Void();
        }

        void test(const RIL_AppStatus& t, const V1_0::AppStatus& appStatus) {
            EXPECT_EQ(t.app_type, (int)appStatus.appType);
            EXPECT_EQ(t.app_state, (int)appStatus.appState);
            EXPECT_EQ(t.perso_substate, (int)appStatus.persoSubstate);
            EXPECT_TRUE(t.aid_ptr == appStatus.aidPtr);
            EXPECT_TRUE(t.app_label_ptr == appStatus.appLabelPtr);
            EXPECT_EQ(t.pin1_replaced, appStatus.pin1Replaced);
            EXPECT_EQ(t.pin1, (int)appStatus.pin1);
            EXPECT_EQ(t.pin2, (int)appStatus.pin2);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    RIL_CardStatus_v6 invalidParams = {
        .gsm_umts_subscription_app_index = 1,
        .cdma_subscription_app_index = 2,
        .ims_subscription_app_index = 3,
        .num_applications = 1,
    };
    radio::getIccCardStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &invalidParams, sizeof(invalidParams));

    radio::getIccCardStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, supplyIccPinForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::supplyIccPinForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector = 1;

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> supplyIccPinForAppResponse(const RadioResponseInfo& info,
                int32_t remainingRetries) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector, remainingRetries);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::supplyIccPinForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, supplyIccPukForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::supplyIccPukForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector = 1;

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> supplyIccPukForAppResponse(const RadioResponseInfo& info,
                int32_t remainingRetries) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector, remainingRetries);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::supplyIccPukForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, supplyIccPin2ForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::supplyIccPin2ForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector = 1;

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> supplyIccPin2ForAppResponse(const RadioResponseInfo& info,
                int32_t remainingRetries) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector, remainingRetries);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::supplyIccPin2ForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, supplyIccPuk2ForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::supplyIccPuk2ForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector = 1;

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> supplyIccPuk2ForAppResponse(const RadioResponseInfo& info,
                int32_t remainingRetries) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector, remainingRetries);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::supplyIccPuk2ForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, changeIccPinForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::changeIccPinForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector = 1;

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> changeIccPinForAppResponse(const RadioResponseInfo& info,
                int32_t remainingRetries) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector, remainingRetries);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::changeIccPinForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, changeIccPin2ForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::changeIccPin2ForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector = 1;

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> changeIccPinForAppResponse(const RadioResponseInfo& info,
                int32_t remainingRetries) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector, remainingRetries);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::changeIccPin2ForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, supplyNetworkDepersonalizationResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::supplyNetworkDepersonalizationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector = 1;

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> supplyNetworkDepersonalizationResponse(const RadioResponseInfo& info,
                int32_t remainingRetries) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector, remainingRetries);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::supplyNetworkDepersonalizationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCurrentCallsResponse) {
    // TODO
}

TEST(ril_service_legacy_rsp_tests, dialResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::dialResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> dialResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::dialResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getIMSIForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getIMSIForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char testVector[] = "00101131234123123";

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getIMSIForAppResponse(const RadioResponseInfo& info,
                const ::android::hardware::hidl_string& imsi) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_TRUE(testVector == imsi);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getIMSIForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, hangupConnectionResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::hangupConnectionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> hangupConnectionResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::hangupConnectionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, hangupWaitingOrBackgroundResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::hangupWaitingOrBackgroundResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> hangupWaitingOrBackgroundResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::hangupWaitingOrBackgroundResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, hangupForegroundResumeBackgroundResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::hangupForegroundResumeBackgroundResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> hangupForegroundResumeBackgroundResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::hangupForegroundResumeBackgroundResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, switchWaitingOrHoldingAndActiveResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::switchWaitingOrHoldingAndActiveResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> switchWaitingOrHoldingAndActiveResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::switchWaitingOrHoldingAndActiveResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, conferenceResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::conferenceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> conferenceResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::conferenceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, rejectCallResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::rejectCallResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> rejectCallResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::rejectCallResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getLastCallFailCauseResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getLastCallFailCauseResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_LastCallFailCauseInfo testVector = {
        RIL_LastCallFailCause::CALL_FAIL_CALL_BARRED, (char *)"vendor_error"
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getLastCallFailCauseResponse(const RadioResponseInfo& info,
                const LastCallFailCauseInfo& failCauseInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.cause_code, (int)failCauseInfo.causeCode);
                EXPECT_TRUE((testVector.vendor_cause == failCauseInfo.vendorCause) ||
                        failCauseInfo.vendorCause.empty());
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::getLastCallFailCauseResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
                RIL_E_SUCCESS, 0, 0);
    radio::getLastCallFailCauseResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
                RIL_E_SUCCESS, &testVector, 0);

    // cause only
    radio::getLastCallFailCauseResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector.cause_code, sizeof(testVector.cause_code));

    // cause with vendor cause
    radio::getLastCallFailCauseResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
                RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getSignalStrengthResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSignalStrengthResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SignalStrength_v10 testVector = {
        {}, // GW
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getSignalStrengthResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_0::SignalStrength& signalStrength) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.signalStrength,
                          signalStrength.lte.signalStrength);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.rsrp,
                          signalStrength.lte.rsrp);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.rsrq,
                          signalStrength.lte.rsrq);
                EXPECT_EQ(testVector.LTE_SignalStrength.rssnr,
                        signalStrength.lte.rssnr);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.cqi,
                          signalStrength.lte.cqi);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.timingAdvance,
                          signalStrength.lte.timingAdvance);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getSignalStrengthResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getSignalStrengthResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getVoiceRegistrationStateResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getVoiceRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_VoiceRegistrationStateResponse testVector = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        0, -1, -1, -1, 0,
        {}
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getVoiceRegistrationStateResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_0::VoiceRegStateResult& voiceRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.regState, (int)voiceRegResponse.regState);
                EXPECT_EQ(testVector.rat, voiceRegResponse.rat);
                EXPECT_EQ(testVector.cssSupported, voiceRegResponse.cssSupported);
                EXPECT_EQ(testVector.roamingIndicator, voiceRegResponse.roamingIndicator);
                EXPECT_EQ(testVector.systemIsInPrl, voiceRegResponse.systemIsInPrl);
                EXPECT_EQ(testVector.defaultRoamingIndicator, voiceRegResponse.defaultRoamingIndicator);
                EXPECT_EQ(testVector.reasonForDenial, voiceRegResponse.reasonForDenial);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getVoiceRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getVoiceRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector) + 1);

    radio::getVoiceRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDataRegistrationStateResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_DataRegistrationStateResponse testVector = {
        RIL_RegState::RIL_REG_DENIED,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        7, 4, {}
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getDataRegistrationStateResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_0::DataRegStateResult& dataRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.regState, (int)dataRegResponse.regState);
                EXPECT_EQ(testVector.rat, dataRegResponse.rat);
                EXPECT_EQ(testVector.reasonDataDenied, dataRegResponse.reasonDataDenied);
                EXPECT_EQ(testVector.maxDataCalls, dataRegResponse.maxDataCalls);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getDataRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getDataRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector) + 1);

    radio::getDataRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getOperatorResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getOperatorResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char *testVector[] = {
        (char *)"SK Telecom", (char *)"SKT", (char *)"45005"
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getOperatorResponse(const RadioResponseInfo& info,
                const ::android::hardware::hidl_string& longName,
                const ::android::hardware::hidl_string& shortName,
                const ::android::hardware::hidl_string& numeric) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_TRUE(testVector[0] == longName);
                EXPECT_TRUE(testVector[1] == shortName);
                EXPECT_TRUE(testVector[2] == numeric);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getOperatorResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getOperatorResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setRadioPowerResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setRadioPowerResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setRadioPowerResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setRadioPowerResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendDtmfResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendDtmfResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendDtmfResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendDtmfResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendSmsResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SMS_Response testVector = {
        200, (char *)"", 0
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendSmsResponse(const RadioResponseInfo& info, const SendSmsResult& sms) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.messageRef, sms.messageRef);
            EXPECT_TRUE(testVector.ackPDU == sms.ackPDU);
            EXPECT_EQ(testVector.errorCode, sms.errorCode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendSMSExpectMoreResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendSMSExpectMoreResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SMS_Response testVector = {
        200, (char *)"", 0
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendSMSExpectMoreResponse(const RadioResponseInfo& info, const SendSmsResult& sms) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.messageRef, sms.messageRef);
            EXPECT_TRUE(testVector.ackPDU == sms.ackPDU);
            EXPECT_EQ(testVector.errorCode, sms.errorCode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendSMSExpectMoreResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setupDataCallResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setupDataCallResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_Data_Call_Response_v11 testVector = {
            0, -1, 1, 2, "IP", (char *)"rmnet0", (char *)"192.164.0.2", (char *)"192.164.0.1",
            (char *)"192.164.0.1", (char *)"", 1500
        };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setupDataCallResponse(
                const RadioResponseInfo& info,
                const android::hardware::radio::V1_0::SetupDataCallResult& dcResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            test(testVector, dcResponse);
            return Void();
        }

        void test(const RIL_Data_Call_Response_v11& t, const V1_0::SetupDataCallResult& dcResult) {
            EXPECT_EQ(t.status, (int)dcResult.status);
            EXPECT_EQ(t.suggestedRetryTime, dcResult.suggestedRetryTime);
            EXPECT_EQ(t.cid, dcResult.cid);
            EXPECT_EQ(t.active, dcResult.active);
            EXPECT_TRUE(t.type == dcResult.type);
            EXPECT_TRUE(t.ifname == dcResult.ifname);
            EXPECT_TRUE(t.addresses == dcResult.addresses);
            EXPECT_TRUE(t.dnses == dcResult.dnses);
            EXPECT_TRUE(t.gateways == dcResult.gateways);
            EXPECT_TRUE(t.pcscf == dcResult.pcscf);
            EXPECT_EQ(t.mtu, dcResult.mtu);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setupDataCallResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, iccIOForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::iccIOForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SIM_IO_Response testVector = {
        1, 0, (char *)"0A1B3C4D5E6F",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> iccIOForAppResponse(const RadioResponseInfo& info, const IccIoResult& iccIo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.sw1, iccIo.sw1);
            EXPECT_EQ(testVector.sw2, iccIo.sw2);
            EXPECT_TRUE(testVector.simResponse == iccIo.simResponse);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::iccIOForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendUssdResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendUssdResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendUssdResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendUssdResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, cancelPendingUssdResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::cancelPendingUssdResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> cancelPendingUssdResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::cancelPendingUssdResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getClirResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getClirResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 10, 11 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getClirResponse(const RadioResponseInfo& info, int32_t n, int32_t m) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector[0], n);
                EXPECT_EQ(testVector[1], m);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getClirResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getClirResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setClirResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setClirResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setClirResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setClirResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCallForwardStatusResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCallForwardStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CallForwardInfo *testVector[] = {
        new RIL_CallForwardInfo {
            0, 1, 2, 91, (char *)"821012345678", 10
        },
        new RIL_CallForwardInfo {
            1, 2, 3, 91, (char *)"821077778888", 5
        },
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCallForwardStatusResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<CallForwardInfo>& call_forwardInfos) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(size, call_forwardInfos.size());
                for (size_t i = 0; i < call_forwardInfos.size(); i++) {
                    EXPECT_EQ(testVector[i]->status, (int)call_forwardInfos[i].status);
                    EXPECT_EQ(testVector[i]->reason, call_forwardInfos[i].reason);
                    EXPECT_EQ(testVector[i]->serviceClass, call_forwardInfos[i].serviceClass);
                    EXPECT_EQ(testVector[i]->toa, call_forwardInfos[i].toa);
                    EXPECT_TRUE(testVector[i]->number == call_forwardInfos[i].number);
                    EXPECT_EQ(testVector[i]->timeSeconds, call_forwardInfos[i].timeSeconds);
                }
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getCallForwardStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getCallForwardStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);

    for (size_t i = 0; i < size; i++) {
        delete testVector[i];
    }
}

TEST(ril_service_legacy_rsp_tests, setCallForwardResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setCallForwardResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setCallForwardResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setCallForwardResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCallWaitingResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCallWaitingResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1, 11 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCallWaitingResponse(const RadioResponseInfo& info, bool enable,
                int32_t serviceClass) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector[0], enable);
                EXPECT_EQ(testVector[1], serviceClass);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getCallWaitingResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getCallWaitingResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setCallWaitingResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setCallWaitingResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setCallWaitingResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setCallWaitingResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, acknowledgeLastIncomingGsmSmsResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::acknowledgeLastIncomingGsmSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> acknowledgeLastIncomingGsmSmsResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::acknowledgeLastIncomingGsmSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, acceptCallResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::acceptCallResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> acceptCallResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::acceptCallResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, deactivateDataCallResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::deactivateDataCallResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> deactivateDataCallResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::deactivateDataCallResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getFacilityLockForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getFacilityLockForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 10 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getFacilityLockForAppResponse(const RadioResponseInfo& info, int32_t response) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], response);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getFacilityLockForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setFacilityLockForAppResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setFacilityLockForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 3 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setFacilityLockForAppResponse(const RadioResponseInfo& info, int32_t retry) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], retry);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setFacilityLockForAppResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setBarringPasswordResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setBarringPasswordResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setBarringPasswordResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setBarringPasswordResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getNetworkSelectionModeResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getNetworkSelectionModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getNetworkSelectionModeResponse(const RadioResponseInfo& info, bool manual) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector[0], manual);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getNetworkSelectionModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getNetworkSelectionModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setNetworkSelectionModeAutomaticResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setNetworkSelectionModeAutomaticResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setNetworkSelectionModeAutomaticResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setNetworkSelectionModeAutomaticResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setNetworkSelectionModeManualResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setNetworkSelectionModeManualResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setNetworkSelectionModeManualResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setNetworkSelectionModeManualResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getAvailableNetworksResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getAvailableNetworksResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char *testVector[] = {
        (char *)"SK Telecom", (char *)"SKT", (char *)"45005", (char *)"available",
        (char *)"KT", (char *)"KT", (char *)"45006", (char *)"current",
        (char *)"Test bed", (char *)"Test bed", (char *)"00101", (char *)"forbidden",
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]) / 4;

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getAvailableNetworksResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<OperatorInfo>& networkInfos) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(size, networkInfos.size());
                for (size_t i = 0; i < networkInfos.size(); i++) {
                    EXPECT_TRUE(testVector[i*4] == networkInfos[i].alphaLong);
                    EXPECT_TRUE(testVector[i*4 + 1] == networkInfos[i].alphaShort);
                    EXPECT_TRUE(testVector[i*4 + 2] == networkInfos[i].operatorNumeric);
                }
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getAvailableNetworksResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    char *invalidParams[] = {
         (char *)"SK Telecom", (char *)"SKT", (char *)"45005", (char *)"wrong_status",
    };
    radio::getAvailableNetworksResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, invalidParams, sizeof(invalidParams));

    radio::getAvailableNetworksResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, startDtmfResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::startDtmfResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> startDtmfResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::startDtmfResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, stopDtmfResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::stopDtmfResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> stopDtmfResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::stopDtmfResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getBasebandVersionResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getBasebandVersionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char testVector[] = "BASEBAND_QB123456";

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getBasebandVersionResponse(const RadioResponseInfo& info,
                const ::android::hardware::hidl_string& version) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_TRUE(testVector == version);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getBasebandVersionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, separateConnectionResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::separateConnectionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> separateConnectionResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::separateConnectionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setMuteResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setMuteResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setMuteResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setMuteResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getMuteResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getMuteResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getMuteResponse(const RadioResponseInfo& info, bool enable) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector[0], enable);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getMuteResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getMuteResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getClipResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getClipResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 2 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getClipResponse(const RadioResponseInfo& info, ClipStatus status) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], (int)status);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getClipResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDataCallListResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setupDataCallResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_Data_Call_Response_v11 testVector[] = {
        { 0, -1, 1, 2, "IP", (char *)"rmnet0", (char *)"192.164.0.2", (char *)"192.164.0.1",
                (char *)"192.164.0.1", (char *)"", 1500 },
        { 0, -1, 2, 2, "IP", (char *)"rmnet1", (char *)"192.164.1.2", (char *)"192.164.1.1",
                (char *)"192.164.1.1", (char *)"10.1.2.4", 1500 },
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getDataCallListResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<
                        android::hardware::radio::V1_0::SetupDataCallResult>& dcResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            if (dcResponse.size() > 0) {
                EXPECT_EQ(size, dcResponse.size());
                for (size_t i = 0; i < dcResponse.size(); i++) {
                    test(testVector[i], dcResponse[i]);
                }
            }
            return Void();
        }

        void test(const RIL_Data_Call_Response_v11& t, const V1_0::SetupDataCallResult& dcResult) {
            EXPECT_EQ(t.status, (int)dcResult.status);
            EXPECT_EQ(t.suggestedRetryTime, dcResult.suggestedRetryTime);
            EXPECT_EQ(t.cid, dcResult.cid);
            EXPECT_EQ(t.active, dcResult.active);
            EXPECT_TRUE(t.type == dcResult.type);
            EXPECT_TRUE(t.ifname == dcResult.ifname);
            EXPECT_TRUE(t.addresses == dcResult.addresses);
            EXPECT_TRUE(t.dnses == dcResult.dnses);
            EXPECT_TRUE(t.gateways == dcResult.gateways);
            EXPECT_TRUE(t.pcscf == dcResult.pcscf);
            EXPECT_EQ(t.mtu, dcResult.mtu);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setupDataCallResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setSuppServiceNotificationsResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setSuppServiceNotificationsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setSuppServiceNotificationsResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setSuppServiceNotificationsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, deleteSmsOnSimResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::deleteSmsOnSimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> deleteSmsOnSimResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::deleteSmsOnSimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setBandModeResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setBandModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setBandModeResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setBandModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, writeSmsToSimResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::writeSmsToSimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> writeSmsToSimResponse(const RadioResponseInfo& info, int32_t index) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], index);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::writeSmsToSimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getAvailableBandModesResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getAvailableBandModesResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1, 2, 3, 4, 5 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getAvailableBandModesResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<RadioBandMode>& bandModes) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(sizeof(testVector) / sizeof(testVector[0]), bandModes.size());
                for (size_t i = 0; i < bandModes.size(); i++) {
                    EXPECT_EQ(testVector[i], (int)bandModes[i]);
                }
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getAvailableBandModesResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getAvailableBandModesResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendEnvelopeResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendEnvelopeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char testVector[] = "sendEnvelopeResponse";

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendEnvelopeResponse(const RadioResponseInfo& info,
                const ::android::hardware::hidl_string& commandResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_TRUE(testVector == commandResponse);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendEnvelopeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendTerminalResponseToSimResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendTerminalResponseToSimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendTerminalResponseToSimResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendTerminalResponseToSimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, handleStkCallSetupRequestFromSimResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::handleStkCallSetupRequestFromSimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> handleStkCallSetupRequestFromSimResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::handleStkCallSetupRequestFromSimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, explicitCallTransferResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::explicitCallTransferResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> explicitCallTransferResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::explicitCallTransferResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setPreferredNetworkTypeResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setPreferredNetworkTypeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setPreferredNetworkTypeResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setPreferredNetworkTypeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getPreferredNetworkTypeResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getPreferredNetworkTypeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getPreferredNetworkTypeResponse(const RadioResponseInfo& info,
                PreferredNetworkType nwType) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], (int)nwType);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getPreferredNetworkTypeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getNeighboringCidsResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getNeighboringCidsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_NeighboringCell *testVector[] = {
        new RIL_NeighboringCell {
            (char *)"1", -100
        },
        new RIL_NeighboringCell {
            (char *)"2", -120
        },
        new RIL_NeighboringCell {
            (char *)"3", -140
        },
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getNeighboringCidsResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<NeighboringCell>& cells) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(size, cells.size());
                for (size_t i = 0; i < cells.size(); i++) {
                    EXPECT_TRUE(testVector[i]->cid == cells[i].cid);
                    EXPECT_EQ(testVector[i]->rssi, cells[i].rssi);
                }
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getNeighboringCidsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getNeighboringCidsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);


    for (size_t i = 0; i < size; i++) {
        delete testVector[i];
    }
}

TEST(ril_service_legacy_rsp_tests, setLocationUpdatesResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setLocationUpdatesResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setLocationUpdatesResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setLocationUpdatesResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setCdmaSubscriptionSourceResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setCdmaSubscriptionSourceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setCdmaSubscriptionSourceResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setCdmaSubscriptionSourceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setCdmaRoamingPreferenceResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setCdmaRoamingPreferenceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setCdmaRoamingPreferenceResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setCdmaRoamingPreferenceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCdmaRoamingPreferenceResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCdmaRoamingPreferenceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCdmaRoamingPreferenceResponse(const RadioResponseInfo& info,
                CdmaRoamingType type) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], (int)type);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getCdmaRoamingPreferenceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setTTYModeResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setTTYModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setTTYModeResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setTTYModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getTTYModeResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getTTYModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getTTYModeResponse(const RadioResponseInfo& info, TtyMode mode) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], (int)mode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getTTYModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setPreferredVoicePrivacyResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setPreferredVoicePrivacyResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setPreferredVoicePrivacyResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setPreferredVoicePrivacyResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getPreferredVoicePrivacyResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getPreferredVoicePrivacyResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getPreferredVoicePrivacyResponse(const RadioResponseInfo& info, bool enable) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector[0], enable);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getPreferredVoicePrivacyResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getPreferredVoicePrivacyResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendCDMAFeatureCodeResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendCDMAFeatureCodeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendCDMAFeatureCodeResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendCDMAFeatureCodeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendBurstDtmfResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendBurstDtmfResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendBurstDtmfResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendBurstDtmfResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendCdmaSmsResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendCdmaSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SMS_Response testVector = {
        200, (char *)"", 0
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendCdmaSmsResponse(const RadioResponseInfo& info, const SendSmsResult& sms) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.messageRef, sms.messageRef);
            EXPECT_TRUE(testVector.ackPDU == sms.ackPDU);
            EXPECT_EQ(testVector.errorCode, sms.errorCode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendCdmaSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, acknowledgeLastIncomingCdmaSmsResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::acknowledgeLastIncomingCdmaSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> acknowledgeLastIncomingCdmaSmsResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::acknowledgeLastIncomingCdmaSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getGsmBroadcastConfigResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getGsmBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_GSM_BroadcastSmsConfigInfo *testVector[] = {
        new RIL_GSM_BroadcastSmsConfigInfo {
            1, 2, 3, 4, 1
        },
        new RIL_GSM_BroadcastSmsConfigInfo {
            5, 6, 7, 8, 0
        },
        new RIL_GSM_BroadcastSmsConfigInfo {
            10, 11, 12, 13, 1
        },
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getGsmBroadcastConfigResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<GsmBroadcastSmsConfigInfo>& configs) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(size, configs.size());
                for (size_t i = 0; i < configs.size(); i++) {
                    EXPECT_EQ(testVector[i]->fromServiceId, configs[i].fromServiceId);
                    EXPECT_EQ(testVector[i]->toServiceId, configs[i].toServiceId);
                    EXPECT_EQ(testVector[i]->fromCodeScheme, configs[i].fromCodeScheme);
                    EXPECT_EQ(testVector[i]->toCodeScheme, configs[i].toCodeScheme);
                    EXPECT_EQ(testVector[i]->selected, configs[i].selected);
                }
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getGsmBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getGsmBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);


    for (size_t i = 0; i < size; i++) {
        delete testVector[i];
    }
}

TEST(ril_service_legacy_rsp_tests, setGsmBroadcastConfigResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setGsmBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setGsmBroadcastConfigResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setGsmBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setGsmBroadcastActivationResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setGsmBroadcastActivationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setGsmBroadcastActivationResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setGsmBroadcastActivationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCdmaBroadcastConfigResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCdmaBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CDMA_BroadcastSmsConfigInfo *testVector[] = {
        new RIL_CDMA_BroadcastSmsConfigInfo {
            1, 2, 1
        },
        new RIL_CDMA_BroadcastSmsConfigInfo {
            5, 6, 0
        },
        new RIL_CDMA_BroadcastSmsConfigInfo {
            10, 11, 1
        },
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCdmaBroadcastConfigResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<CdmaBroadcastSmsConfigInfo>& configs) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(size, configs.size());
                for (size_t i = 0; i < configs.size(); i++) {
                    EXPECT_EQ(testVector[i]->service_category, configs[i].serviceCategory);
                    EXPECT_EQ(testVector[i]->language, configs[i].language);
                    EXPECT_EQ(testVector[i]->selected, configs[i].selected);
                }
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getCdmaBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getCdmaBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);


    for (size_t i = 0; i < size; i++) {
        delete testVector[i];
    }
}

TEST(ril_service_legacy_rsp_tests, setCdmaBroadcastConfigResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setCdmaBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setCdmaBroadcastConfigResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setCdmaBroadcastConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setCdmaBroadcastActivationResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setCdmaBroadcastActivationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setCdmaBroadcastActivationResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setCdmaBroadcastActivationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCDMASubscriptionResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCDMASubscriptionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char *testVector[] = {
            (char *)"MDN", (char *)"SID", (char *)"NID", (char *)"MIN", (char *)"PRL"
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCDMASubscriptionResponse(const RadioResponseInfo& info,
                const ::android::hardware::hidl_string& mdn,
                const ::android::hardware::hidl_string& hSid,
                const ::android::hardware::hidl_string& hNid,
                const ::android::hardware::hidl_string& min,
                const ::android::hardware::hidl_string& prl) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_TRUE(testVector[0] == mdn);
                EXPECT_TRUE(testVector[1] == hSid);
                EXPECT_TRUE(testVector[2] == hNid);
                EXPECT_TRUE(testVector[3] == min);
                EXPECT_TRUE(testVector[4] == prl);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getCDMASubscriptionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getCDMASubscriptionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, writeSmsToRuimResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::writeSmsToRuimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 11 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> writeSmsToRuimResponse(const RadioResponseInfo& info, uint32_t index) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector[0], index);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::writeSmsToRuimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, deleteSmsOnRuimResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::deleteSmsOnRuimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> deleteSmsOnRuimResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::deleteSmsOnRuimResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDeviceIdentityResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDeviceIdentityResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char *testVector[] = {
            (char *)"IMEI", (char *)"IMEISV", (char *)"ESH", (char *)"MEID"
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getDeviceIdentityResponse(const RadioResponseInfo& info,
                const ::android::hardware::hidl_string& imei,
                const ::android::hardware::hidl_string& imeisv,
                const ::android::hardware::hidl_string& esn,
                const ::android::hardware::hidl_string& meid) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_TRUE(testVector[0] == imei);
                EXPECT_TRUE(testVector[1] == imeisv);
                EXPECT_TRUE(testVector[2] == esn);
                EXPECT_TRUE(testVector[3] == meid);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getDeviceIdentityResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getDeviceIdentityResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, exitEmergencyCallbackModeResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::exitEmergencyCallbackModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> exitEmergencyCallbackModeResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::exitEmergencyCallbackModeResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getSmscAddressResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSmscAddressResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char testVector[] = "0F911012345678";

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getSmscAddressResponse(const RadioResponseInfo& info,
                const ::android::hardware::hidl_string& smsc) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_TRUE(testVector == smsc);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getSmscAddressResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setSmscAddressResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setSmscAddressResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setSmscAddressResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setSmscAddressResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, reportSmsMemoryStatusResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::reportSmsMemoryStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> reportSmsMemoryStatusResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::reportSmsMemoryStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, reportStkServiceIsRunningResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::reportStkServiceIsRunningResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> reportStkServiceIsRunningResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::reportStkServiceIsRunningResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCdmaSubscriptionSourceResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCdmaSubscriptionSourceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCdmaSubscriptionSourceResponse(const RadioResponseInfo& info,
                CdmaSubscriptionSource source) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], (int)source);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getCdmaSubscriptionSourceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, requestIsimAuthenticationResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::requestIsimAuthenticationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char testVector[] = "requestIsimAuthenticationResponse";

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> requestIsimAuthenticationResponse(
                const RadioResponseInfo& info, const ::android::hardware::hidl_string& response) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_TRUE(testVector == response);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::requestIsimAuthenticationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, acknowledgeIncomingGsmSmsWithPduResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::acknowledgeIncomingGsmSmsWithPduResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> acknowledgeIncomingGsmSmsWithPduResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::acknowledgeIncomingGsmSmsWithPduResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendEnvelopeWithStatusResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendEnvelopeWithStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SIM_IO_Response testVector = {
        1, 0, (char *)"0A1B3C4D5E6F",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendEnvelopeWithStatusResponse(const RadioResponseInfo& info,
                const IccIoResult& iccIo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.sw1, iccIo.sw1);
            EXPECT_EQ(testVector.sw2, iccIo.sw2);
            EXPECT_TRUE(testVector.simResponse == iccIo.simResponse);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendEnvelopeWithStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getVoiceRadioTechnologyResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getVoiceRadioTechnologyResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 14 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getVoiceRadioTechnologyResponse(
                const RadioResponseInfo& info, ::android::hardware::radio::V1_0::RadioTechnology rat) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], (int)rat);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getVoiceRadioTechnologyResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCellInfoListResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCellInfoListResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_v12 testVector[] = {
        // TODO
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCellInfoListResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>&
                        cellInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                for (size_t i = 0; i < cellInfo.size(); i++) {
                    test(testVector[i], cellInfo[i]);
                }
            }
            return Void();
        }

        void test(const RIL_CellInfo_v12& /*t*/, const V1_0::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getCellInfoListResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getCellInfoListResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setCellInfoListRateResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setCellInfoListRateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setCellInfoListRateResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setCellInfoListRateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setInitialAttachApnResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setInitialAttachApnResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setInitialAttachApnResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setInitialAttachApnResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getImsRegistrationStateResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getImsRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1,  RIL_RadioTechnologyFamily::RADIO_TECH_3GPP };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getImsRegistrationStateResponse(const RadioResponseInfo& info, bool isRegistered,
                RadioTechnologyFamily ratFamily) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector[0], isRegistered);
                EXPECT_EQ(testVector[1], (int)ratFamily);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getImsRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getImsRegistrationStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendImsSmsResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendImsSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SMS_Response testVector = {
        200, (char *)"", 0
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendImsSmsResponse(const RadioResponseInfo& info, const SendSmsResult& sms) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.messageRef, sms.messageRef);
            EXPECT_TRUE(testVector.ackPDU == sms.ackPDU);
            EXPECT_EQ(testVector.errorCode, sms.errorCode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendImsSmsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, iccTransmitApduBasicChannelResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::iccTransmitApduBasicChannelResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SIM_IO_Response testVector = {
        1, 0, (char *)"0A1B3C4D5E6F",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> iccTransmitApduBasicChannelResponse(const RadioResponseInfo& info,
                const IccIoResult& result) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.sw1, result.sw1);
            EXPECT_EQ(testVector.sw2, result.sw2);
            EXPECT_TRUE(testVector.simResponse == result.simResponse);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::iccTransmitApduBasicChannelResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, iccOpenLogicalChannelResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::iccOpenLogicalChannelResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static struct {
        int channelId;
        int selectResponse[10];
    } testVector = {
        2,
        { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> iccOpenLogicalChannelResponse(
                const RadioResponseInfo& info, int32_t channelId,
                const ::android::hardware::hidl_vec<int8_t>& selectResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.channelId, channelId);
                size_t size = sizeof(testVector.selectResponse) / sizeof(testVector.selectResponse[0]);
                EXPECT_EQ(size, selectResponse.size());
                for(size_t i = 0; i < selectResponse.size(); i++ ) {
                    EXPECT_EQ((uint8_t)testVector.selectResponse[i], selectResponse[i]);
                }
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::iccOpenLogicalChannelResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::iccOpenLogicalChannelResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, iccCloseLogicalChannelResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::iccCloseLogicalChannelResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> iccCloseLogicalChannelResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::iccCloseLogicalChannelResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, iccTransmitApduLogicalChannelResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::iccTransmitApduLogicalChannelResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SIM_IO_Response testVector = {
        1, 0, (char *)"0A1B3C4D5E6F",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> iccTransmitApduLogicalChannelResponse(const RadioResponseInfo& info,
                const IccIoResult& result) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.sw1, result.sw1);
            EXPECT_EQ(testVector.sw2, result.sw2);
            EXPECT_TRUE(testVector.simResponse == result.simResponse);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::iccTransmitApduLogicalChannelResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, nvReadItemResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::nvReadItemResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static char testVector[] = "nvReadItemResponse";

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> nvReadItemResponse(const RadioResponseInfo& info,
                const ::android::hardware::hidl_string& result) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_TRUE(testVector == result);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::nvReadItemResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, nvWriteItemResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::nvWriteItemResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> nvWriteItemResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::nvWriteItemResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, nvWriteCdmaPrlResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::nvWriteCdmaPrlResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> nvWriteCdmaPrlResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::nvWriteCdmaPrlResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, nvResetConfigResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::nvResetConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> nvResetConfigResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::nvResetConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setUiccSubscriptionResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setUiccSubscriptionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setUiccSubscriptionResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setUiccSubscriptionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setDataAllowedResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setDataAllowedResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setDataAllowedResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setDataAllowedResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getHardwareConfigResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getHardwareConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_HardwareConfig testVector[] = {
        {
            .type = RIL_HardwareConfig_Type::RIL_HARDWARE_CONFIG_MODEM,
            .uuid = {},
            .state = RIL_HardwareConfig_State::RIL_HARDWARE_CONFIG_STATE_ENABLED,
            .cfg.modem = { 15, 0x1FF, 1, 2, 2 },
        },
        {
            .type = RIL_HardwareConfig_Type::RIL_HARDWARE_CONFIG_SIM,
            .uuid = {},
            .state = RIL_HardwareConfig_State::RIL_HARDWARE_CONFIG_STATE_STANDBY,
            .cfg.sim = { "uuid1234567" },
        },
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getHardwareConfigResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<HardwareConfig>& configs) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(size, configs.size());
                for (size_t i = 0; i < configs.size(); i++) {
                    test(testVector[i], configs[i]);
                }
            }
            return Void();
        }

        void test(const RIL_HardwareConfig& t, const HardwareConfig& hwconfig) {
            EXPECT_EQ(t.type, (int)hwconfig.type);
            EXPECT_EQ(t.state, (int)hwconfig.state);
            switch (t.type) {
            case RIL_HARDWARE_CONFIG_MODEM: {
                EXPECT_EQ((unsigned long)1, hwconfig.modem.size());
                EXPECT_EQ(t.cfg.modem.rat, hwconfig.modem[0].rat);
                EXPECT_EQ(t.cfg.modem.maxVoice, hwconfig.modem[0].maxVoice);
                EXPECT_EQ(t.cfg.modem.maxData, hwconfig.modem[0].maxData);
                EXPECT_EQ(t.cfg.modem.maxStandby, hwconfig.modem[0].maxStandby);
                break;
            }
            case RIL_HARDWARE_CONFIG_SIM: {
                EXPECT_EQ((unsigned long)1, hwconfig.sim.size());
                EXPECT_TRUE(t.cfg.sim.modemUuid == hwconfig.sim[0].modemUuid);
                break;
            }
            default:
                break;
            }
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getHardwareConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getHardwareConfigResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, requestIccSimAuthenticationResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::requestIccSimAuthenticationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SIM_IO_Response testVector = {
        1, 0, (char *)"0A1B3C4D5E6F",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> requestIccSimAuthenticationResponse(const RadioResponseInfo& info,
                const IccIoResult& result) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.sw1, result.sw1);
            EXPECT_EQ(testVector.sw2, result.sw2);
            EXPECT_TRUE(testVector.simResponse == result.simResponse);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::requestIccSimAuthenticationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setDataProfileResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setDataProfileResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setDataProfileResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setDataProfileResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, requestShutdownResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::requestShutdownResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> requestShutdownResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::requestShutdownResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getRadioCapabilityResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getRadioCapabilityResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RadioCapability testVector = {
        1, 2, 3, 4, {}, 0,
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getRadioCapabilityResponse(
                const RadioResponseInfo& info,
                const android::hardware::radio::V1_0::RadioCapability& rc) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.session, rc.session);
            EXPECT_EQ(testVector.phase, (int)rc.phase);
            EXPECT_EQ(testVector.rat, rc.raf);
            EXPECT_EQ(testVector.status, (int)rc.status);
            EXPECT_TRUE(testVector.logicalModemUuid == rc.logicalModemUuid);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getRadioCapabilityResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setRadioCapabilityResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setRadioCapabilityResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setRadioCapabilityResponse(
                const RadioResponseInfo& info,
                const android::hardware::radio::V1_0::RadioCapability&) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::INVALID_RESPONSE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setRadioCapabilityResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, startLceServiceResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::startLceServiceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_LceStatusInfo testVector = {
        1, 128
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> startLceServiceResponse(const RadioResponseInfo& info,
                const LceStatusInfo& statusInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.lce_status, (int)statusInfo.lceStatus);
            EXPECT_EQ(testVector.actual_interval_ms, statusInfo.actualIntervalMs);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::startLceServiceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, stopLceServiceResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::stopLceServiceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_LceStatusInfo testVector = {
        1, 111
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> stopLceServiceResponse(const RadioResponseInfo& info,
                const LceStatusInfo& statusInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.lce_status, (int)statusInfo.lceStatus);
            EXPECT_EQ(testVector.actual_interval_ms, statusInfo.actualIntervalMs);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::stopLceServiceResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, pullLceDataResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::pullLceDataResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    LceDataInfo testVector = {};

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> pullLceDataResponse(const RadioResponseInfo& info, const LceDataInfo&) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::pullLceDataResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::pullLceDataResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getModemActivityInfoResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getModemActivityInfoResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    RIL_ActivityStatsInfo testVector = {};

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getModemActivityInfoResponse(const RadioResponseInfo& info,
                const ActivityStatsInfo&) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getModemActivityInfoResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::getModemActivityInfoResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setAllowedCarriersResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setAllowedCarriersResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 2 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setAllowedCarriersResponse(const RadioResponseInfo& info, int32_t numAllowed) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], numAllowed);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setAllowedCarriersResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getAllowedCarriersResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getAllowedCarriersResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    RIL_Carrier testAllowedCarriers[] = {
        { (char *)"001", (char *)"01", RIL_CarrierMatchType::RIL_MATCH_ALL, (char *)"0A1B" }
    };
    RIL_Carrier testExcludedCarriers[] = {
        { (char *)"001", (char *)"01", RIL_CarrierMatchType::RIL_MATCH_ALL, (char *)"0A1B" }
    };
    static RIL_CarrierRestrictions testVector = {
        1, 1, testAllowedCarriers, testExcludedCarriers,
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getAllowedCarriersResponse(const RadioResponseInfo& info, bool,
                const CarrierRestrictions&) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);

            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getAllowedCarriersResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::getAllowedCarriersResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getAllowedCarriersResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendDeviceStateResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendDeviceStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendDeviceStateResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendDeviceStateResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setCarrierInfoForImsiEncryptionResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setCarrierInfoForImsiEncryptionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setCarrierInfoForImsiEncryptionResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setCarrierInfoForImsiEncryptionResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setIndicationFilterResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setIndicationFilterResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setIndicationFilterResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setIndicationFilterResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setSimCardPowerResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setIndicationFilterResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setSimCardPowerResponse_1_1(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::setSimCardPowerResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, startNetworkScanResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::startNetworkScanResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> startNetworkScanResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::startNetworkScanResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, stopNetworkScanResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::stopNetworkScanResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> stopNetworkScanResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::stopNetworkScanResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, startKeepaliveResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::startKeepaliveResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_KeepaliveStatus testVector = {
        1,  RIL_KeepaliveStatusCode::KEEPALIVE_ACTIVE
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> startKeepaliveResponse(const RadioResponseInfo& info,
                const KeepaliveStatus& status) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.sessionHandle, (unsigned int)/* NEED TO CHECK */ status.sessionHandle);
                EXPECT_EQ(testVector.code, (int)status.code);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::startKeepaliveResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::startKeepaliveResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, stopKeepaliveResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::stopKeepaliveResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> stopKeepaliveResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::stopKeepaliveResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}


TEST(ril_service_legacy_rsp_tests, sendRequestRawResponse) {
    radio::sendRequestRawResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);
}

TEST(ril_service_legacy_rsp_tests, sendRequestStringsResponse) {
    radio::sendRequestRawResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);
}

TEST(ril_service_legacy_rsp_tests, setSignalStrengthReportingCriteriaResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setSignalStrengthReportingCriteriaResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setSignalStrengthReportingCriteriaResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setSignalStrengthReportingCriteriaResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setLinkCapacityReportingCriteriaResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setLinkCapacityReportingCriteriaResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setLinkCapacityReportingCriteriaResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setLinkCapacityReportingCriteriaResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

// V1_2::IRadioResponse
TEST(ril_service_legacy_rsp_tests, getIccCardStatusResponse_1_2) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getIccCardStatusResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CardStatus_V1_2 testVector = {
        RIL_CardState::RIL_CARDSTATE_PRESENT,
        RIL_PinState::RIL_PINSTATE_ENABLED_VERIFIED,
        0, -1, -1, 1,
        {
            {
                RIL_AppType::RIL_APPTYPE_USIM,
                RIL_AppState::RIL_APPSTATE_READY,
                RIL_PersoSubstate::RIL_PERSOSUBSTATE_UNKNOWN,
                (char *)"aid1234",
                (char *)"",
                0,
                RIL_PinState::RIL_PINSTATE_DISABLED,
                RIL_PinState::RIL_PINSTATE_DISABLED
            }
        },
        0,
        (char *)"atr_1234",
        (char *)"812347889123",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getIccCardStatusResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_2::CardStatus& cardStatus) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.card_state, (int)cardStatus.base.cardState);
                EXPECT_EQ(testVector.universal_pin_state, (int)cardStatus.base.universalPinState);
                EXPECT_EQ(testVector.gsm_umts_subscription_app_index, cardStatus.base.gsmUmtsSubscriptionAppIndex);
                EXPECT_EQ(testVector.cdma_subscription_app_index, cardStatus.base.cdmaSubscriptionAppIndex);
                EXPECT_EQ(testVector.ims_subscription_app_index, cardStatus.base.imsSubscriptionAppIndex);
                if (cardStatus.base.applications.size() > 0) {
                    EXPECT_EQ((unsigned long)/* NEED TO CHECK */ testVector.num_applications, cardStatus.base.applications.size());
                    for (size_t i = 0; i < cardStatus.base.applications.size(); i++) {
                        test(testVector.applications[i], cardStatus.base.applications[i]);
                    }
                }
                EXPECT_EQ(testVector.physicalSlotId, cardStatus.physicalSlotId);
                EXPECT_TRUE(testVector.atr == cardStatus.atr);
                EXPECT_TRUE(testVector.iccid == cardStatus.iccid);
            }
            return Void();
        }

        void test(const RIL_AppStatus& t, const V1_0::AppStatus& appStatus) {
            EXPECT_EQ(t.app_type, (int)appStatus.appType);
            EXPECT_EQ(t.app_state, (int)appStatus.appState);
            EXPECT_EQ(t.perso_substate, (int)appStatus.persoSubstate);
            EXPECT_TRUE(t.aid_ptr == appStatus.aidPtr);
            EXPECT_TRUE(t.app_label_ptr == appStatus.appLabelPtr);
            EXPECT_EQ(t.pin1_replaced, appStatus.pin1Replaced);
            EXPECT_EQ(t.pin1, (int)appStatus.pin1);
            EXPECT_EQ(t.pin2, (int)appStatus.pin2);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    RIL_CardStatus_V1_2 invalidParams = {
        .gsm_umts_subscription_app_index = 1,
        .cdma_subscription_app_index = 2,
        .ims_subscription_app_index = 3,
        .num_applications = 1,
    };
    radio::getIccCardStatusResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &invalidParams, sizeof(invalidParams));

    radio::getIccCardStatusResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getIccCardStatusResponse_1_2_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getIccCardStatusResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CardStatus_V1_2 testVector = {
        RIL_CardState::RIL_CARDSTATE_PRESENT,
        RIL_PinState::RIL_PINSTATE_ENABLED_VERIFIED,
        0, -1, -1, 1,
        {
            {
                RIL_AppType::RIL_APPTYPE_USIM,
                RIL_AppState::RIL_APPSTATE_READY,
                RIL_PersoSubstate::RIL_PERSOSUBSTATE_UNKNOWN,
                (char *)"aid1234",
                (char *)"",
                0,
                RIL_PinState::RIL_PINSTATE_DISABLED,
                RIL_PinState::RIL_PINSTATE_DISABLED
            }
        },
        0,
        (char *)"atr_1234",
        (char *)"812347889123",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_1 {
        Return<void> getIccCardStatusResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_0::CardStatus& cardStatus) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.card_state, (int)cardStatus.cardState);
                EXPECT_EQ(testVector.universal_pin_state, (int)cardStatus.universalPinState);
                EXPECT_EQ(testVector.gsm_umts_subscription_app_index, cardStatus.gsmUmtsSubscriptionAppIndex);
                EXPECT_EQ(testVector.cdma_subscription_app_index, cardStatus.cdmaSubscriptionAppIndex);
                EXPECT_EQ(testVector.ims_subscription_app_index, cardStatus.imsSubscriptionAppIndex);
                if (cardStatus.applications.size() > 0) {
                    EXPECT_EQ((unsigned long)/* NEED To CHECK */ testVector.num_applications, cardStatus.applications.size());
                    for (size_t i = 0; i < cardStatus.applications.size(); i++) {
                        test(testVector.applications[i], cardStatus.applications[i]);
                    }
                }
            }
            return Void();
        }

        void test(const RIL_AppStatus& t, const V1_0::AppStatus& appStatus) {
            EXPECT_EQ(t.app_type, (int)appStatus.appType);
            EXPECT_EQ(t.app_state, (int)appStatus.appState);
            EXPECT_EQ(t.perso_substate, (int)appStatus.persoSubstate);
            EXPECT_TRUE(t.aid_ptr == appStatus.aidPtr);
            EXPECT_TRUE(t.app_label_ptr == appStatus.appLabelPtr);
            EXPECT_EQ(t.pin1_replaced, appStatus.pin1Replaced);
            EXPECT_EQ(t.pin1, (int)appStatus.pin1);
            EXPECT_EQ(t.pin2, (int)appStatus.pin2);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_1::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_1;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getIccCardStatusResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCurrentCallsResponse_1_2) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCurrentCallsResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_Call_V1_2 *testVector[] = {
        new RIL_Call_V1_2 {
            RIL_CallState::RIL_CALL_DIALING,
            0,
            0x91,
            1,
            0,
            0,
            1,
            0,
            (char *)"01012345678",
            0,
            (char *)"test1",
            0,
            nullptr,
            RIL_AudioQuality::AMR_WB
        },
        new RIL_Call_V1_2 {
            RIL_CallState::RIL_CALL_DIALING,
            0,
            0x91,
            0,
            1,
            0,
            1,
            0,
            (char *)"01077778888",
            0,
            (char *)"test2",
            0,
            nullptr,
            RIL_AudioQuality::AMR_WB
        },
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCurrentCallsResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::Call>& calls) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(size, calls.size());
                for (size_t i = 0; i < calls.size(); i++) {
                    test(*testVector[i], calls[i]);
                }
            }
            return Void();
        }

        void test(const RIL_Call_V1_2& t, const V1_2::Call& call) {
            test(*((RIL_Call *)&t) ,call.base);
            EXPECT_EQ(t.audioQuality, (int)call.audioQuality);
        }

        void test(const RIL_Call& t, const V1_0::Call& call) {
            EXPECT_EQ(t.state, (int)call.state);
            EXPECT_EQ(t.index, call.index);
            EXPECT_EQ(t.toa, call.toa);
            EXPECT_EQ(t.isMpty, call.isMpty);
            EXPECT_EQ(t.isMT, call.isMT);
            EXPECT_EQ(t.als, call.als);
            EXPECT_EQ(t.isVoicePrivacy, call.isVoicePrivacy);
            EXPECT_TRUE(t.number == call.number);
            EXPECT_EQ(t.numberPresentation, (int)call.numberPresentation);
            EXPECT_TRUE(t.name == call.name);
            EXPECT_EQ(t.namePresentation, (int)call.namePresentation);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getCurrentCallsResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getCurrentCallsResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);

    for (size_t i = 0; i < size; i++) {
        delete testVector[i];
    }
}

TEST(ril_service_legacy_rsp_tests, getVoiceRegistrationStateResponse_1_2) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getVoiceRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_VoiceRegistrationStateResponse_V1_2 testVector = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        0, -1, -1, -1, 0,
        {}
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getVoiceRegistrationStateResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_2::VoiceRegStateResult& voiceRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.regState, (int)voiceRegResponse.regState);
                EXPECT_EQ(testVector.rat, voiceRegResponse.rat);
                EXPECT_EQ(testVector.cssSupported, voiceRegResponse.cssSupported);
                EXPECT_EQ(testVector.roamingIndicator, voiceRegResponse.roamingIndicator);
                EXPECT_EQ(testVector.systemIsInPrl, voiceRegResponse.systemIsInPrl);
                EXPECT_EQ(testVector.defaultRoamingIndicator, voiceRegResponse.defaultRoamingIndicator);
                EXPECT_EQ(testVector.reasonForDenial, voiceRegResponse.reasonForDenial);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getVoiceRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getVoiceRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getVoiceRegistrationStateResponse_1_2_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getVoiceRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_VoiceRegistrationStateResponse_V1_2 testVector = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        0, -1, -1, -1, 0,
        {}
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_1 {
        Return<void> getVoiceRegistrationStateResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_2::VoiceRegStateResult& voiceRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                RIL_VoiceRegistrationStateResponse voiceRegResult {};
                RadioConvUtils::convertVoiceRegistrationStateResult(voiceRegResult,
                        testVector);

                EXPECT_EQ(voiceRegResult.regState, (int)voiceRegResponse.regState);
                EXPECT_EQ(voiceRegResult.rat, voiceRegResponse.rat);
                EXPECT_EQ(voiceRegResult.cssSupported, voiceRegResponse.cssSupported);
                EXPECT_EQ(voiceRegResult.roamingIndicator, voiceRegResponse.roamingIndicator);
                EXPECT_EQ(voiceRegResult.systemIsInPrl, voiceRegResponse.systemIsInPrl);
                EXPECT_EQ(voiceRegResult.defaultRoamingIndicator, voiceRegResponse.defaultRoamingIndicator);
                EXPECT_EQ(voiceRegResult.reasonForDenial, voiceRegResponse.reasonForDenial);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_1::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_1;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getVoiceRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDataRegistrationStateResponse_1_2) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_DataRegistrationStateResponse_V1_2 testVector = {
        RIL_RegState::RIL_REG_DENIED,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        7, 4, {}
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getDataRegistrationStateResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_2::DataRegStateResult& dataRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector.regState, (int)dataRegResponse.regState);
                EXPECT_EQ(testVector.rat, dataRegResponse.rat);
                EXPECT_EQ(testVector.reasonDataDenied, dataRegResponse.reasonDataDenied);
                EXPECT_EQ(testVector.maxDataCalls, dataRegResponse.maxDataCalls);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getDataRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getDataRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDataRegistrationStateResponse_1_2_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_DataRegistrationStateResponse_V1_2 testVector = {
        RIL_RegState::RIL_REG_DENIED,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        7, 4, {}
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_1 {
        Return<void> getDataRegistrationStateResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_2::DataRegStateResult& dataRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                RIL_DataRegistrationStateResponse dataRegResult {};
                RadioConvUtils::convertDataRegistrationStateResult(dataRegResult,
                        testVector);

                EXPECT_EQ(dataRegResult.regState, (int)dataRegResponse.regState);
                EXPECT_EQ(dataRegResult.rat, dataRegResponse.rat);
                EXPECT_EQ(dataRegResult.reasonDataDenied, dataRegResponse.reasonDataDenied);
                EXPECT_EQ(dataRegResult.maxDataCalls, dataRegResponse.maxDataCalls);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_1::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_1;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getDataRegistrationStateResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getSignalStrengthResponse_1_2) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSignalStrengthResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SignalStrength_V1_2 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getSignalStrengthResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_2::SignalStrength& signalStrength) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.signalStrength,
                          signalStrength.lte.signalStrength);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.rsrp,
                          signalStrength.lte.rsrp);
                EXPECT_EQ((unsigned int)/* NEED To CHECK */ testVector.LTE_SignalStrength.rsrq,
                          signalStrength.lte.rsrq);
                EXPECT_EQ(testVector.LTE_SignalStrength.rssnr,
                        signalStrength.lte.rssnr);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.cqi,
                          signalStrength.lte.cqi);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.timingAdvance,
                          signalStrength.lte.timingAdvance);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getSignalStrengthResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getSignalStrengthResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getSignalStrengthResponse_1_2_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSignalStrengthResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SignalStrength_V1_2 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_1 {
        Return<void> getSignalStrengthResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_0::SignalStrength& signalStrength) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);

            RIL_SignalStrength_v10 ss;
            RadioConvUtils::convertSignalStrengthResult(ss, testVector);

            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.signalStrength,
                      signalStrength.lte.signalStrength);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.rsrp,
                      signalStrength.lte.rsrp);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.rsrq,
                      signalStrength.lte.rsrq);
            EXPECT_EQ(ss.LTE_SignalStrength.rssnr,
                    signalStrength.lte.rssnr);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.cqi,
                      signalStrength.lte.cqi);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.timingAdvance,
                      signalStrength.lte.timingAdvance);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_1::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_1;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getSignalStrengthResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCellInfoListResponse_1_2) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCellInfoListResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_2 testVector[] = {
        // TODO
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCellInfoListResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>&
                        cellInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                for (size_t i = 0; i < cellInfo.size(); i++) {
                    test(testVector[i], cellInfo[i]);
                }
            }
            return Void();
        }

        void test(const RIL_CellInfo_V1_2& /*t*/, const V1_2::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getCellInfoListResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getCellInfoListResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCellInfoListResponse_1_2_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCellInfoListResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_2 testVector[] = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.gsm = {
                { 450, 5, INT_MAX, INT_MAX, 0, 0xFF, {} },
                { 99, 99, INT_MAX },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.wcdma = {
                { 450, 5, INT_MAX, INT_MAX, 0, 0, {} },
                { 99, 99, 255, 255 },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.lte = {
                { 450, 5, INT_MAX, 0, INT_MAX, 0, {}, 0 },
                { 99, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX },
            },
        },
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_1 {
        Return<void> getCellInfoListResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>&
                    records) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(records.size(), size);
            for (size_t i = 0; i < records.size(); i++) {
                RIL_CellInfo_v12 cellInfo {};
                RadioConvUtils::convertCellInfo(cellInfo, testVector[i]);
                test(cellInfo, records[i]);
            }
            return Void();
        }

        void test(const RIL_CellInfo_v12& /*t*/, const V1_0::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_1::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_1;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getCellInfoListResponse_1_2(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

// V1_3::IRadioResponse
TEST(ril_service_legacy_rsp_tests, setSystemSelectionChannelsResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setSystemSelectionChannelsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setSystemSelectionChannelsResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setSystemSelectionChannelsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, enableModemResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::enableModemResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> enableModemResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::enableModemResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getModemStackStatusResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getModemStackStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getModemStackStatusResponse(const RadioResponseInfo& info, const bool enabled) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector[0], enabled);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getModemStackStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::getModemStackStatusResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

// V1_4::IRadioResponse
TEST(ril_service_legacy_rsp_tests, getIccCardStatusResponse_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getIccCardStatusResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CardStatus_V1_4 testVector = {
        RIL_CardState::RIL_CARDSTATE_PRESENT,
        RIL_PinState::RIL_PINSTATE_ENABLED_VERIFIED,
        0, -1, -1, 1,
        {
            {
                RIL_AppType::RIL_APPTYPE_USIM,
                RIL_AppState::RIL_APPSTATE_READY,
                RIL_PersoSubstate::RIL_PERSOSUBSTATE_UNKNOWN,
                (char *)"aid1234",
                (char *)"",
                0,
                RIL_PinState::RIL_PINSTATE_DISABLED,
                RIL_PinState::RIL_PINSTATE_DISABLED
            }
        },
        0,
        (char *)"atr_1234",
        (char *)"812347889123",
        (char *)"eid12345678",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getIccCardStatusResponse_1_4(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_4::CardStatus& cardStatus) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                test(testVector, cardStatus);
            }
            return Void();
        }

        void test(const RIL_CardStatus_V1_4& t, const V1_4::CardStatus& cardStatus) {
            test(*((RIL_CardStatus_V1_2 *)&t), cardStatus.base);
            EXPECT_TRUE(t.eid == cardStatus.eid);
        }

        void test(const RIL_CardStatus_V1_2& t, const V1_2::CardStatus& cardStatus) {
            test(*((RIL_CardStatus_v6 *)&t), cardStatus.base);
            EXPECT_EQ(t.physicalSlotId, cardStatus.physicalSlotId);
            EXPECT_TRUE(t.atr == cardStatus.atr);
            EXPECT_TRUE(t.iccid == cardStatus.iccid);
        }

        void test(const RIL_CardStatus_v6& t, const V1_0::CardStatus& cardStatus) {
            EXPECT_EQ(t.card_state, (int)cardStatus.cardState);
            EXPECT_EQ(t.universal_pin_state, (int)cardStatus.universalPinState);
            EXPECT_EQ(t.gsm_umts_subscription_app_index, cardStatus.gsmUmtsSubscriptionAppIndex);
            EXPECT_EQ(t.cdma_subscription_app_index, cardStatus.cdmaSubscriptionAppIndex);
            EXPECT_EQ(t.ims_subscription_app_index, cardStatus.imsSubscriptionAppIndex);
            if (cardStatus.applications.size() > 0) {
                EXPECT_EQ((unsigned long)/* NEED TO CHECK */ t.num_applications, cardStatus.applications.size());
                for (size_t i = 0; i < cardStatus.applications.size(); i++) {
                    test(t.applications[i], cardStatus.applications[i]);
                }
            }
        }

        void test(const RIL_AppStatus& t, const V1_0::AppStatus& appStatus) {
            EXPECT_EQ(t.app_type, (int)appStatus.appType);
            EXPECT_EQ(t.app_state, (int)appStatus.appState);
            EXPECT_EQ(t.perso_substate, (int)appStatus.persoSubstate);
            EXPECT_TRUE(t.aid_ptr == appStatus.aidPtr);
            EXPECT_TRUE(t.app_label_ptr == appStatus.appLabelPtr);
            EXPECT_EQ(t.pin1_replaced, appStatus.pin1Replaced);
            EXPECT_EQ(t.pin1, (int)appStatus.pin1);
            EXPECT_EQ(t.pin2, (int)appStatus.pin2);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    RIL_CardStatus_V1_4 invalidParams = {
        .gsm_umts_subscription_app_index = 1,
        .cdma_subscription_app_index = 2,
        .ims_subscription_app_index = 3,
        .num_applications = 1,
    };
    radio::getIccCardStatusResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &invalidParams, sizeof(invalidParams));

    radio::getIccCardStatusResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getIccCardStatusResponse_1_4_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getIccCardStatusResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CardStatus_V1_4 testVector = {
        RIL_CardState::RIL_CARDSTATE_PRESENT,
        RIL_PinState::RIL_PINSTATE_ENABLED_VERIFIED,
        0, -1, -1, 1,
        {
            {
                RIL_AppType::RIL_APPTYPE_USIM,
                RIL_AppState::RIL_APPSTATE_READY,
                RIL_PersoSubstate::RIL_PERSOSUBSTATE_UNKNOWN,
                (char *)"aid1234",
                (char *)"",
                0,
                RIL_PinState::RIL_PINSTATE_DISABLED,
                RIL_PinState::RIL_PINSTATE_DISABLED
            }
        },
        0,
        (char *)"atr_1234",
        (char *)"812347889123",
        (char *)"eid12345678",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_2 {
        Return<void> getIccCardStatusResponse_1_2(
               const RadioResponseInfo& info,
               const ::android::hardware::radio::V1_2::CardStatus& cardStatus) {
           EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
           EXPECT_EQ(info.serial, serial);
           EXPECT_EQ(info.error, RadioError::NONE);
           EXPECT_EQ(testVector.card_state, (int)cardStatus.base.cardState);
           EXPECT_EQ(testVector.universal_pin_state, (int)cardStatus.base.universalPinState);
           EXPECT_EQ(testVector.gsm_umts_subscription_app_index, cardStatus.base.gsmUmtsSubscriptionAppIndex);
           EXPECT_EQ(testVector.cdma_subscription_app_index, cardStatus.base.cdmaSubscriptionAppIndex);
           EXPECT_EQ(testVector.ims_subscription_app_index, cardStatus.base.imsSubscriptionAppIndex);
           if (cardStatus.base.applications.size() > 0) {
               EXPECT_EQ((unsigned long)/* NEED TO CHECK */ testVector.num_applications, cardStatus.base.applications.size());
               for (size_t i = 0; i < cardStatus.base.applications.size(); i++) {
                   test(testVector.applications[i], cardStatus.base.applications[i]);
               }
           }
           EXPECT_EQ(testVector.physicalSlotId, cardStatus.physicalSlotId);
           EXPECT_TRUE(testVector.atr == cardStatus.atr);
           EXPECT_TRUE(testVector.iccid == cardStatus.iccid);
           return Void();
       }

       void test(const RIL_AppStatus& t, const V1_0::AppStatus& appStatus) {
           EXPECT_EQ(t.app_type, (int)appStatus.appType);
           EXPECT_EQ(t.app_state, (int)appStatus.appState);
           EXPECT_EQ(t.perso_substate, (int)appStatus.persoSubstate);
           EXPECT_TRUE(t.aid_ptr == appStatus.aidPtr);
           EXPECT_TRUE(t.app_label_ptr == appStatus.appLabelPtr);
           EXPECT_EQ(t.pin1_replaced, appStatus.pin1Replaced);
           EXPECT_EQ(t.pin1, (int)appStatus.pin1);
           EXPECT_EQ(t.pin2, (int)appStatus.pin2);
       }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_2::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_2;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getIccCardStatusResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDataRegistrationStateResponse_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataRegistrationStateResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_DataRegistrationStateResponse_V1_4 testVector = {
        RIL_RegState::RIL_REG_DENIED,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        7, 4, {},
        { true, true },
        { true, false, true },
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getDataRegistrationStateResponse_1_4(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_4::DataRegStateResult& dataRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                test(testVector, dataRegResponse);
            }
            return Void();
        }

        void test(const RIL_DataRegistrationStateResponse_V1_4& t, const V1_4::DataRegStateResult& dataRegResponse) {
            test(*((RIL_DataRegistrationStateResponse_V1_2 *)&t), dataRegResponse.base);
            EXPECT_EQ(t.lteVopsInfo.isVopsSupported, dataRegResponse.vopsInfo.lteVopsInfo().isVopsSupported);
            EXPECT_EQ(t.nrIndicators.isEndcAvailable, dataRegResponse.nrIndicators.isEndcAvailable);
            EXPECT_EQ(t.nrIndicators.isDcNrRestricted, dataRegResponse.nrIndicators.isDcNrRestricted);
            EXPECT_EQ(t.nrIndicators.isNrAvailable, dataRegResponse.nrIndicators.isNrAvailable);
        }

        void test(const RIL_DataRegistrationStateResponse_V1_2& t, const V1_2::DataRegStateResult& dataRegResponse) {
            EXPECT_EQ(t.regState, (int)dataRegResponse.regState);
            EXPECT_EQ(t.rat, dataRegResponse.rat);
            EXPECT_EQ(t.reasonDataDenied, dataRegResponse.reasonDataDenied);
            EXPECT_EQ(t.maxDataCalls, dataRegResponse.maxDataCalls);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getDataRegistrationStateResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::getDataRegistrationStateResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDataRegistrationStateResponse_1_4_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataRegistrationStateResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_DataRegistrationStateResponse_V1_4 testVector = {
        RIL_RegState::RIL_REG_DENIED,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        7, 4, {},
        { true, true },
        { true, false, true },
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_2 {
        Return<void> getDataRegistrationStateResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_2::DataRegStateResult& dataRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);

            RIL_DataRegistrationStateResponse_V1_2 dataRegResult;
            RadioConvUtils::convertDataRegistrationStateResult(dataRegResult,
                    testVector);

            EXPECT_EQ(dataRegResult.regState, (int)dataRegResponse.regState);
            EXPECT_EQ(dataRegResult.rat, dataRegResponse.rat);
            EXPECT_EQ(dataRegResult.reasonDataDenied, dataRegResponse.reasonDataDenied);
            EXPECT_EQ(dataRegResult.maxDataCalls, dataRegResponse.maxDataCalls);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_2::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_2;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getDataRegistrationStateResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getSignalStrengthResponse_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSignalStrengthResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SignalStrength_V1_4 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
        {}, // NR
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getSignalStrengthResponse_1_4(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_4::SignalStrength& signalStrength) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.signalStrength,
                          signalStrength.lte.signalStrength);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.rsrp,
                          signalStrength.lte.rsrp);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.rsrq,
                          signalStrength.lte.rsrq);
                EXPECT_EQ(testVector.LTE_SignalStrength.rssnr,
                        signalStrength.lte.rssnr);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.cqi,
                          signalStrength.lte.cqi);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ testVector.LTE_SignalStrength.timingAdvance,
                          signalStrength.lte.timingAdvance);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getSignalStrengthResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::getSignalStrengthResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getSignalStrengthResponse_1_4_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getSignalStrengthResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SignalStrength_V1_4 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
        {}, // NR
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_2 {
        Return<void> getSignalStrengthResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_2::SignalStrength& signalStrength) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);

            RIL_SignalStrength_V1_2 ss;
            RadioConvUtils::convertSignalStrengthResult(ss, testVector);

            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.signalStrength,
                      signalStrength.lte.signalStrength);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.rsrp,
                      signalStrength.lte.rsrp);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.rsrq,
                      signalStrength.lte.rsrq);
            EXPECT_EQ(ss.LTE_SignalStrength.rssnr,
                    signalStrength.lte.rssnr);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.cqi,
                      signalStrength.lte.cqi);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ ss.LTE_SignalStrength.timingAdvance,
                      signalStrength.lte.timingAdvance);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_2::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_2;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getSignalStrengthResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setupDataCallResponse_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setupDataCallResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setupDataCallResponse_1_4(
                const RadioResponseInfo& info,
                const android::hardware::radio::V1_4::SetupDataCallResult& /*dcResponse*/) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::INVALID_RESPONSE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setupDataCallResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDataCallListResponse_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataCallListResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_Data_Call_Response_V1_4 testVector[] = {
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getDataCallListResponse_1_4(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<
                        ::android::hardware::radio::V1_4::SetupDataCallResult>& dcResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                for (size_t i = 0; i < dcResponse.size(); i++) {
                    test(testVector[i], dcResponse[i]);
                }
            }
            return Void();
        }

        void test(const RIL_Data_Call_Response_V1_4& /*t*/, const V1_4::SetupDataCallResult& /*dcResponse*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getDataCallListResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(RIL_Data_Call_Response_V1_4));

    radio::getDataCallListResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setPreferredNetworkTypeBitmapResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setPreferredNetworkTypeBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setPreferredNetworkTypeBitmapResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setPreferredNetworkTypeBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getPreferredNetworkTypeBitmapResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getPreferredNetworkTypeBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 0x7FF };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getPreferredNetworkTypeBitmapResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_bitfield<
                        ::android::hardware::radio::V1_4::RadioAccessFamily>
                        networkTypeBitmap) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], (int)networkTypeBitmap);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getPreferredNetworkTypeBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getPreferredNetworkTypeBitmapResponse_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getPreferredNetworkTypeBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 0x7FF };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_2 {
        Return<void> getPreferredNetworkTypeResponse(const RadioResponseInfo& info,
                PreferredNetworkType nwType) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(RadioConvUtils::getNetworkTypeFromRaf(testVector[0]),
                    (int)nwType);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_2::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_2;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getPreferredNetworkTypeBitmapResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCellInfoListResponse_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCellInfoListResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_4 testVector[] = {
        // TODO
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCellInfoListResponse_1_4(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>&
                        cellInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                for (size_t i = 0; i < cellInfo.size(); i++) {
                    test(testVector[i], cellInfo[i]);
                }
            }
            return Void();
        }

        void test(const RIL_CellInfo_V1_4& /*t*/, const V1_4::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getCellInfoListResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getCellInfoListResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCellInfoListResponse_1_4_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCellInfoListResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_4 testVector[] = {
        // TODO
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_2 {
        Return<void> getCellInfoListResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>&
                        cellInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                for (size_t i = 0; i < cellInfo.size(); i++) {
                    RIL_CellInfo_V1_2 cellInfoV1_2 {};
                    RadioConvUtils::convertCellInfo(cellInfoV1_2, testVector[i]);
                    test(cellInfoV1_2, cellInfo[i]);
                }
            }
            return Void();
        }

        void test(const RIL_CellInfo_V1_2& /*t*/, const V1_2::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_2::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_2;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getCellInfoListResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setAllowedCarriersResponse_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setAllowedCarriersResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setAllowedCarriersResponse_1_4(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setAllowedCarriersResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getAllowedCarriersResponse_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getAllowedCarriersResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    RIL_Carrier testAllowedCarriers[] = {
        { (char *)"001", (char *)"01", RIL_CarrierMatchType::RIL_MATCH_ALL, (char *)"0A1B" }
    };
    RIL_Carrier testExcludedCarriers[] = {
        { (char *)"001", (char *)"01", RIL_CarrierMatchType::RIL_MATCH_ALL, (char *)"0A1B" }
    };
    static RIL_CarrierRestrictions_V1_4 testVector = {
        { 1, 1, testAllowedCarriers, testExcludedCarriers, 1 }, 1
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getAllowedCarriersResponse_1_4(const RadioResponseInfo& info,
                const CarrierRestrictionsWithPriority&,
                SimLockMultiSimPolicy) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    radio::getAllowedCarriersResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
                RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getAllowedCarriersResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
                RIL_E_SUCCESS, &testVector, sizeof(testVector) + 1);

    radio::getAllowedCarriersResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, startNetworkScanResponse_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::startNetworkScanResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> startNetworkScanResponse_1_4(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::startNetworkScanResponse_1_4(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, emergencyDialResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::emergencyDialResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> emergencyDialResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::emergencyDialResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

// V1_5::IRadioResponse
TEST(ril_service_legacy_rsp_tests, setSignalStrengthReportingCriteriaResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setSignalStrengthReportingCriteriaResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setSignalStrengthReportingCriteriaResponse_1_5(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setSignalStrengthReportingCriteriaResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, enableUiccApplicationsResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::enableUiccApplicationsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> enableUiccApplicationsResponse(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::enableUiccApplicationsResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, areUiccApplicationsEnabledResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::areUiccApplicationsEnabledResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 1 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> areUiccApplicationsEnabledResponse(const RadioResponseInfo& info, bool enabled) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector[0], enabled);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::areUiccApplicationsEnabledResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setSystemSelectionChannelsResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setSystemSelectionChannelsResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setSystemSelectionChannelsResponse_1_5(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setSystemSelectionChannelsResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, startNetworkScanResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::startNetworkScanResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> startNetworkScanResponse_1_5(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::startNetworkScanResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setupDataCallResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setupDataCallResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_Data_Call_Response_v11 testVector = {};

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setupDataCallResponse_1_5(
                const RadioResponseInfo& info,
                const android::hardware::radio::V1_5::SetupDataCallResult& /*dcResponse*/) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setupDataCallResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::setupDataCallResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setInitialAttachApnResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setInitialAttachApnResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setInitialAttachApnResponse_1_5(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setInitialAttachApnResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setDataProfileResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setDataProfileResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setDataProfileResponse_1_5(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setDataProfileResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setRadioPowerResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setRadioPowerResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setRadioPowerResponse_1_5(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setRadioPowerResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setIndicationFilterResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setIndicationFilterResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setIndicationFilterResponse_1_5(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setIndicationFilterResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getBarringInfoResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getBarringInfoResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_BarringInfo barringInfos[] = {
        { RIL_BarringServiceType::CS_SERVICE, RIL_BarringType::BARRING_TYPE_CONDITIONAL, {} },
        { RIL_BarringServiceType::PS_SERVICE, RIL_BarringType::BARRING_TYPE_UNCONDITIONAL, {} },
    };
    static RIL_CellIdAndBarringInfo testVector = {
        {},
        sizeof(barringInfos) / sizeof(barringInfos[0]),
        barringInfos,
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getBarringInfoResponse(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>&
                        barringInfos) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::REQUEST_NOT_SUPPORTED);
            if (info.error == RadioError::NONE) {
                test(testVector.cellIdentity, cellIdentity);
                EXPECT_EQ(testVector.numOfrecord, barringInfos.size());
                for (size_t i = 0; i < barringInfos.size(); i++) {
                    test(testVector.precords[i], barringInfos[i]);
                }
            }
            return Void();
        }

        void test(const RIL_CellIdentity_V1_5& /*t*/, const V1_5::CellIdentity& /*cellIdentity*/) {
            // TODO
        }

        void test(const RIL_BarringInfo& t, const V1_5::BarringInfo& barringInfo) {
            EXPECT_EQ(t.service, (int)barringInfo.serviceType);
            EXPECT_EQ(t.type, (int)barringInfo.barringType);
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getBarringInfoResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::getBarringInfoResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getVoiceRegistrationStateResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getVoiceRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RegStateResult testVector = {
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
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_5::RegStateResult& regResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                test(testVector, regResponse);
            }
            return Void();
        }

        void test(const RIL_RegStateResult& t, const V1_5::RegStateResult& regResponse) {
            EXPECT_EQ(t.regState, (int)regResponse.regState);
            EXPECT_EQ(t.rat, (int)regResponse.rat);
            EXPECT_EQ(t.reasonForDenial, (int)regResponse.reasonForDenial);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getVoiceRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::getVoiceRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getVoiceRegistrationStateResponse_1_5_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getVoiceRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RegStateResult testVector = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        RIL_RegistrationFailCause::REG_FAIL_CAUSE_NONE,
        {},
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_4 {
        Return<void> getVoiceRegistrationStateResponse_1_2(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_2::VoiceRegStateResult& voiceRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);

            RIL_VoiceRegistrationStateResponse_V1_2 voiceRegResult;
            RadioConvUtils::convertVoiceRegistrationStateResult(voiceRegResult,
                    testVector);

            EXPECT_EQ(voiceRegResult.regState, (int)voiceRegResponse.regState);
            EXPECT_EQ(voiceRegResult.rat, voiceRegResponse.rat);
            EXPECT_EQ(voiceRegResult.cssSupported, voiceRegResponse.cssSupported);
            EXPECT_EQ(voiceRegResult.roamingIndicator, voiceRegResponse.roamingIndicator);
            EXPECT_EQ(voiceRegResult.systemIsInPrl, voiceRegResponse.systemIsInPrl);
            EXPECT_EQ(voiceRegResult.defaultRoamingIndicator, voiceRegResponse.defaultRoamingIndicator);
            EXPECT_EQ(voiceRegResult.reasonForDenial, voiceRegResponse.reasonForDenial);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_4::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_4;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getVoiceRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDataRegistrationStateResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RegStateResult testVector = {
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
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_5::RegStateResult& regResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                test(testVector, regResponse);
            }
            return Void();
        }

        void test(const RIL_RegStateResult& t, const V1_5::RegStateResult& regResponse) {
            EXPECT_EQ(t.regState, (int)regResponse.regState);
            EXPECT_EQ(t.rat, (int)regResponse.rat);
            EXPECT_EQ(t.reasonForDenial, (int)regResponse.reasonForDenial);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getDataRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);

    radio::getDataRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getDataRegistrationStateResponse_1_5_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_RegStateResult testVector = {
        RIL_RegState::RIL_REG_HOME,
        RIL_RadioTechnology::RADIO_TECH_LTE,
        RIL_RegistrationFailCause::REG_FAIL_CAUSE_NONE,
        {},
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_4 {
        Return<void> getDataRegistrationStateResponse_1_4(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_4::DataRegStateResult& dataRegResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            RIL_DataRegistrationStateResponse_V1_4 dataRegResult;
            RadioConvUtils::convertDataRegistrationStateResult(dataRegResult,
                    testVector);
            test(dataRegResult, dataRegResponse);
            return Void();
        }

        void test(const RIL_DataRegistrationStateResponse_V1_4& t, const V1_4::DataRegStateResult& dataRegResponse) {
            test(*((RIL_DataRegistrationStateResponse_V1_2 *)&t), dataRegResponse.base);
            EXPECT_EQ(t.lteVopsInfo.isVopsSupported, dataRegResponse.vopsInfo.lteVopsInfo().isVopsSupported);
            EXPECT_EQ(t.nrIndicators.isEndcAvailable, dataRegResponse.nrIndicators.isEndcAvailable);
            EXPECT_EQ(t.nrIndicators.isDcNrRestricted, dataRegResponse.nrIndicators.isDcNrRestricted);
            EXPECT_EQ(t.nrIndicators.isNrAvailable, dataRegResponse.nrIndicators.isNrAvailable);
        }

        void test(const RIL_DataRegistrationStateResponse_V1_2& t, const V1_2::DataRegStateResult& dataRegResponse) {
            EXPECT_EQ(t.regState, (int)dataRegResponse.regState);
            EXPECT_EQ(t.rat, dataRegResponse.rat);
            EXPECT_EQ(t.reasonDataDenied, dataRegResponse.reasonDataDenied);
            EXPECT_EQ(t.maxDataCalls, dataRegResponse.maxDataCalls);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_4::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_4;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getDataRegistrationStateResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setNetworkSelectionModeManualResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setNetworkSelectionModeManualResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setNetworkSelectionModeManualResponse_1_5(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setNetworkSelectionModeManualResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, sendCdmaSmsExpectMoreResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::sendCdmaSmsExpectMoreResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_SMS_Response testVector = {
        200, (char *)"", 0
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> sendCdmaSmsExpectMoreResponse(const RadioResponseInfo& info,
                const SendSmsResult& sms) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            EXPECT_EQ(testVector.messageRef, sms.messageRef);
            EXPECT_TRUE(testVector.ackPDU == sms.ackPDU);
            EXPECT_EQ(testVector.errorCode, sms.errorCode);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::sendCdmaSmsExpectMoreResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, supplySimDepersonalizationResponse) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::supplySimDepersonalizationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static int testVector[] = { 2, 9 };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> supplySimDepersonalizationResponse(
                const RadioResponseInfo& info,
                ::android::hardware::radio::V1_5::PersoSubstate persoType, int32_t remainingRetries) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE || info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                EXPECT_EQ(testVector[0], (int)persoType);
                EXPECT_EQ(testVector[1], remainingRetries);
            }
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::supplySimDepersonalizationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    radio::supplySimDepersonalizationResponse(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, setLinkCapacityReportingCriteriaResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::setLinkCapacityReportingCriteriaResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> setLinkCapacityReportingCriteriaResponse_1_5(const RadioResponseInfo& info) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::setLinkCapacityReportingCriteriaResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCellInfoListResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCellInfoListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_5 testVector[] = {
        // TODO
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getCellInfoListResponse_1_5(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                        cellInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE || info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                for (size_t i = 0; i < cellInfo.size(); i++) {
                    test(testVector[i], cellInfo[i]);
                }
            }
            return Void();
        }

        void test(const RIL_CellInfo_V1_5& /*t*/, const V1_5::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getCellInfoListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(RIL_CellInfo_V1_5));

    radio::getCellInfoListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getCellInfoListResponse_1_5_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getCellInfoListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_5 testVector[] = {
        // TODO
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_4 {
        Return<void> getCellInfoListResponse_1_4(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>&
                        cellInfo) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            for (size_t i = 0; i < cellInfo.size(); i++) {
                RIL_CellInfo_V1_4 cellInfoV1_4 = {};
                RadioConvUtils::convertCellInfo(cellInfoV1_4, testVector[i]);
                test(cellInfoV1_4, cellInfo[i]);
            }
            return Void();
        }

        void test(const RIL_CellInfo_V1_4& /*t*/, const V1_4::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_4::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_4;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getCellInfoListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getIccCardStatusResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getIccCardStatusResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CardStatus_V1_4 testVector = {
        RIL_CardState::RIL_CARDSTATE_PRESENT,
        RIL_PinState::RIL_PINSTATE_ENABLED_VERIFIED,
        0, -1, -1, 1,
        {
            {
                RIL_AppType::RIL_APPTYPE_USIM,
                RIL_AppState::RIL_APPSTATE_READY,
                RIL_PersoSubstate::RIL_PERSOSUBSTATE_UNKNOWN,
                (char *)"aid1234",
                (char *)"",
                0,
                RIL_PinState::RIL_PINSTATE_DISABLED,
                RIL_PinState::RIL_PINSTATE_DISABLED
            }
        },
        0,
        (char *)"atr_1234",
        (char *)"812347889123",
        (char *)"eid12345678",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getIccCardStatusResponse_1_5(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_5::CardStatus& cardStatus) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE || info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                test(testVector, cardStatus.base);
            }
            return Void();
        }

        void test(const RIL_CardStatus_V1_4& t, const V1_4::CardStatus& cardStatus) {
            test(*((RIL_CardStatus_V1_2 *)&t), cardStatus.base);
            EXPECT_TRUE(t.eid == cardStatus.eid);
        }

        void test(const RIL_CardStatus_V1_2& t, const V1_2::CardStatus& cardStatus) {
            test(*((RIL_CardStatus_v6 *)&t), cardStatus.base);
            EXPECT_EQ(t.physicalSlotId, cardStatus.physicalSlotId);
            EXPECT_TRUE(t.atr == cardStatus.atr);
            EXPECT_TRUE(t.iccid == cardStatus.iccid);
        }

        void test(const RIL_CardStatus_v6& t, const V1_0::CardStatus& cardStatus) {
            EXPECT_EQ(t.card_state, (int)cardStatus.cardState);
            EXPECT_EQ(t.universal_pin_state, (int)cardStatus.universalPinState);
            EXPECT_EQ(t.gsm_umts_subscription_app_index, cardStatus.gsmUmtsSubscriptionAppIndex);
            EXPECT_EQ(t.cdma_subscription_app_index, cardStatus.cdmaSubscriptionAppIndex);
            EXPECT_EQ(t.ims_subscription_app_index, cardStatus.imsSubscriptionAppIndex);
            if (cardStatus.applications.size() > 0) {
                EXPECT_EQ((unsigned long)/* NEED TO CHECK */ t.num_applications, cardStatus.applications.size());
                for (size_t i = 0; i < cardStatus.applications.size(); i++) {
                    test(t.applications[i], cardStatus.applications[i]);
                }
            }
        }

        void test(const RIL_AppStatus& t, const V1_0::AppStatus& appStatus) {
            EXPECT_EQ(t.app_type, (int)appStatus.appType);
            EXPECT_EQ(t.app_state, (int)appStatus.appState);
            EXPECT_EQ(t.perso_substate, (int)appStatus.persoSubstate);
            EXPECT_TRUE(t.aid_ptr == appStatus.aidPtr);
            EXPECT_TRUE(t.app_label_ptr == appStatus.appLabelPtr);
            EXPECT_EQ(t.pin1_replaced, appStatus.pin1Replaced);
            EXPECT_EQ(t.pin1, (int)appStatus.pin1);
            EXPECT_EQ(t.pin2, (int)appStatus.pin2);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    RIL_CardStatus_V1_4 invalidParams = {
        .gsm_umts_subscription_app_index = 1,
        .cdma_subscription_app_index = 2,
        .ims_subscription_app_index = 3,
        .num_applications = 1,
    };
    radio::getIccCardStatusResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &invalidParams, sizeof(invalidParams));

    radio::getIccCardStatusResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, getIccCardStatusResponse_1_5_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getIccCardStatusResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CardStatus_V1_4 testVector = {
        RIL_CardState::RIL_CARDSTATE_PRESENT,
        RIL_PinState::RIL_PINSTATE_ENABLED_VERIFIED,
        0, -1, -1, 1,
        {
            {
                RIL_AppType::RIL_APPTYPE_USIM,
                RIL_AppState::RIL_APPSTATE_READY,
                RIL_PersoSubstate::RIL_PERSOSUBSTATE_UNKNOWN,
                (char *)"aid1234",
                (char *)"",
                0,
                RIL_PinState::RIL_PINSTATE_DISABLED,
                RIL_PinState::RIL_PINSTATE_DISABLED
            }
        },
        0,
        (char *)"atr_1234",
        (char *)"812347889123",
        (char *)"eid12345678",
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_4 {
        Return<void> getIccCardStatusResponse_1_4(
                const RadioResponseInfo& info,
                const ::android::hardware::radio::V1_4::CardStatus& cardStatus) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_EQ(info.error, RadioError::NONE);
            test(testVector, cardStatus);
            return Void();
        }

        void test(const RIL_CardStatus_V1_4& t, const V1_4::CardStatus& cardStatus) {
            test(*((RIL_CardStatus_V1_2 *)&t), cardStatus.base);
            EXPECT_TRUE(t.eid == cardStatus.eid);
        }

        void test(const RIL_CardStatus_V1_2& t, const V1_2::CardStatus& cardStatus) {
            test(*((RIL_CardStatus_v6 *)&t), cardStatus.base);
            EXPECT_EQ(t.physicalSlotId, cardStatus.physicalSlotId);
            EXPECT_TRUE(t.atr == cardStatus.atr);
            EXPECT_TRUE(t.iccid == cardStatus.iccid);
        }

        void test(const RIL_CardStatus_v6& t, const V1_0::CardStatus& cardStatus) {
            EXPECT_EQ(t.card_state, (int)cardStatus.cardState);
            EXPECT_EQ(t.universal_pin_state, (int)cardStatus.universalPinState);
            EXPECT_EQ(t.gsm_umts_subscription_app_index, cardStatus.gsmUmtsSubscriptionAppIndex);
            EXPECT_EQ(t.cdma_subscription_app_index, cardStatus.cdmaSubscriptionAppIndex);
            EXPECT_EQ(t.ims_subscription_app_index, cardStatus.imsSubscriptionAppIndex);
            if (cardStatus.applications.size() > 0) {
                EXPECT_EQ((unsigned long)/* NEED TO CHECK */ t.num_applications, cardStatus.applications.size());
                for (size_t i = 0; i < cardStatus.applications.size(); i++) {
                    test(t.applications[i], cardStatus.applications[i]);
                }
            }
        }

        void test(const RIL_AppStatus& t, const V1_0::AppStatus& appStatus) {
            EXPECT_EQ(t.app_type, (int)appStatus.appType);
            EXPECT_EQ(t.app_state, (int)appStatus.appState);
            EXPECT_EQ(t.perso_substate, (int)appStatus.persoSubstate);
            EXPECT_TRUE(t.aid_ptr == appStatus.aidPtr);
            EXPECT_TRUE(t.app_label_ptr == appStatus.appLabelPtr);
            EXPECT_EQ(t.pin1_replaced, appStatus.pin1Replaced);
            EXPECT_EQ(t.pin1, (int)appStatus.pin1);
            EXPECT_EQ(t.pin2, (int)appStatus.pin2);
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_4::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_4;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getIccCardStatusResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}


TEST(ril_service_legacy_rsp_tests, getDataCallListResponse_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioResponse == NULL
    radio::getDataCallListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, 0,
            RIL_E_INTERNAL_ERR, 0, 0);

    static int32_t serial = GetRandomSerialNumber();
    static RIL_Data_Call_Response_v11 testVector[] = {
        {},
        {},
    };

    // test with mRadioResponse != NULL
    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        Return<void> getDataCallListResponse_1_5(
                const RadioResponseInfo& info,
                const hidl_vec<::android::hardware::radio::V1_5::SetupDataCallResult>& dcResponse) {
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE ||
                    info.error == RadioError::INVALID_RESPONSE);
            if (info.error == RadioError::NONE) {
                for (size_t i = 0; i < dcResponse.size(); i++) {
                    test(testVector[i], dcResponse[i]);
                }
            }
            return Void();
        }

        void test(const RIL_Data_Call_Response_v11& /*t*/,
                const V1_5::SetupDataCallResult& /*dcResponse*/) {
            // TODO
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::getDataCallListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, 0, sizeof(RIL_Data_Call_Response_v11));

    radio::getDataCallListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, remote_died) {
    sp<V1_5::IRadio> radio = setupRadio();

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_5 testVector[] = {
        // TODO
        {},
        {},
    };

    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        bool called = false;
        Return<void> getCellInfoListResponse_1_5(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                        /*cellInfo*/) {
            called = true;
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE || info.error == RadioError::INVALID_RESPONSE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;

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
    radio::getCellInfoListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    EXPECT_FALSE(responseCallback->called);

    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getCellInfoListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    EXPECT_TRUE(responseCallback->called);
    tearDown(radio);
}

TEST(ril_service_legacy_rsp_tests, checkReturnStatus_not_okay) {
    sp<V1_5::IRadio> radio = setupRadio();

    static int32_t serial = GetRandomSerialNumber();
    static RIL_CellInfo_V1_5 testVector[] = {
        // TODO
        {},
        {},
    };

    struct TestRadioResponseImpl : public TestRadioResponseBase_1_5 {
        bool called = false;
        Return<void> getCellInfoListResponse_1_5(
                const RadioResponseInfo& info,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                        /*cellInfo*/) {
            called = true;
            EXPECT_EQ(info.type, RadioResponseType::SOLICITED);
            EXPECT_EQ(info.serial, serial);
            EXPECT_TRUE(info.error == RadioError::NONE || info.error == RadioError::INVALID_RESPONSE);
            return Void();
        }
    };
    sp<TestRadioResponseImpl> responseCallback = new TestRadioResponseImpl;
    sp<V1_5::IRadioIndication> indicationCallback = new TestRadioIndicationBase_1_5;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::getCellInfoListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    EXPECT_TRUE(responseCallback->called);

    // Return<void> is not okay.
    responseCallback->called = false;
    Return<void> ret(Status::fromExceptionCode(Status::EX_TRANSACTION_FAILED));
    radio::impl::legacy::checkReturnStatus(RIL_SOCKET_1, ret);

    radio::getCellInfoListResponse_1_5(RIL_SOCKET_1, RESPONSE_SOLICITED, serial,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    EXPECT_TRUE(responseCallback->called);
    tearDown(radio);
}
