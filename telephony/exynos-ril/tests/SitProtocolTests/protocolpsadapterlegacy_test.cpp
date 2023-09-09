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

// Ril Service Layer
#include <messageid.h>
#include <telephony/ril.h>
#include <base/rildef.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "legacy/ps/protocolpsadapterlegacy.h"

TEST(ProtocolPsAdapterLegacy, ProtocolPsStartKeepAliveAdapterLegacy) {

    struct {
        UINT32 session_handle;
        INT32 status_code;
    } testVector[] = {
        {10, KEEPALIVE_ACTIVE},
    };

    ModemData * modemData = ModemDataBuilder::BuildResponse(SIT_START_KEEPALIVE, 0, RCM_E_SUCCESS, &testVector, sizeof(testVector));

    ASSERT_NE(modemData, nullptr);
    if (modemData) {
        ProtocolPsStartKeepAliveAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_START_KEEPALIVE);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_pdp_start_keepalive_rsp));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);

        EXPECT_EQ(testAdapter.getSessionHandle(), testVector[0].session_handle);
        EXPECT_EQ(testAdapter.getCode(), testVector[0].status_code);
    }

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolPsAdapterLegacy, ProtocolPsKeepAliveStatusAdapterLegacy) {

    struct {
        UINT32 session_handle;
        INT32 status_code;
    } testVector[] = {
        {10, KEEPALIVE_ACTIVE},
    };

    ModemData *modemData = ModemDataBuilder::BuildIndication(SIT_IND_KEEPALIVE_STATUS, &testVector, sizeof(testVector));

    ASSERT_NE(modemData, nullptr);

    if (modemData) {
        ProtocolPsKeepAliveStatusAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_KEEPALIVE_STATUS);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_pdp_keepalive_status_ind));
        EXPECT_NE(testAdapter.GetParameter(), nullptr);

        EXPECT_EQ(testAdapter.getSessionHandle(), testVector[0].session_handle);
        EXPECT_EQ(testAdapter.getCode(), testVector[0].status_code);
    }

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolPsAdapterLegacy, ProtocolPsPcoDataAdapterLegacy) {

    char contents[10] = {1,2,3,4};
    struct {
        BYTE cid;
        BYTE pdp_type;
        BYTE pco_num;
        sit_pdp_pco_data_entry pcd_data;
    } testVector[] = {
        {1, PDP_TYPE_IPV4V6, 1, {1, 4, contents}},
    };

    ModemData *modemData = ModemDataBuilder::BuildIndication(SIT_IND_PCO_DATA, &testVector, sizeof(testVector));

    ASSERT_NE(modemData, nullptr);

    if (modemData) {
        ProtocolPsPcoDataAdapterLegacy testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_PCO_DATA);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);

        EXPECT_EQ(testAdapter.GetCid(), 1);
        EXPECT_EQ(testAdapter.GetPdpType(), PDP_TYPE_IPV4V6);
        EXPECT_EQ(testAdapter.GetPcoNum(), 1);

        sit_pdp_pco_data_entry e;
        testAdapter.GetPcoData(e);

        EXPECT_EQ(e.pco_id, 1);
        EXPECT_EQ(e.contents_len, 4);
    }

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolPsAdapterLegacy, ProtocolPsDedicatedBearInfoAdapterLegacy) {

    const unsigned int sitId = SIT_IND_DEDICATED_BEARER_INFO;
    char testVector[][111] = {
        {
            /*SIT HEADER*/0x02, 0x00, 0x21, 0x06, 0x4d, 0x01, 0x00, 0x00,/*CID*/0x08,/*STATUS*/0x01,/*SESSION ID*/0x00, 0xe7, 0x00, 0x00,/*RAT TYPE*/ 0x01,
            /*LTE QOS INFO*/0x01, 0x00,/**/0x34, 0x00, 0x00, 0x00,/**/0x34, 0x00, 0x00, 0x00,/**/0x34, 0x00, 0x00, 0x00,/**/0x34, 0x00, 0x00, 0x00,/**/0x00, 0x00, 0x00,
            /*FILTER NUM*/ 0x01,
            /*FLAG*/0x1F, 0x00, 0x00, 0x00,
            /*LOCAL ADD*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,/*PREFIX*/0x00, 0x00,
            /*REMOTE ADD*/ 0x24, 0x09, 0x80, 0x00, 0x50, 0x02, 0x32, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, /*PREFIX*/0x00, 0x00,
            /*LOCAL PORT*/0x63, 0x1b, 0x63, 0x1b,
            /*REMOTE PORT*/0xb5, 0xc3, 0xb5, 0xc3,
            /*QOS PROTOCOL*/0x11, 0x00, 0x00, 0x00,
            /*TYPE OF SERVICE*/0x00,
            /*FLOW LABEL*/ 0x00, 0x00, 0x00, 0x00,
            /*IP_SEC_SPI*/0x00, 0x00, 0x00, 0x00,
            /*DETECTION*/0x01,
            /*PRECEDENCE*/0x00, 0x00, 0x00, 0x00,
        },

        {
            /*SIT HEADER*/0x02, 0x00, 0x21, 0x06, 0x4d, 0x01, 0x00, 0x00,/*CID*/0x08,/*STATUS*/0x01,/*SESSION ID*/0x00, 0xe7, 0x00, 0x00,/*RAT TYPE*/ 0x02,
            /*NR QOS INFO*/0x01, 0x00,/**/0x34, 0x00, 0x00, 0x00,/**/0x34, 0x00, 0x00, 0x00,/**/0x34, 0x00, 0x00, 0x00,/**/0x34, 0x00, 0x00, 0x00,/**/0x01,/**/0x01, 0x00,
            /*FILTER NUM*/ 0x01,
            /*FLAG*/0x1F, 0x00, 0x00, 0x00,
            /*LOCAL ADD*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,/*PREFIX*/0x00, 0x00,
            /*REMOTE ADD*/ 0x24, 0x09, 0x80, 0x00, 0x50, 0x02, 0x32, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, /*PREFIX*/0x00, 0x00,
            /*LOCAL PORT*/0x63, 0x1b, 0x63, 0x1b,
            /*REMOTE PORT*/0xb5, 0xc3, 0xb5, 0xc3,
            /*QOS PROTOCOL*/0x11, 0x00, 0x00, 0x00,
            /*TYPE OF SERVICE*/0x00,
            /*FLOW LABEL*/ 0x00, 0x00, 0x00, 0x00,
            /*IP_SEC_SPI*/0x00, 0x00, 0x00, 0x00,
            /*DETECTION*/0x01,
            /*PRECEDENCE*/0x00, 0x00, 0x00, 0x00,
        },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size ; i++){
        ModemData modemData(testVector[i], sizeof(testVector[0]));
        ProtocolPsDedicatedBearInfoAdapterLegacy testAdapter(&modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);

        EXPECT_EQ(testAdapter.GetCid(), 0x08);
        EXPECT_EQ(testAdapter.GetStatus(), 0x01);

        QosSession_t m_QosSession = testAdapter.GetQosSessionInfo();

        EXPECT_EQ(m_QosSession.qosSessionId, 59136);

        if(m_QosSession.defaultQosType == EPS_QOS){
            EXPECT_EQ(m_QosSession.defaultEpsQos.qci, 1);
            EXPECT_EQ(m_QosSession.defaultEpsQos.downlink.guaranteedBitrateKbps, 52);
            EXPECT_EQ(m_QosSession.defaultEpsQos.downlink.maxBitrateKbps, 52);
            EXPECT_EQ(m_QosSession.defaultEpsQos.uplink.guaranteedBitrateKbps, 52);
            EXPECT_EQ(m_QosSession.defaultEpsQos.uplink.maxBitrateKbps, 52);
        }
        else if(m_QosSession.defaultQosType == NR_QOS){
            EXPECT_EQ(m_QosSession.defaultNrQos.fiveQi, 1);
            EXPECT_EQ(m_QosSession.defaultNrQos.downlink.guaranteedBitrateKbps, 52);
            EXPECT_EQ(m_QosSession.defaultNrQos.downlink.maxBitrateKbps, 52);
            EXPECT_EQ(m_QosSession.defaultNrQos.uplink.guaranteedBitrateKbps, 52);
            EXPECT_EQ(m_QosSession.defaultNrQos.uplink.maxBitrateKbps, 52);
            EXPECT_EQ(m_QosSession.defaultNrQos.qfi, 1);
            EXPECT_EQ(m_QosSession.defaultNrQos.averagingWindowMs, 1);
        }

        for(int k = 0 ; k < m_QosSession.QosFilterCnt ; k++){
            for(int j = 0 ; j < 16 ; j ++){
                EXPECT_EQ(m_QosSession.QosFilters[k].localAddress[0][j], testVector[i][41 + j]);
                EXPECT_EQ(m_QosSession.QosFilters[k].remoteAddress[0][j], testVector[i][63 + j]);
            }

            for(int j = 0 ; j < 4 ; j ++){
                EXPECT_EQ(m_QosSession.QosFilters[k].localAddress[1][j], testVector[i][41 + 16 + j]);
                EXPECT_EQ(m_QosSession.QosFilters[k].remoteAddress[1][j], testVector[i][63 + 16 + j]);
            }

            EXPECT_EQ(m_QosSession.QosFilters[k].localAddrPrefix_v4, (const unsigned int)0);
            EXPECT_EQ(m_QosSession.QosFilters[k].localAddrPrefix_v6, 0);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortprefix_4, 0);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortprefix_6, 0);
            EXPECT_EQ(m_QosSession.QosFilters[k].localPortFlag, MAYBE_PORT_VALID);
            EXPECT_EQ(m_QosSession.QosFilters[k].localPortStart, 7011);
            EXPECT_EQ(m_QosSession.QosFilters[k].localPortEnd, 7011);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortFlag, MAYBE_PORT_VALID);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortStart, 50101);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortEnd, 50101);
            EXPECT_EQ(m_QosSession.QosFilters[k].protocol, (const unsigned int)17);
            EXPECT_EQ(m_QosSession.QosFilters[k].tosFlag, 1);
            EXPECT_EQ(m_QosSession.QosFilters[k].tosValue, 0);
            EXPECT_EQ(m_QosSession.QosFilters[k].flowLabelFlag, IPV6_FLOW_LABEL_VALID);
            EXPECT_EQ(m_QosSession.QosFilters[k].flowLabelValue, (const unsigned int)0);
            EXPECT_EQ(m_QosSession.QosFilters[k].spiFlag, IP_SEC_SPI_VALID);
            EXPECT_EQ(m_QosSession.QosFilters[k].spiValue, (const unsigned int)0);
            EXPECT_EQ(m_QosSession.QosFilters[k].direction, 0);
            EXPECT_EQ(m_QosSession.QosFilters[k].precedence, 0);
        }
    }
}
