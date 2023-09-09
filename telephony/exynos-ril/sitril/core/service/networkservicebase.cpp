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
 * networkservicebase.cpp
 *
 *  Created on: 2021. 2. 5.
 */

#include <librilutils/radio_conv_utils.h>
#include "networkservicebase.h"
#include <librilutils/systemproperty.h>
#include "callreqdata.h"
#include "emergencynumbermanager.h"
#include "networkutils.h"
#include "protocolnetbuilder.h"
#include "rilapplication.h"
#include "rillog.h"
#include "servicemgr.h"
#include "servicestate.h"
#include "simservice.h"

#define TIMEOUT_NET_5SEC    5000

#define NET_SIM_EF_PNN    (0x6fc5)
#define NET_SIM_EF_OPL    (0x6fc6)

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define NULL_REQ(msg)    { if(msg==NULL || msg->GetRequestData()==NULL) { RilLogE("%s::%s() msg or RequestData is NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_RSP(msg)    { if(msg==NULL || msg->GetModemData()==NULL) { RilLogE("%s::%s() msg or ModemData is NULL", m_szSvcName, __FUNCTION__); return -1; } }

NetworkServiceBase::NetworkServiceBase(RilContext* pRilContext)
    : Service(pRilContext, RIL_SERVICE_NETWORK) {
    m_radioState = RADIO_STATE_UNAVAILABLE;
    m_desiredRadioState = RADIO_STATE_UNAVAILABLE;
    mDelayedRadioPower = false;
    m_nRadioReady = RADIO_NOT_READY;
    m_nVoiceRat = RADIO_TECH_UNKNOWN;
    m_nDataRat = RADIO_TECH_UNKNOWN;
    m_nDataRegState = NOT_REGISTERED;
    m_cardState = RIL_CARDSTATE_ABSENT;
    m_appState = RIL_APPSTATE_UNKNOWN;
    mCurrentSim = 0;

    m_bShutdown = false;
    m_nRegState = NOT_REGISTERED;

    mEmergencyNumberManager = NULL;
    mIsDisabledESim = false;

    mIsNrTestMode = SystemProperty::GetInt("persist.vendor.radio.nr_test", 0) == 1;
    RilLogV("[%s] mIsNrTestMode=%d", GetServiceName(), mIsNrTestMode);
}

NetworkServiceBase::~NetworkServiceBase() {
    if (mEmergencyNumberManager) {
        delete mEmergencyNumberManager;
        mEmergencyNumberManager = NULL;
    }
    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).unregisterMonitoringCallback(this);
}

int NetworkServiceBase::OnCreate(RilContext * /* pRilContext */) {
    RilLogI("[%s] %s", m_szSvcName, __FUNCTION__);

    mEmergencyNumberManager = new EmergencyNumberManager(this);

    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).registerMonitoringCallback(this, true);
    return 0;
}

void NetworkServiceBase::OnStart() {
    RilLogI("[%s] %s", m_szSvcName, __FUNCTION__);
}

void NetworkServiceBase::OnModemOnline() {
    RilLog("[%s] %s ", GetServiceName(), __FUNCTION__);
    if (m_nRadioReady != RADIO_READY) {
        RilLog("[%d] query current radio state from modem.", GetRilSocketId());
        OnRequest(RIL_REQUEST_OEM_GET_RADIO_STATE);
    }
}

void NetworkServiceBase::OnReset() {
    RilLog("[%s] %s", GetServiceName(), __FUNCTION__);
    const std::string reason = "modem_reset";
    OnUnsolicitedResponse(RIL_UNSOL_MODEM_RESTART, reason.c_str(), reason.length());

    RilLog("[%d] current radioState=%d m_radioState=%d", GetRilSocketId(), GetRadioState(), m_radioState);
    m_nRadioReady = RADIO_NOT_READY;
    UpdateRadioState(RADIO_STATE_UNAVAILABLE, true);
}

void NetworkServiceBase::OnRadioStateChanged(int radioState) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    m_radioState = (RIL_RadioState)radioState;
}

void NetworkServiceBase::OnRadioNotAvailable() {
    RilLog("[%s] %s", GetServiceName(), __FUNCTION__);
    m_cardState = RIL_CARDSTATE_ABSENT;
}

void NetworkServiceBase::OnRadioOffOrNotAvailable() {
    RilLog("[%s] %s", GetServiceName(), __FUNCTION__);

    m_desiredRadioState = RADIO_STATE_UNAVAILABLE;
    m_appState = RIL_APPSTATE_UNKNOWN;
    m_nVoiceRat = RADIO_TECH_UNKNOWN;
    m_nDataRat = RADIO_TECH_UNKNOWN;

    // clear VoLTE and EMC network status
    WriteVolteEmcServiceStatus(false, EMC_NOT_SUPPORTED, RADIO_TECH_UNKNOWN, NOT_REGISTERED, true);
}

void NetworkServiceBase::OnRadioAvailable() {
    RilLog("[%s] %s", GetServiceName(), __FUNCTION__);
    if (mDelayedRadioPower) {
        mDelayedRadioPower = false;
        if (m_desiredRadioState != GetRadioState()) {
            RilLogI("[%d] TrySetRadioPower due to a delayed request", GetRilSocketId());
            TrySetRadioPower(m_desiredRadioState);
        }
    }
}

void NetworkServiceBase::OnSimStatusChanged(int cardState, int appState) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    // Current USIM is present and
    // any app state -> SIM READY
    NetworkServiceBase* pOppNetworkService = (NetworkServiceBase*)GetOppositeService(RIL_SERVICE_NETWORK);
    if (cardState == RIL_CARDSTATE_PRESENT) {
        if ((m_appState != appState && appState == RIL_APPSTATE_READY) ||
            (m_cardState != cardState) ||
            (mIsDisabledESim != IsDisabledESim())) {
            // try to update current PS available state
            QueryCurrentPsDomainState();
            ResetDbEccId();
            if (pOppNetworkService != NULL) {
                pOppNetworkService->ResetDbEccId();
            }
        }
    } else if ( cardState == RIL_CARDSTATE_ABSENT ) {
        // RIL handle error for pending commands only when card is removed (e.g. hotswap)
        // ignore no SIM cases at start up
        if ( m_cardState == RIL_CARDSTATE_PRESENT ) {
            if ( m_pCurReqMsg != NULL ) {
                RilLogW("[%s] %s() return error for pending request (%d)", m_szSvcName, __FUNCTION__, m_pCurReqMsg->GetMsgId());
                OnRequestComplete(RIL_E_GENERIC_FAILURE);
            }

            //update Main SIM to invalid
            RilProperty *property = GetRilApplicationProperty();
            if (property != NULL) {
                RilLogV("[%s] Update Main SIM slot to unknown (%s,%d)", __FUNCTION__, RIL_APP_MAIN_SIM, RIL_SOCKET_NUM);
                property->Put(RIL_APP_MAIN_SIM, RIL_SOCKET_NUM);
            }
        }
    }

    m_cardState = cardState;
    m_appState = appState;
    mIsDisabledESim = IsDisabledESim();

    mEmergencyNumberManager->SetSimState(m_cardState, m_appState, mIsDisabledESim);
}

void NetworkServiceBase::OnServiceStateChanged(const ServiceState& state) {
    // template of OnServiceStateChanged callback
    RilLogV("[%s] %s  ServiceState=%s", GetServiceName(), __FUNCTION__, state.toString().c_str());
}

void NetworkServiceBase::SetRadioReady(int state) {
    int oldState = m_nRadioReady;
    m_nRadioReady = state;
    if (oldState != RADIO_READY && state == RADIO_READY) {
        OnRadioReady();
    }
}

void NetworkServiceBase::UpdateRadioState(RIL_RadioState radioState, bool notifyResult/* = false*/) {
    switch (radioState) {
    RilLogV("New radioState : %d", radioState);
    case RADIO_STATE_OFF:
        ResetDbEccId();
        break;
    case RADIO_STATE_UNAVAILABLE:
    case RADIO_STATE_ON:
        break;
    default:
        RilLogW("Unknown Radio State : %d", radioState);
        return ;
    }

    if (m_radioState != radioState) {
        RilLogV("UpdateRadioState : %d -> %d", m_radioState, radioState);
        m_radioState = radioState; //real RIL radio state

        ServiceMgr *serviceMgr = GetRilContext()->GetServiceManager();
        if (serviceMgr != NULL) {
            RilDataInts *data = new RilDataInts(1);
            if (data != NULL) {
                data->SetInt(0, (int)radioState);
                serviceMgr->BroadcastSystemMessage(MSG_SYSTEM_RADIO_STATE_CHANGED, data);
            }
        }

        if (notifyResult) {
            OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, (int *)&m_radioState, sizeof(int));
        }
    } else {
        RilLogV("Current Radio State : %d ", radioState);
    }
}

void NetworkServiceBase::TrySetRadioPower(int desiredRadioState) {
    if (desiredRadioState != RADIO_STATE_UNAVAILABLE) {
        int on = (desiredRadioState == RADIO_STATE_ON);
        OnRequest(RIL_REQUEST_RADIO_POWER, &on, sizeof(on));
    }
    else {
        RilLogW("[%d] Ignore TrySetRadioPower due to RADIO_STATE_UNAVAILABLE",
                GetRilSocketId());
    }
}

void NetworkServiceBase::OnRadioReady() {
    // set radio ready state to RADIO_READY
    m_nRadioReady = RADIO_READY;
    UpdateRadioState(RADIO_STATE_UNAVAILABLE, true);
}

int NetworkServiceBase::OnRadioReady(Message * /* pMsg */) {
    RilLogI("[%s] %s()", m_szSvcName, __FUNCTION__);
    OnRadioReady();
    return 0;
}

void NetworkServiceBase::WriteVolteEmcServiceStatus(bool isVopsSupported,
                                                    int emcService, int rat,
                                                    int regState, bool notify/* = false*/) {
    mEmergencyNumberManager->SetEmcBearerSupported(emcService == EMC_NR_CONNECTED_TO_5GCN ||
                                                   emcService == EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN);

    // for legacy
    bool supportOemImsSupportInd = (RilApplication::RIL_VersionCode <= HAL_VERSION_CODE(1, 4));
    if (supportOemImsSupportInd) {
#pragma pack(1)
        struct {
            BYTE voPS;
            BYTE emc;
            BYTE rilsocketId;
            BYTE rat;
            BYTE regState;
        } ims_support_ind_data;
#pragma pack()
        ims_support_ind_data.voPS = isVopsSupported ? 1 : 0;
        ims_support_ind_data.emc = (BYTE) emcService;
        ims_support_ind_data.rilsocketId = GetRilSocketId();
        ims_support_ind_data.rat = (BYTE) rat;
        ims_support_ind_data.regState = (BYTE) regState;
        int overallCardState = 0;
        int activeSim = -1;

        RilProperty *property = GetRilApplicationProperty();
        if (property != NULL) {
            activeSim = property->GetInt(RIL_APP_PS_ACTIVE_SIM, -1);
            overallCardState = property->GetInt(RIL_SIM_OVERALL_CARDSTATE, 0);
            RilLogV("WriteVolteEmcServiceStatus overallCardState=0x%08X", overallCardState);

            if (overallCardState == 0) {
                // 9 is invalid value
                ims_support_ind_data.voPS = 9;
            }
        }

        if (notify) {
            RilLogV("RIL_UNSOL_OEM_IMS_SUPPORT_SERVICE={RilSocketId=%d VoLTE Available=%d Emergency Call=%d RadioTech=%d}",
                    GetRilSocketId(), isVopsSupported, emcService, rat);
            // notify status (Framework or RIL client)
            OnUnsolicitedResponse(RIL_UNSOL_OEM_IMS_SUPPORT_SERVICE, &ims_support_ind_data, sizeof(ims_support_ind_data));
        }
    }
}

void NetworkServiceBase::QueryEmergencyCallAvailableRadioTech() {
    RilLog("[%s] %s", GetServiceName(), __FUNCTION__);
    // set correct status and rat info
    // Query
    // status : start
    // RAT : unspecified (0xFF)
    // Baseband will notify the current EMC available Radio Tech.
    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetEmergencyCallStatus(EMERGENCY_CALL_STATUS_START, RADIO_TECH_UNSPECIFIED);
    if (pModemData != NULL) {
        if (SendRequest(pModemData) < 0) {
            RilLogW("Fail to send SetEmergencyCallStatus");
        }
        delete pModemData;
    }
}

void NetworkServiceBase::QueryCurrentPsDomainState() {
    RilLogI("[%s] %s()", GetServiceName(), __FUNCTION__);
    RilContext *pRilContext = GetRilContext();
    if (pRilContext != NULL) {
        pRilContext->OnRequest(RIL_REQUEST_OEM_GET_PS_ATTACH_DETACH, NULL, 0, 0);
    }
}

bool NetworkServiceBase::IsPossibleToPassInRadioOffState(int request_id) {
    switch (request_id) {
        case RIL_REQUEST_RADIO_POWER:
        case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
        case RIL_REQUEST_OEM_GET_PS_ATTACH_DETACH:
        case RIL_REQUEST_GET_RADIO_CAPABILITY:
        case RIL_REQUEST_SHUTDOWN:
        case RIL_REQUEST_ALLOW_DATA:
#ifdef RIL_EXTENSION
        case RIL_REQUEST_GET_ALLOW_DATA_STATE:
        case RIL_REQUEST_GET_DUPLEX_MODE:
        case RIL_REQUEST_SET_DUPLEX_MODE:
#endif
            break;
        // allow cs/ps reg query even in radio off
        // in order to update current not-reg state.
        // allow all 4 polling context command id
        case RIL_REQUEST_OPERATOR:
        case RIL_REQUEST_DATA_REGISTRATION_STATE:
        case RIL_REQUEST_VOICE_REGISTRATION_STATE:
        case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:

        case RIL_REQUEST_SET_DS_NETWORK_TYPE:
        case RIL_REQUEST_SET_DUAL_NETWORK_AND_ALLOW_DATA:
        case RIL_REQUEST_OEM_GET_IMS_SUPPORT_SERVICE:

        case RIL_REQUEST_OEM_GET_MANUAL_RAT_MODE:
        case RIL_REQUEST_OEM_SET_MANUAL_RAT_MODE:
        case RIL_REQUEST_OEM_GET_FREQUENCY_LOCK:
        case RIL_REQUEST_OEM_SET_FREQUENCY_LOCK:
        case RIL_REQUEST_SET_ACTIVATE_VSIM:
        case RIL_REQUEST_OEM_SET_ENDC_MODE:
        case RIL_REQUEST_OEM_GET_ENDC_MODE:
        case RIL_REQUEST_OEM_GET_FREQUENCY_INFO:
        case RIL_REQUEST_OEM_SET_NR_MODE:
        case RIL_REQUEST_OEM_SET_NR_SILENCE_MODE:
        case RIL_REQUEST_OEM_GET_NR_MODE:
        case RIL_REQUEST_OEM_GET_NR_SILENCE_MODE:
        case RIL_REQUEST_OEM_SET_VONR_CAPA:
        case RIL_REQUEST_OEM_GET_VONR_CAPA:
        case RIL_REQUEST_SET_ALLOWED_NETWORK_TYPES_BITMAP:
        case RIL_REQUEST_GET_ALLOWED_NETWORK_TYPES_BITMAP:
        case RIL_REQUEST_OEM_SET_PLMN_LIST:
        case RIL_REQUEST_OEM_GET_PLMN_LIST:
        case RIL_REQUEST_OEM_SET_IMS_REGISTRATION_STATE:
        case RIL_REQUEST_OEM_SET_MOBILE_DATA_STATE:
        case RIL_REQUEST_ENABLE_VONR:
        case RIL_REQUEST_IS_VONR_ENABLED:
        case RIL_REQUEST_SET_USAGE_SETTING:
        case RIL_REQUEST_GET_USAGE_SETTING:
        case RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA:
            break;
        default:
            return false;
    }
    return true;

}

bool NetworkServiceBase::IsPossibleToPassInRadioUnavailableState(int request_id) {
    switch(request_id) {
        case RIL_REQUEST_RADIO_POWER:
        case RIL_REQUEST_OEM_GET_RADIO_STATE:
        case RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA:
            break;
        default:
            return false;
    }
    return true;
}


void NetworkServiceBase::setVoiceRat(RIL_RadioTechnology rat) {
    RilLogV("[%s] %s() Change rat to %d for switching phone type", GetServiceName(), __FUNCTION__, rat);
    m_nVoiceRat = rat;
    return;
}


////// Update emergency number list
void NetworkServiceBase::OnImsiUpdated(const char *imsi) {
    if (imsi == NULL || *imsi == 0 || !(strlen(imsi) >= 5)) {
        RilLogE("Invalid IMSI value. Please check it");
        return ;
    }

    this->UpdateDbEccInfo();
}

void NetworkServiceBase::UpdateDbEccInfo(bool forceToUpdate, bool callByOpposite) {
    mEmergencyNumberManager->UpdateDbEccInfo(forceToUpdate, callByOpposite);
}

void NetworkServiceBase::UpdateEmergencyNumberList() {
    mEmergencyNumberManager->UpdateEmergencyNumberList();
}

EccInfo *NetworkServiceBase::GetRadioEccInfos() {
    return mEmergencyNumberManager->GetRadioEccInfos();
}

bool NetworkServiceBase::IsEmergencyCallRouting(const char *number, bool *normalRoutingWhenInService) {
    return mEmergencyNumberManager->IsEmergencyCallRouting(number, normalRoutingWhenInService);
}

int NetworkServiceBase::GetEmergencyNumberCategory(const char *number) {
    return mEmergencyNumberManager->GetEmergencyNumberCategory(number);
}

void NetworkServiceBase::SetEmcLabTestMode(bool isEmcLabTestMode) {
    mEmergencyNumberManager->SetEmcLabTestMode(isEmcLabTestMode);
}

void NetworkServiceBase::ResetDbEccId(bool forceToErase) {
    mEmergencyNumberManager->ResetDbEccId(forceToErase);
}

void NetworkServiceBase::SetTempEmergencyNumber(int operation, int number_len, char *number, int category) {
    mEmergencyNumberManager->SetTempEmergencyNumber(operation, number_len, number, category);
}

bool NetworkServiceBase::IsDisabledESim() {
    SimService *pSimService = ((SimService *)GetCurrentService(RIL_SERVICE_SIM));
    bool isDisabledESim = false;
    if (pSimService != NULL) {
        const V2_0::RIL_CardStatus &cardStatus = pSimService->GetRilCardStatus();
        //  num_applications == 0 indicates disabled eSIM.
        isDisabledESim = (cardStatus.num_applications == 0);
    }
    RilLogI("[%s] %s() eSIM is disabled: %s.", GetServiceName(), __FUNCTION__,
            (isDisabledESim ? "true" : "false"));
    return isDisabledESim;
}

void NetworkServiceBase::dump(std::stringstream& ss) {
    int voiceRat = RADIO_TECH_UNKNOWN;
    int voiceRegState = RIL_NOT_REG_AND_NOT_SEARCHING;
    int dataRat = RADIO_TECH_UNKNOWN;
    int dataRegState = RIL_NOT_REG_AND_NOT_SEARCHING;
    std::string netPlmn = "";
    int imsRegiState = RIL_IMS_NOT_REGISTERED;

    RilProperty *property = GetRilContextProperty();
    if (property == NULL) {
        return;
    }
    voiceRat = property->GetInt(RIL_CONTEXT_NET_VOICE_RADIO_TECH);
    voiceRegState = property->GetInt(RIL_CONTEXT_NET_VOICE_REGISTRATION_STATE, 0);
    dataRat = property->GetInt(RIL_CONTEXT_NET_DATA_RADIO_TECH);
    dataRegState = property->GetInt(RIL_CONTEXT_NET_DATA_REGISTRATION_STATE, 0);
    netPlmn = property->GetString(RIL_CONTEXT_NET_CURRENT_PLMN);
    imsRegiState = property->GetInt(RIL_CONTEXT_IMS_REGISTRATION);

    ss << GetServiceName() << ":{";
    ss << "VoiceRat=" << NetworkUtils::getRadioTechnologyString(voiceRat);
    ss << "(" << NetworkUtils::getRegStateString(voiceRegState) << ")";
    ss << ", DataRat=" << NetworkUtils::getRadioTechnologyString(dataRat);
    ss << "(" << NetworkUtils::getRegStateString(dataRegState) << ")";
    ss << ", NetPlmn=" << netPlmn;
    ss << ", ImsRegiState=" << (imsRegiState ? "Registered" : "Not registered");
    ss << "}";
}
