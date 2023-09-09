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
#include <hidl/1.6/hidlhalresponse_sim.h>
#include <hidl/hidlhalresponse_sim.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_internal.h"
#include "ril_service_1_6.h"

#include <slsi/radio_v1_6.h>

#define TAG "HidlHalSimResponse_1_6"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalSimResponse_1_6::getSimPhonebookCapacityResponse(int slotId,
                        int responseType, int serial, RIL_Errno e, void *response,
                            size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        V1_6::PhonebookCapacity pbCapacity = {};
        if (response != NULL && responseLen > 0) {
            RIL_PhonebookCapacity* capa = (RIL_PhonebookCapacity*) response;
            pbCapacity.maxAdnRecords = capa->maxAdnRecords;
            pbCapacity.usedAdnRecords = capa->usedAdnRecords;
            pbCapacity.maxEmailRecords = capa->maxEmailRecords;
            pbCapacity.usedEmailRecords = capa->usedEmailRecords;
            pbCapacity.maxAdditionalNumberRecords = capa->maxAdditionalNumberRecords;
            pbCapacity.usedAdditionalNumberRecords = capa->usedAdditionalNumberRecords;
            pbCapacity.maxNameLen = capa->maxNameLen;
            pbCapacity.maxNumberLen = capa->maxNumberLen;
            pbCapacity.maxEmailLen = capa->maxEmailLen;
            pbCapacity.maxAdditionalNumberLen = capa->maxAdditionalNumberLen;
        }
        Return<void> retStatus =
                mRadioResponseV1_6->getSimPhonebookCapacityResponse(responseInfo, pbCapacity);
        checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL, ret = %d", __FUNCTION__,
                slotId, ret);
    }
    return 0;
}

int HidlHalSimResponse_1_6::getSimPhonebookRecordsResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                                size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus =
                mRadioResponseV1_6->getSimPhonebookRecordsResponse(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL, ret = %d", __FUNCTION__,
                slotId, ret);
    }
    return 0;
}

int HidlHalSimResponse_1_6::setSimCardPowerResponse_1_6(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                                size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        Return<void> retStatus =
                mRadioResponseV1_6->setSimCardPowerResponse_1_6(responseInfo);
        checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL, ret = %d", __FUNCTION__,
                slotId, ret);
        return HidlHalSimResponse(*this).
                setSimCardPowerResponse(slotId, responseType, serial, e, response, responseLen);
    }
    return 0;
}

int HidlHalSimResponse_1_6::updateSimPhonebookRecordsResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                                size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        int recordId = 0;
        if (response != NULL && responseLen > 0) {
            RIL_UpdatePbRsp* rsp = (RIL_UpdatePbRsp*) response;
            recordId = rsp->recordId;

            RLOGD("%s: updated recordId:%d", __FUNCTION__, rsp->recordId);
        }

        Return<void> retStatus =
                mRadioResponseV1_6->updateSimPhonebookRecordsResponse(responseInfo, recordId);
        checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL, ret = %d", __FUNCTION__, slotId,
                ret);
    }
    return 0;
}

