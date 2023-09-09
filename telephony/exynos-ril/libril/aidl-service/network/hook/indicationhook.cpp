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

#include "radionetworkresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_network = ::aidl::android::hardware::radio::network;
using namespace ::android::hardware::radio;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::Return;
using ::android::hardware::Void;

#define TAG "RadioNetworkIndicationHook"

void RadioNetworkIndicationHook::setResponseFunction(std::shared_ptr<RadioNetworkIndicationCallback> networkCb) {
    RLOGD("%s::%s networkCb=%p", TAG, __FUNCTION__, networkCb.get());
    mNetworkCb = networkCb;
}

Return<void> RadioNetworkIndicationHook::barringInfoChanged (V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>& barringInfos) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->barringInfoChanged(toAidl(type), toAidl(cellIdentity), toAidl(barringInfos));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::cdmaPrlChanged (V1_0::RadioIndicationType type, int32_t version) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->cdmaPrlChanged(toAidl(type), version);
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::imsNetworkStateChanged (V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->imsNetworkStateChanged(toAidl(type));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::networkStateChanged (V1_0::RadioIndicationType type) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->networkStateChanged(toAidl(type));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::registrationFailed (V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
        const ::android::hardware::hidl_string& chosenPlmn,
        ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_5::Domain> domain,
        int32_t causeCode, int32_t additionalCauseCode) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->registrationFailed(toAidl(type), toAidl(cellIdentity), chosenPlmn,
                domain, causeCode, additionalCauseCode);
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::restrictedStateChanged (V1_0::RadioIndicationType type, V1_0::PhoneRestrictedState state) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->restrictedStateChanged(toAidl(type), aidl_network::PhoneRestrictedState(state));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::suppSvcNotify (V1_0::RadioIndicationType type, const V1_0::SuppSvcNotification& suppSvc) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->suppSvcNotify(toAidl(type), toAidl(suppSvc));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::voiceRadioTechChanged (V1_0::RadioIndicationType type,
        ::android::hardware::radio::V1_0::RadioTechnology rat) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->voiceRadioTechChanged(toAidl(type), aidl_common::RadioTechnology(rat));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::currentLinkCapacityEstimate_1_6 (V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lce) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->currentLinkCapacityEstimate(toAidl(type), toAidl(lce));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::cellInfoList_1_6 (V1_0::RadioIndicationType type,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>& records) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->cellInfoList(toAidl(type), toAidl(records));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::currentPhysicalChannelConfigs_1_6 (V1_0::RadioIndicationType type,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::PhysicalChannelConfig>& configs) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->currentPhysicalChannelConfigs(toAidl(type), toAidl(configs));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::currentSignalStrength_1_6 (V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_6::SignalStrength& signalStrength) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->currentSignalStrength(toAidl(type), toAidl(signalStrength));
    }
    return Void();
}

Return<void> RadioNetworkIndicationHook::networkScanResult_1_6 (V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_6::NetworkScanResult& result) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->networkScanResult(toAidl(type), toAidl(result));
    }
    return Void();
}

