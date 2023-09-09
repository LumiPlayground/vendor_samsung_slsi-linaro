/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _EMBMS_JSON_DEF_H_
#define _EMBMS_JSON_DEF_H_

#include "json/json.h"

/******************************************************************************
 *
 * File: embmsjsondef.h
 *
 * Description: This file will have key-value pair definition of all eMBMS SIT commands
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
 *******************************************************************************/

/*
   SIT_SET_EMBMS_SERVICE (RCM ID = 0x1000)
 */
namespace ns_embms_set_service_req {
// Int, M
static const Json::StaticString key_state("state");  // 0 : disable, 1 : enable
}

namespace ns_embms_set_service_rsp { }

/*
   SIT_SET_EMBMS_SESSION (RCM ID = 0x1001)
 */
namespace ns_embms_set_session_req {
// Int, M
static const Json::StaticString key_state("state");  // 0 : deactivate, 1 : activate. 2 : activate/deactivate
// Int[], M
static const Json::StaticString key_tmgi("tmgi");
// Int[], C
static const Json::StaticString key_deact_tmgi("deact_tmgi");  // sent only when state = 0x2
// Int[], M
static const Json::StaticString key_sai_list("sai_list");
// UInt[], M
static const Json::StaticString key_freq_list("freq_list");
}

namespace ns_embms_set_session_rsp { }

/*
   SIT_IND_EMBMS_COVERAGE (RCM ID = 0x1002)
 */
namespace ns_embms_coverage_ind {
// Int, M
static const Json::StaticString key_coverage("coverage");  // sit_embms_coverage_e_type
}

/*
   SIT_GET_EMBMS_SESSION_LIST (RCM ID = 0x1003)
 */
namespace ns_embms_session_list_req {
// Int, M
static const Json::StaticString key_state("state");
}

namespace ns_embms_session_list_rsp {
// Int, M
static const Json::StaticString key_state("state");
// Int, M
static const Json::StaticString key_oos_reason("oos_reason");
// Int[], M
static const Json::StaticString key_tmgi_list("tmgi_list");
}

/*
   SIT_IND_EMBMS_SESSION_LIST (RCM ID = 0x1004)
 */
namespace ns_embms_session_list_ind {
// Int, M
static const Json::StaticString key_state("state");
// Int, M
static const Json::StaticString key_oos_reason("oos_reason");
// Int[], M
static const Json::StaticString key_tmgi_list("tmgi_list");
}

/*
   SIT_GET_EMBMS_SIGNAL_STRENGTH (RCM ID = 0x1005)
 */
namespace ns_embms_signal_strength_req { }

namespace ns_embms_signal_strength_rsp {
// Obj[], M
static const Json::StaticString key_signal_record_list("signal_record_list");  // ns_embms_signal_record
}

namespace ns_embms_signal_record {
// UInt, M
static const Json::StaticString key_snr("snr");
// Int, M
static const Json::StaticString key_mbsfn_area_id("mbsfn_area_id");
// UInt, M
static const Json::StaticString key_esnr("esnr");
// Int[], M
static const Json::StaticString key_tmgi_list("tmgi_list");
}

/*
   SIT_IND_EMBMS_SIGNAL_STRENGTH (RCM ID = 0x1006)
 */
namespace ns_embms_signal_strength_ind = ns_embms_signal_strength_rsp;

/*
   SIT_GET_EMBMS_NETWORK_TIME (RCM ID = 0x1007)
 */
namespace ns_embms_network_time_req { }

namespace ns_embms_network_time_rsp {
// Int, M
static const Json::StaticString key_sib16_acquired("sib16_acquired");
// Int, M
static const Json::StaticString key_time_info_type("time_info_type");
// Int, M
static const Json::StaticString key_daylight_valid("daylight_valid");
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
static const Json::StaticString key_daylight_adjust("daylight_adjust");
// Int, M
static const Json::StaticString key_day_of_week("day_of_week");
// String, M
static const Json::StaticString key_plmn("plmn");
// ULong, M
static const Json::StaticString key_absolute_time("absolute_time");
// Int, M
static const Json::StaticString key_leap_second("leap_second");
}

/*
   SIT_IND_EMBMS_NETWORK_TIME (RCM ID = 0x1008)
 */
namespace ns_embms_network_time_ind = ns_embms_network_time_rsp;

/*
   SIT_IND_EMBMS_SAI_LIST (RCM ID = 0x1009)
 */
namespace ns_embms_sailist_ind {
// Int[], M
static const Json::StaticString key_intra_sai_list("intra_sai_list");
// Obj[], M
static const Json::StaticString key_inter_sai_list("inter_sai_list");  // ns_embms_inter_sai
}

namespace ns_embms_inter_sai {
// UInt, M
static const Json::StaticString key_frequency("frequency");
// Int[], M
static const Json::StaticString key_inter_sai_info_list("inter_sai_info_list");
// Int[], M
static const Json::StaticString key_multiband_info("multiband_info");
}

/*
   SIT_IND_EMBMS_GLOBAL_CELL_ID (RCM ID = 0x100A)
 */
namespace ns_embms_global_cellid_ind {
// String, M
static const Json::StaticString key_mcc("mcc");
// String, M
static const Json::StaticString key_mnc("mnc");
// UInt, M
static const Json::StaticString key_cell_id("cell_id");
}

#endif  /* _EMBMS_JSON_DEF_H_ */
