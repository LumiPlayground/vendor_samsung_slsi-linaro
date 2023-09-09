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
#include "json/protocolsmsbuilderjson.h"
#include "smsjsondef.h"
#include "modemdata.h"

using namespace std;

TEST(ProtocolSmsBuilderJson, BuildSendSms) {
    struct {
        char smsc[MAX_GSM_SMS_SERVICE_CENTER_ADDR];
        int smsc_len;
        char pdu[MAX_GSM_SMS_TPDU_SIZE];
        int pdu_size;
        bool expect_more;
        unsigned int sit_id;
        bool isModemDataNull;
        bool isSmscValid;
    } testVector[] = {
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44 }, 8, { 0x11, 0x00, 0xb, 0x81, 0x10, 0x60, 0x78, 0x4e },
            8, false, SIT_SEND_SMS, false, true },
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44 }, 8, { 0x11, 0x00, 0xb, 0x81, 0x10, 0x60, 0x78, 0x4e },
            8, true, SIT_SEND_SMS_EXPECT_MORE, false, true },
        // modemData NULL cases
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44 }, 8, { 0x00,  }, 8, false, SIT_SEND_SMS, true, true },
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44 }, 8, {  }, 0, true, SIT_SEND_SMS_EXPECT_MORE, true, true },
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44 }, 8, { 0x00,  }, MAX_GSM_SMS_TPDU_SIZE + 1, false, SIT_SEND_SMS, true, true },
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44 }, 8, { 0x02, 0x01 }, -1, false, SIT_SEND_SMS, true, true },
        // invalid SMSC cases
        { { }, 0, { 0x11, 0x00, 0xb, 0x81, 0x10, 0x60, 0x78, 0x4e }, 8, false, SIT_SEND_SMS, false, false },
        { { 0x0, }, 13, { 0x11, 0x00, 0xb, 0x81, 0x10, 0x60, 0x78, 0x4e }, 8, true, SIT_SEND_SMS_EXPECT_MORE, false, false },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSendSms(testVector[i].smsc, testVector[i].smsc_len,
                testVector[i].pdu, testVector[i].pdu_size, testVector[i].expect_more);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].sit_id);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        // System Prop depedent, not testing now
        // EXPECT_EQ(0 /* CS_PREF */, jsonReqData[ns_sms_send_sms_req::key_sms_domain].asInt());

        if (testVector[i].isSmscValid) {
            EXPECT_EQ((unsigned int) testVector[i].smsc_len, jsonReqData[ns_sms_send_sms_req::key_smsc].size());
            for (int j = 0; j < testVector[i].smsc_len; j++) {
                EXPECT_EQ(testVector[i].smsc[j], jsonReqData[ns_sms_send_sms_req::key_smsc][j].asInt());
            }
        } else {
            EXPECT_EQ(1U, jsonReqData[ns_sms_send_sms_req::key_smsc].size());
            EXPECT_EQ(0, jsonReqData[ns_sms_send_sms_req::key_smsc][0].asInt());
        }
        EXPECT_EQ((unsigned int) testVector[i].pdu_size, jsonReqData[ns_sms_send_sms_req::key_sms_data].size());
        for (int j = 0; j < testVector[i].pdu_size; j++) {
            EXPECT_EQ(testVector[i].pdu[j], jsonReqData[ns_sms_send_sms_req::key_sms_data][j].asInt());
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildSmsAck) {
    struct {
        int result;
        int tpid;
        int error;
        bool isModemDataNull;
    } testVector[] = {
        { 1, 3, 0, false },
        { 0, 4, 42, false },
        // modemData NULL cases
        { -1, 5, 0, true },
        {  0, -1, 0, true},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSmsAck(testVector[i].result, testVector[i].tpid,
                testVector[i].error);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_SMS_ACK);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].result, jsonReqData[ns_sms_send_sms_ack_req::key_result].asInt());
        EXPECT_EQ(testVector[i].tpid, jsonReqData[ns_sms_send_sms_ack_req::key_msg_tpid].asInt());
        EXPECT_EQ(testVector[i].error, jsonReqData[ns_sms_send_sms_ack_req::key_error_code].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildSmscAddress) {
    ProtocolSmsBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSmscAddress();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_SMSC_ADDR);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSmsBuilderJson, BuildWriteSmsToSim) {
    struct {
        int status;
        int sit_status_type;
        int index;
        int pduSize;
        char pdu[MAX_GSM_SMS_TPDU_SIZE+1];  // add one to check modemData null case
        bool isModemDataNull;
    } testVector[] = {
        { 0, SIT_SIM_STATUS_RECEIVED_UNREAD, 0xFFFF, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, false },
        { 1, SIT_SIM_STATUS_RECEIVED_READ, 0xFFFF, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, false },
        { 2, SIT_SIM_STATUS_STORED_UNSENT, 0xFFFF, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, false },
        { 3, SIT_SIM_STATUS_STORED_SENT, 0xFFFF, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, false },
        // modemData NULL cases
        { -1, -1, 0xFFFF, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, true },
        { 2, SIT_SIM_STATUS_STORED_UNSENT, 0xFFFF, 0, { }, true },
        { 1, SIT_SIM_STATUS_RECEIVED_READ, 0xFFFF, MAX_GSM_SMS_TPDU_SIZE + 1, { 0, }, true},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildWriteSmsToSim(testVector[i].status, testVector[i].index,
                testVector[i].pduSize, testVector[i].pdu);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_WRITE_SMS_TO_SIM);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].sit_status_type, jsonReqData[ns_sms_write_sms_to_sim_req::key_msg_status].asInt());
        EXPECT_EQ(testVector[i].index, jsonReqData[ns_sms_write_sms_to_sim_req::key_index].asInt());
        EXPECT_EQ((unsigned int) testVector[i].pduSize, jsonReqData[ns_sms_write_sms_to_sim_req::key_pdu_data].size());

        for (int j = 0; j < testVector[i].pduSize; j++) {
            EXPECT_EQ(testVector[i].pdu[j], jsonReqData[ns_sms_write_sms_to_sim_req::key_pdu_data][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildDeleteSmsOnSim) {
    int testVector[] = { 0x01, 0x02, 0x05, 0x11, 0xFFFF };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildDeleteSmsOnSim(testVector[i]);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_DELETE_SMS_ON_SIM);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i], jsonReqData[ns_sms_delete_sms_on_sim_req::key_index].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildGetBroadcastSmsConfig) {
    ProtocolSmsBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetBroadcastSmsConfig();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BCST_SMS_CFG);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSmsBuilderJson, BuildSetBroadcastSmsConfig) {
    RIL_GSM_BroadcastSmsConfigInfo bcstInfoList[3] =  { { 13, 20, 3, 7, 1 }, { 25, 31, 4, 8, 1},
        { 1234, 2121, 25, 37, 0} };
    struct {
        RIL_GSM_BroadcastSmsConfigInfo *pBcstInfoList;
        int num;
        bool isModemDataNull;
    } testVector[] = {
        { bcstInfoList, 3, false },
        // modemData NULL cases
        { NULL, 0, true },
        { { }, 0, true },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetBroadcastSmsConfig(testVector[i].pBcstInfoList, testVector[i].num);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_BCST_SMS_CFG);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        Json::Value jsonBcstInfoList;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ((unsigned int) testVector[i].num, jsonReqData[ns_sms_set_bcst_sms_cfg_req::key_bcst_conf_info_list].size());
        jsonBcstInfoList = jsonReqData[ns_sms_set_bcst_sms_cfg_req::key_bcst_conf_info_list];

        for (int j = 0; j < testVector[i].num; j++) {
            EXPECT_EQ(testVector[i].pBcstInfoList[j].fromServiceId,
                    jsonBcstInfoList[j][ns_sms_bcst_conf_info::key_from_svc_id].asInt());
            EXPECT_EQ(testVector[i].pBcstInfoList[j].toServiceId,
                    jsonBcstInfoList[j][ns_sms_bcst_conf_info::key_to_svc_id].asInt());
            EXPECT_EQ(testVector[i].pBcstInfoList[j].fromCodeScheme,
                    jsonBcstInfoList[j][ns_sms_bcst_conf_info::key_from_code_scheme].asInt());
            EXPECT_EQ(testVector[i].pBcstInfoList[j].toCodeScheme,
                    jsonBcstInfoList[j][ns_sms_bcst_conf_info::key_to_code_scheme].asInt());
            EXPECT_EQ(testVector[i].pBcstInfoList[j].selected,
                    jsonBcstInfoList[j][ns_sms_bcst_conf_info::key_selected].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildSmsBroadcastActivation) {
    int testVector[] = { 0, 1 };   // 0 : activate, 1 : deactivate

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSmsBroadcastActivation(testVector[i]);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_ACT_BCST_SMS);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i], jsonReqData[ns_sms_act_bcst_sms_req::key_bcst_act].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildSetSmscAddress) {
    struct {
        char smsc[MAX_GSM_SMS_SERVICE_CENTER_ADDR + 1];
        int smsc_len;
    } testVector[] = {
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44 }, 8 },
        // smsc len is more than MAX_GSM_SMS_SERVICE_CENTER_ADDR
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44, 0x31, 0x86, 0x00, 0x19, 0x14 }, MAX_GSM_SMS_SERVICE_CENTER_ADDR + 1 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSmscAddress(testVector[i].smsc_len, testVector[i].smsc);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_SMSC_ADDR);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        if (i == 0) {
            EXPECT_EQ((unsigned int) testVector[i].smsc_len, jsonReqData[ns_sms_set_smsc_addr_req::key_service_center_addr].size());
            for (int j = 0; j < testVector[i].smsc_len; j++) {
                EXPECT_EQ(testVector[i].smsc[j], jsonReqData[ns_sms_set_smsc_addr_req::key_service_center_addr][j].asInt());
            }
        } else {
            // SMSC len is more than MAX_GSM_SMS_SERVICE_CENTER_ADDR
            EXPECT_EQ((unsigned int) MAX_GSM_SMS_SERVICE_CENTER_ADDR, jsonReqData[ns_sms_set_smsc_addr_req::key_service_center_addr].size());
            for (int j = 0; j < MAX_GSM_SMS_SERVICE_CENTER_ADDR; j++) {
                EXPECT_EQ(testVector[i].smsc[j], jsonReqData[ns_sms_set_smsc_addr_req::key_service_center_addr][j].asInt());
            }
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildSmsMemoryStatus) {
    struct {
        int status;
        int sit_mem_status;
    } testVector[] = {
        { 0x00, MEMORY_CAPACITY_EXCEEDED },
        { 0x01, MEMORY_AVAILABLE },
    };


    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSmsMemoryStatus(testVector[i].status);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_SMS_MEM_STATUS);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i].sit_mem_status, jsonReqData[ns_sms_send_sms_mem_status_req::key_mem_status].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildSmsAck_IncomingSms) {
    struct {
        int result;
        int tpid;
        int error;
        int pdu_size;
        char pdu[MAX_GSM_SMS_TPDU_SIZE];
        bool isModemDataNull;
    } testVector[] = {
        { 1, 3, 0, 8, { 0x11, 0x00, 0xB, 0x81, 0x10, 0x60, 0x78, 0x4E }, false },
        { 0, 4, 42, 10, { 0x11, 0x00, 0xB, 0x81, 0x10, 0x60, 0x78, 0x4E, 0x12, 0x28 }, false },
        // modemData NULL cases
        { -1, 5, 0, 8, { 0x11, 0x00, 0xB, 0x81, 0x10, 0x60, 0x78, 0x4E }, true },
        {  0, -1, 0, 8, { 0x11, 0x00, 0xB, 0x81, 0x10, 0x60, 0x78, 0x4E }, true},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSmsAck(testVector[i].result, testVector[i].tpid,
                testVector[i].pdu, testVector[i].pdu_size);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SEND_ACK_INCOMING_SMS);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].result, jsonReqData[ns_sms_send_ack_incoming_sms_req::key_result].asInt());
        EXPECT_EQ(testVector[i].tpid, jsonReqData[ns_sms_send_ack_incoming_sms_req::key_msg_tpid].asInt());
        EXPECT_EQ((unsigned int) testVector[i].pdu_size, jsonReqData[ns_sms_send_ack_incoming_sms_req::key_tpdu].size());

        for (int j = 0; j < testVector[i].pdu_size; j++) {
            EXPECT_EQ(testVector[i].pdu[j], jsonReqData[ns_sms_send_ack_incoming_sms_req::key_tpdu][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildSendCdmaSms) {
    struct {
        char msg[MAX_CDMA_SMS_MSG_SIZE];
        int msg_len;
        bool expect_more;
        unsigned int sit_id;
        bool isModemDataNull;
    } testVector[] = {
        { { 0x11, 0x00, 0xb, 0x81, 0x10, 0x60, 0x78, 0x4e }, 8, false, SIT_CDMA_SEND_SMS, false },
        { { 0x11, 0x00, 0xb, 0x81, 0x10, 0x60, 0x78, 0x4e }, 8, true, SIT_CDMA_SEND_SMS_EXPECT_MORE, false },
        // modemData NULL cases
        { {  }, 0, true, SIT_CDMA_SEND_SMS_EXPECT_MORE, true },
        { { 0x00,  }, MAX_CDMA_SMS_MSG_SIZE + 1, false, SIT_CDMA_SEND_SMS, true },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSendCdmaSms(testVector[i].msg, testVector[i].msg_len, testVector[i].expect_more);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].sit_id);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ((unsigned int) testVector[i].msg_len, jsonReqData[ns_sms_cdma_send_sms_req::key_msg_data].size());
        for (int j = 0; j < testVector[i].msg_len; j++) {
            EXPECT_EQ(testVector[i].msg[j], jsonReqData[ns_sms_cdma_send_sms_req::key_msg_data][j].asInt());
        }
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildSendCdmaSmsAck) {
    struct {
        int msg_tpid;
        int error_class;
        int error_code;
    } testVector[] = {
        { 0xa, 0x00, 0x00 },
        { 0xb, 0x01, 0x04 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSendCdmaSmsAck(testVector[i].msg_tpid,
                testVector[i].error_class, testVector[i].error_code);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_CDMA_SEND_SMS_ACK);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].msg_tpid, jsonReqData[ns_sms_cdma_send_sms_ack_req::key_msg_tpid].asInt());
        EXPECT_EQ(testVector[i].error_class, jsonReqData[ns_sms_cdma_send_sms_ack_req::key_error_class].asInt());
        EXPECT_EQ(testVector[i].error_code, jsonReqData[ns_sms_cdma_send_sms_ack_req::key_error_code].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildGetCdmaBroadcastSmsConfig) {
    ProtocolSmsBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetCdmaBroadcastSmsConfig();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_CDMA_GET_BCST_SMS_CFG);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSmsBuilderJson, BuildSetCdmaBroadcastSmsConfig) {
    RIL_CDMA_BroadcastSmsConfigInfo bcstInfoList[3] =  { { 0x0002, 1, 1 }, { 0x0010, 2, 1 }, { 0x001E, 5, 1 } };
    struct {
        RIL_CDMA_BroadcastSmsConfigInfo *pBcstInfoList;
        int num;
        bool isModemDataNull;
    } testVector[] = {
        { bcstInfoList, 3, false },
        // modemData NULL cases
        { NULL, 0, true },
        { { }, 0, true },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetCdmaBroadcastSmsConfig(testVector[i].pBcstInfoList, testVector[i].num);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_CDMA_SET_BCST_SMS_CFG);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        Json::Value jsonBcstInfoList;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ((unsigned int) testVector[i].num, jsonReqData[ns_sms_cdma_set_bcst_sms_cfg_req::key_cdma_bcst_conf_info_list].size());
        jsonBcstInfoList = jsonReqData[ns_sms_cdma_set_bcst_sms_cfg_req::key_cdma_bcst_conf_info_list];

        for (int j = 0; j < testVector[i].num; j++) {
            EXPECT_EQ(testVector[i].pBcstInfoList[j].service_category,
                    jsonBcstInfoList[j][ns_sms_cdma_bcst_conf_info::key_service_category].asInt());
            EXPECT_EQ(testVector[i].pBcstInfoList[j].language,
                    jsonBcstInfoList[j][ns_sms_cdma_bcst_conf_info::key_language].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildCdmaSmsBroadcastActivation) {
    struct {
        int ril_bcst_act;
        int sit_bcst_act;
    } testVector[] = {
        { RIL_SMS_CDMA_BCST_ACT_ACTIVATE, SIT_SMS_CDMA_BCST_ACT_ACTIVATE },
        { RIL_SMS_CDMA_BCST_ACT_DEACTIVATE, SIT_SMS_CDMA_BCST_ACT_DEACTIVATE },
        // undefined act code
        { 2, SIT_SMS_CDMA_BCST_ACT_DEACTIVATE },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildCdmaSmsBroadcastActivation(testVector[i].ril_bcst_act);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_CDMA_ACT_BCST_SMS);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        EXPECT_EQ(testVector[i].sit_bcst_act, jsonReqData[ns_sms_cdma_act_bcst_sms_req::key_cdma_bcst_act].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildWriteCdmaSmsToRuim) {
    struct {
        int status;
        int sit_status_type;
        int msg_len;
        char msg[MAX_CDMA_SMS_RUIM_MSG_SIZE];
        bool isModemDataNull;
    } testVector[] = {
        { RIL_RUIM_STATUS_RECEIVED_UNREAD, SIT_RUIM_STATUS_RECEIVED_UNREAD, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, false },
        { RIL_RUIM_STATUS_RECEIVED_READ, SIT_RUIM_STATUS_RECEIVED_READ, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, false },
        { RIL_RUIM_STATUS_STORED_UNSENT, SIT_RUIM_STATUS_STORED_UNSENT, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, false },
        { RIL_RUIM_STATUS_STORED_SENT, SIT_RUIM_STATUS_STORED_SENT, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, false },
        // wrong sms status
        { 5, SIT_RUIM_STATUS_RECEIVED_UNREAD, 7, { 0x10, 0x60, 0x78, 0x8, 0x5B, 0x1C }, false },
        // modemData NULL cases
        { RIL_RUIM_STATUS_RECEIVED_UNREAD, SIT_RUIM_STATUS_RECEIVED_UNREAD, 0, { }, true },
        { RIL_RUIM_STATUS_RECEIVED_READ, SIT_RUIM_STATUS_RECEIVED_READ, MAX_CDMA_SMS_RUIM_MSG_SIZE + 1, { 0, }, true},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildWriteCdmaSmsToRuim(testVector[i].status,
                testVector[i].msg, testVector[i].msg_len);
        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_CDMA_WRITE_SMS_TO_RUIM);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].sit_status_type, jsonReqData[ns_sms_cdma_write_sms_to_ruim_req::key_msg_status].asInt());
        EXPECT_EQ((unsigned int) testVector[i].msg_len, jsonReqData[ns_sms_cdma_write_sms_to_ruim_req::key_msg_data].size());

        for (int j = 0; j < testVector[i].msg_len; j++) {
            EXPECT_EQ(testVector[i].msg[j], jsonReqData[ns_sms_cdma_write_sms_to_ruim_req::key_msg_data][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildDeleteCdmaSmsOnRuim) {
    int testVector[] = { 0x01, 0x02, 0x05, 0x11, 0xFF };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildDeleteCdmaSmsOnRuim(testVector[i]);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_CDMA_DELETE_SMS_ON_RUIM);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i], jsonReqData[ns_sms_cdma_delete_sms_on_ruim_req::key_index].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsBuilderJson, BuildGetStoredSmsCount) {
    struct {
        int sim_id;
        int sim_type;
    } testVector[] = {
        { 0x00, STORED_SIM },
        { 0x01, STORED_RUIM },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        ProtocolSmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildGetStoredSmsCount(testVector[i].sim_id);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_STORED_SMS_COUNT);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].sim_type, jsonReqData[ns_sms_get_stored_sms_count_req::key_sim_type].asInt());

        delete modemData;
        modemData = nullptr;
    }
}
