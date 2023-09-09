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
 * protocolsoundbuilder.cpp
 *
 *  Created on: 2021. 8. 11.
 *      Author: lnarayana.k
 */

#include "protocolsoundbuilder.h"
#include "legacy/protocolsoundbuilderlegacy.h"
#include "json/protocolsoundbuilderjson.h"

/**
 * ProtocolSoundBuilder
 */
ProtocolSoundBuilder::ProtocolSoundBuilder()
    : ProtocolSoundBuilderInterface() {
    if (IsPayloadTypeJson()) {
        m_pProtocolSoundBuilderInterface = new ProtocolSoundBuilderJson();
    } else {
        m_pProtocolSoundBuilderInterface = new ProtocolSoundBuilderLegacy();
    }
}

ProtocolSoundBuilder::~ProtocolSoundBuilder() {
    if (m_pProtocolSoundBuilderInterface != NULL)
        delete m_pProtocolSoundBuilderInterface;
    m_pProtocolSoundBuilderInterface = NULL;
}

ModemData *ProtocolSoundBuilder::BuildGetMute() {
    return m_pProtocolSoundBuilderInterface->BuildGetMute();
}

ModemData *ProtocolSoundBuilder::BuildSetMute(int muteMode) {
    return m_pProtocolSoundBuilderInterface->BuildSetMute(muteMode);
}

ModemData *ProtocolSoundBuilder::BuildSetVolume(int volume) {
    return m_pProtocolSoundBuilderInterface->BuildSetVolume(volume);
}

ModemData *ProtocolSoundBuilder::BuildGetVolume() {
    return m_pProtocolSoundBuilderInterface->BuildGetVolume();
}

ModemData *ProtocolSoundBuilder::BuildSetAudioPath(int audiopath) {
    return m_pProtocolSoundBuilderInterface->BuildSetAudioPath(audiopath);
}

ModemData *ProtocolSoundBuilder::BuildGetAudioPath() {
    return m_pProtocolSoundBuilderInterface->BuildGetAudioPath();
}

ModemData *ProtocolSoundBuilder::BuildSetMultiMic(int mode) {
    return m_pProtocolSoundBuilderInterface->BuildSetMultiMic(mode);
}

ModemData *ProtocolSoundBuilder::BuildGetMultiMic() {
    return m_pProtocolSoundBuilderInterface->BuildGetMultiMic();
}

ModemData *ProtocolSoundBuilder::BuildSetAudioClock(int mode) {
    return m_pProtocolSoundBuilderInterface->BuildSetAudioClock(mode);
}

ModemData *ProtocolSoundBuilder::BuildSetAudioLoopback(int onoff, int path) {
    return m_pProtocolSoundBuilderInterface->BuildSetAudioLoopback(onoff, path);
}

ModemData *ProtocolSoundBuilder::BuildSwitchVoiceCallAudio(BYTE siminfo) {
    return m_pProtocolSoundBuilderInterface->BuildSwitchVoiceCallAudio(siminfo);
}

ModemData *ProtocolSoundBuilder::BuildSetWbAmrCapability(int wbamr_capa) {
    return m_pProtocolSoundBuilderInterface->BuildSetWbAmrCapability(wbamr_capa);
}

ModemData *ProtocolSoundBuilder::BuildGetWbAmrCapability() {
    return m_pProtocolSoundBuilderInterface->BuildGetWbAmrCapability();
}
