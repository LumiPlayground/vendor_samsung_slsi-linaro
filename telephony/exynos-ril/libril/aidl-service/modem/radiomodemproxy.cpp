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

#include <radiomodemproxy.h>
#include "radioresponse.h"
#include <ril_internal.h>

#include "commands.h"

#define TAG "RadioModemProxy"

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::modem;

void RadioModemProxy::onRadioServiceConnected() {
    RadioInterface::onRadioServiceConnected();
    android::onNewCommandConnect((RIL_SOCKET_ID) mSlot);
}

void RadioModemProxy::setResponseFunctions(
        const std::shared_ptr<RadioModemResponseCallback>& responseCb,
        const std::shared_ptr<RadioModemIndicationCallback>& indicationCb) {
    RLOGE("%s::%s responseCb=%p indicationCb=%p", TAG, __FUNCTION__,
                        responseCb.get(), indicationCb.get());
    mResponseCb = responseCb;
    mIndicationCb = indicationCb;

    auto response = mRadioResponse->modem();
    auto indication = mRadioIndication->modem();
    if (response) {
        response->setResponseFunction(responseCb);
    }

    if (indication) {
        indication->setResponseFunction(indicationCb);
    }

    // indication(unsolicted responses)
    listen<RadioStateChangedInd>(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED);

    updateRadioServiceConnectionState(mResponseCb && mIndicationCb);
}

void RadioModemProxy::getModemActivityInfo(int32_t serial) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_GET_ACTIVITY_INFO, HAL_VERSION_CODE(2, 0));
    onRequest<modem::ModemActivityInfoCommand>(request, serial);
}

void RadioModemProxy::setRadioPower(int32_t serial, bool powerOn, bool forEmergencyCall,
                            bool preferredForEmergencyCall) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_RADIO_POWER, HAL_VERSION_CODE(1,6));
    onRequest<modem::SetRadioPowerCommand>(request, serial, powerOn, forEmergencyCall, preferredForEmergencyCall);
}