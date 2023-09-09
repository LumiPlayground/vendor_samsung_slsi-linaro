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

#include "radioconfigresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_vec;

#define TAG "RadioConfigResponseHook"

void RadioConfigResponseHook::setResponseFunction(std::shared_ptr<RadioConfigResponseCallback> configCb) {
    RLOGD("%s::%s configCb=%p", TAG, __FUNCTION__, configCb.get());
    mConfigCb = configCb;
}

Return<void> RadioConfigResponseHook::getModemsConfigResponse(const V1_0::RadioResponseInfo& info, const config::V1_1::ModemsConfig& modemsConfig) {
#if VDBG
    RLOGD("%s::%s mConfigCb=%p", TAG, __FUNCTION__, mConfigCb.get());
#endif
    if (mConfigCb) {
        mConfigCb->getNumOfLiveModemsResponse(toAidl(info), modemsConfig.numOfLiveModems);
    }
    return Void();
}

Return<void> RadioConfigResponseHook::getPhoneCapabilityResponse(const V1_0::RadioResponseInfo& info, const config::V1_1::PhoneCapability& phoneCapability) {
#if VDBG
    RLOGD("%s::%s mConfigCb=%p", TAG, __FUNCTION__, mConfigCb.get());
#endif
    if (mConfigCb) {
        mConfigCb->getPhoneCapabilityResponse(toAidl(info), toAidl(phoneCapability));
    }
    return Void();
}

Return<void> RadioConfigResponseHook::setModemsConfigResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mConfigCb=%p", TAG, __FUNCTION__, mConfigCb.get());
#endif
    if (mConfigCb) {
        mConfigCb->setNumOfLiveModemsResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioConfigResponseHook::setPreferredDataModemResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mConfigCb=%p", TAG, __FUNCTION__, mConfigCb.get());
#endif
    if (mConfigCb) {
        mConfigCb->setPreferredDataModemResponse(toAidl(info));
    }
    return Void();
}