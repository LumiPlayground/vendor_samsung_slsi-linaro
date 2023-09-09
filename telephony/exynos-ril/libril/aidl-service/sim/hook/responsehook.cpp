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

#include "radiosimresponse.h"
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

namespace aidl_sim = ::aidl::android::hardware::radio::sim;

#define TAG "RadioSimResponseHook"

void RadioSimResponseHook::setResponseFunction(
        std::shared_ptr<RadioSimResponseCallback> simCb) {
    RLOGD("%s::%s simCb=%p", TAG, __FUNCTION__, simCb.get());
    mSimCb = simCb;
}

Return<void> RadioSimResponseHook::areUiccApplicationsEnabledResponse(
        const V1_0::RadioResponseInfo& info, bool enabled) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->areUiccApplicationsEnabledResponse(toAidl(info), enabled);
    }
    return Void();
}

Return<void> RadioSimResponseHook::changeIccPin2ForAppResponse(
        const V1_0::RadioResponseInfo& info, int remainingRetries) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->changeIccPin2ForAppResponse(toAidl(info), remainingRetries);
    }
    return Void();
}

Return<void> RadioSimResponseHook::changeIccPinForAppResponse(
        const V1_0::RadioResponseInfo& info, int remainingRetries) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->changeIccPinForAppResponse(toAidl(info), remainingRetries);
    }
    return Void();
}

Return<void> RadioSimResponseHook::enableUiccApplicationsResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->enableUiccApplicationsResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioSimResponseHook::getAllowedCarriersResponse_1_4(
            const V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_4::CarrierRestrictionsWithPriority& carriers,
            ::android::hardware::radio::V1_4::SimLockMultiSimPolicy multiSimPolicy) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->getAllowedCarriersResponse(toAidl(info), toAidl(carriers),
                (aidl_sim::SimLockMultiSimPolicy)multiSimPolicy);
    }
    return Void();
}

Return<void> RadioSimResponseHook::getCDMASubscriptionResponse(
        const V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& mdn,
        const ::android::hardware::hidl_string& hSid, const ::android::hardware::hidl_string& hNid,
        const ::android::hardware::hidl_string& min, const ::android::hardware::hidl_string& prl) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->getCdmaSubscriptionResponse(toAidl(info), mdn, hSid, hNid, min, prl);
    }
    return Void();
}

Return<void> RadioSimResponseHook::getCdmaSubscriptionSourceResponse(
        const V1_0::RadioResponseInfo& info,
        ::android::hardware::radio::V1_0::CdmaSubscriptionSource source) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->getCdmaSubscriptionSourceResponse(toAidl(info),
                (aidl_sim::CdmaSubscriptionSource) source);
    }
    return Void();
}

Return<void> RadioSimResponseHook::getFacilityLockForAppResponse(
        const V1_0::RadioResponseInfo& info, int32_t response) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->getFacilityLockForAppResponse(toAidl(info), response);
    }
    return Void();
}

Return<void> RadioSimResponseHook::getIMSIForAppResponse(
        const V1_0::RadioResponseInfo& info, const ::android::hardware::hidl_string& imsi) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->getIMSIForAppResponse(toAidl(info), imsi);
    }
    return Void();
}

Return<void> RadioSimResponseHook::getSimPhonebookCapacityResponse(
        const V1_6::RadioResponseInfo& info,
        const V1_6::PhonebookCapacity& capacity) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->getSimPhonebookCapacityResponse(toAidl(info), toAidl(capacity));
    }
    return Void();
}

Return<void> RadioSimResponseHook::getSimPhonebookRecordsResponse(
        const V1_6::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->getSimPhonebookRecordsResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioSimResponseHook::iccCloseLogicalChannelResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->iccCloseLogicalChannelResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioSimResponseHook::iccIOForAppResponse(
        const V1_0::RadioResponseInfo& info,
        const V1_0::IccIoResult& iccIo) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->iccIoForAppResponse(toAidl(info), toAidl(iccIo));
    }
    return Void();
}
Return<void> RadioSimResponseHook::iccOpenLogicalChannelResponse(
        const V1_0::RadioResponseInfo& info, int channelId,
        const ::android::hardware::hidl_vec<int8_t>& selectResponse) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->iccOpenLogicalChannelResponse(toAidl(info), channelId, toAidl(selectResponse));
    }
    return Void();
}

Return<void> RadioSimResponseHook::iccTransmitApduBasicChannelResponse(
        const V1_0::RadioResponseInfo& info,
        const V1_0::IccIoResult& result) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->iccTransmitApduBasicChannelResponse(toAidl(info), toAidl(result));
    }
    return Void();
}

Return<void> RadioSimResponseHook::iccTransmitApduLogicalChannelResponse(
        const V1_0::RadioResponseInfo& info,
        const V1_0::IccIoResult& result) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->iccTransmitApduLogicalChannelResponse(toAidl(info), toAidl(result));
    }
    return Void();
}

Return<void> RadioSimResponseHook::reportStkServiceIsRunningResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->reportStkServiceIsRunningResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioSimResponseHook::requestIccSimAuthenticationResponse(
        const V1_0::RadioResponseInfo& info,
        const V1_0::IccIoResult& result) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->requestIccSimAuthenticationResponse(toAidl(info), toAidl(result));
    }
    return Void();
}

Return<void> RadioSimResponseHook::sendEnvelopeResponse(
        const V1_0::RadioResponseInfo& info,
        const ::android::hardware::hidl_string& commandResponse) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->sendEnvelopeResponse(toAidl(info), commandResponse);
    }
    return Void();
}

Return<void> RadioSimResponseHook::sendEnvelopeWithStatusResponse(
        const V1_0::RadioResponseInfo& info,
        const V1_0::IccIoResult& iccIo) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->sendEnvelopeWithStatusResponse(toAidl(info), toAidl(iccIo));
    }
    return Void();
}

Return<void> RadioSimResponseHook::sendTerminalResponseToSimResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->sendTerminalResponseToSimResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioSimResponseHook::setAllowedCarriersResponse_1_4(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->setAllowedCarriersResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioSimResponseHook::setCarrierInfoForImsiEncryptionResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->setCarrierInfoForImsiEncryptionResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioSimResponseHook::setCdmaSubscriptionSourceResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->setCdmaSubscriptionSourceResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioSimResponseHook::setFacilityLockForAppResponse(
        const V1_0::RadioResponseInfo& info, int32_t retry) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->setFacilityLockForAppResponse(toAidl(info), retry);
    }
    return Void();
}

Return<void> RadioSimResponseHook::setUiccSubscriptionResponse(
        const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->setUiccSubscriptionResponse(toAidl(info));
    }
    return Void();
}
Return<void> RadioSimResponseHook::supplyIccPin2ForAppResponse(
        const V1_0::RadioResponseInfo& info, int32_t remainingRetries) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->supplyIccPin2ForAppResponse(toAidl(info), remainingRetries);
    }
    return Void();
}
Return<void> RadioSimResponseHook::supplyIccPinForAppResponse(
    const V1_0::RadioResponseInfo& info, int32_t remainingRetries) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->supplyIccPinForAppResponse(toAidl(info), remainingRetries);
    }
    return Void();
}
Return<void> RadioSimResponseHook::supplyIccPuk2ForAppResponse(
    const V1_0::RadioResponseInfo& info, int32_t remainingRetries) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->supplyIccPuk2ForAppResponse(toAidl(info), remainingRetries);
    }
    return Void();
}
Return<void> RadioSimResponseHook::supplyIccPukForAppResponse(
    const V1_0::RadioResponseInfo& info, int32_t remainingRetries) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->supplyIccPukForAppResponse(toAidl(info), remainingRetries);
    }
    return Void();
}
Return<void> RadioSimResponseHook::supplySimDepersonalizationResponse(
        const V1_0::RadioResponseInfo& info, const V1_5::PersoSubstate persoType,
        int32_t remainingRetries) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->supplySimDepersonalizationResponse(toAidl(info),
                (const ::aidl::android::hardware::radio::sim::PersoSubstate)persoType,
                remainingRetries);
    }
    return Void();
}
Return<void> RadioSimResponseHook::updateSimPhonebookRecordsResponse(
        const V1_6::RadioResponseInfo& info, int32_t updatedRecordIndex) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->updateSimPhonebookRecordsResponse(toAidl(info), updatedRecordIndex);
    }
    return Void();
}

Return<void> RadioSimResponseHook::setSimCardPowerResponse_1_6(
        const V1_6::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mSimCb=%p", TAG, __FUNCTION__, mSimCb.get());
#endif
    if (mSimCb) {
        mSimCb->setSimCardPowerResponse(toAidl(info));
    }
    return Void();
}

