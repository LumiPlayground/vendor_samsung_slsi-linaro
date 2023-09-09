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
#include "modemdata.h"
#include "json/protocolembmsbuilderjson.h"
#include "embmsjsondef.h"
#include "tokengen.h"

TEST(ProtocolEmbmsBuilderJson, BuildSetService) {
    TokenGen::Init();

    int testVector[] = { 0x00, 0x01 };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolEmbmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetService(testVector[i]);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_EMBMS_SERVICE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i], jsonReqData[ns_embms_set_service_req::key_state].asInt());
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolEmbmsBuilderJson, BuildSetSession) {
    TokenGen::Init();

    struct {
        int state;
        uint64_t tmgi;
        int saiListLen;
        uint32_t saiList[MAX_SAI_LIST_LEN];
        int freqListLen;
        uint32_t freqList[MAX_FREQ_LIST_LEN];
    } testVector[] = {
        { 1, 1234567891234, 2, {1122, 1278}, 1, {12423784} },
        { 2, 1234567891234, 2, {1122, 1278}, 2, {12423784, UINT_MAX} },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolEmbmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetSession(testVector[i].state, testVector[i].tmgi, testVector[i].saiListLen, testVector[i].saiList, testVector[i].freqListLen, testVector[i].freqList);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_EMBMS_SESSION);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].state, jsonReqData[ns_embms_set_session_req::key_state].asInt());
        BYTE szTmgi[6] = {1, 31, 113, 251, 9, 34};
        // key_deact_tmgi is conditional, sent only when state is 2
        if (testVector[i].state == 2) {
            EXPECT_EQ(6U, jsonReqData[ns_embms_set_session_req::key_deact_tmgi].size());
            for (int j = 0; j < 6; j++) {
                EXPECT_EQ(szTmgi[j], jsonReqData[ns_embms_set_session_req::key_deact_tmgi][j].asInt());
            }
        } else {
            EXPECT_EQ(0U, jsonReqData[ns_embms_set_session_req::key_deact_tmgi].size());
        }

        EXPECT_EQ(6U, jsonReqData[ns_embms_set_session_req::key_tmgi].size());
        for (int j = 0; j < 6; j++) {
            EXPECT_EQ(szTmgi[j], jsonReqData[ns_embms_set_session_req::key_tmgi][j].asInt());
        }

        EXPECT_EQ((unsigned int) testVector[i].saiListLen, jsonReqData[ns_embms_set_session_req::key_sai_list].size());

        for (int j = 0; j < testVector[i].saiListLen; j++) {
            EXPECT_EQ(testVector[i].saiList[j], jsonReqData[ns_embms_set_session_req::key_sai_list][j].asUInt());
        }

        EXPECT_EQ((unsigned int) testVector[i].freqListLen, jsonReqData[ns_embms_set_session_req::key_freq_list].size());

        for (int j = 0; j < testVector[i].freqListLen; j++) {
            EXPECT_EQ(testVector[i].freqList[j], jsonReqData[ns_embms_set_session_req::key_freq_list][j].asUInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolEmbmsBuilderJson, BuildGetSessionList) {
    TokenGen::Init();

    int testVector[] = { 0, 1 };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolEmbmsBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildGetSessionList(testVector[i]);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_EMBMS_SESSION_LIST);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i], jsonReqData[ns_embms_session_list_req::key_state].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolEmbmsBuilderJson, BuildNetworkTime) {
    TokenGen::Init();

    ProtocolEmbmsBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildNetworkTime();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_EMBMS_NETWORK_TIME);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolEmbmsBuilderJson, BuildSignalStrength) {
    TokenGen::Init();

    ProtocolEmbmsBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSignalStrength();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_EMBMS_SIGNAL_STRENGTH);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}
