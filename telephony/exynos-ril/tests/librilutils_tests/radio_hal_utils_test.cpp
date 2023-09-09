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
 * radio_hal_utils_test.cpp
 *
 *  Created on: 2021. 7. 5.
 */

#include <gtest/gtest.h>
#include <librilutils/radio_hal_utils.h>
#include "radio_hal_utils_test_api.h"
#include <android/hardware/radio/1.5/types.h>
#include <slsi/radio_v1_5.h>

using namespace android::hardware::radio;
using ::android::hardware::hidl_string;

TEST(Radio2HalUtils, convertCharPtrToHidlString) {
    const char *testVector = "test123456";
    hidl_string ret = Radio2HalUtils::convertCharPtrToHidlString(testVector);
    EXPECT_TRUE(ret == hidl_string(testVector));
}

TEST(Radio2HalUtils, parseStrings) {
    vector<string> tmp;
    hidl_vec<hidl_string> strings;
    char testVector[] = "test12345";
    Radio2HalUtils::parseStrings(tmp, strings, testVector);
    ASSERT_EQ(tmp.size(), strings.size());
    for (size_t i = 0; i < tmp.size(); i++) {
        EXPECT_TRUE(hidl_string(tmp[i]) == strings[i]);
    }
}

TEST(Radio2HalUtils, convertResponseStringEntryToInt) {
    const char *testVector[] = {
        "0", "1", "2"
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(Radio2HalUtils::convertResponseStringEntryToInt((char **)testVector, i, size) == (int)i);
    }
    EXPECT_TRUE(Radio2HalUtils::convertResponseStringEntryToInt(nullptr, 0, size) == -1);
}

TEST(Radio2HalUtils, convertResponseHexStringEntryToInt) {
    const char *testVector[] = {
        "0xa", "0xb", "0xc"
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(Radio2HalUtils::convertResponseHexStringEntryToInt((char **)testVector, i, size) == (int)i + 10);
    }
    EXPECT_TRUE(Radio2HalUtils::convertResponseHexStringEntryToInt(nullptr, 0, size) == -1);
}

TEST(Radio2HalUtils, convertStringToAdnString) {
    std::string testVector = "We make a test vector!";
    unsigned char buf[100] {};

    int ret = Radio2HalUtils::convertStringToAdnString(testVector, buf, 50);
    EXPECT_TRUE(ret > 0);
}

TEST(Radio2HalUtils, getPdpType) {
    struct {
        const char *protocol;
        V1_4::PdpProtocolType pdptype;
    } testVector[] = {
        { "IP", V1_4::PdpProtocolType::IP },
        { "IPV6", V1_4::PdpProtocolType::IPV6 },
        { "IPV4V6", V1_4::PdpProtocolType::IPV4V6 },
        { 0, V1_4::PdpProtocolType::IP },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(Radio2HalUtils::getPdpType(testVector[i].protocol) == testVector[i].pdptype);
    }
}

TEST(Radio2HalUtils, convertOperatorStatusToInt) {
    struct {
        const char *status;
        int type;
    } testVector[] = {
        { "unknown", (int)OperatorStatus::UNKNOWN },
        { "available", (int)OperatorStatus::AVAILABLE },
        { "current", (int)OperatorStatus::CURRENT },
        { "forbidden", (int)OperatorStatus::FORBIDDEN },
        { 0, -1 },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        EXPECT_TRUE(Radio2HalUtils::convertOperatorStatusToInt(testVector[i].status) == testVector[i].type);
    }
}

TEST(Radio2HalUtils, convertRilCardStatusToHal) {
    V1_0::CardStatus cardStatus {};
    EXPECT_FALSE(Radio2HalUtils::convertRilCardStatusToHal(nullptr, 0, cardStatus));

    RIL_CardStatus_v6 testVector = {
        .gsm_umts_subscription_app_index = 1,
        .cdma_subscription_app_index = 2,
        .ims_subscription_app_index = 3,
        .num_applications = 1,
    };
    EXPECT_FALSE(Radio2HalUtils::convertRilCardStatusToHal(&testVector, sizeof(RIL_CardStatus_v6), cardStatus));
}

TEST(Radio2HalUtils, convertRilCallToHal) {
    V1_0::Call call {};
    RIL_UUS_Info testUusInfo = {
        RIL_UUS_Type::RIL_UUS_TYPE1_IMPLICIT,
        RIL_UUS_DCS::RIL_UUS_DCS_IA5c,
        (int)strlen("01012345678"),
        (char *)"01012345678",
    };
    RIL_Call testVector = {
        RIL_CallState::RIL_CALL_DIALING,
        0,
        145,
        0,
        0,
        0,
        1,
        0,
        (char *)"01012345678",
        1,
        (char *)"Test",
        0,
        &testUusInfo,
    };

    // an invalid parameter test
    EXPECT_FALSE(Radio2HalUtils::convertRilCallToHal(nullptr, sizeof(testVector), call));
    EXPECT_FALSE(Radio2HalUtils::convertRilCallToHal(&testVector, sizeof(testVector) + 1, call));

    EXPECT_TRUE(Radio2HalUtils::convertRilCallToHal(&testVector, sizeof(testVector), call));
}



TEST(Radio2HalUtils, fillCellIdentityResponse) {
    RIL_CellIdentity_v16 testVector[] = {
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            .cellIdentityGsm = {
                450, 5, INT_MAX, INT_MAX, 0, 0xFF
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            .cellIdentityWcdma = {
                450, 5, INT_MAX, INT_MAX, 0, 0
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
            .cellIdentityCdma = {
                INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            .cellIdentityLte = {
                450, 5, INT_MAX, 0, INT_MAX, 0
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
            .cellIdentityTdscdma = {
                450, 5, INT_MAX, INT_MAX, INT_MAX
            },
        },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        V1_0::CellIdentity cellIdentity {};
        Radio2HalUtils::fillCellIdentityResponse(cellIdentity, testVector[i]);
        switch ((int)cellIdentity.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            EXPECT_EQ((int)cellIdentity.cellIdentityGsm.size(), 1);
            if (cellIdentity.cellIdentityGsm.size() > 0) {
                testCellIdentityGsm(cellIdentity.cellIdentityGsm[0], testVector[i].cellIdentityGsm);
            }
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            EXPECT_EQ((int)cellIdentity.cellIdentityWcdma.size(), 1);
            if (cellIdentity.cellIdentityWcdma.size() > 0) {
                testCellIdentityWcdma(cellIdentity.cellIdentityWcdma[0], testVector[i].cellIdentityWcdma);
            }
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            EXPECT_EQ((int)cellIdentity.cellIdentityCdma.size(), 1);
            if (cellIdentity.cellIdentityCdma.size() > 0) {
                testCellIdentityCdma(cellIdentity.cellIdentityCdma[0], testVector[i].cellIdentityCdma);
            }
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            EXPECT_EQ((int)cellIdentity.cellIdentityLte.size(), 1);
            if (cellIdentity.cellIdentityLte.size() > 0) {
                testCellIdentityLte(cellIdentity.cellIdentityLte[0], testVector[i].cellIdentityLte);
            }
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            EXPECT_EQ((int)cellIdentity.cellIdentityTdscdma.size(), 1);
            if (cellIdentity.cellIdentityTdscdma.size() > 0) {
                testCellIdentityTdscdma(cellIdentity.cellIdentityTdscdma[0], testVector[i].cellIdentityTdscdma);
            }
            break;
        }
    }
}

TEST(Radio2HalUtils, convertRilSignalStrengthToHal) {
    V1_0::SignalStrength signalStrength;
    // an invalid parameter test
    Radio2HalUtils::convertRilSignalStrengthToHal(nullptr, 0, signalStrength);
    Radio2HalUtils::convertRilSignalStrengthToHal(nullptr, 2, signalStrength);

    RIL_SignalStrength_v10 testVecotr[] = {
        {
            .GW_SignalStrength = { 99, 99 },
            .CDMA_SignalStrength = { INT_MAX, INT_MAX },
            .EVDO_SignalStrength = { INT_MAX, INT_MAX, 0 },
            .LTE_SignalStrength = { -1, -1, -1, INT_MAX, -1, INT_MAX },
            .TD_SCDMA_SignalStrength = { INT_MAX },
        },
        {
            .GW_SignalStrength = { 99, 99 },
            .CDMA_SignalStrength = { INT_MAX, INT_MAX },
            .EVDO_SignalStrength = { INT_MAX, INT_MAX, 0 },
            .LTE_SignalStrength = { -1, -2, -1, INT_MAX, -1, INT_MAX },
            .TD_SCDMA_SignalStrength = { INT_MAX },
        },
    };
    size_t size = sizeof(testVecotr) / sizeof(testVecotr[0]);
    for (size_t i = 0; i < size; i++) {
        RIL_SignalStrength_v10 *rilSignalStrength = &testVecotr[i];
        Radio2HalUtils::convertRilSignalStrengthToHal(rilSignalStrength,
                sizeof(RIL_SignalStrength_v10), signalStrength);
        testGsmSignalStrength(signalStrength.gw, rilSignalStrength->GW_SignalStrength);
        testCdmaSignalStrength(signalStrength.cdma, rilSignalStrength->CDMA_SignalStrength);
        testEvdoSignalStrength(signalStrength.evdo, rilSignalStrength->EVDO_SignalStrength);
        testLteSignalStrength(signalStrength.lte, rilSignalStrength->LTE_SignalStrength);
        testTdScdmaSignalStrength(signalStrength.tdScdma, rilSignalStrength->TD_SCDMA_SignalStrength);
    }
}

TEST(Radio2HalUtils, convertRilCellInfoListToHal) {
    hidl_vec<V1_0::CellInfo> cellinfos;
    RIL_CellInfo_v12 testVector[] = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.gsm = {
                { 450, 5, INT_MAX, INT_MAX, 0, 0xFF },
                { 99, 99, INT_MAX },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.wcdma = {
                { 450, 5, INT_MAX, INT_MAX, 0, 0 },
                { 99, 99 },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.cdma = {
                { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, },
                { INT_MAX, INT_MAX },
                { INT_MAX, INT_MAX, 0 },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.lte = {
                { 450, 5, INT_MAX, 0, INT_MAX, 0 },
                { 99, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.tdscdma = {
                { 450, 5, INT_MAX, INT_MAX, INT_MAX },
                { INT_MAX },
            },
        },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoListToHal(nullptr, sizeof(testVector), cellinfos);

    Radio2HalUtils::convertRilCellInfoListToHal(testVector, sizeof(testVector), cellinfos);
    EXPECT_EQ(cellinfos.size(), size);
    for (size_t i = 0; i < cellinfos.size(); i++) {
        testCellInfo(cellinfos[i], testVector[i]);
    }
}

TEST(Radio2HalUtils, convertRilKeepaliveStatusToHal) {
    V1_1::KeepaliveStatus status {};

    // an invalid parameter test
    Radio2HalUtils::convertRilKeepaliveStatusToHal(nullptr, status);

    // TODO
}

TEST(Radio2HalUtils, convertRilDataCallToHal) {
    V1_0::SetupDataCallResult result;

    // an invalid parameter test
    Radio2HalUtils::convertRilDataCallToHal(nullptr, result);

    // TODO
}

TEST(Radio2HalUtils, convertRilDataCallListToHal) {
    hidl_vec<V1_0::SetupDataCallResult> dcResultList;
    RIL_Data_Call_Response_v11 testVector[] = {
        {},
        {},
        {},
    };

    // an invalid parameter test
    Radio2HalUtils::convertRilDataCallListToHal(nullptr, sizeof(testVector), dcResultList);

    // TODO
}

TEST(Radio2HalUtils, convertRilCdmaSignalInfoRecordToHal) {
    V1_0::CdmaSignalInfoRecord record {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCdmaSignalInfoRecordToHal(nullptr, record);

    // TODO
}

TEST(Radio2HalUtils, convertRilHardwareConfigListToHal) {
    hidl_vec<V1_0::HardwareConfig> configs;

    // an invalid parameter test
    Radio2HalUtils::convertRilHardwareConfigListToHal(nullptr, 0, configs);

    // TODO
}

TEST(Radio2HalUtils, convertRilRadioCapabilityToHal) {
    V1_0::RadioCapability rc;

    // an invalid parameter test
    Radio2HalUtils::convertRilRadioCapabilityToHal(nullptr, 0, rc);

    // TODO
}

TEST(Radio2HalUtils, convertRilLceDataInfoToHal) {
    V1_0::LceDataInfo lce {};

    // an invalid parameter test
    Radio2HalUtils::convertRilLceDataInfoToHal(nullptr, 0, lce);

    RIL_LceDataInfo testVector = { 1, 5, 1 };
    Radio2HalUtils::convertRilLceDataInfoToHal(&testVector, sizeof(RIL_LceDataInfo), lce);
    EXPECT_EQ(lce.lastHopCapacityKbps, testVector.last_hop_capacity_kbps);
    EXPECT_EQ(lce.confidenceLevel, testVector.confidence_level);
    EXPECT_EQ(lce.lceSuspended, testVector.lce_suspended);
}

TEST(Radio2HalUtils, convertRilCardStatusToHal_1_2) {
    V1_2::CardStatus cardStatus {};
    EXPECT_FALSE(Radio2HalUtils::convertRilCardStatusToHal(nullptr, 0, cardStatus));

    RIL_CardStatus_V1_2 testVector = {
        .gsm_umts_subscription_app_index = 1,
        .cdma_subscription_app_index = 2,
        .ims_subscription_app_index = 3,
        .num_applications = 1,
    };
    EXPECT_FALSE(Radio2HalUtils::convertRilCardStatusToHal(&testVector, sizeof(testVector), cardStatus));
}

TEST(Radio2HalUtils, convertRilCallToHal_1_2) {
    V1_2::Call call {};
    RIL_Call_V1_2 testVector {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCallToHal(nullptr, sizeof(testVector), call);
    Radio2HalUtils::convertRilCallToHal(&testVector, sizeof(testVector) + 1, call);
}

TEST(Radio2HalUtils, fillCellIdentityResponse_1_2) {
    RIL_CellIdentity_V1_2 testVector[] = {
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            .cellIdentityGsm = {
                450, 5, INT_MAX, INT_MAX, 0, 0xFF, {}
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            .cellIdentityWcdma = {
                450, 5, INT_MAX, INT_MAX, 0, 0, {}
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
            .cellIdentityCdma = {
                INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, {}
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            .cellIdentityLte = {
                450, 5, INT_MAX, 0, INT_MAX, 0, {}, 0
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
            .cellIdentityTdscdma = {
                450, 5, INT_MAX, INT_MAX, INT_MAX, 0, {}
            },
        },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        V1_2::CellIdentity cellIdentity {};
        Radio2HalUtils::fillCellIdentityResponse(cellIdentity, testVector[i]);
        switch ((int)cellIdentity.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            EXPECT_EQ((int)cellIdentity.cellIdentityGsm.size(), 1);
            if (cellIdentity.cellIdentityGsm.size() > 0) {
                testCellIdentityGsm(cellIdentity.cellIdentityGsm[0], testVector[i].cellIdentityGsm);
            }
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            EXPECT_EQ((int)cellIdentity.cellIdentityWcdma.size(), 1);
            if (cellIdentity.cellIdentityWcdma.size() > 0) {
                testCellIdentityWcdma(cellIdentity.cellIdentityWcdma[0], testVector[i].cellIdentityWcdma);
            }
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            EXPECT_EQ((int)cellIdentity.cellIdentityCdma.size(), 1);
            if (cellIdentity.cellIdentityCdma.size() > 0) {
                testCellIdentityCdma(cellIdentity.cellIdentityCdma[0], testVector[i].cellIdentityCdma);
            }
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            EXPECT_EQ((int)cellIdentity.cellIdentityLte.size(), 1);
            if (cellIdentity.cellIdentityLte.size() > 0) {
                testCellIdentityLte(cellIdentity.cellIdentityLte[0], testVector[i].cellIdentityLte);
            }
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            EXPECT_EQ((int)cellIdentity.cellIdentityTdscdma.size(), 1);
            if (cellIdentity.cellIdentityTdscdma.size() > 0) {
                testCellIdentityTdscdma(cellIdentity.cellIdentityTdscdma[0], testVector[i].cellIdentityTdscdma);
            }
            break;
        }
    }
}

TEST(Radio2HalUtils, convertRilDataRegStateToHal_1_2) {
    V1_2::DataRegStateResult result {};
    RIL_DataRegistrationStateResponse_V1_2 testVector {};

    // an invalid parameter test
    Radio2HalUtils::convertRilDataRegStateToHal(nullptr, sizeof(testVector), result);

    // TODO
}

TEST(Radio2HalUtils, convertRilSignalStrengthToHal_1_2) {
    V1_2::SignalStrength ss {};

    // an invalid parameter test
    Radio2HalUtils::convertRilSignalStrengthToHal(nullptr, 0, ss);

    RIL_SignalStrength_V1_2 testVector = {
        .LTE_SignalStrength = {
            30, 20, -100, 0, 0, 0,
        },
    };
    Radio2HalUtils::convertRilSignalStrengthToHal(&testVector, sizeof(testVector), ss);
    EXPECT_EQ(ss.lte.signalStrength, (uint32_t)testVector.LTE_SignalStrength.signalStrength);
    EXPECT_EQ(ss.lte.rsrp, (uint32_t)testVector.LTE_SignalStrength.rsrp);
    EXPECT_EQ(ss.lte.rsrq, (uint32_t)testVector.LTE_SignalStrength.rsrq);
    EXPECT_EQ(ss.lte.rssnr, (int32_t)testVector.LTE_SignalStrength.rssnr);
    EXPECT_EQ(ss.lte.cqi, (uint32_t)testVector.LTE_SignalStrength.cqi);
    EXPECT_EQ(ss.lte.timingAdvance, (uint32_t)testVector.LTE_SignalStrength.timingAdvance);
}

TEST(Radio2HalUtils, convertRilCellInfoGsmToHal_1_2) {
    V1_2::CellInfoGsm cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoGsmToHal(nullptr, cellInfo);

    RIL_CellInfoGsm_V1_2 testVector = {};
    Radio2HalUtils::convertRilCellInfoGsmToHal(&testVector, cellInfo);
    testCellInfoGsm(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoWcdmaToHal_1_2) {
    V1_2::CellInfoWcdma cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoWcdmaToHal(nullptr, cellInfo);

    RIL_CellInfoWcdma_V1_2 testVector = {};
    Radio2HalUtils::convertRilCellInfoWcdmaToHal(&testVector, cellInfo);
    testCellInfoWcdma(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoCdmaToHal_1_2) {
    V1_2::CellInfoCdma cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoCdmaToHal(nullptr, cellInfo);

    RIL_CellInfoCdma_V1_2 testVector = {};
    Radio2HalUtils::convertRilCellInfoCdmaToHal(&testVector, cellInfo);
    testCellInfoCdma(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoLteToHal) {
    V1_2::CellInfoLte cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoLteToHal(nullptr, cellInfo);

    RIL_CellInfoLte_V1_2 testVector = {};
    Radio2HalUtils::convertRilCellInfoLteToHal(&testVector, cellInfo);
    testCellInfoLte(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoTdscdmaToHal_1_2) {
    V1_2::CellInfoTdscdma cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoTdscdmaToHal(nullptr, cellInfo);

    RIL_CellInfoTdscdma_V1_2 testVector = {};
    Radio2HalUtils::convertRilCellInfoTdscdmaToHal(&testVector, cellInfo);
    testCellInfoTdscdma(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoListToHal_1_2) {
    hidl_vec<V1_2::CellInfo> cellinfos;

    RIL_CellInfo_V1_2 testVector[] = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.gsm = {
                { 450, 5, INT_MAX, INT_MAX, 0, 0xFF, {} },
                { 99, 99, INT_MAX },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.wcdma = {
                { 450, 5, INT_MAX, INT_MAX, 0, 0, {} },
                { 99, 99, 255, 255 },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.cdma = {
                { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, {} },
                { INT_MAX, INT_MAX },
                { INT_MAX, INT_MAX, 0 },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.lte = {
                { 450, 5, INT_MAX, 0, INT_MAX, 0, {}, 0 },
                { 99, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.tdscdma = {
                { 450, 5, INT_MAX, INT_MAX, INT_MAX, 0, {} },
                { 99, 99, 255 },
            },
        },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoListToHal(nullptr, sizeof(testVector), cellinfos);

    Radio2HalUtils::convertRilCellInfoListToHal(testVector, sizeof(testVector), cellinfos);
    EXPECT_EQ(cellinfos.size(), size);
    for (size_t i = 0; i < cellinfos.size(); i++) {
        testCellInfo(cellinfos[i], testVector[i]);
    }
}

TEST(Radio2HalUtils, convertRilPhysicalChannelConfigToHal) {
    V1_2::PhysicalChannelConfig pcc {};

    // an invalid parameter test
    Radio2HalUtils::convertRilPhysicalChannelConfigToHal(nullptr, 0, pcc);

    // TODO
}

TEST(Radio2HalUtils, convertRilCardStatusToHal_1_4) {
    V1_4::CardStatus cardStatus {};
    EXPECT_FALSE(Radio2HalUtils::convertRilCardStatusToHal(nullptr, 0, cardStatus));

    RIL_CardStatus_V1_4 testVector = {
        .gsm_umts_subscription_app_index = 1,
        .cdma_subscription_app_index = 2,
        .ims_subscription_app_index = 3,
        .num_applications = 1,
    };
    EXPECT_FALSE(Radio2HalUtils::convertRilCardStatusToHal(&testVector, sizeof(testVector), cardStatus));
}

TEST(Radio2HalUtils, convertRilDataRegStateToHal_1_4) {
    V1_4::DataRegStateResult result {};
    RIL_DataRegistrationStateResponse_V1_4 testVector {};

    // an invalid parameter test
    Radio2HalUtils::convertRilDataRegStateToHal(nullptr, sizeof(testVector), result);
}

TEST(Radio2HalUtils, convertRilDataCallToHal_1_4) {
    V1_4::SetupDataCallResult result {};
    vector<vector<string>> tmp;

    // an invalid parameter test
    Radio2HalUtils::convertRilDataCallToHal(nullptr, result, tmp, 0);

    // TODO
}

TEST(Radio2HalUtils, convertRilDataCallListToHal_1_4) {
    hidl_vec<V1_4::SetupDataCallResult> dcResultList;
    RIL_Data_Call_Response_v11 testVector[] = {
        {},
        {},
        {},
    };
    vector<vector<string>> tmp;

    // an invalid parameter test
    Radio2HalUtils::convertRilDataCallListToHal(nullptr, sizeof(testVector), dcResultList, tmp);

    // TODO
}

TEST(Radio2HalUtils, convertRilSignalStrengthToHal_1_4) {
    V1_4::SignalStrength ss;

    // an invalid parameter test
    Radio2HalUtils::convertRilSignalStrengthToHal(nullptr, 0, ss);

    RIL_SignalStrength_V1_4 testVector = {
        .LTE_SignalStrength = {
            30, 20, -100, 0, 0, 0,
        },
    };
    Radio2HalUtils::convertRilSignalStrengthToHal(&testVector, sizeof(testVector), ss);
    EXPECT_EQ(ss.lte.signalStrength, (uint32_t)testVector.LTE_SignalStrength.signalStrength);
    EXPECT_EQ(ss.lte.rsrp, (uint32_t)testVector.LTE_SignalStrength.rsrp);
    EXPECT_EQ(ss.lte.rsrq, (uint32_t)testVector.LTE_SignalStrength.rsrq);
    EXPECT_EQ(ss.lte.rssnr, (int32_t)testVector.LTE_SignalStrength.rssnr);
    EXPECT_EQ(ss.lte.cqi, (uint32_t)testVector.LTE_SignalStrength.cqi);
    EXPECT_EQ(ss.lte.timingAdvance, (uint32_t)testVector.LTE_SignalStrength.timingAdvance);
}

TEST(Radio2HalUtils, convertRilCellInfoLteToHal_1_4) {
    V1_4::CellInfoLte cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoLteToHal(nullptr, cellInfo);

    RIL_CellInfoLte_V1_4 testVector = {};
    Radio2HalUtils::convertRilCellInfoLteToHal(&testVector, cellInfo);
    testCellInfoLte(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoNrToHal_1_4) {
    V1_4::CellInfoNr cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoNrToHal(nullptr, cellInfo);

    RIL_CellInfoNr_V1_4 testVector = {};
    Radio2HalUtils::convertRilCellInfoNrToHal(&testVector, cellInfo);
    testCellInfoNr(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoListToHal_1_4) {
    hidl_vec<V1_4::CellInfo> cellinfos;
    RIL_CellInfo_V1_4 testVector[] = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.gsm = {
                { 450, 5, INT_MAX, INT_MAX, 0, 0xFF, {} },
                { 99, 99, INT_MAX },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.wcdma = {
                { 450, 5, INT_MAX, INT_MAX, 0, 0, {} },
                { 99, 99, 255, 255 },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.cdma = {
                { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, {} },
                { INT_MAX, INT_MAX },
                { INT_MAX, INT_MAX, 0 },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.lte = {
                {
                    { 450, 5, INT_MAX, 0, INT_MAX, 0, {}, 0 },
                    { 99, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX },
                },
                {}
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.tdscdma = {
                { 450, 5, INT_MAX, INT_MAX, INT_MAX, 0, {} },
                { 99, 99, 255 },
            },
        },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoListToHal(nullptr, sizeof(testVector), cellinfos);

    Radio2HalUtils::convertRilCellInfoListToHal(testVector, sizeof(testVector), cellinfos);
    EXPECT_EQ(cellinfos.size(), size);
    for (size_t i = 0; i < cellinfos.size(); i++) {
        testCellInfo(cellinfos[i], testVector[i]);
    }
}

TEST(Radio2HalUtils, convertRilPhysicalChannelConfigToHal_1_4) {
    V1_4::PhysicalChannelConfig pcc {};

    // an invalid parameter test
    Radio2HalUtils::convertRilPhysicalChannelConfigToHal(nullptr, 0, pcc);

    int testContextIds[2] = { 1, 2 };
    RIL_PhysicalChannelConfig_V1_4 testVector = {
        .len_contextIds = 2,
        .contextIds = testContextIds,
    };
    Radio2HalUtils::convertRilPhysicalChannelConfigToHal(&testVector, sizeof(testVector), pcc);
    EXPECT_EQ((int)pcc.contextIds.size(), testVector.len_contextIds);
    for (size_t i = 0; i < pcc.contextIds.size(); i++) {
        EXPECT_EQ((int)pcc.contextIds[i], testVector.contextIds[i]);
    }
}

TEST(Radio2HalUtils, convertRilEmergencyNumberToHal) {
    V1_4::EmergencyNumber emergencyNumber {};

    // an invalid parameter test
    Radio2HalUtils::convertRilEmergencyNumberToHal(nullptr, 0, emergencyNumber);

    const char *tetUrns[] = { "urns1", "urns2", };
    RIL_EmergencyNumber testVector = {
        .len_urns = 2,
        .urns = (char **)tetUrns,
    };
    Radio2HalUtils::convertRilEmergencyNumberToHal(&testVector, sizeof(testVector), emergencyNumber);
    EXPECT_EQ((int)emergencyNumber.urns.size(), testVector.len_urns);
    for (size_t i = 0; i < emergencyNumber.urns.size(); i++) {
        EXPECT_TRUE(emergencyNumber.urns[i] == testVector.urns[i]);
    }
}

TEST(Radio2HalUtils, convertRilEmergencyNumberListToHal) {
    hidl_vec<V1_4::EmergencyNumber> emergencyNumberList {};

    // an invalid parameter test
    Radio2HalUtils::convertRilEmergencyNumberListToHal(nullptr, 0, emergencyNumberList);

    // TODO
}

TEST(Radio2HalUtils, convertRilDataCallListToHal_1_5) {
    hidl_vec<V1_5::SetupDataCallResult> dcResultList;
    RIL_Data_Call_Response_v11 testVector[] = {
        {},
        {},
        {},
    };
    vector<vector<string>> tmp;

    // an invalid parameter test
    Radio2HalUtils::convertRilDataCallListToHal(nullptr, sizeof(testVector), dcResultList, tmp);

    // TODO
}

TEST(Radio2HalUtils, convertRilBarringInfoToHal_1_5) {
    V1_5::BarringInfo barring {};
    RIL_BarringInfo testVector {};

    // an invalid parameter test
    Radio2HalUtils::convertRilBarringInfoToHal(barring, testVector);

    // TODO
}

TEST(Radio2HalUtils, convertRilBarringInfosToHal_1_5) {
    hidl_vec<V1_5::BarringInfo> barringInfos;

    // an invalid parameter test
    Radio2HalUtils::convertRilBarringInfosToHal(barringInfos, nullptr, 0);

    // TODO
}

TEST(Radio2HalUtils, convertRilRegStateToHal_1_5) {
    V1_5::RegStateResult result {};

    // an invalid parameter test
    Radio2HalUtils::convertRilRegStateToHal(nullptr, 0, result);

    // TODO
}

TEST(Radio2HalUtils, convertRilCsgInfoToHal) {

    RIL_ClosedSubscriberGroupInfo testVector[] = {
        { true, "cgs12345", 1 },
        { true, "cgs12345", 0x7FFFFFF },
        { false, "cgs67890", 1 },
        { false, "cgs67890", 0x7FFFFFF },
        { false, "", INT_MAX },
    };
    size_t size = sizeof(testVector) / sizeof(testVector);

    for (size_t i = 0; i < size; i++) {
        V1_5::ClosedSubscriberGroupInfo halCsgInfo {};
        Radio2HalUtils::convertRilCsgInfoToHal(halCsgInfo, testVector[i]);
        EXPECT_EQ(halCsgInfo.csgIndication, testVector[i].csgIndication);
        EXPECT_TRUE(halCsgInfo.homeNodebName == hidl_string(testVector[i].homeNodebName));
        EXPECT_EQ(halCsgInfo.csgIdentity, testVector[i].csgIdentity);
    }
}

TEST(Radio2HalUtils, convertRilOptionalCsgInfoToHal) {
    RIL_OptionalCsgInfo testVector[] = {
        {{ true, "cgs12345", 1 }},
        {{ true, "cgs12345", 0x7FFFFFF }},
        {{ false, "cgs67890", 1 }},
        {{ false, "cgs67890", 0x7FFFFFF }},
    };
    size_t size = sizeof(testVector) / sizeof(testVector);

    for (size_t i = 0; i < size; i++) {
        V1_5::OptionalCsgInfo halOptionalCsgInfo {};
        Radio2HalUtils::convertRilOptionalCsgInfoToHal(halOptionalCsgInfo, testVector[i]);

        EXPECT_TRUE(halOptionalCsgInfo.getDiscriminator() == V1_5::OptionalCsgInfo::hidl_discriminator::csgInfo);
        const V1_5::ClosedSubscriberGroupInfo& halCsgInfo = halOptionalCsgInfo.csgInfo();
        EXPECT_EQ(halCsgInfo.csgIndication, testVector[i].csgInfo.csgIndication);
        EXPECT_TRUE(halCsgInfo.homeNodebName == hidl_string(testVector[i].csgInfo.homeNodebName));
        EXPECT_EQ(halCsgInfo.csgIdentity, testVector[i].csgInfo.csgIdentity);
    }

    RIL_OptionalCsgInfo testVectorNoinit = {{ false, "", INT_MAX }};
    {
        V1_5::OptionalCsgInfo halOptionalCsgInfo {};
        Radio2HalUtils::convertRilOptionalCsgInfoToHal(halOptionalCsgInfo, testVectorNoinit);
        EXPECT_TRUE(halOptionalCsgInfo.getDiscriminator() == V1_5::OptionalCsgInfo::hidl_discriminator::noinit);
    }
}

TEST(Radio2HalUtils, convertRilCellIdentityToHal_1_5_cell_info_type) {
    struct {
        uint32_t rilCellInfoType;
        V1_5::CellIdentity::hidl_discriminator halCellInfoType;
    } testVector[] = {
        { RIL_CELL_INFO_TYPE_NONE, V1_5::CellIdentity::hidl_discriminator::noinit },
        { RIL_CELL_INFO_TYPE_GSM, V1_5::CellIdentity::hidl_discriminator::gsm },
        { RIL_CELL_INFO_TYPE_WCDMA, V1_5::CellIdentity::hidl_discriminator::wcdma },
        { RIL_CELL_INFO_TYPE_TD_SCDMA, V1_5::CellIdentity::hidl_discriminator::tdscdma },
        { RIL_CELL_INFO_TYPE_CDMA, V1_5::CellIdentity::hidl_discriminator::cdma },
        { RIL_CELL_INFO_TYPE_LTE, V1_5::CellIdentity::hidl_discriminator::lte },
        { RIL_CELL_INFO_TYPE_GSM, V1_5::CellIdentity::hidl_discriminator::nr },
    };
    size_t size = sizeof(testVector) / sizeof(testVector);
    for (size_t i = 0; i < size; i++) {
        RIL_CellIdentity_V1_5 rilCellIdentity = { .cellInfoType = (RIL_CellInfoType)testVector[i].rilCellInfoType,  };
        V1_5::CellIdentity halCellIdentity = {};
        Radio2HalUtils::convertRilCellIdentityToHal(halCellIdentity, rilCellIdentity);
        EXPECT_TRUE(halCellIdentity.getDiscriminator() == testVector[i].halCellInfoType);
    }
}

TEST(Radio2HalUtils, convertRilCellIdentityGsmToHal_1_5) {
    V1_0::CellIdentityGsm cellIdentity {};
    V1_2::CellIdentityGsm cellIdentityV12 {};
    V1_5::CellIdentityGsm cellIdentityV15 {};
    const char *testAdditionalPlmns[] = { "45005", "00101", };
    RIL_CellIdentityGsm_V1_5 testVector = {
        450, 5, 12345, 12345, 1200, 100, { "SK Telecom", "SKT"},
        sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns
    };
    Radio2HalUtils::convertRilCellIdentityGsmToHal(cellIdentityV12, testVector);
    Radio2HalUtils::convertRilCellIdentityGsmToHal(cellIdentityV12, testVector);
    Radio2HalUtils::convertRilCellIdentityGsmToHal(cellIdentityV15, testVector);
    testCellIdentityGsm(cellIdentityV15, testVector);
}

TEST(Radio2HalUtils, convertRilCellIdentityWcdmaToHal_1_5) {
    V1_0::CellIdentityWcdma cellIdentity {};
    V1_2::CellIdentityWcdma cellIdentityV12 {};
    V1_5::CellIdentityWcdma cellIdentityV15 {};
    const char *testAdditionalPlmns[] = { "45005", "00101", };
    RIL_CellIdentityWcdma_V1_5 testVector = {
        450, 5, 12345, 12345, 500, 2300, { "SK Telecom", "SKT"},
        sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
        {},
    };
    Radio2HalUtils::convertRilCellIdentityWcdmaToHal(cellIdentity, testVector);
    Radio2HalUtils::convertRilCellIdentityWcdmaToHal(cellIdentityV12, testVector);
    Radio2HalUtils::convertRilCellIdentityWcdmaToHal(cellIdentityV15, testVector);
    testCellIdentityWcdma(cellIdentityV15, testVector);
}

TEST(Radio2HalUtils, convertRilCellIdentityTdscdmaToHal_1_5) {
    V1_0::CellIdentityTdscdma cellIdentity {};
    V1_2::CellIdentityTdscdma cellIdentityV12 {};
    V1_5::CellIdentityTdscdma cellIdentityV15 {};
    const char *testAdditionalPlmns[] = { "45005", "00101", };
    RIL_CellIdentityTdscdma_V1_5 testVector = {
        450, 5, 12345, 12345, 110, 1500, { "SK Telecom", "SKT"},
        sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
        {},
    };
    Radio2HalUtils::convertRilCellIdentityTdscdmaToHal(cellIdentity, testVector);
    Radio2HalUtils::convertRilCellIdentityTdscdmaToHal(cellIdentityV12, testVector);
    Radio2HalUtils::convertRilCellIdentityTdscdmaToHal(cellIdentityV15, testVector);
    testCellIdentityTdscdma(cellIdentityV15, testVector);
}

TEST(Radio2HalUtils, convertRilCellIdentityCdmaToHal_1_2) {
    V1_0::CellIdentityCdma testV10 {};
    V1_2::CellIdentityCdma testV12 {};
    RIL_CellIdentityCdma_V1_2 testVector = {};
    Radio2HalUtils::convertRilCellIdentityCdmaToHal(testV10, testVector);
    Radio2HalUtils::convertRilCellIdentityCdmaToHal(testV12, testVector);
}

TEST(Radio2HalUtils, convertRilCellIdentityLteToHal_1_5) {
    V1_0::CellIdentityLte cellIdentity {};
    V1_2::CellIdentityLte cellIdentityV12 {};
    V1_5::CellIdentityLte cellIdentityV15 {};
    const char *testAdditionalPlmns[] = { "45005", "00101", };
    RIL_CellIdentityLte_V1_5 testVector = {
        450, 5, 12345, 12345, 110, 1500, { "SK Telecom", "SKT"}, 100,
        sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
        {},
        3, { RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_10, RIL_EutranBands::EUTRAN_BAND_13 },
    };
    Radio2HalUtils::convertRilCellIdentityLteToHal(cellIdentity, testVector);
    Radio2HalUtils::convertRilCellIdentityLteToHal(cellIdentityV12, testVector);
    Radio2HalUtils::convertRilCellIdentityLteToHal(cellIdentityV15, testVector);
    testCellIdentityLte(cellIdentityV15, testVector);
}

TEST(Radio2HalUtils, convertRilCellIdentityNrToHal_1_5) {
    V1_4::CellIdentityNr cellIdentityV14 {};
    V1_5::CellIdentityNr cellIdentityV15 {};
    const char *testAdditionalPlmns[] = { "45005", "00101", };
    RIL_CellIdentityNr_V1_5 testVector = {
        450, 5, 12345, 777, 20000, 2500, { "SK Telecom", "SKT"},
        sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
        3, { RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_12, RIL_NgranBands::NGRAN_BAND_18 },
    };
    Radio2HalUtils::convertRilCellIdentityNrToHal(cellIdentityV14, testVector);
    Radio2HalUtils::convertRilCellIdentityNrToHal(cellIdentityV15, testVector);
    testCellIdentityNr(cellIdentityV15, testVector);
}

TEST(Radio2HalUtils, convertRilCellIdentityToHal_1_5) {
    const char *testAdditionalPlmns[] = { "45005", "00101", };
    RIL_CellIdentity_V1_5 testVector[] = {
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            .gsm = {
                450, 5, 12345, 12345, 1200, 100, { "SK Telecom", "SKT"},
                sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            .wcdma = {
                450, 5, 12345, 12345, 500, 2300, { "SK Telecom", "SKT"},
                sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
                {}
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
            .cdma = {
                INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, {}
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            .lte = {
                450, 5, 12345, 12345, 110, 1500, { "SK Telecom", "SKT"}, 100,
                sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
                {},
                3, { RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_10, RIL_EutranBands::EUTRAN_BAND_13 },
            },
        },
        {
            .cellInfoType = RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
            .tdscdma = {
                450, 5, 12345, 12345, 110, 1500, { "SK Telecom", "SKT"},
                sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
                {},
            },
        },
        {
            .cellInfoType = (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR,
            .nr = {
                450, 5, 12345, 777, 20000, 2500, { "SK Telecom", "SKT"},
                sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
                3, { RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_12, RIL_NgranBands::NGRAN_BAND_18 },
            },
        },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        V1_5::CellIdentity cellIdentity {};
        Radio2HalUtils::convertRilCellIdentityToHal(cellIdentity, testVector[i]);

        testCellIdentityDiscriminator(cellIdentity.getDiscriminator(), (int)testVector[i].cellInfoType);
        switch ((int)cellIdentity.getDiscriminator()) {
        case (int)V1_5::CellIdentity::hidl_discriminator::gsm:
            testCellIdentityGsm(cellIdentity.gsm(), testVector[i].gsm);
            break;
        case (int)V1_5::CellIdentity::hidl_discriminator::wcdma:
            testCellIdentityWcdma(cellIdentity.wcdma(), testVector[i].wcdma);
            break;
        case (int)V1_5::CellIdentity::hidl_discriminator::cdma:
            testCellIdentityCdma(cellIdentity.cdma(), testVector[i].cdma);
            break;
        case (int)V1_5::CellIdentity::hidl_discriminator::lte:
            testCellIdentityLte(cellIdentity.lte(), testVector[i].lte);
            break;
        case (int)V1_5::CellIdentity::hidl_discriminator::tdscdma:
            testCellIdentityTdscdma(cellIdentity.tdscdma(), testVector[i].tdscdma);
            break;
        case (int)V1_5::CellIdentity::hidl_discriminator::nr:
            testCellIdentityNr(cellIdentity.nr(), testVector[i].nr);
            break;
        default:
            break;
        }
    }
}

TEST(Radio2HalUtils, convertRilCellInfoGsmToHal_1_5) {
    V1_5::CellInfoGsm cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoGsmToHal(nullptr, cellInfo);

    RIL_CellInfoGsm_V1_5 testVector = {};
    Radio2HalUtils::convertRilCellInfoGsmToHal(&testVector, cellInfo);
    testCellInfoGsm(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoWcdmaToHal_1_5) {
    V1_5::CellInfoWcdma cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoWcdmaToHal(nullptr, cellInfo);

    RIL_CellInfoWcdma_V1_5 testVector = {};
    Radio2HalUtils::convertRilCellInfoWcdmaToHal(&testVector, cellInfo);
    testCellInfoWcdma(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoLteToHal_1_5) {
    V1_5::CellInfoLte cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoLteToHal(nullptr, cellInfo);

    RIL_CellInfoLte_V1_5 testVector = {};
    Radio2HalUtils::convertRilCellInfoLteToHal(&testVector, cellInfo);
    testCellInfoLte(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoTdscdmaToHal_1_5) {
    V1_5::CellInfoTdscdma cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoTdscdmaToHal(nullptr, cellInfo);

    RIL_CellInfoTdscdma_V1_5 testVector = {};
    Radio2HalUtils::convertRilCellInfoTdscdmaToHal(&testVector, cellInfo);
    testCellInfoTdscdma(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoNrToHal_1_5) {
    V1_5::CellInfoNr cellInfo {};

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoNrToHal(nullptr, cellInfo);

    RIL_CellInfoNr_V1_5 testVector = {};
    Radio2HalUtils::convertRilCellInfoNrToHal(&testVector, cellInfo);
    testCellInfoNr(cellInfo, testVector);
}

TEST(Radio2HalUtils, convertRilCellInfoListToHal_1_5) {
    hidl_vec<V1_5::CellInfo> cellinfos;
    const char *testAdditionalPlmns[] = { "45005", "00101", };
    RIL_CellInfo_V1_5 testVector[] = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.gsm = {
                {
                    450, 5, 12345, 12345, 1200, 100, { "SK Telecom", "SKT"},
                    sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns
                },
                { 99, 99, INT_MAX },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.wcdma = {
                {
                    450, 5, 12345, 12345, 500, 2300, { "SK Telecom", "SKT"},
                    sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
                    {}
                },
                { 99, 99, 255, 255 },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.cdma = {
                { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, {} },
                { INT_MAX, INT_MAX },
                { INT_MAX, INT_MAX, 0 },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.lte = {
                {
                    450, 5, 12345, 12345, 110, 1500, { "SK Telecom", "SKT"}, 100,
                    sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
                    {},
                    3, { RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_10, RIL_EutranBands::EUTRAN_BAND_13 },
                },
                {}, true,
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.tdscdma = {
                {
                    450, 5, 12345, 12345, 110, 1500, { "SK Telecom", "SKT"},
                    sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
                    {},
                },
                { 99, 99, 255 },
            },
        },
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR,
            1, RIL_TimeStampType::RIL_TIMESTAMP_TYPE_OEM_RIL, 0LL,
            .CellInfo.nr = {
                {
                    450, 5, 12345, 777, 20000, 2500, { "SK Telecom", "SKT"},
                    sizeof(testAdditionalPlmns) / sizeof(testAdditionalPlmns[0]), (char **)testAdditionalPlmns,
                    3, { RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_12, RIL_NgranBands::NGRAN_BAND_18 },
                },
                { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX },
            },
        },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    // an invalid parameter test
    Radio2HalUtils::convertRilCellInfoListToHal(nullptr, sizeof(testVector), cellinfos);

    Radio2HalUtils::convertRilCellInfoListToHal(testVector, sizeof(testVector), cellinfos);
    EXPECT_EQ(cellinfos.size(), size);
    for (size_t i = 0; i < cellinfos.size(); i++) {
        testCellInfo(cellinfos[i], testVector[i]);
    }
}

TEST(Radio2HalUtils, convertRilDataCallToHal_1_5) {
    V1_5::SetupDataCallResult result {};
    vector<vector<string>> tmp;

    // an invalid parameter test
    Radio2HalUtils::convertRilDataCallToHal(nullptr, result, tmp, 0);

    // TODO
}

TEST(Radio2HalUtils, convertLinkAddress_1_5) {
    vector<string> container;
    hidl_vec<V1_5::LinkAddress> linkAddresses;
    char str[] = "";

    // an invalid parameter test
    Radio2HalUtils::convertLinkAddress(container, linkAddresses, str);

    // TODO
}

TEST(Radio2HalUtils, convertRilRadioAccessSpecifierToHal_1_5) {
    V1_5::RadioAccessSpecifier ras {};

    // an invalid parameter test
    Radio2HalUtils::convertRilRadioAccessSpecifierToHal(nullptr, 0, ras);

    RIL_RadioAccessSpecifier_V1_5 testVector[] = {
        {
            .radio_access_network = RIL_RadioAccessNetworks::RIL_RAN_GERAN,
            .bands_length = 1, .bands.geran_bands = { RIL_GeranBands::GERAN_BAND_710 },
            .channels_length = 1, .channels = { 1 },
        },
        {
            .radio_access_network = RIL_RadioAccessNetworks::RIL_RAN_UTRAN,
            .bands_length = 1, .bands.utran_bands = { RIL_UtranBands::UTRAN_BAND_19 },
            .channels_length = 1, .channels = { 1 },
        },
        {
            .radio_access_network = RIL_RadioAccessNetworks::RIL_RAN_EUTRAN,
            .bands_length = 1, .bands.eutran_bands = { RIL_EutranBands::EUTRAN_BAND_13 },
            .channels_length = 1, .channels = { 1 },
        },
        {
            .radio_access_network = (RIL_RadioAccessNetworks)RIL_RadioAccessNetworks_V1_5::RIL_RAN_NGRAN,
            .bands_length = 1, .bands.ngran_bands = { RIL_NgranBands::NGRAN_BAND_12 },
            .channels_length = 1, .channels = { 1 },
        },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        bool ret;
        EXPECT_TRUE(ret = Radio2HalUtils::convertRilRadioAccessSpecifierToHal(testVector + i,
                sizeof(RIL_RadioAccessSpecifier_V1_5), ras));

        if (ret) {
            EXPECT_EQ((int)ras.radioAccessNetwork, (int)testVector[i].radio_access_network);
            switch (ras.radioAccessNetwork) {
            case V1_5::RadioAccessNetworks::GERAN:
                EXPECT_EQ((int)ras.bands.geranBands().size(), (int)testVector[i].bands_length);
                for (size_t j = 0; j < ras.bands.geranBands().size(); j++) {
                    EXPECT_EQ((int)ras.bands.geranBands()[j], (int)testVector[i].bands.geran_bands[j]);
                }
                break;
            case V1_5::RadioAccessNetworks::UTRAN:
                EXPECT_EQ((int)ras.bands.utranBands().size(), (int)testVector[i].bands_length);
                for (size_t j = 0; j < ras.bands.utranBands().size(); j++) {
                    EXPECT_EQ((int)ras.bands.utranBands()[j], (int)testVector[i].bands.utran_bands[j]);
                }
                break;
            case V1_5::RadioAccessNetworks::EUTRAN:
                EXPECT_EQ((int)ras.bands.eutranBands().size(), (int)testVector[i].bands_length);
                for (size_t j = 0; j < ras.bands.eutranBands().size(); j++) {
                    EXPECT_EQ((int)ras.bands.eutranBands()[j], (int)testVector[i].bands.eutran_bands[j]);
                }
                break;
            case V1_5::RadioAccessNetworks::NGRAN:
                EXPECT_EQ((int)ras.bands.ngranBands().size(), (int)testVector[i].bands_length);
                for (size_t j = 0; j < ras.bands.ngranBands().size(); j++) {
                    EXPECT_EQ((int)ras.bands.ngranBands()[j], (int)testVector[i].bands.ngran_bands[j]);
                }
                break;
            default:
                break;
            }
            EXPECT_EQ(ras.channels.size(), testVector[i].channels_length);
            for (size_t i = 0; i < ras.channels.size(); i++) {
                EXPECT_EQ((int)ras.channels[i], (int)testVector[i].channels[i]);
            }
        }
    }
}
