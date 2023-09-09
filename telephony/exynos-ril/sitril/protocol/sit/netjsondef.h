/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _NET_JSON_DEF_H_
#define _NET_JSON_DEF_H_

#include "json/json.h"

/******************************************************************************
 *
 * File: netjsondef.h
 *
 * Description: This file will have key-value pair definition of all NET SIT commands
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
 * Netowork
 *****************************************************************************/

/*
    SIT_GET_CS_REG_STATE (RCM ID = 0x0700)
*/
namespace ns_net_get_cs_reg_rsp {
// Int, M
static const Json::StaticString key_reg_status("reg_status"); // sit_net_reg_state_e_type
// Int, M
static const Json::StaticString key_reg_cause("reg_cause");
// Int, M
static const Json::StaticString key_rat("rat"); // sit_rat_type_e_type
// Obj, C
static const Json::StaticString key_2g_3g_cs_reg_info("2g_3g_cs_reg_info"); // ns_net_2g_3g_reg_info
// Obj, C
static const Json::StaticString key_lte_cs_reg_info("lte_cs_reg_info"); // ns_net_4g_reg_info
// Obj, C
static const Json::StaticString key_nr_cs_reg_info("nr_cs_reg_info"); // ns_net_5g_reg_info
// Obj, C
static const Json::StaticString key_cdma_cs_reg_info("cdma_cs_reg_info"); // ns_net_cdma_reg_info
}

/*
    SIT_GET_PS_REG_STATE (RCM ID = 0x0701)
*/
namespace ns_net_get_ps_reg_rsp {
// Int, M
static const Json::StaticString key_reg_status("reg_status");    // sit_net_reg_state_e_type
// Int, M
static const Json::StaticString key_rej_cause("reg_cause");
// Int, M
static const Json::StaticString key_rat("rat");
// Obj, C
static const Json::StaticString key_2g_3g_ps_reg_info("2g_3g_ps_reg_info"); // ns_net_2g_3g_reg_info
// Obj, C
static const Json::StaticString key_lte_ps_reg_info("lte_ps_reg_info"); // ns_net_4g_reg_info
// Obj, C
static const Json::StaticString key_nr_ps_reg_info("nr_ps_reg_info"); // ns_net_5g_reg_info
// Int, M
static const Json::StaticString key_volte_svc("volte_svc");
// Int, M
static const Json::StaticString key_emc_svc("emc_svc");
}

namespace ns_net_2g_3g_reg_info {
// UInt, M
static const Json::StaticString key_2g_3g_lac("lac");
// UInt, M
static const Json::StaticString key_2g_3g_cid("cid");
// Int, M
static const Json::StaticString key_2g_3g_psc("psc");
// UInt, M
static const Json::StaticString key_2g_3g_channel("channel");
}

namespace ns_net_4g_reg_info  {
// UInt, M
static const Json::StaticString key_4g_tac("tac");
// UInt, M
static const Json::StaticString key_4g_pcid("pcid");
// UInt, M
static const Json::StaticString key_4g_eci("eci");
// UInt, M
static const Json::StaticString key_4g_csgid("csgid");
// UInt, M
static const Json::StaticString key_4g_tadv("tadv");
// UInt, M
static const Json::StaticString key_4g_channel("channel");
// Int, M (only PS)
static const Json::StaticString key_4g_endc("endc");
// Int, M (only PS)
static const Json::StaticString key_4g_dcnr_restricted("dcnr_restricted");
// Int, M (only PS)
static const Json::StaticString key_4g_nr_available("nr_available");
}

namespace ns_net_5g_reg_info  {
// ULong, M
static const Json::StaticString key_5g_cid("cid");
// Int, M
static const Json::StaticString key_5g_pcid("pcid");
// UInt, M
static const Json::StaticString key_5g_tac("tac");
// Int, M
static const Json::StaticString key_5g_channel("channel");
// Int M (only PS)
static const Json::StaticString key_5g_emf("emf");
}

namespace ns_net_cdma_reg_info {
// UInt, M
static const Json::StaticString key_cdma_base_sta_id("base_sta_id");
// Int, M
static const Json::StaticString key_cdma_base_sta_lt("base_sta_lt");
// Int, M
static const Json::StaticString key_cdma_base_sta_lng("base_sta_lng");
// Int, M
static const Json::StaticString key_cdma_concurrent("concurrent");
// Int, M
static const Json::StaticString key_cdma_sid("sid");
// Int, M
static const Json::StaticString key_cdma_nid("nid");
// Int, M
static const Json::StaticString key_cdma_roaming_id("roaming_id");
// Int, M
static const Json::StaticString key_cdma_registered_prl("registered_prl");
// Int, M
static const Json::StaticString key_cdma_roaming_ind_prl("roaming_ind_prl");
}

/*
    SIT_GET_OPERATOR (RCM ID = 0x0702)
*/
namespace ns_net_get_operator_rsp {
// String, M
static const Json::StaticString key_plmn("plmn");
// String, O
static const Json::StaticString key_short_name("short_name");
// String, O
static const Json::StaticString key_long_name("long_name");
// Int, M
static const Json::StaticString key_reg_status("reg_status");    // OPERATOR_REG_NOT(=0), OPERATOR_REG_HOME(=1), OPERATOR_REG_ROAM(=2)
// UInt, M
static const Json::StaticString key_location("location");
}

/*
    SIT_GET_NTW_MODE (RCM ID = 0x0703)
*/
namespace ns_net_get_ntw_mode_rsp {
// Int, M
static const Json::StaticString key_ntw_mode("ntw_mode");    // sit_net_network_mode_e_type
}

/*
    SIT_SET_NTW_MODE_AUTO (RCM ID = 0x0704)
*/

/*
    SIT_SET_NTW_MODE_MANUAL (RCM ID = 0x0705)
*/
namespace ns_net_set_ntw_mode_manual_req {
// Int, M
static const Json::StaticString key_rat("rat");        // sit_rat_type_e_type
// String, M
static const Json::StaticString key_plmn("plmn");
}

/*
    SIT_GET_AVAILABLE_NETWORKS (RCM ID = 0x0706)
*/
namespace ns_net_get_available_networks_req {
// Int, M
static const Json::StaticString key_ran("ran");        // sit_net_ran_e_type
// Int, M
static const Json::StaticString key_band("band");      // 0(all bands), 1(b1, b3, b5 for EUTRAN)
}

namespace ns_net_get_available_networks_rsp {
// Obj[], M
static const Json::StaticString key_network_list("network_list");
}

namespace ns_net_network_info {
// Int, M
static const Json::StaticString key_rat("rat");                // sit_rat_type_e_type
// String, M
static const Json::StaticString key_plmn("plmn");
// Int, M
static const Json::StaticString key_plmn_status("plmn_status"); // sit_net_plmn_status_e_type
}

/*
    SIT_CANCEL_GET_AVAILABLE_NETWORKS (RCM ID = 0x0707)
*/

/*
    SIT_SET_BAND_MODE (RCM ID = 0x0708)
*/
namespace ns_net_set_band_mode_req {
// Int, M
static const Json::StaticString key_band("band");        // sit_net_band_e_type
}

/*
    SIT_GET_BAND_MODE (RCM ID = 0x0709)
*/
namespace ns_net_set_band_mode_rsp {
// Int[], M
static const Json::StaticString key_band_mode_list("band_mode_list");        // array of sit_net_band_e_type
}

/*
    SIT_SET_PREFERRED_NTW_TYPE (RCM ID = 0x070A)
*/
namespace ns_net_getset_preferred_ntw_type {
// Int, M
static const Json::StaticString key_pref_net_type("pref_net_type");    // sit_net_pref_network_type_e_type
}

/*
    SIT_GET_PREFERRED_NTW_TYPE (RCM ID = 0x070B)
*/
// same to ns_net_getset_preferred_ntw_type

/*
    SIT_GET_CELL_INFO_LIST (RCM ID = 0x070C)
*/
namespace ns_net_cell_info_list {
// Obj[], M
static const Json::StaticString key_cell_info_list("cell_info_list");
}

namespace ns_net_cell_info {
// Int, M
static const Json::StaticString key_cell_info_type("cell_info_type");    // sit_net_cell_info_type_e_type
// Obj, C
static const Json::StaticString key_cell_info_gsm("cell_info_gsm"); // ns_net_cell_info_gsm
// Obj, C
static const Json::StaticString key_cell_info_cdma("cell_info_cdma"); // ns_net_cell_info_cdma
// Obj, C
static const Json::StaticString key_cell_info_wcdma("cell_info_wcdma"); // ns_net_cell_info_wcdma
// Obj, C
static const Json::StaticString key_cell_info_lte("cell_info_lte"); // ns_net_cell_info_lte
// Obj, C
static const Json::StaticString key_cell_info_tdscdma("cell_info_tdscdma"); // ns_net_cell_info_tdscdma
// Obj, C
static const Json::StaticString key_cell_info_nr("cell_info_nr"); // ns_net_cell_info_nr
}

namespace ns_net_cell_info_gsm {
// Obj, M
static const Json::StaticString key_cell_info_common("cell_info_common"); // ns_net_cell_info_common
// Obj, M
static const Json::StaticString key_cell_identity_gsm("cell_identity_gsm"); // ns_net_cell_identity_gsm
// Obj, M
static const Json::StaticString key_sig_str_gsm("sig_str_gsm"); // ns_net_cell_str_gsm
}

namespace ns_net_cell_identity_gsm {
// String, M
static const Json::StaticString key_2g_plmn("plmn");
// Int, M
static const Json::StaticString key_2g_lac("lac");
// Int, M
static const Json::StaticString key_2g_cid("cid");
// UInt, M
static const Json::StaticString key_2g_arfcn("arfcn");
// Int, M
static const Json::StaticString key_2g_bsic("bsic");
// Obj, M
static const Json::StaticString key_2g_cell_info_plmn("cell_info_plmn"); // ns_net_cell_info_plmn
}

namespace ns_net_cell_str_gsm {
// Int, M
static const Json::StaticString key_2g_sig_stren("sig_stren");
// Int, M
static const Json::StaticString key_2g_ber("ber");
// UInt, M
static const Json::StaticString key_2g_ta("ta");
}

namespace ns_net_cell_info_cdma {
// Obj, M
static const Json::StaticString key_cell_info_common("cell_info_common"); // ns_net_cell_info_common
// Obj, M
static const Json::StaticString key_cell_identity_cdma("cell_identity_cdma"); // ns_net_cell_identity_cdma
// Obj, M
static const Json::StaticString key_sig_str_cdma("sig_str_cdma"); // ns_net_cell_str_cdma
}

namespace ns_net_cell_identity_cdma {
// Int, M
static const Json::StaticString key_cdma_nid("nid");
// Int, M
static const Json::StaticString key_cdma_sid("sid");
// Int, M
static const Json::StaticString key_cdma_bsid("bsid");
// UInt, M
static const Json::StaticString key_cdma_longitude("longitude");
// UInt, M
static const Json::StaticString key_cdma_latitude("latitude");
}

namespace ns_net_cell_str_cdma {
// UInt, M
static const Json::StaticString key_cdma_dbm("cdma_dbm");
// UInt, M
static const Json::StaticString key_cdma_ecio("cdma_ecio");
// UInt, M
static const Json::StaticString key_cdma_dodbm("dodbm");
// UInt, M
static const Json::StaticString key_cdma_doecio("doecio");
// Int, M
static const Json::StaticString key_cdma_dosnr("dosnr");
}

namespace ns_net_cell_info_wcdma {
// Obj, M
static const Json::StaticString key_cell_info_common("cell_info_common"); // ns_net_cell_info_common
// Obj, M
static const Json::StaticString key_cell_identity_wcdma("cell_identity_wcdma"); // ns_net_cell_identity_wcdma
// Obj, M
static const Json::StaticString key_sig_str_wcdma("sig_str_wcdma"); // ns_net_cell_str_wcdma
}

namespace ns_net_cell_identity_wcdma {
// String, M
static const Json::StaticString key_3g_plmn("plmn");
// UInt, M
static const Json::StaticString key_3g_lac("lac");
// UInt, M
static const Json::StaticString key_3g_cid("cid");
// UInt, M
static const Json::StaticString key_3g_psc("psc");
// UInt, M
static const Json::StaticString key_3g_uarfcn("uarfcn");
// Obj, M
static const Json::StaticString key_3g_cell_info_plmn("cell_info_plmn"); // ns_net_cell_info_plmn
// Obj, M
static const Json::StaticString key_3g_cell_info_csg("cell_info_csg"); // ns_net_cell_info_csg
}

namespace ns_net_cell_str_wcdma {
// Int, M
static const Json::StaticString key_3g_sig_stren("sig_stren");
// Int, M
static const Json::StaticString key_3g_ber("ber");
// Int, M
static const Json::StaticString key_3g_rscp("rscp");
// Int, M
static const Json::StaticString key_3g_ecno("ecno");
}

namespace ns_net_cell_info_lte {
// Obj, M
static const Json::StaticString key_cell_info_common("cell_info_common"); // ns_net_cell_info_common
// Obj, M
static const Json::StaticString key_cell_identity_lte("cell_identity_lte"); // ns_net_cell_identity_lte
// Int, M
static const Json::StaticString key_endc_available("endc_available");
// Obj, M
static const Json::StaticString key_sig_str_lte("sig_str_lte"); // ns_net_cell_str_lte
}

namespace ns_net_cell_identity_lte {
// String, M
static const Json::StaticString key_4g_plmn("plmn");
// UInt, M
static const Json::StaticString key_4g_cid("cid");
// UInt, M
static const Json::StaticString key_4g_pcid("pcid");
// UInt, M
static const Json::StaticString key_4g_tac("tac");
// UInt, M
static const Json::StaticString key_4g_earfcn("earfcn");
// UInt, M
static const Json::StaticString key_4g_bandwidth("bandwidth");
// Obj, M
static const Json::StaticString key_4g_cell_info_plmn("cell_info_plmn"); // ns_net_cell_info_plmn
// Obj, M
static const Json::StaticString key_4g_cell_info_csg("cell_info_csg"); // ns_net_cell_info_csg
// Int[], M
static const Json::StaticString key_4g_eutran_band_list("eutran_band_list");
}

namespace ns_net_cell_str_lte {
// Int, M
static const Json::StaticString key_4g_sig_stren("sig_stren");
// Int, M
static const Json::StaticString key_4g_rsrp("rsrp");
// Int, M
static const Json::StaticString key_4g_rsrq("rsrq");
// Int, M
static const Json::StaticString key_4g_rssnr("rssnr");
// Int, M
static const Json::StaticString key_4g_cqi_table_ind("cqi_table_ind");
// Int, M
static const Json::StaticString key_4g_cqi("cqi");
// Int, M
static const Json::StaticString key_4g_ta("ta");
}

namespace ns_net_cell_info_tdscdma {
// Obj, M
static const Json::StaticString key_cell_info_common("cell_info_common"); // ns_net_cell_info_common
// Obj, M
static const Json::StaticString key_cell_identity_tdscdma("cell_identity_tdscdma"); // ns_net_cell_identity_tdscdma
// Obj, M
static const Json::StaticString key_sig_str_tdscdma("sig_str_tdscdma"); // ns_net_cell_str_tdscdma
}

namespace ns_net_cell_identity_tdscdma {
// String, M
static const Json::StaticString key_tdscdma_plmn("plmn");
// UInt, M
static const Json::StaticString key_tdscdma_lac("lac");
// UInt, M
static const Json::StaticString key_tdscdma_cid("cid");
// UInt, M
static const Json::StaticString key_tdscdma_cpid("cpid");
// UInt, M
static const Json::StaticString key_tdscdma_uarfcn("uarfcn");
// Obj, M
static const Json::StaticString key_tdscdma_cell_info_plmn("cell_info_plmn"); // ns_net_cell_info_plmn
// Obj, M
static const Json::StaticString key_tdscdma_cell_info_csg("cell_info_csg"); // ns_net_cell_info_csg
}

namespace ns_net_cell_str_tdscdma {
// Int, M
static const Json::StaticString key_tdscdma_sig_stren("sig_stren");
// Int, M
static const Json::StaticString key_tdscdma_ber("ber");
// Int, M
static const Json::StaticString key_tdscdma_rscp("rscp");
}

namespace ns_net_cell_info_nr {
// Obj, M
static const Json::StaticString key_cell_info_common("cell_info_common"); // ns_net_cell_info_common
// Obj, M
static const Json::StaticString key_cell_identity_nr("cell_identity_nr"); // ns_net_cell_identity_nr
// Obj, M
static const Json::StaticString key_sig_str_nr("sig_str_nr"); // ns_net_cell_str_nr
}

namespace ns_net_cell_identity_nr {
// String, M
static const Json::StaticString key_5g_plmn("plmn");
// ULong, M
static const Json::StaticString key_5g_nci("nci");
// Int, M
static const Json::StaticString key_5g_pcid("pcid");
// UInt, M
static const Json::StaticString key_5g_tac("tac");
// Int, M
static const Json::StaticString key_5g_nrarfcn("nrarfcn");
// Obj, M
static const Json::StaticString key_5g_cell_info_plmn("cell_info_plmn"); // ns_net_cell_info_plmn
// Obj, M
static const Json::StaticString key_5g_cell_info_csg("cell_info_csg"); // ns_net_cell_info_csg
// Int[], M
static const Json::StaticString key_5g_nr_band_list("nr_band_list");
}

namespace ns_net_cell_str_nr {
// Int, M
static const Json::StaticString key_5g_ssrsrp("ssrsrp");
// Int, M
static const Json::StaticString key_5g_ssrsrq("ssrsrq");
// Int, M
static const Json::StaticString key_5g_sssinr("sssinr");
// Int, M
static const Json::StaticString key_5g_csirsrp("csirsrp");
// Int, M
static const Json::StaticString key_5g_csirsrq("csirsrq");
// Int, M
static const Json::StaticString key_5g_csisinr("csisinr");
// Int, M
static const Json::StaticString key_5g_cqi_table_ind("cqi_table_ind");
// Int, M
static const Json::StaticString key_5g_wbcqi("wbcqi");
// UInt[], M
static const Json::StaticString key_5g_sbcqi_list("sbcqi_list");
}

namespace ns_net_cell_info_common {
// Int, M
static const Json::StaticString key_cell_type("cell_type");              // sit_net_cell_info_type_e_type
// Int, M
static const Json::StaticString key_reg_status("reg_status");            // 0(UNREG), 1(REG)
// Int, M
static const Json::StaticString key_connect_state("connect_state");      // RIL_CellConnectionStatus
}

namespace ns_net_cell_info_plmn {
// String, O
static const Json::StaticString key_plmn_short_name("plmn_short_name");
// String, O
static const Json::StaticString key_plmn_long_name("plmn_long_name");
// String[], O
static const Json::StaticString key_plmn_info_list("plmn_info_list");
}

namespace ns_net_cell_info_csg {
// Int, M
static const Json::StaticString key_csg_ind("csg_ind");
// String, M
static const Json::StaticString key_home_nodeb_name("home_nodeb_name");
// Int, M
static const Json::StaticString key_csg_identiy("csg_identity");
}

/*
    SIT_SET_CELL_INFO_LIST_REPORT_RATE (RCM ID = 0x070D)
*/
namespace ns_net_set_cell_info_list_report_rate_req {
// Int, M
static const Json::StaticString key_rate("rate");
}

/*
    SIT_IND_NTW_STATE_CHANGED (RCM ID = 0x070E)
*/

/*
    SIT_IND_CELL_INFO_LIST (RCM ID = 0x070F)
*/
// same to ns_net_cell_info_list

/*
    SIT_SET_PS_SERVICE (RCM ID = 0x0710)
*/
namespace ns_net_getset_ps_service {
// Int, M
static const Json::StaticString key_ps_state("psstate");    // sit_net_enable_e_type
}

/*
    SIT_GET_PS_SERVICE (RCM ID = 0x0711)
*/
// same to ns_net_getset_ps_service

/*
   SIT_SET_EMERGENCY_CALL_STATUS (RCM ID = 0x0712)
 */
namespace ns_net_set_emergency_call_status_req {
// Int, M
static const Json::StaticString key_status("status");    // EMERGENCY_CALL_STATUS_START, etc
// Int, M
static const Json::StaticString key_rat("rat");          // sit_rat_type_e_type
}

/*
   SIT_IND_EMERGENCY_ACT_INFO (RCM ID = 0x0713)
 */
namespace ns_net_emergency_act_info_ind {
// Int, M
static const Json::StaticString key_rat("rat");               // sit_rat_type_e_type
// Int, M
static const Json::StaticString key_act_status("act_status"); // SIT_NET_NO_MORE_ACT_EMERGENCY_CALL, etc
}

/*
    SIT_SET_UPLMN (RCM ID = 0x0714) - Not used
*/
namespace ns_net_set_uplmn_req {
// Int, M
static const Json::StaticString key_mode("mode");             // sit_net_preferred_plmn_mode_e_type
// Int, M
static const Json::StaticString key_index("index");
// String, M
static const Json::StaticString key_plmn("plmn");
// Int M
static const Json::StaticString key_access_tech("act");       // sit_net_preferred_plmn_act_e_type: bitmap field
}

/*
    SIT_GET_UPLMN (RCM ID = 0x0715) - Not used
*/
namespace ns_net_get_uplmn_rsp {
// Obj[], M
static const Json::StaticString key_preferred_plmn_list("preferred_plmn_list");
}

namespace ns_net_preferred_plmn_info {
// Int, M
static const Json::StaticString key_index("index");
// String, M
static const Json::StaticString key_plmn("plmn");
// Int M
static const Json::StaticString key_access_tech("act");       // sit_net_preferred_plmn_act_e_type: bitmap field
}

/*
    SIT_SET_DS_NTW_TYPE (RCM ID = 0x0716)
*/
namespace ns_net_set_ds_ntw_type_req {
// Int, M
static const Json::StaticString key_pref_net_type("pref_net_type");    // sit_net_pref_ds_type_e_type
}

/*
    SIT_IND_NET_CURRENT_LTE_MODE  (RCM ID = 0x0717) - Not Used
*/

/*
    SIT_GET_RADIO_CAPABILITY (RCM ID = 0x0718)
*/
namespace ns_net_get_radio_capability_rsp {
// Int, M
static const Json::StaticString key_raf("raf");    // RAF_CP_UNKNOWN, etc: bitfiled setting
}

/*
    SIT_SET_RADIO_CAPABILITY (RCM ID = 0x0719)
*/
namespace ns_net_set_radio_capability_req {
// Int, M
static const Json::StaticString key_raf("raf");    // RAF_CP_UNKNOWN, etc: bitfiled setting
// Int, O
//static const Json::StaticString key_version("version");
// Int, O
//static const Json::StaticString key_session_id("session_id");
// Int, O
//static const Json::StaticString key_phase("phase");
// Int, O
//static const Json::StaticString key_status("status");
// String, O
//static const Json::StaticString key_uuid("uuid");
}

/*
    SIT_GET_DUPLEX_MODE (RCM ID = 0x071A)
*/
namespace ns_net_getset_duplex_mode {
// Int, M
static const Json::StaticString key_duplex_lte("duplex_lte");        // sit_net_duplex_mode_e_type
// Int, M
static const Json::StaticString key_duplex_hedge("duplex_hedge");    // sit_net_duplex_mode_e_type
}

/*
    SIT_SET_DUPLEX_MODE (RCM ID = 0x071B)
*/
// same to ns_net_getset_duplex_mode

/*
    SIT_IND_LTE_CA_CHANGED (RCM ID = 0x071C) - Not Used
*/

/*
    SIT_GET_LTE_CA_INFO (RCM ID = 0x071D) - Not Used
*/

/*
    SIT_GET_GSM_CELL_INFO (RCM ID = 0x071E) - Not Used
*/


/*
    SIT_SET_MICRO_CELL_SEARCH (RCM ID = 0x071F)
*/
namespace ns_net_set_micro_cell_search_req {
// Int, M
static const Json::StaticString key_search_mode("search_mode");        // sit_net_mc_srch_mode_type
}

namespace ns_net_set_micro_cell_search_rsp {
// Int, M
static const Json::StaticString key_search_result("search_result");    // sit_net_mc_srch_result_type
// String, M
static const Json::StaticString key_plmn("plmn");
}

/*
   SIT_IND_AC_BARRING_INFO (RCM ID = 0x0720)
*/
namespace ns_net_ac_barring_info_ind {
// Int, M
static const Json::StaticString key_ac_barring_emc("ac_barring_emc");   // 0:false, 1:true
// Obj[], M
static const Json::StaticString key_ac_barring_info_list("ac_barring_info_list");
}

namespace ns_net_ac_barring_info {
// Int, M
static const Json::StaticString key_ac_barring_info_type("ac_barring_info_type");    // SIT_AC_BARRING_TYPE_NONE, etc
// Obj, C
static const Json::StaticString key_mo_sig("mo_sig");                                // ns_net_ac_barring
// Obj, C
static const Json::StaticString key_mo_data("mo_data");                              // ns_net_ac_barring
// Obj, C
static const Json::StaticString key_mmtel_voice("mmtel_voice");                      // ns_net_ac_barring
// Obj, C
static const Json::StaticString key_mmtel_video("mmtel_video");                      // ns_net_ac_barring
}

namespace ns_net_ac_barring {
// Int, M
static const Json::StaticString key_barring_factor("barring_factor");
// Int, M
static const Json::StaticString key_barring_time("barring_time");
// Int[], M
static const Json::StaticString key_special_ac_list("special_ac_list");
}

/*
   SIT_SET_CDMA_ROAMING_PREFERENCE (RCM ID = 0x0721)
*/
namespace ns_net_getset_cdma_roaming_preferrence {
// Int, M
static const Json::StaticString key_cdma_mode("cdma_mode");          // sit_net_cdma_roaming_type
}

/*
   SIT_GET_CDMA_ROAMING_PREFERENCE (RCM ID = 0x0722)
*/
// same to ns_net_getset_cdma_roaming_preferrence

/*
    SIT_SET_CDMA_HYBRID_MODE (RCM ID = 0x723)
*/
namespace ns_net_getset_cdma_hybrid_mode {
// Int, M
static const Json::StaticString key_hybrid_mode("hybrid_mode");          // RIL_CdmaHybridMode
}

/*
    SIT_GET_CDMA_HYBRID_MODE (RCM ID = 0x724)
*/
// same to ns_net_getset_cdma_hybrid_mode

/*
    SIT_GET_LTE_BAND_ENABLE (RCM ID = 0x0725) - Not Used
*/

/*
    SIT_SET_LTE_BAND_ENABLE (RCM ID = 0x0726) - Not Used
*/

/*
    SIT_GET_LTE_BAND_PRIORITY (RCM ID = 0x0727) - Not Used
*/

/*
    SIT_SET_LTE_BAND_PRIORITY (RCM ID = 0x0728) - Not Used
*/

/*
    SIT_GET_LTE_ROAMING (RCM ID = 0x0729) - Not Used
*/

/*
    SIT_SET_LTE_ROAMING (RCM ID = 0x072A) - Not Used
*/

/*
    SIT_SET_DUAL_NTW_AND_PS_TYPE (RCM ID = 0x72B)
*/
namespace ns_net_set_dual_ntw_and_ps_type_req {
// Int, M
static const Json::StaticString key_pri_pref_net_type("pri_pref_nettype");    // sit_net_pref_network_type_e_type
// Int, M
static const Json::StaticString key_sec_pref_net_type("sec_pref_nettype");    // sit_net_pref_network_type_e_type
// Int, M
static const Json::StaticString key_pri_ps_capa("pri_ps_capa");          // sit_net_enable_e_type
// Int, M
static const Json::StaticString key_sec_ps_capa("sec_ps_capa");          // sit_net_enable_e_type
}

/*
    SIT_IND_TOTAL_OOS (RCM ID = 0x072C)
*/
namespace ns_net_total_oos_ind {
}

/*
    SIT_IND_MCC (RCM ID = 0x072D)
*/
namespace ns_net_mcc_ind {
// Int, M
static const Json::StaticString key_pref_net_type("pref_net_type");      // sit_net_pref_network_type_e_type
// String, M
static const Json::StaticString key_mcc("mcc");
}

/*
    SIT_SET_CARRIER_RESTRICTIONS (RCM ID = 0x072E)
*/
// This is defined in simjsondef.h.

/*
    SIT_GET_CARRIER_RESTRICTIONS (RCM ID = 0x072F)
*/
// This is defined in simjsondef.h.

/*
    SIT_GET_MANUAL_BAND_MODE (RCM ID = 0x0730)
*/
// This is defined in miscjsondef.h.

/*
    SIT_SET_MANUAL_BAND_MODE (RCM ID = 0x0731)
*/
// This is defined in miscjsondef.h.

/*
    SIT_GET_RF_DESENSE_MODE (RCM ID = 0x0732)
*/
// This is defined in miscjsondef.h.

/*
    SIT_SET_RF_DESENSE_MODE (RCM ID = 0x0733)
*/
// This is defined in miscjsondef.h.

/*
    SIT_START_SCANNING_NETWORKS (RCM ID = 0x0734)
*/
namespace ns_net_start_scanning_networks_req {
// Int, M
static const Json::StaticString key_scan_type_bitmask("scan_type_bitmask");
// Int, C
static const Json::StaticString key_interval("interval");
// Int, C
static const Json::StaticString key_max_search_time("max_search_time");
// Int, C
static const Json::StaticString key_incremental_result("incremental_result");
// Int, C
static const Json::StaticString key_incremental_result_periodicity("incremental_result_periodicity");
// Obj[], C
static const Json::StaticString key_network_scan_info_list("network_scan_info_list");
// String[], C
static const Json::StaticString key_plmn_list("plmn_list");
}

namespace ns_net_scan_info {
// Int, M
static const Json::StaticString key_ran("ran");           // sit_net_ran_e_type
// Int[], M
static const Json::StaticString key_band_list("band_list");
// UInt[], M
static const Json::StaticString key_channel_list("channel_list");
}

/*
    SIT_STOP_SCANNING_NETWORKS (RCM ID = 0x0735) - Not Used
*/


/*
    SIT_IND_SCANNING_NETWORKS (RCM ID = 0x0736)
*/
namespace ns_net_scanning_networks_ind {
// Int, M
static const Json::StaticString key_status("status");                   // sit_net_scan_type
// Obj[], M
static const Json::StaticString key_cell_info_list("cell_info_list");   // ns_net_cell_info
//
}

/*
    SIT_IND_RADIO_CAPABILITY (RCM ID = 0x0737) - Not Used
*/

/*
    SIT_GET_MANUAL_RAT_MODE (RCM ID = 0x0738)
*/
namespace ns_net_getset_manual_rat_mode {
// Int, M
static const Json::StaticString key_manual_rat_mode("manual_rat_mode");  // sit_net_enable_e_type
// Int, M
static const Json::StaticString key_pref_net_type("pref_net_type");    // sit_net_pref_network_type_e_type
}

/*
    SIT_SET_MANUAL_RAT_MODE (RCM ID = 0x0739)
*/
// req is same to ns_net_getset_manual_rat_mode

namespace ns_net_set_manual_rat_mode_rsp {
// Int, M
static const Json::StaticString key_manual_rat_cause("manual_rat_cause");  // 0:success, 1:RAT violation
}

/*
    SIT_GET_FREQUENCY_LOCK (RCM ID = 0x073A)
*/
namespace ns_net_getset_freq_lock {
// Int, M
static const Json::StaticString key_freq_lock_mode("freq_lock_mode");    // sit_net_enable_e_type
// Int, M
static const Json::StaticString key_rat("rat");                          // sit_freq_lock_rat_e_type
// UInt, C
static const Json::StaticString key_pcid("pcid");
// UInt, C
static const Json::StaticString key_earfcn("earfcn");
// UInt, C
static const Json::StaticString key_arfcn("arfcn");
// UInt, C
static const Json::StaticString key_psc("psc");
// UInt, C
static const Json::StaticString key_uarfcn("uarfcn");
}

/*
    SIT_SET_FREQUENCY_LOCK (RCM ID = 0x073B)
*/
// req is same to ns_net_getset_freq_lock

namespace ns_net_set_freq_lock_rsp {
// Int ,M
static const Json::StaticString key_freq_lock_cause("freq_lock_cause");    // 0:success, 1: not supported arrange, 2: unknown rat
}

/*
    SIT_IND_B2_B1_CONFIG (RCM ID = 0x073C)
*/
namespace ns_b2_b1_config_ind {
// Obj[], M
static const Json::StaticString key_bb_config_list("bb_config_list");
}

namespace ns_b2_b1_config_info {
// Int, M
static const Json::StaticString key_event_type("event_type");    // 0:event B1, 1:event B2
// Int, M
static const Json::StaticString key_ran("ran");                  // sit_net_ran_e_type
// Int, C
static const Json::StaticString key_eutran_threshold_type("eutran_threshold_type");    // 0:rsrp, 1:rsrq, 2;rssnr
// Int, C
static const Json::StaticString key_eutran_threshold("eutran_threshold");
// Int, C
static const Json::StaticString key_utran_threshold_type("utran_threshold_type");      // 0:rscp, 1:ecno
// Int, C
static const Json::StaticString key_utran_threshold("utran_threshold");
// Int, C
static const Json::StaticString key_geran_threshold("geran_threshold");
// Int, C
static const Json::StaticString key_cdma_threshold("cdma_threshold");
// Int, M
static const Json::StaticString key_hysteresis("hysteresis");
// Int, M
static const Json::StaticString key_time_to_trigger("time_to_trigger");
}

/*
    SIT_SET_ENDC_MODE (RCM ID = 0x073D)
*/
namespace ns_net_getset_endc_mode {
// Int, M
static const Json::StaticString key_endc_mode("endc_mode");                 // sit_net_enable_e_type
// Int, M
static const Json::StaticString key_immediate_scg_rel("immediate_scg_rel"); // sit_endc_release_e_type
}

/*
    SIT_GET_ENDC_MODE (RCM ID = 0x073E)
*/
// rsp is same to ns_net_getset_endc_mode

/*
   SIT_IND_SCG_BEARER_ALLOCATION (RCM ID = 0x073F)  - deprecated: legacy before AOSP Q
*/
namespace ns_net_scg_bearer_alloc_ind {
// Int, M
static const Json::StaticString key_rat("rat");                 // sit_rat_type_e_type
// Int, M
static const Json::StaticString key_scg_status("scg_status");   // 0(No SCG), 1(SCG ADDED)
}

/*
    SIT_SET_PREFERRED_DATA_MODEM (RCM ID = 0x0740)
*/
namespace ns_net_set_preffered_data_modem_req {
// Int, M
static const Json::StaticString key_stack_id("stack_id");    // 0: stack id 1,  1: stack id 2
}

/*
   SIT_IND_FREQUENCY_INFO (RCM ID = 0x0741)
*/
namespace ns_net_frequency_info_list {
// Obj[], M
static const Json::StaticString key_freq_info_list("freq_info_list");
}

namespace ns_net_freq_info {
// Int, M
static const Json::StaticString key_info_type("info_type");         // 0:primary, 1:secondary
// Int, M
static const Json::StaticString key_rat("rat");                     // 0:RAT_NONE, 1:RAT_GSM, 2:RAT_WCDMA, 3:RAT_LTE, 4:RAT_TDSCDMA, 5:RAT_CDMA, 6:RAT_NR
// UInt, M
static const Json::StaticString key_band("band");
// UInt, M
static const Json::StaticString key_freq_number("freq_info");
}

/*
    SIT_IND_PHYSICAL_CHANNEL_CONFIG (RCM ID = 0x0742)
*/
namespace ns_net_physical_channel_config_ind {
// Obj[], M
static const Json::StaticString key_pci_list("pci_list");
}

namespace ns_net_physical_channel_config_info {
// Int, M
static const Json::StaticString key_cell_status("cell_status");    // RIL_CellConnectionStatus
// UInt, M
static const Json::StaticString key_cell_bw_dl("cell_bw_dl");
// Int, M
static const Json::StaticString key_rat("rat");                    // sit_rat_type_e_type
// Int, M
static const Json::StaticString key_freq_range("freq_range");      // SIT_FREQUENCY_RANGE_NONE, etc
// UInt, M
static const Json::StaticString key_channel("channel");
// Int[], M
static const Json::StaticString key_context_id_list("context_id_list");
// UInt, M
static const Json::StaticString key_pcid("pcid");
// UInt, M
static const Json::StaticString key_channel_uplink("channel_uplink");
// UInt, M
static const Json::StaticString key_cell_bw_ul("cell_bw_uplink");
// Int, M
static const Json::StaticString key_band_num("band_num");
}

/*
    SIT_SET_LOCATION_UPDATE_SETTING (RCM ID = 0x0744)
*/
// This is defined in miscjsondef.h.

/*
    SIT_GET_FREQUENCY_INFO (RCM ID = 0x0746)
*/
// rsp is same to ns_net_frequency_info_list

/*
    SIT_IND_RRC_INFO (RCM ID = 0x0747)
*/
namespace ns_net_rrc_info_ind {
// Int, M
static const Json::StaticString key_rat("rat");                // sit_rat_type_e_type
// Int, M
static const Json::StaticString key_rrc_state("rrc_state");    // sit_net_rrc_info_state
}

/*
    SIT_SET_NR_MODE (RCM ID = 0x0748)
*/
namespace ns_net_getset_nr_mode {
// Int, M
static const Json::StaticString key_nr_mode("nr_mode"); // sit_nr_mode_e_type
}

/*
    SIT_GET_NR_MODE (RCM ID = 0x0749)
*/
// rsp is same to ns_net_getset_nr_mode

/*
    SIT_IND_REG_FAILED (RCM ID = 0x074A)
*/
namespace ns_net_reg_failed_ind {
// String, M
static const Json::StaticString key_plmn("plmn");
// Int, M
static const Json::StaticString key_domain("domain");                   // sit_service_domain_type
// Int, M
static const Json::StaticString key_rej_cause("rej_cause");
// Int, M
static const Json::StaticString key_add_rej_cause("add_rej_cause");
// Int, M
static const Json::StaticString key_cell_info_type("cell_info_type");   // sit_net_cell_info_type_e_type
// Obj, C
static const Json::StaticString key_cell_identity_gsm("cell_identity_gsm");         // ns_net_cell_identity_gsm
// Obj, C
static const Json::StaticString key_cell_identity_cdma("cell_identity_cdma");       // ns_net_cell_identity_cdma
// Obj, C
static const Json::StaticString key_cell_identity_lte("cell_identity_lte");         // ns_net_cell_identity_lte
// Obj, C
static const Json::StaticString key_cell_identity_wcdma("cell_identity_wcdma");     // ns_net_cell_identity_wcdma
// Obj, C
static const Json::StaticString key_cell_identity_tdscdma("cell_identity_tdscdma"); // ns_net_cell_identity_tdscdma
// Obj, C
static const Json::StaticString key_cell_identity_nr("cell_identity_nr");           // ns_net_cell_identity_nr
}

/*
    SIT_IND_BARRING_INFO_CHANGED (RCM ID = 0x074B)
*/
namespace ns_net_barring_info_changed {
// Int, M
static const Json::StaticString key_cell_info_type("cell_info_type");               // sit_net_cell_info_type_e_type
// Obj, C
static const Json::StaticString key_cell_identity_gsm("cell_identity_gsm");         // ns_net_cell_identity_gsm
// Obj, C
static const Json::StaticString key_cell_identity_cdma("cell_identity_cdma");       // ns_net_cell_identity_cdma
// Obj, C
static const Json::StaticString key_cell_identity_lte("cell_identity_lte");         // ns_net_cell_identity_lte
// Obj, C
static const Json::StaticString key_cell_identity_wcdma("cell_identity_wcdma");     // ns_net_cell_identity_wcdma
// Obj, C
static const Json::StaticString key_cell_identity_tdscdma("cell_identity_tdscdma"); // ns_net_cell_identity_tdscdma
// Obj, C
static const Json::StaticString key_cell_identity_nr("cell_identity_nr");           // ns_net_cell_identity_nr
// Obj[], M
static const Json::StaticString key_cell_barring_info_list("barring_info_list");
}

namespace ns_net_barring_info {
// Int, M
static const Json::StaticString key_service_type("service_type");        // RIL_BarringServiceType
// Int, M
static const Json::StaticString key_barring_type("barring_type");        // RIL_BarringType
// Int, M
static const Json::StaticString key_barring_factor("barring_factor");
// Int, M
static const Json::StaticString key_time_seconds("time_seconds");
// Int, M
static const Json::StaticString key_is_barred("is_barred");               // 0: false, 1: true
}

/*
    SIT_SET_SYSTEM_SELECTION_CHANNELS (RCM ID = 0x074C)
*/
namespace ns_net_set_system_selection_channels_req {
// Int, M
static const Json::StaticString key_is_specify_channels("is_specify_channels");    // 0, 1(TRUE)
// Obj[], M
static const Json::StaticString key_radio_access_specifier_list("radio_access_specifier");
}

namespace ns_net_ras {
// Int, M
static const Json::StaticString key_ran("ran");    // sit_net_ran_e_type
// Int[], M
static const Json::StaticString key_band_list("band_list");
// Int[], M
static const Json::StaticString key_channel_list("channel_list");
}

/*
    SIT_GET_BARRING_INFOS (RCM ID = 0x074D)
*/
// same to ns_net_barring_info_changed

/*
    SIT_GET_SYSTEM_SELECTION_CHANNELS (RCM ID = 0x074E)
      - Changes for this ILM are under verification at CP. so it is not in document.
*/

/*
    SIT_SET_ALLOW_NETWORK (RCM ID = 0x074F)
*/
namespace ns_net_getset_allow_network {
// Int, M
static const Json::StaticString key_raf("rat_mode");    // RAF_CP_UNKNOWN, etc: bitfiled setting
}

/*
    SIT_GET_ALLOW_NETWORK (RCM ID = 0x0750)
*/
// rsp is same to ns_net_getset_allow_network

/*
    SIT_SET_PLMN_LIST (RCM ID = 0x0751) - Not defined in doc
*/

/*
    SIT_GET_PLMN_LIST (RCM ID = 0x0752) - Not defined in doc
*/

/******************************************************************************
 * SIM
 *****************************************************************************/
/*
    SIT_IND_SIM_DATA_INFO (RCM ID = 0x024F)
*/
namespace ns_net_sim_file_data_info_ind {
// Int, M
static const Json::StaticString key_sim_file_id("sim_file_id");
// Int, M
static const Json::StaticString key_record_len("record_len");
// Int, M
static const Json::StaticString key_num_records("num_rec");
// Int, C
static const Json::StaticString key_ef_data_list("ef_data_list");  // received only when num_records > 0
} // namespace ns_net_sim_file_data_info_ind


/******************************************************************************
 * Power
 *****************************************************************************/
/*
   SIT_SET_RADIO_POWER (RCM ID = 0x0800)
 */
namespace ns_pwr_set_radio_power_req {
// Int, M
static const Json::StaticString key_radio_state("radio_state"); // sit_pwr_radio_state_e_type
// Int, M
static const Json::StaticString key_for_emc("for_emc");
// Int, M
static const Json::StaticString key_preferred_for_emc("preferred_for_emc");
} // namespace ns_pwr_set_radio_power_req

namespace ns_pwr_radio_power_rsp {
// no key required as rsp has no data apart from header
} // namespace ns_pwr_radio_power_rsp

/*
   SIT_GET_RADIO_POWER (RCM ID = 0x801)
 */
namespace ns_pwr_get_radio_power_req {
// no key required as req has no data apart from header
} // namespace ns_pwr_get_radio_power_req

namespace ns_pwr_get_radio_power_rsp {
// Int, M
static const Json::StaticString key_radio_state("radio_state"); // sit_pwr_radio_sim_state_e_type
} // namesapce ns_pwr_get_radio_power_rsp

/*
   SIT_IND_RADIO_STATE_CHANGED (RCM ID = 0x0802)
 */
namespace ns_pwr_radio_state_changed_ind {
// Int, M
static const Json::StaticString key_radio_state("radio_state"); // sit_pwr_radio_state_e_type
} // namespace ns_pwr_radio_state_changed_ind

/*
   SIT_IND_RADIO_READY (RCM ID = 0x0803)
 */
namespace ns_pwr_radio_ready_ind { }

/*
   SIT_IND_PHONE_RESET (RCM ID = 0x0804)
 */
// this is defined in miscjsondef

/*
   SIT_SET_STACK_STATUS (RCM ID = 0x080F)
 */
// this is defined in miscjsondef

/*
   SIT_GET_STACK_STATUS (RCM ID = 0x0810)
 */
// this is defined in miscjsondef

/******************************************************************************
 * MISC
 *****************************************************************************/
/*
   SIT_SET_NR_SILENCE_MODE (RCM ID = 0x094C)
 */
namespace ns_net_misc_set_nr_silence_mode_req {
// Int, M
static const Json::StaticString key_nsa_mode("nsa_silence_mode");
// Int, M
static const Json::StaticString key_sa_mode("sa_silence_mode");
} // namespace ns_net_misc_set_nr_silence_mode_req

namespace ns_net_misc_set_nr_silence_mode_rsp { }

/*
   SIT_GET_NR_SILENCE_MODE (RCM ID = 0x094EF)
 */
namespace ns_net_misc_get_nr_silence_mode_req { }

namespace ns_net_misc_get_nr_silence_mode_rsp {
// Int, M
static const Json::StaticString key_nsa_mode("nsa_silence_mode");
// Int, M
static const Json::StaticString key_sa_mode("sa_silence_mode");
} // namespace ns_net_misc_get_nr_silence_mode_rsp

/*
   SIT_GET_VONR_CAPABILITY (RCM ID = 0x0953)
 */
namespace ns_net_misc_get_vonr_capability_req { }

namespace ns_net_misc_get_vonr_capability_rsp {
// Int, M
static const Json::StaticString key_vonr_capa("vonr_capa");
} // namespace ns_net_misc_get_vonr_capability_rsp

/*
   SIT_SET_VONR_CAPABILITY (RCM ID = 0x0954)
 */
namespace ns_net_misc_set_vonr_capability_req {
// Int, M
static const Json::StaticString key_vonr_capa("vonr_capa");
} // namespace ns_net_misc_set_vonr_capability_req

namespace ns_net_misc_set_vonr_capability_rsp { }

/******************************************************************************
 * PS
 *****************************************************************************/
/*
   SIT_SET_MOBILE_DATA_STATE (RCM ID = 0x0625)
 */
namespace ns_net_ps_set_mobile_data_state_req {
// int, M
static const Json::StaticString key_mobile_data("mobile_data");
// int, M
static const Json::StaticString key_roaming_data("roaming_data");
} // namespace ns_net_ps_set_mobile_data_state_req

#endif /* _NET_JSON_DEF_H_ */
