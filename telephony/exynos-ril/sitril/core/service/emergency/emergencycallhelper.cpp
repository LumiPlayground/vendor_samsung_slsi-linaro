/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include "emergencycallhelper.h"
#include "cscservice.h"
#include "rillog.h"

#define EMERGENCY_CALL_DBG false
// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) if(EMERGENCY_CALL_DBG) \
        CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) if(EMERGENCY_CALL_DBG) \
        CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) if(EMERGENCY_CALL_DBG) \
        CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define MIN(a, b)            ((a) > (b) ? (b) : (a))

#define TIMEOUT_TICK_FOR_NORMAL_ROUTING_WITHOUT_SERVICE 2
#define TIMEOUT_TICK 6
#define TIMEOUT_CALL_DEFAULT 5000

static const char TAG[] = "EmergencyCallHelper";

static operator_wait_config operatorWaitConfigForEmergencyType[] = {
#include "../../database/operatorWaitConfigForEmergencyType.h"
};

static operator_wait_config operatorWaitConfigForNormalType[] = {
#include "../../database/operatorWaitConfigForNormalType.h"
};

static const char* WaitTypeToString(WAIT_TYPE config) {
    static const char* WAIT_TYPE[] = {"NONE", "CS", "VOLTE", "ANY"};
    if (WAIT_NONE <= config && config <= WAIT_ANY) {
        return WAIT_TYPE[config];
    }
    return "UNKNOWN";
}

void Timer::Start(const Interval &interval, const Function &function, unsigned int maxTick) {
    RilLogI("[%s] %s()", TAG, __FUNCTION__);
    mRunning = true;
    mTick = 0;

    mTh = thread([=]() {
        while (mRunning == true && mTick <= maxTick) {
            function(mTick == maxTick);
            if (mTick++ != maxTick) {
                this_thread::sleep_for(interval);
            }
        }
    });
    mTh.detach();
}
void Timer::Stop() {
    mRunning = false;
}

EmergencyCallHelper::EmergencyCallHelper(CscService *service) : mService(NULL), mIsTesting(false) {
    mService = service;
    Clear();
}

EmergencyCallHelper::~EmergencyCallHelper() {
    mService = NULL;
    Clear();
}

bool EmergencyCallHelper::IsRadioOnForEmergencyCall() {
    RilLogV("[%s] %s() mIsRadioOnForEmergencyCall:%d", TAG, __FUNCTION__, mIsRadioOnForEmergencyCall);
    return mIsRadioOnForEmergencyCall;
}

void EmergencyCallHelper::SetRadioOnForEmergencyCall(bool radioOnForEmergencyCall) {
    RilLogV("[%s] %s() radioOnForEmergencyCall:%d", TAG, __FUNCTION__, radioOnForEmergencyCall);
    mIsRadioOnForEmergencyCall = radioOnForEmergencyCall;
}

bool EmergencyCallHelper::IsWaitingRedialIms() {
    RilLogV("[%s] %s() mWaitingRedialIms:%d", TAG, __FUNCTION__, mWaitingRedialIms);
    return mWaitingRedialIms;
}

void EmergencyCallHelper::ResetWaitingRedialIms() {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    mWaitingRedialIms = false;
}

void EmergencyCallHelper::SetConfig(CallType callType, bool normalRoutingWhenInService) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    operator_wait_config *configTable = operatorWaitConfigForEmergencyType;
    size_t numOfConfig = std::size(operatorWaitConfigForEmergencyType);
    string simPlmn = mService->GetSimOperatorNumeric();
    RilLogV("[%s] %s() simPlmn is %s", TAG, __FUNCTION__, simPlmn.c_str());
    mWaitType = WAIT_NONE;

    if (!simPlmn.empty()) {
        if (callType == CALL_TYPE_VOICE || callType == CALL_TYPE_CDMA_VOICE) {
            RilLogV("[%s] %s() change default wait type to WAIT_ANY.", TAG, __FUNCTION__);
            mWaitType = WAIT_ANY;
            configTable = operatorWaitConfigForNormalType;
            numOfConfig = std::size(operatorWaitConfigForNormalType);
        } else if (normalRoutingWhenInService) {
            mWaitType = WAIT_VOLTE;
        }

        RilLogV("[%s] %s() numOfConfig is %d", TAG, __FUNCTION__, numOfConfig);
        for (size_t i = 0; i < numOfConfig; i++) {
            operator_wait_config config = configTable[i];
            if (strcmp(config.mccmnc, simPlmn.c_str()) == 0) {
                RilLogV("[%s] %s() found config wait_type:%d", TAG, __FUNCTION__,
                        config.wait_type);
                mWaitType = config.wait_type;
            }
        }
    }
}

WAIT_TYPE EmergencyCallHelper::GetConfig() {
    RilLogV("[%s] %s() mWaitType:%d", TAG, __FUNCTION__, mWaitType);
    return mWaitType;
}

bool EmergencyCallHelper::NeedToWaitRegi(CallType callType, bool normalRoutingWhenInService) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    if (!IsRadioOnForEmergencyCall()) {
        RilLogV("[%s] %s() IsRadioOnForEmergencyCall is false", TAG, __FUNCTION__);
        return false;
    }

    SetConfig(callType, normalRoutingWhenInService);

    WAIT_TYPE config = GetConfig();
    if (WAIT_NONE == config) {
        RilLogV("[%s] %s() WAIT_TYPE is NONE.", TAG, __FUNCTION__);
        return false;
    }

    if (IsOkToCall()) {
        RilLogV("[%s] %s() call is already available", TAG, __FUNCTION__);
        return false;
    }

    mCallType = callType;
    mNormalRoutingWhenInService = normalRoutingWhenInService;

    RilLogI("[%s] %s() waiting registration type: %s", TAG, __FUNCTION__, WaitTypeToString(config));
    return true;
}

void EmergencyCallHelper::MakeEmergencyCall(bool timeout) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);

    if (!mUnderWaiting) {
        RilLogV("[%s] %s() not under waiting state.", TAG, __FUNCTION__);
        return;
    }

    if (!(IsOkToCall() || timeout)) {
        RilLogV("[%s] %s() can't make call. timeout:%d", TAG, __FUNCTION__, timeout);
        return;
    }

    mTimer.Stop();
    mIsRadioOnForEmergencyCall = false;

    WAIT_TYPE config = GetConfig();
    if ((config == WAIT_ANY || config == WAIT_VOLTE) && mImsRegState == RIL_IMS_REGISTERED) {
        Clear();
        // Try VoLTE redial using CallFailCause.EMC_REDIAL_ON_IMS.
        // Empty call list is expected. Then redial will be happened.
        mService->OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED);
        // Set mWaitingRedialIms flag for last call fail cause.
        mWaitingRedialIms = true;
    } else {
        RIL_EmergencyDial reqData;
        reqData.dialInfo.address = mNumber;
        reqData.dialInfo.clir = mClir;
        reqData.dialInfo.uusInfo = &mUusInfo;
        reqData.categories = mEccType;
        reqData.isTesting = mIsTesting;
        mService->OnRequest(RIL_REQUEST_EMERGENCY_DIAL, &reqData, sizeof(reqData));

        // Clear later for dial args.
        Clear();
    }
}

void EmergencyCallHelper::WaitForEmergencyCall(char *number, ClirType clir, UusInfo uusInfo/*not used*/,
                                               UINT eccType, bool isTesting) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    unsigned int maxTick = TIMEOUT_TICK;

    if (mUnderWaiting) {
        RilLogV("[%s] %s() Already in waiting state", TAG, __FUNCTION__);
        return;
    }

    if ((mCallType == CALL_TYPE_EMERGENCY ||
           mCallType == CALL_TYPE_CDMA_EMERGENCY) &&
        mNormalRoutingWhenInService) {
        maxTick = TIMEOUT_TICK_FOR_NORMAL_ROUTING_WITHOUT_SERVICE;
    }

    mUnderWaiting = true;
    SetDialArgs(number, clir, uusInfo, eccType, isTesting);

    std::function<void(bool)> func = std::bind(&EmergencyCallHelper::MakeEmergencyCall,
                                      this, std::placeholders::_1);

    mTimer.Start(chrono::milliseconds(TIMEOUT_CALL_DEFAULT), func, maxTick);
}

bool EmergencyCallHelper::IsUnderWaiting() {
    RilLogV("[%s] %s() mUnderWaiting:%d", TAG, __FUNCTION__, mUnderWaiting);
    return mUnderWaiting;
}

void EmergencyCallHelper::SetDialArgs(char *number, ClirType clir, UusInfo uusInfo/*not used*/,
                                      UINT eccType, bool isTesting) {
    strlcpy(mNumber, number, sizeof(mNumber));
    mClir = clir;
    mEccType = eccType;
    mIsTesting = isTesting;
    mUusInfo.uusType = uusInfo.m_uusType;
    mUusInfo.uusDcs = uusInfo.m_uusDcs;
    mUusInfo.uusLength = uusInfo.m_uusLength;
    memcpy(mUusInfoData, uusInfo.m_uusData, uusInfo.m_uusLength);
    mUusInfo.uusData = mUusInfoData;
}

bool EmergencyCallHelper::IsOkToCall() {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    WAIT_TYPE config = GetConfig();
    RilProperty *property = mService->GetRilContextProperty();
    if (property != NULL) {
        int regState = property->GetInt(RIL_CONTEXT_NET_VOICE_REGISTRATION_STATE, 0);
        int imsRegState = property->GetInt(RIL_CONTEXT_IMS_REGISTRATION);
        RilLogI("[%s] %s() regState:%d imsRegState:%d", TAG, __FUNCTION__,
                regState, imsRegState);
        mImsRegState = imsRegState;

        if (config == WAIT_ANY) {
            if (imsRegState == RIL_IMS_REGISTERED || regState == RIL_REG_HOME || regState == RIL_REG_ROAMING) {
                return true;
            }
        } else if (config == WAIT_CS) {
            if (regState == RIL_REG_HOME || regState == RIL_REG_ROAMING) {
                return true;
            }
        } else if (config == WAIT_VOLTE) {
            if (imsRegState == RIL_IMS_REGISTERED) {
                return true;
            }
        }
    }
    return false;
}

void EmergencyCallHelper::Clear() {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    mUnderWaiting = false;
    mIsRadioOnForEmergencyCall = false;
    memset(mNumber, 0, MAX_DIAL_NUM);
    mClir = CLIR_DEFAULT;
    mUusInfo.uusType = RIL_UUS_TYPE1_IMPLICIT;
    mUusInfo.uusDcs = RIL_UUS_DCS_USP;
    mUusInfo.uusLength = 0;
    memset(mUusInfoData, 0, sizeof(mUusInfoData));
    mUusInfo.uusData = NULL;
    mEccType = 0;
    mWaitingRedialIms = false;
    mWaitType = WAIT_NONE;
    mImsRegState = RIL_IMS_NOT_REGISTERED;
    mTimer.Stop();
    mCallType = CALL_TYPE_EMERGENCY;
    mNormalRoutingWhenInService = false;
}

void EmergencyCallHelper::GetFakeDialingCallList(CallList* callList) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    callList->m_nCount = 1;
    CallInfo *pCallInfo = callList->GetCallInfo();
    pCallInfo->m_state = RIL_CALL_DIALING;
    pCallInfo->m_nIndex = 1;
    pCallInfo->m_toa = RIL_TOA_UNKNOWN;
    pCallInfo->m_als = 0;
    pCallInfo->m_isVoice = true;
    pCallInfo->m_numPresent = RIL_CALL_NAME_PRESENTATION_ALLOW;
    pCallInfo->m_namePresent = RIL_CALL_NAME_PRESENTATION_ALLOW;
    int len = MIN(strlen(mNumber), MAX_DIAL_NUM);
    memcpy(pCallInfo->m_number, mNumber, len);
}

void EmergencyCallHelper::Abort() {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    Clear();
}
