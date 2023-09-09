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
 * protocolcallbuilderjson.cpp
 *
 *  Created on: 2021. 07. 13.
 *      Author: p1.bansal
 */
#include "protocolcallbuilderjson.h"
#include "callprotocolutils.h"
#include "protocolutils.h"
#include "calljsondef.h"
#include "json/json.h"
#include "util.h"
#include "rillog.h"
#include <string>

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * ProtocolCallBuilderJson
 */
ModemData *ProtocolCallBuilderJson::BuildGetCallList() {
    return GetJsonNullReqModemData(SIT_GET_CURRENT_CALLS);
}

ModemData *ProtocolCallBuilderJson::BuildDial(char *number, ClirType clir, UusInfo & /* uusInfo */,
                                              CallType cType, UINT eccType, UINT eCallUi)
{
    Json::Value jsonReqData;

    jsonReqData[ns_call_dial_req::key_call_type] = CallProtocolUtils::ConvertRilDefineToSitDefine(SIT_CALL_TB_IDX_TYPE,cType);
    jsonReqData[ns_call_dial_req::key_emc_call_sub_type] = eccType;
    string numberString = number;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, numberString, MAX_DIAL_NUM);
    jsonReqData[ns_call_dial_req::key_number] = numberString;
    if (strncmp(number, "+", 1) == 0) {
        jsonReqData[ns_call_dial_req::key_number_type] = SIT_CALL_NUMBER_TYPE_INTERNATIONAL;
    } else {
        jsonReqData[ns_call_dial_req::key_number_type] = SIT_CALL_NUMBER_TYPE_NATIONAL;
    }
    jsonReqData[ns_call_dial_req::key_number_plan] = SIT_CALL_NUMBER_PLAN_ISDN;
    jsonReqData[ns_call_dial_req::key_clir] = CallProtocolUtils::ConvertRilDefineToSitDefine(SIT_CALL_TB_IDX_CLIR, clir);
    jsonReqData[ns_call_dial_req::key_cug_call]  = SIT_CALL_CUG_CALL_DISABLED;
    jsonReqData[ns_call_dial_req::key_cug_info_index] = 0; //Optional
    jsonReqData[ns_call_dial_req::key_cug_info] = SIT_CALL_CUG_INFO_NONE; //Optional
    jsonReqData[ns_call_dial_req::key_ecall_mode] = eCallUi; //Optional

    return GetJsonReqModemData(jsonReqData, SIT_DIAL);
}

ModemData *ProtocolCallBuilderJson::BuildAnswer()
{
    return GetJsonNullReqModemData(SIT_ANSWER);
}

ModemData *ProtocolCallBuilderJson::BuildExplicitCallTransfer()
{
    return GetJsonNullReqModemData(SIT_SEND_EXPLICIT_CALL_TRANSFER);
}

ModemData *ProtocolCallBuilderJson::BuildHangup(int callId) {
    Json::Value jsonReqData;
    jsonReqData[ns_call_hangup_req::key_call_index] = callId;
    return GetJsonReqModemData(jsonReqData, SIT_HANGUP);
}

ModemData *ProtocolCallBuilderJson::BuildHangupMulti(int callId) {
    Json::Value jsonReqData;
    jsonReqData[ns_call_hangup_req::key_call_index] = callId;
    return GetJsonReqModemData(jsonReqData, SIT_HANGUP);
}

ModemData *ProtocolCallBuilderJson::BuildLastCallFailCause() {
    return GetJsonNullReqModemData(SIT_GET_LAST_CALL_FAIL_CAUSE);
}

ModemData *ProtocolCallBuilderJson::BuildCdmaBurstDtmf(int /* dtmf_length */, const char *dtmf,
                                                       int /* on_len */, const char *on_length, int /* off_len */, const char *off_length) {
    Json::Value jsonReqData;
    jsonReqData[ns_call_cdma_burst_dtmf_req::key_dtmf_digits] = string();
    jsonReqData[ns_call_cdma_burst_dtmf_req::key_dtmf_on_length] = string();
    jsonReqData[ns_call_cdma_burst_dtmf_req::key_dtmf_off_length] = string();

    if ( dtmf != NULL) {
        jsonReqData[ns_call_cdma_burst_dtmf_req::key_dtmf_digits] = string(dtmf);
    }
    if ( on_length != NULL) {
        jsonReqData[ns_call_cdma_burst_dtmf_req::key_dtmf_on_length] = string(on_length);
    }
    if ( off_length != NULL) {
        jsonReqData[ns_call_cdma_burst_dtmf_req::key_dtmf_off_length] = string(off_length);
    }

    return GetJsonReqModemData(jsonReqData, SIT_CDMA_BURST_DTMF);
}

ModemData *ProtocolCallBuilderJson::BuildCdmaSetPreferredVoicePrivacyMode(int vpMode) {
    Json::Value jsonReqData;
    jsonReqData[ns_call_set_cdma_voice_privacy_mode_req::key_vp_mode] = vpMode;
    return GetJsonReqModemData(jsonReqData, SIT_SET_CDMA_VOICE_PRIVACY_MODE);
}

ModemData *ProtocolCallBuilderJson::BuildCdmaGetPreferredVoicePrivacyMode() {
    return GetJsonNullReqModemData(SIT_GET_CDMA_VOICE_PRIVACY_MODE);
}

ModemData *ProtocolCallBuilderJson::BuildHangupWaitingOrBackground() {
    return GetJsonNullReqModemData(SIT_HANGUP_WAITING_OR_BACKGROUND);
}

ModemData *ProtocolCallBuilderJson::BuildHangupForegroundResumeBackground() {
    return GetJsonNullReqModemData(SIT_HANGUP_FORGROUND_RESUME_BACKGROUND);
}

ModemData *ProtocolCallBuilderJson::BuildSwitchWaitingOrHoldingAndActive() {
    return GetJsonNullReqModemData(SIT_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE);
}

ModemData *ProtocolCallBuilderJson::BuildConference() {
    return GetJsonNullReqModemData(SIT_CONFERENCE);
}

ModemData *ProtocolCallBuilderJson::BuildCdmaFlash(const char* flash) {
    Json::Value jsonReqData;
    if (flash != NULL) {
        string flashStr = flash;
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, flashStr, MAX_FLASH_LEN);
        jsonReqData[ns_ss_set_cdma_flash_info_req::key_flash] = flashStr;
    }

    return GetJsonReqModemData(jsonReqData, SIT_SET_CDMA_FLASH_INFO);
}

ModemData *ProtocolCallBuilderJson::BuildUdub() {
    return GetJsonNullReqModemData(SIT_UDUB);
}

ModemData *ProtocolCallBuilderJson::BuildGetClip() {
    return GetJsonNullReqModemData(SIT_GET_CLIP);
}

ModemData *ProtocolCallBuilderJson::BuildGetClir() {
    return GetJsonNullReqModemData(SIT_GET_CLIR);
}

ModemData *ProtocolCallBuilderJson::BuildSetCallForwarding(SsModeType status, SsCfReason reason,
        SsClassX service_class, int toa, char* number, int timeseconds) {
    Json::Value jsonReqData;
    jsonReqData[ns_ss_set_call_forward_req::key_status] =
        CallProtocolUtils::ConvertRilDefineToSitDefine(SIT_CALL_TB_IDX_SERVICE_CLASS_CF_SET, status);
    jsonReqData[ns_ss_set_call_forward_req::key_reason] = (int) reason;  // same reason value defined in TS 27.007 7.11 "reason"

#ifdef _CHANGE_UNKNOWN_SERVICE_CLASS_
    if (service_class == RIL_SS_CLASS_UNKNOWN) {   /* 0 */
#ifdef _USE_SERVICE_CLASS_DEFAULT_7_
        service_class = RIL_SS_CLASS_DEFAULT;    /* 7 <- defined in <classx> of TS 27.007 7.11 */
#else
        service_class = RIL_SS_CLASS_VOICE;     /* 1 */
#endif
        RilLogE("RIL changes unknown service class(0) to [%d]", service_class);
    }
#endif

    jsonReqData[ns_ss_set_call_forward_req::key_srvc_class] = (int) service_class;  // same class value defined in TS 27.007 "class"
    jsonReqData[ns_ss_set_call_forward_req::key_toa] = toa;   // same toa  /* "type" from TS 27.007 7.11 */   /* TS24.008 table 10.5.118 */

    // truncate if length of number is more than MAX_SS_NUM_LEN
    string numberStr(number, MIN(MAX_SS_NUM_LEN, strlen(number)));
    jsonReqData[ns_ss_set_call_forward_req::key_number] = numberStr;

    if (0 < timeseconds && timeseconds <= 30) {    // timeseconds : 1 ~ 30
        jsonReqData[ns_ss_set_call_forward_req::key_time_secs] = timeseconds;
    } else {
        jsonReqData[ns_ss_set_call_forward_req::key_time_secs] = 20;    // default
    }

    return GetJsonReqModemData(jsonReqData, SIT_SET_CALL_FORWARD);
}

ModemData *ProtocolCallBuilderJson::BuildGetCallForwardingStatus(SsStatusType /* status */,
        SsCfReason reason, SsClassX service_class, int /* toa */, char* /* number */,
        int /* timeseconds */) {
    // For get call forward, f/w always sends number with null value
    // so we don't need to send toa, number and timseconds to CP
    // as all these values are never parsed by CP
    Json::Value jsonReqData;
    jsonReqData[ns_ss_get_call_forward_status_req::key_status] =
        SIT_SS_CALL_FARWARD_STATUS_INTERROGATE;
    jsonReqData[ns_ss_get_call_forward_status_req::key_reason] = (int) reason;  // same reason value defined in TS 27.007 7.11 "reason"

#ifdef _CHANGE_UNKNOWN_SERVICE_CLASS_
    if (service_class == RIL_SS_CLASS_UNKNOWN) {    /* 0 */
#ifdef _USE_SERVICE_CLASS_DEFAULT_7_
        service_class = RIL_SS_CLASS_DEFAULT;    /* 7 <- defined in <classx> of TS 27.007 7.11 */
#else
        service_class = RIL_SS_CLASS_VOICE;     /* 1 */
#endif
        RilLogE("RIL changes unknown service class(0) to [%d]", service_class);
    }
#endif

    jsonReqData[ns_ss_get_call_forward_status_req::key_srvc_class] = (int) service_class;

    return GetJsonReqModemData(jsonReqData, SIT_GET_CALL_FORWARD_STATUS);
}

ModemData *ProtocolCallBuilderJson::BuildSetCallWaiting(SsModeType status, SsClassX service_class) {
    Json::Value jsonReqData;
    jsonReqData[ns_ss_set_call_waiting_req::key_service_status] =
        CallProtocolUtils::ConvertRilDefineToSitDefine(SIT_CALL_TB_IDX_SERVICE_STATUS_CALL_WAITING, status);
#ifdef _CHANGE_UNKNOWN_SERVICE_CLASS_
    if (service_class == RIL_SS_CLASS_UNKNOWN) {    /* 0 */
#ifdef _USE_SERVICE_CLASS_DEFAULT_7_
        service_class = RIL_SS_CLASS_DEFAULT;    /*7 <- defined in <classx> of TS 27.007 7.11*/
#else
        service_class = RIL_SS_CLASS_VOICE;     /*1*/
#endif
        RilLogE("RIL changes unknown service class(0) to [%d]", service_class);
    }
#endif
    jsonReqData[ns_ss_set_call_waiting_req::key_service_class] = (int) service_class;  // same class value defined in TS 27.007 "class"

    return GetJsonReqModemData(jsonReqData, SIT_SET_CALL_WAITING);
}

ModemData *ProtocolCallBuilderJson::BuildGetCallWaiting(SsClassX service_class) {
#ifdef _CHANGE_UNKNOWN_SERVICE_CLASS_
    if ( service_class == RIL_SS_CLASS_UNKNOWN )    /* 0 */
    {
#ifdef _USE_SERVICE_CLASS_DEFAULT_7_
        service_class = RIL_SS_CLASS_DEFAULT;    /*7 <- defined in <classx> of TS 27.007 7.11*/
#else
        service_class = RIL_SS_CLASS_VOICE;     /*1*/
#endif
        RilLogE("%s()::RIL changes unknown service class(0) to [%d]", __FUNCTION__, service_class);
    }
#endif
    Json::Value jsonReqData;
    jsonReqData[ns_ss_get_call_waiting_req::key_service_class] = (int) service_class;  // same class value defined in TS 27.007 "class"

    return GetJsonReqModemData(jsonReqData, SIT_GET_CALL_WAITING);
}

ModemData *ProtocolCallBuilderJson::BuildChangeBarringPwd(const char* oldpasswd, const char* newpasswd, const char* newpasswd_again) {
    Json::Value jsonReqData;
    if (oldpasswd != NULL && strlen(oldpasswd) > 0) {
        string oldPassword = oldpasswd;
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, oldPassword, 4);
        jsonReqData[ns_ss_change_barring_pwd_req::key_password] = oldPassword;
    }
    if (newpasswd != NULL && strlen(newpasswd) > 0) {
        string newPassword = newpasswd;
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, newPassword, 4);
        jsonReqData[ns_ss_change_barring_pwd_req::key_new_password] = newPassword;
    }
    if (newpasswd_again != NULL && strlen(newpasswd_again) > 0) {
        string newPasswordAgain = newpasswd_again;
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, newPasswordAgain, 4);
        jsonReqData[ns_ss_change_barring_pwd_req::key_new_password_again] = newPasswordAgain;
    }

    return GetJsonReqModemData(jsonReqData, SIT_CHG_BARRING_PWD);
}

ModemData *ProtocolCallBuilderJson::BuildSendUssd(const char* ussd, bool user_initiated) {
    RilLogV("[%s] user initiated : %d", __FUNCTION__, user_initiated);
    RilLogV("[%s] org ussd : %s(%d)", __FUNCTION__, ussd, strlen(ussd));
    unsigned char encodedUssd[MAX_USSD_DATA_LEN];
    memset(encodedUssd, 0x00, sizeof(encodedUssd));
    // convert different code value between ascii table <-> gsm 7bit basic character set
    unsigned int gsmlen;
    unsigned char gsm7[MAX_USSD_DATA_LEN];
    memset(gsm7, 0x00, sizeof(gsm7));
    gsmlen = convertAsciiToGsm7bitBasicCharSet((unsigned char *) ussd, strlen(ussd),
            gsm7, MAX_USSD_DATA_LEN);

    int encodedLen = packing_gsm7bit(gsm7, gsmlen, encodedUssd);
    encodedLen = MIN(encodedLen, MAX_USSD_DATA_LEN);
    // RilLogV("[%s] encoded ussd : %s(%d)", __FUNCTION__, encodedUssd, encodedLen);
    CallProtocolUtils::PrintHexBytes("encoded ussd", encodedUssd, encodedLen);

    Json::Value jsonReqData;
    jsonReqData[ns_ss_send_ussd_req::key_ussd_type] = user_initiated == true ?
        SIT_SS_USSD_USER_INITIATED : SIT_SS_USSD_USER_RESPONSE;
    jsonReqData[ns_ss_send_ussd_req::key_dcs_type] = 0b00001111;  // GSM 7 bit default alphabet - language unspecified
    jsonReqData[ns_ss_send_ussd_req::key_ussd_data] = Json::arrayValue;
    for (int i = 0; i < encodedLen; i++) {
        jsonReqData[ns_ss_send_ussd_req::key_ussd_data].append(encodedUssd[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SEND_USSD);
}

ModemData *ProtocolCallBuilderJson::BuildCancelUssd() {
    return GetJsonNullReqModemData(SIT_CANCEL_USSD);
}

ModemData *ProtocolCallBuilderJson::BuildSeparateConnection(int callId)
{
    Json::Value jsonReqData;
    jsonReqData[ns_ss_separate_connection_req::key_index] = callId;

    return GetJsonReqModemData(jsonReqData, SIT_SEPARATE_CONNECTION);
}

ModemData *ProtocolCallBuilderJson::BuildQueryColp() {
    return GetJsonNullReqModemData(SIT_GET_COLP);
}

ModemData *ProtocolCallBuilderJson::BuildQueryColr() {
    return GetJsonNullReqModemData(SIT_GET_COLR);
}

ModemData *ProtocolCallBuilderJson::BuildSendEncodedUssd(BYTE dcs, const char* encodedUssd, bool user_initiated) {
    RilLogV("[%s] user initiated : %d", __FUNCTION__, user_initiated);
    RilLogV("[%s] dcs : %d(0x%02X)", __FUNCTION__, dcs, dcs);
    RilLogV("[%s] encode ussd : %s(%d)", __FUNCTION__, encodedUssd, strlen(encodedUssd));

    unsigned char ussdData[MAX_USSD_DATA_LEN];
    int ussdLen = HexString2Value(ussdData, sizeof(ussdData), encodedUssd);
    //CallProtocolUtils::PrintHexBytes("encoded ussd", encodedUssd, encodedLen);

    Json::Value jsonReqData;
    jsonReqData[ns_ss_send_ussd_req::key_ussd_type] = user_initiated == true ?
            SIT_SS_USSD_USER_INITIATED : SIT_SS_USSD_USER_RESPONSE;
    jsonReqData[ns_ss_send_ussd_req::key_dcs_type] = dcs;
    jsonReqData[ns_ss_send_ussd_req::key_ussd_data] = Json::arrayValue;
    for (int i = 0; i < ussdLen; i++) {
        jsonReqData[ns_ss_send_ussd_req::key_ussd_data].append(ussdData[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SEND_USSD);
}

ModemData *ProtocolCallBuilderJson::BuildSendCallConfirm() {
    return GetJsonNullReqModemData(SIT_CALL_CONFIRM);
}

ModemData *ProtocolCallBuilderJson::BuildExitEmergencyCbMode()
{
    return NULL;
}
