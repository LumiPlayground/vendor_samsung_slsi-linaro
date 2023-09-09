/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _SIM_JSON_DEF_H_
#define _SIM_JSON_DEF_H_

#include "json/json.h"

/******************************************************************************
 *
 * File: simjsondef.h
 *
 * Description: This file will have key-value pair definition of all SIM SIT commands
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

// SIM

/*
    SIT_GET_SIM_STATUS (RCM ID = 0x0200)
*/
namespace ns_sim_get_sim_status_req { }

namespace ns_sim_get_sim_status_rsp {
// Int, M
static const Json::StaticString key_card_state("card_state"); // sit_sim_card_state_e_type
// Int, M
static const Json::StaticString key_univ_pin_state("univ_pin_state"); // sit_sim_pin_state_e_type
// Obj[], M
static const Json::StaticString key_app_status_info_list("app_info_list"); // see ns_sim_app_status_info
// Int, M
static const Json::StaticString key_esim_no_profile("esim_no_profile");
// Int, M
static const Json::StaticString key_phy_slotid("phy_slotid");
// Int[], M
static const Json::StaticString key_atr("atr");
// Int[], M
static const Json::StaticString key_iccid("iccid");
// Int[], M
static const Json::StaticString key_eid("eid");
} // namespace ns_sim_get_sim_status_rsp

namespace ns_sim_apps_status_info {
// Int, M
static const Json::StaticString key_app_type("app_type"); // sit_sim_apps_type_e_type
// Int, M
static const Json::StaticString key_app_state("app_state"); // sit_sim_apps_state_e_type
// Int, M
static const Json::StaticString key_perso_substate("perso_substate"); // sit_sim_perso_substate_e_type
// Int[], M
static const Json::StaticString key_aid("aid");
// Int[], M
static const Json::StaticString key_app_label("app_label");
// Int, M
static const Json::StaticString key_pin1_replaced("pin1_replaced");
// Int, M
static const Json::StaticString key_pin1_state("pin1_state"); // sit_sim_pin_state_e_type
// Int, M
static const Json::StaticString key_pin2_state("pin2_state"); // sit_sim_pin_state_e_type
// Int, M
static const Json::StaticString key_pin1_remain_cnt("pin1_remain_cnt");
// Int, M
static const Json::StaticString key_puk1_remain_cnt("puk1_remain_cnt");
// Int, M
static const Json::StaticString key_pin2_remain_cnt("pin2_remain_cnt");
// Int, M
static const Json::StaticString key_puk2_remain_cnt("puk2_remain_cnt");
} // namespace ns_sim_apps_status_info

/*
   SIT_VERIFY_SIM_PIN (RCM ID = 0x0201)
*/
namespace ns_sim_verify_sim_pin_req {
// String, M
static const Json::StaticString key_pin("pin");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_verify_sim_pin_req

namespace ns_sim_verify_sim_pin_rsp {
// Int, M
static const Json::StaticString key_remain_cnt("remain_cnt");
} // namespace ns_sim_verify_sim_pin_rsp

/*
   SIT_VERIFY_SIM_PUK (RCM ID = 0x0202)
*/
namespace ns_sim_verify_sim_puk_req {
// String, M
static const Json::StaticString key_puk("puk");
// String, M
static const Json::StaticString key_new_pin("new_pin");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_verify_sim_puk_req

namespace ns_sim_verify_sim_puk_rsp {
// Int, M
static const Json::StaticString key_remain_cnt("remain_cnt");
} // namespace ns_sim_verify_sim_puk_rsp

/*
   SIT_VERIFY_SIM_PIN2 (RCM ID = 0x0203)
*/
namespace ns_sim_verify_sim_pin2_req {
// String, M
static const Json::StaticString key_pin("pin");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_verify_sim_pin_req

namespace ns_sim_verify_sim_pin2_rsp {
// Int, M
static const Json::StaticString key_remain_cnt("remain_cnt");
} // namespace ns_sim_verify_sim_pin_rsp

/*
   SIT_VERIFY_SIM_PUK2 (RCM ID = 0x0204)
*/
namespace ns_sim_verify_sim_puk2_req {
// String, M
static const Json::StaticString key_puk("puk");
// String, M
static const Json::StaticString key_new_pin("new_pin");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_verify_sim_puk_req

namespace ns_sim_verify_sim_puk2_rsp {
// Int, M
static const Json::StaticString key_remain_cnt("remain_cnt");
} // namespace ns_sim_verify_sim_puk_rsp

/*
   SIT_CHG_SIM_PIN (RCM ID = 0x0205)
 */
namespace ns_sim_change_sim_pin_req {
// Int, M
static const Json::StaticString key_fac_type("fac_type");  // sit_sim_fac_lock_type_e_type
// String, M
static const Json::StaticString key_old_pin("old_pin");
// String, M
static const Json::StaticString key_new_pin("new_pin");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_change_sim_pin_req

namespace ns_sim_change_sim_pin_rsp {
// Int, m
static const Json::StaticString key_remain_cnt("remain_cnt");
} // namespace ns_sim_change_sim_pin_rsp

/*
   SIT_CHG_SIM_PIN2 (RCM ID = 0x0206)
 */
namespace ns_sim_change_sim_pin2_req {
// Int, M
static const Json::StaticString key_fac_type("fac_type");  // sit_sim_fac_lock_type_e_type
// String, M
static const Json::StaticString key_old_pin("old_pin");
// String, M
static const Json::StaticString key_new_pin("new_pin");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_change_sim_pin2_req

namespace ns_sim_change_sim_pin2_rsp {
// Int, m
static const Json::StaticString key_remain_cnt("remain_cnt");
} // namespace ns_sim_change_sim_pin2_rsp

/*
   SIT_VERIFY_NETWORK_LOCK  (RCM ID = 0x0207)
*/
namespace ns_sim_verify_network_lock_req {
// Int, M
static const Json::StaticString key_fac_type("fac_type");  // sit_sim_fac_lock_type_e_type
// String, M
static const Json::StaticString key_pw("pw");
// Int, M
static const Json::StaticString key_class("class");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_verify_network_lock_req

namespace ns_sim_verify_network_lock_rsp {
// Int, M
static const Json::StaticString key_remain_cnt("remain_cnt");
} // namespace ns_sim_verify_network_lock_rsp

/*
   SIT_SIM_IO (RCM ID = 0x0208)
 */
namespace ns_sim_sim_io_req {
// Int, M
static const Json::StaticString key_cmd("cmd"); // sit_sim_sim_io_command_e_type
// Int, M
static const Json::StaticString key_app_type("app_type");
// Int, M
static const Json::StaticString key_file_id("file_id");
// Int[], M
static const Json::StaticString key_path("path");
// Int, M
static const Json::StaticString key_p1("p1");
// Int, M
static const Json::StaticString key_p2("p2");
// Int, M
static const Json::StaticString key_p3("p3");
// Int[], M
static const Json::StaticString key_data("data");
// String, M
static const Json::StaticString key_pin2("pin2");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_sim_io_req

namespace ns_sim_sim_io_rsp {
// Int, M
static const Json::StaticString key_sw1("sw1");
// Int, M
static const Json::StaticString key_sw2("sw2");
// Int[], M
static const Json::StaticString key_rsp("rsp");
} // namespace ns_sim_sim_io_rsp

/*
    SIT_GET_FACILITY_LOCK (RCM ID = 0x0209)
*/
namespace ns_sim_get_facility_lock_req {
// Int, M
static const Json::StaticString key_fac_type("fac_type");  // sit_sim_fac_lock_type_e_type
// String, M
static const Json::StaticString key_pw("pw");
// Int, M
static const Json::StaticString key_class("class");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_get_facility_lock_req

namespace ns_sim_get_facility_lock_rsp {
// Int, M
static const Json::StaticString key_fac_lock_status("fac_lock_status");
} // namespace ns_sim_get_facility_lock_rsp

/*
    SIT_SET_FACILITY_LOCK (RCM ID = 0x020A)
*/
namespace ns_sim_set_facility_lock_req {
// Int, M
static const Json::StaticString key_fac_type("fac_type");  // sit_sim_fac_lock_type_e_type
// Int, M
static const Json::StaticString key_lock_mode("lock_mode");
// string, M
static const Json::StaticString key_pw("pw");
// Int, M
static const Json::StaticString key_class("class");
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_set_facility_lock_req

namespace ns_sim_set_facility_lock_rsp {
// Int, M
static const Json::StaticString key_remain_cnt("remain_cnt");
} // namespace ns_sim_set_facility_lock_rsp

/*
   SIT_GET_SIM_AUTH  (RCM ID = 0x020B)
 */
namespace ns_sim_get_sim_auth_req {
// Int, M
static const Json::StaticString key_auth_type("auth_type"); // sit_sim_get_sim_auth_type
// Int, M
static const Json::StaticString key_rand("rand");
// Int, C
static const Json::StaticString key_autn("autn");  // not sent for GSM auth
} // namespace ns_sim_get_sim_auth_req

namespace ns_sim_get_sim_auth_rsp {
// Int, M
static const Json::StaticString key_auth_type("auth_type");
// Int, M
static const Json::StaticString key_auth_result("auth_result");
// Int[], M
static const Json::StaticString key_res_auts("res_auts");
// Int[], C
static const Json::StaticString key_ck("ck"); // not used for GSM auth
// Int[], C
static const Json::StaticString key_ik("ik"); // not used for GSM auth
// Int[], M
static const Json::StaticString key_kc("kc");
// Int, M
static const Json::StaticString key_sw1("sw1");
// Int, M
static const Json::StaticString key_sw2("sw2");
} // namespace ns_sim_get_sim_auth_rsp

/*
   SIT_TRANSMIT_SIM_APDU_BASIC (RCM ID = 0x020C)
 */
namespace ns_sim_transmit_sim_apdu_basic_req {
// Int, M
static const Json::StaticString key_sid("sid");
// Int[], M
static const Json::StaticString key_data("data");
} // namespace ns_sim_transmit_sim_apdu_basic_req

namespace ns_sim_transmit_sim_apdu_basic_rsp {
// Int[], M
static const Json::StaticString key_rsp("rsp");
} // namespace ns_sim_transmit_sim_apdu_basic_rsp

/*
   SIT_OPEN_SIM_CHANNEL (RCM ID = 0x020D)
 */
namespace ns_sim_open_sim_channel_req {
// Int[], M
static const Json::StaticString key_aid("aid");
} // namespace ns_sim_open_sim_channel_req

namespace ns_sim_open_sim_channel_rsp {
// UInt, M
static const Json::StaticString key_sid("sid");
// Int, M
static const Json::StaticString key_sw1("sw1");
// Int, M
static const Json::StaticString key_sw2("sw2");
// Int[], M
static const Json::StaticString key_rsp("rsp");
} // namespace ns_sim_open_sim_channel_rsp

/*
    SIT_CLOSE_SIM_CHANNEL (RCM ID = 0x020E)
*/
namespace ns_sim_close_sim_channel_req {
// Int, M
static const Json::StaticString key_sid("sid");
} // namespace sim_close_sim_channel_req

namespace ns_sim_close_sim_channel_rsp { }

/*
   SIT_TRANSMIT_SIM_APDU_CHANNEL (RCM ID = 0x020F)
 */
namespace ns_sim_transmit_sim_apdu_channel_req {
// Int, M
static const Json::StaticString key_sid("sid");
// Int, M
static const Json::StaticString key_cla("cla");
// Int, M
static const Json::StaticString key_instruction("ins");
// Int, M
static const Json::StaticString key_p1("p1");
// Int, M
static const Json::StaticString key_p2("p2");
// Int, M
static const Json::StaticString key_p3("p3");
// Int[], M
static const Json::StaticString key_data("data");
} // namespace ns_sim_transmit_sim_apdu_channel_req

namespace ns_sim_transmit_sim_apdu_channel_rsp {
// Int, M
static const Json::StaticString key_sw1("sw1");
// Int, M
static const Json::StaticString key_sw2("sw2");
// Int[], M
static const Json::StaticString key_rsp("rsp");
} // namespace ns_sim_transmit_sim_apdu_channel_rsp

/*
   SIT_IND_SIM_STATUS_CHANGED (RCM ID = 0x0210)
 */
namespace ns_sim_sim_status_changed_ind { }

/*
   SIT_GET_GBA_CONTEXT  (RCM ID = 0x0211)
 */
namespace ns_sim_get_sim_gba_auth_req {
// Int, M
static const Json::StaticString key_auth_type("auth_type"); // sit_sim_get_sim_gba_auth_type
// Int, M
static const Json::StaticString key_gba_type("gba_type");
// Int, M
static const Json::StaticString key_gba_tag("gba_tag");
// Int[], M
static const Json::StaticString key_data1("data1");
// Int[], M
static const Json::StaticString key_data2("data2");
}

namespace ns_sim_get_sim_gba_auth_rsp {
// Int, M
static const Json::StaticString key_result("result");
// Int, M
static const Json::StaticString key_auth_type("auth_type"); // sit_sim_get_sim_gba_auth_type
// Int, M
static const Json::StaticString key_gba_type("gba_type");
// Int, M
static const Json::StaticString key_gba_tag("gba_tag");
// Int[], M
static const Json::StaticString key_data1("data1");
} // namespace ns_sim_get_sim_gba_auth_rsp

/*
   SIT_GET_ATR (RCM ID = 0x0212)
 */
namespace ns_sim_id_get_atr_req { }

namespace ns_sim_id_get_atr_rsp {
// Int, M
static const Json::StaticString key_result("result");
// Int[], M
static const Json::StaticString key_atr("atr");
} // namespace ns_ns_id_get_atr_rsp

/*
   SIT_SAP_CONNECT (RCM ID = 0x0220)
 */
namespace ns_sap_connect_req {
// Int, M
static const Json::StaticString key_max_msg_size("max_msg_size");
} // namespace ns_sap_connect_req

namespace ns_sap_connect_rsp {
// Int, M
static const Json::StaticString key_status("status"); // 0 : success, 1 : failure
// Int, M
static const Json::StaticString key_max_msg_size("max_msg_size");
} // namespace ns_sap_connect_rsp

/*
   SIT_SAP_DISCONNECT (RCM ID = 0x0221)
 */
namespace ns_sap_disconnect_req { }

namespace ns_sap_disconnect_rsp {
// Int, M
static const Json::StaticString key_status("status"); // 0 : success, 1 : failure
} // namespace ns_sap_disconnect_rsp

/*
   SIT_IND_SAP_DISCONNECT (RCM ID = 0x0222)
 */
namespace ns_sap_disconnect_ind {
// Int, M
static const Json::StaticString key_disconnect_type("disconnect_type"); // 0 : Graceful, 1 : Immediate
} // namespace ns_sap_disconnect_ind

/*
   SIT_SAP_TRANSFER_APDU (RCM ID = 0x0223)
 */
namespace ns_sap_transfer_apdu_req {
// Int[], M
static const Json::StaticString key_req_apdu("apdu");
} // namespace ns_sap_transfer_apdu_req

namespace ns_sap_transfer_apdu_rsp {
// Int, M
static const Json::StaticString key_result("result");
// Int[], M
static const Json::StaticString key_rsp_apdu("rsp_apdu");
} // namespace ns_sap_transfer_apdu_rsp

/*
   SIT_SAP_TRANSFER_ATR (RCM ID = 0x0224)
 */
namespace ns_sap_transfer_atr_req { }

namespace ns_sap_transfer_atr_rsp {
// Int, M
static const Json::StaticString key_result("result");
// Int[], M
static const Json::StaticString key_atr("atr");
} // namespace ns_sap_transfer_atr_rsp

/*
  SIT_SAP_POWER_SIM_OFF (RCM ID = 0x0225)
  SIT_SAP_POWER_SIM_ON (RCM ID = 0x0226)
*/
namespace ns_sap_power_sim_onoff_req { }

namespace ns_sap_power_sim_onoff_rsp {
// Int, M
static const Json::StaticString key_result("result"); // 0 : OK, 1 : Error, no reason
} // namespace ns_sap_power_sim_onoff_rsp

/*
  SIT_SAP_RESET_SIM (RCM ID = 0x0227)
*/
namespace ns_sap_reset_sim_req { }

namespace ns_sap_reset_sim_rsp {
// Int, M
static const Json::StaticString key_result("result"); // 0 : OK, 1 : Error, no reason
} // namespace ns_sap_reset_sim_rsp

/*
   SIT_IND_SAP_STATUS (RCM ID = 0x0228)
 */
namespace ns_sap_status_ind {
// Int, M
static const Json::StaticString key_card_status("status");
} // namespace ns_sap_status_ind

/*
   SIT_SAP_TRANSFER_CARD_READER_STATUS (RCM ID = 0x0229)
 */
namespace ns_sap_transfer_card_reader_status_req { }

namespace ns_sap_transfer_card_reader_status_rsp {
// Int, M
static const Json::StaticString key_result("result"); // 0 : OK, 1 : Error, no reason
} // namespace ns_sap_transfer_card_reader_status_rsp

/*
   SIT_SAP_SET_TRANPORT_PROTOCOL (RCM ID = 0x022A)
 */
namespace ns_sap_tranport_protocol_req {
// Int, M
static const Json::StaticString key_protocol("protocol");
} // namespace ns_sap_tranport_protocol_req

namespace ns_sap_tranport_protocol_rsp {
// Int, M
static const Json::StaticString key_result("result"); // 0 : OK, 1 : Error, no reason
} // namespace ns_sap_tranport_protocol_rsp

/*
   SIT_READ_PB_ENTRY (RCM_ID = 0x0240)
 */
namespace ns_read_pb_entry_req {
// Int, M
static const Json::StaticString key_pb_type("pb_type");
// Int, M
static const Json::StaticString key_record_id("index");
} // namespace ns_read_pb_entry_req

namespace ns_read_pb_resp {
// Int, M
static const Json::StaticString key_pb_type("pb_type");
// Int, M
static const Json::StaticString key_record_id("index");
// Int[], M
static const Json::StaticString key_entry_data("data");
} // namespace ns_read_pb_resp

/*
   SIT_UPDATE_PB_ENTRY (RCM_ID = 0x0241)
 */
namespace ns_update_pb_entry_req {
// Int, M
static const Json::StaticString key_mode("mode");
// Int, M
static const Json::StaticString key_pb_type("pb_type");
// Int, M
static const Json::StaticString key_record_id("index");
// Int[], C
static const Json::StaticString key_entry_data("data"); // not sent when mode = 2 (DELETE)
} // namespace ns_update_pb_entry_req

namespace ns_update_pb_entry_resp {
// Int, M
static const Json::StaticString key_mode("mode");
// Int, M
static const Json::StaticString key_pb_type("pb_type");
// Int, M
static const Json::StaticString key_record_id("index");
} // namespace ns_update_pb_entry_resp

/*
   SIT_GET_PB_STORAGE_INFO (RCM_ID = 0x0242)
 */
namespace ns_sim_pb_storage_info {
// Int, M
static const Json::StaticString key_pb_type("pb_type");
} // namespace ns_sim_pb_storage_info

namespace ns_sim_pb_storage_info_rsp {
// Int, M
static const Json::StaticString key_pb_type("pb_type");
// Int, M
static const Json::StaticString key_total_cnt("total_cnt");
// Int, M
static const Json::StaticString key_used_cnt("used_cnt");
} // namespace ns_sim_pb_storage_info_rsp

/*
   SIT_GET_3G_PB_CAPA (RCM_ID = 0x0245)
 */
namespace ns_sim_pb_capa { }

namespace ns_sim_pb_capa_rsp {
// Obj[], M
static const Json::StaticString key_pb_cap_list("pb_cap_list"); // see ns_pb_cap_list
} // namespace ns_sim_pb_capa_rsp

namespace ns_pb_cap_list {
// Int, M
static const Json::StaticString key_pb_type("pb_type");
// Int, M
static const Json::StaticString key_max_index("max_indx");
// Int, M
static const Json::StaticString key_max_entry("max_entry");
// Int, M
static const Json::StaticString key_used_cnt("used_cnt");
} // namespace ns_pb_cap_list

/*
   SIT_SIM_IND_PB_READY (RCM ID = 0x0246)
 */
namespace ns_sim_pb_ready_ind {
// Int, M
static const Json::StaticString key_pb_status("pb_status");
} // namespace ns_sim_pb_ready_ind

/*
   SIT_OPEN_SIM_CHANNEL_WITH_P2 (RCM ID = 0x0247)
 */
namespace ns_sim_open_sim_channel_with_p2_req {
// Int[], M
static const Json::StaticString key_aid("aid");
// Int, M
static const Json::StaticString key_p2("p2");
} // namespace ns_sim_open_sim_channel_with_p2_req

namespace ns_sim_open_sim_channel_with_p2_rsp {
// UInt, M
static const Json::StaticString key_sid("sid");
// Int, M
static const Json::StaticString key_sw1("sw1");
// Int, M
static const Json::StaticString key_sw2("sw2");
// Int[], M
static const Json::StaticString key_rsp("rsp");
} // namespace ns_sim_open_sim_channel_with_p2_rsp

/*
   SIT_SET_UICC_SUBSCRIPTION (RCM ID = 0x0249)
 */
namespace ns_sim_set_uicc_sub_req {
// Int, M
static const Json::StaticString key_state("state"); // 0: deactivated, 1: activated (same to RIL_UiccSubActStatus)
} // namespace ns_sim_set_uicc_sub_req

namespace ns_sim_set_uicc_sub_rsp { }

/*
   SIT_IND_UICC_SUBSCRIPTION_STATE_CHANGED (RCM ID = 0x024A)
 */
namespace ns_sim_uicc_sub_state_changed_ind {
// Int, M
static const Json::StaticString key_state("state"); // 0: deactivated, 1: activated (same to RIL_UiccSubActStatus)
} // namespace ns_sim_uicc_sub_state_changed_ind

/*
   SIT_SET_SIM_CARD_POWER (RCM ID = 0x024C)
 */
namespace ns_sim_set_sim_card_power_req {
// Int, M
static const Json::StaticString key_mode("mode"); // 0: UP, 1: DOWON
} // namespace ns_sim_set_sim_card_power_req

namespace ns_sim_set_sim_card_power_rsp { }

/*
   SIT_GET_SLOT_STATUS (RCM ID = 0x024D)
 */
namespace ns_sim_get_slot_status_req { }

namespace ns_sim_get_slot_status_resp {
// Obj[], M
static const Json::StaticString key_slot_status_info_list("slot_info_list"); // see ns_sim_slot_status_info
} // namespace ns_sim_get_slot_status_resp

namespace ns_sim_slot_status_info {
// Int, M
static const Json::StaticString key_card_state("card_state"); // RIL_CardState;
// Int, M
static const Json::StaticString key_slot_state("slot_state"); // RIL_SlotState
// Int[], M
static const Json::StaticString key_atr("atr");
// Int, M
static const Json::StaticString key_log_slot_id("log_slot_id");
// Int[], M
static const Json::StaticString key_iccid("iccid");
// Int[], M
static const Json::StaticString key_eid("eid");
} // namespace ns_sim_slot_status_info

/*
   SIT_IND_SIM_SLOT_STATUS_CHANGED (RCM ID = 0x024E)
 */
namespace ns_sim_slot_status_changed_ind {
// Obj[], M
static const Json::StaticString key_slot_status_info_list("slot_info_list"); // see ns_sim_slot_status_info
} // namespace ns_sim_slot_status_changed_ind

/*
    SIT_IND_SIM_DATA_INFO (RCM ID = 0x024F)
 */
// check ns_net_sim_file_data_info_ind in netjsondef.h

/*
    SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING (RCM ID = 0x0250)
 */
namespace ns_sim_set_logical_to_physical_slot_mapping_req {
// Int[], M
static const Json::StaticString key_slot_map("slot_map");
}  // namespace ns_sim_set_logical_to_physical_slot_mapping_req

namespace ns_sim_set_logical_to_physical_slot_mapping_rsp { }

/*
   SIT_IND_SIM_TRAY_STATUS_CHANGED (RCM ID = 0x0251)
 */
namespace ns_sim_tray_status_changed_ind {
// Int, M
static const Json::StaticString key_tray_status("tray_status"); // 0:unknown, 1: inserted, 2: removed
} // namespace ns_sim_tray_status_changed_ind



/******************************************************************************
 * Network
 *****************************************************************************/
/*
    SIT_SET_CARRIER_RESTRICTIONS (RCM ID = 0x072E)
*/
namespace ns_sim_net_getset_carrier_restrictions {
// Obj[], M
static const Json::StaticString key_allowed_list("allowed_list");
// Obj[], M
static const Json::StaticString key_excluded_list("excluded_list");
// Int, M
static const Json::StaticString key_allowed_priority("allowed_priority");    // 0: false, 1:true
// Int, M
static const Json::StaticString key_multi_simlock("multi_simlock");         // sit_multi_sim_lock_policy_e_type
}

namespace ns_sim_net_carrier_info {
// String, M
static const Json::StaticString key_plmn("plmn");
// Int, M
static const Json::StaticString key_match_type("match_type");    // sit_carrier_match_type_e_type
// Int[], M
static const Json::StaticString key_match_data_list("match_data_list");
}

/*
    SIT_GET_CARRIER_RESTRICTIONS (RCM ID = 0x072F)
*/
// same to ns_sim_net_getset_carrier_restrictions

/********************************************************************************
 * ID
 ********************************************************************************/
/*
   SIT_GET_IMSI (RCM ID = 0x0400)
 */
namespace ns_sim_id_get_imsi_req {
// Int[], M
static const Json::StaticString key_aid("aid");
}

namespace ns_sim_id_get_imsi_rsp {
// String, M
static const Json::StaticString key_imsi("imsi");
}

/******************************************************************************
 * ps
 *****************************************************************************/
/*
   SIT_GET_PHONE_CAPABILITY (RCM ID = 0x0615)
 */
namespace ns_sim_ps_get_phone_capability_req {
} // ns_ps_get_phone_capability_req

namespace ns_sim_ps_get_phone_capability_rsp {
// Int, M
static const Json::StaticString key_max_simultaneous_data_stack("max_simultaneous_data_stack");
// Int, M
static const Json::StaticString key_max_simultaneous_internet_pdn("max_simultaneous_internet_pdn");
// Int, M
static const Json::StaticString key_internet_lingering_support("internet_lingering_support");
// Int, M
static const Json::StaticString key_max_supported_stack("max_supported_stack");
} // ns_sim_ps_get_phone_capability_rsp

#endif /* _SIM_JSON_DEF_H_ */
