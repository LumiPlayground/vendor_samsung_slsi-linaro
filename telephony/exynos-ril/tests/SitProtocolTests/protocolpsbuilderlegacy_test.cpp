/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include <gtest/gtest.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "protocoladapter.h"
#include "legacy/ps/protocolpsbuilderlegacy.h"


TEST(ProtocolPsBuilderLegacy, BuildDeactDataCall) {
    struct {
        int cid;
        int reason;
    } testVector[] = {
        { 1, DEACT_REASON_NORMAL},
        { 2, DEACT_REASON_PDP_RESET},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolPsBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildDeactDataCall(testVector[i].cid, testVector[i].reason);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_DEACT_DATA_CALL);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_pdp_deact_data_call_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 2U);
        sit_pdp_deact_data_call_req *p = (sit_pdp_deact_data_call_req *)modemData->GetRawData();
        EXPECT_EQ(p->cid, testVector[i].cid);
        EXPECT_EQ(p->deact_reason, testVector[i].reason);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}




TEST(ProtocolPsBuilderLegacy, BuildStartKeepAlive) {

    char test_addresses[MAX_INADDR_LEN] = { "10.20.30.40" };

    RIL_KeepaliveRequest testVector = {
        .type=NATT_IPV4,
        .sourcePort = 10,
        .destinationPort = 11,
        .maxKeepaliveIntervalMillis = 12,
        .cid = 1,
    };

    memcpy(testVector.sourceAddress, test_addresses, MAX_INADDR_LEN);
    memcpy(testVector.destinationAddress, test_addresses, MAX_INADDR_LEN);

    ProtocolPsBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildStartKeepAlive(testVector);

    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_START_KEEPALIVE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_pdp_start_keepalive_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 49U);

    sit_pdp_start_keepalive_req *p = (sit_pdp_start_keepalive_req *)modemData->GetRawData();

    EXPECT_EQ(p->keepalive_type, testVector.type);
    EXPECT_EQ(p->source_port, testVector.sourcePort);
    EXPECT_EQ(p->dst_port, testVector.destinationPort);
    EXPECT_EQ(p->max_interval, testVector.maxKeepaliveIntervalMillis);
    EXPECT_EQ(p->cid, testVector.cid);

    for(int i = 0;i<MAX_INADDR_LEN;i++){
        EXPECT_EQ(p->src_addr[i], testVector.sourceAddress[i]);
        EXPECT_EQ(p->dst_addr[i], testVector.destinationAddress[i]);
    }

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}


TEST(ProtocolPsBuilderLegacy, BuildStopKeepAlive) {
    unsigned int keepalive_handle = 10U;

    ProtocolPsBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildStopKeepAlive(keepalive_handle);

    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_STOP_KEEPALIVE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_pdp_stop_keepalive_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 4U);

    sit_pdp_stop_keepalive_req *p = (sit_pdp_stop_keepalive_req *)modemData->GetRawData();

    EXPECT_EQ(p->keepalive_handle, keepalive_handle);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }

}

