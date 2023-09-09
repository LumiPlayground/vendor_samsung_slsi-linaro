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
 * protocolsoundbuilder.h
 *
 *  Created on: 2021. 08. 2K
 *      Author: lnarayana.k
 */

#ifndef __PROTOCOL_SOUND_BUILDER_H__
#define __PROTOCOL_SOUND_BUILDER_H__

#include "protocolsoundbuilderinterface.h"

class ProtocolSoundBuilder : public ProtocolSoundBuilderInterface
{
private:
    ProtocolSoundBuilderInterface *m_pProtocolSoundBuilderInterface = NULL;

public:
    ProtocolSoundBuilder();
    ProtocolSoundBuilder(const ProtocolSoundBuilder&) = delete;
    ProtocolSoundBuilder& operator=(ProtocolSoundBuilder const&) = delete;
    virtual ~ProtocolSoundBuilder();
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
#endif /* __PROTOCOL_SOUND_BUILDER_H__ */
