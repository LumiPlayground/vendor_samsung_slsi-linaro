/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <aidl-service/aidl-log.h>

#include "radiomodemresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_string;

#define TAG "RadioModemResponseHook"

void RadioModemResponseHook::setResponseFunction(std::shared_ptr<RadioModemResponseCallback> modemCb) {
    RLOGD("%s::%s modemCb=%p", TAG, __FUNCTION__, modemCb.get());
    mModemCb = modemCb;
}

Return<void> RadioModemResponseHook::enableModemResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->enableModemResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioModemResponseHook::getBasebandVersionResponse(const V1_0::RadioResponseInfo& info,
                                    const hidl_string& version) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->getBasebandVersionResponse(toAidl(info), version);
    }
    return Void();
}

Return<void> RadioModemResponseHook::getDeviceIdentityResponse(const V1_0::RadioResponseInfo& info,
                                const ::android::hardware::hidl_string& imei,
                                const ::android::hardware::hidl_string& imeisv,
                                const ::android::hardware::hidl_string& esn,
                                const ::android::hardware::hidl_string& meid) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->getDeviceIdentityResponse(toAidl(info), imei, imeisv, esn, meid);
    }
    return Void();
}

Return<void> RadioModemResponseHook::getHardwareConfigResponse(
                                const V1_0::RadioResponseInfo& info,
                                const ::android::hardware::hidl_vec<V1_0::HardwareConfig>& config) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->getHardwareConfigResponse(toAidl(info), toAidl(config));
    }
    return Void();
}

Return<void> RadioModemResponseHook::getModemActivityInfoResponse(const V1_0::RadioResponseInfo& info,
                                    const V1_0::ActivityStatsInfo& activityInfo) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->getModemActivityInfoResponse(toAidl(info), toAidl(activityInfo));
    }
    return Void();
}

Return<void> RadioModemResponseHook::getModemStackStatusResponse(const V1_0::RadioResponseInfo& info,
                                const bool enabled) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->getModemStackStatusResponse(toAidl(info), enabled);
    }
    return Void();
}

Return<void> RadioModemResponseHook::getRadioCapabilityResponse(const V1_0::RadioResponseInfo& info,
                                const android::hardware::radio::V1_0::RadioCapability& rc) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->getRadioCapabilityResponse(toAidl(info), toAidl(rc));
    }
    return Void();
}

Return<void> RadioModemResponseHook::nvReadItemResponse(const V1_0::RadioResponseInfo& info,
                                const ::android::hardware::hidl_string& result) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->nvReadItemResponse(toAidl(info), result);
    }
    return Void();
}

Return<void> RadioModemResponseHook::nvResetConfigResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->nvResetConfigResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioModemResponseHook::nvWriteCdmaPrlResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->nvWriteCdmaPrlResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioModemResponseHook::nvWriteItemResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->nvWriteItemResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioModemResponseHook::requestShutdownResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->requestShutdownResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioModemResponseHook::sendDeviceStateResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->sendDeviceStateResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioModemResponseHook::setRadioCapabilityResponse(const V1_0::RadioResponseInfo& info,
                                const android::hardware::radio::V1_0::RadioCapability& rc) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->setRadioCapabilityResponse(toAidl(info), toAidl(rc));
    }
    return Void();
}

Return<void> RadioModemResponseHook::setRadioPowerResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->setRadioPowerResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioModemResponseHook::setRadioPowerResponse_1_5(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->setRadioPowerResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioModemResponseHook::setRadioPowerResponse_1_6(const V1_6::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mModemCb=%p", TAG, __FUNCTION__, mModemCb.get());
#endif
    if (mModemCb) {
        mModemCb->setRadioPowerResponse(toAidl(info));
    }
    return Void();
}

