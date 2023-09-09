/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <librilutils/systemproperty.h>
#include <librilutils/textutils.h>
#include "emergencynumbermanager.h"
#include "emergencynumberupdater.h"
#include "networkservicebase.h"
#include "rillog.h"
#include "iccUtil.h"

#define ECC_DBG false
// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG, format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) \
    if (ECC_DBG)             \
    CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) \
    if (ECC_DBG)             \
    CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG, format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) \
    if (ECC_DBG)             \
    CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG, format, ##__VA_ARGS__)

EmergencyNumberManager::EmergencyNumberManager(NetworkServiceBase *service)
    : mService(NULL) {
    mService = service;
    snprintf(TAG, sizeof(TAG) - 1, "EmergencyNumberManager_%d", mService->GetRilSocketId());

    mDbEccInfos[SIM_DB_ECC_INFO_IDX].Clear();
    mDbEccInfos[NET_DB_ECC_INFO_IDX].Clear();
    mRadioEccInfos[SIT_ECC_INFO_TYPE_ENL].Clear();
    mRadioEccInfos[SIT_ECC_INFO_TYPE_EENL].Clear();
    mTempEmergencyNumber.Clear();

    mCardState = RIL_CARDSTATE_ABSENT;
    mAppState = RIL_APPSTATE_UNKNOWN;
    mEmcBearerSupported = false;
    mIsEmcLabTestMode = false;
    mIsDisabledESim = false;

    mEmergencyNumberUpdater = new EmergencyNumberUpdater(mService, this);
    mDbEccInfoLoader = new DbEccInfoLoader(mService, this);
}

EmergencyNumberManager::~EmergencyNumberManager() {
    mService = NULL;
    if (NULL != mEmergencyNumberUpdater) {
        delete mEmergencyNumberUpdater;
        mEmergencyNumberUpdater = NULL;
    }
    if (NULL != mDbEccInfoLoader) {
        delete mDbEccInfoLoader;
        mDbEccInfoLoader = NULL;
    }
}

/**
 * Get database ECC info.
 * The call handler will get this function and update this.
 *
 * @return EccInfo the database ECC info.
 */
EccInfo *EmergencyNumberManager::GetDbEccInfos() {
    return mDbEccInfos;
}

/**
 * Get radio ECC info.
 * The call handler will get this function and update this.
 *
 * @return EccInfo the radio ECC info.
 */
EccInfo *EmergencyNumberManager::GetRadioEccInfos() {
    return mRadioEccInfos;
}

/**
 * Get SIM card state.
 *
 * @return SIM card state. See @RIL_CardState.
 */
int EmergencyNumberManager::GetCardState() {
    return mCardState;
}

/**
 * Get SIM app state.
 *
 * @return SIM app state. See @RIL_AppState.
 */
int EmergencyNumberManager::GetAppState() {
    return mAppState;
}

/**
 * Get the ESIM disabled state.
 *
 * @return true if ESIM state is disabled.
 */
bool EmergencyNumberManager::IsDisabledESim() {
    return mIsDisabledESim;
}

/**
 * Get EMC bearer is supported.
 *
 * @return true if EMC bearer supports over LTE or NR SA.
 */
bool EmergencyNumberManager::IsEmcBearerSupported() {
    return mEmcBearerSupported;
}

/**
 * Get EMC lab test mode
 *
 * @return EMC lab test mode value.
 */
bool EmergencyNumberManager::IsEmcLabTestMode() {
    return mIsEmcLabTestMode;
}

/**
 * Set the card and app state of SIM.
 * When card or app state is changed, UpdateDbEccInfo is called.
 *
 * @param cardState SIM card state. See @RIL_CardState.
 * @param appState SIM app state. See @RIL_AppState.
 * @param isDisabledESim The ESIM disabled state.
 */
void EmergencyNumberManager::SetSimState(int cardState, int appState, bool isDisabledESim) {
    if (mCardState != cardState || mAppState != appState || mIsDisabledESim != isDisabledESim) {
        mCardState = cardState;
        mAppState = appState;
        mIsDisabledESim = isDisabledESim;

        RilLogI("[%s] %s() SIM state is changed card state: %s, app state: %s.", TAG, __FUNCTION__,
                IccUtil::covertCardStateToString((RIL_CardState)cardState).c_str(),
                IccUtil::covertAppStateToString((RIL_AppState)appState).c_str());
        UpdateDbEccInfo();
    } else {
        RilLogV("[%s] %s() SIM state is not changed.", TAG, __FUNCTION__);
    }
}

/**
 * Set whether EMC bearer supports over LTE or NR SA.
 * See EMC_SERVICE field in SIT_GET_PS_REG_STATE.
 *
 * @param isEmcBearerSupported true if EMC bearer is 1(available) over LTE or
 *                             EMC_NR_CONNECTED_TO_5GCN over NR SA or
                               EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN over NR SA.
 */
void EmergencyNumberManager::SetEmcBearerSupported(bool isEmcBearerSupported) {
    if (mEmcBearerSupported != isEmcBearerSupported) {
        mEmcBearerSupported = isEmcBearerSupported;
        RilLogI("[%s] %s() EMC Bearer is changed: %s.", TAG, __FUNCTION__,
                (mEmcBearerSupported ? "true" : "false"));
        UpdateEmergencyNumberList();
    } else {
        RilLogV("[%s] %s() EMC Bearer is not changed.", TAG, __FUNCTION__);
    }
}

/**
 * Set on/off value from "EMC Lab test setting" of test mode app.
 * RIL_REQUEST_OEM_SET_EMC_LAB_TEST_MODE is delivered from the test mode app.
 *
 * @param isEmcLabTestMode true if EMC lab test mode is on.
 */
void EmergencyNumberManager::SetEmcLabTestMode(bool isEmcLabTestMode) {
    if (mIsEmcLabTestMode != isEmcLabTestMode) {
        mIsEmcLabTestMode = isEmcLabTestMode;
        RilLogI("[%s] %s() Lab test mode is changed: %s.", TAG, __FUNCTION__,
                (mIsEmcLabTestMode ? "true" : "false"));
        UpdateEmergencyNumberList();
    } else {
        RilLogV("[%s] %s() Lab test mode is not changed.", TAG, __FUNCTION__);
    }
}

/**
 * Add the emergency number lists to RIL and AIMS emergency number list.
 * and send unsolicite response.
 */
void EmergencyNumberManager::UpdateEmergencyNumberList() {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);

    mEmergencyNumberUpdater->Update(&mTempEmergencyNumber, mDbEccInfos, mRadioEccInfos);
}

/**
 * Update ECC Info from database.
 *
 * @param forceToUpdate force to call UpdateEmergencyNumberList API.
 *                      By the conditions, the emergency number list can be changed.
 *                      false is only came when receiving operator reponse.
 * @param callByOpposite If one stack has an update, the other stack must be updated,
 *                       as it may vary depending on the state of the other stack..
 */
void EmergencyNumberManager::UpdateDbEccInfo(bool forceToUpdate, bool callByOpposite) {
    RilLogV("[%s] %s() forceToUpdate: %s, callByOpposite: %s.",
            TAG, __FUNCTION__, (forceToUpdate ? "true" : "false"),
            (callByOpposite ? "true" : "false"));
    bool needUpdate = false;
    needUpdate = mDbEccInfoLoader->Load(mDbEccInfos);

    if (needUpdate || forceToUpdate) {
        UpdateEmergencyNumberList();

        if (!callByOpposite) {
            UpdateOppositeDbEccInfo();
        }
    }
}

/**
 * Update opposite ECC Info asynchronously.
 */
void EmergencyNumberManager::UpdateOppositeDbEccInfo() {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    BYTE callByOpposite = 1;  //  true
    RequestData *pData = RilParser::CreateRawData(RIL_REQUEST_OEM_UPDATE_EMERGENCY_NUMBER_LIST, 0,
                          (char *)&callByOpposite, sizeof(callByOpposite));
    if (NULL != pData) {
        Message *msg = Message::ObtainMessage(pData, RIL_SERVICE_MISC, MSG_MISC_OEM_UPDATE_EMERGENCY_NUMBER_LIST);
        RilContext *curRilContext = mService->GetRilContext();
        if (NULL != curRilContext) {
            RilContext* pTargetRilContext = curRilContext->GetOppositeRilContext();
            if (NULL != pTargetRilContext) {
                if (pTargetRilContext->GetServiceManager()->SendMessage(msg) < 0) {
                    if (msg) {
                        delete msg;
                    }
                }
            } else {
                delete msg;
            }
        } else {
            delete msg;
        }
    }
}

void EmergencyNumberManager::ResetDbEccId(bool forceToErase) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    mDbEccInfoLoader->ResetDbEccId(forceToErase);
}

/**
 * Get emergency call routing value that matches the number.
 * The default value is true.
 *
 * @param number the emergency number.
 * @return emergency call routing value that matches the number.
 */
bool EmergencyNumberManager::IsEmergencyCallRouting(const char *number,
                                                    bool *normalRoutingWhenInService) {
    bool ret = true;
    int i, j;
    int numberLen = strlen(number);

    // Check temp ECC first.
    if (mTempEmergencyNumber.GetNumberLen() != 0 &&
        strncmp(mTempEmergencyNumber.GetNumber(), number,
                MIN(mTempEmergencyNumber.GetNumberLen(), numberLen)) == 0) {
        ret = mTempEmergencyNumber.IsEmergencyCallRouting();
    }

    int radioEccInfoType = mEmergencyNumberUpdater->GetRadioEccInfoType(mRadioEccInfos);
    EmergencyNumber *emergencyNumber = NULL;
    for (i = 0; i < mRadioEccInfos[radioEccInfoType].GetCount(); i++) {
        emergencyNumber = mRadioEccInfos[radioEccInfoType].GetEmergencyNumber(i);
        if (strncmp(emergencyNumber->GetNumber(), number,
                    MIN(emergencyNumber->GetNumberLen(), numberLen)) == 0) {
            ret = emergencyNumber->IsEmergencyCallRouting();
        }
    }

    for (i = SIM_DB_ECC_INFO_IDX; i < MAX_DB_ECC_INFO_IDX; i++) {
        for (j = 0; j < mDbEccInfos[i].GetCount(); j++) {
            emergencyNumber = mDbEccInfos[i].GetEmergencyNumber(j);
            if (emergencyNumber->IsEmergencyType() &&
                strncmp(emergencyNumber->GetNumber(), number,
                        MIN(emergencyNumber->GetNumberLen(), numberLen)) == 0) {
                if (normalRoutingWhenInService != NULL &&
                    EmergencyNumberUpdater::HasCondition(emergencyNumber->GetConditions(),
                                                         SIT_EMERGENCY_CONDITION_UI_ONLY)) {
                    *normalRoutingWhenInService = true;
                }
                ret = emergencyNumber->IsEmergencyCallRouting();
                break;
            }
        }
    }

    RilLogV("[%s] %s() number: %s, emergencyCallRouting: %s.", TAG, __FUNCTION__,
            number, (ret ? "true" : "false"));
    return ret;
}

/**
 * Get the category that matches the number.
 * If the category from AOSP database is UNSPECIFIED,
 * it should be replaced with the category from database emergency number.
 *
 * @param number the emergency number.
 * @return category the bitfield of @1.4::EmergencyServiceCategory(s).
 */
int EmergencyNumberManager::GetEmergencyNumberCategory(const char *number) {
    int ret = RIL_EMERGENCY_CATEGORY_UNSPECIFIED;
    int i, j;
    int numberLen = strlen(number);

    // Check temp emergency number first.
    if (mTempEmergencyNumber.GetNumberLen() != 0 &&
        strncmp(mTempEmergencyNumber.GetNumber(), number,
                MIN(mTempEmergencyNumber.GetNumberLen(), numberLen)) == 0) {
        ret = mTempEmergencyNumber.GetCategory();
    }

    EmergencyNumber *emergencyNumber = NULL;
    for (i = SIM_DB_ECC_INFO_IDX; i < MAX_DB_ECC_INFO_IDX; i++) {
        for (j = 0; j < mDbEccInfos[i].GetCount(); j++) {
            emergencyNumber = mDbEccInfos[i].GetEmergencyNumber(j);
            if (strncmp(emergencyNumber->GetNumber(), number,
                        MIN(emergencyNumber->GetNumberLen(), numberLen)) == 0) {
                ret = emergencyNumber->GetCategory();
            }
        }
    }

    RilLogV("[%s] %s() number: %s, category: %s.", TAG, __FUNCTION__,
            number, DbEccInfoLoader::EmergencyCategoryToString(ret));
    return ret;
}

/**
 * Set temp emergency number for SIP 380 redial case.
 *
 * @param operation Add(=1) and remove(=0) operations.
 * @param number_len the length of emergency number
 * @param number the emergency number
 * @param category the bitfield of @1.4::EmergencyServiceCategory(s).
 */
void EmergencyNumberManager::SetTempEmergencyNumber(int operation, int number_len,
                                                    char *number, int category) {
    RilLogV("[%s] %s() operation: %s, Number: %s, category: %s.", TAG, __FUNCTION__,
            (operation == 1 ? "Add" : "Remove"), number, DbEccInfoLoader::EmergencyCategoryToString(category));
    mTempEmergencyNumber.Clear();
    if (operation == 1) {  //  Add temp ECC
        int len = MIN(number_len, MAX_EMERGENCY_NUMBER_LEN);
        mTempEmergencyNumber.Add("" /*mcc*/, "" /*mnc*/, number, len, category,
                                  SIT_EMERGENCY_CONDITION_ALWAYS,
                                  RIL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING);
    }
    // update
    ResetDbEccId(true /*forceToErase*/);
    UpdateDbEccInfo();
}
