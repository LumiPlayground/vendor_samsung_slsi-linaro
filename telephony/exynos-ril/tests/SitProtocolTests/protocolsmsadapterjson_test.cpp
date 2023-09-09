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
#include "json/protocolsmsadapterjson.h"
#include "smsjsondef.h"
#include "cdmasmsdata.h"

using namespace std;

TEST(ProtocolSmsAdapterJson, ProtocolSendSmsRespAdapterJson) {
    struct {
        int msg_ref;
        int ack_pdu[7];
        int error_code;
        unsigned int sit_id;
    } testVector[] = {
        { 0xa, { 0x10, 0x60, 0x78, 0x4e, 0x8, 0x5b, 0x1c }, 0, SIT_SEND_SMS },
        { 0xb, { 0x10, 0x32, 0x78, 0x44, 0x8, 0x5b, 0x20 }, 0, SIT_SEND_SMS_EXPECT_MORE },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sms_send_sms_rsp::key_msg_ref] = testVector[i].msg_ref;
        jsonRspData[ns_sms_send_sms_rsp::key_error_code] = testVector[i].error_code;
        int nAckPduLen = sizeof(testVector[i].ack_pdu)/sizeof(testVector[i].ack_pdu[0]);
        for(int j = 0; j < nAckPduLen; j++) {
            jsonRspData[ns_sms_send_sms_rsp::key_ack_pdu].append(testVector[i].ack_pdu[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(testVector[i].sit_id, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolSendSmsRespAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].sit_id);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_EQ(testAdapter.GetRef(), testVector[i].msg_ref);
        EXPECT_EQ(testAdapter.GetSmsRspErrorCode(), testVector[i].error_code);
        EXPECT_EQ(testAdapter.GetPduSize(), nAckPduLen);

        const char* pAckPdu = testAdapter.GetPdu();
        for (int j = 0; j < nAckPduLen; j++) {
            EXPECT_EQ(pAckPdu[j], testVector[i].ack_pdu[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsAdapterJson, PProtocolWriteSmsToSimRespAdapterJson) {
    int testVector[] = { 0x1, 0x2, 0x5, 0x11, 0xFFFF };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sms_write_sms_to_sim_rsp::key_index] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_WRITE_SMS_TO_SIM, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolWriteSmsToSimRespAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_WRITE_SMS_TO_SIM);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetIndex(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsAdapterJson, ProtocolGetBcstSmsConfRespAdapterJson) {
    struct {
        int from_svc_id;
        int to_svc_id;
        int from_code_scheme;
        int to_code_scheme;
        int selected;
    } testVector[] = {
        { 13, 20, 3, 7, 1 },
        { 25, 31, 4, 8, 1 },
        { 1234, 2121, 25, 37, 0 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    Json::Value jsonRspData;
    Json::Value jsonBcstInfoList;
    for (int i = 0; i < size; i++) {
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_from_svc_id] = testVector[i].from_svc_id;
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_to_svc_id] = testVector[i].to_svc_id;
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_from_code_scheme] = testVector[i].from_code_scheme;
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_to_code_scheme] = testVector[i].to_code_scheme;
        jsonBcstInfoList[i][ns_sms_bcst_conf_info::key_selected] = testVector[i].selected;
    }
    jsonRspData[ns_sms_get_bcst_sms_cfg_rsp::key_bcst_conf_info_list] = jsonBcstInfoList;

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_BCST_SMS_CFG, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolGetBcstSmsConfRespAdapterJson testAdapter(modemData);
    // General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BCST_SMS_CFG);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
    // IPC response checks
    EXPECT_EQ(testAdapter.GetConfigsNumber(), size);
    RIL_GSM_BroadcastSmsConfigInfo **ppBcstConfInfoList = testAdapter.GetConfigsInfoPointers();
    for (int i = 0; i < size; i++) {
        RIL_GSM_BroadcastSmsConfigInfo *pBcstConfInfo = ppBcstConfInfoList[i];
        EXPECT_EQ(pBcstConfInfo->fromServiceId, testVector[i].from_svc_id);
        EXPECT_EQ(pBcstConfInfo->toServiceId, testVector[i].to_svc_id);
        EXPECT_EQ(pBcstConfInfo->fromCodeScheme, testVector[i].from_code_scheme);
        EXPECT_EQ(pBcstConfInfo->toCodeScheme, testVector[i].to_code_scheme);
        EXPECT_EQ(pBcstConfInfo->selected, testVector[i].selected);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSmsAdapterJson, ProtocolSmscAddrRespAdapterJson) {
    struct {
        int smsc[MAX_GSM_SMS_SERVICE_CENTER_ADDR + 1];
        int smsc_len;
    } testVector[] = {
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44 }, 8 },
        // smsc len is more than MAX_GSM_SMS_SERVICE_CENTER_ADDR
        { { 0x7, 0x91, 0x19, 0x89, 0x68, 0x0, 0x45, 0x44, 0x31, 0x86, 0x00, 0x19, 0x14 }, MAX_GSM_SMS_SERVICE_CENTER_ADDR + 1 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        for(int j = 0; j < testVector[i].smsc_len; j++) {
            jsonRspData[ns_sms_get_smsc_addr_rsp::key_service_center_addr].append(testVector[i].smsc[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_SMSC_ADDR, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolSmscAddrRespAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SMSC_ADDR);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        int smscLength = testAdapter.GetSmscLength();
        const char *smsc = testAdapter.GetSmsc();

        if (i == 0) {
            EXPECT_EQ(smscLength, testVector[i].smsc_len);
        } else {
            // when smsc len is more than MAX_GSM_SMS_SERVICE_CENTER_ADDR
            EXPECT_EQ(smscLength, MAX_GSM_SMS_SERVICE_CENTER_ADDR);
        }

        for (int j = 0; j < smscLength; j++) {
            EXPECT_EQ(smsc[j], testVector[i].smsc[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsAdapterJson, ProtocolNewSmsIndAdapterJson) {
    struct {
        int msg_tpid;
        int tpdu[29];
        unsigned int sit_id;
    } testVector[] = {
        { 1, { 0x7, 0x91, 0x19, 0x89, 0x13, 0x20, 0x49, 0x20, 0x4, 0xC, 0x91, 0x19, 0x78,
                 0x90, 0x02, 0x34, 0x85, 0x0, 0x0, 0x12, 0x90, 0x20, 0x31, 0x50, 0x62, 0x22,
                 0x2, 0xC8, 0x34 }, SIT_IND_NEW_SMS },
        { 2, { 0x7, 0x91, 0x19, 0x89, 0x13, 0x20, 0x49, 0x20, 0x4, 0xC, 0x91, 0x19, 0x78,
                 0x90, 0x02, 0x34, 0x85, 0x0, 0x0, 0x12, 0x90, 0x20, 0x31, 0x50, 0x62, 0x22,
                 0x2, 0xC8, 0x34 }, SIT_IND_NEW_SMS_STATUS_REPORT },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_sms_new_sms_ind::key_msg_tpid] = testVector[i].msg_tpid;
        int nPduLen = sizeof(testVector[i].tpdu)/sizeof(testVector[i].tpdu[0]);
        for(int j = 0; j < nPduLen; j++) {
            jsonIndData[ns_sms_new_sms_ind::key_tpdu].append(testVector[i].tpdu[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(testVector[i].sit_id, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolNewSmsIndAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].sit_id);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // Payload Check
        EXPECT_EQ(testAdapter.GetTpid(), testVector[i].msg_tpid);
        const char *expPdu = "0791198913204920040C9119789002348500001290203150622202C834";
        int expPduLen = strlen(expPdu);
        EXPECT_EQ(testAdapter.GetPduSize(), expPduLen);
        const char* pdu = testAdapter.GetPdu();
        EXPECT_EQ(strcmp(pdu, expPdu), 0);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsAdapterJson, ProtocolNewBcstSmsAdapterJson) {
    struct {
        int bcst_msg[29];
    } testVector[] = {
        { { 0x7, 0x91, 0x19, 0x89, 0x13, 0x20, 0x49, 0x20, 0x4, 0xC, 0x91, 0x19, 0x78, 0x90, 0x02, 0x34, 0x85, 0x0, 0x0, 0x12, 0x90, 0x20, 0x31, 0x50, 0x62, 0x22, 0x2, 0xC8, 0x34 } },
        { { 0x11, 0x00, 0xb, 0x81, 0x10, 0x60, 0x78, 0x4e } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        int nBcstMsgLen = sizeof(testVector[i].bcst_msg)/sizeof(testVector[i].bcst_msg[0]);
        for(int j = 0; j < nBcstMsgLen; j++) {
            jsonIndData[ns_sms_new_bcst_sms_ind::key_bcst_msg].append(testVector[i].bcst_msg[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_NEW_BCST_SMS, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolNewBcstSmsAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_NEW_BCST_SMS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // Payload Check
        EXPECT_EQ(testAdapter.GetBcstLen(), nBcstMsgLen);
        BYTE *pBcst = testAdapter.GetBcst();
        for (int j = 0; j < nBcstMsgLen; j++) {
            EXPECT_EQ(pBcst[j], testVector[i].bcst_msg[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsAdapterJson, ProtocolCdmaSendSmsRespAdapterJson) {
    struct {
        int msg_ref;
        int error_class;
        int error_code;
        unsigned int sit_id;
    } testVector[] = {
        { 0xa, 0, 0, SIT_CDMA_SEND_SMS },
        { 0xb, 0, 0, SIT_CDMA_SEND_SMS_EXPECT_MORE },
        { 0xa, 2, 3, SIT_CDMA_SEND_SMS },
        { 0xa, 3, 4, SIT_CDMA_SEND_SMS },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sms_cdma_send_sms_rsp::key_msg_ref] = testVector[i].msg_ref;
        jsonRspData[ns_sms_cdma_send_sms_rsp::key_error_class] = testVector[i].error_class;
        jsonRspData[ns_sms_cdma_send_sms_rsp::key_error_code] = testVector[i].error_code;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(testVector[i].sit_id, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolCdmaSendSmsRespAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].sit_id);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_EQ(testAdapter.GetRef(), testVector[i].msg_ref);
        EXPECT_EQ(testAdapter.GetSmsRspErrorClass(), testVector[i].error_class);
        EXPECT_EQ(testAdapter.GetSmsRspCauseCode(), testVector[i].error_code);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsAdapterJson, ProtocolGetCdmaBcstSmsConfRespAdapterJson) {
    struct {
        int service_category;
        int language;
    } testVector[] = {
        { 0x0002, 1 },
        { 0x0010, 2 },
        { 0x001E, 5 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    Json::Value jsonRspData;
    Json::Value jsonBcstInfoList;
    for (int i = 0; i < size; i++) {
        jsonBcstInfoList[i][ns_sms_cdma_bcst_conf_info::key_service_category] = testVector[i].service_category;
        jsonBcstInfoList[i][ns_sms_cdma_bcst_conf_info::key_language] = testVector[i].language;
    }
    jsonRspData[ns_sms_cdma_get_bcst_sms_cfg_rsp::key_cdma_bcst_conf_info_list] = jsonBcstInfoList;

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_CDMA_GET_BCST_SMS_CFG, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolGetCdmaBcstSmsConfRespAdapterJson testAdapter(modemData);
    // General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_CDMA_GET_BCST_SMS_CFG);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
    // IPC response checks
    EXPECT_EQ(testAdapter.GetConfigsNumber(), size);
    RIL_CDMA_BroadcastSmsConfigInfo **ppBcstConfInfoList = testAdapter.GetConfigsInfoPointers();
    for (int i = 0; i < size; i++) {
        RIL_CDMA_BroadcastSmsConfigInfo *pBcstConfInfo = ppBcstConfInfoList[i];
        EXPECT_EQ(pBcstConfInfo->service_category, testVector[i].service_category);
        EXPECT_EQ(pBcstConfInfo->language, testVector[i].language);
        EXPECT_TRUE(pBcstConfInfo->selected);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSmsAdapterJson, ProtocolCdmaWriteSmsToRuimRespAdapterJson) {
    int testVector[] = { 0x1, 0x2, 0x5, 0x11, 0xFF };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sms_cdma_write_sms_to_ruim_rsp::key_index] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_CDMA_WRITE_SMS_TO_RUIM, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolCdmaWriteSmsToRuimRespAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_CDMA_WRITE_SMS_TO_RUIM);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetIndex(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsAdapterJson, ProtocolCdmaNewSmsIndAdapterJson) {
    struct {
        int msg_tpid;
        int msg_data[29];
    } testVector[] = {
        { 1, { 0X00, 0X00, 0X02, 0X10, 0X02, 0X04, 0X07, 0X02, 0Xe8, 0x69, 0x69,
                 0x99, 0x48, 0xd8, 0x06, 0x01, 0x00, 0x08, 0x0a, 0x00, 0x03, 0x20,
                 0x00, 0x20, 0x01, 0x03, 0x10, 0x0c, 0x10 } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_sms_cdma_new_sms_ind::key_msg_tpid] = testVector[i].msg_tpid;
        int nMsgDataLen = sizeof(testVector[i].msg_data)/sizeof(testVector[i].msg_data[0]);
        for(int j = 0; j < nMsgDataLen; j++) {
            jsonIndData[ns_sms_cdma_new_sms_ind::key_msg_data].append(testVector[i].msg_data[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_CDMA_NEW_SMS, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolCdmaNewSmsIndAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_CDMA_NEW_SMS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // Payload Check
        EXPECT_EQ(testAdapter.GetTpid(), testVector[i].msg_tpid);
        EXPECT_EQ(testAdapter.GetMessageLength(), 372 /* sizeof(RIL_CDMA_SMS_Message) */);

        const RIL_CDMA_SMS_Message *pCdmaSmsMsg = testAdapter.GetRilCdmaSmsMsg();
        EXPECT_NE(pCdmaSmsMsg, nullptr);
        EXPECT_EQ(pCdmaSmsMsg->uTeleserviceID, 4098);
        EXPECT_EQ(pCdmaSmsMsg->bIsServicePresent, 0);
        EXPECT_EQ(pCdmaSmsMsg->uServicecategory, 0);
        EXPECT_EQ(pCdmaSmsMsg->sAddress.digit_mode, 0);
        EXPECT_EQ(pCdmaSmsMsg->sAddress.number_mode, 0);
        EXPECT_EQ(pCdmaSmsMsg->sAddress.number_type, 0);
        EXPECT_EQ(pCdmaSmsMsg->sAddress.number_plan, 0);
        EXPECT_EQ(pCdmaSmsMsg->sAddress.number_of_digits, 11);
        EXPECT_EQ(pCdmaSmsMsg->sSubAddress.subaddressType, 0);
        EXPECT_EQ(pCdmaSmsMsg->sSubAddress.odd, 0);
        EXPECT_EQ(pCdmaSmsMsg->sSubAddress.number_of_digits, 0);
        EXPECT_EQ(pCdmaSmsMsg->uBearerDataLen, 10);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsAdapterJson, ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson) {
    int testVector[] = { 1, 4, 7, 10, 14, 255 };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_sms_cdma_voice_msg_waiting_info_ind::key_msg_count] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_CDMA_VOICE_MSG_WAITING_INFO, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_CDMA_VOICE_MSG_WAITING_INFO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // Payload Check
        EXPECT_EQ(testAdapter.GetMessageLength(), 372 /* sizeof(RIL_CDMA_SMS_Message) */);

        const RIL_CDMA_SMS_Message *ptRcsm = testAdapter.GetRilCdmaSmsMsg();
        EXPECT_EQ(ptRcsm->uTeleserviceID, TELESERVICE_IDENTIFIER_MWI);
        EXPECT_EQ(ptRcsm->uBearerDataLen, 1);
        EXPECT_EQ(ptRcsm->aBearerData[0], testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSmsAdapterJson, ProtocolSmsCapacityOnSimRespAdapterJson) {
    struct {
        int sim_type;
        int total_num;
        int used_num;
    } testVector[] = {
        { 0x00, 100, 20 },
        { 0x01, 200, 30 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sms_get_stored_sms_count_rsp::key_sim_type] = testVector[i].sim_type;
        jsonRspData[ns_sms_get_stored_sms_count_rsp::key_total_num] = testVector[i].total_num;
        jsonRspData[ns_sms_get_stored_sms_count_rsp::key_used_num] = testVector[i].used_num;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_STORED_SMS_COUNT, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSmsCapacityOnSimRespAdapterJson testAdapter(modemData);
        // General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_STORED_SMS_COUNT);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        // Payload Check
        EXPECT_EQ(testAdapter.GetSimId(), testVector[i].sim_type);
        EXPECT_EQ(testAdapter.GetTotalNum(), testVector[i].total_num);
        EXPECT_EQ(testAdapter.GetUsedNum(), testVector[i].used_num);

        delete modemData;
        modemData = nullptr;
    }
}
