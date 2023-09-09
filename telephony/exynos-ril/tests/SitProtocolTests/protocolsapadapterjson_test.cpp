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
#include "json/protocolsapadapterjson.h"
#include "simjsondef.h"
#include "rildef.h"
#include "sitdef.h"
#include <librilutils/textutils.h>

TEST(ProtocolSapAdapterJson, ProtocolSapConnectAdapterJson) {
    struct {
        int nStatus;
        int nMaxMsgSize;
    } testVector[] = {
        { 0, 512 },
        { 1, 10  },
        { 2, 100 },
        { 3, 1000 },
        { 0, 65535 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sap_connect_rsp::key_status] = testVector[i].nStatus;
        jsonRspData[ns_sap_connect_rsp::key_max_msg_size] = testVector[i].nMaxMsgSize;


        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SAP_CONNECT, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapConnectAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SAP_CONNECT);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetStatus(), testVector[i].nStatus);
        EXPECT_EQ(testAdapter.GetMaxMsgSize(), testVector[i].nMaxMsgSize);
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapAdapterJson, ProtocolSapDisconnectAdapterJson) {
    int testVector[] = { 0, 1 };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sap_disconnect_rsp::key_status] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SAP_DISCONNECT, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapDisconnectAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SAP_DISCONNECT);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetStatus(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapAdapterJson, ProtocolSapTransferApduAdapterJson) {
    struct {
        int result;
        int apdu[10] = {0};
    } testVector[] = {
        { 0,  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 } },
        { 1,  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 } },
        { 1,  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sap_transfer_apdu_rsp::key_result] = testVector[i].result;
        int apdu_len = sizeof(testVector[i].apdu)/sizeof(testVector[i].apdu[0]);
        for(int j = 0; j < apdu_len; j++) {
            jsonRspData[ns_sap_transfer_apdu_rsp::key_rsp_apdu].append(testVector[i].apdu[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SAP_TRANSFER_APDU, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapTransferApduAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SAP_TRANSFER_APDU);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //IPC response checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);
        EXPECT_EQ(testAdapter.GetApduLen(), apdu_len);
        if (testVector[i].apdu[0] == 0) {
            EXPECT_TRUE(TextUtils::IsEmpty(testAdapter.GetApdu()));
        } else {
            const char *pApdu = testAdapter.GetApdu();
            for(int j = 0; j < apdu_len; j++) {
                EXPECT_EQ(pApdu[j], testVector[i].apdu[j]);
            }
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapAdapterJson, ProtocolSapTransferAtrAdapterJson) {
    struct {
        int result;
        int atr[10];
    } testVector[] = {
        { 0,  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 } },
        { 1,  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 } },
        { 1,  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sap_transfer_atr_rsp::key_result] = testVector[i].result;
        int atr_len = sizeof(testVector[i].atr)/sizeof(testVector[i].atr[0]);
        for(int j = 0; j < atr_len; j++) {
            jsonRspData[ns_sap_transfer_atr_rsp::key_atr].append(testVector[i].atr[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SAP_TRANSFER_ATR, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapTransferAtrAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SAP_TRANSFER_ATR);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //IPC response checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);
        EXPECT_EQ(testAdapter.GetAtrLen(), atr_len);
        if (testVector[i].atr[0] == 0) {
            EXPECT_TRUE(TextUtils::IsEmpty(testAdapter.GetAtr()));
        } else {
            const char *pAtr = testAdapter.GetAtr();
            for(int j = 0; j < atr_len; j++) {
                EXPECT_EQ(pAtr[j], testVector[i].atr[j]);
            }
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapAdapterJson, ProtocolSapPowerSimOnOffAdapterJson) {
    int testVector[] = {
        1, 2, 3, 4, 5,
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        unsigned int sitId = (i % 2) ? SIT_SAP_POWER_SIM_OFF : SIT_SAP_POWER_SIM_ON;
        Json::Value jsonRspData;
        jsonRspData[ns_sap_power_sim_onoff_rsp::key_result] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapPowerSimOnOffAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapAdapterJson, ProtocolSapResetSimAdapterJson) {
    int testVector[] = {
        1, 2, 3, 4, 5,
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sap_reset_sim_rsp::key_result] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SAP_RESET_SIM, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapResetSimAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SAP_RESET_SIM);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapAdapterJson, ProtocolSapTransferCardReaderStatusAdapterJson) {
    int testVector[] = {
        1, 2, 3, 4, 5,
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sap_transfer_card_reader_status_rsp::key_result] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SAP_TRANSFER_CARD_READER_STATUS, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapTransferCardReaderStatusAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SAP_TRANSFER_CARD_READER_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapAdapterJson, ProtocolSapSetTranportProtocolAdapterJson) {
    int testVector[] = {
        1, 2, 3, 4, 5,
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sap_tranport_protocol_rsp::key_result] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SAP_SET_TRANPORT_PROTOCOL, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapSetTranportProtocolAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SAP_SET_TRANPORT_PROTOCOL);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapAdapterJson, ProtocolSapDisconnectTypeAdapterJson) {
    int testVector[] = { 0, 1 };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_sap_disconnect_ind::key_disconnect_type] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SAP_DISCONNECT, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapDisconnectTypeAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SAP_DISCONNECT);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetDisconStatus(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSapAdapterJson, ProtocolSapStatusAdapterJson) {
    int testVector[] = {
        1, 2, 3, 4, 5,
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_sap_status_ind::key_card_status] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SAP_STATUS, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSapStatusAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SAP_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetCardStatus(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}
