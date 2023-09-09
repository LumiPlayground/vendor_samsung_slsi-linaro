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
#include <hidl/1.6/hidlhalresponse_voice.h>
#include <hidl/hidlhalresponse_voice.h>
#include <librilutils/radio_hal_utils.h>
#include <librilutils/radio_hal_utils_1_6.h>
#include "ril_internal.h"
#include "ril_service_1_6.h"

#define TAG "HidlHalVoiceResponse_1_6"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalVoiceResponse_1_6::getCurrentCallsResponse_1_6(int slotId, int responseType, int serial,
        RIL_Errno e, void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        populateResponseInfo(responseInfo, serial, responseType, e);

        hidl_vec<V1_6::Call> calls;
        if ((response == NULL && responselen != 0)
                || (responselen % sizeof(RIL_Call_V1_6 *)) != 0) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
        } else {
            int num = responselen / sizeof(RIL_Call_V1_6 *);
            calls.resize(num);

            for (int i = 0 ; i < num ; i++) {
                RIL_Call_V1_6 *p_cur = ((RIL_Call_V1_6 **) response)[i];
                if (!Radio2HalUtils::convertRilCallToHal(p_cur, sizeof(RIL_Call_V1_6), calls[i])) {
                    if (e == RIL_E_SUCCESS) responseInfo.error = V1_6::RadioError::INVALID_RESPONSE;
                    break;
                }
            }
        }

        Return<void> retStatus = mRadioResponseV1_6->getCurrentCallsResponse_1_6(responseInfo, calls);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
    }
    return 0;
}
