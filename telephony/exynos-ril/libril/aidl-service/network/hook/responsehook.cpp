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

#include "radionetworkresponse.h"
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
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_network = ::aidl::android::hardware::radio::network;

#define TAG "RadioNetworkResponseHook"

void RadioNetworkResponseHook::setResponseFunction(std::shared_ptr<RadioNetworkResponseCallback> networkCb) {
    RLOGD("%s::%s networkCb=%p", TAG, __FUNCTION__, networkCb.get());
    mNetworkCb = networkCb;
}

Return<void> RadioNetworkResponseHook::getAvailableBandModesResponse(const V1_0::RadioResponseInfo& info,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::RadioBandMode>& bandModes) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getAvailableBandModesResponse(toAidl(info), toAidl(bandModes));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getAvailableNetworksResponse(const V1_0::RadioResponseInfo& info,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::OperatorInfo>& networkInfos) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getAvailableNetworksResponse(toAidl(info), toAidl(networkInfos));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getCdmaRoamingPreferenceResponse(const V1_0::RadioResponseInfo& info,
        V1_0::CdmaRoamingType type) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getCdmaRoamingPreferenceResponse(toAidl(info), aidl_network::CdmaRoamingType(type));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getImsRegistrationStateResponse(const V1_0::RadioResponseInfo& info,
        bool isRegistered, V1_0::RadioTechnologyFamily ratFamily) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getImsRegistrationStateResponse(toAidl(info), isRegistered,
                aidl_common::RadioTechnologyFamily(ratFamily));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getNetworkSelectionModeResponse(const V1_0::RadioResponseInfo& info, bool manual) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getNetworkSelectionModeResponse(toAidl(info), manual);
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getOperatorResponse(const V1_0::RadioResponseInfo& info,
        const ::android::hardware::hidl_string& longName, const ::android::hardware::hidl_string& shortName, const ::android::hardware::hidl_string& numeric) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getOperatorResponse(toAidl(info), longName, shortName, numeric);
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getVoiceRadioTechnologyResponse(const V1_0::RadioResponseInfo& info,
        ::android::hardware::radio::V1_0::RadioTechnology rat) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getVoiceRadioTechnologyResponse(toAidl(info), aidl_common::RadioTechnology(rat));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setBandModeResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setBandModeResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setBarringPasswordResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setBarringPasswordResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setCdmaRoamingPreferenceResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setCdmaRoamingPreferenceResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setCellInfoListRateResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setCellInfoListRateResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setLocationUpdatesResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setLocationUpdatesResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setNetworkSelectionModeAutomaticResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setNetworkSelectionModeAutomaticResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setSuppServiceNotificationsResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setSuppServiceNotificationsResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::supplyNetworkDepersonalizationResponse(const V1_0::RadioResponseInfo& info, int remainingRetries) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->supplyNetworkDepersonalizationResponse(toAidl(info), remainingRetries);
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::stopNetworkScanResponse(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->stopNetworkScanResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getBarringInfoResponse(const V1_0::RadioResponseInfo& info,
        const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>& barringInfos) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getBarringInfoResponse(toAidl(info), toAidl(cellIdentity), toAidl(barringInfos));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setIndicationFilterResponse_1_5(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setIndicationFilterResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setLinkCapacityReportingCriteriaResponse_1_5(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setLinkCapacityReportingCriteriaResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setNetworkSelectionModeManualResponse_1_5(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setNetworkSelectionModeManualResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setSignalStrengthReportingCriteriaResponse_1_5(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setSignalStrengthReportingCriteriaResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setSystemSelectionChannelsResponse_1_5(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setSystemSelectionChannelsResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::startNetworkScanResponse_1_5(const V1_0::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->startNetworkScanResponse(toAidl(info));
    }
    return Void();
}


Return<void> RadioNetworkResponseHook::getAllowedNetworkTypesBitmapResponse(const V1_6::RadioResponseInfo& info,
        const ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::RadioAccessFamily> networkTypeBitmap) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getAllowedNetworkTypesBitmapResponse(toAidl(info), networkTypeBitmap);
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getCellInfoListResponse_1_6(const V1_6::RadioResponseInfo& info,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>& cellInfo) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getCellInfoListResponse(toAidl(info), toAidl(cellInfo));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getDataRegistrationStateResponse_1_6(const V1_6::RadioResponseInfo& info,
        const ::android::hardware::radio::V1_6::RegStateResult& dataRegResponse) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getDataRegistrationStateResponse(toAidl(info), toAidl(dataRegResponse));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getSignalStrengthResponse_1_6(const V1_6::RadioResponseInfo& info,
        const ::android::hardware::radio::V1_6::SignalStrength& signalStrength) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getSignalStrengthResponse(toAidl(info), toAidl(signalStrength));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getSystemSelectionChannelsResponse(const V1_6::RadioResponseInfo& info,
        const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::RadioAccessSpecifier>& specifiers) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getSystemSelectionChannelsResponse(toAidl(info), toAidl(specifiers));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::getVoiceRegistrationStateResponse_1_6(const V1_6::RadioResponseInfo& info,
        const ::android::hardware::radio::V1_6::RegStateResult& voiceRegResponse) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->getVoiceRegistrationStateResponse(toAidl(info), toAidl(voiceRegResponse));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::isNrDualConnectivityEnabledResponse(const V1_6::RadioResponseInfo& info, bool isEnabled) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->isNrDualConnectivityEnabledResponse(toAidl(info), isEnabled);
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setAllowedNetworkTypesBitmapResponse(const V1_6::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setAllowedNetworkTypesBitmapResponse(toAidl(info));
    }
    return Void();
}

Return<void> RadioNetworkResponseHook::setNrDualConnectivityStateResponse(const V1_6::RadioResponseInfo& info) {
#if VDBG
    RLOGD("%s::%s mNetworkCb=%p", TAG, __FUNCTION__, mNetworkCb.get());
#endif
    if (mNetworkCb) {
        mNetworkCb->setNrDualConnectivityStateResponse(toAidl(info));
    }
    return Void();
}
