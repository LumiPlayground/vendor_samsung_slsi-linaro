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
 * protocolSoundBuilderjson.cpp
 *
 *  Created on: 2021. 8. 18.
 *      Author: lnarayana.k
 */

#include "protocolsoundbuilderjson.h"
#include "soundjsondef.h"
#include "rillog.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SOUND, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SOUND, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SOUND, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SOUND, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * ProtocolSoundBuilderJson
 */
ModemData *ProtocolSoundBuilderJson::BuildGetMute()
{
    return GetJsonNullReqModemData(SIT_GET_MUTE);
}

ModemData *ProtocolSoundBuilderJson::BuildSetMute(int muteMode)
{
    Json::Value jsonReqData; //key_set_mute
    jsonReqData[ns_snd_set_mute_req::key_mute_mode] = (muteMode == 0 ? SIT_SND_MUTE_MODE_DISABLE : SIT_SND_MUTE_MODE_ENABLE);
    return GetJsonReqModemData(jsonReqData, SIT_SET_MUTE);
}

ModemData *ProtocolSoundBuilderJson::BuildSetVolume(int volume)
{
    Json::Value jsonReqData; //key_volume_level
    jsonReqData[ns_snd_set_volume_req::key_volume_level] = volume;
    return GetJsonReqModemData(jsonReqData, SIT_SET_VOLUME);
}

ModemData *ProtocolSoundBuilderJson::BuildGetVolume()
{
    return GetJsonNullReqModemData(SIT_GET_VOLUME);
}

//unsed api. But kept as unimplemented to avoid linking lld errors for virtual api
ModemData *ProtocolSoundBuilderJson::BuildSwitchVoiceCallAudio(BYTE /* siminfo */) {
    return NULL;
}

ModemData *ProtocolSoundBuilderJson::BuildSetAudioPath(int audiopath)
{
    Json::Value jsonReqData; //key_audio_path
    jsonReqData[ns_snd_set_audio_path_req::key_audio_path] = audiopath;
    return GetJsonReqModemData(jsonReqData, SIT_SET_AUDIO_PATH);
}

ModemData *ProtocolSoundBuilderJson::BuildGetAudioPath()
{
    return GetJsonNullReqModemData(SIT_GET_AUDIO_PATH);
}

ModemData *ProtocolSoundBuilderJson::BuildSetMultiMic(int mode)
{
    Json::Value jsonReqData;
    jsonReqData[ns_snd_set_multi_mic_req::key_multi_mic_on_off] = mode;
    return GetJsonReqModemData(jsonReqData, SIT_SET_MULTI_MIC);
}

ModemData *ProtocolSoundBuilderJson::BuildGetMultiMic()
{
    return GetJsonNullReqModemData(SIT_GET_MULTI_MIC);
}

ModemData *ProtocolSoundBuilderJson::BuildSetAudioClock(int mode)
{
    Json::Value jsonReqData; //clock_mode
    jsonReqData[ns_snd_set_audio_clock_req::key_clock_mode] = mode;
    return GetJsonReqModemData(jsonReqData, SIT_SET_AUDIO_CLK);
}

ModemData *ProtocolSoundBuilderJson::BuildSetAudioLoopback(int on_off, int audio_path)
{
    Json::Value jsonReqData;

    jsonReqData[ns_snd_set_audio_loopback_req::key_on_off] =(on_off== 0 ? SITRIL_AUDIO_LOOPBACK_STOP : SITRIL_AUDIO_LOOPBACK_START);

    if ( (SITRIL_AUDIO_LOOPBACK_PATH_NA <= audio_path
        && audio_path <= SITRIL_AUDIO_LOOPBACK_PATH_LINEOUT)
        ||
        (SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSET <= audio_path
        && audio_path <= SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_LINEOUT)
        ||
        (SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC1 <= audio_path
        && audio_path <= SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC3) ) {
        jsonReqData[ns_snd_set_audio_loopback_req::key_audio_path]=audio_path;
    }
    else {
        jsonReqData[ns_snd_set_audio_loopback_req::key_audio_path] = SITRIL_AUDIO_LOOPBACK_PATH_HANDSET;    //default
    }
    return GetJsonReqModemData(jsonReqData, SIT_SET_AUDIO_LOOPBACK);
}

ModemData *ProtocolSoundBuilderJson::BuildSetWbAmrCapability(int wbamr_capa)
{
    Json::Value jsonReqData;

    if ( wbamr_capa < 0
        || wbamr_capa >= SITRIL_AUDIO_WMAMR_MAX ) {
        return NULL;
    }
    jsonReqData[ns_snd_set_wb_amr_3g_req::key_wbamr] =wbamr_capa ;
    return GetJsonReqModemData(jsonReqData, SIT_SET_WB_CAPABILITY);
}

ModemData *ProtocolSoundBuilderJson::BuildGetWbAmrCapability()
{
    return GetJsonNullReqModemData(SIT_GET_WB_CAPABILITY);
}
