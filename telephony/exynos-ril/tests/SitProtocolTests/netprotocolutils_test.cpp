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
 * netprotocolutils_test.cpp
 *
 * Author: sungwoo48.choi
 */

#include <gtest/gtest.h>

#include "rildef.h"
#include "sitdef.h"
#include "netprotocolutils.h"
#include "ts25table.h"
#include <librilutils/textutils.h>
#include <telephony/ril_mnc.h>

RIL_CellInfo_V1_6 gsm = {
    .cellInfoType = RIL_CELL_INFO_TYPE_GSM,
    .registered = true,
    .timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL,
    .connectionStatus = PRIMARY_SERVING,
    .CellInfo.gsm.cellIdentityGsm = {
            INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, 255,
            {}, 0, nullptr,
    },
    .CellInfo.gsm.signalStrengthGsm = { 99, 99, INT_MAX },
};
RIL_CellInfo_V1_6 cdma = {
    .cellInfoType = RIL_CELL_INFO_TYPE_CDMA,
    .registered = true,
    .timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL,
    .connectionStatus = PRIMARY_SERVING,
    .CellInfo.cdma.cellIdentityCdma = {
            INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
            {},
    },
    .CellInfo.cdma.signalStrengthCdma = { 0, 0 },
    .CellInfo.cdma.signalStrengthEvdo = { 0, 0, INT_MAX },
};
RIL_CellInfo_V1_6 wcdma = {
    .cellInfoType = RIL_CELL_INFO_TYPE_WCDMA,
    .registered = true,
    .timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL,
    .connectionStatus = PRIMARY_SERVING,
    .CellInfo.wcdma.cellIdentityWcdma = {
            INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
            {}, 0, nullptr, { false, {}, INT_MAX },
    },
    .CellInfo.wcdma.signalStrengthWcdma = { 99, 99, 255, 255 },
};
RIL_CellInfo_V1_6 lte = {
    .cellInfoType = RIL_CELL_INFO_TYPE_LTE,
    .registered = true,
    .timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL,
    .connectionStatus = PRIMARY_SERVING,
    .CellInfo.lte.cellIdentityLte = {
            INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
            {}, INT_MAX, 0, nullptr, { false, {}, INT_MAX }, 0, {}
    },
    .CellInfo.lte.signalStrengthLte = { { 99, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX }, INT_MAX },
    .CellInfo.lte.isEndcAvailable = false,
};
RIL_CellInfo_V1_6 tdscdma = {
    .cellInfoType = RIL_CELL_INFO_TYPE_TD_SCDMA,
    .registered = true,
    .timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL,
    .connectionStatus = PRIMARY_SERVING,
    .CellInfo.tdscdma.cellIdentityTdscdma = {
            INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
            {}, 0, nullptr, { false, {}, INT_MAX },
    },
    .CellInfo.tdscdma.signalStrengthTdscdma = { 99, 99, 255 },
};
RIL_CellInfo_V1_6 nr = {
    .cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_NR,
    .registered = true,
    .timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL,
    .connectionStatus = PRIMARY_SERVING,
    .CellInfo.nr.cellIdentityNr = {
            INT_MAX, INT_MAX, LONG_MAX, INT_MAX, INT_MAX, INT_MAX,
            {}, 0, nullptr, 0, {},
    },
    .CellInfo.nr.signalStrengthNr = { { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX }, INT_MAX, 0, {} },
};

TEST(CellInfoProtocolUtils, InitCellInfo) {
    RIL_CellInfo_V1_6 testGsmCell {};
    CellInfoProtocolUtils::InitCellInfo(testGsmCell, RIL_CELL_INFO_TYPE_GSM);
    EXPECT_EQ(testGsmCell.CellInfo.gsm.cellIdentityGsm.mcc, gsm.CellInfo.gsm.cellIdentityGsm.mcc);
    EXPECT_EQ(testGsmCell.CellInfo.gsm.cellIdentityGsm.mnc, gsm.CellInfo.gsm.cellIdentityGsm.mnc);
    EXPECT_EQ(testGsmCell.CellInfo.gsm.cellIdentityGsm.lac, gsm.CellInfo.gsm.cellIdentityGsm.lac);
    EXPECT_EQ(testGsmCell.CellInfo.gsm.cellIdentityGsm.cid, gsm.CellInfo.gsm.cellIdentityGsm.cid);
    EXPECT_EQ(testGsmCell.CellInfo.gsm.cellIdentityGsm.arfcn, gsm.CellInfo.gsm.cellIdentityGsm.arfcn);
    EXPECT_EQ(testGsmCell.CellInfo.gsm.cellIdentityGsm.bsic, gsm.CellInfo.gsm.cellIdentityGsm.bsic);
    EXPECT_EQ(testGsmCell.CellInfo.gsm.signalStrengthGsm.signalStrength, gsm.CellInfo.gsm.signalStrengthGsm.signalStrength);
    EXPECT_EQ(testGsmCell.CellInfo.gsm.signalStrengthGsm.bitErrorRate, gsm.CellInfo.gsm.signalStrengthGsm.bitErrorRate);
    EXPECT_EQ(testGsmCell.CellInfo.gsm.signalStrengthGsm.timingAdvance, gsm.CellInfo.gsm.signalStrengthGsm.timingAdvance);

    RIL_CellInfo_V1_6 testCdmaCell {};
    CellInfoProtocolUtils::InitCellInfo(testCdmaCell, RIL_CELL_INFO_TYPE_CDMA);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.cellIdentityCdma.networkId, cdma.CellInfo.cdma.cellIdentityCdma.networkId);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.cellIdentityCdma.systemId, cdma.CellInfo.cdma.cellIdentityCdma.systemId);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.cellIdentityCdma.basestationId, cdma.CellInfo.cdma.cellIdentityCdma.basestationId);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.cellIdentityCdma.longitude, cdma.CellInfo.cdma.cellIdentityCdma.longitude);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.cellIdentityCdma.latitude, cdma.CellInfo.cdma.cellIdentityCdma.latitude);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.signalStrengthCdma.dbm, cdma.CellInfo.cdma.signalStrengthCdma.dbm);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.signalStrengthCdma.ecio, cdma.CellInfo.cdma.signalStrengthCdma.ecio);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.signalStrengthEvdo.dbm, cdma.CellInfo.cdma.signalStrengthEvdo.dbm);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.signalStrengthEvdo.ecio, cdma.CellInfo.cdma.signalStrengthEvdo.ecio);
    EXPECT_EQ(testCdmaCell.CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio, cdma.CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio);

    RIL_CellInfo_V1_6 testWcdmaCell {};
    CellInfoProtocolUtils::InitCellInfo(testWcdmaCell, RIL_CELL_INFO_TYPE_WCDMA);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.cellIdentityWcdma.mcc, wcdma.CellInfo.wcdma.cellIdentityWcdma.mcc);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.cellIdentityWcdma.mnc, wcdma.CellInfo.wcdma.cellIdentityWcdma.mnc);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.cellIdentityWcdma.lac, wcdma.CellInfo.wcdma.cellIdentityWcdma.lac);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.cellIdentityWcdma.cid, wcdma.CellInfo.wcdma.cellIdentityWcdma.cid);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.cellIdentityWcdma.psc, wcdma.CellInfo.wcdma.cellIdentityWcdma.psc);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.cellIdentityWcdma.uarfcn, wcdma.CellInfo.wcdma.cellIdentityWcdma.uarfcn);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.homeNodebName[0],
              wcdma.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.homeNodebName[0]);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIdentity,
              wcdma.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIdentity);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.signalStrengthWcdma.signalStrength, wcdma.CellInfo.wcdma.signalStrengthWcdma.signalStrength);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.signalStrengthWcdma.bitErrorRate, wcdma.CellInfo.wcdma.signalStrengthWcdma.bitErrorRate);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.signalStrengthWcdma.rscp, wcdma.CellInfo.wcdma.signalStrengthWcdma.rscp);
    EXPECT_EQ(testWcdmaCell.CellInfo.wcdma.signalStrengthWcdma.ecno, wcdma.CellInfo.wcdma.signalStrengthWcdma.ecno);

    RIL_CellInfo_V1_6 testLteCell {};
    CellInfoProtocolUtils::InitCellInfo(testLteCell, RIL_CELL_INFO_TYPE_LTE);
    EXPECT_EQ(testLteCell.CellInfo.lte.cellIdentityLte.mcc, lte.CellInfo.lte.cellIdentityLte.mcc);
    EXPECT_EQ(testLteCell.CellInfo.lte.cellIdentityLte.mnc, lte.CellInfo.lte.cellIdentityLte.mnc);
    EXPECT_EQ(testLteCell.CellInfo.lte.cellIdentityLte.ci, lte.CellInfo.lte.cellIdentityLte.ci);
    EXPECT_EQ(testLteCell.CellInfo.lte.cellIdentityLte.pci, lte.CellInfo.lte.cellIdentityLte.pci);
    EXPECT_EQ(testLteCell.CellInfo.lte.cellIdentityLte.tac, lte.CellInfo.lte.cellIdentityLte.tac);
    EXPECT_EQ(testLteCell.CellInfo.lte.cellIdentityLte.earfcn, lte.CellInfo.lte.cellIdentityLte.earfcn);
    EXPECT_EQ(testLteCell.CellInfo.lte.cellIdentityLte.bandwidth, lte.CellInfo.lte.cellIdentityLte.bandwidth);
    EXPECT_EQ(testLteCell.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.homeNodebName[0],
            lte.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.homeNodebName[0]);
    EXPECT_EQ(testLteCell.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.csgIdentity,
            lte.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.csgIdentity);
    EXPECT_EQ(testLteCell.CellInfo.lte.signalStrengthLte.signalStrength, lte.CellInfo.lte.signalStrengthLte.signalStrength);
    EXPECT_EQ(testLteCell.CellInfo.lte.signalStrengthLte.rsrp, lte.CellInfo.lte.signalStrengthLte.rsrp);
    EXPECT_EQ(testLteCell.CellInfo.lte.signalStrengthLte.rsrq, lte.CellInfo.lte.signalStrengthLte.rsrq);
    EXPECT_EQ(testLteCell.CellInfo.lte.signalStrengthLte.rssnr, lte.CellInfo.lte.signalStrengthLte.rssnr);
    EXPECT_EQ(testLteCell.CellInfo.lte.signalStrengthLte.cqi, lte.CellInfo.lte.signalStrengthLte.cqi);
    EXPECT_EQ(testLteCell.CellInfo.lte.signalStrengthLte.timingAdvance, lte.CellInfo.lte.signalStrengthLte.timingAdvance);
    EXPECT_EQ(testLteCell.CellInfo.lte.signalStrengthLte.cqiTableIndex, lte.CellInfo.lte.signalStrengthLte.cqiTableIndex);
    EXPECT_EQ(testLteCell.CellInfo.lte.isEndcAvailable, lte.CellInfo.lte.isEndcAvailable);

    RIL_CellInfo_V1_6 testTdscdmaCell {};
    CellInfoProtocolUtils::InitCellInfo(testTdscdmaCell, RIL_CELL_INFO_TYPE_TD_SCDMA);
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.cellIdentityTdscdma.mcc, tdscdma.CellInfo.tdscdma.cellIdentityTdscdma.mcc);
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.cellIdentityTdscdma.mnc, tdscdma.CellInfo.tdscdma.cellIdentityTdscdma.mnc);
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.cellIdentityTdscdma.cid, tdscdma.CellInfo.tdscdma.cellIdentityTdscdma.cid);
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.cellIdentityTdscdma.cpid, tdscdma.CellInfo.tdscdma.cellIdentityTdscdma.cpid);
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.cellIdentityTdscdma.uarfcn, tdscdma.CellInfo.tdscdma.cellIdentityTdscdma.uarfcn);
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.signalStrengthTdscdma.signalStrength, tdscdma.CellInfo.tdscdma.signalStrengthTdscdma.signalStrength);
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.signalStrengthTdscdma.bitErrorRate, tdscdma.CellInfo.tdscdma.signalStrengthTdscdma.bitErrorRate);
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.signalStrengthTdscdma.rscp , tdscdma.CellInfo.tdscdma.signalStrengthTdscdma.rscp );
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.cellIdentityTdscdma.optionalCsgInfo.csgInfo.homeNodebName[0],
            tdscdma.CellInfo.tdscdma.cellIdentityTdscdma.optionalCsgInfo.csgInfo.homeNodebName[0]);
    EXPECT_EQ(testTdscdmaCell.CellInfo.tdscdma.cellIdentityTdscdma.optionalCsgInfo.csgInfo.csgIdentity,
            tdscdma.CellInfo.tdscdma.cellIdentityTdscdma.optionalCsgInfo.csgInfo.csgIdentity);

    RIL_CellInfo_V1_6 testNrCell {};
    CellInfoProtocolUtils::InitCellInfo(testNrCell, RIL_CELL_INFO_TYPE_NR);
    EXPECT_EQ(testNrCell.CellInfo.nr.cellIdentityNr.mcc, nr.CellInfo.nr.cellIdentityNr.mcc);
    EXPECT_EQ(testNrCell.CellInfo.nr.cellIdentityNr.mnc, nr.CellInfo.nr.cellIdentityNr.mnc);
    EXPECT_EQ(testNrCell.CellInfo.nr.cellIdentityNr.nci, nr.CellInfo.nr.cellIdentityNr.nci);
    EXPECT_EQ(testNrCell.CellInfo.nr.cellIdentityNr.pci, nr.CellInfo.nr.cellIdentityNr.pci);
    EXPECT_EQ(testNrCell.CellInfo.nr.cellIdentityNr.tac, nr.CellInfo.nr.cellIdentityNr.tac);
    EXPECT_EQ(testNrCell.CellInfo.nr.cellIdentityNr.nrarfcn, nr.CellInfo.nr.cellIdentityNr.nrarfcn);
    EXPECT_EQ(testNrCell.CellInfo.nr.signalStrengthNr.ssRsrp, nr.CellInfo.nr.signalStrengthNr.ssRsrp);
    EXPECT_EQ(testNrCell.CellInfo.nr.signalStrengthNr.ssRsrq, nr.CellInfo.nr.signalStrengthNr.ssRsrq);
    EXPECT_EQ(testNrCell.CellInfo.nr.signalStrengthNr.ssSinr, nr.CellInfo.nr.signalStrengthNr.ssSinr);
    EXPECT_EQ(testNrCell.CellInfo.nr.signalStrengthNr.csiRsrp, nr.CellInfo.nr.signalStrengthNr.csiRsrp);
    EXPECT_EQ(testNrCell.CellInfo.nr.signalStrengthNr.csiRsrq, nr.CellInfo.nr.signalStrengthNr.csiRsrq);
    EXPECT_EQ(testNrCell.CellInfo.nr.signalStrengthNr.csiSinr, nr.CellInfo.nr.signalStrengthNr.csiSinr);
    EXPECT_EQ(testNrCell.CellInfo.nr.signalStrengthNr.csiCqiTableIndex, nr.CellInfo.nr.signalStrengthNr.csiCqiTableIndex);
    EXPECT_EQ(testNrCell.CellInfo.nr.signalStrengthNr.csiCqiReportLen, nr.CellInfo.nr.signalStrengthNr.csiCqiReportLen);
}

TEST(CellInfoProtocolUtils, FetchMcc_FetchMnc_Legacy) {
    struct {
        char plmn[6];
        int mcc;
        int mnc;
    } testVector[] = {
        { { '4', '5', '0', '0', '5', '#' }, 450, 5 },
        { { '4', '6', '0', '0', '0', '#' }, 460, 0 },
        { { '3', '1', '0', '4', '8', '0' }, 310, 480 },
        { { '0', '0', '1', '0', '1', '#' }, 1, 1 },
        { { '0', '0', '1', '0', '1', '0' }, 1, 10 },
        { { '4', '5', 'a', '0', '5', '#' }, INT_MAX, INT_MAX },
        { { '4', '5', '0', 'a', '5', '#' }, 450, INT_MAX },
        { { '0', '0', '0', '0', '1', '#' }, 0, INT_MAX },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        int mcc = CellInfoProtocolUtils::Legacy::FetchMcc(testVector[i].plmn);
        int encodedMnc = CellInfoProtocolUtils::Legacy::FetchMnc(testVector[i].plmn);
        int mnc = encodedMnc != INT_MAX ? strtol(ril::util::mnc::decode(encodedMnc).c_str(), NULL, 10) : INT_MAX;
        EXPECT_EQ(mcc, testVector[i].mcc);
        EXPECT_EQ(mnc, testVector[i].mnc);
    }  // end for i ~
}

TEST(CellInfoProtocolUtils, FetchMcc_FetchMnc_JSON) {
    struct {
        const char* plmn;
        int mcc;
        int mnc;
    } testVector[] = {
        { "45005",  450,      5 },
        { "46000",  460,      0 },
        { "310480", 310,      480 },
        { "00101",  1,        1 },
        { "001010", 1,        10 },
        { "45a05",  INT_MAX,  INT_MAX },
        { "450a5",  INT_MAX,  INT_MAX },
        { "00001",  INT_MAX,  INT_MAX },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        int mcc = CellInfoProtocolUtils::JSON::FetchMcc(testVector[i].plmn);
        int encodedMnc = CellInfoProtocolUtils::JSON::FetchMnc(testVector[i].plmn);
        int mnc = encodedMnc != INT_MAX ? strtol(ril::util::mnc::decode(encodedMnc).c_str(), NULL, 10) : INT_MAX;
        EXPECT_EQ(mcc, testVector[i].mcc);
        EXPECT_EQ(mnc, testVector[i].mnc);
    }  // end for i ~
}

TEST(CellInfoProtocolUtils, FillOperatorName) {
    TS25Table::MakeInstance();

    struct {
        int mcc;
        int mnc;
        RIL_CellIdentityOperatorNames operatorName;
    } testVector[] = {
        { 450, ril::util::mnc::encode(5, 2),  { "KOR SK Telecom", "SKT" } },
        { 310, ril::util::mnc::encode(280, 3),  { "AT&T", "AT&T" } },
        { 460, ril::util::mnc::encode(7, 2),  { "CHINA MOBILE", "CMCC" } },
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RIL_CellIdentityOperatorNames test {};
        CellInfoProtocolUtils::Legacy::FillOperatorName(test, testVector[i].mcc, testVector[i].mnc);
        EXPECT_TRUE(TextUtils::Equals(test.alphaLong, testVector[i].operatorName.alphaLong));
        EXPECT_TRUE(TextUtils::Equals(test.alphaShort, testVector[i].operatorName.alphaShort));
    }  // end for i ~
}

TEST(CellInfoProtocolUtils, FillGsmCellInfo_RIL_CellInfoGsm_V1_5_cell_info_gsm_v12) {
    TS25Table::MakeInstance();

    RIL_CellInfoGsm_V1_5 cellinfo {};
    cell_info_gsm_v12 testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 10,
        .cid = 20,
        .arfcn = 1500,
        .bsic = 30,
        .sig_str = 15,
        .sig_ber = 5,
        .sig_ta = 25,
    };
    CellInfoProtocolUtils::Legacy::FillGsmCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityGsm.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityGsm.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityGsm.lac, testVector.lac);
    EXPECT_EQ(cellinfo.cellIdentityGsm.cid, testVector.cid);
    EXPECT_EQ(cellinfo.cellIdentityGsm.arfcn, testVector.arfcn);
    EXPECT_EQ(cellinfo.cellIdentityGsm.bsic, testVector.bsic);
    EXPECT_EQ(cellinfo.signalStrengthGsm.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthGsm.bitErrorRate, testVector.sig_ber);
    EXPECT_EQ(cellinfo.signalStrengthGsm.timingAdvance, testVector.sig_ta);

    // test with out-of-range
    cell_info_gsm_v12 testVectorInvalid = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 0x10000,
        .cid = 0x10000,
        .arfcn = 0x10000,
        .bsic = 0x40,
        .sig_str = 32,
        .sig_ber = 10,
        .sig_ta = 64,
    };

    CellInfoProtocolUtils::Legacy::FillGsmCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoGsm_V1_5 &defaultGsmCell = gsm.CellInfo.gsm;
    EXPECT_EQ(cellinfo.cellIdentityGsm.lac, defaultGsmCell.cellIdentityGsm.lac);
    EXPECT_EQ(cellinfo.cellIdentityGsm.cid, defaultGsmCell.cellIdentityGsm.cid);
    EXPECT_EQ(cellinfo.cellIdentityGsm.arfcn, defaultGsmCell.cellIdentityGsm.arfcn);
    EXPECT_EQ(cellinfo.cellIdentityGsm.bsic, defaultGsmCell.cellIdentityGsm.bsic);
    EXPECT_EQ(cellinfo.signalStrengthGsm.signalStrength, defaultGsmCell.signalStrengthGsm.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthGsm.bitErrorRate, defaultGsmCell.signalStrengthGsm.bitErrorRate);
    EXPECT_EQ(cellinfo.signalStrengthGsm.timingAdvance, defaultGsmCell.signalStrengthGsm.timingAdvance);
}

TEST(CellInfoProtocolUtils, FillGsmCellInfo_RIL_CellInfoGsm_V1_5_cell_info_type_gsm_v3) {
    TS25Table::MakeInstance();

    RIL_CellInfoGsm_V1_5 cellinfo {};
    cell_info_type_gsm_v3 testVector = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 10,
            .cid = 20,
            .arfcn = 1500,
            .bsic = 30,
        },
        .sig_str = 15,
        .sig_ber = 5,
        .sig_ta = 25,
    };
    CellInfoProtocolUtils::Legacy::FillGsmCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityGsm.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityGsm.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityGsm.lac, testVector.cell_identity.lac);
    EXPECT_EQ(cellinfo.cellIdentityGsm.cid, testVector.cell_identity.cid);
    EXPECT_EQ(cellinfo.cellIdentityGsm.arfcn, testVector.cell_identity.arfcn);
    EXPECT_EQ(cellinfo.cellIdentityGsm.bsic, testVector.cell_identity.bsic);
    EXPECT_EQ(cellinfo.signalStrengthGsm.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthGsm.bitErrorRate, testVector.sig_ber);
    EXPECT_EQ(cellinfo.signalStrengthGsm.timingAdvance, testVector.sig_ta);

    // test with out-of-range
    cell_info_type_gsm_v3 testVectorInvalid = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 0x10000,
            .cid = 0x10000,
            .arfcn = 0x10000,
            .bsic = 0x40,
        },
        .sig_str = 32,
        .sig_ber = 10,
        .sig_ta = 64,
    };
    CellInfoProtocolUtils::Legacy::FillGsmCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoGsm_V1_5 &defaultGsmCell = gsm.CellInfo.gsm;
    EXPECT_EQ(cellinfo.cellIdentityGsm.lac, defaultGsmCell.cellIdentityGsm.lac);
    EXPECT_EQ(cellinfo.cellIdentityGsm.cid, defaultGsmCell.cellIdentityGsm.cid);
    EXPECT_EQ(cellinfo.cellIdentityGsm.arfcn, defaultGsmCell.cellIdentityGsm.arfcn);
    EXPECT_EQ(cellinfo.cellIdentityGsm.bsic, defaultGsmCell.cellIdentityGsm.bsic);
    EXPECT_EQ(cellinfo.signalStrengthGsm.signalStrength, defaultGsmCell.signalStrengthGsm.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthGsm.bitErrorRate, defaultGsmCell.signalStrengthGsm.bitErrorRate);
    EXPECT_EQ(cellinfo.signalStrengthGsm.timingAdvance, defaultGsmCell.signalStrengthGsm.timingAdvance);
}

TEST(CellInfoProtocolUtils, FillGsmCellInfo_RIL_CellInfoGsm_V1_5_cell_info_type_gsm_v4) {
    TS25Table::MakeInstance();

    RIL_CellInfoGsm_V1_5 cellinfo {};
    cell_info_type_gsm_v4 testVector = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 10,
            .cid = 20,
            .arfcn = 1500,
            .bsic = 30,
        },
        .sig_str = 15,
        .sig_ber = 5,
        .sig_ta = 25,
    };
    CellInfoProtocolUtils::Legacy::FillGsmCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityGsm.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityGsm.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityGsm.lac, testVector.cell_identity.lac);
    EXPECT_EQ(cellinfo.cellIdentityGsm.cid, testVector.cell_identity.cid);
    EXPECT_EQ(cellinfo.cellIdentityGsm.arfcn, testVector.cell_identity.arfcn);
    EXPECT_EQ(cellinfo.cellIdentityGsm.bsic, testVector.cell_identity.bsic);
    EXPECT_EQ(cellinfo.signalStrengthGsm.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthGsm.bitErrorRate, testVector.sig_ber);
    EXPECT_EQ(cellinfo.signalStrengthGsm.timingAdvance, testVector.sig_ta);

    // test with out-of-range
    cell_info_type_gsm_v4 testVectorInvalid = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 0x10000,
            .cid = 0x10000,
            .arfcn = 0x10000,
            .bsic = 0x40,
        },
        .sig_str = 32,
        .sig_ber = 10,
        .sig_ta = 64,
    };
    CellInfoProtocolUtils::Legacy::FillGsmCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoGsm_V1_5 &defaultGsmCell = gsm.CellInfo.gsm;
    EXPECT_EQ(cellinfo.cellIdentityGsm.lac, defaultGsmCell.cellIdentityGsm.lac);
    EXPECT_EQ(cellinfo.cellIdentityGsm.cid, defaultGsmCell.cellIdentityGsm.cid);
    EXPECT_EQ(cellinfo.cellIdentityGsm.arfcn, defaultGsmCell.cellIdentityGsm.arfcn);
    EXPECT_EQ(cellinfo.cellIdentityGsm.bsic, defaultGsmCell.cellIdentityGsm.bsic);
    EXPECT_EQ(cellinfo.signalStrengthGsm.signalStrength, defaultGsmCell.signalStrengthGsm.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthGsm.bitErrorRate, defaultGsmCell.signalStrengthGsm.bitErrorRate);
    EXPECT_EQ(cellinfo.signalStrengthGsm.timingAdvance, defaultGsmCell.signalStrengthGsm.timingAdvance);
}

TEST(CellInfoProtocolUtils, FillCdmaCellInfo_RIL_CellInfoCdma_V1_2_cell_info_cdma) {
    TS25Table::MakeInstance();

    RIL_CellInfoCdma_V1_2 cellinfo {};
    cell_info_cdma testVector = {
        .ntw_id = 1000,
        .sys_id = 1120,
        .bs_id = 2000,
        .longitude = 1000000,
        .lat = 1000000,
        .sig_dbm = -75,
        .sig_ecio = -15,
        .sig_snr = 5,
        .evdo_sig_dbm = -8,
        .evdo_sig_ecio = -20,
        .evdo_sig_snr = 4,
    };
    CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityCdma.networkId, testVector.ntw_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.systemId, testVector.sys_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.basestationId, testVector.bs_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.longitude, testVector.longitude);
    EXPECT_EQ(cellinfo.cellIdentityCdma.latitude, testVector.lat);
    EXPECT_EQ(cellinfo.signalStrengthCdma.dbm, -testVector.sig_dbm);
    EXPECT_EQ(cellinfo.signalStrengthCdma.ecio, (-10) * testVector.sig_ecio);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.dbm, -testVector.evdo_sig_dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.ecio, (-10) * testVector.evdo_sig_ecio);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.signalNoiseRatio, testVector.evdo_sig_snr);

    cell_info_cdma testVectorInvalid = {
        .ntw_id = 0x10000,
        .sys_id = 0x8FFF,
        .bs_id = 0x10000,
        .longitude = 2592001,
        .lat = 1296001,
        .sig_dbm = 0,
        .sig_ecio = 0,
        .sig_snr = 0,
        .evdo_sig_dbm = 0,
        .evdo_sig_ecio = 0,
        .evdo_sig_snr = INT_MAX,
    };
    CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoCdma_V1_2 &defaultCdmaCell = cdma.CellInfo.cdma;
    EXPECT_EQ(cellinfo.cellIdentityCdma.networkId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.systemId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.basestationId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.longitude, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.latitude, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.signalStrengthCdma.dbm, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthCdma.ecio, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.dbm, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.ecio, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.signalNoiseRatio, defaultCdmaCell.signalStrengthEvdo.signalNoiseRatio);
}

TEST(CellInfoProtocolUtils, FillCdmaCellInfo_RIL_CellInfoCdma_V1_2_cell_info_cdma_v14) {
    TS25Table::MakeInstance();

    RIL_CellInfoCdma_V1_2 cellinfo {};
    cell_info_cdma_v14 testVector = {
        .ntw_id = 1000,
        .sys_id = 1120,
        .bs_id = 2000,
        .longitude = 1000000,
        .lat = 1000000,
        .sig_dbm = -75,
        .sig_ecio = -15,
        .evdo_sig_dbm = -8,
        .evdo_sig_ecio = -20,
        .evdo_sig_snr = 4,
    };
    CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityCdma.networkId, testVector.ntw_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.systemId, testVector.sys_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.basestationId, testVector.bs_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.longitude, testVector.longitude);
    EXPECT_EQ(cellinfo.cellIdentityCdma.latitude, testVector.lat);
    EXPECT_EQ(cellinfo.signalStrengthCdma.dbm, -testVector.sig_dbm);
    EXPECT_EQ(cellinfo.signalStrengthCdma.ecio, (-10) * testVector.sig_ecio);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.dbm, -testVector.evdo_sig_dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.ecio, (-10) * testVector.evdo_sig_ecio);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.signalNoiseRatio, testVector.evdo_sig_snr);

    cell_info_cdma_v14 testVectorInvalid = {
        .ntw_id = 0x10000,
        .sys_id = 0x8FFF,
        .bs_id = 0x10000,
        .longitude = 2592001,
        .lat = 1296001,
        .sig_dbm = 0,
        .sig_ecio = 0,
        .evdo_sig_dbm = 0,
        .evdo_sig_ecio = 0,
        .evdo_sig_snr = INT_MAX,
    };
    CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoCdma_V1_2 &defaultCdmaCell = cdma.CellInfo.cdma;
    EXPECT_EQ(cellinfo.cellIdentityCdma.networkId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.systemId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.basestationId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.longitude, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.latitude, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.signalStrengthCdma.dbm, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthCdma.ecio, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.dbm, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.ecio, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.signalNoiseRatio, defaultCdmaCell.signalStrengthEvdo.signalNoiseRatio);
}

TEST(CellInfoProtocolUtils, FillCdmaCellInfo_RIL_CellInfoCdma_V1_2_cell_info_type_cdma_v3) {
    TS25Table::MakeInstance();

    RIL_CellInfoCdma_V1_2 cellinfo {};
    cell_info_type_cdma_v3 testVector = {
        .cell_identity = {
            .ntw_id = 1000,
            .sys_id = 1120,
            .bs_id = 2000,
            .longitude = 1000000,
            .latitude = 1000000,
        },
        .cdma_dbm = -75,
        .cdma_ecio = -15,
        .evdo_dbm = -8,
        .evdo_ecio = -20,
        .evdo_snr = 4,
    };
    CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityCdma.networkId, testVector.cell_identity.ntw_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.systemId, testVector.cell_identity.sys_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.basestationId, testVector.cell_identity.bs_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.longitude, testVector.cell_identity.longitude);
    EXPECT_EQ(cellinfo.cellIdentityCdma.latitude, testVector.cell_identity.latitude);
    EXPECT_EQ(cellinfo.signalStrengthCdma.dbm, -testVector.cdma_dbm);
    EXPECT_EQ(cellinfo.signalStrengthCdma.ecio, (-10) * testVector.cdma_ecio);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.dbm, -testVector.evdo_dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.ecio, (-10) * testVector.evdo_ecio);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.signalNoiseRatio, testVector.evdo_snr);

    cell_info_type_cdma_v3 testVectorInvalid = {
        .cell_identity = {
            .ntw_id = 0x10000,
            .sys_id = 0x8FFF,
            .bs_id = 0x10000,
            .longitude = 2592001,
            .latitude = 1296001,
        },
        .cdma_dbm = 0,
        .cdma_ecio = 0,
        .evdo_dbm = 0,
        .evdo_ecio = 0,
        .evdo_snr = INT_MAX,
    };
    CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoCdma_V1_2 &defaultCdmaCell = cdma.CellInfo.cdma;
    EXPECT_EQ(cellinfo.cellIdentityCdma.networkId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.systemId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.basestationId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.longitude, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.latitude, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.signalStrengthCdma.dbm, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthCdma.ecio, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.dbm, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.ecio, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.signalNoiseRatio, defaultCdmaCell.signalStrengthEvdo.signalNoiseRatio);
}

TEST(CellInfoProtocolUtils, FillCdmaCellInfo_RIL_CellInfoCdma_V1_2_cell_info_type_cdma_v4) {
    TS25Table::MakeInstance();

    RIL_CellInfoCdma_V1_2 cellinfo {};
    cell_info_type_cdma_v4 testVector = {
        .cell_identity = {
            .ntw_id = 1000,
            .sys_id = 1120,
            .bs_id = 2000,
            .longitude = 1000000,
            .latitude = 1000000,
        },
        .cdma_dbm = -75,
        .cdma_ecio = -15,
        .evdo_dbm = -8,
        .evdo_ecio = -20,
        .evdo_snr = 4,
    };
    CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityCdma.networkId, testVector.cell_identity.ntw_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.systemId, testVector.cell_identity.sys_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.basestationId, testVector.cell_identity.bs_id);
    EXPECT_EQ(cellinfo.cellIdentityCdma.longitude, testVector.cell_identity.longitude);
    EXPECT_EQ(cellinfo.cellIdentityCdma.latitude, testVector.cell_identity.latitude);
    EXPECT_EQ(cellinfo.signalStrengthCdma.dbm, -testVector.cdma_dbm);
    EXPECT_EQ(cellinfo.signalStrengthCdma.ecio, (-10) * testVector.cdma_ecio);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.dbm, -testVector.evdo_dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.ecio, (-10) * testVector.evdo_ecio);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.signalNoiseRatio, testVector.evdo_snr);

    cell_info_type_cdma_v4 testVectorInvalid = {
        .cell_identity = {
            .ntw_id = 0x10000,
            .sys_id = 0x8FFF,
            .bs_id = 0x10000,
            .longitude = 2592001,
            .latitude = 1296001,
        },
        .cdma_dbm = 0,
        .cdma_ecio = 0,
        .evdo_dbm = 0,
        .evdo_ecio = 0,
        .evdo_snr = INT_MAX,
    };
    CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoCdma_V1_2 &defaultCdmaCell = cdma.CellInfo.cdma;
    EXPECT_EQ(cellinfo.cellIdentityCdma.networkId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.systemId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.basestationId, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.longitude, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.cellIdentityCdma.latitude, defaultCdmaCell.cellIdentityCdma.networkId);
    EXPECT_EQ(cellinfo.signalStrengthCdma.dbm, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthCdma.ecio, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.dbm, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.ecio, defaultCdmaCell.signalStrengthCdma.dbm);
    EXPECT_EQ(cellinfo.signalStrengthEvdo.signalNoiseRatio, defaultCdmaCell.signalStrengthEvdo.signalNoiseRatio);
}

TEST(CellInfoProtocolUtils, FillLteCellInfo_RIL_CellInfoLte_V1_6_cell_info_lte_v12) {
    TS25Table::MakeInstance();

    RIL_CellInfoLte_V1_6 cellinfo {};
    cell_info_lte_v12 testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .cell_id = 123456,
        .phy_cell_id = 250,
        .tac = 3000,
        .earfcn = 2200,
        .sig_str = 15,
        .sig_rsrp = 100,
        .sig_rsrq = 7,
        .sig_rssnr = -100,
        .sig_cqi = 7,
        .ta = 1234567,
    };
    CellInfoProtocolUtils::Legacy::FillLteCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityLte.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityLte.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityLte.ci, testVector.cell_id);
    EXPECT_EQ(cellinfo.cellIdentityLte.pci, testVector.phy_cell_id);
    EXPECT_EQ(cellinfo.cellIdentityLte.tac, testVector.tac);
    EXPECT_EQ(cellinfo.cellIdentityLte.earfcn, testVector.earfcn);
    EXPECT_EQ(cellinfo.signalStrengthLte.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrp, testVector.sig_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrq, testVector.sig_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthLte.rssnr, testVector.sig_rssnr);
    EXPECT_EQ(cellinfo.signalStrengthLte.cqi, testVector.sig_cqi);
    EXPECT_EQ(cellinfo.signalStrengthLte.timingAdvance, testVector.ta);

    cell_info_lte_v12 testVectorInvalid = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .cell_id = 0x10000000,
        .phy_cell_id = 504,
        .tac = 0x10000,
        .earfcn = 0x40000,
        .sig_str = 32,
        .sig_rsrp = 141,
        .sig_rsrq = 21,
        .sig_rssnr = 301,
        .sig_cqi = 16,
        .ta = 0x7FFFFFFF,
    };
    CellInfoProtocolUtils::Legacy::FillLteCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoLte_V1_6 &defaultLteCell = lte.CellInfo.lte;
    EXPECT_EQ(cellinfo.cellIdentityLte.ci, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.pci, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.tac, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.earfcn, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.signalStrengthLte.signalStrength, defaultLteCell.signalStrengthLte.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrp, defaultLteCell.signalStrengthLte.rsrp);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrq, defaultLteCell.signalStrengthLte.rsrq);
    EXPECT_EQ(cellinfo.signalStrengthLte.rssnr, defaultLteCell.signalStrengthLte.rssnr);
    EXPECT_EQ(cellinfo.signalStrengthLte.cqi, defaultLteCell.signalStrengthLte.cqi);
    EXPECT_EQ(cellinfo.signalStrengthLte.timingAdvance, defaultLteCell.signalStrengthLte.timingAdvance);
}

TEST(CellInfoProtocolUtils, FillLteCellInfo_RIL_CellInfoLte_V1_6_cell_info_lte_v14) {
    TS25Table::MakeInstance();

    RIL_CellInfoLte_V1_6 cellinfo {};
    cell_info_lte_v14 testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .cell_id = 123456,
        .phy_cell_id = 250,
        .tac = 3000,
        .earfcn = 2200,
        .bandwidth = 100,
        .endc_available = 1,
        .sig_str = 15,
        .sig_rsrp = 100,
        .sig_rsrq = 7,
        .sig_rssnr = -100,
        .sig_cqi = 7,
        .ta = 1234567,
    };
    CellInfoProtocolUtils::Legacy::FillLteCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityLte.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityLte.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityLte.ci, testVector.cell_id);
    EXPECT_EQ(cellinfo.cellIdentityLte.pci, testVector.phy_cell_id);
    EXPECT_EQ(cellinfo.cellIdentityLte.tac, testVector.tac);
    EXPECT_EQ(cellinfo.cellIdentityLte.earfcn, testVector.earfcn);
    EXPECT_EQ(cellinfo.cellIdentityLte.bandwidth, testVector.bandwidth);
    EXPECT_EQ(cellinfo.isEndcAvailable, testVector.endc_available);
    EXPECT_EQ(cellinfo.signalStrengthLte.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrp, testVector.sig_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrq, testVector.sig_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthLte.rssnr, testVector.sig_rssnr);
    EXPECT_EQ(cellinfo.signalStrengthLte.cqi, testVector.sig_cqi);
    EXPECT_EQ(cellinfo.signalStrengthLte.timingAdvance, testVector.ta);

    cell_info_lte_v14 testVectorInvalid = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .cell_id = 0x10000000,
        .phy_cell_id = 504,
        .tac = 0x10000,
        .earfcn = 0x40000,
        .bandwidth = -1,
        .endc_available = 0,
        .sig_str = 32,
        .sig_rsrp = 141,
        .sig_rsrq = 21,
        .sig_rssnr = 301,
        .sig_cqi = 16,
        .ta = 0x7FFFFFFF,
    };
    CellInfoProtocolUtils::Legacy::FillLteCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoLte_V1_6 &defaultLteCell = lte.CellInfo.lte;
    EXPECT_EQ(cellinfo.cellIdentityLte.ci, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.pci, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.tac, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.earfcn, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.bandwidth, defaultLteCell.cellIdentityLte.bandwidth);
    EXPECT_EQ(cellinfo.isEndcAvailable, defaultLteCell.isEndcAvailable);
    EXPECT_EQ(cellinfo.signalStrengthLte.signalStrength, defaultLteCell.signalStrengthLte.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrp, defaultLteCell.signalStrengthLte.rsrp);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrq, defaultLteCell.signalStrengthLte.rsrq);
    EXPECT_EQ(cellinfo.signalStrengthLte.rssnr, defaultLteCell.signalStrengthLte.rssnr);
    EXPECT_EQ(cellinfo.signalStrengthLte.cqi, defaultLteCell.signalStrengthLte.cqi);
    EXPECT_EQ(cellinfo.signalStrengthLte.timingAdvance, defaultLteCell.signalStrengthLte.timingAdvance);
}

TEST(CellInfoProtocolUtils, FillLteCellInfo_RIL_CellInfoLte_V1_6_cell_info_type_lte_v3) {
    TS25Table::MakeInstance();

    RIL_CellInfoLte_V1_6 cellinfo {};
    cell_info_type_lte_v3 testVector = {
         .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .cell_id = 123456,
            .phy_cell_id = 250,
            .tac = 3000,
            .earfcn = 2200,
            .bandwidth = 100,
            .plmn_info = { "SKT", "SK Telecom", 0, {}, },
            .csg_info = { 1, "test_network1", 1234567, },
            .band_len = 3,
            .band_info = { 1, 2, 3, },
         },
        .endc_available = 1,
        .sig_str = 15,
        .sig_rsrp = 100,
        .sig_rsrq = 7,
        .sig_rssnr = -100,
        .cqi = 7,
        .ta = 1234567,
    };
    CellInfoProtocolUtils::Legacy::FillLteCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityLte.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityLte.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityLte.ci, testVector.cell_identity.cell_id);
    EXPECT_EQ(cellinfo.cellIdentityLte.pci, testVector.cell_identity.phy_cell_id);
    EXPECT_EQ(cellinfo.cellIdentityLte.tac, testVector.cell_identity.tac);
    EXPECT_EQ(cellinfo.cellIdentityLte.earfcn, testVector.cell_identity.earfcn);
    EXPECT_EQ(cellinfo.cellIdentityLte.bandwidth, testVector.cell_identity.bandwidth);
    EXPECT_EQ(cellinfo.cellIdentityLte.optionalCsgInfo.csgInfo.csgIndication,
            testVector.cell_identity.csg_info.csg_indication);
    EXPECT_TRUE(TextUtils::Equals(cellinfo.cellIdentityLte.optionalCsgInfo.csgInfo.homeNodebName,
            testVector.cell_identity.csg_info.hnb_name));
    EXPECT_EQ(cellinfo.cellIdentityLte.optionalCsgInfo.csgInfo.csgIdentity,
                testVector.cell_identity.csg_info.csg_identity);
    EXPECT_EQ(cellinfo.cellIdentityLte.leng_bands, testVector.cell_identity.band_len);
    for (int i = 0; i < cellinfo.cellIdentityLte.leng_bands; i++) {
        EXPECT_EQ(cellinfo.cellIdentityLte.bands[i], testVector.cell_identity.band_info[i]);
    }
    EXPECT_EQ(cellinfo.isEndcAvailable, testVector.endc_available);
    EXPECT_EQ(cellinfo.signalStrengthLte.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrp, testVector.sig_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrq, testVector.sig_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthLte.rssnr, testVector.sig_rssnr);
    EXPECT_EQ(cellinfo.signalStrengthLte.cqi, testVector.cqi);
    EXPECT_EQ(cellinfo.signalStrengthLte.timingAdvance, testVector.ta);

    cell_info_type_lte_v3 testVectorInvalid = {
         .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .cell_id = 0x10000000,
            .phy_cell_id = 504,
            .tac = 0x10000,
            .earfcn = 0x40000,
            .bandwidth = -1,
            .plmn_info = {},
            .csg_info = {},
            .band_len = 0,
            .band_info = {},
         },
         .endc_available = 0,
         .sig_str = 32,
         .sig_rsrp = 141,
         .sig_rsrq = 21,
         .sig_rssnr = 301,
         .cqi = 16,
         .ta = 0x7FFFFFFF,
    };
    CellInfoProtocolUtils::Legacy::FillLteCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoLte_V1_6 &defaultLteCell = lte.CellInfo.lte;
    EXPECT_EQ(cellinfo.cellIdentityLte.ci, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.pci, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.tac, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.earfcn, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.bandwidth, defaultLteCell.cellIdentityLte.bandwidth);
    EXPECT_EQ(cellinfo.isEndcAvailable, defaultLteCell.isEndcAvailable);
    EXPECT_EQ(cellinfo.signalStrengthLte.signalStrength, defaultLteCell.signalStrengthLte.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrp, defaultLteCell.signalStrengthLte.rsrp);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrq, defaultLteCell.signalStrengthLte.rsrq);
    EXPECT_EQ(cellinfo.signalStrengthLte.rssnr, defaultLteCell.signalStrengthLte.rssnr);
    EXPECT_EQ(cellinfo.signalStrengthLte.cqi, defaultLteCell.signalStrengthLte.cqi);
    EXPECT_EQ(cellinfo.signalStrengthLte.timingAdvance, defaultLteCell.signalStrengthLte.timingAdvance);
}

TEST(CellInfoProtocolUtils, FillLteCellInfo_RIL_CellInfoLte_V1_6_cell_info_type_lte_v4) {
    TS25Table::MakeInstance();

    RIL_CellInfoLte_V1_6 cellinfo {};
    cell_info_type_lte_v4 testVector = {
         .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .cell_id = 123456,
            .phy_cell_id = 250,
            .tac = 3000,
            .earfcn = 2200,
            .bandwidth = 100,
            .plmn_info = { "SKT", "SK Telecom", 0, {}, },
            .csg_info = { 1, "test_network1", 1234567, },
            .band_len = 3,
            .band_info = { 1, 2, 3, },
         },
        .endc_available = 1,
        .sig_str = 15,
        .sig_rsrp = 100,
        .sig_rsrq = 7,
        .sig_rssnr = -100,
        .cqi = 7,
        .ta = 1234567,
    };
    CellInfoProtocolUtils::Legacy::FillLteCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityLte.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityLte.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityLte.ci, testVector.cell_identity.cell_id);
    EXPECT_EQ(cellinfo.cellIdentityLte.pci, testVector.cell_identity.phy_cell_id);
    EXPECT_EQ(cellinfo.cellIdentityLte.tac, testVector.cell_identity.tac);
    EXPECT_EQ(cellinfo.cellIdentityLte.earfcn, testVector.cell_identity.earfcn);
    EXPECT_EQ(cellinfo.cellIdentityLte.bandwidth, testVector.cell_identity.bandwidth);
    EXPECT_EQ(cellinfo.cellIdentityLte.optionalCsgInfo.csgInfo.csgIndication,
            testVector.cell_identity.csg_info.csg_indication);
    EXPECT_TRUE(TextUtils::Equals(cellinfo.cellIdentityLte.optionalCsgInfo.csgInfo.homeNodebName,
            testVector.cell_identity.csg_info.hnb_name));
    EXPECT_EQ(cellinfo.cellIdentityLte.optionalCsgInfo.csgInfo.csgIdentity,
                testVector.cell_identity.csg_info.csg_identity);
    EXPECT_EQ(cellinfo.cellIdentityLte.leng_bands, testVector.cell_identity.band_len);
    for (int i = 0; i < cellinfo.cellIdentityLte.leng_bands; i++) {
        EXPECT_EQ(cellinfo.cellIdentityLte.bands[i], testVector.cell_identity.band_info[i]);
    }
    EXPECT_EQ(cellinfo.isEndcAvailable, testVector.endc_available);
    EXPECT_EQ(cellinfo.signalStrengthLte.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrp, testVector.sig_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrq, testVector.sig_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthLte.rssnr, testVector.sig_rssnr);
    EXPECT_EQ(cellinfo.signalStrengthLte.cqi, testVector.cqi);
    EXPECT_EQ(cellinfo.signalStrengthLte.timingAdvance, testVector.ta);

    cell_info_type_lte_v4 testVectorInvalid = {
         .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .cell_id = 0x10000000,
            .phy_cell_id = 504,
            .tac = 0x10000,
            .earfcn = 0x40000,
            .bandwidth = -1,
            .plmn_info = {},
            .csg_info = {},
            .band_len = 0,
            .band_info = {},
         },
         .endc_available = 0,
         .sig_str = 32,
         .sig_rsrp = 141,
         .sig_rsrq = 21,
         .sig_rssnr = 301,
         .cqi = 16,
         .ta = 0x7FFFFFFF,
    };
    CellInfoProtocolUtils::Legacy::FillLteCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoLte_V1_6 &defaultLteCell = lte.CellInfo.lte;
    EXPECT_EQ(cellinfo.cellIdentityLte.ci, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.pci, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.tac, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.earfcn, defaultLteCell.cellIdentityLte.ci);
    EXPECT_EQ(cellinfo.cellIdentityLte.bandwidth, defaultLteCell.cellIdentityLte.bandwidth);
    EXPECT_EQ(cellinfo.isEndcAvailable, defaultLteCell.isEndcAvailable);
    EXPECT_EQ(cellinfo.signalStrengthLte.signalStrength, defaultLteCell.signalStrengthLte.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrp, defaultLteCell.signalStrengthLte.rsrp);
    EXPECT_EQ(cellinfo.signalStrengthLte.rsrq, defaultLteCell.signalStrengthLte.rsrq);
    EXPECT_EQ(cellinfo.signalStrengthLte.rssnr, defaultLteCell.signalStrengthLte.rssnr);
    EXPECT_EQ(cellinfo.signalStrengthLte.cqi, defaultLteCell.signalStrengthLte.cqi);
    EXPECT_EQ(cellinfo.signalStrengthLte.timingAdvance, defaultLteCell.signalStrengthLte.timingAdvance);
}

TEST(CellInfoProtocolUtils, FillWcdmaCellInfo_RIL_RIL_CellInfoWcdma_V1_5_cell_info_wcdma_v12) {
    TS25Table::MakeInstance();

    RIL_CellInfoWcdma_V1_5 cellinfo {};
    cell_info_wcdma_v12 testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 12345,
        .cid = 250,
        .psc = 150,
        .uarfcn = 2200,
        .sig_str = 15,
        .sig_ber = 5,
    };
    CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.lac, testVector.lac);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.cid, testVector.cid);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.psc, testVector.psc);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.uarfcn, testVector.uarfcn);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.bitErrorRate, testVector.sig_ber);

    cell_info_wcdma_v12 testVectorInvalid = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 0x10000,
        .cid = 0x10000000,
        .psc = 0x200,
        .uarfcn = 0x10000,
        .sig_str = 32,
        .sig_ber = 8,
    };
    CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoWcdma_V1_5 &defaultWcdmaCell = wcdma.CellInfo.wcdma;
    EXPECT_EQ(cellinfo.cellIdentityWcdma.lac, defaultWcdmaCell.cellIdentityWcdma.lac);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.cid, defaultWcdmaCell.cellIdentityWcdma.cid);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.psc, defaultWcdmaCell.cellIdentityWcdma.psc);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.uarfcn, defaultWcdmaCell.cellIdentityWcdma.uarfcn);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.signalStrength, defaultWcdmaCell.signalStrengthWcdma.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.bitErrorRate, defaultWcdmaCell.signalStrengthWcdma.bitErrorRate);
}

TEST(CellInfoProtocolUtils, FillWcdmaCellInfo_RIL_CellInfoWcdma_V1_5_cell_info_wcdma_v14) {
    TS25Table::MakeInstance();

    RIL_CellInfoWcdma_V1_5 cellinfo {};
    cell_info_wcdma_v14 testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 12345,
        .cid = 250,
        .psc = 150,
        .uarfcn = 2200,
        .sig_str = 15,
        .sig_ber = 5,
        .rscp = 80,
        .ecno = 30,
    };
    CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.lac, testVector.lac);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.cid, testVector.cid);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.psc, testVector.psc);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.uarfcn, testVector.uarfcn);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.bitErrorRate, testVector.sig_ber);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.rscp, (unsigned int)/* NEED TO CHECK */ testVector.rscp);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.ecno, (unsigned int)/* NEED TO CHECK */ testVector.ecno);

    cell_info_wcdma_v14 testVectorInvalid = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 0x10000,
        .cid = 0x10000000,
        .psc = 0x200,
        .uarfcn = 0x10000,
        .sig_str = 32,
        .sig_ber = 8,
        .rscp = 97,
        .ecno = 50,
    };
    CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoWcdma_V1_5 &defaultWcdmaCell = wcdma.CellInfo.wcdma;
    EXPECT_EQ(cellinfo.cellIdentityWcdma.lac, defaultWcdmaCell.cellIdentityWcdma.lac);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.cid, defaultWcdmaCell.cellIdentityWcdma.cid);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.psc, defaultWcdmaCell.cellIdentityWcdma.psc);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.uarfcn, defaultWcdmaCell.cellIdentityWcdma.uarfcn);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.signalStrength, defaultWcdmaCell.signalStrengthWcdma.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.bitErrorRate, defaultWcdmaCell.signalStrengthWcdma.bitErrorRate);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.rscp, (unsigned int)/* NEED TO CHECK */ defaultWcdmaCell.signalStrengthWcdma.rscp);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.ecno, (unsigned int)/* NEED TO CHECK */ defaultWcdmaCell.signalStrengthWcdma.ecno);
}

TEST(CellInfoProtocolUtils, FillWcdmaCellInfo_RIL_CellInfoWcdma_V1_5_cell_info_type_wcdma_v3) {
    TS25Table::MakeInstance();

    RIL_CellInfoWcdma_V1_5 cellinfo {};
    cell_info_type_wcdma_v3 testVector = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 12345,
            .cid = 250,
            .psc = 150,
            .uarfcn = 2200,
            .plmn_info = { "SKT", "SK Telecom", 0, {}, },
            .csg_info = { 1, "test_network1", 1234567, },
        },
        .sig_str = 15,
        .sig_ber = 5,
        .rscp = 80,
        .ecno = 30,
    };
    CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.lac, testVector.cell_identity.lac);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.cid, testVector.cell_identity.cid);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.psc, testVector.cell_identity.psc);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.uarfcn, testVector.cell_identity.uarfcn);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIndication,
            testVector.cell_identity.csg_info.csg_indication);
    EXPECT_TRUE(TextUtils::Equals(cellinfo.cellIdentityWcdma.optionalCsgInfo.csgInfo.homeNodebName,
            testVector.cell_identity.csg_info.hnb_name));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIdentity,
                testVector.cell_identity.csg_info.csg_identity);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.bitErrorRate, testVector.sig_ber);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.rscp, (unsigned int)/* NEED TO CHECK */ testVector.rscp);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.ecno, (unsigned int)/* NEED TO CHECK */ testVector.ecno);

    cell_info_type_wcdma_v3 testVectorInvalid = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 0x10000,
            .cid = 0x10000000,
            .psc = 0x200,
            .uarfcn = 0x10000,
            .plmn_info = {},
            .csg_info = {},
        },
        .sig_str = 32,
        .sig_ber = 8,
        .rscp = 97,
        .ecno = 50,
    };
    CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoWcdma_V1_5 &defaultWcdmaCell = wcdma.CellInfo.wcdma;
    EXPECT_EQ(cellinfo.cellIdentityWcdma.lac, defaultWcdmaCell.cellIdentityWcdma.lac);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.cid, defaultWcdmaCell.cellIdentityWcdma.cid);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.psc, defaultWcdmaCell.cellIdentityWcdma.psc);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.uarfcn, defaultWcdmaCell.cellIdentityWcdma.uarfcn);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.signalStrength, defaultWcdmaCell.signalStrengthWcdma.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.bitErrorRate, defaultWcdmaCell.signalStrengthWcdma.bitErrorRate);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.rscp, (unsigned int)/* NEED TO CHECK */ defaultWcdmaCell.signalStrengthWcdma.rscp);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.ecno, (unsigned int)/* NEED TO CHECK */ defaultWcdmaCell.signalStrengthWcdma.ecno);
}

TEST(CellInfoProtocolUtils, FillWcdmaCellInfo_RIL_CellInfoWcdma_V1_5_cell_info_type_wcdma_v4) {
    TS25Table::MakeInstance();

    RIL_CellInfoWcdma_V1_5 cellinfo {};
    cell_info_type_wcdma_v4 testVector = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 12345,
            .cid = 250,
            .psc = 150,
            .uarfcn = 2200,
            .plmn_info = { "SKT", "SK Telecom", 0, {}, },
            .csg_info = { 1, "test_network1", 1234567, },
        },
        .sig_str = 15,
        .sig_ber = 5,
        .rscp = 80,
        .ecno = 30,
    };
    CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.lac, testVector.cell_identity.lac);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.cid, testVector.cell_identity.cid);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.psc, testVector.cell_identity.psc);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.uarfcn, testVector.cell_identity.uarfcn);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIndication,
            testVector.cell_identity.csg_info.csg_indication);
    EXPECT_TRUE(TextUtils::Equals(cellinfo.cellIdentityWcdma.optionalCsgInfo.csgInfo.homeNodebName,
            testVector.cell_identity.csg_info.hnb_name));
    EXPECT_EQ(cellinfo.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIdentity,
                testVector.cell_identity.csg_info.csg_identity);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.signalStrength, testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.bitErrorRate, testVector.sig_ber);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.rscp, (unsigned int)/* NEED TO CHECK */ testVector.rscp);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.ecno, (unsigned int)/* NEED TO CHECK */ testVector.ecno);

    cell_info_type_wcdma_v4 testVectorInvalid = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 0x10000,
            .cid = 0x10000000,
            .psc = 0x200,
            .uarfcn = 0x10000,
            .plmn_info = {},
            .csg_info = {},
        },
        .sig_str = 32,
        .sig_ber = 8,
        .rscp = 97,
        .ecno = 50,
    };
    CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoWcdma_V1_5 &defaultWcdmaCell = wcdma.CellInfo.wcdma;
    EXPECT_EQ(cellinfo.cellIdentityWcdma.lac, defaultWcdmaCell.cellIdentityWcdma.lac);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.cid, defaultWcdmaCell.cellIdentityWcdma.cid);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.psc, defaultWcdmaCell.cellIdentityWcdma.psc);
    EXPECT_EQ(cellinfo.cellIdentityWcdma.uarfcn, defaultWcdmaCell.cellIdentityWcdma.uarfcn);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.signalStrength, defaultWcdmaCell.signalStrengthWcdma.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.bitErrorRate, defaultWcdmaCell.signalStrengthWcdma.bitErrorRate);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.rscp, (unsigned int)/* NEED TO CHECK */ defaultWcdmaCell.signalStrengthWcdma.rscp);
    EXPECT_EQ(cellinfo.signalStrengthWcdma.ecno, (unsigned int)/* NEED TO CHECK */ defaultWcdmaCell.signalStrengthWcdma.ecno);
}

TEST(CellInfoProtocolUtils, FillTdscdmaCellInfo_RIL_CellInfoTdscdma_V1_5_cell_info_tdscdma) {
    TS25Table::MakeInstance();

    RIL_CellInfoTdscdma_V1_5 cellinfo {};
    cell_info_tdscdma testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 12345,
        .cid = 250,
        .cpid = 75,
        .rscp = 100,
    };
    CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.lac, testVector.lac);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cid, testVector.cid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cpid, testVector.cpid);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.rscp, (unsigned int)/* NEED TO CHECK */ testVector.rscp);

    cell_info_tdscdma testVectorInvalid = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 0x10000,
        .cid = 0x10000000,
        .cpid = 0x100,
        .rscp = 121,
    };
    CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoTdscdma_V1_5 &defaultTdscdmaCell = tdscdma.CellInfo.tdscdma;
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.lac, defaultTdscdmaCell.cellIdentityTdscdma.lac);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cid, defaultTdscdmaCell.cellIdentityTdscdma.cid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cpid, defaultTdscdmaCell.cellIdentityTdscdma.cpid);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.rscp, (unsigned int)/* NEED TO CHECK */ INT_MAX);  // <= IRadio@1.1
}

TEST(CellInfoProtocolUtils, FillTdscdmaCellInfo_RIL_CellInfoTdscdma_V1_5_cell_info_tdscdma_v14) {
    TS25Table::MakeInstance();

    RIL_CellInfoTdscdma_V1_5 cellinfo {};
    cell_info_tdscdma_v14 testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 12345,
        .cid = 250,
        .cpid = 75,
        .sig_str = 25,
        .ber = 3,
        .rscp = 80,
    };
    CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.lac, testVector.lac);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cid, testVector.cid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cpid, testVector.cpid);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.signalStrength, (unsigned int)/* NEED TO CHECK */ testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.bitErrorRate, (unsigned int)/* NEED TO CHECK */ testVector.ber);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.rscp, (unsigned int)/* NEED TO CHECK */ testVector.rscp);

    cell_info_tdscdma_v14 testVectorInvalid = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .lac = 0x10000,
        .cid = 0x10000000,
        .cpid = 0x100,
        .sig_str = 32,
        .ber = 8,
        .rscp = 121,
    };
    CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoTdscdma_V1_5 &defaultTdscdmaCell = tdscdma.CellInfo.tdscdma;
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.lac, defaultTdscdmaCell.cellIdentityTdscdma.lac);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cid, defaultTdscdmaCell.cellIdentityTdscdma.cid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cpid, defaultTdscdmaCell.cellIdentityTdscdma.cpid);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.signalStrength, defaultTdscdmaCell.signalStrengthTdscdma.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.bitErrorRate, defaultTdscdmaCell.signalStrengthTdscdma.bitErrorRate);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.rscp, (unsigned int)/* NEED TO CHECK */ INT_MAX);  // <= IRadio@1.1
}

TEST(CellInfoProtocolUtils, FillTdscdmaCellInfo_RIL_CellInfoTdscdma_V1_5_cell_info_type_tdscdma_v3) {
    TS25Table::MakeInstance();

    RIL_CellInfoTdscdma_V1_5 cellinfo {};
    cell_info_type_tdscdma_v3 testVector = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 12345,
            .cid = 250,
            .cpid = 75,
            .uarfcn = 1500,
            .plmn_info = {},
            .csg_info = {},
        },
        .sig_str = 25,
        .ber = 3,
        .rscp = 80,
    };
    CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.lac, testVector.cell_identity.lac);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cid, testVector.cell_identity.cid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cpid, testVector.cell_identity.cpid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.uarfcn, testVector.cell_identity.uarfcn);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.signalStrength, (unsigned int)/* NEED TO CHECK */ testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.bitErrorRate, (unsigned int)/* NEED TO CHECK */ testVector.ber);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.rscp, (unsigned int)/* NEED TO CHECK */ testVector.rscp);

    cell_info_type_tdscdma_v3 testVectorInvalid = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 0x10000,
            .cid = 0x10000000,
            .cpid = 0x100,
            .uarfcn = 0x10000,
        },
        .sig_str = 32,
        .ber = 8,
        .rscp = 97,
    };
    CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoTdscdma_V1_5 &defaultTdscdmaCell = tdscdma.CellInfo.tdscdma;
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.lac, defaultTdscdmaCell.cellIdentityTdscdma.lac);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cid, defaultTdscdmaCell.cellIdentityTdscdma.cid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cpid, defaultTdscdmaCell.cellIdentityTdscdma.cpid);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.signalStrength, defaultTdscdmaCell.signalStrengthTdscdma.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.bitErrorRate, defaultTdscdmaCell.signalStrengthTdscdma.bitErrorRate);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.rscp, (unsigned int)/* NEED TO CHECK */ defaultTdscdmaCell.signalStrengthTdscdma.rscp);
}

TEST(CellInfoProtocolUtils, FillTdscdmaCellInfo_RIL_CellInfoTdscdma_V1_5_cell_info_type_tdscdma_v4) {
    TS25Table::MakeInstance();

    RIL_CellInfoTdscdma_V1_5 cellinfo {};
    cell_info_type_tdscdma_v4 testVector = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 12345,
            .cid = 250,
            .cpid = 75,
            .uarfcn = 1500,
            .plmn_info = {},
            .csg_info = {},
        },
        .sig_str = 25,
        .ber = 3,
        .rscp = 80,
    };
    CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.lac, testVector.cell_identity.lac);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cid, testVector.cell_identity.cid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cpid, testVector.cell_identity.cpid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.uarfcn, testVector.cell_identity.uarfcn);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.signalStrength, (unsigned int)/* NEED TO CHECK */ testVector.sig_str);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.bitErrorRate, (unsigned int)/* NEED TO CHECK */ testVector.ber);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.rscp, (unsigned int)/* NEED TO CHECK */ testVector.rscp);

    cell_info_type_tdscdma_v4 testVectorInvalid = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .lac = 0x10000,
            .cid = 0x10000000,
            .cpid = 0x100,
            .uarfcn = 0x10000,
        },
        .sig_str = 32,
        .ber = 8,
        .rscp = 97,
    };
    CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoTdscdma_V1_5 &defaultTdscdmaCell = tdscdma.CellInfo.tdscdma;
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.lac, defaultTdscdmaCell.cellIdentityTdscdma.lac);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cid, defaultTdscdmaCell.cellIdentityTdscdma.cid);
    EXPECT_EQ(cellinfo.cellIdentityTdscdma.cpid, defaultTdscdmaCell.cellIdentityTdscdma.cpid);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.signalStrength, defaultTdscdmaCell.signalStrengthTdscdma.signalStrength);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.bitErrorRate, defaultTdscdmaCell.signalStrengthTdscdma.bitErrorRate);
    EXPECT_EQ(cellinfo.signalStrengthTdscdma.rscp, (unsigned int)/* NEED TO CHECK */ defaultTdscdmaCell.signalStrengthTdscdma.rscp);
}

TEST(CellInfoProtocolUtils, FillNrCellInfo_RIL_CellInfoNr_V1_6_cell_info_nr) {
    TS25Table::MakeInstance();

    RIL_CellInfoNr_V1_6 cellinfo {};
    cell_info_nr testVector = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .cell_id = static_cast<ULONG>(1234567),
        .phy_cell_id = 900,
        .tac = 16000000,
        .arfcn = 20000,
        .ss_rsrp = 100,
        .ss_rsrq = 15,
        .ss_sinr = -11,
        .csi_rsrp = 100,
        .csi_rsrq = 15,
        .csi_sinr = -11,
    };
    CellInfoProtocolUtils::Legacy::FillNrCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityNr.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.plmn));
    EXPECT_EQ(cellinfo.cellIdentityNr.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.plmn));
    // Do not use EXPECT_EQ for 64-bit type to support a compatibility on 32 bit machine.
    EXPECT_TRUE(cellinfo.cellIdentityNr.nci == testVector.cell_id);
    EXPECT_EQ(cellinfo.cellIdentityNr.pci, testVector.phy_cell_id);
    EXPECT_EQ(cellinfo.cellIdentityNr.tac, testVector.tac);
    EXPECT_EQ(cellinfo.cellIdentityNr.nrarfcn, testVector.arfcn);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrp, testVector.ss_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrq, testVector.ss_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssSinr, testVector.ss_sinr);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrp, testVector.csi_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrq, testVector.csi_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiSinr, testVector.csi_sinr);

    cell_info_nr testVectorInvalid = {
        .plmn = { '4', '5', '0', '0', '5', '#' },
        .cell_id = static_cast<ULONG>(68719476736),
        .phy_cell_id = 1008,
        .tac = 0x1000000,
        .arfcn = 3279166,
        .ss_rsrp = 141,
        .ss_rsrq = 21,
        .ss_sinr = 41,
        .csi_rsrp = 141,
        .csi_rsrq = 21,
        .csi_sinr = 41,
    };
    CellInfoProtocolUtils::Legacy::FillNrCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoNr_V1_6 &defaultNrCell = nr.CellInfo.nr;
    EXPECT_EQ(cellinfo.cellIdentityNr.nci, defaultNrCell.cellIdentityNr.nci);
    EXPECT_EQ(cellinfo.cellIdentityNr.pci, defaultNrCell.cellIdentityNr.pci);
    EXPECT_EQ(cellinfo.cellIdentityNr.tac, defaultNrCell.cellIdentityNr.tac);
    EXPECT_EQ(cellinfo.cellIdentityNr.nrarfcn, defaultNrCell.cellIdentityNr.nrarfcn);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrp, defaultNrCell.signalStrengthNr.ssRsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrq, defaultNrCell.signalStrengthNr.ssRsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssSinr, defaultNrCell.signalStrengthNr.ssSinr);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrp, defaultNrCell.signalStrengthNr.csiRsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrq, defaultNrCell.signalStrengthNr.csiRsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiSinr, defaultNrCell.signalStrengthNr.csiSinr);
}

TEST(CellInfoProtocolUtils, FillNrCellInfo_RIL_CellInfoNr_V1_6_cell_info_type_nr_v3) {
    TS25Table::MakeInstance();

    RIL_CellInfoNr_V1_6 cellinfo {};
    cell_info_type_nr_v3 testVector = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .cell_id = static_cast<ULONG>(1234567),
            .phy_cell_id = 900,
            .tac = 16000000,
            .arfcn = 20000,
            .plmn_info = { "SKT", "SK Telecom", 0, {}, },
            .csg_info = { 1, "test_network1", 1234567, },
            .band_len = 3,
            .band_info = { 1, 2, 3, },
        },
        .ss_rsrp = 100,
        .ss_rsrq = 15,
        .ss_sinr = -11,
        .csi_rsrp = 100,
        .csi_rsrq = 15,
        .csi_sinr = -11,
    };
    CellInfoProtocolUtils::Legacy::FillNrCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityNr.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityNr.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    // Do not use EXPECT_EQ for 64-bit type to support a compatibility on 32 bit machine.
    EXPECT_TRUE(cellinfo.cellIdentityNr.nci == testVector.cell_identity.cell_id);
    EXPECT_EQ(cellinfo.cellIdentityNr.pci, testVector.cell_identity.phy_cell_id);
    EXPECT_EQ(cellinfo.cellIdentityNr.tac, testVector.cell_identity.tac);
    EXPECT_EQ(cellinfo.cellIdentityNr.nrarfcn, testVector.cell_identity.arfcn);
    EXPECT_EQ(cellinfo.cellIdentityNr.leng_bands, testVector.cell_identity.band_len);
    for (int i = 0; i < cellinfo.cellIdentityNr.leng_bands; i++) {
        EXPECT_EQ(cellinfo.cellIdentityNr.bands[i], testVector.cell_identity.band_info[i]);
    }
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrp, testVector.ss_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrq, testVector.ss_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssSinr, testVector.ss_sinr);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrp, testVector.csi_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrq, testVector.csi_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiSinr, testVector.csi_sinr);

    cell_info_type_nr_v3 testVectorInvalid = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .cell_id = static_cast<ULONG>(68719476736),
            .phy_cell_id = 1008,
            .tac = 0x1000000,
            .arfcn = 3279166,
        },
        .ss_rsrp = 141,
        .ss_rsrq = 21,
        .ss_sinr = 41,
        .csi_rsrp = 141,
        .csi_rsrq = 21,
        .csi_sinr = 41,
    };
    CellInfoProtocolUtils::Legacy::FillNrCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoNr_V1_6 &defaultNrCell = nr.CellInfo.nr;
    // Do not use EXPECT_EQ for 64-bit type to support a compatibility on 32 bit machine.
    EXPECT_TRUE(cellinfo.cellIdentityNr.nci == defaultNrCell.cellIdentityNr.nci);
    EXPECT_EQ(cellinfo.cellIdentityNr.pci, defaultNrCell.cellIdentityNr.pci);
    EXPECT_EQ(cellinfo.cellIdentityNr.tac, defaultNrCell.cellIdentityNr.tac);
    EXPECT_EQ(cellinfo.cellIdentityNr.nrarfcn, defaultNrCell.cellIdentityNr.nrarfcn);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrp, defaultNrCell.signalStrengthNr.ssRsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrq, defaultNrCell.signalStrengthNr.ssRsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssSinr, defaultNrCell.signalStrengthNr.ssSinr);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrp, defaultNrCell.signalStrengthNr.csiRsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrq, defaultNrCell.signalStrengthNr.csiRsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiSinr, defaultNrCell.signalStrengthNr.csiSinr);
}

TEST(CellInfoProtocolUtils, FillNrCellInfo_RIL_CellInfoNr_V1_6_cell_info_type_nr_v4) {
    TS25Table::MakeInstance();

    RIL_CellInfoNr_V1_6 cellinfo {};
    cell_info_type_nr_v4 testVector = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .cell_id = static_cast<ULONG>(1234567),
            .phy_cell_id = 900,
            .tac = 16000000,
            .arfcn = 20000,
            .plmn_info = { "SKT", "SK Telecom", 0, {}, },
            .csg_info = { 1, "test_network1", 1234567, },
            .band_len = 3,
            .band_info = { 1, 2, 3, },
        },
        .ss_rsrp = 100,
        .ss_rsrq = 15,
        .ss_sinr = -11,
        .csi_rsrp = 100,
        .csi_rsrq = 15,
        .csi_sinr = -11,
        .cqi_table_index = 2,
        .wb_cqi = 3,
        .sb_cqi = {},
    };
    CellInfoProtocolUtils::Legacy::FillNrCellInfo(cellinfo, testVector);
    EXPECT_EQ(cellinfo.cellIdentityNr.mcc, CellInfoProtocolUtils::Legacy::FetchMcc(testVector.cell_identity.plmn));
    EXPECT_EQ(cellinfo.cellIdentityNr.mnc, CellInfoProtocolUtils::Legacy::FetchMnc(testVector.cell_identity.plmn));
    // Do not use EXPECT_EQ for 64-bit type to support a compatibility on 32 bit machine.
    EXPECT_TRUE(cellinfo.cellIdentityNr.nci == testVector.cell_identity.cell_id);
    EXPECT_EQ(cellinfo.cellIdentityNr.pci, testVector.cell_identity.phy_cell_id);
    EXPECT_EQ(cellinfo.cellIdentityNr.tac, testVector.cell_identity.tac);
    EXPECT_EQ(cellinfo.cellIdentityNr.nrarfcn, testVector.cell_identity.arfcn);
    EXPECT_EQ(cellinfo.cellIdentityNr.leng_bands, testVector.cell_identity.band_len);
    for (int i = 0; i < cellinfo.cellIdentityNr.leng_bands; i++) {
        EXPECT_EQ(cellinfo.cellIdentityNr.bands[i], testVector.cell_identity.band_info[i]);
    }
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrp, testVector.ss_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrq, testVector.ss_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssSinr, testVector.ss_sinr);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrp, testVector.csi_rsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrq, testVector.csi_rsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiSinr, testVector.csi_sinr);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiCqiTableIndex, (unsigned int)/* NEED TO CHECK */ testVector.cqi_table_index);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiCqiReportLen, 1);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiCqiReport[0], (unsigned int)/* NEED TO CHECK */ testVector.wb_cqi);

    cell_info_type_nr_v4 testVector2 = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .cell_id = static_cast<ULONG>(1234567),
            .phy_cell_id = 900,
            .tac = 16000000,
            .arfcn = 20000,
            .plmn_info = { "SKT", "SK Telecom", 0, {}, },
            .csg_info = { 1, "test_network1", 1234567, },
            .band_len = 3,
            .band_info = { 1, 2, 3, },
        },
        .ss_rsrp = 100,
        .ss_rsrq = 15,
        .ss_sinr = -11,
        .csi_rsrp = 100,
        .csi_rsrq = 15,
        .csi_sinr = -11,
        .cqi_table_index = 2,
        .wb_cqi = INT_MAX,
        .sb_cqi = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2 },
    };
    CellInfoProtocolUtils::Legacy::FillNrCellInfo(cellinfo, testVector2);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiCqiReportLen, 19);
    for (int i = 0; i < cellinfo.signalStrengthNr.csiCqiReportLen; i++) {
        EXPECT_EQ(cellinfo.signalStrengthNr.csiCqiReport[i], (unsigned int)/* NEED TO CHECK */ testVector2.sb_cqi[i]);
    }

    cell_info_type_nr_v4 testVectorInvalid = {
        .cell_identity = {
            .plmn = { '4', '5', '0', '0', '5', '#' },
            .cell_id = static_cast<ULONG>(68719476736),
            .phy_cell_id = 1008,
            .tac = 0x1000000,
            .arfcn = 3279166,
        },
        .ss_rsrp = 141,
        .ss_rsrq = 21,
        .ss_sinr = 41,
        .csi_rsrp = 141,
        .csi_rsrq = 21,
        .csi_sinr = 41,
    };
    CellInfoProtocolUtils::Legacy::FillNrCellInfo(cellinfo, testVectorInvalid);
    RIL_CellInfoNr_V1_6 &defaultNrCell = nr.CellInfo.nr;
    // Do not use EXPECT_EQ for 64-bit type to support a compatibility on 32 bit machine.
    EXPECT_TRUE(cellinfo.cellIdentityNr.nci == defaultNrCell.cellIdentityNr.nci);
    EXPECT_EQ(cellinfo.cellIdentityNr.pci, defaultNrCell.cellIdentityNr.pci);
    EXPECT_EQ(cellinfo.cellIdentityNr.tac, defaultNrCell.cellIdentityNr.tac);
    EXPECT_EQ(cellinfo.cellIdentityNr.nrarfcn, defaultNrCell.cellIdentityNr.nrarfcn);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrp, defaultNrCell.signalStrengthNr.ssRsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssRsrq, defaultNrCell.signalStrengthNr.ssRsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.ssSinr, defaultNrCell.signalStrengthNr.ssSinr);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrp, defaultNrCell.signalStrengthNr.csiRsrp);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiRsrq, defaultNrCell.signalStrengthNr.csiRsrq);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiSinr, defaultNrCell.signalStrengthNr.csiSinr);
    EXPECT_EQ(cellinfo.signalStrengthNr.csiCqiTableIndex, defaultNrCell.signalStrengthNr.csiCqiTableIndex);
}

/*
 * Below functions can be tested by the previous cell info tests
    void FillCellIdentityGsm(RIL_CellIdentityGsm_V1_5& out, cell_identity_gsm_v3& cellIdentity);
    void FillCellIdentityGsm(RIL_CellIdentityGsm_V1_5& out, cell_identity_gsm_v4& cellIdentity);
    void FillCellIdentityCdma(RIL_CellIdentityCdma_V1_2& out, cell_identity_cdma_v3& cellIdentity);
    void FillCellIdentityCdma(RIL_CellIdentityCdma_V1_2& out, cell_identity_cdma_v4& cellIdentity);
    void FillCellIdentityLte(RIL_CellIdentityLte_V1_5& out, cell_identity_lte_v3& cellIdentity);
    void FillCellIdentityLte(RIL_CellIdentityLte_V1_5& out, cell_identity_lte_v4& cellIdentity);
    void FillCellIdentityWcdma(RIL_CellIdentityWcdma_V1_5& out, cell_identity_wcdma_v3& cellIdentity);
    void FillCellIdentityWcdma(RIL_CellIdentityWcdma_V1_5& out, cell_identity_wcdma_v4& cellIdentity);
    void FillCellIdentityTdscdma(RIL_CellIdentityTdscdma_V1_5& out, cell_identity_tdscdma_v3& cellIdentity);
    void FillCellIdentityTdscdma(RIL_CellIdentityTdscdma_V1_5& out, cell_identity_tdscdma_v4& cellIdentity);
    void FillCellIdentityNr(RIL_CellIdentityNr_V1_5& out, cell_identity_nr_v3& cellIdentity);
    void FillCellIdentityNr(RIL_CellIdentityNr_V1_5& out, cell_identity_nr_v4& cellIdentity);
 */
