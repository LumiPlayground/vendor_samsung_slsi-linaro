/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * audiohandlerfactory.cpp
 *
 *  Created on: 2020. 7. 21.
 *      Author: sungwoo48.choi
 */

#include "audiohandler.h"
#include "audiohandlerfactory.h"
#include "messageid.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(AudioHandlerFactory)

BEGIN_FACTORY_ENTRY(AudioHandlerFactory)
    ADD_FACTORY_ENTRY(SetVolumeHandler, MSG_AUDIO_SET_VOLUME)
    ADD_FACTORY_ENTRY(GetVolumeHandler, MSG_AUDIO_GET_VOLUME)
    ADD_FACTORY_ENTRY(SetAudioPathHandler, MSG_AUDIO_SET_AUDIOPATH)
    ADD_FACTORY_ENTRY(GetAudioPathHandler, MSG_AUDIO_GET_AUDIOPATH)
    ADD_FACTORY_ENTRY(SetMultiMicHandler, MSG_AUDIO_SET_MULTIMIC)
    ADD_FACTORY_ENTRY(GetMultiMicHandler, MSG_AUDIO_GET_MULTIMIC)
    ADD_FACTORY_ENTRY(SetAudioClockHandler, MSG_AUDIO_SET_AUDIO_CLOCK)
    ADD_FACTORY_ENTRY(SetAudioLoopbackHandler, MSG_AUDIO_SET_AUDIO_LOOPBACK)

    ADD_FACTORY_ENTRY(SetMuteHandler, MSG_CS_SOUND_SET_MUTE)
    ADD_FACTORY_ENTRY(GetMuteHandler, MSG_CS_SOUND_GET_MUTE)
    ADD_FACTORY_ENTRY(SwitchVoiceCallAudioHandler, MSG_CS_SOUND_SWITCH_VOICE_CALL)
    ADD_FACTORY_ENTRY(UnsolResendInCallMuteHandler, MSG_CS_IND_RESEND_IN_CALL_MUTE)
END_FACTORY_ENTRY()
