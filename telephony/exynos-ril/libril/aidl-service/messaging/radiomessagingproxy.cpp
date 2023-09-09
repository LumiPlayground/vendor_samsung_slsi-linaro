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

#include <radiomessagingproxy.h>
#include "radioresponse.h"
#include <ril_internal.h>

#include "commands.h"

#define TAG "RadioMessagingProxy"

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::messaging;

void RadioMessagingProxy::setResponseFunctions(
    const std::shared_ptr<RadioMessagingResponseCallback> &responseCb,
    const std::shared_ptr<RadioMessagingIndicationCallback> &indicationCb) {
    RLOGE("%s::%s responseCb=%p indicationCb=%p", TAG, __FUNCTION__,
          responseCb.get(), indicationCb.get());
    mResponseCb = responseCb;
    mIndicationCb = indicationCb;

    auto response = mRadioResponse->messaging();
    auto indication = mRadioIndication->messaging();
    if (response) {
        response->setResponseFunction(responseCb);
    }

    if (indication) {
        indication->setResponseFunction(indicationCb);
    }

    // indication(unsolicted responses)
    updateRadioServiceConnectionState(mResponseCb && mIndicationCb);
}