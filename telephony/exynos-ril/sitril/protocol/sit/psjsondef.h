/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _PS_JSON_DEF_H_
#define _PS_JSON_DEF_H_

#include "commondef.h"
#include "json/json.h"

/******************************************************************************
 *
 * File: psjsondef.h
 *
 * Description: This file will have key-value pair definition of all PS SIT commands
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

/*
   SIT_SETUP_DATA_CALL (RCM ID = 0x0600)
 */
namespace ns_ps_setup_data_call_req {
// Int, M
static const Json::StaticString key_cid("cid");
// Int, M
static const Json::StaticString key_rat("rat"); // sit_ps_rat_e_type
// Obj, M
static const Json::StaticString key_apn_info("apn_info"); // ns_ps_apn_info
// Int[], M
static const Json::StaticString key_ipv4_address("ipv4_address");
// Int[], M
static const Json::StaticString key_ipv6_address("ipv6_address");
// Int, M
static const Json::StaticString key_apn_type_bitmask("apn_type_bitmask");
// Int, O
static const Json::StaticString key_req_ipc_version("req_ipc_version");
// Int, O
static const Json::StaticString key_pdu_session_id("pdu_session_id");
// Int, O
static const Json::StaticString key_snssai_type("snssai_type");
// Int, O
static const Json::StaticString key_slice_service_type("slice_service_type");// sit_ps_slice_service_type_e_type
// Int, O
static const Json::StaticString key_slice_differentiator("slice_differentiator");
// Int, O
static const Json::StaticString key_mapped_hplmn_sst("mapped_hplmn_sst");// sit_ps_slice_service_type_e_type
// Int, O
static const Json::StaticString key_mapped_hplmn_sd("mapped_hplmn_sd");
// Int, O
static const Json::StaticString key_match_all_rule_allowed("match_all_rule_allowed");
// Obj, O
static const Json::StaticString key_traffic_descriptor("req_traffic_descriptor"); // ns_ps_data_call_traffic_descriptor
} // ns_ps_setup_data_call_req

namespace ns_ps_setup_data_call_rsp {
// Int, M
static const Json::StaticString key_data_call_status("data_call_status"); // sit_ps_call_state_e_type
// Int, M
static const Json::StaticString key_cid("cid");
// Int, M
static const Json::StaticString key_pdp_status("pdp_status");// sit_ps_pdp_state_e_type
// Int, M
static const Json::StaticString key_data_call_ipc_version("data_call_ipc_version");
// Long, M
static const Json::StaticString key_retry_timer("retry_timer");
// Obj, M
static const Json::StaticString key_call_info("call_info");
} // ns_ps_setup_data_call_rsp

namespace ns_ps_call_info {
// Int, M
static const Json::StaticString key_pdp_type("pdp_type"); // sit_ps_pdp_type_e_type
// Int[], O
static const Json::StaticString key_ipv4_address("ipv4_address");
// Int[], O
static const Json::StaticString key_ipv6_address("ipv6_address");
// Int, M
static const Json::StaticString key_dns_type("dns_type"); // sit_ps_dns_type_e_type
// Int[], O
static const Json::StaticString key_primary_dns_ipv4("primary_dns_ipv4");
// Int[], M
static const Json::StaticString key_primary_dns_ipv6("primary_dns_ipv6");
// Int[], M
static const Json::StaticString key_secondary_dns_ipv4("secondary_dns_ipv4");
// Int[], M
static const Json::StaticString key_secondary_dns_ipv6("secondary_dns_ipv6");
// Int, M
static const Json::StaticString key_pcscf_type("pcscf_type");
// Int[], M
static const Json::StaticString key_pcscf_ipv4("pcscf_ipv4");
// Int[], M
static const Json::StaticString key_pcscf_ipv6("pcscf_ipv6");
// UInt, M
static const Json::StaticString key_ipv4_mtu("ipv4_mtu");
// Int, M
static const Json::StaticString key_pco_value("pco_value");
// Int, M
static const Json::StaticString key_reserved("reserved");
// Int[], O
static const Json::StaticString key_pcscf_ipv4_ext("pcscf_ipv4_ext");
// Int[], O
static const Json::StaticString key_pcscf_ipv6_ext("pcscf_ipv6_ext");
// Int[], M
static const Json::StaticString key_apn_ambr("apn_ambr");
// UInt, M
static const Json::StaticString key_ipv6_mtu("ipv6_mtu");
// UInt, O
static const Json::StaticString key_pdu_session_id("pdu_session_id");
// Int, O
static const Json::StaticString key_qos_rat_type("qos_rat_type"); // sit_ps_rat_type_e_type
// Obj, O
static const Json::StaticString key_qos("qos"); // ns_ps_nr_qos_information or ns_ps_lte_qos_information
// Int, O
static const Json::StaticString key_snssai_type("snssai_type");
// Int, O
static const Json::StaticString key_slice_service_type("slice_service_type");
// Int, O
static const Json::StaticString key_slice_differentiator("slice_differentiator");
// Int, O
static const Json::StaticString key_mapped_hplmn_sst("mapped_hplmn_sst");
// Int, O
static const Json::StaticString key_mapped_hplmn_sd("mapped_hplmn_sd");
// Obj[], O
static const Json::StaticString key_traffic_descriptor_list("rsp_traffic_descriptor_list"); // ns_ps_data_call_traffic_descriptor
} // ns_ps_call_info

namespace ns_ps_apn_info {
// Int, M
static const Json::StaticString key_data_profile("data_profile"); // sit_ps_data_profile_e_type
// Int, M
static const Json::StaticString key_apn_type("apn_type"); // sit_ps_apn_type_e_type
// String, M
static const Json::StaticString key_apn("apn");
// String, M
static const Json::StaticString key_username("username");
// String, M
static const Json::StaticString key_password("password");
// Int, M
static const Json::StaticString key_auth_type("auth_type"); // sit_ps_auth_type_e_type
// Int, M
static const Json::StaticString key_pdp_type("pdp_type"); // sit_ps_pdp_type_e_type
// Int, M
static const Json::StaticString key_pcscf_addr_req("pcscf_addr_req");
} // ns_ps_apn_info

namespace ns_ps_data_call_traffic_descriptor {
// Int, M
static const Json::StaticString key_td_bit_mask("td_bit_mask");
// String, C
static const Json::StaticString key_dnn("dnn");
// Int[], C
static const Json::StaticString key_os_app_id("os_app_id");
// Int[], C
static const Json::StaticString key_os_id("os_id");
} // ns_ps_data_call_traffic_descriptor

/*
   SIT_DEACT_DATA_CALL (RCM ID = 0x0601)
 */
namespace ns_ps_deact_data_call_req {
// Int, M
static const Json::StaticString key_cid("cid");
// Int, M
static const Json::StaticString key_reason("reason");// sit_ps_reason_e_type
} // ns_ps_deact_data_call_req

/*
   SIT_SET_INITIAL_ATTACH_APN (RCM ID = 0x0603)
 */
namespace ns_ps_set_initial_attach_apn_req {
// Int, M
static const Json::StaticString key_cid("cid");
// Int, M
static const Json::StaticString key_rat("rat"); // sit_ps_rat_e_type
// Obj, M
static const Json::StaticString key_apn_info("apn_info"); // ns_ps_apn_info
// Int[], M
static const Json::StaticString key_ipv4_address("ipv4_address");
// Int[], M
static const Json::StaticString key_ipv6_address("ipv6_address");
// Int, O
static const Json::StaticString key_profile_id("profile_id");
// Int, O
static const Json::StaticString key_apn_disable_flag("apn_disable_flag");
// Int, O
static const Json::StaticString key_max_pdn_conn_per_block("max_pdn_conn_per_block");
// Int, O
static const Json::StaticString key_max_pdn_conn_timer("max_pdn_conn_timer");
// Int, O
static const Json::StaticString key_pdn_req_wait_interval("pdn_req_wait_interval");
// Int, M
static const Json::StaticString key_roaming_pdp_type("roaming_pdp_type");
// Int, M
static const Json::StaticString key_roaming_pcscf_req_type("roaming_pcscf_req_type");
// Int, M
static const Json::StaticString key_priority("priority");
}// namesapce ns_ps_set_initial_attach_apn_req

namespace ns_ps_set_initial_attach_apn_rsp { }

/*
   SIT_IND_DATA_CALL_LIST_CHANGED (RCM ID = 0x0604)
 */
namespace ns_ps_data_call_list_changed_ind {
// Int, M
static const Json::StaticString key_data_call_info_list("data_call_info_list"); // ns_ps_setup_data_call_rsp
} // ns_ps_data_call_list_changed_ind

/*
   SIT_SET_FD_INFO (RCM ID = 0x0605)
 */
namespace ns_ps_set_fd_info_req {
// Int, M
static const Json::StaticString key_lcd_on_timer("lcd_on_timer");
// Int, M
static const Json::StaticString key_lcd_off_timer("lcd_off_timer");
// Int, M
static const Json::StaticString key_rel8_lcd_on_timer("rel8_lcd_on_timer");
// Int, M
static const Json::StaticString key_rel8_lcd_off_timer("rel8_lcd_off_timer");
} // ns_ps_set_fd_info_req

namespace ns_ps_set_fd_info_rsp {
} // ns_ps_set_fd_info_rsp

/*
   SIT_IND_NAS_TIMER_STATUS (RCM ID = 0x0607)
 */
namespace ns_ps_nas_timer_status_ind {
// Int, M
static const Json::StaticString key_timer_type("timer_type");
// Int, M
static const Json::StaticString key_timer_status("timer_status");
// Int, M
static const Json::StaticString key_timer_value("timer_value");
// String , M
static const Json::StaticString key_apn("apn");
} // ns_ps_data_call_list_changed_ind

/*
   SIT_DETACH (RCM ID = 0x0608)
 */
namespace ns_ps_detach_req {
// Int, M
static const Json::StaticString key_detach_reason("detach_reason");
} // ns_ps_detach_req

namespace ns_ps_set_fd_info_rsp {
} // ns_ps_set_fd_info_rsp

/*
   SIT_IND_DATA_STATE_CHANGE (RCM ID = 0x0609)
 */
// Refer miscjsondef.h

/*
   SIT_START_KEEPALIVE (RCM ID = 0x060A)
 */
namespace ns_ps_start_keepalive_req {
// Int, M
static const Json::StaticString key_keepalive_type("keepalive_type"); // sit_ps_keepalivetype_e_type
// Int[], M
static const Json::StaticString key_source_addr("source_addr");
// Int, M
static const Json::StaticString key_source_port("source_port");
// Int[], M
static const Json::StaticString key_dest_addr("dest_addr");
// Int, M
static const Json::StaticString key_dest_port("dest_port");
// Int, M
static const Json::StaticString key_max_interval("max_interval");
// Int, M
static const Json::StaticString key_cid("cid");
} // ns_ps_start_keepalive_req

namespace ns_ps_start_keepalive_rsp {
// Int, M
static const Json::StaticString key_session_handle("session_handle");
// Int, M
static const Json::StaticString key_status_code("status_code"); // sit_ps_rat_e_type
} // ns_ps_start_keepalive_rsp

/*
   SIT_START_KEEPALIVE (RCM ID = 0x060B)
 */
namespace ns_ps_stop_keepalive_req {
// Int, M
static const Json::StaticString key_keepalive_handle("keepalive_handle");
} // ns_ps_stop_keepalive_req

namespace ns_ps_stop_keepalive_rsp { }

/*
   SIT_IND_KEEPALIVE_STATUS (RCM ID = 0x060C)
 */
namespace ns_ps_keepalive_status_ind {
// Int, M
static const Json::StaticString key_session_handle("session_handle");
// Int, M
static const Json::StaticString key_status_code("status_code"); // sit_ps_rat_e_type
} // ns_ps_keepalive_status_ind

/*
   SIT_IND_PCO_DATA (RCM ID = 0x060D)
 */
namespace ns_ps_pco_data_ind {
// Int, M
static const Json::StaticString key_cid("cid");
// Int, M
static const Json::StaticString key_pdp_type("pdp_type"); // sit_ps_pdp_type_e_type
// Obj[], M
static const Json::StaticString key_operator_pco_info_list("key_operator_pco_info_list");
} // ns_ps_pco_data_ind

namespace ns_ps_operator_pco_info {
// Int, M
static const Json::StaticString key_pco_id("pco_id"); // sit_rat_type_e_type
// Int[], M
static const Json::StaticString key_contents("contents");
} // ns_ps_operator_pco_info

/*
   SIT_SET_IMS_TEST_MODE (RCM ID = 0x0614)
 */
//Refer miscjsondef.j

namespace ns_ps_set_ims_test_mode_rsp {
} // ns_ps_set_ims_test_mode_rsp

/*
   SIT_SET_DATA_PROFILE (RCM ID = 0x0613)
 */
namespace ns_ps_set_data_profile_req {
// UInt, M
static const Json::StaticString key_profile_id("profile_id");
// String, M
static const Json::StaticString key_apn("apn");
// Int, M
static const Json::StaticString key_pdp_type("pdp_type");
// Int, M
static const Json::StaticString key_roaming_pdp_type("roaming_pdp_type");
// Int, M
static const Json::StaticString key_auth_type("auth_type");
// String, M
static const Json::StaticString key_username("username");
// String, M
static const Json::StaticString key_password("password");
// Int, M
static const Json::StaticString key_data_profile_info_type("data_profile_info_type");
// Int, M
static const Json::StaticString key_max_conn_time("max_conn_time");
// Int, M
static const Json::StaticString key_max_conn("max_conn");
// Int, M
static const Json::StaticString key_wait_time("wait_time");
// Int, M
static const Json::StaticString key_enabled("enabled");
// Int, M
static const Json::StaticString key_apn_type("apn_type");
// Int, M
static const Json::StaticString key_radio_access_family("radio_access_family");
} // ns_ps_set_data_profile_req

namespace ns_ps_set_data_profile_rsp {
} // ns_ps_set_data_profile_rsp

/*
   SIT_GET_PHONE_CAPABILITY (RCM ID = 0x0615)
 */
// Refer simjsondef.h

/*
   SIT_IND_PS_DATA_OFF_STATUS (RCM ID = 0x0619)
 */
namespace ns_ps_data_off_status_ind {
// Int, M
static const Json::StaticString key_ps_data_off_status("ps_data_off_status");
// Int, M
static const Json::StaticString key_exempt_home_list("exempt_home_list");
// Int, M
static const Json::StaticString key_exempt_roam_list("exempt_roam_list");
} // ns_ps_data_off_status_ind

/*
   SIT_ALLOC_PDU_SESSION (RCM ID = 0x061A)
 */
namespace ns_ps_alloc_pdu_session_req {
} // ns_ps_alloc_pdu_session_req

namespace ns_ps_alloc_pdu_session_rsp {
// Int, M
static const Json::StaticString key_session_id("session_id");
} // ns_ps_alloc_pdu_session_rsp

/*
   SIT_REL_PDU_SESSION (RCM ID = 0x061B)
 */
namespace ns_ps_rel_pdu_session_req {
// Int, M
static const Json::StaticString key_session_id("session_id");
} // ns_ps_rel_pdu_session_req

namespace ns_ps_rel_pdu_session_rsp {
} // ns_ps_rel_pdu_session_rsp

/*
   SIT_IND_IP_ADDR_STATUS (RCM ID = 0x0617)
 */
namespace ns_ps_ip_addr_status_ind {
// Int, M
static const Json::StaticString key_cid("cid");
// Int[], M
static const Json::StaticString key_ipv4_address("ipv4_address");
// Int[], M
static const Json::StaticString key_ipv6_address("ipv6_address");
// Int, M
static const Json::StaticString key_status("status");
} // ns_ps_ip_addr_status_ind

/*
   SIT_IND_UNTHROTTLE_APN (RCM ID = 0x061E)
 */
namespace ns_ps_unthrottle_apn_ind {
// String, M
static const Json::StaticString key_apn("apn");
} // ns_ps_unthrottle_apn_ind

/*
   SIT_IND_DEDICATED_BEARER_INFO (RCM ID = 0x0621)
 */
namespace ns_ps_dedicated_bearer_info_ind {
// Int, M
static const Json::StaticString key_default_cid("default_cid");
// Int, M
static const Json::StaticString key_status("status"); // sit_ps_status_e_type
// Int, M
static const Json::StaticString key_qos_session_id("qos_session_id");
// Int, M
static const Json::StaticString key_qos_rat_type("qos_rat_type"); //// sit_ps_rat_type_e_type
// Obj, M
static const Json::StaticString key_qos("qos"); // ns_ps_nr_qos_information or ns_ps_lte_qos_information
// Obj[], M
static const Json::StaticString key_packet_filter_list("packet_filter_list"); // ns_ps_packet_filter_information
} // ns_ps_dedocated_bearer_info_ind

namespace ns_ps_lte_qos_information {
// Int, M
static const Json::StaticString key_qci("qci");
// UInt, M
static const Json::StaticString key_dl_max_bit_rate("dl_max_bit_rate");
// UInt, M
static const Json::StaticString key_dl_guaranteed_bit_rate("dl_guaranteed_bit_rate");
// UInt, M
static const Json::StaticString key_ul_max_bit_rate("ul_max_bit_rate");
// UInt, M
static const Json::StaticString key_ul_guaranteed_bit_rate("ul_guaranteed_bit_rate");
} // ns_ps_lte_qos_information

namespace ns_ps_nr_qos_information {
// Int, M
static const Json::StaticString key_five_qi("five_qi");
// UInt, M
static const Json::StaticString key_dl_max_bit_rate("dl_max_bit_rate");
// UInt, M
static const Json::StaticString key_dl_guaranteed_bit_rate("dl_guaranteed_bit_rate");
// UInt, M
static const Json::StaticString key_ul_max_bit_rate("ul_max_bit_rate");
// UInt, M
static const Json::StaticString key_ul_guaranteed_bit_rate("ul_guaranteed_bit_rate");
// Int, M
static const Json::StaticString key_qfi("qfi");
// Int, M
static const Json::StaticString key_average_window("average_window");
} // ns_ps_nr_qos_information

namespace ns_ps_packet_filter_information {
// Int, M
static const Json::StaticString key_field_flag("field_flag");
// Int, M
static const Json::StaticString key_local_address("local_address");
// Int, M
static const Json::StaticString key_local_address_prefix_ipv4("local_address_prefix_ipv4");
// Int, M
static const Json::StaticString key_local_address_prefix_ipv6("local_address_prefix_ipv6");
// Int, M
static const Json::StaticString key_remote_address("remote_address");
// Int, M
static const Json::StaticString key_remote_address_prefix_ipv4("remote_address_prefix_ipv4");
// Int, M
static const Json::StaticString key_remote_address_prefix_ipv6("remote_address_prefix_ipv6");
// Int, M
static const Json::StaticString key_local_port_low("local_port_low");
// Int, M
static const Json::StaticString key_local_port_high("local_port_high");
// Int, M
static const Json::StaticString key_remote_prot_low("remote_prot_low");
// Int, M
static const Json::StaticString key_remote_port_high("remote_port_high");
// Int, M
static const Json::StaticString key_qos_protocol("qos_protocol");
// Int, M
static const Json::StaticString key_type_of_service("type_of_service");
// Int, M
static const Json::StaticString key_ipv6_flow_label("ipv6_flow_label");
// Int, M
static const Json::StaticString key_ip_sec_spi("ip_sec_spi");
// Int, M
static const Json::StaticString key_direction("direction");
// Int, M
static const Json::StaticString key_precedence("precedence");
} // ns_ps_packet_filter_information

/*
   SIT_GET_SLICING_CONFIG (RCM ID = 0x0622)
 */
namespace ns_ps_get_slicing_config_req {
// Int, M
static const Json::StaticString key_ipc_version("ipc_version");
} // ns_ps_get_slicing_config_req

namespace ns_ps_get_slicing_config_rsp {
// Int, M
static const Json::StaticString key_ipc_version("ipc_version");
// Obj[], M
static const Json::StaticString key_ursp_rule_list("ursp_rule_list"); // ns_ps_ursp_rule_info
// Obj[], M
static const Json::StaticString key_slice_info_list("slice_info_list"); // ns_ps_slice_info
} // ns_ps_get_slicing_config_rsp

namespace ns_ps_ursp_rule_info {
// Int, M
static const Json::StaticString key_precedence("precedence");
// Obj, M
static const Json::StaticString key_traffic_descriptor("traffic_descriptor"); // ns_ps_traffic_descriptor
// Obj[], M
static const Json::StaticString key_route_selection_descriptor_list("route_selection_descriptor_list"); //ns_ps_route_selection_descriptor_info
} // ns_ps_ursp_rule_info

namespace ns_ps_traffic_descriptor {
// String, M
static const Json::StaticString key_dnn("dnn");
// Int[], M
static const Json::StaticString key_os_app_id("os_app_id");
// Int[], M
static const Json::StaticString key_os_id("os_id");
} // ns_ps_traffic_descriptor

namespace ns_ps_route_selection_descriptor_info {
// String, M
static const Json::StaticString key_precedence("precedence");
// Int[], M
static const Json::StaticString key_session_type("session_type");
// Int[], M
static const Json::StaticString key_ssc_mode("ssc_mode"); // sit_ps_ssc_mode_type_e_type
// Obj[], M
static const Json::StaticString key_slice_info_list("slice_info_list"); // ns_ps_slice_info
// Obj[], M
static const Json::StaticString key_dnn_list("dnn_list"); // ns_ps_dnn
} // ns_ps_route_selection_descriptor_info

namespace ns_ps_slice_info {
// Int, M
static const Json::StaticString key_nssai_type("nssai_type");
// Int, M
static const Json::StaticString key_slice_service_type("slice_service_type");  // sit_ps_slice_service_type_e_type
// Int, M
static const Json::StaticString key_slice_differentiator("slice_differentiator");
// Int, M
static const Json::StaticString key_mapped_hplmn_sst("mapped_hplmn_sst");  // sit_ps_slice_service_type_e_type
// Int, M
static const Json::StaticString key_mapped_hplmn_sd("mapped_hplmn_sd");
// Int, M
static const Json::StaticString key_status("status");  // sit_ps_slice_status_e_type
} // ns_ps_slice_info

namespace ns_ps_dnn {
// String, M
static const Json::StaticString key_dnn("dnn");
} // ns_ps_dnn

/******************************************************************************
 * net
 *****************************************************************************/
/*
   SIT_SET_PREFERRED_DATA_MODEM (RCM ID = 0x0740)
 */
namespace ns_ps_net_set_preferred_data_modem_req {
// Int, M
static const Json::StaticString key_stack_id("stack_id");
} // ns_ps_net_set_preferred_data_modem_req

#endif /* _PS_JSON_DEF_H_ */
