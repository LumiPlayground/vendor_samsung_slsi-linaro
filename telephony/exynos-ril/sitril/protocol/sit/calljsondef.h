/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _CALL_JSON_DEF_H_
#define _CALL_JSON_DEF_H_

#include "json/json.h"

/******************************************************************************
 *
 * File: calljsondef.h
 *
 * Description: This file will have key-value pair definition of all CALL SIT commands
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
   SIT_GET_CURRENT_CALLS (RCM ID = 0x0000)
 */
namespace ns_call_get_current_calls_req { }

namespace ns_call_get_current_calls_rsp {
// Obj[], M
static const Json::StaticString key_current_call_list("current_call_list"); // ns_call_call_info
} // namesapce ns_call_get_current_calls_rsp

namespace ns_call_call_info {
// Int, M
static const Json::StaticString key_call_state("call_state"); // sit_call_state_e_type
// Int, M
static const Json::StaticString key_index("index");
// Int, M
static const Json::StaticString key_toa("address_type");
// Int, M
static const Json::StaticString key_is_mpty("is_mpty"); // sit_call_is_mpty_e_type
// Int, M
static const Json::StaticString key_is_mt("is_mt"); // sit_call_is_mtpy_e_type
// Int, M
static const Json::StaticString key_als("als");
// Int, M
static const Json::StaticString key_call_type("call_type"); // sit_call_call_type_e_type
// Int, M
static const Json::StaticString key_is_voice_privacy("is_voice_privacy"); // sit_call_is_voice_privacy_e_type
// String, M
static const Json::StaticString key_number("number");
// Int, M
static const Json::StaticString key_number_presentation("number_presentation"); // sit_call_presentation_e_type
// String, M
static const Json::StaticString key_name("name");
// Int, M
static const Json::StaticString key_name_presentation("name_presentation"); // sit_call_presentation_e_type
// Int, M
static const Json::StaticString key_name_dcs_type("name_dcs_type");
// Int, M
static const Json::StaticString key_uus_type("uus_type"); // sit_call_uus_type_e_type
// Int, M
static const Json::StaticString key_uus_dcs("uus_dcs"); // sit_call_uus_type_e_type
// String, M
static const Json::StaticString key_uus_data("uus_data");
// Int, M
static const Json::StaticString key_srvcc_call("srvcccall");
// Int, M
static const Json::StaticString key_audio_quality("audio_quality"); // sit_call_audio_quality_e_type
// String, M
static const Json::StaticString key_forwarded_num("forwared_num");
} // namespace ns_call_call_info

/*
   SIT_DIAL (RCM ID = 0x0001)
*/
namespace ns_call_dial_req {
// Int, M
static const Json::StaticString key_call_type("call_type"); // sit_call_call_type_e_type
// Int, M
static const Json::StaticString key_emc_call_sub_type("emc_call_sub_type"); // sit_call_emergency_call_subtype_e_type
// String, M
static const Json::StaticString key_number("number");
// Int, M
static const Json::StaticString key_number_type("number_type"); // sit_call_number_type_e_type
// Int, M
static const Json::StaticString key_number_plan("number_plan"); // sit_call_number_plan_e_type
// Int, M
static const Json::StaticString key_clir("clir"); // sit_call_clir_e_type
// Int, M
static const Json::StaticString key_cug_call("cug_call"); // sit_call_cug_call_e_type
// Int, O
static const Json::StaticString key_cug_info_index("cug_info_index");
// Int, O
static const Json::StaticString key_cug_info("cug_info"); // sit_call_cug_info_e_type
// Int, C
static const Json::StaticString key_ecall_variant("ecall_variant"); // Currently not in use
// Int, C
static const Json::StaticString key_ecall_mode("ecall_mode"); // currently not in use
// String, C
static const Json::StaticString key_msd_data("msd_data"); // Currently not in use
} // namespace ns_call_dial_req

namespace ns_call_dial_rsp { }

/*
  SIT_GET_LAST_CALL_FAIL_CAUSE (RCM ID = 0x0002)
*/
namespace ns_call_get_last_call_fail_cause_req { }

namespace ns_call_get_last_call_fail_cause_rsp {
// Int, M
static const Json::StaticString key_last_call_fail_cause("last_call_fail_cause"); // sit_call_last_call_fail_cause_e_type
} // namespace ns_call_get_last_call_fail_cause_rsp

/*
  SIT_DTMF (RCM ID = 0x0003)
*/
// Refer ns_misc_call_dtmf_req
// Refer ns_misc_call_dtmf_rsp

/*
  SIT_ANSWER (RCM ID = 0x0004)
*/
namespace ns_call_answer_req { }

namespace ns_call_answer_rsp { }

/*
  SIT_DTMF_START (RCM ID = 0x0005)
*/
// Refer ns_misc_call_dtmf_start_req
// Refer ns_misc_call_dtmf_start_rsp

/*
  SIT_DTMF_STOP (RCM ID = 0x0006)
*/
// Refer ns_misc_call_dtmf_stop_req
// Refer ns_misc_call_dtmf_stop_rsp

/*
  SIT_SEND_EXPLICIT_CALL_TRANSFER (RCM ID = 0x0007)
*/
namespace ns_call_ect_req { }

namespace ns_call_ect_rsp { }

/*
  SIT_HANGUP (RCM ID = 0x0008)
*/
namespace ns_call_hangup_req {
// Int, M
static const Json::StaticString key_call_index("index");
} // namespace ns_call_hangup_req

namespace ns_call_hangup_rsp { }

/*
   SIT_IND_CALL_STATE_CHANGED (RCM ID = 0x0009)
*/
namespace ns_call_call_state_changed_ind { }

/*
   SIT_IND_CALL_RING (RCM ID = 0x000A)
*/
namespace ns_call_call_ring_ind { }

/*
   SIT_IND_SRVCC_HO (RCM ID = 0x000C)
*/
namespace ns_call_srvcc_ho_ind {
// Int, M
static const Json::StaticString key_srvcc_ho_type("ho_type");
// Int, M
static const Json::StaticString key_srvcc_ho_status("ho_status"); //  sit_call_srvcc_ho_status_e_type;
// Int, M
static const  Json::StaticString key_srvcc_ho_fail_cause("srvcc_fail_cause");
} // ns_call_srvcc_ho_ind

/*
   SIT_IND_CALL_PRESENT_IND (RCM ID = 0x0010)
*/
namespace ns_call_call_present_ind { }

/*
   SIT_IND_EMERGENCY_CALL_LIST (RCM ID = 0x000D)
*/
namespace ns_call_emc_call_list_ind {
// String, M
static const Json::StaticString key_mcc("mcc");
// String, M
static const Json::StaticString key_mnc("mnc");
// Obj[], M
static const Json::StaticString key_emc_nums_info_data("emc_num_info_data"); // ns_call_emc_num_info
} // namespace ns_call_emc_call_list_ind

namespace ns_call_emc_num_info {
// Int, M
static const Json::StaticString key_service_category("service_category");
// String, M
static const Json::StaticString key_emc_number("emc_number");
// Int, M
static const Json::StaticString key_source("source");
} // ns_call_emc_num_info

/*
   SIT_CALL_CONFIRM (RCM ID = 0x000F
 */
namespace ns_call_call_confirm_req { }

namespace ns_call_call_confirm_rsp {
//Int, M
static const Json::StaticString key_result("result");
} // namespace ns_call_call_confirm_rsp

/*
   SIT_SET_CDMA_VOICE_PRIVACY_MODE (RCM ID = 0x0011)
 */
namespace ns_call_set_cdma_voice_privacy_mode_req {
// Int, M
static const Json::StaticString key_vp_mode("vp_mode");
} // namesapce ns_call_set_cdma_voice_privacy_mode_req

namespace ns_call_set_cdma_voice_privacy_mode_rsp { }

/*
   SIT_GET_CDMA_VOICE_PRIVACY_MODE (RCM ID = 0x0013
 */
namespace ns_call_get_cdma_voice_privacy_mode_req { }

namespace ns_call_get_cdma_voice_privacy_mode_rsp {
// Int, M
static const Json::StaticString key_vp_mode("vp_mode");
} // namesapce ns_call_get_cdma_voice_privacy_mode_rsp

/*
   SIT_CDMA_BURST_DTMF (RCMS ID = 0x0015)
 */
namespace ns_call_cdma_burst_dtmf_req {
// String, M
static const Json::StaticString key_dtmf_digits("dtmf_digits");
// String, M
static const Json::StaticString key_dtmf_on_length("dtmf_on_length");
// String, M
static const Json::StaticString key_dtmf_off_length("dtmf_off_length");
} // namespace ns_call_cdma_burst_dtmf_req

namespace ns_call_cdma_burst_dtmf_rsp { }

/*
  SIT_IND_OTA_PROVISION_STATUS (RCMD ID = 0x0016)
 */
namespace ns_call_ota_provision_status_ind {
// Int, M
static const Json::StaticString key_ota_type("ota_type"); // sit_ota_e_type
// Int, M
static const Json::StaticString key_ota_status("ota_status"); // sit_otasp_status_e_type & sit_otapa_status_e_type
} // namespace ns_call_ota_provision_status_ind

/*
   SIT_IND_CDMA_INFO_REC (RCM ID = 0x0017)
 */
namespace ns_call_cdma_info_rec_ind {
// Obj[], M
static const Json::StaticString key_cdma_info_records("cdma_info_records"); // ns_call_cdma_info_record
} // namespace ns_call_cdma_info_rec_ind

namespace ns_call_cdma_info_record {
// Int, M
static const Json::StaticString key_cdma_info_name("cdma_info_name"); // sit_cdma_info_rec_name_e_type
// Obj, C
static const Json::StaticString key_display_info_record("display_info_record"); // ns_call_display_info_record
// Obj, C
static const Json::StaticString key_num_info_record("num_info_record"); // ns_call_num_info_record
// Obj, C
static const Json::StaticString key_signal_info_record("signal_info_record"); // ns_call_signal_info_record
// Obj, C
static const Json::StaticString key_redirecting_info_record("redirecting_info_record"); // ns_call_redirecting_info_record
// Obj, C
static const Json::StaticString key_line_ctr_info_record("line_ctr_info_record"); // ns_call_line_ctr_info_record
} // namespaec ns_call_cdma_info_record

namespace ns_call_display_info_record {
// String, M
static const Json::StaticString key_display_char("display_char");
} // namespace ns_call_display_info_record

namespace ns_call_num_info_record {
// String, M
static const Json::StaticString key_number("number");
// Int, M
static const Json::StaticString key_number_type("number_type");
// Int, M
static const Json::StaticString key_number_plan("number_plan");
// Int, M
static const Json::StaticString key_presentation_indicator("pi");
// Int, M
static const Json::StaticString key_screening_indicator("si");
} // namesapce ns_call_num_info_record

namespace ns_call_signal_info_record {
// Int, M
static const Json::StaticString key_is_present("is_present");
// Int, M
static const Json::StaticString key_signal_type("signal_type");
// Int, M
static const Json::StaticString key_alert_pitch("alert_pitch");
// Int, M
static const Json::StaticString key_signal("signal");
} // namespace ns_call_signal_info_record

namespace ns_call_redirecting_info_record {
// String, M
static const Json::StaticString key_number("number");
// Int, M
static const Json::StaticString key_number_type("number_type");
// Int, M
static const Json::StaticString key_number_plan("number_plan");
// Int, M
static const Json::StaticString key_presentation_indicator("pi");
// Int, M
static const Json::StaticString key_screening_indicator("si");
// Int, M
static const Json::StaticString key_reason("reason"); // sit_cdma_redirecting_reason_e_type
} // namespace ns_call_redirecting_info_record

namespace ns_call_line_ctr_info_record {
// Int, M
static const Json::StaticString key_polarity("polarity");
// Int, M
static const Json::StaticString key_toggle("toggel");
// Int, M
static const Json::StaticString key_reverse("reverse");
// Int, M
static const Json::StaticString key_power_denial("power_denial");
} // namespace ns_call_line_ctr_info_record

/*
   SIT_IND_EMERGENCY_SUPPORT_RAT_MODE (RCM ID = 0x0018)
 */
namespace ns_call_emc_support_rat_mode_ind {
// Int, M
static const Json::StaticString key_available_tech("available_tech"); // sit_emergency_support_rat_mode_e_type
} // namespace ns_call_emc_support_rat_mode_ind

/*
   SIT_HANGUP_WAITING_OR_BACKGROUND (RCM ID = 0x0500)
 */
namespace ns_ss_hangup_waiting_or_background_req { }

namespace ns_ss_hangup_waiting_or_background_rsp { }

/*
   SIT_HANGUP_FORGROUND_RESUME_BACKGROUND (RCM ID = 0x0501)
 */
namespace ns_ss_hangup_foreground_resume_background_req { }

namespace ns_ss_hangup_foreground_resume_background_rsp { }

/*
   SIT_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE (RCM ID = 0x0502)
 */
namespace ns_ss_switch_waiting_or_holding_and_active_req { }

namespace ns_ss_switch_waiting_or_holding_and_active_rsp { }

/*
   SIT_CONFERENCE (RCM ID = 0x0503)
 */
namespace ns_ss_conference_req { }

namespace ns_ss_conference_rsp { }

/*
   SIT_UDUB (RCM ID = 0x0504)
 */
namespace ns_ss_udub_req { }

namespace ns_ss_udub_rsp { }

/*
   SIT_SEND_USSD (RCM ID = 0x0505)
 */
namespace ns_ss_send_ussd_req {
// Int, M
static const Json::StaticString key_ussd_type("ussd_type");  // sit_ss_ussd_type_e_type
// Int, C
static const Json::StaticString key_dcs_type("dcs_type");
// Int[], C
static const Json::StaticString key_ussd_data("ussd_data");
}

namespace ns_ss_send_ussd_rsp { }

/*
   SIT_CANCEL_USSD (RCM ID = 0x0506)
 */
namespace ns_ss_cancel_ussd_req { }

namespace ns_ss_cancel_ussd_rsp { }

/*
   SIT_GET_CLIR (RCM ID = 0x0507)
 */
namespace ns_ss_get_clir_req { }

namespace ns_ss_get_clir_rsp {
// Int, M
static const Json::StaticString key_clir_status("status");  // sit_ss_get_clir_status_e_type
}

/*
   SIT_GET_COLP (RCM ID = 0x0508)
 */
namespace ns_ss_get_colp_req { }

namespace ns_ss_get_colp_rsp {
// Int, M
static const Json::StaticString key_colp_status("status");
}

/*
   SIT_GET_CALL_FORWARD_STATUS (RCM ID = 0x0509)
 */

namespace ns_ss_get_call_forward_status_req {
// Int, M
static const Json::StaticString key_status("status");
// Int, M
static const Json::StaticString key_reason("reason");
// Int, M
static const Json::StaticString key_srvc_class("srvc_class");
}

namespace ns_ss_get_call_forward_status_rsp {
// Obj[], M
static const Json::StaticString key_call_frwrd_status_list("call_frwd_status_list");  // ns_ss_call_forward_status
}

namespace ns_ss_call_forward_status {
// Int, M
static const Json::StaticString key_status("status");
// Int, M
static const Json::StaticString key_reason("reason");  // sit_ss_call_forward_reason_e_type
// Int, M
static const Json::StaticString key_srvc_class("srvc_class");  // sit_ss_service_class_e_type
// Int, M
static const Json::StaticString key_toa("toa");  // sit_ss_toa_e_type
// Int, M
static const Json::StaticString key_number("number");
// Int, M
static const Json::StaticString key_time_secs("time_secs");
}

/*
   SIT_SET_CALL_FORWARD (RCM ID = 0x050A)
 */
namespace ns_ss_set_call_forward_req {
// Int, M
static const Json::StaticString key_status("status");  // sit_ss_call_forward_status_e_type
// Int, M
static const Json::StaticString key_reason("reason");
// Int, M
static const Json::StaticString key_srvc_class("srvc_class");
// Int, M
static const Json::StaticString key_toa("toa");
// String, M
static const Json::StaticString key_number("number");
// Int, M
static const Json::StaticString key_time_secs("time_secs");
}

namespace ns_ss_set_call_forward_rsp { }

/*
   SIT_GET_CALL_WAITING (RCM ID = 0x050B)
 */
namespace ns_ss_get_call_waiting_req {
// Int, M
static const Json::StaticString key_service_class("srvc_class");
}

namespace ns_ss_get_call_waiting_rsp {
// Int, M
static const Json::StaticString key_service_status("srvc_status");
// Int, M
static const Json::StaticString key_service_class("srvc_class");
}

/*
   SIT_SET_CALL_WAITING (RCM ID = 0x050C)
 */
namespace ns_ss_set_call_waiting_req {
// Int, M
static const Json::StaticString key_service_status("srvc_status");
// Int, M
static const Json::StaticString key_service_class("srvc_class");
}

namespace ns_ss_set_call_waiting_rsp { }

/*
   SIT_CHG_BARRING_PWD (RCM ID = 0x050D)
 */
namespace ns_ss_change_barring_pwd_req {
// String, M
static const Json::StaticString key_password("pwd");
// String, M
static const Json::StaticString key_new_password("new_pwd");
// String, M
static const Json::StaticString key_new_password_again("new_pwd_again");
}

namespace ns_ss_change_barring_pwd_rsp { }

/*
   SIT_SEPARATE_CONNECTION (RCM ID = 0x050E)
 */
namespace ns_ss_separate_connection_req {
// Int, M
static const Json::StaticString key_index("index");
}

namespace ns_ss_separate_connection_rsp { }

/*
   SIT_GET_CLIP (RCM ID = 0x050F)
 */
namespace ns_ss_get_clip_req { }

namespace ns_ss_get_clip_rsp {
// Int, M
static const Json::StaticString key_status("status");  // sit_ss_get_clip_status_e_type
}

/*
   SIT_IND_ON_USSD (RCM ID = 0x0510)
 */
namespace ns_ss_ussd_ind {
// Int, M
static const Json::StaticString key_status("status");  // sit_ss_ussd_ind_status_e_type
// Int, M
static const Json::StaticString key_dcs_type("dcs_type");
// Int[], M
static const Json::StaticString key_ussd_data("ussd_data");
}

/*
   SIT_IND_SUPP_SVC_NOTIFICATION (RCM ID = 0x0511)
 */
namespace ns_ss_supp_svc_notification_ind {
// Int, M
static const Json::StaticString key_noti_type("noti_type");  // sit_ss_ssnoti_type_e_type
// Int, M
static const Json::StaticString key_code("code");  // sit_ss_code1_e_type, sit_ss_code2_e_type
// Int, M
static const Json::StaticString key_index("index");
// Int, C
static const Json::StaticString key_number_type("type");  // received only for RIL_SSNOTI_TYPE_MT
// String, C
static const Json::StaticString key_number("number");  // received only for RIL_SSNOTI_TYPE_MT
}

/*
   SIT_GET_COLR (RCM ID = 0x0512)
 */
namespace ns_ss_get_colr_req { }

namespace ns_ss_get_colr_rsp {
// Int, M
static const Json::StaticString key_status("status");
}

/*
   SIT_IND_SS_RETURN_RESULT (RCM ID = 0x0513)
 */
// This is defined in stkjsondef.h

/*
   SIT_SET_CDMA_FLASH_INFO (RCM ID = 0x0514)
 */
namespace ns_ss_set_cdma_flash_info_req {
// String, M
static const Json::StaticString key_flash("flash");
}

namespace ns_ss_set_cdma_flash_info_rsp { }

/*
   SIT_IND_CDMA_CALL_WAITING (RCM ID = 0x0515)
 */
namespace ns_ss_cdma_call_waiting_ind {
// String, M
static const Json::StaticString key_number("number");
// Int, M
static const Json::StaticString key_number_presentation("nmbr_presentation");
// String, M
static const Json::StaticString key_name("name");
// Int, M
static const Json::StaticString key_signal_type("signal_type");
// Int, M
static const Json::StaticString key_alert_pitch("alert_pitch");
// Int, M
static const Json::StaticString key_signal("signal");
// Int, M
static const Json::StaticString key_number_type("nmbr_type");
// Int, M
static const Json::StaticString key_number_plan("nmbr_plan");
}

#endif /* _CALL_JSON_DEF_H_ */
