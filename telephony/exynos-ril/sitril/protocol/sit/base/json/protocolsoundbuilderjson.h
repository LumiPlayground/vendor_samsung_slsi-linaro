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
 * protocolsoundbuilderjson.h
 *
 *  Created on: 2021. 08. 18.
 *      Author: lnarayana.k
 */

#ifndef __PROTOCOL_SOUND_BUILDER_JSON_H__
#define __PROTOCOL_SOUND_BUILDER_JSON_H__

#include "protocoljsonbuilder.h"
#include "protocolsoundbuilderinterface.h"

class ProtocolSoundBuilderJson : public ProtocolSoundBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolSoundBuilderJson()
        : ProtocolSoundBuilderInterface(), ProtocolJsonBuilder() {}
    virtual ~ProtocolSoundBuilderJson() {}

  public:
    ModemData *BuildGetMute();
    ModemData *BuildSetMute(int muteMode);
    ModemData *BuildSetVolume(int volume);
    ModemData *BuildGetVolume();
    ModemData *BuildSetAudioPath(int audio_path);
    ModemData *BuildGetAudioPath();
    ModemData *BuildSetMultiMic(int mode);
    ModemData *BuildGetMultiMic();
    ModemData *BuildSetAudioClock(int mode);
    ModemData *BuildSetAudioLoopback(int on_off, int audio_path);
    ModemData *BuildSwitchVoiceCallAudio(BYTE siminfo);
    ModemData *BuildSetWbAmrCapability(int wbamr_capa);
    ModemData *BuildGetWbAmrCapability();
};

#endif /* __PROTOCOL_SOUND_BUILDER_JSON_H__ */
