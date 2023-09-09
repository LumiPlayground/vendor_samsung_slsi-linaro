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
 * cellinfodecorator_test.cpp
 *
 *  Created on: 2021. 10. 5.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "cellinfodecorator.h"
#include "eonsResolver.h"
#include "operatortable.h"
#include "test_api.h"
#include <telephony/ril_mnc.h>
#include <list>
#include <string>

TEST(CellInfoDecorator, SignalStrengthVisibleDecorator) {
    RIL_CellInfo_V1_6 test = {
        .cellInfoType = RIL_CELL_INFO_TYPE_LTE,
        .CellInfo.lte = {},
    };
    CellInfoBase base(&test);
    SignalStrengthVisibleDecorator visible(base);
    visible.decorate();
    EXPECT_EQ(test.CellInfo.lte.signalStrengthLte.signalStrength, 31);
    EXPECT_EQ(test.CellInfo.lte.signalStrengthLte.rsrp, 44);
    EXPECT_EQ(test.CellInfo.lte.signalStrengthLte.rsrq, 3);

    test.cellInfoType = RIL_CELL_INFO_TYPE_GSM;
    visible.decorate();
    EXPECT_EQ(test.CellInfo.gsm.signalStrengthGsm.signalStrength, 30);

    test.cellInfoType = RIL_CELL_INFO_TYPE_WCDMA;
    visible.decorate();
    EXPECT_EQ(test.CellInfo.wcdma.signalStrengthWcdma.signalStrength, 30);

    test.cellInfoType = RIL_CELL_INFO_TYPE_TD_SCDMA;
    visible.decorate();
    EXPECT_EQ(test.CellInfo.tdscdma.signalStrengthTdscdma.signalStrength, 30U);
    EXPECT_EQ(test.CellInfo.tdscdma.signalStrengthTdscdma.rscp, 90U);

    test.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_NR;
    visible.decorate();
    EXPECT_EQ(test.CellInfo.nr.signalStrengthNr.ssRsrp, 44);
    EXPECT_EQ(test.CellInfo.nr.signalStrengthNr.ssRsrq, 3);
    EXPECT_EQ(test.CellInfo.nr.signalStrengthNr.ssSinr, 20);
}

TEST(CellInfoDecorator, AlphaEnosDecorator) {
    OperatorNameProvider::MakeInstance();
    RIL_CellInfo_V1_6 test = {
        .cellInfoType = RIL_CELL_INFO_TYPE_LTE,
        .CellInfo.lte = {
            { .mcc = 450, .mnc = ril::util::mnc::encode(5, 2), }, {}, false,
        },
    };
    CellInfoBase base(&test);
    std::string simNumeric = "45005";
    AlphaEnosDecorator alpha(base, simNumeric, "", EonsResolver::createForPhoneId(0));
    alpha.decorate();

    std::string alphaLong = "SKTelecom";
    std::string alphaShort = "SKTelecom";
    EXPECT_TRUE(alphaLong.compare(test.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong) == 0);
    EXPECT_TRUE(alphaShort.compare(test.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort) == 0);
}

TEST(CellInfoDecorator, combined_and_list) {
    RIL_CellInfo_V1_6 test = {
        .cellInfoType = RIL_CELL_INFO_TYPE_LTE,
        .CellInfo.lte = {
            { .mcc = 450, .mnc = ril::util::mnc::encode(5, 2), }, {}, false,
        },
    };
    std::list<RIL_CellInfo_V1_6> testVector;
    testVector.push_back(test);

    std::string simNumeric = "45005";
    std::string alphaLong = "SKTelecom";
    std::string alphaShort = "SKTelecom";
    std::list<RIL_CellInfo_V1_6>::iterator iter = testVector.begin();
    std::list<RIL_CellInfo_V1_6>::iterator end = testVector.end();
    while (iter != end) {
        CellInfoBase cellinfo(&(*iter));
        SignalStrengthVisibleDecorator visible(cellinfo);
        AlphaEnosDecorator alpha(visible, simNumeric, "", EonsResolver::createForPhoneId(0));
        CellInfoDecorator& decorator = alpha;
        decorator.decorate();
        iter++;
    }

    RIL_CellInfo_V1_6& r = testVector.front();
    EXPECT_EQ(r.CellInfo.lte.signalStrengthLte.signalStrength, 31);
    EXPECT_EQ(r.CellInfo.lte.signalStrengthLte.rsrp, 44);
    EXPECT_EQ(r.CellInfo.lte.signalStrengthLte.rsrq, 3);
    EXPECT_TRUE(alphaLong.compare(r.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong) == 0);
    EXPECT_TRUE(alphaShort.compare(r.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort) == 0);
}
