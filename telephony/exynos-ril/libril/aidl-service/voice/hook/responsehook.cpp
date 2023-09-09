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

#include "radiovoiceresponse.h"
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

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_voice = ::aidl::android::hardware::radio::voice;

#define TAG "RadioVoiceResponseHook"

void RadioVoiceResponseHook::setResponseFunction(std::shared_ptr<RadioVoiceResponseCallback> voiceCb) {
    RLOGD("%s::%s voiceCb=%p", TAG, __FUNCTION__, voiceCb.get());
    mVoiceCb = voiceCb;
}

Return<void> RadioVoiceResponseHook::acceptCallResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->acceptCallResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::cancelPendingUssdResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->cancelPendingUssdResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::conferenceResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->conferenceResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::dialResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->dialResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::emergencyDialResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->emergencyDialResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::exitEmergencyCallbackModeResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->exitEmergencyCallbackModeResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::explicitCallTransferResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->explicitCallTransferResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::getCallForwardStatusResponse(const V1_0::RadioResponseInfo& info,
        const ::android::hardware::hidl_vec<V1_0::CallForwardInfo>& callForwardInfos) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->getCallForwardStatusResponse(toAidl(info), toAidl(callForwardInfos));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::getCallWaitingResponse(const V1_0::RadioResponseInfo& info,
        bool enable, int32_t serviceClass) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->getCallWaitingResponse(toAidl(info), enable, serviceClass);
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::getClipResponse(const V1_0::RadioResponseInfo& info,
        V1_0::ClipStatus status) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->getClipResponse(toAidl(info), aidl_voice::ClipStatus(status));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::getClirResponse(const V1_0::RadioResponseInfo& info,
        int32_t n, int32_t m) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->getClirResponse(toAidl(info), n, m);
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::getCurrentCallsResponse_1_6(const V1_6::RadioResponseInfo& info,
        const ::android::hardware::hidl_vec<V1_6::Call>& calls) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->getCurrentCallsResponse(toAidl(info), toAidl(calls));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::getLastCallFailCauseResponse(const V1_0::RadioResponseInfo& info,
        const V1_0::LastCallFailCauseInfo& failCauseinfo) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->getLastCallFailCauseResponse(toAidl(info), toAidl(failCauseinfo));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::getMuteResponse(const V1_0::RadioResponseInfo& info, bool enable) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->getMuteResponse(toAidl(info), enable);
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::getPreferredVoicePrivacyResponse(const V1_0::RadioResponseInfo& info,
        bool enable) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->getPreferredVoicePrivacyResponse(toAidl(info), enable);
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::getTTYModeResponse(const V1_0::RadioResponseInfo& info, V1_0::TtyMode mode) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->getTtyModeResponse(toAidl(info), aidl_voice::TtyMode(mode));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::handleStkCallSetupRequestFromSimResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->handleStkCallSetupRequestFromSimResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::hangupConnectionResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->hangupConnectionResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::hangupForegroundResumeBackgroundResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->hangupForegroundResumeBackgroundResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::hangupWaitingOrBackgroundResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->hangupWaitingOrBackgroundResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::rejectCallResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->rejectCallResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::sendBurstDtmfResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->sendBurstDtmfResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::sendCDMAFeatureCodeResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->sendCdmaFeatureCodeResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::sendDtmfResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->sendDtmfResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::sendUssdResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->sendUssdResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::separateConnectionResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->separateConnectionResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::setCallForwardResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->setCallForwardResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::setCallWaitingResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->setCallWaitingResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::setClirResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->setClirResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::setMuteResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->setMuteResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::setPreferredVoicePrivacyResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->setPreferredVoicePrivacyResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::setTTYModeResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->setTtyModeResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::startDtmfResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->startDtmfResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::stopDtmfResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->stopDtmfResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioVoiceResponseHook::switchWaitingOrHoldingAndActiveResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mVoiceCb=%p", TAG, __FUNCTION__, mVoiceCb.get());
#endif
    if (mVoiceCb) {
        mVoiceCb->switchWaitingOrHoldingAndActiveResponse(toAidl(info));
    }
    return Void();
}
