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

#include <librilutils/textutils.h>

#include "json/misc/protocolmiscbuilderjson.h"
#include "protocoljsontestadapter.h"
#include "miscjsondef.h"
#include "rilproperty.h"
#include "oemjsondef.h"
#include "sitdef.h"
#include "rildef.h"
#include "modemconfiguration.h"
#include "modemdata.h"
#include "tokengen.h"

#include <android/log.h>

#include <iostream>

using namespace std;

TEST(ProtocolDtmfBuilderJson, BuildDtmf) {
    TokenGen::Init();

    ProtocolDtmfBuilderJson testBuilder;
    char dtmfDigits[] = "*123#";
    ModemData *modemData = testBuilder.BuildDtmf(5 /*dtmfDigits length*/, dtmfDigits);
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_DTMF);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) { delete modemData; modemData = nullptr; }
    ASSERT_NE(false, parsed);

    string cpDtmfString = jsonReqData[ns_misc_call_dtmf_req::key_dtmf_digits].asString();
    const char *cpDtmfDigits = cpDtmfString.c_str();
    EXPECT_EQ(strcmp(cpDtmfDigits, dtmfDigits), 0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolDtmfBuilderJson, BuildDtmfStart) {
    TokenGen::Init();

    struct {
        bool rilToneType;
        int rilDtmfDigit;
        int cpToneType;
        int cpDtmfDigit;
    } testVector[] = {
        { false, '0', SIT_CALL_LOCAL_DTMF_OFF, 48},
        { true, '2', SIT_CALL_LOCAL_DTMF_ON, 50},
        { false, '*', SIT_CALL_LOCAL_DTMF_OFF, 42},
        { true, '#', SIT_CALL_LOCAL_DTMF_ON, 35},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolDtmfBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildDtmfStart(testVector[i].rilToneType, testVector[i].rilDtmfDigit);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_DTMF_START);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpToneType, cpToneDuration, cpDtmfDigit;
        cpToneType = jsonReqData[ns_misc_call_dtmf_start_req::key_tone_type].asInt();
        cpToneDuration = jsonReqData[ns_misc_call_dtmf_start_req::key_tone_duration].asInt();
        cpDtmfDigit = jsonReqData[ns_misc_call_dtmf_start_req::key_dtmf_digit].asInt();

        EXPECT_EQ(cpToneType, testVector[i].cpToneType);
        EXPECT_EQ(cpToneDuration, SIT_CALL_DTMF_TONE_DURATION_SHORT);
        EXPECT_EQ(cpDtmfDigit, testVector[i].cpDtmfDigit);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolDtmfBuilderJson, BuildDtmfStop) {
    TokenGen::Init();

    ProtocolDtmfBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildDtmfStop();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_DTMF_STOP);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolDeviceInfoBuilderJson, GetBaseBandVersion) {
    TokenGen::Init();

    ProtocolDeviceInfoBuilderJson testBuilder;
    int bitmask = 0xFF;
    ModemData *modemData = testBuilder.GetBaseBandVersion(bitmask);
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BASEBAND_VERSION);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) { delete modemData; modemData = nullptr; }
    ASSERT_NE(false, parsed);

    int cpBitMask = jsonReqData[ns_misc_get_baseband_version_req::key_ver_mask].asInt();
    EXPECT_EQ(cpBitMask, 0xFF);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolDeviceInfoBuilderJson, SetScreenState) {
    TokenGen::Init();

    ProtocolDeviceInfoBuilderJson testBuilder;
    int screenState = SIT_MISC_SCREEN_STATE_OFF;
    ModemData *modemData = testBuilder.SetScreenState(screenState);
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_SCREEN_STATE);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) { delete modemData; modemData = nullptr; }
    ASSERT_NE(false, parsed);

    int cpScreenState = jsonReqData[ns_misc_set_screen_state_req::key_screen_state].asInt();
    EXPECT_EQ(cpScreenState, SIT_MISC_SCREEN_STATE_OFF);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolDeviceInfoBuilderJson, BuildSetFeatureInfo) {
    TokenGen::Init();

    struct {
        bool isWlanApAssist;
        int cpFeature;
    } testVector[] = {
        { true, SIT_FEATURE_IWLAN_AP_ASSIST },
        { false, 0 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolDeviceInfoBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        RilProperty bundle;
        bundle.Put("iwlan_ap_assist", testVector[i].isWlanApAssist);
        ModemData *modemData = testBuilder.BuildSetFeatureInfo(bundle);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_FEATURE_INFO);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int bitMask = jsonReqData[ns_misc_set_feature_info_req::key_feature_bitmask].asInt();
        EXPECT_EQ(bitMask, testVector[i].cpFeature);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~

}

TEST(ProtocolDeviceInfoBuilderJson, SetPinControl) {
    TokenGen::Init();

    struct {
        BYTE signal;
        BYTE status;
    } testVector[] = {
        { 0x00 /*NONE*/, 0x00 /*off*/},
        { 0x01 /*DCD*/, 0x01 /*on*/},
        { 0x02 /*DTR*/, 0x01 /*on*/},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolDeviceInfoBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SetPinControl(testVector[i].signal, testVector[i].status);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_PIN_CONTROL);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpPinSignal;
        int cpPinStatus;
        cpPinSignal  = jsonReqData[ns_misc_set_pin_control_req::key_pin_signal].asInt();
        cpPinStatus = jsonReqData[ns_misc_set_pin_control_req::key_pin_status].asInt();

        EXPECT_EQ(cpPinSignal, testVector[i].signal);
        EXPECT_EQ(cpPinStatus, testVector[i].status);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolDeviceInfoBuilderJson, BuildSetUnsolicitedResponseFilter) {
    TokenGen::Init();

    struct {
        unsigned int bitmask;
    } testVector[] = {
        { RIL_INDFILTER_NONE },
        { RIL_INDFILTER_SIGNAL_STRENGTH },
        { RIL_INDFILTER_FULL_NETWORK_STATE },
        { RIL_INDFILTER_DATA_CALL_DORMANCY_CHANGED },
        { RIL_INDFILTER_SIGNAL_STRENGTH | RIL_INDFILTER_FULL_NETWORK_STATE | RIL_INDFILTER_DATA_CALL_DORMANCY_CHANGED },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolDeviceInfoBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetUnsolicitedResponseFilter(testVector[i].bitmask);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_IND_CMD_FILTER);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        unsigned int cpBitmask;
        cpBitmask = jsonReqData[ns_misc_set_ind_cmd_filter_req::key_filter_bitmask].asUInt();

        EXPECT_EQ(cpBitmask, testVector[i].bitmask);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolDeviceInfoBuilderJson, BuildSetSignalReportCriteria) {
    TokenGen::Init();

    struct {
        unsigned int ms;
        unsigned int db;
        int dbm[10+2]; // 2 extra for TC where API received more than 10
        int ran;
        int cpRan;
        int measureType;
        int enable;
    } testVector[] = {
        { 123, 456, { [0 ... 11] = 5 }, ACCESS_NETWORK_GERAN,    SIT_NET_ACCESS_RADIO_TYPE_GERAN,           0x01, 0x00 },
        { 123, 456, { [0 ... 11] = 5 }, ACCESS_NETWORK_UTRAN,    SIT_NET_ACCESS_RADIO_TYPE_UTRAN,           0x02, 0x01 },
        { 123, 456, { [0 ... 11] = 5 }, ACCESS_NETWORK_EUTRAN,   SIT_NET_ACCESS_RADIO_TYPE_EUTRAN,          0x03, 0x00 },
        { 123, 456, { [0 ... 11] = 5 }, ACCESS_NETWORK_NGRAN,    SIT_NET_ACCESS_RADIO_TYPE_NGRAN,           0x04, 0x01 },
        { 123, 456, { [0 ... 11] = 5 }, ACCESS_NETWORK_CDMA2000, SIT_NET_ACCESS_RADIO_TYPE_CDMA2000,        0x05, 0x00 },
        { UINT_MAX, UINT_MAX, { [0 ... 11] = 5 }, ACCESS_NETWORK_IWLAN,    SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN,  0x08, 0x01 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolDeviceInfoBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetSignalReportCriteria(testVector[i].ms, testVector[i].db,
                12, testVector[i].dbm, testVector[i].ran, testVector[i].measureType, testVector[i].enable);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        unsigned int ms, db;
        int ran, measureType, enable;
        ms = jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_h_ms].asUInt();
        EXPECT_EQ(ms, testVector[i].ms);

        db = jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_h_db].asUInt();
        EXPECT_EQ(db, testVector[i].db);

        ran = jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_ran].asInt();
        EXPECT_EQ(ran, testVector[i].cpRan);

        measureType = jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_measure_type].asInt();
        EXPECT_EQ(measureType, testVector[i].measureType);

        enable = jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_enable].asInt();
        EXPECT_EQ(enable, testVector[i].enable);

        int dbmLength = jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_t_dbm].size();
        EXPECT_TRUE(dbmLength <= 10);

        for(int j = 0; j < dbmLength; j++) {
            int dbm = jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_t_dbm][j].asInt();
            EXPECT_EQ(dbm, testVector[i].dbm[j]);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolDeviceInfoBuilderJson, BuildSetLinkCapaReportCriteria) {
    TokenGen::Init();

    struct {
        int hMs;
        int hDlKbps;
        int hUlKbps;
        unsigned int tDlKbps[20+2]; // 2 extra for TC where API received more than 20
        unsigned int tUlKbps[20+2]; // 2 extra for TC where API received more than 20
        int ran;
        int cpRan;
    } testVector[] = {
        { 123, 456, 458, { [0 ... 11] = 5 }, { [0 ... 11] = 6 }, ACCESS_NETWORK_GERAN,    SIT_NET_ACCESS_RADIO_TYPE_GERAN },
        { 123, 456, 458, { [0 ... 11] = 5 }, { [0 ... 11] = 6 }, ACCESS_NETWORK_UTRAN,    SIT_NET_ACCESS_RADIO_TYPE_UTRAN },
        { 123, 456, 458, { [0 ... 11] = 5 }, { [0 ... 11] = 6 }, ACCESS_NETWORK_EUTRAN,   SIT_NET_ACCESS_RADIO_TYPE_EUTRAN },
        { 123, 456, 458, { [0 ... 11] = 5 }, { [0 ... 11] = 6 }, ACCESS_NETWORK_NGRAN,    SIT_NET_ACCESS_RADIO_TYPE_NGRAN },
        { 123, 456, 458, { [0 ... 11] = 5 }, { [0 ... 11] = 6 }, ACCESS_NETWORK_CDMA2000, SIT_NET_ACCESS_RADIO_TYPE_CDMA2000 },
        { 123, 456, 458, { [0 ... 11] = 5 }, { [0 ... 11] = 6 }, ACCESS_NETWORK_IWLAN,    SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN },
        { 123, 456, 458, { [0 ... 11] = UINT_MAX }, { [0 ... 11] = UINT_MAX }, ACCESS_NETWORK_IWLAN,    SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolDeviceInfoBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetLinkCapaReportCriteria(testVector[i].hMs, testVector[i].hDlKbps, testVector[i].hUlKbps,
                22, (int *)testVector[i].tDlKbps, 22, (int *)testVector[i].tUlKbps, testVector[i].ran);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_LINK_CAPACITY_REPORTING_CRITERIA);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int hMs, hDlKbps, hUlKbps, ran;
        hMs = jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_h_ms].asInt();
        EXPECT_EQ(hMs, testVector[i].hMs);

        hDlKbps = jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_h_dl_kbps].asInt();
        EXPECT_EQ(hDlKbps, testVector[i].hDlKbps);

        hUlKbps = jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_h_ul_kbps].asInt();
        EXPECT_EQ(hUlKbps, testVector[i].hUlKbps);

        ran = jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_ran].asInt();
        EXPECT_EQ(ran, testVector[i].cpRan);

        int tDlLength = jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_t_dl_kbps].size();
        EXPECT_TRUE(tDlLength <= 20);

        for(int j = 0; j < tDlLength; j++) {
            unsigned int kbps = jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_t_dl_kbps][j].asUInt();
            EXPECT_EQ(kbps, testVector[i].tDlKbps[j]);
        }

        int tUlLength = jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_t_ul_kbps].size();
        EXPECT_TRUE(tUlLength <= 20);

        for(int j = 0; j < tUlLength; j++) {
            unsigned int kbps = jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_t_ul_kbps][j].asUInt();
            EXPECT_EQ(kbps, testVector[i].tUlKbps[j]);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolDeviceInfoBuilderJson, GetSignalStrength) {
    TokenGen::Init();

    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.GetSignalStrength();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_SIGNAL_STRENGTH);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, SendDeviceInfo) {
    struct {
        string modelName;
        string swVer;
        string productName;
        string osVer;
    } testVector[] = {
        { "modelName_xx", "sw_ver_xx", "product_name_xx", "os_Ver_xx" },
        { string(MAX_MODEL_NAME_LEN + 2, 'M'),      string(MAX_BB_SW_VER_LEN + 2, 'S'),
          string(MAX_BB_PRODUCT_CODE_LEN + 2, 'P'), string(MAX_AP_OS_VERSION_LEN + 2, 'O')}, // TC where data is more than expected
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolDeviceInfoBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SendDeviceInfo(testVector[i].modelName.c_str(), testVector[i].swVer.c_str(),
                testVector[i].productName.c_str(), testVector[i].osVer.c_str());
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_DEVICE_INFO);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        string modelName, swVer, productName, osVer;
        modelName = jsonReqData[ns_misc_set_device_info_req::key_ap_model_name].asString();
        swVer = jsonReqData[ns_misc_set_device_info_req::key_ap_sw_version].asString();
        productName = jsonReqData[ns_misc_set_device_info_req::key_ap_product_name].asString();
        osVer = jsonReqData[ns_misc_set_device_info_req::key_ap_os_version].asString();

        EXPECT_TRUE(modelName.length() <= MAX_MODEL_NAME_LEN);
        EXPECT_EQ(strncmp(testVector[i].modelName.c_str(), modelName.c_str(), MAX_MODEL_NAME_LEN), 0);

        EXPECT_TRUE(swVer.length() <= MAX_BB_SW_VER_LEN);
        EXPECT_EQ(strncmp(testVector[i].swVer.c_str(), swVer.c_str(), MAX_BB_SW_VER_LEN), 0);

        EXPECT_TRUE(productName.length() <= MAX_BB_PRODUCT_CODE_LEN);
        EXPECT_EQ(strncmp(testVector[i].productName.c_str(), productName.c_str(), MAX_BB_PRODUCT_CODE_LEN), 0);

        EXPECT_TRUE(osVer.length() <= MAX_AP_OS_VERSION_LEN);
        EXPECT_EQ(strncmp(testVector[i].osVer.c_str(), osVer.c_str(), MAX_AP_OS_VERSION_LEN), 0);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetTtyMode) {
    TokenGen::Init();

    struct {
        int ttyMode;
    } testVector[] = {
        { SIT_MISC_TTY_MODE_OFF },
        { SIT_MISC_TTY_MODE_FULL },
        { SIT_MISC_TTY_MODE_HCO },
        { SIT_MISC_TTY_MODE_VCO },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SetTtyMode(testVector[i].ttyMode);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_TTY_MODE);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpTtyMode;
        cpTtyMode = jsonReqData[ns_misc_set_tty_mode_req::key_tty_mode].asInt();

        EXPECT_EQ(cpTtyMode, testVector[i].ttyMode);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, GetTtyMode) {
    TokenGen::Init();

    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.GetTtyMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_TTY_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscBuilderJson, BuildSetElevatorSensor) {
    TokenGen::Init();

    struct {
        int status;
    } testVector[] = {
        { 0x00 }, // Enter elevator
        { 0x01 }, // Exit elevator
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetElevatorSensor(testVector[i].status);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_ELEVATOR_SENSOR);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpStatus;
        cpStatus = jsonReqData[ns_misc_set_elevator_sensor_req::key_sensor_status].asInt();

        EXPECT_EQ(cpStatus, testVector[i].status);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetDataThrottling) {
    TokenGen::Init();

    struct {
        int throttlingAction;
        long completionDuration;
    } testVector[] = {
        { 0x00, 0xFFFF },
        { 0x01, INT_MAX },
        { 0x02, LONG_MAX },
        { 0x03, 0 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetDataThrottling(testVector[i].throttlingAction, testVector[i].completionDuration);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_DATA_THROTTLING);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int throttlingAction;
        throttlingAction = jsonReqData[ns_misc_set_data_throttling_req::key_throttling_action].asInt();
        EXPECT_EQ(throttlingAction, testVector[i].throttlingAction);

        long completionDuration;
        completionDuration = jsonReqData[ns_misc_set_data_throttling_req::key_completion_duration_millis].asUInt64();
        EXPECT_EQ(completionDuration, testVector[i].completionDuration);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, SetLocationUpdates) {
    TokenGen::Init();

    struct {
        int enable;
        bool isModemDataNull;
    } testVector[] = {
        { 0, false },
        { 1, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetLocationUpdates(testVector[i].enable);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_LOCATION_UPDATE_SETTING);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = testAdapter.CheckParsing(reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }
        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolMiscBuilderJson, GetModemActivityInfo) {
    TokenGen::Init();

    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.GetModemActivityInfo();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_ACTIVITY_INFO);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscBuilderJson, SetRfDesenseMode) {
    TokenGen::Init();

    const int vaildDataLen = sizeof(sit_rf_desense_info_type);    // sizeof(sit_rf_desense_info_type)

    // sit_rf_desense_info_type
    sit_rf_desense_info_type data[4];
    memset(data, 0, vaildDataLen*4);

    data[0].desense_operation = 1; data[0].rat = SIT_RF_DESENSE_RAT_GSM;
    data[0].power_level[0] = 0; data[0].power_level[1] = 1;
    data[0].arfcn[0] = 1; data[0].arfcn[2] = 2;
    data[0].tsc[0] = 3;

    data[1].desense_operation = 1; data[1].rat = SIT_RF_DESENSE_RAT_WCDMA;
    data[1].power_level[0] = 0; data[1].power_level[1] = 2;
    data[1].arfcn[0] = 2; data[1].arfcn[2] = 2;

    data[2].desense_operation = 1; data[2].rat = SIT_RF_DESENSE_RAT_LTE;
    data[2].power_level[0] = 0; data[2].power_level[1] = 3;
    data[2].arfcn[0] = 3; data[2].arfcn[2] = 2;
    data[2].mcs = 1;

    data[3].desense_operation = 1; data[3].rat = SIT_RF_DESENSE_RAT_CDMA;
    data[3].power_level[0] = 0; data[3].power_level[1] = 4;
    data[3].arfcn[0] = 4; data[3].arfcn[2] = 2;
    data[3].cdma_modulation = 1;

    struct {
        sit_rf_desense_info_type *inputData;
        int length;
        bool isModemDataNull;
    } testVector[] = {
        { &data[0], 10, true },
        { &data[0], vaildDataLen, false },
        { &data[1], vaildDataLen, false },
        { &data[2], vaildDataLen, false },
        { &data[3], vaildDataLen, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolSensorBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SetRfDesenseMode((BYTE *)(testVector[i].inputData), testVector[i].length);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_RF_DESENSE_MODE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = testAdapter.CheckParsing(reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }
        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolMiscBuilderJson, GetRfDesenseMode) {
    TokenGen::Init();

    ProtocolSensorBuilderJson testBuilder;

    ModemData *modemData = testBuilder.GetRfDesenseMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_RF_DESENSE_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolMiscBuilderJson, SetManualBandMode) {
    TokenGen::Init();

    const int vaildDataLen = 1 + 32 + 8 + 1 + 2;
    BYTE data[2][vaildDataLen] = {
            {0, 1, 2, 3, 4, 5, 6, 7, 8, },
            {1, 4, 3, 2, 1, 4, 3, 2, 1, },
        };
    data[0][vaildDataLen-1] = 3;
    data[1][vaildDataLen-1] = 4;

    struct {
        BYTE *byteArrary;
        int length;
        bool isModemDataNull;
    } testVector[] = {
        { data[0], 10, true },
        { data[0], vaildDataLen, false },
        { data[1], vaildDataLen, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscNetworkBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SetManualBandMode(testVector[i].byteArrary, testVector[i].length);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_MANUAL_BAND_MODE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = testAdapter.CheckParsing(reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }
        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolMiscBuilderJson, GetManualBandMode) {
    TokenGen::Init();

    ProtocolMiscNetworkBuilderJson testBuilder;

    ModemData *modemData = testBuilder.GetManualBandMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_MANUAL_BAND_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolMiscBuilderJson, BuildSetLabTestConfig) {
    TokenGen::Init();

    struct {
        int configStatus;
    } testVector[] = {
        { 0x00 }, // Disables lab test mode
        { 0x01 }, // Enables lab test mode
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetLabTestConfig(0x01, testVector[i].configStatus);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_LAB_TEST_CONFIG);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int mode, status;
        mode = jsonReqData[ns_misc_set_lab_test_config_req::key_config_mode].asInt();
        status = jsonReqData[ns_misc_set_lab_test_config_req::key_config_status].asInt();

        EXPECT_EQ(mode, 0x01); // Value is always fixed
        EXPECT_EQ(status,testVector[i].configStatus);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildRadioConfigReset) {
    TokenGen::Init();

    struct {
        int resetType;
    } testVector[] = {
        { SIT_MISC_CFG_RESET_FACTORY },
        { SIT_MISC_CFG_RESET_NETWORK },
        { SIT_MISC_CFG_RESET_MODEM },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildRadioConfigReset(testVector[i].resetType);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_CFG_DEFAULT);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpResetType;
        cpResetType = jsonReqData[ns_misc_set_cfg_default_req::key_reset_type].asInt();

        EXPECT_EQ(cpResetType, testVector[i].resetType);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscNetworkBuilderJson, BuildCdmaSubscription) {
    TokenGen::Init();

    ProtocolMiscNetworkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildCdmaSubscription();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CDMA_SUBSCRIPTION);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscNetworkBuilderJson, BuildSetVoiceOperation) {
    TokenGen::Init();

    struct {
        unsigned int rilMode;
        unsigned int cpMode;
    } testVector[] = {
        { 0, 0 },
        { 1, 3 },
        { 2, 3 },
        { 3, 3 },
        { UINT_MAX, 3 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscNetworkBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetVoiceOperation(testVector[i].rilMode);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_VOICE_OPERATION);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        unsigned int mode;
        mode = jsonReqData[ns_misc_set_voice_operation_req::key_mode].asUInt();

        EXPECT_EQ(mode, testVector[i].cpMode);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscNetworkBuilderJson, BuildGetVoiceOperation) {
    TokenGen::Init();

    ProtocolMiscNetworkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetVoiceOperation();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_VOICE_OPERATION);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscNetworkBuilderJson, BuildSetPreferredCallCapability) {
    TokenGen::Init();

    struct {
        int mode;
    } testVector[] = {
        { 0x00 }, // cs only
        { 0x01 }, // ps only
        { 0x02 }, // cs pref and ps sec
        { 0x03 }, // ps pref and cs sec
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscNetworkBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetPreferredCallCapability(testVector[i].mode);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_PREFERRED_CALL_CAPABILITY);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int mode;
        mode = jsonReqData[ns_misc_set_preferred_call_capability_req::key_mode].asInt();

        EXPECT_EQ(mode, testVector[i].mode);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolSensorBuilderJson, BuildPSensorStatus) {
    TokenGen::Init();

    struct {
        unsigned int bitmask;
    } testVector[] = {
        { 0x00 },
        { 0x01 },
        { 0x02 },
        { 0x04 },
        { 0x08 },
        { 0x10 },
        { 0x20 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolSensorBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildPSensorStatus(testVector[i].bitmask);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_SENSOR_STATUS);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        unsigned int cpBitmask;
        cpBitmask = jsonReqData[ns_misc_set_sensor_status_req::key_status_bitmask].asUInt();

        EXPECT_EQ(cpBitmask, testVector[i].bitmask);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolSensorBuilderJson, BuildScanRssi) {
    TokenGen::Init();

    struct TX_INFO {
        int status;
        int band; // Only valid for 3G/LTE/LTE_CA/TDSCDMA/NR
        int bw;
        int freq;
        int power;
        int rbNum;
        int rbOffset;
        int mcs;
    } txInfo[] = {
        { 0x00, -1,               0x01, FREQUENCY_MIN, 100, 50, 50, 28 }, // when rat is anything other than 3G/TDSCDMA/LTE/LTE_CA/NR
        { 0x01, BAND_3G_MAX,      0x01, FREQUENCY_MAX, 100, 50, 50, 28 }, // When rat is 3G
        { 0x01, BAND_TDSCDMA_MAX, 0x01, FREQUENCY_MAX, 100, 50, 50, 28 }, // When rat is TDSCDMA
        { 0x01, BAND_LTE_MAX,     0x01, FREQUENCY_MAX, 100, 50, 50, 28 }, // When rat is LTE
        { 0x01, BAND_LTE_CA_MAX,  0x01, FREQUENCY_MAX, 100, 50, 50, 28 }, // When rat is LTE_CA
        { 0x01, BAND_NR_MAX,      0x01, FREQUENCY_MAX, 100, 50, 50, 28 }, // When rat is NR
    };

    struct RSSI_SCAN {
        int rat;
        int rxBand;
        int rbw;
        int scanMode;
        int startFreq; // Valid when scan mode is partial
        int endFreq;   // Valid when scan mode is partial
        int offset;
        int antennaSelection;
        int samplingCount;
        TX_INFO tx1;
        TX_INFO tx2;
    } tv[] = {
         { SCAN_RAT_2G,       BAND_2G_MAX,      0x00,   SCAN_ALL,     FREQUENCY_INVALID, FREQUENCY_INVALID, 0x01, ANTENNA_ALL,         0x0001, txInfo[0], txInfo[0] },
         { SCAN_RAT_3G,       BAND_3G_MAX,      0x01,   SCAN_PARTIAL, FREQUENCY_MIN,     FREQUENCY_MAX,     0x01, ANTENNA_MAIN_ONLY,   0x0001, txInfo[1], txInfo[1] },
         { SCAN_RAT_LTE,      BAND_LTE_MAX,     0x02,   SCAN_PARTIAL, FREQUENCY_MIN,     FREQUENCY_MAX,     0x01, ANTENNA_SUB_ONLY,    0x0001, txInfo[3], txInfo[3] },
         { SCAN_RAT_LTE_CA,   BAND_LTE_CA_MAX,  0x03,   SCAN_PARTIAL, FREQUENCY_MIN,     FREQUENCY_MAX,     0x01, ANTENNA_ALL_PRESET,  0x0001, txInfo[4], txInfo[4] },
         { SCAN_RAT_CDMA,     BAND_CDMA_MAX,    0x04,   SCAN_PARTIAL, FREQUENCY_MIN,     FREQUENCY_MAX,     0x01, ANTENNA_MAIN_PRESET, 0x0001, txInfo[0], txInfo[0] },
         { SCAN_RAT_TDSCDMA,  BAND_TDSCDMA_MAX, 0x05,   SCAN_PARTIAL, FREQUENCY_MIN,     FREQUENCY_MAX,     0x01, ANTENNA_SUB_PRESET,  0x0001, txInfo[2], txInfo[2] },
         { SCAN_RAT_NR,       BAND_NR_MAX,      0x00,   SCAN_PARTIAL, FREQUENCY_MIN,     FREQUENCY_MAX,     0x01, ANTENNA_ALL,         0x0001, txInfo[5], txInfo[5] },
    };
    int size = sizeof(tv) / sizeof(tv[0]);

    ProtocolSensorBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildScanRssi(tv[i].rat, tv[i].rxBand, tv[i].rbw, tv[i].scanMode, tv[i].startFreq, tv[i].endFreq,
                tv[i].offset, tv[i].antennaSelection, tv[i].samplingCount,
                tv[i].tx1.status, tv[i].tx1.band, tv[i].tx1.bw, tv[i].tx1.freq, tv[i].tx1.power, tv[i].tx1.rbNum, tv[i].tx1.rbOffset, tv[i].tx1.mcs,
                tv[i].tx2.status, tv[i].tx2.band, tv[i].tx2.bw, tv[i].tx2.freq, tv[i].tx2.power, tv[i].tx2.rbNum, tv[i].tx2.rbOffset, tv[i].tx2.mcs);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_RSSI_SCAN);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);


        RSSI_SCAN cpData;
        cpData.rat = jsonReqData[ns_misc_set_rssi_scan_req::key_rat].asInt();
        EXPECT_EQ(cpData.rat, tv[i].rat);

        cpData.rxBand = jsonReqData[ns_misc_set_rssi_scan_req::key_rx_band].asInt();
        EXPECT_EQ(cpData.rxBand, tv[i].rxBand);

        cpData.rbw = jsonReqData[ns_misc_set_rssi_scan_req::key_rbw].asInt();
        EXPECT_EQ(cpData.rbw, tv[i].rbw);

        cpData.scanMode = jsonReqData[ns_misc_set_rssi_scan_req::key_scan_mode].asInt();
        EXPECT_EQ(cpData.scanMode, tv[i].scanMode);

        if(cpData.scanMode == SCAN_PARTIAL) {
            cpData.startFreq = jsonReqData[ns_misc_set_rssi_scan_req::key_start_freq].asInt();
            EXPECT_EQ(cpData.startFreq, tv[i].startFreq);

            cpData.endFreq = jsonReqData[ns_misc_set_rssi_scan_req::key_end_freq].asInt();
            EXPECT_EQ(cpData.endFreq, tv[i].endFreq);
        } else {
            EXPECT_EQ(jsonReqData.isMember(ns_misc_set_rssi_scan_req::key_start_freq), false);
            EXPECT_EQ(jsonReqData.isMember(ns_misc_set_rssi_scan_req::key_end_freq), false);
        }

        cpData.antennaSelection = jsonReqData[ns_misc_set_rssi_scan_req::key_antenna_selection].asInt();
        EXPECT_EQ(cpData.antennaSelection, tv[i].antennaSelection);

        cpData.samplingCount = jsonReqData[ns_misc_set_rssi_scan_req::key_sampling_count].asInt();
        EXPECT_EQ(cpData.samplingCount, tv[i].samplingCount);

        Json::Value jsonTx1 = jsonReqData[ns_misc_set_rssi_scan_req::key_tx1_info];
        Json::Value jsonTx2 = jsonReqData[ns_misc_set_rssi_scan_req::key_tx2_info];

        cpData.tx1.status = jsonTx1[ns_misc_tx_info::key_status].asInt();
        cpData.tx2.status = jsonTx2[ns_misc_tx_info::key_status].asInt();
        EXPECT_EQ(cpData.tx1.status, tv[i].tx1.status);
        EXPECT_EQ(cpData.tx2.status, tv[i].tx2.status);

        if(cpData.rat == SCAN_RAT_3G || cpData.rat == SCAN_RAT_LTE || cpData.rat == SCAN_RAT_LTE_CA || cpData.rat == SCAN_RAT_TDSCDMA || cpData.rat == SCAN_RAT_NR) {
            cpData.tx1.band = jsonTx1[ns_misc_tx_info::key_band].asInt();
            cpData.tx2.band = jsonTx2[ns_misc_tx_info::key_band].asInt();
            EXPECT_EQ(cpData.tx1.band, tv[i].tx1.band);
            EXPECT_EQ(cpData.tx2.band, tv[i].tx2.band);
        } else {
            EXPECT_EQ(jsonTx1.isMember(ns_misc_tx_info::key_band), false);
            EXPECT_EQ(jsonTx2.isMember(ns_misc_tx_info::key_band), false);
        }

        cpData.tx1.bw = jsonTx1[ns_misc_tx_info::key_bw].asInt();
        cpData.tx2.bw = jsonTx2[ns_misc_tx_info::key_bw].asInt();
        EXPECT_EQ(cpData.tx1.bw, tv[i].tx1.bw);
        EXPECT_EQ(cpData.tx2.bw, tv[i].tx2.bw);

        cpData.tx1.freq = jsonTx1[ns_misc_tx_info::key_freq].asInt();
        cpData.tx2.freq = jsonTx2[ns_misc_tx_info::key_freq].asInt();
        EXPECT_EQ(cpData.tx1.freq, tv[i].tx1.freq);
        EXPECT_EQ(cpData.tx2.freq, tv[i].tx2.freq);

        cpData.tx1.power = jsonTx1[ns_misc_tx_info::key_power].asInt();
        cpData.tx2.power = jsonTx2[ns_misc_tx_info::key_power].asInt();
        EXPECT_EQ(cpData.tx1.power, tv[i].tx1.power);
        EXPECT_EQ(cpData.tx2.power, tv[i].tx2.power);

        cpData.tx1.rbNum = jsonTx1[ns_misc_tx_info::key_rb_num].asInt();
        cpData.tx2.rbNum = jsonTx2[ns_misc_tx_info::key_rb_num].asInt();
        EXPECT_EQ(cpData.tx1.rbNum, tv[i].tx1.rbNum);
        EXPECT_EQ(cpData.tx2.rbNum, tv[i].tx2.rbNum);

        cpData.tx1.rbOffset = jsonTx1[ns_misc_tx_info::key_rb_offset].asInt();
        cpData.tx2.rbOffset = jsonTx2[ns_misc_tx_info::key_rb_offset].asInt();
        EXPECT_EQ(cpData.tx1.rbOffset, tv[i].tx1.rbOffset);
        EXPECT_EQ(cpData.tx2.rbOffset, tv[i].tx2.rbOffset);

        cpData.tx1.mcs = jsonTx1[ns_misc_tx_info::key_mcs].asInt();
        cpData.tx2.mcs = jsonTx2[ns_misc_tx_info::key_mcs].asInt();
        EXPECT_EQ(cpData.tx1.mcs, tv[i].tx1.mcs);
        EXPECT_EQ(cpData.tx2.mcs, tv[i].tx2.mcs);

        EXPECT_EQ(jsonReqData[ns_misc_set_rssi_scan_req::key_rsvd_info_list].size(), 0U);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolOemOmaDmBuilderJson, GetMslCode) {
    TokenGen::Init();

    ProtocolOemOmaDmBuilderJson testBuilder;
    ModemData *modemData = testBuilder.GetMslCode();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_MSL_CODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}


TEST(ProtocolMiscDebugBuilderJson, SetEngMode_without_submode) {
    TokenGen::Init();

    struct {
        int mode;
    } testVector[] = {
        { 0x00 },
        { 0x01 },
        //{ 0x02 }, // If mode is 0x02 then submode is also required
        { 0xFF },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscDebugBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SetEngMode(testVector[i].mode);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_ENG_MODE);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpEngMode;
        cpEngMode = jsonReqData[ns_misc_set_eng_mode_req::key_mode].asInt();

        EXPECT_EQ(cpEngMode, testVector[i].mode);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscDebugBuilderJson, SetEngMode_with_submode) {
    TokenGen::Init();

    struct {
        int mode;
        int subMode;
    } testVector[] = {
        { 0x02, 0x00 }, // If mode is 0x02 then only submode is required
        { 0x02, 0x01 }, // If mode is 0x02 then only submode is required
        { 0x02, 0x02 }, // If mode is 0x02 then only submode is required
        { 0x02, 0x03 }, // If mode is 0x02 then only submode is required
        { 0x02, 0x04 }, // If mode is 0x02 then only submode is required
        { 0x02, 0x05 }, // If mode is 0x02 then only submode is required
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscDebugBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SetEngMode(testVector[i].mode, testVector[i].subMode);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_ENG_MODE);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpEngMode;
        int cpEngSubMode;
        cpEngMode = jsonReqData[ns_misc_set_eng_mode_req::key_mode].asInt();
        cpEngSubMode = jsonReqData[ns_misc_set_eng_mode_req::key_sub_mode].asInt();

        EXPECT_EQ(cpEngMode, testVector[i].mode);
        EXPECT_EQ(cpEngSubMode, testVector[i].subMode);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscDebugBuilderJson, SetScrLine) {
    TokenGen::Init();

    struct {
        int lineNo;
    } testVector[] = {
        { 0x00 },
        { 0x01 },
        { 0x31 },
        { 0x32 },
        { 0xFF }, // It means back to parent menu
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscDebugBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SetScrLine(testVector[i].lineNo);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_SCREEN_LINE);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpLineNo;
        cpLineNo = jsonReqData[ns_misc_set_screen_line_req::key_screen_line].asInt();

        EXPECT_EQ(cpLineNo, testVector[i].lineNo);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscDebugBuilderJson, SetDebugTrace) {
    TokenGen::Init();

    struct {
        int value;
    } testVector[] = {
        { 0 }, // Disable
        { 1 }, // Enable
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscDebugBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SetDebugTrace(testVector[i].value);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_DEBUG_TRACE);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpValue;
        cpValue = jsonReqData[ns_misc_set_debug_trace_req::key_debug_trace].asInt();

        EXPECT_EQ(cpValue, testVector[i].value);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscDebugBuilderJson, SetEngStringInput) {
    TokenGen::Init();

    struct {
        char input[255+1] = {0}; // Max length allowed 255
    } testVector[] = {
        { "yesIAmAString" },
        { { [0 ... 254] = 'a', [255] = 0 } },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscDebugBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.SetEngStringInput(strlen(testVector[i].input), testVector[i].input);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_ENG_STRING_INPUT);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        const char *cpInput;
        cpInput = jsonReqData[ns_misc_set_eng_string_input_req::key_string].asCString();

        EXPECT_EQ(strcmp(cpInput, testVector[i].input), 0);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscDebugBuilderJson, BuildATCommand) {
    TokenGen::Init();

    struct {
        string command;
    } testVector[] = {
        { "yesIAmAString" },
        { string(MAX_SIT_AT_COMMAND_LENGTH + 2, 'S') }, // TC where data is more than expected
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscDebugBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildATCommand(testVector[i].command.c_str());
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_FORWARDING_AT_COMMAND);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        const char *cpData;
        cpData = jsonReqData[ns_misc_set_forwarding_at_command_req::key_data].asCString();

        EXPECT_TRUE(strlen(cpData) <= MAX_SIT_AT_COMMAND_LENGTH);
        EXPECT_EQ(strncmp(cpData, testVector[i].command.c_str(), MAX_SIT_AT_COMMAND_LENGTH), 0);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscDebugBuilderJson, BuildSetSelflog) {
    TokenGen::Init();

    struct {
        int mode;
        int size;
    } testVector[] = {
        { 0 /*Start self logging*/, 1 },
        { 1 /*Stop self logging*/, 32 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscDebugBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetSelflog(testVector[i].mode, testVector[i].size);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_SELFLOG);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int cpMode;
        int cpBufSize;
        cpMode = jsonReqData[ns_misc_set_selflog_req::key_mode].asInt();
        cpBufSize = jsonReqData[ns_misc_set_selflog_req::key_buf_size].asInt();

        EXPECT_EQ(cpMode, testVector[i].mode);
        EXPECT_EQ(cpBufSize, testVector[i].size);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscDebugBuilderJson, BuildGetSelflogStatus) {
    TokenGen::Init();

    ProtocolMiscDebugBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetSelflogStatus();
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_SELFLOG_STATUS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscDebugBuilderJson, BuildSetModemLogDump) {
    TokenGen::Init();

    ProtocolMiscDebugBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetModemLogDump();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_MODEM_LOG_DUMP);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);

    Json::Value jsonReqData;
    bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) { delete modemData; modemData = nullptr; }
    ASSERT_NE(false, parsed);

    int cpType;
    cpType = jsonReqData[ns_misc_set_modem_log_dump_req::key_type].asInt();

    EXPECT_EQ(cpType, 0x01);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolDeviceInfoBuilderJson, GetIMEI) {
    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.GetIMEI();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_IMEI);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, GetIMEISV) {
    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.GetIMEISV();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_IMEISV);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, GetDevID) {
    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.GetDevID();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_DEVICE_ID);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, SendSGCValue) {
    TokenGen::Init();

    struct {
        int targetOperator;
        int sitMsgId;
    } testVector[] = {
        { TARGET_OPER_CHNOPEN, SIT_ID_SGC_SP_CHNOPEN },
        { TARGET_OPER_MOV_AR, SIT_ID_SGC_MOV_AR },
        { TARGET_OPER_CLARO_HN, SIT_ID_SGC_CLARO_HN },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolDeviceInfoBuilderJson testBuilder;
        ModemData *modemData = testBuilder.SendSGCValue(testVector[i].targetOperator, 0, 0);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_SGC);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].sitMsgId, jsonReqData[ns_misc_id_set_sgc_req::key_sgc].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscNetworkBuilderJson, BuildSetStatckStatus) {
    TokenGen::Init();

    int testVector[] = { SIT_PWR_STATCK_DISABLE, SIT_PWR_STATCK_ENABLE };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {

        ProtocolMiscNetworkBuilderJson testBuilder;
        ModemData *modemData = testBuilder.BuildSetStatckStatus(testVector[i]);

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_STACK_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i], jsonReqData[ns_misc_pwr_set_stack_status_req::key_status].asInt());
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscNetworkBuilderJson, BuildGetStatckStatus) {
    TokenGen::Init();

    ProtocolMiscNetworkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetStatckStatus();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_STACK_STATUS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscDebugBuilderJson, BuildSetApSystemTime) {
    TokenGen::Init();

    ProtocolMiscDebugBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetApSystemTime();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_AP_SYSTEM_TIME);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);

    Json::Value jsonReqData;
    bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    if(!parsed) { delete modemData; modemData = nullptr; }
    ASSERT_NE(false, parsed);

    // TODO: System time related parameters

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscBuilderJson, BuildModemInfo) {
    TokenGen::Init();

    struct {
        int type;
        char* data;
        int size;
    } testVector[] = {
        { 14,(char *)"123456",6},
        { 15,(char *)"1",1 },
        { 16,(char *)"sss",4},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildModemInfo(testVector[i].type, testVector[i].data, testVector[i].size);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_NW_INFO);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int type = jsonReqData[ns_oem_nw_info_req::key_command_id].asInt();
        string data_str = jsonReqData[ns_oem_nw_info_req::key_buffer].asString();
        const char *data = data_str.c_str();

        EXPECT_EQ(type, testVector[i].type);
        EXPECT_EQ(strncmp(data, testVector[i].data, data_str.length()), 0);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSwitchModemFunction) {
    TokenGen::Init();

    struct {
        int feature;
        UINT8 enable;
    } testVector[] = {
        { 1,0x1},
        { 2,0x0 },
        { 5,0x1},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSwitchModemFunction(testVector[i].feature, testVector[i].enable);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_FUNC_SWITCH_REQ);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int feature = jsonReqData[ns_oem_set_fun_swtich_req::key_feature].asInt();
        int enable = jsonReqData[ns_oem_set_fun_swtich_req::key_enable].asInt();

        EXPECT_EQ(feature, testVector[i].feature);
        EXPECT_EQ(enable, testVector[i].enable);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetPdcpDiscardTimer) {
    TokenGen::Init();

    struct {
        int timer;
    } testVector[] = {
        {1},
        {0xFF},
        {0xFFFF}
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetPdcpDiscardTimer(testVector[i].timer);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_PDCP_DISCARD_TIMER);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int timer = jsonReqData[ns_oem_set_pdcp_discard_timer_req::key_discard_timer].asInt();

        EXPECT_EQ(timer, testVector[i].timer);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetActivateVsim) {
    TokenGen::Init();

    struct {
        int slot;
        int iccid_len;
        char * iccId;
        int imsi_len;
        char * imsi;
        char * Hplmn;
        int vsimState;
        int vsimCardType;
    } testVector[] = {
        {1, 10, (char *)"1234567890", 10, (char *)"9874561230", (char *)"46001", 0, 4},
        {1, 9, (char *)"123456789", 8, (char *)"98745612", (char *)"460010", 0, 4},
        {1, 21, (char *)"123456789021212112123213132", 16, (char *)"f1231231231231231312312", (char *)"46011", 0, 3},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetActivateVsim(testVector[i].slot, testVector[i].iccid_len ,testVector[i].iccId,
            testVector[i].imsi_len, testVector[i].imsi, testVector[i].Hplmn,
            testVector[i].vsimState, testVector[i].vsimCardType);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_ACTIVATE_VSIM);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int slot = jsonReqData[ns_oem_set_activate_factory_vsim_req::key_sim_slot].asInt();
        string iccid_str = jsonReqData[ns_oem_set_activate_factory_vsim_req::key_iccid].asString();
        string imsi_str = jsonReqData[ns_oem_set_activate_factory_vsim_req::key_imsi].asString();
        string hplmn_str = jsonReqData[ns_oem_set_activate_factory_vsim_req::key_hplmn].asString();
        int vsimState = jsonReqData[ns_oem_set_activate_factory_vsim_req::key_vsim_state].asInt();
        int cardType = jsonReqData[ns_oem_set_activate_factory_vsim_req::key_vsim_card_type].asInt();

        const char * iccid = iccid_str.c_str();
        const char * imsi = imsi_str.c_str();
        const char * hplmn = hplmn_str.c_str();

        EXPECT_EQ(slot, testVector[i].slot);
        EXPECT_EQ(strncmp(iccid, testVector[i].iccId, strlen(iccid)), 0);
        EXPECT_LE(strlen(iccid), (unsigned long)MAX_ICCID_STRING_LEN);
        EXPECT_EQ(strncmp(imsi, testVector[i].imsi, strlen(imsi)), 0);
        EXPECT_LE(strlen(imsi), (unsigned long)MAX_IMSI_LEN);
        if(strlen(testVector[i].Hplmn) == 5)
        {
            EXPECT_EQ(hplmn[5], '#');
            EXPECT_EQ(strncmp(hplmn, testVector[i].Hplmn, 5), 0);
        }
        else
        {
            EXPECT_STREQ(hplmn, testVector[i].Hplmn);
        }
        EXPECT_EQ(vsimState, testVector[i].vsimState);
        EXPECT_EQ(cardType, testVector[i].vsimCardType);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildGetCqiInfo) {
    TokenGen::Init();

    ProtocolMiscBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildGetCqiInfo());
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_CQI_INFO);
    EXPECT_EQ(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
}

TEST(ProtocolMiscBuilderJson, BuildSetSarSetting) {
    TokenGen::Init();

    struct {
        int dsi;
    } testVector[] = {
        {1},
        {0xFF},
        {0xFFFF}
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetSarSetting(testVector[i].dsi);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_SAR_SETTING);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int dsi = jsonReqData[ns_oem_set_sar_setting_req::key_dsi].asInt();

        EXPECT_EQ(dsi, testVector[i].dsi);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetGmoSwitch) {
    TokenGen::Init();

    struct {
        int feature;
    } testVector[] = {
        {1},
        {0xFF},
        {0xFFFF}
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetGmoSwitch(testVector[i].feature);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_GMO_SWITCH);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int feature = jsonReqData[ns_oem_set_gmo_switch_req::key_feature].asInt();

        EXPECT_EQ(feature, testVector[i].feature);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetTcsFci) {
    TokenGen::Init();

    struct {
        int state;
        char * fci;
    } testVector[] = {
        {1 , NULL},
        {1 , (char *)"GCF01"},
        {0 , (char *)"CHN01"}
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetTcsFci(testVector[i].state, 0, testVector[i].fci);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_TCS_FCI_REQ);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int state = jsonReqData[ns_oem_set_tcs_fci_req::key_state].asInt();
        string fci_str = jsonReqData[ns_oem_set_tcs_fci_req::key_fci].asString();

        EXPECT_EQ(state, testVector[i].state);

        if(testVector[i].fci == NULL)
        {
            EXPECT_STREQ(fci_str.c_str(), "");
        }
        else
        {
            EXPECT_STREQ(fci_str.c_str(), testVector[i].fci);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildGetTcsFci) {
    TokenGen::Init();

    ProtocolMiscBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildGetTcsFci());
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_TCS_FCI_INFO);
    EXPECT_EQ(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
}

TEST(ProtocolMiscBuilderJson, BuildSetForbidLteCell) {
    struct {
        int mode;
        int cellId;
        int forbTimer;
        char * plmn;
        bool isModemDataNull;
    } testVector[] = {
        {1, 2, 0xFF, (char *)"46001", false},
        {0, 5, 0xFFF, (char *)"460020", false},
        {1, 23, 0xFFE, (char *)"460123", false},
        {1, 4, 0xFFA,(char *)"46022", false},
        {1, 21, 0xFF, (char *)"4abc", true},  // invalid plmn case
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetForbidLteCell(testVector[i].mode, testVector[i].cellId, testVector[i].forbTimer, testVector[i].plmn);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_FORBID_LTE_CELL);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int mode = jsonReqData[ns_oem_set_forbid_lte_req::key_mode].asInt();
        int cellId = jsonReqData[ns_oem_set_forbid_lte_req::key_cell_id].asInt();
        int forbTimer = jsonReqData[ns_oem_set_forbid_lte_req::key_forbidden_timer].asInt();
        string plmn_str = jsonReqData[ns_oem_set_forbid_lte_req::key_plmn].asString();

        EXPECT_EQ(mode, testVector[i].mode);
        EXPECT_EQ(cellId, testVector[i].cellId);
        EXPECT_EQ(forbTimer, testVector[i].forbTimer);

        if(strlen(testVector[i].plmn) == 5) {
            EXPECT_EQ(strncmp(plmn_str.c_str(), testVector[i].plmn, 5), 0);
        } else {
            EXPECT_STREQ(plmn_str.c_str(), testVector[i].plmn);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildGetModemActivityInfo) {
    TokenGen::Init();

    ProtocolMiscBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildGetModemActivityInfo());
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_ACTIVITY_INFO);
    EXPECT_EQ(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
}

TEST(ProtocolMiscBuilderJson, BuildOemSetSvn) {
    TokenGen::Init();

    struct {
        char * svn;
        bool isModemDataNull;
    } testVector[] = {
        {(char *)"01", false},
        {(char *)"20", false},
        {(char *)"3", false},
        {(char *)"2", false},
        {NULL, true},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildOemSetSvn(testVector[i].svn);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM5_SET_SVN);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        string svn_str = jsonReqData[ns_oem_set_svn_req::key_svn].asString();

        EXPECT_STREQ(svn_str.c_str(), testVector[i].svn);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetForbidFrequency) {
    TokenGen::Init();

    struct {
        int mode;
        int freq;
        int forbTimer;
    } testVector[] = {
        {1, 11025, 123},
        {2, 123123, 21},
        {1, 0xFFA, 0xFF},
        {0, 0x00, 0x23},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetForbidFrequency(testVector[i].mode, testVector[i].freq, testVector[i].forbTimer);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_FORBIDDEN_FREQUENCY);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int mode = jsonReqData[ns_oem_set_forbidden_freq_req::key_mode].asInt();
        int freq = jsonReqData[ns_oem_set_forbidden_freq_req::key_frequency].asInt();
        int forbTimer = jsonReqData[ns_oem_set_forbidden_freq_req::key_forbidden_timer].asInt();

        EXPECT_EQ(mode, testVector[i].mode);
        EXPECT_EQ(freq, testVector[i].freq);
        EXPECT_EQ(forbTimer, testVector[i].forbTimer);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetBsrPara) {
    TokenGen::Init();

    struct {
        int switchStatus;
        int packetPeriod;
        int packetSize;
    } testVector[] = {
        {1, 11025, 123},
        {2, 123123, 21},
        {1, 0xFFA, 0xFF},
        {0, 0x00, 0x23},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetBsrPara(testVector[i].switchStatus, testVector[i].packetPeriod, testVector[i].packetSize);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_BSR_PARA_REQ);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int switchStatus = jsonReqData[ns_oem_set_bsr_param_req::key_switch_status].asInt();
        int packetPeriod = jsonReqData[ns_oem_set_bsr_param_req::key_packet_period].asInt();
        int packetSize = jsonReqData[ns_oem_set_bsr_param_req::key_packet_size].asInt();

        EXPECT_EQ(switchStatus, testVector[i].switchStatus);
        EXPECT_EQ(packetPeriod, testVector[i].packetPeriod);
        EXPECT_EQ(packetSize, testVector[i].packetSize);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetAllDivSetting) {
    TokenGen::Init();

    ProtocolMiscBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildSetAllDivSetting());
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_ALL_4RX_DIV_SETTING_DEFAULT);
    EXPECT_EQ(testAdapter.GetParameter(), nullptr);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
}

TEST(ProtocolMiscBuilderJson, BuildSetAheadRre) {
    TokenGen::Init();

    struct {
        int searchType;
        int earfcn;
        int globalCellId;
    } testVector[] = {
        {1, 11025, 123},
        {2, 123123, 21},
        {1, 0xFFA, 0xFF},
        {0, 0x00, 0x23},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetAheadRre(testVector[i].searchType, testVector[i].earfcn, testVector[i].globalCellId);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_AHEAD_RRE);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int searchType = jsonReqData[ns_oem_set_ahead_rre_req::key_search_type].asInt();
        int earfcn = jsonReqData[ns_oem_set_ahead_rre_req::key_earfcn].asInt();
        int globalCellId = jsonReqData[ns_oem_set_ahead_rre_req::key_global_cell_id].asInt();

        EXPECT_EQ(searchType, testVector[i].searchType);
        EXPECT_EQ(earfcn, testVector[i].earfcn);
        EXPECT_EQ(globalCellId, testVector[i].globalCellId);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetDrdv) {
    TokenGen::Init();

    struct {
        int flag;
    } testVector[] = {
        {123},
        {123123},
        {1},
        {0x23},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetDrdv(testVector[i].flag);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_DRDV);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int flag = jsonReqData[ns_oem_set_drdv_req::key_drdv_mask].asInt();

        EXPECT_EQ(flag, testVector[i].flag);
        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildSetForbidNrCell) {
    TokenGen::Init();

    struct {
        int mode;
        ULONG cellId;
        int forbTimer;
        char * plmn;
        bool isModemDataNull;
    } testVector[] = {
        {1, 0xFFFFFFFFFFFF, 0xFF, (char *)"46001", false},
        {0, 0x123FFFFFFF, 0xFFF, (char *)"460020", false},
        {1, 23, 0xFFE, (char *)"460123", false},
        {1, 4, 0xFFA,(char *)"46022", false},
        {1, 4, 0xFFA,(char *)"4abc", true},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolMiscBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetForbidNrCell(testVector[i].mode, &(testVector[i].cellId), testVector[i].forbTimer, testVector[i].plmn);

        if (testVector[i].isModemDataNull) {
            ASSERT_EQ(modemData, nullptr);
            continue;
        }

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_FORBID_NR_CELL);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int mode = jsonReqData[ns_oem_set_forbid_nr_req::key_mode].asInt();
        ULONG cellId = jsonReqData[ns_oem_set_forbid_nr_req::key_cell_id].asLargestUInt();
        int forbTimer = jsonReqData[ns_oem_set_forbid_nr_req::key_forbidden_timer].asInt();
        string plmn_str = jsonReqData[ns_oem_set_forbid_nr_req::key_plmn].asString();

        EXPECT_EQ(mode, testVector[i].mode);
        EXPECT_EQ(cellId, testVector[i].cellId);
        EXPECT_EQ(forbTimer, testVector[i].forbTimer);

        if(strlen(testVector[i].plmn) == 5)
        {
            EXPECT_EQ(strncmp(plmn_str.c_str(), testVector[i].plmn, 5), 0);
        }
        else
        {
            EXPECT_STREQ(plmn_str.c_str(), testVector[i].plmn);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolMiscBuilderJson, BuildGetCdmaCapabilityInfo) {
    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetCdmaCapabilityInfo();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_OEM_GET_CDMA_CAPABILITY_INFO);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscNetworkBuilderJson, BuildGetPreferredCallCapability) {
    ProtocolMiscNetworkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetPreferredCallCapability();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PREFERRED_CALL_CAPABILITY);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscNetworkBuilderJson, BuildSetImsTestMode) {

    int mode = 1;

    ProtocolMiscBuilderJson testBuilder;

    ModemData *modemData = testBuilder.BuildSetImsTestMode(mode);
    ASSERT_NE(modemData, nullptr);

    if( modemData != nullptr ){
        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_IMS_TEST_MODE);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }

        ASSERT_NE(false, parsed);

        EXPECT_EQ(1, jsonReqData[ns_misc_ps_set_ims_test_mode_req::key_mode].asInt());

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolMiscBuilderJson, BuildNvReadItem) {
    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildNvReadItem(0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscBuilderJson, BuildNvWriteItem) {
    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildNvWriteItem(0, NULL);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscBuilderJson, BuildSetSuppSvcNotification) {
    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetSuppSvcNotification(0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscBuilderJson, BuildSetCABandwidthFilter) {
    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetCABandwidthFilter(0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscBuilderJson, BuildSetForbidLetBand) {
    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetForbidLetBand(0, 0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscBuilderJson, BuildSetForbidLteTac) {
    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetForbidLteTac(0, 0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscBuilderJson, BuildSetVcrt) {
    ProtocolMiscBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetVcrt(0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, BuildGetHwConfig) {
    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetHwConfig();
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, BuildLceStart) {
    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildLceStart(0, 0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, BuildLceStop) {
    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildLceStop();
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, BuildLcePullLceData) {
    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildLcePullLceData();
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, BuildSetCarrierInfoImsiEncryption) {
    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetCarrierInfoImsiEncryption(NULL, NULL, 0, NULL, 0, NULL, 0, 0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolDeviceInfoBuilderJson, BuildSetOpenCarierInfo) {
    ProtocolDeviceInfoBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetOpenCarierInfo(0, NULL);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSensorBuilderJson, BuildSetSarState) {
    ProtocolSensorBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetSarState(0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSensorBuilderJson, BuildGetSarState) {
    ProtocolSensorBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetSarState();
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolOemOmaDmBuilderJson, BuildGetRadioNode) {
    ProtocolOemOmaDmBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetRadioNode(NULL);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolOemOmaDmBuilderJson, BuildSetRadioNode) {
    ProtocolOemOmaDmBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetRadioNode(NULL, NULL);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolOemOmaDmBuilderJson, BuildGetVoLteProvisionUpdate) {
    ProtocolOemOmaDmBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetVoLteProvisionUpdate();
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolOemOmaDmBuilderJson, BuildSetVoLteProvisionUpdate) {
    ProtocolOemOmaDmBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetVoLteProvisionUpdate();
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscDebugBuilderJson, BuildSetSelflogProfile) {
    ProtocolMiscDebugBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetSelflogProfile();
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscDebugBuilderJson, BuildSetSelflogTcpDumpMode) {
    ProtocolMiscDebugBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetSelflogTcpDumpMode(0, 0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscDebugBuilderJson, StoreAdbSerialNumber) {
    ProtocolMiscDebugBuilderJson testBuilder;
    ModemData *modemData = testBuilder.StoreAdbSerialNumber(NULL, 0);
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolMiscDebugBuilderJson, ReadAdbSerialNumber) {
    ProtocolMiscDebugBuilderJson testBuilder;
    ModemData *modemData = testBuilder.ReadAdbSerialNumber();
    ASSERT_EQ(modemData, nullptr);

    delete modemData;
    modemData = nullptr;
}
