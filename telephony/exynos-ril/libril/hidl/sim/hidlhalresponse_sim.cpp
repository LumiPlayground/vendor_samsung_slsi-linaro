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
#include <hidl/hidlhalresponse_sim.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_internal.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalSimResponse"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

// in ril_service_legacy.cpp
extern IccIoResult responseIccIo(RadioResponseInfo& responseInfo, int serial, int responseType,
                            RIL_Errno e, void *response, size_t responseLen);
extern int responseIntOrEmpty(RadioResponseInfo& responseInfo, int serial, int responseType,
                            RIL_Errno e, void *response, size_t responseLen);

int HidlHalSimResponse::areUiccApplicationsEnabledResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        bool enabled = false;
        if (response != NULL && responseLen > 0) {
            enabled = ((int *)response)[0] > 0;
        }

        Return<void> retStatus =
                mRadioResponseV1_5->areUiccApplicationsEnabledResponse(responseInfo, enabled);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::changeIccPin2ForAppResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                                size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int remainingRetries
                = responseIntOrEmpty(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus
                = mRadioResponse->changeIccPin2ForAppResponse(responseInfo, remainingRetries);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimResponse::changeIccPinForAppResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                                size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int remainingRetries
                = responseIntOrEmpty(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus
                = mRadioResponse->changeIccPinForAppResponse(responseInfo, remainingRetries);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimResponse::enableUiccApplicationsResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponseV1_5->enableUiccApplicationsResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::getAllowedCarriersResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e,
                                      void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        CarrierRestrictions carrierInfo = {};
        bool allAllowed = true;
        if (response == NULL) {
#if VDBG
            // buider in vendor is not implemented for RIL_CarrierRestrictions
            // So NULL comes
            RLOGD("%s response is NULL: all allowed", __FUNCTION__);
#endif
            carrierInfo.allowedCarriers.resize(0);
            carrierInfo.excludedCarriers.resize(0);
        } else if (responseLen != sizeof(RIL_CarrierRestrictions)) {
            RLOGE("%s Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            RIL_CarrierRestrictions *pCr = (RIL_CarrierRestrictions *)response;
            if (pCr->len_allowed_carriers > 0 || pCr->len_excluded_carriers > 0) {
                allAllowed = false;
            }

            carrierInfo.allowedCarriers.resize(pCr->len_allowed_carriers);
            for (int i = 0; i < pCr->len_allowed_carriers; i++) {
                RIL_Carrier *carrier = pCr->allowed_carriers + i;
                carrierInfo.allowedCarriers[i].mcc = carrier->mcc;
                carrierInfo.allowedCarriers[i].mnc = carrier->mnc;
                carrierInfo.allowedCarriers[i].matchType = (CarrierMatchType) carrier->match_type;
                carrierInfo.allowedCarriers[i].matchData = carrier->match_data;
            }

            carrierInfo.excludedCarriers.resize(pCr->len_excluded_carriers);
            for (int i = 0; i < pCr->len_excluded_carriers; i++) {
                RIL_Carrier *carrier = pCr->excluded_carriers + i;
                carrierInfo.excludedCarriers[i].mcc = carrier->mcc;
                carrierInfo.excludedCarriers[i].mnc = carrier->mnc;
                carrierInfo.excludedCarriers[i].matchType = (CarrierMatchType) carrier->match_type;
                carrierInfo.excludedCarriers[i].matchData = carrier->match_data;
            }
        }

        Return<void> retStatus = mRadioResponse->getAllowedCarriersResponse(responseInfo,
                allAllowed, carrierInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::getAllowedCarriersResponse_1_4(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                                size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_4::CarrierRestrictionsWithPriority carrierInfo = {};
        V1_4::SimLockMultiSimPolicy multiSimPolicy = V1_4::SimLockMultiSimPolicy::NO_MULTISIM_POLICY;

        if (response == NULL) {
#if VDBG
            // buider in vendor is not implemented for RIL_CarrierRestrictions_V1_4
            // So NULL comes
            RLOGD("%s response is NULL: all allowed", __FUNCTION__);
#endif
            carrierInfo.allowedCarriers.resize(0);
            carrierInfo.excludedCarriers.resize(0);
        } else if (responseLen != sizeof(RIL_CarrierRestrictions_V1_4)) {
            RLOGE("%s Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            RIL_CarrierRestrictionsWithPriority *pCr =
                    &((RIL_CarrierRestrictions_V1_4 *)response)->carriers;

            carrierInfo.allowedCarriers.resize(pCr->len_allowed_carriers);
            for(int i = 0; i < pCr->len_allowed_carriers; i++) {
                RIL_Carrier *carrier = pCr->allowed_carriers + i;
                carrierInfo.allowedCarriers[i].mcc = carrier->mcc;
                carrierInfo.allowedCarriers[i].mnc = carrier->mnc;
                carrierInfo.allowedCarriers[i].matchType = (CarrierMatchType) carrier->match_type;
                carrierInfo.allowedCarriers[i].matchData = carrier->match_data;
            }

            carrierInfo.excludedCarriers.resize(pCr->len_excluded_carriers);
            for(int i = 0; i < pCr->len_excluded_carriers; i++) {
                RIL_Carrier *carrier = pCr->excluded_carriers + i;
                carrierInfo.excludedCarriers[i].mcc = carrier->mcc;
                carrierInfo.excludedCarriers[i].mnc = carrier->mnc;
                carrierInfo.excludedCarriers[i].matchType = (CarrierMatchType) carrier->match_type;
                carrierInfo.excludedCarriers[i].matchData = carrier->match_data;
            }

            carrierInfo.allowedCarriersPrioritized = pCr->allowedCarriersPrioritized;
            multiSimPolicy = (V1_4::SimLockMultiSimPolicy)
                    ((RIL_CarrierRestrictions_V1_4 *)response)->multiSimPolicy;
        }

        Return<void> retStatus = mRadioResponseV1_4->
                getAllowedCarriersResponse_1_4(responseInfo, carrierInfo, multiSimPolicy);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL", __FUNCTION__, slotId);
        this->getAllowedCarriersResponse(slotId, responseType, serial, e,
                response, responseLen);
    }
    return 0;
}

int HidlHalSimResponse::getCDMASubscriptionResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        int numStrings = responseLen / sizeof(char *);
        hidl_string emptyString;
        if (response == NULL || numStrings != 5) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
            Return<void> retStatus
                    = mRadioResponse->getCDMASubscriptionResponse(
                    responseInfo, emptyString, emptyString, emptyString, emptyString, emptyString);
            checkReturnStatus(retStatus);
        } else {
            char **resp = (char **) response;
            Return<void> retStatus
                    = mRadioResponse->getCDMASubscriptionResponse(
                    responseInfo,
                    Radio2HalUtils::convertCharPtrToHidlString(resp[0]),
                    Radio2HalUtils::convertCharPtrToHidlString(resp[1]),
                    Radio2HalUtils::convertCharPtrToHidlString(resp[2]),
                    Radio2HalUtils::convertCharPtrToHidlString(resp[3]),
                    Radio2HalUtils::convertCharPtrToHidlString(resp[4]));
            checkReturnStatus(retStatus);
        }
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::getCdmaSubscriptionSourceResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        int source = CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM;
        if (response != NULL && responseLen > 0) {
            source = ((int *)response)[0];
        }

        Return<void> retStatus = mRadioResponse->getCdmaSubscriptionSourceResponse(responseInfo,
                (CdmaSubscriptionSource) source);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::getFacilityLockForAppResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseIntOrEmpty(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus = mRadioResponse->getFacilityLockForAppResponse(responseInfo,
                ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::getIMSIForAppResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus = mRadioResponse->getIMSIForAppResponse(responseInfo,
                Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimResponse::iccCloseLogicalChannelResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus = mRadioResponse->iccCloseLogicalChannelResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::iccIoForAppResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        IccIoResult result = responseIccIo(responseInfo, serial, responseType, e, response,
                        responseLen);

        Return<void> retStatus = mRadioResponse->iccIOForAppResponse(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::iccOpenLogicalChannelResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        int channelId = -1;
        hidl_vec<int8_t> selectResponse;
        int numInts = responseLen / sizeof(int);
        if (response == NULL || responseLen % sizeof(int) != 0) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) {
                responseInfo.error = RadioError::INVALID_RESPONSE;
            }
        } else {
            int *pInt = (int *) response;
            channelId = pInt[0];
            selectResponse.resize(numInts - 1);
            for (int i = 1; i < numInts; i++) {
                selectResponse[i - 1] = (char) pInt[i];
            }
        }

        Return<void> retStatus = mRadioResponse->
                iccOpenLogicalChannelResponse(responseInfo, channelId, selectResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::iccTransmitApduBasicChannelResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        IccIoResult result = responseIccIo(responseInfo, serial, responseType, e, response,
                responseLen);

        Return<void> retStatus = mRadioResponse->
                iccTransmitApduBasicChannelResponse(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::iccTransmitApduLogicalChannelResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        IccIoResult result = responseIccIo(responseInfo, serial, responseType, e, response,
                responseLen);

        Return<void> retStatus = mRadioResponse->
                iccTransmitApduLogicalChannelResponse(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::reportStkServiceIsRunningResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus = mRadioResponse->reportStkServiceIsRunningResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::requestIccSimAuthenticationResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        IccIoResult result = responseIccIo(responseInfo, serial, responseType, e, response,
                responseLen);

        Return<void> retStatus = mRadioResponse->
                requestIccSimAuthenticationResponse(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::sendEnvelopeResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus = mRadioResponse->sendEnvelopeResponse(responseInfo,
                Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::sendEnvelopeWithStatusResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        IccIoResult result = responseIccIo(responseInfo, serial, responseType, e,
                response, responseLen);

        Return<void> retStatus = mRadioResponse->
                sendEnvelopeWithStatusResponse(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::sendTerminalResponseToSimResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus = mRadioResponse->sendTerminalResponseToSimResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::setAllowedCarriersResponse_1_4(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus = mRadioResponseV1_4->setAllowedCarriersResponse_1_4(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}


int HidlHalSimResponse::setCarrierInfoForImsiEncryptionResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponseV1_1 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus = mRadioResponseV1_1->
                setCarrierInfoForImsiEncryptionResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_1 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::setCdmaSubscriptionSourceResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus = mRadioResponse->setCdmaSubscriptionSourceResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::setFacilityLockForAppResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseIntOrEmpty(responseInfo, serial, responseType, e, response, responseLen);

        Return<void> retStatus = mRadioResponse->setFacilityLockForAppResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalSimResponse::setSimCardPowerResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        if (mRadioResponseV1_1 != NULL) {
            Return<void> retStatus = mRadioResponseV1_1->setSimCardPowerResponse_1_1(responseInfo);
            checkReturnStatus(retStatus);
        } else {
            RLOGD("%s: radioService[%d]->mRadioResponseV1_1 == NULL", __FUNCTION__, slotId);
            Return<void> retStatus = mRadioResponse->setSimCardPowerResponse(responseInfo);
            checkReturnStatus(retStatus);
        }
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL && "
                "radioService[%d]->mRadioResponseV1_1 == NULL", __FUNCTION__, slotId, slotId);
    }

    return 0;
}

int HidlHalSimResponse::setUiccSubscriptionResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus = mRadioResponse->setUiccSubscriptionResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);

    }

    return 0;
}

int HidlHalSimResponse::supplyIccPin2ForAppResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseIntOrEmpty(responseInfo, serial, responseType, e, response, responseLen);

        Return<void> retStatus = mRadioResponse->supplyIccPin2ForAppResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);

    }

    return 0;
}

int HidlHalSimResponse::supplyIccPinForAppResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseIntOrEmpty(responseInfo, serial, responseType, e, response, responseLen);

        Return<void> retStatus = mRadioResponse->supplyIccPinForAppResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);

    }

    return 0;
}

int HidlHalSimResponse::supplyIccPuk2ForAppResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseIntOrEmpty(responseInfo, serial, responseType, e, response, responseLen);

        Return<void> retStatus = mRadioResponse->supplyIccPuk2ForAppResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);

    }

    return 0;
}

int HidlHalSimResponse::supplyIccPukForAppResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseIntOrEmpty(responseInfo, serial, responseType, e, response, responseLen);

        Return<void> retStatus = mRadioResponse->supplyIccPukForAppResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);

    }

    return 0;
}

int HidlHalSimResponse::supplySimDepersonalizationResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_5::PersoSubstate persoType = V1_5::PersoSubstate::UNKNOWN;
        int32_t remainingRetries = -1;
        if (response == NULL || responseLen != (sizeof(int) * 2)) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int *pRet = (int *)response;
            persoType = (V1_5::PersoSubstate)pRet[0];
            remainingRetries = pRet[1];
        }

        Return<void> retStatus = mRadioResponseV1_5->
                supplySimDepersonalizationResponse(responseInfo, persoType, remainingRetries);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);

    }

    return 0;
}

