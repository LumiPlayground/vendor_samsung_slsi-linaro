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
 * protocolcalladapterjson.cpp
 *
 *  Created on: 2021. 07. 15.
 *      Author: p1.bansal
 */


#include "protocolcalladapterjson.h"
#include "callprotocolutils.h"
#include "protocolutils.h"
#include "callreqdata.h"
#include "util.h"
#include "rillog.h"
#include "calljsondef.h"
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
 * ProtocolGetCurrentCallAdapterJson
 */
ProtocolGetCurrentCallAdapterJson::ProtocolGetCurrentCallAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_GET_CURRENT_CALLS);
}

bool ProtocolGetCurrentCallAdapterJson::HasValidLength() {
    if(!m_bParsingSuccess)
        return false;
    return true;
}

int ProtocolGetCurrentCallAdapterJson::GetCallNum() {
    if(!m_bParsingSuccess)
        return 0;

    return m_jsonData[ns_call_get_current_calls_rsp::key_current_call_list].size();
}

int ProtocolGetCurrentCallAdapterJson::ConvertSitToUusInfo(UusInfo* pUusInfo, int uusType, int uus_dcs, int len, BYTE* pData) {
    if ( pUusInfo != NULL )
    {
        pUusInfo->m_uusType = (RIL_UUS_Type)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_UUS_TYPE, uusType);
        pUusInfo->m_uusDcs = (RIL_UUS_DCS)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_UUS_DCS, uus_dcs);
        pUusInfo->m_uusLength = len;
        if ( pUusInfo->m_uusLength > 0 && pData != NULL )
        {
            memcpy(pUusInfo->m_uusData, pData, pUusInfo->m_uusLength);
        }
        return 0;
    }
    RilLogE("[%s] pUusInfo is NULL", __FUNCTION__);
    return -1;
}

int ProtocolGetCurrentCallAdapterJson::GetCallInfo(CallInfo* pCallInfo, int index) {
    if ( !m_bParsingSuccess )
        return -4;
    if ( pCallInfo == NULL) {
        RilLogE("[%s] pCallInfo is NULL", __FUNCTION__);
        return -1;
    } else if ( index < 0 || index >= MAX_CALL_LIST_NUM || index >= GetCallNum() ) {
        RilLogE("[%s] Invalid index = %d ", __FUNCTION__, index);
        return -2;
    }

    Json::Value callInfo = m_jsonData[ns_call_get_current_calls_rsp::key_current_call_list][index];

    pCallInfo->m_state = (RIL_CallState)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_STATE, callInfo[ns_call_call_info::key_call_state].asInt());
    pCallInfo->m_nIndex = callInfo[ns_call_call_info::key_index].asInt();
    RilLogV("pCallInfo->m_nIndex : %d", pCallInfo->m_nIndex);
    pCallInfo->m_toa = (callInfo[ns_call_call_info::key_toa].asInt() == RIL_TOA_INTERNATIONAL) ? RIL_TOA_INTERNATIONAL : RIL_TOA_UNKNOWN;
    pCallInfo->m_isMParty = (callInfo[ns_call_call_info::key_is_mpty].asInt() == SIT_CALL_IS_MPTY_MULTIPARTY) ? true : false;
    pCallInfo->m_isMt = (callInfo[ns_call_call_info::key_is_mt].asInt() == SIT_CALL_IS_MT_MT);
    pCallInfo->m_als = callInfo[ns_call_call_info::key_als].asInt();

    int callType = callInfo[ns_call_call_info::key_call_type].asInt();
    if(  callType <= SIT_CALL_CALL_TYPE_VOICE
            || callType == SIT_CALL_CALL_TYPE_EMERGENCY
            || callType == SIT_CALL_CALL_TYPE_VOLTE
            || callType == SIT_CALL_CALL_TYPE_CDMA_VOICE
            || callType == SIT_CALL_CALL_TYPE_CDMA_EMERGENCY ) {
        pCallInfo->m_isVoice = true;
    } else {
        pCallInfo->m_isVoice = false;
    }

    pCallInfo->m_isVideo = (callType == SIT_CALL_CALL_TYPE_VIDEO)? true: false;
    if (pCallInfo->m_isVoice != true && pCallInfo->m_isVideo != true) {
        RilLogE("[%s] Invalid callType", __FUNCTION__);
        return -3;
    }

    int isVoicePrivacy = callInfo[ns_call_call_info::key_is_voice_privacy].asInt();
    pCallInfo->m_isVoicePrivacy = ( isVoicePrivacy == SIT_CALL_IS_VOICE_PRIVACY_INACTIVATED) ? RIL_CALL_CDMA_VOICEPRIVACY_INACTIVE : RIL_CALL_CDMA_VOICEPRIVACY_ACTIVE;

    int numberPresentation = callInfo[ns_call_call_info::key_number_presentation].asInt();
    pCallInfo->m_numPresent = (CallPresentation)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_PRESENTATION, numberPresentation);
    if ( numberPresentation != RIL_CALL_NAME_PRESENTATION_ALLOW
            && numberPresentation != RIL_CALL_NAME_PRESENTATION_UNKNOWN) {
        memset(pCallInfo->m_number, 0, sizeof(pCallInfo->m_number));
    } else {
        string number = callInfo[ns_call_call_info::key_number].asString();
        memset(pCallInfo->m_number, 0, sizeof(pCallInfo->m_number));
        if ( number.length() > 0) {
            ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, number, MAX_DIAL_NUM);
            memcpy(pCallInfo->m_number, number.c_str(), number.length());
            pCallInfo->m_numPresent = RIL_CALL_NAME_PRESENTATION_ALLOW;
        }
    }

    int namePresentation = callInfo[ns_call_call_info::key_name_presentation].asInt();
    pCallInfo->m_namePresent = (CallPresentation)CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_PRESENTATION, namePresentation);
    if ( namePresentation != RIL_CALL_NAME_PRESENTATION_ALLOW
            && namePresentation != RIL_CALL_NAME_PRESENTATION_UNKNOWN) {
        memset(pCallInfo->m_name, 0, sizeof(pCallInfo->m_name));
    } else {
        string name = callInfo[ns_call_call_info::key_name].asString();
        memset(pCallInfo->m_name, 0, sizeof(pCallInfo->m_name));
        if ( name.length() > 0) {
            int decodedLen;
            char decodedName[MAX_DIAL_NAME];
            const unsigned char dcsType = (BYTE)callInfo[ns_call_call_info::key_name_dcs_type].asInt();
            decodedLen = DecodingUssd(dcsType, (const unsigned char *)name.c_str(), name.length(), (unsigned char*)decodedName, sizeof(decodedName));
            RilLogV("[%s] decoded name : %s [dcs:%x, len:%d]", __FUNCTION__, decodedName, dcsType, decodedLen);
            int len = MIN(decodedLen, MAX_DIAL_NAME);
            memcpy(pCallInfo->m_name, decodedName, len);

            pCallInfo->m_namePresent = RIL_CALL_NAME_PRESENTATION_ALLOW;
        }
    }
    pCallInfo->m_audioQuality = (RIL_AudioQuality) callInfo[ns_call_call_info::key_audio_quality].asInt();

    string forwardedNumber = callInfo[ns_call_call_info::key_forwarded_num].asString();
    memset(pCallInfo->m_forwardedNumber, 0, sizeof(pCallInfo->m_forwardedNumber));
    if ( forwardedNumber.length() > 0) {
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, forwardedNumber, MAX_SS_NUM_LEN);
        memcpy(pCallInfo->m_forwardedNumber, forwardedNumber.c_str(), forwardedNumber.length());
    }

    int uusType = callInfo[ns_call_call_info::key_uus_type].asInt();
    int uusDcs = callInfo[ns_call_call_info::key_uus_dcs].asInt();
    string uusData = callInfo[ns_call_call_info::key_uus_data].asString();
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, uusData, MAX_UUS_DATA_LEN);
    ConvertSitToUusInfo(&(pCallInfo->m_uusInfo), uusType, uusDcs, uusData.length(), (BYTE *)uusData.c_str());
    return 0;
}

void ProtocolGetCurrentCallAdapterJson::DebugPrintCallInfo(CallInfo* pCallInfo) {
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
 * ProtocolGetLastCallFailCauseAdapterJson
 */
ProtocolGetLastCallFailCauseAdapterJson::ProtocolGetLastCallFailCauseAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_GET_LAST_CALL_FAIL_CAUSE);
}

int ProtocolGetLastCallFailCauseAdapterJson::GetLastCallFailCause()
{
    if(!m_bParsingSuccess) {
        return CALL_FAIL_ERROR_UNSPECIFIED;
    }

    //RilLogI("[%s] Json_Data = %s",  __FUNCTION__, GetParameter());
    int lastCallFailCause = m_jsonData[ns_call_get_last_call_fail_cause_rsp::key_last_call_fail_cause].asInt();

    return CallProtocolUtils::getRilLastCallFailCause(lastCallFailCause);
}

/**
 * ProtocolGetCallForwardingStatusAdapterJson
 */
ProtocolGetCallForwardingStatusAdapterJson::ProtocolGetCallForwardingStatusAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData), ProtocolGetCallForwardingStatusAdapterInterface() {
    Init();
}

void ProtocolGetCallForwardingStatusAdapterJson::Init() {
    memset(m_number, 0, sizeof(m_number[0][0]) * MAX_CALL_FORWARD_STATUS_NUM * (MAX_SS_NUM_LEN + 1));
    ParseJsonPayload(SIT_GET_CALL_FORWARD_STATUS);
    if (m_bParsingSuccess) {
        m_jsonCallFrwrdStatus = m_jsonData[ns_ss_get_call_forward_status_rsp::key_call_frwrd_status_list];
    }
}

int ProtocolGetCallForwardingStatusAdapterJson::GetCfNum() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_ss_get_call_forward_status_rsp::key_call_frwrd_status_list].size();
    }

    return 0;
}

bool ProtocolGetCallForwardingStatusAdapterJson::HasValidLength() {
    return true;
}

int ProtocolGetCallForwardingStatusAdapterJson::GetCfInfo(RIL_CallForwardInfo* pCfInfo, int index) {
    if (pCfInfo == NULL) {
        return -1;
    } else if (index < 0 || index >= MAX_CALL_FORWARD_STATUS_NUM || index >= GetCfNum()) {
        return -2;
    } else if (!m_bParsingSuccess) {
        return -3;
    }

    char tNumber[MAX_SS_NUM_LEN];

    pCfInfo->status = CallProtocolUtils::ConvertSitDefineToRilDefine(
            SIT_CALL_TB_IDX_SERVICE_CLASS_CF_GET,
            m_jsonCallFrwrdStatus[index][ns_ss_call_forward_status::key_status].asInt());
    pCfInfo->reason = m_jsonCallFrwrdStatus[index][ns_ss_call_forward_status::key_reason].asInt();
    pCfInfo->serviceClass = m_jsonCallFrwrdStatus[index][ns_ss_call_forward_status::key_srvc_class].asInt();
    pCfInfo->toa = m_jsonCallFrwrdStatus[index][ns_ss_call_forward_status::key_toa].asInt();

    std::string numberStr = m_jsonCallFrwrdStatus[index][ns_ss_call_forward_status::key_number].asString();
    const char* number = numberStr.c_str();
    int numLen = strlen(number);

    // TOA 0x91 means international number, add "+" to number.
    if (pCfInfo->toa == 0x91 && numLen != 0) {
        // +2 is for "+" and terminating null character.
        numLen = (numLen + 2) > MAX_SS_NUM_LEN ? MAX_SS_NUM_LEN : (numLen + 2);
        snprintf(tNumber, numLen, "+%s", number);
        memcpy(m_number[index], tNumber, numLen);
    } else if (pCfInfo->toa != 0x91 && numLen != 0) {
        numLen = numLen > MAX_SS_NUM_LEN ? MAX_SS_NUM_LEN : numLen;
        memcpy(m_number[index], number, numLen);
    }

    pCfInfo->number = m_number[index];
    pCfInfo->timeSeconds = m_jsonCallFrwrdStatus[index][ns_ss_call_forward_status::key_time_secs].asInt();

    return 0;
}

void ProtocolGetCallForwardingStatusAdapterJson::DebugPrintCfInfo(RIL_CallForwardInfo* pCfInfo)
{
    if (pCfInfo == NULL) return;

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
 * ProtocolGetCallWaitingAdapterJson
 */
int ProtocolGetCallWaitingAdapterJson::GetServiceStatus() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_ss_get_call_waiting_rsp::key_service_status].asInt();
    }

    return -1;
}

int ProtocolGetCallWaitingAdapterJson::GetServiceClass() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_ss_get_call_waiting_rsp::key_service_class].asInt();
    }

    return -1;
}

/**
 * ProtocolGetLastCallFailCauseAdapterJson
 */
int ProtocolGetClipAdapterJson::GetClipStatus() {
    if (m_bParsingSuccess) {
        int clipStatus = m_jsonData[ns_ss_get_clip_rsp::key_status].asInt();
        return CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_GET_CLIP_STATE, clipStatus);
    }

    return -1;
}

/**
 * ProtocolGetClirAdapterJson
 */
int ProtocolGetClirAdapterJson::GetClirStatus() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_ss_get_clir_rsp::key_clir_status].asInt();  // same value defined in TS 27.007 7.7
    }
    return -1;
}

/**
 * ProtocolUssdIndAdapterJson
 */
int ProtocolUssdIndAdapterJson::GetDecodedUssd(char* decodedUssd, size_t buf_size, int& dcs) {
    if (m_bParsingSuccess && decodedUssd != NULL) {
        int ussdLen = m_jsonData[ns_ss_ussd_ind::key_ussd_data].size();
        unsigned char ussd[ussdLen];
        for (int i = 0; i < ussdLen; i++) {
            ussd[i] = (unsigned char) m_jsonData[ns_ss_ussd_ind::key_ussd_data][i].asInt();
        }

        // Do not print "ussd" as a string. (not null-terminated ASCII)
        dcs = m_jsonData[ns_ss_ussd_ind::key_dcs_type].asInt();

        int decodedLen = DecodingUssd(dcs, ussd, ussdLen, (unsigned char*)decodedUssd, buf_size);
        RilLogV("[%s] decoded ussd : %s(%d) by dcs(%x)", __FUNCTION__, decodedUssd, decodedLen, dcs);
        return decodedLen;
    }

    return 0;
}

int ProtocolUssdIndAdapterJson::GetUssdStatus() {
    if (m_bParsingSuccess) {
        int ussdStatus = m_jsonData[ns_ss_ussd_ind::key_status].asInt();
        return CallProtocolUtils::ConvertSitDefineToRilDefine(SIT_CALL_TB_IDX_USSD_STATUS, ussdStatus);
    }

    return -1;
}

/**
 * ProtocolSsSvcIndAdapterJson
 */
ProtocolSsSvcIndAdapterJson::ProtocolSsSvcIndAdapterJson(const ModemData *pModemData) :
    ProtocolJsonIndAdapter(pModemData), ProtocolSsSvcIndAdapterInterface()
{
    m_pNumber = NULL;
    ParseJsonPayload(SIT_IND_SUPP_SVC_NOTIFICATION);
}

ProtocolSsSvcIndAdapterJson::~ProtocolSsSvcIndAdapterJson() {
    if (m_pNumber) {
        delete[] m_pNumber;
        m_pNumber = NULL;
    }
}

int ProtocolSsSvcIndAdapterJson::GetNotificationType() {
    if (m_bParsingSuccess) {
        return CallProtocolUtils::ConvertSitDefineToRilDefine(
                SIT_CALL_TB_IDX_SSNOTI_TYPE,
                m_jsonData[ns_ss_supp_svc_notification_ind::key_noti_type].asInt());
    }

    return -1;
}

int ProtocolSsSvcIndAdapterJson::GetCode() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_ss_supp_svc_notification_ind::key_code].asInt();
    }

    return -1;
}

int ProtocolSsSvcIndAdapterJson::GetCugIndex() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_ss_supp_svc_notification_ind::key_index].asInt();
    }

    return -1;
}

UINT ProtocolSsSvcIndAdapterJson::GetSSType() {
    /* 0, 16, 32, 48, 64, (80, 96, 112) */
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
    if (m_bParsingSuccess) {
        return m_jsonData[ns_ss_supp_svc_notification_ind::key_number_type].asUInt();
    }

    return 0;
}

int ProtocolSsSvcIndAdapterJson::GetNumberLength() {
    if (m_bParsingSuccess) {
        string number = m_jsonData[ns_ss_supp_svc_notification_ind::key_number].asString();
        return number.size();
    }

    return 0;
}

char* ProtocolSsSvcIndAdapterJson::GetNumber() {
    if (!m_bParsingSuccess) return NULL;
    if (m_pNumber == nullptr) {
        string number = m_jsonData[ns_ss_supp_svc_notification_ind::key_number].asString();
        const char *pNumber = number.c_str();
        int numberLen = strlen(pNumber);
        m_pNumber = new char[numberLen + 1];
        memset(m_pNumber, 0, numberLen + 1);
        memcpy(m_pNumber, pNumber, numberLen);
    }
    return m_pNumber;
}

/**
 * ProtocolGetColpAdapterJson
 */
int ProtocolGetColpAdapterJson::GetColpStatus() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_ss_get_colp_rsp::key_colp_status].asInt();
    }
    return -1;
}

/**
 * ProtocolGetColrAdapterJson
 */
int ProtocolGetColrAdapterJson::GetColrStatus() {
    if (m_bParsingSuccess) {
        return m_jsonData[ns_ss_get_colr_rsp::key_status].asInt();
    }

    return -1;
}

/**
 * ProtocolSendCallConfirmRespAdapterJson
 */
ProtocolSendCallConfirmRespAdapterJson::ProtocolSendCallConfirmRespAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_CALL_CONFIRM);
}

int ProtocolSendCallConfirmRespAdapterJson::GetResult() const {
    if (!m_bParsingSuccess) {
        return -1;
    }
    return m_jsonData[ns_call_call_confirm_rsp::key_result].asInt();
}

/**
 * ProtocolGetPreferredVoicePrivacyModeAdapterJson
 */
ProtocolGetPreferredVoicePrivacyModeAdapterJson::ProtocolGetPreferredVoicePrivacyModeAdapterJson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData) {
    ParseJsonPayload(SIT_GET_CDMA_VOICE_PRIVACY_MODE);
}

int ProtocolGetPreferredVoicePrivacyModeAdapterJson::GetPreferredVoicePrivacyMode() {
    if (!m_bParsingSuccess) {
        return -1;
    }
    return m_jsonData[ns_call_get_cdma_voice_privacy_mode_rsp::key_vp_mode].asInt();
}

/**
 * ProtocolEmergencyCallListIndAdapterJson
 */
int ProtocolEmergencyCallListIndAdapterJson::GetVersion() const {
    return mVersion;
}

/**
 * ProtocolEmergencyCallListIndAdapterJson
 */
const char *ProtocolEmergencyCallListIndAdapterJson::GetMcc() const {
    return mMcc;
}

/**
 * ProtocolEmergencyCallListIndAdapterJson
 */
const char *ProtocolEmergencyCallListIndAdapterJson::GetMnc() const {
    return mMnc;
}

/**
 * ProtocolEmergencyCallListIndAdapterJson
 */
int ProtocolEmergencyCallListIndAdapterJson::GetNum() const {
    return mNum;
}

/**
 * ProtocolEmergencyCallListIndAdapterJson
 */
ProtocolEmergencyCallListIndAdapterJson::ProtocolEmergencyCallListIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData), ProtocolEmergencyCallListIndAdapterInterface() {
        Init();
}

void ProtocolEmergencyCallListIndAdapterJson::Init() {
    memset(mMcc, 0, size(mMcc));
    memset(mMnc, 0, size(mMnc));
    mNum = 0;
    mVersion = 4;
    ParseJsonPayload(SIT_IND_EMERGENCY_CALL_LIST);
    if (m_bParsingSuccess) {
        string mccString = m_jsonData[ns_call_emc_call_list_ind::key_mcc].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, mccString, MAX_MCC_LEN);
        memcpy(mMcc, mccString.c_str(), mccString.length());

        string mncString = m_jsonData[ns_call_emc_call_list_ind::key_mnc].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, mncString, MAX_MNC_LEN);
        memcpy(mMnc, mncString.c_str(), mncString.length());
        if (mMnc[MAX_MNC_LEN - 1] == '#') {
            mMnc[MAX_MNC_LEN - 1] = 0;
        }

        mNum = m_jsonData[ns_call_emc_call_list_ind::key_emc_nums_info_data].size();
    }
}

/**
 * ProtocolEmergencyCallListIndAdapterJson
 */
void ProtocolEmergencyCallListIndAdapterJson::GetEccInfo(EccInfo *eccInfo) {
    if (!m_bParsingSuccess) {
        return;
    }

    int idx = 0;
    while (idx < GetNum()) {
        Json::Value emcNumberInfo = m_jsonData[ns_call_emc_call_list_ind::key_emc_nums_info_data][idx++];

        int category = emcNumberInfo[ns_call_emc_num_info::key_service_category].asInt();
        string emcNumber = emcNumberInfo[ns_call_emc_num_info::key_emc_number].asString();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, emcNumber, MAX_EMERGENCY_NUMBER_LEN);
        int source = emcNumberInfo[ns_call_emc_num_info::key_source].asInt();
        eccInfo[SIT_ECC_INFO_TYPE_ENL].AddEmergencyNumber(mMcc, mMnc,
                                                          emcNumber.c_str(),
                                                          emcNumber.length(),
                                                          category,
                                                          SIT_EMERGENCY_CONDITION_ALWAYS,
                                                          source);
    }
}

/**
 * ProtocolSrvccHoIndAdapterJson
 */
ProtocolSrvccHoIndAdapterJson::ProtocolSrvccHoIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_SRVCC_HO);
}

BYTE ProtocolSrvccHoIndAdapterJson::GetSrvccHoType() const {
    if(!m_bParsingSuccess)
        return (BYTE)0;

    return (BYTE)m_jsonData[ns_call_srvcc_ho_ind::key_srvcc_ho_type].asInt();
}

BYTE ProtocolSrvccHoIndAdapterJson::GetSrvccHoStatus() const {
    if(!m_bParsingSuccess)
        return (BYTE)0;

    return (BYTE)m_jsonData[ns_call_srvcc_ho_ind::key_srvcc_ho_status].asInt();
}

BYTE ProtocolSrvccHoIndAdapterJson::GetSrvccHoFailCause() const {
    if(!m_bParsingSuccess)
        return (BYTE)0;

    return (BYTE)m_jsonData[ns_call_srvcc_ho_ind::key_srvcc_ho_fail_cause].asInt();
}

/**
 * ProtocolCdmaOtaProvisionStatusIndAdapterJson
 */
ProtocolCdmaOtaProvisionStatusIndAdapterJson::ProtocolCdmaOtaProvisionStatusIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_OTA_PROVISION_STATUS);
}

int ProtocolCdmaOtaProvisionStatusIndAdapterJson::GetOtaProvisionStatus() const
{
    int provisionStatus = -1;
    int otaType;
    int otaStatus;

    if(!m_bParsingSuccess) {
        return -1;
    }

    otaType = m_jsonData[ns_call_ota_provision_status_ind::key_ota_type].asInt();
    otaStatus = m_jsonData[ns_call_ota_provision_status_ind::key_ota_status].asInt();

    provisionStatus = CallProtocolUtils::getProvisionStatus(otaType, otaStatus);

    return provisionStatus;
}

/**
 * ProtocolEmergencySupportRatModeIndAdapterJson
 */
ProtocolEmergencySupportRatModeIndAdapterJson::ProtocolEmergencySupportRatModeIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_EMERGENCY_SUPPORT_RAT_MODE);
}

int ProtocolEmergencySupportRatModeIndAdapterJson::GetSupportRatMode() const {
    if(!m_bParsingSuccess) {
        return SUPPORT_RAT_MODE_3GPP;
    }

    int available_tech = m_jsonData[ns_call_emc_support_rat_mode_ind::key_available_tech].asInt();
    if (available_tech == SIT_SUPPORT_RAT_MODE_3GPP) {
        return SUPPORT_RAT_MODE_3GPP;
    } else if (available_tech == SIT_SUPPORT_RAT_MODE_3GPP2) {
        return SUPPORT_RAT_MODE_3GPP2;
    } else if (available_tech == SIT_SUPPORT_RAT_MODE_ALL) {
        return SUPPORT_RAT_MODE_ALL;
    }
    return SUPPORT_RAT_MODE_3GPP;
}

/**
 * ProtocolCdmaCallWaitingIndAdapterJson
 */
ProtocolCdmaCallWaitingIndAdapterJson::ProtocolCdmaCallWaitingIndAdapterJson(const ModemData *pModemData) :
    ProtocolJsonIndAdapter(pModemData), ProtocolCdmaCallWaitingIndAdapterInterface()
{
    m_pNumber = NULL;
    m_pName = NULL;
    ParseJsonPayload(SIT_IND_CDMA_CALL_WAITING);
}

ProtocolCdmaCallWaitingIndAdapterJson::~ProtocolCdmaCallWaitingIndAdapterJson() {
    if (m_pNumber) {
        delete[] m_pNumber;
        m_pNumber = NULL;
    }

    if (m_pName) {
        delete[] m_pName;
        m_pName = NULL;
    }
}

int ProtocolCdmaCallWaitingIndAdapterJson::GetCwInfo(RIL_CDMA_CallWaiting_v6 *pCwInfo) {
    if (!m_bParsingSuccess || pCwInfo == NULL)  return -1;
    pCwInfo->numberPresentation = (CallPresentation) CallProtocolUtils::ConvertSitDefineToRilDefine(
            SIT_CALL_TB_IDX_PRESENTATION,
            m_jsonData[ns_ss_cdma_call_waiting_ind::key_number_presentation].asInt());

    string number = m_jsonData[ns_ss_cdma_call_waiting_ind::key_number].asString();
    int numLen = number.size();
    if (pCwInfo->numberPresentation != RIL_CALL_NAME_PRESENTATION_ALLOW) {
        numLen = 0;
    }

    if (numLen > 0 ) {
        m_pNumber = new char[numLen + 1];
        memset(m_pNumber, 0, numLen + 1);
        const char* pNumber = number.c_str();
        memcpy(m_pNumber, pNumber, numLen);
        pCwInfo->number = m_pNumber;
    }

    string name = m_jsonData[ns_ss_cdma_call_waiting_ind::key_name].asString();
    int nameLen = name.size();
    if (nameLen > 0) {
        m_pName = new char[nameLen + 1];
        memset(m_pName, 0, nameLen + 1);
        const char* pName = name.c_str();
        memcpy(m_pName, pName, nameLen);
        pCwInfo->name = m_pName;
    }

    pCwInfo->signalInfoRecord.isPresent = SIGNAL_INFO_REC_PRESENT;
    pCwInfo->signalInfoRecord.signalType =
        m_jsonData[ns_ss_cdma_call_waiting_ind::key_signal_type].asInt();
    pCwInfo->signalInfoRecord.alertPitch =
        m_jsonData[ns_ss_cdma_call_waiting_ind::key_alert_pitch].asInt();
    pCwInfo->signalInfoRecord.signal =
        m_jsonData[ns_ss_cdma_call_waiting_ind::key_signal].asInt();
    pCwInfo->number_type = m_jsonData[ns_ss_cdma_call_waiting_ind::key_number_type].asInt();
    pCwInfo->number_plan = m_jsonData[ns_ss_cdma_call_waiting_ind::key_number_plan].asInt();

    return 0;
}

/**
 * ProtocolCdmaInfoListIndAdapterJson
 */
ProtocolCdmaInfoListIndAdapterJson::ProtocolCdmaInfoListIndAdapterJson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData) {
    ParseJsonPayload(SIT_IND_CDMA_INFO_REC);
}

int ProtocolCdmaInfoListIndAdapterJson::GetNumberOfInfoRecs() const {
    if(!m_bParsingSuccess)
        return -1;
    return m_jsonData[ns_call_cdma_info_rec_ind::key_cdma_info_records].size();
}

int ProtocolCdmaInfoListIndAdapterJson::GetCdmaInfo(RIL_CDMA_InformationRecord &cdmaInfo, int index) const {
    if(!m_bParsingSuccess)
        return -1;

    Json::Value cdmaInfoRecord = m_jsonData[ns_call_cdma_info_rec_ind::key_cdma_info_records][index];
    cdmaInfo.name = (RIL_CDMA_InfoRecName)cdmaInfoRecord[ns_call_cdma_info_record::key_cdma_info_name].asInt();
    switch(cdmaInfo.name) {
        case RIL_CDMA_DISPLAY_INFO_REC:
        case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC:
            {
                Json::Value displayInfoRecord = cdmaInfoRecord[ns_call_cdma_info_record::key_display_info_record];
                string displayChar = displayInfoRecord[ns_call_display_info_record::key_display_char].asString();
                ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, displayChar, MAX_ALPHA_INFO_BUF_LEN);
                cdmaInfo.rec.display.alpha_len = displayChar.length(); // MAX_ALPHA_INFO_BUF_LEN (64)
                memcpy(cdmaInfo.rec.display.alpha_buf, displayChar.c_str(), cdmaInfo.rec.display.alpha_len);
                break;
            }
        case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CONNECTED_NUMBER_INFO_REC:
            {
                Json::Value numInfoRecord = cdmaInfoRecord[ns_call_cdma_info_record::key_num_info_record];
                string number = numInfoRecord[ns_call_num_info_record::key_number].asString();
                ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, number, MAX_NUMBER_INFO_BUFFER_LEN);
                cdmaInfo.rec.number.len = number.length(); // MAX_NUMBER_INFO_BUFFER_LEN (81)
                memcpy(cdmaInfo.rec.number.buf, number.c_str(), cdmaInfo.rec.number.len);

                cdmaInfo.rec.number.number_type = numInfoRecord[ns_call_num_info_record::key_number_type].asInt();
                cdmaInfo.rec.number.number_plan = numInfoRecord[ns_call_num_info_record::key_number_plan].asInt();
                cdmaInfo.rec.number.pi = numInfoRecord[ns_call_num_info_record::key_presentation_indicator].asInt();
                cdmaInfo.rec.number.si = numInfoRecord[ns_call_num_info_record::key_screening_indicator].asInt();
                break;
            }
        case RIL_CDMA_SIGNAL_INFO_REC:
            {
                Json::Value signalInfoRecord = cdmaInfoRecord[ns_call_cdma_info_record::key_signal_info_record];
                cdmaInfo.rec.signal.isPresent = signalInfoRecord[ns_call_signal_info_record::key_is_present].asInt();
                cdmaInfo.rec.signal.signalType = signalInfoRecord[ns_call_signal_info_record::key_signal_type].asInt();
                cdmaInfo.rec.signal.alertPitch = signalInfoRecord[ns_call_signal_info_record::key_alert_pitch].asInt();
                cdmaInfo.rec.signal.signal = signalInfoRecord[ns_call_signal_info_record::key_signal].asInt();
                break;
            }
        case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC:
            {
                Json::Value redirectingInfoRecord = cdmaInfoRecord[ns_call_cdma_info_record::key_redirecting_info_record];
                string number = redirectingInfoRecord[ns_call_redirecting_info_record::key_number].asString();
                ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_CALL, __CLASS_FUNC__, number, MAX_NUMBER_INFO_BUFFER_LEN);
                cdmaInfo.rec.redir.redirectingNumber.len = number.length(); // MAX_NUMBER_INFO_BUFFER_LEN (81)
                memcpy(cdmaInfo.rec.redir.redirectingNumber.buf, number.c_str(), cdmaInfo.rec.redir.redirectingNumber.len);

                cdmaInfo.rec.redir.redirectingNumber.number_type = redirectingInfoRecord[ns_call_redirecting_info_record::key_number_type].asInt();
                cdmaInfo.rec.redir.redirectingNumber.number_plan = redirectingInfoRecord[ns_call_redirecting_info_record::key_number_plan].asInt();
                cdmaInfo.rec.redir.redirectingNumber.pi = redirectingInfoRecord[ns_call_redirecting_info_record::key_presentation_indicator].asInt();
                cdmaInfo.rec.redir.redirectingNumber.si = redirectingInfoRecord[ns_call_redirecting_info_record::key_screening_indicator].asInt();
                cdmaInfo.rec.redir.redirectingReason = (RIL_CDMA_RedirectingReason)redirectingInfoRecord[ns_call_redirecting_info_record::key_reason].asInt();
                break;
            }
        case RIL_CDMA_LINE_CONTROL_INFO_REC:
            {
                Json::Value lineCtrInfoRecord = cdmaInfoRecord[ns_call_cdma_info_record::key_line_ctr_info_record];
                cdmaInfo.rec.lineCtrl.lineCtrlPolarityIncluded = lineCtrInfoRecord[ns_call_line_ctr_info_record::key_polarity].asInt();
                cdmaInfo.rec.lineCtrl.lineCtrlToggle = lineCtrInfoRecord[ns_call_line_ctr_info_record::key_toggle].asInt();
                cdmaInfo.rec.lineCtrl.lineCtrlReverse = lineCtrInfoRecord[ns_call_line_ctr_info_record::key_reverse].asInt();
                cdmaInfo.rec.lineCtrl.lineCtrlPowerDenial = lineCtrInfoRecord[ns_call_line_ctr_info_record::key_power_denial].asInt();
                break;
            }
        default:
            return -1;
    }
    return 0;
}
