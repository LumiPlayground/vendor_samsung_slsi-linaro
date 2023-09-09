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
#include <aidl-log.h>
#include <ril_internal.h>
#include "commands.h"
#include "commonvendorstructs.h"
#include "vendorstructs.h"

namespace android::hardware::radio::impl::sim {

namespace aidl = ::aidl::android::hardware::radio;

/**
 * GetIccCardStatusCommand
 */
void GetIccCardStatusCommand::onRequest(int serial) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    mVendorFunctions.dispatchVoid(allocateRequestInfo(serial));
}

int GetIccCardStatusCommand::onResponse(int slotId,int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        aidl::sim::CardStatus resp = {};

        RLOGD("RIL_CardStatus_V1_4 : %d", (int)sizeof(RIL_CardStatus_V1_4));
        RLOGD("V2_0::RIL_CardStatus : %d", (int)sizeof(sim::V2_0::RIL_CardStatus));
        RLOGD("responselen : %d", (int)responselen);

        if (response == nullptr || responselen != sizeof(sim::V2_0::RIL_CardStatus)) {
            RLOGE("%s::%s Invalid response", TAG, __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = aidl::RadioError(RIL_E_INVALID_RESPONSE);
        } else {
            const sim::V2_0::RIL_CardStatus& info = *(sim::V2_0::RIL_CardStatus *)response;
            resp = toAidl(info);
        }
        mCb->getIccCardStatusResponse(responseInfo, resp);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }
    return 0;
}

}
