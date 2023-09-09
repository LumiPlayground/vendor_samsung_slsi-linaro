/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <testutils.h>
#include <string>

#include "netdatabuilder.h"
#include "networkutils.h"
#include "netdata.h"
#include <librilutils/radio_conv_utils.h>
#include <librilutils/textutils.h>
#include <slsi/radio_v2_0.h>

#include <iostream>
using namespace std;

TEST(NetDataBuilder, NetworkDataBuilder_BuildNetAvailableNetweorkResponse) {
    NetworkDataBuilder testBuilder;
    auto rildataEmptyRsp = testBuilder.BuildNetAvailableNetweorkResponse(0, 0);
    ASSERT_NE(rildataEmptyRsp, nullptr);

    NetworkInfo testVector[] = {
        { "SKTelecom", "SKT", "45005", (char *)"current", 0 },
        { "Testbed", "Testbed", "00101", (char *)"forbidden", 0 }
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    auto rildata = testBuilder.BuildNetAvailableNetweorkResponse(testVector, size);
    EXPECT_NE(rildata, nullptr);
    if (rildata != nullptr) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(char *) * size * 4);
        RilDataStrings *strings = (RilDataStrings *)rildata.get();
        for (size_t i = 0; i < size; i++) {
            EXPECT_TRUE(TextUtils::Equals(testVector[i].longPlmn, strings->GetString(i * 4)));
            EXPECT_TRUE(TextUtils::Equals(testVector[i].shortPlmn, strings->GetString(i * 4 + 1)));
            EXPECT_TRUE(TextUtils::Equals(testVector[i].plmn, strings->GetString(i * 4 + 2)));
            EXPECT_TRUE(TextUtils::Equals(testVector[i].status, strings->GetString(i * 4 + 3)));
        }
    }
}

TEST(NetDataBuilder, CellIdentityBuilder_operator) {

    struct {
        int cellInfoType;
        std::string numeric;
        std::string alphaLong;
        std::string alphaShort;
    } testVector[] = {
        { RIL_CELL_INFO_TYPE_GSM, "45005", "SKT Telecom", "SKT" },
        { RIL_CELL_INFO_TYPE_WCDMA, "45006", "KT", "KT" },
        { RIL_CELL_INFO_TYPE_WCDMA, "311480", "Verizon Wireless", "VZW" },
        { RIL_CELL_INFO_TYPE_LTE, "45005", "", "" },
        { RIL_CELL_INFO_TYPE_NR, "", "", "" },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        CellIdentityBuilder testBuilder(testVector[i].cellInfoType);
        testBuilder.SetCellIdentity(testVector[i].numeric.c_str(),
                testVector[i].alphaLong.c_str(),
                testVector[i].alphaShort.c_str());

        // under HAL 1.2
        RIL_CellIdentity_v16 *ret_old =
                (RIL_CellIdentity_v16 *)testBuilder.cellIdentity(HAL_VERSION_CODE(1, 0));
        ASSERT_NE(ret_old, nullptr);
        EXPECT_EQ(ret_old->cellInfoType, testVector[i].cellInfoType);
        int mcc = INT_MAX;
        int mnc = INT_MAX;
        if (!TextUtils::IsEmpty(testVector[i].numeric)) {
            mcc = NetworkUtils::fetchMcc(testVector[i].numeric.c_str());
            mnc = NetworkUtils::fetchMnc(testVector[i].numeric.c_str());
        }

        switch (testVector[i].cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            EXPECT_EQ(ret_old->cellIdentityGsm.mcc, mcc);
            EXPECT_EQ(ret_old->cellIdentityGsm.mnc, mnc);
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            EXPECT_EQ(ret_old->cellIdentityWcdma.mcc, mcc);
            EXPECT_EQ(ret_old->cellIdentityWcdma.mnc, mnc);
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            EXPECT_EQ(ret_old->cellIdentityLte.mcc, mcc);
            EXPECT_EQ(ret_old->cellIdentityLte.mnc, mnc);
            break;
        case RIL_CELL_INFO_TYPE_NR:
            // no NR
            break;
        } // end switch ~

        // over HAL 1.2
        RIL_CellIdentity_V1_5 *ret_v1_5 =
                (RIL_CellIdentity_V1_5 *)testBuilder.cellIdentity(HAL_VERSION_CODE(1, 5));
        switch (testVector[i].cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            EXPECT_EQ(ret_v1_5->gsm.mcc, mcc);
            EXPECT_EQ(ret_v1_5->gsm.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ret_v1_5->gsm.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ret_v1_5->gsm.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            EXPECT_EQ(ret_v1_5->wcdma.mcc, mcc);
            EXPECT_EQ(ret_v1_5->wcdma.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ret_v1_5->wcdma.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ret_v1_5->wcdma.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            EXPECT_EQ(ret_v1_5->lte.mcc, mcc);
            EXPECT_EQ(ret_v1_5->lte.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ret_v1_5->lte.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ret_v1_5->lte.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_NR:
            EXPECT_EQ(ret_v1_5->nr.mcc, mcc);
            EXPECT_EQ(ret_v1_5->nr.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ret_v1_5->nr.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ret_v1_5->nr.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        } // end switch ~
    }  // end for i ~
}

TEST(NetDataBuilder, VoiceRegStateResultBuilder_operator_hal_legacy) {
    struct {
        int rat;
        std::string numeric;
        std::string alphaLong;
        std::string alphaShort;
    } testVector[] = {
        { RADIO_TECH_EDGE, "45005", "SKT Telecom", "SKT" },
        { RADIO_TECH_UMTS, "45006", "KT", "KT" },
        { RADIO_TECH_LTE, "311480", "Verizon Wireless", "VZW" },
        { RADIO_TECH_NR, "", "", "" },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        VoiceRegStateResultBuilder testBuilder;
        testBuilder.SetRegistrationState(REGISTERED_HOME, testVector[i].rat, 0);
        testBuilder.SetCellIdentity(testVector[i].numeric.c_str(),
                testVector[i].alphaLong.c_str(),
                testVector[i].alphaShort.c_str());

        auto rildata = testBuilder.Build();
        ASSERT_NE(rildata, nullptr);
        ASSERT_NE(rildata->GetData(), nullptr);
        EXPECT_NE(rildata, nullptr);
        EXPECT_NE(rildata->GetData(), nullptr);
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_RegStateResult));

        const RIL_RegStateResult &regResult = *(RIL_RegStateResult *)rildata->GetData();
        const RIL_CellIdentity_V1_5& ci = regResult.cellIdentity;
        int cellInfoType = RadioConvUtils::getCellInfoTypeRadioTechnology(testVector[i].rat);
        EXPECT_EQ(ci.cellInfoType, cellInfoType);

        int mcc = INT_MAX;
        int mnc = INT_MAX;
        if (!TextUtils::IsEmpty(testVector[i].numeric)) {
            mcc = NetworkUtils::fetchMcc(testVector[i].numeric.c_str());
            mnc = NetworkUtils::fetchMnc(testVector[i].numeric.c_str());
        }

        switch (cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            EXPECT_EQ(ci.gsm.mcc, mcc);
            EXPECT_EQ(ci.gsm.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.gsm.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.gsm.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            EXPECT_EQ(ci.wcdma.mcc, mcc);
            EXPECT_EQ(ci.wcdma.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.wcdma.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.wcdma.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            EXPECT_EQ(ci.lte.mcc, mcc);
            EXPECT_EQ(ci.lte.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.lte.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.lte.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_NR:
            EXPECT_EQ(ci.nr.mcc, mcc);
            EXPECT_EQ(ci.nr.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.nr.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.nr.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        } // end switch ~
    }  // end for i ~
}

TEST(NetDataBuilder, VoiceRegStateResultBuilder_operator_hal_v1_6) {
    struct {
        int rat;
        std::string numeric;
        std::string alphaLong;
        std::string alphaShort;
    } testVector[] = {
        { RADIO_TECH_EDGE, "45005", "SKT Telecom", "SKT" },
        { RADIO_TECH_UMTS, "45006", "KT", "KT" },
        { RADIO_TECH_LTE, "311480", "Verizon Wireless", "VZW" },
        { RADIO_TECH_NR, "", "", "" },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        VoiceRegStateResultBuilder testBuilder(HAL_VERSION_CODE(1, 6));
        testBuilder.SetRegistrationState(REGISTERED_HOME, testVector[i].rat, 0);
        testBuilder.SetCellIdentity(testVector[i].numeric.c_str(),
                testVector[i].alphaLong.c_str(),
                testVector[i].alphaShort.c_str());

        auto rildata = testBuilder.Build();
        ASSERT_NE(rildata, nullptr);
        ASSERT_NE(rildata->GetData(), nullptr);
        EXPECT_NE(rildata, nullptr);
        EXPECT_NE(rildata->GetData(), nullptr);
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_RegStateResult_V1_6));

        const RIL_RegStateResult_V1_6 &regResult = *(RIL_RegStateResult_V1_6 *)rildata->GetData();
        const RIL_CellIdentity_V1_5& ci = regResult.cellIdentity;
        int cellInfoType = RadioConvUtils::getCellInfoTypeRadioTechnology(testVector[i].rat);
        EXPECT_EQ(ci.cellInfoType, cellInfoType);

        int mcc = INT_MAX;
        int mnc = INT_MAX;
        if (!TextUtils::IsEmpty(testVector[i].numeric)) {
            mcc = NetworkUtils::fetchMcc(testVector[i].numeric.c_str());
            mnc = NetworkUtils::fetchMnc(testVector[i].numeric.c_str());
        }

        switch (cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            EXPECT_EQ(ci.gsm.mcc, mcc);
            EXPECT_EQ(ci.gsm.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.gsm.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.gsm.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            EXPECT_EQ(ci.wcdma.mcc, mcc);
            EXPECT_EQ(ci.wcdma.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.wcdma.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.wcdma.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            EXPECT_EQ(ci.lte.mcc, mcc);
            EXPECT_EQ(ci.lte.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.lte.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.lte.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_NR:
            EXPECT_EQ(ci.nr.mcc, mcc);
            EXPECT_EQ(ci.nr.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.nr.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.nr.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        } // end switch ~
    }  // end for i ~
}

TEST(NetDataBuilder, DataRegStateResultBuilder_operator_hal_legacy) {
    struct {
        int rat;
        std::string numeric;
        std::string alphaLong;
        std::string alphaShort;
    } testVector[] = {
        { RADIO_TECH_EDGE, "45005", "SKT Telecom", "SKT" },
        { RADIO_TECH_UMTS, "45006", "KT", "KT" },
        { RADIO_TECH_LTE, "311480", "Verizon Wireless", "VZW" },
        { RADIO_TECH_NR, "", "", "" },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        DataRegStateResultBuilder testBuilder;
        testBuilder.SetRegistrationState(REGISTERED_HOME, testVector[i].rat, 0, 0);
        testBuilder.SetCellIdentity(testVector[i].numeric.c_str(),
                testVector[i].alphaLong.c_str(),
                testVector[i].alphaShort.c_str());

        auto rildata = testBuilder.Build();
        ASSERT_NE(rildata, nullptr);
        ASSERT_NE(rildata->GetData(), nullptr);
        EXPECT_NE(rildata, nullptr);
        EXPECT_NE(rildata->GetData(), nullptr);
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_RegStateResult));

        const RIL_RegStateResult &regResult = *(RIL_RegStateResult *)rildata->GetData();
        const RIL_CellIdentity_V1_5& ci = regResult.cellIdentity;
        int cellInfoType = RadioConvUtils::getCellInfoTypeRadioTechnology(testVector[i].rat);
        EXPECT_EQ(ci.cellInfoType, cellInfoType);

        int mcc = INT_MAX;
        int mnc = INT_MAX;
        if (!TextUtils::IsEmpty(testVector[i].numeric)) {
            mcc = NetworkUtils::fetchMcc(testVector[i].numeric.c_str());
            mnc = NetworkUtils::fetchMnc(testVector[i].numeric.c_str());
        }

        switch (cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            EXPECT_EQ(ci.gsm.mcc, mcc);
            EXPECT_EQ(ci.gsm.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.gsm.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.gsm.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            EXPECT_EQ(ci.wcdma.mcc, mcc);
            EXPECT_EQ(ci.wcdma.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.wcdma.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.wcdma.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            EXPECT_EQ(ci.lte.mcc, mcc);
            EXPECT_EQ(ci.lte.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.lte.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.lte.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_NR:
            EXPECT_EQ(ci.nr.mcc, mcc);
            EXPECT_EQ(ci.nr.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.nr.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.nr.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        } // end switch ~
    }  // end for i ~
}

TEST(NetDataBuilder, DataRegStateResultBuilder_operator_hal_v1_6) {
    struct {
        int rat;
        std::string numeric;
        std::string alphaLong;
        std::string alphaShort;
    } testVector[] = {
        { RADIO_TECH_EDGE, "45005", "SKT Telecom", "SKT" },
        { RADIO_TECH_UMTS, "45006", "KT", "KT" },
        { RADIO_TECH_LTE, "311480", "Verizon Wireless", "VZW" },
        { RADIO_TECH_NR, "", "", "" },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        DataRegStateResultBuilder testBuilder(HAL_VERSION_CODE(1, 6));
        testBuilder.SetRegistrationState(REGISTERED_HOME, testVector[i].rat, 0, 0);
        testBuilder.SetCellIdentity(testVector[i].numeric.c_str(),
                testVector[i].alphaLong.c_str(),
                testVector[i].alphaShort.c_str());

        auto rildata = testBuilder.Build();
        ASSERT_NE(rildata, nullptr);
        ASSERT_NE(rildata->GetData(), nullptr);
        EXPECT_NE(rildata, nullptr);
        EXPECT_NE(rildata->GetData(), nullptr);
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_RegStateResult_V1_6));

        const RIL_RegStateResult_V1_6 &regResult = *(RIL_RegStateResult_V1_6 *)rildata->GetData();
        const RIL_CellIdentity_V1_5& ci = regResult.cellIdentity;
        int cellInfoType = RadioConvUtils::getCellInfoTypeRadioTechnology(testVector[i].rat);
        EXPECT_EQ(ci.cellInfoType, cellInfoType);

        int mcc = INT_MAX;
        int mnc = INT_MAX;
        if (!TextUtils::IsEmpty(testVector[i].numeric)) {
            mcc = NetworkUtils::fetchMcc(testVector[i].numeric.c_str());
            mnc = NetworkUtils::fetchMnc(testVector[i].numeric.c_str());
        }

        switch (cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            EXPECT_EQ(ci.gsm.mcc, mcc);
            EXPECT_EQ(ci.gsm.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.gsm.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.gsm.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            EXPECT_EQ(ci.wcdma.mcc, mcc);
            EXPECT_EQ(ci.wcdma.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.wcdma.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.wcdma.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            EXPECT_EQ(ci.lte.mcc, mcc);
            EXPECT_EQ(ci.lte.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.lte.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.lte.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        case RIL_CELL_INFO_TYPE_NR:
            EXPECT_EQ(ci.nr.mcc, mcc);
            EXPECT_EQ(ci.nr.mnc, mnc);
            EXPECT_TRUE(TextUtils::Equals(ci.nr.operatorNames.alphaLong, testVector[i].alphaLong));
            EXPECT_TRUE(TextUtils::Equals(ci.nr.operatorNames.alphaShort, testVector[i].alphaShort));
            break;
        } // end switch ~
    }  // end for i ~
}

TEST(NetDataBuilder, DataRegStateResultBuilder_lte_vops_nrindicator) {
    DataRegStateResultBuilder testBuilder(HAL_VERSION_CODE(1, 6));
    testBuilder.SetCellIdentity(450, 5, "SKTelecom", "SKT");
    testBuilder.SetLteVopsInfo(true, true);
    testBuilder.SetNrIndicators(true, false, true);
    auto rildata = testBuilder.Build();
    if (rildata != nullptr) {
        EXPECT_NE(rildata->GetData(), nullptr);
        const RIL_RegStateResult_V1_6 &regResult = *(RIL_RegStateResult_V1_6 *)rildata->GetData();
        EXPECT_EQ(regResult.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isVopsSupported, true);
        EXPECT_EQ(regResult.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isEmcBearerSupported, true);
        EXPECT_EQ(regResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isEndcAvailable, true);
        EXPECT_EQ(regResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isDcNrRestricted, false);
        EXPECT_EQ(regResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isNrAvailable, true);
    }
}

TEST(NetDataBuilder, DataRegStateResultBuilder_nr_vops) {
    DataRegStateResultBuilder testBuilder(HAL_VERSION_CODE(1, 6));
    testBuilder.SetCellIdentity(450, 5, "SKTelecom", "SKT");
    testBuilder.SetNrVopsInfo(true, true, true);
    auto rildata = testBuilder.Build();
    if (rildata != nullptr) {
        EXPECT_NE(rildata->GetData(), nullptr);
        const RIL_RegStateResult_V1_6 &regResult = *(RIL_RegStateResult_V1_6 *)rildata->GetData();
        EXPECT_EQ(regResult.accessTechnologySpecificInfo.ngranInfo.vopsSupported, true);
        EXPECT_EQ(regResult.accessTechnologySpecificInfo.ngranInfo.emcSupported, true);
        EXPECT_EQ(regResult.accessTechnologySpecificInfo.ngranInfo.emfSupported, true);
    }
}

TEST(NetDataBuilder, PhysicalChannelConfigs_V1_2) {
    int contextIds[] = { 1, 2, 3 };
    PhysicalChannelConfigsData pcc;
    pcc.setCurrentStatusRat(RIL_CellConnectionStatus::PRIMARY_SERVING, RADIO_TECH_LTE);
    pcc.setDownLinkInfo(1400, 40, RF_INFO_TYPE_CHANNEL_NUMBER, 60000);
    pcc.setContextIds(3, contextIds);
    pcc.setUplinkInfo(2000, 20, 19);

    list<PhysicalChannelConfigsData> testVector = { pcc };

    PhysicalChannelConfigsBuilder testBuilder;
    std::unique_ptr<const RilData> rildata(testBuilder.Build(testVector));
    ASSERT_NE(rildata.get(), nullptr);
    EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_PhysicalChannelConfig) * testVector.size());
    const RIL_PhysicalChannelConfig *testPcc = (const RIL_PhysicalChannelConfig *)rildata->GetData();
    EXPECT_EQ((int)testPcc->status, pcc.getStatus());
    EXPECT_EQ(testPcc->cellBandwidthDownlink, pcc.getCellBandwidthDownlink());
}

TEST(NetDataBuilder, PhysicalChannelConfigs_V1_4) {
    int contextIds[] = { 1, 2, 3 };
    PhysicalChannelConfigsData pcc;
    pcc.setCurrentStatusRat(RIL_CellConnectionStatus::PRIMARY_SERVING, RADIO_TECH_LTE);
    pcc.setDownLinkInfo(1400, 40, RF_INFO_TYPE_CHANNEL_NUMBER, 60000);
    pcc.setContextIds(3, contextIds);
    pcc.setUplinkInfo(2000, 20, 19);

    list<PhysicalChannelConfigsData> testVector = { pcc };

    PhysicalChannelConfigsBuilder testBuilder(HAL_VERSION_CODE(1, 4));
    std::unique_ptr<const RilData> rildata(testBuilder.Build(testVector));
    ASSERT_NE(rildata.get(), nullptr);
    EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_PhysicalChannelConfig_V1_4) * testVector.size());
    const RIL_PhysicalChannelConfig_V1_4 *testPcc = (const RIL_PhysicalChannelConfig_V1_4 *)rildata->GetData();
    EXPECT_EQ((int)testPcc->status, pcc.getStatus());
    EXPECT_EQ(testPcc->cellBandwidthDownlink, pcc.getCellBandwidthDownlink());
    EXPECT_EQ(testPcc->rat, (int)pcc.getRat());
    EXPECT_EQ(testPcc->rfInfo.channelNumber, pcc.getChannelNumber());
    EXPECT_EQ(testPcc->len_contextIds, pcc.getLenContextIds());
    EXPECT_TRUE(memcmp(testPcc->contextIds, pcc.getContextIds(), sizeof(contextIds)) == 0);
    EXPECT_EQ(testPcc->physicalCellId, (unsigned int)/* NEED TO CHECK */ pcc.getPhysicalCellId());
}

TEST(NetDataBuilder, PhysicalChannelConfigs_V1_6) {
    int contextIds[] = { 1, 2, 3 };
    PhysicalChannelConfigsData pcc;
    pcc.setCurrentStatusRat(RIL_CellConnectionStatus::PRIMARY_SERVING, RADIO_TECH_LTE);
    pcc.setDownLinkInfo(1400, 40, RF_INFO_TYPE_CHANNEL_NUMBER, 60000);
    pcc.setContextIds(3, contextIds);
    pcc.setUplinkInfo(2000, 20, 19);

    list<PhysicalChannelConfigsData> testVector = { pcc };

    PhysicalChannelConfigsBuilder testBuilder(HAL_VERSION_CODE(1, 6));
    std::unique_ptr<const RilData> rildata(testBuilder.Build(testVector));
    ASSERT_NE(rildata.get(), nullptr);
    EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_PhysicalChannelConfig_V1_6) * testVector.size());
    const RIL_PhysicalChannelConfig_V1_6 *testPcc = (const RIL_PhysicalChannelConfig_V1_6 *)rildata->GetData();
    EXPECT_EQ((int)testPcc->status, pcc.getStatus());
    EXPECT_EQ(testPcc->rat, (int)pcc.getRat());
    EXPECT_EQ(testPcc->downlinkChannelNumber, pcc.getChannelNumber());
    EXPECT_EQ(testPcc->downlinkChannelNumber, pcc.getChannelNumber());
    EXPECT_EQ(testPcc->uplinkChannelNumber, pcc.getUplinkChannelNumber());
    EXPECT_EQ(testPcc->cellBandwidthDownlink, pcc.getCellBandwidthDownlink());
    EXPECT_EQ(testPcc->cellBandwidthUplink, pcc.getCellBandwidthUplink());
    EXPECT_TRUE(memcmp(testPcc->contextIds, pcc.getContextIds(), sizeof(contextIds)) == 0);
    EXPECT_EQ(testPcc->physicalCellId, (unsigned int)/* NEED TO CHECK */ pcc.getPhysicalCellId());
}

TEST(NetDataBuilder, RestrictedStateBuilder) {
    RestrictedStateBuilder testBuilder;

    const RilData *result = nullptr;
    // RIL_RESTRICTED_STATE_NONE
    // null input, no 3G, no barring infos and no barred at all
    result = testBuilder.Build(nullptr);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_NONE);
        delete result;
        result = nullptr;
    }

    RIL_CellIdAndBarringInfo testVector1 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_LTE, }, 0, 0
    };
    result = testBuilder.Build(&testVector1);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_NONE);
        delete result;
        result = nullptr;
    }

    RIL_CellIdAndBarringInfo testVector2 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 0, 0
    };
    result = testBuilder.Build(&testVector2);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_NONE);
        delete result;
        result = nullptr;
    }

    RIL_CellIdAndBarringInfo testVector3 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 1, 0
    };
    result = testBuilder.Build(&testVector3);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_NONE);
        delete result;
        result = nullptr;
    }

    RIL_CellIdAndBarringInfo testVector4 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 1,
        new RIL_BarringInfo[1] {
            { CS_SERVICE, BARRING_TYPE_NONE, {{}} },
        },
    };
    result = testBuilder.Build(&testVector4);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_NONE);
        delete result;
        result = nullptr;
        delete testVector4.precords;
        testVector4.precords = nullptr;
    }

    // RIL_RESTRICTED_STATE_CS_EMERGENCY
    RIL_CellIdAndBarringInfo testVector5 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 1,
        new RIL_BarringInfo[1] {
            { EMERGENCY, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
        },
    };
    result = testBuilder.Build(&testVector5);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_CS_EMERGENCY);
        delete result;
        result = nullptr;
    }

    if (testVector5.precords) {
        delete testVector5.precords;
        testVector5.precords = nullptr;
    }

    // RIL_RESTRICTED_STATE_CS_NORMAL
    RIL_CellIdAndBarringInfo testVector6 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 1,
        new RIL_BarringInfo[1] {
            { CS_SERVICE, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
        },
    };
    result = testBuilder.Build(&testVector6);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_CS_NORMAL);
        delete result;
        result = nullptr;
    }

    if (testVector6.precords) {
        delete testVector6.precords;
        testVector6.precords = nullptr;
    }

    // RIL_RESTRICTED_STATE_CS_ALL
    RIL_CellIdAndBarringInfo testVector7 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 2,
        new RIL_BarringInfo[2] {
            { EMERGENCY, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
            { CS_SERVICE, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
        },
    };
    result = testBuilder.Build(&testVector7);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_CS_ALL);
        delete result;
        result = nullptr;
    }

    if (testVector7.precords) {
        delete testVector7.precords;
        testVector7.precords = nullptr;
    }

    // RIL_RESTRICTED_STATE_PS_ALL
    RIL_CellIdAndBarringInfo testVector8 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 1,
        new RIL_BarringInfo[1] {
            { PS_SERVICE, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
        },
    };
    result = testBuilder.Build(&testVector8);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_PS_ALL);
        delete result;
        result = nullptr;
    }

    if (testVector8.precords) {
        delete testVector8.precords;
        testVector8.precords = nullptr;
    }

    // combinations
    RIL_CellIdAndBarringInfo testVector9 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 2,
        new RIL_BarringInfo[2] {
            { EMERGENCY, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
            { PS_SERVICE, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
        },
    };
    result = testBuilder.Build(&testVector9);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(),
                (RIL_RESTRICTED_STATE_CS_EMERGENCY | RIL_RESTRICTED_STATE_PS_ALL));
        delete result;
        result = nullptr;
    }

    if (testVector9.precords) {
        delete testVector9.precords;
        testVector9.precords = nullptr;
    }

    RIL_CellIdAndBarringInfo testVector10 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 2,
        new RIL_BarringInfo[2] {
            { CS_SERVICE, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
            { PS_SERVICE, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
        },
    };
    result = testBuilder.Build(&testVector10);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(),
                (RIL_RESTRICTED_STATE_CS_NORMAL | RIL_RESTRICTED_STATE_PS_ALL));
        delete result;
        result = nullptr;
    }

    if (testVector10.precords) {
        delete testVector10.precords;
        testVector10.precords = nullptr;
    }

    RIL_CellIdAndBarringInfo testVector11 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 3,
        new RIL_BarringInfo[3] {
            { EMERGENCY, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
            { CS_SERVICE, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
            { PS_SERVICE, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
        },
    };
    result = testBuilder.Build(&testVector11);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(),
                (RIL_RESTRICTED_STATE_CS_ALL | RIL_RESTRICTED_STATE_PS_ALL));
        delete result;
        result = nullptr;
    }

    if (testVector11.precords) {
        delete testVector11.precords;
        testVector11.precords = nullptr;
    }

    // RIL_RESTRICTED_STATE_NONE
    // SMS, MO_DATA
    RIL_CellIdAndBarringInfo testVector12 = {
        { .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA, }, 2,
        new RIL_BarringInfo[2] {
            { SMS, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
            { MO_DATA, BARRING_TYPE_UNCONDITIONAL, {{0, 0, true}} },
        },
    };
    result = testBuilder.Build(&testVector12);
    if (result) {
        EXPECT_EQ(result->GetDataLength(), sizeof(int32_t));
        EXPECT_EQ(*(int32_t *)result->GetData(), RIL_RESTRICTED_STATE_NONE);
        delete result;
        result = nullptr;
    }

    if (testVector12.precords) {
        delete testVector12.precords;
        testVector12.precords = nullptr;
    }
}

TEST(NetDataBuilder, ActivityStatsInfoBuilder_2_0) {
    namespace V2_0 = android::hardware::radio::impl::modem::V2_0;
    V2_0::RIL_ActivityStatsInfo testVector = {
        10, // sleepModeTimeMs
        20, // idleModeTimeMs
        // techSpecificInfo
        {
            // GERAN
            { RIL_RAN_GERAN, FREQUENCY_RANGE_UNKNOWN, { 1, 2, 3, 4, 5 }, 1 },
            // UTRAN
            { RIL_RAN_UTRAN, FREQUENCY_RANGE_UNKNOWN, { 1, 2, 3, 4, 5 }, 2 },
            // EUTRAN
            { RIL_RAN_EUTRAN, FREQUENCY_RANGE_UNKNOWN, { 1, 2, 3, 4, 5 }, 3 },
            // NGRAN - sub6
            { (RIL_RadioAccessNetworks)RIL_RAN_NGRAN, FREQUENCY_RANGE_HIGH, { 1, 2, 3, 4, 5 }, 4 },
            // NGRAN - mmWave
            { (RIL_RadioAccessNetworks)RIL_RAN_NGRAN, FREQUENCY_RANGE_MMWAVE, { 1, 2, 3, 4, 5 }, 5 },
        },
    };

    ActivityStatsInfoBuilder test(HAL_VERSION_CODE(2, 0));
    auto rildata = std::unique_ptr<const RilData>(test.build(testVector));
    ASSERT_TRUE(rildata);
    ASSERT_TRUE(rildata->GetData());
    EXPECT_EQ(rildata->GetDataLength(), sizeof(V2_0::RIL_ActivityStatsInfo));
    EXPECT_TRUE(*(V2_0::RIL_ActivityStatsInfo *)rildata->GetData() == testVector);
}

TEST(NetDataBuilder, ActivityStatsInfoBuilder_legacy) {
    namespace V2_0 = android::hardware::radio::impl::modem::V2_0;
    V2_0::RIL_ActivityStatsInfo testVector = {
        10, // sleepModeTimeMs
        20, // idleModeTimeMs
        // techSpecificInfo
        {
            // GERAN
            { RIL_RAN_GERAN, FREQUENCY_RANGE_UNKNOWN, { 1, 2, 3, 4, 5 }, 1 },
            // UTRAN
            { RIL_RAN_UTRAN, FREQUENCY_RANGE_UNKNOWN, { 1, 2, 3, 4, 5 }, 2 },
            // EUTRAN
            { RIL_RAN_EUTRAN, FREQUENCY_RANGE_UNKNOWN, { 1, 2, 3, 4, 5 }, 3 },
            // NGRAN - sub6
            { (RIL_RadioAccessNetworks)RIL_RAN_NGRAN, FREQUENCY_RANGE_HIGH, { 1, 2, 3, 4, 5 }, 4 },
            // NGRAN - mmWave
            { (RIL_RadioAccessNetworks)RIL_RAN_NGRAN, FREQUENCY_RANGE_MMWAVE, { 1, 2, 3, 4, 5 }, 5 },
        },
    };

    RIL_ActivityStatsInfo expected {};
    expected.sleep_mode_time_ms = testVector.sleepModeTimeMs;
    expected.idle_mode_time_ms = testVector.idleModeTimeMs;
    for (auto& t : testVector.techSpecificInfo) {
        for (size_t i = 0; i < t.txmModetimeMs.size(); i++) {
            expected.tx_mode_time_ms[i] += t.txmModetimeMs[i];
        }
        expected.rx_mode_time_ms += t.rxModeTimeMs;
    }

    auto comp = [](const RIL_ActivityStatsInfo& lhs, const RIL_ActivityStatsInfo& rhs) {
        return (lhs.sleep_mode_time_ms == rhs.sleep_mode_time_ms) &&
               (lhs.idle_mode_time_ms == rhs.idle_mode_time_ms) &&
               (memcmp(lhs.tx_mode_time_ms, rhs.tx_mode_time_ms, sizeof(lhs.tx_mode_time_ms)) == 0) &&
               (lhs.rx_mode_time_ms == rhs.rx_mode_time_ms);
    };

    ActivityStatsInfoBuilder test;
    auto rildata = std::unique_ptr<const RilData>(test.build(testVector));
    ASSERT_TRUE(rildata);
    ASSERT_TRUE(rildata->GetData());
    EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_ActivityStatsInfo));
    EXPECT_TRUE(comp(*(RIL_ActivityStatsInfo *)rildata->GetData(), expected));
}

TEST(NetDataBuilder, PhysicalChannelConfigsBuilder_2_0) {
    static int contextIds1[] = { 0 };
    static int contextIds2[] = { 0, 1 };
    static RIL_PhysicalChannelConfig_V1_6 testVector[] = {
        {
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            RADIO_TECH_LTE, 1000, 1100, 40, 20,
            NUM_ELEMS(contextIds1), contextIds1,
            3000, { .eutranBand = RIL_EutranBands::EUTRAN_BAND_10 }
        },
        {
            RIL_CellConnectionStatus::SECONDARY_SERVING,
            RADIO_TECH_LTE, 2000, 2100, 80, 40,
            NUM_ELEMS(contextIds2), contextIds2,
            5000, { .eutranBand = RIL_EutranBands::EUTRAN_BAND_3 }
        },
    };
    size_t size = NUM_ELEMS(testVector);
    auto gen = [](const RIL_PhysicalChannelConfig_V1_6& pcc) {
        PhysicalChannelConfigsData ret {};
        ret.setCurrentStatusRat(pcc.status, pcc.rat);
        ret.setDownLinkInfo(pcc.downlinkChannelNumber, pcc.cellBandwidthDownlink,
                RF_INFO_TYPE_CHANNEL_NUMBER, pcc.physicalCellId);
        ret.setContextIds(pcc.len_contextIds, pcc.contextIds);
        ret.setUplinkInfo(pcc.uplinkChannelNumber, pcc.cellBandwidthUplink, pcc.band.eutranBand);
        return ret;
    };
    std::list<PhysicalChannelConfigsData> pccList;
    for (size_t i = 0; i < size; i++) {
        pccList.push_back(gen(testVector[i]));
    }
    int halVer = HAL_VERSION_CODE(2, 0);
    PhysicalChannelConfigsBuilder test(halVer);
    auto rildata = std::unique_ptr<const RilData>(test.Build(pccList));
    ASSERT_TRUE(rildata);
    ASSERT_EQ(rildata->GetDataLength(), sizeof(testVector));

    auto compare = [](const RIL_PhysicalChannelConfig_V1_6& l, const RIL_PhysicalChannelConfig_V1_6& r) {
        std::vector<int32_t> llist;
        std::vector<int32_t> rlist;
        for (int i = 0; l.contextIds && i < l.len_contextIds; i++) {
            llist.push_back(l.contextIds[i]);
        }
        for (int i = 0; r.contextIds && i < r.len_contextIds; i++) {
            rlist.push_back(r.contextIds[i]);
        }
        return std::tie(l.status, l.rat, l.downlinkChannelNumber, l.uplinkChannelNumber,
                        l.cellBandwidthDownlink, l.cellBandwidthUplink, l.physicalCellId, llist) ==
               std::tie(r.status, r.rat, r.downlinkChannelNumber, r.uplinkChannelNumber,
                        r.cellBandwidthDownlink, r.cellBandwidthUplink, r.physicalCellId, rlist);
        return true;
    };
    for (size_t i = 0; i < size; i++) {
        RIL_PhysicalChannelConfig_V1_6& ret =
                    (static_cast<RIL_PhysicalChannelConfig_V1_6 *>(rildata->GetData()))[i];
        EXPECT_TRUE(compare(ret, testVector[i]));
    }
}

TEST(NetDataBuilder, PhysicalChannelConfigsBuilder_1_6) {
    static int contextIds1[] = { 0 };
    static int contextIds2[] = { 0, 1 };
    static RIL_PhysicalChannelConfig_V1_6 testVector[] = {
        {
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            RADIO_TECH_LTE, 1000, 1100, 40, 20,
            NUM_ELEMS(contextIds1), contextIds1,
            3000, { .eutranBand = RIL_EutranBands::EUTRAN_BAND_10 }
        },
        {
            RIL_CellConnectionStatus::SECONDARY_SERVING,
            RADIO_TECH_LTE, 2000, 2100, 80, 40,
            NUM_ELEMS(contextIds2), contextIds2,
            5000, { .eutranBand = RIL_EutranBands::EUTRAN_BAND_3 }
        },
    };
    size_t size = NUM_ELEMS(testVector);
    auto gen = [](const RIL_PhysicalChannelConfig_V1_6& pcc) {
        PhysicalChannelConfigsData ret {};
        ret.setCurrentStatusRat(pcc.status, pcc.rat);
        ret.setDownLinkInfo(pcc.downlinkChannelNumber, pcc.cellBandwidthDownlink,
                RF_INFO_TYPE_CHANNEL_NUMBER, pcc.physicalCellId);
        ret.setContextIds(pcc.len_contextIds, pcc.contextIds);
        ret.setUplinkInfo(pcc.uplinkChannelNumber, pcc.cellBandwidthUplink, pcc.band.eutranBand);
        return ret;
    };
    std::list<PhysicalChannelConfigsData> pccList;
    for (size_t i = 0; i < size; i++) {
        pccList.push_back(gen(testVector[i]));
    }
    int halVer = HAL_VERSION_CODE(1, 6);
    PhysicalChannelConfigsBuilder test(halVer);
    auto rildata = std::unique_ptr<const RilData>(test.Build(pccList));
    ASSERT_TRUE(rildata);
    ASSERT_EQ(rildata->GetDataLength(), sizeof(testVector));

    auto compare = [](const RIL_PhysicalChannelConfig_V1_6& l, const RIL_PhysicalChannelConfig_V1_6& r) {
        std::vector<int32_t> llist;
        std::vector<int32_t> rlist;
        for (int i = 0; l.contextIds && i < l.len_contextIds; i++) {
            llist.push_back(l.contextIds[i]);
        }
        for (int i = 0; r.contextIds && i < r.len_contextIds; i++) {
            rlist.push_back(r.contextIds[i]);
        }
        return std::tie(l.status, l.rat, l.downlinkChannelNumber, l.uplinkChannelNumber,
                        l.cellBandwidthDownlink, l.cellBandwidthUplink, l.physicalCellId, llist) ==
               std::tie(r.status, r.rat, r.downlinkChannelNumber, r.uplinkChannelNumber,
                        r.cellBandwidthDownlink, r.cellBandwidthUplink, r.physicalCellId, rlist);
        return true;
    };
    for (size_t i = 0; i < size; i++) {
        RIL_PhysicalChannelConfig_V1_6& ret =
                    (static_cast<RIL_PhysicalChannelConfig_V1_6 *>(rildata->GetData()))[i];
        EXPECT_TRUE(compare(ret, testVector[i]));
    }
}

TEST(NetDataBuilder, PhysicalChannelConfigsBuilder_1_5) {
    static int contextIds1[] = { 0 };
    static int contextIds2[] = { 0, 1 };
    static RIL_PhysicalChannelConfig_V1_6 testVector[] = {
        {
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            RADIO_TECH_LTE, 1000, 1100, 40, 20,
            NUM_ELEMS(contextIds1), contextIds1,
            3000, { .eutranBand = RIL_EutranBands::EUTRAN_BAND_10 }
        },
        {
            RIL_CellConnectionStatus::SECONDARY_SERVING,
            RADIO_TECH_LTE, 2000, 2100, 80, 40,
            NUM_ELEMS(contextIds2), contextIds2,
            5000, { .eutranBand = RIL_EutranBands::EUTRAN_BAND_3 }
        },
    };
    size_t size = NUM_ELEMS(testVector);
    auto gen = [](const RIL_PhysicalChannelConfig_V1_6& pcc) {
        PhysicalChannelConfigsData ret {};
        ret.setCurrentStatusRat(pcc.status, pcc.rat);
        ret.setDownLinkInfo(pcc.downlinkChannelNumber, pcc.cellBandwidthDownlink,
                RF_INFO_TYPE_CHANNEL_NUMBER, pcc.physicalCellId);
        ret.setContextIds(pcc.len_contextIds, pcc.contextIds);
        ret.setUplinkInfo(pcc.uplinkChannelNumber, pcc.cellBandwidthUplink, pcc.band.eutranBand);
        return ret;
    };
    std::list<PhysicalChannelConfigsData> pccList;
    for (size_t i = 0; i < size; i++) {
        pccList.push_back(gen(testVector[i]));
    }
    int halVer = HAL_VERSION_CODE(1, 5);
    PhysicalChannelConfigsBuilder test(halVer);
    auto rildata = std::unique_ptr<const RilData>(test.Build(pccList));
    ASSERT_TRUE(rildata);
    ASSERT_EQ(rildata->GetDataLength(), sizeof(RIL_PhysicalChannelConfig_V1_4) * size);

    auto compare = [](const RIL_PhysicalChannelConfig_V1_4& l, const RIL_PhysicalChannelConfig_V1_6& r) {
        std::vector<int32_t> llist;
        std::vector<int32_t> rlist;
        for (int i = 0; l.contextIds && i < l.len_contextIds; i++) {
            llist.push_back(l.contextIds[i]);
        }
        for (int i = 0; r.contextIds && i < r.len_contextIds; i++) {
            rlist.push_back(r.contextIds[i]);
        }
        return std::tie(l.status, l.rat, l.cellBandwidthDownlink, l.physicalCellId, llist) ==
               std::tie(r.status, r.rat, r.cellBandwidthDownlink, r.physicalCellId, rlist);
        return true;
    };
    for (size_t i = 0; i < size; i++) {
        RIL_PhysicalChannelConfig_V1_4& ret =
                    (static_cast<RIL_PhysicalChannelConfig_V1_4 *>(rildata->GetData()))[i];
        EXPECT_TRUE(compare(ret, testVector[i]));
    }
}

TEST(NetDataBuilder, NetworkScanResultBuilder_2_0) {
    static RIL_CellInfo_V1_6 cellInfos[] = {
        {}, {}
    };
    static RIL_NetworkScanResult_V1_6 testVector[] = {
        { RIL_ScanStatus::PARTIAL, 1, cellInfos, RIL_E_SUCCESS },
        { RIL_ScanStatus::COMPLETE, 2, cellInfos, RIL_E_SUCCESS },
    };
    static size_t size = NUM_ELEMS(testVector);
    int halVer = HAL_VERSION_CODE(2, 0);
    for (size_t i = 0; i < size; i++) {
        NetworkScanResultBuilder test(halVer);
        std::list<RIL_CellInfo_V1_6> list;
        for (size_t j = 0; j < testVector[i].network_infos_length; j++) {
            list.push_back(testVector[i].network_infos[j]);
        }
        auto rildata = std::unique_ptr<const RilData>(test.Build(testVector[i].status,
                                testVector[i].error, list));
        ASSERT_TRUE(rildata);
        ASSERT_NE(rildata->GetData(), nullptr);
        ASSERT_EQ(rildata->GetDataLength(), sizeof(RIL_NetworkScanResult_V1_6));
        RIL_NetworkScanResult_V1_6& ret = *(static_cast<RIL_NetworkScanResult_V1_6 *>(rildata->GetData()));
        EXPECT_EQ(ret.status, testVector[i].status);
        EXPECT_EQ(ret.error, testVector[i].error);
        EXPECT_EQ(ret.network_infos_length, testVector[i].network_infos_length);
    }
}

TEST(NetDataBuilder, NetworkScanResultBuilder_1_6) {
    static RIL_CellInfo_V1_6 cellInfos[] = {
        {}, {}
    };
    static RIL_NetworkScanResult_V1_6 testVector[] = {
        { RIL_ScanStatus::PARTIAL, 1, cellInfos, RIL_E_SUCCESS },
        { RIL_ScanStatus::COMPLETE, 2, cellInfos, RIL_E_SUCCESS },
    };
    static size_t size = NUM_ELEMS(testVector);
    int halVer = HAL_VERSION_CODE(1, 6);
    for (size_t i = 0; i < size; i++) {
        NetworkScanResultBuilder test(halVer);
        std::list<RIL_CellInfo_V1_6> list;
        for (size_t j = 0; j < testVector[i].network_infos_length; j++) {
            list.push_back(testVector[i].network_infos[j]);
        }
        auto rildata = std::unique_ptr<const RilData>(test.Build(testVector[i].status,
                                testVector[i].error, list));
        ASSERT_TRUE(rildata);
        ASSERT_NE(rildata->GetData(), nullptr);
        ASSERT_EQ(rildata->GetDataLength(), sizeof(RIL_NetworkScanResult_V1_6));
        RIL_NetworkScanResult_V1_6& ret = *(static_cast<RIL_NetworkScanResult_V1_6 *>(rildata->GetData()));
        EXPECT_EQ(ret.status, testVector[i].status);
        EXPECT_EQ(ret.error, testVector[i].error);
        EXPECT_EQ(ret.network_infos_length, testVector[i].network_infos_length);
    }
}

TEST(NetDataBuilder, NetworkScanResultBuilder_1_5) {
    static RIL_CellInfo_V1_6 cellInfos[] = {
        {}, {}
    };
    static RIL_NetworkScanResult_V1_6 testVector[] = {
        { RIL_ScanStatus::PARTIAL, 1, cellInfos, RIL_E_SUCCESS },
        { RIL_ScanStatus::COMPLETE, 2, cellInfos, RIL_E_SUCCESS },
    };
    static size_t size = NUM_ELEMS(testVector);
    int halVer = HAL_VERSION_CODE(1, 5);
    for (size_t i = 0; i < size; i++) {
        NetworkScanResultBuilder test(halVer);
        std::list<RIL_CellInfo_V1_6> list;
        for (size_t j = 0; j < testVector[i].network_infos_length; j++) {
            list.push_back(testVector[i].network_infos[j]);
        }
        auto rildata = std::unique_ptr<const RilData>(test.Build(testVector[i].status,
                                testVector[i].error, list));
        ASSERT_TRUE(rildata);
        ASSERT_NE(rildata->GetData(), nullptr);
        ASSERT_EQ(rildata->GetDataLength(), sizeof(RIL_NetworkScanResult_V1_5));
        RIL_NetworkScanResult_V1_5& ret = *(static_cast<RIL_NetworkScanResult_V1_5 *>(rildata->GetData()));
        EXPECT_EQ(ret.status, testVector[i].status);
        EXPECT_EQ(ret.error, testVector[i].error);
        EXPECT_EQ(ret.network_infos_length, testVector[i].network_infos_length);
    }
}

TEST(NetDataBuilder, CellInfoListBuilder_1_5) {
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
    CellInfoListBuilder test(HAL_VERSION_CODE(1,5));
    auto rildata = std::unique_ptr<const RilData>(test.Build(testVector));
    ASSERT_TRUE(rildata);
    ASSERT_TRUE(rildata->GetDataLength() == tmpTestVector.size() * sizeof(RIL_CellInfo_V1_5));
    size_t i = 0;
    for (auto& info : tmpTestVector) {
        auto& cur = *((RIL_CellInfo_V1_5 *)rildata->GetData() + i++);
        EXPECT_TRUE(cur == info);
    }
}

TEST(NetDataBuilder, CellInfoListBuilder_1_6) {
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
    CellInfoListBuilder test(HAL_VERSION_CODE(1,6));
    auto rildata = std::unique_ptr<const RilData>(test.Build(testVector));
    ASSERT_TRUE(rildata);
    ASSERT_TRUE(rildata->GetDataLength() == testVector.size() * sizeof(RIL_CellInfo_V1_6));
    size_t i = 0;
    for (auto& info : testVector) {
        auto& cur = *((RIL_CellInfo_V1_6 *)rildata->GetData() + i++);
        EXPECT_TRUE(cur == info);
    }
}

TEST(NetDataBuilder, CellInfoListBuilder_2_0) {
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
    CellInfoListBuilder test(HAL_VERSION_CODE(2,0));
    auto rildata = std::unique_ptr<const RilData>(test.Build(testVector));
    ASSERT_TRUE(rildata);
    ASSERT_TRUE(rildata->GetDataLength() == testVector.size() * sizeof(RIL_CellInfo_V1_6));
    size_t i = 0;
    for (auto& info : testVector) {
        auto& cur = *((RIL_CellInfo_V1_6 *)rildata->GetData() + i++);
        EXPECT_TRUE(cur == info);
    }
}