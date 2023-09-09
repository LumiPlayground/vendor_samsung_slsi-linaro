/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "callutils.h"
#include "cscservice.h"
#include "rillog.h"
#include "emergencycallhelper.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

CscService::CscService(RilContext *pRilContext)
    : Service(pRilContext, RIL_SERVICE_CSC) {
    m_currCallList = NULL;
    m_emergencyCallHelper = NULL;
}

CscService::~CscService() {
    if (m_currCallList)
    {
        delete m_currCallList;
        m_currCallList = NULL;
    }
    if (m_emergencyCallHelper) {
        delete m_emergencyCallHelper;
        m_emergencyCallHelper = NULL;
    }
    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).unregisterMonitoringCallback(this);
}

int CscService::OnCreate(RilContext * /* pRilContext */) {
    RilLogI("[%s] %s", m_szSvcName, __FUNCTION__);

    m_currCallList = new CallList();
    m_currCallList->Clear();

    m_CallId.Init();

    m_emergencyCallHelper = new EmergencyCallHelper(this);

    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).registerMonitoringCallback(this, true);

    return 0;
}

BOOL CscService::OnHandleRequest(Message * /* pMsg */) {
    // TODO Implement handlers if necessary in the future
    return FALSE;
}

BOOL CscService::OnHandleSolicitedResponse(Message * /* pMsg */) {
    // TODO Implement handlers if necessary in the future
    return FALSE;
}

BOOL CscService::OnHandleUnsolicitedResponse(Message * /* pMsg */) {
    // TODO Implement handlers if necessary in the future
    return TRUE;
}

bool CscService::IsPossibleToPassInRadioOffState(int request_id) {
    switch (request_id) {
        case RIL_REQUEST_GET_CURRENT_CALLS:
        case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
            break;
        default:
            return false;
    }
    return true;
}

bool CscService::IsPossibleToPassInRadioUnavailableState(int request_id) {
    switch (request_id) {
        case RIL_REQUEST_GET_CURRENT_CALLS:
        case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
            return true;
    }
    return false;
}

BOOL CscService::IsInCallState() {
    if (m_currCallList == NULL || m_currCallList->GetCount() == 0) {
        RilLogE("[%s] %s() Call list is NULL or empty", m_szSvcName, __FUNCTION__);
        return FALSE;
    }

    RilLogV("[%s] %s() Call list Count == %d", m_szSvcName, __FUNCTION__, m_currCallList->GetCount());
    return TRUE;
}

bool CscService::IsRadioOnForEmergencyCall() {
    return m_emergencyCallHelper->IsRadioOnForEmergencyCall();
}

void CscService::SetRadioOnForEmergencyCall(bool radioOnForEmergencyCall) {
    m_emergencyCallHelper->SetRadioOnForEmergencyCall(radioOnForEmergencyCall);
}

void CscService::OnVoiceRegistrationStateChanged(int regState) {
    RilLogV("[%s] %s  regState=%d", m_szSvcName, __FUNCTION__, regState);
    if (regState == RIL_REG_HOME || regState == RIL_REG_ROAMING) {
        m_emergencyCallHelper->MakeEmergencyCall(false);
    }
}

void CscService::OnImsRegistrationStateChanged(int regState) {
    RilLogV("[%s] %s ims reg state:%d", m_szSvcName, __FUNCTION__, regState);
    if (regState == RIL_IMS_REGISTERED) {
        m_emergencyCallHelper->MakeEmergencyCall(false);
    }
}

void CscService::dump(std::stringstream& ss) {
    ss << GetServiceName() << ":{" << "\n";
    ss << "Call count=" << m_currCallList->GetCount() << "\n";
    for (int i = 0; i < m_currCallList->GetCount(); i++) {
        CallInfo &callInfo = m_currCallList->GetCallInfo()[i];
        ss << "Index=" << callInfo.m_nIndex;
        ss << ", State=" << CallUtils::CallStateToString(callInfo.m_state);
        ss << ", Toa=" << callInfo.m_toa;
        ss << ", IsMT=" << (callInfo.m_isMt ? "true" : "false");
        ss << ", NumPresent=" << CallUtils::getPresentationString(callInfo.m_numPresent);
        ss << ", NamePresent=" << CallUtils::getPresentationString(callInfo.m_namePresent);
        ss << ", Number=" << callInfo.m_number;
        ss << ", Name=" << callInfo.m_name;
        ss << "\n";
    }
    ss << "}";
}
