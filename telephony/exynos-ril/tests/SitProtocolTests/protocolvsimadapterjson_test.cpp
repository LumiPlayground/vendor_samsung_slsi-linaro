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
#include "json/protocolvsimadapterjson.h"
#include "vsimjsondef.h"
#include <telephony/ril.h>
#include <telephony/ril_ext.h>
#include "sitdef.h"

TEST(ProtocolVsimAdapterJson, ProtocolVsimOperationAdapterJson) {
    struct {
        int tid;
        int result;
        int event_type;
        int ril_event_type;
        int operationData[7];
    } testVector[] = {
        { 1, 0, SIT_VSIM_OPERATION_ATR, REQUEST_TYPE_ATR_EVENT, { 0,164,0,4,2,63,0 } },
        { 2, 0, SIT_VSIM_OPERATION_APDU, REQUEST_TYPE_APDU_EVENT, { 0,164,0,4,2,63,0 } },
        { 3, 0, SIT_VSIM_OPERATION_POWERDOWN, REQUEST_TYPE_CARD_POWER_DOWN, { 0,164,0,4,2,63,0 } },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_vsim_opertaion_ind::key_tid] = testVector[i].tid;
        jsonIndData[ns_vsim_opertaion_ind::key_event_type] = testVector[i].event_type;
        jsonIndData[ns_vsim_opertaion_ind::key_result] = testVector[i].result;
        int nOperationDataLen = sizeof(testVector[i].operationData)/sizeof(testVector[i].operationData[0]);
        for(int j = 0; j < nOperationDataLen; j++) {
            jsonIndData[ns_vsim_opertaion_ind::key_operation_data].append(testVector[i].operationData[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_VSIM_OPERATION, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolVsimOperationAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_VSIM_OPERATION);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        EXPECT_EQ(testAdapter.GetTransactionId(), testVector[i].tid);
        EXPECT_EQ(testAdapter.GetEventId(), testVector[i].ril_event_type);
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);
        EXPECT_EQ(testAdapter.GetOperationDataLength(), nOperationDataLen*2+1);

        const char* expOperationData = "00A40004023F00";
        EXPECT_EQ(strcmp(expOperationData, testAdapter.GetOperationData()), 0);

        delete modemData;
        modemData = nullptr;
    }
}
