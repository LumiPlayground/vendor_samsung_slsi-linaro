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

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "protocoladapter.h"
#include "legacy/protocolsapbuilderlegacy.h"
#include <librilutils/textutils.h>

TEST(ProtocolSapBuilderLegacy, ProtocolSapBuilderLegacy_BuildSapConnect) {
    ProtocolSapBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSapConnect(100);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_CONNECT);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_connect_req));
    EXPECT_EQ(((sit_sap_connect_req *)modemData->GetRawData())->max_msg_size, 100);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapBuilderLegacy, ProtocolSapBuilderLegacy_BuildSapDisconnect) {
    ProtocolSapBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSapDisconnect();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_DISCONNECT);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapBuilderLegacy, ProtocolSapBuilderLegacy_BuildSapTransferApdu) {
    ProtocolSapBuilderLegacy testBuilder;

    char testStr[] = "test123456789";
    ModemData *modemData = testBuilder.BuildSapTransferApdu(strlen(testStr), testStr);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_TRANSFER_APDU);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + 2 + strlen(testStr));
    sit_sap_transfer_apdu_req *p = (sit_sap_transfer_apdu_req *)modemData->GetRawData();
    EXPECT_EQ(p->apdu_len, strlen(testStr));
    EXPECT_TRUE(memcmp(p->apdu, testStr, p->apdu_len) == 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapBuilderLegacy, ProtocolSapBuilderLegacy_BuildSapTransferAtr) {
    ProtocolSapBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSapTransferAtr();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_TRANSFER_ATR);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapBuilderLegacy, ProtocolSapBuilderLegacy_BuildSapPowerSimOnOff) {
    ProtocolSapBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSapPowerSimOnOff(true);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_POWER_SIM_ON);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }

    modemData = testBuilder.BuildSapPowerSimOnOff(false);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter2(modemData);
    EXPECT_EQ(testAdapter2.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter2.GetId(), SIT_SAP_POWER_SIM_OFF);
    EXPECT_EQ(testAdapter2.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapBuilderLegacy, ProtocolSapBuilderLegacy_BuildSapResetSim) {
    ProtocolSapBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSapResetSim();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_RESET_SIM);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapBuilderLegacy, ProtocolSapBuilderLegacy_BuildSapTransferCardReaderStatus) {
    ProtocolSapBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSapTransferCardReaderStatus();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_TRANSFER_CARD_READER_STATUS);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapBuilderLegacy, ProtocolSapBuilderLegacy_BuildSapTranportProtocol) {
    ProtocolSapBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSapTranportProtocol(3);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SAP_SET_TRANPORT_PROTOCOL);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_tranport_protocol_req));
    EXPECT_EQ(((sit_sap_tranport_protocol_req *)modemData->GetRawData())->protocol, 3);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}
