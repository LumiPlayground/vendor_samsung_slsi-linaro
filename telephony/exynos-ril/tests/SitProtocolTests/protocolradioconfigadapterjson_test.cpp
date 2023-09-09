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
#include "json/protocolradioconfigadapterjson.h"
#include "simjsondef.h"
#include "sitdef.h"
#include "rildef.h"

using namespace std;

TEST(ProtocolPsAdapterJson, ProtocolPhoneCapabilityAdapterJson) {
    unsigned int sitId = SIT_GET_PHONE_CAPABILITY;

    Json::Value jsonData;
    jsonData[ns_sim_ps_get_phone_capability_rsp::key_max_simultaneous_data_stack] = 1;
    jsonData[ns_sim_ps_get_phone_capability_rsp::key_max_simultaneous_internet_pdn] = 0;
    jsonData[ns_sim_ps_get_phone_capability_rsp::key_internet_lingering_support] = 1;
    jsonData[ns_sim_ps_get_phone_capability_rsp::key_max_supported_stack] = 2;

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
    ASSERT_NE(modemData, nullptr);

    ProtocolPhoneCapabilityAdapterJson testAdapter(modemData);

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    EXPECT_EQ(testAdapter.GetMaxActiveData(), 1);
    EXPECT_EQ(testAdapter.GetMaxActiveInternetData(), 0);
    EXPECT_EQ(testAdapter.IsInternetLingeringSupported(), 1);
    ASSERT_EQ(testAdapter.GetLogicalModemListSize(), 2);

    int *logicalModemList = testAdapter.GetLogicalModemList();
    for(int i = 0; i < testAdapter.GetLogicalModemListSize(); i++){
        EXPECT_EQ(logicalModemList[i], i);
    }

    delete modemData;
    modemData = nullptr;
}

// SIM needs to check
#if 0
TEST(ProtocolRadioConfigAdapterJson, ProtocolSimSlotStatusAdapterJson) {
    struct {
        int nCardState;
        int nSlotState;
        int nAtrLen;
        int atr[21];
        int nLogicalSlotId;
        int nIccIdLen;
        int iccid[10];
        int nEidLen;
        int eid[16];
    } testVector[] = {
        { 1, 1, 21, {59, 158, 149, 128, 31, 199, 128, 49, 224, 115, 254, 33, 27, 102, 208, 1, 161, 115, 16, 0, 192}, 0,
         10, {152, 25, 104, 0, 0, 22, 67, 68, 145, 244}, 16, {137, 4, 144, 50, 0, 0, 1, 0, 0, 0, 1, 114, 128, 137, 0, 89} },
        { 1, 1, 21, {59,159,150,192,10,63,199,160,128,49,224,115,254,33,27,101,208,1,144,15,193}, 1,
         10, {152,0,0,0,0,0,0,0,0,243}, 0, {0, } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    Json::Value jsonRspData;
    for(int i = 0; i < size; i++) {
        // fill each slot's data
        Json::Value jsonSlotStatusData;
        jsonSlotStatusData[ns_sim_slot_status_info::key_card_state] = testVector[i].nCardState;
        jsonSlotStatusData[ns_sim_slot_status_info::key_slot_state] = testVector[i].nSlotState;
        for(int j = 0; j < testVector[i].nAtrLen; j++) {
            jsonSlotStatusData[ns_sim_slot_status_info::key_atr].append(testVector[i].atr[j]);
        }
        jsonSlotStatusData[ns_sim_slot_status_info::key_log_slot_id] = testVector[i].nLogicalSlotId;
        for(int j = 0; j < testVector[i].nIccIdLen; j++) {
            jsonSlotStatusData[ns_sim_slot_status_info::key_iccid].append(testVector[i].iccid[j]);
        }
        if(testVector[i].nEidLen > 0) {
            for(int j = 0; j < testVector[i].nEidLen; j++) {
                jsonSlotStatusData[ns_sim_slot_status_info::key_eid].append(testVector[i].eid[j]);
            }
        } else {
            jsonSlotStatusData[ns_sim_slot_status_info::key_eid] = Json::arrayValue;
        }
        jsonRspData[ns_sim_get_slot_status_resp::key_slot_status_info_list].append(jsonSlotStatusData);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_SLOT_STATUS, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSimSlotStatusAdapterJson testAdapter(modemData);
    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_SLOT_STATUS);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    //IPC response checks for each slot
    EXPECT_EQ(testAdapter.GetNumOfSlotStatus(), size);
    for(int i = 0; i < size; i++) {
        EXPECT_EQ(testAdapter.GetCardState(i), testVector[i].nCardState);
        EXPECT_EQ(testAdapter.GetSlotState(i), testVector[i].nSlotState);
        EXPECT_EQ(testAdapter.GetLogicalSlotId(i), testVector[i].nLogicalSlotId);
        EXPECT_EQ(testAdapter.GetAtrSize(i), testVector[i].nAtrLen*2+1);
        EXPECT_EQ(testAdapter.GetIccIdSize(i), testVector[i].nIccIdLen*2+1);
        if(i == 0) {
            const char *expAtr = "3B9E95801FC78031E073FE211B66D001A1731000C0";
            char *pAtr = testAdapter.GetAtr(i);
            EXPECT_EQ(strcmp(expAtr, pAtr),  0);
            if (pAtr) delete[] pAtr;
            string expIccid = "8991860000613444194F";
            string iccid = testAdapter.GetIccId(i);
            EXPECT_EQ(expIccid.compare(iccid),  0);
            EXPECT_EQ(testAdapter.GetEidSize(i), testVector[i].nEidLen*2+1);
            const char *expEid = "89049032000001000000017280890059";
            char *pEid = testAdapter.GetEid(i);
            EXPECT_EQ(strcmp(expEid, pEid),  0);
            if (pEid) delete[] pEid;
        } else {
            const char *expAtr = "3B9F96C00A3FC7A08031E073FE211B65D001900FC1";
            char *pAtr = testAdapter.GetAtr(i);
            EXPECT_EQ(strcmp(expAtr, pAtr),  0);
            if (pAtr) delete[] pAtr;
            string expIccid = "8900000000000000003F";
            string iccid = testAdapter.GetIccId(i);
            EXPECT_EQ(expIccid.compare(iccid),  0);
            // empty EID
            EXPECT_EQ(testAdapter.GetEidSize(i), testVector[i].nEidLen);
            const char *pEid = testAdapter.GetEid(i);
            EXPECT_EQ(pEid, nullptr);
            if (pEid) delete[] pEid;
        }
    }
    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolRadioConfigAdapterJson, ProtocolSlotStatusChangedAdapterJson) {
    struct {
        int nCardState;
        int nSlotState;
        int nAtrLen;
        int atr[21];
        int nLogicalSlotId;
        int nIccIdLen;
        int iccid[10];
        int nEidLen;
        int eid[16];
    } testVector[] = {
        { 1, 1, 21, {59, 158, 149, 128, 31, 199, 128, 49, 224, 115, 254, 33, 27, 102, 208, 1, 161, 115, 16, 0, 192}, 0,
            10, {152, 25, 104, 0, 0, 22, 67, 68, 145, 244}, 16, {137, 4, 144, 50, 0, 0, 1, 0, 0, 0, 1, 114, 128, 137, 0, 89} },
        { 1, 1, 21, {59,159,150,192,10,63,199,160,128,49,224,115,254,33,27,101,208,1,144,15,193}, 1,
            10, {152,0,0,0,0,0,0,0,0,243}, 0, {0, } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    Json::Value jsonIndData;
    for(int i = 0; i < size; i++) {
        // fill each slot's data
        Json::Value jsonSlotStatusData;
        jsonSlotStatusData[ns_sim_slot_status_info::key_card_state] = testVector[i].nCardState;
        jsonSlotStatusData[ns_sim_slot_status_info::key_slot_state] = testVector[i].nSlotState;
        for(int j = 0; j < testVector[i].nAtrLen; j++) {
            jsonSlotStatusData[ns_sim_slot_status_info::key_atr].append(testVector[i].atr[j]);
        }
        jsonSlotStatusData[ns_sim_slot_status_info::key_log_slot_id] = testVector[i].nLogicalSlotId;
        for(int j = 0; j < testVector[i].nIccIdLen; j++) {
            jsonSlotStatusData[ns_sim_slot_status_info::key_iccid].append(testVector[i].iccid[j]);
        }
        if(testVector[i].nEidLen > 0) {
            for(int j = 0; j < testVector[i].nEidLen; j++) {
                jsonSlotStatusData[ns_sim_slot_status_info::key_eid].append(testVector[i].eid[j]);
            }
        } else {
            jsonSlotStatusData[ns_sim_slot_status_info::key_eid] = Json::arrayValue;
        }
        jsonIndData[ns_sim_slot_status_changed_ind::key_slot_status_info_list].append(jsonSlotStatusData);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SIM_SLOT_STATUS_CHANGED, jsonIndData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSlotStatusChangedAdapterJson testAdapter(modemData);
    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_SIM_SLOT_STATUS_CHANGED);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    //IPC response checks for each slot
    EXPECT_EQ(testAdapter.GetNumOfSlotStatus(), size);
    for(int i = 0; i < size; i++) {
        EXPECT_EQ(testAdapter.GetCardState(i), testVector[i].nCardState);
        EXPECT_EQ(testAdapter.GetSlotState(i), testVector[i].nSlotState);
        EXPECT_EQ(testAdapter.GetLogicalSlotId(i), testVector[i].nLogicalSlotId);
        EXPECT_EQ(testAdapter.GetAtrSize(i), testVector[i].nAtrLen*2+1);
        EXPECT_EQ(testAdapter.GetIccIdSize(i), testVector[i].nIccIdLen*2+1);
        if(i == 0) {
            const char *expAtr = "3B9E95801FC78031E073FE211B66D001A1731000C0";
            char *pAtr = testAdapter.GetAtr(i);
            EXPECT_EQ(strcmp(expAtr, pAtr),  0);
            if (pAtr) delete[] pAtr;
            string expIccid = "8991860000613444194F";
            string iccid = testAdapter.GetIccId(i);
            EXPECT_EQ(expIccid.compare(iccid),  0);
            EXPECT_EQ(testAdapter.GetEidSize(i), testVector[i].nEidLen*2+1);
            const char *expEid = "89049032000001000000017280890059";
            char *pEid = testAdapter.GetEid(i);
            EXPECT_EQ(strcmp(expEid, pEid),  0);
            if (pEid) delete[] pEid;
        } else {
            const char *expAtr = "3B9F96C00A3FC7A08031E073FE211B65D001900FC1";
            char *pAtr = testAdapter.GetAtr(i);
            EXPECT_EQ(strcmp(expAtr, pAtr),  0);
            if (pAtr) delete[] pAtr;
            string expIccid = "8900000000000000003F";
            string iccid = testAdapter.GetIccId(i);
            EXPECT_EQ(expIccid.compare(iccid),  0);
            // empty EID
            EXPECT_EQ(testAdapter.GetEidSize(i), testVector[i].nEidLen);
            const char *pEid = testAdapter.GetEid(i);
            EXPECT_EQ(pEid, nullptr);
            if (pEid) delete[] pEid;
        }
    }
    delete modemData;
    modemData = nullptr;
}
#endif
