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
 * protocolpsutils_test.cpp
 *
 */

#include <gtest/gtest.h>

#include "rildef.h"
#include "sitdef.h"
#include "protocolpsutil.h"
#include "apnsetting.h"
#include <librilutils/textutils.h>

TEST(ProtocolPsUtils, GetPcsfReqTypeforNullAPN) {
    ApnSetting *apn = ApnSetting::NewInstance("test", "testapn", APN_TYPE_BIT_DEFAULT, "user", "password", "IPV4", "IPV4", SETUP_DATA_AUTH_NONE);

    GetPcscfReqTypeforNullAPN(apn, "IPV4");
}

TEST(ProtocolPsUtils, isRatForCMDA) {
    EXPECT_EQ(isRatForCDMA(RADIO_TECH_IS95A), true);
    EXPECT_EQ(isRatForCDMA(RADIO_TECH_IS95B), true);
    EXPECT_EQ(isRatForCDMA(RADIO_TECH_1xRTT), true);
    EXPECT_EQ(isRatForCDMA(RADIO_TECH_EVDO_0), true);
    EXPECT_EQ(isRatForCDMA(RADIO_TECH_EVDO_A), true);
    EXPECT_EQ(isRatForCDMA(RADIO_TECH_EVDO_B), true);
    EXPECT_EQ(isRatForCDMA(RADIO_TECH_LTE), false);
    EXPECT_EQ(isRatForCDMA(RADIO_TECH_HSDPA), false);
}

// Deprecated?
TEST(ProtocolPsUtils, FillHandoverIpAddressLegacy) {
    FillHandoverIpAddressLegacy(0);
}

TEST(ProtocolPsUtils, ParseHandoverAddresses) {
    sockaddr_in sa;
    sockaddr_in6 sa6;
    char t4[INET_ADDRSTRLEN+1];
    char t6[INET6_ADDRSTRLEN+1];

    ParseHandoverAddresses("10.20.30.40", sa, sa6, t4, t6);
    ParseHandoverAddresses("2001:2003:4001::5455", sa, sa6, t4, t6);
}

TEST(ProtocolPsUtils, ConvertConnectionCapabilities) {
    ConvertConnectionCapabilities("ConnectionCapa");
}

TEST(ProtocolPsUtils, FillIpAddr) {
    char ip4[INET_ADDRSTRLEN];
    char ip6[INET6_ADDRSTRLEN];
    FillIpAddr(false, "10.20.30.40", ip4);
    FillIpAddr(true, "1002:2001:3001::44", ip6);
}