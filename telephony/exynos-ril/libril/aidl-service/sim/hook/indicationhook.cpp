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

#include "radiosimresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"

#include <ril_internal.h>

namespace aidl_sim = ::aidl::android::hardware::radio::sim;
using namespace ::android::hardware::radio;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::Return;
using ::android::hardware::Void;

#define TAG "RadioSimIndicationHook"

void RadioSimIndicationHook::setResponseFunction(
        std::shared_ptr<RadioSimIndicationCallback> simCb) {
    RLOGD("%s::%s simCb=%p", TAG, __FUNCTION__, simCb.get());
    mSimCb = simCb;
}

Return<void> RadioSimIndicationHook::carrierInfoForImsiEncryption(
        V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->carrierInfoForImsiEncryption(toAidl(type));
    }
    return Void();
}

Return<void> RadioSimIndicationHook::cdmaSubscriptionSourceChanged(
        V1_0::RadioIndicationType type, V1_0::CdmaSubscriptionSource cdmaSource) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->cdmaSubscriptionSourceChanged(toAidl(type),
                (aidl_sim::CdmaSubscriptionSource)cdmaSource);
    }
    return Void();
}

Return<void> RadioSimIndicationHook::simRefresh(V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_0::SimRefreshResult& refreshResult) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->simRefresh(toAidl(type), toAidl(refreshResult));
    }
    return Void();
}

Return<void> RadioSimIndicationHook::simStatusChanged(V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->simStatusChanged(toAidl(type));
    }
    return Void();
}

Return<void> RadioSimIndicationHook::stkEventNotify(V1_0::RadioIndicationType type,
        const ::android::hardware::hidl_string& cmd) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->stkEventNotify(toAidl(type), cmd);
    }
    return Void();
}

Return<void> RadioSimIndicationHook::stkProactiveCommand(
        V1_0::RadioIndicationType type, const ::android::hardware::hidl_string& cmd) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->stkProactiveCommand(toAidl(type), cmd);
    }
    return Void();
}

Return<void> RadioSimIndicationHook::stkSessionEnd(V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->stkSessionEnd(toAidl(type));
    }
    return Void();
}

Return<void> RadioSimIndicationHook::subscriptionStatusChanged(
        V1_0::RadioIndicationType type, bool activate) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->subscriptionStatusChanged(toAidl(type), activate);
    }
    return Void();
}

Return<void> RadioSimIndicationHook::uiccApplicationsEnablementChanged(
        V1_0::RadioIndicationType type, bool enabled) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->uiccApplicationsEnablementChanged(toAidl(type), enabled);
    }
    return Void();
}

// V1.6
Return<void> RadioSimIndicationHook::simPhonebookChanged(V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->simPhonebookChanged(toAidl(type));
    }
    return Void();
}

Return<void> RadioSimIndicationHook::simPhonebookRecordsReceived(
        V1_0::RadioIndicationType type, ::android::hardware::radio::V1_6::PbReceivedStatus status,
        const ::android::hardware::hidl_vec<V1_6::PhonebookRecordInfo>& records) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->simPhonebookRecordsReceived(toAidl(type), (aidl_sim::PbReceivedStatus)status,
                toAidl(records));
    }
    return Void();
}

