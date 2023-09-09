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

#include <radioconfigproxy.h>
#include "radioresponse.h"
#include <ril_internal.h>
#include <slsi/radioconfig_v1_0.h>
#include <slsi/radioconfig_v1_1.h>
#include <slsi/radioconfig_v1_2.h>
#include <slsi/radioconfig_v1_3.h>
#include <slsi/radioconfig_v2_0.h>

#include "commands.h"

#define TAG "RadioConfigProxy"

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::config;

void RadioConfigProxy::setResponseFunctions(
        const std::shared_ptr<RadioConfigResponseCallback>& responseCb,
        const std::shared_ptr<RadioConfigIndicationCallback>& indicationCb) {
    RLOGD("%s::%s responseCb=%p indicationCb=%p", TAG, __FUNCTION__,
                        responseCb.get(), indicationCb.get());
    mResponseCb = responseCb;
    mIndicationCb = indicationCb;

    auto response = mRadioResponse->config();
    auto indication = mRadioIndication->config();
    if (response) {
        response->setResponseFunction(responseCb);
    }

    if (indication) {
        indication->setResponseFunction(indicationCb);
    }

    listen<SimSlotsStatusChangedIndication>(RIL_UNSOL_ICC_SLOT_STATUS);
}

void RadioConfigProxy::getHalDeviceCapabilities(int32_t serial) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_GET_HAL_DEVICE_CAPABILITIES, HAL_VERSION_CODE(2, 0));
    onRequest<config::GetHalDeviceCapabilitiesCommand>(request, serial);
}

void RadioConfigProxy::getSimSlotsStatus(int32_t serial) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_GET_SLOT_STATUS, HAL_VERSION_CODE(2, 0));
    onRequest<config::GetSimSlotsStatusCommand>(request, serial);
}

void RadioConfigProxy::setSimSlotsMapping(int32_t serial,
        const std::vector<aidl::android::hardware::radio::config::SlotPortMapping>& slotMap) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING, HAL_VERSION_CODE(2, 0));
    onRequest<config::SetLogicalToPhysicalSlotPortMapping>(request, serial, slotMap);
}
