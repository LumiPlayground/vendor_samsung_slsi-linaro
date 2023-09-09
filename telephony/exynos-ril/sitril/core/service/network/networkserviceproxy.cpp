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
 * networkserviceproxy.cpp
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#include "cscservice.h"
#include "networkservice.h"
#include "networkserviceproxy.h"
#include "rillog.h"

NetworkServiceProxy::NetworkServiceProxy(Service *service)
    : mService(NULL)
{
    // mNetworkService should be not NULL
    mService = NetworkService::AsService(service);
}

NetworkServiceProxy::~NetworkServiceProxy()
{
    mService = NULL;
}

int NetworkServiceProxy::GetRadioReady()
{
    if (!HasService()) {
        return RADIO_NOT_READY;
    }
    return mService->GetRadioReady();
}

void NetworkServiceProxy::SetRadioReady(int state)
{
    if (HasService()) {
        mService->SetRadioReady(state);
    }
}

int NetworkServiceProxy::GetDesiredRadioState()
{
    if (!HasService()) {
        return RADIO_STATE_UNAVAILABLE;
    }
    return mService->m_desiredRadioState;
}

void NetworkServiceProxy::SetDesiredRadioState(int desiredRadioState)
{
    if (HasService()) {
        mService->m_desiredRadioState = (RIL_RadioState)desiredRadioState;
    }
}

bool NetworkServiceProxy::IsDelayedRadioPower()
{
    if (!HasService()) {
        return false;
    }
    return mService->mDelayedRadioPower;
}

void NetworkServiceProxy::SetDelayedRadioPower(bool delayed)
{
    if (HasService()) {
        mService->mDelayedRadioPower = delayed;
    }
}

int NetworkServiceProxy::GetRadioState()
{
    if (!HasService()) {
        return RADIO_STATE_UNAVAILABLE;
    }
    return mService->m_radioState;
}

void NetworkServiceProxy::UpdateRadioState(int radioState, bool notifyResult/* = false*/)
{
    if (HasService()) {
        mService->UpdateRadioState((RIL_RadioState)radioState, notifyResult);
    }
}

bool NetworkServiceProxy::IsRadioShutdown()
{
    if (!HasService()) {
        return false;
    }
    return mService->m_bShutdown;
}

void NetworkServiceProxy::SetRadioShutdown(bool flag)
{
    if (HasService()) {
        mService->m_bShutdown = flag;
    }
}

int NetworkServiceProxy::GetVoiceRegistrationState()
{
    if (!HasService()) {
        return UNKNOWN;
    }
    return mService->m_nRegState;
}

void NetworkServiceProxy::SetVoiceRegistrationState(int state)
{
    if (HasService()) {
        mService->m_nRegState = state;
    }
}

int NetworkServiceProxy::GetDataRegistrationState()
{
    if (!HasService()) {
        return UNKNOWN;
    }
    return mService->m_nDataRegState;
}

void NetworkServiceProxy::SetDataRegistrationState(int state)
{
    if (HasService()) {
        mService->m_nDataRegState = state;
    }
}

int NetworkServiceProxy::GetVoiceRadioTech()
{
    if (!HasService()) {
        return RADIO_TECH_UNKNOWN;
    }
    return mService->m_nVoiceRat;
}

void NetworkServiceProxy::SetVoiceRadioTech(int rat)
{
    if (HasService()) {
        mService->m_nVoiceRat = (RIL_RadioTechnology)rat;
    }
}

int NetworkServiceProxy::GetDataRadioTech()
{
    if (!HasService()) {
        return RADIO_TECH_UNKNOWN;
    }
    return mService->m_nDataRat;
}

void NetworkServiceProxy::SetDataRadioTech(int rat)
{
    if (HasService()) {
        mService->m_nDataRat = (RIL_RadioTechnology)rat;
    }
}

int NetworkServiceProxy::GetImsRegistrationState()
{
    int state = RIL_IMS_NOT_REGISTERED;
    if (HasService()) {
        RilProperty *property = mService->GetRilContextProperty();
        if (property != NULL) {
            state = property->GetInt(RIL_CONTEXT_IMS_REGISTRATION);
        }
    }
    return state;
}

void NetworkServiceProxy::NotifyVoiceRegistrationFailed(int regState, int rejectCause)
{
    if (HasService()) {
        mService->OnVoiceRegistrationCustomNotification(regState, rejectCause);
    }
}

void NetworkServiceProxy::WriteVolteEmcServiceStatus(bool isVopsSupported, int emcService,
        int rat, int regState, bool notify/* = false*/)
{
    if (HasService()) {
        mService->WriteVolteEmcServiceStatus(isVopsSupported, emcService, rat, regState, notify);
    }
}

bool NetworkServiceProxy::IsNrTestMode()
{
    if (!HasService()) {
        return false;
    }
    return mService->mIsNrTestMode;
}

bool NetworkServiceProxy::IsIwlanEnabled()
{
    if (!HasService()) {
        return false;
    }
    return mService->mIsWfcEnabled;
}

void NetworkServiceProxy::SetIwlanEnabled(bool enabled)
{
    if (HasService()) {
        mService->mIsWfcEnabled = enabled;
    }
}

void NetworkServiceProxy::UpdateDbEccInfo(bool forceToUpdate, bool callByOpposite) {
    if (HasService()) {
        return mService->UpdateDbEccInfo(forceToUpdate, callByOpposite);
    }
}

UINT NetworkServiceProxy::IsOppsiteStackOccupyRF()
{
    if (HasService()) {
        return mService->IsOppsiteStackOccupyRF();
    }
    return 0;
}

UINT NetworkServiceProxy::IsCurrentStackOccupyRF()
{
    if (HasService()) {
        return mService->IsCurrentStackOccupyRF();
    }
    return 0;
}

CscService *NetworkServiceProxy::FindCscService() {
    if (!HasService()) {
        return NULL;
    }
    return CscService::AsService(mService->GetCurrentService(RIL_SERVICE_CSC));
}
