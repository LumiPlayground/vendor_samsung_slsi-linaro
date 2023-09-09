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
 * miscdata_test.cpp
 *
 *  Created on: 2021. 8. 9.
 *      Author: sungwoo48.choi
 */
#include <testutils.h>
#include "miscdata.h"
#include <librilutils/textutils.h>

TEST(MiscData, ApnSettingsData) {
    ApnSettingsData test(RIL_REQUEST_OEM_APN_SETTINGS, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, 1), -1);
    EXPECT_EQ(test.encode((char *)"AT+VZWAPNE?", strlen("AT+VZWAPNE?")), 0);
    char testVector[] = "AT+VZWAPNE=wapn_val,,apnni_val\r\n";
    EXPECT_EQ(test.encode(testVector, strlen(testVector)), 0);
    std::string value1 = test.GetApnValues((char *)"wapn");
    EXPECT_TRUE(TextUtils::Equals(value1, "wapn_val"));
    std::string value2 = test.GetApnValues((char *)"apncl");
    EXPECT_TRUE(TextUtils::Equals(value2, ""));
    std::string value3 = test.GetApnValues((char *)"apnni");
    EXPECT_TRUE(TextUtils::Equals(value3, "apnni_val"));
    test.ToString(testVector, strlen(testVector));
}

TEST(MiscData, CarrierInfoForImsiEncryptionData) {
    RIL_CarrierInfoForImsiEncryption_1_6 testVector = {
        (char *)"00101", (char *)"01012",
        (uint8_t *)"12345678", 8,
        (char *)"1234",
        10LL, PublicKeyType::EPDG,
    };
    CarrierInfoForImsiEncryptionData test(ENCODE_REQUEST(
            RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION, HAL_VERSION_CODE(1, 6)), 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(RIL_CarrierInfoForImsiEncryption_1_6)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_CarrierInfoForImsiEncryption_1_6)), 0);
    EXPECT_TRUE(TextUtils::Equals(test.GetMcc(), std::string(testVector.mcc).substr(0, 3)));
    EXPECT_TRUE(TextUtils::Equals(test.GetMnc(), std::string(testVector.mnc).substr(0, 3)));
    EXPECT_EQ(test.GetCarrierKeyLen(), testVector.carrierKeyLength);
    EXPECT_TRUE(memcmp(test.GetCarrierKey(), testVector.carrierKey, test.GetCarrierKeyLen()) == 0);
    EXPECT_EQ((unsigned long)/* NEED TO CHECK */ test.GetKeyIdLen(), strlen(testVector.keyIdentifier));
    EXPECT_TRUE(test.GetEpirationTime() == testVector.expirationTime);
    EXPECT_EQ(test.GetKeyType(), testVector.keyType);
}

TEST(MiscData, SignalStrengthReportingCriteria_V1_2) {
    int testThreshold[5] = { -120, -100, -80, -60, -20 };
    RIL_SignalStrengthReportingCriteria_V1_2 testVector = {
        3000, 20,
        5, (uint32_t *)testThreshold,
        RIL_AccessNetwork::ACCESS_NETWORK_EUTRAN,
    };
    SignalStrengthReportingCriteria test(
            RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetHysteresisMs(), testVector.hysteresisMs);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetHysteresisDb(), testVector.hysteresisDb);
    EXPECT_EQ(test.GetNumOfThresholdsDbm(), (int)testVector.numOfThresholdsDbm);
    EXPECT_TRUE(memcmp(test.GetThresholdsDbm(), testVector.thresholdsDbm,
            sizeof(int) * test.GetNumOfThresholdsDbm()) == 0);
    EXPECT_EQ((int)test.GetAccessNetwork(), testVector.accessNetwork);
}

TEST(MiscData, SignalStrengthReportingCriteria_V1_5) {
    int testThreshold[5] = { -120, -100, -80, -60, -20 };
    RIL_SignalStrengthReportingCriteria_V1_5 testVector = {
        RIL_AccessNetwork::ACCESS_NETWORK_EUTRAN,
        {
            RIL_SignalMeasurementType::RSRP, 3000, 20,
            5, testThreshold, true
        },
    };
    SignalStrengthReportingCriteria test(ENCODE_REQUEST(
            RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, HAL_VERSION_CODE(1, 5)),
            0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetHysteresisMs(), testVector.signalThresholdInfo.hysteresisMs);
    EXPECT_EQ(test.GetHysteresisDb(), testVector.signalThresholdInfo.hysteresisDb);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetNumOfThresholdsDbm(), testVector.signalThresholdInfo.len_thresholds);
    EXPECT_TRUE(memcmp(test.GetThresholdsDbm(), testVector.signalThresholdInfo.thresholds,
            sizeof(int) * test.GetNumOfThresholdsDbm()) == 0);
    EXPECT_EQ((int)test.GetAccessNetwork(), testVector.accessNetwork);
    EXPECT_EQ((int)test.GetMeasureType(), testVector.signalThresholdInfo.signalMeasurement);
    EXPECT_EQ(test.GetIsEnabled(), testVector.signalThresholdInfo.isEnabled);
}

TEST(MiscData, SignalStrengthReportingCriteria_V2_0) {
    using SignalThresholdInfo =
        android::hardware::radio::impl::network::V2_0::RIL_SignalThresholdInfo;
    SignalThresholdInfo testVector[] = {
        // setSignalStrengthReportingCriteria_EmptyThresholds
        {
            RIL_SignalMeasurementType::RSSI, 0, 0,
            {}, true, ACCESS_NETWORK_GERAN,
        },
        // setSignalStrengthReportingCriteria_Geran
        {
            RIL_SignalMeasurementType::RSSI, 5000, 2,
            {-109, -103, -97, -89}, true, ACCESS_NETWORK_GERAN,
        },
        // setSignalStrengthReportingCriteria_Utran
        {
            RIL_SignalMeasurementType::RSSI, 5000, 2,
            {-110, -97, -73, -49, -25}, true, ACCESS_NETWORK_UTRAN,
        },
        // setSignalStrengthReportingCriteria_Eutran_RSRP
        {
            RIL_SignalMeasurementType::RSRP, 5000, 2,
            {-128, -108, -88, -68}, true, ACCESS_NETWORK_EUTRAN,
        },
        // setSignalStrengthReportingCriteria_Eutran_RSRQ
        {
            RIL_SignalMeasurementType::RSRQ, 5000, 2,
            {-27, -20, -13, -6}, true, ACCESS_NETWORK_EUTRAN,
        },
        // setSignalStrengthReportingCriteria_Eutran_RSSNR
        {
            RIL_SignalMeasurementType::RSSNR, 5000, 2,
            {-10, 0, 10, 20}, true, ACCESS_NETWORK_EUTRAN,
        },
        // setSignalStrengthReportingCriteria_Cdma2000
        {
            RIL_SignalMeasurementType::RSSI, 5000, 2,
            {-105, -90, -75, -65}, true, ACCESS_NETWORK_CDMA2000,
        },
        // setSignalStrengthReportingCriteria_NGRAN_SSRSRP
        {
            RIL_SignalMeasurementType::SSRSRP, 5000, 0,
            {-105, -90, -75, -65}, true, ACCESS_NETWORK_NGRAN,
        },
        // setSignalStrengthReportingCriteria_NGRAN_SSRSRQ
        {
            RIL_SignalMeasurementType::SSRSRQ, 5000, 0,
            {-43, -20, 0, 20}, true, ACCESS_NETWORK_NGRAN,
        },
        // setSignalStrengthReportingCriteria_NGRAN_SSRSRQ
        {
            RIL_SignalMeasurementType::SSSINR, 5000, 0,
            {-10, 3, 16, 18}, true, ACCESS_NETWORK_NGRAN,
        },
    };
    size_t size = NUM_ELEMS(testVector);
    SignalStrengthReportingCriteria_V2_0 test(
            RIL_REQUEST_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, 0);
    EXPECT_TRUE(test.encode((char *)testVector, sizeof(testVector)) == 0);
    EXPECT_TRUE(test.isValidArguements());
    EXPECT_EQ(test.GetSignalThresholdInfoNum(), (int)size);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(test.at(i) == testVector[i]);
    }
}

TEST(MiscData, SignalStrengthReportingCriteria_V2_0_invalidHysteresisDb) {
    using SignalThresholdInfo =
        android::hardware::radio::impl::network::V2_0::RIL_SignalThresholdInfo;
    SignalThresholdInfo testVector[] = {
        // setSignalStrengthReportingCriteria_invalidHysteresisDb
        {
            RIL_SignalMeasurementType::RSSI, 5000, 10,
            {-109, -103, -97, -89}, true, ACCESS_NETWORK_GERAN,
        },
        // setSignalStrengthReportingCriteria_EmptyThresholds
        {
            RIL_SignalMeasurementType::RSSI, 0, 0,
            {}, true, ACCESS_NETWORK_GERAN,
        },
    };
    SignalStrengthReportingCriteria_V2_0 test(
            RIL_REQUEST_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, 0);
    EXPECT_TRUE(test.encode((char *)testVector, sizeof(testVector)) == 0);
    EXPECT_FALSE(test.isValidArguements());
}

TEST(MiscData, LinkCapacityReportingCriteria) {
    uint32_t testThresholdDl[5] = { 0, 100000, 200000, 300000, 500000 };
    uint32_t testThresholdUl[5] = { 0, 100000, 200000, 300000, 500000 };
    RIL_LinkCapacityReportingCriteria_V1_5 testVector = {
        3000, 500000, 300000,
        5, testThresholdDl,
        5, testThresholdUl,
        (RIL_AccessNetwork_V1_5)RIL_AccessNetwork::ACCESS_NETWORK_EUTRAN,
    };
    LinkCapacityReportingCriteria test(ENCODE_REQUEST(
            RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA, HAL_VERSION_CODE(1, 6)),
            0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetHysteresisMs(), testVector.hysteresisMs);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetHysteresisDlKbps(), testVector.hysteresisDlKbps);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetHysteresisUlKpbs(), testVector.hysteresisUlKbps);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetNumOfThresholdsDownlinkKbps(), testVector.numOfThresholdsDownlinkKbps);
    EXPECT_TRUE(memcmp(test.GetThresholdsDownlinkKbps(), testVector.thresholdsDownlinkKbps,
            sizeof(uint32_t) * test.GetNumOfThresholdsDownlinkKbps()) == 0);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetNumOfThresholdsUplinkKbps(), testVector.numOfThresholdsUplinkKbps);
    EXPECT_TRUE(memcmp(test.GetThresholdsUplinkKbps(), testVector.thresholdsDownlinkKbps,
            sizeof(uint32_t) * test.GetNumOfThresholdsUplinkKbps()) == 0);
    EXPECT_EQ((int)test.GetAccessNetwork(), testVector.accessNetwork);
}

TEST(MiscData, SetActivateVsimReqData) {
    RIL_SetActivateVsim testVector = {
        0, (char *)"8912345678990", (char *)"00101012345", (char *)"00101", 0, 1
    };
    SetActivateVsimReqData test(RIL_REQUEST_SET_ACTIVATE_VSIM, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetSlotId(), testVector.simSlot);
    EXPECT_TRUE(TextUtils::Equals(test.GetIccid(), testVector.iccid));
    EXPECT_TRUE(TextUtils::Equals(test.GetImsi(), testVector.imsi));
    EXPECT_TRUE(TextUtils::Equals(test.GetHomePlmn(), testVector.hplmn));
    EXPECT_EQ(test.GetVsimState(), testVector.vsimState);
    EXPECT_EQ(test.GetVsimCardType(), testVector.vsimCardType);
}

TEST(MiscData, UiccSubscription) {
    RIL_SelectUiccSub testVector = {
        0, 0, RIL_SubscriptionType::RIL_SUBSCRIPTION_1,
        RIL_UiccSubActStatus::RIL_UICC_SUBSCRIPTION_DEACTIVATE,
    };
    UiccSubscription test(RIL_REQUEST_SET_ACTIVATE_VSIM, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetSlot(), testVector.slot);
    EXPECT_EQ(test.GetAppIndex(), testVector.app_index);
    EXPECT_EQ(test.GetSubscriptionType(), (int)testVector.sub_type);
    EXPECT_EQ(test.GetActivationStatus(), (int)testVector.act_status);

    UiccSubscription copy(RIL_REQUEST_SET_ACTIVATE_VSIM, 0);
    copy = test;
    EXPECT_EQ(copy.GetSlot(), test.GetSlot());
    EXPECT_EQ(copy.GetAppIndex(), test.GetAppIndex());
    EXPECT_EQ(copy.GetSubscriptionType(), test.GetSubscriptionType());
    EXPECT_EQ(copy.GetActivationStatus(), test.GetActivationStatus());
}
