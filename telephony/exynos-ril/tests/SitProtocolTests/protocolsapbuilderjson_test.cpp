/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include <gtest/gtest.h>

#include "protocoljsontestadapter.h"
#include "json/protocolsapbuilderjson.h"
#include "simjsondef.h"
#include "modemdata.h"

TEST(ProtocolSapBuilderJson, BuildSapConnect) {
    int testVector[] = { 0, 300, 65535 };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSapBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSapConnect(testVector[i]);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SAP_CONNECT);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i], jsonReqData[ns_sap_connect_req::key_max_msg_size].asInt());
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapBuilderJson, BuildSapDisconnect) {
    ProtocolSapBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSapDisconnect();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_DISCONNECT);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSapBuilderJson, BuildSapTransferApdu) {
    char testApdu[] = "test123456789";

    ProtocolSapBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSapTransferApdu(strlen(testApdu),testApdu);
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_TRANSFER_APDU);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) { delete modemData; modemData = nullptr; }

    ASSERT_NE(false, parsed);
    int apduLen = jsonReqData[ns_sap_transfer_apdu_req::key_req_apdu].size();
    int testApduLen = strlen(testApdu);
    EXPECT_EQ(apduLen, testApduLen);
    for(int i = 0; i < apduLen; i++) {
        EXPECT_EQ(testApdu[i], jsonReqData[ns_sap_transfer_apdu_req::key_req_apdu][i].asInt());
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSapBuilderJson, BuildSapTransferAtr) {
    ProtocolSapBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSapTransferAtr();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_TRANSFER_ATR);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSapBuilderJson, BuildSapPowerSimOnOff) {
    bool testVector[] = {
        false, true
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        ProtocolSapBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSapPowerSimOnOff(testVector[i]);
        ASSERT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        if(testVector[i] == false) {
            EXPECT_EQ(testAdapter.GetId(), SIT_SAP_POWER_SIM_OFF);
        } else {
            EXPECT_EQ(testAdapter.GetId(), SIT_SAP_POWER_SIM_ON);
        }
        EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
        ASSERT_EQ(testAdapter.GetParameter(), nullptr);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapBuilderJson, BuildSapResetSim) {
    ProtocolSapBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSapResetSim();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_RESET_SIM);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSapBuilderJson, BuildSapTransferCardReaderStatus) {
    ProtocolSapBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSapTransferCardReaderStatus();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_TRANSFER_CARD_READER_STATUS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSapBuilderJson, BuildSapTranportProtocol) {
    int testVector[] = { 0, 1 };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolSapBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSapTranportProtocol(testVector[i]);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SAP_SET_TRANPORT_PROTOCOL);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i], jsonReqData[ns_sap_tranport_protocol_req::key_protocol].asInt());
        delete modemData;
        modemData = nullptr;
    }
}
