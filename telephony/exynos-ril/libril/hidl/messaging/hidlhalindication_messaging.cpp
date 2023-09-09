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
#include <hidl/hidlhalindication_messaging.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_service_common.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalMessagingIndication"

using ::android::hardware::hidl_string;
using ::android::hardware::Return;

#define INVALID_HEX_CHAR 16

uint8_t hexCharToInt(uint8_t c) {
    if (c >= '0' && c <= '9')
        return (c - '0');
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);

    return INVALID_HEX_CHAR;
}

uint8_t *convertHexStringToBytes(void *response, size_t responseLen) {
    if (responseLen % 2 != 0) {
        return NULL;
    }

    uint8_t *bytes = (uint8_t *)calloc(responseLen / 2, sizeof(uint8_t));
    if (bytes == NULL) {
        RLOGE("%s: cannot allocate memory for bytes string", __FUNCTION__);
        return NULL;
    }
    uint8_t *hexString = (uint8_t *)response;

    for (size_t i = 0; i < responseLen; i += 2) {
        uint8_t hexChar1 = hexCharToInt(hexString[i]);
        uint8_t hexChar2 = hexCharToInt(hexString[i + 1]);

        if (hexChar1 == INVALID_HEX_CHAR || hexChar2 == INVALID_HEX_CHAR) {
            RLOGE("%s: invalid hex char %d %d",
                  __FUNCTION__, hexString[i], hexString[i + 1]);
            free(bytes);
            return NULL;
        }
        bytes[i / 2] = ((hexChar1 << 4) | hexChar2);
    }

    return bytes;
}

int HidlHalMessagingIndication::cdmaNewSmsInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responseLen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responseLen != sizeof(RIL_CDMA_SMS_Message)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        CdmaSmsMessage msg = {};
        RIL_CDMA_SMS_Message *rilMsg = (RIL_CDMA_SMS_Message *)response;
        msg.teleserviceId = rilMsg->uTeleserviceID;
        msg.isServicePresent = rilMsg->bIsServicePresent;
        msg.serviceCategory = rilMsg->uServicecategory;
        msg.address.digitMode =
            (V1_0::CdmaSmsDigitMode)rilMsg->sAddress.digit_mode;
        msg.address.numberMode =
            (V1_0::CdmaSmsNumberMode)rilMsg->sAddress.number_mode;
        msg.address.numberType =
            (V1_0::CdmaSmsNumberType)rilMsg->sAddress.number_type;
        msg.address.numberPlan =
            (V1_0::CdmaSmsNumberPlan)rilMsg->sAddress.number_plan;

        int digitLimit = MIN((rilMsg->sAddress.number_of_digits), RIL_CDMA_SMS_ADDRESS_MAX);
        msg.address.digits.setToExternal(rilMsg->sAddress.digits, digitLimit);

        msg.subAddress.subaddressType = (V1_0::CdmaSmsSubaddressType)
                                            rilMsg->sSubAddress.subaddressType;
        msg.subAddress.odd = rilMsg->sSubAddress.odd;

        digitLimit = MIN((rilMsg->sSubAddress.number_of_digits), RIL_CDMA_SMS_SUBADDRESS_MAX);
        msg.subAddress.digits.setToExternal(rilMsg->sSubAddress.digits, digitLimit);

        digitLimit = MIN((rilMsg->uBearerDataLen), RIL_CDMA_SMS_BEARER_DATA_MAX);
        msg.bearerData.setToExternal(rilMsg->aBearerData, digitLimit);

#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->cdmaNewSms(
            convertIntToRadioIndicationType(indicationType), msg);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingIndication::cdmaRuimSmsStorageFullInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responseLen) {
    if (mRadioIndication != NULL) {
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->cdmaRuimSmsStorageFull(
            convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL",
              __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingIndication::newBroadcastSmsInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responseLen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responseLen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        hidl_vec<uint8_t> data;
        data.setToExternal((uint8_t *)response, responseLen);
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->newBroadcastSms(
            convertIntToRadioIndicationType(indicationType), data);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingIndication::newSmsInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responseLen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responseLen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        uint8_t *bytes = convertHexStringToBytes(response, responseLen);
        if (bytes == NULL) {
            RLOGE("%s: convertHexStringToBytes failed", __FUNCTION__);
            return 0;
        }

        hidl_vec<uint8_t> pdu;
        pdu.setToExternal(bytes, responseLen / 2);
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->newSms(
            convertIntToRadioIndicationType(indicationType), pdu);
        checkReturnStatus(retStatus);
        free(bytes);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingIndication::newSmsOnSimInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responseLen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responseLen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        int32_t recordNumber = ((int32_t *)response)[0];
#if VDBG
        RLOGD("%s::%s: slotIndex %d", TAG, __FUNCTION__, recordNumber);
#endif
        Return<void> retStatus = mRadioIndication->newSmsOnSim(
            convertIntToRadioIndicationType(indicationType), recordNumber);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingIndication::newSmsStatusReportInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responseLen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responseLen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        uint8_t *bytes = convertHexStringToBytes(response, responseLen);
        if (bytes == NULL) {
            RLOGE("%s: convertHexStringToBytes failed", __FUNCTION__);
            return 0;
        }

        hidl_vec<uint8_t> pdu;
        pdu.setToExternal(bytes, responseLen / 2);
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->newSmsStatusReport(
            convertIntToRadioIndicationType(indicationType), pdu);
        checkReturnStatus(retStatus);
        free(bytes);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalMessagingIndication::simSmsStorageFullInd(int slotId, int indicationType, int token, RIL_Errno e, void *response, size_t responseLen) {
    if (mRadioIndication != NULL) {
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->simSmsStorageFull(
            convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}
