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
#include <android/hardware/radio/1.2/ISap.h>

// RIL lyaer
#include "../../libril/hidl/ril_internal.h"

// MIN macro is duplicate in ril_internal.h and sitdef(basedef).h
#ifdef MIN
#undef MIN
#endif
// Modem SIT Layer
#include "../../sitril/protocol/sit/sitdef.h"

#include "slsi/radio_v1_4.h"
#include "slsi/radio_v1_5.h"
#include "slsi/radio_v1_6.h"
#include "slsi/radio_v2_0.h"
#include "slsi/ril_external.h"
#include "../../libril/ril_event.h"

using std::is_pod;
using std::is_trivially_copyable;
using std::is_standard_layout;
namespace DataV2_0 = android::hardware::radio::impl::data::V2_0;

// SIT struct checker
TEST(PODCheckTest, SITStructCommonTest) {
    EXPECT_EQ(true, is_pod<RCM_REQ_EXT>::value);
    EXPECT_EQ(true, is_pod<RCM_RSP_EXT>::value);
    EXPECT_EQ(true, is_pod<RCM_RSP_EXT>::value);
    EXPECT_EQ(true, is_pod<RCM_IND_EXT>::value);
    EXPECT_EQ(true, is_pod<RCM_GEN_HEADER>::value);
    EXPECT_EQ(true, is_pod<RCM_HEADER>::value);
    EXPECT_EQ(true, is_pod<RCM_IND_HEADER>::value);
    EXPECT_EQ(true, is_pod<null_data_format>::value);
    EXPECT_EQ(true, is_pod<null_ind_data_format>::value);
    EXPECT_EQ(true, is_pod<sit_simple_data_format>::value);
    EXPECT_EQ(true, is_pod<sit_simple_ind_data_format>::value);
}

TEST(PODCheckTest, SITStructCallTest) {
    EXPECT_EQ(true, is_pod<sit_call_get_current_calls_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_info_type_v1_1>::value);
    EXPECT_EQ(true, is_pod<sit_call_get_current_calls_rsp_v1_1>::value);

    EXPECT_EQ(false, is_pod<sit_call_info_type_v1_2>::value);
    EXPECT_EQ(false, is_pod<sit_call_get_current_calls_rsp_v1_2>::value);

    EXPECT_EQ(true, is_pod<sit_call_info_type_v1_6>::value);
    EXPECT_EQ(true, is_pod<sit_call_dial_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_dial_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_call_get_last_call_fail_cause_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_get_last_call_fail_cause_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_call_dtmf_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_answer_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_answer_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_call_dtmf_start_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_dtmf_stop_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_dtmf_stop_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_call_explicit_call_transfer_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_explicit_call_transfer_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_call_hangup_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_hangup_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_call_call_state_changed_ind>::value);
    EXPECT_EQ(true, is_pod<sit_call_call_ring_ind>::value);
    EXPECT_EQ(true, is_pod<sit_call_emergency_call_number_info_v1_1>::value);
    EXPECT_EQ(true, is_pod<sit_call_emergency_call_list_ind_v1_1>::value);
    EXPECT_EQ(true, is_pod<sit_call_emergency_call_number_info_v1_4>::value);
    EXPECT_EQ(true, is_pod<sit_call_emergency_call_list_ind_v1_4>::value);
    EXPECT_EQ(true, is_pod<sit_call_emergency_call_eenl_record>::value);
    EXPECT_EQ(true, is_pod<sit_call_emergency_call_eenl>::value);
    EXPECT_EQ(true, is_pod<sit_call_emergency_call_list_ind_v1_6>::value);
    EXPECT_EQ(true, is_pod<sit_call_enter_emergency_cb_ind>::value);
    EXPECT_EQ(true, is_pod<sit_call_exit_emergency_cb_ind>::value);
    EXPECT_EQ(true, is_pod<sit_call_exit_emergency_cb_mode_req>::value);
    EXPECT_EQ(true, is_pod<sit_call_exit_emergency_cb_mode_rsp>::value);
}

TEST(PODCheckTest, SITStructSmsTest) {
    EXPECT_EQ(true, is_pod<sit_sms_send_sms_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_send_sms_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_send_sms_ack_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_send_sms_ack_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_write_sms_to_sim_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_write_sms_to_sim_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_delete_sms_on_sim_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_delete_sms_on_sim_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_get_bcst_sms_cfg_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_bcst_sms_cfg_item>::value);
    EXPECT_EQ(true, is_pod<sit_sms_get_bcst_sms_cfg_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_set_bcst_sms_cfg_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_set_bcst_sms_cfg_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_act_bcst_sms_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_act_bcst_sms_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_get_smsc_addr_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_get_smsc_addr_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_set_smsc_addr_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_set_smsc_addr_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_send_sms_mem_status_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_send_sms_mem_status_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_send_ack_incoming_sms_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_send_ack_incoming_sms_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sms_new_sms_ind>::value);
    EXPECT_EQ(true, is_pod<sit_sms_new_sms_status_report_ind>::value);
    EXPECT_EQ(true, is_pod<sit_sms_new_sms_on_sim_ind>::value);
    EXPECT_EQ(true, is_pod<sit_sms_sim_sms_storage_full_ind>::value);
    EXPECT_EQ(true, is_pod<sit_sms_new_bcst_sms_ind>::value);
    EXPECT_EQ(true, is_pod<sit_sms_get_stored_sms_count_req>::value);
    EXPECT_EQ(true, is_pod<sit_sms_get_stored_sms_count_rsp>::value);
}

TEST(PODCheckTest, SITStructSimTest) {
    EXPECT_EQ(true, is_pod<sit_sim_get_sim_status_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_apps_status_info>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_sim_status_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_sim_status_rsp_ext>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_sim_pin_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_sim_pin_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_sim_puk_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_sim_puk_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_sim_pin2_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_sim_pin2_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_sim_puk2_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_sim_puk2_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_change_sim_pin_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_change_sim_pin_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_change_sim_pin2_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_change_sim_pin2_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_network_lock_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_verify_network_lock_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_sim_io_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_sim_io_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_facility_lock_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_facility_lock_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_set_facility_lock_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_set_facility_lock_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_sim_auth_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_sim_auth_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_sim_gba_auth_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_sim_gba_auth_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_apdu>::value);
    EXPECT_EQ(true, is_pod<sit_sim_transmit_sim_apdu_basic_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_transmit_sim_apdu_basic_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_open_sim_channel_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_open_channel_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_close_sim_channel_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_close_sim_channel_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_transmit_sim_apdu_channel_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_transmit_sim_apdu_channel_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_sim_status_changed_ind>::value);

    EXPECT_EQ(true, is_pod<sit_sim_open_sim_channel_with_p2_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_set_uicc_sub_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_uicc_sub_state_changed_ind>::value);
    EXPECT_EQ(true, is_pod<sit_sim_set_sim_card_power_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_slot_status_info_v1_2>::value);
    EXPECT_EQ(true, is_pod<PORT_INFO>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_slot_status_info>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_slot_status_rsp_v1_2>::value);
    EXPECT_EQ(true, is_pod<sit_sim_get_slot_status_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_slot_status_changed_ind_v1_2>::value);
    EXPECT_EQ(true, is_pod<sit_sim_slot_status_changed_ind>::value);
    EXPECT_EQ(true, is_pod<sit_sim_file_data_info_ind>::value);
    EXPECT_EQ(true, is_pod<sit_sim_set_logical_to_physical_slot_mapping_req_v1_2>::value);
    EXPECT_EQ(true, is_pod<SLOT_PORT_MAPPING>::value);
    EXPECT_EQ(true, is_pod<sit_sim_set_logical_to_physical_slot_mapping_req>::value);
    EXPECT_EQ(true, is_pod<sit_sim_tray_status_changed_ind>::value);
    EXPECT_EQ(true, is_pod<sit_stk_send_stk_envelope_cmd_req>::value);
    EXPECT_EQ(true, is_pod<sit_stk_send_stk_envelope_cmd_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_stk_send_stk_terminal_rsp_req>::value);
    EXPECT_EQ(true, is_pod<sit_stk_send_stk_terminal_rsp_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_stk_send_stk_envelope_with_status_req>::value);
    EXPECT_EQ(true, is_pod<sit_stk_send_stk_envelope_with_status_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_stk_stk_proactive_cmd_ind>::value);
    EXPECT_EQ(true, is_pod<sit_stk_sim_refresh_ind>::value);
    EXPECT_EQ(true, is_pod<sit_stk_call_setup_req>::value);
    EXPECT_EQ(true, is_pod<sit_stk_call_setup_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_stk_cc_alpha_notify_ind>::value);
}

TEST(PODCheckTest, SITStructSimPhoneBookTest) {
    EXPECT_EQ(true, is_pod<sit_read_pb_entry_req>::value);
    EXPECT_EQ(true, is_pod<sit_read_pb_resp>::value);
    EXPECT_EQ(true, is_pod<sit_update_pb_entry_req>::value);
    EXPECT_EQ(true, is_pod<sit_update_pb_entry_resp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_pb_storage_info>::value);
    EXPECT_EQ(true, is_pod<sit_sim_pb_storage_info_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_pb_capa>::value);
    EXPECT_EQ(true, is_pod<sit_sim_pb_capa_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_sim_pb_ready_ind>::value);
}

TEST(PODCheckTest, SITStructDeviceInfoTest) {
    EXPECT_EQ(true, is_pod<sit_id_get_imsi_req>::value);
    EXPECT_EQ(true, is_pod<sit_id_get_imsi_rp>::value);
    EXPECT_EQ(true, is_pod<sit_id_get_imei_req>::value);
    EXPECT_EQ(true, is_pod<sit_id_get_imei_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_id_get_imeisv_req>::value);
    EXPECT_EQ(true, is_pod<sit_id_get_imeisv_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_id_get_deviceid_req>::value);
    EXPECT_EQ(true, is_pod<sit_id_get_deviceid_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_id_set_sgc_req>::value);
    EXPECT_EQ(true, is_pod<sit_id_set_sgc_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_id_set_carrier_info_imsi_encription_req>::value);
    EXPECT_EQ(true, is_pod<sit_id_set_carrier_info_imsi_encription_rsp>::value);
    EXPECT_EQ(true, is_pod<sit_id_carrier_info_imsi_encription_ind>::value);
    EXPECT_EQ(true, is_pod<sit_set_carrier_config_manifest_req>::value);
    EXPECT_EQ(true, is_pod<sit_carrier_config_status_ind>::value);
    EXPECT_EQ(true, is_pod<sit_ind_carrier_config_sim_info>::value);
}

TEST(PODCheckTest, SITStructSupplimentaryServiceTest) {
    EXPECT_EQ(true, is_pod<sit_ss_hangup_waiting_or_background_req>::value);
    EXPECT_EQ(true, is_pod<sit_ss_hangup_waiting_or_background_rsp>::value);
}

TEST(PODCheckTest, SITStructDataServiceTest) {
    EXPECT_EQ(true, is_pod<sit_pdp_setup_data_call_req>::value);

    EXPECT_EQ(false, is_pod<sit_pdp_setup_data_call_v2_req>::value);
    EXPECT_EQ(false, is_pod<sit_pdp_setup_data_call_v3_req>::value);
}

TEST(PODCheckTest, RILStructDataServiceTest) {
    EXPECT_EQ(true, is_pod<RIL_SetupDataCallResult_V1_5>::value);
    EXPECT_EQ(true, is_pod<RIL_DataProfileInfo_V1_5>::value);
    EXPECT_EQ(true, is_pod<RIL_SetupDataCallInfo_V1_5>::value);

    /* 1.6 */
    EXPECT_EQ(true, is_pod<RIL_SliceInfo>::value);
    EXPECT_EQ(false, is_pod<RIL_SetupDataCallResult_V1_6>::value);
    EXPECT_EQ(false, is_pod<RIL_SetupDataCallInfo_V1_6>::value);
    EXPECT_EQ(false, is_pod<RIL_SlicingConfig_aosp>::value);
    EXPECT_EQ(false, is_pod<RIL_SlicingConfig>::value);
    EXPECT_EQ(false, is_pod<RIL_UrspRule_aosp>::value);
    EXPECT_EQ(false, is_pod<RIL_UrspRule>::value);
    EXPECT_EQ(false, is_pod<RIL_RouteSelectionDescriptor>::value);
    EXPECT_EQ(false, is_pod<RIL_TrafficDescriptor_aosp>::value);
    EXPECT_EQ(false, is_pod<RIL_TrafficDescriptor>::value);
    EXPECT_EQ(false, is_pod<RIL_OSAppId>::value);
    EXPECT_EQ(false, is_pod<RIL_IpDescriptors>::value);
    EXPECT_EQ(false, is_pod<RIL_QosFilter>::value);

    /* 2.0 */
    EXPECT_EQ(false, is_pod<DataV2_0::RIL_DataProfileInfo>::value);
    EXPECT_EQ(false, is_pod<DataV2_0::RIL_SetupDataCallInfo>::value);
    EXPECT_EQ(false, is_pod<DataV2_0::RIL_SlicingConfig>::value);
    EXPECT_EQ(false, is_pod<DataV2_0::RIL_TrafficDescriptor>::value);
    EXPECT_EQ(false, is_pod<DataV2_0::RIL_UrspRule>::value);
}

TEST(PODCheckTest, RILStructV2_0OtherServiceTest) {
    EXPECT_EQ(false, is_pod<android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsTechSpecificInfo>::value);
    EXPECT_EQ(false, is_pod<android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsInfo>::value);
    EXPECT_EQ(false, is_pod<android::hardware::radio::impl::sim::V2_0::RIL_CarrierRestrictions>::value);
    EXPECT_EQ(false, is_pod<android::hardware::radio::impl::sim::V2_0::RIL_CardStatus>::value);
    EXPECT_EQ(false, is_pod<android::hardware::radio::impl::network::V2_0::RIL_NitzTime>::value);
    EXPECT_EQ(false, is_pod<android::hardware::radio::impl::network::V2_0::RIL_SignalThresholdInfo>::value);
    EXPECT_EQ(true, is_pod<android::hardware::radio::impl::network::V2_0::RIL_UsageSetting>::value);
}

TEST(TriviallyCopyableTest, SITStructCommonTest) {
    EXPECT_EQ(true, is_trivially_copyable<RCM_REQ_EXT>::value);
    EXPECT_EQ(true, is_trivially_copyable<RCM_RSP_EXT>::value);
    EXPECT_EQ(true, is_trivially_copyable<RCM_RSP_EXT>::value);
    EXPECT_EQ(true, is_trivially_copyable<RCM_IND_EXT>::value);
    EXPECT_EQ(true, is_trivially_copyable<RCM_GEN_HEADER>::value);
    EXPECT_EQ(true, is_trivially_copyable<RCM_HEADER>::value);
    EXPECT_EQ(true, is_trivially_copyable<RCM_IND_HEADER>::value);
    EXPECT_EQ(true, is_trivially_copyable<null_data_format>::value);
    EXPECT_EQ(true, is_trivially_copyable<null_ind_data_format>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_simple_data_format>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_simple_ind_data_format>::value);
}

TEST(TriviallyCopyableTest, SITStructCallTest) {
    EXPECT_EQ(true, is_trivially_copyable<sit_call_get_current_calls_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_info_type_v1_1>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_get_current_calls_rsp_v1_1>::value);

    EXPECT_EQ(true, is_trivially_copyable<sit_call_info_type_v1_2>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_get_current_calls_rsp_v1_2>::value);

    EXPECT_EQ(true, is_trivially_copyable<sit_call_info_type_v1_6>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_dial_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_dial_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_get_last_call_fail_cause_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_get_last_call_fail_cause_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_dtmf_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_answer_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_answer_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_dtmf_start_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_dtmf_stop_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_dtmf_stop_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_explicit_call_transfer_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_explicit_call_transfer_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_hangup_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_hangup_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_call_state_changed_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_call_ring_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_emergency_call_number_info_v1_1>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_emergency_call_list_ind_v1_1>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_emergency_call_number_info_v1_4>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_emergency_call_list_ind_v1_4>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_emergency_call_eenl_record>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_emergency_call_eenl>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_emergency_call_list_ind_v1_6>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_enter_emergency_cb_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_exit_emergency_cb_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_exit_emergency_cb_mode_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_call_exit_emergency_cb_mode_rsp>::value);
}

TEST(TriviallyCopyableTest, SITStructSmsTest) {
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_send_sms_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_send_sms_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_send_sms_ack_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_send_sms_ack_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_write_sms_to_sim_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_write_sms_to_sim_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_delete_sms_on_sim_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_delete_sms_on_sim_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_get_bcst_sms_cfg_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_bcst_sms_cfg_item>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_get_bcst_sms_cfg_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_set_bcst_sms_cfg_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_set_bcst_sms_cfg_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_act_bcst_sms_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_act_bcst_sms_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_get_smsc_addr_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_get_smsc_addr_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_set_smsc_addr_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_set_smsc_addr_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_send_sms_mem_status_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_send_sms_mem_status_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_send_ack_incoming_sms_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_send_ack_incoming_sms_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_new_sms_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_new_sms_status_report_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_new_sms_on_sim_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_sim_sms_storage_full_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_new_bcst_sms_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_get_stored_sms_count_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sms_get_stored_sms_count_rsp>::value);
}


TEST(TriviallyCopyableTest, SITStructSimTest) {
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_sim_status_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_apps_status_info>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_sim_status_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_sim_status_rsp_ext>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_sim_pin_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_sim_pin_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_sim_puk_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_sim_puk_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_sim_pin2_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_sim_pin2_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_sim_puk2_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_sim_puk2_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_change_sim_pin_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_change_sim_pin_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_change_sim_pin2_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_change_sim_pin2_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_network_lock_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_verify_network_lock_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_sim_io_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_sim_io_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_facility_lock_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_facility_lock_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_set_facility_lock_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_set_facility_lock_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_sim_auth_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_sim_auth_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_sim_gba_auth_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_sim_gba_auth_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_apdu>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_transmit_sim_apdu_basic_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_transmit_sim_apdu_basic_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_open_sim_channel_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_open_channel_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_close_sim_channel_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_close_sim_channel_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_transmit_sim_apdu_channel_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_transmit_sim_apdu_channel_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_sim_status_changed_ind>::value);

    EXPECT_EQ(true, is_trivially_copyable<sit_sim_open_sim_channel_with_p2_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_set_uicc_sub_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_uicc_sub_state_changed_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_set_sim_card_power_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_slot_status_info_v1_2>::value);
    EXPECT_EQ(true, is_trivially_copyable<PORT_INFO>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_slot_status_info>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_slot_status_rsp_v1_2>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_get_slot_status_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_slot_status_changed_ind_v1_2>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_slot_status_changed_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_file_data_info_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_set_logical_to_physical_slot_mapping_req_v1_2>::value);
    EXPECT_EQ(true, is_trivially_copyable<SLOT_PORT_MAPPING>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_set_logical_to_physical_slot_mapping_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_tray_status_changed_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_send_stk_envelope_cmd_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_send_stk_envelope_cmd_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_send_stk_terminal_rsp_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_send_stk_terminal_rsp_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_send_stk_envelope_with_status_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_send_stk_envelope_with_status_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_stk_proactive_cmd_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_sim_refresh_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_call_setup_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_call_setup_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_stk_cc_alpha_notify_ind>::value);
}

TEST(TriviallyCopyableTest, SITStructSimPhoneBookTest) {
    EXPECT_EQ(true, is_trivially_copyable<sit_read_pb_entry_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_read_pb_resp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_update_pb_entry_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_update_pb_entry_resp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_pb_storage_info>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_pb_storage_info_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_pb_capa>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_pb_capa_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_sim_pb_ready_ind>::value);
}

TEST(TriviallyCopyableTest, SITStructDeviceInfoTest) {
    EXPECT_EQ(true, is_trivially_copyable<sit_id_get_imsi_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_get_imsi_rp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_get_imei_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_get_imei_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_get_imeisv_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_get_imeisv_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_get_deviceid_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_get_deviceid_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_set_sgc_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_set_sgc_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_set_carrier_info_imsi_encription_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_set_carrier_info_imsi_encription_rsp>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_id_carrier_info_imsi_encription_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_set_carrier_config_manifest_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_carrier_config_status_ind>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_ind_carrier_config_sim_info>::value);
}

TEST(TriviallyCopyableTest, SITStructDataServiceTest) {
    EXPECT_EQ(true, is_trivially_copyable<sit_pdp_setup_data_call_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_pdp_setup_data_call_v2_req>::value);
    EXPECT_EQ(true, is_trivially_copyable<sit_pdp_setup_data_call_v3_req>::value);
}

TEST(TriviallyCopyableTest, RILStructDataServiceTest) {
    EXPECT_EQ(true, is_trivially_copyable<RIL_SetupDataCallResult_V1_5>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_DataProfileInfo_V1_5>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SetupDataCallInfo_V1_5>::value);

    /* 1.6 */
    EXPECT_EQ(true, is_trivially_copyable<RIL_SliceInfo>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_SetupDataCallResult_V1_6>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_SetupDataCallInfo_V1_6>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_SlicingConfig_aosp>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_SlicingConfig>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_UrspRule_aosp>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_UrspRule>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_RouteSelectionDescriptor>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_TrafficDescriptor_aosp>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_TrafficDescriptor>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_OSAppId>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_IpDescriptors>::value);
    EXPECT_EQ(false, is_trivially_copyable<RIL_QosFilter>::value);

    /* 2.0 */
    EXPECT_EQ(false, is_trivially_copyable<DataV2_0::RIL_DataProfileInfo>::value);
    EXPECT_EQ(false, is_trivially_copyable<DataV2_0::RIL_SetupDataCallInfo>::value);
    EXPECT_EQ(false, is_trivially_copyable<DataV2_0::RIL_SlicingConfig>::value);
    EXPECT_EQ(false, is_trivially_copyable<DataV2_0::RIL_TrafficDescriptor>::value);
    EXPECT_EQ(false, is_trivially_copyable<DataV2_0::RIL_UrspRule>::value);
}

TEST(TriviallyCopyableTest, RILStructV2_0OtherServiceTest) {
    EXPECT_EQ(false, is_trivially_copyable<android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsTechSpecificInfo>::value);
    EXPECT_EQ(false, is_trivially_copyable<android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsInfo>::value);
    EXPECT_EQ(true, is_trivially_copyable<android::hardware::radio::impl::sim::V2_0::RIL_CarrierRestrictions>::value);
    EXPECT_EQ(true, is_trivially_copyable<android::hardware::radio::impl::sim::V2_0::RIL_CardStatus>::value);
    EXPECT_EQ(false, is_trivially_copyable<android::hardware::radio::impl::network::V2_0::RIL_NitzTime>::value);
    EXPECT_EQ(false, is_trivially_copyable<android::hardware::radio::impl::network::V2_0::RIL_SignalThresholdInfo>::value);
    EXPECT_EQ(true, is_trivially_copyable<android::hardware::radio::impl::network::V2_0::RIL_UsageSetting>::value);
}

#include <hardware/ril/librilutils/proto/sap-api.pb.h>
TEST(TriviallyCopyableTest, Libril_sitrilMemsetMemcpyCallerTest) {
    /* memset caller */
    EXPECT_EQ(true, is_trivially_copyable<android::RequestInfo>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_OptionalDNN>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_OptionalOSAppId>::value);
    EXPECT_EQ(true, is_trivially_copyable<struct ril_event>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SIM_SAP_CONNECT_REQ>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SIM_SAP_DISCONNECT_REQ>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SIM_SAP_APDU_REQ>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SIM_SAP_TRANSFER_ATR_REQ>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SIM_SAP_POWER_REQ>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SIM_SAP_RESET_SIM_REQ>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_CellInfo_V1_5>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_CellInfo_V1_4>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_CellInfo_V1_2>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_CellInfo_v12>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_DataProfileInfo_V1_5>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_DataProfileInfo_V1_4>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_Carrier>::value);

    /* memcpy caller */
    EXPECT_EQ(true, is_trivially_copyable<RIL_RadioFunctions>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_RadioExternalFunctions>::value);
    EXPECT_EQ(true, is_trivially_copyable<fd_set>::value);
}

#include "../../sitril/core/service/uicc/uiccphonebookhandler.h"
#include "../../sitril/core/service/emergency/emergencynumberupdater.h"
#include "../../sitril/stk/stkmodule.h"
TEST(TriviallyCopyableTest, LibsitrilMemsetMemcpyCallerTest) {
    /* memset caller */
    EXPECT_EQ(true, is_trivially_copyable<timeval>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_NetworkScanResult_V1_4>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_RadioCapability>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_CDMA_InformationRecords>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_CDMA_CallWaiting_v6>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_UUS_Info>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_EmergencyNumber>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_AimsEmergencyNumberList>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_Call>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_Call_V1_2>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_Call_V1_6>::value);
    EXPECT_EQ(true, is_trivially_copyable<SetupEventList>::value);
    EXPECT_EQ(true, is_trivially_copyable<SETUP_EVENTLIST>::value);
    EXPECT_EQ(true, is_trivially_copyable<NetworkInfo>::value);
    EXPECT_EQ(true, is_trivially_copyable<UiccPhonebookHandler::SIM_PhonebookCapacity>::value);
    EXPECT_EQ(true, is_trivially_copyable<android::hardware::radio::impl::sim::V2_0::RIL_CardStatus>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_CallForwardInfo>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_SuppSvcNotification>::value);
    // Checked upto sitril/core/service/supp/supphandler.cpp:981:    memset(&respSuppSvcNoti, 0x00, sizeof(RIL_SuppSvcNotification));
    // TBD

    /* memcpy caller */
    // TBD
}

#ifdef INTENTIONAL_DISABLING_TC
// Writing testcase for each external library is recommneded
// Integrating difference library can make some conflict
// This will point out what can be an issue when extern C API conflict
// Same extern "C" function name Open(), Close() has different signature for each client library
#include "../external/libsitril-embms/RileMbms.h"
// SIM_COUNT macro is duplicate in RileMbms.h and SitRilSe.h
#ifdef SIM_COUNT
#undef SIM_COUNT
#endif
#include "../external/libsitril-se/SitRilSe.h"
#include "../external/libsitril-audio/rilaudioapi.h"
#include "../external/libsitril-embms/EwLteConnector.h"
#include "../external/libsitril-common/SITRilCommon.h"
#include "../external/libsitril-client/sitril-client.h"
#include "../external/libsitril-gps/SITRilGps.h"

TEST(TriviallyCopyableTest, ExternalLibsitrilMemsetMemcpyCallerTest) {
    /* memset caller */
    EXPECT_EQ(true, is_trivially_copyable<RIL_EMBMS_AvailablityInfo_t>::value);
    EXPECT_EQ(true, is_trivially_copyable<EMBMS_CONTEXT>::value);
    /* implement specific in cpp
    EXPECT_EQ(true, is_trivially_copyable<rilc_record>::value);
    EXPECT_EQ(true, is_trivially_copyable<rilc_client>::value);
    EXPECT_EQ(true, is_trivially_copyable<SitSimOpenChannelReq>::value);
    EXPECT_EQ(true, is_trivially_copyable<SitRilSeOpenChannelResponse>::value);
    EXPECT_EQ(true, is_trivially_copyable<SitRilSeTransmitApduChannelRsp>::value);
    */
    EXPECT_EQ(true, is_trivially_copyable<LengthData>::value);

    /* memcpy caller */
    EXPECT_EQ(true, is_trivially_copyable<EwLTECON_Configration_t>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_EMBMS_NetworkInformation_t>::value);
    EXPECT_EQ(true, is_trivially_copyable<RIL_EMBMS_SessionControl>::value);
    /* implement specific in cpp
    EXPECT_EQ(true, is_trivially_copyable<SecureElementResponse>::value);
    */
}
#endif
