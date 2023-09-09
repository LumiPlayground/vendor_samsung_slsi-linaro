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

#include "radiomodemresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

namespace aidl_modem = ::aidl::android::hardware::radio::modem;
using namespace ::android::hardware::radio;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::Return;
using ::android::hardware::Void;

#define TAG "RadioModemIndicationHook"

void RadioModemIndicationHook::setResponseFunction(std::shared_ptr<RadioModemIndicationCallback> modemCb) {
    RLOGD("%s::%s modemCb=%p", TAG, __FUNCTION__, modemCb.get());
    mModemCb = modemCb;
}

Return<void> RadioModemIndicationHook::hardwareConfigChanged(
        V1_0::RadioIndicationType type,
        const ::android::hardware::hidl_vec<V1_0::HardwareConfig>& configs) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->hardwareConfigChanged(toAidl(type), toAidl(configs));
    }
    return Void();
}

Return<void> RadioModemIndicationHook::modemReset(V1_0::RadioIndicationType type,
                                              const ::android::hardware::hidl_string& reason) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->modemReset(toAidl(type), reason);
    }
    return Void();
}

Return<void> RadioModemIndicationHook::radioCapabilityIndication(
        V1_0::RadioIndicationType type,
        const android::hardware::radio::V1_0::RadioCapability& rc) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->radioCapabilityIndication(toAidl(type), toAidl(rc));
    }
    return Void();
}

Return<void> RadioModemIndicationHook::radioStateChanged(V1_0::RadioIndicationType type,
                                                     V1_0::RadioState radioState) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->radioStateChanged(toAidl(type), aidl_modem::RadioState(radioState));
    }
    return Void();
}

Return<void> RadioModemIndicationHook::rilConnected(V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->rilConnected(toAidl(type));
    }
    return Void();
}