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
#include <hidl/1.6/hidlhalindication_sim.h>
#include <hidl/hidlhalindication_sim.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_service_common.h"
#include "ril_service_1_6.h"
#include <slsi/radio_v1_6.h>

#define TAG "HidlHalSimIndication_1_6"

using namespace android::hardware::radio::V1_6;
using ::android::hardware::Return;
using ::android::hardware::hidl_string;

android::hardware::radio::V1_0::RadioIndicationType convertIntToRadioIndicationType(int indicationType);

int HidlHalSimIndication_1_6::simPhonebookChanged(int slotId,
        int indicationType, int token, RIL_Errno e, void *response, size_t responselen) {
    if (mRadioIndicationV1_6 != NULL) {
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndicationV1_6->simPhonebookChanged(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_6 == NULL", __FUNCTION__, slotId);
    }
    return 0;
}

int HidlHalSimIndication_1_6::simPhonebookRecordsReceived(int slotId,
        int indicationType, int token, RIL_Errno e, void *response, size_t responseLen) {
    if (mRadioIndicationV1_6 != NULL) {
#if VDBG
        RLOGD("%s::%s", TAG, __FUNCTION__);
#endif
        if (response != NULL && responseLen > 0) {
            hidl_vec<V1_6::PhonebookRecordInfo> records;
            RIL_PhonebookRecInfoResult *result = (RIL_PhonebookRecInfoResult *) response;

            int loadedCount = result->loadedCount;
            RLOGD("%s, result loadedCount:%d, receivedStatus:%d", __FUNCTION__, loadedCount,
                    result->receivedStatus);
            records.resize(loadedCount);
            V1_6::PbReceivedStatus receivedStatus = (V1_6::PbReceivedStatus) result->receivedStatus;

            for (int i = 0; i < loadedCount; i++) {
                records[i].recordId = result->mPbRec[i].recordId;
                records[i].name = Radio2HalUtils::convertCharPtrToHidlString(result->mPbRec[i].name);
                records[i].number = result->mPbRec[i].number;

                RLOGD("%s, record info current index:%d, recordId:%d, name:%s, number:%s, "
                        "emailSetCount:%d, anrSetCount:%d",__FUNCTION__, i, records[i].recordId,
                        records[i].name.c_str(), records[i].number.c_str(),
                        result->mPbRec[i].emailSetCount, result->mPbRec[i].anrSetCount);
                int emailSetCount = result->mPbRec[i].emailSetCount;
                int anrSetCount = result->mPbRec[i].anrSetCount;

                records[i].emails.resize(emailSetCount);
                if (emailSetCount > 0) {
                    for (int j = 0; j < emailSetCount; j++) {
                        records[i].emails[j] = result->mPbRec[i].emails[j];
                        RLOGD(", result email[%d]:%s", j, records[i].emails[j].c_str());
                    }
                }

                // TODO: anr type should be added to PhonebookRecordInfo if it is needed.
                records[i].additionalNumbers.resize(anrSetCount);
                if (anrSetCount > 0) {
                    for (int j = 0; j < anrSetCount; j++) {
                        records[i].additionalNumbers[j] = result->mPbRec[i].additionalNumbers[j];
                        RLOGD(", result anr[%d]:%s", j, records[i].additionalNumbers[j].c_str());
                    }
                }
            }

            Return<void> retStatus =
                    mRadioIndicationV1_6->simPhonebookRecordsReceived(
                            convertIntToRadioIndicationType(indicationType),
                            receivedStatus, records);
            checkReturnStatus(retStatus);
        } else {
            RLOGE("%s: response is NULL!!", __FUNCTION__);
            Return<void> retStatus =
                    mRadioIndicationV1_6->simPhonebookRecordsReceived(
                            convertIntToRadioIndicationType(indicationType),
                            V1_6::PbReceivedStatus::PB_RECEIVED_ERROR, NULL);
            checkReturnStatus(retStatus);
        }
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }
    return 0;
}
