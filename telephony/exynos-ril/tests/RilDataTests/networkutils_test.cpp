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
 * networkutils_test.cpp
 *
 *  Created on: 2021. 8. 3.
 */
#include <gtest/gtest.h>
#include "slsi/radio_v1_5.h"
#include <telephony/ril_mnc.h>
#include "librilutils/systemproperty.h"
#include "librilutils/textutils.h"
#include "networkutils.h"
#include "rildef.h"

TEST(NetworkUtils, getRadioTechnologyFromAccessNetwork) {
    struct {
        int accessNetwork;
        int result;
    } testVector[] = {
        { ACCESS_NETWORK_UNKNOWN, RADIO_TECH_UNKNOWN },
        { ACCESS_NETWORK_GERAN, RADIO_TECH_GPRS },
        { ACCESS_NETWORK_UTRAN, RADIO_TECH_UMTS },
        { ACCESS_NETWORK_EUTRAN, RADIO_TECH_LTE },
        { ACCESS_NETWORK_CDMA2000, RADIO_TECH_1xRTT },
        { ACCESS_NETWORK_NGRAN, RADIO_TECH_NR },
        { -1, RADIO_TECH_UNKNOWN },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(NetworkUtils::getRadioTechnologyFromAccessNetwork(testVector[i].accessNetwork),
                testVector[i].result);
    }
}

TEST(NetworkUtils, getRadioTechnologyFromRadioAccessNetwork) {
    struct {
        int ran;
        int result;
    } testVector[] = {
        { RIL_RAN_UNKNOWN, RADIO_TECH_UNKNOWN },
        { RIL_RAN_GERAN, RADIO_TECH_GPRS },
        { RIL_RAN_UTRAN, RADIO_TECH_UMTS },
        { RIL_RAN_EUTRAN, RADIO_TECH_LTE },
        { RIL_RAN_NGRAN, RADIO_TECH_NR },
        { -1, RADIO_TECH_UNKNOWN },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(testVector[i].ran),
                testVector[i].result);
    }
}

TEST(NetworkUtils, getRadioTechnologyToAccessNetworkType) {
    struct {
        int rat;
        int result;
    } testVector[] = {
        { RADIO_TECH_GPRS, ACCESS_NETWORK_GERAN },
        { RADIO_TECH_EDGE, ACCESS_NETWORK_GERAN },
        { RADIO_TECH_GSM, ACCESS_NETWORK_GERAN },
        { RADIO_TECH_UMTS, ACCESS_NETWORK_UTRAN },
        { RADIO_TECH_HSDPA, ACCESS_NETWORK_UTRAN },
        { RADIO_TECH_HSPAP, ACCESS_NETWORK_UTRAN },
        { RADIO_TECH_HSUPA, ACCESS_NETWORK_UTRAN },
        { RADIO_TECH_HSPA, ACCESS_NETWORK_UTRAN },
        { RADIO_TECH_TD_SCDMA, ACCESS_NETWORK_UTRAN },
        { RADIO_TECH_IS95A, ACCESS_NETWORK_CDMA2000 },
        { RADIO_TECH_IS95B, ACCESS_NETWORK_CDMA2000 },
        { RADIO_TECH_1xRTT, ACCESS_NETWORK_CDMA2000 },
        { RADIO_TECH_EVDO_0, ACCESS_NETWORK_CDMA2000 },
        { RADIO_TECH_EVDO_A, ACCESS_NETWORK_CDMA2000 },
        { RADIO_TECH_EVDO_B, ACCESS_NETWORK_CDMA2000 },
        { RADIO_TECH_LTE, ACCESS_NETWORK_EUTRAN },
        { RADIO_TECH_LTE_CA, ACCESS_NETWORK_EUTRAN },
        { RADIO_TECH_IWLAN, ACCESS_NETWORK_IWLAN },
        { RADIO_TECH_NR, ACCESS_NETWORK_NGRAN },
        { RADIO_TECH_UNKNOWN, ACCESS_NETWORK_UNKNOWN },
        { -1, ACCESS_NETWORK_UNKNOWN },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(NetworkUtils::getRadioTechnologyToAccessNetworkType(testVector[i].rat),
                testVector[i].result);
    }
}

TEST(NetworkUtils, getDefaultRAF) {
    std::string name = "persist.vendor.ril.support_nr";
    std::string prop = SystemProperty::Get(name, "");

    SystemProperty::Set(name, 0);
    EXPECT_EQ(NetworkUtils::getDefaultRAF(), RAF_RIL_3GPP | RAF_RIL_3GPP2);
    SystemProperty::Set(name, 1);
    EXPECT_EQ(NetworkUtils::getDefaultRAF(), RAF_RIL_3GPP | RAF_RIL_3GPP2 | RAF_NR);
    SystemProperty::Set(name, prop);
}

TEST(NetworkUtils, getTestRAF) {
    std::string name = "persist.vendor.ril.test_raf0";
    std::string prop = SystemProperty::Get(name, "");

    SystemProperty::Set(name, "");
    EXPECT_EQ(NetworkUtils::getTestRAF(0), -1);
    SystemProperty::Set(name, 0x1FFF);
    EXPECT_EQ(NetworkUtils::getTestRAF(0), 0x1FFF);
    SystemProperty::Set(name, prop);
}

TEST(NetworkUtils, getRadioTechnologyString) {
    struct {
        int rat;
        const char *result;
    } testVector[] = {
        { RADIO_TECH_GPRS, "GPRS" },
        { RADIO_TECH_EDGE, "EDGE" },
        { RADIO_TECH_GSM, "GSM" },
        { RADIO_TECH_UMTS, "UMTS" },
        { RADIO_TECH_HSDPA, "HSDPA" },
        { RADIO_TECH_HSPAP, "HSPAP" },
        { RADIO_TECH_HSUPA, "HSUPA" },
        { RADIO_TECH_HSPA, "HSPA" },
        { RADIO_TECH_TD_SCDMA, "TD_SCDMA" },
        { RADIO_TECH_IS95A, "IS95A" },
        { RADIO_TECH_IS95B, "IS95B" },
        { RADIO_TECH_1xRTT, "1xRTT" },
        { RADIO_TECH_EVDO_0, "EVDO_0" },
        { RADIO_TECH_EVDO_A, "EVDO_A" },
        { RADIO_TECH_EVDO_B, "EVDO_B" },
        { RADIO_TECH_EHRPD, "EHRPD" },
        { RADIO_TECH_LTE, "LTE" },
        { RADIO_TECH_LTE_CA, "LTE_CA" },
        { RADIO_TECH_IWLAN, "IWLAN" },
        { RADIO_TECH_NR, "NR" },
        { RADIO_TECH_UNKNOWN, "UNKNOWN" },
        { -1, "UNKNOWN" },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(TextUtils::Equals(
                NetworkUtils::getRadioTechnologyString(testVector[i].rat),
                testVector[i].result));
    }
}

TEST(NetworkUtils, printRafDetail) {
    int raf = NETWORK_TYPE_BITMAP_GSM |
            NETWORK_TYPE_BITMAP_WCDMA |
            NETWORK_TYPE_BITMAP_CDMA |
            NETWORK_TYPE_BITMAP_EVDO |
            NETWORK_TYPE_BITMAP_LTE |
            NETWORK_TYPE_BITMAP_NR |
            NETWORK_TYPE_BITMAP_TDS_CDMA;
    NetworkUtils::printRafDetail(raf);
}

TEST(NetworkUtils, getRegStateString) {
    struct {
        int state;
        const char *result;
    } testVector[] = {
        { RIL_NOT_REG_AND_NOT_SEARCHING, "NOT_REG_AND_NOT_SEARCHING" },
        { RIL_REG_HOME, "REG_HOME" },
        { RIL_NOT_REG_AND_SEARCHING, "NOT_REG_AND_SEARCHING" },
        { RIL_REG_DENIED, "REG_DENIED" },
        { RIL_UNKNOWN, "UNKNOWN" },
        { RIL_REG_ROAMING, "REG_ROAMING" },
        { RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING, "NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING" },
        { RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING, "NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING" },
        { RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE, "REG_DENIED_AND_EMERGENCY_AVAILABLE" },
        { RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE, "UNKNOWN_AND_EMERGENCY_AVAILABLE" },
        { -1, "UNKNOWN" },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(TextUtils::Equals(
                NetworkUtils::getRegStateString(testVector[i].state),
                testVector[i].result));
    }
}

TEST(NetworkUtils, getAccessNewtorkString) {
    struct {
        int accessNetwork;
        const char *result;
    } testVector[] = {
        { ACCESS_NETWORK_UNKNOWN, "UNKNOWN" },
        { ACCESS_NETWORK_GERAN, "GERAN" },
        { ACCESS_NETWORK_UTRAN, "UTRAN" },
        { ACCESS_NETWORK_EUTRAN, "EUTRAN" },
        { ACCESS_NETWORK_CDMA2000, "CDMA2000" },
        { ACCESS_NETWORK_NGRAN, "NGRAN" },
        { -1, "UNKNOWN" },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(TextUtils::Equals(
                NetworkUtils::getAccessNewtorkString(testVector[i].accessNetwork),
                testVector[i].result));
    }
}

TEST(NetworkUtils, isInService) {
    EXPECT_TRUE(NetworkUtils::isInService(RIL_REG_HOME));
    EXPECT_TRUE(NetworkUtils::isInService(RIL_REG_ROAMING));
    EXPECT_FALSE(NetworkUtils::isInService(RIL_REG_DENIED));
}

TEST(NetworkUtils, isEmergencyOnly) {
    EXPECT_TRUE(NetworkUtils::isEmergencyOnly(RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING));
    EXPECT_TRUE(NetworkUtils::isEmergencyOnly(RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING));
    EXPECT_TRUE(NetworkUtils::isEmergencyOnly(RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE));
    EXPECT_TRUE(NetworkUtils::isEmergencyOnly(RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE));
    EXPECT_FALSE(NetworkUtils::isEmergencyOnly(RIL_REG_HOME));
}

TEST(NetworkUtils, isGsm) {
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_GPRS));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_EDGE));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_UMTS));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_HSDPA));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_HSUPA));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_HSPA));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_LTE));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_HSPAP));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_GSM));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_TD_SCDMA));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_IWLAN));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_LTE_CA));
    EXPECT_TRUE(NetworkUtils::isGsm(RADIO_TECH_NR));
    EXPECT_FALSE(NetworkUtils::isGsm(RADIO_TECH_EHRPD));
}

TEST(NetworkUtils, isCdma) {
    EXPECT_TRUE(NetworkUtils::isCdma(RADIO_TECH_IS95A));
    EXPECT_TRUE(NetworkUtils::isCdma(RADIO_TECH_IS95B));
    EXPECT_TRUE(NetworkUtils::isCdma(RADIO_TECH_1xRTT));
    EXPECT_TRUE(NetworkUtils::isCdma(RADIO_TECH_EVDO_0));
    EXPECT_TRUE(NetworkUtils::isCdma(RADIO_TECH_EVDO_A));
    EXPECT_TRUE(NetworkUtils::isCdma(RADIO_TECH_EVDO_B));
    EXPECT_TRUE(NetworkUtils::isCdma(RADIO_TECH_EHRPD));
    EXPECT_FALSE(NetworkUtils::isCdma(RADIO_TECH_LTE));
}

TEST(NetworkUtils, isLte) {
    EXPECT_TRUE(NetworkUtils::isLte(RADIO_TECH_LTE));
    EXPECT_TRUE(NetworkUtils::isLte(RADIO_TECH_LTE_CA));
    EXPECT_FALSE(NetworkUtils::isLte(RADIO_TECH_UMTS));
}

TEST(NetworkUtils, mcc_mnc) {
    EXPECT_EQ(NetworkUtils::fetchMcc("abcde"), INT_MAX);
    EXPECT_EQ(NetworkUtils::fetchMcc("45005"), 450);

    EXPECT_EQ(NetworkUtils::fetchMnc(nullptr), INT_MAX);
    EXPECT_EQ(NetworkUtils::fetchMnc("450ab"), INT_MAX);
    EXPECT_EQ(NetworkUtils::fetchMnc("45005#"), ril::util::mnc::encode(5, 2));
    EXPECT_EQ(NetworkUtils::fetchMnc("45005"), ril::util::mnc::encode(5, 2));
    EXPECT_EQ(NetworkUtils::fetchMnc("310480"), ril::util::mnc::encode(480, 3));

    EXPECT_EQ(NetworkUtils::fetchMncNoEncoding(nullptr), INT_MAX);
    EXPECT_EQ(NetworkUtils::fetchMncNoEncoding("450ab"), INT_MAX);
    EXPECT_EQ(NetworkUtils::fetchMncNoEncoding("45005#"), 5);
    EXPECT_EQ(NetworkUtils::fetchMncNoEncoding("45005"), 5);
    EXPECT_EQ(NetworkUtils::fetchMncNoEncoding("310480"), 480);
}

TEST(NetworkUtils, isLteEnhancedAvailable) {
    struct {
        const char *alpha;
        int result;
    } testVector[] = {
        { "test...1", 1 },
        { "test...2", 2 },
        { "test...3", 0 },
        { "test", 0 },
        { "", 0 },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_EQ(NetworkUtils::isLteEnhancedAvailable(testVector[i].alpha),
                testVector[i].result);
    }
}

TEST(NetworkUtils, addLteEnhancedAvailableToAlphaTag) {
    std::string testAlpha = "test_alpha";
    stringstream ss;
    ss << testAlpha << "..." << 1;
    EXPECT_TRUE(TextUtils::Equals(NetworkUtils::addLteEnhancedAvailableToAlphaTag(testAlpha, 1),
            ss.str()));
}

TEST(NetworkUtils, removeLteEnhancedAvailableFromAlphaTag) {
    std::string testAlpha = "test_alpha";
    stringstream ss;
    ss << testAlpha << "..." << 1;
    EXPECT_TRUE(TextUtils::Equals(NetworkUtils::removeLteEnhancedAvailableFromAlphaTag(ss.str()),
            testAlpha));

    ss.str(testAlpha);
    ss << "..." << 3;
    EXPECT_TRUE(TextUtils::Equals(NetworkUtils::removeLteEnhancedAvailableFromAlphaTag(ss.str()),
            ss.str()));
}
