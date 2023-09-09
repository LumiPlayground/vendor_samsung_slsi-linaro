/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _MISC_JSON_DEF_H_
#define _MISC_JSON_DEF_H_

#include "json/json.h"

/******************************************************************************
 *
 * File: miscjsondef.h
 *
 * Description: This file will have key-value pair definition of all MISC SIT commands
 *              Above each key, we have defined type of value it will hold and
 *              staus of the key.
 *              Type can be one of below:-
 *              1. Integer / Unsigned Integer (Int / UInt)
 *              2. Long Int / Unsigned Long (Long / ULong)
 *              3. String (String)
 *              4. Object (Obj)
 *              5. Array of Integers / Unsigned Integers (Int[] / UInt[])
 *              6. Array of Long Int / Long Unsigned Int (Long[] / ULong[])
 *              7. Array of Strings (String[])
 *              8. Array of Objects (Obj[])
 *              Status can be one of below:-
 *              1. Mandatory (M)
 *              2. Optional (O)
 *              3. Conditional (C)
 *
 *****************************************************************************/

/******************************************************************************
 * Call
 ******************************************************************************/
/*
  SIT_DTMF (RCM ID = 0x0003)
*/
namespace ns_misc_call_dtmf_req {
// String, M
static const Json::StaticString key_dtmf_digits("dtmf_digits");
} // namespace ns_misc_call_dtmf_req

namespace ns_misc_call_dtmf_rsp { }

/*
  SIT_DTMF_START (RCM ID = 0x0005)
*/
namespace ns_misc_call_dtmf_start_req {
// Int, M
static const Json::StaticString key_tone_type("tone_type"); // sit_call_dtmf_local_tone_e_type
// Int, M
static const Json::StaticString key_tone_duration("tone_duration"); // sit_call_dtmf_tone_duration_e_type
// Int, M
static const Json::StaticString key_dtmf_digit("dtmf_digit");
} // namespace ns_misc_call_dtmf_start_req

namespace ns_misc_call_dtmf_start_rsp { }

/*
  SIT_DTMF_STOP (RCM ID = 0x0006)
*/
namespace ns_misc_call_dtmf_stop_req { }

namespace ns_misc_call_dtmf_stop_rsp { }


/******************************************************************************
 * MISC
 ******************************************************************************/
/*
   SIT_GET_SIGNAL_STRENGTH (RCM ID = 0x0900)
 */
namespace ns_misc_get_signal_strength_req { }

namespace ns_misc_get_signal_strength_rsp {
// Int, M
static const Json::StaticString key_valid_ss_rat_bitmask("valid_ss_rat_bitmask");
// Obj, C, key_valid_ss_rat_bitmask
static const Json::StaticString key_gsm_signal_strength("gsm_signal_strength"); // ns_misc_gsm_signal_strength
// Obj, C, key_valid_ss_rat_bitmask
static const Json::StaticString key_wcdma_signal_strength("wcdma_signal_strength"); // ns_misc_wcdma_signal_strength
// Obj, C, key_valid_ss_rat_bitmask
static const Json::StaticString key_cdma_signal_strength("cdma_signal_strength"); // ns_misc_cdma_signal_strength
// Obj, C, key_valid_ss_rat_bitmask
static const Json::StaticString key_evdo_signal_strength("evdo_signal_strength"); // ns_misc_evdo_signal_strength
// Obj, C, key_valid_ss_rat_bitmask
static const Json::StaticString key_lte_signal_strength("lte_signal_strength"); // ns_misc_lte_signal_strength
// Obj, C, key_valid_ss_rat_bitmask
static const Json::StaticString key_tdscdma_signal_strength("tdscdma_signal_strength"); // ns_misc_tdscdma_signal_strength
// Obj, C, key_valid_ss_rat_bitmask
static const Json::StaticString key_nr_signal_strength("nr_signal_strength"); // ns_misc_nr_signal_strength
} // namespace ns_misc_get_signal_strength_rsp

namespace ns_misc_gsm_signal_strength {
// Int, M
static const Json::StaticString key_signal_strength("gsm_sig_strength");
// Int, M
static const Json::StaticString key_ber("gsm_ber");
// UInt, M
static const Json::StaticString key_ta("gsm_ta");
} // namespace ns_misc_gsm_signal_strength

namespace ns_misc_wcdma_signal_strength {
// Int, M
static const Json::StaticString key_signal_strength("wcdma_sig_strength");
// Int, M
static const Json::StaticString key_ber("wcdma_ber");
// Int, M
static const Json::StaticString key_rscp("wcdma_rscp");
// Int, M
static const Json::StaticString key_ecno("wcdma_ecno");
} // namespace ns_misc_wcmda_signal_strength

namespace ns_misc_cdma_signal_strength {
// UInt, M
static const Json::StaticString key_dbm("cdma_dbm");
// UInt, M
static const Json::StaticString key_ecio("cdma_ecio");
} // namespace ns_misc_cdma_signal_strength

namespace ns_misc_evdo_signal_strength {
// UInt, M
static const Json::StaticString key_dbm("evdo_dbm");
// UInt, M
static const Json::StaticString key_ecio("evdo_ecio");
// Int, M
static const Json::StaticString key_snr("evdo_snr");
} // namespace ns_misc_evdo_signal_strength

namespace ns_misc_tdscdma_signal_strength {
// Int, M
static const Json::StaticString key_signal_strength("tdscdma_sig_strength");
// Int, M
static const Json::StaticString key_ber("tdscdma_ber");
// Int, M
static const Json::StaticString key_rscp("tdscdma_rscp");
} // namespace ns_misc_tdscdma_signal_strength


namespace ns_misc_lte_signal_strength {
// Int, M
static const Json::StaticString key_signal_strength("lte_sig_strength");
// Int, M
static const Json::StaticString key_rsrp("lte_rsrp");
// Int, M
static const Json::StaticString key_rsrq("lte_rsrq");
// Int, M
static const Json::StaticString key_rssnr("lte_rssnr");
// Int, M
static const Json::StaticString key_cqi_table_index("lte_cqi_table_index");
// Int, M
static const Json::StaticString key_cqi("lte_cqi");
// Int, M
static const Json::StaticString key_ta("lte_ta");
} // namespace ns_misc_lte_signal_strength


namespace ns_misc_nr_signal_strength {
// Int, M
static const Json::StaticString key_ss_rsrp("nr_ss_rsrp");
// Int, M
static const Json::StaticString key_ss_rsrq("nr_ss_rsrq");
// Int, M
static const Json::StaticString key_ss_sinr("nr_ss_sinr");
// Int, M
static const Json::StaticString key_csi_rsrp("nr_csi_rsrp");
// Int, M
static const Json::StaticString key_csi_rsrq("nr_csi_rsrq");
// Int, M
static const Json::StaticString key_csi_sinr("nr_csi_sinr");
// UInt, M
static const Json::StaticString key_cqi_table_index("nr_cqi_table_index");
// Int, M
static const Json::StaticString key_wb_cqi("nr_wb_cqi");
// UInt[], M
static const Json::StaticString key_sb_cqi_list("nr_sb_cqi_list");
} // namespace ns_misc_nr_signal_strength

/*
  SIT_GET_BASEBAND_VERSION (RCM ID = 0x901)
*/
namespace ns_misc_get_baseband_version_req {
// Int, M
static const Json::StaticString key_ver_mask("ver_mask"); // bitmask
} // namesapce ns_misc_get_baseband_version_req

namespace ns_misc_get_baseband_version_rsp {
// Int, M
static const Json::StaticString key_ver_mask("ver_mask"); // Bitmask
// String, C, key_ver_mask
static const Json::StaticString key_sw_ver("sw_ver");
// String, C, key_ver_mask
static const Json::StaticString key_hw_ver("hw_ver");
// String, C, key_ver_mask
static const Json::StaticString key_rf_cal_date("rf_cal_date");
// String, C, key_ver_mask
static const Json::StaticString key_product_code("product_code");
// String, C, key_ver_mask
static const Json::StaticString key_model_id("model_id");
// String, C, key_ver_mask
static const Json::StaticString key_prl_ver("prl_ver");
// String, C, key_ver_mask
static const Json::StaticString key_eri_ver("eri_ver");
// String, C, key_ver_mask
static const Json::StaticString key_cp_chipset("cp_chipset");
} // namespace ns_misc_get_baseband_version_rsp

/*
  SIT_SET_SCREEN_STATE (RCM ID = 0x0902)
*/
namespace ns_misc_set_screen_state_req {
// Int M
static const Json::StaticString key_screen_state("screen_state"); // sit_misc_screen_state_e_type
} // namespace ns_misc_set_screen_state_req

namespace ns_misc_set_screen_state_rsp { }

/*
   SIT_SET_TTY_MODE (RCM ID = 0x0903)
*/
namespace ns_misc_set_tty_mode_req {
// Int, M
static const Json::StaticString key_tty_mode("tty_mode"); // sit_misc_tty_mode_e_type
} // namesapce ns_misc_set_tty_mode_req

namespace ns_misc_set_tty_mode_rsp { }

/*
   SIT_GET_TTY_MODE (RCM ID = 0x0904)
*/
namespace ns_misc_get_tty_mode_req { }

namespace ns_misc_get_tty_mode_rsp {
// Int, M
static const Json::StaticString key_tty_mode("tty_mode"); // sit_misc_tty_mode_e_type
} // namespace ns_misc_get_tty_mode_req

/*
   SIT_IND_NITZ_TIME_RECEIVED (RCM ID = 0x0905)
*/
namespace ns_misc_nitz_time_ind {
// Int, O
static const Json::StaticString key_daylight_valid("daylight_valid"); // TODO: Consider to remove it later as it is not used by RIL anymore
// Int, M
static const Json::StaticString key_year("year");
// Int, M
static const Json::StaticString key_month("month");
// Int, M
static const Json::StaticString key_day("day");
// Int, M
static const Json::StaticString key_hour("hour");
// Int, M
static const Json::StaticString key_minute("minute");
// Int, M
static const Json::StaticString key_second("second");
// Int, M
static const Json::StaticString key_time_zone("time_zone");
// Int, M
static const Json::StaticString key_daylight_adjust("daylight_adjust"); // sit_misc_nitz_daylight_adjust_e_type
// Int, O
static const Json::StaticString key_day_of_week("day_of_week"); // TODO: Consider to remove it later as it is not used by RIL anymore
// Int, O
static const Json::StaticString key_mm_info_type("mm_info_type"); // TODO: Consider to remove it later as it is not used by RIL anymore
// Int, O
static const Json::StaticString key_plmn("plmn"); // TODO: Consider to remove it later as it is not used by RIL anymore
} // namespace ns_misc_nitz_time_ind

/*
   SIT_IND_SIGNAL_STRENGTH (RCMD ID: 0x0906)
 */
namespace ns_misc_signal_strength_ind = ns_misc_get_signal_strength_rsp;

/*
   SIT_SET_CFG_DEFAULT (RCMD ID: 0x0907)
 */
namespace ns_misc_set_cfg_default_req {
// Int, M
static const Json::StaticString key_reset_type("reset_type"); // NOTE: In legacy interface, this field is not used
} // namesapce ns_misc_set_cfg_default_req

namespace ns_misc_set_cfg_deafult_rsp { }

/*
   SIT_SET_ENG_MODE (RCM ID: 0x0908)
 */
namespace ns_misc_set_eng_mode_req {
// Int, M
static const Json::StaticString key_mode("eng_mode");
// Int, C, key_mode
static const Json::StaticString key_sub_mode("eng_sub_mode");
} // namesapce ns_misc_set_eng_mode_req

namespace ns_misc_set_eng_mode_rsp { }

/*
   SIT_SET_SCREEN_LINE (RCM ID: 0x0909)
 */
namespace ns_misc_set_screen_line_req {
// Int, M
static const Json::StaticString key_screen_line("screen_line");
} // namesapce ns_misc_set_screen_line_req

namespace ns_misc_set_screen_line_rsp { }

/*
   SIT_SET_DEBUG_TRACE (RCM ID: 0x090B)
 */
namespace ns_misc_set_debug_trace_req {
// Int, M
static const Json::StaticString key_debug_trace("debug_trace");
} // namesapce ns_misc_set_debug_trace_req

namespace ns_misc_set_debug_trace_rsp { }

/*
   SIT_GET_ACTIVITY_INFO (RCM ID: 0x090C)
 */
namespace ns_misc_get_activity_info_req { }

namespace ns_misc_get_activity_info_rsp {
// UInt, M
static const Json::StaticString key_sleep_mode_time("sleep_mode_time");
// UInt, M
static const Json::StaticString key_idle_mode_time("idle_mode_time");
// UInt[], M
static const Json::StaticString key_tx_mode_time("tx_mode_time");
// UInt, M
static const Json::StaticString key_rx_mode_time("rx_mode_time");
} // namespace ns_misc_get_activity_info_rsp

/*
   SIT_SET_FEATURE_INFO (RCM ID: 0x090E)
 */
namespace ns_misc_set_feature_info_req {
// Int, M
static const Json::StaticString key_feature_bitmask("feature_bitmask");
} // namespace ns_misc_set_feature_info_req

namespace ns_misc_set_feature_info_rsp { }

/*
   SIT_GET_CDMA_SUBSCRIPTION (RCM ID: 0x090F)
 */
namespace ns_msic_get_cdma_subscription_req { }

namespace ns_misc_get_cdma_subscription_rsp {
// String, M
static const Json::StaticString key_mdn("mdn");
// String, M
static const Json::StaticString key_min("min");
// Int, M
static const Json::StaticString key_home_sid("home_sid");
// Int, M
static const Json::StaticString key_home_nid("home_nid");
// UInt, M
static const Json::StaticString key_prl_version("prl_version");
} // namespace ns_misc_get_cdma_subscription_rsp

/*
   SIT_SET_ENG_STRING_INPUT (RCM ID: 0x0910)
 */
namespace ns_misc_set_eng_string_input_req {
// String, M
static const Json::StaticString key_string("string");
} // namespace ns_misc_set_eng_string_input_req

namespace ns_misc_set_eng_string_input_rsp { }

/*
   SIT_GET_MSL_CODE (RCM ID: 0x0911)
 */
namespace ns_misc_get_msl_code_req { }

namespace ns_misc_get_msl_code_rsp {
// String, M
static const Json::StaticString key_msl_code("msl_code");
} // namespace ns_misc_msl_code_rsp

/*
   SIT_SET_VOICE_OPERATION (RCM ID: 0x091A)
 */
namespace ns_misc_set_voice_operation_req {
// UInt, M
static const Json::StaticString key_mode("mode");
} // namespace ns_misc_set_voice_operation_req

namespace ns_misc_set_voice_operation_rsp { }

/*
   SIT_GET_VOICE_OPERATION (RCM ID: 0x091B)
 */
namespace ns_misc_get_voice_operation_req { }

namespace ns_misc_get_voice_operation_rsp {
// UInt, M
static const Json::StaticString key_mode("mode");
} // namespace ns_misc_get_voice_operation_rsp

/*
   SIT_SET_PIN_CONTROL (RCM ID: 0x0920)
 */
namespace ns_misc_set_pin_control_req {
// Int, M
static const Json::StaticString key_pin_signal("pin_signal");
// Int, M
static const Json::StaticString key_pin_status("pin_status");
} // namespace ns_misc_set_pin_control_req

namespace ns_misc_set_pin_control_rsp { }

/*
   SIT_IND_PIN_CONTROL (RCM ID: 0x0921)
 */
namespace ns_misc_pin_control_ind {
// Int, M
static const Json::StaticString key_pin_signal("pin_signal");
// Int, M
static const Json::StaticString key_pin_status("pin_status");
} // namespace ns_misc_pin_control_ind

/*
   SIT_SET_DEVICE_INFO (RCM ID: 0x0922)
 */
namespace ns_misc_set_device_info_req {
// String, M
static const Json::StaticString key_ap_model_name("ap_model_name");
// String, M
static const Json::StaticString key_ap_sw_version("ap_sw_version");
// String, M
static const Json::StaticString key_ap_product_name("ap_product_name");
// String, M
static const Json::StaticString key_ap_os_version("ap_os_version");
} // namespace ns_misc_set_device_info_req

namespace ns_misc_set_device_info_rsp {
// String, M
static const Json::StaticString key_sit_version_info("sit_version_info");
} // namespace ns_misc_set_device_info_rsp

/*
   SIT_SET_IND_CMD_FILTER (RCM ID: 0x0928)
 */
namespace ns_misc_set_ind_cmd_filter_req {
// UInt, M
static const Json::StaticString key_filter_bitmask("filter_bitmask");
} // namespace ns_misc_set_ind_cmd_filter_req

namespace ns_misc_set_ind_cmd_filter_rsp { }

/*
   SIT_IND_SAR_CONTROL_STATE (RCM ID: 0x092A)
 */
namespace ns_misc_sar_control_state_ind {
// Int, M
static const Json::StaticString key_device_state("device_state");
} // namespace ns_misc_sar_control_state_ind

/*
   SIT_SET_SENSOR_STATUS (RCM ID: 0x092B)
 */
namespace ns_misc_set_sensor_status_req {
// Int, M
static const Json::StaticString key_status_bitmask("status_bitmask");
} // namespace ns_misc_set_sensor_status_req

namespace ns_misc_set_sensor_status_rsp { }

/*
   SIT_SET_RSSI_SCAN (RCM ID: 0x092D)
 */
namespace ns_misc_set_rssi_scan_req {
// Int, M
static const Json::StaticString key_rat("rat");
// Int, M
static const Json::StaticString key_rx_band("rx_band");
// Int, C, key_rat
static const Json::StaticString key_rbw("rbw");
// Int, M
static const Json::StaticString key_scan_mode("scan_mode");
// Int, C, key_rat
static const Json::StaticString key_start_freq("start_freq");
// Int, C, key_rat
static const Json::StaticString key_end_freq("end_freq");
// Int, M
static const Json::StaticString key_offset("offset");
// Int, M
static const Json::StaticString key_antenna_selection("antenna_selection");
// Int, M
static const Json::StaticString key_sampling_count("sampling_cnt");
// Obj, M
static const Json::StaticString key_tx1_info("tx1_info"); // ns_misc_tx_info
// Obj, M
static const Json::StaticString key_tx2_info("tx2_info"); // ns_misc_tx_info
// Int[], M
static const Json::StaticString key_rsvd_info_list("rsvd_info_list");
} // namespace ns_misc_set_rssi_scan_req

namespace ns_misc_tx_info {
// Int, M
static const Json::StaticString key_status("tx_status");
// Int, M, key_rat
static const Json::StaticString key_band("tx_band");
// Int, M
static const Json::StaticString key_bw("tx_bw");
// Int, M
static const Json::StaticString key_freq("tx_freq");
// Int, M
static const Json::StaticString key_power("tx_power");
// Int, M
static const Json::StaticString key_rb_num("rb_num");
// Int, M
static const Json::StaticString key_rb_offset("rb_offset");
// Int, M
static const Json::StaticString key_mcs("tx_mcs");
} // namespace ns_misc_tx_info

/*
   SIT_IND_RSSI_SCAN (RCM ID: 0x092E)
 */
namespace ns_misc_rssi_scan_ind {
// Int, M
static const Json::StaticString key_total_page("total_page");
// Int, M
static const Json::StaticString key_current_page("current_page");
// Int, M
static const Json::StaticString key_start_freq("start_freq");
// Int, M
static const Json::StaticString key_end_freq("end_freq");
// Int, M
static const Json::StaticString key_offset("offset");
// Int[], M
static const Json::StaticString key_scan_result("scan_result");
} // namespace ns_misc_rssi_scan_ind

/*
   SIT_SET_PREFERRED_CALL_CAPABILITY (RCM ID: 0x092F)
 */
namespace ns_misc_set_preferred_call_capability_req {
// Int, M
static const Json::StaticString key_mode("mode");
} // namespace ns_misc_set_preferred_call_capability_req

namespace ns_misc_set_preferred_call_capability_rsp { }

/*
   SIT_GET_PREFERRED_CALL_CAPABILITY (RCM ID: 0x0930)
 */
namespace ns_misc_get_preferred_call_capability_req { }

namespace ns_misc_get_preferred_call_capability_rsp {
// Int, M
static const Json::StaticString key_mode("mode");
} // namespace ns_misc_get_preferred_call_capability_rsp

/*
   SIT_SET_FORWARDING_AT_COMMAND (RCM ID: 0x0931)
 */
namespace ns_misc_set_forwarding_at_command_req {
// String, M
static const Json::StaticString key_data("data");
} // namespace ns_misc_set_forwarding_at_command_req

namespace ns_misc_set_forwarding_at_command_rsp { }

/*
   SIT_IND_FORWARDING_AT_COMMAND (RCM ID: 0x0932)
 */
namespace ns_misc_forwarding_at_command_ind {
// String, M
static const Json::StaticString key_data("data");
} // namespace ns_misc_forwarding_at_command_ind

/*
   SIT_SET_SELFLOG (RCM ID: 0x093A)
 */
namespace ns_misc_set_selflog_req {
// Int, M
static const Json::StaticString key_mode("mode");
// Int, M
static const Json::StaticString key_buf_size("buf_size");
} // namespace ns_misc_set_selflog_req

namespace ns_misc_set_selflog_rsp {
// Int, M
static const Json::StaticString key_result("result");
} // namespace ns_misc_set_selflog_rsp

/*
   SIT_GET_SELFLOG_STATUS (RCM ID: 0x093B)
 */
namespace ns_misc_get_selflog_status_req { }

namespace ns_misc_get_selflog_status_rsp {
// Int, M
static const Json::StaticString key_status("status");
} // ns_misc_get_selflog_status_rsp

/*
   SIT_IND_SELFLOG_STATUS (RCM ID: 0x093C)
 */
namespace ns_misc_selflog_status_ind {
// Int, M
static const Json::StaticString key_status("status");
} // namespace ns_misc_selflog_status_ind

/*
   SIT_SET_ELEVATOR_SENSOR (RCM ID: 0x093D)
 */
namespace ns_misc_set_elevator_sensor_req {
// Int, M
static const Json::StaticString key_sensor_status("sensor_status");
} // namespace ns_misc_set_elevator_sensor_req

namespace ns_misc_set_elevator_sensor_rsp { }

/*
   SIT_SET_MODEM_CONFIG (RCM ID: 0x093F)
 */
namespace ns_misc_set_modem_config_req {
// Int, M
static const Json::StaticString key_config("config");
} // namespace ns_misc_set_modem_config_req

namespace ns_misc_set_modem_config_rsp { }

/*
   SIT_SET_MODEM_LOG_DUMP (RCM ID: 0x0940)
 */
namespace ns_misc_set_modem_log_dump_req {
// Int, M
static const Json::StaticString key_type("type");
} // namespace ns_misc_set_modem_log_dump_req

namespace ns_misc_set_modem_log_dump_rsp {
// Int, M
static const Json::StaticString key_result("result");
} // namesapce ns_misc_set_modem_log_dump_rsp

/*
   SIT_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA (RCM ID: 0x0943)
 */
namespace ns_misc_set_signal_strength_reporting_criteria_req {
// UInt, M
static const Json::StaticString key_h_ms("hysteresis_ms");
// UInt, M
static const Json::StaticString key_h_db("hysteresis_db");
// Int[], M
static const Json::StaticString key_t_dbm("thresholds_dbm");
// Int, M
static const Json::StaticString key_ran("radio_access_network");
// Int, M
static const Json::StaticString key_measure_type("measure_type");;
// Int, M
static const Json::StaticString key_enable("enable");
} // namespace ns_misc_set_signal_strength_reporting_criteria_req

namespace ns_misc_set_ss_reporting_criteria_rsp { }

/*
   SIT_SET_LINK_CAPACITY_REPORTING_CRITERIA (RCM ID: 0x0944)
 */
namespace ns_misc_set_link_capacity_reporting_criteria_req {
// UInt, M
static const Json::StaticString key_h_ms("hysteresis_ms");
// UInt, M
static const Json::StaticString key_h_dl_kbps("hysteresis_dl_kbps");
// UInt, M
static const Json::StaticString key_h_ul_kbps("hysteresis_ul_kbps");
// UInt[], M
static const Json::StaticString key_t_dl_kbps("thresholds_dl_kbps");
// UInt[], M
static const Json::StaticString key_t_ul_kbps("thresholds_ul_kbps");
// Int, M
static const Json::StaticString key_ran("radio_access_network");
} // namespace ns_misc_set_link_capacity_reporting_criteria_req

namespace ns_misc_set_link_capacity_reporting_criteria_rsp { }

/*
   SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE (RCM ID: 0x0945)
 */
namespace ns_misc_current_link_capacity_estimate_ind {
// UInt, M
static const Json::StaticString key_dl_capa_kbps("downlink_capability_kbps");
// UInt, M
static const Json::StaticString key_ul_capa_kbps("uplink_capability_kbps");
// UInt, M
static const Json::StaticString key_dcnr_sec_dl_capa_kbps("dcnr_sec_downlink_capability_kbps");
// UInt, M
static const Json::StaticString key_dcnr_sec_ul_capa_kbps("dcnr_sec_uplink_capability_kbps");
} // namespace ns_misc_current_link_capacity_estimate_ind

/*
   SIT_SET_AP_SYSTEM_TTIME (RCM ID: 0x0949)
 */
namespace ns_misc_set_ap_system_time_req {
// Int, M
static const Json::StaticString key_year("year");
// Int, M
static const Json::StaticString key_month("month");
// Int, M
static const Json::StaticString key_day("day");
// Int, M
static const Json::StaticString key_hour("hour");
// Int, M
static const Json::StaticString key_minute("minute");
// Int, M
static const Json::StaticString key_second("second");
} // namespace ns_misc_set_ap_system_time_req

namespace ns_misc_set_ap_system_time_rsp {
// Int, M
static const Json::StaticString key_result("result");
} // namespace ns_misc_set_ap_system_time_rsp

/*
   SIT_SET_NR_SILENCE_MODE (RCM ID: 0x094C)
 */
// Refer ns_net_misc_set_nr_silence_mode_req
// Refer ns_net_misc_set_nr_silence_mode_rsp

/*
   SIT_SET_DATA_THROTTLING (RCM ID: 0x094D)
 */
namespace ns_misc_set_data_throttling_req {
// Int, M
static const Json::StaticString key_throttling_action("throttling_action");
// ULong, M
static const Json::StaticString key_completion_duration_millis("completion_duration_millis");
} // namespace ns_misc_set_data_throttling_req

namespace ns_misc_set_data_throttling_rsp { }

/*
   SIT_SET_LAB_TEST_CONFIG (RCM ID: 0X094E)
 */
namespace ns_misc_set_lab_test_config_req {
// Int, M
static const Json::StaticString key_config_mode("config_mode");
// Int, M
static const Json::StaticString key_config_status("config_status");
} // namespace ns_misc_set_lab_test_config_req

namespace ns_misc_set_lab_test_config_rsp { }

/*
   SIT_IND_RF_CONNECTION (RCM ID: 0x0950)
 */
namespace ns_misc_rf_connection_ind {
// Int, M
static const Json::StaticString key_rf_cable_state("rf_cable_state");
} // namespace ns_misc_rf_connection_ind

/******************************************************************************
 * Network
 *****************************************************************************/
/*
    SIT_GET_MANUAL_BAND_MODE (RCM ID = 0x0730)
*/
// This is defined in miscjsondef.h.
namespace ns_misc_net_get_manual_band_mode_rsp {
// Int, M
static const Json::StaticString key_manual_mode_status("manual_mode_status");    // 0:Default(= Not set), 1: set
// ULong[], M
static const Json::StaticString key_support_lte_band_bitmask_list("sup_lte_band_bitmask_list");  // bit masking
// ULong, M
static const Json::StaticString key_support_wcdma_bitmask_band("sup_wcdma_band_bitmask");        // bit masking
// Int, M
static const Json::StaticString key_support_gsm_bitmask_band("sup_gsm_band_bitmask");            // bit masking
// Int, M
static const Json::StaticString key_support_cdma_bitmask_band("sup_cdma_band_bitmask");          // bit masking
// ULong[], M
static const Json::StaticString key_current_lte_band_bitmask_list("cur_lte_band_bitmask_list");  // bit masking
// ULong, M
static const Json::StaticString key_current_wcdma_band_bitmask("cur_wcdma_band_bitmask");        // bit masking
// Int, M
static const Json::StaticString key_current_gsm_band_bitmask("cur_gsm_band_bitmask");            // bit masking
// Int, M
static const Json::StaticString key_current_cdma_band_bitmask("cur_cdma_band_bitmask");          // bit masking
}

/*
    SIT_SET_MANUAL_BAND_MODE (RCM ID = 0x0731)
*/
namespace ns_misc_net_set_manual_band_mode_req {
// Int, M
static const Json::StaticString key_manual_mode_status("manual_mode_status");    // 0:Default(= Not set), 1: set
// ULong[], M
static const Json::StaticString key_lte_band_bitmask_list("lte_band_bitmask_list");  // bit masking
// ULong, M
static const Json::StaticString key_wcdma_band_bitmask("wcdma_band_bitmask");        // bit masking
// Int, M
static const Json::StaticString key_gsm_band_bitmask("gsm_band_bitmask");            // bit masking
// Int, M
static const Json::StaticString key_cdma_band_bitmask("cdma_band_bitmask");          // bit masking
}

namespace ns_misc_net_set_manual_band_mode_rsp {
// Int, M
static const Json::StaticString key_manual_mode_cause("manual_mode_cause");
}

/*
    SIT_GET_RF_DESENSE_MODE (RCM ID = 0x0732)
*/
namespace ns_misc_net_get_rf_desense_mode_rsp {
// Int, M
static const Json::StaticString key_desense_status("desense_status");     // 0: stop, 1:start, 2:pause
// Int, M
static const Json::StaticString key_desense_rat("desense_rat");           // 0: gsm, 1:wcdma, 2:lte, 3:cdma
}

/*
    SIT_SET_RF_DESENSE_MODE (RCM ID = 0x0733)
*/
namespace misc_ns_net_set_rf_desense_mode_req {
// Int, M
static const Json::StaticString key_desense_status("desense_status");     // 0: stop, 1:start, 2:pause
// Int, C
static const Json::StaticString key_desense_rat("desense_rat");           // sit_rf_desense_rat_e_type
// Int, M
static const Json::StaticString key_power_level("power_level");
// UInt, M
static const Json::StaticString key_arfcn("arfcn");
// Obj, C
static const Json::StaticString key_gsm_desense_info("gsm_desense_info"); // ns_misc_net_gsm_desense_info
// Obj, C
static const Json::StaticString key_wcdma_desense_info("wcdma_desense_info"); // ns_misc_net_wcdma_desense_info
// Obj, C
static const Json::StaticString key_lte_desense_info("lte_desense_info"); // ns_misc_net_lte_desense_info
// Obj, C
static const Json::StaticString key_cdma_desense_info("cdma_desense_info"); // ns_misc_net_cdma_desense_info
}

namespace ns_misc_net_set_rf_desense_mode_rsp {
// Int, M
static const Json::StaticString key_desense_cause("desense_cause");        // 0:success, 1:fail
}

namespace ns_misc_net_gsm_desense_info {
// Int, M
static const Json::StaticString key_gsm_band_bitmask("gsm_band_bitmask");                 // bit masking
// Int, M
static const Json::StaticString key_afc("afc");
// Int, M
static const Json::StaticString key_tsc("tsc");
// Int, M
static const Json::StaticString key_pattern("pattern");
}

namespace ns_misc_net_wcdma_desense_info {
// ULong, M
static const Json::StaticString key_wcdma_band_bitmask("wcdma_band_bitmask");             // bit masking
}

namespace ns_misc_net_lte_desense_info {
// ULong[], M
static const Json::StaticString key_lte_band_bitmask_list("lte_band_bitmask_list");       // bit masking
// Int, M
static const Json::StaticString key_duplex("duplex");
// Int, M
static const Json::StaticString key_ul_bw("ul_bw");
// UInt, M
static const Json::StaticString key_ul_freq("ul_freq");
// Int, M
static const Json::StaticString key_config_index("config_index");
// Int, M
static const Json::StaticString key_sf_config_index("sfconfig_index");
// Int, M
static const Json::StaticString key_vrb_start("vrb_start");
// Int, M
static const Json::StaticString key_vrb_len("vrb_len");
// Int, M
static const Json::StaticString key_mcs("mcs");
}

namespace ns_misc_net_cdma_desense_info {
// Int, M
static const Json::StaticString key_cdma_band_bitmask("cdma_band_bitmask");               // bit masking
// Int, M
static const Json::StaticString key_cdma_mod("cdma_mod");                 // CDMA only
}

/*
  SIT_SET_LOCATION_UPDATE_SETTING (RCM ID = 0x0744)
*/
namespace ns_misc_net_set_location_update_setting_req {
// Int, M
static const Json::StaticString key_update_setting("update_setting");    // sit_net_enable_e_type
}

/******************************************************************************
 * ID
 ******************************************************************************/
/*
    SIT_GET_IMSI (RCM ID = 0x0400)
*/
// This is defined in simjsondef.h

/*
    SIT_GET_IMEI (RCM ID = 0x0401)
*/
namespace ns_misc_id_get_imei_req { }

namespace ns_misc_id_get_imei_rsp {
// String, M
static const Json::StaticString key_imei("imei");
}

/*
    SIT_GET_IMEISV (RCM ID = 0x0402)
*/
namespace ns_misc_id_get_imeisv_req { }

namespace ns_misc_id_get_imeisv_rsp {
// String, M
static const Json::StaticString key_imeisv("imeisv");
}

/*
   SIT_GET_DEVICE_ID (RCM ID = 0x0403)
 */
namespace ns_misc_id_get_deviceid_req { }

namespace ns_misc_id_get_deviceid_rsp {
// String, M
static const Json::StaticString key_imei("imei");
// String, M
static const Json::StaticString key_imeisv("imeisv");
// String, M
static const Json::StaticString key_esn("esn");
// String, M
static const Json::StaticString key_meid("meid");
}

/*
   SIT_SET_SGC (RCM ID = 0x0404)
 */
namespace ns_misc_id_set_sgc_req {
// Int, M
static const Json::StaticString key_sgc("sgc");
// Int, O
static const Json::StaticString key_rsv1("rsv1");  // currently not parsed by CP
// Int, O
static const Json::StaticString key_rsv2("rsv2");  // currently not parsed by CP
}

namespace ns_misc_id_set_sgc_rsp { }

/**************************************************************
 * Power
 *************************************************************/
/*
   SIT_IND_PHONE_RESET (RCM ID = 0x0804)
 */
namespace ns_pwr_phone_reset_ind {
// Int, M
static const Json::StaticString key_reset_type("reset_type");  // sit_pwr_phone_reset_type_e_type
}

/*
   SIT_SET_STACK_STATUS (RCM ID = 0x080F)
 */
namespace ns_misc_pwr_set_stack_status_req {
// Int, M
static const Json::StaticString key_status("status");  // sit_pwr_stack_mode_e_type
}

namespace ns_misc_pwr_set_stack_status_rsp { }

/*
   SIT_GET_STACK_STATUS (RCM ID = 0x0810)
 */
namespace ns_misc_pwr_get_stack_status_req { }

namespace ns_misc_pwr_get_stack_status_rsp {
// Int, M
static const Json::StaticString key_status("status");  // sit_pwr_stack_mode_e_type
}

/**************************************************************
 * ps
 *************************************************************/
/*
   SIT_IND_DATA_STATE_CHANGE (RCM ID = 0x0609)
 */
namespace ns_misc_ps_data_state_change_ind {
// Int, M
static const Json::StaticString key_expected_state("expected_state");
} // ns_misc_ps_data_state_change_ind

/*
   SIT_SET_IMS_TEST_MODE (RCM ID = 0x0614)
 */
namespace ns_misc_ps_set_ims_test_mode_req {
// Int, M
static const Json::StaticString key_mode("mode");
} // ns_misc_ps_set_ims_test_mode_req

#endif /* _MISC_JSON_DEF_H_ */
