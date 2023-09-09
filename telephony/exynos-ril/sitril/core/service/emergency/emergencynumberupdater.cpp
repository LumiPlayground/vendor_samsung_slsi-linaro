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
#include "networkutils.h"

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

EmergencyNumberUpdater::EmergencyNumberUpdater(NetworkServiceBase *service,
                                               EmergencyNumberManager *emergencyNumberManager)
    : mService(NULL), mEmergencyNumberManager(NULL) {
    mService = service;
    mEmergencyNumberManager = emergencyNumberManager;
    snprintf(TAG, sizeof(TAG) - 1, "EmergencyNumberUpdater_%d", mService->GetRilSocketId());

    memset(mRilEmergencyNumberList, 0x00, sizeof(mRilEmergencyNumberList));
    memset(&mAimsEmergencyNumberList, 0x00, sizeof(mAimsEmergencyNumberList));
}

EmergencyNumberUpdater::~EmergencyNumberUpdater() {
    mService = NULL;
    mEmergencyNumberManager = NULL;
}

/**
 * Check if conditions includes the bitmask of condition.
 *
 * @param conditions A set of bitmask
 * @param condition A bitmask
 * @return true if conditions includes the bitmask of condition
 */
bool EmergencyNumberUpdater::HasCondition(int conditions, int condition) {
    return (conditions & condition) == condition;
}

/**
 * Convert conditions value to string.
 *
 * @param conditions a set of condition values. Refer to sit_emergency_condition.
 */
string EmergencyNumberUpdater::ConditionsToString(int conditions) {
    string str;
    static const char *CONDITIONS[] = {"ALWAYS", "NO_SIM", "TESTMODE_NORMAL",
                                       "NOT_IMS_REGI", "UI_ONLY", "EMC_AVAILABLE",
                                       "NOT_CS_REGI", "BOTH_NOT_REGI"};
    for (int i = 0, j = 1; j <= SIT_EMERGENCY_CONDITION_BOTH_NOT_REGI; j = 1 << i) {
        if (HasCondition(conditions, j)) {
            str.append(CONDITIONS[i]);
            str.append(" ");
        }
        i++;
    }
    return str;
}

/**
 * Print the emergency number list in ECC info.
 *
 * @param eccInfo the ECC info to print the emergency number list.
 *                Either mDbEccInfos or mRadioEccInfos can be came.
 */
void EmergencyNumberUpdater::PrintEccInfo(EccInfo *eccInfo) {
    for (int i = 0; i < eccInfo->GetCount(); i++) {
        RilLogV("[%s] %s() EccInfo_%d number= %s, category= %d, conditions= %x.",
                TAG, __FUNCTION__, i,
                eccInfo->GetEmergencyNumber(i)->GetNumber(),
                eccInfo->GetEmergencyNumber(i)->GetCategory(),
                eccInfo->GetEmergencyNumber(i)->GetConditions());
    }
}

/**
 * Add emergency number to RIL and AIMS emergency number.
 *
 * @param rilEmergencyNumber the destination where the emergency number is added.
 * @param aimsEmergencyNumber the destination where the emergency number is added.
 * @param emergencyNumber the source to be added to destination.
 */
void EmergencyNumberUpdater::AddEmergencyNumber(RIL_EmergencyNumber *rilEmergencyNumber,
                                                RIL_AimsEmergencyNumber *aimsEmergencyNumber,
                                                EmergencyNumber *emergencyNumber) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    rilEmergencyNumber->mcc = emergencyNumber->GetMcc();
    rilEmergencyNumber->mnc = emergencyNumber->GetMnc();
    rilEmergencyNumber->number = emergencyNumber->GetNumber();
    rilEmergencyNumber->categories = emergencyNumber->GetCategory();
    rilEmergencyNumber->sources = emergencyNumber->GetSource();
    rilEmergencyNumber->len_urns = emergencyNumber->GetUrnCount();
    rilEmergencyNumber->urns = emergencyNumber->GetUrns();
    for (int i = 0; i < emergencyNumber->GetUrnCount(); i++) {
        rilEmergencyNumber->urns[i] = emergencyNumber->GetUrn(i);
    }

    aimsEmergencyNumber->mcc_len = MIN(strlen(emergencyNumber->GetMcc()), MAX_MCC_LEN);
    strncpy(aimsEmergencyNumber->mcc, emergencyNumber->GetMcc(), aimsEmergencyNumber->mcc_len);
    aimsEmergencyNumber->mnc_len = MIN(strlen(emergencyNumber->GetMnc()), MAX_MNC_LEN);
    strncpy(aimsEmergencyNumber->mnc, emergencyNumber->GetMnc(), aimsEmergencyNumber->mnc_len);
    aimsEmergencyNumber->number_len = MIN(strlen(emergencyNumber->GetNumber()), MAX_EMERGENCY_NUMBER_LEN);
    strncpy(aimsEmergencyNumber->number, emergencyNumber->GetNumber(), aimsEmergencyNumber->number_len);
    aimsEmergencyNumber->categories = emergencyNumber->GetCategory();
    aimsEmergencyNumber->sources = emergencyNumber->GetSource();
    // AIMS doesn't have URN field, but routing field exists.
    aimsEmergencyNumber->routing = (emergencyNumber->IsEmergencyCallRouting()
                                        ? RIL_EMERGENCY_CALL_ROUTING_EMERGENCY
                                        : RIL_EMERGENCY_CALL_ROUTING_NORMAL);
}

/**
 * Update the information of emergency number and routing regarding conditions.
 *
 * @param emergencyNumber the source to be updated regarding conditions and states.
 */
void EmergencyNumberUpdater::UpdateEmergencyTypeAndRouting(EmergencyNumber *emergencyNumber) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    int conditions = emergencyNumber->GetConditions();

    int voiceRegState = 0;
    int imsRegState = RIL_IMS_NOT_REGISTERED;
    int cardState = mEmergencyNumberManager->GetCardState();
    bool isDisabledESim = mEmergencyNumberManager->IsDisabledESim();
    bool emcBearerSupported = mEmergencyNumberManager->IsEmcBearerSupported();
    bool isEmcLabTestMode = mEmergencyNumberManager->IsEmcLabTestMode();

    RilProperty *property = mService->GetRilContextProperty();
    if (property != NULL) {
        voiceRegState = property->GetInt(RIL_CONTEXT_NET_VOICE_REGISTRATION_STATE, 0);
        imsRegState = property->GetInt(RIL_CONTEXT_IMS_REGISTRATION, 0);
    }

    RilLogI("[%s] %s() found entry emergency number: %s, card state: %s,"
            " voice regi state: %s, imsRegState: %s, mIsEmcLabTestMode: %s, mEmcBearerSupported: %s,"
            " conditions=[ %s].",
            TAG, __FUNCTION__, emergencyNumber->GetNumber(),
            IccUtil::covertCardStateToString((RIL_CardState)cardState).c_str(),
            NetworkUtils::getRegStateString(voiceRegState),
            (imsRegState == RIL_IMS_NOT_REGISTERED ? "NOT_REGISTERED" : "REGISTERED"),
            (isEmcLabTestMode ? "true" : "false"),
            (emcBearerSupported ? "true" : "false"),
            ConditionsToString(conditions).c_str());

    if (HasCondition(conditions, SIT_EMERGENCY_CONDITION_UI_ONLY)) {
        emergencyNumber->SetEmergencyType(true);
        emergencyNumber->SetEmergencyCallRouting(false);
    }
    if (HasCondition(conditions, SIT_EMERGENCY_CONDITION_ALWAYS)) {
        emergencyNumber->SetEmergencyType(true);
        emergencyNumber->SetEmergencyCallRouting(true);
    }
    if (HasCondition(conditions, SIT_EMERGENCY_CONDITION_NO_SIM)) {
        if (cardState == RIL_CARDSTATE_ABSENT ||
            isDisabledESim) {
            emergencyNumber->SetEmergencyType(true);
            emergencyNumber->SetEmergencyCallRouting(true);
        }
    }
    if (HasCondition(conditions, SIT_EMERGENCY_CONDITION_NOT_IMS_REGI)) {
        if (imsRegState != RIL_IMS_REGISTERED) {
            emergencyNumber->SetEmergencyType(true);
            emergencyNumber->SetEmergencyCallRouting(true);
        }
    }
    if (HasCondition(conditions, SIT_EMERGENCY_CONDITION_EMC_AVAILABLE)) {
        if (emcBearerSupported) {
            emergencyNumber->SetEmergencyType(true);
            emergencyNumber->SetEmergencyCallRouting(true);
        }
    }
    if (HasCondition(conditions, SIT_EMERGENCY_CONDITION_NOT_CS_REGI)) {
        if (!(voiceRegState == REGISTERED_HOME || voiceRegState == REGISTERED_ROAMING)) {
            emergencyNumber->SetEmergencyType(true);
            emergencyNumber->SetEmergencyCallRouting(true);
        }
    }
    if (HasCondition(conditions, SIT_EMERGENCY_CONDITION_BOTH_NOT_REGI)) {
        if (imsRegState != RIL_IMS_REGISTERED &&
            !(voiceRegState == REGISTERED_HOME || voiceRegState == REGISTERED_ROAMING)) {
            emergencyNumber->SetEmergencyType(true);
            emergencyNumber->SetEmergencyCallRouting(true);
        }
    }
    if (HasCondition(conditions, SIT_EMERGENCY_CONDITION_TESTMODE_NORMAL)) {
        if (isEmcLabTestMode) {
            emergencyNumber->SetEmergencyType(false);
            emergencyNumber->SetEmergencyCallRouting(false);
        }
    }
}

/**
 * Add the emergency number lists to RIL and AIMS emergency number list.
 * Temp emergency number and database ECC info and radio ECC info are added.
 *
 * @param rilEmergencyNumberList the destination where the emergency number is added.
 * @param aimsEmergencyNumberList the destination where the emergency number is added.
 * @param tempEmergencyNumber The emergency number which is added temporary by SIP 380 redial case.
 * @param dbEccInfos the source which is database ECC infos(considering SIM_NET_COMBINE).
 * @param radioEccInfo the source which is radio ECC info.
 * @return count total count of merged emergency number list.
 */
int EmergencyNumberUpdater::AddEmergencyNumberList(RIL_EmergencyNumber *rilEmergencyNumberList,
                                                   RIL_AimsEmergencyNumber *aimsEmergencyNumberList,
                                                   EmergencyNumber *tempEmergencyNumber,
                                                   EccInfo *dbEccInfos, EccInfo *radioEccInfo) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    int i, j, count = 0;

    if (tempEmergencyNumber->GetNumberLen() != 0) {
        UpdateEmergencyTypeAndRouting(tempEmergencyNumber);
        AddEmergencyNumber(&rilEmergencyNumberList[count],
                           &aimsEmergencyNumberList[count],
                           tempEmergencyNumber);
        count++;
    }

    EmergencyNumber *emergencyNumber = NULL;
    //  Radio ECC INFO
    for (i = 0; i < radioEccInfo->GetCount(); i++) {
        emergencyNumber = radioEccInfo->GetEmergencyNumber(i);
        UpdateEmergencyTypeAndRouting(emergencyNumber);
        AddEmergencyNumber(&rilEmergencyNumberList[count],
                           &aimsEmergencyNumberList[count],
                           emergencyNumber);
        count++;
    }

    //  DB ECC INFOS
    for (i = SIM_DB_ECC_INFO_IDX; i < MAX_DB_ECC_INFO_IDX; i++) {
        for (j = 0; j < dbEccInfos[i].GetCount(); j++) {
            emergencyNumber = dbEccInfos[i].GetEmergencyNumber(j);
            UpdateEmergencyTypeAndRouting(emergencyNumber);
            //  The emergency number from database can not be emergency number by conditions.
            if (emergencyNumber->IsEmergencyType()) {
                AddEmergencyNumber(&rilEmergencyNumberList[count],
                                   &aimsEmergencyNumberList[count],
                                   emergencyNumber);
                count++;
            }
        }
    }

    return count;
}

/**
 * Get radio ECC info type by EENLV and PLMN.
 * There are EENLV and PLMN information from mRadioEccInfos[SIT_ECC_INFO_TYPE_EENL].
 * If EENLV is 1 and PLMN is not same with current PLMN, ENL is used.
 *
 * @param radioEccInfos The ECC INFO using radio(network)
 * @return Either SIT_ECC_INFO_TYPE_ENL or SIT_ECC_INFO_TYPE_EENL.
 */
int EmergencyNumberUpdater::GetRadioEccInfoType(EccInfo *radioEccInfos) {
    EccInfo extendedEccInfo = radioEccInfos[SIT_ECC_INFO_TYPE_EENL];
    if (extendedEccInfo.GetCount() == 0) {
        RilLogV("[%s] %s() EENL count is 0. Use ENL.", TAG, __FUNCTION__);
        return SIT_ECC_INFO_TYPE_ENL;
    }

    string netPlmn = "";
    RilProperty *property = mService->GetRilContextProperty();
    if (property != NULL) {
        netPlmn = property->GetString(RIL_CONTEXT_NET_CURRENT_PLMN);
    }
    RilLogV("[%s] %s() EENLV: %d, EENL netPlmn: %s, cur netPlmn: %s. Use EENL.", TAG, __FUNCTION__,
            extendedEccInfo.GetEenlv(), extendedEccInfo.GetPlmn(), netPlmn.c_str());

    if (extendedEccInfo.GetEenlv() == 1 &&
        strncmp(extendedEccInfo.GetPlmn(), netPlmn.c_str(), strlen(extendedEccInfo.GetPlmn())) != 0) {
        RilLogV("[%s] %s() Use ENL.", TAG, __FUNCTION__);
        return SIT_ECC_INFO_TYPE_ENL;
    }

    RilLogV("[%s] %s() Use EENL.", TAG, __FUNCTION__);
    return SIT_ECC_INFO_TYPE_EENL;
}

/**
 * Add the emergency number lists to RIL and AIMS emergency number list.
 * and send unsolicite response.
 *
 * @param tempEmergencyNumber The emergency number which is added temporary by SIP 380 redial case.
 * @param dbEccInfos the source which is database ECC infos(considering SIM_NET_COMBINE).
 * @param radioEccInfo the source which is radio ECC info.
 */
void EmergencyNumberUpdater::Update(EmergencyNumber *tempEmergencyNumber,
                                    EccInfo *dbEccInfos, EccInfo *radioEccInfos) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);

    memset(mRilEmergencyNumberList, 0x00, sizeof(mRilEmergencyNumberList));
    memset(&mAimsEmergencyNumberList, 0x00, sizeof(mAimsEmergencyNumberList));

    int len = 0;
    len = AddEmergencyNumberList(mRilEmergencyNumberList,
                                 mAimsEmergencyNumberList.AimsEmergencyNumberList,
                                 tempEmergencyNumber,
                                 dbEccInfos,
                                 &radioEccInfos[GetRadioEccInfoType(radioEccInfos)]);
    mAimsEmergencyNumberList.count = len;

    mService->OnUnsolicitedResponse(RIL_UNSOL_EMERGENCY_NUMBER_LIST,
                                    mRilEmergencyNumberList, len * sizeof(RIL_EmergencyNumber));
    mService->OnUnsolicitedResponse(RIL_UNSOL_OEM_AIMS_EMERGENCY_NUMBER_LIST,
                                    &mAimsEmergencyNumberList,
                                    sizeof(mAimsEmergencyNumberList.count) +
                                        (len * sizeof(RIL_AimsEmergencyNumber)));
}
