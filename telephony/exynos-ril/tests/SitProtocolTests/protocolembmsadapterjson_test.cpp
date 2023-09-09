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

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "json/protocolembmsadapterjson.h"
#include "embmsjsondef.h"

TEST(ProtocolEmbmsAdapterJson, ProtocolEmbmsCoverageAdapterJson) {
    struct {
        int coverage;
    } testVector[] = {
        { EMBMS_NO_COVERAGE },
        { EMBMS_UNICAST_COVERAGE },
        { EMBMS_FULL_COVERAGE },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_embms_coverage_ind::key_coverage] = testVector[i].coverage;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd( SIT_IND_EMBMS_COVERAGE, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolEmbmsCoverageAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_EMBMS_COVERAGE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        // Payload Check
        EXPECT_EQ(testAdapter.GetCoverage(), testVector[i].coverage);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolEmbmsAdapterJson, ProtocolEmbmsSessionListAdapterJson) {
    struct {
        int state;
        int oos_reason;
        int tmgi_list_len;
        int tmgi_list[EMBMS_MAX_INTRA_SAILIST_NUMBER * EMBMS_TMGI_LEN];
    } testVector[] = {
        { 0x00, 0x00, 12, { 23, 112, 76, 77, 69, 11, 77, 88, 103, 112, 4, 15 } },
        { 0x01, 0x01, 6, { 1, 31, 113, 251, 9, 34 } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_embms_session_list_rsp::key_state] = testVector[i].state;
        jsonRspData[ns_embms_session_list_rsp::key_oos_reason] = testVector[i].oos_reason;

        for (int j = 0; j < testVector[i].tmgi_list_len; j++) {
            jsonRspData[ns_embms_session_list_rsp::key_tmgi_list].append(testVector[i].tmgi_list[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
                SIT_GET_EMBMS_SESSION_LIST, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolEmbmsSessionListAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_EMBMS_SESSION_LIST);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        // Payload Check
        EXPECT_EQ(testAdapter.GetState(), testVector[i].state);
        EXPECT_EQ(testAdapter.GetOosReason(), testVector[i].oos_reason);
        EXPECT_EQ(testAdapter.GetRecordNum(), testVector[i].tmgi_list_len/EMBMS_TMGI_LEN);

        const BYTE *tmgiList = testAdapter.GetTMGI();
        for (int j = 0; j < testVector[i].tmgi_list_len; j++) {
            EXPECT_EQ(tmgiList[j], testVector[i].tmgi_list[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolEmbmsAdapterJson, ProtocolEmbmsSessionListIndAdapterJson) {
    struct {
        int state;
        int oos_reason;
        int tmgi_list_len;
        int tmgi_list[EMBMS_MAX_INTRA_SAILIST_NUMBER * EMBMS_TMGI_LEN];
    } testVector[] = {
        { 0x00, 0x00, 12, { 23, 112, 76, 77, 69, 11, 77, 88, 103, 112, 4, 15 } },
        { 0x01, 0x01, 6, { 1, 31, 113, 251, 9, 34 } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_embms_session_list_rsp::key_state] = testVector[i].state;
        jsonIndData[ns_embms_session_list_rsp::key_oos_reason] = testVector[i].oos_reason;

        for (int j = 0; j < testVector[i].tmgi_list_len; j++) {
            jsonIndData[ns_embms_session_list_rsp::key_tmgi_list].append(testVector[i].tmgi_list[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_EMBMS_SESSION_LIST, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolEmbmsSessionListIndAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_EMBMS_SESSION_LIST);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        // Payload Check
        EXPECT_EQ(testAdapter.GetState(), testVector[i].state);
        EXPECT_EQ(testAdapter.GetOosReason(), testVector[i].oos_reason);
        EXPECT_EQ(testAdapter.GetRecordNum(), testVector[i].tmgi_list_len/EMBMS_TMGI_LEN);

        const BYTE *tmgiList = testAdapter.GetTMGI();
        for (int j = 0; j < testVector[i].tmgi_list_len; j++) {
            EXPECT_EQ(tmgiList[j], testVector[i].tmgi_list[j]);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolEmbmsAdapterJson, ProtocolEmbmsNetworkTimeAdapterJson) {
    struct {
        int sib16_acquired;
        int time_info_type;
        int daylight_valid;
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
        int time_zone;
        int daylight_adjust;
        int day_of_week;
        int plmn[6];
        uint64_t absolute_time;
        int leap_second;
    } testVector[] = {
        { 1, 2, 1, 21, 6, 8, 12, 30, 22, 12, 1, 3, { 52, 54, 48, 48, 48, 49 },
            68719476858, 71 },
        { 0, 2, 1, 21, 10, 26, 15, 3, 10, 12, 0, 1, { 52, 48, 52, 52, 53, 102 },
            68719476858, 68 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_embms_network_time_rsp::key_sib16_acquired] = testVector[i].sib16_acquired;
        jsonRspData[ns_embms_network_time_rsp::key_time_info_type] = testVector[i].time_info_type;
        jsonRspData[ns_embms_network_time_rsp::key_daylight_valid] = testVector[i].daylight_valid;
        jsonRspData[ns_embms_network_time_rsp::key_year] = testVector[i].year;
        jsonRspData[ns_embms_network_time_rsp::key_month] = testVector[i].month;
        jsonRspData[ns_embms_network_time_rsp::key_day] = testVector[i].day;
        jsonRspData[ns_embms_network_time_rsp::key_hour] = testVector[i].hour;
        jsonRspData[ns_embms_network_time_rsp::key_minute] = testVector[i].minute;
        jsonRspData[ns_embms_network_time_rsp::key_second] = testVector[i].second;
        jsonRspData[ns_embms_network_time_rsp::key_time_zone] = testVector[i].time_zone;
        jsonRspData[ns_embms_network_time_rsp::key_daylight_adjust] = testVector[i].daylight_adjust;
        jsonRspData[ns_embms_network_time_rsp::key_day_of_week] = testVector[i].day_of_week;
        for (int j = 0; j < 6; j++) {
            jsonRspData[ns_embms_network_time_rsp::key_plmn].append(testVector[i].plmn[j]);
        }
        jsonRspData[ns_embms_network_time_rsp::key_absolute_time] = testVector[i].absolute_time;
        jsonRspData[ns_embms_network_time_rsp::key_leap_second] = testVector[i].leap_second;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
                SIT_GET_EMBMS_NETWORK_TIME, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolEmbmsNetworkTimeAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_EMBMS_NETWORK_TIME);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        // Payload Check
        EXPECT_EQ(testAdapter.DayLightValid(), testVector[i].daylight_valid);
        EXPECT_EQ(testAdapter.Year(), testVector[i].year);
        EXPECT_EQ(testAdapter.Month(), testVector[i].month);
        EXPECT_EQ(testAdapter.Day(), testVector[i].day);
        EXPECT_EQ(testAdapter.Hour(), testVector[i].hour);
        EXPECT_EQ(testAdapter.Minute(), testVector[i].minute);
        EXPECT_EQ(testAdapter.Second(), testVector[i].second);
        EXPECT_EQ(testAdapter.TimeZone(), testVector[i].time_zone);
        EXPECT_EQ(testAdapter.DayLightAdjust(), testVector[i].daylight_adjust);
        EXPECT_EQ(testAdapter.DayofWeek(), testVector[i].day_of_week);
        EXPECT_EQ(testAdapter.TimeInfoType(), 0);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolEmbmsAdapterJson, ProtocolEmbmsNetworkTimeIndAdapterJson) {
    struct {
        int sib16_acquired;
        int time_info_type;
        int daylight_valid;
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
        int time_zone;
        int daylight_adjust;
        int day_of_week;
        int plmn[6];
        uint64_t absolute_time;
        int leap_second;
    } testVector[] = {
        { 1, 2, 1, 21, 6, 8, 12, 30, 22, 12, 1, 3, { 52, 54, 48, 48, 48, 49 },
            68719476858, 71 },
        { 0, 2, 1, 21, 10, 26, 15, 3, 10, 12, 0, 1, { 52, 48, 52, 52, 53, 102 },
            68719476858, 68 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_embms_network_time_ind::key_sib16_acquired] = testVector[i].sib16_acquired;
        jsonIndData[ns_embms_network_time_ind::key_time_info_type] = testVector[i].time_info_type;
        jsonIndData[ns_embms_network_time_ind::key_daylight_valid] = testVector[i].daylight_valid;
        jsonIndData[ns_embms_network_time_ind::key_year] = testVector[i].year;
        jsonIndData[ns_embms_network_time_ind::key_month] = testVector[i].month;
        jsonIndData[ns_embms_network_time_ind::key_day] = testVector[i].day;
        jsonIndData[ns_embms_network_time_ind::key_hour] = testVector[i].hour;
        jsonIndData[ns_embms_network_time_ind::key_minute] = testVector[i].minute;
        jsonIndData[ns_embms_network_time_ind::key_second] = testVector[i].second;
        jsonIndData[ns_embms_network_time_ind::key_time_zone] = testVector[i].time_zone;
        jsonIndData[ns_embms_network_time_ind::key_daylight_adjust] = testVector[i].daylight_adjust;
        jsonIndData[ns_embms_network_time_ind::key_day_of_week] = testVector[i].day_of_week;
        for (int j = 0; j < 6; j++) {
            jsonIndData[ns_embms_network_time_ind::key_plmn].append(testVector[i].plmn[j]);
        }
        jsonIndData[ns_embms_network_time_ind::key_absolute_time] = testVector[i].absolute_time;
        jsonIndData[ns_embms_network_time_ind::key_leap_second] = testVector[i].leap_second;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_EMBMS_NETWORK_TIME, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolEmbmsNetworkTimeIndAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_EMBMS_NETWORK_TIME);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        // Payload Check
        EXPECT_EQ(testAdapter.DayLightValid(), testVector[i].daylight_valid);
        EXPECT_EQ(testAdapter.Year(), testVector[i].year);
        EXPECT_EQ(testAdapter.Month(), testVector[i].month);
        EXPECT_EQ(testAdapter.Day(), testVector[i].day);
        EXPECT_EQ(testAdapter.Hour(), testVector[i].hour);
        EXPECT_EQ(testAdapter.Minute(), testVector[i].minute);
        EXPECT_EQ(testAdapter.Second(), testVector[i].second);
        EXPECT_EQ(testAdapter.TimeZone(), testVector[i].time_zone);
        EXPECT_EQ(testAdapter.DayLightAdjust(), testVector[i].daylight_adjust);
        EXPECT_EQ(testAdapter.DayofWeek(), testVector[i].day_of_week);
        EXPECT_EQ(testAdapter.TimeInfoType(), 0);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolEmbmsAdapterJson, ProtocolEmbmsSignalStrengthAdapterJson) {
    struct {
        UINT32 snr;
        int mbsfn_area_id;
        UINT32 esnr;
        int tmgi_list[12];
    } testVector[] = {
        { 200, 50, 134, { 41, 35, 190, 132, 225, 108, 214, 174, 82, 144, 73, 241 } },
        { UINT_MAX, 51, UINT_MAX, { 241, 187, 233, 235, 179, 166, 219, 60, 135, 12, 62, 153 } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    Json::Value jsonRspData;
    Json::Value jsonSignalRecord;
    for (int i = 0; i < size; i++) {
        jsonSignalRecord[i][ns_embms_signal_record::key_snr] = testVector[i].snr;
        jsonSignalRecord[i][ns_embms_signal_record::key_mbsfn_area_id] = testVector[i].mbsfn_area_id;
        jsonSignalRecord[i][ns_embms_signal_record::key_esnr] = testVector[i].esnr;
        for (int j = 0; j < 12; j++) {
            jsonSignalRecord[i][ns_embms_signal_record::key_tmgi_list].append(testVector[i].tmgi_list[j]);
        }
    }
    jsonRspData[ns_embms_signal_strength_rsp::key_signal_record_list] = jsonSignalRecord;

    ModemData *modemData = ModemDataBuilder::BuildJsonRsp(
            SIT_GET_EMBMS_SIGNAL_STRENGTH, 0 /*token*/, RCM_E_SUCCESS, jsonRspData);
    ASSERT_NE(modemData, nullptr);

    // General IPC checks
    ProtocolEmbmsSignalStrengthAdapterJson testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_EMBMS_SIGNAL_STRENGTH);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    // Payload Check
    EXPECT_EQ(testAdapter.GetCount(), size);
    const UINT32* snrList = testAdapter.GetSnrList();
    for (int i = 0; i < size; i++) {
        EXPECT_EQ(snrList[i], testVector[i].snr);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolEmbmsAdapterJson, ProtocolEmbmsSignalStrengthIndAdapterJson) {
    struct {
        UINT32 snr;
        int mbsfn_area_id;
        UINT32 esnr;
        int tmgi_list[12];
    } testVector[] = {
        { 200, 50, 134, { 41, 35, 190, 132, 225, 108, 214, 174, 82, 144, 73, 241 } },
        { UINT_MAX, 51, UINT_MAX, { 241, 187, 233, 235, 179, 166, 219, 60, 135, 12, 62, 153 } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    Json::Value jsonIndData;
    Json::Value jsonSignalRecord;
    for (int i = 0; i < size; i++) {
        jsonSignalRecord[i][ns_embms_signal_record::key_snr] = testVector[i].snr;
        jsonSignalRecord[i][ns_embms_signal_record::key_mbsfn_area_id] = testVector[i].mbsfn_area_id;
        jsonSignalRecord[i][ns_embms_signal_record::key_esnr] = testVector[i].esnr;
        for (int j = 0; j < 12; j++) {
            jsonSignalRecord[i][ns_embms_signal_record::key_tmgi_list].append(testVector[i].tmgi_list[j]);
        }
    }
    jsonIndData[ns_embms_signal_strength_ind::key_signal_record_list] = jsonSignalRecord;

    ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_EMBMS_SIGNAL_STRENGTH, jsonIndData);
    ASSERT_NE(modemData, nullptr);

    // General IPC checks
    ProtocolEmbmsSignalStrengthIndAdapterJson testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_EMBMS_SIGNAL_STRENGTH);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);

    // Payload Check
    EXPECT_EQ(testAdapter.GetCount(), size);
    const UINT32* snrList = testAdapter.GetSnrList();
    for (int i = 0; i < size; i++) {
        EXPECT_EQ(snrList[i], testVector[i].snr);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolEmbmsAdapterJson, ProtocolEmbmsSaiListAdapterJson) {
    struct {
        unsigned int frequency;
        int inter_sai_len;
        int inter_sai_info_list[MAX_INTER_SAI_NUMBER];
        int multiband_info_len;
        int multiband_info[MAX_MULTI_BAND_NUMBER];
    } testVector[] = {
        { 100, 2, { 24187, 23235 }, 2, { 41, 35 } },
        { UINT_MAX, 2, { 14286, 23235 }, 2, { 141, 55 } },
    };

    int intra_sai_list[2] = { 4135, 1932 };
    int size = sizeof(testVector)/sizeof(testVector[0]);

    Json::Value jsonIndData;
    Json::Value jsonInterSaiList;
    for (int i = 0; i < 2; i++) {
        jsonIndData[ns_embms_sailist_ind::key_intra_sai_list].append(intra_sai_list[i]);
    }
    for (int i = 0; i < size; i++) {
        jsonInterSaiList[i][ns_embms_inter_sai::key_frequency] = testVector[i].frequency;
        for (int j = 0; j < testVector[i].inter_sai_len; j++) {
            jsonInterSaiList[i][ns_embms_inter_sai::key_inter_sai_info_list].append(testVector[i].inter_sai_info_list[j]);
        }
        for (int j = 0; j < testVector[i].multiband_info_len; j++) {
            jsonInterSaiList[i][ns_embms_inter_sai::key_multiband_info].append(testVector[i].multiband_info[j]);
        }
    }
    jsonIndData[ns_embms_sailist_ind:: key_inter_sai_list] = jsonInterSaiList;

    ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_EMBMS_SAI_LIST, jsonIndData);
    ASSERT_NE(modemData, nullptr);

    // General IPC checks
    ProtocolEmbmsSaiListAdapterJson testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_EMBMS_SAI_LIST);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);

    // Payload Check
    EXPECT_EQ(testAdapter.GetIntraSaiListLen(), 2);
    EXPECT_EQ(testAdapter.GetInterSaiListLen(), size);
    const UINT16 *intraSaiList = testAdapter.GetIntraSaiList();
    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(intraSaiList[i], intra_sai_list[i]);
    }
    const EMBMS_InterSaiList *interSaiList = testAdapter.GetInterSaiList();
    for (int i = 0; i < size; i++) {
        EXPECT_EQ(interSaiList[i].Frequency, testVector[i].frequency);
        EXPECT_EQ(interSaiList[i].InterSaiNumber, testVector[i].inter_sai_len);
        for (int j = 0; j < interSaiList[i].InterSaiNumber; j++) {
            EXPECT_EQ(interSaiList[i].InterSaiInfo[j], testVector[i].inter_sai_info_list[j]);
        }
        EXPECT_EQ(interSaiList[i].MultiBandInfoNumber, testVector[i].multiband_info_len);
        for (int j = 0; j < interSaiList[i].MultiBandInfoNumber; j++) {
            EXPECT_EQ(interSaiList[i].MultiBandInfo[j], testVector[i].multiband_info[j]);
        }
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolEmbmsAdapterJson, ProtocolEmbmsGlobalCellIdAdapterJson) {
    struct {
        char mcc[MAX_MCC_LEN + 1];
        char mnc[MAX_MNC_LEN + 1];
        unsigned int cell_id;
    } testVector[] = {
        { "460", "45", 68 },
        { "310", "160", UINT_MAX},
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_embms_global_cellid_ind::key_mcc] = string(testVector[i].mcc);
        jsonIndData[ns_embms_global_cellid_ind::key_mnc] = string(testVector[i].mnc);
        jsonIndData[ns_embms_global_cellid_ind::key_cell_id] = testVector[i].cell_id;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_EMBMS_GLOBAL_CELL_ID, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolEmbmsGlobalCellIdAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_EMBMS_GLOBAL_CELL_ID);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        // Payload Check
        EXPECT_STREQ(testAdapter.GetMcc(), testVector[i].mcc);
        EXPECT_STREQ(testAdapter.GetMnc(), testVector[i].mnc);
        EXPECT_EQ(testAdapter.GetCellId(), testVector[i].cell_id);

        delete modemData;
        modemData = nullptr;
    }
}
