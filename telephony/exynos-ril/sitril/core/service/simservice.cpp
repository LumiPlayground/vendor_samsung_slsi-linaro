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
 * simservice.cpp
 *
 *  Created on: 2014. 7. 1.
 *      Author: MOX
 */

#include "carrierconfig/carrierconfigmanager.h"
#include "simservice.h"
#include "networkservice.h"
#include "rillog.h"
#include "protocoladapter.h"
#include "protocolsimbuilder.h"
#include "protocolsimadapter.h"
#include "util.h"
#include "rilapplication.h"
#include <librilutils/systemproperty.h>
#include "simlockhandler.h"
#include "uiccphonebookhandler.h"
#include "simautoverifypinhandler.h"
#include <assert.h>
#include <pthread.h>
#include <sstream>
#include <string.h>
#include "iccUtil.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define PARAM_NULL(msg)         { if(msg==NULL) { RilLogE("%s::%s() Parameter = NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_REQ(msg)           { if(msg==NULL || msg->GetRequestData()==NULL) { RilLogE("%s::%s() RequestData = NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_RSP(msg)           { if(msg==NULL || msg->GetModemData()==NULL) { RilLogE("%s::%s() ModemData = NULL", m_szSvcName, __FUNCTION__); return -1; } }
#define ENTER_FUNC()            { RilLogI("%s::%s() [<-- ", m_szSvcName, __FUNCTION__); }
#define LEAVE_FUNC()            { RilLogI("%s::%s() [--> ", m_szSvcName, __FUNCTION__); }
#define NOT_IMPLEMENT()         { RilLogE("%s::%s() Not Implemented", m_szSvcName, __FUNCTION__); }

#define LOGI(format, ...)       RilLogI("%s::%s() " format, m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGV(format, ...)       RilLogV("%s::%s() " format, m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#undef LOGD
#define LOGD(format, ...)       RilLog("%s::%s() " format, m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGE(format, ...)       RilLogE("%s::%s() " format, m_szSvcName, __FUNCTION__, ##__VA_ARGS__)

#define PROPERTY_NAME_LEN       (40)
#ifdef AUTO_VERIFY_PIN_WITH_CP
#define INVALID_PIN_LEN         (-1)
#define INVALID_PIN_STR         ""
#endif

#define TIME_INTERVAL_BOOT_STATE  2000
string cardstate_str[4] = { "RIL_CARDSTATE_ABSENT", "RIL_CARDSTATE_PRESENT", "RIL_CARDSTATE_ERROR",
        "RIL_CARDSTATE_RESTRICTED" };

static pthread_mutex_t s_SimStateChangeGuaranteeMutexlock = PTHREAD_RWLOCK_INITIALIZER;

SimService::SimService(RilContext* pRilContext)
    : Service(pRilContext, RIL_SERVICE_SIM) {
    memset(&mRilCardStatus, 0, sizeof(mRilCardStatus));
    mRilCardStatus.atr = m_szAtr;
    mRilCardStatus.iccid = m_szIccid;
    mRilCardStatus.eid = m_szEid;
    memset(mRilCardStatus.atr, 0,  sizeof(m_szAtr));
    memset(mRilCardStatus.iccid, 0,  sizeof(m_szIccid));
    memset(mRilCardStatus.eid, 0, sizeof(m_szEid));

    memset(m_aszAID, 0, sizeof(char)*RIL_CARD_MAX_APPS*36);

    // SIM HotSwap Feature
    m_nSimCardState = SIM_CARDSTATE_UNKNOWN;

    // uicc app is enabled by default
    m_nCurrentUiccEnableState = UICC_APP_STATE_ENABLED;

    // ensure that duplicate indication is not sent to f/w
    m_bUiccApplicationsEnablementChanged = TRUE;
    m_nReqUiccEnableState = UICC_APP_STATE_ENABLED; // Initialize to "enabled"
    memset(m_szLastDisabledIccid, 0, sizeof(m_szLastDisabledIccid));

    m_nRadioState = RADIO_STATE_UNAVAILABLE;
    m_nImsAppType = -1;

    m_nBootState = BOOT_STATE_UNKNOWN;
    m_nPrevAppState = RIL_APPSTATE_UNKNOWN;
    m_bSendVoiceRat = false;

    m_pSimLockHandler = NULL;
    m_pUiccPbHandler = NULL;
    m_pSimAutoVerifyPinHandler = NULL;

    previousIccId.clear();
    orderedIccId.clear();
}

SimService::~SimService() {
    if (m_pSimAutoVerifyPinHandler != NULL) {
        delete m_pSimAutoVerifyPinHandler ;
        m_pSimAutoVerifyPinHandler = NULL;
    }

    if (m_pSimLockHandler != NULL) {
        delete m_pSimLockHandler ;
        m_pSimLockHandler = NULL;
    }

    if (m_pUiccPbHandler != NULL) {
        delete m_pUiccPbHandler ;
        m_pUiccPbHandler = NULL;
    }

    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).unregisterMonitoringCallback(this);
}

int SimService::OnCreate(RilContext * /* pRilContext */) {
    ENTER_FUNC();

    m_pSimAutoVerifyPinHandler = new SimAutoVerifyPinHandler();
    if (m_pSimAutoVerifyPinHandler != NULL) {
        m_pSimAutoVerifyPinHandler->Init(this);
    }

    m_pSimLockHandler = new SimLockHandler();
    if (m_pSimLockHandler != NULL) {
        m_pSimLockHandler->Init(this);
    }

    m_pUiccPbHandler = new UiccPhonebookHandler();
    if (m_pUiccPbHandler != NULL) {
        m_pUiccPbHandler->Init(this);
    }

    if (GetRilSocketId() == RIL_SOCKET_1) {
        strcpy(m_szLastDisabledIccid, RIL_SIM1_DISABLED_ICCID);
    }
#if (SIM_COUNT >= 2)
    else if (GetRilSocketId() == RIL_SOCKET_2) {
        strcpy(m_szLastDisabledIccid, RIL_SIM2_DISABLED_ICCID);
    }
#endif
    else LOGV("Fail to load GetRilSocketId(%d)", GetRilSocketId());

    // register
    TelephonyStatusTracker::createForPhoneId(GetRilSocketId()).registerMonitoringCallback(this, true);

    LEAVE_FUNC();
    return 0;
}

void SimService::OnStart() {
    // init CarrierConfigManager
    if (GetRilSocketId() == RIL_SOCKET_1) {
        CarrierConfigManager::GetInstance();
    }
}

BOOL SimService::OnHandleRequest(Message* pMsg) {
    if (pMsg == NULL) {
        return FALSE;
    }

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_SIM_GET_STATUS:
        ret = DoGetSimStatus(pMsg);
        break;
    case MSG_SIM_VERIFY_PIN:
        ret = m_pSimLockHandler->DoVerifyPin(pMsg);
        break;
    case MSG_SIM_VERIFY_PUK:
        ret = m_pSimLockHandler->DoVerifyPuk(pMsg);
        break;
    case MSG_SIM_VERIFY_PIN2:
        ret = m_pSimLockHandler->DoVerifyPin2(pMsg);
        break;
    case MSG_SIM_VERIFY_PUK2:
        ret = m_pSimLockHandler->DoVerifyPuk2(pMsg);
        break;
    case MSG_SIM_CHANGE_PIN:
        ret = m_pSimLockHandler->DoChangePin(pMsg);
        break;
    case MSG_SIM_CHANGE_PIN2:
        ret = m_pSimLockHandler->DoChangePin2(pMsg);
        break;
    case MSG_SIM_VERIFY_NETWORK_LOCK:
        ret = m_pSimLockHandler->DoVerifyNetworkLock(pMsg);
        break;
    case MSG_SIM_SET_FACILITY_LOCK:
        ret = m_pSimLockHandler->DoSetFacilityLock(pMsg);
        break;
    // PhoneBook
    case MSG_SIM_READ_PB_ENTRY:
        ret = m_pUiccPbHandler->DoReadPbEntry(pMsg);
        break;
    case MSG_SIM_UPDATE_PB_ENTRY:
        ret = m_pUiccPbHandler->DoUpdatePbEntry(pMsg);
        break;
    case MSG_SIM_GET_PHONEBOOK_CAPACITY:
        ret = m_pUiccPbHandler->DoGetPbCapa(pMsg);
        break;
    case MSG_SIM_GET_PHONEBOOK_RECORDS:
        ret = m_pUiccPbHandler->DoGetPbRec(pMsg);
        break;
    default:
        // TODO log unsupported message id
        return FALSE;
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

BOOL SimService::OnHandleSolicitedResponse(Message* pMsg) {
    PARAM_NULL(pMsg);

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_SIM_GET_STATUS_DONE:
        ret = OnGetSimStatusDone(pMsg);
        break;
    case MSG_SIM_VERIFY_PIN_DONE:
        ret = m_pSimLockHandler->OnVerifyPinDone(pMsg);
        break;
    case MSG_SIM_VERIFY_PUK_DONE:
        ret = m_pSimLockHandler->OnVerifyPukDone(pMsg);
        break;
    case MSG_SIM_VERIFY_PIN2_DONE:
        ret = m_pSimLockHandler->OnVerifyPin2Done(pMsg);
        break;
    case MSG_SIM_VERIFY_PUK2_DONE:
        ret = m_pSimLockHandler->OnVerifyPuk2Done(pMsg);
        break;
    case MSG_SIM_CHANGE_PIN_DONE:
        ret = m_pSimLockHandler->OnChangePinDone(pMsg);
        break;
    case MSG_SIM_CHANGE_PIN2_DONE:
        ret = m_pSimLockHandler->OnChangePin2Done(pMsg);
        break;
    case MSG_SIM_VERIFY_NETWORK_LOCK_DONE:
        ret = m_pSimLockHandler->OnVerifyNetworkLockDone(pMsg);
        break;
    case MSG_SIM_SET_FACILITY_LOCK_DONE:
        ret = m_pSimLockHandler->OnSetFacilityLockDone(pMsg);
        break;
        // PhoneBook
    case MSG_SIM_READ_PB_ENTRY_DONE:
        ret = m_pUiccPbHandler->OnReadPbEntryDone(pMsg);
        break;
    case MSG_SIM_UPDATE_PB_ENTRY_DONE:
        ret = m_pUiccPbHandler->OnUpdatePbEntryDone(pMsg);
        break;
    case MSG_SIM_GET_PB_STORAGE_INFO_DONE:
        ret = m_pUiccPbHandler->OnGetPbStorageInfoDone(pMsg);
        break;
    case MSG_SIM_GET_3G_PB_CAPA_DONE:
        ret = m_pUiccPbHandler->OnGet3GPbCapaDone(pMsg);
        break;
    case MSG_SIM_AUTO_DISABLE_UICC_APP_DONE:
        ret = OnAutoDisableUiccDone(pMsg);
        break;
#ifdef AUTO_VERIFY_PIN_WITH_CP
    case MSG_SIM_VERIFY_ENCRYPTED_PIN_DONE:
        ret = m_pSimAutoVerifyPinHandler->OnVerifyEncryptedPinDone(pMsg);
        break;
#endif
    default:
        // TODO log unsupported message id
        return FALSE;
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

BOOL SimService::OnHandleUnsolicitedResponse(Message* pMsg) {
    PARAM_NULL(pMsg);

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_SIM_PB_READY:
        ret = m_pUiccPbHandler->OnPbReady(pMsg);
        break;
#ifdef AUTO_VERIFY_PIN_WITH_CP
    case MSG_SIM_IND_ENCRYPTED_PIN:
        ret = m_pSimAutoVerifyPinHandler->OnReceiveEncryptedPin(pMsg);
        break;
#endif
    default:
        // TODO log unsupported message id
        return FALSE;
    } // end switch ~

    return (ret < 0 ? FALSE : TRUE);
}

BOOL SimService::OnHandleRequestTimeout(Message* pMsg) {
    if (pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return FALSE;
    }

    int ret = -1;
    switch (pMsg->GetMsgId()) {
    case MSG_SIM_GET_STATUS:
        ret = OnGetSimStatusTimeout(pMsg);
        break;
    }

    return (ret < 0 ? FALSE : TRUE);
}

void SimService::OnReset() {
    ENTER_FUNC();
    m_nCurrentUiccEnableState = UICC_APP_STATE_ENABLED;
    m_bUiccApplicationsEnablementChanged = TRUE;
    LEAVE_FUNC();
}

void SimService::OnSimStatusChanged(int cardState, int /* appState */) {
    ENTER_FUNC();
    OnUnsolicitedResponse(RIL_UNSOL_OEM_SE_SIM_STATE_CHANGED, &cardState, sizeof(int));
    LEAVE_FUNC();
}

void SimService::OnRadioStateChanged(int radioState) {
    m_nRadioState = radioState;
    // Send cached ICCID to framework if condition is satisfied.
}

void SimService::OnRadioNotAvailable() {
    ENTER_FUNC();
    // Set auto pin state to READY in here.
    // SimService cannot receive OnModemStateChanged Ind after continuous CP crash
    if (m_pSimAutoVerifyPinHandler->GetAutoPinState()
                            == SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLED)
        m_pSimAutoVerifyPinHandler->SetAutoPinState(
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_CP_CRASH);
    m_nCurrentUiccEnableState = UICC_APP_STATE_ENABLED;
    m_bUiccApplicationsEnablementChanged = TRUE;
    LEAVE_FUNC();
}

int SimService::DoGetSimStatus(Message *pMsg) {
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    int nResult = -1;

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimGetStatus();
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, MSG_SIM_GET_STATUS_DONE);

    LEAVE_FUNC();
    return (nResult < 0) ? -1 : 0;
}

void SimService::InitRilCardStatus() {
    memset(&mRilCardStatus, 0, sizeof(mRilCardStatus));
    mRilCardStatus.card_state = RIL_CARDSTATE_ABSENT;
    mRilCardStatus.universal_pin_state = RIL_PINSTATE_UNKNOWN;
    mRilCardStatus.gsm_umts_subscription_app_index = -1;
    mRilCardStatus.cdma_subscription_app_index = -1;
    mRilCardStatus.ims_subscription_app_index = -1;
    mRilCardStatus.num_applications = 0;
    mRilCardStatus.atr = m_szAtr;
    mRilCardStatus.iccid = m_szIccid;
    mRilCardStatus.eid = m_szEid;
    memset(m_szAtr, 0,  sizeof(m_szAtr));
    memset(m_szIccid, 0,  sizeof(m_szIccid));
    memset(m_szEid, 0, sizeof(m_szEid));
}

void SimService::BuildRilCardStatusApplications(ProtocolSimStatusAdapter &adapter) {
    int index = 0;
    for (int i = 0; i < mRilCardStatus.num_applications; i++) {
        if (((RIL_AppType) adapter.GetAppsType(i)) == RIL_APPTYPE_UNKNOWN) {
            char *pszAid = adapter.GetAID(i);
            RilLogV("%s::%s() Skip AID: %s, AppType: %d", m_szSvcName, "BuildSimStatus",
                    pszAid, adapter.GetAppsType(i));
            if(pszAid != NULL) delete [] pszAid;
            continue;
        }

        mRilCardStatus.applications[index].app_type = (RIL_AppType) adapter.GetAppsType(i);
        mRilCardStatus.applications[index].perso_substate
                = (RIL_PersoSubstate) adapter.GetPersonalSubstate(i);

        switch(mRilCardStatus.applications[index].app_type) {
        case RIL_APPTYPE_SIM:
        case RIL_APPTYPE_USIM:
            mRilCardStatus.gsm_umts_subscription_app_index = index;
            break;
        case RIL_APPTYPE_RUIM:
        case RIL_APPTYPE_CSIM:
            mRilCardStatus.cdma_subscription_app_index = index;
            break;
        case RIL_APPTYPE_ISIM:
            mRilCardStatus.ims_subscription_app_index = index;
            break;
        default:
            break;
        }

        if (m_nSimCardState == SIM_CARDSTATE_PRESENT) {
            mRilCardStatus.applications[index].app_state =
                    (RIL_AppState) adapter.GetAppsState(i);
        } else {
            mRilCardStatus.applications[index].app_state = RIL_APPSTATE_UNKNOWN;
        }

#ifdef _SIM_DEPERSONALIZATION_TEST_
        // for sim unlock testing
        mRilCardStatus.applications[index].app_state = RIL_APPSTATE_SUBSCRIPTION_PERSO;
        mRilCardStatus.applications[index].perso_substate =
                (RIL_PersoSubstate) RIL_PERSOSUBSTATE_SIM_NETWORK; // RIL_PERSOSUBSTATE_SIM_SPN
#endif

        char *pszAID = NULL;
        char *pszAdapterAID = adapter.GetAID(i);
        if (pszAdapterAID != NULL) {
            memset(m_aszAID[index], 0, (MAX_SIM_AID_LEN * 2) + 1);
            strncpy(m_aszAID[index], pszAdapterAID, strlen(pszAdapterAID));
            delete [] pszAdapterAID;
            pszAdapterAID = NULL;
            pszAID = m_aszAID[index];
        }

        mRilCardStatus.applications[index].aid_ptr = pszAID;
        mRilCardStatus.applications[index].app_label_ptr = NULL;        // Ignore
        mRilCardStatus.applications[index].pin1_replaced = adapter.GetPin1Replaced(i);
        mRilCardStatus.applications[index].pin1 = (RIL_PinState) adapter.GetPinState(i, 1);
        mRilCardStatus.applications[index].pin2 = (RIL_PinState) adapter.GetPinState(i, 2);

        RilLogV("%s::%s() %d. AppType(%d), AppState(%d), PersoState(%d), PinState(%d, %d), AID(%s)",
                m_szSvcName, "BuildSimStatus", index,
                mRilCardStatus.applications[index].app_type,
                mRilCardStatus.applications[index].app_state,
                mRilCardStatus.applications[index].perso_substate,
                mRilCardStatus.applications[index].pin1,
                mRilCardStatus.applications[index].pin2,
                mRilCardStatus.applications[index].aid_ptr);

        RilLogV("%s::%s()  - PinRemain(%d, %d), PukRemain(%d, %d)", m_szSvcName, "BuildSimStatus",
                adapter.GetPinRemainCount(i, 1),
                adapter.GetPinRemainCount(i, 2),
                adapter.GetPukRemainCount(i, 1),
                adapter.GetPukRemainCount(i, 2));

        index++;
    }

    if (mRilCardStatus.num_applications != index) {
        RilLogI("%s::%s() Changed num_applications from %d to %d", m_szSvcName, "BuildSimStatus",
                mRilCardStatus.num_applications, index);
        mRilCardStatus.num_applications = index;
    }

    RilLogV("%s::%s() AppIdx(GSM/UMTS:%d, CDMA:%d, IMS:%d)", m_szSvcName, "BuildSimStatus",
            mRilCardStatus.gsm_umts_subscription_app_index,
            mRilCardStatus.cdma_subscription_app_index,
            mRilCardStatus.ims_subscription_app_index);
}

void SimService::FillRilCardStatusFromAdapter(ProtocolSimStatusAdapter &adapter,
        bool isBuildNeeded) {
    int ret;
    pthread_mutex_t* simStateChangeGuaranteeMutexlockHook = &s_SimStateChangeGuaranteeMutexlock;
    ret = pthread_mutex_lock(simStateChangeGuaranteeMutexlockHook);
    assert (ret == 0);

    InitRilCardStatus();
    UINT uErrCode = adapter.GetErrorCode();
    if (isBuildNeeded) {
        if (uErrCode == RIL_E_SUCCESS) {
            mRilCardStatus.card_state = (RIL_CardState) adapter.GetCardState();
            mRilCardStatus.universal_pin_state = (RIL_PinState) adapter.GetUniversalPinState();
            mRilCardStatus.num_applications = adapter.GetApplicationCount();
            RilLogV("%s::%s() CardState:%s, NumApps:%d", m_szSvcName, "BuildSimStatus",
                    cardstate_str[mRilCardStatus.card_state].c_str(), mRilCardStatus.num_applications);

            // APPLICATIONS
            BuildRilCardStatusApplications(adapter);
            int nHexLen = 0;
            // ATR
            if (adapter.GetAtrLength() > 0) {
                nHexLen = Value2HexString(mRilCardStatus.atr, (BYTE *) adapter.GetAtr(),
                        adapter.GetAtrLength());
            }

            // ICC ID
            if (adapter.GetIccidLength() > 0) {
                int iccIdLength = adapter.GetIccidLength();
                const BYTE *iccId =(BYTE *) adapter.GetIccid();
                orderedIccId = bchToString(iccId, iccIdLength);
                if (orderedIccId.length()) {
                    if (mRilCardStatus.card_state == RIL_CARDSTATE_PRESENT) {
                        previousIccId = mRilCardStatus.iccid;
                    }

                    strncpy(mRilCardStatus.iccid, orderedIccId.c_str(), orderedIccId.length());
                 }
            }

            // EID
            if (adapter.GetEidLength() > 0) {
                nHexLen = Value2HexString(mRilCardStatus.eid, (BYTE *) adapter.GetEid(),
                        adapter.GetEidLength());
            }

            // PORT ID
            mRilCardStatus.portId = adapter.GetPortId();
        }
    } else {
        mRilCardStatus.card_state = (uErrCode == RIL_E_SIM_ABSENT) ? RIL_CARDSTATE_ABSENT
                : RIL_CARDSTATE_ERROR;
        RilLogV("%s::%s() CardState:%s", m_szSvcName, "BuildSimStatus",
                (uErrCode == RIL_E_SIM_ABSENT) ? "RIL_CARDSTATE_ABSENT" : "RIL_CARDSTATE_ERROR");
    }

    // PHYSICAL SLOT ID
    int nPhysicalSlotId = adapter.GetPhysicalSlotId();
    mRilCardStatus.physicalSlotId =
            (nPhysicalSlotId == INVALID_SLOT_ID) ? -1 : nPhysicalSlotId;
    ret = pthread_mutex_unlock(simStateChangeGuaranteeMutexlockHook);
    assert (ret == 0);
}

bool SimService::IsEarlySimDetection(ProtocolSimStatusAdapter &adapter) {
    RIL_CardState eCardState = (RIL_CardState) adapter.GetCardState();
    RIL_PinState ePinState = (RIL_PinState) adapter.GetUniversalPinState();
    int nAtrLength = adapter.GetAtrLength();
    int nIccIdLength = adapter.GetIccidLength();

    if (RIL_CARDSTATE_PRESENT == eCardState && ePinState == RIL_PINSTATE_UNKNOWN
            && nAtrLength == 0 && nIccIdLength == 0) {
        RilLogI("%s::%s() Early detection is changed to ABSENT", m_szSvcName, "BuildSimStatus");
        FillRilCardStatusFromAdapter(adapter, false);
        return true;
    }

    return false;
}

char *SimService::BuildSimStatus(ProtocolSimStatusAdapter &adapter) {
    ENTER_FUNC();

    RIL_CardState eCardState = (RIL_CardState) adapter.GetCardState();
    // Check for HotSwap
    int nSimCardState = SIM_CARDSTATE_UNKNOWN;
    if(eCardState == RIL_CARDSTATE_PRESENT || eCardState == RIL_CARDSTATE_RESTRICTED) {
        nSimCardState = SIM_CARDSTATE_PRESENT;
    } else if(eCardState == RIL_CARDSTATE_ABSENT) {
        nSimCardState = SIM_CARDSTATE_ABSENT;
    } else {
        nSimCardState = SIM_CARDSTATE_ERROR;
    }

    if (m_nSimCardState == SIM_CARDSTATE_UNKNOWN) {
        m_nSimCardState = nSimCardState;
    } else {
        if (m_nSimCardState != nSimCardState) {
            // SIM Insertion
            if (m_nSimCardState == SIM_CARDSTATE_ABSENT && nSimCardState == SIM_CARDSTATE_PRESENT)
                OnSimHotSwap(FALSE);
            // SIM Removal
            else if (m_nSimCardState == SIM_CARDSTATE_PRESENT
                    && nSimCardState == SIM_CARDSTATE_ABSENT)
                OnSimHotSwap(TRUE);

            m_nSimCardState = nSimCardState;
        }
    }

    // Fill sim status.
    FillRilCardStatusFromAdapter(adapter, true);

    // save ICC ID to ril property
    if (orderedIccId.length()) {
        // If iccid is not same as previous iccid, all clear saved pin
        if (previousIccId.length() && orderedIccId.compare(previousIccId) != 0) {
            m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
        }

        RilProperty *pProperty = GetRilContextProperty();
        if (pProperty != NULL) pProperty->Put(RIL_CONTEXT_SIM_ICC_ID, orderedIccId.c_str());
    } else {
        RilProperty *pProperty = GetRilContextProperty();
        if (pProperty != NULL) pProperty->Put(RIL_CONTEXT_SIM_ICC_ID, "");

        if (m_pSimAutoVerifyPinHandler->m_nAutoPinState ==
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_ENABLED &&
                mRilCardStatus.eid != NULL && mRilCardStatus.card_state == RIL_CARDSTATE_PRESENT &&
                mRilCardStatus.num_applications == 0) {
            LOGV("As eSIM profile is disabled, the auto verify pin variables is initialized.");
            m_pSimLockHandler->InitializePinPuk();
            m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
        }
    }

    for (int app_index = 0; app_index < mRilCardStatus.num_applications; app_index++) {
        // set pin/puk remain for APPTYPE USIM / SIM
        if (mRilCardStatus.applications[app_index].app_type == RIL_APPTYPE_SIM
                || mRilCardStatus.applications[app_index].app_type == RIL_APPTYPE_USIM) {
            m_pSimLockHandler->SetPinPukRemain(SIM_PIN, PIN1_PUK1,
                    adapter.GetPinRemainCount(app_index, 1));
            m_pSimLockHandler->SetPinPukRemain(SIM_PIN, PIN2_PUK2,
                    adapter.GetPinRemainCount(app_index, 2));
            m_pSimLockHandler->SetPinPukRemain(SIM_PUK, PIN1_PUK1,
                    adapter.GetPukRemainCount(app_index, 1));
            m_pSimLockHandler->SetPinPukRemain(SIM_PUK, PIN2_PUK2,
                    adapter.GetPukRemainCount(app_index, 2));
        }
    }

    int nAuthType = 0x00/*ISIM_AUTH_IMS*/;
    BOOL bExistIMS = FALSE, bExist3G = FALSE, bExistGSM = FALSE;
    for (int i = 0; i < mRilCardStatus.num_applications; i++) {
        switch(mRilCardStatus.applications[i].app_type) {
        case RIL_APPTYPE_SIM: bExistGSM = TRUE; break;
        case RIL_APPTYPE_USIM: bExist3G = TRUE; break;
        case RIL_APPTYPE_ISIM: bExistIMS = TRUE; break;
        default: break;
        }
    }

    RilLogV("%s::%s() IMS(%c), 3G(%c), GSM(%c)", m_szSvcName, __FUNCTION__,
            bExistIMS? 'O':'X',
            bExist3G? 'O':'X',
            bExistGSM? 'O':'X');

    if (bExistIMS == TRUE) nAuthType = 0x00/*ISIM_AUTH_IMS*/;
    else if (bExistGSM == TRUE) nAuthType = 0x01/*ISIM_AUTH_GSM*/;
    else if (bExist3G == TRUE) nAuthType = 0x02/*ISIM_AUTH_3G*/;

    if (m_nImsAppType != nAuthType) {
        RilProperty *pProperty = GetRilContextProperty();
        pProperty->Put(RIL_CONTEXT_IMS_APP_TYPE, nAuthType);

        m_nImsAppType = nAuthType;
    }

    RilLogV("%s::%s() eSIM No Profile(%s), ATR(%s), ICCID(%s), EID(%s)",
            m_szSvcName, __FUNCTION__, (adapter.GetEsimNoProfile() ? "TRUE" : "FALSE"),
            mRilCardStatus.atr, mRilCardStatus.iccid, mRilCardStatus.eid);
    RilLogV("%s::%s() physicalSlotId(%d), PortId(%d)", m_szSvcName, __FUNCTION__,
            mRilCardStatus.physicalSlotId, mRilCardStatus.portId);

    LEAVE_FUNC();
    return (char *) &mRilCardStatus;
}

int SimService::OnSimHotSwap(BOOL bRemoval) {
    ENTER_FUNC();

    if (bRemoval) {
        LOGV("SIM Removal !!!");

        // PIN/PUK Verification
        m_pSimLockHandler->InitializePinPuk();

        // clear open carrier information
        SetOpenCarrierIndex("");

        // Initialize variables of the application property for PS service state.
        RilProperty *pProperty = GetRilContextProperty();
        pProperty = GetRilApplicationProperty();
        stringstream ss;
        ss << RIL_APP_NET_PS_SERVICE_ALLOW << GetRilSocketId();;
        string strPropName = ss.str();
        pProperty->Put(strPropName, false);
        m_nPrevAppState = RIL_APPSTATE_UNKNOWN;
        m_bSendVoiceRat = false;

        m_pSimAutoVerifyPinHandler->ClearEncryptedPin();

        // SIMPB
        if (RilApplication::RIL_HalVersionCode >= HAL_VERSION_CODE(1, 6)) {
            if (m_pUiccPbHandler->IsSimPbBeingLoaded()) {
                m_pUiccPbHandler->SendLoadingPbError(PB_RECEIVED_ERROR);
            } else if (true == m_pUiccPbHandler->IsSimPbSupported()
                    && true == m_pUiccPbHandler->IsSimPbInitialized()) {
                LOGD("Send RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_CHANGED");
                OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_CHANGED);
                m_pUiccPbHandler->InitPbFlags();
            }
        }

#ifdef _SIM_HOTSWAP_SILENT_RESET_
        RilReset("SIM_HOTSWAP - Removal");
#endif
    } else {
        m_nCurrentUiccEnableState = UICC_APP_STATE_ENABLED;
        m_bUiccApplicationsEnablementChanged = TRUE;

        LOGV("SIM Insertion !!!");
#ifdef _SIM_HOTSWAP_SILENT_RESET_
        RilReset("SIM_HOTSWAP - Insertion");
#endif
    }

    LEAVE_FUNC();
    return 0;
}

void SimService::OnModemStateChanged(int /* state */) {
}

int SimService::CheckAndAutoVerifyPin(UINT uErrCode, void *pResponse, int nFinalLength)
{
    int nResult = 0;
    int nPinState = m_pSimLockHandler->GetPinState();
    bool bEncryptedPinPropertyExist = m_pSimAutoVerifyPinHandler->IsEncryptedPinPropertyExist();
    if (nPinState == RIL_PINSTATE_ENABLED_NOT_VERIFIED && bEncryptedPinPropertyExist) {
        if (m_pSimAutoVerifyPinHandler->GetAutoPinState() ==
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_CP_CRASH) {
            nResult = m_pSimAutoVerifyPinHandler->DoAutoVerifyPin();
            if (nResult < 0) {
                LOGE("DoAutoVerifyPin operation is abnormal!!");
                m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
                OnRequestComplete(uErrCode, (char *)pResponse, nFinalLength);
            }
            LEAVE_FUNC();
            return 0;
        }
#ifdef AUTO_VERIFY_PIN_WITH_CP
#else
        else if (m_pSimAutoVerifyPinHandler->GetAutoPinState() ==
                SimAutoVerifyPinHandler::AUTO_PIN_STATE_DISABLED) {
            if (BOOT_STATE_UNKNOWN == m_nBootState) {
                m_nBootState = m_pSimAutoVerifyPinHandler->CheckBootReason();
                if (m_nBootState == BOOT_STATE_KERNEL_PANIC) {
                    LOGV("Kernel PANIC - load auto verify pin");
                    nResult = m_pSimAutoVerifyPinHandler->DoAutoVerifyPin();
                    if (nResult < 0) {
                        LOGE("DoAutoVerifyPin operation is abnormal!!");
                        m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
                        OnRequestComplete(uErrCode, (char *)pResponse, nFinalLength);
                    }
                    LEAVE_FUNC();
                    return 0;
                } else if (m_nBootState == BOOT_STATE_NORMAL) {
                    m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
                } else if (m_nBootState == BOOT_STATE_UNKNOWN) {
                    LOGV("BOOT STATE UNKNOWN, setTimeout 2000");
                    GetCurrentMsg()->SetTimeout(TIME_INTERVAL_BOOT_STATE);
                    LEAVE_FUNC();
                    return 0;
                }
            }
        }
    } else if (nPinState == RIL_PINSTATE_DISABLED && bEncryptedPinPropertyExist) {
        m_pSimAutoVerifyPinHandler->ClearEncryptedPin();
#endif
    }
    return 1;
}

int SimService::OnGetSimStatusDone(Message *pMsg) {
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolSimResponseAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();

    // Currently, pResponse is built with highest verion(RIL_CardStatus_V1_4 = RIL_CardStatus_v6)
    // If hal version is low, nLength should be adjusted.
    int nFinalLength = 0;
    int halVer = GetCurrentRequestData()->GetHalVersion();
    if (halVer >= HAL_VERSION_CODE(2, 0)) {
        nFinalLength = sizeof(V2_0::RIL_CardStatus);
    } else {
        if (RilApplication::RIL_HalVersionCode >= HAL_VERSION_CODE(1, 4)) {
            nFinalLength = sizeof(RIL_CardStatus_V1_4);
        } else if (RilApplication::RIL_HalVersionCode >= HAL_VERSION_CODE(1, 2)) {
            nFinalLength = sizeof(RIL_CardStatus_V1_2);
        } else {
            // HAL_VERSION_CODE(1, 1) or HAL_VERSION_CODE(1, 0)
            // Remove   uint32_t physicalSlotId and  char *atr and  char *iccid;
            nFinalLength = sizeof(RIL_CardStatus_V1_2) - (sizeof(uint32_t) + sizeof(char *) * 2);
        }
    }

    ProtocolSimStatusAdapter adapterStatus(pMsg->GetModemData());
    if (uErrCode == RIL_E_SUCCESS) {
        int nResult = 0;

        // when early detection return absent
        if (IsEarlySimDetection(adapterStatus)) {
            OnRequestComplete(RIL_E_SUCCESS, (char *) &mRilCardStatus, nFinalLength);
            LEAVE_FUNC();
            return 0;
        }

        char *pResponse = BuildSimStatus(adapterStatus);
        //LOGV("data ptr:0x%08X, length:%d", pResponse);

        if (CheckAndAutoVerifyPin(RIL_E_SUCCESS, (char *)pResponse, nFinalLength) == 0)
            return 0;

        // If there only is RUIM/CSIM app or USIM/SIM app,
        // Send UNSOL_VOICE_RADIO_TECH_CHANGED to telephony framework for switching phone type
        if (GetRadioState() != RADIO_STATE_UNAVAILABLE) {
            int gsm_app = mRilCardStatus.gsm_umts_subscription_app_index;
            int cdma_app = mRilCardStatus.cdma_subscription_app_index;
            int app_state = RIL_APPSTATE_UNKNOWN;
            RIL_RadioTechnology rat = RADIO_TECH_UNKNOWN;

            if (gsm_app == -1 && cdma_app != -1) {
                app_state = mRilCardStatus.applications[cdma_app].app_state;
                rat = RADIO_TECH_1xRTT;
            } else if (gsm_app != -1 && cdma_app == -1) {
                app_state = mRilCardStatus.applications[gsm_app].app_state;
                rat = RADIO_TECH_UMTS;
            }

            if (rat != RADIO_TECH_UNKNOWN) {
                if (m_bSendVoiceRat == false
                        && app_state != RIL_APPSTATE_UNKNOWN && m_nPrevAppState != app_state) {
                    RilContext *pContext = GetRilContext();
                    NetworkService *pNetworkService =
                        (NetworkService *) pContext->GetService(RIL_SERVICE_NETWORK);
                    if (pNetworkService != NULL) {
                        pNetworkService->setVoiceRat(rat);
                    }

                    OnUnsolicitedResponse(RIL_UNSOL_VOICE_RADIO_TECH_CHANGED,
                            &rat, sizeof(rat));
                    m_bSendVoiceRat = true;
                }
                // LOGV("darkleo m_nPrevAppState : %d, app_state : %d",
                //        m_nPrevAppState, app_state);
                m_nPrevAppState = app_state;
            }
        }

        if (RIL_CARDSTATE_PRESENT == mRilCardStatus.card_state
                && TRUE == m_bUiccApplicationsEnablementChanged) {
            //check whether this sim is disabled sim.
            if(CheckDisabledIccid()) {
                nResult = DoAutoDisableUicc();
                if (nResult < 0) {
                    LOGE("DoAutoDisableUicc operation is abnormal!!");
                } else {
                    LEAVE_FUNC();
                    return 0;
                }
            }

            OnRequestComplete(RIL_E_SUCCESS, (char *) pResponse, nFinalLength);
            OnUnsolicitedResponse(RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED,
                    &m_nCurrentUiccEnableState, sizeof(m_nCurrentUiccEnableState));
            m_bUiccApplicationsEnablementChanged = FALSE;
        } else {
            OnRequestComplete(RIL_E_SUCCESS, (char *) pResponse, nFinalLength);
        }
    } else {    // Result must not be error.
        FillRilCardStatusFromAdapter(adapterStatus, false);
        OnRequestComplete(RIL_E_SUCCESS, (char *) &mRilCardStatus, nFinalLength);
    }

    LEAVE_FUNC();
    return 0;
}

int SimService::OnGetSimStatusTimeout(Message *pMsg) {
    ENTER_FUNC();
    NULL_RSP(pMsg);

    for (int i = 0; i < mRilCardStatus.num_applications; i++) {
        mRilCardStatus.applications[i].app_state = RIL_APPSTATE_UNKNOWN;
    }

    OnRequestComplete(RIL_E_SUCCESS, (char *) &mRilCardStatus, sizeof(mRilCardStatus));
    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED);
    LEAVE_FUNC();
    return 0;
}

bool SimService::IsPossibleToPassInRadioOffState(int request_id) {
    switch (request_id) {
        case RIL_REQUEST_GET_SIM_STATUS:
        case RIL_REQUEST_SIM_IO:
        case RIL_REQUEST_GET_IMSI:
        case RIL_REQUEST_ENTER_SIM_PIN:
        case RIL_REQUEST_ENTER_SIM_PIN2:
        case RIL_REQUEST_ENTER_SIM_PUK:
        case RIL_REQUEST_ENTER_SIM_PUK2:
        case RIL_REQUEST_CHANGE_SIM_PIN:
        case RIL_REQUEST_CHANGE_SIM_PIN2:
        case RIL_REQUEST_SET_FACILITY_LOCK:
        case RIL_REQUEST_QUERY_FACILITY_LOCK:
        case RIL_REQUEST_ISIM_AUTHENTICATION:
        case RIL_REQUEST_SIM_OPEN_CHANNEL:
        case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC:
        case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:
        case RIL_REQUEST_SIM_CLOSE_CHANNEL:
        case RIL_REQUEST_SIM_AUTHENTICATION:
        // Secure Element
        case RIL_REQUEST_SIM_GET_ATR:
        case RIL_REQUEST_OEM_SIM_OPEN_CHANNEL:
        case RIL_REQUEST_OEM_SIM_TRANSMIT_APDU_LOGICAL:
        case RIL_REQUEST_OEM_SIM_TRANSMIT_APDU_BASIC:
        case RIL_REQUEST_OEM_SIM_PRESENT:
        case RIL_REQUEST_OEM_SIM_RESET:
        // SIM lock status
        case RIL_REQUEST_GET_SIM_LOCK_STATUS:
        // Radio Config
        case RIL_REQUEST_GET_SLOT_STATUS:
        case RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING:
        // OEM-IMS
        case RIL_REQUEST_OEM_IMS_SIM_IO:
        case RIL_REQUEST_OEM_ICC_DEPERSONALIZATION:
        //Enable UiccApp Status
        case RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT:
        //Sim card power
        case RIL_REQUEST_SET_SIM_CARD_POWER:
            break;
        default:
            return false;
    }

    return true;
}

int SimService::DoAutoDisableUicc() {
    ENTER_FUNC();

    m_nReqUiccEnableState = UICC_APP_STATE_DISABLED;

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSetUicc(m_nReqUiccEnableState);
    int nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, MSG_SIM_AUTO_DISABLE_UICC_APP_DONE);

    LEAVE_FUNC();
    return nResult;
}

int SimService::OnAutoDisableUiccDone(Message *pMsg) {
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolRespAdapter adapter(pMsg->GetModemData());
    UINT errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        if(m_nCurrentUiccEnableState != m_nReqUiccEnableState) {
            m_nCurrentUiccEnableState = m_nReqUiccEnableState;
        }
        DoGetSimStatus(pMsg);
    }
    else {
         RilLogE("%s::%s() AutoDisabledUicc failed due to CP", m_szSvcName, __FUNCTION__);
    }

    return 0;
}

int SimService::CheckDisabledIccid()
{
    ENTER_FUNC();

    int nResult = 0;

    if (mRilCardStatus.applications[0].app_state != RIL_APPSTATE_DETECTED) {
        // check whether this sim is disabled sim right before.
        string disabled_iccid = SystemProperty::Get(m_szLastDisabledIccid, "");
        if(disabled_iccid.length() && m_nCurrentUiccEnableState == UICC_APP_STATE_ENABLED) {
            RilLogI("mRilCardStatus.iccid =%s disabled_iccid=%s", mRilCardStatus.iccid, disabled_iccid.c_str());
            if(strcmp(mRilCardStatus.iccid, disabled_iccid.c_str()) == 0) {
                nResult = 1;
            }
        }
    }

    return nResult;
}

int SimService::GetGsmAppState() {
    int app_state = -1;
    V2_0::RIL_CardStatus status = GetRilCardStatus();
    int gsm_index = status.gsm_umts_subscription_app_index;
    if (gsm_index >= 0) {
        app_state = (int) status.applications[gsm_index].app_state;
    }

    return app_state;
}

V2_0::RIL_CardStatus SimService::GetRilCardStatus() {
    int ret;
    pthread_mutex_t* simStateChangeGuaranteeMutexlockHook = &s_SimStateChangeGuaranteeMutexlock;
    ret = pthread_mutex_lock(simStateChangeGuaranteeMutexlockHook);
    assert (ret == 0);

    V2_0::RIL_CardStatus status;
    memset(&status, 0, sizeof(V2_0::RIL_CardStatus));
    memcpy(&status, &mRilCardStatus, sizeof(V2_0::RIL_CardStatus));
    ret = pthread_mutex_unlock(simStateChangeGuaranteeMutexlockHook);
    assert (ret == 0);

    return status;
}

void SimService::dump(std::stringstream& ss) {
    V2_0::RIL_CardStatus rilCardStatus = GetRilCardStatus();
    ss << GetServiceName() << ":{";
    ss << "Card_state=" << IccUtil::covertCardStateToString(rilCardStatus.card_state);
    ss << ", CurrentUiccEnableState=" << IccUtil::covertUiccEnableStateToString(m_nCurrentUiccEnableState);
    ss << ", SimOperatorNumeric=" << Service::GetSimOperatorNumeric();
    ss << "}";
}
