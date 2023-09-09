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
#include <hidl/hidlhalresponse_network.h>
#include <librilutils/radio_conv_utils.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_internal.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalNetworkResponse"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalNetworkResponse::getAvailableBandModesResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_vec<RadioBandMode> modes;
        if ((response == NULL && responselen != 0) || responselen % sizeof(int) != 0) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int *pInt = (int *) response;
            int numInts = responselen / sizeof(int);
            modes.resize(numInts);
            for (int i = 0; i < numInts; i++) {
                modes[i] = (RadioBandMode) pInt[i];
            }
        }
        Return<void> retStatus
                = mRadioResponse->getAvailableBandModesResponse(responseInfo, modes);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::getAvailableNetworksResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_vec<OperatorInfo> networks;
        if ((response == NULL && responselen != 0)
                || responselen % (4 * sizeof(char *))!= 0) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            char **resp = (char **) response;
            int numStrings = responselen / sizeof(char *);
            networks.resize(numStrings/4);
            for (int i = 0, j = 0; i < numStrings; i = i + 4, j++) {
                networks[j].alphaLong = Radio2HalUtils::convertCharPtrToHidlString(resp[i]);
                networks[j].alphaShort = Radio2HalUtils::convertCharPtrToHidlString(resp[i + 1]);
                networks[j].operatorNumeric = Radio2HalUtils::convertCharPtrToHidlString(resp[i + 2]);
                int status = Radio2HalUtils::convertOperatorStatusToInt(resp[i + 3]);
                if (status == -1) {
                    if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
                } else {
                    networks[j].status = (OperatorStatus) status;
                }
            }
        }
        Return<void> retStatus
                = mRadioResponse->getAvailableNetworksResponse(responseInfo,
                networks);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::getBarringInfoResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 == NULL) {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return 0;
    }

    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);

    V1_5::CellIdentity cellIdentity;
    hidl_vec<V1_5::BarringInfo> records;
    if (response == NULL || responselen == 0 || responselen % sizeof(RIL_CellIdAndBarringInfo) != 0) {
        RLOGE("%s: Invalid response", __FUNCTION__);
        // VTS expects only this error code.
        responseInfo.error = RadioError::REQUEST_NOT_SUPPORTED;
        records.resize(0);
    } else {
        RIL_CellIdAndBarringInfo *bic = (RIL_CellIdAndBarringInfo *)response;
        Radio2HalUtils::convertRilCellIdentityToHal(cellIdentity, bic->cellIdentity);
        Radio2HalUtils::convertRilBarringInfosToHal(records, bic->precords, bic->numOfrecord);
    }

    Return<void> retStatus =
            mRadioResponseV1_5->getBarringInfoResponse(responseInfo, cellIdentity, records);
    checkReturnStatus(retStatus);

    return 0;
}

int HidlHalNetworkResponse::getCdmaRoamingPreferenceResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responselen);
        Return<void> retStatus
                = mRadioResponse->getCdmaRoamingPreferenceResponse(
                responseInfo, (CdmaRoamingType) ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::getCellInfoListResponse(int slotId,
                                   int responseType,
                                   int serial, RIL_Errno e, void *response,
                                   size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<CellInfo> ret;
        if ((response == NULL && responseLen != 0)
                || responseLen % sizeof(RIL_CellInfo_v12) != 0) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilCellInfoListToHal(response, responseLen, ret);
        }
        Return<void> retStatus = mRadioResponse->getCellInfoListResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::getCellInfoListResponse_1_2(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_2 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_2::CellInfo> ret;
        if ((response == NULL && responselen != 0)
                || responselen % sizeof(RIL_CellInfo_V1_2) != 0) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilCellInfoListToHal(response, responselen, ret);
        }
        Return<void> retStatus =
                mRadioResponseV1_2->getCellInfoListResponse_1_2(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_2 == NULL", __FUNCTION__, slotId);
        int numOfCellInfo = 0;
        RIL_CellInfo_v12 *cellInfo = NULL;
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_CellInfo_V1_2) == 0) {
            numOfCellInfo = responselen / sizeof(RIL_CellInfo_V1_2);
            cellInfo = new RIL_CellInfo_v12[numOfCellInfo] {};
            for (int i = 0; i < numOfCellInfo; i++) {
                RadioConvUtils::convertCellInfo(cellInfo[i], ((RIL_CellInfo_V1_2 *)response)[i]);
            } // end for i ~
        }
        int ret = this->getCellInfoListResponse(slotId, responseType, serial, e, cellInfo,
                sizeof(RIL_CellInfo_v12) * numOfCellInfo);
        if (cellInfo != NULL) {
            delete[] cellInfo;
        }
        return ret;
    }
    return 0;
}

int HidlHalNetworkResponse::getCellInfoListResponse_1_4(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_4 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_4::CellInfo> ret;
        if ((response == NULL && responselen != 0)
                || responselen % sizeof(RIL_CellInfo_V1_4) != 0) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilCellInfoListToHal(response, responselen, ret);
        }
        Return<void> retStatus =
                mRadioResponseV1_4->getCellInfoListResponse_1_4(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL", __FUNCTION__, slotId);
        int numOfCellInfo = 0;
        RIL_CellInfo_V1_2 *cellInfoV1_2 = NULL;
        if (response != NULL && responselen > 0
                && responselen % sizeof(RIL_CellInfo_V1_4) == 0) {
            numOfCellInfo = responselen / sizeof(RIL_CellInfo_V1_4);
            cellInfoV1_2 = new RIL_CellInfo_V1_2[numOfCellInfo] {};
            for (int i = 0; i < numOfCellInfo; i++) {
                RadioConvUtils::convertCellInfo(cellInfoV1_2[i], ((RIL_CellInfo_V1_4 *)response)[i]);
            } // end for i ~
        }
        int ret = this->getCellInfoListResponse_1_2(slotId, responseType, serial, e, cellInfoV1_2,
                sizeof(RIL_CellInfo_V1_2) * numOfCellInfo);
        if (cellInfoV1_2 != NULL) {
            delete[] cellInfoV1_2;
        }
        return ret;
    }
    return 0;
}

int HidlHalNetworkResponse::getCellInfoListResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_5::CellInfo> ret;
        if ((response == NULL && responselen != 0)
                || responselen % sizeof(RIL_CellInfo_V1_5) != 0) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilCellInfoListToHal(response, responselen, ret);
        }
        Return<void> retStatus = mRadioResponseV1_5->getCellInfoListResponse_1_5(
                responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        int numOfCellInfo = 0;
        RIL_CellInfo_V1_4 *cellInfoV1_4 = NULL;
        if (response != NULL && responselen > 0 && responselen % sizeof(RIL_CellInfo_V1_5) == 0) {
            numOfCellInfo = responselen / sizeof(RIL_CellInfo_V1_5);
            cellInfoV1_4 = new RIL_CellInfo_V1_4[numOfCellInfo] {};
            for (int i = 0; i < numOfCellInfo; i++) {
                RadioConvUtils::convertCellInfo(cellInfoV1_4[i], ((RIL_CellInfo_V1_5 *)response)[i]);
            } // end for i ~
        }
        int ret = this->getCellInfoListResponse_1_4(slotId, responseType, serial, e, cellInfoV1_4,
                sizeof(RIL_CellInfo_V1_4) * numOfCellInfo);
        if (cellInfoV1_4 != NULL) {
            delete[] cellInfoV1_4;
        }
        return ret;
    }
    return 0;
}

int HidlHalNetworkResponse::getDataRegistrationStateResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        DataRegStateResult dataRegResponse = {};
        if (response == NULL) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            RIL_DataRegistrationStateResponse *dataRegState =
                    (RIL_DataRegistrationStateResponse *)response;
            if (responselen != sizeof(RIL_DataRegistrationStateResponse)) {
                RLOGE("%s Invalid response: NULL", __FUNCTION__);
                if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
            } else {
                dataRegResponse.regState = (RegState) dataRegState->regState;
                dataRegResponse.rat = dataRegState->rat;;
                dataRegResponse.reasonDataDenied = dataRegState->reasonDataDenied;
                dataRegResponse.maxDataCalls = dataRegState->maxDataCalls;
                Radio2HalUtils::fillCellIdentityResponse(dataRegResponse.cellIdentity, dataRegState->cellIdentity);
            }
        }
        Return<void> retStatus =
                mRadioResponse->getDataRegistrationStateResponse(responseInfo, dataRegResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::getDataRegistrationStateResponse_1_2(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_2 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_2::DataRegStateResult dataRegResponse = {};
        if (!Radio2HalUtils::convertRilDataRegStateToHal(
                (RIL_DataRegistrationStateResponse_V1_2 *)response, responselen, dataRegResponse)) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        }
        Return<void> retStatus = mRadioResponseV1_2->getDataRegistrationStateResponse_1_2(
                                        responseInfo, dataRegResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_2 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_DataRegistrationStateResponse)) {
            RIL_DataRegistrationStateResponse dataRegResult = {};
            RadioConvUtils::convertDataRegistrationStateResult(dataRegResult,
                    *((RIL_DataRegistrationStateResponse_V1_2 *)response));
            return this->getDataRegistrationStateResponse(slotId, responseType, serial, e,
                    &dataRegResult, sizeof(RIL_DataRegistrationStateResponse));
        }
    }
    return 0;
}

int HidlHalNetworkResponse::getDataRegistrationStateResponse_1_4(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_4::DataRegStateResult dataRegResponse = {};
        if (!Radio2HalUtils::convertRilDataRegStateToHal(
                (RIL_DataRegistrationStateResponse_V1_4 *)response, responselen, dataRegResponse)) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        }
        Return<void> retStatus = mRadioResponseV1_4->getDataRegistrationStateResponse_1_4(
                                                        responseInfo, dataRegResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_DataRegistrationStateResponse_V1_2)) {
            RIL_DataRegistrationStateResponse_V1_2 dataRegResult = {};
            RadioConvUtils::convertDataRegistrationStateResult(dataRegResult,
                    *((RIL_DataRegistrationStateResponse_V1_4 *)response));
            return this->getDataRegistrationStateResponse_1_2(slotId, responseType, serial, e,
                    &dataRegResult, sizeof(RIL_DataRegistrationStateResponse_V1_2));
        }
    }
    return 0;
}

int HidlHalNetworkResponse::getDataRegistrationStateResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_5::RegStateResult dataRegResponse = {};
        if (!Radio2HalUtils::convertRilRegStateToHal((RIL_RegStateResult *)response,
                responselen, dataRegResponse)) {
            RLOGE("getDataRegistrationStateResponse_1_5 Invalid response: NULL");
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        }
        Return<void> retStatus = mRadioResponseV1_5->getDataRegistrationStateResponse_1_5(
                                                        responseInfo, dataRegResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_DataRegistrationStateResponse_V1_4)) {
            RIL_DataRegistrationStateResponse_V1_4 dataRegResult = {};
            RadioConvUtils::convertDataRegistrationStateResult(dataRegResult,
                    *((RIL_RegStateResult *)response));
            return this->getDataRegistrationStateResponse_1_4(slotId, responseType, serial, e,
                    &dataRegResult, sizeof(RIL_DataRegistrationStateResponse_V1_4));
        }
    }
    return 0;
}

int HidlHalNetworkResponse::getImsRegistrationStateResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        bool isRegistered = false;
        int ratFamily = 0;
        int numInts = responselen / sizeof(int);
        if (response == NULL || numInts != 2) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int *pInt = (int *) response;
            isRegistered = pInt[0] == 1 ? true : false;
            ratFamily = pInt[1];
        }
        Return<void> retStatus
                = mRadioResponse->getImsRegistrationStateResponse(
                responseInfo, isRegistered, (RadioTechnologyFamily) ratFamily);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::getNetworkSelectionModeResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        bool manual = false;
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int *pInt = (int *) response;
            manual = pInt[0] == 1 ? true : false;
        }
        Return<void> retStatus
                = mRadioResponse->getNetworkSelectionModeResponse(
                responseInfo,
                manual);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::getOperatorResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_string longName;
        hidl_string shortName;
        hidl_string numeric;
        int numStrings = responselen / sizeof(char *);
        if (response == NULL || numStrings < 3) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;

        } else {
            char **resp = (char **) response;
            longName = Radio2HalUtils::convertCharPtrToHidlString(resp[0]);
            shortName = Radio2HalUtils::convertCharPtrToHidlString(resp[1]);
            numeric = Radio2HalUtils::convertCharPtrToHidlString(resp[2]);
        }
        Return<void> retStatus = mRadioResponse->getOperatorResponse(
                responseInfo, longName, shortName, numeric);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::getSignalStrengthResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        SignalStrength signalStrength = {};
        if (response == NULL || responselen != sizeof(RIL_SignalStrength_v10)) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilSignalStrengthToHal(response, responselen, signalStrength);
        }
        Return<void> retStatus = mRadioResponse->getSignalStrengthResponse(
                responseInfo, signalStrength);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::getSignalStrengthResponse_1_2(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_2 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_2::SignalStrength signalStrength = {};
        if (response == NULL || responselen != sizeof(RIL_SignalStrength_V1_2)) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilSignalStrengthToHal(response, responselen, signalStrength);
        }
        Return<void> retStatus = mRadioResponseV1_2->getSignalStrengthResponse_1_2(
                responseInfo, signalStrength);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_2 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_SignalStrength_V1_2)) {
            RIL_SignalStrength_v10 signalStrength = {};
            RadioConvUtils::convertSignalStrengthResult(signalStrength,
                    *((RIL_SignalStrength_V1_2 *)response));
            return this->getSignalStrengthResponse(slotId, responseType, serial, e,
                    &signalStrength, sizeof(RIL_SignalStrength_v10));
        }
    }
    return 0;
}

int HidlHalNetworkResponse::getSignalStrengthResponse_1_4(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        V1_4::SignalStrength signalStrength = {};
        if (response == NULL || responselen != sizeof(RIL_SignalStrength_V1_4)) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            Radio2HalUtils::convertRilSignalStrengthToHal(response, responselen, signalStrength);
        }
        Return<void> retStatus = mRadioResponseV1_4->getSignalStrengthResponse_1_4(
                responseInfo, signalStrength);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL", __FUNCTION__, slotId);
        if (response != NULL && responselen == sizeof(RIL_SignalStrength_V1_4)) {
            RIL_SignalStrength_V1_2 signalStrength = {};
            RadioConvUtils::convertSignalStrengthResult(
                    signalStrength, *((RIL_SignalStrength_V1_4 *)response));
            return this->getSignalStrengthResponse_1_2(slotId, responseType, serial, e,
                     &signalStrength, sizeof(RIL_SignalStrength_V1_2));
        }
    }
    return 0;
}

int HidlHalNetworkResponse::getVoiceRadioTechnologyResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responselen);
        Return<void> retStatus
                = mRadioResponse->getVoiceRadioTechnologyResponse(
                responseInfo, (RadioTechnology) ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::getVoiceRegistrationStateResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                           void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        VoiceRegStateResult voiceRegResponse = {};
        if (response == NULL) {
               RLOGE("%s Invalid response: NULL", __FUNCTION__);
               if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            RIL_VoiceRegistrationStateResponse *voiceRegState =
                    (RIL_VoiceRegistrationStateResponse *)response;
            if (responselen != sizeof(RIL_VoiceRegistrationStateResponse)) {
                RLOGE("%s Invalid response: NULL", __FUNCTION__);
                if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
            } else {
                voiceRegResponse.regState = (RegState) voiceRegState->regState;
                voiceRegResponse.rat = voiceRegState->rat;;
                voiceRegResponse.cssSupported = voiceRegState->cssSupported;
                voiceRegResponse.roamingIndicator = voiceRegState->roamingIndicator;
                voiceRegResponse.systemIsInPrl = voiceRegState->systemIsInPrl;
                voiceRegResponse.defaultRoamingIndicator = voiceRegState->defaultRoamingIndicator;
                voiceRegResponse.reasonForDenial = voiceRegState->reasonForDenial;
                Radio2HalUtils::fillCellIdentityResponse(voiceRegResponse.cellIdentity,
                        voiceRegState->cellIdentity);
            }
        }
        Return<void> retStatus =
                mRadioResponse->getVoiceRegistrationStateResponse(responseInfo, voiceRegResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::getVoiceRegistrationStateResponse_1_2(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_2 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_2::VoiceRegStateResult voiceRegResponse = {};
        if (response == NULL ||
            responselen != sizeof(RIL_VoiceRegistrationStateResponse_V1_2)) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            RIL_VoiceRegistrationStateResponse_V1_2 *voiceRegState =
                    (RIL_VoiceRegistrationStateResponse_V1_2 *)response;
            voiceRegResponse.regState = (RegState) voiceRegState->regState;
            voiceRegResponse.rat = voiceRegState->rat;;
            voiceRegResponse.cssSupported = voiceRegState->cssSupported;
            voiceRegResponse.roamingIndicator = voiceRegState->roamingIndicator;
            voiceRegResponse.systemIsInPrl = voiceRegState->systemIsInPrl;
            voiceRegResponse.defaultRoamingIndicator = voiceRegState->defaultRoamingIndicator;
            voiceRegResponse.reasonForDenial = voiceRegState->reasonForDenial;
            Radio2HalUtils::fillCellIdentityResponse(voiceRegResponse.cellIdentity,
                    voiceRegState->cellIdentity);
        }
        Return<void> retStatus = mRadioResponseV1_2->getVoiceRegistrationStateResponse_1_2(
                                                        responseInfo, voiceRegResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_2 == NULL",
                __FUNCTION__, slotId);
        if (response != NULL && responselen >= sizeof(RIL_VoiceRegistrationStateResponse)) {
            RIL_VoiceRegistrationStateResponse voiceRegResult = {};
            RadioConvUtils::convertVoiceRegistrationStateResult(voiceRegResult,
                    *((RIL_VoiceRegistrationStateResponse_V1_2 *)response));
            return this->getVoiceRegistrationStateResponse(slotId, responseType, serial, e,
                    &voiceRegResult, sizeof(RIL_VoiceRegistrationStateResponse));
        }
    }
    return 0;
}

int HidlHalNetworkResponse::getVoiceRegistrationStateResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_5::RegStateResult voiceRegResponse = {};
        if (!Radio2HalUtils::convertRilRegStateToHal((RIL_RegStateResult *)response,
                 responselen, voiceRegResponse)) {
            RLOGE("getVoiceRegistrationStateResponse_1_5 Invalid response: NULL");
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        }
        Return<void> retStatus = mRadioResponseV1_5->getVoiceRegistrationStateResponse_1_5(
                    responseInfo, voiceRegResponse);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("getVoiceRegistrationStateResponse_1_5: radioService[%d]->mRadioResponseV1_5 == NULL", slotId);
        if (response != NULL && responselen >= sizeof(RIL_VoiceRegistrationStateResponse_V1_2)) {
            RIL_VoiceRegistrationStateResponse_V1_2 voiceRegResult = {};
            RadioConvUtils::convertVoiceRegistrationStateResult(voiceRegResult, *((RIL_RegStateResult *)response));
            return this->getVoiceRegistrationStateResponse_1_2(slotId, responseType, serial, e,
                    &voiceRegResult, sizeof(RIL_VoiceRegistrationStateResponse_V1_2));
        }
    }
    return 0;
}

int HidlHalNetworkResponse::setBandModeResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->setBandModeResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::setBarringPasswordResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->setBarringPasswordResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::setCdmaRoamingPreferenceResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->setCdmaRoamingPreferenceResponse(
                responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::setCellInfoListRateResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->setCellInfoListRateResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::setIndicationFilterResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setIndicationFilterResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::setIndicationFilterResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
                mRadioResponseV1_5->setIndicationFilterResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return this->setIndicationFilterResponse(slotId, responseType, serial, e, response,
                 responselen);
    }
    return 0;
}

int HidlHalNetworkResponse::setLinkCapacityReportingCriteriaResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    // If we don't have a radio service, there's nothing we can do
    if (mRadioResponseV1_2 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
                mRadioResponseV1_2->setLinkCapacityReportingCriteriaResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_2 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::setLinkCapacityReportingCriteriaResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
                mRadioResponseV1_5->setLinkCapacityReportingCriteriaResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return this->setLinkCapacityReportingCriteriaResponse(slotId, responseType, serial, e,
                response, responselen);
    }
    return 0;
}

int HidlHalNetworkResponse::setLocationUpdatesResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->setLocationUpdatesResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::setNetworkSelectionModeAutomaticResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->setNetworkSelectionModeAutomaticResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::setNetworkSelectionModeManualResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setNetworkSelectionModeManualResponse(
                responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->setNetworkSelectionModeManualResponse == NULL", __FUNCTION__,
                slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::setNetworkSelectionModeManualResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
                mRadioResponseV1_5->setNetworkSelectionModeManualResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return this->setNetworkSelectionModeManualResponse(slotId, responseType, serial, e,
                response, responselen);
    }
    return 0;
}

int HidlHalNetworkResponse::setSignalStrengthReportingCriteriaResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_2 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
                mRadioResponseV1_2->setSignalStrengthReportingCriteriaResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else{
        RLOGE("%s: radioService[%d]->mRadioResponseV1_2 == NULL", __FUNCTION__, slotId);
        return 0;
    }
    return 0;
}

int HidlHalNetworkResponse::setSignalStrengthReportingCriteriaResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
                mRadioResponseV1_5->setSignalStrengthReportingCriteriaResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return this->setSignalStrengthReportingCriteriaResponse(slotId, responseType, serial, e,
                response, responselen);
    }
    return 0;
}

int HidlHalNetworkResponse::setSuppServiceNotificationsResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponse->setSuppServiceNotificationsResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalNetworkResponse::setSystemSelectionChannelsResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_3 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
                mRadioResponseV1_3->setSystemSelectionChannelsResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_3 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::setSystemSelectionChannelsResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
                mRadioResponseV1_5->setSystemSelectionChannelsResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return this->setSystemSelectionChannelsResponse(slotId, responseType, serial, e,
                response, responselen);
    }
    return 0;
}

int HidlHalNetworkResponse::startNetworkScanResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_1 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponseV1_1->startNetworkScanResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::startNetworkScanResponse_1_4(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_4 != nullptr) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponseV1_4->startNetworkScanResponse_1_4(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL", __FUNCTION__, slotId);
        return this->startNetworkScanResponse(slotId, responseType, serial, e, response,
                responselen);
    }
    return 0;
}

int HidlHalNetworkResponse::startNetworkScanResponse_1_5(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_5 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponseV1_5->startNetworkScanResponse_1_5(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return this->startNetworkScanResponse_1_4(slotId, responseType, serial, e, response,
                responselen);
    }
    return 0;
}

int HidlHalNetworkResponse::stopNetworkScanResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_1 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus
                = mRadioResponseV1_1->stopNetworkScanResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_1 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalNetworkResponse::supplyNetworkDepersonalizationResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        int ret = -1;
        if (response == NULL && responselen == 0) {
            // Earlier RILs did not send a response for some cases although the interface
            // expected an integer as response. Do not return error if response is empty. Instead
            // Return -1 in those cases to maintain backward compatibility.
        } else if (response == NULL || responselen != sizeof(int)) {
            RLOGE("responseIntOrEmpty: Invalid response");
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int *p_int = (int *) response;
            ret = p_int[0];
        }
        Return<void> retStatus
            = mRadioResponse->supplyNetworkDepersonalizationResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}