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
 * ril_service_legacy_indication_test.cpp
 *
 *  Created on: 2021. 7. 21.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include <testutils.h>
#include <telephony/ril_mcc.h>
#include <telephony/ril_mnc.h>
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

typedef int (*RadioIndicationHandler)(int, int, int, RIL_Errno, void *, size_t);

TEST(ril_service_legacy, radio_1_0_indication) {
    radio::impl::legacy::initRadios();

    RadioIndicationHandler testVector[] = {
        radio::radioStateChangedInd,
        radio::callStateChangedInd,
        radio::networkStateChangedInd,
        radio::newSmsInd,
        radio::newSmsStatusReportInd,
        radio::newSmsOnSimInd,
        radio::onUssdInd,
        radio::nitzTimeReceivedInd,
        radio::currentSignalStrengthInd,
        radio::dataCallListChangedInd,
        radio::suppSvcNotifyInd,
        radio::stkSessionEndInd,
        radio::stkProactiveCommandInd,
        radio::stkEventNotifyInd,
        radio::stkCallSetupInd,
        radio::simSmsStorageFullInd,
        radio::simRefreshInd,
        radio::callRingInd,
        radio::simStatusChangedInd,
        radio::cdmaNewSmsInd,
        radio::newBroadcastSmsInd,
        radio::cdmaRuimSmsStorageFullInd,
        radio::restrictedStateChangedInd,
        radio::enterEmergencyCallbackModeInd,
        radio::cdmaCallWaitingInd,
        radio::cdmaOtaProvisionStatusInd,
        radio::cdmaInfoRecInd,
        radio::indicateRingbackToneInd,
        radio::resendIncallMuteInd,
        radio::cdmaSubscriptionSourceChangedInd,
        radio::cdmaPrlChangedInd,
        radio::exitEmergencyCallbackModeInd,
        radio::rilConnectedInd,
        radio::voiceRadioTechChangedInd,
        radio::cellInfoListInd,
        radio::imsNetworkStateChangedInd,
        radio::subscriptionStatusChangedInd,
        radio::srvccStateNotifyInd,
        radio::hardwareConfigChangedInd,
        radio::radioCapabilityIndicationInd,
        radio::onSupplementaryServiceIndicationInd,
        radio::stkCallControlAlphaNotifyInd,
        radio::lceDataInd,
        radio::pcoDataInd,
        radio::modemResetInd,
        radio::networkScanResultInd,
        radio::keepaliveStatusInd,
        radio::currentPhysicalChannelConfigs,
        radio::sendRequestRawResponse,
        radio::sendRequestStringsResponse,
        radio::setCarrierInfoForImsiEncryptionResponse,
        radio::carrierInfoForImsiEncryption,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        RadioIndicationHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_UNSOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_service_legacy, radio_1_2_indication) {
    radio::impl::legacy::initRadios();

    RadioIndicationHandler testVector[] = {
        radio::currentSignalStrengthInd_1_2,
        radio::cellInfoListInd_1_2,
        radio::networkScanResultInd_1_2,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        RadioIndicationHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_UNSOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_service_legacy, radio_1_4_indication) {
    radio::impl::legacy::initRadios();

    RadioIndicationHandler testVector[] = {
        radio::currentSignalStrengthInd_1_4,
        radio::dataCallListChangedInd_1_4,
        radio::cellInfoListInd_1_4,
        radio::networkScanResultInd_1_4,
        radio::currentPhysicalChannelConfigs_1_4,
        radio::currentEmergencyNumberListInd,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        RadioIndicationHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_UNSOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_service_legacy, radio_1_5_indication) {
    radio::impl::legacy::initRadios();

    RadioIndicationHandler testVector[] = {
        radio::uiccApplicationsEnablementChangedInd,
        radio::registrationFailedInd,
        radio::barringInfoChangedInd,
        radio::cellInfoListInd_1_5,
        radio::networkScanResultInd_1_5,
        radio::dataCallListChangedInd_1_5,
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

TEST(ril_service_legacy_ind_tests, radioStateChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::radioStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> radioStateChanged(RadioIndicationType type,
                RadioState radioState) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(radioState, RadioState::ON);

            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::radioStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, callStateChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::callStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> callStateChanged(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);

            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::callStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, networkStateChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> networkStateChanged(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);

            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::networkStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, newSmsInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::newSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static char testVector[] = "1A2B3C4D5E6F";
    static uint8_t testPdu[] = { 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F };
    static size_t testPduLen = sizeof(testPdu) / sizeof(testPdu[0]);
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> newSms(RadioIndicationType type,
                const ::android::hardware::hidl_vec<uint8_t>& pdu) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            hidl_vec<uint8_t> tmp;
            tmp.setToExternal(testPdu, testPduLen);
            EXPECT_TRUE(pdu == tmp);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::newSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    radio::newSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));

    // test
    radio::newSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, strlen(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, newSmsStatusReportInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::newSmsStatusReportInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static char testVector[] = "1A2B3C4D5E6F";
    static uint8_t testPdu[] = { 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F };
    static size_t testPduLen = sizeof(testPdu) / sizeof(testPdu[0]);
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> newSmsStatusReport(RadioIndicationType type,
                const ::android::hardware::hidl_vec<uint8_t>& pdu) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            hidl_vec<uint8_t> tmp;
            tmp.setToExternal(testPdu, testPduLen);
            EXPECT_TRUE(pdu == tmp);

            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::newSmsStatusReportInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    radio::newSmsStatusReportInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));

    // test
    radio::newSmsStatusReportInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, strlen(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, newSmsOnSimInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::newSmsOnSimInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int testRecordNumber = 3;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> newSmsOnSim(RadioIndicationType type, int32_t recordNumber) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testRecordNumber, recordNumber);

            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::newSmsOnSimInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::newSmsOnSimInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testRecordNumber, sizeof(testRecordNumber));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, onUssdInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::onUssdInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static const char *testUssd[] = {
        "1",
        "ussd_ind_test",
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> onUssd(RadioIndicationType type, UssdModeType modeType,
                const ::android::hardware::hidl_string& msg) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(std::stoi(testUssd[0]), (int)modeType);
            EXPECT_TRUE(testUssd[1] == msg);

            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::onUssdInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::onUssdInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testUssd, sizeof(testUssd));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, nitzTimeReceivedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::nitzTimeReceivedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static char testNitz[] = "21/07/21,05:48:15+40,00";
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> nitzTimeReceived(RadioIndicationType type,
                const ::android::hardware::hidl_string& nitzTime,
                uint64_t) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testNitz, nitzTime);

            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::nitzTimeReceivedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::nitzTimeReceivedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testNitz, sizeof(testNitz));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, currentSignalStrengthInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentSignalStrengthInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_SignalStrength_v10 testSignalStrength = {
        {}, // GW
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> currentSignalStrength(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_0::SignalStrength& signalStrength) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ((unsigned int)/* NEED To CHECK */ testSignalStrength.LTE_SignalStrength.signalStrength,
                      signalStrength.lte.signalStrength);
            EXPECT_EQ((unsigned int)/* NEED To CHECK */ testSignalStrength.LTE_SignalStrength.rsrp,
                      signalStrength.lte.rsrp);
            EXPECT_EQ((unsigned int)/* NEED To CHECK */ testSignalStrength.LTE_SignalStrength.rsrq,
                      signalStrength.lte.rsrq);
            EXPECT_EQ(testSignalStrength.LTE_SignalStrength.rssnr,
                    signalStrength.lte.rssnr);
            EXPECT_EQ((unsigned int)/* NEED To CHECK */ testSignalStrength.LTE_SignalStrength.cqi,
                      signalStrength.lte.cqi);
            EXPECT_EQ((unsigned int)/* NEED To CHECK */ testSignalStrength.LTE_SignalStrength.timingAdvance,
                      signalStrength.lte.timingAdvance);

            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::currentSignalStrengthInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::currentSignalStrengthInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testSignalStrength, sizeof(testSignalStrength));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, dataCallListChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::dataCallListChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_Data_Call_Response_v11 testVector[] = {
        { 0, -1, 1, 2, "IP", (char *)"rmnet0", (char *)"192.164.0.2", (char *)"192.164.0.1",
                (char *)"192.164.0.1", (char *)"", 1500 },
        { 0, -1, 2, 2, "IP", (char *)"rmnet1", (char *)"192.164.1.2", (char *)"192.164.1.1",
                (char *)"192.164.1.1", (char *)"10.1.2.4", 1500 },
    };
    static size_t testSize = sizeof(testVector) / sizeof(testVector[0]);
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> dataCallListChanged(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<
                        android::hardware::radio::V1_0::SetupDataCallResult>& dcList) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (dcList.size() > 0) {
                EXPECT_EQ(testSize, dcList.size());
                for (size_t i = 0; i < dcList.size(); i++) {
                    test(testVector[i], dcList[i]);
                }
            }
            return Void();
        }

        void test(const RIL_Data_Call_Response_v11& testDc,
                const V1_0::SetupDataCallResult& dcResult) {
            EXPECT_EQ(testDc.status, (int)dcResult.status);
            EXPECT_EQ(testDc.suggestedRetryTime, dcResult.suggestedRetryTime);
            EXPECT_EQ(testDc.cid, dcResult.cid);
            EXPECT_EQ(testDc.active, dcResult.active);
            EXPECT_TRUE(testDc.type == dcResult.type);
            EXPECT_TRUE(testDc.ifname == dcResult.ifname);
            EXPECT_TRUE(testDc.addresses == dcResult.addresses);
            EXPECT_TRUE(testDc.dnses == dcResult.dnses);
            EXPECT_TRUE(testDc.gateways == dcResult.gateways);
            EXPECT_TRUE(testDc.pcscf == dcResult.pcscf);
            EXPECT_EQ(testDc.mtu, dcResult.mtu);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    // nullptr with 0 length is a valud input for radio::dataCallListChangedInd
    radio::dataCallListChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(testVector) + 1);

    // test
    // an empty list
    radio::dataCallListChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    radio::dataCallListChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, suppSvcNotifyInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::suppSvcNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_SuppSvcNotification testVector = {
        1, 1, 2, 3, (char *)"12345678",
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> suppSvcNotify(RadioIndicationType type, const SuppSvcNotification& suppSvc) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.notificationType, suppSvc.isMT);
            EXPECT_EQ(testVector.code, suppSvc.code);
            EXPECT_EQ(testVector.index, suppSvc.index);
            EXPECT_TRUE(testVector.number == suppSvc.number);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::suppSvcNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::suppSvcNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, stkSessionEndInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::stkSessionEndInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> stkSessionEnd(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::stkSessionEndInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, stkProactiveCommandInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::stkProactiveCommandInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static char testVector[] = "stkProactiveCommandInd";
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> stkProactiveCommand(RadioIndicationType type,
                const ::android::hardware::hidl_string& cmd) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_TRUE(testVector == cmd);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::stkProactiveCommandInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::stkProactiveCommandInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, stkEventNotifyInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::stkEventNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static char testVector[] = "stkEventNotifyInd";
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> stkEventNotify(RadioIndicationType type,
                const ::android::hardware::hidl_string& cmd) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_TRUE(testVector == cmd);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::stkEventNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::stkEventNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, stkCallSetupInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::stkCallSetupInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int32_t testVector = 12345;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> stkCallSetup(RadioIndicationType type, int64_t timeout) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_TRUE(static_cast<int64_t>(testVector) == timeout);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::stkCallSetupInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::stkCallSetupInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, simSmsStorageFullInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::simSmsStorageFullInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> simSmsStorageFull(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::simSmsStorageFullInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, simRefreshInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::simRefreshInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_SimRefreshResponse_v7 testVector = {
        RIL_SimRefreshResult::SIM_INIT,
        125,
        (char *)"aid_12345678",
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> simRefresh(RadioIndicationType type, const SimRefreshResult& refreshResult) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.result, (int)refreshResult.type );
            EXPECT_EQ(testVector.ef_id, refreshResult.efId);
            EXPECT_TRUE(testVector.aid == refreshResult.aid);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::simRefreshInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::simRefreshInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, callRingInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::callRingInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CDMA_SignalInfoRecord testVector = { 1, 2, 3, 4 };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> callRing(RadioIndicationType type, bool isGsm, const CdmaSignalInfoRecord& record) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (isGsm) {
                EXPECT_EQ(0, record.isPresent);
                EXPECT_EQ(0, record.signalType);
                EXPECT_EQ(0, record.alertPitch);
                EXPECT_EQ(0, record.signal);
            } else {
                EXPECT_EQ(testVector.isPresent, record.isPresent);
                EXPECT_EQ(testVector.signalType, record.signalType);
                EXPECT_EQ(testVector.alertPitch, record.alertPitch);
                EXPECT_EQ(testVector.signal, record.signal);
            }
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::callRingInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector) + 1);

    // isGsm true
    radio::callRingInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // isGsm false
    radio::callRingInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, simStatusChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::simStatusChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> simStatusChanged(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::simStatusChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cdmaNewSmsInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cdmaNewSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CDMA_SMS_Message testVector = {

    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> cdmaNewSms(RadioIndicationType type, const CdmaSmsMessage& msg) {
            hidl_vec<uint8_t> tmp;
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.uTeleserviceID, msg.teleserviceId);
            EXPECT_EQ(testVector.bIsServicePresent, msg.isServicePresent);
            EXPECT_EQ(testVector.uServicecategory, msg.serviceCategory);
            EXPECT_EQ(testVector.sAddress.digit_mode, (int)msg.address.digitMode);
            EXPECT_EQ(testVector.sAddress.number_mode, (int)msg.address.numberMode);
            EXPECT_EQ(testVector.sAddress.number_type, (int)msg.address.numberType);
            EXPECT_EQ(testVector.sAddress.number_plan, (int)msg.address.numberPlan);
            tmp.setToExternal(testVector.sAddress.digits, testVector.sAddress.number_of_digits);
            EXPECT_TRUE(tmp == msg.address.digits);
            EXPECT_EQ(testVector.sSubAddress.subaddressType, (int)msg.subAddress.subaddressType);
            EXPECT_EQ(testVector.sSubAddress.odd, msg.subAddress.odd);
            tmp.setToExternal(testVector.sSubAddress.digits, testVector.sSubAddress.number_of_digits);
            EXPECT_TRUE(tmp == msg.subAddress.digits);
            tmp.setToExternal(testVector.aBearerData, testVector.uBearerDataLen);
            EXPECT_TRUE(tmp == msg.bearerData);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cdmaNewSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::cdmaNewSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, newBroadcastSmsInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::newBroadcastSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static uint8_t testVector[] = { 0x1, 0x2, 0x3, 0x4, 0x5 };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> newBroadcastSms(RadioIndicationType type,
                const ::android::hardware::hidl_vec<uint8_t>& data) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            hidl_vec<uint8_t> tmp;
            tmp.setToExternal(testVector, sizeof(testVector));
            EXPECT_TRUE(tmp == data);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::newBroadcastSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::newBroadcastSmsInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cdmaRuimSmsStorageFullInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cdmaRuimSmsStorageFullInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> cdmaRuimSmsStorageFull(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::cdmaRuimSmsStorageFullInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, restrictedStateChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::restrictedStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int32_t testVector = 4;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> restrictedStateChanged(RadioIndicationType type, PhoneRestrictedState state) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector, (int)state);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::restrictedStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::restrictedStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, enterEmergencyCallbackModeInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::enterEmergencyCallbackModeInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> enterEmergencyCallbackMode(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::enterEmergencyCallbackModeInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cdmaCallWaitingInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cdmaCallWaitingInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CDMA_CallWaiting_v6 testVector = {
        (char *)"91821012345678",
        1,
        (char *)"test_name",
        { 1, 2, 3, 4 },
        1,
        3,
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> cdmaCallWaiting(RadioIndicationType type,
                const CdmaCallWaiting& callWaitingRecord) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_TRUE(testVector.number == callWaitingRecord.number);
            EXPECT_EQ(testVector.numberPresentation, (int)callWaitingRecord.numberPresentation);
            EXPECT_TRUE(testVector.name == callWaitingRecord.name);
            EXPECT_TRUE(testVector.signalInfoRecord.isPresent == callWaitingRecord.signalInfoRecord.isPresent);
            EXPECT_TRUE(testVector.signalInfoRecord.signalType == callWaitingRecord.signalInfoRecord.signalType);
            EXPECT_TRUE(testVector.signalInfoRecord.alertPitch == callWaitingRecord.signalInfoRecord.alertPitch);
            EXPECT_TRUE(testVector.signalInfoRecord.signal == callWaitingRecord.signalInfoRecord.signal);
            EXPECT_EQ(testVector.number_type, (int)callWaitingRecord.numberType);
            EXPECT_EQ(testVector.number_plan, (int)callWaitingRecord.numberPlan);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cdmaCallWaitingInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::cdmaCallWaitingInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cdmaOtaProvisionStatusInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cdmaOtaProvisionStatusInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int32_t testVector = 1;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> cdmaOtaProvisionStatus(RadioIndicationType type, CdmaOtaProvisionStatus status) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector, (int)status);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cdmaOtaProvisionStatusInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::cdmaOtaProvisionStatusInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cdmaInfoRecInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cdmaInfoRecInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CDMA_InformationRecords testVector = {
        7,
        {
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_DISPLAY_INFO_REC,
                .rec.display = { 5, "12345"},
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC,
                .rec.number = { 5, "12345", 1, 1, 0, 0 },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_SIGNAL_INFO_REC,
                .rec.signal = { 1, 2, 3, 4 },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_REDIRECTING_NUMBER_INFO_REC,
                .rec.redir = { { 5, "12345", 1, 1, 0, 0 }, RIL_CDMA_RedirectingReason::RIL_REDIRECTING_REASON_CALL_FORWARDING_BY_THE_CALLED_DTE },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_LINE_CONTROL_INFO_REC,
                .rec.lineCtrl = { 1, 2, 3, 4 },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_T53_CLIR_INFO_REC,
                .rec.clir = { 3 },
            },
            {
                .name = RIL_CDMA_InfoRecName::RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC,
                .rec.audioCtrl = { 3, 7 },
            },
        }
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> cdmaInfoRec(RadioIndicationType type, const CdmaInformationRecords& records) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.numberOfInfoRecs, records.infoRec.size());
            for (size_t i = 0; i < records.infoRec.size(); i++) {
                const CdmaInformationRecord& r = records.infoRec[i];
                RIL_CDMA_InformationRecord& t = testVector.infoRec[i];
                EXPECT_EQ(t.name, (int)r.name);
                switch (t.name) {
                case RIL_CDMA_InfoRecName::RIL_CDMA_DISPLAY_INFO_REC:
                    EXPECT_EQ((unsigned long)1, r.display.size());
                    test(t.rec.display, r.display[0]);
                    break;
                case RIL_CDMA_InfoRecName::RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
                    EXPECT_EQ((unsigned long)1, r.number.size());
                    test(t.rec.number, r.number[0]);
                    break;
                case RIL_CDMA_InfoRecName::RIL_CDMA_SIGNAL_INFO_REC:
                    EXPECT_EQ((unsigned long)1, r.signal.size());
                    test(t.rec.signal, r.signal[0]);
                    break;
                case RIL_CDMA_InfoRecName::RIL_CDMA_REDIRECTING_NUMBER_INFO_REC:
                    EXPECT_EQ((unsigned long)1, r.redir.size());
                    test(t.rec.redir, r.redir[0]);
                    break;
                case RIL_CDMA_InfoRecName::RIL_CDMA_LINE_CONTROL_INFO_REC:
                    EXPECT_EQ((unsigned long)1, r.lineCtrl.size());
                    test(t.rec.lineCtrl, r.lineCtrl[0]);
                    break;
                case RIL_CDMA_InfoRecName::RIL_CDMA_T53_CLIR_INFO_REC:
                    EXPECT_EQ((unsigned long)1, r.clir.size());
                    test(t.rec.clir, r.clir[0]);
                    break;
                case RIL_CDMA_InfoRecName::RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC:
                    EXPECT_EQ((unsigned long)1, r.audioCtrl.size());
                    test(t.rec.audioCtrl, r.audioCtrl[0]);
                    break;
                default:
                    break;
                }
            }

            return Void();
        }

        void test(const RIL_CDMA_DisplayInfoRecord& t, const CdmaDisplayInfoRecord& display) {
            EXPECT_EQ(t.alpha_len, display.alphaBuf.size());
            EXPECT_TRUE(t.alpha_buf == display.alphaBuf);

        }
        void test(const RIL_CDMA_NumberInfoRecord& t, const CdmaNumberInfoRecord& number) {
            EXPECT_EQ(t.len, number.number.size());
            EXPECT_TRUE(t.buf == number.number);
            EXPECT_EQ(t.number_type, number.numberType);
            EXPECT_EQ(t.number_plan, number.numberPlan);
            EXPECT_EQ(t.pi, number.pi);
            EXPECT_EQ(t.si, number.si);
        }
        void test(const RIL_CDMA_SignalInfoRecord& t, const CdmaSignalInfoRecord& signal) {
            EXPECT_EQ(t.isPresent, signal.isPresent);
            EXPECT_EQ(t.signalType, signal.signalType);
            EXPECT_EQ(t.alertPitch, signal.alertPitch);
            EXPECT_EQ(t.signal, signal.signal);
        }
        void test(const RIL_CDMA_RedirectingNumberInfoRecord& t, const CdmaRedirectingNumberInfoRecord& redir) {
            test(t.redirectingNumber, redir.redirectingNumber);
            EXPECT_EQ(t.redirectingReason, (int)redir.redirectingReason);
        }
        void test(const RIL_CDMA_LineControlInfoRecord& t, const CdmaLineControlInfoRecord& lineCtrl) {
            EXPECT_EQ(t.lineCtrlPolarityIncluded, lineCtrl.lineCtrlPolarityIncluded);
            EXPECT_EQ(t.lineCtrlToggle, lineCtrl.lineCtrlToggle);
            EXPECT_EQ(t.lineCtrlReverse, lineCtrl.lineCtrlReverse);
            EXPECT_EQ(t.lineCtrlPowerDenial, lineCtrl.lineCtrlPowerDenial);
        }
        void test(const RIL_CDMA_T53_CLIRInfoRecord& t, const CdmaT53ClirInfoRecord& clir) {
            EXPECT_EQ(t.cause, clir.cause);
        }
        void test(const RIL_CDMA_T53_AudioControlInfoRecord& t, const CdmaT53AudioControlInfoRecord& audioCtrl) {
            EXPECT_EQ(t.upLink, audioCtrl.upLink);
            EXPECT_EQ(t.downLink, audioCtrl.downLink);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cdmaInfoRecInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    // invalid params 1
    RIL_CDMA_InformationRecords invalidParam1 = { 1, { { RIL_CDMA_InfoRecName::RIL_CDMA_T53_RELEASE_INFO_REC, {} } } };
    radio::cdmaInfoRecInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &invalidParam1, sizeof(RIL_CDMA_InformationRecords));

    // invalid params 2
    RIL_CDMA_InformationRecords invalidParam2 = { 1, { { (RIL_CDMA_InfoRecName)INT_MAX, {} } } };
    radio::cdmaInfoRecInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
                RIL_E_SUCCESS, &invalidParam2, sizeof(RIL_CDMA_InformationRecords));

    // test
    radio::cdmaInfoRecInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, indicateRingbackToneInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::indicateRingbackToneInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int32_t testVector = 1;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> indicateRingbackTone(RadioIndicationType type, bool start) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector, start);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::indicateRingbackToneInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::indicateRingbackToneInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(int));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, resendIncallMuteInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::resendIncallMuteInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> resendIncallMute(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::resendIncallMuteInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cdmaSubscriptionSourceChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cdmaSubscriptionSourceChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int testVector = RIL_CdmaSubscriptionSource::CDMA_SUBSCRIPTION_SOURCE_NV;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> cdmaSubscriptionSourceChanged(RadioIndicationType type,
                CdmaSubscriptionSource cdmaSource) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector, (int)cdmaSource);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cdmaSubscriptionSourceChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::cdmaSubscriptionSourceChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cdmaPrlChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cdmaPrlChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int testVector = 15;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> cdmaPrlChanged(RadioIndicationType type, int32_t version) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector, version);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cdmaPrlChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::cdmaPrlChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, exitEmergencyCallbackModeInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::exitEmergencyCallbackModeInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> exitEmergencyCallbackMode(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::exitEmergencyCallbackModeInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, rilConnectedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::rilConnectedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> rilConnected(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::rilConnectedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, voiceRadioTechChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::voiceRadioTechChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int testVector = 14;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> voiceRadioTechChanged(RadioIndicationType type,
                ::android::hardware::radio::V1_0::RadioTechnology rat) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector, (int)rat);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::voiceRadioTechChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::voiceRadioTechChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cellInfoListInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cellInfoListInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_v12 testVector[] = {
            // TODO
            {},
            {},
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        TestChecker check;
        Return<void> cellInfoList(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>&
                        /*records*/) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            // TODO
            return Void();
        }

        void test(const RIL_CellInfo_v12& /*t*/, const V1_0::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cellInfoListInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_CellInfo_v12));

    // test
    radio::cellInfoListInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, imsNetworkStateChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::imsNetworkStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> imsNetworkStateChanged(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::imsNetworkStateChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, subscriptionStatusChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::subscriptionStatusChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int testVector = 1;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> subscriptionStatusChanged(RadioIndicationType type, bool activate) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector, (int) activate);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::subscriptionStatusChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::subscriptionStatusChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(int));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, srvccStateNotifyInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::srvccStateNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int testVector = RIL_SrvccState::HANDOVER_STARTED;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> srvccStateNotify(RadioIndicationType type, SrvccState state) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector, (int)state);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::srvccStateNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::srvccStateNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, hardwareConfigChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::hardwareConfigChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
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
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> hardwareConfigChanged(
                RadioIndicationType type, const ::android::hardware::hidl_vec<HardwareConfig>& configs) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            for (size_t i = 0; i < configs.size(); i++) {
                test(testVector[i], configs[i]);
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
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::hardwareConfigChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_HardwareConfig));

    // test
    radio::hardwareConfigChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, radioCapabilityIndicationInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::radioCapabilityIndicationInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_RadioCapability testVector {};
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> radioCapabilityIndication(
                RadioIndicationType type, const android::hardware::radio::V1_0::RadioCapability& /*rc*/) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::radioCapabilityIndicationInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::radioCapabilityIndicationInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, onSupplementaryServiceIndicationInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::onSupplementaryServiceIndicationInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_StkCcUnsolSsResponse testVector[] = {
        {
            RIL_SsServiceType::SS_BAOIC,
            RIL_SsRequestType::SS_ACTIVATION,
            RIL_SsTeleserviceType::SS_ALL_TELESERVICES_EXCEPT_SMS,
            3,
            RIL_Errno::RIL_E_SUCCESS,
            {},
        },
        {
            RIL_SsServiceType::SS_CF_BUSY,
            RIL_SsRequestType::SS_INTERROGATION,
            RIL_SsTeleserviceType::SS_ALL_TELESERVICES_EXCEPT_SMS,
            3,
            RIL_Errno::RIL_E_SUCCESS,
            .cfData = { 8, { { 1, 0, 0, 147, (char *)"1234567", 0 }, } },
        }
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    static size_t i = 0;

    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> onSupplementaryServiceIndication(RadioIndicationType type,
                const StkCcUnsolSsResult& ss) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector[i].serviceType, (int)ss.serviceType);
            EXPECT_EQ(testVector[i].requestType, (int)ss.requestType);
            EXPECT_EQ(testVector[i].teleserviceType, (int)ss.teleserviceType);
            EXPECT_EQ(testVector[i].serviceClass, ss.serviceClass);
            EXPECT_EQ(testVector[i].result, (int)ss.result);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::onSupplementaryServiceIndicationInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    for (; i < size; i++) {
        radio::onSupplementaryServiceIndicationInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
                RIL_E_SUCCESS, &testVector[i], sizeof(RIL_StkCcUnsolSsResponse));
    }
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, stkCallControlAlphaNotifyInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::stkCallControlAlphaNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static char testVector[] = "stkCallControlAlphaNotifyInd";
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> stkCallControlAlphaNotify(RadioIndicationType type,
                const ::android::hardware::hidl_string& alpha) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_TRUE(testVector == alpha);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::stkCallControlAlphaNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::stkCallControlAlphaNotifyInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

// IRadioIndication#lceData is deprecated
// Use V1_2::IRadioIndication#currentLinkCapacityEstimate
//TEST(ril_service_legacy_ind_tests, lceDataInd)

TEST(ril_service_legacy_ind_tests, pcoDataInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::pcoDataInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static char contents[] = { 0x1, 0x2, 0x3, 0x4, 0x5 };
    static RIL_PCO_Data testVector = {
        1,
        (char *)"IP",
        0xFF00,
        sizeof(contents),
        contents,
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> pcoData(RadioIndicationType type, const PcoDataInfo& pco) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.cid, pco.cid);
            EXPECT_TRUE(testVector.bearer_proto == pco.bearerProto);
            EXPECT_EQ(testVector.pco_id, pco.pcoId);
            hidl_vec<uint8_t> tmp;
            tmp.setToExternal((uint8_t *)testVector.contents, testVector.contents_length);
            EXPECT_TRUE(tmp == pco.contents);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::pcoDataInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::pcoDataInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, modemResetInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::modemResetInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static char testVector[] = "cp_crash_haha";
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> modemReset(RadioIndicationType type,
                const ::android::hardware::hidl_string& reason) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_TRUE(reason == testVector || reason == "unknown");
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::modemResetInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::modemResetInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

// V1_1::IRadioIndicaiton
TEST(ril_service_legacy_ind_tests, networkScanResultInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkScanResultInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_v12 network_infos[] = {
        // TODO
        {}, {},
    };
    static RIL_NetworkScanResult testVector= {
        RIL_ScanStatus::COMPLETE,
        sizeof(network_infos) / sizeof(network_infos[0]),
        network_infos,
        RIL_Errno::RIL_E_SUCCESS,
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> networkScanResult(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_1::NetworkScanResult& result) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.status, (int)result.status);
            EXPECT_EQ(testVector.error, (int)result.error);
            // TODO
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::networkScanResultInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::networkScanResultInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, carrierInfoForImsiEncryption) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::carrierInfoForImsiEncryption(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> carrierInfoForImsiEncryption(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    radio::carrierInfoForImsiEncryption(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, keepaliveStatusInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::keepaliveStatusInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_KeepaliveStatus testVector = { 1, RIL_KeepaliveStatusCode::KEEPALIVE_ACTIVE };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> keepaliveStatus(RadioIndicationType type, const KeepaliveStatus& status) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.sessionHandle, (unsigned int)/* NEED TO CHECK */ status.sessionHandle);
            EXPECT_EQ(testVector.code, (int)status.code);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::keepaliveStatusInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::keepaliveStatusInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

// V1_2::IRadioIndicaiton
TEST(ril_service_legacy_ind_tests, currentPhysicalChannelConfigs) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentPhysicalChannelConfigs(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_PhysicalChannelConfig testVector[]= {
        { RIL_CellConnectionStatus::PRIMARY_SERVING, 100 },
        { RIL_CellConnectionStatus::SECONDARY_SERVING, 200 },
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> currentPhysicalChannelConfigs(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<
                        ::android::hardware::radio::V1_2::PhysicalChannelConfig>& configs) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (configs.size() > 0) {
                EXPECT_EQ(sizeof(testVector) / sizeof(testVector[0]), configs.size());
                for (size_t i = 0; i < configs.size(); i++) {
                    EXPECT_EQ(testVector[i].status, (int)configs[i].status);
                    EXPECT_EQ(testVector[i].cellBandwidthDownlink, configs[i].cellBandwidthDownlink);
                }
            }
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::currentPhysicalChannelConfigs(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    // test
    radio::currentPhysicalChannelConfigs(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, currentSignalStrengthInd_1_2) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentSignalStrengthInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_SignalStrength_V1_2 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> currentSignalStrength_1_2(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_2::SignalStrength& signalStrength) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
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

            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::currentSignalStrengthInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::currentSignalStrengthInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, currentSignalStrengthInd_1_2_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentSignalStrengthInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_SignalStrength_V1_2 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
    };
    sp<V1_1::IRadioResponse> responseCallback = new TestRadioResponseBase_1_1;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_1 {
        Return<void> currentSignalStrength(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_0::SignalStrength& signalStrength) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);

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
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::currentSignalStrengthInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cellInfoListInd_1_2) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cellInfoListInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_2 testVector[] = {
            // TODO
            {},
            {},
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        TestChecker check;
        Return<void> cellInfoList_1_2(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>&
                        /*records*/) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            // TODO
            return Void();
        }

        void test(const RIL_CellInfo_v12& /*t*/, const V1_0::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cellInfoListInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_CellInfo_V1_2));

    // test
    radio::cellInfoListInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cellInfoListInd_1_2_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cellInfoListInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
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
    sp<V1_1::IRadioResponse> responseCallback = new TestRadioResponseBase_1_1;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_1 {
        TestChecker check;
        Return<void> cellInfoList(
                    RadioIndicationType type,
                    const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CellInfo>&
                            records) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
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
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::cellInfoListInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, networkScanResultInd_1_2) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkScanResultInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_2 network_infos[] = {
        // TODO
        {}, {},
    };
    static RIL_NetworkScanResult_V1_2 testVector= {
        RIL_ScanStatus::PARTIAL,
        sizeof(network_infos) / sizeof(network_infos[0]),
        network_infos,
        RIL_Errno::RIL_E_SUCCESS,
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> networkScanResult_1_2(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_2::NetworkScanResult& result) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.status, (int)result.status);
            EXPECT_EQ(testVector.error, (int)result.error);
            // TODO
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::networkScanResultInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::networkScanResultInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, networkScanResultInd_1_2_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkScanResultInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_2 network_infos[] = {
        // TODO
        {}, {},
    };
    static RIL_NetworkScanResult_V1_2 testVector= {
        RIL_ScanStatus::PARTIAL,
        sizeof(network_infos) / sizeof(network_infos[0]),
        network_infos,
        RIL_Errno::RIL_E_SUCCESS,
    };
    sp<V1_1::IRadioResponse> responseCallback = new TestRadioResponseBase_1_1;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_1 {
        Return<void> networkScanResult_1_2(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_2::NetworkScanResult& result) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.status, (int)result.status);
            EXPECT_EQ(testVector.error, (int)result.error);
            // TODO
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::networkScanResultInd_1_2(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, currentLinkCapacityEstimateInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::lceDataInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_LinkCapacityEstimate testVector= { 100000, 200000 };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> currentLinkCapacityEstimate(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_2::LinkCapacityEstimate& lce) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.uplinkCapacityKbps, lce.uplinkCapacityKbps);
            EXPECT_EQ(testVector.downlinkCapacityKbps, lce.downlinkCapacityKbps);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::lceDataInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::lceDataInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, currentSignalStrengthInd_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentSignalStrengthInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_SignalStrength_V1_4 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
        {}, // NR
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> currentSignalStrength_1_4(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_4::SignalStrength& signalStrength) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
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
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::currentSignalStrengthInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::currentSignalStrengthInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, currentSignalStrengthInd_1_4_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentSignalStrengthInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_SignalStrength_V1_4 testVector = {
        {}, // GSM
        {}, // CDMA
        {}, // EVDO
        { 30, 50, 10, -100, 3, 0 }, // LTE
        {}, // TD-SCDMA
        {}, // WCDMA
        {}, // NR
    };
    sp<V1_2::IRadioResponse> responseCallback = new TestRadioResponseBase_1_2;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_2 {
        Return<void> currentSignalStrength_1_2(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_2::SignalStrength& signalStrength) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);

            RIL_SignalStrength_V1_2 ss {};
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
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::currentSignalStrengthInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, dataCallListChangedInd_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::dataCallListChangedInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_Data_Call_Response_V1_4 testVector[] = {
        { 0, -1, 1, 2, 1, (char *)"rmnet0", 1, (char **)"192.164.0.2", 1, (char **)"192.164.0.1",
                1, (char **)"192.164.0.1", 0, (char **)"", 1500 },
        { 0, -1, 2, 2, 1, (char *)"rmnet1", 1, (char **)"192.164.0.2", 1, (char **)"192.164.0.1",
                1, (char **)"192.164.0.1", 1, (char **)"10.1.2.4", 1500 },
    };
    static size_t testSize = sizeof(testVector) / sizeof(testVector[0]);
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> dataCallListChanged_1_4(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<
                        android::hardware::radio::V1_4::SetupDataCallResult>& dcList) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (dcList.size() > 0) {
                EXPECT_EQ(testSize, dcList.size());
                for (size_t i = 0; i < dcList.size(); i++) {
                    test(testVector[i], dcList[i]);
                }
            }
            return Void();
        }

        void test(const RIL_Data_Call_Response_V1_4& /*testDc*/,
                const V1_4::SetupDataCallResult& /*dcResult*/) {
            // TODO
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    // nullptr with 0 length is a valid input for radio::dataCallListChangedInd
    radio::dataCallListChangedInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    // test
    // an empty list
    radio::dataCallListChangedInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    radio::dataCallListChangedInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cellInfoListInd_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cellInfoListInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_4 testVector[] = {
        // TODO
        {},
        {},
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        TestChecker check;
        Return<void> cellInfoList_1_4(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>&
                        records) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (records.size() > 0) {
                // TODO

            }
            return Void();
        }

        void test(const RIL_CellInfo_V1_4& t, const V1_4::CellInfo& cellinfo) {
            // TODO
            EXPECT_EQ(t.registered, (int)cellinfo.isRegistered);
            EXPECT_EQ(t.connectionStatus, (int)cellinfo.connectionStatus);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cellInfoListInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_CellInfo_V1_4));

    // test
    radio::cellInfoListInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cellInfoListInd_1_4_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cellInfoListInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_4 testVector[] = {
        // TODO
        {},
        {},
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);
    sp<V1_2::IRadioResponse> responseCallback = new TestRadioResponseBase_1_2;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_2 {
        TestChecker check;
        Return<void> cellInfoList_1_2(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_2::CellInfo>&
                        records) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(records.size(), size);
            if (records.size() > 0) {
                for (size_t i = 0; i < records.size(); i++) {
                    RIL_CellInfo_V1_2 cellInfo = {};
                    RadioConvUtils::convertCellInfo(cellInfo, testVector[i]);
                    test(cellInfo, records[i]);
                }
            }
            return Void();
        }

        void test(const RIL_CellInfo_V1_2& /*t*/, const V1_2::CellInfo& /*cellinfo*/) {
            // TODO
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::cellInfoListInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, networkScanResultInd_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkScanResultInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_4 network_infos[] = {
        // TODO
        {}, {},
    };
    static RIL_NetworkScanResult_V1_4 testVector= {
        RIL_ScanStatus::PARTIAL,
        sizeof(network_infos) / sizeof(network_infos[0]),
        network_infos,
        RIL_Errno::RIL_E_SUCCESS,
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> networkScanResult_1_4(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_4::NetworkScanResult& result) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.status, (int)result.status);
            EXPECT_EQ(testVector.error, (int)result.error);
            // TODO
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::networkScanResultInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::networkScanResultInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, networkScanResultInd_1_4_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkScanResultInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_4 network_infos[] = {
        // TODO
        {}, {},
    };
    static RIL_NetworkScanResult_V1_4 testVector= {
        RIL_ScanStatus::PARTIAL,
        sizeof(network_infos) / sizeof(network_infos[0]),
        network_infos,
        RIL_Errno::RIL_E_SUCCESS,
    };
    sp<V1_2::IRadioResponse> responseCallback = new TestRadioResponseBase_1_2;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_2 {
        Return<void> networkScanResult_1_2(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_2::NetworkScanResult& result) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.status, (int)result.status);
            EXPECT_EQ(testVector.error, (int)result.error);
            // TODO
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::networkScanResultInd_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, currentPhysicalChannelConfigs_1_4) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentPhysicalChannelConfigs_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_PhysicalChannelConfig_V1_4 testVector[]= {
        { RIL_CellConnectionStatus::PRIMARY_SERVING, 100, RADIO_TECH_LTE, RF_INFO_TYPE_CHANNEL_NUMBER, .rfInfo.channelNumber = 2300, 0, 0 },
        { RIL_CellConnectionStatus::SECONDARY_SERVING, 200, RADIO_TECH_NR, RF_INFO_TYPE_RANGE, .rfInfo.range = FREQUENCY_RANGE_MID, 0, 0 },
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> currentPhysicalChannelConfigs_1_4(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<
                        ::android::hardware::radio::V1_4::PhysicalChannelConfig>& configs) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (configs.size() > 0) {
                EXPECT_EQ(sizeof(testVector) / sizeof(testVector[0]), configs.size());
                for (size_t i = 0; i < configs.size(); i++) {
                    test(testVector[i], configs[i]);
                }
            }
            return Void();
        }

        void test(const RIL_PhysicalChannelConfig_V1_4& t, const V1_4::PhysicalChannelConfig& pcc) {
            EXPECT_EQ(t.status, (int)pcc.base.status);
            EXPECT_EQ(t.cellBandwidthDownlink, pcc.base.cellBandwidthDownlink);
            EXPECT_EQ(t.physicalCellId, pcc.physicalCellId);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::currentPhysicalChannelConfigs_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_PhysicalChannelConfig_V1_4));

    // test
    radio::currentPhysicalChannelConfigs_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, currentPhysicalChannelConfigs_1_4_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentPhysicalChannelConfigs_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_PhysicalChannelConfig_V1_4 testVector[]= {
        { RIL_CellConnectionStatus::PRIMARY_SERVING, 100, RADIO_TECH_LTE, RF_INFO_TYPE_CHANNEL_NUMBER, .rfInfo.channelNumber = 2300, 0, 0 },
        { RIL_CellConnectionStatus::SECONDARY_SERVING, 200, RADIO_TECH_NR, RF_INFO_TYPE_RANGE, .rfInfo.range = FREQUENCY_RANGE_MID, 0, 0 },
    };
    static size_t size = sizeof(testVector) / sizeof(testVector[0]);
    sp<V1_2::IRadioResponse> responseCallback = new TestRadioResponseBase_1_2;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_2 {
        Return<void> currentPhysicalChannelConfigs(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<
                        ::android::hardware::radio::V1_2::PhysicalChannelConfig>& configs) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (configs.size() > 0) {
                EXPECT_EQ(size, configs.size());
                for (size_t i = 0; i < configs.size(); i++) {
                    EXPECT_EQ(testVector[i].status, (int)configs[i].status);
                    EXPECT_EQ(testVector[i].cellBandwidthDownlink, configs[i].cellBandwidthDownlink);
                }
            }
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::currentPhysicalChannelConfigs_1_4(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, currentEmergencyNumberListInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentEmergencyNumberListInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_EmergencyNumber testVector[]= {
        {
            (char *)"911",
            (char *)"001",
            (char *)"01",
            0x1F,
            0,
            nullptr,
            0x1F,
        },
        {
            (char *)"112",
            (char *)"001",
            (char *)"01",
            0x1F,
            0,
            nullptr,
            0x1F,
        },
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> currentEmergencyNumberList(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<EmergencyNumber>& emergencyNumberList) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (emergencyNumberList.size() > 0) {
                EXPECT_EQ(sizeof(testVector) / sizeof(testVector[0]), emergencyNumberList.size());
                for (size_t i = 0; i < emergencyNumberList.size(); i++) {
                    test(testVector[i], emergencyNumberList[i]);
                }
            }
            return Void();
        }

        void test(const RIL_EmergencyNumber& t, const EmergencyNumber& emergencyNumber) {
            EXPECT_TRUE(t.number == emergencyNumber.number);
            EXPECT_TRUE(t.mcc == emergencyNumber.mcc);
            EXPECT_TRUE(t.mnc == emergencyNumber.mnc);
            EXPECT_EQ(t.categories, emergencyNumber.categories);
            EXPECT_EQ((unsigned long)0, emergencyNumber.urns.size());
            EXPECT_EQ(t.sources, emergencyNumber.sources);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::currentEmergencyNumberListInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_EmergencyNumber));

    // test
    radio::currentEmergencyNumberListInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

// V1_5::IRadioIndicaiton
TEST(ril_service_legacy_ind_tests, uiccApplicationsEnablementChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::uiccApplicationsEnablementChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int testVector = 1;
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> uiccApplicationsEnablementChanged(RadioIndicationType type, bool enabled) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector, (int) enabled);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::uiccApplicationsEnablementChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::uiccApplicationsEnablementChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, registrationFailedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::registrationFailedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_RegistrationFailed testVector = {
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            .lte = {
                311, ril::util::mnc::encode(480, 3), 1000, 300, 35000, 1500,
                {"test long", "test short"}, 40, 0, 0, {}, 0, {}
            },
        },
        (char *)"311480", RIL_Domain::DOMAIN_CS, INT_MAX, INT_MAX
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> registrationFailed(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
                const ::android::hardware::hidl_string& chosenPlmn,
                ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_5::Domain> domain,
                int32_t causeCode, int32_t additionalCauseCode) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            test(cellIdentity, testVector.cellIdentity);
            EXPECT_TRUE(testVector.chosenPlmn == chosenPlmn);
            EXPECT_EQ(testVector.domainBitmap, (int)domain);
            EXPECT_EQ(testVector.causeCode, causeCode);
            EXPECT_EQ(testVector.additionalCauseCode, additionalCauseCode);
            return Void();
        }

        void test(const V1_5::CellIdentity& cellIdentity, const RIL_CellIdentity_V1_5& rilCellIdentity) {
            using Discr = V1_5::CellIdentity::hidl_discriminator;
            switch (cellIdentity.getDiscriminator()) {
            case Discr::noinit:
                EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE);
                break;
            case Discr::gsm:
                EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM);
                break;
            case Discr::wcdma:
                EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA);
                break;
            case Discr::tdscdma:
                EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA);
                break;
            case Discr::cdma:
                EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA);
                break;
            case Discr::lte:
                EXPECT_EQ(rilCellIdentity.cellInfoType, RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE);
                test(cellIdentity.lte(), rilCellIdentity.lte);
                break;
            case Discr::nr:
                EXPECT_EQ((int)rilCellIdentity.cellInfoType, (int)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR);
                break;
            default:
                ASSERT_TRUE(false);
                break;
            };
        }

        void test(const V1_5::CellIdentityLte& lte, const RIL_CellIdentityLte_V1_5& rilLte) {
            EXPECT_TRUE(lte.base.base.mcc == ril::util::mcc::decode(rilLte.mcc));
            EXPECT_TRUE(lte.base.base.mnc == ril::util::mnc::decode(rilLte.mnc));
            EXPECT_EQ(lte.base.base.ci, rilLte.ci);
            EXPECT_EQ(lte.base.base.pci, rilLte.pci);
            EXPECT_EQ(lte.base.base.tac, rilLte.tac);
            EXPECT_EQ(lte.base.base.earfcn, rilLte.earfcn);
            EXPECT_TRUE(lte.base.operatorNames.alphaLong == rilLte.operatorNames.alphaLong);
            EXPECT_TRUE(lte.base.operatorNames.alphaShort == rilLte.operatorNames.alphaShort);
            EXPECT_EQ(lte.base.bandwidth, rilLte.bandwidth);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::registrationFailedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::registrationFailedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, barringInfoChangedInd) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::barringInfoChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_BarringInfo barringInfos[] = {
        { RIL_BarringServiceType::CS_SERVICE, RIL_BarringType::BARRING_TYPE_CONDITIONAL, {} },
        { RIL_BarringServiceType::PS_SERVICE, RIL_BarringType::BARRING_TYPE_UNCONDITIONAL, {} },
    };
    static RIL_CellIdAndBarringInfo testVector = {
        {},
        sizeof(barringInfos) / sizeof(barringInfos[0]),
        barringInfos,
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> barringInfoChanged(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>&
                barringInfos) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            test(testVector.cellIdentity, cellIdentity);
            EXPECT_EQ(testVector.numOfrecord, barringInfos.size());
            for (size_t i = 0; i < barringInfos.size(); i++) {
                test(testVector.precords[i], barringInfos[i]);
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
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::barringInfoChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::barringInfoChangedInd(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cellInfoListInd_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cellInfoListInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_5 testVector[] = {
        // TODO
        {},
        {},
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        TestChecker check;
        Return<void> cellInfoList_1_5(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                        records) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (records.size() > 0) {
                // TODO

            }
            return Void();
        }

        void test(const RIL_CellInfo_V1_5& t, const V1_5::CellInfo& cellinfo) {
            // TODO
            EXPECT_EQ(t.registered, (int)cellinfo.registered);
            EXPECT_EQ(t.timeStampType, (int)cellinfo.timeStampType);
            EXPECT_TRUE(t.timeStamp == cellinfo.timeStamp);
            EXPECT_EQ(t.connectionStatus, (int)cellinfo.connectionStatus);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cellInfoListInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_CellInfo_V1_5));

    // test
    radio::cellInfoListInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, cellInfoListInd_1_5_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cellInfoListInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_5 testVector[] = {
        // TODO
        {},
        {},
    };
    sp<V1_4::IRadioResponse> responseCallback = new TestRadioResponseBase_1_4;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_4 {
        TestChecker check;
        Return<void> cellInfoList_1_4(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::CellInfo>&
                        records) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (records.size() > 0) {
                // TODO

            }
            return Void();
        }

        void test(const RIL_CellInfo_V1_5& t, const V1_5::CellInfo& cellinfo) {
            // TODO
            EXPECT_EQ(t.registered, (int)cellinfo.registered);
            EXPECT_EQ(t.timeStampType, (int)cellinfo.timeStampType);
            EXPECT_TRUE(t.timeStamp == cellinfo.timeStamp);
            EXPECT_EQ(t.connectionStatus, (int)cellinfo.connectionStatus);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::cellInfoListInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, networkScanResultInd_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkScanResultInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_5 network_infos[] = {
        // TODO
        {}, {},
    };
    static RIL_NetworkScanResult_V1_5 testVector= {
        RIL_ScanStatus::PARTIAL,
        sizeof(network_infos) / sizeof(network_infos[0]),
        network_infos,
        RIL_Errno::RIL_E_SUCCESS,
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> networkScanResult_1_5(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_5::NetworkScanResult& result) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.status, (int)result.status);
            EXPECT_EQ(testVector.error, (int)result.error);
            // TODO
            return Void();
        }

        void test(const RIL_CellInfo_V1_5& t, const V1_5::CellInfo& cellinfo) {
            // TODO
            EXPECT_EQ(t.registered, (int)cellinfo.registered);
            EXPECT_EQ(t.timeStampType, (int)cellinfo.timeStampType);
            EXPECT_TRUE(t.timeStamp == cellinfo.timeStamp);
            EXPECT_EQ(t.connectionStatus, (int)cellinfo.connectionStatus);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::networkScanResultInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::networkScanResultInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, networkScanResultInd_1_5_fallback) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkScanResultInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_5 network_infos[] = {
        // TODO
        {}, {},
    };
    static RIL_NetworkScanResult_V1_5 testVector= {
        RIL_ScanStatus::PARTIAL,
        sizeof(network_infos) / sizeof(network_infos[0]),
        network_infos,
        RIL_Errno::RIL_E_SUCCESS,
    };
    sp<V1_4::IRadioResponse> responseCallback = new TestRadioResponseBase_1_4;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_4 {
        Return<void> networkScanResult_1_4(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_4::NetworkScanResult& result) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(testVector.status, (int)result.status);
            EXPECT_EQ(testVector.error, (int)result.error);
            // TODO
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::networkScanResultInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_legacy_ind_tests, dataCallListChangedInd_1_5) {
    sp<V1_5::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::dataCallListChangedInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_Data_Call_Response_V1_5 testVector[] = {
//        { 0, -1, 1, 2, 1, (char *)"rmnet0", 1, (char **)"192.164.0.2", 1, (char **)"192.164.0.1",
//                1, (char **)"192.164.0.1", 0, (char **)"", 1500 },
//        { 0, -1, 2, 2, 1, (char *)"rmnet1", 1, (char **)"192.164.0.2", 1, (char **)"192.164.0.1",
//                1, (char **)"192.164.0.1", 1, (char **)"10.1.2.4", 1500 },
            {}, {}
    };
    static size_t testSize = sizeof(testVector) / sizeof(testVector[0]);
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> dataCallListChanged_1_5(
                RadioIndicationType type,
                const hidl_vec<::android::hardware::radio::V1_5::SetupDataCallResult>& dcList) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (dcList.size() > 0) {
                EXPECT_EQ(testSize, dcList.size());
                for (size_t i = 0; i < dcList.size(); i++) {
                    test(testVector[i], dcList[i]);
                }
            }
            return Void();
        }

        void test(const RIL_Data_Call_Response_V1_5& /*testDc*/,
                const V1_5::SetupDataCallResult& /*dcResult*/) {
            // TODO
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    // nullptr with 0 length is a valud input for radio::dataCallListChangedInd
    radio::dataCallListChangedInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_Data_Call_Response_V1_5));

    // test
    // an empty list
    radio::dataCallListChangedInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    radio::dataCallListChangedInd_1_5(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}
