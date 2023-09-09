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
#include <hidl/1.6/hidlhalresponse_messaging.h>
#include <hidl/hidlhalresponse_messaging.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_internal.h"
#include "ril_service_1_6.h"

#define TAG "HidlHalMessagingResponse_1_6"

using ::android::hardware::hidl_string;
using ::android::hardware::Return;

int HidlHalMessagingResponse_1_6::sendSmsResponse_1_6(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        SendSmsResult result = makeSendSmsResult(responseInfo, serial, responseType, e, response,
                                                 responseLen);

        Return<void> retStatus = mRadioResponseV1_6->sendSmsResponse_1_6(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL, ret = %d", __FUNCTION__, slotId, ret);
        return HidlHalMessagingResponse(*this).sendSmsResponse(slotId, responseType, serial, e, response, responseLen);
    }

    return 0;
}

int HidlHalMessagingResponse_1_6::sendSmsExpectMoreResponse_1_6(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        SendSmsResult result = makeSendSmsResult(responseInfo, serial, responseType, e, response,
                                                 responseLen);

        Return<void> retStatus = mRadioResponseV1_6->sendSmsExpectMoreResponse_1_6(
            responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL, ret = %d", __FUNCTION__, slotId, ret);
        return HidlHalMessagingResponse(*this).sendSmsExpectMoreResponse(slotId, responseType, serial, e, response, responseLen);
    }

    return 0;
}

int HidlHalMessagingResponse_1_6::sendCdmaSmsResponse_1_6(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        SendSmsResult result = makeSendSmsResult(responseInfo, serial, responseType, e, response,
                                                 responseLen);

        Return<void> retStatus = mRadioResponseV1_6->sendCdmaSmsResponse_1_6(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL, ret = %d", __FUNCTION__, slotId, ret);
        return HidlHalMessagingResponse(*this).sendCdmaSmsResponse(slotId, responseType, serial, e, response, responseLen);
    }

    return 0;
}

int HidlHalMessagingResponse_1_6::sendCdmaSmsExpectMoreResponse_1_6(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int ret = 0;
    if (mRadioResponseV1_6 != NULL) {
        V1_6::RadioResponseInfo responseInfo = {};
        SendSmsResult result = makeSendSmsResult(responseInfo, serial, responseType, e, response,
                                                 responseLen);
        Return<void> retStatus =
            mRadioResponseV1_6->sendCdmaSmsExpectMoreResponse_1_6(responseInfo, result);
        checkReturnStatus(retStatus);
    } else {
        // response is void, so no need to convert
        RLOGE("%s: radioService[%d]->mRadioResponseV1_6 == NULL, ret = %d", __FUNCTION__, slotId, ret);
        return HidlHalMessagingResponse(*this).sendCdmaSmsExpectMoreResponse(slotId, responseType, serial, e, response, responseLen);
    }

    return 0;
}
