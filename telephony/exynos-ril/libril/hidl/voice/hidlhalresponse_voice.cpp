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
#include <hidl/hidlhalresponse_voice.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_internal.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalVoiceResponse"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalVoiceResponse::acceptCallResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->acceptCallResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::cancelPendingUssdResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->cancelPendingUssdResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::conferenceResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->conferenceResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::dialResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->dialResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::emergencyDialResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponseV1_4 != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponseV1_4->emergencyDialResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponseV1_4 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::exitEmergencyCallbackModeResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->exitEmergencyCallbackModeResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::explicitCallTransferResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->explicitCallTransferResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL",
                __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::getCallForwardStatusResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        hidl_vec<CallForwardInfo> callForwardInfos;

        if ((response == NULL && responselen != 0)
                || responselen % sizeof(RIL_CallForwardInfo *) != 0) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int num = responselen / sizeof(RIL_CallForwardInfo *);
            callForwardInfos.resize(num);
            for (int i = 0 ; i < num; i++) {
                RIL_CallForwardInfo *resp = ((RIL_CallForwardInfo **) response)[i];
                callForwardInfos[i].status = (CallForwardInfoStatus) resp->status;
                callForwardInfos[i].reason = resp->reason;
                callForwardInfos[i].serviceClass = resp->serviceClass;
                callForwardInfos[i].toa = resp->toa;
                callForwardInfos[i].number = Radio2HalUtils::convertCharPtrToHidlString(resp->number);
                callForwardInfos[i].timeSeconds = resp->timeSeconds;
            }
        }

        Return<void> retStatus = mRadioResponse->getCallForwardStatusResponse(responseInfo, callForwardInfos);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::getCallWaitingResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        bool enable = false;
        int serviceClass = -1;
        int numInts = responselen / sizeof(int);
        if (response == NULL || numInts != 2) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int *pInt = (int *) response;
            enable = pInt[0] == 1 ? true : false;
            serviceClass = pInt[1];
        }
        Return<void> retStatus = mRadioResponse->getCallWaitingResponse(responseInfo, enable, serviceClass);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::getClipResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responselen);
        Return<void> retStatus = mRadioResponse->getClipResponse(responseInfo, (ClipStatus) ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::getClirResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        int n = -1, m = -1;
        int numInts = responselen / sizeof(int);
        if (response == NULL || numInts != 2) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int *pInt = (int *) response;
            n = pInt[0];
            m = pInt[1];
        }
        Return<void> retStatus = mRadioResponse->getClirResponse(responseInfo, n, m);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::getLastCallFailCauseResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        LastCallFailCauseInfo info = {};
        info.vendorCause = hidl_string();
        if (response == NULL) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else if (responselen == sizeof(int)) {
            int *pInt = (int *) response;
            info.causeCode = (LastCallFailCause) pInt[0];
        } else if (responselen == sizeof(RIL_LastCallFailCauseInfo))  {
            RIL_LastCallFailCauseInfo *pFailCauseInfo = (RIL_LastCallFailCauseInfo *) response;
            info.causeCode = (LastCallFailCause) pFailCauseInfo->cause_code;
            info.vendorCause = Radio2HalUtils::convertCharPtrToHidlString(pFailCauseInfo->vendor_cause);
        } else {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        }

        Return<void> retStatus = mRadioResponse->getLastCallFailCauseResponse(responseInfo, info);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::getMuteResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        bool enable = false;
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int *pInt = (int *) response;
            enable = pInt[0] == 1 ? true : false;
        }
        Return<void> retStatus = mRadioResponse->getMuteResponse(responseInfo, enable);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::getPreferredVoicePrivacyResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        bool enable = false;
        int numInts = responselen / sizeof(int);
        if (response == NULL || numInts != 1) {
            RLOGE("%s Invalid response: NULL", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = RadioError::INVALID_RESPONSE;
        } else {
            int *pInt = (int *) response;
            enable = pInt[0] == 1 ? true : false;
        }
        Return<void> retStatus = mRadioResponse->getPreferredVoicePrivacyResponse(responseInfo, enable);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::getTTYModeResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        int ret = responseInt(responseInfo, serial, responseType, e, response, responselen);
        Return<void> retStatus = mRadioResponse->getTTYModeResponse(responseInfo, (TtyMode) ret);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::handleStkCallSetupRequestFromSimResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->handleStkCallSetupRequestFromSimResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::hangupConnectionResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->hangupConnectionResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::hangupForegroundResumeBackgroundResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->hangupForegroundResumeBackgroundResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::hangupWaitingOrBackgroundResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->hangupWaitingOrBackgroundResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::rejectCallResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->rejectCallResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::sendBurstDtmfResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->sendBurstDtmfResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::sendCDMAFeatureCodeResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->sendCDMAFeatureCodeResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::sendDtmfResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->sendDtmfResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::sendUssdResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->sendUssdResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::separateConnectionResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->separateConnectionResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::setCallForwardResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setCallForwardResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::setCallWaitingResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setCallWaitingResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::setClirResponse(int slotId, int responseType, int serial, RIL_Errno e,
    void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setClirResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::setMuteResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setMuteResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::setPreferredVoicePrivacyResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setPreferredVoicePrivacyResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::setTTYModeResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->setTTYModeResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::startDtmfResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->startDtmfResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::stopDtmfResponse(int slotId, int responseType, int serial, RIL_Errno e,
        void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->stopDtmfResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceResponse::switchWaitingOrHoldingAndActiveResponse(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

    if (mRadioResponse != NULL) {
        RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);
        Return<void> retStatus = mRadioResponse->switchWaitingOrHoldingAndActiveResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }

    return 0;
}
