/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include <gtest/gtest.h>
#include <string.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "json/misc/protocolmiscadapterjson.h"
#include "miscjsondef.h"
#include "oemjsondef.h"
#include "sitdef.h"
#include "rildef.h"

#include <android/log.h>

#include <iostream>
using namespace std;

static void FillGsmSignalStrengthTestData(Json::Value &jsonGsmData, GSM_SIGNAL_STRENGTH &SS) {
    jsonGsmData[ns_misc_gsm_signal_strength::key_signal_strength] = SS.sig_str;
    jsonGsmData[ns_misc_gsm_signal_strength::key_ber] = SS.ber;
    jsonGsmData[ns_misc_gsm_signal_strength::key_ta] = SS.ta;
}

static void FillWcdmaSignalStrengthTestData(Json::Value &jsonWcdmaData, WCDMA_SIGNAL_STRENGTH &SS) {
    jsonWcdmaData[ns_misc_wcdma_signal_strength::key_signal_strength] = SS.sig_str;
    jsonWcdmaData[ns_misc_wcdma_signal_strength::key_ber] = SS.ber;
    jsonWcdmaData[ns_misc_wcdma_signal_strength::key_rscp] = SS.rscp;
    jsonWcdmaData[ns_misc_wcdma_signal_strength::key_ecno] = SS.ecno;
}

static void FillCdmaSignalStrengthTestData(Json::Value &jsonCdmaData, CDMA_SIGNAL_STRENGTH &SS) {
    jsonCdmaData[ns_misc_cdma_signal_strength::key_dbm] = SS.dbm;
    jsonCdmaData[ns_misc_cdma_signal_strength::key_ecio] = SS.ecio;
}

static void FillEvdoSignalStrengthTestData(Json::Value &jsonEvdoData, EVDO_SIGNAL_STRENGTH &SS) {
    jsonEvdoData[ns_misc_evdo_signal_strength::key_dbm] = SS.dbm;
    jsonEvdoData[ns_misc_evdo_signal_strength::key_ecio] = SS.ecio;
    jsonEvdoData[ns_misc_evdo_signal_strength::key_snr] = SS.snr;
}

static void FillTdscdmaSignalStrengthV2TestData(Json::Value &jsonTdscdmaData, TD_SCDMA_SIGNAL_STRENGTH_V2 &SS) {
    jsonTdscdmaData[ns_misc_tdscdma_signal_strength::key_signal_strength] = SS.sig_str;
    jsonTdscdmaData[ns_misc_tdscdma_signal_strength::key_ber] = SS.ber;
    jsonTdscdmaData[ns_misc_tdscdma_signal_strength::key_rscp] = SS.rscp;
}

static void FillLteSignalStrengthV4TestData(Json::Value &jsonLteData, LTE_SIGNAL_STRENGTH_V4 &SS) {
    jsonLteData[ns_misc_lte_signal_strength::key_signal_strength] = SS.sig_str;
    jsonLteData[ns_misc_lte_signal_strength::key_rsrp] = SS.rsrp;
    jsonLteData[ns_misc_lte_signal_strength::key_rsrq] = SS.rsrq;
    jsonLteData[ns_misc_lte_signal_strength::key_rssnr] = SS.rssnr;
    jsonLteData[ns_misc_lte_signal_strength::key_cqi_table_index] = SS.cqi_table_index;
    jsonLteData[ns_misc_lte_signal_strength::key_cqi] = SS.cqi;
    jsonLteData[ns_misc_lte_signal_strength::key_ta] = SS.timing_adv;
}

static void FillNrSignalStrengthV4TestData(Json::Value &jsonNrData, NR_SIGNAL_STRENGTH_V4 &SS) {
    jsonNrData[ns_misc_nr_signal_strength::key_ss_rsrp] = SS.ss_rsrp;
    jsonNrData[ns_misc_nr_signal_strength::key_ss_rsrq] = SS.ss_rsrq;
    jsonNrData[ns_misc_nr_signal_strength::key_ss_sinr] = SS.ss_sinr;
    jsonNrData[ns_misc_nr_signal_strength::key_csi_rsrp] = SS.csi_rsrp;
    jsonNrData[ns_misc_nr_signal_strength::key_csi_rsrq] = SS.csi_rsrq;
    jsonNrData[ns_misc_nr_signal_strength::key_csi_sinr] = SS.csi_sinr;
    jsonNrData[ns_misc_nr_signal_strength::key_cqi_table_index] = SS.cqi_table_index;
    jsonNrData[ns_misc_nr_signal_strength::key_wb_cqi] = SS.wb_cqi;

    for( int i = 0; i < SIT_MAX_NR_SUBBAND_INDEX; i++) {
        jsonNrData[ns_misc_nr_signal_strength::key_sb_cqi_list].append(SS.sb_cqi[i]);
    }
}

static int CompareGsmSignalStrength(RIL_GSM_SignalStrength_v12 &rilSS, GSM_SIGNAL_STRENGTH &cpSS) {
    if(rilSS.signalStrength != cpSS.sig_str) return -1;
    if(rilSS.bitErrorRate != cpSS.ber) return -2;
    if(rilSS.timingAdvance != cpSS.ta) return -3;

    return 0;
}

static int CompareWcdmaSignalStrength(RIL_WCDMA_SignalStrength_V1_2 &rilSS, WCDMA_SIGNAL_STRENGTH &cpSS) {
    if(rilSS.signalStrength != cpSS.sig_str) return -1;
    if(rilSS.bitErrorRate != cpSS.ber) return -2;
    if (rilSS.rscp != cpSS.rscp)
        return -3;
    if (rilSS.ecno != cpSS.ecno)
        return -4;

    return 0;
}

static int CompareCdmaSignalStrength(RIL_CDMA_SignalStrength &rilSS, CDMA_SIGNAL_STRENGTH &cpSS) {
    if(rilSS.dbm != cpSS.dbm) return -1;
    if(rilSS.ecio != cpSS.ecio) return -2;

    return 0;
}

static int CompareEvdoSignalStrength(RIL_EVDO_SignalStrength &rilSS, EVDO_SIGNAL_STRENGTH &cpSS) {
    if(rilSS.dbm != cpSS.dbm) return -1;
    if(rilSS.ecio != cpSS.ecio) return -2;
    if(rilSS.signalNoiseRatio != cpSS.snr) return -3;

    return 0;
}

static int CompareTdscdmaSignalStrength(RIL_TD_SCDMA_SignalStrength_V1_2 &rilSS, TD_SCDMA_SIGNAL_STRENGTH_V2 &cpSS) {
    if (rilSS.signalStrength != cpSS.sig_str)
        return -1;
    if (rilSS.bitErrorRate != cpSS.ber)
        return -2;
    if (rilSS.rscp != cpSS.rscp)
        return -3;

    return 0;
}

static int CompareLteSignalStrength(RIL_LTE_SignalStrength_V1_6 &rilSS, LTE_SIGNAL_STRENGTH_V4 &cpSS) {
    if(rilSS.signalStrength != cpSS.sig_str) return -1;
    if(rilSS.rsrp != cpSS.rsrp) return -2;
    if(rilSS.rsrq != cpSS.rsrq) return -3;
    if(rilSS.rssnr != cpSS.rssnr) return -4;
    if(rilSS.cqi != cpSS.cqi) return -5;
    if(rilSS.timingAdvance != cpSS.timing_adv) return -6;
    if (rilSS.cqiTableIndex != cpSS.cqi_table_index)
        return -7;

    return 0;
}

static int CompareNrSignalStrength(RIL_NR_SignalStrength_V1_6 &rilSS, NR_SIGNAL_STRENGTH_V4 &cpSS) {
    if(rilSS.ssRsrp != cpSS.ss_rsrp) return -1;
    if(rilSS.ssRsrq != cpSS.ss_rsrq) return -2;
    if(rilSS.ssSinr != cpSS.ss_sinr) return -3;
    if(rilSS.csiRsrp != cpSS.csi_rsrp) return -4;
    if(rilSS.csiRsrq != cpSS.csi_rsrq) return -5;
    if(rilSS.csiSinr != cpSS.csi_sinr) return -6;
    if (rilSS.csiCqiTableIndex != cpSS.cqi_table_index)
        return -7;

    if(cpSS.wb_cqi != INT_MAX) {
        if(rilSS.csiCqiReportLen != 1) return -8;
        if (rilSS.csiCqiReport[0] != (unsigned int)cpSS.wb_cqi)
            return -9;
    } else {
        if(rilSS.csiCqiReportLen >= MAX_NR_BAND_INDEX /*20*/) return -10; // SIT legacy/json SIT_MAX_NR_SUBBAND_INDEX (19)
        for(int i = 0; i < rilSS.csiCqiReportLen; i++) {
            if(rilSS.csiCqiReport[i] != cpSS.sb_cqi[i]) return -11;
        }
    }

    return 0;
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetManualBandModeAdapterJson) {
    struct {
        int sitData;
    } testVector[] {
        { 1 },
        { 2 },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_SET_MANUAL_BAND_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_misc_net_set_manual_band_mode_rsp::key_manual_mode_cause] = testVector[i].sitData;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetManualBandModeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetCause(), testVector[i].sitData);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetRfDesenseModeAdapterJson) {
    struct {
        int sitData;
    } testVector[] {
        { 3 },
        { 4 },
    };
    int size = sizeof(testVector)/sizeof(testVector[0]);
    unsigned int sitId = SIT_SET_RF_DESENSE_MODE;

    for(int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_misc_net_set_rf_desense_mode_rsp::key_desense_cause] = testVector[i].sitData;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetRfDesenseModeAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), sitId);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        EXPECT_EQ(testAdapter.GetCause(), testVector[i].sitData);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscVersionAdapterJson) {
    struct {
        int verMask;
        string swVer;
        string hwVer;
        string rfCalDate;
        string productCode;
        string modelId;
        string prlVer;
        string eriVer;
        string cpChipset;
    } testVector[] = {
        { 0xFF, "swVer", "hwVer", "rfCalDAte", "productCode", "modelId", string(51, 'x'), string(51, 'y'), "cpChipset"},
        { 0xFF, string(MAX_SW_VER_LEN, 's'), string(MAX_HW_VER_LEN, 'h'), string(MAX_RF_CAL_DATE_LEN, 'r'), string(MAX_PRODUCT_CODE_LEN, 'P'),
            string(MAX_MODEL_ID_LEN, 'm'), string(MAX_PRL_VER_LEN, 'x'), string(MAX_ERI_VER_LEN, 'y'), string(MAX_CP_CHIPSET_LEN, 'C') }
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_get_baseband_version_rsp::key_ver_mask] = testVector[i].verMask;
        jsonRspData[ns_misc_get_baseband_version_rsp::key_sw_ver] = testVector[i].swVer;
        jsonRspData[ns_misc_get_baseband_version_rsp::key_hw_ver] = testVector[i].hwVer;
        jsonRspData[ns_misc_get_baseband_version_rsp::key_rf_cal_date] = testVector[i].rfCalDate;
        jsonRspData[ns_misc_get_baseband_version_rsp::key_product_code] = testVector[i].productCode;
        jsonRspData[ns_misc_get_baseband_version_rsp::key_model_id] = testVector[i].modelId;
        jsonRspData[ns_misc_get_baseband_version_rsp::key_prl_ver] = testVector[i].prlVer;
        jsonRspData[ns_misc_get_baseband_version_rsp::key_eri_ver] = testVector[i].eriVer;
        jsonRspData[ns_misc_get_baseband_version_rsp::key_cp_chipset] = testVector[i].cpChipset;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_BASEBAND_VERSION, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscVersionAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_BASEBAND_VERSION);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
        //Payload data checks

        EXPECT_EQ(testAdapter.GetMask(), testVector[i].verMask);

        EXPECT_TRUE(strlen(testAdapter.GetSwVer()) <= MAX_SW_VER_LEN);
        EXPECT_EQ(strncmp(testAdapter.GetSwVer(), testVector[i].swVer.c_str(), MAX_SW_VER_LEN), 0);

        EXPECT_TRUE(strlen(testAdapter.GetHwVer()) <= MAX_HW_VER_LEN);
        EXPECT_EQ(strncmp(testAdapter.GetHwVer(), testVector[i].hwVer.c_str(), MAX_HW_VER_LEN), 0);

        EXPECT_TRUE(strlen(testAdapter.GetRfCalDate()) <= MAX_RF_CAL_DATE_LEN);
        EXPECT_EQ(strncmp(testAdapter.GetRfCalDate(), testVector[i].rfCalDate.c_str(), MAX_RF_CAL_DATE_LEN), 0);

        EXPECT_TRUE(strlen(testAdapter.GetProdCode()) <= MAX_PRODUCT_CODE_LEN);
        EXPECT_EQ(strncmp(testAdapter.GetProdCode(), testVector[i].productCode.c_str(), MAX_PRODUCT_CODE_LEN), 0);

        EXPECT_TRUE(strlen(testAdapter.GetModelID()) <= MAX_MODEL_ID_LEN);
        EXPECT_EQ(strcmp(testAdapter.GetModelID(), testVector[i].modelId.c_str()), 0);

        EXPECT_EQ(testAdapter.GetPrlNamNum(), 0);
        EXPECT_TRUE(strlen((const char *)testAdapter.GetPrlVersion()) == MAX_PRL_VER_LEN); // CP always sends string of 51 length
        EXPECT_EQ(strncmp((const char *)testAdapter.GetPrlVersion(), testVector[i].prlVer.c_str(), MAX_PRL_VER_LEN), 0);

        EXPECT_EQ(testAdapter.GetEriNamNum(), 0);
        EXPECT_TRUE(strlen((const char *)testAdapter.GetEriVersion()) == MAX_ERI_VER_LEN); // CP always sends string of 51 length
        EXPECT_EQ(strncmp((const char *)testAdapter.GetEriVersion(), testVector[i].eriVer.c_str(), MAX_ERI_VER_LEN), 0);


        EXPECT_TRUE(strlen((const char *)testAdapter.GetCPChipSet()) <= MAX_CP_CHIPSET_LEN);
        EXPECT_EQ(strncmp((const char *)testAdapter.GetCPChipSet(), testVector[i].cpChipset.c_str(), MAX_CP_CHIPSET_LEN), 0);

        delete modemData;
        modemData = nullptr;
    }
}


TEST(ProtocolMiscAdapterJson, ProtocolMiscGetTtyAdapterJson) {
    struct {
        int ttyMode;
    } testVector[] = {
        { SIT_MISC_TTY_MODE_OFF },
        { SIT_MISC_TTY_MODE_FULL },
        { SIT_MISC_TTY_MODE_HCO },
        { SIT_MISC_TTY_MODE_VCO },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_get_tty_mode_rsp::key_tty_mode] = testVector[i].ttyMode;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_TTY_MODE, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscGetTtyAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_TTY_MODE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetTtyMode(), testVector[i].ttyMode);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolOemModemInfoAdapter) {
    struct {
        int command_id;
        char * buffer;
    } testVector[] = {
        { 1, (char *)"12313455123123" },
        { 2, (char *)"abcdefg" },
        { 3, (char *)"123456" },
        { 4, (char *)"1" },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_nw_info_rsp::key_command_id] = testVector[i].command_id;
        jsonRspData[ns_oem_nw_info_rsp::key_buffer] = testVector[i].buffer;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_NW_INFO, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolOemModemInfoAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_NW_INFO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetCommandType(), testVector[i].command_id);
        EXPECT_STREQ((char *)testAdapter.GetData(), testVector[i].buffer);
        EXPECT_EQ(testAdapter.GetSize(), strlen(testVector[i].buffer));

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolOemModemInfoIndAdapterJson) {
    struct {
        int command_id;
        char * buffer;
    } testVector[] = {
        { 1, (char *)"12313455123123" },
        { 2, (char *)"abcdefg" },
        { 3, (char *)"123456" },
        { 4, (char *)"1" },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_oem_nw_info_ind::key_command_id] = testVector[i].command_id;
        jsonIndData[ns_oem_nw_info_ind::key_buffer] = testVector[i].buffer;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_OEM_IND_NW_INFO, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolOemModemInfoIndAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_IND_NW_INFO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetCommandType(), testVector[i].command_id);
        EXPECT_STREQ((char *)testAdapter.GetData(), testVector[i].buffer);
        EXPECT_EQ(testAdapter.GetSize(), strlen(testVector[i].buffer));

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetForbidLteCellAdapterJson) {
    struct {
        int result;
    } testVector[] = {
        { 1 },
        { 2 },
        { 3 },
        { 4 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_set_forbid_lte_rsp::key_result] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_SET_FORBID_LTE_CELL, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetForbidLteCellAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_FORBID_LTE_CELL);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetActiveVsimAdapterJson) {
    struct {
        int result;
    } testVector[] = {
        { 1 },
        { 2 },
        { 3 },
        { 4 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_set_activate_factory_vsim_rsp::key_result] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_SET_ACTIVATE_VSIM, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetActiveVsimAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_ACTIVATE_VSIM);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolOemSwitchModemFunctionAdapterJson) {
    struct {
        int result;
    } testVector[] = {
        { 1},
        { 2},
        { 3},
        { 4},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_set_fun_swtich_rsp::key_result] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_SET_FUNC_SWITCH_REQ, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolOemSwitchModemFunctionAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_FUNC_SWITCH_REQ);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetForbidNrCellAdapterJson) {
    struct {
        int result;
    } testVector[] = {
        { 1},
        { 2},
        { 3},
        { 4},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_set_forbid_nr_rsp::key_result] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_SET_FORBID_NR_CELL, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetForbidNrCellAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_FORBID_NR_CELL);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolOemGetCqiInfoAdapterJson) {
    struct {
        int type;
        int cqi0;
        int cqi1;
        int ri;
    } testVector[] = {
        { 1, 2, 3, 4},
        { 2, 3, 4, 5},
        { 3, 0xFF, 0xFA, 0x12},
        { 4, 0x123, 0x12, 0x23},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_get_cqi_info_rsp::key_type] = testVector[i].type;
        jsonRspData[ns_oem_get_cqi_info_rsp::key_cqi_info0] = testVector[i].cqi0;
        jsonRspData[ns_oem_get_cqi_info_rsp::key_cqi_info1] = testVector[i].cqi1;
        jsonRspData[ns_oem_get_cqi_info_rsp::key_ri] = testVector[i].ri;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_GET_CQI_INFO, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolOemGetCqiInfoAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_CQI_INFO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetCqiType(), testVector[i].type);
        EXPECT_EQ(testAdapter.GetCqiInfo0(), testVector[i].cqi0);
        EXPECT_EQ(testAdapter.GetCqiInfo1(), testVector[i].cqi1);
        EXPECT_EQ(testAdapter.GetRi(), testVector[i].ri);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetTcsFciAdapterJson) {
    struct {
        int result;
    } testVector[] = {
        { 1},
        { 2},
        { 3},
        { 4},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_set_tcs_fci_rsp::key_result] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_SET_TCS_FCI_REQ, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetTcsFciAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_TCS_FCI_REQ);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscGetTcsFciAdapterJson) {
    struct {
        char * fci;
    } testVector[] = {
        { (char *)"CHN01"},
        { (char *)"GCF1"},
        { (char *)"CAN1"},
        { (char *)""},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_get_tcs_fci_rsp::key_fci] = testVector[i].fci;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_GET_TCS_FCI_INFO, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscGetTcsFciAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_TCS_FCI_INFO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_STREQ(testAdapter.GetFci(), testVector[i].fci);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscEndcCapabilityIndAdapterJson) {
    struct {
        int capability;
        int cause;
    } testVector[] = {
        { 1 , 2},
        { 2 , 0x3},
        { 3 , 0xFA},
        { 4 , 0xC},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_oem_endc_capability_ind::key_endc_capa] = testVector[i].capability;
        jsonIndData[ns_oem_endc_capability_ind::key_cause] = testVector[i].cause;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_OEM_IND_ENDC_CAPABILITY, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscEndcCapabilityIndAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_IND_ENDC_CAPABILITY);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetCapability(), testVector[i].capability);
        EXPECT_EQ(testAdapter.GetCause(), testVector[i].cause);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscDropMtCallInfoIndAdapterJson) {
    // TODO: Make proper test data

    Json::Value jsonIndData;

    ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_OEM_IND_DROPPED_MT_CALL_INFO, jsonIndData);
    ASSERT_NE(modemData, nullptr);

    ProtocolMiscDropMtCallInfoIndAdapterJson testAdapter(modemData);
    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_IND_DROPPED_MT_CALL_INFO);

#ifdef OEM_JSON_APPFW_CONVERTION
    // TODO: Payload data checks
#else
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    ASSERT_NE(testAdapter.GetParameterLength(), 0U);

#endif //OEM_JSON_APPFW_CONVERTION
    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscGetModemActivityInfoAdapterJson) {
    struct __packed rxInfo {
        UINT8 rxIndex;
        int count;
    } rx_info[] = {
        {1, 100},
        {2, 101},
        {3, 0xA},
        {4, 0x0F},
        {5, 02},
    };
    struct __packed baseStationInfoList {
        BYTE mcc [3];
        BYTE mnc [3];
        BYTE rat;
        BYTE isNsa;
        UINT16 band;
        BYTE sigalLevel;
        int count;
    } bsList [] = {
        {{'4', '6', '0'}, {'0', '1', '2'}, 1, 1, 0xFF, 3, 1},
        {{'4', '5', '0'}, {'0', '1', '3'}, 1, 1, 0x1234, 2, 2},
        {{'4', '5', '6'}, {'0', '1', '4'}, 1, 1, 0xFD, 1, 3},
        {{'4', '4', '5'}, {'0', '1', '5'}, 1, 1, 0xFA, 3, 4},
        {{'4', '5', '4'}, {'0', '1', '6'}, 1, 1, 0xFC, 3, 5},
        {{'4', '1', '3'}, {'0', '1', '7'}, 1, 0, 0xFA, 3, 6},
        {{'4', '2', '2'}, {'0', '1', '8'}, 0, 1, 0xFF, 3,7},
    };
    struct __packed {
        int handOverCnt;
        int reselCnt;
        int cdrxConfTime;
        int rrcConnTime;
        BYTE rx_info[25];//struct rxInfo rxInfo[5];
        BYTE bsLen;
        //struct baseStationInfoList[0];
    } testVector[] = {
        { 1 , 2 , 3, 4, {}, 1},
        { 2 , 1 , 2, 3, {}, 3},
        { 3 , 4 , 1, 4, {}, 3},
        { 4 , 3 , 8, 6, {}, 4},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        memcpy(&(testVector[i].rx_info), &rx_info, 25);
        Json::Value jsonRspData;
        jsonRspData[ns_oem_get_activity_info_rsp::key_handover_count] = testVector[i].handOverCnt;
        jsonRspData[ns_oem_get_activity_info_rsp::key_reselection_count] = testVector[i].reselCnt;
        jsonRspData[ns_oem_get_activity_info_rsp::key_cdrx_config_time] = testVector[i].cdrxConfTime;
        jsonRspData[ns_oem_get_activity_info_rsp::key_rrc_connected_time] = testVector[i].rrcConnTime;

        Json::Value jsonRxInfo;
        for(int rxIndex = 0; rxIndex < 5; rxIndex++) {
            jsonRxInfo[rxIndex][ns_oem_activity_rx_info::key_rx] = rx_info[rxIndex].rxIndex;
            jsonRxInfo[rxIndex][ns_oem_activity_rx_info::key_count] = rx_info[rxIndex].count;
        }
        jsonRspData[ns_oem_get_activity_info_rsp::key_rx_info_list] = jsonRxInfo;

        Json::Value jsonBsInfoList;
        for(int bsIndex = 0; bsIndex < testVector[i].bsLen; bsIndex++) {
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_mcc] = string((const char *)bsList[bsIndex].mcc, 3);
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_mnc] = string((const char *)bsList[bsIndex].mnc, 3);
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_rat] = bsList[bsIndex].rat;
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_is_nsa] = bsList[bsIndex].isNsa;
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_band] = bsList[bsIndex].band;
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_signal_level] = bsList[bsIndex].sigalLevel;
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_count] = bsList[bsIndex].count;
        }
        jsonRspData[ns_oem_get_activity_info_rsp::key_base_station_info_list] = jsonBsInfoList;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_GET_ACTIVITY_INFO, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscGetModemActivityInfoAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_ACTIVITY_INFO);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

#ifdef OEM_JSON_APPFW_CONVERTION
        //Payload data checks
        char * expectedData = NULL;
        int expLen = sizeof(testVector[i]) + testVector[i].bsLen * sizeof(bsList[0]);
        expectedData = new char[expLen];
        char * buffer = expectedData;
        ASSERT_NE(expectedData, nullptr);

        //handOverCnt,reselCnt,cdrxConfTime,rrcConnTime,and rx_info
        memcpy(buffer, &(testVector[i]), sizeof(testVector[i]));
        buffer = buffer + sizeof(testVector[i]) - sizeof(testVector[i].bsLen);

        //Bs len
        memcpy(buffer, &(testVector[i].bsLen), sizeof(BYTE));
        buffer  += sizeof(BYTE);

        //Bs info
        memcpy(buffer, &bsList, (sizeof(bsList[0]) * testVector[i].bsLen));

        EXPECT_EQ(testAdapter.GetParameterLength(), expLen);
        for(int j = 0; j< expLen; j++) {
            const char * received = testAdapter.GetParameter();
            EXPECT_EQ(expectedData[j], received[j]);
        }

        delete [] expectedData;
        expectedData = nullptr;
#else
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        ASSERT_NE(testAdapter.GetParameterLength(), 0U);

#endif //OEM_JSON_APPFW_CONVERTION
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscBaseStationIndAdapter) {
    struct __packed baseStationInfoList {
        BYTE mcc [3];
        BYTE mnc [3];
        BYTE rat;
        BYTE isNsa;
        UINT16 band;
        BYTE sigalLevel;
        int count;
    } bsList [] = {
        {{'4', '6', '0'}, {'0', '1', '2'}, 1, 1, 0xFF, 3, 1},
        {{'4', '5', '0'}, {'0', '1', '3'}, 1, 1, 0x1234, 2, 2},
        {{'4', '5', '6'}, {'0', '1', '4'}, 1, 1, 0xFD, 1, 3},
        {{'4', '4', '5'}, {'0', '1', '5'}, 1, 1, 0xFA, 3, 4},
        {{'4', '5', '4'}, {'0', '1', '6'}, 1, 1, 0xFC, 3, 5},
        {{'4', '1', '3'}, {'0', '1', '7'}, 1, 0, 0xFA, 3, 6},
        {{'4', '2', '2'}, {'0', '1', '8'}, 0, 1, 0xFF, 3,7},
    };
    struct __packed {
        BYTE bsLen;
        //struct baseStationInfoList[0];
    } testVector[] = {
        { 1},
        { 3},
        { 5},
        { 4},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        Json::Value jsonBsInfoList;
        for(int bsIndex = 0; bsIndex < testVector[i].bsLen; bsIndex++) {
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_mcc] = string((const char *)bsList[bsIndex].mcc, 3);
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_mnc] = string((const char *)bsList[bsIndex].mnc, 3);
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_rat] = bsList[bsIndex].rat;
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_is_nsa] = bsList[bsIndex].isNsa;
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_band] = bsList[bsIndex].band;
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_signal_level] = bsList[bsIndex].sigalLevel;
            jsonBsInfoList[bsIndex][ns_oem_activity_base_station_info::key_count] = bsList[bsIndex].count;
        }
        jsonIndData[ns_oem_base_station_info_ind::key_base_station_info_list] = jsonBsInfoList;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_OEM_IND_BASESTATION_INFO, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscBaseStationIndAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_IND_BASESTATION_INFO);

#ifdef OEM_JSON_APPFW_CONVERTION
        //Payload data checks
        char * expectedData = NULL;
        int expLen = sizeof(testVector[i]) + testVector[i].bsLen * sizeof(bsList[0]);
        expectedData = new char[expLen];
        char * buffer = expectedData;
        ASSERT_NE(expectedData, nullptr);

        //Bs len
        memcpy(buffer, &(testVector[i].bsLen), sizeof(BYTE));
        buffer  += sizeof(BYTE);

        //Bs info
        memcpy(buffer, &bsList, (sizeof(bsList[0]) * testVector[i].bsLen));

        EXPECT_EQ(testAdapter.GetParameterLength(), expLen);
        for(int j = 0; j< expLen; j++) {
            const char * received = testAdapter.GetParameter();
            EXPECT_EQ(expectedData[j], received[j]);
        }

        delete [] expectedData;
        expectedData = nullptr;
#else
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        ASSERT_NE(testAdapter.GetParameterLength(), 0U);

#endif //OEM_JSON_APPFW_CONVERTION

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetForbidFrequencyAdapterJson) {
    struct {
        int result;
    } testVector[] = {
        { 1},
        { 2},
        { 3},
        { 4},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_set_tcs_fci_rsp::key_result] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_OEM_SET_FORBIDDEN_FREQUENCY, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetForbidFrequencyAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_FORBIDDEN_FREQUENCY);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscNITZTimeAdapterJson) {
    struct {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
        int cpTimeZone; // This field is 2s complement
        int rilTimeZone;
        int daylightAdjust;
    } testVector[] = {
        { 2021, 8, 20, 15, 23, 30, 2, 2, SIT_NITZ_DAYLIGHT_ADJUST_NOADJUST },
        { 2022, 9, 21, 6, 13, 45, 248, -8, SIT_NITZ_DAYLIGHT_ADJUST_PLUS1HOUR },
        { 2022, 9, 21, 6, 13, 45, 231, -25, SIT_NITZ_DAYLIGHT_ADJUST_PLUS2HOUR },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_nitz_time_ind::key_year] = testVector[i].year;
        jsonIndData[ns_misc_nitz_time_ind::key_month] = testVector[i].month;
        jsonIndData[ns_misc_nitz_time_ind::key_day] = testVector[i].day;
        jsonIndData[ns_misc_nitz_time_ind::key_hour] = testVector[i].hour;
        jsonIndData[ns_misc_nitz_time_ind::key_minute] = testVector[i].minute;
        jsonIndData[ns_misc_nitz_time_ind::key_second] = testVector[i].second;
        jsonIndData[ns_misc_nitz_time_ind::key_time_zone] = testVector[i].cpTimeZone;
        jsonIndData[ns_misc_nitz_time_ind::key_daylight_adjust] = testVector[i].daylightAdjust;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_NITZ_TIME_RECEIVED, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscNITZTimeAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_NITZ_TIME_RECEIVED);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.TimeInfoType(), 0);
        EXPECT_EQ(testAdapter.GetMMInfo(), 0);
        EXPECT_EQ(testAdapter.DayofWeek(), SIT_NITZ_DAY_OF_WEEK_SUN);
        EXPECT_EQ(testAdapter.DayLightValid(), SIT_NITZ_DAYLIGHT_INFO_VALID);
        EXPECT_EQ(testAdapter.GetPLMN(), nullptr);
        EXPECT_EQ(testAdapter.Year(), testVector[i].year);
        EXPECT_EQ(testAdapter.Month(), testVector[i].month);
        EXPECT_EQ(testAdapter.Day(), testVector[i].day);
        EXPECT_EQ(testAdapter.Hour(), testVector[i].hour);
        EXPECT_EQ(testAdapter.Minute(), testVector[i].minute);
        EXPECT_EQ(testAdapter.Second(), testVector[i].second);
        EXPECT_EQ(testAdapter.TimeZone(), testVector[i].rilTimeZone);
        EXPECT_EQ(testAdapter.DayLightAdjust(), testVector[i].daylightAdjust);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscGetActivityInfoAdapterJson) {
    struct {
        unsigned int sleepModeTime;
        unsigned int idleModeTime;
        unsigned int txModeTime[RIL_NUM_TX_POWER_LEVELS] = {UINT_MAX};
        unsigned int rxModeTime;
    } testVector[] = {
        { UINT_MAX, UINT_MAX, {[0 ... 4] = UINT_MAX}, UINT_MAX},
        { 0, 0, {[0 ... 4] = 0}, 0},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_get_activity_info_rsp::key_sleep_mode_time] = testVector[i].sleepModeTime;
        jsonRspData[ns_misc_get_activity_info_rsp::key_idle_mode_time] = testVector[i].idleModeTime;
        for(int j = 0; j < RIL_NUM_TX_POWER_LEVELS; j++) {
            jsonRspData[ns_misc_get_activity_info_rsp::key_tx_mode_time].append(testVector[i].txModeTime[j]);
        }
        jsonRspData[ns_misc_get_activity_info_rsp::key_rx_mode_time] = testVector[i].rxModeTime;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_ACTIVITY_INFO, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        //printf("[%s] JSON Data = %s\n", __FUNCTION__, rspData);
        ProtocolMiscGetActivityInfoAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_ACTIVITY_INFO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetSleepPeriod(), testVector[i].sleepModeTime);
        EXPECT_EQ(testAdapter.GetIdlePeriod(), testVector[i].idleModeTime);
        EXPECT_EQ(testAdapter.GetRxPeriod(), testVector[i].rxModeTime);

        unsigned int *txPeriod = testAdapter.GetTxPeriod();
        for(int j = 0; j < RIL_NUM_TX_POWER_LEVELS; j++) {
            EXPECT_EQ(txPeriod[j], testVector[i].txModeTime[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolCdmaSubscriptionAdapterJson) {
    struct {
        string mdn;
        string min;
        int sid;
        int nid;
        unsigned int prlVer;
    } testVector[] = {
        { "mdnString", "minString", 22, 33, 44 },
        { string(15+5, 's'), string(10+5, 'h'),22, 33, UINT_MAX},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_get_cdma_subscription_rsp::key_mdn] = testVector[i].mdn;
        jsonRspData[ns_misc_get_cdma_subscription_rsp::key_min] = testVector[i].min;
        jsonRspData[ns_misc_get_cdma_subscription_rsp::key_home_sid] = testVector[i].sid;
        jsonRspData[ns_misc_get_cdma_subscription_rsp::key_home_nid] = testVector[i].nid;
        jsonRspData[ns_misc_get_cdma_subscription_rsp::key_prl_version] = testVector[i].prlVer;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_CDMA_SUBSCRIPTION, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolCdmaSubscriptionAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_CDMA_SUBSCRIPTION);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
        //Payload data checks

        EXPECT_TRUE(strlen(testAdapter.GetMdn()) <= 15);
        EXPECT_EQ(strncmp(testAdapter.GetMdn(), testVector[i].mdn.c_str(), 15), 0);

        EXPECT_TRUE(strlen(testAdapter.GetMin()) <= 10);
        EXPECT_EQ(strncmp(testAdapter.GetMin(), testVector[i].min.c_str(), 10), 0);

        EXPECT_EQ(testAdapter.GetSid(), testVector[i].sid);
        EXPECT_EQ(testAdapter.GetNid(), testVector[i].nid);
        EXPECT_EQ(testAdapter.GetPrlVersion(), testVector[i].prlVer);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscGetMslCodeAdapterJson) {
    struct {
        string mslCode;
    } testVector[] = {
        { "mslCode_1" },
        { "mslCode_2" },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_get_msl_code_rsp::key_msl_code] = testVector[i].mslCode;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_MSL_CODE, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscGetMslCodeAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_MSL_CODE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(strcmp(testAdapter.getMslCode(), (testVector[i].mslCode).c_str()), 0);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolGetVoiceOperationAdapterJson) {
    struct {
        unsigned int cpMode;
        int rilMode;
    } testVector[] = {
        { 0, 0 },
        { 1, 1 },
        { 2, 1 },
        { 3, 1 },
        { UINT_MAX, 1 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_get_voice_operation_rsp::key_mode] = testVector[i].cpMode;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_VOICE_OPERATION, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetVoiceOperationAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_VOICE_OPERATION);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetMode(), testVector[i].rilMode);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscPinControlAdapterJson) {
    struct {
        int signal;
        int status;
    } testVector[] = {
        { 0x00 /*NONE*/, 0x00 /*off*/},
        { 0x01 /*DCD*/, 0x01 /*on*/},
        { 0x02 /*DTR*/, 0x01 /*on*/},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_pin_control_ind::key_pin_signal] = testVector[i].signal;
        jsonIndData[ns_misc_pin_control_ind::key_pin_status] = testVector[i].status;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_PIN_CONTROL, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscPinControlAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_PIN_CONTROL);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetSignal(), testVector[i].signal);
        EXPECT_EQ(testAdapter.GetStatus(), testVector[i].status);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSarControlStateAdapterJson) {
    struct {
        int deviceState;
    } testVector[] = {
        { 0x00 /*Disable*/ },
        { 0x01 /*Enable*/ },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_sar_control_state_ind::key_device_state] = testVector[i].deviceState;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SAR_CONTROL_STATE, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSarControlStateAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SAR_CONTROL_STATE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetDeviceState(), testVector[i].deviceState);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscGetPreferredCallCapabilityAdapterJson) {
    struct {
        int mode;
    } testVector[] = {
        { 0x00 }, // cs only
        { 0x01 }, // ps only
        { 0x02 }, // cs pref and ps sec
        { 0x03 }, // ps pref and cs sec
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_get_preferred_call_capability_rsp::key_mode] = testVector[i].mode;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_PREFERRED_CALL_CAPABILITY, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscGetPreferredCallCapabilityAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_PREFERRED_CALL_CAPABILITY);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetMode(), testVector[i].mode);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscATCommandAdapterJson) {
    struct {
        string command;
    } testVector[] = {
        { "yesIAmAString" },
        { string(MAX_SIT_AT_COMMAND_LENGTH + 2, 'S') }, // TC where data is more than expected
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_forwarding_at_command_ind::key_data] = testVector[i].command;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_FORWARDING_AT_COMMAND, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscATCommandAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_FORWARDING_AT_COMMAND);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        const char *data = testAdapter.GetCommand();
        EXPECT_TRUE(testAdapter.GetCommandLength() <= MAX_SIT_AT_COMMAND_LENGTH);
        EXPECT_EQ(strncmp(data, testVector[i].command.c_str(), MAX_SIT_AT_COMMAND_LENGTH), 0);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetSelflogAdapterJson) {
    struct {
        int result;
    } testVector[] = {
        { 0x00 },
        { 0x01 },
        { 0x02 },
        { 0x03 },
        { 0xFF },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_set_selflog_rsp::key_result] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SET_SELFLOG, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetSelflogAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_SELFLOG);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetSelflogResult(), testVector[i].result);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscGetSelflogStatusAdapterJson) {
    struct {
        int status;
    } testVector[] = {
        { 0x00 },
        { 0x01 },
        { 0x02 },
        { 0xFF },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_get_selflog_status_rsp::key_status] = testVector[i].status;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_SELFLOG_STATUS, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscGetSelflogStatusAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SELFLOG_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetSelflogStatus(), testVector[i].status);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSelflogStatusAdapterJson) {
    struct {
        int status;
    } testVector[] = {
        { 0x01 },
        { 0x02 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_selflog_status_ind::key_status] = testVector[i].status;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SELFLOG_STATUS, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSelflogStatusAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SELFLOG_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetIndSelflogStatus(), testVector[i].status);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetModemLogDumpAdapterJson) {
    struct {
        int result;
    } testVector[] = {
        { 0x00 }, // Success
        { 0x01 }, // Failure (On-processing)
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_set_modem_log_dump_rsp::key_result] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SET_MODEM_LOG_DUMP, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetModemLogDumpAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_MODEM_LOG_DUMP);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].result);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscCurrentLinkCapacityEstimateAdapterJson) {
    struct {
        unsigned int dlCapaKbps;
        unsigned int ulCapaKbps;
        unsigned int dcnrSecDlCapaKbps;
        unsigned int dcnrSecUlCapaKbps;
    } testVector[] = {
        { 123, 246, 234, 789},
        { UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX}, // Becasue CP can send the UINT_MAX
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_current_link_capacity_estimate_ind::key_dl_capa_kbps] = testVector[i].dlCapaKbps;
        jsonIndData[ns_misc_current_link_capacity_estimate_ind::key_ul_capa_kbps] = testVector[i].ulCapaKbps;
        jsonIndData[ns_misc_current_link_capacity_estimate_ind::key_dcnr_sec_dl_capa_kbps] = testVector[i].dcnrSecDlCapaKbps;
        jsonIndData[ns_misc_current_link_capacity_estimate_ind::key_dcnr_sec_ul_capa_kbps] = testVector[i].dcnrSecUlCapaKbps;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscCurrentLinkCapacityEstimateAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetDlCapaKbps(), (int)testVector[i].dlCapaKbps);
        EXPECT_EQ(testAdapter.GetUlCapaKbps(), (int)testVector[i].ulCapaKbps);
        EXPECT_EQ(testAdapter.GetSecDlCapaKbps(), (int)testVector[i].dcnrSecDlCapaKbps);
        EXPECT_EQ(testAdapter.GetSecUlCapaKbps(), (int)testVector[i].dcnrSecUlCapaKbps);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolRfConnectionStateAdapterJson) {
    struct {
        int rfCableState;
        int rilRfCableState;
    } testVector[] = {
        { 0x00, 0x00 },
        { 0x01, 0x01 },
        { 0x02, 0x01 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_rf_connection_ind::key_rf_cable_state] = testVector[i].rfCableState;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_RF_CONNECTION, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolRfConnectionStateAdapterJson testAdapter(modemData);

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_RF_CONNECTION);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetState(), testVector[i].rilRfCableState);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscRssiScanResultAdapterJson) {
    struct {
        int totalPage;
        int currentPage;
        int startFreq;
        int endFreq;
        int offset;
        int scanResultLen; // To control len of scanResult in indication
        int scanResult[MAX_RSSI_SCAN_RESULT + 1]; // Extra data to cover TC where CP sends data more than expected
    } testVector[] = {
        { 1, 200, 456, 456, 456, 1024, { [0 ... MAX_RSSI_SCAN_RESULT] = 5 } },
        { 1, 200, 456, 456, 456, MAX_RSSI_SCAN_RESULT + 1, { [0 ... MAX_RSSI_SCAN_RESULT] = 6 } },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_rssi_scan_ind::key_total_page] = testVector[i].totalPage;
        jsonIndData[ns_misc_rssi_scan_ind::key_current_page] = testVector[i].currentPage;
        jsonIndData[ns_misc_rssi_scan_ind::key_start_freq] = testVector[i].startFreq;
        jsonIndData[ns_misc_rssi_scan_ind::key_end_freq] = testVector[i].endFreq;
        jsonIndData[ns_misc_rssi_scan_ind::key_offset] = testVector[i].offset;
        for(int j = 0; j < testVector[i].scanResultLen; j++) {
            jsonIndData[ns_misc_rssi_scan_ind::key_scan_result].append(testVector[i].scanResult[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_RSSI_SCAN, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscRssiScanResultAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_RSSI_SCAN);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testAdapter.GetTotalPage(), testVector[i].totalPage);
        EXPECT_EQ(testAdapter.GetCurrentPage(), testVector[i].currentPage);
        EXPECT_EQ(testAdapter.GetStartFrequency(), testVector[i].startFreq);
        EXPECT_EQ(testAdapter.GetEndFrequency(), testVector[i].endFreq);
        EXPECT_EQ(testAdapter.GetStep(), testVector[i].offset);

        EXPECT_TRUE(testAdapter.GetScanResultSize() <= MAX_RSSI_SCAN_RESULT);
        INT16 *scanResult = testAdapter.GetScanResult();
        for(int j = 0; j < testAdapter.GetScanResultSize(); j++) {
            EXPECT_EQ(scanResult[j], testVector[i].scanResult[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolSignalStrengthAdapterJson) {
    GSM_SIGNAL_STRENGTH gsm_signal_strength[] = {
        { 99, 99, INT_MAX }, // Default values
        { 31, 7, 1111111 },
    };

    WCDMA_SIGNAL_STRENGTH wcdma_signal_strength[] = {
        { 99, 99, 255, 255 }, // Default values
        { 31, 7, 96, 49 },
    };

    CDMA_SIGNAL_STRENGTH cdma_signal_strength[] = {
        { -1, -1 }, // Default values
        { 11, 11 },
    };

    EVDO_SIGNAL_STRENGTH evdo_signal_strength[] = {
        { -1, -1, INT_MAX }, // Default values
        { 11, 11, 8 },
    };

    TD_SCDMA_SIGNAL_STRENGTH_V2 tdscdma_signal_strength[] = {
        { 99, 99, 255 }, // Default values
        { 31, 7, 96 },
    };

    LTE_SIGNAL_STRENGTH_V4 lte_signal_strength[] = {
        { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX }, // Default values
        { 31, 140, 20, 300, 6, 15, 1111 },
    };

    NR_SIGNAL_STRENGTH_V4 nr_signal_strength[] = {
        { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, { [0 ... SIT_MAX_NR_SUBBAND_INDEX-1] = 0 } }, // Default values
        { 140, 20, 40, 140, 20, 40, 3, 15, { [0 ... SIT_MAX_NR_SUBBAND_INDEX-1] = UINT_MAX-1 } }, // when wb_cqi != INT_MAX
        { 140, 20, 40, 140, 20, 40, 3, INT_MAX, { [0 ... SIT_MAX_NR_SUBBAND_INDEX-1] = UINT_MAX-1 } }, // when wb_cqi == INT_MAX
    };

    struct {
        int ratBitmask;
        GSM_SIGNAL_STRENGTH gsmSS;
        WCDMA_SIGNAL_STRENGTH wcdmaSS;
        LTE_SIGNAL_STRENGTH_V4 lteSS;
        CDMA_SIGNAL_STRENGTH cdmaSS;
        EVDO_SIGNAL_STRENGTH evdoSS;
        TD_SCDMA_SIGNAL_STRENGTH_V2 tdscdmaSS;
        NR_SIGNAL_STRENGTH_V4 nrSS;
    } tv[] = {
        { 0x0000, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0001, gsm_signal_strength[1], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0002, gsm_signal_strength[0], wcdma_signal_strength[1], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0004, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[1], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0008, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[1], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0010, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[1], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0020, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[1], nr_signal_strength[0] },
        { 0x0040, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[1] },
        { 0x0040, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[2] },
    };

    int size = sizeof(tv) / sizeof(tv[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_get_signal_strength_rsp::key_valid_ss_rat_bitmask] = tv[i].ratBitmask;

        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_GSM) {
            Json::Value jsonGsmData;
            FillGsmSignalStrengthTestData(jsonGsmData, tv[i].gsmSS);
            jsonRspData[ns_misc_get_signal_strength_rsp::key_gsm_signal_strength] = jsonGsmData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_WCDMA) {
            Json::Value jsonWcdmaData;
            FillWcdmaSignalStrengthTestData(jsonWcdmaData, tv[i].wcdmaSS);
            jsonRspData[ns_misc_get_signal_strength_rsp::key_wcdma_signal_strength] = jsonWcdmaData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_CDMA_V1_4) {
            Json::Value jsonCdmaData;
            FillCdmaSignalStrengthTestData(jsonCdmaData, tv[i].cdmaSS);
            jsonRspData[ns_misc_get_signal_strength_rsp::key_cdma_signal_strength] = jsonCdmaData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_EVDO_V1_4) {
            Json::Value jsonEvdoData;
            FillEvdoSignalStrengthTestData(jsonEvdoData, tv[i].evdoSS);
            jsonRspData[ns_misc_get_signal_strength_rsp::key_evdo_signal_strength] = jsonEvdoData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_LTE_V1_4) {
            Json::Value jsonLteData;
            FillLteSignalStrengthV4TestData(jsonLteData, tv[i].lteSS);
            jsonRspData[ns_misc_get_signal_strength_rsp::key_lte_signal_strength] = jsonLteData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4) {
            Json::Value jsonTdscdmaData;
            FillTdscdmaSignalStrengthV2TestData(jsonTdscdmaData, tv[i].tdscdmaSS);
            jsonRspData[ns_misc_get_signal_strength_rsp::key_tdscdma_signal_strength] = jsonTdscdmaData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_NR_V1_4) {
            Json::Value jsonNrData;
            FillNrSignalStrengthV4TestData(jsonNrData, tv[i].nrSS);
            jsonRspData[ns_misc_get_signal_strength_rsp::key_nr_signal_strength] = jsonNrData;
        }

        auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonRsp(
                    SIT_GET_SIGNAL_STRENGTH, 0, RCM_E_SUCCESS, jsonRspData));
        ASSERT_NE(modemData, nullptr);

        ProtocolSignalStrengthAdapterJson testAdapter(modemData.get());
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIGNAL_STRENGTH);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        RIL_SignalStrength_V1_6 &rilSS = testAdapter.GetSignalStrength(4 /*ssVer*/);

        EXPECT_EQ(CompareEvdoSignalStrength(rilSS.EVDO_SignalStrength, tv[i].evdoSS), 0);
        EXPECT_EQ(CompareGsmSignalStrength(rilSS.GSM_SignalStrength, tv[i].gsmSS), 0);
        EXPECT_EQ(CompareWcdmaSignalStrength(rilSS.WCDMA_SignalStrength, tv[i].wcdmaSS), 0);
        EXPECT_EQ(CompareCdmaSignalStrength(rilSS.CDMA_SignalStrength, tv[i].cdmaSS), 0);
        EXPECT_EQ(CompareTdscdmaSignalStrength(rilSS.TD_SCDMA_SignalStrength, tv[i].tdscdmaSS), 0);
        EXPECT_EQ(CompareLteSignalStrength(rilSS.LTE_SignalStrength, tv[i].lteSS), 0);
        EXPECT_EQ(CompareNrSignalStrength(rilSS.NR_SignalStrength, tv[i].nrSS), 0);
    }

}

TEST(ProtocolMiscAdapterJson, ProtocolSignalStrengthIndAdapterJson) {
    GSM_SIGNAL_STRENGTH gsm_signal_strength[] = {
        { 99, 99, INT_MAX }, // Default values
        { 31, 7, 1111111 },
    };

    WCDMA_SIGNAL_STRENGTH wcdma_signal_strength[] = {
        { 99, 99, 255, 255 }, // Default values
        { 31, 7, 96, 49 },
    };

    CDMA_SIGNAL_STRENGTH cdma_signal_strength[] = {
        { -1, -1 }, // Default values
        { 11, 11 },
    };

    EVDO_SIGNAL_STRENGTH evdo_signal_strength[] = {
        { -1, -1, INT_MAX }, // Default values
        { 11, 11, 8 },
    };

    TD_SCDMA_SIGNAL_STRENGTH_V2 tdscdma_signal_strength[] = {
        { 99, 99, 255 }, // Default values
        { 31, 7, 96 },
    };

    LTE_SIGNAL_STRENGTH_V4 lte_signal_strength[] = {
        { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX }, // Default values
        { 31, 140, 20, 300, 6, 15, 1111 },
    };

    NR_SIGNAL_STRENGTH_V4 nr_signal_strength[] = {
        { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, { [0 ... SIT_MAX_NR_SUBBAND_INDEX-1] = 0 } }, // Default values
        { 140, 20, 40, 140, 20, 40, 3, 15, { [0 ... SIT_MAX_NR_SUBBAND_INDEX-1] = UINT_MAX-1 } }, // when wb_cqi != INT_MAX
        { 140, 20, 40, 140, 20, 40, 3, INT_MAX, { [0 ... SIT_MAX_NR_SUBBAND_INDEX-1] = UINT_MAX-1 } }, // when wb_cqi == INT_MAX
    };

    struct {
        int ratBitmask;
        GSM_SIGNAL_STRENGTH gsmSS;
        WCDMA_SIGNAL_STRENGTH wcdmaSS;
        LTE_SIGNAL_STRENGTH_V4 lteSS;
        CDMA_SIGNAL_STRENGTH cdmaSS;
        EVDO_SIGNAL_STRENGTH evdoSS;
        TD_SCDMA_SIGNAL_STRENGTH_V2 tdscdmaSS;
        NR_SIGNAL_STRENGTH_V4 nrSS;
    } tv[] = {
        { 0x0000, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0001, gsm_signal_strength[1], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0002, gsm_signal_strength[0], wcdma_signal_strength[1], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0004, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[1], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0008, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[1], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0010, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[1], tdscdma_signal_strength[0], nr_signal_strength[0] },
        { 0x0020, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[1], nr_signal_strength[0] },
        { 0x0040, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[1] },
        { 0x0040, gsm_signal_strength[0], wcdma_signal_strength[0], lte_signal_strength[0], cdma_signal_strength[0], evdo_signal_strength[0], tdscdma_signal_strength[0], nr_signal_strength[2] },
    };

    int size = sizeof(tv) / sizeof(tv[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_signal_strength_ind::key_valid_ss_rat_bitmask] = tv[i].ratBitmask;


        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_GSM) {
            Json::Value jsonGsmData;
            FillGsmSignalStrengthTestData(jsonGsmData, tv[i].gsmSS);
            jsonIndData[ns_misc_signal_strength_ind::key_gsm_signal_strength] = jsonGsmData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_WCDMA) {
            Json::Value jsonWcdmaData;
            FillWcdmaSignalStrengthTestData(jsonWcdmaData, tv[i].wcdmaSS);
            jsonIndData[ns_misc_signal_strength_ind::key_wcdma_signal_strength] = jsonWcdmaData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_CDMA_V1_4) {
            Json::Value jsonCdmaData;
            FillCdmaSignalStrengthTestData(jsonCdmaData, tv[i].cdmaSS);
            jsonIndData[ns_misc_signal_strength_ind::key_cdma_signal_strength] = jsonCdmaData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_EVDO_V1_4) {
            Json::Value jsonEvdoData;
            FillEvdoSignalStrengthTestData(jsonEvdoData, tv[i].evdoSS);
            jsonIndData[ns_misc_signal_strength_ind::key_evdo_signal_strength] = jsonEvdoData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_LTE_V1_4) {
            Json::Value jsonLteData;
            FillLteSignalStrengthV4TestData(jsonLteData, tv[i].lteSS);
            jsonIndData[ns_misc_signal_strength_ind::key_lte_signal_strength] = jsonLteData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4) {
            Json::Value jsonTdscdmaData;
            FillTdscdmaSignalStrengthV2TestData(jsonTdscdmaData, tv[i].tdscdmaSS);
            jsonIndData[ns_misc_signal_strength_ind::key_tdscdma_signal_strength] = jsonTdscdmaData;
        }
        if (tv[i].ratBitmask & SIT_MISC_SIG_RAT_SIG_NR_V1_4) {
            Json::Value jsonNrData;
            FillNrSignalStrengthV4TestData(jsonNrData, tv[i].nrSS);
            jsonIndData[ns_misc_signal_strength_ind::key_nr_signal_strength] = jsonNrData;
        }

        auto modemData = std::unique_ptr<ModemData>(
                ModemDataBuilder::BuildJsonInd(SIT_IND_SIGNAL_STRENGTH, jsonIndData));
        ASSERT_NE(modemData, nullptr);

        ProtocolSignalStrengthIndAdapterJson testAdapter(modemData.get());
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SIGNAL_STRENGTH);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        RIL_SignalStrength_V1_6 &rilSS = testAdapter.GetSignalStrength(4 /*ssVer*/);

        EXPECT_EQ(CompareEvdoSignalStrength(rilSS.EVDO_SignalStrength, tv[i].evdoSS), 0);
        EXPECT_EQ(CompareGsmSignalStrength(rilSS.GSM_SignalStrength, tv[i].gsmSS), 0);
        EXPECT_EQ(CompareWcdmaSignalStrength(rilSS.WCDMA_SignalStrength, tv[i].wcdmaSS), 0);
        EXPECT_EQ(CompareCdmaSignalStrength(rilSS.CDMA_SignalStrength, tv[i].cdmaSS), 0);
        EXPECT_EQ(CompareTdscdmaSignalStrength(rilSS.TD_SCDMA_SignalStrength, tv[i].tdscdmaSS), 0);
        EXPECT_EQ(CompareLteSignalStrength(rilSS.LTE_SignalStrength, tv[i].lteSS), 0);
        EXPECT_EQ(CompareNrSignalStrength(rilSS.NR_SignalStrength, tv[i].nrSS), 0);
    }

}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetDeviceInfoAdapterJson) {
    struct {
        string sitVersionInfo;
    } testVector[] = {
        { "CellInfo=1,CellIdentity=1,SignalStrength=1" },
        { "CellInfo=2,CellIdentity=2,SignalStrength=2" },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_set_device_info_rsp::key_sit_version_info] = testVector[i].sitVersionInfo;


        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SET_DEVICE_INFO, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetDeviceInfoAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_DEVICE_INFO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        string key1 = STRING_CELL_INFO;
        EXPECT_EQ(i+1, testAdapter.GetVersionInfo(key1.c_str()));
        string key2 = STRING_CELL_IDENTITY;
        EXPECT_EQ(i+1, testAdapter.GetVersionInfo(key2.c_str()));
        string key3 = STRING_SIGNAL_STRENGTH;
        EXPECT_EQ(i+1, testAdapter.GetVersionInfo(key3.c_str()));

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscIMEIAdapterJson) {
    struct {
        const char* imei;
    } testVector[] = {
        { "867252020127265" },
        { "867252020127250" },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_id_get_imei_rsp::key_imei] = string(testVector[i].imei);

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
                SIT_GET_IMEI, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolMiscIMEIAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_IMEI);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_EQ((unsigned int) testAdapter.GetIMEILen(), strlen(testVector[i].imei));
        EXPECT_STREQ((char *) testAdapter.GetIMEI(), testVector[i].imei);


        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscIMEISVAdapterJson) {
    struct {
        const char* imeisv;
    } testVector[] = {
        { "86725202012726501" },
        { "86725202012725002" },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_id_get_imeisv_rsp::key_imeisv] = string(testVector[i].imeisv);

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
                SIT_GET_IMEISV, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolMiscIMEISVAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_IMEISV);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_EQ((unsigned int) testAdapter.GetIMEISVLen(), strlen(testVector[i].imeisv));
        EXPECT_STREQ((char *) testAdapter.GetIMEISV(), testVector[i].imeisv);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscDeviceIDAdapterJson) {
    struct {
        const char* imei;
        const char* imeisv;
        const char* esn;
        const char* meid;
    } testVector[] = {
        { "867252020127265", "86725202012726501", "12807859508", "12345678901234" },
        { "867252020127250", "86725202012725002", "12807859234", "12345678905678" },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_id_get_deviceid_rsp::key_imei] = string(testVector[i].imei);
        jsonRspData[ns_misc_id_get_deviceid_rsp::key_imeisv] = string(testVector[i].imeisv);
        jsonRspData[ns_misc_id_get_deviceid_rsp::key_esn] = string(testVector[i].esn);
        jsonRspData[ns_misc_id_get_deviceid_rsp::key_meid] = string(testVector[i].meid);

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
                SIT_GET_DEVICE_ID, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolMiscDeviceIDAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_DEVICE_ID);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_EQ((unsigned int) testAdapter.GetIMEILen(), strlen(testVector[i].imei));
        EXPECT_STREQ((char*)testAdapter.GetIMEI(), testVector[i].imei);

        EXPECT_EQ((unsigned int) testAdapter.GetIMEISVLen(), strlen(testVector[i].imeisv));
        EXPECT_STREQ((char*)testAdapter.GetIMEISV(), testVector[i].imeisv);

        EXPECT_EQ((unsigned int) testAdapter.GetESNLen(), strlen(testVector[i].esn));
        EXPECT_STREQ((char*)testAdapter.GetESN(), testVector[i].esn);

        EXPECT_EQ((unsigned int) testAdapter.GetMEIDLen(), strlen(testVector[i].meid));
        EXPECT_STREQ((char*)testAdapter.GetMEID(), testVector[i].meid);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscGetStackStatusAdapterJson) {
    struct {
        int status;
        int sit_status;
    } testVector[] = {
        { 0x00, SIT_PWR_STATCK_DISABLE },
        { 0x01, SIT_PWR_STATCK_ENABLE },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_pwr_get_stack_status_rsp::key_status] = testVector[i].status;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
                SIT_GET_STACK_STATUS, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolMiscGetStackStatusAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_STACK_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_EQ(testAdapter.GetMode(), testVector[i].sit_status);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscPhoneResetAdapterJson) {
    struct {
        int reset_type;
    } testVector[] = {
        { SIT_PWR_RESET_TYPE_PHONE_ONLY },
        { SIT_PWR_RESET_TYPE_RESET_BOTH },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_pwr_phone_reset_ind::key_reset_type] = testVector[i].reset_type;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd( SIT_IND_PHONE_RESET, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolMiscPhoneResetAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_PHONE_RESET);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        // Payload Check
        EXPECT_EQ(testAdapter.GetResetType(), testVector[i].reset_type);
        EXPECT_EQ(testAdapter.GetResetCause(), 0);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetApSystemTimeAdapterJson) {
    struct {
        int result;
    } testVector[] = {
        { 0 },
        { 1 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_misc_set_ap_system_time_rsp::key_result] = testVector[i].result;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_SET_AP_SYSTEM_TIME, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolMiscSetApSystemTimeAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_AP_SYSTEM_TIME);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        //Payload data checks
        EXPECT_EQ(testVector[i].result, testAdapter.GetResult());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscGetCdmaCapabilityInfoAdapterJson) {
    int testVector[] = {0, 1};

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_get_cdma_capability_info_rsp::key_result] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
                SIT_OEM_GET_CDMA_CAPABILITY_INFO, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolMiscGetCdmaCapabilityInfoAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_CDMA_CAPABILITY_INFO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_EQ(testAdapter.GetCapability(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscSetAheadRreAdapterJson) {
    int testVector[] = {0, 1};

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_oem_set_ahead_rre_rsp::key_result] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
                SIT_OEM_SET_AHEAD_RRE, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolMiscSetAheadRreAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_AHEAD_RRE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_EQ(testAdapter.GetResult(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscAdapterJson, ProtocolMiscDataStateChangeAdapterJson) {
    struct {
        int expectedState;
    } testVector[] = {
        { 0 },
        { 1 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_misc_ps_data_state_change_ind::key_expected_state] = testVector[i].expectedState;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd( SIT_IND_DATA_STATE_CHANGE, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolMiscDataStateChangeAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_DATA_STATE_CHANGE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        //Payload data checks
        EXPECT_EQ(testVector[i].expectedState, testAdapter.GetExpectedState());

        delete modemData;
        modemData = nullptr;
    }
}

