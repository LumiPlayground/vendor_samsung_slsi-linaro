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
#include "json/protocolstkadapterjson.h"
#include "stkjsondef.h"
#include "sitdef.h"

TEST(ProtocolStkAdpaterJson, ProtocolStkEnvelopeCommandAdapterJson) {
    struct {
        int envelopeRspLen;
        int envelopeRsp[MAX_SIM_IO_DATA_LEN];
    } testVector[] = {
        { 6, { 0x02, 0x01, 0x81, 0x90, 0x01, 0x06 } },
        { 9, { 0xD3, 0x07, 0x82, 0x02, 0x01, 0x02, 0x90, 0x01, 0x01 } },
    };

    for(auto test : testVector) {
        Json::Value jsonRspData;
        for(int j = 0; j < test.envelopeRspLen; j++) {
            jsonRspData[ns_stk_send_stk_envelope_cmd_rsp::key_envelope_rsp].append(test.envelopeRsp[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SEND_STK_ENVELOPE_CMD, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolStkEnvelopeCommandAdapterJson testAdapter(modemData);
        // General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_STK_ENVELOPE_CMD);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // IPC response checks
        EXPECT_EQ(testAdapter.GetEnvelopeCmdLength(), test.envelopeRspLen);
        BYTE *pEnvelopeRsp = testAdapter.GetEnvelopeCommand();
        for(int j = 0; j < test.envelopeRspLen; j++) {
            EXPECT_EQ(pEnvelopeRsp[j], test.envelopeRsp[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolStkAdpaterJson, ProtocolStkEnvelopeStatusAdapterJson) {
    struct {
        int sw1;
        int sw2;
        int envelopeRspLen;
        int envelopeRsp[MAX_SIM_IO_DATA_LEN];
    } testVector[] = {
        { 0x91, 0x6D, 6, { 0x02, 0x01, 0x81, 0x90, 0x01, 0x06 } },
        { 0x91, 0x6D, 9, { 0xD3, 0x07, 0x82, 0x02, 0x01, 0x02, 0x90, 0x01, 0x01 } },
    };

    for(auto test : testVector) {
        Json::Value jsonRspData;
        jsonRspData[ns_stk_send_stk_envelope_with_status_rsp::key_sw1] = test.sw1;
        jsonRspData[ns_stk_send_stk_envelope_with_status_rsp::key_sw2] = test.sw2;
        for(int j = 0; j < test.envelopeRspLen; j++) {
            jsonRspData[ns_stk_send_stk_envelope_with_status_rsp::key_envelope_rsp].append(test.envelopeRsp[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SEND_STK_ENVELOPE_WITH_STATUS, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolStkEnvelopeStatusAdapterJson testAdapter(modemData);
        // General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_STK_ENVELOPE_WITH_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // IPC response checks
        EXPECT_EQ(testAdapter.GetSW1(), test.sw1);
        EXPECT_EQ(testAdapter.GetSW2(), test.sw2);
        EXPECT_EQ(testAdapter.GetEnvelopeRspLength(), test.envelopeRspLen);
        BYTE *pEnvelopeRsp = testAdapter.GetEnvelopeResponse();
        for(int j = 0; j < test.envelopeRspLen; j++) {
            EXPECT_EQ(pEnvelopeRsp[j], test.envelopeRsp[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolStkAdapterJson, ProtocolStkSimRefreshAdapterJson) {
    int testVector[] = {0, 1, 2};

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        int nResfreshResult = testVector[i];
        Json::Value jsonIndData;
        jsonIndData[ns_stk_sim_refresh_ind::key_refresh_result] = nResfreshResult;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SIM_REFRESH, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolStkSimRefreshAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SIM_REFRESH);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetResult(), nResfreshResult);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolStkAdapterJson, ProtocolStkProactiveCommandAdapterJson) {
    const int MAX_PROACTIVE_COMMAND_LEN = 256;

    struct {
        int nProactiveCmdLen;
        int proactiveCmd[MAX_PROACTIVE_COMMAND_LEN];
    } testVector[] = {
        { 35, { 0xD0, 0x21, 0x81, 0x03, 0x01, 0x01, 0x06, 0x82, 0x02, 0x81, 0x82, 0x92, 0x13, 0x03, 0x3F, 0x00,
                  0x7F, 0xFF, 0x6F, 0x07, 0x3F, 0x00, 0x7F, 0xFF, 0x6F, 0x73, 0x3F, 0x00, 0x7F, 0xFF, 0x6F, 0x7E } },
        { 41, { 0xD0, 0x27, 0x81, 0x03, 0x01, 0x01, 0x06, 0x82, 0x02, 0x81, 0x82, 0x2F, 0x07, 0xA0, 0x00, 0x00,
                  0x00, 0x87, 0x10, 0x04, 0x83, 0x01, 0x00, 0x84, 0x02, 0x00, 0x01, 0x85, 0x0C, 0x61, 0x69, 0x72,
                  0x74, 0x65, 0x6c, 0x20, 0x6c, 0x69, 0x76, 0x65, 0x21 } },
        { 50, { 0xd0, 0x30, 0x81, 0x03, 0x01, 0x10, 0x00, 0x82, 0x02, 0x81, 0x83, 0x85, 0x1f, 0x80, 0x00, 0x43, 0x00, 0x61,
                  0x00, 0x6c, 0x00, 0x6c, 0x00, 0x69, 0x00, 0x6e, 0x00, 0x67, 0x00, 0x2e, 0x00, 0x2e, 0x00, 0x20, 0x00, 0x35,
                  0x00, 0x39, 0x00, 0x35, 0x00, 0x30, 0x00, 0x30, 0x86, 0x04, 0x81, 0x95, 0x05, 0xf0 } },
        { 39, { 0xd0, 0x30, 0x81, 0x03, 0x01, 0x10, 0x00, 0x82, 0x02, 0x81, 0x83, 0x8D, 0x1A, 0x00,
                  0xD5, 0xE9, 0x94, 0x08, 0x9A, 0xD3, 0xE5, 0x69, 0xF7, 0x19, 0x24, 0x2F, 0x8F,
                  0xCB, 0x69, 0x7B, 0x99, 0x0C, 0x32, 0xCB, 0xDF, 0x6D, 0xD0, 0x74, 0x0A } },
        { 42, { 0xd0, 0x30, 0x81, 0x03, 0x01, 0x21, 0x07, 0x82, 0x02, 0x81, 0x83, 0x8D, 0x1D, 0x04,
                  0x55, 0x53, 0x53, 0x44, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x20, 0x72,
                  0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x64, 0x20, 0x66, 0x72, 0x6F, 0x6D, 0x20, 0x53, 0x53 } },
        { 70, { 0xd0, 0x30, 0x81, 0x03, 0x01, 0x21, 0x80, 0x82, 0x02, 0x81, 0x83, 0x8D, 0x39, 0x08, 0x00,
                  0x55, 0x00, 0x53, 0x00, 0x53, 0x00, 0x44, 0x00, 0x20, 0x00,
                  0x73, 0x00, 0x74, 0x00, 0x72, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x20,
                  0x00, 0x72, 0x00, 0x65, 0x00, 0x63, 0x00, 0x65, 0x00, 0x69, 0x00, 0x76, 0x00,
                  0x65, 0x00, 0x64, 0x00, 0x20, 0x00, 0x66, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x6D,
                  0x00, 0x20, 0x00, 0x53, 0x00, 0x53 } },
};

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        for(int j = 0; j < testVector[i].nProactiveCmdLen; j++) {
            jsonIndData[ns_stk_proactive_cmd_ind::key_proactive_cmd].append(testVector[i].proactiveCmd[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_STK_PROACTIVE_COMMAND, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolStkProactiveCommandAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_STK_PROACTIVE_COMMAND);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        EXPECT_EQ(testAdapter.GetProactiveCmdLength(), testVector[i].nProactiveCmdLen);
        BYTE *pProactiveCmd = testAdapter.GetProactiveCommand();
        for(int j = 0; j < testVector[i].nProactiveCmdLen; j++) {
            EXPECT_EQ(pProactiveCmd[j], testVector[i].proactiveCmd[j]);
        }
        if(i == 0) {
            // proactiveCmd has EF file data
            EXPECT_EQ(testAdapter.GetEfidCount(), 3);
            unsigned int efData[] = {0x6F07, 0x6F73, 0x6F7E};
            for(int j = 0; j < 3; j++) {
                EXPECT_EQ(testAdapter.GetEFID(j), efData[j]);
            }
            // no AID
            EXPECT_EQ(testAdapter.GetAidLength(), 0);
        } else if (i == 1){
            // proactiveCmd has AID data
            EXPECT_EQ(testAdapter.GetAidLength(), 7);
            int aid[] = { 0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x04 };
            BYTE* pAid = testAdapter.GetAID();
            for(int j = 0; j < 7; j++) {
                EXPECT_EQ(pAid[j], aid[j]);
            }
            // no EF file data
            EXPECT_EQ(testAdapter.GetEfidCount(), 0);
        } else {
            // added this for coverage only
            EXPECT_EQ(testAdapter.GetAidLength(), 0);
            EXPECT_EQ(testAdapter.GetEfidCount(), 0);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolStkAdapterJson, ProtocolSsReturnResultAdapterJson) {
    struct {
        int return_result_len;
        int return_result[255];
    } testVector[] = {
        { 20, { 0xe, 0xa3, 0x11, 0x30, 0xf, 0x83, 0x1, 0x10, 0x84, 0x1, 0x6, 0x85,
                  0x7, 0x91, 0x19, 0x87, 0x92, 0x66, 0x6, 0x9a } },
    };

    for (auto test : testVector) {
        Json::Value jsonIndData;
        for(int j = 0; j < test.return_result_len; j++) {
            jsonIndData[ns_stk_ss_return_result_ind::key_ret_result_list].append(test.return_result[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SS_RETURN_RESULT, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolSsReturnResultAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SS_RETURN_RESULT);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // Payload Check
        EXPECT_EQ(testAdapter.GetReturnResultLength(), test.return_result_len);
        BYTE *returnResult = testAdapter.GetReturnResult();
        for (int j = 0; j < test.return_result_len; j++) {
            EXPECT_EQ(test.return_result[j], returnResult[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}
