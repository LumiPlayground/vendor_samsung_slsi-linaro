/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _SMS_JSON_DEF_H_
#define _SMS_JSON_DEF_H_

#include "json/json.h"

/******************************************************************************
 *
 * File: smsjsondef.h
 *
 * Description: This file will have key-value pair definition of all SMS SIT commands
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
   SIT_SEND_SMS (RCM ID = 0x0100)
 */
namespace ns_sms_send_sms_req {
// Int, M
static const Json::StaticString key_sms_domain("sms_domain");
// Int, M
static const Json::StaticString key_smsc("smsc");
// Int[], M
static const Json::StaticString key_sms_data("sms_data");
}

namespace ns_sms_send_sms_rsp {
// Int, M
static const Json::StaticString key_msg_ref("msg_ref");
// Int[], M
static const Json::StaticString key_ack_pdu("ack_pdu");
// Int, M
static const Json::StaticString key_error_code("error_code");
}

/*
   SIT_SEND_EXPECT_MORE (RCM ID = 0x0101)
 */
// json definition same as SIT_SEND_SMS

/*
   SIT_SEND_SMS_ACK (RCM ID = 0x0102)
 */
namespace ns_sms_send_sms_ack_req {
// Int, M
static const Json::StaticString key_result("result");
// Int, M
static const Json::StaticString key_msg_tpid("msg_tpid");
// Int, M
static const Json::StaticString key_error_code("error_code");
}

namespace ns_sms_send_sms_ack_rsp { }

/*
   SIT_WRITE_SMS_TO_SIM (RCM ID = 0x0103)
 */
namespace ns_sms_write_sms_to_sim_req {
// Int, M
static const Json::StaticString key_msg_status("msg_status");  // sit_sms_status_e_type
// Int, M
static const Json::StaticString key_index("index");
// Int[], M
static const Json::StaticString key_pdu_data("pdu_data");
}

namespace ns_sms_write_sms_to_sim_rsp {
// Int, C
static const Json::StaticString key_index("index");
}

/*
   SIT_DELETE_SMS_ON_SIM (RCM ID = 0x0104)
 */
namespace ns_sms_delete_sms_on_sim_req {
// Int, M
static const Json::StaticString key_index("index");
}

namespace ns_sms_delete_sms_on_sim_rsp { }

/*
    SIT_GET_BCST_SMS_CFG (RCM ID = 0x0105)
 */
namespace ns_sms_get_bcst_sms_cfg_req { }

namespace ns_sms_get_bcst_sms_cfg_rsp {
// Obj[], M
static const Json::StaticString key_bcst_conf_info_list("bcst_conf_info_list");  // ns_sms_bcst_conf_info
}

namespace ns_sms_bcst_conf_info {
// Int, M
static const Json::StaticString key_from_svc_id("from_svc_id");
// Int, M
static const Json::StaticString key_to_svc_id("to_svc_id");
// Int, M
static const Json::StaticString key_from_code_scheme("from_code_scheme");
// Int, M
static const Json::StaticString key_to_code_scheme("to_code_scheme");
// Int, M
static const Json::StaticString key_selected("selected");
}

/*
    SIT_SET_BCST_SMS_CFG (RCM ID = 0x0106)
 */
namespace ns_sms_set_bcst_sms_cfg_req {
// Obj[], M
static const Json::StaticString key_bcst_conf_info_list("bcst_conf_info_list");  // ns_sms_bcst_conf_info
}

namespace ns_sms_set_bcst_sms_cfg_rsp { }

/*
   SIT_ACT_BCST_SMS (RCM ID = 0x0107)
 */
namespace ns_sms_act_bcst_sms_req {
// Int, M
static const Json::StaticString key_bcst_act("bcst_act");  // sit_sms_bcst_act_e_type
}

namespace ns_sms_act_bcst_sms_rsp { }

/*
   SIT_GET_SMSC_ADDR (RCM ID = 0x0108)
 */
namespace ns_sms_get_smsc_addr_req { }

namespace ns_sms_get_smsc_addr_rsp {
// Int[], M
static const Json::StaticString key_service_center_addr("sca");
}

/*
   SIT_SET_SMSC_ADDR (RCM ID = 0x0109)
 */
namespace ns_sms_set_smsc_addr_req {
// Int[], M
static const Json::StaticString key_service_center_addr("sca");
}

namespace ns_sms_set_smsc_addr_rsp { }

/*
   SIT_SEND_SMS_MEM_STATUS (RCM ID = 0x010A)
 */
namespace ns_sms_send_sms_mem_status_req {
// Int, M
static const Json::StaticString key_mem_status("mem_status");
}

namespace ns_sms_send_sms_mem_status_rsp { }

/*
   SIT_SEND_ACK_INCOMING_SMS (RCM ID = 0x010B)
 */
namespace ns_sms_send_ack_incoming_sms_req {
// Int, M
static const Json::StaticString key_result("result");
// Int, M
static const Json::StaticString key_msg_tpid("msg_tpid");
// Int[], M
static const Json::StaticString key_tpdu("tpdu");
}

namespace sms_send_ack_incoming_sms_rsp { }

/*
   SIT_IND_NEW_SMS (RCM ID = 0x010C)
 */
namespace ns_sms_new_sms_ind {
// Int, M
static const Json::StaticString key_msg_tpid("msg_tpid");
// Int[], M
static const Json::StaticString key_tpdu("tpdu");
}

/*
   SIT_IND_NEW_SMS_STATUS_REPORT (RCM ID = 0x010D)
 */
// same as SIT_IND_NEW_SMS

/*
   SIT_IND_SIM_SMS_STORAGE_FULL (RCM ID = 0x010F)
 */
namespace ns_sms_sim_sms_storage_full_ind { }

/*
   SIT_IND_NEW_BCST_SMS (RCM ID = 0x0110)
 */
namespace ns_sms_new_bcst_sms_ind {
// Int[], M
static const Json::StaticString key_bcst_msg("bcst_msg");
}

/*
   SIT_CDMA_SEND_SMS (RCM ID = 0x0111)
 */
namespace ns_sms_cdma_send_sms_req {
// Int[], M
static const Json::StaticString key_msg_data("msg_data");
}

namespace ns_sms_cdma_send_sms_rsp {
// Int, M
static const Json::StaticString key_msg_ref("msg_ref");
// Int, M
static const Json::StaticString key_error_class("error_class");  // 0 : No Error, 1 : Temp Error, 2 : Perm Error
// Int, M
static const Json::StaticString key_error_code("error_code");
}

/*
   SIT_CDMA_SEND_SMS_EXPECT_MORE (RCM ID = 0x0122)
 */
// same as SIT_CDMA_SEND_SMS

/*
   SIT_CDMA_SEND_SMS_ACK (RCM ID = 0x0112)
 */
namespace ns_sms_cdma_send_sms_ack_req {
// Int, M
static const Json::StaticString key_msg_tpid("msg_tpid");
// Int, M
static const Json::StaticString key_error_class("error_class"); // 0 : No Error, 1 : Error
// Int, M
static const Json::StaticString key_error_code("error_code");
}

namespace ns_sms_cdma_send_sms_ack_rsp { }

/*
   SIT_CDMA_GET_BCST_SMS_CFG (RCM ID = 0x0113)
 */
namespace ns_sms_cdma_get_bcst_sms_cfg_req { }

namespace ns_sms_cdma_get_bcst_sms_cfg_rsp {
// Obj[], M
static const Json::StaticString key_cdma_bcst_conf_info_list("cdma_bcst_conf_info_list"); // see ns_sms_cdma_bcst_conf_info
}

namespace ns_sms_cdma_bcst_conf_info {
// Int, M
static const Json::StaticString key_service_category("service_category");
// Int, M
static const Json::StaticString key_language("language");
}

/*
   SIT_CDMA_SET_BCST_SMS_CFG (RCM ID = 0x0114)
 */
namespace ns_sms_cdma_set_bcst_sms_cfg_req {
// Obj[], M
static const Json::StaticString key_cdma_bcst_conf_info_list("cdma_bcst_conf_info_list"); // see ns_sms_cdma_bcst_conf_info
}

namespace ns_sms_cdma_set_bcst_sms_cfg_rsp { }

/*
   SIT_CDMA_ACT_BCST_SMS (RCM ID = 0x0115)
 */
namespace ns_sms_cdma_act_bcst_sms_req {
// Int, M
static const Json::StaticString key_cdma_bcst_act("cdma_bcst_act");  // see sit_sms_cdma_bcst_act_e_type
}

namespace ns_sms_cdma_act_bcst_sms_rsp { }

/*
   SIT_CDMA_WRITE_SMS_TO_RUIM (RCM ID = 0x0116)
 */
namespace ns_sms_cdma_write_sms_to_ruim_req {
// Int, M
static const Json::StaticString key_msg_status("msg_status");  // sit_sms_cdma_status_e_type
// Int[], M
static const Json::StaticString key_msg_data("msg_data");
}

namespace ns_sms_cdma_write_sms_to_ruim_rsp {
// Int, M
static const Json::StaticString key_index("index");
}

/*
   SIT_CDMA_DELETE_SMS_ON_RUIM (RCM ID = 0x0117)
 */
namespace ns_sms_cdma_delete_sms_on_ruim_req {
// Int, M
static const Json::StaticString key_index("index");
}

namespace ns_sms_cdma_delete_sms_on_ruim_rsp { }

/*
   SIT_IND_CDMA_NEW_SMS (RCM ID = 0x0118)
 */
namespace ns_sms_cdma_new_sms_ind {
// Int, M
static const Json::StaticString key_msg_tpid("msg_tpid");
// Int[], M
static const Json::StaticString key_msg_data("msg_data");
}

/*
   SIT_IND_CDMA_RUIM_SMS_STORAGE_FULL (RCM ID = 0x0119)
 */
namespace ns_sms_cdma_ruim_sms_storage_full_ind { }

/*
   SIT_IND_CDMA_VOICE_MSG_WAITING_INFO (RCM ID = 0x0120)
 */
namespace ns_sms_cdma_voice_msg_waiting_info_ind {
// Int, M
static const Json::StaticString key_msg_count("msg_count");
}

/*
   SIT_GET_STORED_SMS_COUNT (RCM ID = 0x0121)
 */
namespace ns_sms_get_stored_sms_count_req {
// Int, M
static const Json::StaticString key_sim_type("sim_type");
}

namespace ns_sms_get_stored_sms_count_rsp {
// Int, M
static const Json::StaticString key_sim_type("sim_type");
// Int, M
static const Json::StaticString key_total_num("total_num");
// Int, M
static const Json::StaticString key_used_num("used_num");
}

#endif /* _SMS_JSON_DEF_H_ */
