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
 * ril_service_1_6_indication_test.cpp
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
#include "slsi/radio_v1_6.h"
#include "testchecker.h"

using namespace android::hardware::radio;

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

TEST(ril_service_1_6_ind_tests, dataCallListChangedInd_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::dataCallListChangedInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_SetupDataCallResult_V1_6 testVector[] = {
        {}, {}
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);
    sp<V1_6::IRadioResponse> responseCallback = new TestRadioResponseBase_1_6;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_6 {
        Return<void> dataCallListChanged_1_6(
                RadioIndicationType type,
                const hidl_vec<::android::hardware::radio::V1_6::SetupDataCallResult>& dcList) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (dcList.size() > 0) {
                EXPECT_EQ(dcList.size(), size);
                for (size_t i = 0; i < dcList.size(); i++) {
                    test(dcList[i], testVector[i]);
                }
            }
            return Void();
        }

        void test(const V1_6::SetupDataCallResult& /*test*/,
                const RIL_SetupDataCallResult_V1_6& /*dc*/) {

        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    // nullptr with 0 length is a valud input for radio::dataCallListChangedInd
    radio::dataCallListChangedInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_SetupDataCallResult_V1_6));

    // test
    // an empty list
    radio::dataCallListChangedInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    radio::dataCallListChangedInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, currentLinkCapacityEstimate_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentLinkCapacityEstimate_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_LinkCapacityEstimate_V1_6 testVector = {
        10, 20, 30, 40
    };
    sp<V1_6::IRadioResponse> responseCallback = new TestRadioResponseBase_1_6;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_6 {
        TestChecker check;
        Return<void> currentLinkCapacityEstimate_1_6(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lce) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(lce.downlinkCapacityKbps, testVector.downlinkCapacityKbps);
            EXPECT_EQ(lce.uplinkCapacityKbps, testVector.uplinkCapacityKbps);
            EXPECT_EQ(lce.secondaryDownlinkCapacityKbps, testVector.DCNRSecondaryDownlinkCapacityKbps);
            EXPECT_EQ(lce.secondaryUplinkCapacityKbps, testVector.DCNRSecondaryUplinkCapacityKbps);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::currentLinkCapacityEstimate_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    // test
    radio::currentLinkCapacityEstimate_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, currentLinkCapacityEstimate_1_6_fallback) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentLinkCapacityEstimate_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_LinkCapacityEstimate_V1_6 testVector = {
        10, 20, 30, 40
    };
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        TestChecker check;
        Return<void> currentLinkCapacityEstimate(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_2::LinkCapacityEstimate& lce) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(lce.downlinkCapacityKbps, testVector.downlinkCapacityKbps);
            EXPECT_EQ(lce.uplinkCapacityKbps, testVector.uplinkCapacityKbps);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::currentLinkCapacityEstimate_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, unthrottleApn) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::unthrottleApn(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static char testVector[] = "test_apn";
    sp<V1_6::IRadioResponse> responseCallback = new TestRadioResponseBase_1_6;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_6 {
        Return<void> unthrottleApn(RadioIndicationType type,
                const ::android::hardware::hidl_string& apn) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_TRUE(apn == testVector);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::unthrottleApn(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, strlen(testVector));

    // test
    radio::unthrottleApn(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, strlen(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, currentPhysicalChannelConfigs_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentPhysicalChannelConfigs_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int contextIds1[] = { 0 };
    static int contextIds2[] = { 0, 1 };
    static RIL_PhysicalChannelConfig_V1_6 testVector[] = {
        {
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            RADIO_TECH_LTE, 1000, 1100, 40, 20,
            NUM_ELEMS(contextIds1), contextIds1,
            3000, {}
        },
        {
            RIL_CellConnectionStatus::SECONDARY_SERVING,
            RADIO_TECH_LTE, 2000, 2100, 80, 40,
            NUM_ELEMS(contextIds2), contextIds2,
            5000, {}
        },
    };
    static size_t size = NUM_ELEMS(testVector);
    sp<V1_6::IRadioResponse> responseCallback = new TestRadioResponseBase_1_6;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_6 {
        Return<void> currentPhysicalChannelConfigs_1_6(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<
                        ::android::hardware::radio::V1_6::PhysicalChannelConfig>& configs) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (configs.size() > 0) {
                EXPECT_EQ(configs.size(), size);
                for (size_t i = 0; i < configs.size(); i++) {
                    test(configs[i], testVector[i]);
                }
            }
            return Void();
        }

        void test(const V1_6::PhysicalChannelConfig& test,
                const RIL_PhysicalChannelConfig_V1_6& pcc) {
            EXPECT_EQ((int)test.status, (int)pcc.status);
            EXPECT_EQ((int)test.rat, (int)pcc.rat);
            EXPECT_EQ(test.downlinkChannelNumber, pcc.downlinkChannelNumber);
            EXPECT_EQ(test.uplinkChannelNumber, pcc.uplinkChannelNumber);
            EXPECT_EQ(test.cellBandwidthDownlinkKhz, pcc.cellBandwidthDownlink);
            EXPECT_EQ(test.cellBandwidthUplinkKhz, pcc.cellBandwidthUplink);
            EXPECT_EQ(test.contextIds.size(), (unsigned int)/* NEED TO CHECK */ pcc.len_contextIds);
            if (pcc.contextIds) {
                for (size_t i = 0; i < test.contextIds.size(); i++) {
                    EXPECT_EQ(test.contextIds[i], pcc.contextIds[i]);
                }
            }
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::currentPhysicalChannelConfigs_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    // test
    radio::currentPhysicalChannelConfigs_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    radio::currentPhysicalChannelConfigs_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, currentPhysicalChannelConfigs_1_6_fallback) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::currentPhysicalChannelConfigs_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static int contextIds1[] = { 0 };
    static int contextIds2[] = { 0, 1 };
    static RIL_PhysicalChannelConfig_V1_6 testVector[] = {
        {
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            RADIO_TECH_LTE, 1000, 1100, 40, 20,
            NUM_ELEMS(contextIds1), contextIds1,
            3000, {}
        },
        {
            RIL_CellConnectionStatus::SECONDARY_SERVING,
            RADIO_TECH_LTE, 2000, 2100, 80, 40,
            NUM_ELEMS(contextIds2), contextIds2,
            5000, {}
        },
    };
    static size_t size = NUM_ELEMS(testVector);
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        Return<void> currentPhysicalChannelConfigs_1_4(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<
                        ::android::hardware::radio::V1_4::PhysicalChannelConfig>& configs) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            if (configs.size() > 0) {
                EXPECT_EQ(configs.size(), size);
                for (size_t i = 0; i < configs.size(); i++) {
                    RIL_PhysicalChannelConfig_V1_4 pcc {};
                    RadioConvUtils::convertPhysicalChannelConfig(pcc, testVector[i]);
                    test(configs[i], pcc);
                    RadioConvUtils::freePhysicalChannelConfig(pcc);
                }
            }
            return Void();
        }

        void test(const V1_4::PhysicalChannelConfig& test,
                const RIL_PhysicalChannelConfig_V1_4& pcc) {
            EXPECT_EQ((int)test.base.status, (int)pcc.status);
            EXPECT_EQ(test.base.cellBandwidthDownlink, pcc.cellBandwidthDownlink);
            EXPECT_EQ((int)test.rat, (int)pcc.rat);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::currentPhysicalChannelConfigs_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(testVector));

    // test
    radio::currentPhysicalChannelConfigs_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    radio::currentPhysicalChannelConfigs_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, currentSignalStrengthInd_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
   // test with mRadioIndication == NULL
   radio::currentSignalStrengthInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
           RIL_E_SUCCESS, 0, 0);

   // test with mRadioIndication != NULL
   static RIL_SignalStrength_V1_6 testVector = {
       {}, // GSM
       {}, // CDMA
       {}, // EVDO
       { { 30, 50, 10, -100, 3, 0 }, 0 }, // LTE
       {}, // TD-SCDMA
       {}, // WCDMA
       {}, // NR
   };
   sp<V1_6::IRadioResponse> responseCallback = new TestRadioResponseBase_1_6;
   struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_6 {
       Return<void> currentSignalStrength_1_6(
               RadioIndicationType type,
               const ::android::hardware::radio::V1_6::SignalStrength& signalStrength) {
           EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
           test(signalStrength.lte.base,
                   static_cast<RIL_LTE_SignalStrength_v8&>(testVector.LTE_SignalStrength));
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
   sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
   radio->setResponseFunctions(responseCallback, indicationCallback);

   // invalid params
   radio::currentSignalStrengthInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
           RIL_E_SUCCESS, 0, 0);
   radio::currentSignalStrengthInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
           RIL_E_SUCCESS, &testVector, 0);

   // test
   radio::currentSignalStrengthInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
           RIL_E_SUCCESS, &testVector, sizeof(testVector));
   tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, currentSignalStrengthInd_1_6_fallback) {
    sp<V1_6::IRadio> radio = setupRadio();
   // test with mRadioIndication == NULL
   radio::currentSignalStrengthInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
           RIL_E_SUCCESS, 0, 0);

   // test with mRadioIndication != NULL
   static RIL_SignalStrength_V1_6 testVector = {
       {}, // GSM
       {}, // CDMA
       {}, // EVDO
       { { 30, 50, 10, -100, 3, 0 }, 0 }, // LTE
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
           test(signalStrength.lte,
                   static_cast<RIL_LTE_SignalStrength_v8&>(testVector.LTE_SignalStrength));
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
   sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
   radio->setResponseFunctions(responseCallback, indicationCallback);

   // invalid params
   radio::currentSignalStrengthInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
           RIL_E_SUCCESS, 0, 0);
   radio::currentSignalStrengthInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
           RIL_E_SUCCESS, &testVector, 0);

   // test
   radio::currentSignalStrengthInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
           RIL_E_SUCCESS, &testVector, sizeof(testVector));
   tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, cellInfoListInd_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cellInfoListInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_6 testVector[] = {
        // TODO
        {},
        {},
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);
    sp<V1_6::IRadioResponse> responseCallback = new TestRadioResponseBase_1_6;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_6 {
        TestChecker check;
        Return<void> cellInfoList_1_6(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>&
                        records) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(records.size(), size);
            for (size_t i = 0; i < records.size(); i++) {
                test(records[i], testVector[i]);
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
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::cellInfoListInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, sizeof(RIL_CellInfo_V1_6));

    // test
    radio::cellInfoListInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, cellInfoListInd_1_6_fallback) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::cellInfoListInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_6 testVector[] = {
        // TODO
        {},
        {},
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);
    sp<V1_5::IRadioResponse> responseCallback = new TestRadioResponseBase_1_5;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_5 {
        TestChecker check;
        Return<void> cellInfoList_1_5(
                RadioIndicationType type,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::CellInfo>&
                        records) {
            check.set(true);
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ(records.size(), size);
            for (size_t i = 0; i < records.size(); i++) {
                RIL_CellInfo_V1_5 ci {};
                RadioConvUtils::convertCellInfo(ci, testVector[i]);
                test(records[i], ci);
            }
            return Void();
        }

        void test(const V1_5::CellInfo& /*test*/, const RIL_CellInfo_V1_5& /*cellInfo*/) {
            // TODO
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::cellInfoListInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, networkScanResultInd_1_6) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkScanResultInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_6 network_infos[] = {
        // TODO
        {}, {},
    };
    static RIL_NetworkScanResult_V1_6 testVector = {
        RIL_ScanStatus::PARTIAL,
        sizeof(network_infos) / sizeof(network_infos[0]),
        network_infos,
        RIL_Errno::RIL_E_SUCCESS,
    };
    sp<V1_6::IRadioResponse> responseCallback = new TestRadioResponseBase_1_6;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_6 {
        Return<void> networkScanResult_1_6(
                RadioIndicationType type,
                const ::android::hardware::radio::V1_6::NetworkScanResult& result) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_EQ((int)result.status, testVector.status);
            EXPECT_EQ((int)result.error, testVector.error);
            EXPECT_EQ(result.networkInfos.size(), testVector.network_infos_length);
            for (size_t i = 0; i < result.networkInfos.size(); i++) {
                test(result.networkInfos[i], testVector.network_infos[i]);
            }
            return Void();
        }

        void test(const V1_6::CellInfo& test, const RIL_CellInfo_V1_6& cellInfo) {
            // TODO
            testCellInfoRatSpecificInfoDiscriminator(test.ratSpecificInfo.getDiscriminator(),
                    (int)cellInfo.cellInfoType);
            EXPECT_EQ(test.registered, (int)cellInfo.registered);
            EXPECT_EQ((int)test.connectionStatus, (int)cellInfo.connectionStatus);
        }

        void testCellInfoRatSpecificInfoDiscriminator(V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator descriptor,
                int rilCellInfoType) {
            switch (rilCellInfoType) {
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM:
                EXPECT_EQ(V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::gsm, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA:
                EXPECT_EQ(V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::wcdma, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA:
                EXPECT_EQ(V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::cdma, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE:
                EXPECT_EQ(V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::lte, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA:
                EXPECT_EQ(V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::tdscdma, descriptor);
                return;
            case RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR:
                EXPECT_EQ(V1_6::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::nr, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE:
                return;
            }
            EXPECT_TRUE(false);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);
    // invalid params
    radio::networkScanResultInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::networkScanResultInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, networkScanResultInd_1_6_fallback) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::networkScanResultInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    static RIL_CellInfo_V1_6 network_infos[] = {
        // TODO
        {}, {},
    };
    static RIL_NetworkScanResult_V1_6 testVector = {
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
            EXPECT_EQ((int)result.status, testVector.status);
            EXPECT_EQ((int)result.error, testVector.error);
            EXPECT_EQ(result.networkInfos.size(), testVector.network_infos_length);
            for (size_t i = 0; i < result.networkInfos.size(); i++) {
                RIL_CellInfo_V1_5 cellInfo {};
                RadioConvUtils::convertCellInfo(cellInfo, testVector.network_infos[i]);
                test(result.networkInfos[i], cellInfo);
            }
            return Void();
        }

        void test(const V1_5::CellInfo& test, const RIL_CellInfo_V1_5& cellInfo) {
            // TODO
            testCellInfoRatSpecificInfoDiscriminator(test.ratSpecificInfo.getDiscriminator(),
                    (int)cellInfo.cellInfoType);
            EXPECT_EQ(test.registered, (int)cellInfo.registered);
            EXPECT_EQ((int)test.connectionStatus, (int)cellInfo.connectionStatus);
        }

        void testCellInfoRatSpecificInfoDiscriminator(V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator descriptor,
                int rilCellInfoType) {
            switch (rilCellInfoType) {
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM:
                EXPECT_EQ(V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::gsm, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA:
                EXPECT_EQ(V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::wcdma, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA:
                EXPECT_EQ(V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::cdma, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE:
                EXPECT_EQ(V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::lte, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA:
                EXPECT_EQ(V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::tdscdma, descriptor);
                return;
            case RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR:
                EXPECT_EQ(V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::nr, descriptor);
                return;
            case RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE:
                return;
            }
            EXPECT_TRUE(false);
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::networkScanResultInd_1_6(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, simPhonebookChanged) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::simPhonebookChanged(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    sp<V1_6::IRadioResponse> responseCallback = new TestRadioResponseBase_1_6;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_6 {
        Return<void> simPhonebookChanged(RadioIndicationType type) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            return Void();
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // test
    radio::simPhonebookChanged(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);
    tearDown(radio);
}

TEST(ril_service_1_6_ind_tests, simPhonebookRecordsReceived) {
    sp<V1_6::IRadio> radio = setupRadio();
    // test with mRadioIndication == NULL
    radio::simPhonebookChanged(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test with mRadioIndication != NULL
    RIL_PhonebookRecordInfo testRecords[] = {
        { 0, 2, 1, 1, "1234", "1234", {"test@samsung.com"}, {"0000"} },
    };
    static RIL_PhonebookRecInfoResult testVector = {
        NUM_OF_ELEMENTS(testRecords),
        PB_RECEIVED_OK,
        testRecords,
    };
    sp<V1_6::IRadioResponse> responseCallback = new TestRadioResponseBase_1_6;
    struct TestRadioIndicationImpl : public TestRadioIndicationBase_1_6 {
        Return<void> simPhonebookRecordsReceived(
                RadioIndicationType type,
                ::android::hardware::radio::V1_6::PbReceivedStatus status,
                const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::PhonebookRecordInfo>&
                        records) {
            EXPECT_EQ(type, RadioIndicationType::UNSOLICITED);
            EXPECT_TRUE(status ==  V1_6::PbReceivedStatus::PB_RECEIVED_OK ||
                    status ==  V1_6::PbReceivedStatus::PB_RECEIVED_ERROR);
            if (status ==  V1_6::PbReceivedStatus::PB_RECEIVED_OK) {
                EXPECT_EQ(records.size(), (unsigned long)/* NEED TO CHECK */ testVector.loadedCount);
                for (size_t i = 0; i < records.size(); i++) {
                    test(records[i], testVector.mPbRec[i]);
                }
            }
            return Void();
        }

        void test(const V1_6::PhonebookRecordInfo& test,
                const RIL_PhonebookRecordInfo& record) {
            EXPECT_EQ(test.recordId, (unsigned int)/* NEED TO CHECK */ record.recordId);
            EXPECT_EQ(test.emails.size(), (unsigned long)/* NEED TO CHECK */ record.emailSetCount);
            EXPECT_TRUE(test.name == record.name);
            EXPECT_TRUE(test.number == record.number);
            for (size_t i = 0; i < test.emails.size(); i++) {
                EXPECT_TRUE(test.emails[i] == record.emails[i]);
            }
            EXPECT_EQ(test.additionalNumbers.size(), (unsigned long)/* NEED TO CHECK */ record.anrSetCount);
            for (size_t i = 0; i < test.additionalNumbers.size(); i++) {
                EXPECT_TRUE(test.additionalNumbers[i] == record.additionalNumbers[i]);
            }
        }
    };
    sp<TestRadioIndicationImpl> indicationCallback = new TestRadioIndicationImpl;
    radio->setResponseFunctions(responseCallback, indicationCallback);

    // invalid params
    radio::simPhonebookRecordsReceived(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, 0, 0);

    // test
    radio::simPhonebookRecordsReceived(RIL_SOCKET_1, RESPONSE_UNSOLICITED, 0,
            RIL_E_SUCCESS, &testVector, sizeof(testVector));
    tearDown(radio);
}
