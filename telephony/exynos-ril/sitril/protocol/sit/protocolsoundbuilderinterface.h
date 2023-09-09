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
 * Protocolsoundbuilderinterface.h
 *
 *  Created on: 2021. 08. 17.
 *      Author: lnarayana.k
 */

#ifndef __PROTOCOL_SOUND_BUILDER_INTERFACE_H__
#define __PROTOCOL_SOUND_BUILDER_INTERFACE_H__

#include <base/types.h>

class ModemData;

class ProtocolSoundBuilderInterface
{
public:
    ProtocolSoundBuilderInterface() {}
    virtual ~ProtocolSoundBuilderInterface() {}

public:
    virtual ModemData *BuildGetMute() = 0;
    virtual ModemData *BuildSetMute(int muteMode) = 0;
    virtual ModemData *BuildSetVolume(int volume) = 0;
    virtual ModemData *BuildGetVolume() = 0;
    virtual ModemData *BuildSetAudioPath(int audiopath) = 0;
    virtual ModemData *BuildGetAudioPath() = 0;
    virtual ModemData *BuildSetMultiMic(int mode) = 0;
    virtual ModemData *BuildGetMultiMic() = 0;
    virtual ModemData *BuildSetAudioClock(int mode) = 0;
    virtual ModemData *BuildSetAudioLoopback(int onoff, int path) = 0;
    virtual ModemData *BuildSwitchVoiceCallAudio(BYTE siminfo) = 0;
    virtual ModemData *BuildSetWbAmrCapability(int wbamr_capa) = 0;
    virtual ModemData *BuildGetWbAmrCapability() = 0;
};

#endif /* __PROTOCOL_SOUND_BUILDER_INTERFACE_H__ */
