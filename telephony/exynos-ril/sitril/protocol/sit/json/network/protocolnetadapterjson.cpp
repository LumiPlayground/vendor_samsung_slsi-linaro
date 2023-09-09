/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolnetadapterjson.h"
#include "netprotocolutils.h"
#include "protocolutils.h"
#include "mcctable.h"
#include "rillog.h"
#include "json/json.h"
#include "netjsondef.h"
#include "DataWriter.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

static bool debug = true;

static bool Is2g3gRegInfo(int rilRat) {
    return rilRat == RADIO_TECH_GPRS
            || rilRat == RADIO_TECH_EDGE
            || rilRat == RADIO_TECH_UMTS
            || rilRat == RADIO_TECH_HSDPA
            || rilRat == RADIO_TECH_HSUPA
            || rilRat == RADIO_TECH_HSPA
            || rilRat == RADIO_TECH_HSPAP
            || rilRat == RADIO_TECH_GSM
            || rilRat == RADIO_TECH_TD_SCDMA;
}

static bool IsLteRegInfo(int rilRat) {
    return rilRat == RADIO_TECH_LTE
            || rilRat == RADIO_TECH_LTE_CA;
}

static bool IsNrRegInfo(int rilRat) {
    return rilRat == RADIO_TECH_NR;
}

static bool IsCdmaRegInfo(int rilRat) {
    return rilRat == RADIO_TECH_IS95A
            || rilRat == RADIO_TECH_IS95B
            || rilRat == RADIO_TECH_1xRTT
            || rilRat == RADIO_TECH_EVDO_0
            || rilRat == RADIO_TECH_EVDO_A
            || rilRat == RADIO_TECH_EVDO_B
            || rilRat == RADIO_TECH_EHRPD;
}

/**
 * CellInfoAdapterFactoryJson
 */
CellInfoListAdapterJson *CellInfoAdapterFactoryJson::GetCellInfoListAdapterJson(int cellInfoVersion) {
    if (cellInfoVersion == 4) {
        RilLogI("CellInfoAdapterFactoryJson::GetCellInfoListAdapter cellInfoVersion=%d", cellInfoVersion);
        return &mCellInfoV4;
    } else {
        // if cellInfoVersion is invalid, return default CellInfo Version
        RilLogW("CellInfoAdapterFactoryJson::GetCellInfoListAdapter cellInfoVersion=%d", cellInfoVersion);
        return &mCellInfoV4;
    }
}

/*
 * CellInfoListParserV4Json
 */
void CellInfoListParserV4Json::FillCellInfo(int cellInfoNum,  Json::Value &jsonData)
{
    mCellInfoNum = cellInfoNum;

    if (mCellInfoNum <= 0) {
        return ;
    }

    for (int i = 0; i < mCellInfoNum; i++) {
        Json::Value jsonCellInfo = jsonData[i];

        RIL_CellInfo_V1_6 rilCellInfo;
        memset(&rilCellInfo, 0, sizeof(rilCellInfo));
        rilCellInfo.cellInfoType = (RIL_CellInfoType)(jsonCellInfo[ns_net_cell_info::key_cell_info_type].asInt() + 1);
        CellInfoProtocolUtils::InitCellInfo(rilCellInfo, (int)rilCellInfo.cellInfoType);

        switch ((int)rilCellInfo.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            FillGsmCellInfo(rilCellInfo, jsonCellInfo[ns_net_cell_info::key_cell_info_gsm]);
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            FillCdmaCellInfo(rilCellInfo, jsonCellInfo[ns_net_cell_info::key_cell_info_cdma]);
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            FillLteCellInfo(rilCellInfo, jsonCellInfo[ns_net_cell_info::key_cell_info_lte]);
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            FillWcdmaCellInfo(rilCellInfo, jsonCellInfo[ns_net_cell_info::key_cell_info_wcdma]);
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            FillTdscdmaCellInfo(rilCellInfo, jsonCellInfo[ns_net_cell_info::key_cell_info_tdscdma]);
            break;
        case RIL_CELL_INFO_TYPE_NR:
            FillNrCellInfo(rilCellInfo, jsonCellInfo[ns_net_cell_info::key_cell_info_nr]);
            break;
        default:
            // TODO stop parsing because of untrusted data type
            RilLogW("[CellInfoListParserV4Json::%s][%d] Unsupported CellInfoType %d", __FUNCTION__, (int)rilCellInfo.cellInfoType);
            return;
        }


        if (NetProtocolUtils::IsValidCellInfo(rilCellInfo)) {
            mCellInfoList.push_back(rilCellInfo);
            if (debug) {
                CellInfoProtocolUtils::PrintCellInfo(i, rilCellInfo);
            }
        } else {
            RilLogW("CellInfoListParserV4Json::%s[%d] Drop a cell information, cellType=%d", __FUNCTION__, i, (int)rilCellInfo.cellInfoType);
        }
    }
}

void CellInfoListParserV4Json::FillGsmCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    Json::Value jsonCommonInfo = jsonData[ns_net_cell_info_gsm::key_cell_info_common];
    FillCellCommon(dst, jsonCommonInfo);

    Json::Value jsonIdInfo = jsonData[ns_net_cell_info_gsm::key_cell_identity_gsm];
    FillCellIdentityGsm(dst, jsonIdInfo);

    Json::Value jsonStrInfo = jsonData[ns_net_cell_info_gsm::key_sig_str_gsm];
    int val = 0;
    // BER (0-7,99)
    val = jsonStrInfo[ns_net_cell_str_gsm::key_2g_ber].asInt();
    if (val < 0 || val > 7) val = 99;
    dst.CellInfo.gsm.signalStrengthGsm.bitErrorRate = val;

    // Signal Strength (0-31,99)
    val = jsonStrInfo[ns_net_cell_str_gsm::key_2g_sig_stren].asInt();
    if (val < 0 || val > 31) val = 99;
    dst.CellInfo.gsm.signalStrengthGsm.signalStrength = val;

    // Timing Advance (0~63)
    val = (int)jsonStrInfo[ns_net_cell_str_gsm::key_2g_ta].asUInt();
    if (val < 0 || val > 63) val = INT32_MAX;
    dst.CellInfo.gsm.signalStrengthGsm.timingAdvance = val;
}

void CellInfoListParserV4Json::FillCdmaCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    Json::Value jsonCommonInfo = jsonData[ns_net_cell_info_cdma::key_cell_info_common];
    FillCellCommon(dst, jsonCommonInfo);

    Json::Value jsonIdInfo = jsonData[ns_net_cell_info_cdma::key_cell_identity_cdma];
    FillCellIdentityCdma(dst, jsonIdInfo);

    Json::Value jsonStrInfo = jsonData[ns_net_cell_info_cdma::key_sig_str_cdma];
    int val = 0;
    // actual RSSI value (multiplied by -1)
    val = (int)jsonStrInfo[ns_net_cell_str_cdma::key_cdma_dbm].asUInt();
    if (val < 0) val *= -1;
    dst.CellInfo.cdma.signalStrengthCdma.dbm = val;

    // actual Ec/Io (multiplied by -10)
    val = (int)jsonStrInfo[ns_net_cell_str_cdma::key_cdma_ecio].asUInt();
    if (val < 0) val *= -10;
    dst.CellInfo.cdma.signalStrengthCdma.ecio = val;

    // actual RSSI value (multiplied by -1)
    val = (int)jsonStrInfo[ns_net_cell_str_cdma::key_cdma_dodbm].asUInt();
    if (val < 0) val *= -1;
    dst.CellInfo.cdma.signalStrengthEvdo.dbm = val;

    // actual Ec/Io (multiplied by -10)
    val = (int)jsonStrInfo[ns_net_cell_str_cdma::key_cdma_doecio].asUInt();
    if (val < 0) val *= -10;
    dst.CellInfo.cdma.signalStrengthEvdo.ecio = val;

    // signal noise ratio (0-8)
    val = jsonStrInfo[ns_net_cell_str_cdma::key_cdma_dosnr].asInt();
    if (val < 0 || val > 8) val = INT_MAX;
    dst.CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio = val;
}

void CellInfoListParserV4Json::FillLteCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    Json::Value jsonCommonInfo = jsonData[ns_net_cell_info_lte::key_cell_info_common];
    FillCellCommon(dst, jsonCommonInfo);

    Json::Value jsonIdInfo = jsonData[ns_net_cell_info_lte::key_cell_identity_lte];
    FillCellIdentityLte(dst, jsonIdInfo);

    // E-UTRA-NR Dual Connectivity available
    dst.CellInfo.lte.isEndcAvailable = (jsonData[ns_net_cell_info_lte::key_endc_available] == 1)? true: false;

    Json::Value jsonStrInfo = jsonData[ns_net_cell_info_lte::key_sig_str_lte];
    int val = 0;
    // Signal Strength (0-31,99)
    val = jsonStrInfo[ns_net_cell_str_lte::key_4g_sig_stren].asInt();
    if (val < 0 || val > 31)
        val = 99;
    dst.CellInfo.lte.signalStrengthLte.signalStrength = val;

    // RSRP (44-140)
    val = jsonStrInfo[ns_net_cell_str_lte::key_4g_rsrp].asInt();
    if (val < 44 || val > 140)
        val = INT32_MAX;
    dst.CellInfo.lte.signalStrengthLte.rsrp = val;

    // RSRQ (3-20)
    val = jsonStrInfo[ns_net_cell_str_lte::key_4g_rsrq].asInt();
    if (val < 3 || val > 20)
        val = INT32_MAX;
    dst.CellInfo.lte.signalStrengthLte.rsrq = val;

    // RSSNR (-200 - 300)
    val = jsonStrInfo[ns_net_cell_str_lte::key_4g_rssnr].asInt();
    if (val < -200 || val > 300)
        val = INT32_MAX;
    dst.CellInfo.lte.signalStrengthLte.rssnr = val;

    // CQI Table Index
    val = jsonStrInfo[ns_net_cell_str_lte::key_4g_cqi_table_ind].asInt();
    if (val < 1 || val > 6)
        val = INT32_MAX;
    dst.CellInfo.lte.signalStrengthLte.cqiTableIndex = val;

    // CQI (0-15)
    val = jsonStrInfo[ns_net_cell_str_lte::key_4g_cqi].asInt();
    if (val < 0 || val > 15)
        val = INT32_MAX;
    dst.CellInfo.lte.signalStrengthLte.cqi = val;

    // TAVD (0-0x7FFFFFFE)
    val = jsonStrInfo[ns_net_cell_str_lte::key_4g_ta].asInt();
    if (val < 0 || val > 0x7FFFFFFE)
        val = INT32_MAX;
    dst.CellInfo.lte.signalStrengthLte.timingAdvance = val;
}

void CellInfoListParserV4Json::FillWcdmaCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    Json::Value jsonCommonInfo = jsonData[ns_net_cell_info_wcdma::key_cell_info_common];
    FillCellCommon(dst, jsonCommonInfo);

    Json::Value jsonIdInfo = jsonData[ns_net_cell_info_wcdma::key_cell_identity_wcdma];
    FillCellIdentityWcdma(dst, jsonIdInfo);

    Json::Value jsonStrInfo = jsonData[ns_net_cell_info_wcdma::key_sig_str_wcdma];
    int val = 0;
    // BER (0-7,99)
    val = jsonStrInfo[ns_net_cell_str_wcdma::key_3g_ber].asInt();
    if (val < 0 || val > 7)
        val = 99;
    dst.CellInfo.wcdma.signalStrengthWcdma.bitErrorRate = val;

    // Signal Strength (0-31,99)
    val = jsonStrInfo[ns_net_cell_str_wcdma::key_3g_sig_stren].asInt();
    if (val < 0 || val > 31)
        val = 99;
    dst.CellInfo.wcdma.signalStrengthWcdma.signalStrength = val;

    // Received Signal Code Power (0-96,255)
    val = jsonStrInfo[ns_net_cell_str_wcdma::key_3g_rscp].asInt();
    val = (val < 0)? 0:val;
    if (val > 96)
        val= 255;
    dst.CellInfo.wcdma.signalStrengthWcdma.rscp = val;

    // Ec/No Value (0-49,255)
    val = jsonStrInfo[ns_net_cell_str_wcdma::key_3g_ecno].asInt();
    val = (val < 0)? 0:val;
    if (val > 49)
        val = 255;
    dst.CellInfo.wcdma.signalStrengthWcdma.ecno = val;
}

void CellInfoListParserV4Json::FillTdscdmaCellInfo(RIL_CellInfo_V1_6 &dst,Json::Value &jsonData) {
    Json::Value jsonCommonInfo = jsonData[ns_net_cell_info_tdscdma::key_cell_info_common];
    FillCellCommon(dst, jsonCommonInfo);

    Json::Value jsonIdInfo = jsonData[ns_net_cell_info_tdscdma::key_cell_identity_tdscdma];
    FillCellIdentityTdscdma(dst, jsonIdInfo);

    Json::Value jsonStrInfo = jsonData[ns_net_cell_info_tdscdma::key_sig_str_tdscdma];
    int val = 0;
    // Received Signal Strength Indication (RSSI) measured from TDSCDMA (0-31,99)
    val = jsonStrInfo[ns_net_cell_str_tdscdma::key_tdscdma_sig_stren].asInt();
    val = (val < 0)? 0: val;
    if (val > 31)
        val = 99;
    dst.CellInfo.tdscdma.signalStrengthTdscdma.signalStrength = val;

    // Bit error rate (0-7, 99)
    val = jsonStrInfo[ns_net_cell_str_tdscdma::key_tdscdma_ber].asInt();
    val = (val < 0)? 0:val;
    if (val > 7)
        val = 99;
    dst.CellInfo.tdscdma.signalStrengthTdscdma.bitErrorRate = val;

    // RSCP (0-96, 255)
    val = jsonStrInfo[ns_net_cell_str_tdscdma::key_tdscdma_rscp].asInt();
    if (val > 96)
        val = 255;
    dst.CellInfo.tdscdma.signalStrengthTdscdma.rscp = val;
}

void CellInfoListParserV4Json::FillNrCellInfo(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    Json::Value jsonCommonInfo = jsonData[ns_net_cell_info_nr::key_cell_info_common];
    FillCellCommon(dst, jsonCommonInfo);

    Json::Value jsonIdInfo = jsonData[ns_net_cell_info_nr::key_cell_identity_nr];
    FillCellIdentityNr(dst, jsonIdInfo);

    Json::Value jsonStrInfo = jsonData[ns_net_cell_info_nr::key_sig_str_nr];
    int val = 0;
    // SS reference signal received power (44-140)
    val = jsonStrInfo[ns_net_cell_str_nr::key_5g_ssrsrp].asInt();
    if (val < 44 || val > 140)
        val = INT32_MAX;
    dst.CellInfo.nr.signalStrengthNr.ssRsrp = val;

    // SS reference signal received quality (3-20)
    val = jsonStrInfo[ns_net_cell_str_nr::key_5g_ssrsrq].asInt();
    if (val < 3 || val > 20)
        val = INT32_MAX;
    dst.CellInfo.nr.signalStrengthNr.ssRsrq = val;

    // SS signal-to-noise and interference ratio (-23~40)
    val = jsonStrInfo[ns_net_cell_str_nr::key_5g_sssinr].asInt();
    if (val < -23 || val > 40)
        val = INT32_MAX;
    dst.CellInfo.nr.signalStrengthNr.ssSinr = val;

    // CSI reference signal received power (44-140)
    val = jsonStrInfo[ns_net_cell_str_nr::key_5g_csirsrp].asInt();
    if (val < 44 || val > 140)
        val = INT32_MAX;
    dst.CellInfo.nr.signalStrengthNr.csiRsrp = val;

    // CSI reference signal received quality (3-20)
    val = jsonStrInfo[ns_net_cell_str_nr::key_5g_csirsrq].asInt();
    if (val < 3 || val > 20)
        val = INT32_MAX;
    dst.CellInfo.nr.signalStrengthNr.csiRsrq = val;

    // CSI signal-to-noise and interference ratio (-23~40)
    val = jsonStrInfo[ns_net_cell_str_nr::key_5g_csisinr].asInt();
    if (val < -23 || val > 40)
        val = INT32_MAX;
    dst.CellInfo.nr.signalStrengthNr.csiSinr = val;

    // CQI Table Index (1~3)
    val = jsonStrInfo[ns_net_cell_str_nr::key_5g_cqi_table_ind].asInt();
    if (val < 1 || val > 3)
        val = INT32_MAX;
    dst.CellInfo.nr.signalStrengthNr.csiCqiTableIndex = val;

    // CQI
    val = jsonStrInfo[ns_net_cell_str_nr::key_5g_wbcqi].asInt();
    if (val != INT_MAX) {
        dst.CellInfo.nr.signalStrengthNr.csiCqiReportLen = 1;
        dst.CellInfo.nr.signalStrengthNr.csiCqiReport[0] = val;
    } else {
        unsigned int sbcqiLen = jsonStrInfo[ns_net_cell_str_nr::key_5g_sbcqi_list].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, sbcqiLen, MAX_NR_BAND_INDEX);
        dst.CellInfo.nr.signalStrengthNr.csiCqiReportLen = sbcqiLen;
        for (unsigned int i = 0; i < MAX_NR_BAND_INDEX; ++i) {
            dst.CellInfo.nr.signalStrengthNr.csiCqiReport[i] = (int)jsonStrInfo[ns_net_cell_str_nr::key_5g_sbcqi_list][i].asUInt();
        }
    }
}

void CellInfoListParserV4Json::FillCellCommon(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    dst.registered = (jsonData[ns_net_cell_info_common::key_reg_status].asInt() == 1 ? true : false);
    dst.timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
    dst.connectionStatus = (RIL_CellConnectionStatus)(jsonData[ns_net_cell_info_common::key_connect_state].asInt());
}

void CellInfoListParserV4Json::FillCellIdentityGsm(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    string plmn = jsonData[ns_net_cell_identity_gsm::key_2g_plmn].asString();
    if (plmn.length() > 0) {
        dst.CellInfo.gsm.cellIdentityGsm.mcc = CellInfoProtocolUtils::JSON::FetchMcc(plmn.c_str());
        dst.CellInfo.gsm.cellIdentityGsm.mnc = CellInfoProtocolUtils::JSON::FetchMnc(plmn.c_str());
    }

    int val = 0;
    // 16-bit LAC
    val = jsonData[ns_net_cell_identity_gsm::key_2g_lac].asInt();
    if (val < 0 || val > 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.gsm.cellIdentityGsm.lac = val;

    // 16-bit GSM CID
    val = jsonData[ns_net_cell_identity_gsm::key_2g_cid].asInt();
    if (val < 0 || val > 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.gsm.cellIdentityGsm.cid = val;

    // 16-bit GSM ARFCN
    val = (int) jsonData[ns_net_cell_identity_gsm::key_2g_arfcn].asUInt();
    if (val < 0 || val > 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.gsm.cellIdentityGsm.arfcn = val;

    // 6-bit GSM BSIC
    val = jsonData[ns_net_cell_identity_gsm::key_2g_bsic].asInt();
    if (val > 0x3F)
        val = 0xFF;
    dst.CellInfo.gsm.cellIdentityGsm.bsic = val;

    // Operator name
    Json::Value jsonPlmnInfos = jsonData[ns_net_cell_identity_gsm::key_2g_cell_info_plmn];
    string plmnAlphaShort = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_short_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaShort, MAX_SHORT_NAME_LEN-1);
    string plmnAlphaLong = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_long_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaLong, MAX_FULL_NAME_LEN-1);
    FillAlphaName(dst.CellInfo.gsm.cellIdentityGsm.operatorNames, plmnAlphaShort, plmnAlphaLong);

    // Additional PLMN
    dst.CellInfo.gsm.cellIdentityGsm.leng_additionalPlmns = jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list].size();
    dst.CellInfo.gsm.cellIdentityGsm.additionalPlmns = FillGetAdditionalPlmns(
                                jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list]);

}

void CellInfoListParserV4Json::FillCellIdentityCdma(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    int val = 0;

    // 16-bit Network Id
    val = jsonData[ns_net_cell_identity_cdma::key_cdma_nid].asInt();
    if (val < 0 || val > 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.cdma.cellIdentityCdma.networkId = val;

    // 15-bit CDMA System Id
    val = jsonData[ns_net_cell_identity_cdma::key_cdma_sid].asInt();

    if (val < 0 || val > 0x7FFF)
        val = INT32_MAX;
    dst.CellInfo.cdma.cellIdentityCdma.systemId = val;

    // 16-bit Base Station Id
    val = jsonData[ns_net_cell_identity_cdma::key_cdma_bsid].asInt();
    if (val < 0 || val > 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.cdma.cellIdentityCdma.basestationId = val;

    // Longitude (-2592000-2592000)
    val = (int)jsonData[ns_net_cell_identity_cdma::key_cdma_longitude].asUInt();
    if (val < -2592000 || val > 2592000)
        val = INT32_MAX;
    dst.CellInfo.cdma.cellIdentityCdma.longitude = val;

    // Latitude (-1296000-1296000)
    val = (int)jsonData[ns_net_cell_identity_cdma::key_cdma_latitude].asUInt();
    if (val < -1296000 || val > 1296000)
        val = INT32_MAX;
    dst.CellInfo.cdma.cellIdentityCdma.latitude = val;
}

void CellInfoListParserV4Json::FillCellIdentityLte(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    string plmn = jsonData[ns_net_cell_identity_lte::key_4g_plmn].asString();
    if (plmn.length() > 0) {
        dst.CellInfo.lte.cellIdentityLte.mcc = CellInfoProtocolUtils::JSON::FetchMcc(plmn.c_str());
        dst.CellInfo.lte.cellIdentityLte.mnc = CellInfoProtocolUtils::JSON::FetchMnc(plmn.c_str());
    }

    int val = 0;

    // 28-bit CID
    val = (int)jsonData[ns_net_cell_identity_lte::key_4g_cid].asUInt();
    if (val < 0 || val > 0xFFFFFFF)
        val = INT32_MAX;
    dst.CellInfo.lte.cellIdentityLte.ci = val;

    // Physical CID
    val = (int)jsonData[ns_net_cell_identity_lte::key_4g_pcid].asUInt();
    if (val < 0 || val > 503)
        val = INT32_MAX;
    dst.CellInfo.lte.cellIdentityLte.pci = val;

    // 16-bit TAC
    val = (int)jsonData[ns_net_cell_identity_lte::key_4g_tac].asUInt();
    if (val < 0 || val> 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.lte.cellIdentityLte.tac = val;

    // 18-bit earfcn
    val = (int)jsonData[ns_net_cell_identity_lte::key_4g_earfcn].asUInt();
    if (val < 0 || val > 0x3FFFF)
        val = INT32_MAX;
    dst.CellInfo.lte.cellIdentityLte.earfcn = val;

    // bandwidth
    val = jsonData[ns_net_cell_identity_lte::key_4g_bandwidth].asInt();
    if (val < 0)
        val = INT32_MAX;
    dst.CellInfo.lte.cellIdentityLte.bandwidth = val;

    // Operator name
    Json::Value jsonPlmnInfos = jsonData[ns_net_cell_identity_lte::key_4g_cell_info_plmn];
    string plmnAlphaShort = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_short_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaShort, MAX_SHORT_NAME_LEN-1);
    string plmnAlphaLong = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_long_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaLong, MAX_FULL_NAME_LEN-1);
    FillAlphaName(dst.CellInfo.lte.cellIdentityLte.operatorNames, plmnAlphaShort, plmnAlphaLong);

    // Additional PLMN
    dst.CellInfo.lte.cellIdentityLte.leng_additionalPlmns = jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list].size();
    dst.CellInfo.lte.cellIdentityLte.additionalPlmns = FillGetAdditionalPlmns(
                                jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list]);

    // CSG Info;
    Json::Value jsonCsgInfo = jsonData[ns_net_cell_identity_lte::key_4g_cell_info_csg];
    FillCellIdentityCsgInfo(dst.CellInfo.lte.cellIdentityLte.optionalCsgInfo, jsonCsgInfo);

    // Band Length & Band Info
    val = jsonData[ns_net_cell_identity_lte::key_4g_eutran_band_list].size();
    ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, val, MAX_BANDS);
    dst.CellInfo.lte.cellIdentityLte.leng_bands = val;
    for (int i = 0; i < val; i++) {
        dst.CellInfo.lte.cellIdentityLte.bands[i] = (RIL_EutranBands)jsonData[ns_net_cell_identity_lte::key_4g_eutran_band_list][i].asInt();
    }
}

void CellInfoListParserV4Json::FillCellIdentityWcdma(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    string plmn = jsonData[ns_net_cell_identity_wcdma::key_3g_plmn].asString();
    if (plmn.length() > 0) {
        dst.CellInfo.wcdma.cellIdentityWcdma.mcc = CellInfoProtocolUtils::JSON::FetchMcc(plmn.c_str());
        dst.CellInfo.wcdma.cellIdentityWcdma.mnc = CellInfoProtocolUtils::JSON::FetchMnc(plmn.c_str());
    }

    int val = 0;

    // 16-bit LAC
    val = (int)jsonData[ns_net_cell_identity_wcdma::key_3g_lac].asUInt();
    if (val < 0 || val > 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.wcdma.cellIdentityWcdma.lac = val;

    // 28-bit UMTS CID
    val = (int)jsonData[ns_net_cell_identity_wcdma::key_3g_cid].asUInt();
    if (val < 0 || val > 0xFFFFFFF)
        val= INT32_MAX;
    dst.CellInfo.wcdma.cellIdentityWcdma.cid = val;

    // 9-bit UMTS PSC
    val = (int)jsonData[ns_net_cell_identity_wcdma::key_3g_psc].asUInt();
    if (val < 0 || val > 0x1FF)
        val = INT32_MAX;
    dst.CellInfo.wcdma.cellIdentityWcdma.psc = val;

    // 16-bit UMTS uarfcn
    val = (int) jsonData[ns_net_cell_identity_wcdma::key_3g_uarfcn].asUInt();
    if (val < 0 || val > 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.wcdma.cellIdentityWcdma.uarfcn = val;

    // Operator name
    Json::Value jsonPlmnInfos = jsonData[ns_net_cell_identity_wcdma::key_3g_cell_info_plmn];
    string plmnAlphaShort = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_short_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaShort, MAX_SHORT_NAME_LEN-1);
    string plmnAlphaLong = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_long_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaLong, MAX_FULL_NAME_LEN-1);
    FillAlphaName(dst.CellInfo.wcdma.cellIdentityWcdma.operatorNames, plmnAlphaShort, plmnAlphaLong);

    // Additional PLMN
    dst.CellInfo.wcdma.cellIdentityWcdma.leng_additionalPlmns = jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list].size();
    dst.CellInfo.wcdma.cellIdentityWcdma.additionalPlmns = FillGetAdditionalPlmns(
                                jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list]);

    // CSG Info;
    Json::Value jsonCsgInfo = jsonData[ns_net_cell_identity_wcdma::key_3g_cell_info_csg];
    FillCellIdentityCsgInfo(dst.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo, jsonCsgInfo);
}

void CellInfoListParserV4Json::FillCellIdentityTdscdma(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    string plmn = jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_plmn].asString();
    if (plmn.length() > 0) {
        dst.CellInfo.tdscdma.cellIdentityTdscdma.mcc = CellInfoProtocolUtils::JSON::FetchMcc(plmn.c_str());
        dst.CellInfo.tdscdma.cellIdentityTdscdma.mnc = CellInfoProtocolUtils::JSON::FetchMnc(plmn.c_str());
    }

    int val = 0;

    // 16-bit LAC
    val = (int)jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_lac].asUInt();
    if (val < 0 || val > 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.tdscdma.cellIdentityTdscdma.lac = val;

    // 28-bit UMTS CID
    val = (int)jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_cid].asUInt();
    if (val < 0 || val > 0xFFFFFFF)
        val = INT32_MAX;
    dst.CellInfo.tdscdma.cellIdentityTdscdma.cid = val;

    // 8-bit CPID
    val = (int)jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_cpid].asUInt();
    if (val < 0 || val > 0xFF)
        val = INT32_MAX;
    dst.CellInfo.tdscdma.cellIdentityTdscdma.cpid = val;

    // 16-bit UMTS Absolute RF Channel Number
    val = (int) jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_uarfcn].asUInt();
    if (val < 0 || val > 0xFFFF)
        val = INT32_MAX;
    dst.CellInfo.tdscdma.cellIdentityTdscdma.uarfcn = val;

    // Operator name
    Json::Value jsonPlmnInfos = jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_cell_info_plmn];
    string plmnAlphaShort = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_short_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaShort, MAX_SHORT_NAME_LEN-1);
    string plmnAlphaLong = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_long_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaLong, MAX_FULL_NAME_LEN-1);
    FillAlphaName(dst.CellInfo.tdscdma.cellIdentityTdscdma.operatorNames, plmnAlphaShort, plmnAlphaLong);

    // Additional PLMN
    dst.CellInfo.tdscdma.cellIdentityTdscdma.leng_additionalPlmns = jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list].size();
    dst.CellInfo.tdscdma.cellIdentityTdscdma.additionalPlmns = FillGetAdditionalPlmns(
                                jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list]);

    // CSG Info;
    Json::Value jsonCsgInfo = jsonData[ns_net_cell_identity_tdscdma::key_tdscdma_cell_info_csg];
    FillCellIdentityCsgInfo(dst.CellInfo.tdscdma.cellIdentityTdscdma.optionalCsgInfo, jsonCsgInfo);
}

void CellInfoListParserV4Json::FillCellIdentityNr(RIL_CellInfo_V1_6 &dst, Json::Value &jsonData) {
    string plmn = jsonData[ns_net_cell_identity_nr::key_5g_plmn].asString();
    if (plmn.length() > 0) {
        dst.CellInfo.nr.cellIdentityNr.mcc = CellInfoProtocolUtils::JSON::FetchMcc(plmn.c_str());
        dst.CellInfo.nr.cellIdentityNr.mnc = CellInfoProtocolUtils::JSON::FetchMnc(plmn.c_str());
    }

    int val = 0;
    ULONG lval = 0;

    // Cell Identity (36 bits)
    lval = jsonData[ns_net_cell_identity_nr::key_5g_nci].asUInt64();
    if (lval > 68719476735)
        lval = LONG_MAX;
    dst.CellInfo.nr.cellIdentityNr.nci = lval;

    // Physical cell id (0-1007)
    val = jsonData[ns_net_cell_identity_nr::key_5g_pcid].asInt();
    if (val > 1007)
        val = INT32_MAX;
    dst.CellInfo.nr.cellIdentityNr.pci = val;

    // 16-bit tracking area code
    val = (int)jsonData[ns_net_cell_identity_nr::key_5g_tac].asUInt();
    if (val < 0 || val > 16777215)
        val = INT32_MAX;
    dst.CellInfo.nr.cellIdentityNr.tac = val;

    // NR Absolute Radio Frequency Channel Number (0-3279165)
    val = (int)jsonData[ns_net_cell_identity_nr::key_5g_nrarfcn].asUInt();
    if (val < 0 || val > 3279165)
        val = INT32_MAX;
    dst.CellInfo.nr.cellIdentityNr.nrarfcn = val;

    // Operator name
    Json::Value jsonPlmnInfos = jsonData[ns_net_cell_identity_nr::key_5g_cell_info_plmn];
    string plmnAlphaShort = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_short_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaShort, MAX_SHORT_NAME_LEN-1);
    string plmnAlphaLong = jsonPlmnInfos.get(ns_net_cell_info_plmn::key_plmn_long_name, "").asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmnAlphaLong, MAX_FULL_NAME_LEN-1);
    FillAlphaName(dst.CellInfo.nr.cellIdentityNr.operatorNames, plmnAlphaShort, plmnAlphaLong);

    // Additional PLMN
    dst.CellInfo.nr.cellIdentityNr.leng_additionalPlmns = jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list].size();
    dst.CellInfo.nr.cellIdentityNr.additionalPlmns = FillGetAdditionalPlmns(
                                jsonPlmnInfos[ns_net_cell_info_plmn::key_plmn_info_list]);

    // Band Length
    // Band Length & Band Info
    val = jsonData[ns_net_cell_identity_nr::key_5g_nr_band_list].size();
    ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, val, MAX_BANDS);
    dst.CellInfo.nr.cellIdentityNr.leng_bands = val;
    for (int i = 0; i < val; i++) {
        dst.CellInfo.nr.cellIdentityNr.bands[i] = (RIL_NgranBands)jsonData[ns_net_cell_identity_nr::key_5g_nr_band_list][i].asInt();
    }
}

void CellInfoListParserV4Json::FillAlphaName(CellIdentityOperatorNames &dst, string alphaShort, string alphaLong) {
    if (alphaShort.length() > 0)
        strncpy(dst.alphaShort, alphaShort.c_str(), alphaShort.length());
    if (alphaLong.length() != 0)
        strncpy(dst.alphaLong, alphaLong.c_str(), alphaLong.length());
}

char **CellInfoListParserV4Json::FillGetAdditionalPlmns(Json::Value &jsonData) {
    int listLen = jsonData.size();
    if (listLen > 0) {
        char **additionalPlmms = (char **)calloc(listLen, sizeof(char*));
        for (int i = 0; i < listLen; ++i) {
            string srcPlmn = jsonData[i].asString();
            char *dstPlmn = (char *)calloc((srcPlmn.length() + 1), sizeof(char)); // including null char.
            memcpy(dstPlmn, srcPlmn.c_str(), srcPlmn.length() + 1);
            *(additionalPlmms + i) = dstPlmn;
        }
        return additionalPlmms;
    }
    return NULL;
}

void CellInfoListParserV4Json::FillCellIdentityCsgInfo(RIL_OptionalCsgInfo &dst, Json::Value &jsonData) {
    bool csg_indication = (jsonData[ns_net_cell_info_csg::key_csg_ind].asInt() > 0) ? true : false;
    int csg_identity = jsonData[ns_net_cell_info_csg::key_csg_identiy].asInt();

    if (!csg_indication && !(csg_identity > 0 && csg_identity <= 0x7FFFFFF)) {
        // noninit
        dst = { { false, {}, INT_MAX} };
    } else {
        dst.csgInfo.csgIndication = csg_indication;
        string nb_name = jsonData[ns_net_cell_info_csg::key_home_nodeb_name].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, nb_name, MAX_CSG_HOME_NODEB_NAME - 1);
        if (nb_name.length() > 0) {
            strncpy(dst.csgInfo.homeNodebName, nb_name.c_str(), nb_name.length());
        }

        // Modem must guarantee a valid range of cid.
        // IntRage(from = 1, to = 0x7FFFFFF)
        dst.csgInfo.csgIdentity = csg_identity;
    }
}

/**
 * ProtocolNetVoiceRegStateAdapterJson
 */
ProtocolNetVoiceRegStateAdapterJson::ProtocolNetVoiceRegStateAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_CS_REG_STATE) {
    Init();
}

void ProtocolNetVoiceRegStateAdapterJson::Init() {
    m_rat = (int)RADIO_TECH_UNKNOWN;
    if (m_bParsingSuccess) m_rat = NetProtocolUtils::ConvertSitRat2RilRat(m_jsonData[ns_net_get_cs_reg_rsp::key_rat].asInt());
}

int ProtocolNetVoiceRegStateAdapterJson::GetRegState() const {
    int regStatus = UNKNOWN;
    if (m_bParsingSuccess) regStatus = NetProtocolUtils::ConvertSitRegState2Ril(m_jsonData[ns_net_get_cs_reg_rsp::key_reg_status].asInt());
    return regStatus;
}

int ProtocolNetVoiceRegStateAdapterJson::GetRejectCause() const {
    int rejCause  = SIT_NET_REJ_CAUSE_GENERAL;;
    if (m_bParsingSuccess) rejCause = m_jsonData[ns_net_get_cs_reg_rsp::key_reg_cause].asInt();
    return rejCause;
}

int ProtocolNetVoiceRegStateAdapterJson::GetRadioTech() const {
    return m_rat;
}

int ProtocolNetVoiceRegStateAdapterJson::GetLAC() const {
    int lac = 0xFFFF;
    if (m_bParsingSuccess && Is2g3gRegInfo(m_rat)) {
        Json::Value jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_2g_3g_cs_reg_info];
        lac = (int)jsonRatRegInfo[ns_net_2g_3g_reg_info::key_2g_3g_lac].asUInt();
    }
    return lac;
}

int ProtocolNetVoiceRegStateAdapterJson::GetCellId() const {
    int cid = 0xFFFFFFFF;
    if (m_bParsingSuccess && Is2g3gRegInfo(m_rat)) {
        Json::Value jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_2g_3g_cs_reg_info];
        cid = (int)jsonRatRegInfo[ns_net_2g_3g_reg_info::key_2g_3g_cid].asUInt();
    }
    return cid;
}

int ProtocolNetVoiceRegStateAdapterJson::GetPSC() const {
    int psc = 0xFF;
    if (m_bParsingSuccess && Is2g3gRegInfo(m_rat)) {
        Json::Value jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_2g_3g_cs_reg_info];
        psc = jsonRatRegInfo[ns_net_2g_3g_reg_info::key_2g_3g_psc].asInt();
    }
    return psc;
}

int ProtocolNetVoiceRegStateAdapterJson::GetTAC() const {
    int tac = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_lte_cs_reg_info];
            tac = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_tac].asUInt();
        } else if (IsNrRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_nr_cs_reg_info];
            tac = (int)jsonRatRegInfo[ns_net_5g_reg_info::key_5g_tac].asUInt();
        }
    }
    return tac;
}

int ProtocolNetVoiceRegStateAdapterJson::GetPCID() const {
    int pcid  = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_lte_cs_reg_info];
            pcid  = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_pcid].asUInt();
        } else if (IsNrRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_nr_cs_reg_info];
            pcid = jsonRatRegInfo[ns_net_5g_reg_info::key_5g_pcid].asInt();
        }
    }
    return pcid;
}

int ProtocolNetVoiceRegStateAdapterJson::GetECI() const {
    int eci = 0xFFFFFFFF;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_lte_cs_reg_info];
            eci = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_eci].asUInt();
        }
    }
    return eci;
}

int ProtocolNetVoiceRegStateAdapterJson::GetStationId() const {
    int stationId = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsCdmaRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info];
            stationId = (int)jsonRatRegInfo[ns_net_cdma_reg_info::key_cdma_base_sta_id].asUInt();
        }
    }
    return stationId;
}

int ProtocolNetVoiceRegStateAdapterJson::GetStationLat() const {
    int stationLat = 0;
    if (m_bParsingSuccess) {
        if (IsCdmaRegInfo(m_rat)) {
            Json::Value jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info];
            stationLat = jsonRatRegInfo[ns_net_cdma_reg_info::key_cdma_base_sta_lt].asInt();
        }
    }
    return stationLat;
}

int ProtocolNetVoiceRegStateAdapterJson::GetStationLong() const {
    int stationLong = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsCdmaRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info];
            stationLong = jsonRatRegInfo[ns_net_cdma_reg_info::key_cdma_base_sta_lng].asInt();
        }
    }
    return stationLong;
}
int ProtocolNetVoiceRegStateAdapterJson::GetConCurrent() const {
    int conCur = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsCdmaRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info];
            conCur = jsonRatRegInfo[ns_net_cdma_reg_info::key_cdma_concurrent].asInt();
        }
    }
    return conCur;
}

int ProtocolNetVoiceRegStateAdapterJson::GetSystemId() const {
    int systemId = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsCdmaRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info];
            systemId = jsonRatRegInfo[ns_net_cdma_reg_info::key_cdma_sid].asInt();
        }
    }
    return systemId;
}

int ProtocolNetVoiceRegStateAdapterJson::GetNetworkId() const {
    int networkId = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsCdmaRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info];
            networkId = jsonRatRegInfo[ns_net_cdma_reg_info::key_cdma_nid].asInt();
        }
    }
    return networkId;
}

int ProtocolNetVoiceRegStateAdapterJson::GetRoamingInd() const {
    int roamingInd = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsCdmaRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info];
            roamingInd = jsonRatRegInfo[ns_net_cdma_reg_info::key_cdma_roaming_id].asInt();
        }
    }
    return roamingInd;
}

int ProtocolNetVoiceRegStateAdapterJson::GetRegPrl() const {
    int regPrl = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsCdmaRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info];
            regPrl = jsonRatRegInfo[ns_net_cdma_reg_info::key_cdma_registered_prl].asInt();
        }
    }
    return regPrl;
}

int ProtocolNetVoiceRegStateAdapterJson::GetRoamingIndPrl() const {
    int roamingPrl = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsCdmaRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_cdma_cs_reg_info];
            roamingPrl = jsonRatRegInfo[ns_net_cdma_reg_info::key_cdma_roaming_ind_prl].asInt();
        }
    }
    return roamingPrl;
}

int ProtocolNetVoiceRegStateAdapterJson::getChannelNumber() const {
    int channelNumber = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (Is2g3gRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_2g_3g_cs_reg_info];
            channelNumber = (int)jsonRatRegInfo[ns_net_2g_3g_reg_info::key_2g_3g_channel].asUInt();
        } else if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_lte_cs_reg_info];
            channelNumber = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_channel].asUInt();
        } else if (IsNrRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_nr_cs_reg_info];
            channelNumber = (int)jsonRatRegInfo[ns_net_5g_reg_info::key_5g_channel].asUInt();
        }
    }
    return channelNumber;
}

long int ProtocolNetVoiceRegStateAdapterJson::getNrCid() const {
    long int cid = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsNrRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_cs_reg_rsp::key_nr_cs_reg_info];
            cid = (long int)jsonRatRegInfo[ns_net_5g_reg_info::key_5g_cid].asUInt64();
        }
    }
    return cid;
}

/**
 * ProtocolNetDataRegStateAdapterJson
 */
ProtocolNetDataRegStateAdapterJson::ProtocolNetDataRegStateAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_PS_REG_STATE) {
    Init();
}

void ProtocolNetDataRegStateAdapterJson::Init() {
    m_rat = (int)RADIO_TECH_UNKNOWN;
    if (m_bParsingSuccess) m_rat = NetProtocolUtils::ConvertSitRat2RilRat(m_jsonData[ns_net_get_ps_reg_rsp::key_rat].asInt());
}

int ProtocolNetDataRegStateAdapterJson::GetRegState() const {
    int regStatus = UNKNOWN;
    if (m_bParsingSuccess) regStatus = NetProtocolUtils::ConvertSitRegState2Ril(m_jsonData[ns_net_get_ps_reg_rsp::key_reg_status].asInt());
    return regStatus;
}

int ProtocolNetDataRegStateAdapterJson::GetRejectCause() const {
    int rejCause  = SIT_NET_REJ_CAUSE_GENERAL;;
    if (m_bParsingSuccess) rejCause = m_jsonData[ns_net_get_ps_reg_rsp::key_rej_cause].asInt();
    return rejCause;
}

int ProtocolNetDataRegStateAdapterJson::GetMaxSDC() const {
    int sdc = 4;
    // not used.
    return sdc;
}

int ProtocolNetDataRegStateAdapterJson::GetRadioTech() const {
    return m_rat;
}

int ProtocolNetDataRegStateAdapterJson::GetLAC() const {
    int lac = 0xFFFF;
    if (m_bParsingSuccess && Is2g3gRegInfo(m_rat)) {
        Json::Value jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_2g_3g_ps_reg_info];
        lac = (int)jsonRatRegInfo[ns_net_2g_3g_reg_info::key_2g_3g_lac].asUInt();
    }
    return lac;
}

int ProtocolNetDataRegStateAdapterJson::GetCellId() const {
    int cid = 0;
    if (m_bParsingSuccess && Is2g3gRegInfo(m_rat)) {
        Json::Value jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_2g_3g_ps_reg_info];
        cid = (int)jsonRatRegInfo[ns_net_2g_3g_reg_info::key_2g_3g_cid].asUInt();
    }
    return cid;
}

int ProtocolNetDataRegStateAdapterJson::GetPSC() const {
    int psc = 0xFF;
    if (m_bParsingSuccess && Is2g3gRegInfo(m_rat)) {
        Json::Value jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_2g_3g_ps_reg_info];
        psc = jsonRatRegInfo[ns_net_2g_3g_reg_info::key_2g_3g_psc].asInt();
    }
    return psc;
}

int ProtocolNetDataRegStateAdapterJson::GetTAC() const {
    int tac = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info];
            tac = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_tac].asUInt();
        } else if (IsNrRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_nr_ps_reg_info];
            tac = (int)jsonRatRegInfo[ns_net_5g_reg_info::key_5g_tac].asUInt();
        }
    }
    return tac;
}

int ProtocolNetDataRegStateAdapterJson::GetPCID() const {
    int pcid  = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info];
            pcid  = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_pcid].asUInt();
        } else if (IsNrRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_nr_ps_reg_info];
            pcid = jsonRatRegInfo[ns_net_5g_reg_info::key_5g_pcid].asInt();
        }
    }
    return pcid;
}

int ProtocolNetDataRegStateAdapterJson::GetECI() const {
    int eci = 0xFFFFFFFF;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info];
            eci = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_eci].asUInt();
        }
    }
    return eci;
}

int ProtocolNetDataRegStateAdapterJson::GetCSGID() const {
    int csgid = 0xFFFFFFFF;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info];
            csgid = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_csgid].asUInt();
        }
    }
    return csgid;
}

int ProtocolNetDataRegStateAdapterJson::GetTADV() const {
    int tadv = 0xFFFFFFFF;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info];
            tadv = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_tadv].asUInt();
        }
    }
    return tadv;
}

int ProtocolNetDataRegStateAdapterJson::GetImsVops() const {
    int imsVoPs = VOPS_NOT_SUPPORTED;
    if (m_bParsingSuccess) {
        imsVoPs = m_jsonData[ns_net_get_ps_reg_rsp::key_volte_svc].asInt() & 0xFF;
    }
    return imsVoPs;
}

int ProtocolNetDataRegStateAdapterJson::GetEmcService() const {
    int emcSvc = EMC_NOT_SUPPORTED;
    if (m_bParsingSuccess) {
        emcSvc = m_jsonData[ns_net_get_ps_reg_rsp::key_emc_svc].asInt() & 0xFF;
    }
    return emcSvc;
}

int ProtocolNetDataRegStateAdapterJson::getChannelNumber() const {
    int channelNumber = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (Is2g3gRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_2g_3g_ps_reg_info];
            channelNumber = (int)jsonRatRegInfo[ns_net_2g_3g_reg_info::key_2g_3g_channel].asUInt();
        } else if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info];
            channelNumber = (int)jsonRatRegInfo[ns_net_4g_reg_info::key_4g_channel].asUInt();
        } else if (IsNrRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_nr_ps_reg_info];
            channelNumber = (int)jsonRatRegInfo[ns_net_5g_reg_info::key_5g_channel].asUInt();
        }
    }
    return channelNumber;
}

bool ProtocolNetDataRegStateAdapterJson::IsEndcAvailable() const {
    bool ret = false;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info];
            ret = jsonRatRegInfo[ns_net_4g_reg_info::key_4g_endc].asInt() == SERVICE_AVAILABLE;
        }
    }
    return ret;
}

bool ProtocolNetDataRegStateAdapterJson::IsDcNrRestricted() const {
    bool ret = false;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info];
            ret = jsonRatRegInfo[ns_net_4g_reg_info::key_4g_dcnr_restricted].asInt() == SERVICE_AVAILABLE;
        }
    }
    return ret;
}
bool ProtocolNetDataRegStateAdapterJson::IsNrAvailable() const {
    bool ret = false;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsLteRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_lte_ps_reg_info];
            ret = jsonRatRegInfo[ns_net_4g_reg_info::key_4g_nr_available].asInt() == SERVICE_AVAILABLE;
        }
    }
    return ret;
}

long int ProtocolNetDataRegStateAdapterJson::getNrCid() const {
    long int cid = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsNrRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_nr_ps_reg_info];
            cid = (long int)jsonRatRegInfo[ns_net_5g_reg_info::key_5g_cid].asUInt64();
        }
    }
    return cid;
}

int ProtocolNetDataRegStateAdapterJson::GetEmf() const {
    int emf = 0;
    if (m_bParsingSuccess) {
        Json::Value jsonRatRegInfo;
        if (IsNrRegInfo(m_rat)) {
            jsonRatRegInfo = m_jsonData[ns_net_get_ps_reg_rsp::key_nr_ps_reg_info];
            emf = jsonRatRegInfo[ns_net_5g_reg_info::key_5g_emf].asInt();
        }
    }
    return emf;
}

/**
 * ProtocolNetOperatorAdapterJson
 */
ProtocolNetOperatorAdapterJson::ProtocolNetOperatorAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_OPERATOR) {
    Init();
}

void ProtocolNetOperatorAdapterJson::Init() {
    memset(m_szPlmn, 0, sizeof(m_szPlmn));
    memset(m_szShortPlmn, 0, sizeof(m_szShortPlmn));
    memset(m_szLongPlmn, 0, sizeof(m_szLongPlmn));
    m_regState = OPERATOR_REG_UNKNOWN;
    m_lac = -1;

    if (m_bParsingSuccess && ProtocolJsonRespAdapter::GetErrorCode() == RCM_E_SUCCESS) {
        string strData;
        // plmm
        strData = m_jsonData[ns_net_get_operator_rsp::key_plmn].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, strData, MAX_PLMN_LEN);
        if (strData.size() > 0) memcpy(m_szPlmn, strData.c_str(), strData.length());
        // short name
        strData = m_jsonData.get(ns_net_get_operator_rsp::key_short_name, "").asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, strData, MAX_SHORT_NAME_LEN);
        if (strData.size() > 0) memcpy(m_szShortPlmn, strData.c_str(), strData.length());
        // long name
        strData = m_jsonData.get(ns_net_get_operator_rsp::key_long_name, "").asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, strData, MAX_FULL_NAME_LEN);
        if (strData.size() > 0) memcpy(m_szLongPlmn, strData.c_str(), strData.length());

        m_regState = m_jsonData[ns_net_get_operator_rsp::key_reg_status].asInt();
        m_lac = m_jsonData[ns_net_get_operator_rsp::key_location].asUInt();
    }
}

const char *ProtocolNetOperatorAdapterJson::GetPlmn() const {
    return (m_szPlmn[0] == 0 ? NULL : m_szPlmn);
}

const char *ProtocolNetOperatorAdapterJson::GetShortPlmn() const {
    return (m_szShortPlmn[0] == 0 ? NULL : m_szShortPlmn);
}

const char *ProtocolNetOperatorAdapterJson::GetLongPlmn() const {
    return (m_szLongPlmn[0] == 0 ? NULL : m_szLongPlmn);
}

int ProtocolNetOperatorAdapterJson::GetRegState() const {
    return m_regState;
}

int ProtocolNetOperatorAdapterJson::GetLac() const {
    return m_lac;
}

/**
 * ProtocolNetSelModeAdapterJson
 */
int ProtocolNetSelModeAdapterJson::GetNetworkSelectionMode() const {
    if(m_bParsingSuccess) {
        int mode = m_jsonData[ns_net_get_ntw_mode_rsp::key_ntw_mode].asInt() & 0xFF;
        RilLogV("NetworkSelectionMode=%s(0x%02x)", mode == 0 ? "Automatic" : "Manual", mode);
        return mode;
    }
    return SIT_NET_NETWORK_MODE_AUTOMATIC;
}

/**
 * ProtocolRadioPowerAdapterJson
 */

/**
 * ProtocolGetRadioStateRespAdapterJson
 */
int ProtocolGetRadioStateRespAdapterJson::GetRadioState() const {
    if(m_bParsingSuccess) {
        int radioState = m_jsonData[ns_pwr_get_radio_power_rsp::key_radio_state].asInt();
        RilLogV("RadioState=%s(0x%02x)", NetProtocolUtils::ConvertPwrRadioSimState2String(radioState), radioState);
        return ConvertRilRadioState(radioState);
    } else {
        return RADIO_STATE_UNAVAILABLE;
    }
}

int ProtocolGetRadioStateRespAdapterJson::ConvertRilRadioState(int radioState) const {
    if (radioState == SIT_PWR_RADIO_SIM_STATE_OFF) {
        return RADIO_STATE_OFF;
    }
    else if (radioState >= SIT_PWR_RADIO_SIM_STATE_SIM_NOT_READY && radioState <= SIT_PWR_RADIO_SIM_STATE_ON) {
        return RADIO_STATE_ON;
    }
    return RADIO_STATE_UNAVAILABLE;
}

/**
 * ProtocolRadioStateAdapterJson
 */
int ProtocolRadioStateAdapterJson::GetRadioState() const
{
    if(m_bParsingSuccess) {
        int radioState = m_jsonData[ns_pwr_radio_state_changed_ind::key_radio_state].asInt();
        RilLogV("RadioState=%s(0x%02x)", NetProtocolUtils::ConvertRadioState2String(radioState), radioState);
        return ConvertRilRadioState(radioState);
    } else {
        return RADIO_STATE_UNAVAILABLE;
    }

}

int ProtocolRadioStateAdapterJson::ConvertRilRadioState(int radioState) const {
    switch (radioState) {
        case SIT_PWR_RADIO_STATE_INITIALIZED:
        case SIT_PWR_RADIO_STATE_STOP_NETWORK:
            return RADIO_STATE_OFF;
        case SIT_PWR_RADIO_STATE_START_NETWORK:
            return RADIO_STATE_ON;
        case SIT_PWR_RADIO_STATE_POWER_OFF:
            return RADIO_STATE_UNAVAILABLE;
        default:
            return RADIO_STATE_UNAVAILABLE;
    }
}

/**
 * ProtocolNetPrefNetTypeAdapterJson
 */
int ProtocolNetPrefNetTypeAdapterJson::GetPreferredNetworkType() const {
    if (m_bParsingSuccess) {
        int sitPrefNetType = m_jsonData[ns_net_getset_preferred_ntw_type::key_pref_net_type].asInt();
        RilLogV("PreferredNetworkType=%s(0x%02x)", NetProtocolUtils::ConvertSitPreferredNetType2String(sitPrefNetType), sitPrefNetType);
        return NetProtocolUtils::ConvertPrefNetworkType2Ril(sitPrefNetType);
    }
    return PREF_NET_TYPE_GSM_WCDMA;
}

/**
 * ProtocolNetBandModeAdapterJson
 */
ProtocolNetBandModeAdapterJson::ProtocolNetBandModeAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_BAND_MODE) {
    Init();
}

void ProtocolNetBandModeAdapterJson::Init() {
    memset(m_bandMode, 0, sizeof(m_bandMode));
    m_count = 0;

    if (m_bParsingSuccess && ProtocolJsonRespAdapter::GetErrorCode() == RCM_E_SUCCESS) {
        m_count = m_jsonData[ns_net_set_band_mode_rsp::key_band_mode_list].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, m_count, SIT_NET_BAND_MAX);
        for (unsigned int i = 0; i < m_count; ++i ){
            m_bandMode[i] = m_jsonData[ns_net_set_band_mode_rsp::key_band_mode_list][i].asInt();
        }
    }
};

/**
 * ProtocolNetAvailableNetworkAdapterJson
 */
int ProtocolNetAvailableNetworkAdapterJson::GetCount() {
    if (m_bParsingSuccess && GetErrorCode() == RCM_E_SUCCESS) {
        int count = m_jsonData[ns_net_get_available_networks_rsp::key_network_list].size();
        return count;
    }
    return 0;
}

bool ProtocolNetAvailableNetworkAdapterJson::GetNetwork(NetworkInfo &nwkInfo, int index, const char * /* simPlmn */, char * /* simSpn */) {
    if (m_bParsingSuccess && GetErrorCode() == RCM_E_SUCCESS) {
        int count = m_jsonData[ns_net_get_available_networks_rsp::key_network_list].size();
        if (index < 0 || index >= count) return false;

        Json::Value jsonNetworkInfo = m_jsonData[ns_net_get_available_networks_rsp::key_network_list][index];

        nwkInfo.rat = NetProtocolUtils::ConvertSitRat2RilRat(jsonNetworkInfo[ns_net_network_info::key_rat].asInt());

        // copy mcc/mnc
        string strData = jsonNetworkInfo[ns_net_network_info::key_plmn].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, strData, MAX_PLMN_LEN);
        memset(nwkInfo.plmn, 0, sizeof(nwkInfo.plmn));
        if(strData.size() > 0) memcpy(nwkInfo.plmn, strData.c_str(), strData.length());

        // check UNKNOW network
        if(MccTable::isUnknowNetwork(nwkInfo.plmn) > 0) return false;

        nwkInfo.status = NetProtocolUtils::ConvertPlmnStatus2Ril(jsonNetworkInfo[ns_net_network_info::key_plmn_status].asInt());

        // fill Long/Short EONS
        *nwkInfo.shortPlmn = 0;
        *nwkInfo.longPlmn = 0;
    }

    return true;
}

/**
 * ProtocolNetGetPsServiceAdapterJson
 */
int ProtocolNetGetPsServiceAdapterJson::GetState() {
    if (m_bParsingSuccess && GetErrorCode() == RCM_E_SUCCESS) {
        int psState = m_jsonData[ns_net_getset_ps_service::key_ps_state].asInt();
        return (psState == (int)SIT_NET_ENABLE ? ALLOW_DATA_CALL : DISALLOW_DATA_CALL);
    }
    return ALLOW_DATA_CALL;
}

/**
 * ProtocolNetDuplexModeRespAdapterJson
 */
int ProtocolNetDuplexModeRespAdapterJson::Get4gDuplexMode() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_net_getset_duplex_mode::key_duplex_lte].asInt();
    }
    return SIT_NET_DUPLEX_MODE_MAX;
}
int ProtocolNetDuplexModeRespAdapterJson::Get3gDuplexMode() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_net_getset_duplex_mode::key_duplex_hedge].asInt();
    }
    return SIT_NET_DUPLEX_MODE_MAX;
}
int ProtocolNetDuplexModeRespAdapterJson::GetDuplexMode() const {
    int mode = DUPLEX_MODE_INVALID;
    int duplex_mode_4g = Get4gDuplexMode();
    int duplex_mode_3g = Get3gDuplexMode();

    if(duplex_mode_4g == SIT_NET_DUPLEX_MODE_TDD && duplex_mode_3g == SIT_NET_DUPLEX_MODE_TDD) {
        mode = DUPLEX_MODE_LTG;
    }
    else if (duplex_mode_4g == SIT_NET_DUPLEX_MODE_FDD_TDD && duplex_mode_3g == SIT_NET_DUPLEX_MODE_FDD) {
        mode = DUPLEX_MODE_LWG;
    }
    else if (duplex_mode_4g == SIT_NET_DUPLEX_MODE_FDD_TDD && duplex_mode_3g == SIT_NET_DUPLEX_MODE_FDD_TDD) {
        mode = DUPLEX_MODE_GLOBAL;
    }
    return mode;

    return 0;
}


/**
 * ProtocolNetEmergencyActInfoAdapterJson
 */
int ProtocolNetEmergencyActInfoAdapterJson::GetRat() const {
    if (m_bParsingSuccess) {
        int rat = NetProtocolUtils::ConvertSitRat2RilRat(m_jsonData[ns_net_emergency_act_info_ind::key_rat].asInt());
        // consider RADIO_TECH_LTE_CA as RADIO_TECH_LTE
        if (rat == RADIO_TECH_LTE_CA) rat = RADIO_TECH_LTE;
        return rat;
    }
    return RADIO_TECH_UNKNOWN;
}
int ProtocolNetEmergencyActInfoAdapterJson::GetActStatus() const {
    if (m_bParsingSuccess) {
        int actStatus = m_jsonData[ns_net_emergency_act_info_ind::key_act_status].asInt();
        if (actStatus == SIT_NET_CURRENT_ACT_EMERGENCY_CALL) return EMERGENCY_CALL_AVAILABLE;
        else if (actStatus == SIT_NET_RETRY_ACT_EMERGENCY_CALL) return EMERGENCY_CALL_RETRY;
    }
    return EMERGENCY_CALL_NOT_AVAILABLE;
}

/**
 * ProtocolNetMcSrchRespAdapterJson
 */
ProtocolNetMcSrchRespAdapterJson::ProtocolNetMcSrchRespAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_SET_MICRO_CELL_SEARCH) {
    Init();
}

void ProtocolNetMcSrchRespAdapterJson::Init() {
    memset(m_szPlmn, 0, sizeof(m_szPlmn));
    if (m_bParsingSuccess) {
        string plmn = m_jsonData[ns_net_set_micro_cell_search_rsp::key_plmn].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmn, MAX_PLMN_LEN);
        if (plmn.size() > 0) memcpy(m_szPlmn, plmn.c_str(), plmn.length());
    }
};

int ProtocolNetMcSrchRespAdapterJson::GetMcSrchResult() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_net_set_micro_cell_search_rsp::key_search_result].asInt();
    }
    return -1;
}
const char *ProtocolNetMcSrchRespAdapterJson::GetMcSrchPlmn() const {
    return (m_szPlmn[0] == 0 ? NULL : m_szPlmn);
}

/**
 * ProtocolSetNetworkRCRespAdapterJson
 */

/**
 * ProtocolGetNetworkRCRespAdapterJson
 */
int ProtocolGetNetworkRCRespAdapterJson::GetVersion() const {
    return RIL_RADIO_CAPABILITY_VERSION;
}

int ProtocolGetNetworkRCRespAdapterJson::GetSession() const {
    return 0;
}

int ProtocolGetNetworkRCRespAdapterJson::GetPhase() const {
    return RC_PHASE_CONFIGURED;
}

int ProtocolGetNetworkRCRespAdapterJson::GetRafType() const {
    if (m_bParsingSuccess) {
        int raf = m_jsonData[ns_net_get_radio_capability_rsp::key_raf].asInt();
        return NetProtocolUtils::ConvertSitRaf2Ril(raf, true);
    }
    return RAF_CP_UNKNOWN;
}

BYTE *ProtocolGetNetworkRCRespAdapterJson::GetUuid() const {
    return NULL;
}

int ProtocolGetNetworkRCRespAdapterJson::GetStatus() const {
    return RC_STATUS_NONE;
}

/**
 * ProtocolNetworkRCIndAdapterJson
 */
int ProtocolNetworkRCIndAdapterJson::GetVersion() const {
    return RIL_RADIO_CAPABILITY_VERSION;
}
int ProtocolNetworkRCIndAdapterJson::GetSession() const {
    return 0;
}
int ProtocolNetworkRCIndAdapterJson::GetPhase() const {
    return RC_PHASE_CONFIGURED;
}
int ProtocolNetworkRCIndAdapterJson::GetRafType() const {
    return RAF_CP_UNKNOWN;
}
BYTE *ProtocolNetworkRCIndAdapterJson::GetUuid() const {
    return NULL;
}
int ProtocolNetworkRCIndAdapterJson::GetStatus() const {
    return RC_STATUS_NONE;
}

/**
 * ProtocolNetCdmaQueryRoamingTypeAdapterJson
 */
int ProtocolNetCdmaQueryRoamingTypeAdapterJson::QueryRoamingType() const {
    if (m_bParsingSuccess) {
        int cdmaRoamingType = m_jsonData[ns_net_getset_cdma_roaming_preferrence::key_cdma_mode].asInt();
        return NetProtocolUtils::ConvertSitCdmaRoamingType2Ril(cdmaRoamingType);
    }
    return CDMA_ROAMING_HOME_ONLY;
}

/**
 * ProtocolNetCdmaHybridModeAdapterJson
 */
int ProtocolNetCdmaHybridModeAdapterJson::GetCdmaHybridMode() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_net_getset_cdma_hybrid_mode::key_hybrid_mode].asInt();
    }
    return HYBRID_MODE_1X_HRPD;
}

/**
 * ProtocolNetMccAdapterJson
 */
ProtocolNetMccAdapterJson::ProtocolNetMccAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_MCC) {
    Init();
}

void ProtocolNetMccAdapterJson::Init() {
    memset(mMcc, 0 , sizeof(mMcc));
    if (m_bParsingSuccess) {
        string mcc = m_jsonData[ns_net_mcc_ind::key_mcc].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, mcc, MAX_MCC_LEN);
        if (mcc.size() > 0) strncpy(mMcc, mcc.c_str(), mcc.length());
    }
};

int ProtocolNetMccAdapterJson::GetCurrentPrefNetworkMode() const {
    if (m_bParsingSuccess) {
        int prefNetType = m_jsonData[ns_net_mcc_ind::key_pref_net_type].asInt();
        RilLogV("Get Pref.NetMode in Mcc=%s(0x%02x)",
            NetProtocolUtils::ConvertSitPreferredNetType2String(prefNetType), prefNetType);
        return prefNetType;

    }
    return SIT_NET_PREF_NET_TYPE_GSM_WCDMA;
}

/**
 * ProtocolNetCellInfoListAdapterJson
 */
ProtocolNetCellInfoListAdapterJson::~ProtocolNetCellInfoListAdapterJson() {
    /* need to delete the memory of additional plmms */
    list<RIL_CellInfo_V1_6>::iterator iter;
    for (iter = mCellInfoList.begin(); iter != mCellInfoList.end(); iter++) {
        RIL_CellInfo_V1_6 &cur = *iter;
        NetProtocolUtils::FreeAdditionalPlmms(cur);
    }
}

list<RIL_CellInfo_V1_6>& ProtocolNetCellInfoListAdapterJson::GetCellInfoList(int cellInfoVer) {
    mCellInfoList.resize(0);
    if (m_bParsingSuccess) {
        int cellInfoSize = m_jsonData[ns_net_cell_info_list::key_cell_info_list].size();

        CellInfoAdapterFactoryJson cellInfoAdapterFactoryJson;
        CellInfoListAdapterJson *cellInfoListAdapterJson = cellInfoAdapterFactoryJson.GetCellInfoListAdapterJson(cellInfoVer);
        if (cellInfoListAdapterJson != NULL) {
            cellInfoListAdapterJson->FillCellInfo(cellInfoSize, m_jsonData[ns_net_cell_info_list::key_cell_info_list]);
            mCellInfoList = cellInfoListAdapterJson->GetCellInfoList();
        }
    }
    return mCellInfoList;
}

/**
 * ProtocolNetCellInfoListIndAdapterJson
 */
ProtocolNetCellInfoListIndAdapterJson::~ProtocolNetCellInfoListIndAdapterJson() {
    /* need to delete the memory of additional plmms */
    list<RIL_CellInfo_V1_6>::iterator iter;
    for (iter = mCellInfoList.begin(); iter != mCellInfoList.end(); iter++) {
        RIL_CellInfo_V1_6 &cur = *iter;
        NetProtocolUtils::FreeAdditionalPlmms(cur);
    }
}

list<RIL_CellInfo_V1_6>& ProtocolNetCellInfoListIndAdapterJson::GetCellInfoList(int cellInfoVer) {
    if (m_bParsingSuccess) {
        int cellInfoSize = m_jsonData[ns_net_cell_info_list::key_cell_info_list].size();

        CellInfoAdapterFactoryJson cellInfoAdapterFactoryJson;
        CellInfoListAdapterJson *cellInfoListAdapterJson = cellInfoAdapterFactoryJson.GetCellInfoListAdapterJson(cellInfoVer);
        if (cellInfoListAdapterJson != NULL) {
            cellInfoListAdapterJson->FillCellInfo(cellInfoSize, m_jsonData[ns_net_cell_info_list::key_cell_info_list]);
            mCellInfoList = cellInfoListAdapterJson->GetCellInfoList();
        }
    }
    return mCellInfoList;
}

/**
 * ProtocolNetScanResultAdapterJson
 */
ProtocolNetScanResultAdapterJson::ProtocolNetScanResultAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_SCANNING_NETWORKS) {
    Init();
}

ProtocolNetScanResultAdapterJson::~ProtocolNetScanResultAdapterJson() {
    /* need to delete the memory of additional plmms */
    list<RIL_CellInfo_V1_6>::iterator iter;
    for (iter = mCellInfoList.begin(); iter != mCellInfoList.end(); iter++) {
        RIL_CellInfo_V1_6 &cur = *iter;
        NetProtocolUtils::FreeAdditionalPlmms(cur);
    }
}

void ProtocolNetScanResultAdapterJson::Init() {
    mCellInfoList.resize(0);
};

int ProtocolNetScanResultAdapterJson::GetScanStatus() const {
    if (m_bParsingSuccess) {
        if (m_jsonData[ns_net_scanning_networks_ind::key_status].asInt() == SIT_NET_SCAN_STATUS_PARTIAL) {
            return PARTIAL;
        }
    }
    return COMPLETE;
}

int ProtocolNetScanResultAdapterJson::GetScanResult() const {
    // TODO need to get result from modem
    return RIL_E_SUCCESS;
}

list<RIL_CellInfo_V1_6>& ProtocolNetScanResultAdapterJson::GetCellInfoList(int cellInfoVer) {
    if (m_bParsingSuccess) {
        int cellInfoSize = m_jsonData[ns_net_scanning_networks_ind::key_cell_info_list].size();

        CellInfoAdapterFactoryJson cellInfoAdapterFactoryJson;
        CellInfoListAdapterJson *cellInfoListAdapterJson = cellInfoAdapterFactoryJson.GetCellInfoListAdapterJson(cellInfoVer);
        if (cellInfoListAdapterJson != NULL) {
            cellInfoListAdapterJson->FillCellInfo(cellInfoSize, m_jsonData[ns_net_scanning_networks_ind::key_cell_info_list]);
            mCellInfoList = cellInfoListAdapterJson->GetCellInfoList();
        }
    }
    return mCellInfoList;
}

/**
 * ProtocolNetSimFileInfoAdapterJson
 */
ProtocolNetSimFileInfoAdapterJson::ProtocolNetSimFileInfoAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_SIM_DATA_INFO) {
    Init();
}

void ProtocolNetSimFileInfoAdapterJson::Init() {
    m_nNumOfRecords = 0;
    m_ppData = NULL;
    SetRecordsData();
}

void ProtocolNetSimFileInfoAdapterJson::SetRecordsData() {
    if (m_bParsingSuccess) {
        int nRecordLen = GetRecordLen();
        m_nNumOfRecords = m_jsonData[ns_net_sim_file_data_info_ind::key_num_records].asInt();
        m_ppData = new BYTE *[m_nNumOfRecords];
        int nRecordInfoListSize = m_jsonData[ns_net_sim_file_data_info_ind::key_ef_data_list].size();
        BYTE recordsInfo[nRecordInfoListSize];
        memset(recordsInfo, 0, nRecordInfoListSize);
        for (int i = 0; i < nRecordInfoListSize; i++) {
            recordsInfo[i] = (BYTE) m_jsonData[ns_net_sim_file_data_info_ind::key_ef_data_list][i].asInt();
        }
        // fill data for each record
        for (int i = 0; i < m_nNumOfRecords; i++) {
            BYTE *pInputRecord = recordsInfo + (i * nRecordLen);
            BYTE *record = new BYTE[nRecordLen];
            memset(record, 0, nRecordLen);
            memcpy(record, pInputRecord, nRecordLen);
            *(m_ppData + i) = record;
        }
    }
}

ProtocolNetSimFileInfoAdapterJson::~ProtocolNetSimFileInfoAdapterJson() {
    // delete memory
    if (m_ppData != NULL) {
        BYTE **ppTmp = m_ppData;
        BYTE *pDel = NULL;
        int i = m_nNumOfRecords;
        while(i > 0) {
            pDel = *ppTmp++;
            if(pDel != NULL) delete [] pDel;
            i--;
        }
        delete [] m_ppData;
    }
}

int ProtocolNetSimFileInfoAdapterJson::GetSimFileId() const {
    int nSimFileId = 0;
    if (m_bParsingSuccess) {
        nSimFileId = m_jsonData[ns_net_sim_file_data_info_ind::key_sim_file_id].asInt();
    }
    return nSimFileId;
}

int ProtocolNetSimFileInfoAdapterJson::GetRecordLen() const {
    int nRecordLen = 0;
    if (m_bParsingSuccess) {
        nRecordLen = m_jsonData[ns_net_sim_file_data_info_ind::key_record_len].asInt();
    }
    return nRecordLen;
}

/**
 * ProtocolNetPhysicalChannelConfigsJson
 */
ProtocolNetPhysicalChannelConfigsJson::ProtocolNetPhysicalChannelConfigsJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

void ProtocolNetPhysicalChannelConfigsJson::Init() {
    mPcc.resize(0);
    mSize = 0;

    ParseJsonPayload(SIT_IND_PHYSICAL_CHANNEL_CONFIG);
    if (m_bParsingSuccess) {
        mSize = m_jsonData[ns_net_physical_channel_config_ind::key_pci_list].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, mSize, MAX_PHYSICAL_CHANNEL_CONFIGS);

        for (unsigned int i = 0; i < mSize; i++) {
            PhysicalChannelConfigsData pcc = {};
            Json::Value jsonPci = m_jsonData[ns_net_physical_channel_config_ind::key_pci_list][i];
            int val[4] = { 0, };

            val[0] = jsonPci[ns_net_physical_channel_config_info::key_cell_status].asInt();
            val[1] = jsonPci[ns_net_physical_channel_config_info::key_rat].asInt();
            pcc.setCurrentStatusRat(val[0], NetProtocolUtils::ConvertSitRat2RilRat(val[1]));

            val[0] = (int)jsonPci[ns_net_physical_channel_config_info::key_channel].asUInt();
            val[1] = (int)jsonPci[ns_net_physical_channel_config_info::key_cell_bw_dl].asUInt();
            val[2] = jsonPci[ns_net_physical_channel_config_info::key_freq_range].asInt();
            val[3] = (int)jsonPci[ns_net_physical_channel_config_info::key_pcid].asUInt();
            pcc.setDownLinkInfo(val[0], val[1], val[2], val[3]);

            val[0] = jsonPci[ns_net_physical_channel_config_info::key_context_id_list].size();
            int *contextIds = (int *)calloc(val[0], sizeof(int));
            for (int j = 0; j < val[0]; ++j) {
                contextIds[j] = jsonPci[ns_net_physical_channel_config_info::key_context_id_list][j].asInt();
            }
            pcc.setContextIds(val[0], contextIds);
            free(contextIds);

            val[0] = (int)jsonPci[ns_net_physical_channel_config_info::key_channel_uplink].asUInt();
            val[1] = (int)jsonPci[ns_net_physical_channel_config_info::key_cell_bw_ul].asUInt();
            val[2] = jsonPci[ns_net_physical_channel_config_info::key_band_num].asInt();
            pcc.setUplinkInfo(val[0], val[1], val[2]);
            mPcc.push_back(pcc);
        } // end for i ~
    } else {
        ParseJsonPayload(SIT_IND_SCG_BEARER_ALLOCATION);
        if (m_bParsingSuccess) {
            mSize = 1;
            int rat = NetProtocolUtils::ConvertSitRat2RilRat(m_jsonData[ns_net_scg_bearer_alloc_ind::key_rat].asInt());
            int connectionStatus = m_jsonData[ns_net_scg_bearer_alloc_ind::key_scg_status].asInt();
            connectionStatus = (connectionStatus == SIT_SCG_ADDED) ? SECONDARY_SERVING : PRIMARY_SERVING;
            if (connectionStatus == SECONDARY_SERVING) {
                rat = RADIO_TECH_NR;
            }
            PhysicalChannelConfigsData pcc = {};
            pcc.setCurrentStatusRat(connectionStatus, rat);
            mPcc.push_back(pcc);
            // others, default
        }
    }
}

const char *ProtocolNetPhysicalChannelConfigsJson::GetParameter() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_legacyData;
#else
    return ProtocolIndAdapter::GetParameter();
#endif
}

UINT ProtocolNetPhysicalChannelConfigsJson::GetParameterLength() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_legacyDataLength;
#else
    return ProtocolIndAdapter::GetParameterLength();
#endif
}

/**
 * ProtocolNetGetManualRatModeAdapterJson
 */
ProtocolNetGetManualRatModeAdapterJson::ProtocolNetGetManualRatModeAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_MANUAL_RAT_MODE) {
    Init();
}

void ProtocolNetGetManualRatModeAdapterJson::Init() {
    m_manual_rat_mode_set = 0;
    m_rat = 0;
    if (m_bParsingSuccess) {
        m_manual_rat_mode_set = m_jsonData[ns_net_getset_manual_rat_mode::key_manual_rat_mode].asInt();
        m_rat = m_jsonData[ns_net_getset_manual_rat_mode::key_pref_net_type].asInt();
    }
};

void ProtocolNetGetManualRatModeAdapterJson::GetManualRatMode(void *data) {
    *((int *)data+0) = m_manual_rat_mode_set;
    *((int *)data+1) = m_rat;
}

/**
 * ProtocolNetSetManualRatModeAdapterJson
 */
int ProtocolNetSetManualRatModeAdapterJson::GetCause() const {
    if (m_bParsingSuccess) {
        return  m_jsonData[ns_net_set_manual_rat_mode_rsp::key_manual_rat_cause].asInt();
    }
    return 0;
}

/**
 * ProtocolNetGetFreqLockAdapterJson
 */
ProtocolNetGetFreqLockAdapterJson::ProtocolNetGetFreqLockAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_FREQUENCY_LOCK) {
    Init();
}

void ProtocolNetGetFreqLockAdapterJson::Init() {
    m_freq_mode_set = 0;
    m_rat = 0;
    m_lte_pcid = 0;
    m_lte_earfcn = 0;
    m_gsm_arfcn = 0;
    m_wcdma_psc = 0;
    m_wcdma_uarfcn = 0;
    if (m_bParsingSuccess) {
        m_freq_mode_set = m_jsonData[ns_net_getset_freq_lock::key_freq_lock_mode].asInt();
        m_rat = m_jsonData[ns_net_getset_freq_lock::key_rat].asInt();
        if (m_rat == (int)SIT_FREQ_LOCK_RAT_GSM) {
            m_gsm_arfcn = (int)m_jsonData[ns_net_getset_freq_lock::key_arfcn].asUInt();
        }

        if (m_rat == (int)SIT_FREQ_LOCK_RAT_WCDMA) {
            m_wcdma_psc = (int)m_jsonData[ns_net_getset_freq_lock::key_psc].asUInt();
            m_wcdma_uarfcn = (int)m_jsonData[ns_net_getset_freq_lock::key_uarfcn].asUInt();
        }

        if (m_rat == (int)SIT_FREQ_LOCK_RAT_LTE) {
            m_lte_pcid = (int)m_jsonData[ns_net_getset_freq_lock::key_pcid].asUInt();
            m_lte_earfcn = (int)m_jsonData[ns_net_getset_freq_lock::key_earfcn].asUInt();
        }
    }
};

void ProtocolNetGetFreqLockAdapterJson::GetFrequencyLock(void *data) {
    *((int *)data+0) = m_freq_mode_set;
    *((int *)data+1) = m_rat;
    *((int *)data+2) = m_lte_pcid;
    *((int *)data+3) = m_lte_earfcn;
    *((int *)data+4) = m_gsm_arfcn;
    *((int *)data+5) = m_wcdma_psc;
    *((int *)data+6) = m_wcdma_uarfcn;
}

/**
 * ProtocolNetSetFreqLockAdapterJson
 */
int ProtocolNetSetFreqLockAdapterJson::GetResult() const {
    if (m_bParsingSuccess) {
        return  m_jsonData[ns_net_set_freq_lock_rsp::key_freq_lock_cause].asInt();
    }
    return 0;
}

/**
 * ProtocolNetGetEndcModeAdapterJson
 */
int ProtocolNetGetEndcModeAdapterJson::GetEndcMode() const {
    if (m_bParsingSuccess) {
        int encdMode = m_jsonData[ns_net_getset_endc_mode::key_endc_mode].asInt();
        RilLogV("Get endc mode=%d", encdMode);
        return  encdMode;
    }
    return 0;
}

/**
 * ProtocolNetGetFrequencyInfoAdapterJson
 */
ProtocolNetGetFrequencyInfoAdapterJson::ProtocolNetGetFrequencyInfoAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_FREQUENCY_INFO) {
    Init();
}

void ProtocolNetGetFrequencyInfoAdapterJson::Init() {
    const int freqInfoTypePrimary = 0;
    const int freqInfoTypeSecondary = 1;

    m_indexPrimary = -1;
    m_indexSecondary = -1;

    if (m_bParsingSuccess) {
        int size = m_jsonData[ns_net_frequency_info_list::key_freq_info_list].size();
        for (int i = 0; i < size; ++i) {
            Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][i];
            int freqInfoType = freqInfo[ns_net_freq_info::key_info_type].asInt();
            if (freqInfoType == freqInfoTypePrimary) m_indexPrimary = i;
            if (freqInfoType == freqInfoTypeSecondary) m_indexSecondary = i;
        }
    }
};

int ProtocolNetGetFrequencyInfoAdapterJson::GetPrimaryRat() const {
    if (m_bParsingSuccess && m_indexPrimary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexPrimary];
        return freqInfo[ns_net_freq_info::key_rat].asInt();
    }
    return 0;
}
int ProtocolNetGetFrequencyInfoAdapterJson::GetPrimaryBand() const {
    if (m_bParsingSuccess && m_indexPrimary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexPrimary];
        return (int)freqInfo[ns_net_freq_info::key_band].asUInt();
    }
    return 0;
}
int ProtocolNetGetFrequencyInfoAdapterJson::GetPrimaryFrequency() const {
    if (m_bParsingSuccess && m_indexPrimary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexPrimary];
        return (int)freqInfo[ns_net_freq_info::key_freq_number].asUInt();
    }
    return 0;
}
int ProtocolNetGetFrequencyInfoAdapterJson::GetSecondaryRat() const {
    if (m_bParsingSuccess && m_indexSecondary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexSecondary];
        return freqInfo[ns_net_freq_info::key_rat].asInt();
    }
    return -1;
}
int ProtocolNetGetFrequencyInfoAdapterJson::GetSecondaryBand() const {
    if (m_bParsingSuccess && m_indexSecondary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexSecondary];
        return (int)freqInfo[ns_net_freq_info::key_band].asUInt();
    }
    return -1;
}
int ProtocolNetGetFrequencyInfoAdapterJson::GetSecondaryFrequency() const {
    if (m_bParsingSuccess && m_indexSecondary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexSecondary];
        return (int)freqInfo[ns_net_freq_info::key_freq_number].asUInt();
    }
    return -1;
}

/**
 * ProtocolNetworkFrequencyInfoIndAdapterJson
 */
ProtocolNetworkFrequencyInfoIndAdapterJson::ProtocolNetworkFrequencyInfoIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_FREQUENCY_INFO) {
    Init();
}

void ProtocolNetworkFrequencyInfoIndAdapterJson::Init() {
    const int freqInfoTypePrimary = 0;
    const int freqInfoTypeSecondary = 1;

    m_indexPrimary = -1;
    m_indexSecondary = -1;

    if (m_bParsingSuccess) {
        int size = m_jsonData[ns_net_frequency_info_list::key_freq_info_list].size();
        for (int i = 0; i < size; ++i) {
            Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][i];
            int freqInfoType = freqInfo[ns_net_freq_info::key_info_type].asInt();
            if (freqInfoType == freqInfoTypePrimary) m_indexPrimary = i;
            if (freqInfoType == freqInfoTypeSecondary) m_indexSecondary = i;
        }
    }
}

int ProtocolNetworkFrequencyInfoIndAdapterJson::GetPrimaryRat() const {
    if (m_bParsingSuccess && m_indexPrimary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexPrimary];
        return freqInfo[ns_net_freq_info::key_rat].asInt();
    }
    return 0;
}
int ProtocolNetworkFrequencyInfoIndAdapterJson::GetPrimaryBand() const {
    if (m_bParsingSuccess && m_indexPrimary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexPrimary];
        return freqInfo[ns_net_freq_info::key_band].asUInt();
    }
    return 0;
}
int ProtocolNetworkFrequencyInfoIndAdapterJson::GetPrimaryFrequency() const {
    if (m_bParsingSuccess && m_indexPrimary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexPrimary];
        return freqInfo[ns_net_freq_info::key_freq_number].asUInt();
    }
    return 0;
}
int ProtocolNetworkFrequencyInfoIndAdapterJson::GetSecondaryRat() const {
    if (m_bParsingSuccess && m_indexSecondary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexSecondary];
        return freqInfo[ns_net_freq_info::key_rat].asInt();
    }
    return -1;
}
int ProtocolNetworkFrequencyInfoIndAdapterJson::GetSecondaryBand() const {
    if (m_bParsingSuccess && m_indexSecondary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexSecondary];
        return freqInfo[ns_net_freq_info::key_band].asUInt();
    }
    return -1;
}
int ProtocolNetworkFrequencyInfoIndAdapterJson::GetSecondaryFrequency() const {
    if (m_bParsingSuccess && m_indexSecondary != -1) {
        Json::Value freqInfo = m_jsonData[ns_net_frequency_info_list::key_freq_info_list][m_indexSecondary];
        return freqInfo[ns_net_freq_info::key_freq_number].asUInt();
    }
    return -1;
}

/**
 * ProtocolNetAcBarringInfoJson
 */
ProtocolNetAcBarringInfoJson::ProtocolNetAcBarringInfoJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_AC_BARRING_INFO) {
    Init();
}

void ProtocolNetAcBarringInfoJson::Init() {
    NetProtocolUtils::InitAcBarringInfo(mAcBarringInfo);

    if (m_bParsingSuccess) {
        mAcBarringInfo.for_emc = m_jsonData[ns_net_ac_barring_info_ind::key_ac_barring_emc].asInt();

        unsigned int infoSize = m_jsonData[ns_net_ac_barring_info_ind::key_ac_barring_info_list].size();
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, infoSize, 4);

        for (unsigned int i = 0; i < infoSize; ++i) {
            Json::Value barringInfo = m_jsonData[ns_net_ac_barring_info_ind::key_ac_barring_info_list][i];
            int type = barringInfo[ns_net_ac_barring_info::key_ac_barring_info_type].asInt();

            Json::Value barring;
            if (type == SIT_AC_BARRING_TYPE_MO_SIG) {
                barring = barringInfo[ns_net_ac_barring_info::key_mo_sig];
                mAcBarringInfo.for_mo_sig_factor = barring[ns_net_ac_barring::key_barring_factor].asInt();
                mAcBarringInfo.for_mo_sig_time = barring[ns_net_ac_barring::key_barring_time].asInt();
                unsigned int acListSize = barring[ns_net_ac_barring::key_special_ac_list].size();
                ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, acListSize, SPECIAL_AC_LIST);
                for (unsigned int j = 0; j < acListSize; ++j) {
                    mAcBarringInfo.for_mo_sig_ac_list[j] = barring[ns_net_ac_barring::key_special_ac_list][j].asInt();
                }
            } else if (type == SIT_AC_BARRING_TYPE_MO_DATA) {
                barring = barringInfo[ns_net_ac_barring_info::key_mo_data];
                mAcBarringInfo.for_mo_data_factor = barring[ns_net_ac_barring::key_barring_factor].asInt();
                mAcBarringInfo.for_mo_data_time = barring[ns_net_ac_barring::key_barring_time].asInt();
                unsigned int acListSize = barring[ns_net_ac_barring::key_special_ac_list].size();
                ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, acListSize, SPECIAL_AC_LIST);
                for (unsigned int j = 0; j < acListSize; ++j) {
                    mAcBarringInfo.for_mo_data_ac_list[j] = barring[ns_net_ac_barring::key_special_ac_list][j].asInt();
                }
            } else if (type == SIT_AC_BARRING_TYPE_MMTEL_VOICE) {
                barring = barringInfo[ns_net_ac_barring_info::key_mmtel_voice];
                mAcBarringInfo.for_mmtel_voice_factor = barring[ns_net_ac_barring::key_barring_factor].asInt();
                mAcBarringInfo.for_mmtel_voice_time = barring[ns_net_ac_barring::key_barring_time].asInt();
                unsigned int acListSize = barring[ns_net_ac_barring::key_special_ac_list].size();
                ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, acListSize, SPECIAL_AC_LIST);
                for (unsigned int j = 0; j < acListSize; ++j) {
                    mAcBarringInfo.for_mmtel_voice_ac_list[j] = barring[ns_net_ac_barring::key_special_ac_list][j].asInt();
                }
            } else if (type == SIT_AC_BARRING_TYPE_MMTEL_VIDEO) {
                barring = barringInfo[ns_net_ac_barring_info::key_mmtel_video];
                mAcBarringInfo.for_mmtel_video_factor = barring[ns_net_ac_barring::key_barring_factor].asInt();
                mAcBarringInfo.for_mmtel_video_time = barring[ns_net_ac_barring::key_barring_time].asInt();
                unsigned int acListSize = barring[ns_net_ac_barring::key_special_ac_list].size();
                ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, acListSize, SPECIAL_AC_LIST);
                for (unsigned int j = 0; j < acListSize; ++j) {
                   mAcBarringInfo.for_mmtel_video_ac_list[j] = barring[ns_net_ac_barring::key_special_ac_list][j].asInt();
                }
            }
        }
    }
}

void ProtocolNetAcBarringInfoJson::GetAcBarringInfo(void *data, unsigned int /* size */) {
    *((char *)data+0) = mAcBarringInfo.for_emc;
    *((char *)data+1) = mAcBarringInfo.for_mo_sig_factor;
    *((char *)data+2) = mAcBarringInfo.for_mo_data_factor;
    *((char *)data+3) = mAcBarringInfo.for_mmtel_voice_factor;
    *((char *)data+4) = mAcBarringInfo.for_mmtel_video_factor;
}

/**
 * ProtocolNetRrcInfoAdapterJson
 */
int ProtocolNetRrcInfoAdapterJson::GetRat() const {
    if (m_bParsingSuccess) {
        return NetProtocolUtils::ConvertSitRat2RilRat(m_jsonData[ns_net_rrc_info_ind::key_rat].asInt());
    }
    return RADIO_TECH_UNKNOWN;
}

int ProtocolNetRrcInfoAdapterJson::GetState() const {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_net_rrc_info_ind::key_rrc_state].asInt();
    }
    return RRC_IDLE;
}

/**
 * ProtocolNetGetNrModeAdapterJson
 */
int ProtocolNetGetNrModeAdapterJson::GetNrMode() const {
    if (m_bParsingSuccess) {
        int nrMode = m_jsonData[ns_net_getset_nr_mode::key_nr_mode].asInt();
        RilLogV("Get NR mode=%d", nrMode);
        return nrMode;
    }
    return SIT_NR_MODE_NO_NR;
}

/**
 * ProtocolNetGetNrSilenceModeAdapterJson
 */
bool ProtocolNetGetNrSilenceModeAdapterJson::GetNsaMode() const {
    if (m_bParsingSuccess && GetErrorCode() == RIL_E_SUCCESS) {
        int nsaMode = m_jsonData[ns_net_misc_get_nr_silence_mode_rsp::key_nsa_mode].asInt();
        return (nsaMode != 0);
    }
    return false;
}

bool ProtocolNetGetNrSilenceModeAdapterJson::GetSaMode() const {
    if (m_bParsingSuccess && GetErrorCode() == RIL_E_SUCCESS) {
        int saMode = m_jsonData[ns_net_misc_get_nr_silence_mode_rsp::key_sa_mode].asInt();
        return (saMode != 0);
    }
    return false;
}

/**
 * ProtocolRegistrationFailedAdapterJson
 */
ProtocolRegistrationFailedAdapterJson::ProtocolRegistrationFailedAdapterJson(const ModemData *pModemData, int cellIdentityVer)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_REG_FAILED) {
    mCellIdentityVer = cellIdentityVer;
    Init();
}

ProtocolRegistrationFailedAdapterJson::~ProtocolRegistrationFailedAdapterJson() {
    if (mRegFailedInfo.chosenPlmn != NULL) delete[] mRegFailedInfo.chosenPlmn;

    /* need to delete the memory of additional plmms */
    NetProtocolUtils::FreeAdditionalPlmms(mRilCellInfo);
    memset(&mRilCellInfo, 0, sizeof(mRilCellInfo));
}

void ProtocolRegistrationFailedAdapterJson::Init() {
    memset(&mRegFailedInfo, 0, sizeof(mRegFailedInfo));
    memset(&mRilCellInfo, 0, sizeof(mRilCellInfo));

    if (m_bParsingSuccess) {
        mRegFailedInfo.chosenPlmn = new char[MAX_PLMN_LEN + 1];
        memset(mRegFailedInfo.chosenPlmn, 0, MAX_PLMN_LEN + 1);
        string plmn = m_jsonData[ns_net_reg_failed_ind::key_plmn].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_NET, __CLASS_FUNC__, plmn, MAX_PLMN_LEN);
        if (plmn.length() > 0) {
            strncpy(mRegFailedInfo.chosenPlmn, plmn.c_str(), plmn.length());
        } else {
            strncpy(mRegFailedInfo.chosenPlmn, "000000", MAX_PLMN_LEN);
        }

        int domain = m_jsonData[ns_net_reg_failed_ind::key_domain].asInt();
        switch (domain) {
            case SERVICE_DOMAIN_CSPS:
                mRegFailedInfo.domainBitmap = DOMAIN_CS | DOMAIN_PS;
                break;
            case SERVICE_DOMAIN_CS:
                mRegFailedInfo.domainBitmap = DOMAIN_CS;
                break;
            case SERVICE_DOMAIN_PS:
                mRegFailedInfo.domainBitmap = DOMAIN_PS;
                break;
        }

        int val = m_jsonData[ns_net_reg_failed_ind::key_rej_cause].asInt();
        mRegFailedInfo.causeCode = (val == REG_REJ_CAUSE_UNUSED) ? INT_MAX : val;

        val = m_jsonData[ns_net_reg_failed_ind::key_add_rej_cause].asInt();
        mRegFailedInfo.additionalCauseCode = (val == REG_REJ_CAUSE_UNUSED) ? INT_MAX : val;

        mRegFailedInfo.cellIdentity.cellInfoType = (RIL_CellInfoType)(m_jsonData[ns_net_reg_failed_ind::key_cell_info_type].asInt() + 1);

        // decoding cell identity
        mRilCellInfo.cellInfoType = mRegFailedInfo.cellIdentity.cellInfoType;
        CellInfoProtocolUtils::InitCellInfo(mRilCellInfo, (int)mRilCellInfo.cellInfoType);

        CellInfoAdapterFactoryJson cellInfoAdapterFactoryJson;
        CellInfoListAdapterJson *cellInfoListAdapterJson = cellInfoAdapterFactoryJson.GetCellInfoListAdapterJson(mCellIdentityVer);
        if (cellInfoListAdapterJson != NULL) {
            switch ((int)mRilCellInfo.cellInfoType) {
            case RIL_CELL_INFO_TYPE_GSM:
                cellInfoListAdapterJson->FillCellIdentityGsm(mRilCellInfo, m_jsonData[ns_net_reg_failed_ind::key_cell_identity_gsm]);
                mRegFailedInfo.cellIdentity.gsm = mRilCellInfo.CellInfo.gsm.cellIdentityGsm;
                break;
            case RIL_CELL_INFO_TYPE_CDMA:
                cellInfoListAdapterJson->FillCellIdentityCdma(mRilCellInfo, m_jsonData[ns_net_reg_failed_ind::key_cell_identity_cdma]);
                mRegFailedInfo.cellIdentity.cdma = mRilCellInfo.CellInfo.cdma.cellIdentityCdma;
                break;
            case RIL_CELL_INFO_TYPE_LTE:
                cellInfoListAdapterJson->FillCellIdentityLte(mRilCellInfo, m_jsonData[ns_net_reg_failed_ind::key_cell_identity_lte]);
                mRegFailedInfo.cellIdentity.lte = mRilCellInfo.CellInfo.lte.cellIdentityLte;
                break;
            case RIL_CELL_INFO_TYPE_WCDMA:
                cellInfoListAdapterJson->FillCellIdentityWcdma(mRilCellInfo, m_jsonData[ns_net_reg_failed_ind::key_cell_identity_wcdma]);
                mRegFailedInfo.cellIdentity.wcdma = mRilCellInfo.CellInfo.wcdma.cellIdentityWcdma;
                break;
            case RIL_CELL_INFO_TYPE_TD_SCDMA:
                cellInfoListAdapterJson->FillCellIdentityTdscdma(mRilCellInfo, m_jsonData[ns_net_reg_failed_ind::key_cell_identity_tdscdma]);
                mRegFailedInfo.cellIdentity.tdscdma = mRilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma;
                break;
            case RIL_CELL_INFO_TYPE_NR:
                cellInfoListAdapterJson->FillCellIdentityNr(mRilCellInfo, m_jsonData[ns_net_reg_failed_ind::key_cell_identity_nr]);
                mRegFailedInfo.cellIdentity.nr = mRilCellInfo.CellInfo.nr.cellIdentityNr;
                break;
            default:
                // TODO stop parsing because of untrusted data type
                RilLogW("[ProtocolRegistrationFailedAdapterJson::%s] Unsupported CellInfoType %d", __FUNCTION__, (int)mRilCellInfo.cellInfoType);
                return;
            }
        }
    }
}

/**
 * ProtocolBarringInfosJson
 */
ProtocolBarringInfosJson::ProtocolBarringInfosJson() {
    memset(&mRilCellInfo, 0, sizeof(mRilCellInfo));
    memset(&mCellIdAndBarringInfo, 0, sizeof(mCellIdAndBarringInfo));
}

ProtocolBarringInfosJson::~ProtocolBarringInfosJson() {
    if (mCellIdAndBarringInfo.precords != NULL) {
        delete[] mCellIdAndBarringInfo.precords;
    }
    memset(&mCellIdAndBarringInfo, 0, sizeof(mCellIdAndBarringInfo));

    /* need to delete the memory of additional plmms */
    NetProtocolUtils::FreeAdditionalPlmms(mRilCellInfo);
    memset(&mRilCellInfo, 0, sizeof(mRilCellInfo));
}

void ProtocolBarringInfosJson::UpdateCellIdentityInfo(Json::Value &jsonData, int cellIdentityVer) {
    RIL_CellInfoType cellInfoType = (RIL_CellInfoType)(jsonData[ns_net_barring_info_changed::key_cell_info_type].asInt() + 1);

    mCellIdAndBarringInfo.cellIdentity.cellInfoType = cellInfoType;

    mRilCellInfo.cellInfoType = cellInfoType;
    CellInfoProtocolUtils::InitCellInfo(mRilCellInfo, (int)cellInfoType);
    CellInfoAdapterFactoryJson cellInfoAdapterFactoryJson;
    CellInfoListAdapterJson *cellInfoListAdapterJson = cellInfoAdapterFactoryJson.GetCellInfoListAdapterJson(cellIdentityVer);
    if (cellInfoListAdapterJson != NULL) {
        switch ((int)cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            cellInfoListAdapterJson->FillCellIdentityGsm(mRilCellInfo, jsonData[ns_net_barring_info_changed::key_cell_identity_gsm]);
            mCellIdAndBarringInfo.cellIdentity.gsm = mRilCellInfo.CellInfo.gsm.cellIdentityGsm;
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            cellInfoListAdapterJson->FillCellIdentityCdma(mRilCellInfo, jsonData[ns_net_barring_info_changed::key_cell_identity_cdma]);
            mCellIdAndBarringInfo.cellIdentity.cdma = mRilCellInfo.CellInfo.cdma.cellIdentityCdma;
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            cellInfoListAdapterJson->FillCellIdentityLte(mRilCellInfo, jsonData[ns_net_barring_info_changed::key_cell_identity_lte]);
            mCellIdAndBarringInfo.cellIdentity.lte = mRilCellInfo.CellInfo.lte.cellIdentityLte;
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            cellInfoListAdapterJson->FillCellIdentityWcdma(mRilCellInfo, jsonData[ns_net_barring_info_changed::key_cell_identity_wcdma]);
            mCellIdAndBarringInfo.cellIdentity.wcdma = mRilCellInfo.CellInfo.wcdma.cellIdentityWcdma;
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            cellInfoListAdapterJson->FillCellIdentityTdscdma(mRilCellInfo, jsonData[ns_net_barring_info_changed::key_cell_identity_tdscdma]);
            mCellIdAndBarringInfo.cellIdentity.tdscdma = mRilCellInfo.CellInfo.tdscdma.cellIdentityTdscdma;
            break;
        case RIL_CELL_INFO_TYPE_NR:
            cellInfoListAdapterJson->FillCellIdentityNr(mRilCellInfo, jsonData[ns_net_barring_info_changed::key_cell_identity_nr]);
            mCellIdAndBarringInfo.cellIdentity.nr = mRilCellInfo.CellInfo.nr.cellIdentityNr;
            break;
        default:
            // TODO stop parsing because of untrusted data type
            RilLogW("[ProtocolGetBarringInfoRspAdapterJson::%s][%d] Unsupported CellInfoType %d", __FUNCTION__, (int)cellInfoType);
            return;
        }
    }
}

void ProtocolBarringInfosJson::UpdateBarringInfos(Json::Value &jsonData) {
    unsigned int listLen = jsonData.size();
    if (listLen > 0) {
        mCellIdAndBarringInfo.numOfrecord = listLen;
        mCellIdAndBarringInfo.precords = new RIL_BarringInfo[mCellIdAndBarringInfo.numOfrecord];

        for(unsigned int i = 0; i < listLen; ++i) {
            Json::Value jsonSrc = jsonData[i];
            RIL_BarringInfo *pDst = mCellIdAndBarringInfo.precords+i;

            pDst->service = (RIL_BarringServiceType)jsonSrc[ns_net_barring_info::key_service_type].asInt();
            pDst->type = (RIL_BarringType)jsonSrc[ns_net_barring_info::key_barring_type].asInt();
            pDst->typeSpecificInfo.conditionalBarringInfo.barringFactor = jsonSrc[ns_net_barring_info::key_barring_factor].asInt();
            pDst->typeSpecificInfo.conditionalBarringInfo.barringTimeSeconds = jsonSrc[ns_net_barring_info::key_time_seconds].asInt();
            pDst->typeSpecificInfo.conditionalBarringInfo.isBarred =
                jsonSrc[ns_net_barring_info::key_is_barred].asInt() > 0 ? TRUE:FALSE;
            RilLogV("[%d] RIL_BarringServiceType=%d BarringType=%d barringFactor=%d barringTimeSeconds=%d isBarred=%d",
                    i, mCellIdAndBarringInfo.precords[i].service, mCellIdAndBarringInfo.precords[i].type,
                    mCellIdAndBarringInfo.precords[i].typeSpecificInfo.conditionalBarringInfo.barringFactor,
                    mCellIdAndBarringInfo.precords[i].typeSpecificInfo.conditionalBarringInfo.barringTimeSeconds,
                    mCellIdAndBarringInfo.precords[i].typeSpecificInfo.conditionalBarringInfo.isBarred);
        }
    }
}

RIL_CellIdAndBarringInfo *ProtocolBarringInfosJson::GetCellIdAndBarringInfo() {
    if (mCellIdAndBarringInfo.numOfrecord == 0 || mCellIdAndBarringInfo.precords == NULL) {
        return NULL;
    }
    return &mCellIdAndBarringInfo;
}

/**
 * ProtocolGetBarringInfoRspAdapterJson
 */
ProtocolGetBarringInfoRspAdapterJson::ProtocolGetBarringInfoRspAdapterJson(const ModemData *pModemData, int cellIdentityVer)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_BARRING_INFOS) {
    mCellIdentityVer = cellIdentityVer;
    Init();
}

void ProtocolGetBarringInfoRspAdapterJson::Init() {
    if (m_bParsingSuccess && ProtocolJsonRespAdapter::GetErrorCode() == RIL_E_SUCCESS) {
        // decoding cell identity
        UpdateCellIdentityInfo(m_jsonData, mCellIdentityVer);
        // decoding barring info
        UpdateBarringInfos(m_jsonData[ns_net_barring_info_changed::key_cell_barring_info_list]);
    }
}

RIL_CellIdAndBarringInfo *ProtocolGetBarringInfoRspAdapterJson::GetCellIdAndBarringInfo() {
    return ProtocolBarringInfosJson::GetCellIdAndBarringInfo();
}

/**
 * ProtocolBarringInfoChangedAdapterJson
 */
ProtocolBarringInfoChangedAdapterJson::ProtocolBarringInfoChangedAdapterJson(const ModemData *pModemData, int cellIdentityVer)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_BARRING_INFO_CHANGED) {
    mCellIdentityVer = cellIdentityVer;
    Init();
}

void ProtocolBarringInfoChangedAdapterJson::Init() {
    if (m_bParsingSuccess) {
        // decoding cell identity
        UpdateCellIdentityInfo(m_jsonData, mCellIdentityVer);
        // decoding barring info
        UpdateBarringInfos(m_jsonData[ns_net_barring_info_changed::key_cell_barring_info_list]);
    }
}

RIL_CellIdAndBarringInfo *ProtocolBarringInfoChangedAdapterJson::GetCellIdAndBarringInfo() {
    return ProtocolBarringInfosJson::GetCellIdAndBarringInfo();
}

/**
 * ProtocolNetGetVonrCapaAdapterJson
 */
int ProtocolNetGetVonrCapaAdapterJson::GetVonrCapa() const {
    if (m_bParsingSuccess) {
        int vonrCapa = m_jsonData[ns_net_misc_get_vonr_capability_rsp::key_vonr_capa].asInt();
        RilLogV("Get VoNR capability=%d", vonrCapa);
        return vonrCapa;
    }
    return 0;
}

/**
 * ProtocolNetGetAllowNetworkAdapterJson
 */
UINT32 ProtocolNetGetAllowNetworkAdapterJson::GetRat() const {
    if (m_bParsingSuccess) {
        int raf = m_jsonData[ns_net_getset_allow_network::key_raf].asInt();
        return  NetProtocolUtils::ConvertSitRaf2Ril(raf, false);
    }
    return 0;
}

/**
 * ProtocolNetSystemSelectionChannelsAdapterJson
 */
ProtocolNetSystemSelectionChannelsAdapterJson::ProtocolNetSystemSelectionChannelsAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_SYSTEM_SELECTION_CHANNELS) {
    Init();
}

void ProtocolNetSystemSelectionChannelsAdapterJson::Init() {
    memset(m_radioAccessSpecifiers, 0, sizeof(m_radioAccessSpecifiers));
    m_specifiersLength = 0;

    if (m_bParsingSuccess && ProtocolJsonRespAdapter::GetErrorCode() == RIL_E_SUCCESS) {
        // need to implement when json doc have this command.
    }
}

/**
 * ProtocolNetB1B2ConfigIndAdapterJson
 */
ProtocolNetB1B2ConfigIndAdapterJson::ProtocolNetB1B2ConfigIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    Init();
}

ProtocolNetB1B2ConfigIndAdapterJson::~ProtocolNetB1B2ConfigIndAdapterJson() {
    if(m_legacyData) {
        delete [] m_legacyData;
        m_legacyData = NULL;
    }
}

const char *ProtocolNetB1B2ConfigIndAdapterJson::GetParameter() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_legacyData;
#else
    return ProtocolIndAdapter::GetParameter();
#endif
}

UINT ProtocolNetB1B2ConfigIndAdapterJson::GetParameterLength() const {
#ifdef OEM_JSON_APPFW_CONVERTION
    return m_legacyDataLength;
#else
    return ProtocolIndAdapter::GetParameterLength();
#endif
}

void ProtocolNetB1B2ConfigIndAdapterJson::Init() {
    m_legacyData = NULL;
    m_legacyDataLength = 0;
    ParseJsonPayload(SIT_IND_B2_B1_CONFIG);

#ifdef OEM_JSON_APPFW_CONVERTION
    if(m_bParsingSuccess) {
        DataWriter writer;
        Json::Value config_list = m_jsonData[ns_b2_b1_config_ind::key_bb_config_list];
        int list_size = config_list.size();
        for(int i = 0; i < list_size; i++) {
            Json::Value config_data = config_list[i];

            //event type
            int event_type = config_data[ns_b2_b1_config_info::key_event_type].asInt();
            writer.writeByte((UINT8)event_type);

            //RAT type
            int rat_type = config_data[ns_b2_b1_config_info::key_ran].asInt();
            writer.writeByte((UINT8)rat_type);

            //eutran threshold type
            int eutran_threshold_type = config_data[ns_b2_b1_config_info::key_eutran_threshold_type].asInt();
            writer.writeByte((UINT8)eutran_threshold_type);

            //eutran threshold
            int eutran_threshold = config_data[ns_b2_b1_config_info::key_eutran_threshold].asInt();
            writer.writeShort((UINT16)eutran_threshold);

            //utran threshold type
            int utran_threshold_type = config_data[ns_b2_b1_config_info::key_utran_threshold_type].asInt();
            writer.writeByte((UINT8)utran_threshold_type);

            //utran threshold
            int utran_threshold = config_data[ns_b2_b1_config_info::key_utran_threshold].asInt();
            writer.writeShort((UINT16)utran_threshold);

            //geran threshold
            int geran_threshold_type = config_data[ns_b2_b1_config_info::key_geran_threshold].asInt();
            writer.writeByte((UINT8)geran_threshold_type);

            //cdma threshold
            int cdma_threshold = config_data[ns_b2_b1_config_info::key_cdma_threshold].asInt();
            writer.writeShort((UINT16)cdma_threshold);

            //hysteresis
            int hysteresis = config_data[ns_b2_b1_config_info::key_hysteresis].asInt();
            writer.writeShort((UINT16)hysteresis);

            //time_to_trigger
            int time_to_trigger = config_data[ns_b2_b1_config_info::key_time_to_trigger].asInt();
            writer.writeShort((UINT16)time_to_trigger);
        }

        m_legacyDataLength = writer.getSize();
        m_legacyData = new char[m_legacyDataLength];
        if(m_legacyData) {
            memcpy(m_legacyData, writer.getBytes(), m_legacyDataLength);
        } else {
            m_legacyDataLength = 0;
        }
    }
#endif //OEM_JSON_APPFW_CONVERTION
}

/**
 * ProtocolNetGetDeviceServiceAdapterJson
 */
int ProtocolNetGetDeviceServiceAdapterJson::GetUsageSetting() const {
    // To do implementation
    return 0;
}

