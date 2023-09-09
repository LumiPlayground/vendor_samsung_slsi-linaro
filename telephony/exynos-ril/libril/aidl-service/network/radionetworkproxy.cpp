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

#include <radionetworkproxy.h>
#include <slsi/radio_v1_6.h>
#include <slsi/radio_v2_0.h>
#include <slsi/ril_oem.h>
#include "radioresponse.h"
#include <ril_internal.h>

#include "commands.h"

#define TAG "RadioNetworkProxy"

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::network;

void RadioNetworkProxy::init() {
    // NITZ time handler
    auto nitz = listen<NitzTimeReceivedInd>(RIL_UNSOL_NITZ_TIME_RECEIVED);
    registerForRadioServiceConnectionListener(nitz.get());
}

void RadioNetworkProxy::setResponseFunctions(
        const std::shared_ptr<RadioNetworkResponseCallback>& responseCb,
        const std::shared_ptr<RadioNetworkIndicationCallback>& indicationCb) {
    RLOGD("%s::%s responseCb=%p indicationCb=%p", TAG, __FUNCTION__,
                        responseCb.get(), indicationCb.get());
    mResponseCb = responseCb;
    mIndicationCb = indicationCb;

    auto response = mRadioResponse->network();
    auto indication = mRadioIndication->network();
    if (response) {
        response->setResponseFunction(responseCb);
    }

    if (indication) {
        indication->setResponseFunction(indicationCb);
    }

    updateRadioServiceConnectionState(mResponseCb && mIndicationCb);
}

void RadioNetworkProxy::setSignalStrengthReportingCriteria(int32_t serial,
            const std::vector<::aidl::android::hardware::radio::network::SignalThresholdInfo>& signalThresholdInfos) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    onRequest<network::SetSignalStrengthReportingCriteriaCommand>(
            RIL_REQUEST_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, serial, signalThresholdInfos);
}

void RadioNetworkProxy::setUsageSetting(int32_t serial,
            ::aidl::android::hardware::radio::network::UsageSetting usageSetting) {
#if VDBG
    RLOGD("%s::%s: serial %d, usageSetting=%d", TAG, __FUNCTION__, serial, usageSetting);
#endif
    onRequest<network::SetUsageSettingCommand>(RIL_REQUEST_SET_USAGE_SETTING, serial, usageSetting);
}

void RadioNetworkProxy::getUsageSetting(int32_t serial) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    onRequest<network::GetUsageSettingCommand>(RIL_REQUEST_GET_USAGE_SETTING, serial);
}
