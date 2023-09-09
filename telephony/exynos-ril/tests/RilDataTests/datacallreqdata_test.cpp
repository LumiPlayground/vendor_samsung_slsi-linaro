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
 * datacallreqdata_test.cpp
 *
 *  Created on: 2021. 8. 9.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "datacallreqdata.h"
#include <librilutils/textutils.h>

TEST(DataCallReqData, SetupDataCallRequestData) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 0));
    const char *testVector[10] = {};
    SetupDataCallRequestData test(requestId, 0);
    EXPECT_EQ(test.encode((char *)testVector, 0), -1);
    EXPECT_EQ(test.encode((char *)testVector, sizeof(testVector)), 0);

    SetupDataCallRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, SetupDataCallRequestData_V1_4) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 4));
    RIL_SetupDataCallInfo_V1_4 testVector = {
            RIL_AccessNetwork::ACCESS_NETWORK_EUTRAN,
            {
                0, (char *)"test_apn",
                PDP_PROTOCOL_TYPE_IP, PDP_PROTOCOL_TYPE_IPV6,
                SETUP_DATA_AUTH_NONE, 0, 0, 0, 0, 0, 0, true, 0, 0, 0, true, false
            },
            true, 0, (char *)"192.168.1.2", (char *)"192.168.1.1"
    };
    SetupDataCallRequestData test(requestId, 0);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_SetupDataCallInfo_V1_4)), 0);

    SetupDataCallRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, SetupDataCallRequestData_V1_5) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 5));
    RIL_LinkAddress testLinkAddr = {
        (char *)"192.168.1.2", 0, 0, 0,
    };
    RIL_SetupDataCallInfo_V1_5 testVector = {
            RIL_AccessNetwork::ACCESS_NETWORK_EUTRAN,
            {
                RIL_DataProfileId::DEFAULT, (char *)"test_apn",
                PDP_PROTOCOL_TYPE_IP, PDP_PROTOCOL_TYPE_IP,
                NO_PAP_NO_CHAP, 0, 0, RIL_DataProfileInfoType::COMMON,
                0, 0, 0, true, 0, 0, 0, 0, true, false
            },
            true, 0, 1, &testLinkAddr, (char *)"192.168.1.1"
    };
    SetupDataCallRequestData test(requestId, 0);
    EXPECT_EQ(test.encode((char *)0, sizeof(RIL_SetupDataCallInfo_V1_5)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_SetupDataCallInfo_V1_5)), 0);
    EXPECT_EQ((unsigned int)test.GetRadioAccessFamily(), testVector.dataProfileInfo.bearerBitmap);
    EXPECT_EQ(test.GetAPNSettingStatus(), testVector.dataProfileInfo.enabled);
    EXPECT_EQ(test.GetMtuSize(), testVector.dataProfileInfo.mtuV4);
    EXPECT_EQ(test.GetMtuV6Size(), testVector.dataProfileInfo.mtuV6);
    EXPECT_EQ(std::stoi(test.IsDataRoamingAllowed()), testVector.roamingAllow);
    EXPECT_EQ(test.IsRoamingAllowed(), testVector.roamingAllow);
    EXPECT_TRUE(TextUtils::Equals(test.GetAddresses(), testVector.linkAddress[0].address));
    test.PrintDebugInfo();

    SetupDataCallRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, DeactivateDataCallRequestData) {

    const char *testVector[] = { "1", "1" };
    DeactivateDataCallRequestData test(RIL_REQUEST_DEACTIVATE_DATA_CALL, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.GetCid(), -1);
    EXPECT_EQ(test.encode((char *)testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetCid(), std::stoi(testVector[0]));
    EXPECT_EQ(test.GetDisconnectReason(), std::stoi(testVector[1]));

    DeactivateDataCallRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, DeactivateDataCallRequestData_V1_2) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_DEACTIVATE_DATA_CALL, HAL_VERSION_CODE(1, 2));
    int testReason[] = {
        RIL_DataRequestReason::NORMAL,
        RIL_DataRequestReason::SHUTDOWN,
        RIL_DataRequestReason::HANDOVER,
        RIL_DataRequestReason::PDP_RESET,
        (RIL_DataRequestReason)0,
    };
    int testRilReason[] = {
        DEACT_REASON_NORMAL,
        DEACT_REASON_RADIO_SHUTDOWN,
        DEACT_REASON_HANDOVER,
        DEACT_REASON_PDP_RESET,
        DEACT_REASON_NORMAL,
    };
    for (size_t i = 0; i < sizeof(testReason) / sizeof(testReason[0]); i++) {
        std::string reason = std::to_string(testReason[i]);
        const char *testVector[] = { "1", reason.c_str() };
        DeactivateDataCallRequestData test(requestId, 0);
        EXPECT_EQ(test.encode((char *)testVector, sizeof(testVector)), 0);
        EXPECT_EQ(test.GetDisconnectReason(), testRilReason[i]);
    }
}

TEST(DataCallReqData, SetInitialAttachApnRequestData) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(1, 0));
    RIL_InitialAttachApn_v15 testVector = {
        (char *)"test_apn",
        (char *)"IP", (char *)"IPV6",
        SETUP_DATA_AUTH_NONE, 0, 0, 0, 0, 0, 0, 0, 0
    };
    SetInitialAttachApnRequestData test(requestId, 0);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_InitialAttachApn_v15) - 1), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_InitialAttachApn_v15)), 0);

    SetInitialAttachApnRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, SetInitialAttachApnRequestData_V1_4) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(1, 4));
    RIL_DataProfileInfo_V1_4 testVector = {
        0, (char *)"test_apn",
        PDP_PROTOCOL_TYPE_IP, PDP_PROTOCOL_TYPE_IPV6,
        SETUP_DATA_AUTH_NONE, 0, 0, 0, 0, 0, 0, true, 0, 0, 0, true, false
    };
    SetInitialAttachApnRequestData test(requestId, 0);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_DataProfileInfo_V1_4)), 0);

    SetInitialAttachApnRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, SetInitialAttachApnRequestData_V1_5) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(1, 5));
    RIL_DataProfileInfo_V1_5 testVector = {
        RIL_DataProfileId::DEFAULT, (char *)"test_apn",
        PDP_PROTOCOL_TYPE_IP, PDP_PROTOCOL_TYPE_IPV4V6,
        NO_PAP_NO_CHAP, 0, 0, RIL_DataProfileInfoType::COMMON,
        0, 0, 0, true, 0, 0, 0, 0, true, false
    };
    SetInitialAttachApnRequestData test(requestId, 0);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(RIL_DataProfileInfo_V1_5)), 0);
    EXPECT_TRUE(TextUtils::Equals(test.GetApn(), testVector.apn));
    EXPECT_TRUE(TextUtils::Equals(test.GetUsername(), testVector.user));
    EXPECT_TRUE(TextUtils::Equals(test.GetPassword(), testVector.password));
    EXPECT_TRUE(TextUtils::Equals(test.GetProtocol(), "IP"));
    EXPECT_EQ(test.GetPdpProtocolType(), (int)testVector.protocol);
    EXPECT_EQ(test.GetAuthType(), (int)testVector.authType);
    EXPECT_TRUE(TextUtils::Equals(test.GetRoamingProtocol(), "IPV4V6"));
    EXPECT_EQ(test.GetPdpRoamingProtocolType(), (int)testVector.roamingProtocol);
    EXPECT_EQ((unsigned int)test.GetSupportedApnTypesBitmap(), testVector.supportedApnTypesBitmap);
    EXPECT_EQ((unsigned int)test.GetBearerBitmap(), testVector.bearerBitmap);
    EXPECT_EQ(test.GetModemCognitive(), testVector.persistent);
    EXPECT_EQ(test.GetMtu(), testVector.mtuV4);
    EXPECT_EQ(test.GetMtuV6(), testVector.mtuV6);
    EXPECT_EQ(test.IsEnabled(), testVector.enabled);
    EXPECT_EQ(test.IsPersist(), testVector.persistent);
    EXPECT_EQ(test.IsPrefferred(), testVector.preferred);

    SetInitialAttachApnRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, SetDataProfileRequestData) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(1, 0));
    RIL_DataProfileInfo_v15 testProfile = {
        0, (char *)"test_apn",
        (char *)"IP", (char *)"IPV6",
        SETUP_DATA_AUTH_NONE, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0
    };
    RIL_DataProfileInfo_v15 *testVector[] = { &testProfile };
    SetDataProfileRequestData test(requestId, 0);
    EXPECT_EQ(test.encode((char *)0, 0), -1);
    EXPECT_EQ(test.encode((char *)0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector) - 1), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);

    SetDataProfileRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, SetDataProfileRequestData_V1_4) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(1, 4));
    RIL_DataProfileInfo_V1_4 testProfile = {
        0, (char *)"test_apn",
        PDP_PROTOCOL_TYPE_IP, PDP_PROTOCOL_TYPE_IPV6,
        SETUP_DATA_AUTH_NONE, 0, 0, 0, 0, 0, 0, true, 0, 0, 0, true, false
    };
    RIL_DataProfileInfo_V1_4 *testVector[] = { &testProfile };
    SetDataProfileRequestData test(requestId, 0);
    EXPECT_EQ(test.encode((char *)testVector, sizeof(testVector)), 0);

    SetDataProfileRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, SetDataProfileRequestData_V1_5) {
    int requestId = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(1, 5));
    RIL_DataProfileInfo_V1_5 testProfile = {
        RIL_DataProfileId::DEFAULT, (char *)"test_apn",
        PDP_PROTOCOL_TYPE_IP, PDP_PROTOCOL_TYPE_IP,
        NO_PAP_NO_CHAP, 0, 0, RIL_DataProfileInfoType::COMMON,
        0, 0, 0, true, 0, 0, 0, 0, true, false
    };
    RIL_DataProfileInfo_V1_5 *testVector[] = { &testProfile };
    SetDataProfileRequestData test(requestId, 0);
    EXPECT_EQ(test.encode((char *)testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetDataProfileInfo(-1), nullptr);
    EXPECT_EQ(test.GetDataProfileInfo(2), nullptr);
    EXPECT_NE(test.GetDataProfileInfo(0), nullptr);
    EXPECT_EQ((unsigned long)test.GetSize(), sizeof(testVector) / sizeof(testVector[0]));

    SetDataProfileRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}

TEST(DataCallReqData, KeepaliveRequestData) {
    RIL_KeepaliveRequest testVector = {
        RIL_KeepaliveType::NATT_IPV4,
        "192.168.1.5",
        3000,
        "12.36.1.2",
        4000,
        15000,
        1
    };
    KeepaliveRequestData test(RIL_REQUEST_START_KEEPALIVE, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);

    KeepaliveRequestData *clone = test.Clone();
    EXPECT_NE(clone, nullptr);
    if (clone) {
        delete clone;
    }
}
