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
 * radio_hal_utils_test_api.cpp
 *
 *  Created on: 2021. 8. 5.
 */
#include "radio_hal_utils_test_api.h"
#include <gtest/gtest.h>
#include <telephony/ril_mcc.h>
#include <telephony/ril_mnc.h>

void testCellIdentityOperatorNames(V1_2::CellIdentityOperatorNames& operatorNames,
        RIL_CellIdentityOperatorNames& rilOperatorNames) {
    EXPECT_TRUE(operatorNames.alphaLong == rilOperatorNames.alphaLong);
    EXPECT_TRUE(operatorNames.alphaShort == rilOperatorNames.alphaShort);
}

void testCellIdentityAdditionalPlmns(hidl_vec<hidl_string>& additionalPlmns,
        size_t leng_plmns, char **plmns) {
    EXPECT_EQ(additionalPlmns.size(), leng_plmns);
    if (additionalPlmns.size() > 0) {
        EXPECT_NE(plmns, nullptr);
        for (size_t i = 0; i < additionalPlmns.size(); i++) {
            EXPECT_NE(plmns[i], nullptr);
            if (plmns[i]) {
                EXPECT_TRUE(additionalPlmns[i] == plmns[i]);
            }
        }
    }
}

void testCellIdentityGsm(V1_0::CellIdentityGsm& cellIdentityGsm,
        RIL_CellIdentityGsm_v12& rilCellIdentityGsm) {
    EXPECT_TRUE(cellIdentityGsm.mcc == ril::util::mcc::decode(rilCellIdentityGsm.mcc));
    EXPECT_TRUE(cellIdentityGsm.mnc == ril::util::mnc::decode(rilCellIdentityGsm.mnc));
    EXPECT_EQ(cellIdentityGsm.lac, rilCellIdentityGsm.lac);
    EXPECT_EQ(cellIdentityGsm.cid, rilCellIdentityGsm.cid);
    EXPECT_EQ(cellIdentityGsm.arfcn, rilCellIdentityGsm.arfcn);
    EXPECT_EQ(cellIdentityGsm.bsic, rilCellIdentityGsm.bsic);
}

void testCellIdentityGsm(V1_2::CellIdentityGsm& cellIdentityGsm,
        RIL_CellIdentityGsm_V1_2& rilCellIdentityGsm) {
    testCellIdentityGsm(cellIdentityGsm.base, *(RIL_CellIdentityGsm_v12 *)&rilCellIdentityGsm);
    testCellIdentityOperatorNames(cellIdentityGsm.operatorNames, rilCellIdentityGsm.operatorNames);
}

void testCellIdentityGsm(V1_5::CellIdentityGsm& cellIdentityGsm,
        RIL_CellIdentityGsm_V1_5& rilCellIdentityGsm) {
    testCellIdentityGsm(cellIdentityGsm.base, *(RIL_CellIdentityGsm_V1_2 *)&rilCellIdentityGsm);
    testCellIdentityAdditionalPlmns(cellIdentityGsm.additionalPlmns,
            rilCellIdentityGsm.leng_additionalPlmns, rilCellIdentityGsm.additionalPlmns);
}

void testCellIdentityWcdma(V1_0::CellIdentityWcdma& cellIdentityWcdma,
        RIL_CellIdentityWcdma_v12& rilCellIdentityWcdma) {
    EXPECT_TRUE(cellIdentityWcdma.mcc == ril::util::mcc::decode(rilCellIdentityWcdma.mcc));
    EXPECT_TRUE(cellIdentityWcdma.mnc == ril::util::mnc::decode(rilCellIdentityWcdma.mnc));
    EXPECT_EQ(cellIdentityWcdma.lac, rilCellIdentityWcdma.lac);
    EXPECT_EQ(cellIdentityWcdma.cid, rilCellIdentityWcdma.cid);
    EXPECT_EQ(cellIdentityWcdma.psc, rilCellIdentityWcdma.psc);
    EXPECT_EQ(cellIdentityWcdma.uarfcn, rilCellIdentityWcdma.uarfcn);
}

void testCellIdentityWcdma(V1_2::CellIdentityWcdma& cellIdentityWcdma,
        RIL_CellIdentityWcdma_V1_2& rilCellIdentityWcdma) {
    testCellIdentityWcdma(cellIdentityWcdma.base, *(RIL_CellIdentityWcdma_v12 *)&rilCellIdentityWcdma);
    testCellIdentityOperatorNames(cellIdentityWcdma.operatorNames, rilCellIdentityWcdma.operatorNames);
}

void testCellIdentityWcdma(V1_5::CellIdentityWcdma& cellIdentityWcdma,
        RIL_CellIdentityWcdma_V1_5& rilCellIdentityWcdma) {
    testCellIdentityWcdma(cellIdentityWcdma.base, *(RIL_CellIdentityWcdma_V1_2 *)&rilCellIdentityWcdma);
    testCellIdentityAdditionalPlmns(cellIdentityWcdma.additionalPlmns,
            rilCellIdentityWcdma.leng_additionalPlmns, rilCellIdentityWcdma.additionalPlmns);
}

void testCellIdentityCdma(V1_0::CellIdentityCdma& cellIdentityCdma,
        RIL_CellIdentityCdma& rilCellIdentityCdma) {
    EXPECT_EQ(cellIdentityCdma.networkId, rilCellIdentityCdma.networkId);
    EXPECT_EQ(cellIdentityCdma.systemId, rilCellIdentityCdma.systemId);
    EXPECT_EQ(cellIdentityCdma.baseStationId, rilCellIdentityCdma.basestationId);
    EXPECT_EQ(cellIdentityCdma.longitude, rilCellIdentityCdma.longitude);
    EXPECT_EQ(cellIdentityCdma.latitude, rilCellIdentityCdma.latitude);
}

void testCellIdentityCdma(V1_2::CellIdentityCdma& cellIdentityCdma,
        RIL_CellIdentityCdma_V1_2& rilCellIdentityCdma) {
    testCellIdentityCdma(cellIdentityCdma.base, *(RIL_CellIdentityCdma *)&rilCellIdentityCdma);
    testCellIdentityOperatorNames(cellIdentityCdma.operatorNames, rilCellIdentityCdma.operatorNames);
}

void testCellIdentityLte(V1_0::CellIdentityLte& cellIdentityLte,
        RIL_CellIdentityLte_v12& rilCellIdentityLte) {
    EXPECT_TRUE(cellIdentityLte.mcc == ril::util::mcc::decode(rilCellIdentityLte.mcc));
    EXPECT_TRUE(cellIdentityLte.mnc == ril::util::mnc::decode(rilCellIdentityLte.mnc));
    EXPECT_EQ(cellIdentityLte.ci, rilCellIdentityLte.ci);
    EXPECT_EQ(cellIdentityLte.pci, rilCellIdentityLte.pci);
    EXPECT_EQ(cellIdentityLte.tac, rilCellIdentityLte.tac);
    EXPECT_EQ(cellIdentityLte.earfcn, rilCellIdentityLte.earfcn);
}

void testCellIdentityLte(V1_2::CellIdentityLte& cellIdentityLte,
        RIL_CellIdentityLte_V1_2& rilCellIdentityLte) {
    testCellIdentityLte(cellIdentityLte.base, *(RIL_CellIdentityLte_v12 *)&rilCellIdentityLte);
    testCellIdentityOperatorNames(cellIdentityLte.operatorNames, rilCellIdentityLte.operatorNames);
    EXPECT_EQ(cellIdentityLte.bandwidth, rilCellIdentityLte.bandwidth);
}

void testCellIdentityLte(V1_5::CellIdentityLte& cellIdentityLte,
        RIL_CellIdentityLte_V1_5& rilCellIdentityLte) {
    testCellIdentityLte(cellIdentityLte.base, *(RIL_CellIdentityLte_V1_2 *)&rilCellIdentityLte);
    testCellIdentityAdditionalPlmns(cellIdentityLte.additionalPlmns,
            rilCellIdentityLte.leng_additionalPlmns, rilCellIdentityLte.additionalPlmns);
    EXPECT_EQ((int)cellIdentityLte.bands.size(), rilCellIdentityLte.leng_bands);
    for (size_t i = 0; i < cellIdentityLte.bands.size(); i++) {
        EXPECT_EQ((int)cellIdentityLte.bands[i], rilCellIdentityLte.bands[i]);
    }
}

void testCellIdentityTdscdma(V1_0::CellIdentityTdscdma& cellIdentityTdscdma,
        RIL_CellIdentityTdscdma& rilCellIdentityTdscdma) {
    EXPECT_TRUE(cellIdentityTdscdma.mcc == ril::util::mcc::decode(rilCellIdentityTdscdma.mcc));
    EXPECT_TRUE(cellIdentityTdscdma.mnc == ril::util::mnc::decode(rilCellIdentityTdscdma.mnc));
    EXPECT_EQ(cellIdentityTdscdma.lac, rilCellIdentityTdscdma.lac);
    EXPECT_EQ(cellIdentityTdscdma.cid, rilCellIdentityTdscdma.cid);
    EXPECT_EQ(cellIdentityTdscdma.cpid, rilCellIdentityTdscdma.cpid);
}

void testCellIdentityTdscdma(V1_2::CellIdentityTdscdma& cellIdentityTdscdma,
        RIL_CellIdentityTdscdma_V1_2& rilCellIdentityTdscdma) {
    testCellIdentityTdscdma(cellIdentityTdscdma.base, *(RIL_CellIdentityTdscdma *)&rilCellIdentityTdscdma);
    EXPECT_EQ(cellIdentityTdscdma.uarfcn, rilCellIdentityTdscdma.uarfcn);
    testCellIdentityOperatorNames(cellIdentityTdscdma.operatorNames, rilCellIdentityTdscdma.operatorNames);
}

void testCellIdentityTdscdma(V1_5::CellIdentityTdscdma& cellIdentityTdscdma,
        RIL_CellIdentityTdscdma_V1_5& rilCellIdentityTdscdma) {
    testCellIdentityTdscdma(cellIdentityTdscdma.base, *(RIL_CellIdentityTdscdma_V1_2 *)&rilCellIdentityTdscdma);
    testCellIdentityAdditionalPlmns(cellIdentityTdscdma.additionalPlmns,
            rilCellIdentityTdscdma.leng_additionalPlmns, rilCellIdentityTdscdma.additionalPlmns);
}

void testCellIdentityNr(V1_4::CellIdentityNr& cellIdentityNr,
        RIL_CellIdentityNr_V1_4& rilCellIdentityNr) {
    EXPECT_TRUE(cellIdentityNr.mcc == ril::util::mcc::decode(rilCellIdentityNr.mcc));
    EXPECT_TRUE(cellIdentityNr.mnc == ril::util::mnc::decode(rilCellIdentityNr.mnc));
    EXPECT_TRUE(cellIdentityNr.nci == rilCellIdentityNr.nci);
    EXPECT_EQ(cellIdentityNr.pci, rilCellIdentityNr.pci);
    EXPECT_EQ(cellIdentityNr.tac, rilCellIdentityNr.tac);
    EXPECT_EQ(cellIdentityNr.nrarfcn, rilCellIdentityNr.nrarfcn);
    testCellIdentityOperatorNames(cellIdentityNr.operatorNames, rilCellIdentityNr.operatorNames);
}

void testCellIdentityNr(V1_5::CellIdentityNr& cellIdentityNr,
        RIL_CellIdentityNr_V1_5& rilCellIdentityNr) {
    testCellIdentityNr(cellIdentityNr.base, *(RIL_CellIdentityNr_V1_4 *)&rilCellIdentityNr);
    testCellIdentityAdditionalPlmns(cellIdentityNr.additionalPlmns,
            rilCellIdentityNr.leng_additionalPlmns, rilCellIdentityNr.additionalPlmns);
    EXPECT_EQ((int)cellIdentityNr.bands.size(), rilCellIdentityNr.leng_bands);
    for (size_t i = 0; i < cellIdentityNr.bands.size(); i++) {
        EXPECT_EQ((int)cellIdentityNr.bands[i], rilCellIdentityNr.bands[i]);
    }
}

void testGsmSignalStrength(V1_0::GsmSignalStrength& gw,
        RIL_GW_SignalStrength& GW_SignalStrength) {
    EXPECT_EQ((int)gw.signalStrength, GW_SignalStrength.signalStrength);
    EXPECT_EQ((int)gw.bitErrorRate, GW_SignalStrength.bitErrorRate);
    EXPECT_EQ(gw.timingAdvance, INT_MAX);
}

void testGsmSignalStrength(V1_0::GsmSignalStrength& gsm,
        RIL_GSM_SignalStrength_v12& signalStrengthGsm) {
    EXPECT_EQ((int)gsm.signalStrength, signalStrengthGsm.signalStrength);
    EXPECT_EQ((int)gsm.bitErrorRate, signalStrengthGsm.bitErrorRate);
    EXPECT_EQ(gsm.timingAdvance, signalStrengthGsm.timingAdvance);
}

void testCdmaSignalStrength(V1_0::CdmaSignalStrength& cdma,
        RIL_CDMA_SignalStrength& CDMA_SignalStrength) {
    EXPECT_EQ((int)cdma.dbm, CDMA_SignalStrength.dbm);
    EXPECT_EQ((int)cdma.ecio, CDMA_SignalStrength.ecio);
}

void testEvdoSignalStrength(V1_0::EvdoSignalStrength& evdo,
        RIL_EVDO_SignalStrength& EVDO_SignalStrength) {
    EXPECT_EQ((int)evdo.dbm, EVDO_SignalStrength.dbm);
    EXPECT_EQ((int)evdo.ecio, EVDO_SignalStrength.ecio);
    EXPECT_EQ((int)evdo.signalNoiseRatio, EVDO_SignalStrength.signalNoiseRatio);
}

void testLteSignalStrength(V1_0::LteSignalStrength& lte,
        RIL_LTE_SignalStrength_v8& LTE_SignalStrength) {
    EXPECT_EQ((int)lte.signalStrength, LTE_SignalStrength.signalStrength);
    EXPECT_EQ((int)lte.rsrp, LTE_SignalStrength.rsrp);
    EXPECT_EQ((int)lte.rsrq, LTE_SignalStrength.rsrq);
    EXPECT_EQ((int)lte.rssnr, LTE_SignalStrength.rssnr);
    EXPECT_EQ((int)lte.cqi, LTE_SignalStrength.cqi);
    EXPECT_EQ((int)lte.timingAdvance, LTE_SignalStrength.timingAdvance);
}

void testTdScdmaSignalStrength(V1_0::TdScdmaSignalStrength& tdScdma,
        RIL_TD_SCDMA_SignalStrength& TD_SCDMA_SignalStrength) {
    EXPECT_EQ((int)tdScdma.rscp, TD_SCDMA_SignalStrength.rscp);
}

void testTdScdmaSignalStrength(V1_2::TdscdmaSignalStrength& tdscdma,
        RIL_TD_SCDMA_SignalStrength_V1_2& signalStrengthTdscdma) {
    EXPECT_EQ(tdscdma.signalStrength, signalStrengthTdscdma.signalStrength);
    EXPECT_EQ(tdscdma.bitErrorRate, signalStrengthTdscdma.bitErrorRate);
    EXPECT_EQ(tdscdma.rscp, signalStrengthTdscdma.rscp);
}

void testWcdmaSignalStrength(V1_0::WcdmaSignalStrength& wcdma,
        RIL_SignalStrengthWcdma& signalStrengthWcdma) {
    EXPECT_EQ(wcdma.signalStrength, signalStrengthWcdma.signalStrength);
    EXPECT_EQ(wcdma.bitErrorRate, signalStrengthWcdma.bitErrorRate);
}

void testWcdmaSignalStrength(V1_2::WcdmaSignalStrength& wcdma,
        RIL_WCDMA_SignalStrength_V1_2& signalStrengthWcdma) {
    testWcdmaSignalStrength(wcdma.base, *(RIL_SignalStrengthWcdma *)&signalStrengthWcdma);
    EXPECT_EQ(wcdma.rscp, signalStrengthWcdma.rscp);
    EXPECT_EQ(wcdma.ecno, signalStrengthWcdma.ecno);
}

void testNrSignalStrength(V1_4::NrSignalStrength& nr,
        RIL_NR_SignalStrength_V1_4& signalStrengthNr) {
    EXPECT_EQ(nr.ssRsrp, signalStrengthNr.ssRsrp);
    EXPECT_EQ(nr.ssRsrq, signalStrengthNr.ssRsrq);
    EXPECT_EQ(nr.ssSinr, signalStrengthNr.ssSinr);
    EXPECT_EQ(nr.csiRsrp, signalStrengthNr.csiRsrp);
    EXPECT_EQ(nr.csiRsrq, signalStrengthNr.csiRsrq);
    EXPECT_EQ(nr.csiSinr, signalStrengthNr.csiSinr);
}

void testCellInfoGsm(V1_0::CellInfoGsm& gsm, RIL_CellInfoGsm_v12& rilCellInfoGsm) {
    testCellIdentityGsm(gsm.cellIdentityGsm, rilCellInfoGsm.cellIdentityGsm);
    testGsmSignalStrength(gsm.signalStrengthGsm, rilCellInfoGsm.signalStrengthGsm);
}

void testCellInfoGsm(V1_2::CellInfoGsm& gsm, RIL_CellInfoGsm_V1_2& rilCellInfoGsm) {
    testCellIdentityGsm(gsm.cellIdentityGsm, rilCellInfoGsm.cellIdentityGsm);
    testGsmSignalStrength(gsm.signalStrengthGsm, rilCellInfoGsm.signalStrengthGsm);
}

void testCellInfoGsm(V1_5::CellInfoGsm& gsm, RIL_CellInfoGsm_V1_5& rilCellInfoGsm) {
    testCellIdentityGsm(gsm.cellIdentityGsm, rilCellInfoGsm.cellIdentityGsm);
    testGsmSignalStrength(gsm.signalStrengthGsm, rilCellInfoGsm.signalStrengthGsm);
}

void testCellInfoCdma(V1_0::CellInfoCdma& cdma, RIL_CellInfoCdma& rilCellInfoCdma) {
    testCellIdentityCdma(cdma.cellIdentityCdma, rilCellInfoCdma.cellIdentityCdma);
    testCdmaSignalStrength(cdma.signalStrengthCdma, rilCellInfoCdma.signalStrengthCdma);
    testEvdoSignalStrength(cdma.signalStrengthEvdo, rilCellInfoCdma.signalStrengthEvdo);
}

void testCellInfoCdma(V1_2::CellInfoCdma& cdma, RIL_CellInfoCdma_V1_2& rilCellInfoCdma) {
    testCellIdentityCdma(cdma.cellIdentityCdma, rilCellInfoCdma.cellIdentityCdma);
    testCdmaSignalStrength(cdma.signalStrengthCdma, rilCellInfoCdma.signalStrengthCdma);
    testEvdoSignalStrength(cdma.signalStrengthEvdo, rilCellInfoCdma.signalStrengthEvdo);
}

void testCellInfoLte(V1_0::CellInfoLte& lte, RIL_CellInfoLte_v12& rilCellInfoLte) {
    testCellIdentityLte(lte.cellIdentityLte, rilCellInfoLte.cellIdentityLte);
    testLteSignalStrength(lte.signalStrengthLte, rilCellInfoLte.signalStrengthLte);
}

void testCellInfoLte(V1_2::CellInfoLte& lte, RIL_CellInfoLte_V1_2& rilCellInfoLte) {
    testCellIdentityLte(lte.cellIdentityLte, rilCellInfoLte.cellIdentityLte);
    testLteSignalStrength(lte.signalStrengthLte, rilCellInfoLte.signalStrengthLte);
}

void testCellInfoLte(V1_4::CellInfoLte& lte, RIL_CellInfoLte_V1_4& rilCellInfoLte) {
    testCellInfoLte(lte.base, *(RIL_CellInfoLte_V1_2 *)&rilCellInfoLte);
    EXPECT_EQ(lte.cellConfig.isEndcAvailable, rilCellInfoLte.cellConfig.isEndcAvailable);
}

void testCellInfoLte(V1_5::CellInfoLte& lte, RIL_CellInfoLte_V1_5& rilCellInfoLte) {
    testCellIdentityLte(lte.cellIdentityLte, rilCellInfoLte.cellIdentityLte);
    testLteSignalStrength(lte.signalStrengthLte, rilCellInfoLte.signalStrengthLte);
}

void testCellInfoWcdma(V1_0::CellInfoWcdma& wcdma, RIL_CellInfoWcdma_v12& rilCellInfoWcdma) {
    testCellIdentityWcdma(wcdma.cellIdentityWcdma, rilCellInfoWcdma.cellIdentityWcdma);
    testWcdmaSignalStrength(wcdma.signalStrengthWcdma, rilCellInfoWcdma.signalStrengthWcdma);
}

void testCellInfoWcdma(V1_2::CellInfoWcdma& wcdma, RIL_CellInfoWcdma_V1_2& rilCellInfoWcdma) {
    testCellIdentityWcdma(wcdma.cellIdentityWcdma, rilCellInfoWcdma.cellIdentityWcdma);
    testWcdmaSignalStrength(wcdma.signalStrengthWcdma, rilCellInfoWcdma.signalStrengthWcdma);
}

void testCellInfoWcdma(V1_5::CellInfoWcdma& wcdma, RIL_CellInfoWcdma_V1_5& rilCellInfoWcdma) {
    testCellIdentityWcdma(wcdma.cellIdentityWcdma, rilCellInfoWcdma.cellIdentityWcdma);
    testWcdmaSignalStrength(wcdma.signalStrengthWcdma, rilCellInfoWcdma.signalStrengthWcdma);
}

void testCellInfoTdscdma(V1_0::CellInfoTdscdma& tdscdma, RIL_CellInfoTdscdma& rilCellInfoTdscdma) {
    testCellIdentityTdscdma(tdscdma.cellIdentityTdscdma, rilCellInfoTdscdma.cellIdentityTdscdma);
    testTdScdmaSignalStrength(tdscdma.signalStrengthTdscdma, rilCellInfoTdscdma.signalStrengthTdscdma);
}

void testCellInfoTdscdma(V1_2::CellInfoTdscdma& tdscdma, RIL_CellInfoTdscdma_V1_2& rilCellInfoTdscdma) {
    testCellIdentityTdscdma(tdscdma.cellIdentityTdscdma, rilCellInfoTdscdma.cellIdentityTdscdma);
    testTdScdmaSignalStrength(tdscdma.signalStrengthTdscdma, rilCellInfoTdscdma.signalStrengthTdscdma);
}

void testCellInfoTdscdma(V1_5::CellInfoTdscdma& tdscdma, RIL_CellInfoTdscdma_V1_5& rilCellInfoTdscdma) {
    testCellIdentityTdscdma(tdscdma.cellIdentityTdscdma, rilCellInfoTdscdma.cellIdentityTdscdma);
    testTdScdmaSignalStrength(tdscdma.signalStrengthTdscdma, rilCellInfoTdscdma.signalStrengthTdscdma);
}

void testCellInfoNr(V1_4::CellInfoNr& nr, RIL_CellInfoNr_V1_4& rilCellInfoNr) {
    testCellIdentityNr(nr.cellidentity, rilCellInfoNr.cellidentityNr);
    testNrSignalStrength(nr.signalStrength, rilCellInfoNr.signalStrengthNr);
}

void testCellInfoNr(V1_5::CellInfoNr& nr, RIL_CellInfoNr_V1_5& rilCellInfoNr) {
    testCellIdentityNr(nr.cellIdentityNr, rilCellInfoNr.cellIdentityNr);
    testNrSignalStrength(nr.signalStrengthNr, rilCellInfoNr.signalStrengthNr);
}


void testCellInfo(V1_0::CellInfo& cellInfo, RIL_CellInfo_v12& rilCellInfo) {
    EXPECT_EQ((int)cellInfo.cellInfoType, rilCellInfo.cellInfoType);
    EXPECT_EQ((int)cellInfo.registered, rilCellInfo.registered);
    EXPECT_EQ((int)cellInfo.timeStampType, rilCellInfo.timeStampType);
    EXPECT_TRUE(cellInfo.timeStamp == rilCellInfo.timeStamp);

    switch (cellInfo.cellInfoType) {
    case V1_0::CellInfoType::GSM:
        EXPECT_EQ((int)cellInfo.gsm.size(), 1);
        if (cellInfo.gsm.size() > 0) {
            testCellInfoGsm(cellInfo.gsm[0], rilCellInfo.CellInfo.gsm);
        }
        break;
    case V1_0::CellInfoType::CDMA:
        EXPECT_EQ((int)cellInfo.cdma.size(), 1);
        if (cellInfo.cdma.size() > 0) {
            testCellInfoCdma(cellInfo.cdma[0], rilCellInfo.CellInfo.cdma);
        }
        break;
    case V1_0::CellInfoType::LTE:
        EXPECT_EQ((int)cellInfo.lte.size(), 1);
        if (cellInfo.lte.size() > 0) {
            testCellInfoLte(cellInfo.lte[0], rilCellInfo.CellInfo.lte);
        }
        break;
    case V1_0::CellInfoType::WCDMA :
        EXPECT_EQ((int)cellInfo.wcdma.size(), 1);
        if (cellInfo.wcdma.size() > 0) {
            testCellInfoWcdma(cellInfo.wcdma[0], rilCellInfo.CellInfo.wcdma);
        }
        break;
    case V1_0::CellInfoType::TD_SCDMA:
        EXPECT_EQ((int)cellInfo.tdscdma.size(), 1);
        if (cellInfo.tdscdma.size() > 0) {
            testCellInfoTdscdma(cellInfo.tdscdma[0], rilCellInfo.CellInfo.tdscdma);
        }
        break;
    case V1_0::CellInfoType::NONE:
    default:
        break;
    }
}

void testCellInfo(V1_2::CellInfo& cellInfo, RIL_CellInfo_V1_2& rilCellInfo) {
    EXPECT_EQ((int)cellInfo.cellInfoType, rilCellInfo.cellInfoType);
    EXPECT_EQ((int)cellInfo.registered, rilCellInfo.registered);
    EXPECT_EQ((int)cellInfo.timeStampType, rilCellInfo.timeStampType);
    EXPECT_TRUE(cellInfo.timeStamp == rilCellInfo.timeStamp);

    switch (cellInfo.cellInfoType) {
    case V1_0::CellInfoType::GSM:
        EXPECT_EQ((int)cellInfo.gsm.size(), 1);
        if (cellInfo.gsm.size() > 0) {
            testCellInfoGsm(cellInfo.gsm[0], rilCellInfo.CellInfo.gsm);
        }
        break;
    case V1_0::CellInfoType::CDMA:
        EXPECT_EQ((int)cellInfo.cdma.size(), 1);
        if (cellInfo.cdma.size() > 0) {
            testCellInfoCdma(cellInfo.cdma[0], rilCellInfo.CellInfo.cdma);
        }
        break;
    case V1_0::CellInfoType::LTE:
        EXPECT_EQ((int)cellInfo.lte.size(), 1);
        if (cellInfo.lte.size() > 0) {
            testCellInfoLte(cellInfo.lte[0], rilCellInfo.CellInfo.lte);
        }
        break;
    case V1_0::CellInfoType::WCDMA :
        EXPECT_EQ((int)cellInfo.wcdma.size(), 1);
        if (cellInfo.wcdma.size() > 0) {
            testCellInfoWcdma(cellInfo.wcdma[0], rilCellInfo.CellInfo.wcdma);
        }
        break;
    case V1_0::CellInfoType::TD_SCDMA:
        EXPECT_EQ((int)cellInfo.tdscdma.size(), 1);
        if (cellInfo.tdscdma.size() > 0) {
            testCellInfoTdscdma(cellInfo.tdscdma[0], rilCellInfo.CellInfo.tdscdma);
        }
        break;
    case V1_0::CellInfoType::NONE:
    default:
        break;
    }
}

void testCellInfo(V1_4::CellInfo& cellInfo, RIL_CellInfo_V1_4& rilCellInfo) {
    EXPECT_EQ((int)cellInfo.isRegistered, rilCellInfo.registered);
    EXPECT_EQ((int)cellInfo.connectionStatus, rilCellInfo.connectionStatus);

    switch (cellInfo.info.getDiscriminator()) {
    case V1_4::CellInfo::Info::hidl_discriminator::gsm:
        testCellInfoGsm(cellInfo.info.gsm(), rilCellInfo.CellInfo.gsm);
        break;
    case V1_4::CellInfo::Info::hidl_discriminator::cdma:
        testCellInfoCdma(cellInfo.info.cdma(), rilCellInfo.CellInfo.cdma);
        break;
    case V1_4::CellInfo::Info::hidl_discriminator::lte:
        testCellInfoLte(cellInfo.info.lte(), rilCellInfo.CellInfo.lte);
        break;
    case V1_4::CellInfo::Info::hidl_discriminator::wcdma:
        testCellInfoWcdma(cellInfo.info.wcdma(), rilCellInfo.CellInfo.wcdma);
        break;
    case V1_4::CellInfo::Info::hidl_discriminator::tdscdma:
        testCellInfoTdscdma(cellInfo.info.tdscdma(), rilCellInfo.CellInfo.tdscdma);
        break;
    case V1_4::CellInfo::Info::hidl_discriminator::nr:
        testCellInfoNr(cellInfo.info.nr(), rilCellInfo.CellInfo.nr);
        break;
    default:
        break;
    }
}

void testCellInfo(V1_5::CellInfo& cellInfo, RIL_CellInfo_V1_5& rilCellInfo) {
    EXPECT_EQ((int)cellInfo.registered, rilCellInfo.registered);
    //EXPECT_EQ((int)cellInfo.timeStampType, rilCellInfo.timeStampType);
    //EXPECT_TRUE(cellInfo.timeStamp == rilCellInfo.timeStamp);
    EXPECT_EQ((int)cellInfo.connectionStatus, rilCellInfo.connectionStatus);

    switch (cellInfo.ratSpecificInfo.getDiscriminator()) {
    case V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::gsm:
        testCellInfoGsm(cellInfo.ratSpecificInfo.gsm(), rilCellInfo.CellInfo.gsm);
        break;
    case V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::cdma:
        testCellInfoCdma(cellInfo.ratSpecificInfo.cdma(), rilCellInfo.CellInfo.cdma);
        break;
    case V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::lte:
        testCellInfoLte(cellInfo.ratSpecificInfo.lte(), rilCellInfo.CellInfo.lte);
        break;
    case V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::wcdma:
        testCellInfoWcdma(cellInfo.ratSpecificInfo.wcdma(), rilCellInfo.CellInfo.wcdma);
        break;
    case V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::tdscdma:
        testCellInfoTdscdma(cellInfo.ratSpecificInfo.tdscdma(), rilCellInfo.CellInfo.tdscdma);
        break;
    case V1_5::CellInfo::CellInfoRatSpecificInfo::hidl_discriminator::nr:
        testCellInfoNr(cellInfo.ratSpecificInfo.nr(), rilCellInfo.CellInfo.nr);
        break;
    default:
        break;
    }
}

void testCellIdentityDiscriminator(V1_5::CellIdentity::hidl_discriminator descriptor,
        int rilCellInfoType) {
    switch (rilCellInfoType) {
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM:
        EXPECT_EQ(V1_5::CellIdentity::hidl_discriminator::gsm, descriptor);
        break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA:
        EXPECT_EQ(V1_5::CellIdentity::hidl_discriminator::wcdma, descriptor);
        break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA:
        EXPECT_EQ(V1_5::CellIdentity::hidl_discriminator::cdma, descriptor);
        break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE:
        EXPECT_EQ(V1_5::CellIdentity::hidl_discriminator::lte, descriptor);
        break;
    case RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA:
        EXPECT_EQ(V1_5::CellIdentity::hidl_discriminator::tdscdma, descriptor);
        break;
    case RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR:
        EXPECT_EQ(V1_5::CellIdentity::hidl_discriminator::nr, descriptor);
        break;
    }
}
