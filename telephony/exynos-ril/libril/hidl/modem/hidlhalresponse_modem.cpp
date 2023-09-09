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
#include <hidl/hidlhalresponse_modem.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_internal.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalModemResponse"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalModemResponse::enableModemResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponseV1_3 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponseV1_3->enableModemResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_3 == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalModemResponse::getBasebandVersionResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                                size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->getBasebandVersionResponse(responseInfo,
                Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::getDeviceIdentityResponse(int slotId,
                         int responseType, int serial, RIL_Errno e, void *response,
                             size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        int numStrings = responselen / sizeof(char *);
        hidl_string emptyString;
        if (response == NULL || numStrings != 4) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
            Return<void> retStatus
                    = mRadioResponse->getDeviceIdentityResponse(responseInfo,
                    emptyString, emptyString, emptyString, emptyString);
            checkReturnStatus(retStatus);
        } else {
            char **resp = (char **) response;
            Return<void> retStatus = mRadioResponse->getDeviceIdentityResponse(responseInfo,
                                        Radio2HalUtils::convertCharPtrToHidlString(resp[0]),
                                        Radio2HalUtils::convertCharPtrToHidlString(resp[1]),
                                        Radio2HalUtils::convertCharPtrToHidlString(resp[2]),
                                        Radio2HalUtils::convertCharPtrToHidlString(resp[3]));
            checkReturnStatus(retStatus);
        }
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::getHardwareConfigResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                                void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RLOGD("%s::%s: mRadioResponse=%p", TAG, __FUNCTION__, mRadioResponse.get());
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<HardwareConfig> result;
        if ((response == NULL && responselen != 0)
                || responselen % sizeof(RIL_HardwareConfig) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilHardwareConfigListToHal(response, responselen, result);
        }

        Return<void> retStatus = mRadioResponse->getHardwareConfigResponse(
                responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::getModemActivityInfoResponse(int slotId,
                                int responseType, int serial, RIL_Errno e,
                                    void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        ActivityStatsInfo info;
        if (response == NULL || responselen != sizeof(RIL_ActivityStatsInfo)) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            RIL_ActivityStatsInfo *resp = (RIL_ActivityStatsInfo *)response;
            info.sleepModeTimeMs = resp->sleep_mode_time_ms;
            info.idleModeTimeMs = resp->idle_mode_time_ms;
            for(int i = 0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
                info.txmModetimeMs[i] = resp->tx_mode_time_ms[i];
            }
            info.rxModeTimeMs = resp->rx_mode_time_ms;
        }

        Return<void> retStatus
                = mRadioResponse->getModemActivityInfoResponse(responseInfo, info);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::getModemStackStatusResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponseV1_3 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        bool isEnabled = true;
        if (e == RIL_E_SUCCESS) {
            if ((response == NULL || responselen != sizeof(int))) {
                RLOGE("%s Invalid response: NULL", __FUNCTION__);
                responseInfo.error = RadioError::INVALID_RESPONSE;
            }
            else {
                isEnabled = *((int *)response) > 0;
            }
        }

        Return<void> retStatus =
                mRadioResponseV1_3->getModemStackStatusResponse(responseInfo, isEnabled);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_3 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

static void responseRadioCapability(RadioResponseInfo& responseInfo, int serial,
        int responseType, RIL_Errno e, void *response, size_t responseLen, RadioCapability& rc) {
    populateResponseInfo(responseInfo, serial, responseType, e);

    if (response == NULL || responseLen != sizeof(RIL_RadioCapability)) {
        RLOGE("%s: Invalid response", __FUNCTION__);
        if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        rc.logicalModemUuid = hidl_string();
    } else {
        Radio2HalUtils::convertRilRadioCapabilityToHal(response, responseLen, rc);
    }
}

int HidlHalModemResponse::getRadioCapabilityResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                                void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        RadioCapability result = {};
        responseRadioCapability(responseInfo, serial, responseType, e, response, responselen,
                result);
        Return<void> retStatus = mRadioResponse->getRadioCapabilityResponse(
                responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::nvReadItemResponse(int slotId,
                  int responseType, int serial, RIL_Errno e,
                      void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->nvReadItemResponse(
                responseInfo,
                Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::nvResetConfigResponse(int slotId,
                     int responseType, int serial, RIL_Errno e,
                         void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->nvResetConfigResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalModemResponse::nvWriteCdmaPrlResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->nvWriteCdmaPrlResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::nvWriteItemResponse(int slotId,
                   int responseType, int serial, RIL_Errno e,
                       void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->nvWriteItemResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::requestShutdownResponse(int slotId,
                       int responseType, int serial, RIL_Errno e,
                           void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->requestShutdownResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::sendDeviceStateResponse(int slotId,
                          int responseType, int serial, RIL_Errno e,
                              void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->sendDeviceStateResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::setRadioCapabilityResponse(int slotId,
                           int responseType, int serial, RIL_Errno e,
                               void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        RadioCapability result = {};
        responseRadioCapability(responseInfo, serial, responseType, e, response, responselen,
                result);
        Return<void> retStatus = mRadioResponse->setRadioCapabilityResponse(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::setRadioPowerResponse(int slotId,
                     int responseType, int serial, RIL_Errno e, void *response,
                         size_t responselen) {
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setRadioPowerResponse(
                responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalModemResponse::setRadioPowerResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
                mRadioResponseV1_5->setRadioPowerResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        ret = this->setRadioPowerResponse(slotId, responseType, serial, e, response, responselen);
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL, ret = %d", __FUNCTION__, slotId, ret);
    }
    return 0;
}
