/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * netdata_test.cpp
 *
 *  Created on: 2021. 8. 2.
 */

#include <gtest/gtest.h>
#include <telephony/ril.h>
#include "netdata.h"
#include "librilutils/textutils.h"
#include "librilutils/vendor.h"
#include "networkutils.h"
#include <list>
#include <vector>

TEST(NetData, NetRCData) {
    RIL_RadioCapability testVector = {
        1, 2, RadioCapabilityPhase::RC_PHASE_START, RADIO_TECH_LTE,
        "logicalModemUuid", RadioCapabilityStatus::RC_STATUS_SUCCESS,
    };

    NetRCData test(RIL_REQUEST_SET_RADIO_CAPABILITY, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetVersion(), testVector.version);
    EXPECT_EQ(test.GetSession(), testVector.session);
    EXPECT_EQ(test.GetPhase(), testVector.phase);
    EXPECT_TRUE(TextUtils::Equals(test.GetString(), testVector.logicalModemUuid));
    EXPECT_EQ(test.GetStatus(), testVector.status);

    NetRCData copy = test;
    EXPECT_EQ(test.GetVersion(), copy.GetVersion());
    EXPECT_EQ(test.GetSession(), copy.GetSession());
    EXPECT_EQ(test.GetPhase(), copy.GetPhase());
    EXPECT_TRUE(TextUtils::Equals(test.GetString(), copy.GetString()));
    EXPECT_EQ(test.GetStatus(), copy.GetStatus());
}

TEST(NetData, NetworkScanReqData) {
    char *mccMncs[] = { (char *)"45005", (char *)"00101", };
    RIL_NetworkScanRequest_V1_2 testVector = {
        RIL_ScanType::RIL_PERIODIC,
        3000,
        1,
        {
            {
                .radio_access_network = RIL_RadioAccessNetworks::RIL_RAN_EUTRAN,
                .bands_length = 3,
                .bands.eutran_bands = {
                    RIL_EutranBands::EUTRAN_BAND_10,
                    RIL_EutranBands::EUTRAN_BAND_11,
                    RIL_EutranBands::EUTRAN_BAND_12
                },
                .channels_length = 2,
                .channels = { 1000, 1100 }
            },
        },
        300,
        true,
        3,
        sizeof(mccMncs) / sizeof(mccMncs[0]),
        mccMncs,
    };
    NetworkScanReqData test(RIL_REQUEST_START_NETWORK_SCAN, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetScanType(), (int)testVector.type);
    EXPECT_EQ(test.GetTimeInterval(), testVector.interval);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetSpecifiersLength(), testVector.specifiers_length);
    if (test.GetSpecifiersLength() > 0) {
        RIL_RadioAccessSpecifier_V1_5 *testRas = test.GetRadioAccessSpecifier();
        for (int i = 0; testRas && i < test.GetSpecifiersLength(); i++) {
            EXPECT_EQ(testRas[i].radio_access_network, testVector.specifiers[i].radio_access_network);
            EXPECT_EQ(testRas[i].bands_length, testVector.specifiers[i].bands_length);
            EXPECT_TRUE(memcmp(testRas[i].bands.eutran_bands, testVector.specifiers[i].bands.eutran_bands,
                    sizeof(int) * testVector.specifiers[i].bands_length) == 0);
            EXPECT_EQ(testRas[i].channels_length, testVector.specifiers[i].channels_length);
            EXPECT_TRUE(memcmp(testRas[i].channels, testVector.specifiers[i].channels,
                    sizeof(int) * testVector.specifiers[i].channels_length) == 0);
        }
    }
    EXPECT_EQ(test.GetMaxSearchTime(), testVector.maxSearchTime);
    EXPECT_EQ(test.GetIncrementalResults(), testVector.incrementalResults);
    EXPECT_EQ(test.GetIncrementalResultsPeriodicity(), testVector.incrementalResultsPeriodicity);
    EXPECT_EQ(test.GetNumOfMccMncs(), testVector.numOfMccMncs);
    if (test.GetNumOfMccMncs() > 0) {
        EXPECT_NE(test.GetMccMncs(), nullptr);
        char **testMccMncs = test.GetMccMncs();
        for (int i = 0; testMccMncs && i < test.GetNumOfMccMncs(); i++) {
            EXPECT_TRUE(TextUtils::Equals(testMccMncs[i], testVector.mccMncs[i]));
        }
    }
    EXPECT_EQ(test.IsLegacyRequest(), false);

    NetworkScanReqData copy = test;
}

TEST(NetData, NetworkScanReqData_V1_5) {
    char *mccMncs[] = { (char *)"45005", (char *)"00101", };
    RIL_NetworkScanRequest_V1_5 testVector = {
        RIL_ScanType::RIL_PERIODIC,
        3000,
        1,
        {
            {
                .radio_access_network = RIL_RadioAccessNetworks::RIL_RAN_EUTRAN,
                .bands_length = 3,
                .bands.eutran_bands = {
                    RIL_EutranBands::EUTRAN_BAND_10,
                    RIL_EutranBands::EUTRAN_BAND_11,
                    RIL_EutranBands::EUTRAN_BAND_12
                },
                .channels_length = 2,
                .channels = { 1000, 1100 }
            },
        },
        300,
        true,
        3,
        sizeof(mccMncs) / sizeof(mccMncs[0]),
        mccMncs,
    };
    NetworkScanReqData test(ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN,
            HAL_VERSION_CODE(1, 5)), 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetScanType(), (int)testVector.type);
    EXPECT_EQ(test.GetTimeInterval(), testVector.interval);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetSpecifiersLength(), testVector.specifiers_length);
    if (test.GetSpecifiersLength() > 0) {
        RIL_RadioAccessSpecifier_V1_5 *testRas = test.GetRadioAccessSpecifier();
        for (int i = 0; testRas && i < test.GetSpecifiersLength(); i++) {
            EXPECT_TRUE(memcmp(testRas + i, testVector.specifiers + i,
                    sizeof(RIL_RadioAccessSpecifier_V1_5)) == 0);
        }
    }
    EXPECT_EQ(test.GetMaxSearchTime(), testVector.maxSearchTime);
    EXPECT_EQ(test.GetIncrementalResults(), testVector.incrementalResults);
    EXPECT_EQ(test.GetIncrementalResultsPeriodicity(), testVector.incrementalResultsPeriodicity);
    EXPECT_EQ(test.GetNumOfMccMncs(), testVector.numOfMccMncs);
    if (test.GetNumOfMccMncs() > 0) {
        EXPECT_NE(test.GetMccMncs(), nullptr);
        char **testMccMncs = test.GetMccMncs();
        for (int i = 0; testMccMncs && i < test.GetNumOfMccMncs(); i++) {
            EXPECT_TRUE(TextUtils::Equals(testMccMncs[i], testVector.mccMncs[i]));
        }
    }
    EXPECT_EQ(test.IsLegacyRequest(), false);
}

TEST(NetData, SystemSelectionChannelsReqData) {
    RIL_SysSelChannelRequest_V1_5 testVector = {
        true,
        1,
        {
            {
                .radio_access_network = RIL_RadioAccessNetworks::RIL_RAN_EUTRAN,
                .bands_length = 3,
                .bands.eutran_bands = {
                    RIL_EutranBands::EUTRAN_BAND_10,
                    RIL_EutranBands::EUTRAN_BAND_11,
                    RIL_EutranBands::EUTRAN_BAND_12
                },
                .channels_length = 2,
                .channels = { 1000, 1100 }
            },
        },
    };
    SystemSelectionChannelsReqData test(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, 0);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), -1);
}

TEST(NetData, SystemSelectionChannelsReqData_V1_5) {

    RIL_SysSelChannelRequest_V1_5 testVector = {
        true,
        1,
        {
            {
                .radio_access_network = RIL_RadioAccessNetworks::RIL_RAN_EUTRAN,
                .bands_length = 3,
                .bands.eutran_bands = {
                    RIL_EutranBands::EUTRAN_BAND_10,
                    RIL_EutranBands::EUTRAN_BAND_11,
                    RIL_EutranBands::EUTRAN_BAND_12
                },
                .channels_length = 2,
                .channels = { 1000, 1100 }
            },
        },
    };
    SystemSelectionChannelsReqData test(ENCODE_REQUEST(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS,
            HAL_VERSION_CODE(1, 5)), 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetIsSpecifyChannels(), testVector.isSpecifyChannels);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ test.GetSpecifiersLength(), testVector.specifiers_length);
    if (test.GetSpecifiersLength() > 0) {
        RIL_RadioAccessSpecifier_V1_5 *testRas = test.GetRadioAccessSpecifier();
        for (int i = 0; testRas && i < test.GetSpecifiersLength(); i++) {
            EXPECT_TRUE(memcmp(testRas + i, testVector.specifiers + i,
                    sizeof(RIL_RadioAccessSpecifier_V1_5)) == 0);
        }
    }

    SystemSelectionChannelsReqData copy = test;
    EXPECT_EQ(copy.GetIsSpecifyChannels(), test.GetIsSpecifyChannels());
    EXPECT_EQ(copy.GetSpecifiersLength(), test.GetSpecifiersLength());
    if (copy.GetSpecifiersLength() > 0) {
        RIL_RadioAccessSpecifier_V1_5 *testRas = copy.GetRadioAccessSpecifier();
        for (int i = 0; testRas && i < copy.GetSpecifiersLength(); i++) {
            EXPECT_TRUE(memcmp(testRas + i, test.GetRadioAccessSpecifier() + i,
                    sizeof(RIL_RadioAccessSpecifier_V1_5)) == 0);
        }
    }
}

TEST(NetData, DataThrottlingReqData) {
    RIL_DataThrottling testVector = {
        RIL_DataThrottlingAction::THROTTLE_ANCHOR_CARRIER,
        1000LL,
    };
    DataThrottlingReqData test(RIL_REQUEST_SET_DATA_THROTTLING, 0);
    EXPECT_EQ(test.encode(0, 0), -1);
    EXPECT_EQ(test.encode(0, sizeof(testVector)), -1);
    EXPECT_EQ(test.encode((char *)&testVector, sizeof(testVector)), 0);
    EXPECT_EQ(test.GetDataThrottlingAction(), (int)testVector.dataThrottlingAction);
    EXPECT_TRUE(test.GetCompletionDurationMillis() == testVector.completionDurationMillis);

    DataThrottlingReqData copy = test;
    EXPECT_EQ(copy.GetDataThrottlingAction(), test.GetDataThrottlingAction());
    EXPECT_TRUE(copy.GetCompletionDurationMillis() == test.GetCompletionDurationMillis());
}

TEST(NetData, PhysicalChannelConfigsData) {
    PhysicalChannelConfigsData test;
    test.setCurrentStatusRat(RIL_CellConnectionStatus::PRIMARY_SERVING, RADIO_TECH_LTE);
    test.setDownLinkInfo(1400, 40, RF_INFO_TYPE_CHANNEL_NUMBER, 60000);
    int contextIds[] = { 1, 2, 3 };
    test.setContextIds(3, contextIds);
    contextIds[0] = 4;
    test.setContextIds(3, contextIds);
    test.setUplinkInfo(2000, 20, 19);

    EXPECT_EQ(test.getStatus(), (int)RIL_CellConnectionStatus::PRIMARY_SERVING);
    EXPECT_EQ(test.getCellBandwidthDownlink(), 40);
    EXPECT_EQ(test.getRat(), RADIO_TECH_LTE);
    EXPECT_EQ(test.getRfInfoType(), RF_INFO_TYPE_CHANNEL_NUMBER);
    EXPECT_EQ(test.getChannelNumber(), 1400);
    EXPECT_EQ(test.getLenContextIds(), 3);
    ASSERT_NE(test.getContextIds(), nullptr);
    EXPECT_TRUE(memcmp(test.getContextIds(), contextIds, sizeof(contextIds)) == 0);
    EXPECT_EQ(test.getPhysicalCellId(), 60000);
    EXPECT_EQ(test.getUplinkChannelNumber(), 2000);
    EXPECT_EQ(test.getCellBandwidthUplink(), 20);
    EXPECT_EQ(test.getBandNumber(), 19);

    PhysicalChannelConfigsData copy = test;
    EXPECT_TRUE(TextUtils::Equals(test.toString(), copy.toString()));
}

TEST(NetData, CellInfoList_V1_5_with_RIL_CellInfo_V1_5) {
    std::list<RIL_CellInfo_V1_5> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .gsm = {
                    { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 3, 7 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .wcdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 15, 3, 255, 255 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .tdscdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                    },
                    { 15, 3, 255 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .lte = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { 15, 140, 10, -100, 0, 0x7FFFFFFE },
                    true
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
            RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .nr = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                    },
                    { 44, 7, -10, 44, 7, -10 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
    };

    CellInfoList_V1_5 test = testVector;
    EXPECT_NE(test.GetData(), nullptr);
    EXPECT_TRUE(test.GetDataLength() == testVector.size() * sizeof(RIL_CellInfo_V1_5));
    size_t i = 0;
    for (auto& info : testVector) {
        auto& cur = *((RIL_CellInfo_V1_5 *)test.GetData() + i++);
        EXPECT_TRUE(cur == info);
    }
}

TEST(NetData, CellInfoList_V1_5_with_RIL_CellInfo_V1_6) {
    std::list<RIL_CellInfo_V1_6> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .gsm = {
                    { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 3, 7 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .wcdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 15, 3, 255, 255 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .tdscdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                    },
                    { 15, 3, 255 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .lte = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
                    true
                },
            },
        },
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
            RIL_TIMESTAMP_TYPE_OEM_RIL, RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .nr = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                    },
                    { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
                },
            },
        },
    };
    std::vector<RIL_CellInfo_V1_5> tmpTestVector;
    for (auto& info : testVector) {
        RIL_CellInfo_V1_5 tmp {};
        RadioConvUtils::convertCellInfo(tmp, info);
        tmpTestVector.push_back(tmp);
    }
    CellInfoList_V1_5 test = testVector;
    EXPECT_NE(test.GetData(), nullptr);
    EXPECT_TRUE(test.GetDataLength() == tmpTestVector.size() * sizeof(RIL_CellInfo_V1_5));
    size_t i = 0;
    for (auto& info : tmpTestVector) {
        auto& cur = *((RIL_CellInfo_V1_5 *)test.GetData() + i++);
        EXPECT_TRUE(cur == info);
    }
}

TEST(NetData, CellInfoList_V1_6) {
    std::list<RIL_CellInfo_V1_6> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .gsm = {
                    { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 3, 7 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .wcdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 15, 3, 255, 255 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .tdscdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                    },
                    { 15, 3, 255 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .lte = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
                    true
                },
            },
        },
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
            RIL_TIMESTAMP_TYPE_OEM_RIL, RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .nr = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                    },
                    { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
                },
            },
        },
    };

    CellInfoList_V1_6 test = testVector;
    EXPECT_NE(test.GetData(), nullptr);
    EXPECT_TRUE(test.GetDataLength() == testVector.size() * sizeof(RIL_CellInfo_V1_6));
    size_t i = 0;
    for (auto& info : testVector) {
        auto& cur = *((RIL_CellInfo_V1_6 *)test.GetData() + i++);
        EXPECT_TRUE(cur == info);
    }
}