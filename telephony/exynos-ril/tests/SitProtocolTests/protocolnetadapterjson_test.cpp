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
#include "json/network/protocolnetadapterjson.h"
#include "netjsondef.h"

/*
 * Common Test Data
 */
const int testCellTypesNum = 6;
const int testBandNum = 3;
const int testAdditionaPlmnsNum = 3;
char testAdditionaPlmns[testAdditionaPlmnsNum][MAX_PLMN_LEN + 1] = {"11111", "222222", "33333"};
RIL_OptionalCsgInfo rilCsgInfo = {{ true, "nodeName", 10}};
RIL_GSM_SignalStrength_v12 rilGsmSigStr = { 1, 2, 3 };
RIL_CDMA_SignalStrength rilCdmaSigStr = { 1, 2 };
RIL_EVDO_SignalStrength rilEvdoSigStr = { 1, 2, 3 };
RIL_LTE_SignalStrength_V1_6 rilLteSigStr = { { 1, 45, 5, 10, 5, 6}, 5 };
RIL_TD_SCDMA_SignalStrength_V1_2 rilTdscdmaSigStr = { 1, 2, 3 };
RIL_WCDMA_SignalStrength_V1_2 rilWcdmaSigStr = { 1, 2, 3, 4 };
RIL_NR_SignalStrength_V1_6 rilNrSigStr = { {50, 5, 10, 51, 6, 11}, 2, 3, {8, 9, 10} };

static void UpdateTestCommonInfo(int rilCellInfoType, Json::Value &jsonData, int reg) {
    jsonData[ns_net_cell_info_common::key_cell_type] = rilCellInfoType - 1;
    jsonData[ns_net_cell_info_common::key_reg_status] = reg;
    jsonData[ns_net_cell_info_common::key_connect_state] = PRIMARY_SERVING;
}

static void UpdateTestStrInfo(int rilCellInfoType, Json::Value &jsonData) {
    if (rilCellInfoType == RIL_CELL_INFO_TYPE_GSM) {
        jsonData[ns_net_cell_str_gsm::key_2g_sig_stren] = rilGsmSigStr.signalStrength;
        jsonData[ns_net_cell_str_gsm::key_2g_ber] = rilGsmSigStr.bitErrorRate;
        jsonData[ns_net_cell_str_gsm::key_2g_ta] = rilGsmSigStr.timingAdvance;
    }

    if (rilCellInfoType == RIL_CELL_INFO_TYPE_CDMA) {
        jsonData[ns_net_cell_str_cdma::key_cdma_dbm] = rilCdmaSigStr.dbm;
        jsonData[ns_net_cell_str_cdma::key_cdma_ecio] = rilCdmaSigStr.ecio;
        jsonData[ns_net_cell_str_cdma::key_cdma_dodbm] = rilEvdoSigStr.dbm;
        jsonData[ns_net_cell_str_cdma::key_cdma_doecio] = rilEvdoSigStr.ecio;
        jsonData[ns_net_cell_str_cdma::key_cdma_dosnr] = rilEvdoSigStr.signalNoiseRatio;
    }

    if (rilCellInfoType == RIL_CELL_INFO_TYPE_LTE) {
        jsonData[ns_net_cell_str_lte::key_4g_sig_stren] = rilLteSigStr.signalStrength;
        jsonData[ns_net_cell_str_lte::key_4g_rsrp] = rilLteSigStr.rsrp;
        jsonData[ns_net_cell_str_lte::key_4g_rsrq] = rilLteSigStr.rsrq;
        jsonData[ns_net_cell_str_lte::key_4g_rssnr] = rilLteSigStr.rssnr;
        jsonData[ns_net_cell_str_lte::key_4g_cqi_table_ind] = rilLteSigStr.cqiTableIndex;
        jsonData[ns_net_cell_str_lte::key_4g_cqi] = rilLteSigStr.cqi;
        jsonData[ns_net_cell_str_lte::key_4g_ta] = rilLteSigStr.timingAdvance;
    }

    if (rilCellInfoType == RIL_CELL_INFO_TYPE_WCDMA) {
        jsonData[ns_net_cell_str_wcdma::key_3g_sig_stren] = rilWcdmaSigStr.signalStrength;
        jsonData[ns_net_cell_str_wcdma::key_3g_ber] = rilWcdmaSigStr.bitErrorRate;
        jsonData[ns_net_cell_str_wcdma::key_3g_rscp] = rilWcdmaSigStr.rscp;
        jsonData[ns_net_cell_str_wcdma::key_3g_ecno] = rilWcdmaSigStr.ecno;
    }

    if (rilCellInfoType == RIL_CELL_INFO_TYPE_TD_SCDMA) {
        jsonData[ns_net_cell_str_tdscdma::key_tdscdma_sig_stren] = rilTdscdmaSigStr.signalStrength;
        jsonData[ns_net_cell_str_tdscdma::key_tdscdma_ber] = rilTdscdmaSigStr.bitErrorRate;
        jsonData[ns_net_cell_str_tdscdma::key_tdscdma_rscp] = rilTdscdmaSigStr.rscp;
    }

    if (rilCellInfoType == RIL_CELL_INFO_TYPE_NR) {
        jsonData[ns_net_cell_str_nr::key_5g_ssrsrp] = rilNrSigStr.ssRsrp;
        jsonData[ns_net_cell_str_nr::key_5g_ssrsrq] = rilNrSigStr.ssRsrq;
        jsonData[ns_net_cell_str_nr::key_5g_sssinr] = rilNrSigStr.ssSinr;
        jsonData[ns_net_cell_str_nr::key_5g_csirsrp] = rilNrSigStr.csiRsrp;
        jsonData[ns_net_cell_str_nr::key_5g_csirsrq] = rilNrSigStr.csiRsrq;
        jsonData[ns_net_cell_str_nr::key_5g_csisinr] = rilNrSigStr.csiSinr;
        jsonData[ns_net_cell_str_nr::key_5g_cqi_table_ind] = rilNrSigStr.csiCqiTableIndex;
        jsonData[ns_net_cell_str_nr::key_5g_wbcqi] = INT_MAX;
        jsonData[ns_net_cell_str_nr::key_5g_sbcqi_list].append(rilNrSigStr.csiCqiReport[0]);
        jsonData[ns_net_cell_str_nr::key_5g_sbcqi_list].append(rilNrSigStr.csiCqiReport[1]);
        jsonData[ns_net_cell_str_nr::key_5g_sbcqi_list].append(rilNrSigStr.csiCqiReport[2]);
    }
}

static string GetPlmn(int mcc, int mnc) {
    return to_string(mcc) + to_string(mnc);
}

static void SetTestValueforCellIds(RIL_CellIdentity_V1_5 *cellIdlist, int nums, int numOfPlmns, char **plmns) {
    for (int i = 0; i < nums; ++i ) {
        cellIdlist[i].cellInfoType = (RIL_CellInfoType)(i + 1);   // it will be RIL type value.
        if (cellIdlist[i].cellInfoType == RIL_CELL_INFO_TYPE_GSM) {
            cellIdlist[i].gsm = { 123, 123, 1, 1, 1, 1, {"testGsmL", "testGsmS"}, numOfPlmns, plmns};
        }

        if (cellIdlist[i].cellInfoType == RIL_CELL_INFO_TYPE_CDMA) {
            cellIdlist[i].cdma = { 123, 123, 4, 4, 4, {"testCdmaL", "testCdmaS"}};
        }

        if (cellIdlist[i].cellInfoType == RIL_CELL_INFO_TYPE_LTE) {
            cellIdlist[i].lte = {123, 123, 5, 5, 5, 5, {"testLteL", "testLteS"}, 5, numOfPlmns, plmns, rilCsgInfo,
                            testBandNum, {(RIL_EutranBands)10, (RIL_EutranBands)20 ,(RIL_EutranBands)30,}};
        }

        if (cellIdlist[i].cellInfoType == RIL_CELL_INFO_TYPE_WCDMA) {
            cellIdlist[i].wcdma = {123, 123, 2, 2, 2, 2, {"testWcdmaL", "testWcdmaS"}, numOfPlmns, plmns, rilCsgInfo};
        }

        if (cellIdlist[i].cellInfoType == RIL_CELL_INFO_TYPE_TD_SCDMA) {
            cellIdlist[i].tdscdma = {123, 123, 3, 3, 3, 3, {"testTdscdmaL", "testTdscdmaS"}, numOfPlmns, plmns, rilCsgInfo};
        }

        if (cellIdlist[i].cellInfoType == RIL_CELL_INFO_TYPE_NR) {
            cellIdlist[i].nr = {123, 123, 0xFFFFFFFFF, 6, 6, 6, {"testNrL", "testNrS"}, numOfPlmns, plmns,
                            testBandNum, {(RIL_NgranBands)10, (RIL_NgranBands)20 ,(RIL_NgranBands)30,}};
        }
    }
}

static void UpdateTestCellIdInfo(int type, Json::Value &jsonData, RIL_CellIdentity_V1_5 &testCellId,
                                    int numOfPlmns, char **plmns) {
    Json::Value jsonPlmns;
    for (int i = 0; i < numOfPlmns; ++i){
        jsonPlmns[ns_net_cell_info_plmn::key_plmn_info_list].append(string(*(plmns+i)));
    }

    Json::Value josnCsg;
    josnCsg[ns_net_cell_info_csg::key_csg_ind] = (rilCsgInfo.csgInfo.csgIndication == true) ? 1 : 0;
    josnCsg[ns_net_cell_info_csg::key_home_nodeb_name] = string(rilCsgInfo.csgInfo.homeNodebName);
    josnCsg[ns_net_cell_info_csg::key_csg_identiy] = rilCsgInfo.csgInfo.csgIdentity;

    switch (type) {
        case RIL_CELL_INFO_TYPE_GSM:
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_short_name] = string(testCellId.gsm.operatorNames.alphaShort);
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_long_name] = string(testCellId.gsm.operatorNames.alphaLong);
            jsonData[ns_net_cell_identity_gsm::key_2g_plmn] = GetPlmn(testCellId.gsm.mcc, testCellId.gsm.mnc);
            jsonData[ns_net_cell_identity_gsm::key_2g_lac] = testCellId.gsm.lac;
            jsonData[ns_net_cell_identity_gsm::key_2g_cid] = testCellId.gsm.cid;
            jsonData[ns_net_cell_identity_gsm::key_2g_arfcn] = testCellId.gsm.arfcn;
            jsonData[ns_net_cell_identity_gsm::key_2g_bsic] = testCellId.gsm.bsic;
            jsonData[ns_net_cell_identity_gsm::key_2g_cell_info_plmn] = jsonPlmns;
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_short_name] = string(testCellId.cdma.operatorNames.alphaShort);
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_long_name] = string(testCellId.cdma.operatorNames.alphaLong);
            jsonData[ns_net_cell_identity_cdma::key_cdma_nid] = testCellId.cdma.networkId;
            jsonData[ns_net_cell_identity_cdma::key_cdma_sid] = testCellId.cdma.systemId;
            jsonData[ns_net_cell_identity_cdma::key_cdma_bsid] = testCellId.cdma.basestationId;
            jsonData[ns_net_cell_identity_cdma::key_cdma_longitude] = testCellId.cdma.longitude;
            jsonData[ns_net_cell_identity_cdma::key_cdma_latitude] = testCellId.cdma.latitude;
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_short_name] = string(testCellId.lte.operatorNames.alphaShort);
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_long_name] = string(testCellId.lte.operatorNames.alphaLong);
            jsonData[ns_net_cell_identity_lte::key_4g_plmn] = GetPlmn(testCellId.lte.mcc, testCellId.lte.mnc);
            jsonData[ns_net_cell_identity_lte::key_4g_cid] = testCellId.lte.ci;
            jsonData[ns_net_cell_identity_lte::key_4g_pcid] = testCellId.lte.pci;
            jsonData[ns_net_cell_identity_lte::key_4g_tac] = testCellId.lte.tac;
            jsonData[ns_net_cell_identity_lte::key_4g_earfcn] = testCellId.lte.earfcn;
            jsonData[ns_net_cell_identity_lte::key_4g_bandwidth] = testCellId.lte.bandwidth;
            jsonData[ns_net_cell_identity_lte::key_4g_cell_info_plmn] = jsonPlmns;
            jsonData[ns_net_cell_identity_lte::key_4g_cell_info_csg] = josnCsg;
            jsonData[ns_net_cell_identity_lte::key_4g_eutran_band_list].append(testCellId.lte.bands[0]);
            jsonData[ns_net_cell_identity_lte::key_4g_eutran_band_list].append(testCellId.lte.bands[1]);
            jsonData[ns_net_cell_identity_lte::key_4g_eutran_band_list].append(testCellId.lte.bands[2]);
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_short_name] = string(testCellId.wcdma.operatorNames.alphaShort);
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_long_name] = string(testCellId.wcdma.operatorNames.alphaLong);
            jsonData[ns_net_cell_identity_wcdma::key_3g_plmn] = GetPlmn(testCellId.wcdma.mcc, testCellId.wcdma.mnc);
            jsonData[ns_net_cell_identity_wcdma::key_3g_lac] = testCellId.wcdma.lac;
            jsonData[ns_net_cell_identity_wcdma::key_3g_cid] = testCellId.wcdma.cid;
            jsonData[ns_net_cell_identity_wcdma::key_3g_psc] = testCellId.wcdma.psc;
            jsonData[ns_net_cell_identity_wcdma::key_3g_uarfcn] = testCellId.wcdma.uarfcn;
            jsonData[ns_net_cell_identity_wcdma::key_3g_cell_info_plmn] = jsonPlmns;
            jsonData[ns_net_cell_identity_wcdma::key_3g_cell_info_csg] = josnCsg;
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_short_name] = string(testCellId.tdscdma.operatorNames.alphaShort);
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_long_name] = string(testCellId.tdscdma.operatorNames.alphaLong);
            jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_plmn] = GetPlmn(testCellId.tdscdma.mcc, testCellId.tdscdma.mnc);
            jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_lac] = testCellId.tdscdma.lac;
            jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_cid] = testCellId.tdscdma.cid;
            jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_cpid] = testCellId.tdscdma.cpid;
            jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_uarfcn] = testCellId.tdscdma.uarfcn;
            jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_cell_info_plmn] = jsonPlmns;
            jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_cell_info_csg] = josnCsg;
            break;
        case RIL_CELL_INFO_TYPE_NR:
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_short_name] = string(testCellId.nr.operatorNames.alphaShort);
            jsonPlmns[ns_net_cell_info_plmn::key_plmn_long_name] = string(testCellId.nr.operatorNames.alphaLong);
            jsonData[ns_net_cell_identity_nr::key_5g_plmn] = GetPlmn(testCellId.nr.mcc, testCellId.nr.mnc);
            jsonData[ns_net_cell_identity_nr::key_5g_nci] = testCellId.nr.nci;
            jsonData[ns_net_cell_identity_nr::key_5g_pcid] = testCellId.nr.pci;
            jsonData[ns_net_cell_identity_nr::key_5g_tac] = testCellId.nr.tac;
            jsonData[ns_net_cell_identity_nr::key_5g_nrarfcn] = testCellId.nr.nrarfcn;
            jsonData[ns_net_cell_identity_nr::key_5g_cell_info_plmn] = jsonPlmns;
            jsonData[ns_net_cell_identity_nr::key_5g_cell_info_csg] = josnCsg;
            jsonData[ns_net_cell_identity_nr::key_5g_nr_band_list].append(testCellId.nr.bands[0]);
            jsonData[ns_net_cell_identity_nr::key_5g_nr_band_list].append(testCellId.nr.bands[1]);
            jsonData[ns_net_cell_identity_nr::key_5g_nr_band_list].append(testCellId.nr.bands[2]);
            break;
    }
}

static int isSameRilCsgInfo(RIL_OptionalCsgInfo &left, RIL_OptionalCsgInfo &right) {
    if (left.csgInfo.csgIndication != right.csgInfo.csgIndication) return 10;
    if (strcmp(left.csgInfo.homeNodebName, right.csgInfo.homeNodebName) != 0) return 20;
    if (left.csgInfo.csgIdentity != right.csgInfo.csgIdentity) return 30;
    return 0;
}
static int isSameRilCellIdGsm(RIL_CellIdentityGsm_V1_5 &left, RIL_CellIdentityGsm_V1_5 &right) {
    if (left.mcc != right.mcc) return 2;
    if ((left.mnc & 0xFFFFFFF) != right.mnc) return 3;    // because FetchMnc encoding
    if (left.lac != right.lac) return 4;
    if (left.cid != right.cid) return 5;
    if (left.arfcn != right.arfcn) return 6;
    if (left.bsic != right.bsic) return 7;
    if (strcmp(left.operatorNames.alphaLong, right.operatorNames.alphaLong) != 0) return 8;
    if (strcmp(left.operatorNames.alphaShort, right.operatorNames.alphaShort) != 0) return 9;
    if (left.leng_additionalPlmns != right.leng_additionalPlmns) return 10;

    if (left.additionalPlmns != nullptr && right.additionalPlmns != nullptr) {
        for (int i = 0; i < left.leng_additionalPlmns; ++i) {
            if (left.additionalPlmns[i] != nullptr && right.additionalPlmns[i] != nullptr ) {
                if (strcmp(left.additionalPlmns[i], right.additionalPlmns[i]) != 0) return 11;
            }
        }
    }

    return 0;
}

static int isSameRilCellIdCdma(RIL_CellIdentityCdma_V1_2 &left, RIL_CellIdentityCdma_V1_2 &right) {
    if (left.networkId != right.networkId) return 2;
    if (left.systemId != right.systemId) return 3;
    if (left.basestationId != right.basestationId) return 4;
    if (left.longitude != right.longitude) return 5;
    if (left.latitude != right.latitude) return 6;

    return 0;
}

static int isSameRilCellIdLte(RIL_CellIdentityLte_V1_5 &left, RIL_CellIdentityLte_V1_5 &right) {
    if (left.mcc != right.mcc) return 2;
    if ((left.mnc & 0xFFFFFFF) != right.mnc) return 3;    // because FetchMnc encoding
    if (left.ci != right.ci) return 4;
    if (left.pci != right.pci) return 5;
    if (left.tac != right.tac) return 6;
    if (left.earfcn != right.earfcn) return 7;
    if (left.bandwidth != right.bandwidth) return 8;
    if (strcmp(left.operatorNames.alphaLong, right.operatorNames.alphaLong) != 0) return 9;
    if (strcmp(left.operatorNames.alphaShort, right.operatorNames.alphaShort) != 0) return 10;
    if (left.leng_additionalPlmns != right.leng_additionalPlmns) return 11;

    if (left.additionalPlmns != nullptr && right.additionalPlmns != nullptr) {
        for (int i = 0; i < left.leng_additionalPlmns; ++i) {
            if (left.additionalPlmns[i] != nullptr && right.additionalPlmns[i] != nullptr ) {
                if (strcmp(left.additionalPlmns[i], right.additionalPlmns[i]) != 0) return 12;
            }
        }
    }

    int val = isSameRilCsgInfo(left.optionalCsgInfo, right.optionalCsgInfo);
    if (val != 0) return 13 + val;
    if (left.leng_bands != right.leng_bands) return 14;
    for (int i = 0; i < left.leng_bands; ++i) {
        if (left.bands[i] != right.bands[i]) return 15;
    }
    return 0;
}

static int isSameRilCellIdWcdma(RIL_CellIdentityWcdma_V1_5 &left, RIL_CellIdentityWcdma_V1_5 &right) {
    if (left.mcc != right.mcc) return 2;
    if ((left.mnc & 0xFFFFFFF) != right.mnc) return 3;    // because FetchMnc encoding
    if (left.lac != right.lac) return 4;
    if (left.cid != right.cid) return 5;
    if (left.psc != right.psc) return 6;
    if (left.uarfcn != right.uarfcn) return 7;
    if (strcmp(left.operatorNames.alphaLong, right.operatorNames.alphaLong) != 0) return 8;
    if (strcmp(left.operatorNames.alphaShort, right.operatorNames.alphaShort) != 0) return 9;
    if (left.leng_additionalPlmns != right.leng_additionalPlmns) return 10;

    if (left.additionalPlmns != nullptr && right.additionalPlmns != nullptr) {
        for (int i = 0; i < left.leng_additionalPlmns; ++i) {
            if (left.additionalPlmns[i] != nullptr && right.additionalPlmns[i] != nullptr ) {
                if (strcmp(left.additionalPlmns[i], right.additionalPlmns[i]) != 0) return 11;
            }
        }
    }

    int val = isSameRilCsgInfo(left.optionalCsgInfo, right.optionalCsgInfo);
    if (val != 0) return 12 + val;

    return 0;
}

static int isSameRilCellIdTdscdma(RIL_CellIdentityTdscdma_V1_5 &left, RIL_CellIdentityTdscdma_V1_5 &right) {
    if (left.mcc != right.mcc) return 2;
    if ((left.mnc & 0xFFFFFFF) != right.mnc) return 3;    // because FetchMnc encoding
    if (left.lac != right.lac) return 4;
    if (left.cid != right.cid) return 5;
    if (left.cpid != right.cpid) return 6;
    if (left.uarfcn != right.uarfcn) return 7;
    if (strcmp(left.operatorNames.alphaLong, right.operatorNames.alphaLong) != 0) return 8;
    if (strcmp(left.operatorNames.alphaShort, right.operatorNames.alphaShort) != 0) return 9;
    if (left.leng_additionalPlmns != right.leng_additionalPlmns) return 10;

    if (left.additionalPlmns != nullptr && right.additionalPlmns != nullptr) {
        for (int i = 0; i < left.leng_additionalPlmns; ++i) {
            if (left.additionalPlmns[i] != nullptr && right.additionalPlmns[i] != nullptr ) {
                if (strcmp(left.additionalPlmns[i], right.additionalPlmns[i]) != 0) return 11;
            }
        }
    }

    int val = isSameRilCsgInfo(left.optionalCsgInfo, right.optionalCsgInfo);
    if (val != 0) return 12 + val;

    return 0;
}

static int isSameRilCellIdNr(RIL_CellIdentityNr_V1_5 &left, RIL_CellIdentityNr_V1_5 &right) {
    if (left.mcc != right.mcc) return 2;
    if ((left.mnc & 0xFFFFFFF) != right.mnc) return 3;    // because FetchMnc encoding
    if (left.nci != right.nci) return 4;
    if (left.pci != right.pci) return 5;
    if (left.tac != right.tac) return 6;
    if (left.nrarfcn != right.nrarfcn) return 7;
    if (strcmp(left.operatorNames.alphaLong, right.operatorNames.alphaLong) != 0) return 8;
    if (strcmp(left.operatorNames.alphaShort, right.operatorNames.alphaShort) != 0) return 9;
    if (left.leng_additionalPlmns != right.leng_additionalPlmns) return 10;

    if (left.additionalPlmns != nullptr && right.additionalPlmns != nullptr) {
        for (int i = 0; i < left.leng_additionalPlmns; ++i) {
            if (left.additionalPlmns[i] != nullptr && right.additionalPlmns[i] != nullptr ) {
                if (strcmp(left.additionalPlmns[i], right.additionalPlmns[i]) != 0) return 11;
            }
        }
    }

    if (left.leng_bands != right.leng_bands) return 12;
    for (int i = 0; i < left.leng_bands; ++i) {
        if (left.bands[i] != right.bands[i]) return 13;
    }
    return 0;
}

static int IsSameRilCellId(RIL_CellIdentity_V1_5 &left, RIL_CellIdentity_V1_5 &right) {
    if (left.cellInfoType != right.cellInfoType) return 1;
    switch ((int)left.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            return isSameRilCellIdGsm(left.gsm, right.gsm);
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            return isSameRilCellIdCdma(left.cdma, right.cdma);
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            return isSameRilCellIdLte(left.lte, right.lte);
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            return isSameRilCellIdWcdma(left.wcdma, right.wcdma);
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            return isSameRilCellIdTdscdma(left.tdscdma, right.tdscdma);
            break;
        case RIL_CELL_INFO_TYPE_NR:
            return isSameRilCellIdNr(left.nr, right.nr);
            break;
    }
    return 0;
}

static int IsSameRilCellStrGsm(RIL_GSM_SignalStrength_v12 &sigStr, RIL_GSM_SignalStrength_v12 &right) {
    if (sigStr.signalStrength != right.signalStrength) return 1;
    if (sigStr.bitErrorRate != right.bitErrorRate) return 2;
    if (sigStr.timingAdvance != right.timingAdvance) return 3;
    return 0;
}

static int IsSameRilCellStrCdma(RIL_CDMA_SignalStrength &sigStr, RIL_CDMA_SignalStrength &right) {
    if (sigStr.dbm != right.dbm) return 1;
    if (sigStr.ecio != right.ecio) return 2;
    return 0;
}

static int IsSameRilCellStrEvdo(RIL_EVDO_SignalStrength &sigStr, RIL_EVDO_SignalStrength &right) {
    if (sigStr.dbm != right.dbm) return 1;
    if (sigStr.ecio != right.ecio) return 2;
    if (sigStr.signalNoiseRatio != right.signalNoiseRatio) return 3;
    return 0;
}

static int IsSameRilCellStrLte(RIL_LTE_SignalStrength_V1_6 &sigStr, RIL_LTE_SignalStrength_V1_6 &right) {
    if (sigStr.signalStrength != right.signalStrength) return 1;
    if (sigStr.rsrp != right.rsrp) return 2;
    if (sigStr.rsrq != right.rsrq) return 3;
    if (sigStr.rssnr != right.rssnr) return 4;
    if (sigStr.cqi != right.cqi) return 5;
    if (sigStr.timingAdvance != right.timingAdvance) return 6;
    if (sigStr.cqiTableIndex != right.cqiTableIndex) return 7;
    return 0;
}

static int IsSameRilCellStrTdscdma(RIL_TD_SCDMA_SignalStrength_V1_2 &sigStr, RIL_TD_SCDMA_SignalStrength_V1_2 &right) {
    if (sigStr.signalStrength != right.signalStrength) return 1;
    if (sigStr.bitErrorRate != right.bitErrorRate) return 2;
    if (sigStr.rscp != right.rscp) return 3;
    return 0;
}

static int IsSameRilCellStrWcdma(RIL_WCDMA_SignalStrength_V1_2 &sigStr, RIL_WCDMA_SignalStrength_V1_2 &right) {
    if (sigStr.signalStrength != right.signalStrength) return 1;
    if (sigStr.bitErrorRate != right.bitErrorRate) return 2;
    if (sigStr.rscp != right.rscp) return 3;
    if (sigStr.ecno != right.ecno) return 4;
    return 0;
}

static int IsSameRilCellStrNr(RIL_NR_SignalStrength_V1_6 &sigStr, RIL_NR_SignalStrength_V1_6 &right) {
    if (sigStr.ssRsrp != right.ssRsrp) return 1;
    if (sigStr.ssRsrq != right.ssRsrq) return 2;
    if (sigStr.ssSinr != right.ssSinr) return 3;
    if (sigStr.csiRsrp != right.csiRsrp) return 4;
    if (sigStr.csiRsrq != right.csiRsrq) return 5;
    if (sigStr.csiSinr != right.csiSinr) return 6;
    if (sigStr.csiCqiTableIndex != right.csiCqiTableIndex) return 7;
    if (sigStr.csiCqiReportLen != right.csiCqiReportLen) return 8;
    if (sigStr.csiCqiReport[0] != right.csiCqiReport[0]) return 9;
    if (sigStr.csiCqiReport[1] != right.csiCqiReport[1]) return 10;
    if (sigStr.csiCqiReport[2] != right.csiCqiReport[2]) return 11;
    return 0;
}

TEST(ProtocolNetAdapterJson, ProtocolNetVoiceRegStateAdapterJson_2G3G) {
    struct {
        int sitRegState;
        int rilRegState;
        int rejCause;
        int sitRat;
        int rilRat;

        unsigned int lac;
        unsigned int cellId;
        int psc;
        unsigned int channelNumber;
    } testVector[] {
        { SIT_NET_REG_STATE_REGISTERED, REGISTERED_HOME, 0, SIT_RAT_TYPE_UMTS, RADIO_TECH_UMTS,
            11, 12, 13, 14 },
        { SIT_NET_REG_STATE_REGISTERED, REGISTERED_HOME, 0, SIT_RAT_TYPE_GPRS, RADIO_TECH_GPRS,
            UINT_MAX, UINT_MAX, 13, UINT_MAX },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_CS_REG_STATE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        jsonData[ns_net_get_cs_reg_rsp::key_reg_status] = testVector[i].sitRegState;
        jsonData[ns_net_get_cs_reg_rsp::key_reg_cause] = testVector[i].rejCause;
        jsonData[ns_net_get_cs_reg_rsp::key_rat] = testVector[i].sitRat;

        Json::Value jsonRatRegData;
        jsonRatRegData[ns_net_2g_3g_reg_info::key_2g_3g_channel] = testVector[i].channelNumber;
        jsonRatRegData[ns_net_2g_3g_reg_info::key_2g_3g_cid] = testVector[i].cellId;
        jsonRatRegData[ns_net_2g_3g_reg_info::key_2g_3g_lac] = testVector[i].lac;
        jsonRatRegData[ns_net_2g_3g_reg_info::key_2g_3g_psc] = testVector[i].psc;

        jsonData[ns_net_get_cs_reg_rsp::key_2g_3g_cs_reg_info] = jsonRatRegData;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetVoiceRegStateAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].rilRegState);
        EXPECT_EQ(testAdapter.GetRejectCause(), testVector[i].rejCause);
        EXPECT_EQ(testAdapter.GetRadioTech(), testVector[i].rilRat);

        EXPECT_EQ(testAdapter.GetLAC(), (int)testVector[i].lac);
        EXPECT_EQ(testAdapter.GetCellId(), (int)testVector[i].cellId);
        EXPECT_EQ(testAdapter.GetPSC(), testVector[i].psc);
        EXPECT_EQ(testAdapter.getChannelNumber(), (int)testVector[i].channelNumber);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetVoiceRegStateAdapterJson_LTE) {
    struct {
        int sitRegState;
        int rilRegState;
        int rejCause;
        int sitRat;
        int rilRat;

        unsigned int tac;
        unsigned int pcid;
        unsigned int eci;
        unsigned int channelNumber;
    } testVector[] {
        { SIT_NET_REG_STATE_NOT_REG_SEARCHING, SEARCHING, 0, SIT_RAT_TYPE_LTE, RADIO_TECH_LTE,
            14, 15, 16, 17 },
        { SIT_NET_REG_STATE_NOT_REG_SEARCHING, SEARCHING, 0, SIT_RAT_TYPE_LTE, RADIO_TECH_LTE,
            UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_CS_REG_STATE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        jsonData[ns_net_get_cs_reg_rsp::key_reg_status] = testVector[i].sitRegState;
        jsonData[ns_net_get_cs_reg_rsp::key_reg_cause] = testVector[i].rejCause;
        jsonData[ns_net_get_cs_reg_rsp::key_rat] = testVector[i].sitRat;

        Json::Value jsonRatRegData;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_tac] = testVector[i].tac;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_pcid] = testVector[i].pcid;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_eci] = testVector[i].eci;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_channel] = testVector[i].channelNumber;

        jsonData[ns_net_get_cs_reg_rsp::key_lte_cs_reg_info] = jsonRatRegData;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetVoiceRegStateAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].rilRegState);
        EXPECT_EQ(testAdapter.GetRejectCause(), testVector[i].rejCause);
        EXPECT_EQ(testAdapter.GetRadioTech(), testVector[i].rilRat);

        EXPECT_EQ(testAdapter.GetTAC(), (int)testVector[i].tac);
        EXPECT_EQ(testAdapter.GetECI(), (int)testVector[i].eci);
        EXPECT_EQ(testAdapter.GetPCID(), (int)testVector[i].pcid);
        EXPECT_EQ(testAdapter.getChannelNumber(), (int)testVector[i].channelNumber);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetVoiceRegStateAdapterJson_NR) {
    struct {
        int sitRegState;
        int rilRegState;
        int rejCause;
        int sitRat;
        int rilRat;

        int nrCid; //TODO: Change data type to unsigned long int
        int pcid;
        unsigned int tac;
        int channelNumber;
    } testVector[] {
        { SIT_NET_REG_STATE_ROAMING, REGISTERED_ROAMING, 0, SIT_RAT_TYPE_5G, RADIO_TECH_NR,
            14, 15, 16, 17 },
        { SIT_NET_REG_STATE_ROAMING, REGISTERED_ROAMING, 0, SIT_RAT_TYPE_5G, RADIO_TECH_NR,
            14, 15, UINT_MAX, 17 },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_CS_REG_STATE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        jsonData[ns_net_get_cs_reg_rsp::key_reg_status] = testVector[i].sitRegState;
        jsonData[ns_net_get_cs_reg_rsp::key_reg_cause] = testVector[i].rejCause;
        jsonData[ns_net_get_cs_reg_rsp::key_rat] = testVector[i].sitRat;

        Json::Value jsonRatRegData;
        jsonRatRegData[ns_net_5g_reg_info::key_5g_tac] = testVector[i].tac;
        jsonRatRegData[ns_net_5g_reg_info::key_5g_pcid] = testVector[i].pcid;
        jsonRatRegData[ns_net_5g_reg_info::key_5g_channel] = testVector[i].channelNumber;
        jsonRatRegData[ns_net_5g_reg_info::key_5g_cid] = testVector[i].nrCid;

        jsonData[ns_net_get_cs_reg_rsp::key_nr_cs_reg_info] = jsonRatRegData;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetVoiceRegStateAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].rilRegState);
        EXPECT_EQ(testAdapter.GetRejectCause(), testVector[i].rejCause);
        EXPECT_EQ(testAdapter.GetRadioTech(), testVector[i].rilRat);

        EXPECT_EQ(testAdapter.getNrCid(), testVector[i].nrCid);
        EXPECT_EQ(testAdapter.GetPCID(), testVector[i].pcid);
        EXPECT_EQ(testAdapter.GetTAC(), (int)testVector[i].tac);
        EXPECT_EQ(testAdapter.getChannelNumber(), testVector[i].channelNumber);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetVoiceRegStateAdapterJson_CDMA) {
    struct {
        int sitRegState;
        int rilRegState;
        int rejCause;
        int sitRat;
        int rilRat;

        unsigned int stationId;
        int latitude;
        int longitude;
        int concurrent;
        int systemId;
        int networkId;
        int roamingInd;
        int regPrl;
        int roamingIndPrl;
    } testVector[] {
        { SIT_NET_REG_STATE_DENIED, DENIED, 0, SIT_RAT_TYPE_1xRTT, RADIO_TECH_1xRTT,
            17, 18, 19, 20, 21, 22, 23, 24, 25 },
        { SIT_NET_REG_STATE_DENIED, DENIED, 0, SIT_RAT_TYPE_1xRTT, RADIO_TECH_1xRTT,
            UINT_MAX, 18, 19, 20, 21, 22, 23, 24, 25 }
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_CS_REG_STATE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        jsonData[ns_net_get_cs_reg_rsp::key_reg_status] = testVector[i].sitRegState;
        jsonData[ns_net_get_cs_reg_rsp::key_reg_cause] = testVector[i].rejCause;
        jsonData[ns_net_get_cs_reg_rsp::key_rat] = testVector[i].sitRat;

        Json::Value jsonRatRegData;
        jsonRatRegData[ns_net_cdma_reg_info::key_cdma_base_sta_id] = testVector[i].stationId;
        jsonRatRegData[ns_net_cdma_reg_info::key_cdma_base_sta_lt] = testVector[i].latitude;
        jsonRatRegData[ns_net_cdma_reg_info::key_cdma_base_sta_lng] = testVector[i].longitude;
        jsonRatRegData[ns_net_cdma_reg_info::key_cdma_concurrent] = testVector[i].concurrent;
        jsonRatRegData[ns_net_cdma_reg_info::key_cdma_sid] = testVector[i].systemId;
        jsonRatRegData[ns_net_cdma_reg_info::key_cdma_nid] = testVector[i].networkId;
        jsonRatRegData[ns_net_cdma_reg_info::key_cdma_roaming_id] = testVector[i].roamingInd;
        jsonRatRegData[ns_net_cdma_reg_info::key_cdma_registered_prl] = testVector[i].regPrl;
        jsonRatRegData[ns_net_cdma_reg_info::key_cdma_roaming_ind_prl] = testVector[i].roamingIndPrl;

        jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info] = jsonRatRegData;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetVoiceRegStateAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].rilRegState);
        EXPECT_EQ(testAdapter.GetRejectCause(), testVector[i].rejCause);
        EXPECT_EQ(testAdapter.GetRadioTech(), testVector[i].rilRat);

        EXPECT_EQ(testAdapter.GetStationId(), (int)testVector[i].stationId);
        EXPECT_EQ(testAdapter.GetStationLat(), testVector[i].latitude);
        EXPECT_EQ(testAdapter.GetStationLong(), testVector[i].longitude);
        EXPECT_EQ(testAdapter.GetConCurrent(), testVector[i].concurrent);
        EXPECT_EQ(testAdapter.GetSystemId(), testVector[i].systemId);
        EXPECT_EQ(testAdapter.GetNetworkId(), testVector[i].networkId);
        EXPECT_EQ(testAdapter.GetRoamingInd(), testVector[i].roamingInd);
        EXPECT_EQ(testAdapter.GetRegPrl(), testVector[i].regPrl);
        EXPECT_EQ(testAdapter.GetRoamingIndPrl(), testVector[i].roamingIndPrl);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetDataRegStateAdapterJson_2G3G) {
    struct {
        int sitRegState;
        int rilRegState;
        int rejCause;
        int sitRat;
        int rilRat;

        unsigned int lac;
        unsigned int cellId;
        int psc;
        unsigned int channelNumber;

        int voPs;
        int emcService;
    } testVector[] {
        { SIT_NET_REG_STATE_REGISTERED, REGISTERED_HOME, 0, SIT_RAT_TYPE_UMTS, RADIO_TECH_UMTS,
            11, 12, 13, 14, 20, 21 },
        { SIT_NET_REG_STATE_REGISTERED, REGISTERED_HOME, 0, SIT_RAT_TYPE_UMTS, RADIO_TECH_UMTS,
            UINT_MAX, UINT_MAX, 13, UINT_MAX, 20, 21 },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_PS_REG_STATE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        jsonData[ns_net_get_ps_reg_rsp::key_reg_status] = testVector[i].sitRegState;
        jsonData[ns_net_get_ps_reg_rsp::key_rej_cause] = testVector[i].rejCause;
        jsonData[ns_net_get_ps_reg_rsp::key_rat] = testVector[i].sitRat;

        Json::Value jsonRatRegData;
        jsonRatRegData[ns_net_2g_3g_reg_info::key_2g_3g_channel] = testVector[i].channelNumber;
        jsonRatRegData[ns_net_2g_3g_reg_info::key_2g_3g_cid] = testVector[i].cellId;
        jsonRatRegData[ns_net_2g_3g_reg_info::key_2g_3g_lac] = testVector[i].lac;
        jsonRatRegData[ns_net_2g_3g_reg_info::key_2g_3g_psc] = testVector[i].psc;

        jsonData[ns_net_get_ps_reg_rsp::key_2g_3g_ps_reg_info] = jsonRatRegData;
        jsonData[ns_net_get_ps_reg_rsp::key_volte_svc] = testVector[i].voPs;
        jsonData[ns_net_get_ps_reg_rsp::key_emc_svc] = testVector[i].emcService;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetDataRegStateAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].rilRegState);
        EXPECT_EQ(testAdapter.GetRejectCause(), testVector[i].rejCause);
        EXPECT_EQ(testAdapter.GetRadioTech(), testVector[i].rilRat);

        // not used and it is fixed value
        EXPECT_EQ(testAdapter.GetMaxSDC(), 4);

        EXPECT_EQ(testAdapter.GetLAC(), (int)testVector[i].lac);
        EXPECT_EQ(testAdapter.GetCellId(), (int)testVector[i].cellId);
        EXPECT_EQ(testAdapter.GetPSC(), testVector[i].psc);
        EXPECT_EQ(testAdapter.getChannelNumber(), (int)testVector[i].channelNumber);

        EXPECT_EQ(testAdapter.GetImsVops(), testVector[i].voPs);
        EXPECT_EQ(testAdapter.GetEmcService(), testVector[i].emcService);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetDataRegStateAdapterJson_LTE) {
    struct {
        int sitRegState;
        int rilRegState;
        int rejCause;
        int sitRat;
        int rilRat;

        unsigned int tac;
        unsigned int pcid;
        unsigned int eci;
        unsigned int csgId;
        unsigned int tadv;
        unsigned int channelNumber;
        int endcAvailable;
        int dcnrRestricted;
        int nrAvailable;

        int voPs;
        int emcService;
    } testVector[] {
        { SIT_NET_REG_STATE_NOT_REG_SEARCHING, SEARCHING, 0, SIT_RAT_TYPE_LTE, RADIO_TECH_LTE,
            10, 11, 12, 13, 14, 15, 20, 21, 22, 30, 31 },
        { SIT_NET_REG_STATE_NOT_REG_SEARCHING, SEARCHING, 0, SIT_RAT_TYPE_LTE, RADIO_TECH_LTE,
            UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, 20, 21, 22, 30, 31 },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_PS_REG_STATE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        jsonData[ns_net_get_ps_reg_rsp::key_reg_status] = testVector[i].sitRegState;
        jsonData[ns_net_get_ps_reg_rsp::key_rej_cause] = testVector[i].rejCause;
        jsonData[ns_net_get_ps_reg_rsp::key_rat] = testVector[i].sitRat;

        Json::Value jsonRatRegData;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_tac] = testVector[i].tac;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_pcid] = testVector[i].pcid;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_eci] = testVector[i].eci;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_csgid] = testVector[i].csgId;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_tadv] = testVector[i].tadv;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_channel] = testVector[i].channelNumber;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_endc] = testVector[i].endcAvailable;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_dcnr_restricted] = testVector[i].dcnrRestricted;
        jsonRatRegData[ns_net_4g_reg_info::key_4g_nr_available] = testVector[i].nrAvailable;

        jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info] = jsonRatRegData;
        jsonData[ns_net_get_ps_reg_rsp::key_volte_svc] = testVector[i].voPs;
        jsonData[ns_net_get_ps_reg_rsp::key_emc_svc] = testVector[i].emcService;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetDataRegStateAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].rilRegState);
        EXPECT_EQ(testAdapter.GetRejectCause(), testVector[i].rejCause);
        EXPECT_EQ(testAdapter.GetRadioTech(), testVector[i].rilRat);

        // not used and it is fixed value
        EXPECT_EQ(testAdapter.GetMaxSDC(), 4);

        EXPECT_EQ(testAdapter.GetTAC(), (int)testVector[i].tac);
        EXPECT_EQ(testAdapter.GetPCID(), (int)testVector[i].pcid);
        EXPECT_EQ(testAdapter.GetECI(), (int)testVector[i].eci);
        EXPECT_EQ(testAdapter.GetCSGID(), (int)testVector[i].csgId);
        EXPECT_EQ(testAdapter.GetTADV(), (int)testVector[i].tadv);
        EXPECT_EQ(testAdapter.getChannelNumber(), (int)testVector[i].channelNumber);
        EXPECT_EQ(testAdapter.IsEndcAvailable(), testVector[i].endcAvailable == SERVICE_AVAILABLE );
        EXPECT_EQ(testAdapter.IsDcNrRestricted(), testVector[i].dcnrRestricted == SERVICE_AVAILABLE);
        EXPECT_EQ(testAdapter.IsNrAvailable(), testVector[i].nrAvailable == SERVICE_AVAILABLE);

        EXPECT_EQ(testAdapter.GetImsVops(), testVector[i].voPs);
        EXPECT_EQ(testAdapter.GetEmcService(), testVector[i].emcService);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetDataRegStateAdapterJson_NR) {
    struct {
        int sitRegState;
        int rilRegState;
        int rejCause;
        int sitRat;
        int rilRat;

        int nrCid; // TODO: Change data type to unsigned long int
        int pcid;
        unsigned int tac;
        int channelNumber;
        int emf;

        int voPs;
        int emcService;
    } testVector[] {
        { SIT_NET_REG_STATE_ROAMING, REGISTERED_ROAMING, 0, SIT_RAT_TYPE_5G, RADIO_TECH_NR,
            14, 15, 19, 20, 21, 31, 32},
        { SIT_NET_REG_STATE_ROAMING, REGISTERED_ROAMING, 0, SIT_RAT_TYPE_5G, RADIO_TECH_NR,
            14, 15, UINT_MAX, 20, 21, 31, 32},
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_PS_REG_STATE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        jsonData[ns_net_get_ps_reg_rsp::key_reg_status] = testVector[i].sitRegState;
        jsonData[ns_net_get_ps_reg_rsp::key_rej_cause] = testVector[i].rejCause;
        jsonData[ns_net_get_ps_reg_rsp::key_rat] = testVector[i].sitRat;

        Json::Value jsonRatRegData;
        jsonRatRegData[ns_net_5g_reg_info::key_5g_cid] = testVector[i].nrCid;
        jsonRatRegData[ns_net_5g_reg_info::key_5g_pcid] = testVector[i].pcid;
        jsonRatRegData[ns_net_5g_reg_info::key_5g_tac] = testVector[i].tac;
        jsonRatRegData[ns_net_5g_reg_info::key_5g_channel] = testVector[i].channelNumber;
        jsonRatRegData[ns_net_5g_reg_info::key_5g_emf] = testVector[i].emf;

        jsonData[ns_net_get_ps_reg_rsp::key_nr_ps_reg_info] = jsonRatRegData;
        jsonData[ns_net_get_ps_reg_rsp::key_volte_svc] = testVector[i].voPs;
        jsonData[ns_net_get_ps_reg_rsp::key_emc_svc] = testVector[i].emcService;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetDataRegStateAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].rilRegState);
        EXPECT_EQ(testAdapter.GetRejectCause(), testVector[i].rejCause);
        EXPECT_EQ(testAdapter.GetRadioTech(), testVector[i].rilRat);

        // not used and it is fixed value
        EXPECT_EQ(testAdapter.GetMaxSDC(), 4);

        EXPECT_EQ(testAdapter.getNrCid(), testVector[i].nrCid);
        EXPECT_EQ(testAdapter.GetPCID(), testVector[i].pcid);
        EXPECT_EQ(testAdapter.GetTAC(), (int)testVector[i].tac);
        EXPECT_EQ(testAdapter.getChannelNumber(), testVector[i].channelNumber);
        EXPECT_EQ(testAdapter.GetEmf(), testVector[i].emf);

        EXPECT_EQ(testAdapter.GetImsVops(), testVector[i].voPs);
        EXPECT_EQ(testAdapter.GetEmcService(), testVector[i].emcService);

        delete modemData;
        modemData = nullptr;
    }
}


TEST(ProtocolNetAdapterJson, ProtocolNetOperatorAdapterJson) {
    struct {
        bool areOptionalsAvailable;
        string plmn;
        string shortName;
        string longName;
        int regState;
        int lac; // TODO: Check with CP regarding it's range and accordingly handle it in RIL
    } testVector[] {
        { true, "310150", "AT&T", "AT&T", 0, 10 },
        { true, "310160", "T-Mobile", "TMO", 1, 11 },
        { true, "45005", "SKTelecom", "SKTelecom", 2, 12 },
        { false, "45005", "", "", 2, 12 },
        { true, string(MAX_PLMN_LEN+2, '1'), string(MAX_SHORT_NAME_LEN+2, 'S'), string(MAX_FULL_NAME_LEN+2, 'F'), 0, 10 },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_OPERATOR;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_get_operator_rsp::key_plmn] = testVector[i].plmn;
        if(testVector[i].areOptionalsAvailable) {
            jsonData[ns_net_get_operator_rsp::key_short_name] = testVector[i].shortName;
            jsonData[ns_net_get_operator_rsp::key_long_name] = testVector[i].longName;
        }
        jsonData[ns_net_get_operator_rsp::key_location] = testVector[i].lac;
        jsonData[ns_net_get_operator_rsp::key_reg_status] = testVector[i].regState;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetOperatorAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_TRUE(strlen(testAdapter.GetPlmn()) <= MAX_PLMN_LEN);
        EXPECT_EQ(strncmp(testAdapter.GetPlmn(), testVector[i].plmn.c_str(), MAX_PLMN_LEN), 0);

        if(testVector[i].areOptionalsAvailable) {
            EXPECT_TRUE(strlen(testAdapter.GetShortPlmn()) <= MAX_SHORT_NAME_LEN);
            EXPECT_EQ(strncmp(testAdapter.GetShortPlmn(), testVector[i].shortName.c_str(), MAX_SHORT_NAME_LEN), 0);

            EXPECT_TRUE(strlen(testAdapter.GetLongPlmn()) <= MAX_FULL_NAME_LEN);
            EXPECT_EQ(strncmp(testAdapter.GetLongPlmn(), testVector[i].longName.c_str(), MAX_FULL_NAME_LEN), 0);
        } else {
            EXPECT_EQ(testAdapter.GetShortPlmn(), nullptr);
            EXPECT_EQ(testAdapter.GetLongPlmn(), nullptr);
        }
        EXPECT_EQ(testAdapter.GetRegState(), testVector[i].regState);
        EXPECT_EQ(testAdapter.GetLac(), testVector[i].lac);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetSelModeAdapterJson) {
    struct {
        int sitData;
    } testVector[] {
        { SIT_NET_NETWORK_MODE_AUTOMATIC },
        { SIT_NET_NETWORK_MODE_MANUAL },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_NTW_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_get_ntw_mode_rsp::key_ntw_mode] = testVector[i].sitData;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetSelModeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetNetworkSelectionMode(), testVector[i].sitData);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolRadioPowerAdapterJson) {
    // No RX payload
    unsigned int sitId = SIT_SET_RADIO_POWER;

    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS, nullptr, 0);
    ASSERT_NE(modemData, nullptr);

    ProtocolRadioPowerAdapterJson testAdapter(modemData);

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolNetAdapterJson, ProtocolGetRadioStateRespAdapterJson) {
    struct {
        int radio;
        int rilRadioState;
    } testVector[] = {
        { 0, RADIO_STATE_OFF },
        { 1, RADIO_STATE_UNAVAILABLE },
        { 2, RADIO_STATE_ON },
        { 3, RADIO_STATE_ON },
        { 10, RADIO_STATE_ON },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_RADIO_POWER;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_pwr_get_radio_power_rsp::key_radio_state] = testVector[i].radio;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetRadioStateRespAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRadioState(), testVector[i].rilRadioState);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolRadioStateAdapterJson) {
    struct {
        int radio;
        int rilRadioState;
    } testVector[] = {
        { 0, RADIO_STATE_OFF },
        { 1, RADIO_STATE_OFF },
        { 2, RADIO_STATE_ON },
        { 3, RADIO_STATE_UNAVAILABLE },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_RADIO_STATE_CHANGED;
    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_pwr_radio_state_changed_ind::key_radio_state] = testVector[i].radio;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolRadioStateAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_RADIO_STATE_CHANGED);

        EXPECT_EQ(testAdapter.GetRadioState(), testVector[i].rilRadioState);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetPrefNetTypeAdapterJson) {
    struct {
        int sitData;
        int rilData;
    } testVector[] {
        { PREF_NET_TYPE_TD_SCDMA_WCDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_WCDMA },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_PREFERRED_NTW_TYPE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_preferred_ntw_type::key_pref_net_type] = testVector[i].sitData;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetPrefNetTypeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetPreferredNetworkType(), testVector[i].rilData);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetBandModeAdapterJson) {
    struct {
        int bandList[SIT_NET_BAND_MAX+2];
        int cpBandListSize;
        int rilBandListSize;
    } testVector[] {
        { {SIT_NET_BAND_JPN, SIT_NET_BAND_AUS, SIT_NET_BAND_AUS2,}, 3, 3 },
            { { [0 ... SIT_NET_BAND_MAX+1] = 1 }, SIT_NET_BAND_MAX + 2, SIT_NET_BAND_MAX }, // band_list size more than expected
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_BAND_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        for (int j = 0; j < testVector[i].cpBandListSize; ++j ) {
            jsonData[ns_net_set_band_mode_rsp::key_band_mode_list].append(testVector[i].bandList[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetBandModeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetCount(), testVector[i].rilBandListSize);
        const int *resultBands = testAdapter.GetAvialableBandMode();
        if (resultBands != NULL) {
            for (int j = 0; j < testAdapter.GetCount() ; ++j ) {
                EXPECT_EQ(resultBands[j], testVector[i].bandList[j]);;
            }
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetAvailableNetworkAdapterJson) {
    struct {
        string plmn;
        int sitPlmnStatus;
        int sitRat;

        const char *rilPlmnStatus;
        int rilRat;
    } testVector[] {
        { "123456", SIT_NET_PLMN_STATUS_AVAILABLE, SIT_RAT_TYPE_GPRS, STR_NETWORK_STATUS_AVAILABLE, RADIO_TECH_GPRS },
            { "123456", SIT_NET_PLMN_STATUS_CURRENT, SIT_RAT_TYPE_UMTS, STR_NETWORK_STATUS_CURRENT, RADIO_TECH_UMTS },
            { "123456789", SIT_NET_PLMN_STATUS_FORBIDDEN, SIT_RAT_TYPE_5G, STR_NETWORK_STATUS_FORBIDDEN, RADIO_TECH_NR }, // TC where plmn string length more than expected
    };

    const int nwkInfoSize =  sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_AVAILABLE_NETWORKS;

    Json::Value jsonData;

    for(int i = 0; i < nwkInfoSize; i++) {
        Json::Value jsonNet;
        jsonNet[ns_net_network_info::key_plmn] = testVector[i].plmn;
        jsonNet[ns_net_network_info::key_plmn_status] = testVector[i].sitPlmnStatus;
        jsonNet[ns_net_network_info::key_rat] = testVector[i].sitRat;
        jsonData[ns_net_get_available_networks_rsp::key_network_list].append(jsonNet);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
    ASSERT_NE(modemData, nullptr);

    ProtocolNetAvailableNetworkAdapterJson testAdapter(modemData);

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    EXPECT_EQ(testAdapter.GetCount(), nwkInfoSize);
    for(int j = 0; j < MIN(nwkInfoSize, testAdapter.GetCount()); j++) {
        NetworkInfo resultNwkInfo = {};
        testAdapter.GetNetwork(resultNwkInfo, j, nullptr, nullptr);
        EXPECT_STREQ(resultNwkInfo.longPlmn, "");    // name is updated in the caller function
        EXPECT_STREQ(resultNwkInfo.shortPlmn, "");   // name is updated in the caller function
        EXPECT_TRUE(strlen(resultNwkInfo.plmn) <= MAX_PLMN_LEN);
        EXPECT_EQ(strncmp(resultNwkInfo.plmn, testVector[j].plmn.c_str(), MAX_PLMN_LEN), 0);
        EXPECT_STREQ(resultNwkInfo.status, testVector[j].rilPlmnStatus);
        EXPECT_EQ(resultNwkInfo.rat, testVector[j].rilRat);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolNetAdapterJson, ProtocolNetGetPsServiceAdapterJson) {
    struct {
        int sitState;
        int rilState;
    } testVector[] {
        { SIT_NET_ENABLE , ALLOW_DATA_CALL },
        { SIT_NET_DISABLE, DISALLOW_DATA_CALL},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_PS_SERVICE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_ps_service::key_ps_state] = testVector[i].sitState;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetPsServiceAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetState(), testVector[i].rilState);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetDuplexModeRespAdapterJson) {
    struct {
        int sit4gMode;
        int sit3gMode;
        int rilMode;
    } testVector[] {
        { SIT_NET_DUPLEX_MODE_FDD, SIT_NET_DUPLEX_MODE_FDD, DUPLEX_MODE_INVALID},
        { SIT_NET_DUPLEX_MODE_TDD, SIT_NET_DUPLEX_MODE_TDD, DUPLEX_MODE_LTG},
        { SIT_NET_DUPLEX_MODE_FDD_TDD, SIT_NET_DUPLEX_MODE_FDD_TDD, DUPLEX_MODE_GLOBAL},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_DUPLEX_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_duplex_mode::key_duplex_lte] = testVector[i].sit4gMode;
        jsonData[ns_net_getset_duplex_mode::key_duplex_hedge] = testVector[i].sit3gMode;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetDuplexModeRespAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.Get4gDuplexMode(), testVector[i].sit4gMode);
        EXPECT_EQ(testAdapter.Get3gDuplexMode(), testVector[i].sit3gMode);
        EXPECT_EQ(testAdapter.GetDuplexMode(), testVector[i].rilMode);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetEmergencyActInfoAdapterJson) {
    struct {
        int sitRat;
        int sitEmcStatus;
        int rilRat;
        int rilEmcStatus;
    } testVector[] {
        { SIT_RAT_TYPE_5G, SIT_NET_RETRY_ACT_EMERGENCY_CALL, RADIO_TECH_NR, EMERGENCY_CALL_RETRY },
        { SIT_RAT_TYPE_LTE_CA, SIT_NET_CURRENT_ACT_EMERGENCY_CALL, RADIO_TECH_LTE, EMERGENCY_CALL_AVAILABLE },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_EMERGENCY_ACT_INFO;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_emergency_act_info_ind::key_rat] = testVector[i].sitRat;
        jsonData[ns_net_emergency_act_info_ind::key_act_status] = testVector[i].sitEmcStatus;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetEmergencyActInfoAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        EXPECT_EQ(testAdapter.GetRat(), testVector[i].rilRat);
        EXPECT_EQ(testAdapter.GetActStatus(), testVector[i].rilEmcStatus);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetMcSrchRespAdapterJson) {
    struct {
        int srchResult;
        char plmn[7];
    } testVector[] {
        { 2, "34131"},
        { 3, "123456"},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_SET_MICRO_CELL_SEARCH;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_set_micro_cell_search_rsp::key_search_result] = testVector[i].srchResult;
        jsonData[ns_net_set_micro_cell_search_rsp::key_plmn] = string(testVector[i].plmn);

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetMcSrchRespAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetMcSrchResult(), testVector[i].srchResult);
        EXPECT_STREQ(testAdapter.GetMcSrchPlmn(), testVector[i].plmn);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolSetNetworkRCRespAdapterJson) {
    // No RX payload
    unsigned int sitId = SIT_SET_RADIO_CAPABILITY;

    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS, nullptr, 0);
    ASSERT_NE(modemData, nullptr);

    ProtocolSetNetworkRCRespAdapterJson testAdapter(modemData);

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolNetAdapterJson, ProtocolGetNetworkRCRespAdapterJson) {
    struct {
        int sitType;
        int rilType;
    } testVector[] {
        { RAF_CP_EDGE, NETWORK_TYPE_BITMAP_GSM},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_RADIO_CAPABILITY;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_get_radio_capability_rsp::key_raf] = testVector[i].sitType;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetNetworkRCRespAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRafType(), testVector[i].rilType);
        testAdapter.GetVersion();
        testAdapter.GetSession();
        testAdapter.GetPhase();
        testAdapter.GetUuid();
        testAdapter.GetStatus();

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetworkRCIndAdapterJson) {
    // Not used IPC

    unsigned int sitId = SIT_IND_RADIO_CAPABILITY;

    ModemData *modemData = ModemDataBuilder::BuildIndication(sitId, nullptr, 0);
    ASSERT_NE(modemData, nullptr);

    ProtocolNetworkRCIndAdapterJson testAdapter(modemData);

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);

    testAdapter.GetVersion();
    testAdapter.GetSession();
    testAdapter.GetPhase();
    testAdapter.GetRafType();
    testAdapter.GetUuid();
    testAdapter.GetStatus();

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolNetAdapterJson, ProtocolNetCdmaQueryRoamingTypeAdapterJson) {
    struct {
        int sitType;
        int rilType;
    } testVector[] {
        { SIT_CDMA_RM_AFFILIATED, CDMA_ROAMING_AFFILIATED_NETWORKS},
        { SIT_CDMA_RM_ANY, CDMA_ROAMING_ANY_NETWORK},
        { SIT_CDMA_RM_HOME, CDMA_ROAMING_HOME_ONLY},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_CDMA_ROAMING_PREFERENCE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_cdma_roaming_preferrence::key_cdma_mode] = testVector[i].sitType;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetCdmaQueryRoamingTypeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.QueryRoamingType(), testVector[i].rilType);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetCdmaHybridModeAdapterJson) {
    struct {
        int mode;
    } testVector[] {
        { HYBRID_MODE_1X_ONLY},
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_CDMA_HYBRID_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_cdma_hybrid_mode::key_hybrid_mode] = testVector[i].mode;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetCdmaHybridModeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetCdmaHybridMode(), testVector[i].mode);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetMccAdapterJson) {
    struct {
        int sitPrefNet;
        char mcc[3+2]; // Extra byte for case where CP sends data more than expected
    } testVector[] {
        { SIT_NET_PREF_NET_TYPE_GSM_WCDMA_AUTO, "345" },
            { SIT_NET_PREF_NET_TYPE_GSM_WCDMA, "1234" }, // casse where mcc length is more than 3
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_MCC;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_mcc_ind::key_pref_net_type] = testVector[i].sitPrefNet;
        jsonData[ns_net_mcc_ind::key_mcc] = string(testVector[i].mcc);

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetMccAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        EXPECT_EQ(testAdapter.GetCurrentPrefNetworkMode(), testVector[i].sitPrefNet);
        EXPECT_TRUE(strlen(testAdapter.GetMcc()) <= 3);
        EXPECT_EQ(strncmp(testAdapter.GetMcc(), testVector[i].mcc, 3), 0);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetCellInfoListAdapterJson) {
    char **plmns = (char **)calloc(testAdditionaPlmnsNum, sizeof(char*));
    for (int i = 0; i < testAdditionaPlmnsNum; ++i) {
        *(plmns + i) = testAdditionaPlmns[i];
    }

    RIL_CellIdentity_V1_5 testRilCellIds[testCellTypesNum];
    memset(testRilCellIds, 0, sizeof(RIL_CellIdentity_V1_5) * testCellTypesNum);
    SetTestValueforCellIds(testRilCellIds, testCellTypesNum, testAdditionaPlmnsNum, plmns);

    struct {
        unsigned int numOfCells;
        int cellType[testCellTypesNum];
        int reg[testCellTypesNum];
    } testVector[] {
        { testCellTypesNum, {1, 2, 3, 4, 5, 6}, {1, 0, 0, 0, 0, 0} },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_CELL_INFO_LIST;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        for (unsigned int j = 0; j < testVector[i].numOfCells; ++j) {
            Json::Value cellInfo;

            int rilCellType = testVector[i].cellType[j];
            int sitCellType = rilCellType - 1;

            Json::Value commonInfo;
            Json::Value jsonCellId;
            Json::Value strInfo;

            UpdateTestCommonInfo(rilCellType, commonInfo, testVector[i].reg[sitCellType]);
            UpdateTestCellIdInfo(rilCellType, jsonCellId, testRilCellIds[sitCellType], testAdditionaPlmnsNum, plmns);
            UpdateTestStrInfo(rilCellType, strInfo);

            cellInfo[ns_net_cell_info::key_cell_info_type] = sitCellType;

            if (rilCellType == RIL_CELL_INFO_TYPE_GSM) {
                Json::Value jsonGsm;
                jsonGsm[ns_net_cell_info_gsm::key_cell_info_common] = commonInfo;
                jsonGsm[ns_net_cell_info_gsm::key_cell_identity_gsm] = jsonCellId;
                jsonGsm[ns_net_cell_info_gsm::key_sig_str_gsm] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_gsm] = jsonGsm;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_CDMA) {
                Json::Value jsonCdma;
                jsonCdma[ns_net_cell_info_cdma::key_cell_info_common] = commonInfo;
                jsonCdma[ns_net_cell_info_cdma::key_cell_identity_cdma] = jsonCellId;
                jsonCdma[ns_net_cell_info_cdma::key_sig_str_cdma] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_cdma] = jsonCdma;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_LTE) {
                Json::Value jsonLte;
                jsonLte[ns_net_cell_info_lte::key_cell_info_common] = commonInfo;
                jsonLte[ns_net_cell_info_lte::key_cell_identity_lte] = jsonCellId;
                jsonLte[ns_net_cell_info_lte::key_sig_str_lte] = strInfo;
                jsonLte[ns_net_cell_info_lte::key_endc_available] = 1;
                cellInfo[ns_net_cell_info::key_cell_info_lte] = jsonLte;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_WCDMA) {
                Json::Value jsonWcdma;
                jsonWcdma[ns_net_cell_info_wcdma::key_cell_info_common] = commonInfo;
                jsonWcdma[ns_net_cell_info_wcdma::key_cell_identity_wcdma] = jsonCellId;
                jsonWcdma[ns_net_cell_info_wcdma::key_sig_str_wcdma] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_wcdma] = jsonWcdma;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_TD_SCDMA) {
                Json::Value jsonTdscdma;
                jsonTdscdma[ns_net_cell_info_tdscdma::key_cell_info_common] = commonInfo;
                jsonTdscdma[ns_net_cell_info_tdscdma::key_cell_identity_tdscdma] = jsonCellId;
                jsonTdscdma[ns_net_cell_info_tdscdma::key_sig_str_tdscdma] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_tdscdma] = jsonTdscdma;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_NR) {
                    Json::Value jsonNr;
                    jsonNr[ns_net_cell_info_nr::key_cell_info_common] = commonInfo;
                    jsonNr[ns_net_cell_info_nr::key_cell_identity_nr] = jsonCellId;
                    jsonNr[ns_net_cell_info_nr::key_sig_str_nr] = strInfo;
                    cellInfo[ns_net_cell_info::key_cell_info_nr] = jsonNr;
            }

            jsonData[ns_net_cell_info_list::key_cell_info_list].append(cellInfo);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        if (modemData == nullptr) free(plmns);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetCellInfoListAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        list<RIL_CellInfo_V1_6> &cellInfoList = testAdapter.GetCellInfoList(4);

        EXPECT_EQ(cellInfoList.size(), testVector[i].numOfCells);
        if (cellInfoList.size() != testVector[i].numOfCells)
            break;

        list<RIL_CellInfo_V1_6>::iterator iter;
        int j = 0;
        for (iter = cellInfoList.begin(); iter != cellInfoList.end(); iter++, j++) {
            RIL_CellInfo_V1_6 &cur = *iter;
            int rilCellType = testVector[i].cellType[j];
            int sitCellType = rilCellType - 1;

            EXPECT_EQ((int)cur.cellInfoType, rilCellType);
            EXPECT_EQ(cur.registered, (testVector[i].reg[sitCellType] == 1));
            EXPECT_EQ((int)cur.timeStampType, RIL_TIMESTAMP_TYPE_OEM_RIL);
            EXPECT_EQ((int)cur.connectionStatus, 1);

            if (cur.cellInfoType != rilCellType) break;

            switch ((int)rilCellType) {
                case RIL_CELL_INFO_TYPE_GSM:
                    EXPECT_EQ(IsSameRilCellStrGsm(cur.CellInfo.gsm.signalStrengthGsm, rilGsmSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdGsm(cur.CellInfo.gsm.cellIdentityGsm, testRilCellIds[sitCellType].gsm), 0);
                    break;
                case RIL_CELL_INFO_TYPE_CDMA:
                    EXPECT_EQ(IsSameRilCellStrCdma(cur.CellInfo.cdma.signalStrengthCdma, rilCdmaSigStr), 0);
                    EXPECT_EQ(IsSameRilCellStrEvdo(cur.CellInfo.cdma.signalStrengthEvdo, rilEvdoSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdCdma(cur.CellInfo.cdma.cellIdentityCdma, testRilCellIds[sitCellType].cdma), 0);
                    break;
                case RIL_CELL_INFO_TYPE_LTE:
                    EXPECT_EQ(cur.CellInfo.lte.isEndcAvailable, true);
                    EXPECT_EQ(IsSameRilCellStrLte(cur.CellInfo.lte.signalStrengthLte, rilLteSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdLte(cur.CellInfo.lte.cellIdentityLte, testRilCellIds[sitCellType].lte), 0);
                    break;
                case RIL_CELL_INFO_TYPE_WCDMA:
                    EXPECT_EQ(IsSameRilCellStrWcdma(cur.CellInfo.wcdma.signalStrengthWcdma, rilWcdmaSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdWcdma(cur.CellInfo.wcdma.cellIdentityWcdma, testRilCellIds[sitCellType].wcdma), 0);
                    break;
                case RIL_CELL_INFO_TYPE_TD_SCDMA:
                    EXPECT_EQ(IsSameRilCellStrTdscdma(cur.CellInfo.tdscdma.signalStrengthTdscdma, rilTdscdmaSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdTdscdma(cur.CellInfo.tdscdma.cellIdentityTdscdma, testRilCellIds[sitCellType].tdscdma), 0);
                    break;
                case RIL_CELL_INFO_TYPE_NR:
                    EXPECT_EQ(IsSameRilCellStrNr(cur.CellInfo.nr.signalStrengthNr, rilNrSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdNr(cur.CellInfo.nr.cellIdentityNr, testRilCellIds[sitCellType].nr), 0);
                    break;
            }
        } // end iter ~

        delete modemData;
        modemData = nullptr;
    }

    free(plmns);
}

TEST(ProtocolNetAdapterJson, ProtocolNetCellInfoListIndAdapterJson) {
    char **plmns = (char **)calloc(testAdditionaPlmnsNum, sizeof(char*));
    for (int i = 0; i < testAdditionaPlmnsNum; ++i) {
        *(plmns + i) = testAdditionaPlmns[i];
    }

    RIL_CellIdentity_V1_5 testRilCellIds[testCellTypesNum];
    memset(testRilCellIds, 0, sizeof(RIL_CellIdentity_V1_5) * testCellTypesNum);
    SetTestValueforCellIds(testRilCellIds, testCellTypesNum, testAdditionaPlmnsNum, plmns);

    struct {
        unsigned int numOfCells;
        int cellType[testCellTypesNum];
        int reg[testCellTypesNum];
    } testVector[] {
        { testCellTypesNum, {1, 2, 3, 4, 5, 6}, {1, 0, 0, 0, 0, 0} },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_CELL_INFO_LIST;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        for (unsigned int j = 0; j < testVector[i].numOfCells; ++j) {
            Json::Value cellInfo;

            int rilCellType = testVector[i].cellType[j];
            int sitCellType = rilCellType - 1;

            Json::Value commonInfo;
            Json::Value jsonCellId;
            Json::Value strInfo;

            UpdateTestCommonInfo(rilCellType, commonInfo, testVector[i].reg[sitCellType]);
            UpdateTestCellIdInfo(rilCellType, jsonCellId, testRilCellIds[sitCellType], testAdditionaPlmnsNum, plmns);
            UpdateTestStrInfo(rilCellType, strInfo);

            cellInfo[ns_net_cell_info::key_cell_info_type] = sitCellType;

            if (rilCellType == RIL_CELL_INFO_TYPE_GSM) {
                Json::Value jsonGsm;
                jsonGsm[ns_net_cell_info_gsm::key_cell_info_common] = commonInfo;
                jsonGsm[ns_net_cell_info_gsm::key_cell_identity_gsm] = jsonCellId;
                jsonGsm[ns_net_cell_info_gsm::key_sig_str_gsm] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_gsm] = jsonGsm;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_CDMA) {
                Json::Value jsonCdma;
                jsonCdma[ns_net_cell_info_cdma::key_cell_info_common] = commonInfo;
                jsonCdma[ns_net_cell_info_cdma::key_cell_identity_cdma] = jsonCellId;
                jsonCdma[ns_net_cell_info_cdma::key_sig_str_cdma] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_cdma] = jsonCdma;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_LTE) {
                Json::Value jsonLte;
                jsonLte[ns_net_cell_info_lte::key_cell_info_common] = commonInfo;
                jsonLte[ns_net_cell_info_lte::key_cell_identity_lte] = jsonCellId;
                jsonLte[ns_net_cell_info_lte::key_sig_str_lte] = strInfo;
                jsonLte[ns_net_cell_info_lte::key_endc_available] = 1;
                cellInfo[ns_net_cell_info::key_cell_info_lte] = jsonLte;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_WCDMA) {
                Json::Value jsonWcdma;
                jsonWcdma[ns_net_cell_info_wcdma::key_cell_info_common] = commonInfo;
                jsonWcdma[ns_net_cell_info_wcdma::key_cell_identity_wcdma] = jsonCellId;
                jsonWcdma[ns_net_cell_info_wcdma::key_sig_str_wcdma] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_wcdma] = jsonWcdma;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_TD_SCDMA) {
                Json::Value jsonTdscdma;
                jsonTdscdma[ns_net_cell_info_tdscdma::key_cell_info_common] = commonInfo;
                jsonTdscdma[ns_net_cell_info_tdscdma::key_cell_identity_tdscdma] = jsonCellId;
                jsonTdscdma[ns_net_cell_info_tdscdma::key_sig_str_tdscdma] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_tdscdma] = jsonTdscdma;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_NR) {
                    Json::Value jsonNr;
                    jsonNr[ns_net_cell_info_nr::key_cell_info_common] = commonInfo;
                    jsonNr[ns_net_cell_info_nr::key_cell_identity_nr] = jsonCellId;
                    jsonNr[ns_net_cell_info_nr::key_sig_str_nr] = strInfo;
                    cellInfo[ns_net_cell_info::key_cell_info_nr] = jsonNr;
            }

            jsonData[ns_net_cell_info_list::key_cell_info_list].append(cellInfo);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        if (modemData == nullptr) free(plmns);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetCellInfoListIndAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        list<RIL_CellInfo_V1_6> &cellInfoList = testAdapter.GetCellInfoList(4);

        EXPECT_EQ(cellInfoList.size(), testVector[i].numOfCells);
        if (cellInfoList.size() != testVector[i].numOfCells)
            break;

        list<RIL_CellInfo_V1_6>::iterator iter;
        int j = 0;
        for (iter = cellInfoList.begin(); iter != cellInfoList.end(); iter++, j++) {
            RIL_CellInfo_V1_6 &cur = *iter;
            int rilCellType = testVector[i].cellType[j];
            int sitCellType = rilCellType - 1;

            EXPECT_EQ((int)cur.cellInfoType, rilCellType);
            EXPECT_EQ(cur.registered, (testVector[i].reg[sitCellType] == 1));
            EXPECT_EQ((int)cur.timeStampType, RIL_TIMESTAMP_TYPE_OEM_RIL);
            EXPECT_EQ((int)cur.connectionStatus, 1);

            if (cur.cellInfoType != rilCellType) break;

            switch ((int)rilCellType) {
                case RIL_CELL_INFO_TYPE_GSM:
                    EXPECT_EQ(IsSameRilCellStrGsm(cur.CellInfo.gsm.signalStrengthGsm, rilGsmSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdGsm(cur.CellInfo.gsm.cellIdentityGsm, testRilCellIds[sitCellType].gsm), 0);
                    break;
                case RIL_CELL_INFO_TYPE_CDMA:
                    EXPECT_EQ(IsSameRilCellStrCdma(cur.CellInfo.cdma.signalStrengthCdma, rilCdmaSigStr), 0);
                    EXPECT_EQ(IsSameRilCellStrEvdo(cur.CellInfo.cdma.signalStrengthEvdo, rilEvdoSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdCdma(cur.CellInfo.cdma.cellIdentityCdma, testRilCellIds[sitCellType].cdma), 0);
                    break;
                case RIL_CELL_INFO_TYPE_LTE:
                    EXPECT_EQ(cur.CellInfo.lte.isEndcAvailable, true);
                    EXPECT_EQ(IsSameRilCellStrLte(cur.CellInfo.lte.signalStrengthLte, rilLteSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdLte(cur.CellInfo.lte.cellIdentityLte, testRilCellIds[sitCellType].lte), 0);
                    break;
                case RIL_CELL_INFO_TYPE_WCDMA:
                    EXPECT_EQ(IsSameRilCellStrWcdma(cur.CellInfo.wcdma.signalStrengthWcdma, rilWcdmaSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdWcdma(cur.CellInfo.wcdma.cellIdentityWcdma, testRilCellIds[sitCellType].wcdma), 0);
                    break;
                case RIL_CELL_INFO_TYPE_TD_SCDMA:
                    EXPECT_EQ(IsSameRilCellStrTdscdma(cur.CellInfo.tdscdma.signalStrengthTdscdma, rilTdscdmaSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdTdscdma(cur.CellInfo.tdscdma.cellIdentityTdscdma, testRilCellIds[sitCellType].tdscdma), 0);
                    break;
                case RIL_CELL_INFO_TYPE_NR:
                    EXPECT_EQ(IsSameRilCellStrNr(cur.CellInfo.nr.signalStrengthNr, rilNrSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdNr(cur.CellInfo.nr.cellIdentityNr, testRilCellIds[sitCellType].nr), 0);
                    break;
            }
        } // end iter ~

        delete modemData;
        modemData = nullptr;
    }

    free(plmns);
}

TEST(ProtocolNetAdapterJson, ProtocolNetScanResultAdapterJson) {
    char **plmns = (char **)calloc(testAdditionaPlmnsNum, sizeof(char*));
    for (int i = 0; i < testAdditionaPlmnsNum; ++i) {
        *(plmns + i) = testAdditionaPlmns[i];
    }

    RIL_CellIdentity_V1_5 testRilCellIds[testCellTypesNum];
    memset(testRilCellIds, 0, sizeof(RIL_CellIdentity_V1_5) * testCellTypesNum);
    SetTestValueforCellIds(testRilCellIds, testCellTypesNum, testAdditionaPlmnsNum, plmns);

    struct {
        int sitScanStatus;
        int rilScanStatus;
        unsigned int numOfCells;
        int cellType[testCellTypesNum];
        int reg[testCellTypesNum];
    } testVector[] {
        { SIT_NET_SCAN_STATUS_PARTIAL, PARTIAL, testCellTypesNum, {1, 2, 3, 4, 5, 6}, {1, 0, 0, 0, 0, 0} },
        { SIT_NET_SCAN_STATUS_COMPLETE, COMPLETE, testCellTypesNum, {1, 2, 3, 4, 5, 6}, {1, 0, 0, 0, 0, 0} },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_SCANNING_NETWORKS;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        jsonData[ns_net_scanning_networks_ind::key_status] = testVector[i].sitScanStatus;

        for (unsigned int j = 0; j < testVector[i].numOfCells; ++j) {
            Json::Value cellInfo;

            int rilCellType = testVector[i].cellType[j];
            int sitCellType = rilCellType - 1;

            Json::Value commonInfo;
            Json::Value jsonCellId;
            Json::Value strInfo;

            UpdateTestCommonInfo(rilCellType, commonInfo, testVector[i].reg[sitCellType]);
            UpdateTestCellIdInfo(rilCellType, jsonCellId, testRilCellIds[sitCellType], testAdditionaPlmnsNum, plmns);
            UpdateTestStrInfo(rilCellType, strInfo);

            cellInfo[ns_net_cell_info::key_cell_info_type] = sitCellType;

            if (rilCellType == RIL_CELL_INFO_TYPE_GSM) {
                Json::Value jsonGsm;
                jsonGsm[ns_net_cell_info_gsm::key_cell_info_common] = commonInfo;
                jsonGsm[ns_net_cell_info_gsm::key_cell_identity_gsm] = jsonCellId;
                jsonGsm[ns_net_cell_info_gsm::key_sig_str_gsm] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_gsm] = jsonGsm;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_CDMA) {
                Json::Value jsonCdma;
                jsonCdma[ns_net_cell_info_cdma::key_cell_info_common] = commonInfo;
                jsonCdma[ns_net_cell_info_cdma::key_cell_identity_cdma] = jsonCellId;
                jsonCdma[ns_net_cell_info_cdma::key_sig_str_cdma] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_cdma] = jsonCdma;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_LTE) {
                Json::Value jsonLte;
                jsonLte[ns_net_cell_info_lte::key_cell_info_common] = commonInfo;
                jsonLte[ns_net_cell_info_lte::key_cell_identity_lte] = jsonCellId;
                jsonLte[ns_net_cell_info_lte::key_sig_str_lte] = strInfo;
                jsonLte[ns_net_cell_info_lte::key_endc_available] = 1;
                cellInfo[ns_net_cell_info::key_cell_info_lte] = jsonLte;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_WCDMA) {
                Json::Value jsonWcdma;
                jsonWcdma[ns_net_cell_info_wcdma::key_cell_info_common] = commonInfo;
                jsonWcdma[ns_net_cell_info_wcdma::key_cell_identity_wcdma] = jsonCellId;
                jsonWcdma[ns_net_cell_info_wcdma::key_sig_str_wcdma] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_wcdma] = jsonWcdma;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_TD_SCDMA) {
                Json::Value jsonTdscdma;
                jsonTdscdma[ns_net_cell_info_tdscdma::key_cell_info_common] = commonInfo;
                jsonTdscdma[ns_net_cell_info_tdscdma::key_cell_identity_tdscdma] = jsonCellId;
                jsonTdscdma[ns_net_cell_info_tdscdma::key_sig_str_tdscdma] = strInfo;
                cellInfo[ns_net_cell_info::key_cell_info_tdscdma] = jsonTdscdma;
            }

            if (rilCellType == RIL_CELL_INFO_TYPE_NR) {
                    Json::Value jsonNr;
                    jsonNr[ns_net_cell_info_nr::key_cell_info_common] = commonInfo;
                    jsonNr[ns_net_cell_info_nr::key_cell_identity_nr] = jsonCellId;
                    jsonNr[ns_net_cell_info_nr::key_sig_str_nr] = strInfo;
                    cellInfo[ns_net_cell_info::key_cell_info_nr] = jsonNr;
            }

            jsonData[ns_net_scanning_networks_ind::key_cell_info_list].append(cellInfo);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        if (modemData == nullptr) free(plmns);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetScanResultAdapterJson testAdapter(modemData);

        // General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        EXPECT_EQ(testAdapter.GetScanStatus(), testVector[i].rilScanStatus);
        EXPECT_EQ(testAdapter.GetScanResult(), RIL_E_SUCCESS);

        // Checking cell info list
        list<RIL_CellInfo_V1_6> &cellInfoList = testAdapter.GetCellInfoList(4);

        EXPECT_EQ(cellInfoList.size(), testVector[i].numOfCells);
        if (cellInfoList.size() != testVector[i].numOfCells)
            break;

        list<RIL_CellInfo_V1_6>::iterator iter;
        int j = 0;
        for (iter = cellInfoList.begin(); iter != cellInfoList.end(); iter++, j++) {
            RIL_CellInfo_V1_6 &cur = *iter;
            int rilCellType = testVector[i].cellType[j];
            int sitCellType = rilCellType - 1;

            EXPECT_EQ((int)cur.cellInfoType, rilCellType);
            EXPECT_EQ(cur.registered, (testVector[i].reg[sitCellType] == 1));
            EXPECT_EQ((int)cur.timeStampType, RIL_TIMESTAMP_TYPE_OEM_RIL);
            EXPECT_EQ((int)cur.connectionStatus, 1);

            if (cur.cellInfoType != rilCellType) break;

            switch ((int)rilCellType) {
                case RIL_CELL_INFO_TYPE_GSM:
                    EXPECT_EQ(IsSameRilCellStrGsm(cur.CellInfo.gsm.signalStrengthGsm, rilGsmSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdGsm(cur.CellInfo.gsm.cellIdentityGsm, testRilCellIds[sitCellType].gsm), 0);
                    break;
                case RIL_CELL_INFO_TYPE_CDMA:
                    EXPECT_EQ(IsSameRilCellStrCdma(cur.CellInfo.cdma.signalStrengthCdma, rilCdmaSigStr), 0);
                    EXPECT_EQ(IsSameRilCellStrEvdo(cur.CellInfo.cdma.signalStrengthEvdo, rilEvdoSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdCdma(cur.CellInfo.cdma.cellIdentityCdma, testRilCellIds[sitCellType].cdma), 0);
                    break;
                case RIL_CELL_INFO_TYPE_LTE:
                    EXPECT_EQ(cur.CellInfo.lte.isEndcAvailable, true);
                    EXPECT_EQ(IsSameRilCellStrLte(cur.CellInfo.lte.signalStrengthLte, rilLteSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdLte(cur.CellInfo.lte.cellIdentityLte, testRilCellIds[sitCellType].lte), 0);
                    break;
                case RIL_CELL_INFO_TYPE_WCDMA:
                    EXPECT_EQ(IsSameRilCellStrWcdma(cur.CellInfo.wcdma.signalStrengthWcdma, rilWcdmaSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdWcdma(cur.CellInfo.wcdma.cellIdentityWcdma, testRilCellIds[sitCellType].wcdma), 0);
                    break;
                case RIL_CELL_INFO_TYPE_TD_SCDMA:
                    EXPECT_EQ(IsSameRilCellStrTdscdma(cur.CellInfo.tdscdma.signalStrengthTdscdma, rilTdscdmaSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdTdscdma(cur.CellInfo.tdscdma.cellIdentityTdscdma, testRilCellIds[sitCellType].tdscdma), 0);
                    break;
                case RIL_CELL_INFO_TYPE_NR:
                    EXPECT_EQ(IsSameRilCellStrNr(cur.CellInfo.nr.signalStrengthNr, rilNrSigStr), 0);
                    EXPECT_EQ(isSameRilCellIdNr(cur.CellInfo.nr.cellIdentityNr, testRilCellIds[sitCellType].nr), 0);
                    break;
            }
        } // end iter ~

        delete modemData;
        modemData = nullptr;
    }

    free(plmns);
}

TEST(ProtocolNetAdapterJson, ProtocolNetSimFileInfoAdapterJson) {
    struct {
        int nSimFileId;
        int nRecordLen;
        int nNumOfRecords;
        int recordInfoList[15];
    } testVector[] {
        { 28613 /* NET_SIM_EF_PNN */, 5, 1, {1, 2, 3, 4, 5} },
        { 28614 /* NET_SIM_EF_OPL */, 5, 2, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10} },
        { 20232 /* NET_SIM_EF_OPL5G */, 5, 3, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11, 12, 13, 14, 15} },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_SIM_DATA_INFO;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_sim_file_data_info_ind::key_sim_file_id] = testVector[i].nSimFileId;
        jsonData[ns_net_sim_file_data_info_ind::key_record_len] = testVector[i].nRecordLen;
        jsonData[ns_net_sim_file_data_info_ind::key_num_records] = testVector[i].nNumOfRecords;

        int nRecordInfoListSize = testVector[i].nRecordLen * testVector[i].nNumOfRecords;
        for(int j = 0; j < nRecordInfoListSize; j++) {
            jsonData[ns_net_sim_file_data_info_ind::key_ef_data_list].append(testVector[i].recordInfoList[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetSimFileInfoAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        //IPC response checks
        EXPECT_EQ(testAdapter.GetSimFileId(), testVector[i].nSimFileId);
        int nParsedRecordLen = testAdapter.GetRecordLen();
        EXPECT_EQ(nParsedRecordLen, testVector[i].nRecordLen);
        int nParsedNumRecords = testAdapter.GetNumOfRecords();
        EXPECT_EQ(nParsedNumRecords, testVector[i].nNumOfRecords);

        // Check records info data
        BYTE **ppRecordInfoList = testAdapter.GetSimFileData();

        for(int j = 0; j < nParsedNumRecords; j++) {
            BYTE *pRecord = ppRecordInfoList[j];
            int ind = j * nParsedRecordLen;
            for(int k = 0; k < nParsedRecordLen; k++, ind++) {
                EXPECT_EQ(pRecord[k], testVector[i].recordInfoList[ind]);
            }
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetPhysicalChannelConfigsJson_0) {
    struct {
        int sitRat;
        int sitScgStatus;
        int rilRat;
        int rilScgStatus;
    } testVector[] {
        { SIT_RAT_TYPE_5G, SIT_SCG_ADDED, RADIO_TECH_NR, SECONDARY_SERVING },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_SCG_BEARER_ALLOCATION;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_scg_bearer_alloc_ind::key_rat] = testVector[i].sitRat;
        jsonData[ns_net_scg_bearer_alloc_ind::key_scg_status] = testVector[i].sitScgStatus;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetPhysicalChannelConfigsJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        list<PhysicalChannelConfigsData> pccList;
        pccList = testAdapter.GetConfigList();
        EXPECT_EQ(pccList.size(), 1UL);

        PhysicalChannelConfigsData &firstOne = *(pccList.begin());
        EXPECT_EQ(firstOne.getRat(), testVector[i].rilRat);
        EXPECT_EQ(firstOne.getStatus(), testVector[i].rilScgStatus);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetPhysicalChannelConfigsJson_1) {
    int const pciListSize = 2;
    int const cotextIdsLen = 3;
    int const sitDataLen = 9;
    struct {
        int sitData[sitDataLen];
        int cotextIds[cotextIdsLen];
        int rilRat;
        int rilScgStatus;
    } testVector[] {
        { {PRIMARY_SERVING, 100, SIT_RAT_TYPE_5G, 200, 300, 400, 500, 600, 700 }, {1,2,3}, RADIO_TECH_NR, PRIMARY_SERVING },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_PHYSICAL_CHANNEL_CONFIG;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        for (int j = 0; j < pciListSize; ++j) {
            Json::Value jsonPci;
            jsonPci[ns_net_physical_channel_config_info::key_cell_status] = testVector[i].sitData[0];
            jsonPci[ns_net_physical_channel_config_info::key_cell_bw_dl] = testVector[i].sitData[1] + j;
            jsonPci[ns_net_physical_channel_config_info::key_rat] = testVector[i].sitData[2];
            jsonPci[ns_net_physical_channel_config_info::key_freq_range] = testVector[i].sitData[3] + j;
            jsonPci[ns_net_physical_channel_config_info::key_channel] = testVector[i].sitData[4] + j;
            jsonPci[ns_net_physical_channel_config_info::key_pcid] = testVector[i].sitData[5] + j;
            jsonPci[ns_net_physical_channel_config_info::key_channel_uplink] = testVector[i].sitData[6] + j;
            jsonPci[ns_net_physical_channel_config_info::key_cell_bw_ul] = testVector[i].sitData[7] + j;
            jsonPci[ns_net_physical_channel_config_info::key_band_num] = testVector[i].sitData[8] + j;

            for(int k = 0; k < cotextIdsLen; ++k) {
                jsonPci[ns_net_physical_channel_config_info::key_context_id_list].append(testVector[i].cotextIds[k] + j);
            }

            jsonData[ns_net_physical_channel_config_ind::key_pci_list].append(jsonPci);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetPhysicalChannelConfigsJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        list<PhysicalChannelConfigsData> pccList;
        pccList = testAdapter.GetConfigList();
        EXPECT_EQ(pccList.size(),(unsigned long)  pciListSize);

        list<PhysicalChannelConfigsData>::iterator iter;
        int j = 0;
        for (iter = pccList.begin(); iter != pccList.end(); iter++) {
            PhysicalChannelConfigsData &cur = *iter;

            EXPECT_EQ(cur.getStatus(), testVector[i].rilScgStatus);
            EXPECT_EQ(cur.getCellBandwidthDownlink(), testVector[i].sitData[1] + j);
            EXPECT_EQ(cur.getRat(), testVector[i].rilRat);
            EXPECT_EQ(cur.getRange(), testVector[i].sitData[3] + j);
            EXPECT_EQ(cur.getChannelNumber(), testVector[i].sitData[4] + j);
            EXPECT_EQ(cur.getPhysicalCellId(), testVector[i].sitData[5] + j);
            EXPECT_EQ(cur.getUplinkChannelNumber(), testVector[i].sitData[6] + j);
            EXPECT_EQ(cur.getCellBandwidthUplink(), testVector[i].sitData[7] + j);
            EXPECT_EQ(cur.getBandNumber(), testVector[i].sitData[8] + j);

            EXPECT_EQ(cur.getLenContextIds(), cotextIdsLen);
            for( int k = 0; k < cur.getLenContextIds() && k < cotextIdsLen; ++k ){
                EXPECT_EQ(cur.getContextIds()[k], testVector[i].cotextIds[k] + j);
            }
            ++j;
        }

        delete modemData;
        modemData = nullptr;
    }

}

TEST(ProtocolNetAdapterJson, ProtocolNetGetManualRatModeAdapterJson) {
    struct {
        int mode;
        int sitPrefNet;
    } testVector[] {
        { SIT_NET_ENABLE, SIT_NET_PREF_NET_TYPE_GSM_WCDMA_AUTO },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_MANUAL_RAT_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_manual_rat_mode::key_manual_rat_mode] = testVector[i].mode;
        jsonData[ns_net_getset_manual_rat_mode::key_pref_net_type] = testVector[i].sitPrefNet;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetManualRatModeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        int manualRatModeData[2] = {0,};
        testAdapter.GetManualRatMode(manualRatModeData);
        EXPECT_EQ(manualRatModeData[0], testVector[i].mode);
        EXPECT_EQ(manualRatModeData[1], testVector[i].sitPrefNet);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetSetManualRatModeAdapterJson) {
    struct {
        int ratCause;
    } testVector[] {
        { 1 },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_SET_MANUAL_RAT_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_set_manual_rat_mode_rsp::key_manual_rat_cause] = testVector[i].ratCause;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetSetManualRatModeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetCause(), testVector[i].ratCause);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetGetFreqLockAdapterJson) {
    const int sizeFreqLockData = 7;
    struct {
        int freqLockData[sizeFreqLockData];
        int rilfreqLockData[sizeFreqLockData];
    } testVector[] {
        { {1, SIT_FREQ_LOCK_RAT_GSM, 3, 4, 5, 6, 7}, {1, SIT_FREQ_LOCK_RAT_GSM, 0, 0, 5, 0, 0} },
        { {1, SIT_FREQ_LOCK_RAT_WCDMA, 3, 4, 5, 6, 7}, {1, SIT_FREQ_LOCK_RAT_WCDMA, 0, 0, 0, 6, 7} },
        { {1, SIT_FREQ_LOCK_RAT_LTE, 3, 4, 5, 6, 7}, {1, SIT_FREQ_LOCK_RAT_LTE, 3, 4, 0, 0, 0} },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_FREQUENCY_LOCK;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_freq_lock::key_freq_lock_mode] = testVector[i].freqLockData[0];
        jsonData[ns_net_getset_freq_lock::key_rat] = testVector[i].freqLockData[1];
        jsonData[ns_net_getset_freq_lock::key_pcid] = testVector[i].freqLockData[2];
        jsonData[ns_net_getset_freq_lock::key_earfcn] = testVector[i].freqLockData[3];
        jsonData[ns_net_getset_freq_lock::key_arfcn] = testVector[i].freqLockData[4];
        jsonData[ns_net_getset_freq_lock::key_psc] = testVector[i].freqLockData[5];
        jsonData[ns_net_getset_freq_lock::key_uarfcn] = testVector[i].freqLockData[6];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetFreqLockAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        int freqLockData[sizeFreqLockData] = {};
        testAdapter.GetFrequencyLock(freqLockData);
        for (int j = 0; j < sizeFreqLockData; ++ j) EXPECT_EQ(freqLockData[j], testVector[i].rilfreqLockData[j]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetSetFreqLockAdapterJson) {
    struct {
        int result;
    } testVector[] {
        { 2 },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_SET_FREQUENCY_LOCK;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_set_freq_lock_rsp::key_freq_lock_cause] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetSetFreqLockAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetGetEndcModeAdapterJson) {
    struct {
        int endcMode;
        int scgRelease;
    } testVector[] {
        { SIT_NET_ENABLE, SIT_ENDC_RELEASE },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_ENDC_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_endc_mode::key_endc_mode] = testVector[i].endcMode;
        jsonData[ns_net_getset_endc_mode::key_immediate_scg_rel] = testVector[i].scgRelease;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetEndcModeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetEndcMode(), testVector[i].endcMode);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetGetFrequencyInfoAdapterJson) {
    const int freqInfoTypePrimary = 0;
    const int freqInfoTypeSecondary = 1;
    struct {
        int secRat;
        int secBand;
        int secFreq;
        int priRat;
        int priBand;
        int priFreq;
    } testVector[] {
        { 2, 4, 6, 3, 5, 7},
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_FREQUENCY_INFO;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        Json::Value priData;
        Json::Value secData;
        secData[ns_net_freq_info::key_info_type] = freqInfoTypeSecondary;
        secData[ns_net_freq_info::key_rat] = testVector[i].secRat;
        secData[ns_net_freq_info::key_band] = testVector[i].secBand;
        secData[ns_net_freq_info::key_freq_number] = testVector[i].secFreq;
        priData[ns_net_freq_info::key_info_type] = freqInfoTypePrimary;
        priData[ns_net_freq_info::key_rat] = testVector[i].priRat;
        priData[ns_net_freq_info::key_band] = testVector[i].priBand;
        priData[ns_net_freq_info::key_freq_number] = testVector[i].priFreq;
        jsonData[ns_net_frequency_info_list::key_freq_info_list].append(secData);
        jsonData[ns_net_frequency_info_list::key_freq_info_list].append(priData);

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetFrequencyInfoAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetPrimaryRat(), testVector[i].priRat);
        EXPECT_EQ(testAdapter.GetPrimaryBand(), testVector[i].priBand);
        EXPECT_EQ(testAdapter.GetPrimaryFrequency(), testVector[i].priFreq);
        EXPECT_EQ(testAdapter.GetSecondaryRat(), testVector[i].secRat);
        EXPECT_EQ(testAdapter.GetSecondaryBand(), testVector[i].secBand);
        EXPECT_EQ(testAdapter.GetSecondaryFrequency(), testVector[i].secFreq);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetworkFrequencyInfoIndAdapterJson) {
    const int freqInfoTypePrimary = 0;
    const int freqInfoTypeSecondary = 1;
    struct {
        int secRat;
        int secBand;
        int secFreq;
        int priRat;
        int priBand;
        int priFreq;
    } testVector[] {
        { 2, 4, 6, 3, 5, 7},
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_FREQUENCY_INFO;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        Json::Value priData;
        Json::Value secData;
        secData[ns_net_freq_info::key_info_type] = freqInfoTypeSecondary;
        secData[ns_net_freq_info::key_rat] = testVector[i].secRat;
        secData[ns_net_freq_info::key_band] = testVector[i].secBand;
        secData[ns_net_freq_info::key_freq_number] = testVector[i].secFreq;
        priData[ns_net_freq_info::key_info_type] = freqInfoTypePrimary;
        priData[ns_net_freq_info::key_rat] = testVector[i].priRat;
        priData[ns_net_freq_info::key_band] = testVector[i].priBand;
        priData[ns_net_freq_info::key_freq_number] = testVector[i].priFreq;
        jsonData[ns_net_frequency_info_list::key_freq_info_list].append(secData);
        jsonData[ns_net_frequency_info_list::key_freq_info_list].append(priData);

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetworkFrequencyInfoIndAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        EXPECT_EQ(testAdapter.GetPrimaryRat(), testVector[i].priRat);
        EXPECT_EQ(testAdapter.GetPrimaryBand(), testVector[i].priBand);
        EXPECT_EQ(testAdapter.GetPrimaryFrequency(), testVector[i].priFreq);
        EXPECT_EQ(testAdapter.GetSecondaryRat(), testVector[i].secRat);
        EXPECT_EQ(testAdapter.GetSecondaryBand(), testVector[i].secBand);
        EXPECT_EQ(testAdapter.GetSecondaryFrequency(), testVector[i].secFreq);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetAcBarringInfoJson) {
    int barringEmc = 0;
    struct {
        int infoType;
        int factor;
        int time;
        int specialAcList[5];
    } testVector[] {
        { SIT_AC_BARRING_TYPE_MO_SIG, 0, 123, {1,2,3,4,5} },
        { SIT_AC_BARRING_TYPE_MO_DATA, 0, 123, {1,2,3,4,5} },
        { SIT_AC_BARRING_TYPE_MMTEL_VOICE, 0, 123, {1,2,3,4,5} },
        { SIT_AC_BARRING_TYPE_MMTEL_VIDEO, 0, 123, {1,2,3,4,5} },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_AC_BARRING_INFO;

    Json::Value jsonData;
    jsonData[ns_net_ac_barring_info_ind::key_ac_barring_emc] = barringEmc;

    for(int i = 0; i < size; i++) {

        Json::Value jsonAcBarringInfo;
        jsonAcBarringInfo[ns_net_ac_barring_info::key_ac_barring_info_type] = testVector[i].infoType;

        Json::Value jsonAcBarring;
        jsonAcBarring[ns_net_ac_barring::key_barring_factor] = testVector[i].factor;
        jsonAcBarring[ns_net_ac_barring::key_barring_time] = testVector[i].time;

        for(int j = 0; j <= i; j++) { // variable size list for each kind of barring information
            jsonAcBarring[ns_net_ac_barring::key_special_ac_list] = testVector[i].specialAcList[j];
        }

        if(testVector[i].infoType == SIT_AC_BARRING_TYPE_MO_SIG) {
            jsonAcBarringInfo[ns_net_ac_barring_info::key_mo_sig] = jsonAcBarring;
        } else if(testVector[i].infoType == SIT_AC_BARRING_TYPE_MO_DATA) {
            jsonAcBarringInfo[ns_net_ac_barring_info::key_mo_data] = jsonAcBarring;
        } else if(testVector[i].infoType == SIT_AC_BARRING_TYPE_MMTEL_VOICE) {
            jsonAcBarringInfo[ns_net_ac_barring_info::key_mmtel_voice] = jsonAcBarring;
        } else if(testVector[i].infoType == SIT_AC_BARRING_TYPE_MMTEL_VIDEO) {
            jsonAcBarringInfo[ns_net_ac_barring_info::key_mmtel_video] = jsonAcBarring;
        }

        jsonData[ns_net_ac_barring_info_ind::key_ac_barring_info_list].append(jsonAcBarringInfo);
    }


    ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
    ASSERT_NE(modemData, nullptr);


    ProtocolNetAcBarringInfoJson testAdapter(modemData);

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);

    char data[5];
    testAdapter.GetAcBarringInfo(data, 5);
    EXPECT_EQ((int)data[0], barringEmc);
    EXPECT_EQ((int)data[1], testVector[0].factor);
    EXPECT_EQ((int)data[2], testVector[1].factor);
    EXPECT_EQ((int)data[3], testVector[2].factor);
    EXPECT_EQ((int)data[4], testVector[3].factor);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolNetAdapterJson, ProtocolNetRrcInfoAdapterJson) {
    struct {
        int sitRat;
        int sitRrcState;
        int rilRat;
    } testVector[] {
        { SIT_RAT_TYPE_GPRS, RRC_CONNECT, RADIO_TECH_GPRS},
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_RRC_INFO;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_rrc_info_ind::key_rat] = testVector[i].sitRat;
        jsonData[ns_net_rrc_info_ind::key_rrc_state] = testVector[i].sitRrcState;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetRrcInfoAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        EXPECT_EQ(testAdapter.GetRat(), testVector[i].rilRat);
        EXPECT_EQ(testAdapter.GetState(), testVector[i].sitRrcState);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetGetNrModeAdapterJson) {
    struct {
        int nrMode;
    } testVector[] {
        { SIT_NR_MODE_NSA },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_NR_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_nr_mode::key_nr_mode] = testVector[i].nrMode;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetNrModeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetNrMode(), testVector[i].nrMode);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetGetNrSilenceModeAdapterJson) {
    struct {
        int nsaMode;
        int saMode;
    } testVector[] {
        { 0x01, 0x00},
        { 0x00, 0x01},
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_NR_SILENCE_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_net_misc_get_nr_silence_mode_rsp::key_nsa_mode] = testVector[i].nsaMode;
        jsonRspData[ns_net_misc_get_nr_silence_mode_rsp::key_sa_mode] = testVector[i].saMode;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetNrSilenceModeAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        //EXPECT_EQ(testAdapter.GetLength(),  totalLen);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //EXPECT_EQ(testAdapter.GetParameterLength(), rspDataLen);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetNsaMode(), testVector[i].nsaMode);
        EXPECT_EQ(testAdapter.GetSaMode(), testVector[i].saMode);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolRegistrationFailedAdapterJson) {
    char **plmns = (char **)calloc(testAdditionaPlmnsNum, sizeof(char*));
    for (int i = 0; i < testAdditionaPlmnsNum; ++i) {
        *(plmns + i) = testAdditionaPlmns[i];
    }

    RIL_CellIdentity_V1_5 testRilCellIds[testCellTypesNum];
    memset(testRilCellIds, 0, sizeof(RIL_CellIdentity_V1_5) * testCellTypesNum);
    SetTestValueforCellIds(testRilCellIds, testCellTypesNum, testAdditionaPlmnsNum, plmns);

    struct {
        char plmn[7];
        int domain;
        int rejCause;
        int addRejCause;
        int cellType;
        int rilDomain;
    } testVector[] {
        { "11111", SERVICE_DOMAIN_UNKNOWN, 1, 2, RIL_CELL_INFO_TYPE_GSM, 0 },
        { "22222", SERVICE_DOMAIN_CS, 1, 2, RIL_CELL_INFO_TYPE_CDMA, DOMAIN_CS},
        { "33333", SERVICE_DOMAIN_PS, 1, 2, RIL_CELL_INFO_TYPE_LTE, DOMAIN_PS },
        { "444444", SERVICE_DOMAIN_CSPS, 1, 2, RIL_CELL_INFO_TYPE_WCDMA, DOMAIN_CS|DOMAIN_PS},
        { "555555", SERVICE_DOMAIN_CSPS, 1, 2, RIL_CELL_INFO_TYPE_TD_SCDMA, DOMAIN_CS|DOMAIN_PS},
        { "666666", SERVICE_DOMAIN_CSPS, 1, 2, RIL_CELL_INFO_TYPE_NR, DOMAIN_CS|DOMAIN_PS },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_REG_FAILED;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        int rilCellType = testVector[i].cellType;
        int sitCellType = testVector[i].cellType - 1;
        jsonData[ns_net_reg_failed_ind ::key_plmn] = string(testVector[i].plmn);
        jsonData[ns_net_reg_failed_ind ::key_domain] = testVector[i].domain;
        jsonData[ns_net_reg_failed_ind ::key_rej_cause] = testVector[i].rejCause;
        jsonData[ns_net_reg_failed_ind ::key_add_rej_cause] = testVector[i].addRejCause;
        jsonData[ns_net_reg_failed_ind ::key_cell_info_type] = sitCellType;

        Json::Value jsonCellId;
        UpdateTestCellIdInfo(rilCellType, jsonCellId, testRilCellIds[sitCellType], testAdditionaPlmnsNum, plmns);
        switch (rilCellType) {
            case RIL_CELL_INFO_TYPE_GSM:
                jsonData[ns_net_reg_failed_ind ::key_cell_identity_gsm] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_CDMA:
                jsonData[ns_net_reg_failed_ind ::key_cell_identity_cdma] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_LTE:
                jsonData[ns_net_reg_failed_ind ::key_cell_identity_lte] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_WCDMA:
                jsonData[ns_net_reg_failed_ind ::key_cell_identity_wcdma] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_TD_SCDMA:
                jsonData[ns_net_reg_failed_ind ::key_cell_identity_tdscdma] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_NR:
                jsonData[ns_net_reg_failed_ind ::key_cell_identity_nr] = jsonCellId;
                break;
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        if (modemData == nullptr) free(plmns);
        ASSERT_NE(modemData, nullptr);

        ProtocolRegistrationFailedAdapterJson testAdapter(modemData, 4);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        RIL_RegistrationFailed *result = testAdapter.GetRegistrationFailed();

        EXPECT_STREQ(result->chosenPlmn, testVector[i].plmn);
        EXPECT_EQ(result->domainBitmap, testVector[i].rilDomain);
        EXPECT_EQ(result->causeCode, testVector[i].rejCause);
        EXPECT_EQ(result->additionalCauseCode, testVector[i].addRejCause);
        EXPECT_EQ(IsSameRilCellId(result->cellIdentity, testRilCellIds[sitCellType]), 0);

        delete modemData;
        modemData = nullptr;
    }

    free(plmns);
}

TEST(ProtocolNetAdapterJson, ProtocolGetBarringInfoRspAdapterJson) {
    char **plmns = (char **)calloc(testAdditionaPlmnsNum, sizeof(char*));
    for (int i = 0; i < testAdditionaPlmnsNum; ++i) {
        *(plmns + i) = testAdditionaPlmns[i];
    }

    RIL_CellIdentity_V1_5 testRilCellIds[testCellTypesNum];
    memset(testRilCellIds, 0, sizeof(RIL_CellIdentity_V1_5) * testCellTypesNum);
    SetTestValueforCellIds(testRilCellIds, testCellTypesNum, testAdditionaPlmnsNum, plmns);

    struct {
        int cellType;
        unsigned int numOfBarringRecords;
        int barringRecords[2][5];
    } testVector[] {
        { RIL_CELL_INFO_TYPE_GSM, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_CDMA, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_LTE, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_WCDMA, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_TD_SCDMA, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_NR, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_BARRING_INFOS;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        int rilCellType = testVector[i].cellType;
        int sitCellType = testVector[i].cellType - 1;
        jsonData[ns_net_barring_info_changed::key_cell_info_type] = sitCellType;

        Json::Value jsonCellId;
        UpdateTestCellIdInfo(rilCellType, jsonCellId, testRilCellIds[sitCellType], testAdditionaPlmnsNum, plmns);
        switch (rilCellType) {
            case RIL_CELL_INFO_TYPE_GSM:
                jsonData[ns_net_barring_info_changed::key_cell_identity_gsm] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_CDMA:
                jsonData[ns_net_barring_info_changed::key_cell_identity_cdma] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_LTE:
                jsonData[ns_net_barring_info_changed::key_cell_identity_lte] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_WCDMA:
                jsonData[ns_net_barring_info_changed::key_cell_identity_wcdma] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_TD_SCDMA:
                jsonData[ns_net_barring_info_changed::key_cell_identity_tdscdma] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_NR:
                jsonData[ns_net_barring_info_changed::key_cell_identity_nr] = jsonCellId;
                break;
        }

        for (unsigned int j = 0; j < testVector[i].numOfBarringRecords; ++j) {
            Json::Value barrringInfo;
            barrringInfo[ns_net_barring_info::key_service_type] = testVector[i].barringRecords[j][0];
            barrringInfo[ns_net_barring_info::key_barring_type] = testVector[i].barringRecords[j][1];
            barrringInfo[ns_net_barring_info::key_barring_factor] = testVector[i].barringRecords[j][2];
            barrringInfo[ns_net_barring_info::key_time_seconds] = testVector[i].barringRecords[j][3];
            barrringInfo[ns_net_barring_info::key_is_barred] = testVector[i].barringRecords[j][4];

            jsonData[ns_net_barring_info_changed::key_cell_barring_info_list].append(barrringInfo);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        if (modemData == nullptr) free(plmns);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetBarringInfoRspAdapterJson testAdapter(modemData, 4);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        RIL_CellIdAndBarringInfo *result = testAdapter.GetCellIdAndBarringInfo();

        EXPECT_EQ(result->numOfrecord, testVector[i].numOfBarringRecords);
        for (size_t j = 0; j < result->numOfrecord && j < testVector[i].numOfBarringRecords; ++j) {
            EXPECT_EQ(result->precords[j].service, testVector[i].barringRecords[j][0]);
            EXPECT_EQ(result->precords[j].type, testVector[i].barringRecords[j][1]);
            EXPECT_EQ(result->precords[j].typeSpecificInfo.conditionalBarringInfo.barringFactor, testVector[i].barringRecords[j][2]);
            EXPECT_EQ(result->precords[j].typeSpecificInfo.conditionalBarringInfo.barringTimeSeconds, testVector[i].barringRecords[j][3]);
            EXPECT_EQ(result->precords[j].typeSpecificInfo.conditionalBarringInfo.isBarred, (testVector[i].barringRecords[j][4] > 0));
        }

        EXPECT_EQ(IsSameRilCellId(result->cellIdentity, testRilCellIds[sitCellType]), 0);

        delete modemData;
        modemData = nullptr;
    }

    free(plmns);
}

TEST(ProtocolNetAdapterJson, ProtocolBarringInfoChangedAdapterJson) {
    char **plmns = (char **)calloc(testAdditionaPlmnsNum, sizeof(char*));
    for (int i = 0; i < testAdditionaPlmnsNum; ++i) {
        *(plmns + i) = testAdditionaPlmns[i];
    }

    RIL_CellIdentity_V1_5 testRilCellIds[testCellTypesNum];
    memset(testRilCellIds, 0, sizeof(RIL_CellIdentity_V1_5) * testCellTypesNum);
    SetTestValueforCellIds(testRilCellIds, testCellTypesNum, testAdditionaPlmnsNum, plmns);

    struct {
        int cellType;
        unsigned int numOfBarringRecords;
        int barringRecords[2][5];
    } testVector[] {
        { RIL_CELL_INFO_TYPE_GSM, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_CDMA, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_LTE, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_WCDMA, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_TD_SCDMA, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
        { RIL_CELL_INFO_TYPE_NR, 2,{{1,2,3,4,0}, {5,6,7,8,1}} },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_IND_BARRING_INFO_CHANGED;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;

        int rilCellType = testVector[i].cellType;
        int sitCellType = testVector[i].cellType - 1;
        jsonData[ns_net_barring_info_changed::key_cell_info_type] = sitCellType;

        Json::Value jsonCellId;
        UpdateTestCellIdInfo(rilCellType, jsonCellId, testRilCellIds[sitCellType], testAdditionaPlmnsNum, plmns);
        switch (rilCellType) {
            case RIL_CELL_INFO_TYPE_GSM:
                jsonData[ns_net_barring_info_changed::key_cell_identity_gsm] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_CDMA:
                jsonData[ns_net_barring_info_changed::key_cell_identity_cdma] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_LTE:
                jsonData[ns_net_barring_info_changed::key_cell_identity_lte] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_WCDMA:
                jsonData[ns_net_barring_info_changed::key_cell_identity_wcdma] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_TD_SCDMA:
                jsonData[ns_net_barring_info_changed::key_cell_identity_tdscdma] = jsonCellId;
                break;
            case RIL_CELL_INFO_TYPE_NR:
                jsonData[ns_net_barring_info_changed::key_cell_identity_nr] = jsonCellId;
                break;
        }

        for (unsigned int j = 0; j < testVector[i].numOfBarringRecords; ++j) {
            Json::Value barrringInfo;
            barrringInfo[ns_net_barring_info::key_service_type] = testVector[i].barringRecords[j][0];
            barrringInfo[ns_net_barring_info::key_barring_type] = testVector[i].barringRecords[j][1];
            barrringInfo[ns_net_barring_info::key_barring_factor] = testVector[i].barringRecords[j][2];
            barrringInfo[ns_net_barring_info::key_time_seconds] = testVector[i].barringRecords[j][3];
            barrringInfo[ns_net_barring_info::key_is_barred] = testVector[i].barringRecords[j][4];

            jsonData[ns_net_barring_info_changed::key_cell_barring_info_list].append(barrringInfo);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(sitId, jsonData);
        if (modemData == nullptr) free(plmns);
        ASSERT_NE(modemData, nullptr);

        ProtocolBarringInfoChangedAdapterJson testAdapter(modemData, 4);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        RIL_CellIdAndBarringInfo *result = testAdapter.GetCellIdAndBarringInfo();

        EXPECT_EQ(result->numOfrecord, testVector[i].numOfBarringRecords);
        for (size_t j = 0; j < result->numOfrecord && j < testVector[i].numOfBarringRecords; ++j) {
            EXPECT_EQ(result->precords[j].service, testVector[i].barringRecords[j][0]);
            EXPECT_EQ(result->precords[j].type, testVector[i].barringRecords[j][1]);
            EXPECT_EQ(result->precords[j].typeSpecificInfo.conditionalBarringInfo.barringFactor, testVector[i].barringRecords[j][2]);
            EXPECT_EQ(result->precords[j].typeSpecificInfo.conditionalBarringInfo.barringTimeSeconds, testVector[i].barringRecords[j][3]);
            EXPECT_EQ(result->precords[j].typeSpecificInfo.conditionalBarringInfo.isBarred, (testVector[i].barringRecords[j][4] > 0));
        }

        EXPECT_EQ(IsSameRilCellId(result->cellIdentity, testRilCellIds[sitCellType]), 0);

        delete modemData;
        modemData = nullptr;
    }

    free(plmns);
}

TEST(ProtocolNetAdapterJson, ProtocolNetGetVonrCapaAdapterJson) {
    struct {
        int vonrCapa;
    } testVector[] {
        { 0 }, // Disabled
        { 1 }, // Enabled
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_VONR_CAPABILITY;

    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_net_misc_get_vonr_capability_rsp::key_vonr_capa] = testVector[i].vonrCapa;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetVonrCapaAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        //EXPECT_EQ(testAdapter.GetLength(),  totalLen);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //EXPECT_EQ(testAdapter.GetParameterLength(), rspDataLen);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetVonrCapa(), testVector[i].vonrCapa);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetGetAllowNetworkAdapterJson) {
    struct {
        int sitRaf;
        unsigned int raf;
    } testVector[] {
        { 0xFFFFF, (RAF_RIL_3GPP|RAF_RIL_3GPP2|RAF_NR)},
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_ALLOW_NETWORK;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_net_getset_allow_network::key_raf] = testVector[i].sitRaf;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetGetAllowNetworkAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetRat(), testVector[i].raf);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetSystemSelectionChannelsAdapterJson) {
    struct {
        int sitData;
        int rilData;
    } testVector[] {
        { 1, 1},
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_GET_SYSTEM_SELECTION_CHANNELS;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData = Json::nullValue;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolNetSystemSelectionChannelsAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // json is not defined yet. so not checking rsp.
        testAdapter.GetSpecifiersLength();
        testAdapter.GetRadioAccessSpecifier();

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapterJson, ProtocolNetB1B2ConfigIndAdapterJson) {
    // TODO: Make proper test data

    Json::Value jsonIndData;

    ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_B2_B1_CONFIG, jsonIndData);
    ASSERT_NE(modemData, nullptr);

    ProtocolNetB1B2ConfigIndAdapterJson testAdapter(modemData);
    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_B2_B1_CONFIG);

#ifdef OEM_JSON_APPFW_CONVERTION
    // TODO: Payload data checks
#else
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    ASSERT_NE(testAdapter.GetParameterLength(), 0U);
#endif //OEM_JSON_APPFW_CONVERTION

    delete modemData;
    modemData = nullptr;
}

