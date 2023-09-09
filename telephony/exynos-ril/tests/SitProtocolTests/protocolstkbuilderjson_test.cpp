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
#include "json/protocolstkbuilderjson.h"
#include "stkjsondef.h"
#include "modemdata.h"
#include <slsi/ril_oem.h>

TEST(ProtocolStkBuilderJson, BuildStkEnvelopeCommand) {
    struct {
        int nEnvelopeCmdLen;
        BYTE envelopeCmd[9];
        bool isModemDataNull;
    } testVector[] = {
        { 9, { 0xD3, 0x07, 0x82, 0x02, 0x01, 0x81, 0x90, 0x01, 0x03 }, false },
        { 9, { 0xD3, 0x07, 0x82, 0x02, 0x01, 0x81, 0x90, 0x01, 0x04 }, false },
        // modemData NULL case
        { 0, { }, true },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolStkBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildStkEnvelopeCommand(testVector[i].nEnvelopeCmdLen, testVector[i].envelopeCmd);

        if(testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_STK_ENVELOPE_CMD);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        int nParsedEnvelopeCmdLen = jsonReqData[ns_stk_send_stk_envelope_cmd_req::key_envelope_cmd].size();
        EXPECT_EQ(nParsedEnvelopeCmdLen, testVector[i].nEnvelopeCmdLen);

        for(int j = 0; j < nParsedEnvelopeCmdLen; j++) {
            EXPECT_EQ(testVector[i].envelopeCmd[j], jsonReqData[ns_stk_send_stk_envelope_cmd_req::key_envelope_cmd][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolStkBuilderJson, BuildStkTerminalResponse) {
    struct {
        int nTerminalRspLen;
        BYTE terminalRsp[12];
        bool isModemDataNull;
    } testVector[] = {
        { 12, { 0x81, 0x03, 0x01, 0x41, 0x00, 0x02, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00 }, false },
        { 12, { 0x81, 0x03, 0x01, 0x25, 0x00, 0x02, 0x02, 0x82, 0x81, 0x83, 0x01, 0x00 }, false },
        // modemData NULL case
        { 0, { }, true },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolStkBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildStkTerminalResponse(testVector[i].nTerminalRspLen, testVector[i].terminalRsp);

        if(testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_STK_TERMINAL_RSP);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        int nParsedTerminalRspLen = jsonReqData[ns_stk_send_stk_terminal_rsp_req::key_terminal_rsp].size();
        EXPECT_EQ(nParsedTerminalRspLen, testVector[i].nTerminalRspLen);

        for(int j = 0; j < nParsedTerminalRspLen; j++) {
            EXPECT_EQ(testVector[i].terminalRsp[j], jsonReqData[ns_stk_send_stk_terminal_rsp_req::key_terminal_rsp][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolStkBuilderJson, BuildStkEnvelopeStatus) {
    struct {
        int nEnvelopeStatusLen;
        BYTE envelopeStatus[12];
        bool isModemDataNull;
    } testVector[] = {
        { 9, { 0xD3, 0x07, 0x82, 0x02, 0x01, 0x81, 0x90, 0x01, 0x03 }, false },
        // modemData NULL case
        { 0, { }, true },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolStkBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildStkEnvelopeStatus(testVector[i].nEnvelopeStatusLen,
                testVector[i].envelopeStatus);

        if(testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_STK_ENVELOPE_WITH_STATUS);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        int nParsedEnvelopeStatusLen = jsonReqData[ns_stk_send_stk_envelope_with_status_req::key_envelope_cmd].size();
        EXPECT_EQ(nParsedEnvelopeStatusLen, testVector[i].nEnvelopeStatusLen);

        for(int j = 0; j < nParsedEnvelopeStatusLen; j++) {
            EXPECT_EQ(testVector[i].envelopeStatus[j], jsonReqData[ns_stk_send_stk_envelope_with_status_req::key_envelope_cmd][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolStkBuilderJson, BuildStkCallSetup) {
    int testVector[] = {0, 1};

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {

        ProtocolStkBuilderJson testBuilder;
        int nUserOp = testVector[i];
        ModemData *modemData = testBuilder.BuildStkCallSetup(nUserOp);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_STK_CALL_SETUP);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        int nParsedUserOp = jsonReqData[ns_stk_call_setup_req::key_user_operation].asInt();
        EXPECT_EQ(nParsedUserOp, nUserOp);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolStkBuilderJson, BuildOemSatWifiRequest) {
    ProtocolStkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildOemSatWifiRequest(RIL_REQUEST_OEM_WIFI_STATUS_RESPONSE,
            NULL, 0);
    EXPECT_EQ(modemData, nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}
