/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _OEM_JSON_DEF_H_
#define _OEM_JSON_DEF_H_

#include "commondef.h"
#include "json/json.h"

/******************************************************************************
 *
 * File: oemjsondef.h
 *
 * Description: This file will have key-value pair definition of all OEM SIT commands
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
  SIT_OEM_NW_INFO   (RCM ID = 0x4400)
*/
namespace ns_oem_nw_info_req {
// Int M
static const Json::StaticString key_command_id("command_id");

// String O
static const Json::StaticString key_buffer("buffer");
} // namespace ns_oem_nw_info_req

namespace ns_oem_nw_info_rsp {
// Int M
static const Json::StaticString key_command_id("command_id");

// String O
static const Json::StaticString key_buffer("buffer");
}

/*
  SIT_OEM_IND_NW_INFO (RCM ID = 0x4401)
*/
namespace ns_oem_nw_info_ind {
// Int M
static const Json::StaticString key_command_id("command_id");

// String O
static const Json::StaticString key_buffer("buffer");
}

/*
  SIT_OEM_SET_ACTIVATE_VSIM (RCM ID = 0x4402)
*/
namespace ns_oem_set_activate_factory_vsim_req {
// Int M
static const Json::StaticString key_sim_slot("sim_slot");

// Int[] M
static const Json::StaticString key_iccid("iccid");

// Int[] M
static const Json::StaticString key_imsi("imsi");

// String M
static const Json::StaticString key_hplmn("hplmn");

// Int M
static const Json::StaticString key_vsim_state("vsim_state");

// Int M
static const Json::StaticString key_vsim_card_type("vsim_card_type");

}

namespace ns_oem_set_activate_factory_vsim_rsp {
// Int M
static const Json::StaticString key_result("result");
}

/*
  SIT_OEM_SET_FORBID_LTE_CELL (RCM ID = 0x4403)
*/
namespace ns_oem_set_forbid_lte_req {
// Int M
static const Json::StaticString key_mode("mode");

// Int M
static const Json::StaticString key_cell_id("cell_id");

// Int M
static const Json::StaticString key_forbidden_timer("forbidden_timer");

// String M
static const Json::StaticString key_plmn("plmn");

} // namespace ns_oem_set_forbid_lte_req

namespace ns_oem_set_forbid_lte_rsp {
// Int M
static const Json::StaticString key_result("result");
}

/*
  SIT_OEM_SET_CP_SLEEP_BLOCK_REQ (RCM ID = 0x4404)
*/
namespace ns_oem_set_cp_sleep_block_req {
// Int M
static const Json::StaticString key_sleep_block_req("sleep_block_req");

} // namespace ns_oem_set_forbid_lte_req

namespace ns_oem_set_cp_sleep_block_rsp {
}

/*
  SIT_OEM_SET_FUNC_SWITCH_REQ  (RCM ID = 0x4405)
*/
namespace ns_oem_set_fun_swtich_req {
// Int M
static const Json::StaticString key_feature("feature");

// Int M
static const Json::StaticString key_enable("enable");
} // namespace ns_oem_set_fun_swtich_req

namespace ns_oem_set_fun_swtich_rsp {
// Int M
static const Json::StaticString key_result("result");
}

/*
  SIT_OEM_SET_FORBID_NR_CELL (RCM ID = 0x4406)
*/
namespace ns_oem_set_forbid_nr_req {
// Int M
static const Json::StaticString key_mode("mode");

// Int M
static const Json::StaticString key_cell_id("cell_id");

// Int M
static const Json::StaticString key_forbidden_timer("forbidden_timer");

// String M
static const Json::StaticString key_plmn("plmn");

} // namespace ns_oem_set_forbid_lte_req

namespace ns_oem_set_forbid_nr_rsp {
// Int M
static const Json::StaticString key_result("result");
}

/*
  SIT_OEM_SET_PDCP_DISCARD_TIMER (RCM ID = 0x4408)
*/
namespace ns_oem_set_pdcp_discard_timer_req {
// Int M
static const Json::StaticString key_discard_timer("discard_timer");

} // namespace ns_oem_set_pdcp_discard_timer_req

namespace ns_oem_set_pdcp_discard_timer_rsp {
}

/*
  SIT_OEM_GET_CQI_INFO  (RCM ID = 0x4409)
*/
namespace ns_oem_get_cqi_info_req {

} // namespace ns_oem_get_cqi_info_req

namespace ns_oem_get_cqi_info_rsp {
// Int M
static const Json::StaticString key_type("type");

// Int M
static const Json::StaticString key_cqi_info0("cqi_info0");

// Int M //TODO document error
static const Json::StaticString key_cqi_info1("cqi_info1");

// Int M
static const Json::StaticString key_ri("ri");
}

/*
  SIT_OEM_SET_SAR_SETTING (RCM ID = 0x440A)
*/
namespace ns_oem_set_sar_setting_req {
// Int M
static const Json::StaticString key_dsi("dsi");

} // namespace ns_oem_set_sar_setting_req

namespace ns_oem_set_sar_setting_rsp {
}

/*
  SIT_OEM_SET_GMO_SWITCH (RCM ID = 0x440B)
*/
namespace ns_oem_set_gmo_switch_req {
// Int M
static const Json::StaticString key_feature("feature");

} // namespace ns_oem_set_gmo_switch_req

namespace ns_oem_set_gmo_switch_rsp {
}

/*
  SIT_OEM_SET_TCS_FCI_REQ (RCM ID = 0x440C)
*/
namespace ns_oem_set_tcs_fci_req {
// Int M
static const Json::StaticString key_state("state");

// String M
static const Json::StaticString key_fci("fci");

} // namespace ns_oem_set_tcs_fci_req

namespace ns_oem_set_tcs_fci_rsp {
// Int M
static const Json::StaticString key_result("result");
}

/*
  SIT_OEM_GET_TCS_FCI_INFO  (RCM ID = 0x440D)
*/
namespace ns_oem_get_tcs_fci_req {

} // namespace ns_oem_get_tcs_fci_req

namespace ns_oem_get_tcs_fci_rsp {
// String M
static const Json::StaticString key_fci("fci");
}

/*
  SIT_OEM_IND_ENDC_CAPABILITY (RCM ID = 0x440E)
*/
namespace ns_oem_endc_capability_ind {
// Int M
static const Json::StaticString key_endc_capa("endc_capa");

// Int M
static const Json::StaticString key_cause("cause");
} // namespace ns_oem_endc_capability_ind

/*
  SIT_OEM_GET_ACTIVITY_INFO (RCM ID = 0x440F)
*/
namespace ns_oem_get_activity_info_req {
} // namespace ns_oem_get_activity_infor_req

namespace ns_oem_get_activity_info_rsp {
// Int M
static const Json::StaticString key_handover_count("handover_count");

// Int M
static const Json::StaticString key_reselection_count("reselection_count");

// Int M
static const Json::StaticString key_cdrx_config_time("cdrx_config_time");

// Int M
static const Json::StaticString key_rrc_connected_time("rrc_connected_time");

// Obj[] M
static const Json::StaticString key_rx_info_list("rx_info_list"); //ns_oem_activity_rx_info

// Obj[] M
static const Json::StaticString key_base_station_info_list("base_station_info_list"); //ns_oem_activity_base_station_info
}

namespace ns_oem_activity_rx_info {
// Int M
static const Json::StaticString key_rx("rx");

// Int M
static const Json::StaticString key_count("count");
} // namespace ns_oem_activity_rx_info

namespace ns_oem_activity_base_station_info {
// String M
static const Json::StaticString key_mcc("mcc");

// String M
static const Json::StaticString key_mnc("mnc");

// Int M
static const Json::StaticString key_rat("rat");

// Int M
static const Json::StaticString key_is_nsa("is_nsa");

// Int M
static const Json::StaticString key_band("band");

// Int M
static const Json::StaticString key_signal_level("signal_level");

// Int M
static const Json::StaticString key_count("count");

} // namespace ns_oem_activity_base_station_info

/*
  SIT_OEM_IND_BASESTATION_INFO (RCM ID = 0x4410)
*/
namespace ns_oem_base_station_info_ind {
// Obj[] M
static const Json::StaticString key_base_station_info_list("base_station_info_list"); //ns_oem_activity_base_station_info
} // namespace ns_oem_base_station_info_ind

/*
  SIT_OEM_IND_ADDITIONAL_CELL_INFO (RCM ID = 0x4411)
*/
namespace ns_oem_additinal_cell_info_ind {
// Obj[] M
static const Json::StaticString key_frequency_list("frequency_list"); //ns_oem_frequency_info

// Int M
static const Json::StaticString key_t_reselection("t_reselection");
} // namespace ns_oem_additinal_cell_info_ind

namespace ns_oem_frequency_info {
// Int M
static const Json::StaticString key_carrier_freq("carrier_freq");

// Int M
static const Json::StaticString key_sub_carrier_spacing_ssb("sub_carrier_spacing_ssb");

// Int M
static const Json::StaticString key_cell_reselection_priority("cell_reselection_priority");

// Int M
static const Json::StaticString key_cell_reselection_sub_priority("cell_reselection_sub_priority");

// Int M
static const Json::StaticString key_thresh_x_high("thresh_x_high");

// Int M
static const Json::StaticString key_thresh_x_low("thresh_x_low");

// Int M
static const Json::StaticString key_p_max_nr("p_max_nr");

// Int M
static const Json::StaticString key_derive_ssb_index_from_cell("derive_ssb_index_from_cell");
} // namespace ns_oem_frequency_info

/*
  SIT_OEM_SET_SVN (RCM ID = 0x4412)
*/
namespace ns_oem_set_svn_req {
// String M
static const Json::StaticString key_svn("svn");

} // namespace ns_oem_set_svn_req

namespace ns_oem_set_svn_rsp {
}

/*
  SIT_OEM_SET_FORBIDDEN_FREQUENCY (RCM ID = 0x4413)
*/
namespace ns_oem_set_forbidden_freq_req {
// Int M
static const Json::StaticString key_mode("mode");

// Int M
static const Json::StaticString key_frequency("frequency");

// Int M
static const Json::StaticString key_forbidden_timer("forbidden_timer");
} // namespace ns_oem_set_forbidden_freq_req

namespace ns_oem_set_forbidden_freq_rsp {
// Int M
static const Json::StaticString key_result("result");
}

/*
  SIT_OEM_SET_SELFLOG_TCP_DUMP_MODE (RCM ID = 0x4416)
*/
namespace ns_oem_set_selflog_tcpdump_mode_req {
// Int M
static const Json::StaticString key_tcp_dump_enable("tcp_dump_enable");

// Int M
static const Json::StaticString key_header_dump_enable("header_dump_enable");
} // namespace ns_oem_set_selflog_tcpdump_mode_req

namespace ns_oem_set_selflog_tcpdump_mode_rsp {
}

/*
  SIT_OEM_IND_DROPPED_MT_CALL_INFO (RCM ID = 0x4417)
*/
namespace ns_oem_dropped_mt_call_info_ind {
// Int M
static const Json::StaticString key_call_id("call_id");

// Int M
static const Json::StaticString key_call_end_before_ring_reason("call_end_before_ring_reason");

// Int M
static const Json::StaticString key_call_domain("call_domain");

// Int M
static const Json::StaticString key_call_type("call_type");

// Int M
static const Json::StaticString key_call_error_code("call_error_code");

// String M
static const Json::StaticString key_call_error_reason("call_error_reason");

// Int M
static const Json::StaticString key_call_drop_time("call_drop_time");

// String M //TODO document error?
static const Json::StaticString key_calling_party_number("calling_party_number");

// Int M
static const Json::StaticString key_active_rat("active_rat");

// Int M
static const Json::StaticString key_plmn_id("plmn_id");

// Int M
static const Json::StaticString key_cell_id("cell_id");

// Int M
static const Json::StaticString key_tac("tac");

// Int M
static const Json::StaticString key_pci("pci");

// Int M
static const Json::StaticString key_arfcn("arfcn");

// Int M
static const Json::StaticString key_band("band");

} // namespace ns_oem_dropped_mt_call_info_ind

/*
  SIT_OEM_SET_BSR_PARA_REQ (RCM ID = 0x4418)
*/
namespace ns_oem_set_bsr_param_req {
// Int M
static const Json::StaticString key_switch_status("switch_status");

// Int M
static const Json::StaticString key_packet_period("packet_period");

// Int M
static const Json::StaticString key_packet_size("packet_size");

} // namespace ns_oem_set_bsr_param_req

namespace ns_oem_set_bsr_param_rsp {
}

/*
  SIT_OEM_SET_DRDV (RCM ID = 0x4419)
*/
namespace ns_oem_set_drdv_req {
// Int M
static const Json::StaticString key_drdv_mask("drdv_mask");

} // namespace ns_oem_set_drdv_req

namespace ns_oem_set_drdv_rsp {
}

/*
  SIT_OEM_SET_PREFER_RESEL (RCM ID = 0x441A)
*/
namespace ns_oem_set_prefer_resel_req {
// Int M
static const Json::StaticString key_ran("ran");

// Int[] M
static const Json::StaticString key_earfcn_list("earfcn_list");
} // namespace ns_oem_set_prefer_resel_req

namespace ns_oem_set_prefer_resel_rsp {
}

/*
  SIT_OEM_SET_AHEAD_RRE (RCM ID = 0x441B)
*/
namespace ns_oem_set_ahead_rre_req {
// Int M
static const Json::StaticString key_search_type("search_type");

// Int M
static const Json::StaticString key_earfcn("erarfcn");

// String M //TODO document error ?.
static const Json::StaticString key_global_cell_id("global_cell_id");

} // namespace ns_oem_set_ahead_rre_req

namespace ns_oem_set_ahead_rre_rsp {
// Int M
static const Json::StaticString key_result("result");
}

/*
  SIT_OEM_SET_ALL_4RX_DIV_SETTING_DEFAULT (RCM ID = 0x441C)
*/
namespace ns_oem_set_all_4rx_div_setting_default_req {

} // namespace ns_oem_set_all_4rx_div_setting_default_req

namespace ns_oem_set_all_4rx_div_setting_default_rsp {
}

/*
  SIT_OEM_GET_CDMA_CAPABILITY_INFO (RCM ID = 0x441D)
*/
namespace ns_oem_get_cdma_capability_info_req {

} // namespace ns_oem_get_cdma_capability_info_req

namespace ns_oem_get_cdma_capability_info_rsp {
// Int M
static const Json::StaticString key_result("result");
}

/*
  SIT_OEM_SET_SRS_PWR_LIMIT (RCM ID = 0x441F)
*/
namespace ns_oem_set_srs_pwr_limit_req {
// Int, M
static const Json::StaticString key_level("level");
}

namespace ns_oem_set_srs_pwr_limit_rsp {

}

/*
  SIT_OEM_SET_TRACE_DUMP_CFG (RCM ID = 0x4422)
*/
namespace ns_oem_set_trace_dump_cfg_req {
// Int, M
static const Json::StaticString key_config("config");
}

namespace ns_oem_set_trace_dump_cfg_rsp {

}

/*
  SIT_OEM_GET_TRACE_DUMP_CFG (RCM ID = 0x4423)
*/
namespace ns_oem_get_trace_dump_cfg_req {

}

namespace ns_oem_get_trace_dump_cfg_rsp {
// Int M
static const Json::StaticString key_config("config");
}



#endif /* _OEM_JSON_DEF_H_ */

