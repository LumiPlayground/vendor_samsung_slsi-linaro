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
#include <hidl/1.6/hidlhalindication_data.h>
#include <hidl/hidlhalindication_data.h>
#include <librilutils/radio_hal_utils.h>
#include <librilutils/radio_hal_utils_1_6.h>
#include <librilutils/radio_conv_utils.h>
#include "ril_service_common.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalDataIndication"

using ::android::hardware::hidl_string;
using ::android::hardware::Return;

int HidlHalDataIndication_1_6::unthrottleApn(int slotId, int indicationType,
                                             int token, RIL_Errno e,
                                             void *response,
                                             size_t responselen) {
    if (mRadioIndicationV1_6 != NULL) {
        if (response == NULL) {
            RLOGE("[%s]%s: invalid response", TAG, __FUNCTION__);
            return 0;
        }
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndicationV1_6->unthrottleApn(
            convertIntToRadioIndicationType(indicationType), Radio2HalUtils::convertCharPtrToHidlString((char *)response));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("[%s]%s: radioService[%d]->mRadioIndicationV1_6 == NULL", TAG, __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalDataIndication_1_6::dataCallListChangedInd_1_6(
    int slotId, int indicationType, int token, RIL_Errno e, void *response,
    size_t responselen) {
#if VDBG
    RLOGD("[%s]%s", TAG, __FUNCTION__);
#endif
    int ret = 0;
    if (mRadioIndicationV1_6 != NULL) {
        // null response is valid, it means empty datacall, no active datacall,
        // still use v11
        if ((response == NULL && responselen != 0) ||
            responselen % sizeof(RIL_SetupDataCallResult_V1_6) != 0) {
            RLOGE("[%s]%s: invalid response : %p, len:%zu", TAG, __FUNCTION__, response,
                  responselen);
            return 0;
        }

        // RIL_SetupDataCallResult_V1_6 == RIL_Data_Call_Response_1_6
        hidl_vec<V1_6::SetupDataCallResult> dcList;
        Radio2HalUtils::convertRilDataCallListToHal(response, responselen, dcList);
#if VDBG
        RLOGD("[%s]%s", TAG, __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndicationV1_6->dataCallListChanged_1_6(
            convertIntToRadioIndicationType(indicationType), dcList);
        checkReturnStatus(retStatus);
    } else {
        //Convert RIL_SetupDataCallResult_V1_6 to RIL_SetupDataCallResult_v11
        int num = (response == NULL) ? 0 : responselen / sizeof(RIL_SetupDataCallResult_V1_6);
        if (response == NULL && responselen != 0) {
            RLOGE("%s: invalid response : %p", __FUNCTION__, response);
            return 0;
        }

        RIL_Data_Call_Response_v11 *convertedList = new RIL_Data_Call_Response_v11[num];
        RIL_SetupDataCallResult_V1_6 *dcResponse = (RIL_SetupDataCallResult_V1_6 *)response;

        RadioConvUtils::convertDataCallInfo(convertedList, dcResponse, num);

        if (response == NULL && responselen == 0)
            radio::dataCallListChangedInd_1_5(slotId, indicationType, token, e, response, responselen);
        else
            radio::dataCallListChangedInd_1_5(slotId, indicationType, token, e, convertedList, sizeof(RIL_Data_Call_Response_v11) * num);

        RLOGE("%s: radioService[%d]->mRadioIndicationV1_6 == NULL, fallback to "
              "1_5, ret was %d",
              __FUNCTION__, slotId, ret);

        for (int i = 0; i < num; i++)
            RadioConvUtils::freeIPAddrInfo(convertedList[i]);

        delete[] convertedList;
    }

    return 0;
}
