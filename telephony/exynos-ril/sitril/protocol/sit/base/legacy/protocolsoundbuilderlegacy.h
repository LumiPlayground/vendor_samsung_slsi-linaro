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
 * protocolsoundbuilderlegacy.h
 *
 *  Created on: 2014. 12. 2.
 *      Author: sungwoo48.choi
 */

#ifndef __PROTOCOL_SOUND_BUILDER_LEGACY_H__
#define __PROTOCOL_SOUND_BUILDER_LEGACY_H__

#include "protocolbuilder.h"
#include "protocolsoundbuilderinterface.h"

class ProtocolSoundBuilderLegacy : public ProtocolSoundBuilderInterface, public ProtocolBuilder {
  public:
    ProtocolSoundBuilderLegacy()
        : ProtocolSoundBuilderInterface(), ProtocolBuilder() {}
    virtual ~ProtocolSoundBuilderLegacy() {}

  public:
    ModemData *BuildGetMute();
    ModemData *BuildSetMute(int muteMode);
    ModemData *BuildSetVolume(int volume);
    ModemData *BuildGetVolume();
    ModemData *BuildSetAudioPath(int audiopath);
    ModemData *BuildGetAudioPath();
    ModemData *BuildSetMultiMic(int mode);
    ModemData *BuildGetMultiMic();
    ModemData *BuildSetAudioClock(int mode);
    ModemData *BuildSetAudioLoopback(int onoff, int path);
    ModemData *BuildSwitchVoiceCallAudio(BYTE siminfo);
    ModemData *BuildSetWbAmrCapability(int wbamr_capa);
    ModemData *BuildGetWbAmrCapability();
};

#endif /* __PROTOCOL_SOUND_BUILDER_LEGACY_H__ */
