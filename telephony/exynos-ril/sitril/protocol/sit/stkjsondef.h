/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _STK_JSON_DEF_H_
#define _STK_JSON_DEF_H_

#include "json/json.h"

/******************************************************************************
 *
 * File: stkjsondef.h
 *
 * Description: This file will have key-value pair definition of all STK SIT commands
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
   SIT_SEND_STK_ENVELOPE_CMD (RCM ID = 0x0300)
 */
namespace ns_stk_send_stk_envelope_cmd_req {
// Int[], M
static const Json::StaticString key_envelope_cmd("envelope_cmd");
} // namespace ns_stk_send_stk_envelope_cmd_req

namespace ns_stk_send_stk_envelope_cmd_rsp {
// Int[], M
static const Json::StaticString key_envelope_rsp("envelope_rsp");
} // namespace ns_stk_send_stk_envelope_cmd_rsp

/*
   SIT_SEND_STK_TERMINAL_RSP (RCM ID = 0x0301)
 */
namespace ns_stk_send_stk_terminal_rsp_req {
// Int[], M
static const Json::StaticString key_terminal_rsp("terminal_rsp");
} // namespace ns_stk_send_stk_terminal_rsp_req

// response is not used by service/handler
// hence no adapter implementation
namespace ns_stk_send_stk_terminal_rsp_rsp {
// Int, M
static const Json::StaticString key_sw1("sw1");
// Int, M
static const Json::StaticString key_sw2("sw2");
} // namespace ns_stk_send_stk_terminal_rsp_rsp

/*
    SIT_SEND_STK_ENVELOPE_WITH_STATUS (RCM ID = 0x0302)
*/
namespace ns_stk_send_stk_envelope_with_status_req {
// Int[], M
static const Json::StaticString key_envelope_cmd("envelope_cmd");
} // namespace ns_stk_send_stk_envelope_with_status_req

namespace ns_stk_send_stk_envelope_with_status_rsp {
// Int, M
static const Json::StaticString key_sw1("sw1");
// Int, M
static const Json::StaticString key_sw2("sw2");
// Int[], M
static const Json::StaticString key_envelope_rsp("envelope_rsp");
} // namespace ns_stk_send_stk_envelope_with_status_rsp

/*
   SIT_IND_STK_PROACTIVE_COMMAND (RCM ID = 0x0303)
 */
namespace ns_stk_proactive_cmd_ind {
// Int[], M
static const Json::StaticString key_proactive_cmd("proactive_cmd");
}  // namespace ns_stk_proactive_cmd_ind

/*
    SIT_IND_SIM_REFRESH (RCM ID = 0x0304)
*/
namespace ns_stk_sim_refresh_ind {
// Int, M
static const Json::StaticString key_refresh_result("result");
} // namespace ns_stk_sim_refresh_ind

/*
    SIT_STK_CALL_SETUP (RCM ID = 0x0305)
*/
namespace ns_stk_call_setup_req {
// Int, M
static const Json::StaticString key_user_operation("user_operation");
} // namespace ns_stk_call_setup_req

namespace ns_stk_call_setup_rsp { }

/*
   SIT_IND_STK_SESSION_END (RCM ID = 0x0306)
 */
namespace ns_stk_session_end_ind { }

/******************************************************************************
 * SS
 *****************************************************************************/
/*
   SIT_IND_SS_RETURN_RESULT (RCM ID = 0x0513)
 */
namespace ns_stk_ss_return_result_ind {
// Int[], M
static const Json::StaticString key_ret_result_list("ret_result_list");
}

#endif /* _STK_JSON_DEF_H_ */
