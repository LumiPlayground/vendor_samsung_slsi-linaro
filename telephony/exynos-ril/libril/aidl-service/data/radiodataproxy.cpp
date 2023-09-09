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

#include <radiodataproxy.h>
#include <slsi/radio_v1_6.h>
#include "radioresponse.h"
#include <ril_internal.h>
#include "commands.h"

#define TAG "RadioDataProxy"

// using namespace android::hardware::radio::impl;
// using android::hardware::radio::impl::data::unthrottleApnInd;

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::data;

void RadioDataProxy::setupDataCall(
    int32_t serial,
    ::aidl::android::hardware::radio::AccessNetwork accessNetwork,
    const ::aidl::android::hardware::radio::data::DataProfileInfo
        &dataProfileInfo,
    bool roamingAllowed,
    ::aidl::android::hardware::radio::data::DataRequestReason reason,
    const std::vector<::aidl::android::hardware::radio::data::LinkAddress>
        &addresses,
    const std::vector<std::string> &dnses, int32_t pduSessionId,
    const std::optional<::aidl::android::hardware::radio::data::SliceInfo>
        &sliceInfo,
    bool matchAllRuleAllowed) {

#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(2,0));
    onRequest<data::SetupDataCallCommand>(request, serial, accessNetwork, dataProfileInfo, roamingAllowed, reason, addresses,
                                                                        dnses, pduSessionId, sliceInfo, matchAllRuleAllowed);
}

void RadioDataProxy::setDataProfile(int32_t serial, const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo> &profiles) {

#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(2, 0));
    onRequest<data::SetDataProfileCommand>(request, serial, profiles);
}

void RadioDataProxy::setInitialAttachApn(int32_t serial, const std::optional<::aidl::android::hardware::radio::data::DataProfileInfo> &dpInfo) {

#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(2, 0));
    onRequest<data::SetInitialAttachApnCommand>(request, serial, dpInfo);
}

void RadioDataProxy::setResponseFunctions(
    const std::shared_ptr<RadioDataResponseCallback> &responseCb,
    const std::shared_ptr<RadioDataIndicationCallback> &indicationCb) {
    RLOGE("%s::%s responseCb=%p indicationCb=%p", TAG, __FUNCTION__,
          responseCb.get(), indicationCb.get());
    mResponseCb = responseCb;
    mIndicationCb = indicationCb;

    auto response = mRadioResponse->data();
    auto indication = mRadioIndication->data();
    if (response) {
        response->setResponseFunction(responseCb);
    }

    if (indication) {
        indication->setResponseFunction(indicationCb);
    }

    // indication(unsolicted responses)
    listen<UnthrottleApnInd>(RIL_UNSOL_UNTHROTTLE_APN);

    updateRadioServiceConnectionState(mResponseCb && mIndicationCb);
}
