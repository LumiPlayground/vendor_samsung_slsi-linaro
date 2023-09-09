/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _VSIM_JSON_DEF_H_
#define _VSIM_JSON_DEF_H_

#include "json/json.h"

/******************************************************************************
 *
 * File: vsimjsondef.h
 *
 * Description: This file will have key-value pair definition of all VSIM SIT commands
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
   SIT_VSIM_NOTIFICATION (RCM ID = 0x0E00)
 */
namespace ns_vsim_notification_req {
// Int, M
static const Json::StaticString key_tid("tid");
// Int, M
static const Json::StaticString key_event_type("event_type");  // sit_vsim_notification_e_type
// Int, M
static const Json::StaticString key_sim_type("sim_type");  // sit_vsim_sim_type_e_type
} // namespace ns_vsim_notification_req

namespace ns_vsim_notification_rsp { }

/*
   SIT_IND_VSIM_OPERATION (RCM ID = 0x0E01)
 */
namespace ns_vsim_opertaion_ind {
// Int, M
static const Json::StaticString key_tid("tid");
// Int, M
static const Json::StaticString key_event_type("event_type");  // sit_vsim_operation_e_type
// Int, M
static const Json::StaticString key_result("result");
// Int[], M
static const Json::StaticString key_operation_data("data");
} // namespace ns_vsim_opertaion_ind

/*
   SIT_VSIM_OPERATION (RCM ID = 0x0E02)
 */
namespace ns_vsim_opertaion_req {
// Int, M
static const Json::StaticString key_tid("tid");
// Int, M
static const Json::StaticString key_event_type("event_type");  // sit_vsim_operation_e_type
// Int, M
static const Json::StaticString key_result("result");
// Int[], M
static const Json::StaticString key_operation_data("data");
} // namespace ns_vsim_opertaion_req

namespace ns_vsim_opertaion_rsp { }

#endif /* _VSIM_JSON_DEF_H_ */
