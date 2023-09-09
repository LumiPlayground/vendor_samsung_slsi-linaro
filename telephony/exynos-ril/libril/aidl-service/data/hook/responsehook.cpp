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

#include "radiodataresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;

#define TAG "RadioDataResponseHook"

void RadioDataResponseHook::setResponseFunction(
    std::shared_ptr<RadioDataResponseCallback> dataCb) {
    RLOGD("%s::%s dataCb=%p", TAG, __FUNCTION__, dataCb.get());
    mDataCb = dataCb;
}

// Ver >= 1.6
Return<void> RadioDataResponseHook::allocatePduSessionIdResponse(
    const ::android::hardware::radio::V1_6::RadioResponseInfo &info,
    int32_t id) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->allocatePduSessionIdResponse(toAidl(info), id);
    }

    return Void();
}

Return<void> RadioDataResponseHook::releasePduSessionIdResponse(
    const ::android::hardware::radio::V1_6::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->releasePduSessionIdResponse(toAidl(info));
    }

    return Void();
}

Return<void> RadioDataResponseHook::startHandoverResponse(
    const ::android::hardware::radio::V1_6::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->startHandoverResponse(toAidl(info));
    }

    return Void();
}

Return<void> RadioDataResponseHook::cancelHandoverResponse(
    const ::android::hardware::radio::V1_6::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->cancelHandoverResponse(toAidl(info));
    }

    return Void();
}

Return<void> RadioDataResponseHook::getDataCallListResponse_1_6(
    const ::android::hardware::radio::V1_6::RadioResponseInfo &info,
    const ::android::hardware::hidl_vec<
        ::android::hardware::radio::V1_6::SetupDataCallResult> &dcResponse) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->getDataCallListResponse(toAidl(info), toAidl(dcResponse));
    }
    return Void();
}

Return<void> RadioDataResponseHook::getSlicingConfigResponse(
    const ::android::hardware::radio::V1_6::RadioResponseInfo &info,
    const ::android::hardware::radio::V1_6::SlicingConfig &slicingConfig) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->getSlicingConfigResponse(toAidl(info), toAidl(slicingConfig));
    }

    return Void();
}

Return<void> RadioDataResponseHook::setDataThrottlingResponse(
    const ::android::hardware::radio::V1_6::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->setDataThrottlingResponse(toAidl(info));
    }

    return Void();
}

Return<void> RadioDataResponseHook::setupDataCallResponse_1_5(
    const ::android::hardware::radio::V1_0::RadioResponseInfo &info,
    const android::hardware::radio::V1_5::SetupDataCallResult &dcResponse) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->setupDataCallResponse(toAidl(info), toAidl(dcResponse));
    }

    return Void();
}

Return<void> RadioDataResponseHook::setupDataCallResponse_1_6(
    const ::android::hardware::radio::V1_6::RadioResponseInfo &info,
    const android::hardware::radio::V1_6::SetupDataCallResult &dcResponse) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->setupDataCallResponse(toAidl(info), toAidl(dcResponse));
    }

    return Void();
}

// Ver < 1.6

Return<void> RadioDataResponseHook::deactivateDataCallResponse(
    const ::android::hardware::radio::V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->deactivateDataCallResponse(toAidl(info));
    }

    return Void();
}
Return<void> RadioDataResponseHook::setDataAllowedResponse(
    const ::android::hardware::radio::V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->setDataAllowedResponse(toAidl(info));
    }

    return Void();
}

Return<void> RadioDataResponseHook::getDataCallListResponse_1_5(
    const ::android::hardware::radio::V1_0::RadioResponseInfo &info,
    const ::android::hardware::hidl_vec<
        ::android::hardware::radio::V1_5::SetupDataCallResult> &dcResponse) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->getDataCallListResponse(toAidl(info), toAidl(dcResponse));
    }

    return Void();
}

Return<void> RadioDataResponseHook::setDataProfileResponse_1_5(
    const ::android::hardware::radio::V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->setDataProfileResponse(toAidl(info));
    }

    return Void();
}

Return<void> RadioDataResponseHook::setInitialAttachApnResponse_1_5(
    const ::android::hardware::radio::V1_0::RadioResponseInfo &info) {
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
    if (mDataCb) {
        mDataCb->setInitialAttachApnResponse(toAidl(info));
    }

    return Void();
}

Return<void> RadioDataResponseHook::startKeepaliveResponse(
    const ::android::hardware::radio::V1_0::RadioResponseInfo &info,
    const V1_1::KeepaliveStatus &activityInfo) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->startKeepaliveResponse(toAidl(info), toAidl(activityInfo));
    }

    return Void();
}

Return<void> RadioDataResponseHook::stopKeepaliveResponse(
    const ::android::hardware::radio::V1_0::RadioResponseInfo &info) {
#if VDBG
    RLOGD("%s::%s mDataCb=%p", TAG, __FUNCTION__, mDataCb.get());
#endif
    if (mDataCb) {
        mDataCb->stopKeepaliveResponse(toAidl(info));
    }

    return Void();
}
