/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <testutils.h>
#include <modemdata.h>
#include <modemdatabuilder.h>
#include <protocolenv.h>
#include <slsi/radio_v2_0.h>
#include "protocolmiscadapter.h"
#include "sitdef.h"

struct ProtocolTypeSetting {
    ProtocolTypeSetting() {
        ProtocolEnv::overrideEnv({ProtocolPayloadType::STRUCT});
    }
    ~ProtocolTypeSetting() {
        ProtocolEnv::overrideEnv(ProtocolEnv::getDefaultEnv());
    }
    bool isLegacyOnly() {
        return ProtocolEnv::getEnv().isLegacyOnly();
    }
};

#define DECLARE_TEST_ON_LEGACY() \
ProtocolTypeSetting testEvn; \
ASSERT_TRUE(testEvn.isLegacyOnly()); \

TEST(ProtocolMiscAdapter, ProtocolMiscResponseAdapter) {
    DECLARE_TEST_ON_LEGACY()

    struct Test {
        uint32_t sitErrorCode;
        uint32_t errorCode;
    };
    std::vector<Test> testVector = {
        {RCM_E_SUCCESS, RIL_E_SUCCESS},
        {RCM_E_RADIO_NOT_AVAILABLE, RIL_E_RADIO_NOT_AVAILABLE},
        {RCM_E_GENERIC_FAILURE, RCM_E_GENERIC_FAILURE},
        {RCM_E_SIM_ABSENT, RIL_E_SIM_ABSENT},
    };
    for (auto& t : testVector) {
        auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildResponse(
                SIT_GET_SIGNAL_STRENGTH, 0, t.sitErrorCode));
        ProtocolMiscResponseAdapter testAdapter(modemData.get());
        EXPECT_TRUE(testAdapter.GetErrorCode() == t.errorCode);
    }
}

TEST(ProtocolMiscAdapter, ProtocolSignalStrengthIndAdapter) {
    DECLARE_TEST_ON_LEGACY()

    SIGNAL_STRENGTH_V4 ss = {
        .GSM_SignalStrength = { 30, 5, 0 },
        .WCDMA_SignalStrength = { 25, 3, 80, 45 },
        .CDMA_SignalStrength = { 70, 120 },
        .EVDO_SignalStrength = { 65, 110, 8 },
        .TD_SCDMA_SignalStrength = { 27, 7, 90 },
        .LTE_SignalStrength = { 31, 50, 10, 100, 3, 14, 0 },
        .NR_SignalStrength = { 100, 101, 102, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 1, 7,
                { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                  INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                  INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX,
                  INT_MAX, INT_MAX, INT_MAX, INT_MAX } }
    };
    sit_misc_signal_strength_ind_v4 testVector {};
    size_t size = sizeof(sit_misc_signal_strength_ind_v4) - sizeof(RCM_IND_HEADER);
    testVector.valid_rat_sig_flag = SIT_MISC_SIG_RAT_SIG_LTE_V1_4;
    memcpy(&testVector.sig_strength, &ss, sizeof(ss));
    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildIndication(SIT_IND_SIGNAL_STRENGTH,
                                ((sit_simple_ind_data_format *)&testVector)->data, size));
    ProtocolSignalStrengthIndAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
}

TEST(ProtocolMiscAdapter, ProtocolMiscNITZTimeAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_nitz_time_with_age testVector = {
        {0, 22, 2, 19, 12, 15, 30, 60, 0, 3, 0, {}}, 0,
    };
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_NITZ_TIME_RECEIVED,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscNITZTimeAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.TimeInfoType() == 0);
    EXPECT_TRUE(testAdapter.DayLightValid() == static_cast<int32_t>(testVector.daylight_valid));
    EXPECT_TRUE(testAdapter.Year() == static_cast<int32_t>(testVector.year));
    EXPECT_TRUE(testAdapter.Month() == static_cast<int32_t>(testVector.month));
    EXPECT_TRUE(testAdapter.Day() == static_cast<int32_t>(testVector.day));
    EXPECT_TRUE(testAdapter.Hour() == static_cast<int32_t>(testVector.hour));
    EXPECT_TRUE(testAdapter.Minute() == static_cast<int32_t>(testVector.minute));
    EXPECT_TRUE(testAdapter.Second() == static_cast<int32_t>(testVector.second));
    EXPECT_TRUE(testAdapter.TimeZone() == static_cast<int32_t>(testVector.time_zone));
    EXPECT_TRUE(testAdapter.DayLightAdjust() == static_cast<int32_t>(testVector.daylight_adjust));
    EXPECT_TRUE(testAdapter.DayofWeek() == static_cast<int32_t>(testVector.day_of_week));
    EXPECT_TRUE(testAdapter.GetMMInfo() == static_cast<int32_t>(testVector.mminfo));
    EXPECT_TRUE(memcmp(testAdapter.GetPLMN(), testVector.plmn, MAX_PLMN_LEN) == 0);
    EXPECT_TRUE(testAdapter.age() == static_cast<int64_t>(testVector.daylight_valid));
}

TEST(ProtocolMiscAdapter, ProtocolMiscGetNITZTimeAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_nitz_time_with_age testVector = {
        {0, 22, 2, 19, 12, 15, 30, 60, 0, 3, 0, {}}, 0,
    };
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_GET_NITZ_TIME, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscGetNITZTimeAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.TimeInfoType() == 0);
    EXPECT_TRUE(testAdapter.DayLightValid() == static_cast<int32_t>(testVector.daylight_valid));
    EXPECT_TRUE(testAdapter.Year() == static_cast<int32_t>(testVector.year));
    EXPECT_TRUE(testAdapter.Month() == static_cast<int32_t>(testVector.month));
    EXPECT_TRUE(testAdapter.Day() == static_cast<int32_t>(testVector.day));
    EXPECT_TRUE(testAdapter.Hour() == static_cast<int32_t>(testVector.hour));
    EXPECT_TRUE(testAdapter.Minute() == static_cast<int32_t>(testVector.minute));
    EXPECT_TRUE(testAdapter.Second() == static_cast<int32_t>(testVector.second));
    EXPECT_TRUE(testAdapter.TimeZone() == static_cast<int32_t>(testVector.time_zone));
    EXPECT_TRUE(testAdapter.DayLightAdjust() == static_cast<int32_t>(testVector.daylight_adjust));
    EXPECT_TRUE(testAdapter.DayofWeek() == static_cast<int32_t>(testVector.day_of_week));
    EXPECT_TRUE(testAdapter.GetMMInfo() == static_cast<int32_t>(testVector.mminfo));
    EXPECT_TRUE(memcmp(testAdapter.GetPLMN(), testVector.plmn, MAX_PLMN_LEN) == 0);
    EXPECT_TRUE(testAdapter.age() == static_cast<int64_t>(testVector.daylight_valid));
}

TEST(ProtocolMiscAdapter, ProtocolMiscPhoneResetAdapter) {
    DECLARE_TEST_ON_LEGACY()

    struct {
        char resetType;
        char resetCause;
    } testVector[] = {
        { SIT_PWR_RESET_TYPE_PHONE_ONLY, 0 },
        { SIT_PWR_RESET_TYPE_PHONE_ONLY, 1 },
        { SIT_PWR_RESET_TYPE_RESET_BOTH, 0 },
        { SIT_PWR_RESET_TYPE_RESET_BOTH, 1 },
        { SIT_PWR_RESET_TYPE_SHUTDOWN, 0 },
        { SIT_PWR_RESET_TYPE_SHUTDOWN, 1 },
    };

    for (auto& t : testVector) {
        size_t size = sizeof(uint8_t) * 2;
        auto modemData = std::unique_ptr<ModemData>(
                                ModemDataBuilder::BuildIndication(SIT_IND_PHONE_RESET,
                                        &t, size));
        ProtocolMiscPhoneResetAdapter testAdapter(modemData.get());
        EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
        EXPECT_TRUE(testAdapter.GetParameterLength() == size);
        EXPECT_EQ(testAdapter.GetResetType(), t.resetType);
        EXPECT_EQ(testAdapter.GetResetCause(), t.resetCause);
    }
}

TEST(ProtocolMiscAdapter, ProtocolMiscDataStateChangeAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                        ModemDataBuilder::BuildIndication(SIT_IND_DATA_STATE_CHANGE,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscDataStateChangeAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetExpectedState() == testVector);
}

TEST(ProtocolMiscAdapter, ProtocolMiscGetActivityInfoAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_misc_get_activity_info_rsp testVector {
        {}, 10, 14, { 1, 2, 3, 4, 5 }, 36,
    };
    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildResponse(
                                                    SIT_GET_ACTIVITY_INFO, 0, 0,
                                                    &testVector.sleep_mode_time_ms,
                                                    sizeof(testVector) - sizeof(RCM_HEADER)));
    ProtocolMiscGetActivityInfoAdapter test(modemData.get());
    EXPECT_TRUE(test.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_EQ(test.GetSleepPeriod(), testVector.sleep_mode_time_ms);
    EXPECT_EQ(test.GetIdlePeriod(), testVector.idle_mode_time_ms);
    EXPECT_TRUE(memcmp(test.GetTxPeriod(), testVector.tx_mode_time_ms,
                        RIL_NUM_TX_POWER_LEVELS * sizeof(UINT32)) == 0);
    for (int i = 0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
        EXPECT_EQ(test.GetTxPeriod()[i], testVector.tx_mode_time_ms[i]);
    }
    EXPECT_EQ(test.GetRxPeriod(), testVector.rx_mode_time_ms);
}

TEST(ProtocolMiscAdapter, ProtocolMiscPinControlAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector[2] { 1, 0 };
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_PIN_CONTROL,
                                            testVector, sizeof(testVector)));
    ProtocolMiscPinControlAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetSignal() == testVector[0]);
    EXPECT_TRUE(testAdapter.GetStatus() == testVector[1]);
}

TEST(ProtocolMiscAdapter, ProtocolGetVoiceOperationAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint32_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_GET_VOICE_OPERATION, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolGetVoiceOperationAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetMode() == static_cast<int32_t>(testVector));
}

TEST(ProtocolMiscAdapter, ProtocolMiscGetHwConfigAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_misc_get_hw_config_rsp testVector = {};
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_GET_HW_CONFIG, 0, 0,
                                            (char *)&testVector + sizeof(RCM_HEADER),
                                            sizeof(sit_misc_get_hw_config_rsp) - sizeof(RCM_HEADER)));
    ProtocolMiscGetHwConfigAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetNum() == 0);
}

TEST(ProtocolMiscAdapter, ProtocolMiscHwConfigChangeAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_misc_hw_config_change_ind testVector = {};
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_HW_CONFIG_CHANGED,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER),
                                            sizeof(sit_misc_hw_config_change_ind) - sizeof(RCM_IND_HEADER)));
    ProtocolMiscHwConfigChangeAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(sit_misc_hw_config_change_ind) - sizeof(RCM_IND_HEADER));
    EXPECT_TRUE(testAdapter.GetNum() == 0);
}

TEST(ProtocolMiscAdapter, ProtocolMiscCdmaPrlChangeAdapter) {
    int32_t testVector = 15;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_CDMA_PRL_CHANGED,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscCdmaPrlChangeAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetPrlVersion() == testVector);
}

TEST(ProtocolMiscAdapter, ProtocolMiscLceAdapter_start) {
    DECLARE_TEST_ON_LEGACY()

    sit_pdp_start_lce_info_rsp testVector = {{}, 1, 1000};
    size_t size = sizeof(sit_pdp_start_lce_info_rsp) - sizeof(RCM_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_START_LCE_INFO, 0, 0,
                                            (char *)&testVector + sizeof(RCM_HEADER), size));
    ProtocolMiscLceAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.getLceStatus() == static_cast<int32_t>(testVector.status));
    EXPECT_TRUE(testAdapter.getActualIntervalMs() == static_cast<uint32_t>(testVector.interval));
}

TEST(ProtocolMiscAdapter, ProtocolMiscLceAdapter_stop) {
    DECLARE_TEST_ON_LEGACY()

    sit_pdp_stop_lce_info_rsp testVector = {{}, 0, 3000};
    size_t size = sizeof(sit_pdp_stop_lce_info_rsp) - sizeof(RCM_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_STOP_LCE_INFO, 0, 0,
                                            (char *)&testVector + sizeof(RCM_HEADER), size));
    ProtocolMiscLceAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.getLceStatus() == static_cast<int32_t>(testVector.status));
    EXPECT_TRUE(testAdapter.getActualIntervalMs() == static_cast<uint32_t>(testVector.interval));
}

TEST(ProtocolMiscAdapter, ProtocolMiscLceAdapter_data) {
    DECLARE_TEST_ON_LEGACY()

    sit_pdp_get_lce_data_rsp testVector = {{}, 100000, 200000, 1, 1};
    size_t size = sizeof(sit_pdp_get_lce_data_rsp) - sizeof(RCM_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_GET_LCE_DATA, 0, 0,
                                            (char *)&testVector + sizeof(RCM_HEADER), size));
    ProtocolMiscLceAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.getDlCapacityKbps() == static_cast<uint32_t>(testVector.dl_lc));
    EXPECT_TRUE(testAdapter.getUlCapacityKbps() == static_cast<uint32_t>(testVector.ul_lc));
    EXPECT_TRUE(testAdapter.getConfidencelevel() == static_cast<int32_t>(testVector.conf_lvl));
    EXPECT_TRUE(testAdapter.getLceSuspended() == static_cast<int32_t>(testVector.is_suspended));
}

TEST(ProtocolMiscAdapter, ProtocolMiscLceIndAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_pdp_lce_data_ind testVector = {{}, 100000, 200000, 1, 1};
    size_t size = sizeof(sit_pdp_lce_data_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_LCE_DATA,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscLceIndAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetDLLc() == static_cast<int32_t>(testVector.dl_lc));
    EXPECT_TRUE(testAdapter.GetULLc() == static_cast<int32_t>(testVector.ul_lc));
    EXPECT_TRUE(testAdapter.GetConfLevel() == static_cast<int32_t>(testVector.conf_lvl));
    EXPECT_TRUE(testAdapter.GetIsSuspended() == static_cast<int32_t>(testVector.is_suspended));
}

TEST(ProtocolMiscAdapter, ProtocolMiscSarControlStateAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_SAR_CONTROL_STATE,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscSarControlStateAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetDeviceState() == static_cast<uint8_t>(testVector));
}

TEST(ProtocolMiscAdapterLegacy, ProtocolMiscSarRfConnectionAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_RF_CONNECTION,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscSarRfConnectionAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetRfState() == static_cast<uint8_t>(testVector));
}

TEST(ProtocolMiscAdapter, ProtocolMiscRssiScanResultAdapter ) {
    DECLARE_TEST_ON_LEGACY()

    sit_misc_rssi_scan_ind testVector {};
    size_t size = sizeof(sit_misc_rssi_scan_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_RSSI_SCAN,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscRssiScanResultAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetTotalPage() == 0);
    EXPECT_TRUE(testAdapter.GetCurrentPage() == 0);
}

TEST(ProtocolMiscAdapter, ProtocolMiscATCommandAdapter ) {
    DECLARE_TEST_ON_LEGACY()

    sit_misc_forwarding_at_command_ind testVector = {{}, 8, "\r\nATE1\r\n"};
    size_t size = 10;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_FORWARDING_AT_COMMAND,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscATCommandAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetCommandLength() == static_cast<uint32_t>(testVector.length));
    EXPECT_TRUE(TextUtils::Equals(testAdapter.GetCommand(), testVector.data));
}

TEST(ProtocolMiscAdapter, ProtocolMiscGetStackStatusAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector[] = {
        SIT_PWR_STATCK_DISABLE,
        SIT_PWR_STATCK_ENABLE
    };

    for (auto& t : testVector) {
        auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildResponse(
                SIT_GET_STACK_STATUS, 0, RCM_E_SUCCESS,
                &t, sizeof(uint8_t)));
        ASSERT_TRUE(modemData != nullptr);

        ProtocolMiscGetStackStatusAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        EXPECT_EQ(testAdapter.GetMode(), t);
    }
}

TEST(ProtocolMiscAdapter, ProtocolOemModemInfoIndAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_oem_nw_info_ind testVector = {{}, 1, 0, {}};
    size_t size = sizeof(sit_oem_nw_info_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_NW_INFO,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolOemModemInfoIndAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetCommandType() == static_cast<int32_t>(testVector.command));
    EXPECT_TRUE(testAdapter.GetSize() == static_cast<uint32_t>(testVector.length));
}

TEST(ProtocolMiscAdapter, ProtocolMiscSelflogStatusAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_SELFLOG_STATUS,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscSelflogStatusAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetIndSelflogStatus() == testVector);
}

TEST(ProtocolMiscAdapter, ProtocolCaBandwidthFilterIndAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_oem_ca_bw_filter_ind testVector = {{}, 2, 2000};
    size_t size = sizeof(sit_oem_ca_bw_filter_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_CA_BW_FILTER,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolCaBandwidthFilterIndAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetCaConfig() == (testVector.ca_config > 0 ? 1 : 0));
    EXPECT_TRUE(testAdapter.GetNRB() == static_cast<int32_t>(testVector.num_resource_block));
}

TEST(ProtocolMiscAdapter, ProtocolMiscCurrentLinkCapacityEstimate ) {
    DECLARE_TEST_ON_LEGACY()

    sit_misc_current_link_capa_estimate_ind testVector = {{}, 1000, 2000, 3000, 4000};
    size_t size = sizeof(sit_misc_current_link_capa_estimate_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscCurrentLinkCapacityEstimate testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetDlCapaKbps() == static_cast<int32_t>(testVector.dl_capa_kbps));
    EXPECT_TRUE(testAdapter.GetUlCapaKbps() == static_cast<int32_t>(testVector.ul_capa_kbps));
    EXPECT_TRUE(testAdapter.GetSecDlCapaKbps() == static_cast<int32_t>(testVector.dcnr_sec_dl_capa_kbps));
    EXPECT_TRUE(testAdapter.GetSecUlCapaKbps() == static_cast<int32_t>(testVector.dcnr_sec_ul_capa_kbps));
}

TEST(ProtocolMiscAdapter, ProtocolMiscEndcCapabilityIndAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_oem_endc_capability_ind testVector = {{}, 1, 2};
    size_t size = sizeof(sit_oem_endc_capability_ind) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_ENDC_CAPABILITY,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscEndcCapabilityIndAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == size);
    EXPECT_TRUE(testAdapter.GetCapability() == static_cast<int32_t>(testVector.endc_capability));
    EXPECT_TRUE(testAdapter.GetCause() == static_cast<int32_t>(testVector.cause));
}

TEST(ProtocolMiscAdapter, ProtocolMiscSetSelflogProfileAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_SET_SELFLOG_PROFILE, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscSetSelflogProfileAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetResult() == static_cast<int32_t>(testVector));
}

TEST(ProtocolMiscAdapter, ProtocolMiscGetBinaryVersionInfoAdapter) {
    DECLARE_TEST_ON_LEGACY()

    std::string version = "test_version";
    size_t size = version.size() + 1;
    std::vector<uint8_t> testVector(sizeof(uint16_t) + size);
    uint8_t *p = testVector.data();
    *(uint16_t*)p = static_cast<uint16_t>(size);
    memcpy(p + sizeof(uint16_t), version.c_str(), size);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_OEM_GET_BINARY_VERSION_INFO, 0, 0,
                                            testVector.data(), testVector.size()));
    ProtocolMiscGetBinaryVersionInfoAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetVersionInfoLength() == static_cast<int32_t>(version.size() + 1));
    EXPECT_TRUE(version == testAdapter.GetData());
}

TEST(ProtocolMiscAdapter, ProtocolRfConnectionStateAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_RF_CONNECTION,
                                            &testVector, sizeof(testVector)));
    ProtocolRfConnectionStateAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetState() == testVector);
}

TEST(ProtocolMiscAdapter, ProtocolMiscSetDeviceInfoAdapter) {
    DECLARE_TEST_ON_LEGACY()

    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_SET_DEVICE_INFO, 0, 0));
    ProtocolMiscSetDeviceInfoAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
}

TEST(ProtocolMiscAdapter, ProtocolMiscDropMtCallInfoIndAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint32_t testVector = 10;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_OEM_IND_DROPPED_MT_CALL_INFO,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscDropMtCallInfoIndAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
}

TEST(ProtocolMiscAdapter, ProtocolMiscCarrierConfigSimInfoIndAdapter) {
    DECLARE_TEST_ON_LEGACY()

    sit_ind_carrier_config_sim_info testVector {};
    size_t size = sizeof(sit_ind_carrier_config_sim_info) - sizeof(RCM_IND_HEADER);
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_CARRIER_CONFIG_SIM_INFO,
                                            (char *)&testVector + sizeof(RCM_IND_HEADER), size));
    ProtocolMiscCarrierConfigSimInfoIndAdapter testAdapter(modemData.get());
    testAdapter.getMccMnc();
    testAdapter.getIccid();
    testAdapter.getImsi();
    testAdapter.getSpn();
    testAdapter.getPnn();
    testAdapter.getGid1();
    testAdapter.getGid2();
    testAdapter.getPrivilegeAccessRules();
}

TEST(ProtocolMiscAdapter, ProtocolMiscCarrierConfigStatusIndAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint32_t testVector[] = {1, 10};
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildIndication(SIT_IND_CARRIER_CONFIG_STATUS,
                                            testVector, sizeof(testVector)));
    ProtocolMiscCarrierConfigStatusIndAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetParameter());
    EXPECT_TRUE(testAdapter.GetParameterLength() == sizeof(testVector));
    EXPECT_TRUE(testAdapter.GetStatus() == static_cast<int32_t>(testVector[0]));
    EXPECT_TRUE(testAdapter.GetCause() == static_cast<int32_t>(testVector[1]));
}

TEST(ProtocolMiscAdapter, ProtocolMiscGetUeOperationModeAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector = 3;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_OEM_GET_UE_OPERATION_MODE, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscGetUeOperationModeAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetMode() == testVector);
}

TEST(ProtocolMiscAdapter, ProtocolMiscGetNrrrmMinReqEnableAdapter) {
    DECLARE_TEST_ON_LEGACY()

    uint8_t testVector = 1;
    auto modemData = std::unique_ptr<ModemData>(
                            ModemDataBuilder::BuildResponse(SIT_OEM_GET_NRRRM_MIN_REQ_ENABLE, 0, 0,
                                            &testVector, sizeof(testVector)));
    ProtocolMiscGetNrrrmMinReqEnableAdapter testAdapter(modemData.get());
    EXPECT_TRUE(testAdapter.GetErrorCode() == RIL_E_SUCCESS);
    EXPECT_TRUE(testAdapter.GetEnable() == testVector);
}