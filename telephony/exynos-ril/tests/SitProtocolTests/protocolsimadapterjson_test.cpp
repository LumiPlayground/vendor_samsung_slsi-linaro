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
#include "json/protocolsimadapterjson.h"
#include "simjsondef.h"
#include "rildef.h"

using namespace std;

TEST(ProtocolSimAdapterJson, ProtocolSimGetCarrierRestrictionsAdapterJson) {
    const int carrierInfoSize = 2;
    CarrierInfo allowedInfos[carrierInfoSize] = {{"123", "456", RIL_MATCH_ALL, "test"}, {"123", "45", RIL_MATCH_SPN, "test"}};
    CarrierInfo excludedInfos[carrierInfoSize] = {{"345", "678", RIL_MATCH_ALL, "test"}, {"345", "67", RIL_MATCH_SPN, "test"}};

    struct {
        int allowedLength;
        int excludedLength;
        CarrierInfo *pAllowed;
        CarrierInfo *pExcluded;
        bool allowedPrioritized;
        int multiSimPolicy;
    } testVector[] = {
        { 0, 0, allowedInfos, excludedInfos, false, 0, },
        { carrierInfoSize, carrierInfoSize, allowedInfos, excludedInfos, true,  2,},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_CARRIER_RESTRICTIONS;

    for (int i = 0; i < size; i++) {
        Json::Value jsonData;

        jsonData[ns_sim_net_getset_carrier_restrictions::key_multi_simlock] = testVector[i].multiSimPolicy;
        jsonData[ns_sim_net_getset_carrier_restrictions::key_allowed_priority] = testVector[i].allowedPrioritized;

        for (int j = 0; j < testVector[i].allowedLength; ++j) {
            Json::Value jsonCarrierInfo;

            string plmn(testVector[i].pAllowed[j].mcc);
            plmn = plmn + testVector[i].pAllowed[j].mnc;
            jsonCarrierInfo[ns_sim_net_carrier_info::key_plmn] = plmn;
            jsonCarrierInfo[ns_sim_net_carrier_info::key_match_type] = testVector[i].pAllowed[j].match_type;
            int matchDataSize = strlen(testVector[i].pAllowed[j].match_data);
            for (int k = 0; k < matchDataSize; ++k) {
                jsonCarrierInfo[ns_sim_net_carrier_info::key_match_data_list].append(testVector[i].pAllowed[j].match_data[k]);
            }

            jsonData[ns_sim_net_getset_carrier_restrictions::key_allowed_list].append(jsonCarrierInfo);
        }

        for (int j = 0; j < testVector[i].excludedLength; ++j) {
            Json::Value jsonCarrierInfo;

            string plmn(testVector[i].pExcluded[j].mcc);
            plmn = plmn + testVector[i].pExcluded[j].mnc;
            jsonCarrierInfo[ns_sim_net_carrier_info::key_plmn] = plmn;
            jsonCarrierInfo[ns_sim_net_carrier_info::key_match_type] = testVector[i].pExcluded[j].match_type;
            int matchDataSize = strlen(testVector[i].pExcluded[j].match_data);
            for (int k = 0; k < matchDataSize; ++k) {
                jsonCarrierInfo[ns_sim_net_carrier_info::key_match_data_list].append(testVector[i].pExcluded[j].match_data[k]);
            }

            jsonData[ns_sim_net_getset_carrier_restrictions::key_excluded_list].append(jsonCarrierInfo);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimGetCarrierRestrictionsAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetAllowedCarrierLength(), testVector[i].allowedLength);
        EXPECT_EQ(testAdapter.GetExcludedCarrierLength(), testVector[i].excludedLength);
        EXPECT_EQ(testAdapter.GetAllowedCarriersPrioritized(), testVector[i].allowedPrioritized);
        EXPECT_EQ(testAdapter.GetMultiSimPolicy(), testVector[i].multiSimPolicy);

        int allowedLen = testAdapter.GetAllowedCarrierLength();
        int excludeLen = testAdapter.GetExcludedCarrierLength();

        if (allowedLen > 0) {
            RIL_Carrier *pAllowed = new RIL_Carrier[allowedLen];
            testAdapter.GetAllowedCarriers(pAllowed, allowedLen);
            for (int j = 0; j < allowedLen; ++j) {
                EXPECT_STREQ(pAllowed[j].mcc, testVector[i].pAllowed[j].mcc);
                EXPECT_STREQ(pAllowed[j].mnc, testVector[i].pAllowed[j].mnc);
                EXPECT_STREQ(pAllowed[j].match_data, testVector[i].pAllowed[j].match_data);
                EXPECT_EQ(pAllowed[j].match_type, testVector[i].pAllowed[j].match_type);
            }
            delete [] pAllowed;
        }

        if (excludeLen > 0) {
            RIL_Carrier *pExcluded = new RIL_Carrier[excludeLen];
            testAdapter.GetExcludedCarriers(pExcluded, excludeLen);
            for (int j = 0; j < excludeLen; ++j) {
                EXPECT_STREQ(pExcluded[j].mcc, testVector[i].pExcluded[j].mcc);
                EXPECT_STREQ(pExcluded[j].mnc, testVector[i].pExcluded[j].mnc);
                EXPECT_STREQ(pExcluded[j].match_data, testVector[i].pExcluded[j].match_data);
                EXPECT_EQ(pExcluded[j].match_type, testVector[i].pExcluded[j].match_type);
            }
            delete [] pExcluded;
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimStatusAdapterJson) {
    struct {
        int nCardState;
        int nUnivPinState;
        int nAppType;
        int nAppState;
        int nPersoSubState;
        int aid[MAX_SIM_AID_LEN];
        const char *expAID;
        int nPin1Replaced;
        int nPin1State;
        int nPin2State;
        int nPin1RemainCnt;
        int nPuk1RemainCnt;
        int nPin2RemainCnt;
        int nPuk2RemainCnt;
        int eSimNoProfile;
        int slotId;
        int atr[MAX_ATR_LEN];
        int iccid[MAX_ICCID_LEN];
        int eid[MAX_EID_LEN];
    } testVector[] = {
        { 1, 1, 5, 2, 2, { 0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x02, 0xFF, 0x86, 0xF0, 0x00, 0x89, 0xFF, 0xFF, 0xFF, 0xFF },
            "A0000000871002FF86F00089FFFFFFFF", 0, 1, 1, 3, 10, 3, 10, 0, 0, { 0x3B, 0x9D, 0x95, 0x80, 0x1F, 0xC7,
                0x80, 0x31, 0xE0, 0x73, 0xFE, 0x21, 0x13, 0x65, 0xD0, 0x02, 0x0C, 0x11, 0xF4, 0xE0 },
            { 0x89, 0x91, 0x44, 0x24, 0x59, 0x00, 0x17, 0x50, 0x10, 0x4F }, { 0x80, 0x31, 0xE0, 0x73, 0xFE, 0x21, 0x1B, 0x65, 0xD0, 0x01, 0x90, 0x0F, 0xC1, 0x81, 0x0F, 0x19 } },
        { 1, 3, 2, 5, 2, { 0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x02, 0xFF, 0xFF, 0xF0, 0x01, 0x89, 0x00, 0x00,
                             0x01, 0xFF }, "A0000000871002FFFFF00189000001FF", 0, 3, 1, 3, 10, 3, 10,
        1, 1, { 0x3B, 0x9E, 0x95, 0x80, 0x1F, 0xC7, 0x80, 0x31, 0xE0, 0x73, 0xFE, 0x21, 0x1B, 0x66,
            0xD0, 0x01, 0xA1, 0xA5, 0x10, 0x00, 0x16 },
        { 0x89, 0x91, 0x45, 0x09, 0x00, 0x92, 0x07, 0x41, 0x36, 0x78 }, {  } },
    };

    for(auto test : testVector) {
        Json::Value jsonRspData;
        jsonRspData[ns_sim_get_sim_status_rsp::key_card_state] = test.nCardState;
        jsonRspData[ns_sim_get_sim_status_rsp::key_univ_pin_state] = test.nUnivPinState;

        Json::Value jsonRspAppData;
        jsonRspAppData[ns_sim_apps_status_info::key_app_type] = test.nAppType;
        jsonRspAppData[ns_sim_apps_status_info::key_app_state] = test.nAppState;
        jsonRspAppData[ns_sim_apps_status_info::key_perso_substate] = test.nPersoSubState;
        int aidLen = sizeof(test.aid)/sizeof(test.aid[0]);
        for(int j = 0; j < aidLen; j++) {
            jsonRspAppData[ns_sim_apps_status_info::key_aid].append(test.aid[j]);
        }
        jsonRspAppData[ns_sim_apps_status_info::key_pin1_replaced] = test.nPin1Replaced;
        jsonRspAppData[ns_sim_apps_status_info::key_pin1_state] = test.nPin1State;
        jsonRspAppData[ns_sim_apps_status_info::key_pin2_state] = test.nPin2State;
        jsonRspAppData[ns_sim_apps_status_info::key_pin1_remain_cnt] = test.nPin1RemainCnt;
        jsonRspAppData[ns_sim_apps_status_info::key_puk1_remain_cnt] = test.nPuk1RemainCnt;
        jsonRspAppData[ns_sim_apps_status_info::key_pin2_remain_cnt] = test.nPin2RemainCnt;
        jsonRspAppData[ns_sim_apps_status_info::key_puk2_remain_cnt] = test.nPuk2RemainCnt;
        jsonRspAppData[ns_sim_apps_status_info::key_app_label] = Json::arrayValue;

        jsonRspData[ns_sim_get_sim_status_rsp::key_app_status_info_list].append(jsonRspAppData);
        jsonRspData[ns_sim_get_sim_status_rsp::key_esim_no_profile] = test.eSimNoProfile;
        jsonRspData[ns_sim_get_sim_status_rsp::key_phy_slotid] = test.slotId;

        int nAtrLen = sizeof(test.atr)/sizeof(test.atr[0]);
        for(int j = 0; j < nAtrLen; j++) {
            jsonRspData[ns_sim_get_sim_status_rsp::key_atr].append(test.atr[j]);
        }

        int nIccidLen = sizeof(test.iccid)/sizeof(test.iccid[0]);
        for(int j = 0; j < nIccidLen; j++) {
            jsonRspData[ns_sim_get_sim_status_rsp::key_iccid].append(test.iccid[j]);
        }

        int nEidLen = sizeof(test.eid)/sizeof(test.eid[0]);
        for(int j = 0; j < nEidLen; j++) {
            jsonRspData[ns_sim_get_sim_status_rsp::key_eid].append(test.eid[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_SIM_STATUS, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimStatusAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIM_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        //IPC response checks
        EXPECT_EQ(testAdapter.GetCardState(), test.nCardState);
        EXPECT_EQ(testAdapter.GetUniversalPinState(), test.nUnivPinState);
        EXPECT_EQ(testAdapter.GetApplicationCount(), 1);
        EXPECT_EQ(testAdapter.GetAppsType(0), test.nAppType);
        EXPECT_EQ(testAdapter.GetAppsState(0), test.nAppState);
        EXPECT_EQ(testAdapter.GetPersonalSubstate(0), test.nPersoSubState);
        EXPECT_EQ(testAdapter.GetPin1Replaced(0), test.nPin1Replaced);
        EXPECT_EQ(testAdapter.GetPinState(0, 1), test.nPin1State);
        EXPECT_EQ(testAdapter.GetPinState(0, 2), test.nPin2State);
        EXPECT_EQ(testAdapter.GetPinRemainCount(0, 1), test.nPin1RemainCnt);
        EXPECT_EQ(testAdapter.GetPinRemainCount(0, 2), test.nPin2RemainCnt);
        EXPECT_EQ(testAdapter.GetPukRemainCount(0, 1), test.nPuk1RemainCnt);
        EXPECT_EQ(testAdapter.GetPukRemainCount(0, 2), test.nPuk2RemainCnt);

        auto parsedAID = std::unique_ptr<char>(testAdapter.GetAID(0));
        EXPECT_EQ(strcmp(parsedAID.get(), test.expAID), 0);

        EXPECT_EQ(testAdapter.GetAtrLength(), nAtrLen);
        char *parsedAtr = testAdapter.GetAtr();
        for(int j = 0; j < nAtrLen; j++) {
            EXPECT_EQ(test.atr[j], parsedAtr[j]);
        }

        EXPECT_EQ(testAdapter.GetIccidLength(), nIccidLen);
        char *parsedIccid = testAdapter.GetIccid();
        for(int j = 0; j < nIccidLen; j++) {
            EXPECT_EQ(test.iccid[j], parsedIccid[j]);
        }

        EXPECT_EQ(testAdapter.GetEidLength(), nEidLen);
        char *parsedEid = testAdapter.GetEid();
        for(int j = 0; j < nEidLen; j++) {
            EXPECT_EQ(test.eid[j], parsedEid[j]);
        }

        EXPECT_EQ(testAdapter.GetEsimNoProfile(), test.eSimNoProfile);
        EXPECT_EQ(testAdapter.GetPhysicalSlotId(), test.slotId);
        // no data sent by CP for appLabel, calling this function for coverage
        EXPECT_EQ(testAdapter.GetApplicationLabel(0, NULL), -1);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimVerifyPinAdapterJson) {
    struct {
        unsigned int sitId;
        int pinIndex;
        int remainCount;
    } testVector[] = {
        { SIT_VERIFY_SIM_PIN, 1, 3  },
        { SIT_CHG_SIM_PIN, 1, 3 },
        { SIT_VERIFY_SIM_PIN2, 2, 3 },
        { SIT_CHG_SIM_PIN2, 2, 1 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        switch(testVector[i].sitId)
        {
        case SIT_VERIFY_SIM_PIN:
            jsonRspData[ns_sim_verify_sim_pin_rsp::key_remain_cnt] = testVector[i].remainCount;
            break;
        case SIT_CHG_SIM_PIN:
            jsonRspData[ns_sim_change_sim_pin_rsp::key_remain_cnt] = testVector[i].remainCount;
            break;
        case SIT_VERIFY_SIM_PIN2:
            jsonRspData[ns_sim_verify_sim_pin2_rsp::key_remain_cnt] = testVector[i].remainCount;
            break;
        case SIT_CHG_SIM_PIN2:
            jsonRspData[ns_sim_change_sim_pin2_rsp::key_remain_cnt] = testVector[i].remainCount;
            break;
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(testVector[i].sitId, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimVerifyPinAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].sitId);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetPinIndex(), testVector[i].pinIndex);
        EXPECT_EQ(testAdapter.GetRemainCount(), testVector[i].remainCount);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimVerifyPukAdapterJson) {
    struct {
        unsigned int sitId;
        int pukIndex;
        int remainCount;
    } testVector[] = {
        { SIT_VERIFY_SIM_PUK, 1, 3  },
        { SIT_VERIFY_SIM_PUK2, 2, 2 },
        { SIT_VERIFY_SIM_PUK, 1, 10 },
        { SIT_VERIFY_SIM_PUK2, 2, 10  },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        switch(testVector[i].sitId)
        {
            case SIT_VERIFY_SIM_PUK:
                jsonRspData[ns_sim_verify_sim_puk_rsp::key_remain_cnt] = testVector[i].remainCount;
                break;
            case SIT_VERIFY_SIM_PUK2:
                jsonRspData[ns_sim_verify_sim_puk2_rsp::key_remain_cnt] = testVector[i].remainCount;
                break;
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(testVector[i].sitId, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimVerifyPukAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].sitId);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetPukIndex(), testVector[i].pukIndex);
        EXPECT_EQ(testAdapter.GetRemainCount(), testVector[i].remainCount);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimVerifyNetLockAdapterJson) {
    int testVector[] = { 0, 1, 2, 3, 10 };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        int remainCount = testVector[i];
        Json::Value jsonRspData;
        jsonRspData[ns_sim_verify_network_lock_rsp::key_remain_cnt] = remainCount;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_VERIFY_NETWORK_LOCK, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimVerifyNetLockAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_VERIFY_NETWORK_LOCK);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetRemainCount(), remainCount);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimIOAdapterJson) {
    int sw1 = 144;
    int sw2 = 16;
    int response[] = {98, 23, 130, 2, 65, 33, 131, 2, 111, 5, 138, 1, 5, 139, 3, 111, 6, 8, 128, 2, 0, 8, 136, 1, 16};

    Json::Value jsonRspData;
    jsonRspData[ns_sim_sim_io_rsp::key_sw1] = sw1;
    jsonRspData[ns_sim_sim_io_rsp::key_sw2] = sw2;
    int responseLength = sizeof(response)/sizeof(response[0]);
    for(int i = 0; i < responseLength; i++) {
        jsonRspData[ns_sim_sim_io_rsp::key_rsp].append(response[i]);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SIM_IO, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSimIOAdapterJson testAdapter(modemData);
    // General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_SIM_IO);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    // IPC response checks
    EXPECT_EQ(testAdapter.GetSw1(), sw1);
    EXPECT_EQ(testAdapter.GetSw2(), sw2);
    EXPECT_EQ(testAdapter.GetResponseLength(), responseLength);
    BYTE *pResponse = testAdapter.GetResponse();
    for(int i = 0; i < responseLength; i++) {
        EXPECT_EQ(pResponse[i], response[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapterJson, ProtocolSimGetFacilityLockAdapterJson) {
    int testVector[] = { 0, 1 };
    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sim_get_facility_lock_rsp::key_fac_lock_status] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_FACILITY_LOCK, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimGetFacilityLockAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_FACILITY_LOCK);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetServiceClass(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimSetFacilityLockAdapterJson) {
    int testVector[] = { 0, 1, 2, 3 };
    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        int remainCount = testVector[i];;
        Json::Value jsonRspData;
        jsonRspData[ns_sim_verify_network_lock_rsp::key_remain_cnt] = remainCount;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SET_FACILITY_LOCK, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimSetFacilityLockAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_FACILITY_LOCK);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetRemainCount(), remainCount);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimOpenChannelAdapterJson) {
    unsigned int sid = UINT_MAX;
    int sw1 = 144;
    int sw2 = 16;
    int response[] = {98, 23, 130, 2, 65, 33, 131, 2, 111, 5, 138, 1, 5, 139, 3, 111, 6, 8, 128, 2, 0, 8, 136, 1, 16};

    Json::Value jsonRspData;
    jsonRspData[ns_sim_open_sim_channel_rsp::key_sid] = sid;
    jsonRspData[ns_sim_open_sim_channel_rsp::key_sw1] = sw1;
    jsonRspData[ns_sim_open_sim_channel_rsp::key_sw2] = sw2;
    int responseLength = sizeof(response)/sizeof(response[0]);
    for(int i = 0; i < responseLength; i++) {
        jsonRspData[ns_sim_open_sim_channel_rsp::key_rsp].append(response[i]);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OPEN_SIM_CHANNEL, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSimOpenChannelAdapterJson testAdapter(modemData);
    // General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_OPEN_SIM_CHANNEL);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    // IPC response checks
    EXPECT_EQ(testAdapter.GetSessionID(), (int)sid);
    EXPECT_EQ(testAdapter.GetSw1(), sw1);
    EXPECT_EQ(testAdapter.GetSw2(), sw2);
    EXPECT_EQ(testAdapter.GetResponseLength(), responseLength);
    BYTE *pResponse = testAdapter.GetResponse();
    for(int i = 0; i < responseLength; i++) {
        EXPECT_EQ(pResponse[i], response[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapterJson, ProtocolSimOpenChannelWithP2AdapterJson) {
    unsigned int sid = UINT_MAX;
    int sw1 = 144;
    int sw2 = 16;
    int response[] = {98, 23, 130, 2, 65, 33, 131, 2, 111, 5, 138, 1, 5, 139, 3, 111, 6, 8, 128, 2, 0, 8, 136, 1, 16};

    Json::Value jsonRspData;
    jsonRspData[ns_sim_open_sim_channel_with_p2_rsp::key_sid] = sid;
    jsonRspData[ns_sim_open_sim_channel_with_p2_rsp::key_sw1] = sw1;
    jsonRspData[ns_sim_open_sim_channel_with_p2_rsp::key_sw2] = sw2;
    int responseLength = sizeof(response)/sizeof(response[0]);
    for(int i = 0; i < responseLength; i++) {
        jsonRspData[ns_sim_open_sim_channel_with_p2_rsp::key_rsp].append(response[i]);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OPEN_SIM_CHANNEL_WITH_P2, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSimOpenChannelAdapterJson testAdapter(modemData);
    // General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_OPEN_SIM_CHANNEL_WITH_P2);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    // IPC response checks
    EXPECT_EQ(testAdapter.GetSessionID(), (int)sid);
    EXPECT_EQ(testAdapter.GetSw1(), sw1);
    EXPECT_EQ(testAdapter.GetSw2(), sw2);
    EXPECT_EQ(testAdapter.GetResponseLength(), responseLength);
    BYTE *pResponse = testAdapter.GetResponse();
    for(int i = 0; i < responseLength; i++) {
        EXPECT_EQ(pResponse[i], response[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapterJson, ProtocolSimATRAdapterJson) {
    struct {
        int result;
        int atrLen;
        int atr[MAX_ATR_LEN];
    } testVector[] = {
        { 1, 21, { 0x3B, 0x9E, 0x95, 0x80, 0x1F, 0xC7, 0x80, 0x31, 0xE0, 0x73, 0xFE, 0x21, 0x1B, 0x66,
                     0xD0, 0x01, 0xA1, 0xA5, 0x10, 0x00, 0x16 } },
        { 0, 0, { } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sim_id_get_atr_rsp::key_result] = testVector[i].result;
        jsonRspData[ns_sim_id_get_atr_rsp::key_atr] = Json::arrayValue;
        for(int j = 0; j < testVector[i].atrLen; j++) {
            jsonRspData[ns_sim_id_get_atr_rsp::key_atr].append(testVector[i].atr[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_ATR, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimATRAdapterJson testAdapter(modemData);
        // General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_ATR);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        // IPC response checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);
        EXPECT_EQ(testAdapter.GetATRLength(), testVector[i].atrLen);
        const char *pAtr = testAdapter.GetATR();
        for(int j = 0; j < testVector[i].atrLen; j++) {
            EXPECT_EQ(pAtr[j], testVector[i].atr[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimReadPbEntryJson) {
    int nPbType = PB_FDN;
    int nRecordId = 1;
    int data[] = {1, 11, 0, 2, 73, 110, 102, 111, 32, 67, 101, 110, 116, 114, 2, 4, 0, 129, 49, 49, 49, 255};

    Json::Value jsonRspData;
    jsonRspData[ns_read_pb_resp::key_pb_type] = nPbType;
    jsonRspData[ns_read_pb_resp::key_record_id] = nRecordId;
    int dataLen = sizeof(data)/sizeof(data[0]);
    for(int i = 0; i < dataLen; i++) {
        jsonRspData[ns_read_pb_resp::key_entry_data].append(data[i]);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_READ_PB_ENTRY, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSimReadPbEntryJson testAdapter(modemData);
    // General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_READ_PB_ENTRY);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

    // IPC response checks
    EXPECT_EQ(testAdapter.GetPbType(), nPbType);
    EXPECT_EQ(testAdapter.GetRecordId(), nRecordId);
    EXPECT_EQ(testAdapter.GetDataLength(), dataLen);
    char *pData = testAdapter.GetEntryData();
    for(int i = 0; i < dataLen; i++) {
        EXPECT_EQ(pData[i], data[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapterJson, ProtocolSimUpdatePbEntryJson) {
    struct {
        int nMode;
        int nPbType;
        int nRecordId;
    } testVector[] = {
        { 2 /* DELETE */, PB_FDN, 200},
        { 1 /* ADD */, PB_AAS, 128},
        { 3 /* EDIT */, PB_ADN_3G, 10},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_update_pb_entry_resp::key_mode] = testVector[i].nMode;
        jsonRspData[ns_update_pb_entry_resp::key_pb_type] = testVector[i].nPbType;
        jsonRspData[ns_update_pb_entry_resp::key_record_id] = testVector[i].nRecordId;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_UPDATE_PB_ENTRY, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimUpdatePbEntryJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_UPDATE_PB_ENTRY);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //IPC response checks
        EXPECT_EQ(testAdapter.GetMode(), testVector[i].nMode);
        EXPECT_EQ(testAdapter.GetPbtype(), testVector[i].nPbType);
        EXPECT_EQ(testAdapter.GetRecordId(), testVector[i].nRecordId);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimPbStorageInfoAdapterJson) {
    struct {
        int nPbType;
        int nTotalCount;
        int nUsedCount;
    } testVector[] = {
        { PB_FDN, 12, 0},
        { PB_AAS, 20, 4},
        { PB_ADN_3G, 10, 2},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sim_pb_storage_info_rsp::key_pb_type] = testVector[i].nPbType;
        jsonRspData[ns_sim_pb_storage_info_rsp::key_total_cnt] = testVector[i].nTotalCount;
        jsonRspData[ns_sim_pb_storage_info_rsp::key_used_cnt] = testVector[i].nUsedCount;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_PB_STORAGE_INFO, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimPbStorageInfoAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_PB_STORAGE_INFO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //IPC response checks
        EXPECT_EQ(testAdapter.GetPbType(), testVector[i].nPbType);
        EXPECT_EQ(testAdapter.GetTotalCount(), testVector[i].nTotalCount);
        EXPECT_EQ(testAdapter.GetUsedCount(), testVector[i].nUsedCount);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimPbCapaAdapterJson) {
    struct {
        int nPbType;
        int nMaxIndex;
        int nMaxEntry;
        int nUsedCnt;
    } testVector[] = {
        { PB_FDN, 160, 2, 17  },
        { PB_AAS, 83, 0, 10 }
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    Json::Value jsonRspData;
    Json::Value jsonPbList;
    for(int i = 0; i < size; i++) {
        jsonPbList[i][ns_pb_cap_list::key_pb_type] = testVector[i].nPbType;
        jsonPbList[i][ns_pb_cap_list::key_max_index] = testVector[i].nMaxIndex;
        jsonPbList[i][ns_pb_cap_list::key_max_entry] = testVector[i].nMaxEntry;
        jsonPbList[i][ns_pb_cap_list::key_used_cnt] = testVector[i].nUsedCnt;
    }
    jsonRspData[ns_sim_pb_capa_rsp::key_pb_cap_list] = jsonPbList;

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_3G_PB_CAPA, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSimPbCapaAdapterJson testAdapter(modemData);
    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_3G_PB_CAPA);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
    //IPC response checks
    EXPECT_EQ(testAdapter.GetEntryNum(), size);
    int pb[8] = {0};
    testAdapter.GetPbCapa(pb, size);
    int j = 0;
    for(int i = 0; i < size; i++) {
        EXPECT_EQ(pb[j], testVector[i].nPbType);
        EXPECT_EQ(pb[j+1], testVector[i].nMaxIndex);
        EXPECT_EQ(pb[j+2], testVector[i].nMaxEntry);
        EXPECT_EQ(pb[j+3], testVector[i].nUsedCnt);
        j += 4;
    }
    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapterJson, ProtocolSimPbReadyAdapterJson) {
    int testVector[] = { 0, 1, 255 };
    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_sim_pb_ready_ind::key_pb_status] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SIM_PB_READY, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimPbReadyAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SIM_PB_READY);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetPbReady(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolUiccSubStatusChangeAdapterJson) {
    int testVector[] = {0, 1};  // 0: deactivated, 1: activated

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        int nState = testVector[i];
        Json::Value jsonIndData;
        jsonIndData[ns_sim_uicc_sub_state_changed_ind::key_state] = nState;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_UICC_SUBSCRIPTION_STATE_CHANGED, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolUiccSubStatusChangeAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_UICC_SUBSCRIPTION_STATE_CHANGED);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetState(), nState);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimTrayStatusChangeAdapterJson) {
    int testVector[] = {0, 1, 2};  // 0:unknown, 1: inserted, 2: removed

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        int nStatus = testVector[i];
        Json::Value jsonIndData;
        jsonIndData[ns_sim_tray_status_changed_ind::key_tray_status] = nStatus;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SIM_TRAY_STATUS_CHANGED, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimTrayStatusChangeAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SIM_TRAY_STATUS_CHANGED);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetState(), nStatus);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimTransmitApduBasicAdapterJson) {
    int apdu[] = {98, 23, 130, 2, 65, 33, 131, 2, 111, 5, 138, 1, 5, 139, 3, 111, 6, 8, 128, 2, 0, 8, 136, 1, 16, 144, 0};

    Json::Value jsonRspData;
    int apduLength = sizeof(apdu)/sizeof(apdu[0]);
    for(int i = 0; i < apduLength; i++) {
        jsonRspData[ns_sim_transmit_sim_apdu_basic_rsp::key_rsp].append(apdu[i]);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_TRANSMIT_SIM_APDU_BASIC, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSimTransmitApduBasicAdapterJson testAdapter(modemData);
    // General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_TRANSMIT_SIM_APDU_BASIC);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    // IPC response checks
    EXPECT_EQ(testAdapter.GetSw1(), apdu[apduLength-2]);
    EXPECT_EQ(testAdapter.GetSw2(), apdu[apduLength-1]);
    EXPECT_EQ(testAdapter.GetApduLength(), apduLength);
    BYTE *pApdu = testAdapter.GetApdu();
    for(int i = 0; i < apduLength; i++) {
        EXPECT_EQ(pApdu[i], apdu[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapterJson, ProtocolSimTransmitApduChannelAdapterJson) {
    int sw1 = 144;
    int sw2 = 0;
    int apdu[] = {98, 23, 130, 2, 65, 33, 131, 2, 111, 5, 138, 1, 5, 139, 3, 111, 6, 8, 128, 2, 0, 8, 136, 1, 16, 144, 0};

    Json::Value jsonRspData;
    jsonRspData[ns_sim_transmit_sim_apdu_channel_rsp::key_sw1] = sw1;
    jsonRspData[ns_sim_transmit_sim_apdu_channel_rsp::key_sw2] = sw2;
    int apduLength = sizeof(apdu)/sizeof(apdu[0]);
    for(int i = 0; i < apduLength; i++) {
        jsonRspData[ns_sim_transmit_sim_apdu_channel_rsp::key_rsp].append(apdu[i]);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_TRANSMIT_SIM_APDU_CHANNEL, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSimTransmitApduChannelAdapterJson testAdapter(modemData);
    // General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_TRANSMIT_SIM_APDU_CHANNEL);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    // IPC response checks
    EXPECT_EQ(testAdapter.GetSw1(), sw1);
    EXPECT_EQ(testAdapter.GetSw2(), sw2);
    EXPECT_EQ(testAdapter.GetApduLength(), apduLength);
    BYTE *pApdu = testAdapter.GetApdu();
    for(int i = 0; i < apduLength; i++) {
        EXPECT_EQ(pApdu[i], apdu[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapterJson, ProtocolSimGetGbaAuthAdapterJson) {
    int nResult = 0;
    int nAuthType = 1;
    int nGbaType = 0;
    int nGbaTag = 2;
    int data1[] = {64, 75, 26, 93, 242, 47, 133, 214, 238, 0, 245, 177, 239, 173, 139, 144};

    Json::Value jsonRspData;
    jsonRspData[ns_sim_get_sim_gba_auth_rsp::key_result] = nResult;
    jsonRspData[ns_sim_get_sim_gba_auth_rsp::key_auth_type] = nAuthType;
    jsonRspData[ns_sim_get_sim_gba_auth_rsp::key_gba_tag] = nGbaTag;
    jsonRspData[ns_sim_get_sim_gba_auth_rsp::key_gba_type] = nGbaType;
    int data1Length = sizeof(data1)/sizeof(data1[0]);
    for(int i = 0; i < data1Length; i++) {
        jsonRspData[ns_sim_get_sim_gba_auth_rsp::key_data1].append(data1[i]);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_GBA_CONTEXT, 0, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    ProtocolSimGetGbaAuthAdapterJson testAdapter(modemData);
    // General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_GBA_CONTEXT);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    // IPC response checks
    // result + authType + gbaTag + gbaType = 4
    EXPECT_EQ(4+data1Length, testAdapter.GetGbaAuthLength());
    BYTE *pData1 = testAdapter.GetGbaAuth();
    EXPECT_EQ(pData1[0], nResult);
    EXPECT_EQ(pData1[1], nAuthType);
    EXPECT_EQ(pData1[2], nGbaType);
    EXPECT_EQ(pData1[3], nGbaTag);
    for(int i = 0; i < data1Length; i++) {
        EXPECT_EQ(pData1[i+4], data1[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapterJson, ProtocolSimGetSimAuthAdapterJson) {
    struct {
        int nAuthType;
        int nAuthResult;
        int resultAuts[4];
        int ck[16];
        int ik[16];
        int kc[16];
        int sw1;
        int sw2;
    } testVector[] = {
        // IMS auth
        { SIT_SIM_AUTH_IMS, 0, {85,84,87,86}, {84,87,86,81,80,83,82,93,92,95,94,89,88,91,90,85},
            {87,86,81,80,83,82,93,92,95,94,89,88,91,90,85,84}, {95,94,89,88,91,90,85,84,87,86,81,80,83,82,93,92},
            144, 0},
        // 3G auth
        { SIT_SIM_AUTH_3G, 0, {85,84,87,86}, {84,87,86,81,80,83,82,93,92,95,94,89,88,91,90,85},
            {87,86,81,80,83,82,93,92,95,94,89,88,91,90,85,84}, {95,94,89,88,91,90,85,84,87,86,81,80,83,82,93,92},
            144, 0},
        // GSM auth
        // ck and ik data are not used in GSM auth
        { SIT_SIM_AUTH_GSM, 0, {85,84,87,86}, {[0 ... 15] = 0}, {[0 ... 15] = 0},
            {95,94,89,88,91,90,85,84,87,86,81,80,83,82,93,92}, 144, 0},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sim_get_sim_auth_rsp::key_auth_type] = testVector[i].nAuthType;
        jsonRspData[ns_sim_get_sim_auth_rsp::key_auth_result] = testVector[i].nAuthResult;
        int nResAutsLen = sizeof(testVector[i].resultAuts)/sizeof(testVector[i].resultAuts[0]);
        for(int j = 0; j < nResAutsLen; j++) {
            jsonRspData[ns_sim_get_sim_auth_rsp::key_res_auts].append(testVector[i].resultAuts[j]);
        }
        int nCkLen = 0;  // will be 0 for GSM auth
        int nIkLen = 0;  // will be 0 for GSM auth
        if(testVector[i].nAuthType == SIT_SIM_AUTH_IMS || testVector[i].nAuthType == SIT_SIM_AUTH_3G) {
            nCkLen = sizeof(testVector[i].ck)/sizeof(testVector[i].ck[0]);
            for(int j = 0; j < nCkLen; j++) {
                jsonRspData[ns_sim_get_sim_auth_rsp::key_ck].append(testVector[i].ck[j]);
            }
            nIkLen = sizeof(testVector[i].ik)/sizeof(testVector[i].ik[0]);
            for(int j = 0; j < nIkLen; j++) {
                jsonRspData[ns_sim_get_sim_auth_rsp::key_ik].append(testVector[i].ik[j]);
            }
        }
        int nKcLen = sizeof(testVector[i].kc)/sizeof(testVector[i].kc[0]);
        for(int j = 0; j < nKcLen; j++) {
            jsonRspData[ns_sim_get_sim_auth_rsp::key_kc].append(testVector[i].kc[j]);
        }
        jsonRspData[ns_sim_get_sim_auth_rsp::key_sw1] = testVector[i].sw1;
        jsonRspData[ns_sim_get_sim_auth_rsp::key_sw2] = testVector[i].sw2;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_SIM_AUTH, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSimGetSimAuthAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIM_AUTH);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        //IPC response checks
        EXPECT_EQ(testVector[i].nAuthType, testAdapter.GetAuthType());
        // add 1 for authResult
        int expAuthLen = 1 + 1 + nResAutsLen + 1 + nKcLen;
        if(testVector[i].nAuthType == SIT_SIM_AUTH_IMS || testVector[i].nAuthType == SIT_SIM_AUTH_3G) {
            expAuthLen += (1 + nCkLen + 1 + nKcLen);
        }
        EXPECT_EQ(expAuthLen, testAdapter.GetAuthLength());
        BYTE *pAuthData = testAdapter.GetAuth();
        int nIndex = 0;
        EXPECT_EQ(pAuthData[nIndex++], testVector[i].nAuthResult);
        EXPECT_EQ(pAuthData[nIndex++], nResAutsLen);
        for(int j = 0; j < nResAutsLen; j++) {
            EXPECT_EQ(pAuthData[nIndex++], testVector[i].resultAuts[j]);
        }
        if(testVector[i].nAuthType == SIT_SIM_AUTH_IMS || testVector[i].nAuthType == SIT_SIM_AUTH_3G) {
            EXPECT_EQ(pAuthData[nIndex++], nCkLen);
            for(int j = 0; j < nCkLen; j++) {
                EXPECT_EQ(pAuthData[nIndex++], testVector[i].ck[j]);
            }
            EXPECT_EQ(pAuthData[nIndex++], nIkLen);
            for(int j = 0; j < nIkLen; j++) {
                EXPECT_EQ(pAuthData[nIndex++], testVector[i].ik[j]);
            }
        }
        EXPECT_EQ(pAuthData[nIndex++], nKcLen);
        for(int j = 0; j < nKcLen; j++) {
            EXPECT_EQ(pAuthData[nIndex++], testVector[i].kc[j]);
        }
        EXPECT_EQ(pAuthData[nIndex++], testVector[i].sw1);
        EXPECT_EQ(pAuthData[nIndex], testVector[i].sw2);
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapterJson, ProtocolSimImsiAdapterJson) {
    struct {
        const char* imsi;
    } testVector[] = {
        { "404860002344419" },
        { "404860002323000" },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_sim_id_get_imsi_rsp::key_imsi] = string(testVector[i].imsi);

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
                SIT_GET_IMSI, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolSimImsiAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_IMSI);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_STREQ(testAdapter.GetImsi(), testVector[i].imsi);

        delete modemData;
        modemData = nullptr;
    }
}
