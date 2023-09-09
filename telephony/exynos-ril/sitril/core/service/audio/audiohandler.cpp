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
 * soundhandler.cpp
 *
 *  Created on: 2020. 7. 21.
 *      Author: sungwoo48.choi
 */
#include "audiohandler.h"
#include "protocoladapter.h"
#include "protocolsoundbuilder.h"
#include "protocolsoundadapter.h"
#include "rillog.h"
#include "service.h"

#define TIMEOUT_SOUND_DEFAULT   5000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * SetVolumeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetVolumeHandler, MSG_AUDIO_SET_VOLUME)

int SetVolumeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rilData = (IntRequestData*) msg->GetRequestData();
    INT32 volume = rilData->GetInt();

    if (volume == -1) {
        RilLogV("%s::%s() volume(%d) is changed to minimum(0)", mServiceName, __FUNCTION__, volume);
        volume = 0;
    }

    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildSetVolume(volume);
    RilLogV("[%s] %s <req> volume: %d", mServiceName, __FUNCTION__, volume);
    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetVolumeHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetVolumeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetVolumeHandler, MSG_AUDIO_GET_VOLUME)

int GetVolumeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildGetVolume();

    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetVolumeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolSoundGetVolumeRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int volume = adapter.GetVolume();

    RilLogV("[%s] %s <resp> volume: %d", mServiceName, __FUNCTION__, volume);

    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, &volume, sizeof(volume));
    }
    else {
        RilLogE("%s::%s() !! ERROR !!, errorCode(0x%x) in GetErrorCode()", mServiceName, __FUNCTION__, errorCode);
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * SetAudioPathHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetAudioPathHandler, MSG_AUDIO_SET_AUDIOPATH)

int SetAudioPathHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rilData = (IntRequestData*) msg->GetRequestData();
    INT32 audiopath = rilData->GetInt();

    RilLogV("%s::%s() audio path (%d)", mServiceName, __FUNCTION__, audiopath);

    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildSetAudioPath(audiopath);
    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetAudioPathHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetAudioPathHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetAudioPathHandler, MSG_AUDIO_GET_AUDIOPATH)

int GetAudioPathHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildGetAudioPath();

    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetAudioPathHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolSoundGetAudiopathRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int audiopath = adapter.GetAudiopath();

    RilLogV("[%s] %s <resp> audio path: %d", mServiceName, __FUNCTION__, audiopath);

    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, &audiopath, sizeof(audiopath));
    }
    else {
        RilLogE("%s::%s() !! ERROR !!, errorCode(0x%x) in GetErrorCode()", mServiceName, __FUNCTION__, errorCode);
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * SetMultiMicHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetMultiMicHandler, MSG_AUDIO_SET_MULTIMIC)

int SetMultiMicHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rilData = (IntRequestData*) msg->GetRequestData();
    INT32 multiMICmode = rilData->GetInt();

    RilLogV("%s::%s() multi mic mode (%d)", mServiceName, __FUNCTION__, multiMICmode);

    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildSetMultiMic(multiMICmode);
    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetMultiMicHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetMultiMicHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetMultiMicHandler, MSG_AUDIO_GET_MULTIMIC)

int GetMultiMicHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildGetMultiMic();

    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetMultiMicHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolSoundGetMultiMICRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int multiMICmode = adapter.GetMultimicmode();

    RilLogV("[%s] %s <resp> multi mic code: %d", mServiceName, __FUNCTION__, multiMICmode);

    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, &multiMICmode, sizeof(multiMICmode));
    }
    else {
        RilLogE("%s::%s() !! ERROR !!, errorCode(0x%x) in GetErrorCode()", mServiceName, __FUNCTION__, errorCode);
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * SetAudioClockHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetAudioClockHandler, MSG_AUDIO_SET_AUDIO_CLOCK)

int SetAudioClockHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rilData = (IntRequestData*) msg->GetRequestData();
    INT32 clockmode = rilData->GetInt();

    RilLogV("%s::%s() clock mode (%d)", mServiceName, __FUNCTION__, clockmode);

    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildSetAudioClock(clockmode);
    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetAudioClockHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetAudioLoopbackHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetAudioLoopbackHandler, MSG_AUDIO_SET_AUDIO_LOOPBACK)

int SetAudioLoopbackHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rilData = (IntsRequestData *) msg->GetRequestData();
    INT32 onoff = rilData->GetInt(0);
    INT32 path = rilData->GetInt(1);

    RilLogV("%s::%s() onoff(%d), path(%d)", mServiceName, __FUNCTION__, onoff, path);

    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildSetAudioLoopback(onoff, path);
    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetAudioLoopbackHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetMuteHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetMuteHandler, MSG_CS_SOUND_SET_MUTE)

int SetMuteHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *pMuteReqData = (IntRequestData*) msg->GetRequestData();
    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildSetMute(pMuteReqData->GetInt());
    RilLogV("[%s] %s <req> Mute state: %s", mServiceName, __FUNCTION__, pMuteReqData->GetInt() == RIL_MUTE_STATUS_UNMUTE ? "Unmute" : "Mute");
    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetMuteHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetMuteHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetMuteHandler, MSG_CS_SOUND_GET_MUTE)

int GetMuteHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildGetMute();
    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetMuteHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolSoundGetMuteRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int mute_state = adapter.GetMuteState();

    RilLogV("[%s] %s <resp> Mute state: %s", mServiceName, __FUNCTION__,
            mute_state == RIL_MUTE_STATUS_UNMUTE ? "Unmute" : "Mute");

    int cardState = mRilContext->GetUiccCardState();
    if(cardState != RIL_CARDSTATE_PRESENT) {
        RilLogI("[%s] %s mCardState(%d) is not Present(%d)", mServiceName, __FUNCTION__, cardState, RIL_CARDSTATE_PRESENT);
        // Ignore Error, this shall succeed always on SIM ABSENT
        errorCode = RIL_E_SUCCESS;
    }

    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, &mute_state, sizeof(mute_state));
    }
    else {
        RilLogI("[%s] unexpected %s mCardState(%d) is not Present, errorCode=%d", mServiceName, __FUNCTION__, cardState, errorCode);
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * SwitchVoiceCallAudioHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SwitchVoiceCallAudioHandler, MSG_CS_SOUND_SWITCH_VOICE_CALL)

int SwitchVoiceCallAudioHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData* pMuteReqData = (IntRequestData *) msg->GetRequestData();
    ProtocolSoundBuilder builder;
    ModemData *pModemData = builder.BuildSwitchVoiceCallAudio((BYTE)(pMuteReqData->GetInt()));
    RilLogV("[%s] %s <req> SIM #: %d", mServiceName, __FUNCTION__, pMuteReqData->GetInt());
    if (SendRequest(pModemData, TIMEOUT_SOUND_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SwitchVoiceCallAudioHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * UnsolResendInCallMuteHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolResendInCallMuteHandler, MSG_CS_IND_RESEND_IN_CALL_MUTE)

int UnsolResendInCallMuteHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    OnUnsolicitedResponse(RIL_UNSOL_RESEND_INCALL_MUTE);
    return 0;
}
