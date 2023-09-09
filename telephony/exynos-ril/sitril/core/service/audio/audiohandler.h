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
 * soundhandler.h
 *
 *  Created on: 2020. 7. 21.
 *      Author: sungwoo48.choi
 */

#ifndef __SOUND_HANDLER_H__
#define __SOUND_HANDLER_H__

#include "servicehandlerbase.h"

/**
 * SetVolumeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetVolumeHandler)

/**
 * GetVolumeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetVolumeHandler)

/**
 * SetAudioPathHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetAudioPathHandler)

/**
 * GetAudioPathHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetAudioPathHandler)

/**
 * SetMultiMicHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetMultiMicHandler)

/**
 * GetMultiMicHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetMultiMicHandler)

/**
 * SetAudioClockHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetAudioClockHandler)

/**
 * SetAudioLoopbackHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetAudioLoopbackHandler)

/**
 * SetMuteHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetMuteHandler)

/**
 * GetMuteHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetMuteHandler)

/**
 * SwitchVoiceCallAudioHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SwitchVoiceCallAudioHandler)

/**
 * UnsolResendInCallMuteHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolResendInCallMuteHandler)

#endif /* __SOUND_HANDLER_H__ */
