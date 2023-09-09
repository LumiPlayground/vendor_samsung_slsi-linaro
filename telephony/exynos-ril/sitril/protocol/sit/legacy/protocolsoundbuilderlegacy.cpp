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
 * protocolsoundbuilderlegacy.cpp
 *
 *  Created on: 2014. 6. 28.
 *      Author: sungwoo48.choi
 */

#include "protocolsoundbuilderlegacy.h"
#include "modemdata.h"

/**
 * ProtocolSoundBuilderLegacy
 */
ModemData *ProtocolSoundBuilderLegacy::BuildGetMute()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_MUTE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildSetMute(int muteMode)
{
    sit_snd_set_mute_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_MUTE, length);
    req.mute_mode = (muteMode == 0 ? SIT_SND_MUTE_MODE_DISABLE : SIT_SND_MUTE_MODE_ENABLE);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildSetVolume(int volume)
{

    sit_snd_set_volume_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_SET_VOLUME, length);
    req.volume = volume;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildGetVolume(void)
{

    null_data_format req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_GET_VOLUME, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildSetAudioPath(int audiopath)
{

    sit_snd_set_audiopath_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_SET_AUDIO_PATH, length);
    req.audiopath = audiopath;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildGetAudioPath(void)
{

    null_data_format req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_GET_AUDIO_PATH, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildSetMultiMic(int mode)
{

    sit_snd_set_multimic_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_SET_MULTI_MIC, length);
    req.multiMICmode = mode;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildGetMultiMic(void)
{

    null_data_format req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_GET_MULTI_MIC, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildSetAudioClock(int mode)
{
    sit_snd_set_clock_mode_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_SET_AUDIO_CLK, length);
    req.clock_mode= mode;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildSetAudioLoopback(int onoff, int path)
{
    sit_snd_set_loopback_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_SET_AUDIO_LOOPBACK, length);
    req.on_off = onoff== 0 ? SITRIL_AUDIO_LOOPBACK_STOP : SITRIL_AUDIO_LOOPBACK_START;
    if ( (SITRIL_AUDIO_LOOPBACK_PATH_NA <= path
        && path <= SITRIL_AUDIO_LOOPBACK_PATH_LINEOUT)
        ||
        (SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSET <= path
        && path <= SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_LINEOUT)
        ||
        (SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC1 <= path
        && path <= SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC3) )
    {
        req.audio_path = path;
    }
    else
    {
        req.audio_path = SITRIL_AUDIO_LOOPBACK_PATH_HANDSET;    //default
    }

    return new ModemData((char*)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildSwitchVoiceCallAudio(BYTE siminfo)
{
    sit_snd_switch_voice_call_audio_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_SWITCH_VOICE_CALL_AUDIO, length);
    req.active_call_sim_info = siminfo;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildSetWbAmrCapability(int wbamr_capa)
{
    if ( wbamr_capa < 0
        || wbamr_capa >= SITRIL_AUDIO_WMAMR_MAX )
    {
        return NULL;
    }

    sit_snd_set_wbmar_capability_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_SET_WB_CAPABILITY, length);
    req.wbamr = wbamr_capa;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSoundBuilderLegacy::BuildGetWbAmrCapability(void)
{
    sit_snd_get_wbmar_capability_req req;
    int length = sizeof(req);

    InitRequestHeader(&req.hdr, SIT_GET_WB_CAPABILITY, length);

    return new ModemData((char *)&req, length);
}
