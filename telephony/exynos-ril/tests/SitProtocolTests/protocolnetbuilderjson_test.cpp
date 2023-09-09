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

#include "protocoljsontestadapter.h"
#include "json/network/protocolnetbuilderjson.h"
#include "netjsondef.h"
#include "modemdata.h"

using namespace std;

static bool ParsingJsonPayload(const char *inData, Json::Value &outData, int sitId, bool debug_print = false) {
    string st = inData;
    stringstream sstream(st);
    Json::CharReaderBuilder jsonReader;
    string errs;

    if (debug_print) printf("[0x%x] json data = %s\n", sitId, inData);

    bool parsed = Json::parseFromStream(jsonReader, sstream, &outData, &errs);
    if (debug_print) {
        if(!parsed) printf("[0x%x] parsing failed error = %s\n", sitId, errs.c_str());
    }

    return parsed;
}

/*
 * ProtocolNetworkRegistrationBuilderJson class
 */
TEST(ProtocolNetworkRegistrationBuilderJson, BuildNetworkRegistrationState) {
    struct {
        int domain;
        unsigned int sitCmdId;
        bool isModemDataNull;
    } testVector[] = {
        { NETWORK_DOMAIN_UNKNOWN, 0, true},
        { NETWORK_DOMAIN_CS, SIT_GET_CS_REG_STATE, false },
        { NETWORK_DOMAIN_PS, SIT_GET_PS_REG_STATE, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkRegistrationBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildNetworkRegistrationState(testVector[i].domain);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);

            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), testVector[i].sitCmdId);
            EXPECT_EQ(testAdapter.GetParameter(), nullptr);
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkRegistrationBuilderJson, BuildOperator) {
    ProtocolNetworkRegistrationBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildOperator();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_OPERATOR);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

/***********************************************************************
 * ProtocolRadioPowerBuilderJson class
 */
TEST(ProtocolRadioPowerBuilderJson, BuildRadioPower) {
    struct {
        int powerState;
        int forEmergencyCall;
        int preferredForEmergencyCall;
        bool isModemDataNull;
    } testVector[] = {
        { 0, 0, 0, false },
        { 0, 1, 0, false },
        { 0, 1, 1, false },
        { 1, 0, 0, false },
        { 1, 0, 1, false },
        { 1, 1, 1, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolRadioPowerBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildRadioPower(testVector[i].powerState,
                testVector[i].forEmergencyCall, testVector[i].preferredForEmergencyCall);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_RADIO_POWER);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                int powerState, forEmergencyCall, preferredForEmergencyCall;
                Json::Value reqData;

                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(!parsed) { delete modemData; modemData = nullptr; }
                ASSERT_NE(false, parsed);

                powerState = reqData[ns_pwr_set_radio_power_req::key_radio_state].asInt();
                forEmergencyCall = reqData[ns_pwr_set_radio_power_req::key_for_emc].asInt();
                preferredForEmergencyCall =  reqData[ns_pwr_set_radio_power_req::key_preferred_for_emc].asInt();
                if (testVector[i].powerState == 0) {
                    EXPECT_EQ(powerState, SIT_PWR_RADIO_STATE_STOP_NETWORK);
                } else {
                    EXPECT_EQ(powerState, SIT_PWR_RADIO_STATE_START_NETWORK);
                }
                EXPECT_EQ(forEmergencyCall, testVector[i].forEmergencyCall);
                EXPECT_EQ(preferredForEmergencyCall, testVector[i].preferredForEmergencyCall);
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolRadioPowerBuilderJson, BuildShutdown) {
    ProtocolRadioPowerBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildShutdown();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_RADIO_POWER);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    // Checking payload
    if (testAdapter.GetParameter() != nullptr) {
        Json::Value reqData;
        bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
        if(parsed) {
            // need to implement
        }
    }

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolRadioPowerBuilderJson, BuildGetRadioState) {
    ProtocolRadioPowerBuilderJson testBuilder;

    ModemData *modemData = testBuilder.BuildGetRadioState();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_RADIO_POWER);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolRadioPowerBuilderJson, BuildRestartModem) {
    ProtocolRadioPowerBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildRestartModem();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_RADIO_POWER);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    // Checking payload
    if (testAdapter.GetParameter() != nullptr) {
        Json::Value reqData;
        bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
        if(parsed) {
            // need to implement
        }
    }

    if (modemData) { delete modemData; modemData = nullptr; }
}

/*
 * ProtocolNetworkModeBuilderJson class
 */
TEST(ProtocolNetworkModeBuilderJson, BuildQueryNetworkSelectionMode) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildQueryNetworkSelectionMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_NTW_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetNetworkSelectionAuto) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildSetNetworkSelectionAuto();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_NTW_MODE_AUTO);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetNetworkSelectionManual) {
    struct {
        int rat;
        const char *plmn;
        bool isModemDataNull;
        int resultSitRat;
    } testVector[] = {
        { RADIO_TECH_EDGE, "1234567", true, SIT_RAT_TYPE_EDGE }, // case where plmn length is more than expected
        { RADIO_TECH_EDGE, "12345", false, SIT_RAT_TYPE_EDGE },
        { RADIO_TECH_TD_SCDMA, "123456", false, SIT_RAT_TYPE_TD_SCDMA },
        { RADIO_TECH_LTE_CA, "123456", false, SIT_RAT_TYPE_LTE_CA },
        { RADIO_TECH_NR, "12345", false, SIT_RAT_TYPE_5G },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetNetworkSelectionManual(testVector[i].rat, testVector[i].plmn);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_NTW_MODE_MANUAL);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                    int rat = reqData[ns_net_set_ntw_mode_manual_req::key_rat].asInt();
                    string plmn = reqData[ns_net_set_ntw_mode_manual_req::key_plmn].asString();

                    EXPECT_EQ(rat, testVector[i].resultSitRat);

                    EXPECT_TRUE(plmn.length() <= MAX_PLMN_LEN);
                    EXPECT_EQ(strncmp(plmn.c_str(), testVector[i].plmn, MAX_PLMN_LEN), 0);
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetBandMode) {
    struct {
        int band;
        bool isModemDataNull;
    } testVector[] = {
        { SIT_NET_BAND_EURO, false },
        { SIT_NET_BAND_US, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetBandMode(testVector[i].band);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_BAND_MODE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildQueryAvailableBandMode) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildQueryAvailableBandMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BAND_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetEndcMode) {
    struct {
        int endcMode;
        bool isModemDataNull;
        int resultSitEndcMode;
        int resultSitImmScgRel;
    } testVector[] = {
        { 0, false, SIT_NET_DISABLE, SIT_ENDC_NOT_RELEASE },
        { 1, false, SIT_NET_ENABLE, SIT_ENDC_NOT_RELEASE },
        { 2, false, SIT_NET_DISABLE, SIT_ENDC_RELEASE },
        { 3, false, SIT_NET_DISABLE, SIT_ENDC_NOT_RELEASE },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetEndcMode(testVector[i].endcMode);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_ENDC_MODE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetEndcMode) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetEndcMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_ENDC_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetFrequencyInfo) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetFrequencyInfo();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_FREQUENCY_INFO);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetNrMode) {
    struct {
        int nrMode;
        bool isModemDataNull;
    } testVector[] = {
        { SIT_NR_MODE_NSA, false},
        { SIT_NR_MODE_SA, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetNrMode(testVector[i].nrMode);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_NR_MODE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetNrMode) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetNrMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_NR_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetNrSilenceMode) {
    struct {
        int nsa;
        int sa;
    } testVector[] = {
        { 0, 1},
        { 1, 2},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetNrSilenceMode(testVector[i].nsa, testVector[i].sa);
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_NR_SILENCE_MODE);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        if(!parsed) { delete modemData; modemData = nullptr; }
        ASSERT_NE(false, parsed);

        int nsaMode, saMode;
        nsaMode = jsonReqData[ns_net_misc_set_nr_silence_mode_req::key_nsa_mode].asInt();
        saMode = jsonReqData[ns_net_misc_set_nr_silence_mode_req::key_sa_mode].asInt();
        EXPECT_EQ(nsaMode, testVector[i].nsa);
        EXPECT_EQ(saMode, testVector[i].sa);

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetNrSilenceMode) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetNrSilenceMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_NR_SILENCE_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetVonrCapa) {
    struct {
        int vonrCapa;
        bool isModemDataNull;
    } testVector[] = {
        { 1, false},
        { 2, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetVonrCapa(testVector[i].vonrCapa);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_VONR_CAPABILITY);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    int vonrCapa = reqData[ns_net_misc_set_vonr_capability_req::key_vonr_capa].asInt();
                    EXPECT_EQ(vonrCapa, testVector[i].vonrCapa);
                }
            }
        }
        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetVonrCapa) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetVonrCapa();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_VONR_CAPABILITY);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetManualRatMode) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetManualRatMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_MANUAL_RAT_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetManualRatMode) {
    struct {
        int mode;
        int rat;
        bool isModemDataNull;
    } testVector[] = {
        { SIT_NET_DISABLE, SIT_NET_PREF_NET_TYPE_GSM_ONLY, false},
        { SIT_NET_ENABLE, SIT_NET_PREF_NET_TYPE_WCDMA, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetManualRatMode(testVector[i].mode, testVector[i].rat);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_MANUAL_RAT_MODE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetBarringInfo) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetBarringInfo();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BARRING_INFOS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetAllowData) {
    struct {
        int state;
        bool isModemDataNull;
    } testVector[] = {
        { ALLOW_DATA_CALL, false},
        { DISALLOW_DATA_CALL, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetAllowData(testVector[i].state);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_PS_SERVICE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetPsService) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetPsService();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_SERVICE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetDuplexMode0) {
    struct {
        int mode4g;
        int mode3g;
        bool isModemDataNull;
    } testVector[] = {
        { SIT_NET_DUPLEX_MODE_FDD, SIT_NET_DUPLEX_MODE_TDD, false},
        { SIT_NET_DUPLEX_MODE_FDD_TDD, SIT_NET_DUPLEX_MODE_FDD, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetDuplexMode(testVector[i].mode4g, testVector[i].mode3g);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_DUPLEX_MODE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetDuplexMode1) {
    struct {
        int mode;
        int resultSitMode4G;
        int resultSitMode3G;
        bool isModemDataNull;
    } testVector[] = {
        { DUPLEX_MODE_GLOBAL, SIT_NET_DUPLEX_MODE_FDD_TDD, SIT_NET_DUPLEX_MODE_FDD_TDD, false},
        { DUPLEX_MODE_LTG, SIT_NET_DUPLEX_MODE_TDD, SIT_NET_DUPLEX_MODE_TDD, false},
        { DUPLEX_MODE_LWG, SIT_NET_DUPLEX_MODE_FDD_TDD, SIT_NET_DUPLEX_MODE_FDD, false},
        { DUPLEX_MODE_INVALID, SIT_NET_DUPLEX_MODE_FDD_TDD, SIT_NET_DUPLEX_MODE_FDD_TDD, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetDuplexMode(testVector[i].mode);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_DUPLEX_MODE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetDuplexMode) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetDuplexMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_DUPLEX_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetSysSelChannels) {
    RIL_RadioAccessSpecifier_V1_5 ras[4] = {
        {RIL_RAN_GERAN,  2, {}, 2, {1,2,}},
        {RIL_RAN_UTRAN,  2, {}, 2, {1,2,}},
        {RIL_RAN_EUTRAN, 2, {}, 2, {1,2,}},
        {(RIL_RadioAccessNetworks)RIL_RAN_NGRAN,  2, {}, 2, {}},
    };
    ras[0].bands.geran_bands[0]  = GERAN_BAND_T380;
    ras[0].bands.geran_bands[1]  = GERAN_BAND_T410;
    ras[1].bands.utran_bands[0]  = UTRAN_BAND_1;
    ras[1].bands.utran_bands[1]  = UTRAN_BAND_2;
    ras[2].bands.eutran_bands[0] = EUTRAN_BAND_1;
    ras[2].bands.eutran_bands[1] = EUTRAN_BAND_2;
    ras[3].bands.ngran_bands[0]  = NGRAN_BAND_1;
    ras[3].bands.ngran_bands[1]  = NGRAN_BAND_2;

    struct {
        bool isSpecifyChannel;
        int numOfSpecifiers;
        RIL_RadioAccessSpecifier_V1_5 *pSpecifiers;
        bool isModemDataNull;
    } testVector[] = {
        { false, 0, nullptr, false},
        { true, 4, ras, false},
        { true, 5, ras, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetSysSelChannels(testVector[i].isSpecifyChannel,
                                                testVector[i].numOfSpecifiers,
                                                testVector[i].pSpecifiers);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_SYSTEM_SELECTION_CHANNELS);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    int isSpecifyChannel = reqData[ns_net_set_system_selection_channels_req::key_is_specify_channels].asInt();
                    EXPECT_EQ(testVector[i].isSpecifyChannel, isSpecifyChannel);

                    unsigned int rasListSize = reqData[ns_net_set_system_selection_channels_req::key_radio_access_specifier_list].size();
                    EXPECT_EQ(rasListSize, MIN((unsigned int)testVector[i].numOfSpecifiers, SIT_MAX_NO_RAS));

                    for(unsigned int j = 0; j < MIN(rasListSize, SIT_MAX_NO_RAS); j++) {
                        Json::Value jsonRas = reqData[ns_net_set_system_selection_channels_req::key_radio_access_specifier_list][j];

                        int ran = jsonRas[ns_net_ras::key_ran].asInt();
                        EXPECT_EQ(testVector[i].pSpecifiers[j].radio_access_network, ran);

                        unsigned int bandsLength = jsonRas[ns_net_ras::key_band_list].size();
                        EXPECT_EQ(testVector[i].pSpecifiers[j].bands_length, bandsLength);

                        for(unsigned int k = 0; k < bandsLength; k++) {
                            int band = jsonRas[ns_net_ras::key_band_list][k].asInt();
                            switch(ran) {
                                case SIT_NET_ACCESS_RADIO_TYPE_GERAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.geran_bands[k], band);
                                    break;
                                case SIT_NET_ACCESS_RADIO_TYPE_UTRAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.utran_bands[k], band);
                                    break;
                                case SIT_NET_ACCESS_RADIO_TYPE_EUTRAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.eutran_bands[k], band);
                                    break;
                                case SIT_NET_ACCESS_RADIO_TYPE_NGRAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.ngran_bands[k], band);
                                    break;
                            }
                        }

                        unsigned int channelsLength = jsonRas[ns_net_ras::key_channel_list].size();
                        EXPECT_EQ(channelsLength, testVector[i].pSpecifiers[j].channels_length);
                        for(unsigned int k = 0; k < channelsLength; k++) {
                            int channel = jsonRas[ns_net_ras::key_channel_list][k].asInt();
                            EXPECT_EQ((int)testVector[i].pSpecifiers[j].channels[k], channel);
                        }
                    }
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildGetFrequencyLock) {
    ProtocolNetworkModeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetFrequencyLock();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_FREQUENCY_LOCK);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetFrequencyLock) {
    struct {
        int mode;
        int rat;
        int ltePci;
        int lteEarfcn;
        int gsmArfcn;
        int wcdmaPsc;
        int wcdmaUarfcn;
        bool isModemDataNull;
    } testVector[] = {
        { SIT_NET_DISABLE, 1, 0,  0, 120, 0,   0, false},
        { SIT_NET_ENABLE,  2, 0,  0,   0, 7, 130, false},
        { SIT_NET_ENABLE,  3, 4,  5,   0, 0,   0, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetFrequencyLock(testVector[i].mode, testVector[i].rat,
                                            testVector[i].ltePci, testVector[i].lteEarfcn,
                                            testVector[i].gsmArfcn,
                                            testVector[i].wcdmaPsc, testVector[i].wcdmaUarfcn);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_FREQUENCY_LOCK);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetEmergencyCallStatus) {
    struct {
        int status;
        int rat;
        bool isModemDataNull;
    } testVector[] = {
        { 10, 0, true},
        { EMERGENCY_CALL_STATUS_START,  RADIO_TECH_GSM,         false},
        { EMERGENCY_CALL_STATUS_START,  RADIO_TECH_TD_SCDMA,    false},
        { EMERGENCY_CALL_STATUS_START,  RADIO_TECH_IWLAN,       false},
        { EMERGENCY_CALL_STATUS_START,  RADIO_TECH_NR,          false},
        { EMERGENCY_CALL_STATUS_START,  RADIO_TECH_UNSPECIFIED, false},
        { EMERGENCY_CALL_STATUS_START,  RADIO_TECH_LTE_CA,      false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetEmergencyCallStatus(testVector[i].status, testVector[i].rat);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_EMERGENCY_CALL_STATUS);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkModeBuilderJson, BuildSetMicroCellSearch) {
    struct {
        int srchMode;
        bool isModemDataNull;
    } testVector[] = {
        { SIT_MC_SRCH_MODE_AUTO,   false},
        { SIT_MC_SRCH_MODE_CANCEL, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetMicroCellSearch(testVector[i].srchMode);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_MICRO_CELL_SEARCH);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

/*
 * ProtocolNetworkScanBuilderJson class
 */
TEST(ProtocolNetworkScanBuilderJson, BuildQueryAvailableNetwork) {
    struct {
        int ran;
        bool isModemDataNull;
    } testVector[] = {
        { RIL_RAN_GERAN,  false},
        { RIL_RAN_UTRAN,  false},
        { RIL_RAN_EUTRAN, false},
        { RIL_RAN_NGRAN,  false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkScanBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildQueryAvailableNetwork(testVector[i].ran);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_GET_AVAILABLE_NETWORKS);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkScanBuilderJson, BuildCancelQueryAvailableNetwork) {
    ProtocolNetworkScanBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildCancelQueryAvailableNetwork();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_CANCEL_GET_AVAILABLE_NETWORKS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkScanBuilderJson, BuildStartNetworkScan0) {
    RIL_RadioAccessSpecifier_V1_5 ras[4] = {
        {RIL_RAN_GERAN,  2, {}, 2, {1,2,}},
        {RIL_RAN_UTRAN,  2, {}, 2, {1,2,}},
        {RIL_RAN_EUTRAN, 2, {}, 2, {1,2,}},
        {(RIL_RadioAccessNetworks)RIL_RAN_NGRAN,  2, {}, 2, {}},
    };

    ras[0].bands.geran_bands[0]  = GERAN_BAND_T380;
    ras[0].bands.geran_bands[1]  = GERAN_BAND_T410;
    ras[1].bands.utran_bands[0]  = UTRAN_BAND_1;
    ras[1].bands.utran_bands[1]  = UTRAN_BAND_2;
    ras[2].bands.eutran_bands[0] = EUTRAN_BAND_1;
    ras[2].bands.eutran_bands[1] = EUTRAN_BAND_2;
    ras[3].bands.ngran_bands[0]  = NGRAN_BAND_1;
    ras[3].bands.ngran_bands[1]  = NGRAN_BAND_2;

    struct {
        int scanType;
        int cpScanTypeBitmask;
        int timeInterval;
        int cpTimeInterval;
        int lenSpecifiers;
        RIL_RadioAccessSpecifier_V1_5 *pSpecifiers;
        bool isModemDataNull;
    } testVector[] = {
        { RIL_ONE_SHOT, 0x80,  11, 00, 1, nullptr,  true /*due to pSpecifiers*/},
        { RIL_ONE_SHOT, 0x80,  01, 00, 1, ras,      true /*due to time interval*/},
        { RIL_ONE_SHOT, 0x80,  10, 00, 4, ras,      false},
        { RIL_PERIODIC, 0x81,  01, 00, 1, ras,      true /*due to time interval*/},
        { RIL_PERIODIC, 0x81,  11, 11, 1, nullptr,  true /*due to pSpecifiers*/},
        { RIL_PERIODIC, 0x81,  10, 10, 4, ras,      false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkScanBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildStartNetworkScan(testVector[i].scanType, testVector[i].timeInterval,
                testVector[i].lenSpecifiers, testVector[i].pSpecifiers);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_START_SCANNING_NETWORKS);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    int scanTypeBitmask = reqData[ns_net_start_scanning_networks_req::key_scan_type_bitmask].asInt();
                    EXPECT_EQ(testVector[i].cpScanTypeBitmask, scanTypeBitmask);

                    int interval = reqData[ns_net_start_scanning_networks_req::key_interval].asInt();
                    EXPECT_EQ(testVector[i].cpTimeInterval, interval);

                    int maxSearchTime = reqData[ns_net_start_scanning_networks_req::key_max_search_time].asInt();
                    EXPECT_EQ(300, maxSearchTime);

                    int incrementalResult = reqData[ns_net_start_scanning_networks_req::key_incremental_result].asInt();
                    EXPECT_EQ(SIT_SCAN_INCREMENTAL_RESULT, incrementalResult);

                    int incrementalResultPeriod = reqData[ns_net_start_scanning_networks_req::key_incremental_result_periodicity].asInt();
                    EXPECT_EQ(3, incrementalResultPeriod);

                    int plmnListSize = reqData[ns_net_start_scanning_networks_req::key_plmn_list].size();
                    EXPECT_EQ(0, plmnListSize);

                    int rasListSize = reqData[ns_net_start_scanning_networks_req::key_network_scan_info_list].size();
                    EXPECT_EQ(testVector[i].lenSpecifiers, rasListSize);

                    for(int j = 0; j < rasListSize; j++) {
                        Json::Value scanInfo = reqData[ns_net_start_scanning_networks_req::key_network_scan_info_list][j];

                        int radioAccessNetwork =  scanInfo[ns_net_scan_info::key_ran].asInt();
                        EXPECT_EQ((int)testVector[i].pSpecifiers[j].radio_access_network, radioAccessNetwork);

                        unsigned int bandsLength =  scanInfo[ns_net_scan_info::key_band_list].size();
                        EXPECT_EQ(bandsLength, testVector[i].pSpecifiers[j].bands_length);

                        for(unsigned int k = 0; k < bandsLength; k++) {
                            int band = scanInfo[ns_net_scan_info::key_band_list][k].asInt();
                            switch(radioAccessNetwork) {
                                case SIT_NET_ACCESS_RADIO_TYPE_GERAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.geran_bands[k], band);
                                    break;
                                case SIT_NET_ACCESS_RADIO_TYPE_UTRAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.utran_bands[k], band);
                                    break;
                                case SIT_NET_ACCESS_RADIO_TYPE_EUTRAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.eutran_bands[k], band);
                                    break;
                                case SIT_NET_ACCESS_RADIO_TYPE_NGRAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.ngran_bands[k], band);
                                    break;
                            }
                        }

                        unsigned int channelsLength =  scanInfo[ns_net_scan_info::key_channel_list].size();
                        EXPECT_EQ(channelsLength, testVector[i].pSpecifiers[j].channels_length);
                        for(unsigned int k = 0; k < channelsLength; k++) {
                            int channel = scanInfo[ns_net_scan_info::key_channel_list][k].asInt();
                            EXPECT_EQ((int)testVector[i].pSpecifiers[j].channels[k], channel);
                        }
                    }
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkScanBuilderJson, BuildStartNetworkScan1) {
    RIL_RadioAccessSpecifier_V1_5 ras[4] = {
        {RIL_RAN_GERAN,  2, {}, 2, {1,2,}},
        {RIL_RAN_UTRAN,  2, {}, 2, {1,2,}},
        {RIL_RAN_EUTRAN, 2, {}, 2, {1,2,}},
        {(RIL_RadioAccessNetworks)RIL_RAN_NGRAN,  2, {}, 2, {}},
    };
    ras[0].bands.geran_bands[0]  = GERAN_BAND_T380;
    ras[0].bands.geran_bands[1]  = GERAN_BAND_T410;
    ras[1].bands.utran_bands[0]  = UTRAN_BAND_1;
    ras[1].bands.utran_bands[1]  = UTRAN_BAND_2;
    ras[2].bands.eutran_bands[0] = EUTRAN_BAND_1;
    ras[2].bands.eutran_bands[1] = EUTRAN_BAND_2;
    ras[3].bands.ngran_bands[0]  = NGRAN_BAND_1;
    ras[3].bands.ngran_bands[1]  = NGRAN_BAND_2;

    const int numOfPlmns = 3;
    char plmns[numOfPlmns][10] = {"12345", "123456", "123123"};

    size_t memSize = numOfPlmns * sizeof(char *);
    char **ppPlmns = (char **)malloc(memSize);
    if (ppPlmns != nullptr) {
        for (int i = 0; i < numOfPlmns; i++) {
            ppPlmns[i] = plmns[i];
        }
    }

    struct {
        int scanType;
        int cpScanTypeBitmask;
        int timeInterval;
        int cpTimeInterval;
        int lenSpecifiers;
        RIL_RadioAccessSpecifier_V1_5 *pSpecifiers;
        int maxSearchTime;
        bool incrementalResult;
        int incrementalResultPeriod;
        int numOfPlmns;
        char **mccMncs;
        bool isModemDataNull;
    } testVector[] = {
        { RIL_ONE_SHOT, 0x80, 10, 00, 1, nullptr,  100,  true,  1,   0,          nullptr,   true /* due to pSpecifiers */},
        { RIL_ONE_SHOT, 0x80, 01, 00, 4, ras,      200,  false, 3,   0,          nullptr,   true /* due to timeInterval */},
        { RIL_ONE_SHOT, 0x80, 10, 00, 4, ras,      200,  false, 3,   0,          nullptr,   false},
        { RIL_ONE_SHOT, 0x80, 10, 00, 4, ras,      200,  false, 3,   0,          nullptr,   false},
        { RIL_PERIODIC, 0x81, 15, 15, 4, ras,      3601, true,  10,   numOfPlmns, ppPlmns,  true /* due to maximumSearchTime */},
        { RIL_PERIODIC, 0x81, 20, 20, 4, ras,      300,  true,  10,   numOfPlmns, ppPlmns,  false},
        { RIL_PERIODIC, 0x81, 30, 30, 4, ras,      300,  true,  13,   numOfPlmns, ppPlmns,  true /* due to incrementalResultPeriod */},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkScanBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildStartNetworkScan(testVector[i].scanType, testVector[i].timeInterval,
                testVector[i].lenSpecifiers, testVector[i].pSpecifiers,
                testVector[i].maxSearchTime, testVector[i].incrementalResult, testVector[i].incrementalResultPeriod,
                testVector[i].numOfPlmns, testVector[i].mccMncs);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_START_SCANNING_NETWORKS);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    int scanTypeBitmask = reqData[ns_net_start_scanning_networks_req::key_scan_type_bitmask].asInt();
                    EXPECT_EQ(testVector[i].cpScanTypeBitmask, scanTypeBitmask);

                    int interval = reqData[ns_net_start_scanning_networks_req::key_interval].asInt();
                    EXPECT_EQ(testVector[i].cpTimeInterval, interval);

                    int maxSearchTime = reqData[ns_net_start_scanning_networks_req::key_max_search_time].asInt();
                    EXPECT_EQ(testVector[i].maxSearchTime, maxSearchTime);

                    int incrementalResult = reqData[ns_net_start_scanning_networks_req::key_incremental_result].asInt();
                    EXPECT_EQ(testVector[i].incrementalResult, incrementalResult);

                    int incrementalResultPeriod = reqData[ns_net_start_scanning_networks_req::key_incremental_result_periodicity].asInt();
                    if(testVector[i].incrementalResultPeriod < 3) {
                        EXPECT_EQ(3, incrementalResultPeriod);
                    } else if (testVector[i].incrementalResultPeriod > 10) {
                        EXPECT_EQ(10, incrementalResultPeriod);
                    } else {
                        EXPECT_EQ(testVector[i].incrementalResultPeriod, incrementalResultPeriod);
                    }

                    int plmnListSize = reqData[ns_net_start_scanning_networks_req::key_plmn_list].size();
                    EXPECT_EQ(testVector[i].numOfPlmns, plmnListSize);
                    for(int k = 0; k < plmnListSize; k++) {
                        string plmn = reqData[ns_net_start_scanning_networks_req::key_plmn_list][k].asString();
                        EXPECT_STREQ(testVector[i].mccMncs[k], plmn.c_str());
                    }

                    int rasListSize = reqData[ns_net_start_scanning_networks_req::key_network_scan_info_list].size();
                    EXPECT_EQ(testVector[i].lenSpecifiers, rasListSize);

                    for(int j = 0; j < rasListSize; j++) {
                        Json::Value scanInfo = reqData[ns_net_start_scanning_networks_req::key_network_scan_info_list][j];

                        int radioAccessNetwork =  scanInfo[ns_net_scan_info::key_ran].asInt();
                        EXPECT_EQ((int)testVector[i].pSpecifiers[j].radio_access_network, radioAccessNetwork);

                        unsigned int bandsLength =  scanInfo[ns_net_scan_info::key_band_list].size();
                        EXPECT_EQ(bandsLength, testVector[i].pSpecifiers[j].bands_length);

                        for(unsigned int k = 0; k < bandsLength; k++) {
                            int band = scanInfo[ns_net_scan_info::key_band_list][k].asInt();
                            switch(radioAccessNetwork) {
                                case SIT_NET_ACCESS_RADIO_TYPE_GERAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.geran_bands[k], band);
                                    break;
                                case SIT_NET_ACCESS_RADIO_TYPE_UTRAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.utran_bands[k], band);
                                    break;
                                case SIT_NET_ACCESS_RADIO_TYPE_EUTRAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.eutran_bands[k], band);
                                    break;
                                case SIT_NET_ACCESS_RADIO_TYPE_NGRAN:
                                    EXPECT_EQ((int)testVector[i].pSpecifiers[j].bands.ngran_bands[k], band);
                                    break;
                            }
                        }

                        unsigned int channelsLength =  scanInfo[ns_net_scan_info::key_channel_list].size();
                        EXPECT_EQ(channelsLength, testVector[i].pSpecifiers[j].channels_length);
                        for(unsigned int k = 0; k < channelsLength; k++) {
                            int channel = scanInfo[ns_net_scan_info::key_channel_list][k].asInt();
                            EXPECT_EQ((int)testVector[i].pSpecifiers[j].channels[k], channel);
                        }
                    }
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }

    if (ppPlmns != nullptr) free(ppPlmns);
}

TEST(ProtocolNetworkScanBuilderJson, BuildStopNetworkScan) {
    struct {
        int sitScanType;
        bool isModemDataNull;
    } testVector[] = {
        { SIT_SCAN_TYPE_STOP,  false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkScanBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildStopNetworkScan();
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_START_SCANNING_NETWORKS);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    int scanTypeBitmask = reqData[ns_net_start_scanning_networks_req::key_scan_type_bitmask].asInt();
                    EXPECT_EQ((int)(0X02), scanTypeBitmask); // bitmask 0000 0010
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

/*
 * ProtocolNetworkTypeBuilderJson class
 */
TEST(ProtocolNetworkTypeBuilderJson, BuildSetPreferredNetworkType) {
    struct {
        int rilNetType;
        int result;
        bool isModemDataNull;
    } testVector[] = {
        { PREF_NET_TYPE_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_GSM_WCDMA, false },
        { PREF_NET_TYPE_GSM_ONLY, SIT_NET_PREF_NET_TYPE_GSM_ONLY, false },
        { PREF_NET_TYPE_WCDMA, SIT_NET_PREF_NET_TYPE_WCDMA, false },
        { PREF_NET_TYPE_GSM_WCDMA_AUTO, SIT_NET_PREF_NET_TYPE_GSM_WCDMA_AUTO, false },
        { PREF_NET_TYPE_CDMA_EVDO_AUTO, SIT_NET_PREF_NET_TYPE_CDMA_EVDO_AUTO, false },
        { PREF_NET_TYPE_CDMA_ONLY, SIT_NET_PREF_NET_TYPE_CDMA_ONLY, false },
        { PREF_NET_TYPE_EVDO_ONLY, SIT_NET_PREF_NET_TYPE_EVDO_ONLY, false },
        { PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO, SIT_NET_PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO, false },
        { PREF_NET_TYPE_LTE_CDMA_EVDO, SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO, false },
        { PREF_NET_TYPE_LTE_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_GSM_WCDMA, false },
        { PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA, false },
        { PREF_NET_TYPE_LTE_ONLY, SIT_NET_PREF_NET_TYPE_LTE_ONLY, false },
        { PREF_NET_TYPE_LTE_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_ONLY, SIT_NET_PREF_NET_TYPE_TDSCDMA_ONLY, false },
        { PREF_NET_TYPE_TD_SCDMA_WCDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM, SIT_NET_PREF_NET_TYPE_TDSCDMA_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_GSM_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_GSM_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO, SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA_EVDO_GSM_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA, false },
        { PREF_NET_TYPE_NR_ONLY, SIT_NET_PREF_NET_TYPE_NR_ONLY, false },
        { PREF_NET_TYPE_NR_LTE, SIT_NET_PREF_NET_TYPE_NR_LTE, false },
        { PREF_NET_TYPE_NR_LTE_CDMA_EVDO, SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA, false },
        { PREF_NET_TYPE_NR_LTE_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA, false },
        { PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA, false },
        { PREF_NET_TYPE_NR_LTE_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_WCDMA, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_CDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_CDMA_NO_EVDO, SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA_NO_EVDO, false },
        { PREF_NET_TYPE_TD_SCDMA_CDMA_EVDO_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_CDMA_EVDO, false },
        { PREF_NET_TYPE_TD_SCDMA_EVDO_NO_CDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_EVDO_NO_CDMA, false },
        { 100, SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkTypeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetPreferredNetworkType(testVector[i].rilNetType);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_PREFERRED_NTW_TYPE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkTypeBuilderJson, BuildGetPreferredNetworkType) {
    ProtocolNetworkTypeBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetPreferredNetworkType();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PREFERRED_NTW_TYPE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkTypeBuilderJson, BuildSetDSNetworkType) {
    struct {
        int rilNetType;
        int result;
        bool isModemDataNull;
    } testVector[] = {
        { PREF_NET_TYPE_GSM_WCDMA, SIT_NET_DS_NET_TYPE_GSM_WCDMA, false },
        { PREF_NET_TYPE_GSM_ONLY, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_WCDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_GSM_WCDMA_AUTO, SIT_NET_DS_NET_TYPE_GSM_WCDMA, false },
        { PREF_NET_TYPE_CDMA_EVDO_AUTO, SIT_NET_DS_NET_TYPE_CDMA_EVDO_AUTO, false },
        { PREF_NET_TYPE_CDMA_ONLY, SIT_NET_DS_NET_TYPE_CDMA, false },
        { PREF_NET_TYPE_EVDO_ONLY, SIT_NET_DS_NET_TYPE_CDMA, false },
        { PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_LTE_CDMA_EVDO, SIT_NET_DS_NET_TYPE_LTE_CDMA_EVDO, false },
        { PREF_NET_TYPE_LTE_GSM_WCDMA, SIT_NET_DS_NET_TYPE_LTE_GSM_WCDMA, false },
        { PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, SIT_NET_DS_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA, false },
        { PREF_NET_TYPE_LTE_ONLY, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_LTE_WCDMA, SIT_NET_DS_NET_TYPE_LTE_GSM_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_ONLY, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_WCDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_LTE, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM, SIT_NET_DS_NET_TYPE_GSM_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM_LTE, SIT_NET_DS_NET_TYPE_LTE_GSM_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA, SIT_NET_DS_NET_TYPE_GSM_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE, SIT_NET_DS_NET_TYPE_LTE_GSM_WCDMA, false },
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA, SIT_NET_DS_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA, false },
        { PREF_NET_TYPE_NR_ONLY, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE_CDMA_EVDO, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE_GSM_WCDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE_WCDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_CDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_CDMA_NO_EVDO, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_CDMA_EVDO_LTE, SIT_NET_DS_NET_TYPE_GSM, false },
        { PREF_NET_TYPE_TD_SCDMA_EVDO_NO_CDMA, SIT_NET_DS_NET_TYPE_GSM, false },
        { 100, SIT_NET_DS_NET_TYPE_GSM, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkTypeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetDSNetworkType(testVector[i].rilNetType);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_DS_NTW_TYPE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkTypeBuilderJson, BuildSetDualNetworkAndAllowData) {
    struct {
        int rilPrimaryNetType;
        int cpPrimaryNetType;
        int rilSecondaryNetType;
        int cpSecondaryNetType;
        int allowedForPrimary;
        int allowedForSecondary;
        bool isModemDataNull;
    } testVector[] = {
        { PREF_NET_TYPE_NR_LTE_CDMA_EVDO, SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO,
            PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA,
            SIT_NET_DISABLE, SIT_NET_DISABLE, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkTypeBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetDualNetworkAndAllowData(testVector[i].rilPrimaryNetType, testVector[i].rilSecondaryNetType,
                                                    testVector[i].allowedForPrimary, testVector[i].allowedForSecondary);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_DUAL_NTW_AND_PS_TYPE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    int primaryNetType = reqData[ns_net_set_dual_ntw_and_ps_type_req::key_pri_pref_net_type].asInt();
                    int secondaryNetType = reqData[ns_net_set_dual_ntw_and_ps_type_req::key_sec_pref_net_type].asInt();
                    int primaryCapability = reqData[ns_net_set_dual_ntw_and_ps_type_req::key_pri_ps_capa].asInt();
                    int secondaryCapability = reqData[ns_net_set_dual_ntw_and_ps_type_req::key_sec_ps_capa].asInt();

                    EXPECT_EQ(primaryNetType, testVector[i].cpPrimaryNetType);
                    EXPECT_EQ(secondaryNetType, testVector[i].cpSecondaryNetType);
                    EXPECT_EQ(primaryCapability, testVector[i].allowedForPrimary);
                    EXPECT_EQ(secondaryCapability, testVector[i].allowedForSecondary);
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

/*
 * ProtocolCdmaBuilderJson class
 */
TEST(ProtocolCdmaBuilderJson, BuildSetCdmaSetRoamingType) {
    struct {
        int cdmaRoamingType;
        int sitCdmaRoamingType;
        bool isModemDataNull;
    } testVector[] = {
        { CDMA_ROAMING_HOME_ONLY, SIT_CDMA_RM_HOME, false},
        { CDMA_ROAMING_AFFILIATED_NETWORKS, SIT_CDMA_RM_AFFILIATED, false},
        { CDMA_ROAMING_ANY_NETWORK, SIT_CDMA_RM_ANY, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolCdmaBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetCdmaSetRoamingType(testVector[i].cdmaRoamingType);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_CDMA_ROAMING_PREFERENCE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolCdmaBuilderJson, BuildQueryCdmaRoamingType) {
    ProtocolCdmaBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildQueryCdmaRoamingType();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CDMA_ROAMING_PREFERENCE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolCdmaBuilderJson, BuildSetCdmaHybridMode) {
    struct {
        int hybridMode;
        bool isModemDataNull;
    } testVector[] = {
        { -1, true},
        { HYBRID_MODE_1X_HRPD, false},
        { HYBRID_MODE_HRPD_ONLY, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolCdmaBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetCdmaHybridMode(testVector[i].hybridMode);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_CDMA_HYBRID_MODE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolCdmaBuilderJson, BuildGetCdmaHybridMode) {
    ProtocolCdmaBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetCdmaHybridMode();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CDMA_HYBRID_MODE);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

/*
 * ProtocolNetworkBuilderJson class
 */
TEST(ProtocolNetworkBuilderJson, BuildGetCellInfoList) {
    ProtocolNetworkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetCellInfoList();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CELL_INFO_LIST);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkBuilderJson, BuildSetCellInfoListReportRate) {
    struct {
        int rate;
        bool isModemDataNull;
    } testVector[] = {
        { -1, false},
        { 10, false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetCellInfoListReportRate(testVector[i].rate);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_CELL_INFO_LIST_REPORT_RATE);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkBuilderJson, BuildSetRCNetworkType) {
    char uuid[2][10] = {"modem1", "modem2"};
    struct {
        int rcRaf;
        char *pUuid;
        bool isModemDataNull;
    } testVector[] = {
        { RAF_UNKNOWN, nullptr, false},
        { RAF_GPRS,    uuid[0], false},
        { RAF_EDGE,    uuid[1], false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetRCNetworkType(1, 1, 1, testVector[i].rcRaf, testVector[i].pUuid, 1);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_RADIO_CAPABILITY);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkBuilderJson, BuildGetRCNetworkType) {
    ProtocolNetworkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetRCNetworkType();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_RADIO_CAPABILITY);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkBuilderJson, BuildSvNumber) {
    char svn[2][10] = {"12", "23"};
    struct {
        char *pSvn;
        bool isModemDataNull;
    } testVector[] = {
        { nullptr, false},
        { svn[0],  false},
        { svn[1],  false},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSvNumber(testVector[i].pSvn);
/* build fucntion is not implemented yet, so blocks test fuction */
/*
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_OEM_SET_SVN);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }
*/
        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkBuilderJson, BuildGetSystemSelectionChannels) {
    ProtocolNetworkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetSystemSelectionChannels();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_SYSTEM_SELECTION_CHANNELS);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkBuilderJson, BuildSetAllowedNetworkTypeBitmap) {
    struct {
        int networkTypeBitmap;
        bool isModemDataNull;
    } testVector[] = {
        { NETWORK_TYPE_BITMAP_GSM, false },
        { NETWORK_TYPE_BITMAP_CDMA, false },
        { NETWORK_TYPE_BITMAP_EVDO, false },
        { NETWORK_TYPE_BITMAP_HS, false },
        { NETWORK_TYPE_BITMAP_WCDMA, false },
        { NETWORK_TYPE_BITMAP_LTE, false },
        { NETWORK_TYPE_BITMAP_NR, false },
        { NETWORK_TYPE_BITMAP_TDS_CDMA, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetAllowedNetworkTypeBitmap(testVector[i].networkTypeBitmap);
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_ALLOW_NETWORK);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }

        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkBuilderJson, BuildGetAllowedNetworkTypeBitmap) {
    ProtocolNetworkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetAllowedNetworkTypeBitmap();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_ALLOW_NETWORK);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

TEST(ProtocolNetworkBuilderJson, BuildSetPlmnList) {
    const int sampleDataLen = 5;
    BYTE sampleData[sampleDataLen] {1,2,3,4,5};
    struct {
        int requestId;
        void *data;
        unsigned int datalen;
        bool isModemDataNull;
    } testVector[] = {
        { -1, nullptr, 0, true },
        { 2, sampleData, sampleDataLen, false },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetPlmnList(testVector[i].requestId,
                                    testVector[i].data, testVector[i].datalen);
/* build fucntion is not implemented yet, so blocks test fuction */
/*
        EXPECT_EQ(modemData == nullptr, testVector[i].isModemDataNull);

        if (modemData != nullptr && testVector[i].isModemDataNull == false) {
            ProtocolJsonReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), SIT_SET_PLMN_LIST);
            EXPECT_NE(testAdapter.GetParameter(), nullptr);

            // Checking payload
            if (testAdapter.GetParameter() != nullptr) {
                Json::Value reqData;
                bool parsed = ParsingJsonPayload(testAdapter.GetParameter(), reqData, testAdapter.GetId());
                if(parsed) {
                    // need to implement
                }
            }
        }
*/
        if (modemData) { delete modemData; modemData = nullptr; }
    }
}

TEST(ProtocolNetworkBuilderJson, BuildGetPlmnList) {
    ProtocolNetworkBuilderJson testBuilder;
    ModemData *modemData = testBuilder.BuildGetPlmnList();
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PLMN_LIST);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) { delete modemData; modemData = nullptr; }
}

/******************************************************************************
 * PS
 *****************************************************************************/
TEST(ProtocolNetworkBuilderJson, BuildSetMobileDataState) {

    int mobileData = 1;
    int roamingData = 0;

    ProtocolNetworkBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildSetMobileDataState(mobileData, roamingData));
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_MOBILE_DATA_STATE);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());

    ASSERT_NE(false, parsed);

    EXPECT_EQ(mobileData, jsonReqData[ns_net_ps_set_mobile_data_state_req::key_mobile_data].asInt());
    EXPECT_EQ(roamingData, jsonReqData[ns_net_ps_set_mobile_data_state_req::key_roaming_data].asInt());
}

