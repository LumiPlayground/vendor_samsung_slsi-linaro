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
#include <aidl-service/aidl-log.h>

#include <radiosimproxy.h>
#include "radioproxy.h"
#include "radioresponse.h"
#include <ril_internal.h>

#include "commands.h"

#define TAG "RadioSimProxy"

using namespace android::hardware::radio::impl;
namespace sim = android::hardware::radio::impl::sim;

void RadioSimProxy::setResponseFunctions(
        const std::shared_ptr<RadioSimResponseCallback>& responseCb,
        const std::shared_ptr<RadioSimIndicationCallback>& indicationCb) {
    RLOGE("%s::%s responseCb=%p indicationCb=%p", TAG, __FUNCTION__,
          responseCb.get(), indicationCb.get());

    mResponseCb = responseCb;
    mIndicationCb = indicationCb;

    auto response = mRadioResponse->sim();
    auto indication = mRadioIndication->sim();
    if (response) {
        response->setResponseFunction(responseCb);
    }
    if (indication) {
        indication->setResponseFunction(indicationCb);
    }

    // AIDL only interfaces

    updateRadioServiceConnectionState(mResponseCb && mIndicationCb);
}

void RadioSimProxy::getIccCardStatus(int32_t serial) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_GET_SIM_STATUS, HAL_VERSION_CODE(2, 0));
    onRequest<sim::GetIccCardStatusCommand>(request, serial);
}
