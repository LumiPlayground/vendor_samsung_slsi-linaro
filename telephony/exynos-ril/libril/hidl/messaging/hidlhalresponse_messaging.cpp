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
#include <hidl/hidlhalresponse_messaging.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_internal.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalMessagingResponse"

using ::android::hardware::hidl_string;
using ::android::hardware::Return;

int HidlHalMessagingResponse::acknowledgeIncomingGsmSmsWithPduResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->acknowledgeIncomingGsmSmsWithPduResponse(
            responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::acknowledgeLastIncomingCdmaSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->acknowledgeLastIncomingCdmaSmsResponse(
            responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::acknowledgeLastIncomingGsmSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus =
            mRadioResponse->acknowledgeLastIncomingGsmSmsResponse(
                responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::deleteSmsOnRuimResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->deleteSmsOnRuimResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::deleteSmsOnSimResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->deleteSmsOnSimResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::getCdmaBroadcastConfigResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_vec<CdmaBroadcastSmsConfigInfo> configs;

        if ((response == NULL && responseLen != 0) || responseLen % sizeof(RIL_CDMA_BroadcastSmsConfigInfo *) != 0) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS)
                responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int num = responseLen / sizeof(RIL_CDMA_BroadcastSmsConfigInfo *);
            configs.resize(num);
            for (int i = 0; i < num; i++) {
                RIL_CDMA_BroadcastSmsConfigInfo *resp =
                    ((RIL_CDMA_BroadcastSmsConfigInfo **)response)[i];
                configs[i].serviceCategory = resp->service_category;
                configs[i].language = resp->language;
                configs[i].selected = resp->selected == 1 ? true : false;
            }
        }

        Return<void> retStatus = mRadioResponse->getCdmaBroadcastConfigResponse(responseInfo,
                                                                                configs);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
              __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::getGsmBroadcastConfigResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_vec<GsmBroadcastSmsConfigInfo> configs;

        if ((response == NULL && responseLen != 0) || responseLen % sizeof(RIL_GSM_BroadcastSmsConfigInfo *) != 0) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS)
                responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int num = responseLen / sizeof(RIL_GSM_BroadcastSmsConfigInfo *);
            configs.resize(num);
            for (int i = 0; i < num; i++) {
                RIL_GSM_BroadcastSmsConfigInfo *resp =
                    ((RIL_GSM_BroadcastSmsConfigInfo **)response)[i];
                configs[i].fromServiceId = resp->fromServiceId;
                configs[i].toServiceId = resp->toServiceId;
                configs[i].fromCodeScheme = resp->fromCodeScheme;
                configs[i].toCodeScheme = resp->toCodeScheme;
                configs[i].selected = resp->selected == 1 ? true : false;
            }
        }

        Return<void> retStatus = mRadioResponse->getGsmBroadcastConfigResponse(responseInfo,
                                                                               configs);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
              __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::getSmscAddressResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->getSmscAddressResponse(responseInfo,
                                                                        Radio2HalUtils::convertCharPtrToHidlString((char *)response));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::reportSmsMemoryStatusResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->reportSmsMemoryStatusResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
              __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::sendCdmaSmsExpectMoreResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    // If we don't have a radio service, there's nothing we can do
    if (mRadioResponseV1_5 == NULL) {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__, slotId);
        return 0;
    }

    RadioResponseInfo responseInfo = {};
    SendSmsResult result = makeSendSmsResult(responseInfo, serial, responseType, e, response,
                                             responseLen);
    Return<void> retStatus =
        mRadioResponseV1_5->sendCdmaSmsExpectMoreResponse(responseInfo, result);
    checkReturnStatus(retStatus);
    return 0;
}

int HidlHalMessagingResponse::sendCdmaSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        SendSmsResult result = makeSendSmsResult(responseInfo, serial, responseType, e, response,
                                                 responseLen);

        Return<void> retStatus = mRadioResponse->sendCdmaSmsResponse(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::sendImsSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        SendSmsResult result = makeSendSmsResult(responseInfo, serial, responseType, e, response,
                                                 responseLen);

        Return<void> retStatus = mRadioResponse->sendImsSmsResponse(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::sendSmsExpectMoreResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        SendSmsResult result = makeSendSmsResult(responseInfo, serial, responseType, e, response,
                                                 responseLen);

        Return<void> retStatus = mRadioResponse->sendSMSExpectMoreResponse(
            responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::sendSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        SendSmsResult result = makeSendSmsResult(responseInfo, serial, responseType, e, response,
                                                 responseLen);

        Return<void> retStatus = mRadioResponse->sendSmsResponse(responseInfo,
                                                                 result);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::setCdmaBroadcastActivationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setCdmaBroadcastActivationResponse(
            responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
              __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::setCdmaBroadcastConfigResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setCdmaBroadcastConfigResponse(
            responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
              __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::setGsmBroadcastActivationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setGsmBroadcastActivationResponse(
            responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
              __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::setGsmBroadcastConfigResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setGsmBroadcastConfigResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
              __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::setSmscAddressResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setSmscAddressResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::writeSmsToRuimResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus = mRadioResponse->writeSmsToRuimResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingResponse::writeSmsToSimResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responseLen);
        Return<void> retStatus = mRadioResponse->writeSmsToSimResponse(responseInfo, ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}
