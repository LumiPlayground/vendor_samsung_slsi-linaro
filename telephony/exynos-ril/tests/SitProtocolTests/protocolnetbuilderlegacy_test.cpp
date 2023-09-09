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
#include "rildef.h"
#include "protocoladapter.h"
#include "legacy/network/protocolnetbuilderlegacy.h"
#include <algorithm>

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildNetworkRegistrationState) {
    struct {
        int domain;
        unsigned int id;
    } testVector[] = {
        {NETWORK_DOMAIN_UNKNOWN, (unsigned int)-1},
        {NETWORK_DOMAIN_CS, SIT_GET_CS_REG_STATE},
        {NETWORK_DOMAIN_PS, SIT_GET_PS_REG_STATE},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkRegistrationBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildNetworkRegistrationState(testVector[i].domain);
        if (testVector[i].domain == NETWORK_DOMAIN_UNKNOWN) {
            EXPECT_EQ(modemData, nullptr);
        }
        else {
            ASSERT_NE(modemData, nullptr);
            EXPECT_NE(modemData, nullptr);

            ProtocolReqAdapter testAdapter(modemData);
            EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
            EXPECT_EQ(testAdapter.GetId(), testVector[i].id);
            EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_cs_reg_state_req));
            EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildOperator) {
    ProtocolNetworkRegistrationBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildOperator();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_OPERATOR);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_operator_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildRadioPower) {
    struct {
        int powerState;
        int forEmergencyCall;
        int preferredForEmergencyCall;
    } testVector[] = {
        { 0, 0, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
        { 1, 0, 0 },
        { 1, 0, 1 },
        { 1, 1, 1 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolRadioPowerBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildRadioPower(testVector[i].powerState,
                testVector[i].forEmergencyCall, testVector[i].preferredForEmergencyCall);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_RADIO_POWER);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_pwr_set_radio_power_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 6U);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_pwr_set_radio_power_req *p = (sit_pwr_set_radio_power_req *)modemData->GetRawData();
        if (testVector[i].powerState == 0) {
            EXPECT_EQ(p->radio_state, SIT_PWR_RADIO_STATE_STOP_NETWORK);
        } else {
            EXPECT_EQ(p->radio_state, SIT_PWR_RADIO_STATE_START_NETWORK);
        }
        EXPECT_EQ(p->for_emc, testVector[i].forEmergencyCall);
        EXPECT_EQ(p->prefered_for_emc, testVector[i].preferredForEmergencyCall);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildShutdown) {
    ProtocolRadioPowerBuilderLegacy testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildShutdown());
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_RADIO_POWER);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_pwr_set_radio_power_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 6U);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_pwr_set_radio_power_req *p = (sit_pwr_set_radio_power_req *)modemData->GetRawData();
    EXPECT_EQ(p->radio_state, SIT_PWR_RADIO_STATE_POWER_OFF);
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetRadioState) {
    ProtocolRadioPowerBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetRadioState();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_RADIO_POWER);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_pwr_get_radio_power_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildQueryNetworkSelectionMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildQueryNetworkSelectionMode();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_NTW_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetNetworkSelectionAuto) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetNetworkSelectionAuto();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_NTW_MODE_AUTO);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_network_mode_auto_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetNetworkSelectionManual) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = nullptr;

    // invalid plmn (null)
    modemData = testBuilder.BuildSetNetworkSelectionManual(RADIO_TECH_LTE, 0);
    EXPECT_EQ(modemData, nullptr);
    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }

    // invalid plmn (null-string)
    modemData = testBuilder.BuildSetNetworkSelectionManual(RADIO_TECH_LTE, "");
    EXPECT_EQ(modemData, nullptr);
    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }

    // invalid plmn length (smaller than 5)
    modemData = testBuilder.BuildSetNetworkSelectionManual(RADIO_TECH_LTE, "123");
    EXPECT_EQ(modemData, nullptr);
    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }

    // invalid plmn length (larger than 6)
    modemData = testBuilder.BuildSetNetworkSelectionManual(RADIO_TECH_LTE, "1234567");
    EXPECT_EQ(modemData, nullptr);
    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }

    // normal
    const char *testVector[] = { "45005", "311460", "00101", "001010" };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        modemData = testBuilder.BuildSetNetworkSelectionManual(RADIO_TECH_LTE, testVector[i]);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_NTW_MODE_MANUAL);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_metwork_mode_manual_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 10U);
        EXPECT_NE(testAdapter.GetParameter(), nullptr);
        sit_net_set_metwork_mode_manual_req *p = (sit_net_set_metwork_mode_manual_req *)modemData->GetRawData();
        EXPECT_EQ(p->rat, SIT_RAT_TYPE_LTE);
        if (strlen(testVector[i]) == 5) {
            EXPECT_EQ(p->plmn[5], '#');
            EXPECT_TRUE(memcmp(p->plmn, testVector[i], 5) == 0);
        } else {
            EXPECT_NE(p->plmn[5], '#');
            EXPECT_TRUE(memcmp(p->plmn, testVector[i], 6) == 0);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildQueryAvailableNetwork_with_ran) {
    struct {
        int ran;
        int result;
    } testVector[] = {
        { RIL_RAN_GERAN, SIT_NET_ACCESS_RADIO_TYPE_GERAN },
        { RIL_RAN_UTRAN, SIT_NET_ACCESS_RADIO_TYPE_UTRAN },
        { RIL_RAN_EUTRAN, SIT_NET_ACCESS_RADIO_TYPE_EUTRAN },
        { RIL_RAN_NGRAN, SIT_NET_ACCESS_RADIO_TYPE_NGRAN },
        { RIL_RAN_CDMA2000, SIT_NET_ACCESS_RADIO_TYPE_CDMA2000 },
        { -1, SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkScanBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildQueryAvailableNetwork(testVector[i].ran);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_AVAILABLE_NETWORKS);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_available_networks_wit_rat_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 4U);
        sit_net_get_available_networks_wit_rat_req *p = (sit_net_get_available_networks_wit_rat_req *)modemData->GetRawData();
        EXPECT_EQ(p->ran, testVector[i].result);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildCancelQueryAvailableNetwork) {
    ProtocolNetworkScanBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildCancelQueryAvailableNetwork();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_CANCEL_GET_AVAILABLE_NETWORKS);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_cancel_get_available_networks_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetBandMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetBandMode(3);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_BAND_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_band_mode_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 4U);
    sit_net_set_band_mode_req *p = (sit_net_set_band_mode_req *)modemData->GetRawData();
    EXPECT_EQ(p->band, 3);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildQueryAvailableBandMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildQueryAvailableBandMode();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BAND_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_band_mode_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetPreferredNetworkType) {
    struct {
        int rilNetType;
        int result;
    } testVector[] = {
        { PREF_NET_TYPE_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_GSM_WCDMA },
        { PREF_NET_TYPE_GSM_ONLY, SIT_NET_PREF_NET_TYPE_GSM_ONLY },
        { PREF_NET_TYPE_WCDMA, SIT_NET_PREF_NET_TYPE_WCDMA },
        { PREF_NET_TYPE_GSM_WCDMA_AUTO, SIT_NET_PREF_NET_TYPE_GSM_WCDMA_AUTO },
        { PREF_NET_TYPE_CDMA_EVDO_AUTO, SIT_NET_PREF_NET_TYPE_CDMA_EVDO_AUTO },
        { PREF_NET_TYPE_CDMA_ONLY, SIT_NET_PREF_NET_TYPE_CDMA_ONLY },
        { PREF_NET_TYPE_EVDO_ONLY, SIT_NET_PREF_NET_TYPE_EVDO_ONLY },
        { PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO, SIT_NET_PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO },
        { PREF_NET_TYPE_LTE_CDMA_EVDO, SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO },
        { PREF_NET_TYPE_LTE_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_GSM_WCDMA },
        { PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA },
        { PREF_NET_TYPE_LTE_ONLY, SIT_NET_PREF_NET_TYPE_LTE_ONLY },
        { PREF_NET_TYPE_LTE_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_WCDMA },
        { PREF_NET_TYPE_TD_SCDMA_ONLY, SIT_NET_PREF_NET_TYPE_TDSCDMA_ONLY },
        { PREF_NET_TYPE_TD_SCDMA_WCDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_WCDMA },
        { PREF_NET_TYPE_TD_SCDMA_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA },
        { PREF_NET_TYPE_TD_SCDMA_GSM, SIT_NET_PREF_NET_TYPE_TDSCDMA_GSM },
        { PREF_NET_TYPE_TD_SCDMA_GSM_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_GSM },
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_GSM_WCDMA },
        { PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_WCDMA },
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_GSM_WCDMA },
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO, SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA_EVDO_GSM_WCDMA },
        { PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA },
        { PREF_NET_TYPE_NR_ONLY, SIT_NET_PREF_NET_TYPE_NR_ONLY },
        { PREF_NET_TYPE_NR_LTE, SIT_NET_PREF_NET_TYPE_NR_LTE },
        { PREF_NET_TYPE_NR_LTE_CDMA_EVDO, SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA },
        { PREF_NET_TYPE_NR_LTE_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA },
        { PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA },
        { PREF_NET_TYPE_NR_LTE_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_WCDMA },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA },
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA },
        { PREF_NET_TYPE_TD_SCDMA_CDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA },
        { PREF_NET_TYPE_TD_SCDMA_CDMA_NO_EVDO, SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA_NO_EVDO },
        { PREF_NET_TYPE_TD_SCDMA_CDMA_EVDO_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_CDMA_EVDO },
        { PREF_NET_TYPE_TD_SCDMA_EVDO_NO_CDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_EVDO_NO_CDMA },
        { 100, SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkTypeBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetPreferredNetworkType(
                testVector[i].rilNetType);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_PREFERRED_NTW_TYPE);
        EXPECT_EQ(testAdapter.GetLength(),
                sizeof(sit_net_set_pref_network_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 4U);
        sit_net_set_pref_network_req *p =
                (sit_net_set_pref_network_req*) modemData->GetRawData();
        EXPECT_EQ(p->pref_net_type, testVector[i].result);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetPreferredNetworkType) {
    ProtocolNetworkTypeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetPreferredNetworkType();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PREFERRED_NTW_TYPE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetCellInfoList) {
    ProtocolNetworkBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetCellInfoList();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CELL_INFO_LIST);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_cell_info_list_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetCellInfoListReportRate) {
    struct {
        int rate;
        int result;
    } testVector[] = {
        { 1000, 1000 },
        { 777, 777 },
        { -1, 0x7FFFFFFF },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetCellInfoListReportRate(testVector[i].rate);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_CELL_INFO_LIST_REPORT_RATE);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_cell_info_list_report_rate_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 4U);
        sit_net_cell_info_list_report_rate_req *p = (sit_net_cell_info_list_report_rate_req *)modemData->GetRawData();
        EXPECT_EQ(p->report_rate, testVector[i].result);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildAllowData) {
    struct {
        int allow;
        int result;
    } testVector[] = {
        { ALLOW_DATA_CALL, 1 },
        { DISALLOW_DATA_CALL, 0 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetAllowData(testVector[i].allow);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_PS_SERVICE);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_ps_service_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 1U);
        sit_net_set_ps_service_req *p = (sit_net_set_ps_service_req *)modemData->GetRawData();
        EXPECT_EQ(p->state, testVector[i].result);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetPsService) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetPsService();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_PS_SERVICE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_ps_service_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetRCNetworkType) {
    ProtocolNetworkBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetRCNetworkType();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_RADIO_CAPABILITY);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_rc_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetDuplexMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetDuplexMode(1, 2);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_DUPLEX_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_duplex_mode_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 2U);
    sit_net_set_duplex_mode_req *p = (sit_net_set_duplex_mode_req *)modemData->GetRawData();
    EXPECT_EQ(p->duplex_mode_4g, 1);
    EXPECT_EQ(p->duplex_mode_3g, 2);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetDuplexMode_preset) {
    struct {
        int mode;
        int result4g;
        int result3g;
    } testVector[] = {
        { DUPLEX_MODE_GLOBAL, SIT_NET_DUPLEX_MODE_FDD_TDD, SIT_NET_DUPLEX_MODE_FDD_TDD },
        { DUPLEX_MODE_LTG, SIT_NET_DUPLEX_MODE_TDD, SIT_NET_DUPLEX_MODE_TDD },
        { DUPLEX_MODE_LWG, SIT_NET_DUPLEX_MODE_FDD_TDD, SIT_NET_DUPLEX_MODE_FDD },
        { DUPLEX_MODE_INVALID, SIT_NET_DUPLEX_MODE_FDD_TDD, SIT_NET_DUPLEX_MODE_FDD_TDD },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetDuplexMode(testVector[i].mode);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_DUPLEX_MODE);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_duplex_mode_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 2U);
        sit_net_set_duplex_mode_req *p = (sit_net_set_duplex_mode_req *)modemData->GetRawData();
        EXPECT_EQ(p->duplex_mode_4g, testVector[i].result4g);
        EXPECT_EQ(p->duplex_mode_3g, testVector[i].result3g);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetDuplexMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetDuplexMode();
    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_DUPLEX_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetEmergencyCallStatus) {
    struct {
        int status;
        int rat;
        int result;
    } testVector[] = {
        { 1, RADIO_TECH_GSM, RADIO_TECH_GSM },
        { 1, RADIO_TECH_TD_SCDMA, SIT_RAT_TYPE_TD_SCDMA },
        { 1, RADIO_TECH_IWLAN, SIT_RAT_TYPE_IWLAN },
        { 1, RADIO_TECH_NR, SIT_RAT_TYPE_5G },
        { 1, RADIO_TECH_LTE_CA, SIT_RAT_TYPE_LTE },
        { 1, RADIO_TECH_UNSPECIFIED, SIT_RAT_TYPE_UNSPECIFIED },
        { 1, -1, SIT_RAT_TYPE_UNSPECIFIED },
        { 2, RADIO_TECH_GSM, RADIO_TECH_GSM },
        { 2, RADIO_TECH_TD_SCDMA, SIT_RAT_TYPE_TD_SCDMA },
        { 2, RADIO_TECH_IWLAN, SIT_RAT_TYPE_IWLAN },
        { 3, RADIO_TECH_NR, SIT_RAT_TYPE_5G },
        { 3, RADIO_TECH_LTE_CA, SIT_RAT_TYPE_LTE },
        { 3, RADIO_TECH_UNSPECIFIED, SIT_RAT_TYPE_UNSPECIFIED },
        { 4, -1, SIT_RAT_TYPE_UNSPECIFIED },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetEmergencyCallStatus(
                testVector[i].status, testVector[i].rat);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_EMERGENCY_CALL_STATUS);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_emergency_call_status_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 2U);
        sit_net_set_emergency_call_status_req *p = (sit_net_set_emergency_call_status_req *)modemData->GetRawData();
        EXPECT_EQ(p->status, testVector[i].status);
        EXPECT_EQ(p->rat, testVector[i].result);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}


TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetMicroCellSearch) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetMicroCellSearch(SIT_MC_SRCH_MODE_AUTO);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_MICRO_CELL_SEARCH);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_micro_cell_search_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 1U);
    sit_net_set_micro_cell_search_req *p = (sit_net_set_micro_cell_search_req *)modemData->GetRawData();
    EXPECT_EQ(p->srch_mode, SIT_MC_SRCH_MODE_AUTO);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildRestartModem) {
    ProtocolRadioPowerBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildRestartModem();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_RADIO_POWER);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_pwr_set_radio_power_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 6U);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_pwr_set_radio_power_req *p = (sit_pwr_set_radio_power_req *)modemData->GetRawData();
    EXPECT_EQ(p->radio_state, SIT_PWR_RADIO_STATE_RESET);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetCdmaSetRoamingType) {
    struct {
        int type;
        int result;
    } testVector[] = {
        { CDMA_ROAMING_AFFILIATED_NETWORKS, SIT_CDMA_RM_AFFILIATED },
        { CDMA_ROAMING_ANY_NETWORK, SIT_CDMA_RM_ANY },
        { CDMA_ROAMING_HOME_ONLY, SIT_CDMA_RM_HOME },
        { -1, SIT_CDMA_RM_HOME },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolCdmaBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetCdmaSetRoamingType(testVector[i].type);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_CDMA_ROAMING_PREFERENCE);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_cdma_roaming_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 1U);
        sit_net_set_cdma_roaming_req *p = (sit_net_set_cdma_roaming_req *)modemData->GetRawData();
        EXPECT_EQ(p->cdma_roaming_type, testVector[i].result);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetCdmaHybridMode) {
    ProtocolCdmaBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetCdmaHybridMode(10);
    EXPECT_EQ(modemData, nullptr);
    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }

    modemData = testBuilder.BuildSetCdmaHybridMode(HYBRID_MODE_HRPD_ONLY);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_CDMA_HYBRID_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_cdma_hybrid_mode_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 1U);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);
    sit_net_set_cdma_hybrid_mode_req *p = (sit_net_set_cdma_hybrid_mode_req *)modemData->GetRawData();
    EXPECT_EQ(p->hybrid_mode, HYBRID_MODE_HRPD_ONLY);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetCdmaHybridMode) {
    ProtocolCdmaBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetCdmaHybridMode();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_CDMA_HYBRID_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildStartNetworkScan_oneshot) {
    // TBD
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildStopNetworkScan) {
    ProtocolNetworkScanBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildStopNetworkScan();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_START_SCANNING_NETWORKS);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + 10);
    EXPECT_EQ(testAdapter.GetParameterLength(), 10U);
    sit_net_start_scanning_network *p = (sit_net_start_scanning_network *)modemData->GetRawData();
    EXPECT_EQ(p->scan_type, SIT_SCAN_TYPE_STOP);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetBarringInfo) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetBarringInfo();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_BARRING_INFOS);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_barring_infos_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetManualRatMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetManualRatMode();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_MANUAL_RAT_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetManualRatMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetManualRatMode(1, 10);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_MANUAL_RAT_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_manual_rat_mode_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 5U);
    sit_net_set_manual_rat_mode_req *p = (sit_net_set_manual_rat_mode_req *)modemData->GetRawData();
    EXPECT_EQ(p->rat_mode_set, 1);
    EXPECT_EQ(p->rat, 10);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetFrequencyLock) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetFrequencyLock();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_FREQUENCY_LOCK);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetFrequencyLock) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetFrequencyLock(1, 2, 3, 4, 5, 6, 7);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_FREQUENCY_LOCK);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_freq_lock_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 22U);
    sit_net_set_freq_lock_req *p = (sit_net_set_freq_lock_req *)modemData->GetRawData();
    EXPECT_EQ(p->freq_mode_set, 1);
    EXPECT_EQ(p->rat, 2);
    EXPECT_EQ(p->lte_pci, 3);
    EXPECT_EQ(p->lte_earfcn, 4);
    EXPECT_EQ(p->gsm_arfcn, 5);
    EXPECT_EQ(p->wcdma_psc, 6);
    EXPECT_EQ(p->wcdma_uarfcn, 7);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetEndcMode) {
    struct {
        int mode;
        int result1;
        int result2;
    } testVector[] = {
        { 0, 0, 0 },
        { 1, 1, 0 },
        { 2, 0, 1 },
        { -1, 0, 0 },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkModeBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetEndcMode(testVector[i].mode);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_ENDC_MODE);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_endc_mode_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 2U);
        sit_net_set_endc_mode_req *p = (sit_net_set_endc_mode_req *)modemData->GetRawData();
        EXPECT_EQ(p->mode, testVector[i].result1);
        EXPECT_EQ(p->immediate_scg_rel, testVector[i].result2);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetEndcMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetEndcMode();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_ENDC_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetFrequencyInfo) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetFrequencyInfo();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_FREQUENCY_INFO);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetNrMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildSetNrMode(3);
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_NR_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_nr_mode_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 1U);
    sit_net_set_nr_mode_req *p = (sit_net_set_nr_mode_req *)modemData->GetRawData();
    EXPECT_EQ(p->mode, 3);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetNrMode) {
    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetNrMode();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_NR_MODE);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetSysSelChannels) {
    const int sizOfRasArray = 4;
    RIL_RadioAccessSpecifier_V1_5 geran = {
        .radio_access_network = RIL_RAN_GERAN,
        .bands_length = 4,
        .bands.geran_bands = {
            GERAN_BAND_T380,
            GERAN_BAND_T410,
            GERAN_BAND_450,
            GERAN_BAND_480,
        },
        .channels_length = 10,
        .channels = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    };
    RIL_RadioAccessSpecifier_V1_5 utran = {
        .radio_access_network = RIL_RAN_UTRAN,
        .bands_length = 4,
        .bands.utran_bands = {
            UTRAN_BAND_6,
            UTRAN_BAND_7,
            UTRAN_BAND_8,
            UTRAN_BAND_9,
        },
        .channels_length = 15,
        .channels = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    };
    RIL_RadioAccessSpecifier_V1_5 eutran = {
        .radio_access_network = RIL_RAN_EUTRAN,
        .bands_length = 4,
        .bands.eutran_bands = {
            EUTRAN_BAND_14,
            EUTRAN_BAND_17,
            EUTRAN_BAND_18,
            EUTRAN_BAND_19,
        },
        .channels_length = 20,
        .channels = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209},
    };
    RIL_RadioAccessSpecifier_V1_5 ngran = {
        .radio_access_network = (RIL_RadioAccessNetworks)RIL_RAN_NGRAN,
        .bands_length = 4,
        .bands.ngran_bands = {
            NGRAN_BAND_76,
            NGRAN_BAND_77,
            NGRAN_BAND_78,
            NGRAN_BAND_79,
        },
        .channels_length = 32,
        .channels = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                     200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
                     1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009,
                     3200, 4000},
    };
    RIL_RadioAccessSpecifier_V1_5 rasArray[sizOfRasArray] = {geran, utran, eutran, ngran};

    struct {
        bool isSpecifyChannel;
        int specifiersLength;
        RIL_RadioAccessSpecifier_V1_5 *pSpecifiers;
    } testVector[] = {
        {true, sizOfRasArray, rasArray},
    };

    auto IsSameRas = [](RIL_RadioAccessSpecifier_V1_5 &ril, sit_net_radio_access_specifier &sit) -> bool {
        if ((int)ril.radio_access_network != (int)sit.radio_access_network)
            return false;
        if ((int)ril.bands_length != (int)sit.bands_len)
            return false;
        if ((int)ril.channels_length != (int)sit.channels_len)
            return false;

        bool isBandsSame = 0, isChannelsSame = 0;
        switch (sit.radio_access_network) {
        case SIT_NET_ACCESS_RADIO_TYPE_GERAN:
            isBandsSame = std::equal(ril.bands.geran_bands, ril.bands.geran_bands + ril.bands_length, sit.bands.geran_bands);
            isChannelsSame = std::equal(ril.channels, ril.channels + ril.channels_length, sit.channels.geran_channels);
            break;
        case SIT_NET_ACCESS_RADIO_TYPE_UTRAN:
            isBandsSame = std::equal(ril.bands.utran_bands, ril.bands.utran_bands + ril.bands_length, sit.bands.utran_bands);
            isChannelsSame = std::equal(ril.channels, ril.channels + ril.channels_length, sit.channels.utran_channels);
            break;
        case SIT_NET_ACCESS_RADIO_TYPE_EUTRAN:
            isBandsSame = std::equal(ril.bands.eutran_bands, ril.bands.eutran_bands + ril.bands_length, sit.bands.eutran_bands);
            isChannelsSame = std::equal(ril.channels, ril.channels + ril.channels_length, sit.channels.eutran_channels);
            break;
        case SIT_NET_ACCESS_RADIO_TYPE_NGRAN:
            isBandsSame = std::equal(ril.bands.ngran_bands, ril.bands.ngran_bands + ril.bands_length, sit.bands.ngran_bands);
            isChannelsSame = std::equal(ril.channels, ril.channels + ril.channels_length, sit.channels.ngran_channels);
            break;
        }
        return isBandsSame && isChannelsSame;
    };

    ProtocolNetworkModeBuilderLegacy testBuilder;
    for (auto testIn : testVector) {
        auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildSetSysSelChannels(testIn.isSpecifyChannel, testIn.specifiersLength, testIn.pSpecifiers));
        ASSERT_TRUE(modemData != nullptr);

        ProtocolReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_SYSTEM_SELECTION_CHANNELS);
        EXPECT_TRUE(testAdapter.GetLength() != 0);
        EXPECT_TRUE(testAdapter.GetParameterLength() != 0U);
        EXPECT_TRUE(testAdapter.GetParameter() != nullptr);

        sit_net_set_system_selection_channels_req *p = (sit_net_set_system_selection_channels_req *)modemData->GetRawData();
        ASSERT_TRUE(p != nullptr);
        EXPECT_EQ(testIn.isSpecifyChannel, (bool)p->is_specific_channel);
        EXPECT_EQ(testIn.specifiersLength, (int)p->radio_access_specifier_len);
        for (int i = 0; i < testIn.isSpecifyChannel; ++i) {
            EXPECT_TRUE(IsSameRas(*(testIn.pSpecifiers + i), *(p->radio_access_specifier_array + i)));
        }
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSystemSelectionChannels) {
    ProtocolNetworkBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetSystemSelectionChannels();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_SYSTEM_SELECTION_CHANNELS);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetAllowedNetworkTypeBitmap) {
    struct {
        int typebitmap;
        int result;
    } testVector[] = {
        { RAF_GPRS | RAF_UMTS | RAF_1xRTT | RAF_LTE, RAF_CP_GPRS | RAF_CP_UMTS | RAF_CP_1xRTT | RAF_CP_LTE },
        { RAF_GPRS | RAF_UMTS | RAF_1xRTT | RAF_LTE_CA | RAF_TD_SCDMA, RAF_CP_GPRS | RAF_CP_UMTS | RAF_CP_1xRTT | RAF_CP_LTE | RAF_CP_TD_SCDMA },
        { RAF_LTE | RAF_NR, RAF_CP_LTE | RAF_CP_5G },
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolNetworkBuilderLegacy testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildSetAllowedNetworkTypeBitmap(testVector[i].typebitmap);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_ALLOW_NETWORK);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_allow_network_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), 4U);
        sit_net_set_allow_network_req *p = (sit_net_set_allow_network_req *)modemData->GetRawData();
        EXPECT_EQ(p->rat, testVector[i].result);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }

    }  // end for i ~
}

TEST(ProtocolNetworkBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetAllowedNetworkTypeBitmap) {
    ProtocolNetworkBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetAllowedNetworkTypeBitmap();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_ALLOW_NETWORK);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_get_allow_network_req));
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetworkModeBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildSetVonrCapa) {
    TokenGen::Init();

    ProtocolNetworkModeBuilderLegacy testBuilder;
    for (int i = 0; i <= 1; i++) {
        ModemData *modemData = testBuilder.BuildSetVonrCapa(i);
        ASSERT_NE(modemData, nullptr);
        EXPECT_NE(modemData, nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_VONR_CAPABILITY);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(sit_net_set_vonr_capa_req));
        EXPECT_EQ(testAdapter.GetParameterLength(), (uint32_t)4);
        sit_net_set_vonr_capa_req *p = (sit_net_set_vonr_capa_req *)modemData->GetRawData();
        EXPECT_EQ(p->vonr_capa, i);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }
}

TEST(ProtocolNetworkModeBuilderLegacy, ProtocolNetworkBuilderLegacy_BuildGetVonrCapa) {
    TokenGen::Init();

    ProtocolNetworkModeBuilderLegacy testBuilder;
    ModemData *modemData = testBuilder.BuildGetVonrCapa();
    ASSERT_NE(modemData, nullptr);
    EXPECT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData);
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_VONR_CAPABILITY);
    EXPECT_EQ(testAdapter.GetLength(), sizeof(null_data_format));
    EXPECT_EQ(testAdapter.GetParameterLength(), (uint32_t)0);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}