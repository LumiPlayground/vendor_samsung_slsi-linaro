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
 * callhandler.cpp
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#include <librilutils/systemproperty.h>
#include "callhandler.h"
#include "audioservice.h"
#include "calldata.h"
#include "callreqdata.h"
#include "callutils.h"
#include "cscserviceproxy.h"
#include "networkservice.h"
#include "protocoladapter.h"
#include "protocolcalladapter.h"
#include "protocolcallbuilder.h"
#include "protocolsoundadapter.h"
#include "rillog.h"
#include "rilapplication.h"
#include "emergencycallhelper.h"
#include "commondef.h"
#include "dbeccinfoloader.h"

#define TIMEOUT_CALL_DEFAULT            5000
#define TIMEOUT_CS_SUPPLEMENTARY        60000

#define SITRIL_CARRIER_US_VZW           1839
#define SITRIL_CARRIER_US_DISH          2517

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s() msg or RequestData is NULL", mServiceName, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s() msg or ModemData is NULL", mServiceName, __FUNCTION__); return -1; } }

static bool isCdmaVoice(int rat)
{
    switch(rat) {
        case RADIO_TECH_IS95A:
        case RADIO_TECH_IS95B:
        case RADIO_TECH_1xRTT:
        case RADIO_TECH_EVDO_0:
        case RADIO_TECH_EVDO_A:
        case RADIO_TECH_EVDO_B:
        case RADIO_TECH_EHRPD:
            break;
        default:
            return false;
    }
    return true;
}

static const char* GetCallTypeString(CallType callType)
{
    switch(callType)
    {
        case CALL_TYPE_VOICE:
            return "gsm voice";
        case CALL_TYPE_EMERGENCY:
            return "gsm emergency";
        case CALL_TYPE_CDMA_VOICE:
            return "cdma voice";
        case CALL_TYPE_CDMA_EMERGENCY:
            return "cdma emergency";
        default:
            break;
    }
    return "gsm voice";
}

/**
 * GetCurrentCallListHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetCurrentCallListHandler, MSG_CS_CALL_LIST)

void GetCurrentCallListHandler::OnCreate()
{
    memset(mRespCalls, 0, sizeof(mRespCalls));
    memset(mRespCallsData, 0, sizeof(mRespCallsData));
    memset(mRespCalls_V1_2, 0, sizeof(mRespCalls_V1_2));
    memset(mRespCalls_V1_6, 0, sizeof(mRespCalls_V1_6));
    memset(mRespCallsData_V1_2, 0, sizeof(mRespCallsData_V1_2));
    memset(mRespCallsData_V1_6, 0, sizeof(mRespCallsData_V1_6));
}

int GetCurrentCallListHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    EmergencyCallHelper* emergencyCallHelper = csc.GetEmergencyCallHelper();
    if (emergencyCallHelper != NULL && emergencyCallHelper->IsUnderWaiting()) {
        CallList calllist;
        emergencyCallHelper->GetFakeDialingCallList(&calllist);
        int nLength = 0;
        char *response = BuildCallListResponse(&calllist, &nLength);
        OnRequestComplete(RIL_E_SUCCESS, response, nLength);
        return 0;
    }

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildGetCallList();
    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0)
    {
        if (csc.IsRadioOffOrNotAvailable()) {
            OnRequestComplete(RIL_E_SUCCESS, NULL, 0);
            return 0;
        }

        return -1;
    }
    return 0;
}

int GetCurrentCallListHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ModemData *pModemData = msg->GetModemData();

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    CallList *currCallList = csc.GetCallList();
    if (currCallList == NULL) {
        RilLogW("Invalid CallList");
        return -1;
    }

    currCallList->Clear();
    ProtocolGetCurrentCallAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int cardState = mRilContext->GetUiccCardState();
    int phoneId = mService->GetRilSocketId();

    //update ACTIVE CALL cid
    RilProperty *property = mService->GetRilContextProperty();
    if (property != NULL)
    {
        property->Put(RIL_CONTEXT_CS_ACTIVE_CID, -1);
    }

    if (errorCode != RIL_E_SUCCESS || adapter.HasValidLength() == false)
    {
        RilLogE("[%s] %s Get Call List fail(errorCode:%d) or invalid length",
                mServiceName, __FUNCTION__, errorCode);
        currCallList->Clear();
        currCallList->m_nCount = 0;

        if (cardState == RIL_CARDSTATE_ABSENT) {
            // on CardStateAbsent return should be RadioError:NONE
            // For Emergency call this has some call list
            RilLogW("[%s] %s mCardState(%d) is not Present", mServiceName, __FUNCTION__, cardState);
            // Null Response is not allowed
            int nLength = 0;
            char *response = BuildCallListResponse(currCallList, &nLength);
            OnRequestComplete(RIL_E_SUCCESS, response, nLength);
        } else if (errorCode == RIL_E_RADIO_NOT_AVAILABLE) {
            OnRequestComplete(RIL_E_SUCCESS, NULL, 0);
        } else
            OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }

    currCallList->m_nCount = adapter.GetCallNum();
    if (currCallList->m_nCount >= CallList::MAX_CALL_LIST_COUNT)
    {
        RilLogE("[%s] %s Call List Count invalid (%d)", mServiceName, __FUNCTION__, currCallList->m_nCount);
        currCallList->Clear();
        currCallList->m_nCount = 0;
        if (cardState == RIL_CARDSTATE_ABSENT) {
            // on CardStateAbsent return should be RadioError:NONE
            // For Emergency call this has some call list
            RilLogI("[%s] %s mCardState(%d) is not Present", mServiceName, __FUNCTION__, cardState);
            // Null Response is not allowed
            int nLength = 0;
            char *response = BuildCallListResponse(currCallList, &nLength);
            OnRequestComplete(RIL_E_SUCCESS, response, nLength);
        } else
            OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    RilLogV("[%s] %s Current Call Count: %d", mServiceName, __FUNCTION__, currCallList->m_nCount);

    CallInfo *pCallInfo = currCallList->GetCallInfo();
    int callIdx;
    for (callIdx = 0; callIdx < currCallList->m_nCount; callIdx++, pCallInfo++)
    {
        if (0 >  adapter.GetCallInfo(pCallInfo, callIdx))
        {
            //TEST1:callIdx--;
            break;
        }
        RilLogV("[%s] %s < %d CallInfo >", mServiceName, __FUNCTION__, callIdx);
        adapter.DebugPrintCallInfo(pCallInfo);

        //update ACTIVE CALL cid
        if (pCallInfo != NULL && pCallInfo->m_state == RIL_CALL_ACTIVE)
        {
            if (property != NULL)
            {
                RilLogV("[%s] %s Set Active CID=%d in Context for socket_id(%d)",
                        mServiceName, __FUNCTION__, callIdx, phoneId);
                property->Put(RIL_CONTEXT_CS_ACTIVE_CID, callIdx);
            }
        }
    }

    // TEST1: if there are not enough call info, just return with valid(existing) count
    /*if (callIdx != m_currCallList->m_nCount)
    {
        m_currCallList->m_nCount = callIdx;
    }*/

    CallId *callId = csc.GetCallId();
    if (callId != NULL) {
        callId->SyncReady();
        int id = 0;
        pCallInfo = currCallList->GetCallInfo();
        for (int i = 0; i < adapter.GetCallNum(); i++, pCallInfo++)
        {
            id = pCallInfo->m_nIndex;
            pCallInfo->m_nIndex = callId->AddCallId(id);
        }
        callId->SyncDone();
    }

    int nLength = 0;
    char *response = BuildCallListResponse(currCallList, &nLength);
    //char *response = BuildCallListResponseTest(m_currCallList, &nLength);
    OnRequestComplete(RIL_E_SUCCESS, response, nLength);

    return 0;
}

char *GetCurrentCallListHandler::BuildCallListResponse(CallList *data, int *len)
{
    RilLogI("[%s] %s", mServiceName, __FUNCTION__);
    if (data == NULL) {
        return NULL;
    }

    int halVer = mService->GetCurrentRequestDataHalVersion(RilApplication::RIL_HalVersionCode);
    unsigned int nSkipCallCnt = 0;
    if (data->m_nCount <= 0) {
        *len = 0;
        return NULL;
    }
    else {
        int szRilCall = sizeof(RIL_Call *);
        char *ret = NULL;

        memset(mRespCalls, 0, sizeof(mRespCalls));
        memset(mRespCallsData, 0, sizeof(mRespCallsData));

        for (int i = 0; i < data->m_nCount; i++) {
            mRespCallsData[i].state = (RIL_CallState)data->m_szCallInfo[i].m_state;//(RIL_CallState)(data->m_pCallInfo[i
            mRespCallsData[i].index = data->m_szCallInfo[i].m_nIndex;
            mRespCallsData[i].toa = data->m_szCallInfo[i].m_toa;
            mRespCallsData[i].isMpty = (data->m_szCallInfo[i].m_isMParty == true)? 1: 0;
            mRespCallsData[i].isMT = (data->m_szCallInfo[i].m_isMt == true)? 1: 0;
            mRespCallsData[i].als = data->m_szCallInfo[i].m_als;
            mRespCallsData[i].isVoice = (data->m_szCallInfo[i].m_isVoice == true)? 1: 0;
            mRespCallsData[i].isVoicePrivacy = (data->m_szCallInfo[i].m_isVoicePrivacy == true)? 1: 0;
            mRespCallsData[i].number = data->m_szCallInfo[i].m_number;
            mRespCallsData[i].numberPresentation = data->m_szCallInfo[i].m_numPresent;
            if (data->m_szCallInfo[i].m_name[0] != '\0') {
                mRespCallsData[i].name = data->m_szCallInfo[i].m_name;
            }
            mRespCallsData[i].namePresentation = data->m_szCallInfo[i].m_namePresent;
            mRespCallsData[i].uusInfo = NULL;

            mRespCalls[i] = &mRespCallsData[i];
        } // end for ~

        // V1_2
        memset(mRespCalls_V1_2, 0, sizeof(mRespCalls_V1_2));
        memset(mRespCallsData_V1_2, 0, sizeof(mRespCallsData_V1_2));

        for (int i = 0; i < data->m_nCount; i++) {
            memmove(&mRespCallsData_V1_2[i], &mRespCallsData[i], sizeof(RIL_Call));
            mRespCallsData_V1_2[i].audioQuality = data->m_szCallInfo[i].m_audioQuality;
            if (mRespCallsData_V1_2[i].audioQuality == UNSPECIFIED) {
                mRespCallsData_V1_2[i].audioQuality = (RIL_AudioQuality)GetAudioQuality();
            }

            mRespCalls_V1_2[i] = &mRespCallsData_V1_2[i];

        }
        szRilCall = sizeof(RIL_Call_V1_2 *);
        ret = (char *)mRespCalls_V1_2;

        if (halVer >= HAL_VERSION_CODE(1, 6)) {
            memset(mRespCalls_V1_6, 0, sizeof(mRespCalls_V1_6));
            memset(mRespCallsData_V1_6, 0, sizeof(mRespCallsData_V1_6));

            for (int i = 0; i < data->m_nCount; i++) {
                memmove(&mRespCallsData_V1_6[i], &mRespCallsData_V1_2[i], sizeof(RIL_Call_V1_2));
                if (data->m_szCallInfo[i].m_forwardedNumber[0] != '\0') {
                    mRespCallsData_V1_6[i].forwardedNumber = data->m_szCallInfo[i].m_forwardedNumber;
                }
                mRespCalls_V1_6[i] = &mRespCallsData_V1_6[i];
            }
            szRilCall = sizeof(RIL_Call_V1_6 *);
            ret = (char *)mRespCalls_V1_6;
        }

        for (int i = 0; i < data->m_nCount; i++) {
            RilLogV("[%s] %s -------- %d / %d --------", mServiceName, __FUNCTION__, i+1, data->m_nCount);
            RilLogV("[%s] %s Call list %d: index %d, state %s(%d), TOA %d, isMpty %d",
                    mServiceName, __FUNCTION__, i, mRespCallsData[i].index,
                    CallUtils::CallStateToString(mRespCallsData[i].state), mRespCallsData[i].state,
                    mRespCallsData[i].toa, mRespCallsData[i].isMpty);
            RilLogV("[%s] %s isMT %d, als %d, isVoice %d, isVoicePrivacy %d, ",
                    mServiceName, __FUNCTION__, mRespCallsData[i].isMT, mRespCallsData[i].als,
                    mRespCallsData[i].isVoice, mRespCallsData[i].isVoicePrivacy);
            RilLogV("[%s] %s number %s, numberPresent %d, name %s, namePresent %d, uusInfo ...",
                    mServiceName, __FUNCTION__, mRespCallsData[i].number,
                    mRespCallsData[i].numberPresentation, mRespCallsData[i].name,
                    mRespCallsData[i].namePresentation);
            if (RilApplication::RIL_HalVersionCode >= HAL_VERSION_CODE(1, 2)) {
                RilLogV("[%s] %s audioQuality %d",
                        mServiceName, __FUNCTION__, mRespCallsData_V1_2[i].audioQuality);
            }
            if (RilApplication::RIL_HalVersionCode >= HAL_VERSION_CODE(1, 6)) {
                RilLogV("[%s] %s forwardedNumber %s",
                        mServiceName, __FUNCTION__, mRespCallsData_V1_6[i].forwardedNumber);
            }
        }

        *len = (data->m_nCount - nSkipCallCnt) * szRilCall;
        return ret;
    }
}

int GetCurrentCallListHandler::GetAudioQuality()
{
    RIL_AudioQuality audioQuality = UNSPECIFIED;
    int quality = AUDIO_QUALITY_NB;
    int callType = AUDIO_CALL_TYPE_UNKNOWN;

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return callType;
    }

    AudioService* pAudioService = csc.FindAudioService();
    if (pAudioService != NULL) {
        quality = pAudioService->GetVoiceQuality();
        callType = pAudioService->GetVoiceCallType();
    }

    if (callType == AUDIO_CALL_TYPE_GSM || callType == AUDIO_CALL_TYPE_CDMA) {
        switch (quality) {
            case AUDIO_QUALITY_NB:
                audioQuality = AMR;
                break;
            case AUDIO_QUALITY_WB:
            case AUDIO_QUALITY_SWB:
            case AUDIO_QUALITY_FB:
                audioQuality = AMR_WB;
                break;
            default:
                break;
        }
    }
    return audioQuality;
}

/**
 * DialHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DialHandler, MSG_CS_CALL_DIAL)

int DialHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    CallDialReqData* pCallReqData = (CallDialReqData*)msg->GetRequestData();
    CallType cType = CALL_TYPE_VOICE;
    int currentRat = RADIO_TECH_UNKNOWN;
    RilProperty *property = mService->GetRilContextProperty();
    if (property != NULL)
    {
        currentRat = property->GetInt(RIL_CONTEXT_NET_VOICE_RADIO_TECH);
        if(isCdmaVoice(currentRat))
        {
            cType = CALL_TYPE_CDMA_VOICE;
        }
    }

    RilLogV("[%s] %s Dial To : %s , Type(%s)", mServiceName, __FUNCTION__,
            pCallReqData->GetNumber(), GetCallTypeString(cType));

    ProtocolCallBuilder builder;
    UINT eccCat = 0;    //default emergency center

    ClirType dialClirType = pCallReqData->GetClirType();
    if (dialClirType == CLIR_DEFAULT)
    {
        ClirInfo clirInfo;
        dialClirType = (ClirType)clirInfo.GetClirAoc(mService->GetRilSocketId());
    }
    UusInfo uusInfo = pCallReqData->GetUusInfo();
    ModemData *pModemData = builder.BuildDial(pCallReqData->GetNumber(), dialClirType, uusInfo, cType, eccCat);

    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0)
    {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(DialHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * EmergencyDialHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(EmergencyDialHandler, MSG_CS_CALL_EMERGENCY_DIAL)

int EmergencyDialHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    CallEmergencyDialReqData* pECallReqData = (CallEmergencyDialReqData*)msg->GetRequestData();

    bool normalRoutingWhenInService = false;
    char* number = pECallReqData->GetNumber();
    CallType cType = CALL_TYPE_EMERGENCY;
    int currentRat = RADIO_TECH_UNKNOWN;

    //  Set emergency call routing except test number and valuable category.
    bool isTestingMode = pECallReqData->GetIsTesting();
#ifdef SUPPORT_CARRIER_TEST
    string carrierTestingEcc =
        SystemProperty::Get("vendor.radio.testing.ecc", "");
    bool isCarrierTestingEcc = false;
    if (number != NULL && carrierTestingEcc.length() > 0) {
        if (carrierTestingEcc.compare(number) == 0) {
            isCarrierTestingEcc = true;
        }
    }
    // To support EMC 922 on VzW E2E official test
    isTestingMode = isTestingMode && !(isCarrierTestingEcc &&
                                      (isCarrier(SITRIL_CARRIER_US_VZW) ||
                                       isCarrier(SITRIL_CARRIER_US_DISH)));
    RilLogV("[%s] %s %s() isCarrierTestingEcc=%d, isTestingMode=%d",
            mServiceName, TAG, __FUNCTION__, isCarrierTestingEcc, isTestingMode);
#endif

    bool isEmergencyCallRouting = true;
    NetworkService* pNetworkService = csc.FindNetworkService();

    if (isTestingMode || pECallReqData->GetRouting() == RIL_EMERGENCY_CALL_ROUTING_NORMAL) {
        isEmergencyCallRouting = false;
    } else if (pECallReqData->GetRouting() == RIL_EMERGENCY_CALL_ROUTING_UNKNOWN) {
        if (pECallReqData->GetCategories() == RIL_EMERGENCY_CATEGORY_UNSPECIFIED) {
           if (pNetworkService != NULL) {
               isEmergencyCallRouting = pNetworkService->IsEmergencyCallRouting(
                 number, &normalRoutingWhenInService);
           }
        }
    }

    cType = isEmergencyCallRouting ? CALL_TYPE_EMERGENCY : CALL_TYPE_VOICE;

    RilProperty *property = mService->GetRilContextProperty();
    if (property != NULL)
    {
        currentRat = property->GetInt(RIL_CONTEXT_NET_VOICE_RADIO_TECH);
        if(isCdmaVoice(currentRat))
        {
            cType = isEmergencyCallRouting ? CALL_TYPE_CDMA_EMERGENCY : CALL_TYPE_CDMA_VOICE;
        }
    }
    if (!isEmergencyCallRouting) {
        RilLogV("[%s] %s %s() It is normal routing", mServiceName, TAG, __FUNCTION__);
    }

    ProtocolCallBuilder builder;

    UINT eccCat = pECallReqData->GetCategories();
    if (eccCat == RIL_EMERGENCY_CATEGORY_UNSPECIFIED) {
        if (pNetworkService != NULL) {
            eccCat = pNetworkService->GetEmergencyNumberCategory(number);
            if (eccCat != RIL_EMERGENCY_CATEGORY_UNSPECIFIED) {
                RilLogV("[%s] %s %s() category is unpecified from database, set new eccCat: %s",
                        mServiceName, TAG, __FUNCTION__, DbEccInfoLoader::EmergencyCategoryToString(eccCat));
            }
        }
    }

    RilLogV("[%s] %s %s() To : %s, Type: %s, category: %s, normalRoutingWhenInService: %s",
            mServiceName, TAG, __FUNCTION__, number, GetCallTypeString(cType),
            DbEccInfoLoader::EmergencyCategoryToString(eccCat),
            (normalRoutingWhenInService ? "true" : "false"));

    ClirType dialClirType = pECallReqData->GetClirType();
    if (dialClirType == CLIR_DEFAULT) {
        ClirInfo clirInfo;
        dialClirType = (ClirType)clirInfo.GetClirAoc(mService->GetRilSocketId());
    }

    UusInfo uusInfo = pECallReqData->GetUusInfo();
    UINT eCallui = 0;
    int regState = RIL_NOT_REG_AND_NOT_SEARCHING;
    if (!isTestingMode) {
        RilProperty *property = mService->GetRilContextProperty();
        if (property != NULL) {
            regState = property->GetInt(RIL_CONTEXT_NET_VOICE_REGISTRATION_STATE,
                                        RIL_NOT_REG_AND_NOT_SEARCHING);
        }

        RilLogI("[%s] %s %s() regState is (%d)", mServiceName, TAG, __FUNCTION__, regState);

        if (!isEmergencyCallRouting &&
            isNwPlmnAllowedForNormalRoutingEccUnderFDN() &&
            (regState == RIL_REG_HOME || regState == RIL_REG_ROAMING)) {
            eCallui = 1;
        }
    }
    RilLogI("[%s] %s %s() eCallui is (%d)", mServiceName, TAG, __FUNCTION__, eCallui);
    ModemData *pModemData = builder.BuildDial(number, dialClirType, uusInfo, cType, eccCat, eCallui);

    EmergencyCallHelper* emergencyCallHelper = csc.GetEmergencyCallHelper();
    if (emergencyCallHelper != NULL &&
        emergencyCallHelper->NeedToWaitRegi(cType, normalRoutingWhenInService)) {
        emergencyCallHelper->WaitForEmergencyCall(number, dialClirType, uusInfo,
                              eccCat, pECallReqData->GetIsTesting());
        OnRequestComplete(RIL_E_SUCCESS);
        return 0;
    }

    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0 ) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(EmergencyDialHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

int EmergencyDialHandler::isNwPlmnAllowedForNormalRoutingEccUnderFDN() {
    RilProperty *property = mService->GetRilContextProperty();
    if (property != NULL) {
        string nwOperator = property->GetString(RIL_CONTEXT_NET_CURRENT_PLMN);
        const char *strNwPlmn = nwOperator.c_str();
        RilLogV("[%s] %s %s() strNwPlmn:%s", mServiceName,TAG,  __FUNCTION__, strNwPlmn);
        // Check mcc is TW or SG because they are in allowed list.
        if (strNwPlmn != NULL) {
            if (strncmp(strNwPlmn, "466", 3) == 0) {
                return true;
            }
        }
    }
    return false;
}

/**
 * AnswerHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(AnswerHandler, MSG_CS_CALL_ANSWER)

int AnswerHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildAnswer();
    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0)
    {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(AnswerHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * ExplicitCallTransferHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ExplicitCallTransferHandler, MSG_CS_SS_EXPLICIT_CALL_TRANSFER)

int ExplicitCallTransferHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    int cardState = mRilContext->GetUiccCardState();

    // On Sim absent, requesting command into CP cause IPC TIMEOUT
    // So return early,
    if (cardState != RIL_CARDSTATE_PRESENT) {
        RilLogI("[%s] %s mCardState(%d) is not Present(%d)", mServiceName, __FUNCTION__, cardState, RIL_CARDSTATE_PRESENT);
        OnRequestComplete(RIL_E_INVALID_STATE);
        return 0;
    }

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildExplicitCallTransfer();
    if (SendRequest(pModemData, TIMEOUT_CS_SUPPLEMENTARY, mMessageId) < 0)
    {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(ExplicitCallTransferHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * HangupHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(HangupHandler, MSG_CS_CALL_HANGUP)

int HangupHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    EmergencyCallHelper* emergencyCallHelper = csc.GetEmergencyCallHelper();
    if (emergencyCallHelper != NULL) {
        emergencyCallHelper->Abort();
    }

    CallList *callList = csc.GetCallList();
    CallId *callId = csc.GetCallId();
    if (callList == NULL || callId == NULL) {
        RilLogW("Invalid CallList or CallId");
        return -1;
    }

    // Remove temp ECC
    NetworkService* pNetworkService = csc.FindNetworkService();
    if (pNetworkService != NULL) {
        pNetworkService->SetTempEmergencyNumber(0 /*remove*/);
    }

    IntRequestData *data = (IntRequestData *) msg->GetRequestData();

    if (callList->m_nCount < 1)
    {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }
    else if (callList->m_nCount== 1)
    {
        return DoReleaseCall(callId->GetCpIndex(data->GetInt()));
    }
    else
    {
        return DoReleaseCallMulti(callId->GetCpIndex(data->GetInt()));
    }
    return 0;
}

int HangupHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ModemData *pModemData = msg->GetModemData();

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    CallList *callList = csc.GetCallList();
    CallId *callId = csc.GetCallId();
    if (callList == NULL || callId == NULL) {
        RilLogW("Invalid CallList or CallId");
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    if (errorCode == RIL_E_SUCCESS)
    {
        // clear released call information before updating by framework
        //
        // VTS test runtime doesn't guarantee calling getCurrentCalls
        // even if received RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED
        // VtsHalRadioV1_4Target#RadioHidlTest_v1_4.setPreferredNetwrokTypeBitmap
        // can be affected and the result become failed.
        IntRequestData *requestData = (IntRequestData *)mService->GetCurrentRequestData();
        if (requestData != NULL) {
            int callIdex = requestData->GetInt() - 1;
            RilLogV("[%s] %s clear released call info: callId=%d", mServiceName, __FUNCTION__, callIdex);
            int numOfCalls = callList->GetCount();
            if (numOfCalls > 0) {
                // clear CallInfo
                if (callIdex >= 0 && callIdex < numOfCalls) {
                    CallInfo &currentCall = callList->GetCallInfo()[callIdex];
                    currentCall.Clear();
                    // decrease active call count
                    callList->m_nCount--;
                }
            }
        }
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else
    {
        OnRequestComplete(errorCode== RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

int HangupHandler::DoReleaseCall(int callId)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildHangup(callId);
    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0)
    {
        return -1;
    }
    return 0;
}

int HangupHandler::DoReleaseCallMulti(int callId)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildHangupMulti(callId);
    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0)
    {
        return -1;
    }
    return 0;
}

/**
 * GetLastCallFailCauseHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetLastCallFailCauseHandler, MSG_CS_LAST_CALL_FAIL_CAUSE)

int GetLastCallFailCauseHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    // Send fake last call fail cause for IMS redial.
    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    EmergencyCallHelper* emergencyCallHelper = csc.GetEmergencyCallHelper();
    if (emergencyCallHelper != NULL && emergencyCallHelper->IsWaitingRedialIms()) {
        emergencyCallHelper->ResetWaitingRedialIms();
        int status = SIT_CALL_LAST_CALL_FAIL_EMC_REDIAL_ON_IMS;
        OnRequestComplete(RIL_E_SUCCESS, &status, sizeof(int));
        return 0;
    }

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildLastCallFailCause();
    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0 )
    {
        if (csc.IsRadioOffOrNotAvailable()) {
            int status = CALL_FAIL_RADIO_OFF;
            OnRequestComplete(RIL_E_SUCCESS, &status, sizeof(int));
            return 0;
        }

        return -1;
    }
    return 0;
}

int GetLastCallFailCauseHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ModemData *pModemData = msg->GetModemData();

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    ProtocolGetLastCallFailCauseAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int cause = adapter.GetLastCallFailCause();
    if (errorCode != RIL_E_SUCCESS ||
            (cause != SIT_CALL_LAST_CALL_FAIL_EMC_REDIAL_ON_IMS &&
             cause != SIT_CALL_LAST_CALL_FAIL_EMC_REDIAL_ON_VOWIFI)) {
        // Remove temp ECC
        NetworkService* pNetworkService = csc.FindNetworkService();
        if (pNetworkService != NULL) {
            pNetworkService->SetTempEmergencyNumber(0 /*remove*/);
        }
    }

    if (errorCode == RIL_E_SUCCESS) {
        int status = adapter.GetLastCallFailCause();
        OnUnsolicitedResponse(RIL_UNSOL_OEM_LAST_CALL_FAIL_CAUSE, &status, sizeof(int));
        OnRequestComplete(RIL_E_SUCCESS, &status, sizeof(int));
    } else if (errorCode == RIL_E_RADIO_NOT_AVAILABLE) {
        int status = CALL_FAIL_RADIO_OFF;
        OnUnsolicitedResponse(RIL_UNSOL_OEM_LAST_CALL_FAIL_CAUSE, &status, sizeof(int));
        OnRequestComplete(RIL_E_SUCCESS, &status, sizeof(int));
    } else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * RingbackToneHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(RingbackToneHandler, MSG_CS_SOUND_RINGBACKTONE_NTF)

int RingbackToneHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ModemData *pModemData = msg->GetModemData();

    ProtocolSoundRingbackToneIndAdapter adapter(pModemData);
    int ringback_state = adapter.GetRingbackToneState();
    // legacy if flags is negative value
    int flag = adapter.GetFlag();
    RilLogV("[%s] %s ringback_state=%d flag=%d", mServiceName, __FUNCTION__, ringback_state, flag);

    // notify ringback tone status except call altering state
    if (!(flag == RINGBACK_FLAG_PLAY_INBAND_BY_NW &&
            ringback_state == RIL_SND_RINGBACK_TONE_START)) {
        int state = ringback_state;
        OnUnsolicitedResponse(RIL_UNSOL_RINGBACK_TONE, (char *)&state, sizeof(state));
    }

    // notify ringback tone to OEM with flag
    if (flag == RINGBACK_FLAG_PLAY_INBAND_BY_NW) {
        int resp[] = {ringback_state, flag};
        OnUnsolicitedResponse(RIL_UNSOL_OEM_RINGBACK_TONE_BY_NETWORK, resp, sizeof(resp));
    }

    return 0;
}

/**
 * CallStateChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CallStateChangedHandler, MSG_CS_CALL_STATE_CHANGE_NTF)

int CallStateChangedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED);

    return 0;
}

/**
 * CallRingingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CallRingingHandler, MSG_CS_CALL_RINGING_NTF)

int CallRingingHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    OnUnsolicitedResponse(RIL_UNSOL_CALL_RING);
    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED);

    return 0;
}

/**
 * EmergencySupportRatModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(EmergencySupportRatModeHandler, MSG_CS_IND_EMERGENCY_SUPPORT_RAT_MODE)

int EmergencySupportRatModeHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ModemData *pModemData = msg->GetModemData();

    ProtocolEmergencySupportRatModeIndAdapter adapter(pModemData);
    int supportRatMode = adapter.GetSupportRatMode();
    RilLogV("[%s] %s Support RAT Mode=%d", mServiceName, __FUNCTION__, supportRatMode);

    OnUnsolicitedResponse(RIL_UNSOL_EMERGENCY_SUPPORT_RAT_MODE, &supportRatMode, sizeof(int) * 1);

    return 0;
}

/**
 * EmergencyCallListReceivedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(EmergencyCallListReceivedHandler, MSG_NET_EMERGENCY_CALL_LIST_RECEIVED)

int EmergencyCallListReceivedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ModemData *pModemData = msg->GetModemData();

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    EccInfo *radioEccInfos = NULL;
    NetworkService* pNetworkService = csc.FindNetworkService();
    if (pNetworkService != NULL) {
        radioEccInfos = pNetworkService->GetRadioEccInfos();
    }
    if (radioEccInfos == NULL) {
        return -1;
    }
    radioEccInfos[SIT_ECC_INFO_TYPE_ENL].Clear();
    radioEccInfos[SIT_ECC_INFO_TYPE_EENL].Clear();

    ProtocolEmergencyCallListIndAdapter adapter(pModemData);

    if (adapter.GetVersion() == 0) {
        RilLogE("[%s] %s  indication has invalid version.", mServiceName, __FUNCTION__);
        return -1;
    }

    adapter.GetEccInfo(radioEccInfos);

    if (pNetworkService != NULL) {
        pNetworkService->UpdateEmergencyNumberList();
    }

    return 0;
}

/**
 * UdubHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UdubHandler, MSG_CS_SS_UDUB)

int UdubHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildUdub();
    if (SendRequest(pModemData, TIMEOUT_CS_SUPPLEMENTARY, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int UdubHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ModemData *pModemData = msg->GetModemData();

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ? RIL_E_RADIO_NOT_AVAILABLE : RIL_E_INTERNAL_ERR);
    }
    return 0;
}

/**
 * HangupFgResumeBgHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(HangupFgResumeBgHandler, MSG_CS_SS_HANGUP_FOREGROUND_RESUME_BACKGROUND)

int HangupFgResumeBgHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    // Remove temp ECC
    NetworkService* pNetworkService = csc.FindNetworkService();
    if (pNetworkService != NULL) {
        pNetworkService->SetTempEmergencyNumber(0);
    }

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildHangupForegroundResumeBackground();
    if (SendRequest(pModemData, TIMEOUT_CS_SUPPLEMENTARY, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(HangupFgResumeBgHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * HangupWaitOrBgHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(HangupWaitOrBgHandler, MSG_CS_SS_HANGUP_WAITING_OR_BACKGROUND)

int HangupWaitOrBgHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildHangupWaitingOrBackground();
    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(HangupWaitOrBgHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SwitchWaitOrHoldAndActiveHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SwitchWaitOrHoldAndActiveHandler, MSG_CS_SS_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE)

int SwitchWaitOrHoldAndActiveHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildSwitchWaitingOrHoldingAndActive();
    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SwitchWaitOrHoldAndActiveHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * ConferenceHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ConferenceHandler, MSG_CS_SS_CONFERENCE)

int ConferenceHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildConference();
    if (SendRequest(pModemData, TIMEOUT_CS_SUPPLEMENTARY, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(ConferenceHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SeparateConnectionHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SeparateConnectionHandler, MSG_CS_SS_SEPARATE_CONNECTION)

int SeparateConnectionHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    CscServiceProxy csc(mService);
    if (!csc.HasService()) {
        return -1;
    }

    CallList *callList = csc.GetCallList();
    CallId *callId = csc.GetCallId();
    if (callList == NULL || callId == NULL) {
        RilLogE("callList or callId is NULL");
        return -1;
    }

    if (callList->m_nCount < 2)    /* if call is less than 2, call cannot be separated */ {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }
    else {
        ProtocolCallBuilder builder;
        IntRequestData *rildata = (IntRequestData *) msg->GetRequestData();
        ModemData *pModemData = builder.BuildSeparateConnection(callId->GetCpIndex(rildata->GetInt()));
        if (SendRequest(pModemData, TIMEOUT_CS_SUPPLEMENTARY, mMessageId) < 0) {
            OnRequestComplete(RIL_E_MODEM_ERR);
            return 0;
        }
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SeparateConnectionHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetCallConfirmHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCallConfirmHandler, MSG_CS_SEND_CALL_CONFIRM)

int SetCallConfirmHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildSendCallConfirm();
    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetCallConfirmHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ModemData *pModemData = msg->GetModemData();

    ProtocolSendCallConfirmRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int result = adapter.GetResult();
        OnRequestComplete(RIL_E_SUCCESS, &result, sizeof(result));
    }
    else {
        OnRequestComplete(errorCode == RIL_E_RADIO_NOT_AVAILABLE ?
                          RIL_E_RADIO_NOT_AVAILABLE : RIL_E_GENERIC_FAILURE);
    }
    return 0;
}

/**
 * UnsolCallPresentHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolCallPresentHandler, MSG_CS_CALL_PRESENT_NTF)

int UnsolCallPresentHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    OnUnsolicitedResponse(RIL_UNSOL_CALL_PRESENT_IND);
    return 0;
}

/**
 * UnsolEnterEmergencyCallbackModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolEnterEmergencyCallbackModeHandler, MSG_CS_IND_ENTER_EMERGENCY_CB_MODE_NTF)

int UnsolEnterEmergencyCallbackModeHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    OnUnsolicitedResponse(RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE);
    return 0;
}

/**
 * UnsolExitEmergencyCallbackModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolExitEmergencyCallbackModeHandler, MSG_CS_IND_EXIT_EMERGENCY_CB_MODE_NTF)

int UnsolExitEmergencyCallbackModeHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    OnUnsolicitedResponse(RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE);
    return 0;
}

/**
 * ExitEmergencyCallbackModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ExitEmergencyCallbackModeHandler, MSG_CS_EXIT_EMERGENCY_CB_MODE)

int ExitEmergencyCallbackModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    ProtocolCallBuilder builder;
    ModemData *pModemData = builder.BuildExitEmergencyCbMode();
    if (SendRequest(pModemData, TIMEOUT_CALL_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int ExitEmergencyCallbackModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ModemData *pModemData = msg->GetModemData();

    ProtocolExitEmergencyCbModeRespAdapter adapter(pModemData);
    int errorCode = adapter.GetRilErrorCode();

    if (errorCode == RIL_E_SUCCESS) {
        int result = adapter.GetResult();
        OnRequestComplete(RIL_E_SUCCESS, &result, sizeof(int));
    }
    else {
        RilLogE("[%s] %s errorCode=%d", mServiceName, __FUNCTION__, errorCode);
        switch (errorCode) {
            case RIL_E_REQUEST_NOT_SUPPORTED:
            case RIL_E_RADIO_NOT_AVAILABLE:
                OnRequestComplete(errorCode);
                break;
            default:
                OnRequestComplete(RIL_E_INTERNAL_ERR);
                break;
        }
    }
    return 0;
}
