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
 * simdata_test.cpp
 *
 *  Created on: 2021. 8. 9.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "simdata.h"
#include "librilutils/textutils.h"
#include "rillog.h"

TEST(SimData, SimIoData) {
    RIL_SIM_IO_v6 testVector = {
        1, 2, (char *)"sim_io_path",
        1, 2, 3, (char *)"1A2B3C", (char *)"1234", (char *)"aid1234",
    };
    SimIoData test(RIL_REQUEST_SIM_IO, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetCmd(), testVector.command);
    EXPECT_EQ(test.GetFileId(), testVector.fileid);
    EXPECT_EQ(test.GetIndex(), testVector.p1);
    EXPECT_EQ(test.m_nP2, testVector.p2);
    EXPECT_EQ(test.m_nP3, testVector.p3);
    EXPECT_TRUE(TextUtils::Equals(test.m_strPath, testVector.path));
    EXPECT_TRUE(TextUtils::Equals(test.m_strPin2, testVector.pin2));
    EXPECT_TRUE(TextUtils::Equals(test.m_strAid, testVector.aidPtr));

    SimIoData copy = test;
    copy = test;
    EXPECT_EQ(copy.GetCmd(), test.GetCmd());
    EXPECT_EQ(copy.GetFileId(), test.GetFileId());
    EXPECT_EQ(copy.GetIndex(), test.GetIndex());
    EXPECT_EQ(copy.m_nP2, test.m_nP2);
    EXPECT_EQ(copy.m_nP3, test.m_nP3);
    EXPECT_TRUE(TextUtils::Equals(copy.m_strPath, test.m_strPath));
    EXPECT_TRUE(TextUtils::Equals(copy.m_strPin2, test.m_strPin2));
    EXPECT_TRUE(TextUtils::Equals(copy.m_strAid, test.m_strAid));
}

TEST(SimData, SimAuthentication) {
    RIL_SimAuthentication testVector = {
        3,
        (char *)"auth1234",
        (char *)"aid1234",
    };
    SimAuthentication test(RIL_REQUEST_SIM_AUTHENTICATION, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, 0), -1);
    EXPECT_TRUE(test.encode((char *)&testVector, sizeof(testVector)) >= 0);
    EXPECT_EQ(test.GetAuthContext(), testVector.authContext);
    EXPECT_TRUE(TextUtils::Equals(test.GetAuthentication(), testVector.authData));
    EXPECT_TRUE(TextUtils::Equals(test.GetAid(), testVector.aid));
}

#if 0
// must use a correct input data
TEST(SimData, SimGbaAuth) {
    StringRequestData testVector(0, 0);
    testVector.encode((char *)"02ABCDEF", 3);

    SimGbaAuth test;
    EXPECT_FALSE(test.Parse(0));
    EXPECT_TRUE(test.Parse(&testVector));
    test.GetAuth();
    test.GetLength();
    test.GetGbaType();
    test.GetGbaTag();
    test.Erase();

    StringRequestData testVector2(0, 0);
    testVector.encode((char *)"13ABCDEF", 3);
    test.GetAuth();
    test.GetLength();
    test.GetGbaType();
    test.GetGbaTag();
    test.Erase();
}
#endif

TEST(SimData, SimAPDU) {
    RIL_SIM_APDU testVector = {
        .data = (char *)"12345ABCDEF",
    };
    SimAPDU test(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    // again
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    test.SetP3(3);
    EXPECT_TRUE(TextUtils::Equals(test.GetData(), testVector.data));
    EXPECT_EQ(test.GetP3(), 3);
}

TEST(SimData, CarrierRestrictionsData) {
    RIL_Carrier testAllowedCarriers[] = {
        { (char *)"001", (char *)"01", RIL_CarrierMatchType::RIL_MATCH_ALL, (char *)"0A1B" }
    };
    RIL_Carrier testExcludedCarriers[] = {
        { (char *)"001", (char *)"01", RIL_CarrierMatchType::RIL_MATCH_ALL, (char *)"0A1B" }
    };
    static RIL_CarrierRestrictions_V1_4 testVector = {
        { 1, 1, testAllowedCarriers, testExcludedCarriers, 1 }, 1
    };
    CarrierRestrictionsData test(RIL_REQUEST_SET_CARRIER_RESTRICTIONS, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, 0), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
}
