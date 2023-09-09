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
 * protocolsoundadapterjson.cpp
 *
 *  Created on: 2021. 9. 13.
 *      Author: lnarayana.k
 */

#include "protocolsoundadapterjson.h"
#include "soundjsondef.h"
#include "rillog.h"
#include <string>
#include "sitdef.h"
#include "rildef.h"

/**
 * ProtocolSoundGetMuteRespAdapterJson
 */
ProtocolSoundGetMuteRespAdapterJson::ProtocolSoundGetMuteRespAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_GET_MUTE);
}

int ProtocolSoundGetMuteRespAdapterJson::GetMuteState() const
{
    int muteState =(sit_snd_mute_mode_e_type) SIT_SND_MUTE_MODE_DISABLE;
    if(m_bParsingSuccess){
       muteState= m_jsonData[ns_snd_get_mute_rsp::key_mute_mode].asInt();
       return muteState;
    }
    return muteState;
}

/**
 * ProtocolSoundRingbackToneIndAdapterJson
 */
ProtocolSoundRingbackToneIndAdapterJson::ProtocolSoundRingbackToneIndAdapterJson(const ModemData *pModemData) : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_RINGBACK_TONE);
}

static RingbackToneType ConvertSITToRingbackToneType(int type)
{
    //RilLogV("[CscService] %s",__FUNCTION__);
    switch (type)
    {
      case SIT_SND_RINGBACK_STATE_STOP:        /* Ringback Tone End */
        return RIL_SND_RINGBACK_TONE_END;
      case SIT_SND_RINGBACK_STATE_START:   /* Ringback Tone Start */
      default:
        return RIL_SND_RINGBACK_TONE_START;
    }
}

int ProtocolSoundRingbackToneIndAdapterJson::GetRingbackToneState() const
{
    int ringbacktoneState = RIL_SND_RINGBACK_TONE_END;
    if(m_bParsingSuccess)
    {
        ringbacktoneState = m_jsonData[ns_snd_ringback_tone_ind::key_ringback_state].asInt();
        ringbacktoneState = ConvertSITToRingbackToneType(ringbacktoneState);
    }
    return ringbacktoneState;
}

int ProtocolSoundRingbackToneIndAdapterJson::GetFlag() const
{
    int flag = -1;
    if(m_bParsingSuccess)
    {
        flag = m_jsonData[ns_snd_ringback_tone_ind::key_flag].asInt();
    }
    return flag;
}

/**
 * ProtocolSoundGetVolumeRespAdapterJson
 */
ProtocolSoundGetVolumeRespAdapterJson::ProtocolSoundGetVolumeRespAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_GET_VOLUME);
}

int ProtocolSoundGetVolumeRespAdapterJson::GetVolume() const
{
    if(m_bParsingSuccess)
    {
         return m_jsonData[ns_snd_get_volume_rsp::key_volume_level].asInt();
    }
    return -1;
}

/**
 * ProtocolSoundGetAudiopathRespAdapterJson
 */
ProtocolSoundGetAudiopathRespAdapterJson::ProtocolSoundGetAudiopathRespAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_GET_AUDIO_PATH);
}

int ProtocolSoundGetAudiopathRespAdapterJson::GetAudiopath() const
{
    if(m_bParsingSuccess){
       return m_jsonData[ns_snd_get_audio_path_rsp::key_audio_path].asInt();
    }
    return -1;
}

/**
 * ProtocolSoundGetMultiMICRespAdapterJson
 */
ProtocolSoundGetMultiMICRespAdapterJson::ProtocolSoundGetMultiMICRespAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_GET_MULTI_MIC);
}

int ProtocolSoundGetMultiMICRespAdapterJson::GetMultimicmode() const
{
    if(m_bParsingSuccess){
       return m_jsonData[ns_snd_get_multi_mic_rsp::key_multi_mic_on_off].asInt();
    }
    return -1;
}

/**
 * ProtocolSoundWBAMRReportAdapterJson
 */
ProtocolSoundWBAMRReportAdapterJson::ProtocolSoundWBAMRReportAdapterJson(const ModemData *pModemData) : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_WB_AMR_REPORT);
}

int ProtocolSoundWBAMRReportAdapterJson::GetStatus() const
{
    int wb_amr_status = -1;
    if(m_bParsingSuccess){
       wb_amr_status = m_jsonData[ns_snd_wb_amr_report_ind::key_status].asInt();
       return wb_amr_status & 0xFF ;
    }
    return wb_amr_status;
}

int ProtocolSoundWBAMRReportAdapterJson::GetCallType() const
{
    int calltype = AUDIO_CALL_TYPE_UNKNOWN;
    if(m_bParsingSuccess){
       calltype = m_jsonData[ns_snd_wb_amr_report_ind::key_rat].asInt();
       return calltype  & 0xFF ;
    }
    return calltype ;
}

/**
 * ProtocolSoundGetWBAMRCapabilityAdapterJson
 */
ProtocolSoundGetWBAMRCapabilityAdapterJson::ProtocolSoundGetWBAMRCapabilityAdapterJson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_GET_WB_CAPABILITY);
}

int ProtocolSoundGetWBAMRCapabilityAdapterJson::GetWbAmr() const
{
    int wbamr = -1;
    if(m_bParsingSuccess){
       wbamr = m_jsonData[ns_snd_set_wb_amr_3g_req::key_wbamr].asInt();
       return wbamr ;
    }
    return wbamr;
}
