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
#include <hidl/1.6/hidlhalresponse_data.h>
#include <hidl/hidlhalresponse_data.h>
#include <librilutils/radio_hal_utils.h>
#include <librilutils/radio_hal_utils_1_6.h>
#include "ril_internal.h"
#include "ril_service_1_6.h"

#define TAG "HidlHalDataResponse_1_6"

using ::android::hardware::hidl_string;
using ::android::hardware::Return;

int HidlHalDataResponse_1_6::allocatePduSessionIdResponse(
    int slotId, int responseType, int serial, RIL_Errno e, void *response,
    size_t responselen) {
#if VDBG
    RLOGD("[%s]%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        int sessionId = responseInt(responseInfo, serial, responseType, e, response,
                                    responselen);
        Return<void> retStatus = mRadioResponseV1_6->allocatePduSessionIdResponse(
            responseInfo, sessionId);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__,
              slotId);
    }

    return 0;
}

int HidlHalDataResponse_1_6::cancelHandoverResponse(int slotId,
                                                    int responseType,
                                                    int serial, RIL_Errno e,
                                                    void *response,
                                                    size_t responselen) {
#if VDBG
    RLOGD("[%s]%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
            mRadioResponseV1_6->cancelHandoverResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__,
              slotId);
    }

    return 0;
}

int HidlHalDataResponse_1_6::getDataCallListResponse_1_6(
    int slotId, int responseType, int serial, RIL_Errno e, void *response,
    size_t responselen) {
#if VDBG
    RLOGD("[%s]%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_6::SetupDataCallResult> dcList;
        if (mRadioResponseV1_6 == NULL || response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS)
                responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilDataCallListToHal(response, responselen,
                                                        dcList);
        }
        Return<void> retStatus =
            mRadioResponseV1_6->getDataCallListResponse_1_6(responseInfo, dcList);
        checkReturnStatus(retStatus);
    } else {
        // RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL fallback to 1_5",
        // __FUNCTION__, slotId);
        // TODOgetDataCallListResponse(slotId, responseType, serial, e, response,
        // responseLen);
    }
    return ret;
}

int HidlHalDataResponse_1_6::getSlicingConfigResponse_1_6(
    int slotId, int responseType, int serial, RIL_Errno e, void *response,
    size_t responselen) {
#if VDBG
    RLOGD("[%s]%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_6::SlicingConfig slicingConfig = {};

        if (response == NULL) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) {
                responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
            } else if (e == RIL_E_MODEM_ERR) {
                responseInfo.error = V1_6::RadioError::MODEM_ERR;
            }
        } else {
            Radio2HalUtils::convertRilSlicingConfigToHal(*static_cast<RIL_SlicingConfig *>(response), slicingConfig);
        }

        Return<void> retStatus = mRadioResponseV1_6->getSlicingConfigResponse(
            responseInfo, slicingConfig);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("getSlicingConfigResponse: radioService[%d]->mRadioResponse == NULL",
              slotId);
    }

    return 0;
}

int HidlHalDataResponse_1_6::releasePduSessionIdResponse(
    int slotId, int responseType, int serial, RIL_Errno e, void *response,
    size_t responselen) {
#if VDBG
    RLOGD("[%s]%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
            mRadioResponseV1_6->releasePduSessionIdResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("[%s]%s: radioService[%d]->mRadioResponseV1_6 == NULL", TAG, __FUNCTION__,
              slotId);
    }

    return 0;
}
int HidlHalDataResponse_1_6::setupDataCallResponse_1_6(int slotId,
                                                       int responseType,
                                                       int serial, RIL_Errno e,
                                                       void *response,
                                                       size_t responselen) {
#if VDBG
    RLOGD("[%s]%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_6::SetupDataCallResult dcResult = {};
        // User v11 format, For V1_4, only two parameter are removed, and just type
        // change for Cause
        if (response == NULL) {
            RLOGE("[%s]%s: Invalid response", TAG, __FUNCTION__);
            if (e == RIL_E_SUCCESS)
                responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
            dcResult.cause = V1_6::DataCallFailCause::ERROR_UNSPECIFIED;
            dcResult.type = V1_4::PdpProtocolType::UNKNOWN;
            dcResult.ifname = hidl_string();
            dcResult.addresses.resize(0);
            dcResult.dnses.resize(0);
            dcResult.gateways.resize(0);
            dcResult.pcscf.resize(0);
        } else {
            RLOGE("[%s]%s: convertRilDataCallToHal", TAG, __FUNCTION__);
            Radio2HalUtils::convertRilDataCallToHal(
                *static_cast<RIL_SetupDataCallResult_V1_6 *>(response), dcResult);
        }

        Return<void> retStatus =
            mRadioResponseV1_6->setupDataCallResponse_1_6(responseInfo, dcResult);
        checkReturnStatus(retStatus);
    }
    return 0;
}
int HidlHalDataResponse_1_6::startHandoverResponse(int slotId,
                                                   int responseType,
                                                   int serial, RIL_Errno e,
                                                   void *response,
                                                   size_t responselen) {
#if VDBG
    RLOGD("[%s]%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
            mRadioResponseV1_6->startHandoverResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("[%s]%s: radioService[%d]->mRadioResponseV1_6 == NULL", TAG, __FUNCTION__,
              slotId);
    }

    return 0;
}

int HidlHalDataResponse_1_6::setDataThrottlingResponse(
    int slotId, int responseType, int serial, RIL_Errno e, void *response,
    size_t responseLen) {
#if VDBG
    RLOGD("[%s]%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
            mRadioResponseV1_6->setDataThrottlingResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("[%s]%s: radioService[%d]->mRadioResponseV1_6 == NULL", TAG, __FUNCTION__,
              slotId);
    }
    return 0;
}
