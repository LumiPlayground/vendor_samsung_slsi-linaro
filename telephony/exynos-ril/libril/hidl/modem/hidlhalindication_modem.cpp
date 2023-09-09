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
#include <hidl/hidlhalindication_modem.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_service_common.h"
#include "ril_service_legacy.h"


#define TAG "HidlHalModemIndication"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalModemIndication::hardwareConfigChangedInd(int slotId,
                         int indicationType, int token, RIL_Errno e, void *response,
                         size_t responselen) {
    if (mRadioIndication != NULL) {
        if ((response == NULL && responselen != 0)
                || responselen % sizeof(RIL_HardwareConfig) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        hidl_vec<HardwareConfig> configs;
        Radio2HalUtils::convertRilHardwareConfigListToHal(response, responselen, configs);

#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->hardwareConfigChanged(
                convertIntToRadioIndicationType(indicationType), configs);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL",
                __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemIndication::modemResetInd(int slotId,
                  int indicationType, int token, RIL_Errno e, void *response,
                  size_t responselen) {
    if (mRadioIndication != NULL) {
        const char *reason = (char *)response;
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            reason = "unknown";
        }
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->modemReset(
                convertIntToRadioIndicationType(indicationType),
                Radio2HalUtils::convertCharPtrToHidlString(reason));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemIndication::radioCapabilityIndicationInd(int slotId,
                                 int indicationType, int token, RIL_Errno e, void *response,
                                 size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(RIL_RadioCapability)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        RadioCapability rc = {};
        Radio2HalUtils::convertRilRadioCapabilityToHal(response, responselen, rc);

#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->radioCapabilityIndication(
                convertIntToRadioIndicationType(indicationType), rc);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL",
                __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemIndication::radioStateChangedInd(int slotId,
                          int indicationType, int token, RIL_Errno e, void *response,
                          size_t responseLen) {
    if (mRadioIndication != NULL) {
        auto onStateRequest = [](int slotId) {
            if (s_vendorFunctions) {
                return (RadioState) CALL_ONSTATEREQUEST(slotId);;
            }
            return static_cast<RadioState>(RADIO_STATE_UNAVAILABLE);
        };
        RadioState radioState = onStateRequest(slotId);
        RLOGD("%s: radioState %d", __FUNCTION__, radioState);
        Return<void> retStatus = mRadioIndication->radioStateChanged(
                convertIntToRadioIndicationType(indicationType), radioState);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalModemIndication::rilConnectedInd(int slotId,
                    int indicationType, int token, RIL_Errno e, void *response,
                    size_t responselen) {
    if (mRadioIndication != NULL) {
        RLOGD("%s::%s", TAG, __FUNCTION__);
        Return<void> retStatus = mRadioIndication->rilConnected(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}
