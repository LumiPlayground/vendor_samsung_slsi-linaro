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
 * protocolcalladapterlegacy.cpp
 *
 *  Created on: 2014. 6. 27.
 *      Author: jhdaniel.kim
 */


#include "protocolcalladapterlegacy.h"
#include "callprotocolutils.h"
#include "callreqdata.h"
#include "rillog.h"
#include "sitdef.h"
#include "modemdata.h"
#include "util.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

//[[maybe_unused]] const int RET_VALUE_SAME_AS_SIT_COMMAND = -99;

/**
 * ProtocolGetCurrentCallAdapterLegacy
 */
int ProtocolGetCurrentCallAdapterLegacy::GetCallNum()
{
    sit_call_get_current_calls_rsp_v1_1 *rsp = (sit_call_get_current_calls_rsp_v1_1 *)m_pModemData->GetRawData();
    if ( rsp != NULL )
    {
        return rsp->number;
    }

    return 0;
}

bool ProtocolGetCurrentCallAdapterLegacy::HasValidLength()
{
    sit_call_get_current_calls_rsp_v1_1 *rsp = (sit_call_get_current_calls_rsp_v1_1 *)m_pModemData->GetRawData();
    int packet_len = rsp->hdr.length;
    const int CALL_LIST_MIN_LEN = sizeof(RCM_HEADER)+sizeof(rsp->number);
    const int ONE_RECORD_LEN_V1_1 = (sizeof(sit_call_info_type_v1_1) * rsp->number) + CALL_LIST_MIN_LEN;
    const int ONE_RECORD_LEN_V1_2 = (sizeof(sit_call_info_type_v1_2) * rsp->number) + CALL_LIST_MIN_LEN;
    const int ONE_RECORD_LEN_V1_6 = (sizeof(sit_call_info_type_v1_6) * rsp->number) + CALL_LIST_MIN_LEN;

    if ( packet_len < CALL_LIST_MIN_LEN ) {
        RilLogE("ProtocolGetCurrentCallAdapterLegacy::HasValidLength() : packet length(%d) is less than min value(%d)", packet_len, CALL_LIST_MIN_LEN);
        return false;
    }

    if ( packet_len == ONE_RECORD_LEN_V1_1 || packet_len == ONE_RECORD_LEN_V1_2 ||
         packet_len == ONE_RECORD_LEN_V1_6 ) {
        return true;
    }
    RilLogE("ProtocolGetCurrentCallAdapterLegacy::HasValidLength() : packet length(%d) is invalid."
            " Expected length(%d or %d or %d or %d)",
            packet_len, CALL_LIST_MIN_LEN, ONE_RECORD_LEN_V1_1, ONE_RECORD_LEN_V1_2, ONE_RECORD_LEN_V1_6);
    return false;
}

int ProtocolGetCurrentCallAdapterLegacy::ConvertSitToUusInfo(UusInfo* pUusInfo, int uusType, int uus_dcs, int len, BYTE* pData)
{
    if ( pUusInfo != NULL )
    {
        pUusInfo->m_uusType = (RIL_UUS_Type)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_UUS_TYPE, uusType);
        pUusInfo->m_uusDcs = (RIL_UUS_DCS)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_UUS_DCS, uus_dcs);
        pUusInfo->m_uusLength = MIN(len, MAX_UUS_DATA_LEN);
        if ( pUusInfo->m_uusLength > 0 && pData != NULL )
        {
            memcpy(pUusInfo->m_uusData, pData, pUusInfo->m_uusLength);
        }
        return 0;
    }
    return -1;
}

template<typename T>
int ProtocolGetCurrentCallAdapterLegacy::SetCallInfo(CallInfo* (&pCallInfo), int /* index */, T* (&pcallinfo)) {
    pCallInfo->m_state = (RIL_CallState)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_STATE, pcallinfo->state);
    pCallInfo->m_nIndex = pcallinfo->index;
    RilLogV("pCallInfo->m_nIndex : %d", pCallInfo->m_nIndex);
    pCallInfo->m_toa = (pcallinfo->type_of_address == RIL_TOA_INTERNATIONAL) ? RIL_TOA_INTERNATIONAL : RIL_TOA_UNKNOWN;
    pCallInfo->m_isMParty = (pcallinfo->is_mpty == SIT_CALL_IS_MPTY_MULTIPARTY)?true:false;
    pCallInfo->m_isMt = (pcallinfo->is_mt == SIT_CALL_IS_MT_MT);

    if( pcallinfo->call_type <= SIT_CALL_CALL_TYPE_VOICE
            || pcallinfo->call_type == SIT_CALL_CALL_TYPE_EMERGENCY
            || pcallinfo->call_type == SIT_CALL_CALL_TYPE_VOLTE
            || pcallinfo->call_type == SIT_CALL_CALL_TYPE_CDMA_VOICE
            || pcallinfo->call_type == SIT_CALL_CALL_TYPE_CDMA_EMERGENCY )
    {
        pCallInfo->m_isVoice = true;
    }
    else
    {
        pCallInfo->m_isVoice = false;
    }
    pCallInfo->m_isVideo = (pcallinfo->call_type == SIT_CALL_CALL_TYPE_VIDEO)? true: false;
    if (pCallInfo->m_isVoice != true && pCallInfo->m_isVideo != true)
    {
            return -3;
    }
    pCallInfo->m_isVoicePrivacy = (pcallinfo->is_voice_privacy == SIT_CALL_IS_VOICE_PRIVACY_INACTIVATED) ? RIL_CALL_CDMA_VOICEPRIVACY_INACTIVE : RIL_CALL_CDMA_VOICEPRIVACY_ACTIVE;
    pCallInfo->m_numPresent = (CallPresentation)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_PRESENTATION,pcallinfo->number_presentation);
    if (pCallInfo->m_numPresent != RIL_CALL_NAME_PRESENTATION_ALLOW
            && pCallInfo->m_numPresent != RIL_CALL_NAME_PRESENTATION_UNKNOWN)
    {
        pcallinfo->num_len = 0;
        memset(pCallInfo->m_number, 0, sizeof(pCallInfo->m_number));
    }
    pCallInfo->m_namePresent = (CallPresentation)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_PRESENTATION,pcallinfo->name_presentation);
    if (pCallInfo->m_namePresent != RIL_CALL_NAME_PRESENTATION_ALLOW
            && pCallInfo->m_namePresent != RIL_CALL_NAME_PRESENTATION_UNKNOWN)
    {
        pcallinfo->name_len = 0;
        memset(pCallInfo->m_name, 0, sizeof(pCallInfo->m_name));
    }
    pCallInfo->m_als = pcallinfo->als;
    if (pcallinfo->num_len > 0)
    {
        memset(pCallInfo->m_number, 0, sizeof(pCallInfo->m_number));
        int len = MIN(pcallinfo->num_len, MAX_DIAL_NUM);
        memcpy(pCallInfo->m_number, pcallinfo->num, len);
        pCallInfo->m_numPresent = RIL_CALL_NAME_PRESENTATION_ALLOW;
    }
    if (pcallinfo->name_len > (unsigned int) 0)
    {
        int decodedLen;
        char decodedName[MAX_DIAL_NAME * 2];
        memset(pCallInfo->m_name, 0, sizeof(pCallInfo->m_name));
        decodedLen = DecodingUssd(pcallinfo->name_dcs, pcallinfo->name, MIN(pcallinfo->name_len, MAX_DIAL_NAME), (unsigned char*)decodedName, sizeof(decodedName));
        RilLogV("[%s] decoded name : %s [dcs:%x, len:%d]", __FUNCTION__, decodedName, pcallinfo->name_dcs, decodedLen);
        int len = MIN(decodedLen, MAX_DIAL_NAME);
        memcpy(pCallInfo->m_name, decodedName, len);

        pCallInfo->m_namePresent = RIL_CALL_NAME_PRESENTATION_ALLOW;
    }

    pCallInfo->m_audioQuality = UNSPECIFIED;
    memset(pCallInfo->m_forwardedNumber, 0, sizeof(pCallInfo->m_forwardedNumber));

    ConvertSitToUusInfo(&(pCallInfo->m_uusInfo), pcallinfo->uus_type, pcallinfo->uus_dcs, pcallinfo->uus_data_len, pcallinfo->uus_data);

    return 0;
}

int ProtocolGetCurrentCallAdapterLegacy::GetCallInfoV1_1(CallInfo* pCallInfo, int index) {
    sit_call_get_current_calls_rsp_v1_1 *rsp = (sit_call_get_current_calls_rsp_v1_1 *)m_pModemData->GetRawData();

    if ( pCallInfo == NULL ) {
        return -1;
    } else if ( index < 0 || index >= MAX_CALL_LIST_NUM || index >= rsp->number ) {
        return -2;
    }

    sit_call_info_type_v1_1* pcallinfo = &(rsp->record[index]);
    int ret = SetCallInfo<sit_call_info_type_v1_1>(pCallInfo, index, pcallinfo);
    if (ret < 0) return ret;

    pCallInfo->m_audioQuality = UNSPECIFIED;

    return 0;
}

int ProtocolGetCurrentCallAdapterLegacy::GetCallInfoV1_2(CallInfo* pCallInfo, int index) {
    sit_call_get_current_calls_rsp_v1_2 *rsp = (sit_call_get_current_calls_rsp_v1_2 *)m_pModemData->GetRawData();

    if ( pCallInfo == NULL ) {
        return -1;
    } else if ( index < 0 || index >= MAX_CALL_LIST_NUM || index >= rsp->number ) {
        return -2;
    }

    sit_call_info_type_v1_2* pcallinfo = &(rsp->record[index]);
    int ret = SetCallInfo<sit_call_info_type_v1_2>(pCallInfo, index, pcallinfo);
    if (ret < 0) return ret;

    pCallInfo->m_audioQuality = (RIL_AudioQuality) pcallinfo->audioQuality;
    memset(pCallInfo->m_forwardedNumber, 0, sizeof(pCallInfo->m_forwardedNumber));

    return 0;
}

int ProtocolGetCurrentCallAdapterLegacy::GetCallInfoV1_6(CallInfo* pCallInfo, int index)
{
    sit_call_get_current_calls_rsp_v1_6 *rsp = (sit_call_get_current_calls_rsp_v1_6 *)m_pModemData->GetRawData();

    if ( pCallInfo == NULL )
    {
        return -1;
    }
    else if ( index < 0 || index >= MAX_CALL_LIST_NUM || index >= rsp->number )
    {
        return -2;
    }

    sit_call_info_type_v1_6* pcallinfo = &(rsp->record[index]);
    int ret = SetCallInfo<sit_call_info_type_v1_6>(pCallInfo, index, pcallinfo);
    if (ret < 0)
        return ret;

    pCallInfo->m_audioQuality = (RIL_AudioQuality) pcallinfo->audioQuality;

    if (pcallinfo->forwarded_num_len > 0) {
        memset(pCallInfo->m_forwardedNumber, 0, sizeof(pCallInfo->m_forwardedNumber));
        int len = MIN(pcallinfo->forwarded_num_len, MAX_SS_NUM_LEN);
        memcpy(pCallInfo->m_forwardedNumber, pcallinfo->forwarded_num, len);
    }

    return 0;
}

int ProtocolGetCurrentCallAdapterLegacy::GetCallInfo(CallInfo* pCallInfo, int index)
{
    sit_call_get_current_calls_rsp_v1_1 *rsp = (sit_call_get_current_calls_rsp_v1_1 *)m_pModemData->GetRawData();
    int packet_len = rsp->hdr.length;
    const int CALL_LIST_MIN_LEN = sizeof(RCM_HEADER)+sizeof(rsp->number);
    const int ONE_RECORD_LEN_V1_1 = (sizeof(sit_call_info_type_v1_1) * rsp->number) + CALL_LIST_MIN_LEN;
    const int ONE_RECORD_LEN_V1_2 = (sizeof(sit_call_info_type_v1_2) * rsp->number) + CALL_LIST_MIN_LEN;

    if (packet_len == ONE_RECORD_LEN_V1_1) {
        GetCallInfoV1_1(pCallInfo, index);
    } else if (packet_len == ONE_RECORD_LEN_V1_2) {
        GetCallInfoV1_2(pCallInfo, index);
    } else {
        GetCallInfoV1_6(pCallInfo, index);
    }
    return 0;
}

void ProtocolGetCurrentCallAdapterLegacy::DebugPrintCallInfo(CallInfo* pCallInfo)
{
    if ( pCallInfo == NULL )
    {
         return;
    }

    RilLogV("---------------------------\n");
    RilLogV("state : %d\n", pCallInfo->m_state);
    RilLogV("Index : %d\n", pCallInfo->m_nIndex);
    RilLogV("toa : %d\n", pCallInfo->m_toa);
    RilLogV("isMpty : %d\n", pCallInfo->m_isMParty);
    RilLogV("isMt : %d\n", pCallInfo->m_isMt);
    RilLogV("isVoice : %d\n", pCallInfo->m_isVoice);
    RilLogV("isVideo : %d\n", pCallInfo->m_isVideo);
    RilLogV("isVoicePrivacy : %d\n", pCallInfo->m_isVoicePrivacy);
    RilLogV("numPresent : %d\n", pCallInfo->m_numPresent);
    RilLogV("namePresent : %d\n", pCallInfo->m_namePresent);
    RilLogV("als : %d\n", pCallInfo->m_als);
    RilLogV("number : %s\n", pCallInfo->m_number);
    RilLogV("name : %s\n", pCallInfo->m_name);
    RilLogV("uusInfo(type:%d,dcs:%d,datalen:%d,[data])\n", pCallInfo->m_uusInfo.m_uusType, pCallInfo->m_uusInfo.m_uusDcs, pCallInfo->m_uusInfo.m_uusLength);
    RilLogV("audioQuality : %d\n", pCallInfo->m_audioQuality);
    RilLogV("forwarded_number : %s\n", pCallInfo->m_forwardedNumber);
    RilLogV("---------------------------\n");
}

/**
 * ProtocolGetLastCallFailCauseAdapterLegacy
 */
int ProtocolGetLastCallFailCauseAdapterLegacy::GetLastCallFailCause()
{
    sit_call_get_last_call_fail_cause_rsp *rsp = (sit_call_get_last_call_fail_cause_rsp *)m_pModemData->GetRawData();

    if ( rsp == NULL || (GetLength() < sizeof(sit_call_get_last_call_fail_cause_rsp))) {
        RilLogV("invalid LastCallFailCause payload : return error_unspecified(%d)", CALL_FAIL_ERROR_UNSPECIFIED);
        return CALL_FAIL_ERROR_UNSPECIFIED;
    }

    return CallProtocolUtils::getRilLastCallFailCause(rsp->last_call_fail_cause);
}

/**
 * ProtocolGetClipAdapterLegacy
 */
int ProtocolGetClipAdapterLegacy::GetClipStatus()
{
    sit_ss_get_clip_rsp *rsp = (sit_ss_get_clip_rsp*)m_pModemData->GetRawData();

    return CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_GET_CLIP_STATE, rsp->status);
}

/**
 * ProtocolGetClirAdapterLegacy
 */
int ProtocolGetClirAdapterLegacy::GetClirStatus()
{
    sit_ss_get_clir_rsp *rsp = (sit_ss_get_clir_rsp*)m_pModemData->GetRawData();

    return rsp->clir_status;    // same value defined in TS 27.007 7.7
}

/**
 * ProtocolGetCallForwardingStatusAdapterLegacy
 */
int ProtocolGetCallForwardingStatusAdapterLegacy::GetCfNum()
{
    sit_ss_get_call_forward_status_rsp *rsp = (sit_ss_get_call_forward_status_rsp *)m_pModemData->GetRawData();
    if ( rsp != NULL )
    {
        return rsp->call_forward_num;
    }

    return 0;
}

bool ProtocolGetCallForwardingStatusAdapterLegacy::HasValidLength()
{
    sit_ss_get_call_forward_status_rsp *rsp = (sit_ss_get_call_forward_status_rsp *)m_pModemData->GetRawData();
    int packet_len = rsp->hdr.length;
    const int PACKET_MIN_LEN = sizeof(RCM_HEADER)+sizeof(rsp->call_forward_num);
    const int ONE_RECORD_LEN = sizeof(sit_ss_call_forward_item);

    if ( packet_len < PACKET_MIN_LEN )
    {
        RilLogE("ProtocolGetCallForwardingStatusAdapterLegacy::HasValidLength() : packet length(%d) is less than min value(%d)", packet_len, PACKET_MIN_LEN);
        return false;
    }
    else if ( (packet_len - PACKET_MIN_LEN) % ONE_RECORD_LEN != 0 )
    {
        RilLogE("ProtocolGetCallForwardingStatusAdapterLegacy::HasValidLength() : packet length(%d) is invalid", packet_len);
        return false;
    }
    return true;
}


int ProtocolGetCallForwardingStatusAdapterLegacy::GetCfInfo(RIL_CallForwardInfo* pCfInfo, int index)
{
    sit_ss_get_call_forward_status_rsp *rsp = (sit_ss_get_call_forward_status_rsp *)m_pModemData->GetRawData();

    if ( pCfInfo == NULL )
    {
        return -1;
    }
    else if ( index < 0 || index >= MAX_CALL_FORWARD_STATUS_NUM || index >= rsp->call_forward_num )
    {
        return -2;
    }

    char tNumber[MAX_SS_NUM_LEN];

    sit_ss_call_forward_item* p_cfinfo = &(rsp->record[index]);

    pCfInfo->status = CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_SERVICE_CLASS_CF_GET, p_cfinfo->status);
    pCfInfo->reason = p_cfinfo->reason;
    pCfInfo->serviceClass = p_cfinfo->service_class;
    pCfInfo->toa = p_cfinfo->toa;
    // TOA 0x91 means international number, add "+" to number.
    if (pCfInfo->toa == 0x91 && p_cfinfo->num_len != 0) {
        // +2 is for "+" and terminating null character.
        int min = (p_cfinfo->num_len + 2) > MAX_SS_NUM_LEN ? MAX_SS_NUM_LEN : (p_cfinfo->num_len + 2);
        snprintf(tNumber, min, "+%s", (char*)p_cfinfo->number);
        strncpy((char*)p_cfinfo->number, tNumber, min);
    }
    pCfInfo->number = (char*)p_cfinfo->number;
    pCfInfo->timeSeconds = p_cfinfo->timeseconds;

    return 0;
}

void ProtocolGetCallForwardingStatusAdapterLegacy::DebugPrintCfInfo(RIL_CallForwardInfo* pCfInfo)
{
    if ( pCfInfo == NULL )
    {
         return;
    }

    RilLogV("---------------------------\n");
    RilLogV("state : %d\n", pCfInfo->status);
    RilLogV("reason : %d\n", pCfInfo->reason);
    RilLogV("serviceClass : %d\n", pCfInfo->serviceClass);
    RilLogV("toa : %d\n", pCfInfo->toa);
    RilLogV("number : %s\n", pCfInfo->number);
    RilLogV("timeSeconds : %d\n", pCfInfo->timeSeconds);
    RilLogV("---------------------------\n");
}

/**
 * ProtocolGetCallWaitingAdapterLegacy
 */
int ProtocolGetCallWaitingAdapterLegacy::GetServiceStatus()
{
    sit_ss_get_call_waiting_rsp *rsp = (sit_ss_get_call_waiting_rsp*)m_pModemData->GetRawData();

    return CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_SERVICE_STATUS_CALL_WAITING, rsp->status);
}

int ProtocolGetCallWaitingAdapterLegacy::GetServiceClass()
{
    sit_ss_get_call_waiting_rsp *rsp = (sit_ss_get_call_waiting_rsp*)m_pModemData->GetRawData();

    return rsp->service_class;
}

/**
 * ProtocolUssdIndAdapterLegacy
 */
int ProtocolUssdIndAdapterLegacy::GetDecodedUssd(char* decodedUssd, size_t buf_size, int& dcs)
{
    sit_ss_ussd_ind *rsp = (sit_ss_ussd_ind*)m_pModemData->GetRawData();

    RilLogV("[%s] received ussd : %s(%d)", __FUNCTION__, rsp->ussd, rsp->ussd_len);
    if ( decodedUssd != NULL )
    {
        int decodedLen = DecodingUssd(rsp->dcs, rsp->ussd, rsp->ussd_len, (unsigned char*)decodedUssd, buf_size);
        dcs = rsp->dcs;
        RilLogV("[%s] decoded ussd : %s(%d) by dcs(%x)", __FUNCTION__, decodedUssd, decodedLen, dcs);
        return decodedLen;
        //return DecodeUssd(rsp->dcp, rsp->ussd_len, rsp->ussd, decodedUssd);
    }
    return 0;
}

int ProtocolUssdIndAdapterLegacy::GetUssdStatus()
{
    sit_ss_ussd_ind *rsp = (sit_ss_ussd_ind*)m_pModemData->GetRawData();

    return CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_USSD_STATUS, rsp->status);
}

/**
 * ProtocolSsSvcIndAdapterLegacy
 */
int ProtocolSsSvcIndAdapterLegacy::GetNotificationType()
{
    sit_ss_supp_svc_notification_ind *rsp = (sit_ss_supp_svc_notification_ind*)m_pModemData->GetRawData();

    return CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_SSNOTI_TYPE, rsp->noti_type);
}

int ProtocolSsSvcIndAdapterLegacy::GetCode()
{
    sit_ss_supp_svc_notification_ind *rsp = (sit_ss_supp_svc_notification_ind*)m_pModemData->GetRawData();
    return rsp->code;
}

int ProtocolSsSvcIndAdapterLegacy::GetCugIndex()
{
    sit_ss_supp_svc_notification_ind *rsp = (sit_ss_supp_svc_notification_ind*)m_pModemData->GetRawData();
    return rsp->index;
}

UINT ProtocolSsSvcIndAdapterLegacy::GetSSType()
{
    sit_ss_supp_svc_notification_ind *rsp = (sit_ss_supp_svc_notification_ind*)m_pModemData->GetRawData();

    /* 0,  16, 32, 48, 64, (80, 96, 112)*/

/*
Type of number (octet 3) (Note 1)
Bits
7 6 5
0 0 0 unknown (Note 2)
0 0 1 international number (Note 3, Note 5)
0 1 0 national number (Note 3)
0 1 1 network specific number (Note 4)
1 0 0 dedicated access, short code
1 0 1 reserved
1 1 0 reserved
1 1 1 reserved for extension
*/
    return rsp->type;
}

int ProtocolSsSvcIndAdapterLegacy::GetNumberLength()
{
    sit_ss_supp_svc_notification_ind *rsp = (sit_ss_supp_svc_notification_ind*)m_pModemData->GetRawData();
    return rsp->num_len;
}

char* ProtocolSsSvcIndAdapterLegacy::GetNumber()
{
    sit_ss_supp_svc_notification_ind *rsp = (sit_ss_supp_svc_notification_ind*)m_pModemData->GetRawData();

    return (char*)rsp->num;
}

/**
 * ProtocolGetColpAdapterLegacy
 */
int ProtocolGetColpAdapterLegacy::GetColpStatus()
{
    sit_ss_get_colp_rsp *rsp = (sit_ss_get_colp_rsp*)m_pModemData->GetRawData();

    return rsp->status;
}

/**
 * ProtocolGetColrAdapterLegacy
 */
int ProtocolGetColrAdapterLegacy::GetColrStatus()
{
    sit_ss_get_colr_rsp *rsp = (sit_ss_get_colr_rsp*)m_pModemData->GetRawData();

    return rsp->status;
}

/**
 * ProtocolSendCallConfirmRespAdapterLegacy
 */
int ProtocolSendCallConfirmRespAdapterLegacy::GetResult() const
{
    sit_call_send_call_confirm_rsp *data = (sit_call_send_call_confirm_rsp *)m_pModemData->GetRawData();

    return data->result;
}

/**
 * ProtocolEmergencyCallListIndAdapterLegacy
 */
int ProtocolEmergencyCallListIndAdapterLegacy::GetVersion() const {
    return mVersion;
}

/**
 * ProtocolEmergencyCallListIndAdapterLegacy
 */
const char *ProtocolEmergencyCallListIndAdapterLegacy::GetMcc() const {
    return mMcc;
}

/**
 * ProtocolEmergencyCallListIndAdapterLegacy
 */
const char *ProtocolEmergencyCallListIndAdapterLegacy::GetMnc() const {
    return mMnc;
}

/**
 * ProtocolEmergencyCallListIndAdapterLegacy
 */
int ProtocolEmergencyCallListIndAdapterLegacy::GetNum() const {
    return mNum;
}

/**
 * ProtocolEmergencyCallListIndAdapterLegacy
 */
void ProtocolEmergencyCallListIndAdapterLegacy::Init() {
    mVersion = 0;
    mNum = 0;
    memset(mMcc, 0, sizeof(mMcc));
    memset(mMnc, 0, sizeof(mMnc));

    sit_call_emergency_call_list_ind_v1_4 *dataV1_4 = (sit_call_emergency_call_list_ind_v1_4 *)
                                                           m_pModemData->GetRawData();
    sit_call_emergency_call_list_ind_v1_6 *dataV1_6 = (sit_call_emergency_call_list_ind_v1_6 *)
                                                           m_pModemData->GetRawData();

    if (dataV1_4 == NULL || dataV1_4->hdr.id != SIT_IND_EMERGENCY_CALL_LIST) {
        return;
    }

    int packetLen = dataV1_4->hdr.length;
    const int EMERGENCY_LIST_LEN_V1_1 = sizeof(sit_call_emergency_call_list_ind_v1_1);
    const int EMERGENCY_LIST_LEN_V1_4 = sizeof(sit_call_emergency_call_list_ind_v1_4);
    const int EMERGENCY_LIST_MIN_LEN_V1_6 = sizeof(RCM_IND_HEADER) + sizeof(dataV1_6->mcc) +
                                            sizeof(dataV1_6->mnc) + sizeof(dataV1_6->enlNum) +
                                            (sizeof(sit_call_emergency_call_number_info_v1_4) * dataV1_6->enlNum) +
                                            sizeof(dataV1_6->eenl.eenlv) + sizeof(dataV1_6->eenl.eenlNum);

    if (packetLen == EMERGENCY_LIST_LEN_V1_1) {
        mVersion = 1;
        mNum = (int)(dataV1_4->num);
    } else if (packetLen == EMERGENCY_LIST_LEN_V1_4) {
        mVersion = 4;
        mNum = (int)(dataV1_4->num);
    } else if (packetLen >= EMERGENCY_LIST_MIN_LEN_V1_6) {
        mVersion = 6;
        mNum = GetNumV1_6();
    } else {
        RilLogI("[%s] invalid data length. packetLen:%d, expected len: %d or %d or great than %d",
                __FUNCTION__, packetLen, EMERGENCY_LIST_LEN_V1_1,
                EMERGENCY_LIST_LEN_V1_4, EMERGENCY_LIST_MIN_LEN_V1_6);
        return;
    }

    memcpy(mMcc, dataV1_4->mcc, MAX_MCC_LEN);
    memcpy(mMnc, dataV1_4->mnc, MAX_MCC_LEN);
    if (mMnc[MAX_MCC_LEN - 1] == '#') {
        mMnc[MAX_MCC_LEN - 1] = 0;
    }
}

/**
 * ProtocolEmergencyCallListIndAdapterLegacy
 */
int ProtocolEmergencyCallListIndAdapterLegacy::GetNumV1_6() {
    sit_call_emergency_call_list_ind_v1_6 *dataV1_6 = (sit_call_emergency_call_list_ind_v1_6 *)
                                                           m_pModemData->GetRawData();
    int len = sizeof(RCM_IND_HEADER) + sizeof(dataV1_6->mcc) +
              sizeof(dataV1_6->mnc) + sizeof(dataV1_6->enlNum);
    len += (sizeof(sit_call_emergency_call_number_info_v1_4) * dataV1_6->enlNum);
    sit_call_emergency_call_eenl *eenl = (sit_call_emergency_call_eenl *)((char *)dataV1_6 + len);
    int eenlNum = (int)eenl->eenlNum;
    RilLogV("[%s] enlNum:%d, eenlNum:%d", __FUNCTION__, dataV1_6->enlNum, eenlNum);
    return ((int)dataV1_6->enlNum + eenlNum);
}

/**
 * ProtocolEmergencyCallListIndAdapterLegacy
 */
void ProtocolEmergencyCallListIndAdapterLegacy::GetEccInfo(EccInfo *eccInfo) {
    RilLogV("[%s] Enter version:%d, num:%d", __FUNCTION__, mVersion, mNum);
    sit_call_emergency_call_list_ind_v1_6 *data = (sit_call_emergency_call_list_ind_v1_6 *)
                                                      m_pModemData->GetRawData();
    if (data == NULL || data->hdr.id != SIT_IND_EMERGENCY_CALL_LIST) {
        RilLogI("[%s] invalid data", __FUNCTION__);
        return;
    }

    int idx = 0;
    if (mVersion == 6) {
        sit_call_emergency_call_list_ind_v1_6 *dataV1_6 = (sit_call_emergency_call_list_ind_v1_6 *)
                                                               m_pModemData->GetRawData();
        int enlNum = MIN((int)dataV1_6->enlNum, MAX_ENL_RECORD_NUM);
        while (idx < enlNum) {
            sit_call_emergency_call_number_info_v1_4 *enlRecord = &(dataV1_6->enl_records[idx++]);
            if (NULL == enlRecord) {
                break;
            }

            RilLogV("[%s] Add ENL Record: category=%d, number_len=%d, number=%s, source=%d",
                    __FUNCTION__, (int)enlRecord->category, (int)enlRecord->emc_number_len,
                    (char *)enlRecord->emc_number, (int)enlRecord->source);
            eccInfo[SIT_ECC_INFO_TYPE_ENL].AddEmergencyNumber(mMcc, mMnc,
                                                              (char *)enlRecord->emc_number,
                                                              (int)enlRecord->emc_number_len,
                                                              (int)enlRecord->category,
                                                              SIT_EMERGENCY_CONDITION_ALWAYS,
                                                              (int)enlRecord->source);
            eccInfo[SIT_ECC_INFO_TYPE_EENL].AddEmergencyNumber(mMcc, mMnc,
                                                               (char *)enlRecord->emc_number,
                                                               (int)enlRecord->emc_number_len,
                                                               (int)enlRecord->category,
                                                               SIT_EMERGENCY_CONDITION_ALWAYS,
                                                               (int)enlRecord->source);
        }
        int len = sizeof(RCM_IND_HEADER) + sizeof(dataV1_6->mcc) +
                  sizeof(dataV1_6->mnc) + sizeof(dataV1_6->enlNum);
        len += (sizeof(sit_call_emergency_call_number_info_v1_4) * dataV1_6->enlNum);
        sit_call_emergency_call_eenl *eenl = (sit_call_emergency_call_eenl *)((char *)dataV1_6 + len);
        int eenlNum = MIN((int)eenl->eenlNum, MAX_EENL_RECORD_NUM);
        int eenlIdx = 0;
        int eenlOffset = 0;
        while (eenlIdx < eenlNum) {
            sit_call_emergency_call_eenl_record eenlRecord;
            eenlRecord.emc_number_len = MIN(eenl->eenl_records[eenlOffset], MAX_EENL_EMERGENCY_NUMBER_LEN);
            eenlOffset += 1;
            strncpy((char *)eenlRecord.emc_number,
                    (char *)&eenl->eenl_records[eenlOffset],
                    eenlRecord.emc_number_len);
            eenlOffset += eenlRecord.emc_number_len;
            eenlRecord.sub_service_len = MIN(eenl->eenl_records[eenlOffset], MAX_SUB_SERVICE_LEN);
            eenlOffset += 1;
            strncpy((char *)eenlRecord.sub_service,
                    (char *)&eenl->eenl_records[eenlOffset],
                    eenlRecord.sub_service_len);
            eenlOffset += eenlRecord.sub_service_len;

            if ((int)eenlRecord.emc_number_len == 0) {
                RilLogE("[%s] invalid EENL number length.", __FUNCTION__);
                eenlIdx++;
                mNum--;  // reduce total num
                continue;
            }

            char *emcNumber = (char *)eenlRecord.emc_number;
            int emcNumberLen = (int)eenlRecord.emc_number_len;
            char *subService = (char *)eenlRecord.sub_service;
            int subServiceLen = (int)eenlRecord.sub_service_len;

            int foundIdx = findEmergencyNumber(emcNumberLen, emcNumber, &eccInfo[SIT_ECC_INFO_TYPE_EENL]);
            if (foundIdx >= 0) {
                RilLogV("[%s] Update ENL Record with EENL Record: number_len=%d, number=%s,"
                        " sub_service_len=%d, sub_service=%s",
                        __FUNCTION__, emcNumberLen, emcNumber, subServiceLen, subService);
                eccInfo[SIT_ECC_INFO_TYPE_EENL].GetEmergencyNumber(foundIdx)->AddUrn(subService, subServiceLen);
                mNum--;  // reduce total num
            } else {
                RilLogV("[%s] Add EENL Record: number_len=%d, number=%s,"
                        " sub_service_len=%d, sub_service=%s",
                        __FUNCTION__, emcNumberLen, emcNumber, subServiceLen, subService);
                eccInfo[SIT_ECC_INFO_TYPE_EENL].AddEmergencyNumber(mMcc, mMnc,
                                                                   emcNumber, emcNumberLen,
                                                                   subService, subServiceLen,
                                                                   0 /*N/A*/,
                                                                   SIT_EMERGENCY_CONDITION_ALWAYS,
                                                                   SIT_CALL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING);
            }
            eenlIdx++;
        }
        eccInfo[SIT_ECC_INFO_TYPE_ENL].SetCount(enlNum);
        eccInfo[SIT_ECC_INFO_TYPE_EENL].SetCount(mNum);
        eccInfo[SIT_ECC_INFO_TYPE_EENL].SetEenlv(eenl->eenlv);
        eccInfo[SIT_ECC_INFO_TYPE_EENL].SetMcc(mMcc);
        eccInfo[SIT_ECC_INFO_TYPE_EENL].SetMnc(mMnc, strlen(mMnc));
        RilLogI("[%s] eenlv:%d, plmn:%s",
                __FUNCTION__, eccInfo[SIT_ECC_INFO_TYPE_EENL].GetEenlv(),
                eccInfo[SIT_ECC_INFO_TYPE_EENL].GetPlmn());
    } else if (mVersion == 4) {
        sit_call_emergency_call_list_ind_v1_4 *dataV1_4 = (sit_call_emergency_call_list_ind_v1_4 *)
                                                          m_pModemData->GetRawData();
        while (idx < mNum) {
            sit_call_emergency_call_number_info_v1_4 *numInfo = &(dataV1_4->number_info[idx++]);
            if (NULL == numInfo) {
                break;
            }

            RilLogV("[%s] EmcInfo: category=%d, number_len=%d, number=%s, source=%d", __FUNCTION__,
                    (int)numInfo->category, (int)numInfo->emc_number_len,
                    (char *)numInfo->emc_number, (int)numInfo->source);
            eccInfo[SIT_ECC_INFO_TYPE_ENL].AddEmergencyNumber(mMcc, mMnc,
                                                              (char *)numInfo->emc_number,
                                                              (int)numInfo->emc_number_len,
                                                              (int)numInfo->category,
                                                              SIT_EMERGENCY_CONDITION_ALWAYS,
                                                              (int)numInfo->source);
        }
        eccInfo[SIT_ECC_INFO_TYPE_ENL].SetCount(mNum);
    } else {  // assume version 1
        sit_call_emergency_call_list_ind_v1_1 *dataV1_1 = (sit_call_emergency_call_list_ind_v1_1 *)
                                                          m_pModemData->GetRawData();
        while (idx < mNum) {
            sit_call_emergency_call_number_info_v1_1 *numInfo = &(dataV1_1->number_info[idx++]);
            if (NULL == numInfo) {
                break;
            }

            RilLogV("[%s] EmcInfo: category=%d, number_len=%d, number=%s", __FUNCTION__,
                    (int)numInfo->category, (int)numInfo->emc_number_len, (char *)numInfo->emc_number);
            eccInfo[SIT_ECC_INFO_TYPE_ENL].AddEmergencyNumber(mMcc, mMnc,
                                                              (char *)numInfo->emc_number,
                                                              (int)numInfo->emc_number_len,
                                                              (int)numInfo->category,
                                                              SIT_EMERGENCY_CONDITION_ALWAYS,
                                                              RIL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING);
        }
        eccInfo[SIT_ECC_INFO_TYPE_ENL].SetCount(mNum);
    }
}

int ProtocolEmergencyCallListIndAdapterLegacy::findEmergencyNumber(int numLen, const char *targetNumber,
                                                                   EccInfo *eccInfo) {
    if (NULL == eccInfo || numLen == 0) return -1;

    for (int i = 0; i < eccInfo->GetCount(); i++) {
        if (NULL != eccInfo->GetEmergencyNumber(i) &&
            strncmp(eccInfo->GetEmergencyNumber(i)->GetNumber(), targetNumber, numLen) == 0) {
            return i;
        }
    }
    return -1;
}
/**
 * ProtocolGetPreferredVoicePrivacyModeAdapterLegacy
 */
int ProtocolGetPreferredVoicePrivacyModeAdapterLegacy::GetPreferredVoicePrivacyMode()
{
    sit_call_get_preferred_voice_privacy_mode_rsp *rsp = (sit_call_get_preferred_voice_privacy_mode_rsp*)m_pModemData->GetRawData();

    return rsp->status;
}

/**
 * ProtocolCdmaCallWaitingIndAdapterLegacy
 */
int ProtocolCdmaCallWaitingIndAdapterLegacy::GetCwInfo(RIL_CDMA_CallWaiting_v6 *pCwInfo)
{
    sit_ss_cdma_call_waiting_ind *data = (sit_ss_cdma_call_waiting_ind *)m_pModemData->GetRawData();

    if ( data == NULL )
    {
        return -1;
    }

    if ( pCwInfo == NULL )
    {
        return -1;
    }

    pCwInfo->numberPresentation = (CallPresentation)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_PRESENTATION,
            data->number_presentation);
    if (pCwInfo->numberPresentation != RIL_CALL_NAME_PRESENTATION_ALLOW)
    {
        data->num_len = 0;
    }

    if (data->num_len > 0 )
    {
        pCwInfo->number = (char*)data->number;
        pCwInfo->numberPresentation = RIL_CALL_NAME_PRESENTATION_ALLOW;
    }

    if (data->name_len > 0 )
    {
        pCwInfo->name = (char*)data->name;
    }

    pCwInfo->signalInfoRecord.isPresent = SIGNAL_INFO_REC_PRESENT;
    pCwInfo->signalInfoRecord.signalType = data->signal_info.signal_type;
    pCwInfo->signalInfoRecord.alertPitch = data->signal_info.alert_pitch;
    pCwInfo->signalInfoRecord.signal = data->signal_info.signal;
    pCwInfo->number_type = data->number_type;
    pCwInfo->number_plan = data->number_plan;

    return 0;
}

/**
 * ProtocolCdmaInfoListIndAdapterLegacy
 */
int ProtocolCdmaInfoListIndAdapterLegacy::GetNumberOfInfoRecs() const
{
    sit_cdma_information_records *rsp = (sit_cdma_information_records *)m_pModemData->GetRawData();

    if(rsp == NULL)
    {
        return -1;
    }

    return rsp->num_of_info_recs;
}

int ProtocolCdmaInfoListIndAdapterLegacy::GetCdmaInfo(RIL_CDMA_InformationRecord &cdmaInfo, int index) const
{
    sit_cdma_information_records *rsp = (sit_cdma_information_records *)m_pModemData->GetRawData();

    if(rsp == NULL)
    {
        return -1;
    }

    cdmaInfo.name = (RIL_CDMA_InfoRecName)rsp->info_rec[index].cdma_info_name;
    switch(cdmaInfo.name) {
        case RIL_CDMA_DISPLAY_INFO_REC:
        case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC:
            cdmaInfo.rec.display.alpha_len = rsp->info_rec[index].cdma_info.display.alpha_len;
            memcpy(cdmaInfo.rec.display.alpha_buf, rsp->info_rec[index].cdma_info.display.alpha_buf,
                    cdmaInfo.rec.display.alpha_len);
            break;
        case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CONNECTED_NUMBER_INFO_REC:
            cdmaInfo.rec.number.len = rsp->info_rec[index].cdma_info.number.len;
            memcpy(cdmaInfo.rec.number.buf, rsp->info_rec[index].cdma_info.number.buf, cdmaInfo.rec.number.len);
            cdmaInfo.rec.number.number_type = rsp->info_rec[index].cdma_info.number.number_type;
            cdmaInfo.rec.number.number_plan = rsp->info_rec[index].cdma_info.number.number_plan;
            cdmaInfo.rec.number.pi = rsp->info_rec[index].cdma_info.number.pi;
            cdmaInfo.rec.number.si = rsp->info_rec[index].cdma_info.number.si;
            break;
        case RIL_CDMA_SIGNAL_INFO_REC:
            cdmaInfo.rec.signal.isPresent = rsp->info_rec[index].cdma_info.signal.is_present;
            cdmaInfo.rec.signal.signalType = rsp->info_rec[index].cdma_info.signal.signal_type;
            cdmaInfo.rec.signal.alertPitch = rsp->info_rec[index].cdma_info.signal.alert_pitch;
            cdmaInfo.rec.signal.signal = rsp->info_rec[index].cdma_info.signal.signal;
            break;
        case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC:
            cdmaInfo.rec.redir.redirectingNumber.len = rsp->info_rec[index].cdma_info.redirecting_number.redirecting_number.len;
            memcpy(cdmaInfo.rec.redir.redirectingNumber.buf, rsp->info_rec[index].cdma_info.redirecting_number.redirecting_number.buf,
                    cdmaInfo.rec.redir.redirectingNumber.len);
            cdmaInfo.rec.redir.redirectingNumber.number_type = rsp->info_rec[index].cdma_info.redirecting_number.redirecting_number.number_type;
            cdmaInfo.rec.redir.redirectingNumber.number_plan = rsp->info_rec[index].cdma_info.redirecting_number.redirecting_number.number_plan;
            cdmaInfo.rec.redir.redirectingNumber.pi = rsp->info_rec[index].cdma_info.redirecting_number.redirecting_number.pi;
            cdmaInfo.rec.redir.redirectingNumber.si = rsp->info_rec[index].cdma_info.redirecting_number.redirecting_number.si;
            cdmaInfo.rec.redir.redirectingReason = (RIL_CDMA_RedirectingReason)rsp->info_rec[index].cdma_info.redirecting_number.reason;
            break;
        case RIL_CDMA_LINE_CONTROL_INFO_REC:
            cdmaInfo.rec.lineCtrl.lineCtrlPolarityIncluded = rsp->info_rec[index].cdma_info.line_control.line_ctrl_polarity_included;
            cdmaInfo.rec.lineCtrl.lineCtrlToggle = rsp->info_rec[index].cdma_info.line_control.line_ctrl_toggle;
            cdmaInfo.rec.lineCtrl.lineCtrlReverse = rsp->info_rec[index].cdma_info.line_control.line_ctrl_reverse;
            cdmaInfo.rec.lineCtrl.lineCtrlPowerDenial = rsp->info_rec[index].cdma_info.line_control.line_ctrl_power_denial;
            break;
        default:
            return -1;
    }

    return 0;
}

/**
 * ProtocolCdmaOtaProvisionStatusIndAdapterLegacy
 */
int ProtocolCdmaOtaProvisionStatusIndAdapterLegacy::GetOtaProvisionStatus() const
{
    int provisionStatus = -1;
    sit_ota_provision_status_ind *ind = (sit_ota_provision_status_ind *)m_pModemData->GetRawData();

    if(ind == NULL)
    {
        return -1;
    }

    provisionStatus = CallProtocolUtils::getProvisionStatus(ind->otaType, ind->otaStatus);

    return provisionStatus;
}

/**
 * ProtocolEmergencySupportRatModeIndAdapterLegacy
 */
int ProtocolEmergencySupportRatModeIndAdapterLegacy::GetSupportRatMode() const
{
    sit_call_emergency_support_rat_mode_ind *data = (sit_call_emergency_support_rat_mode_ind *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_IND_EMERGENCY_SUPPORT_RAT_MODE) {
        int available_tech = (int)(data->available_tech);
        if (available_tech == SIT_SUPPORT_RAT_MODE_3GPP) {
            return SUPPORT_RAT_MODE_3GPP;
        } else if (available_tech == SIT_SUPPORT_RAT_MODE_3GPP2) {
            return SUPPORT_RAT_MODE_3GPP2;
        } else if (available_tech == SIT_SUPPORT_RAT_MODE_ALL) {
            return SUPPORT_RAT_MODE_ALL;
        }
    }
    return SUPPORT_RAT_MODE_3GPP;
}

/**
 * ProtocolExitEmergencyCbModeRespAdapterLegacy
 */
bool ProtocolExitEmergencyCbModeRespAdapterLegacy::GetResult() const
{
    sit_call_exit_emergency_cb_mode_rsp *data = (sit_call_exit_emergency_cb_mode_rsp *)m_pModemData->GetRawData();

    return (data->result == SIT_RESULT_SUCCESS);
}

RIL_Errno ProtocolExitEmergencyCbModeRespAdapterLegacy::GetRilErrorCode() const
{
    if (m_pModemData != NULL) {
        const RCM_HEADER *rcmdata = (RCM_HEADER *)m_pModemData->GetRawData();
        if (rcmdata != NULL && m_pModemData->GetLength() >= (int)sizeof(RCM_HEADER)) {
            if (rcmdata->type == RCM_TYPE_RESPONSE) {
                int errorCode = rcmdata->ext.rsp.error & 0xFF;
                if ( (int)RCM_E_SUCCESS <= errorCode && errorCode <= RIL_E_NO_SUCH_ELEMENT)
                    return (RIL_Errno)errorCode;
                else if ( (int)RCM_E_UNDEFINED_CMD == errorCode)
                    return (RIL_Errno)RIL_E_REQUEST_NOT_SUPPORTED;
                else if ( (int)RCM_E_NO_SUCH_ELEMENT < errorCode && errorCode < RCM_E_MAX) {
                    return (RIL_Errno)((errorCode - RCM_E_NO_SUCH_ELEMENT) + RIL_E_OEM_ERROR_1 - 1);
                }
            }
        }
    }
    // default error code
    return RIL_E_MODEM_ERR;
}

/**
 * ProtocolUnsolOnSSAdapterLegacy
 */
int ProtocolUnsolOnSSAdapterLegacy::GetServiceType() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_ss_on_ss_ind *data = (sit_ss_on_ss_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_ON_SS) {
            ret = data->service_type;
        }
    }
    return ret;
}

int ProtocolUnsolOnSSAdapterLegacy::GetRequestType() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_ss_on_ss_ind *data = (sit_ss_on_ss_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_ON_SS) {
            ret = data->request_type;
        }
    }
    return ret;
}

int ProtocolUnsolOnSSAdapterLegacy::GetTeleServiceType() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_ss_on_ss_ind *data = (sit_ss_on_ss_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_ON_SS) {
            ret = data->teleservice_type;
        }
    }
    return ret;
}

int ProtocolUnsolOnSSAdapterLegacy::GetServiceClass() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_ss_on_ss_ind *data = (sit_ss_on_ss_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_ON_SS) {
            ret = data->service_class;
        }
    }
    return ret;
}

int ProtocolUnsolOnSSAdapterLegacy::GetResult() const
{
    RilLogW("%s need to implement", __FUNCTION__);
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_ss_on_ss_ind *data = (sit_ss_on_ss_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_ON_SS) {
            // Need to do
            // type conversion is needed from RCM error to RIL error
            //ret = data->result;
            ret = RIL_E_REQUEST_NOT_SUPPORTED;
        }
    }
    return ret;
}

int ProtocolUnsolOnSSAdapterLegacy::GetDataType() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_ss_on_ss_ind *data = (sit_ss_on_ss_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_ON_SS) {
            ret = data->data_type;
        }
    }
    return ret;
}

bool ProtocolUnsolOnSSAdapterLegacy::GetData(void * /* pData */) const
{
    RilLogW("%s need to implement", __FUNCTION__);
    bool ret = false;

    /*
    if (m_pModemData != NULL && pData != NULL) {
        sit_ss_on_ss_ind *data = (sit_ss_on_ss_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_ON_SS) {
            int dataType = data->data_type;
            int index = 0;
            if (dataType == SIT_ON_SS_IND_SS_INFO) {
                int *pSsInfo = (int *)pData;
                for (index = 0; index < MAX_SS_INFO_NUM; index++) {
                    *(pSsInfo + index) = data->data.ss_info[index];
                }
                ret = true;
            } else if (dataType == SIT_ON_SS_IND_CF_INFO) {
                RIL_CfData *pCfData = (RIL_CfData *)pData;
                pCfData->numValidIndexes = data->data.cf_info.call_forward_num;
                for (index = 0; index < MAX_SS_INFO_NUM; index++) {
                    // need to implement
                }
                ret = true;
            } else {
                // It shoud not happen
            }
        }
    }
    */
    return ret;
}

/**
  * ProtocolSrvccHoIndAdapterLegacy
 */
ProtocolSrvccHoIndAdapterLegacy::ProtocolSrvccHoIndAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData), ProtocolSrvccHoIndAdapterInterface() {
    Init();
}

void ProtocolSrvccHoIndAdapterLegacy::Init() {
    memset(srvccInfo, 0, sizeof(srvccInfo));
    if(GetParameter() != NULL && GetId() == SIT_IND_SRVCC_HO) {
        memcpy(srvccInfo, GetParameter(), sizeof(srvccInfo));
    }
}

BYTE ProtocolSrvccHoIndAdapterLegacy::GetSrvccHoType() const {
    return srvccInfo[0];
}

BYTE ProtocolSrvccHoIndAdapterLegacy::GetSrvccHoStatus() const {
    return srvccInfo[1];
}

BYTE ProtocolSrvccHoIndAdapterLegacy::GetSrvccHoFailCause() const {
    return srvccInfo[2];
}
