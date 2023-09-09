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
#include "json/protocolvsimbuilderjson.h"
#include "vsimjsondef.h"
#include "rildef.h"
#include "modemdata.h"

TEST(ProtocolVsimBuilderJson, BuildVsimNotification) {
    struct {
        int tid;
        int event_type;
        int sit_event_type;
        int sim_type;
    } testVector[] = {
        { 1, REQUEST_TYPE_ENABLE_EXTERNAL_SIM, SIT_VSIM_NOTI_EN_EXTERNAL_SIM, SIT_VSIM_SIM_TYPE_LOCAL },
        { 2, REQUEST_TYPE_DISABLE_EXTERNAL_SIM, SIT_VSIM_NOTI_DIS_EXTERNAL_SIM, SIT_VSIM_SIM_TYPE_LOCAL },
        { 3, REQUEST_TYPE_PLUG_OUT, SIT_VSIM_NOTI_PLUG_OUT, SIT_VSIM_SIM_TYPE_REMOTE },
        { 4, REQUEST_TYPE_PLUG_IN, SIT_VSIM_NOTI_PLUG_IN, SIT_VSIM_SIM_TYPE_REMOTE },
        // invalid event_type case
        { 5, -1, -1, SIT_VSIM_SIM_TYPE_REMOTE },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        ProtocolVsimBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildVsimNotification(testVector[i].tid, testVector[i].event_type,
                testVector[i].sim_type);

        // invalid event_type case
        if (i == 4) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_VSIM_NOTIFICATION);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if (!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].tid, jsonReqData[ns_vsim_notification_req::key_tid].asInt());
        EXPECT_EQ(testVector[i].sit_event_type, jsonReqData[ns_vsim_notification_req::key_event_type].asInt());
        EXPECT_EQ(testVector[i].sim_type, jsonReqData[ns_vsim_notification_req::key_sim_type].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolVsimBuilderJson, BuildVsimOperation) {
    struct {
        int tid;
        int event_type;
        int sit_event_type;
        int result;
        char operationData[MAX_VSIM_DATA_LEN * 2 + 2]; //add 2 to check overflow case
    } testVector[] = {
        { 1, REQUEST_TYPE_ATR_EVENT, SIT_VSIM_OPERATION_ATR, 0, "3E0B52024E1553022000230054010555031A0601" },
        { 2, REQUEST_TYPE_APDU_EVENT, SIT_VSIM_OPERATION_APDU, 0, "3E0B52024E1553022000230054010555031A0601" },
        { 3, REQUEST_TYPE_CARD_POWER_DOWN, SIT_VSIM_OPERATION_POWERDOWN, 0, "3E0B52024E1553022000230054010555031A0601" },
        // invalid event_type case
        { 4, -1, -1, 0, "3E0B52024E1553022000230054010555031A0601" },
        // overflow case
        { 5, REQUEST_TYPE_CARD_POWER_DOWN, SIT_VSIM_OPERATION_POWERDOWN, RCM_E_GENERIC_FAILURE,
            { [0 ... MAX_VSIM_DATA_LEN * 2] = 'f', [MAX_VSIM_DATA_LEN * 2 + 1] = 0 } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        ProtocolVsimBuilderJson testBuilder;
        auto modemData = std::unique_ptr<ModemData>(
                testBuilder.BuildVsimOperation(testVector[i].tid, testVector[i].event_type,
                        testVector[i].result, strlen(testVector[i].operationData), testVector[i].operationData)
            );

        // invalid event_type case
        if (i == 3) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_VSIM_OPERATION);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].tid, jsonReqData[ns_vsim_opertaion_req::key_tid].asInt());
        EXPECT_EQ(testVector[i].sit_event_type, jsonReqData[ns_vsim_opertaion_req::key_event_type].asInt());
        EXPECT_EQ(testVector[i].result, jsonReqData[ns_vsim_opertaion_req::key_result].asInt());

        // overflow case
        if (i == 4) {
            EXPECT_EQ(0U, jsonReqData[ns_vsim_opertaion_req::key_operation_data].size());
            continue;
        }

        EXPECT_EQ(strlen(testVector[i].operationData), 2*jsonReqData[ns_vsim_opertaion_req::key_operation_data].size());

        int rawOperationData[] = { 0x3E, 0x0B, 0x52, 0x02, 0x4E, 0x15, 0x53, 0x02, 0x20, 0x00, 0x23,
            0x00, 0x54, 0x01, 0x05, 0x55, 0x03, 0x1A, 0x06, 0x01 };
        for (unsigned int j = 0; j < strlen(testVector[i].operationData) / 2; j++) {
            EXPECT_EQ(rawOperationData[j], jsonReqData[ns_vsim_opertaion_req::key_operation_data][j].asInt());
        }
    }
}
