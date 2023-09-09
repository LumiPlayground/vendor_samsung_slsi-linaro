/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _SOUND_JSON_DEF_H_
#define _SOUND_JSON_DEF_H_

#include "json/json.h"

/******************************************************************************
 *
 * File: soundjsondef.h
 *
 * Description: This file will have key-value pair definition of all Sound SIT commands
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
   SIT_SET_MUTE (RCM ID = 0x0A00)
*/
namespace ns_snd_set_mute_req {
// Int M
static const Json::StaticString key_mute_mode("mute_mode");
}//ns_snd_set_mute_req

/*
   SIT_GET_MUTE (RCM ID = 0x0A01)
*/
namespace ns_snd_get_mute_req {}

namespace ns_snd_get_mute_rsp {
// Int M
static const Json::StaticString key_mute_mode("mute_mode");
}//ns_snd_get_mute_rsp

/*
   SIT_IND_RINGBACK_TONE (RCM ID = 0x0A02)
*/
namespace ns_snd_ringback_tone_ind {
// Int M
static const Json::StaticString key_ringback_state("ringback_state"); //sit_snd_ringback_state_e_type
// Int M
static const Json::StaticString key_flag("flag"); //sit_snd_ringback_tone_flag_e_type
}//ns_snd_ringback_tone_ind

/*
   SIT_SET_VOLUME (RCM ID = 0x0A03)
*/
namespace ns_snd_set_volume_req {
// Int M
static const Json::StaticString key_volume_level("volume_level");

}//ns_snd_set_volume_req

/*
   SIT_GET_VOLUME (RCM ID = 0x0A04)
*/
namespace ns_snd_get_volume_req {}

namespace ns_snd_get_volume_rsp {
// Int M
static const Json::StaticString key_volume_level("volume_level");

}//ns_snd_get_volume_rsp

/*
   SIT_SET_AUDIO_PATH (RCM ID = 0x0A05)
*/
namespace ns_snd_set_audio_path_req {
// Int M
static const Json::StaticString key_audio_path("audio_path"); //sit_snd_audiopath_e_mode
}//ns_snd_set_audio_path_req

/*
   SIT_GET_AUDIO_PATH (RCM ID = 0x0A06)
*/
namespace ns_snd_get_audio_path_req {}

namespace ns_snd_get_audio_path_rsp {
// Int M
static const Json::StaticString key_audio_path("audio_path"); //sit_snd_audiopath_e_mode
}//ns_snd_get_audio_path_rsp

/*
   SIT_SET_MULTI_MIC (RCM ID = 0x0A07)
 */
namespace ns_snd_set_multi_mic_req {
// Int M
static const Json::StaticString key_multi_mic_on_off("multi_mic_on_off");
}// namesapce ns_snd_set_multi_mic_req

/*
   SIT_GET_MULTI_MIC (RCM ID = 0x0A08)
 */
namespace ns_snd_get_multi_mic_req {}

namespace ns_snd_get_multi_mic_rsp {
// Int M
static const Json::StaticString key_multi_mic_on_off("multi_mic_on_off");
}// namesapce ns_snd_get_multi_mic_rsp

//SIT_SWITCH_VOICE_CALL_AUDIO (RCM ID = 0x0A09) - Not used. So implementation not required

/*
   SIT_SET_AUDIO_CLK (RCM ID = 0x0A0A)
 */
namespace ns_snd_set_audio_clock_req {
// Int M
static const Json::StaticString key_clock_mode("clock_mode");
}// namesapce ns_snd_set_audio_clock_req

/*
   SIT_SET_AUDIO_LOOPBACK (RCM ID = 0x0A0B)
 */
 namespace ns_snd_set_audio_loopback_req {
// Int M
static const Json::StaticString key_on_off("on_off");
// Int M
static const Json::StaticString key_audio_path("audio_path"); //sit_snd_audiopath_e_mode
}// namesapce ns_snd_set_audio_clock_req

/*
   SIT_IND_WB_AMR_REPORT (RCM ID = 0x0A0C)
 */
namespace ns_snd_wb_amr_report_ind {
// Int M
static const Json::StaticString key_status("status"); //sit_snd_wb_amr_report_status_e_type
// Int M
static const Json::StaticString key_rat("rat");  //sit_snd_wb_amr_report_rat_e_type
}// namesapce ns_snd_wb_amr_report_ind

/*
   SIT_SET_WB_CAPABILITY_3G (RCM ID = 0x0A0D)
 */
namespace ns_snd_set_wb_amr_3g_req {
// Int M
static const Json::StaticString key_wbamr("wbamr"); //sit_snd_wbamr_capability_e_type
}// namesapce ns_snd_wb_amr_3g_req

/*
   SIT_GET_WB_CAPABILITY (RCM ID = 0x0A0E)
 */
namespace ns_snd_get_wb_capability_req {}

namespace ns_snd_get_wb_capability_rsp {
// Int M
static const Json::StaticString key_wbamr("wbamr");
}// namesapce ns_snd_wb_amr_3g_rsp

// SIT_IND_RESEND_IN_CALL_MUTE (RCM ID = 0x0A0F) - Not used

// SIT_SET_CALL_MODE_INFO (RCM ID = 0x0A10) - Not used

// SIT_IND_CALL_MODE_INFO (RCM ID = 0x0A11) - Not used

#endif /* _SOUND_JSON_DEF_H_ */
