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

#include <librilutils/textutils.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "protocolsapadapter.h"
#include "legacy/protocolsapadapterlegacy.h"
#include "commondef.h"
#include "sitdef.h"

TEST(ProtocolSapAdapterLegacy, ProtocolSapConnectAdapterLegacy) {
#pragma pack(1)
    struct {
        uint8_t status;
        uint16_t max_msg_size;
    } testVector[] = {
        { 1, 10 },
        { 2, 100 },
        { 3, 1000 },
    };
#pragma pack()
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_SAP_CONNECT;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapConnectAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_connect_rsp));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetStatus(), testVector[i].status);
        EXPECT_EQ(testAdapter.GetMaxMsgSize(), testVector[i].max_msg_size);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSapAdapterLegacy, ProtocolSapDisconnectAdapterLegacy) {
    uint8_t testVector[] = {
        1, 2, 3, 4, 5,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_SAP_DISCONNECT;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapDisconnectAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_disconnect_rsp));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetStatus(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSapAdapterLegacy, ProtocolSapTransferApduAdapterLegacy) {
#pragma pack(1)
    struct {
        uint8_t result;
        uint16_t apdu_len;
        char apdu[512];
    } testVector[] = {
        { 0,  10, "1234567890" },
        { 1,  5, "12345" },
        { 1,  10, { 0, '1', '2', '3', '4' } },
    };
#pragma pack()
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_SAP_TRANSFER_APDU;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &testVector[i], 3 + testVector[i].apdu_len);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapTransferApduAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + 3 + testVector[i].apdu_len);
        EXPECT_EQ(testAdapter.GetParameterLength(), 3U + testVector[i].apdu_len);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);
        EXPECT_EQ(testAdapter.GetApduLen(), testVector[i].apdu_len);
        if (testVector[i].apdu[0] == 0) {
            EXPECT_TRUE(TextUtils::IsEmpty(testAdapter.GetApdu()));
        }
        else {
            EXPECT_TRUE(memcmp(testAdapter.GetApdu(), testVector[i].apdu, testVector[i].apdu_len) == 0);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSapAdapterLegacy, ProtocolSapTransferAtrAdapterLegacy) {
#pragma pack(1)
    struct {
        uint8_t result;
        uint8_t atr_len;
        char atr[33];
    } testVector[] = {
        { 0,  10, "1234567890" },
        { 1,  5, "12345" },
        { 1,  10, { 0, '1', '2', '3', '4' } },
    };
#pragma pack()
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_SAP_TRANSFER_ATR;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &testVector[i], 3 + testVector[i].atr_len);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapTransferAtrAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + 3 + testVector[i].atr_len);
        EXPECT_EQ(testAdapter.GetParameterLength(), 3U + testVector[i].atr_len);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);
        EXPECT_EQ(testAdapter.GetAtrLen(), testVector[i].atr_len);
        if (testVector[i].atr[0] == 0) {
            EXPECT_TRUE(TextUtils::IsEmpty(testAdapter.GetAtr()));
        }
        else {
            EXPECT_TRUE(memcmp(testAdapter.GetAtr(), testVector[i].atr, testVector[i].atr_len) == 0);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSapAdapterLegacy, ProtocolSapPowerSimOnOffAdapterLegacy) {
    uint8_t testVector[] = {
        1, 2, 3, 4, 5,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = (i % 2) ? SIT_SAP_POWER_SIM_OFF : SIT_SAP_POWER_SIM_ON;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapPowerSimOnOffAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_power_sim_onoff_rsp));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetResult(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSapAdapterLegacy, ProtocolSapResetSimAdapterLegacy) {
    uint8_t testVector[] = {
        1, 2, 3, 4, 5,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_SAP_RESET_SIM;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapResetSimAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_reset_sim_rsp));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetResult(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSapAdapterLegacy, ProtocolSapTransferCardReaderStatusAdapterLegacy) {
    uint8_t testVector[] = {
        1, 2, 3, 4, 5,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_SAP_TRANSFER_CARD_READER_STATUS;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapTransferCardReaderStatusAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_transfer_card_reader_status_rsp));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetResult(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSapAdapterLegacy, ProtocolSapSetTranportProtocolAdapterLegacy) {
    uint8_t testVector[] = {
        1, 2, 3, 4, 5,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_SAP_SET_TRANPORT_PROTOCOL;
        ModemData *modemData = ModemDataBuilder::BuildResponse(
                id, 0, RCM_E_SUCCESS,
                &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapSetTranportProtocolAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_tranport_protocol_rsp));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetResult(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSapAdapterLegacy, ProtocolSapDisconnectTypeAdapterLegacy) {
    uint8_t testVector[] = {
        1, 2, 3, 4, 5,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_IND_SAP_DISCONNECT;
        ModemData *modemData = ModemDataBuilder::BuildIndication(
                id, &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapDisconnectTypeAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_disconnect_ind));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetDisconStatus(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolSapAdapterLegacy, ProtocolSapStatusAdapterLegacy) {
    uint8_t testVector[] = {
        1, 2, 3, 4, 5,
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        unsigned int id = SIT_IND_SAP_STATUS;
        ModemData *modemData = ModemDataBuilder::BuildIndication(
                id, &testVector[i], sizeof(testVector[i]));
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolSapStatusAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_sap_status_ind));
        EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector[i]));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetCardStatus(), testVector[i]);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}
