/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */

#include <hidl-log.h>
#include <hidl/hidlhalindication_sim.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_service_common.h"
#include "ril_service_legacy.h"


#define TAG "HidlHalSimIndication"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalSimIndication::carrierInfoForImsiEncryption(int slotId,
                    int indicationType, int token, RIL_Errno e, void *response,
                    size_t responselen) {
    if (mRadioIndicationV1_2 != NULL) {
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndicationV1_2->carrierInfoForImsiEncryption(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimIndication::cdmaSubscriptionSourceChanged(int slotId,
                          int indicationType, int token, RIL_Errno e, void *response,
                          size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        int cdmaSource = ((int32_t *) response)[0];
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
        RLOGD("%s: cdmaSubscriptionSource %d", __FUNCTION__, cdmaSource);
#endif
        Return<void> retStatus = mRadioIndication->cdmaSubscriptionSourceChanged(
                convertIntToRadioIndicationType(indicationType), (CdmaSubscriptionSource) cdmaSource);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimIndication::simRefresh(int slotId,
                                 int indicationType, int token, RIL_Errno e, void *response,
                                 size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(RIL_SimRefreshResponse_v7)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        SimRefreshResult refreshResult = {};
        RIL_SimRefreshResponse_v7 *simRefreshResponse = ((RIL_SimRefreshResponse_v7 *) response);
        refreshResult.type = (V1_0::SimRefreshType) simRefreshResponse->result;
        refreshResult.efId = simRefreshResponse->ef_id;
        refreshResult.aid = Radio2HalUtils::convertCharPtrToHidlString(simRefreshResponse->aid);
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->simRefresh(
                convertIntToRadioIndicationType(indicationType), refreshResult);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL",
                __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimIndication::simStatusChanged(int slotId,
                    int indicationType, int token, RIL_Errno e, void *response,
                    size_t responselen) {
    if (mRadioIndication != NULL) {
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->simStatusChanged(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimIndication::stkEventNotify(int slotId,
                                 int indicationType, int token, RIL_Errno e, void *response,
                                 size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->stkEventNotify(
                convertIntToRadioIndicationType(indicationType),
                Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL",
                __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimIndication::stkProactiveCommand(int slotId,
                                 int indicationType, int token, RIL_Errno e, void *response,
                                 size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->stkProactiveCommand(
                convertIntToRadioIndicationType(indicationType),
                Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL",
                __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimIndication::stkSessionEnd(int slotId,
                    int indicationType, int token, RIL_Errno e, void *response,
                    size_t responselen) {
    if (mRadioIndication != NULL) {
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->stkSessionEnd(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimIndication::subscriptionStatusChanged(int slotId,
                                 int indicationType, int token, RIL_Errno e, void *response,
                                 size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        bool activate = ((int *) response)[0];
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->subscriptionStatusChanged(
                convertIntToRadioIndicationType(indicationType), activate);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL",
                __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimIndication::uiccApplicationsEnablementChanged(int slotId,
                                 int indicationType, int token, RIL_Errno e, void *response,
                                 size_t responselen) {
    if (mRadioIndicationV1_5 != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        bool result = ((int *)response)[0] > 0;
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndicationV1_5->uiccApplicationsEnablementChanged(
                convertIntToRadioIndicationType(indicationType), result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_5 == NULL",
                __FUNCTION__, slotId);
    }
    return 0;
}

