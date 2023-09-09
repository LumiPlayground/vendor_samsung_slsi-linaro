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
#include "json/protocolradioconfigbuilderjson.h"
#include "simjsondef.h"
#include "miscjsondef.h"
#include "modemdata.h"

using namespace std;

TEST(ProtocolRadioconfigBuilderJson, BuildGetPhoneCapability) {
    ProtocolRadioConfigBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetPhoneCapability();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PHONE_CAPABILITY);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolRadioconfigBuilderJson, BuildSimGetSlotStatus) {
    ProtocolRadioConfigBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSimGetSlotStatus();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_SLOT_STATUS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolRadioConfigBuilderJson, BuildSimSetLogicalSlotMapping) {
    struct {
        int nSlotMapLen;
        int slotMap[5];
        bool isModemDataNull;
    } testVector[] = {
        { 3, { 0, 1, 255 }, false },
        // modemData NULL case
        { 0, { }, true },
        { 5, { 0, 1, 255, 21, 42 }, true },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolRadioConfigBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSimSetLogicalSlotMapping(testVector[i].slotMap, testVector[i].nSlotMapLen);

        if(testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);
        int parsedSlotMapLen = jsonReqData[ns_sim_set_logical_to_physical_slot_mapping_req::key_slot_map].size();
        EXPECT_EQ(parsedSlotMapLen, testVector[i].nSlotMapLen);
        // data check
        for(int j = 0; j < parsedSlotMapLen; j++) {
            EXPECT_EQ(testVector[i].slotMap[j], jsonReqData[ns_sim_set_logical_to_physical_slot_mapping_req::key_slot_map][j].asInt());
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolRadioConfigBuilderJson, ProtocolRadioConfigBuilderJson_BuildSetModemsConfig) {
    struct {
        int noOfLiveModems;
        int cpConfig;
    } testVector[] = {
        { 1, 0 },
        { 2, 1 },
        { 3, 1 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        ProtocolRadioConfigBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetModemsConfig(testVector[i].noOfLiveModems);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_MODEM_CONFIG);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int config = jsonReqData[ns_misc_set_modem_config_req::key_config].asInt();
        EXPECT_EQ(config, testVector[i].cpConfig);

        delete modemData;
        modemData = nullptr;
    }
}
