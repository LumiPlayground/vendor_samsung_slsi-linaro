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
#include <hidl/1.6/hidlhalresponse_network.h>
#include <hidl/hidlhalresponse_network.h>
#include <librilutils/radio_conv_utils.h>
#include <librilutils/radio_hal_utils.h>
#include <librilutils/radio_hal_utils_1_6.h>
#include "ril_internal.h"
#include "ril_service_1_6.h"

#define TAG "HidlHalNetworkResponse_1_6"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalNetworkResponse_1_6::getAllowedNetworkTypesBitmapResponse(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responselen);
        Return<void> retStatus
            = mRadioResponseV1_6->getAllowedNetworkTypesBitmapResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse_1_6::getSystemSelectionChannelsResponse(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_vec<V1_5::RadioAccessSpecifier> ras;

        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_RadioAccessSpecifier_V1_5) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
        } else {
            int num = responselen / sizeof(RIL_RadioAccessSpecifier_V1_5);
            ras.resize(num);
            for (int i = 0 ; i < num; i++) {
                RIL_RadioAccessSpecifier_V1_5 *resp = &((RIL_RadioAccessSpecifier_V1_5 *)response)[i];
                if(!Radio2HalUtils::convertRilRadioAccessSpecifierToHal(resp, sizeof(RIL_RadioAccessSpecifier_V1_5), ras[i])) {
                    if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
                    break;
                }
            }
        }

        Return<void> retStatus
            = mRadioResponseV1_6->getSystemSelectionChannelsResponse(responseInfo, ras);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse_1_6::isNrDualConnectivityEnabledResponse(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        bool enable = responseInt(responseInfo, serial, responseType, e, response, responselen) > 0 ? true : false;
        Return<void> retStatus
            = mRadioResponseV1_6->isNrDualConnectivityEnabledResponse(responseInfo, enable);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse_1_6::setAllowedNetworkTypesBitmapResponse(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
            = mRadioResponseV1_6->setAllowedNetworkTypesBitmapResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse_1_6::setNrDualConnectivityStateResponse(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
            = mRadioResponseV1_6->setNrDualConnectivityStateResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse_1_6::getCellInfoListResponse_1_6(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_6::CellInfo> ret;
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_CellInfo_V1_6)!= 0) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilCellInfoListToHal(response, responselen, ret);
        }
        Return<void> retStatus = mRadioResponseV1_6->getCellInfoListResponse_1_6(
                responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
        int numOfCellInfo = 0;
        RIL_CellInfo_V1_5 *cellInfoV1_5 = NULL;
        if (response != NULL && responselen > 0 && responselen % sizeof(RIL_CellInfo_V1_6) == 0) {
            numOfCellInfo = responselen / sizeof(RIL_CellInfo_V1_6);
            cellInfoV1_5 = new RIL_CellInfo_V1_5[numOfCellInfo] {};
            for (int i = 0; i < numOfCellInfo; i++) {
                RadioConvUtils::convertCellInfo(cellInfoV1_5[i], ((RIL_CellInfo_V1_6 *)response)[i]);
            } // end for i ~
        }
        int ret = HidlHalNetworkResponse(*this).getCellInfoListResponse_1_5(slotId, responseType,
                serial, e, cellInfoV1_5, sizeof(RIL_CellInfo_V1_5) * numOfCellInfo);
        if (cellInfoV1_5 != NULL) {
            delete[] cellInfoV1_5;
        }
        return ret;
    }
    return 0;
}

int HidlHalNetworkResponse_1_6::getDataRegistrationStateResponse_1_6(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_6::RegStateResult dataRegResponse = {};
        if (!Radio2HalUtils::convertRilRegStateToHal((RIL_RegStateResult_V1_6 *)response,
                responselen, dataRegResponse)) {
            RLOGE("getDataRegistrationStateResponse_1_6 Invalid response: NULL");
            if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
        }
        Return<void> retStatus = mRadioResponseV1_6->getDataRegistrationStateResponse_1_6(
                responseInfo, dataRegResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_RegStateResult)) {
            RIL_RegStateResult dataRegResult = {};
            RadioConvUtils::convertDataRegistrationStateResult(dataRegResult,
                    *((RIL_RegStateResult_V1_6 *)response));
            return HidlHalNetworkResponse(*this).getDataRegistrationStateResponse_1_5(slotId,
                        responseType, serial, e, &dataRegResult, sizeof(RIL_RegStateResult));
        }
    }
    return 0;
}

int HidlHalNetworkResponse_1_6::getSignalStrengthResponse_1_6(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_6::SignalStrength signalStrength = {};
        if (response == NULL || responselen != sizeof(RIL_SignalStrength_V1_6)) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilSignalStrengthToHal(response, responselen, signalStrength);
        }
        Return<void> retStatus = mRadioResponseV1_6->getSignalStrengthResponse_1_6(
                responseInfo, signalStrength);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen == sizeof(RIL_SignalStrength_V1_6)) {
            RIL_SignalStrength_V1_4 signalStrength = {};
            RadioConvUtils::convertSignalStrengthResult(signalStrength, *((RIL_SignalStrength_V1_6 *)response));
            return HidlHalNetworkResponse(*this).getSignalStrengthResponse_1_4(slotId,
                    responseType, serial, e, &signalStrength, sizeof(RIL_SignalStrength_V1_4));
        }
    }
    return 0;
}

int HidlHalNetworkResponse_1_6::getVoiceRegistrationStateResponse_1_6(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_6::RegStateResult voiceRegResponse = {};
        if (!Radio2HalUtils::convertRilRegStateToHal((RIL_RegStateResult_V1_6 *)response,
                responselen, voiceRegResponse)) {
            RLOGE("getVoiceRegistrationStateResponse_1_6 Invalid response: NULL");
            if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
        }
        Return<void> retStatus = mRadioResponseV1_6->getVoiceRegistrationStateResponse_1_6(
                responseInfo, voiceRegResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("getVoiceRegistrationStateResponse_1_6: radioService[%d]->mRadioResponseV1_6 == NULL", slotId);
        if (response != NULL && responselen >= sizeof(RIL_RegStateResult)) {
            RIL_RegStateResult voiceRegResult = {};
            RadioConvUtils::convertVoiceRegistrationStateResult(voiceRegResult, *((RIL_RegStateResult_V1_6 *)response));
            return HidlHalNetworkResponse(*this).getVoiceRegistrationStateResponse_1_5(
                        slotId, responseType, serial, e, &voiceRegResult, sizeof(RIL_RegStateResult));
        }
    }
    return 0;
}