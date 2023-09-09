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
 * callreqdata_test.cpp
 *
 *  Created on: 2021. 8. 9.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "callreqdata.h"
#include <librilutils/textutils.h>

TEST(CallReqData, CallInfo) {
    CallInfo test;
    test.m_state = RIL_CallState::RIL_CALL_ACTIVE;
    EXPECT_EQ(test.GetState(), RIL_CallState::RIL_CALL_ACTIVE);
}

TEST(CallReqData, CallId) {
    CallId test;
    EXPECT_EQ(test.GetCpIndex(0), -1);
    EXPECT_EQ(test.GetCpIndex(10), -1);
    EXPECT_EQ(test.GetCpIndex(1), -1);
    int appId = test.AddCallId(2);
    EXPECT_EQ(test.GetCpIndex(appId), 2);
    test.SyncReady();
    int oldAppId = appId;
    appId = test.AddCallId(2);
    EXPECT_EQ(test.GetCpIndex(appId), 2);
    EXPECT_EQ(appId, oldAppId);
    test.SyncReady();
    test.SyncDone();
    EXPECT_EQ(test.GetCpIndex(appId), -1);
    test.Init();
    for (int i = 1; i <= 10; i++) {
        appId = test.AddCallId(i);
        if (appId > 0) {
            EXPECT_EQ(test.GetCpIndex(appId), i);
        }
    }
}

TEST(CallReqData, EmcInfo) {
    EmergencyNumber test;
    test.Add("001", "01", (char *)"123", 3, (char *)"111", 3, 1, 2, 3);
    test.AddUrn("111", 3);
    test.AddUrn("222", 3);
    test.AddUrn("333", 3);
    test.AddUrn("444", 3);
    test.AddUrn("555", 3);
    test.RemoveUrn("222");
    test.SetEmergencyType(true);
    test.SetEmergencyCallRouting(true);

    EXPECT_TRUE(test.IsEmergencyType());
    EXPECT_TRUE(test.IsEmergencyCallRouting());
    EXPECT_EQ(test.GetCategory(), 1);
    EXPECT_TRUE(TextUtils::Equals(test.GetMcc(), "001"));
    EXPECT_TRUE(TextUtils::Equals(test.GetMnc(), "01"));
    EXPECT_TRUE(TextUtils::Equals(test.GetNumber(), "123"));
    EXPECT_EQ(test.GetConditions(), 2);
    EXPECT_EQ(test.GetSource(), 3);
    EXPECT_EQ(test.GetUrnCount(), 3);
    test.Clear();
    EXPECT_FALSE(test.IsEmergencyType());
    EXPECT_FALSE(test.IsEmergencyCallRouting());
    EXPECT_EQ(test.GetCategory(), -1);
    EXPECT_FALSE(TextUtils::Equals(test.GetMcc(), "001"));
    EXPECT_FALSE(TextUtils::Equals(test.GetMnc(), "01"));
    EXPECT_FALSE(TextUtils::Equals(test.GetNumber(), "123"));
    EXPECT_EQ(test.GetConditions(), -1);
    EXPECT_EQ(test.GetSource(), -1);
    EXPECT_EQ(test.GetUrnCount(), 0);
}

TEST(CallReqData, EccInfo) {
    EccInfo test;
    test.AddEmergencyNumber("123", "456", "12345", 5, 0, 0, 0);
    EXPECT_EQ(test.GetCount(), 1);
    EXPECT_NE(test.GetEmergencyNumberList(), nullptr);
}

TEST(CallReqData, DtmfInfo) {
    DtmfInfo test(RIL_REQUEST_DTMF, 0);
    test = DtmfInfo(RIL_REQUEST_DTMF, 0, '1');
    EXPECT_TRUE(TextUtils::Equals(test.m_szDtmf, "1"));
    test = DtmfInfo(RIL_REQUEST_DTMF, 0, "987");
    EXPECT_TRUE(TextUtils::Equals(test.m_szDtmf, "987"));
}

TEST(CallReqData, CallDialReqData) {
    RIL_UUS_Info testUusInfo = {
        RIL_UUS_Type::RIL_UUS_TYPE1_IMPLICIT,
        RIL_UUS_DCS::RIL_UUS_DCS_IA5c,
        3, (char *)"123",
    };
    RIL_Dial testVector = {
        (char *)"01012345678",
        ClirType::CLIR_DEFAULT, &testUusInfo
    };
    CallDialReqData test(RIL_REQUEST_DIAL, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_TRUE(TextUtils::Equals(test.GetNumber(), testVector.address));
    EXPECT_EQ(test.GetClirType(), testVector.clir);
    test.SetClirType(ClirType::CLIR_INVOCATION);
    EXPECT_EQ(test.GetClirType(), ClirType::CLIR_INVOCATION);
    EXPECT_EQ(test.GetCallType(), CallType::CALL_TYPE_VOICE);
    test.SetCallType(CallType::CALL_TYPE_EMERGENCY);
    EXPECT_EQ(test.GetCallType(), CallType::CALL_TYPE_EMERGENCY);
    EXPECT_EQ(test.GetEccCategory(), 0);
    UusInfo uus = test.GetUusInfo();
    EXPECT_EQ(uus.m_uusType, testUusInfo.uusType);
    EXPECT_EQ(uus.m_uusDcs, testUusInfo.uusDcs);
    EXPECT_EQ(uus.m_uusLength, testUusInfo.uusLength);
    EXPECT_TRUE(memcmp(uus.m_uusData, testUusInfo.uusData, uus.m_uusLength) == 0);
}

TEST(CallReqData, CallEmergencyDialReqData) {
    RIL_EmergencyDial testVector = {};
    CallEmergencyDialReqData test(RIL_REQUEST_EMERGENCY_DIAL, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector) - 1), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetCategories(), testVector.categories);
    EXPECT_EQ(test.GetLenUrns(), testVector.len_urns);
    EXPECT_EQ(test.GetUrns(), nullptr);
    EXPECT_EQ(test.GetRouting(), testVector.routing);
    EXPECT_EQ(test.GetHasKnownUserIntentEmergency(), testVector.hasKnownUserIntentEmergency);
    EXPECT_EQ(test.GetIsTesting(), testVector.isTesting);
}
