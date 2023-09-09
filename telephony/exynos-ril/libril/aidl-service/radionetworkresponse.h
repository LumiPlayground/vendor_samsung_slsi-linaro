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
#pragma once

#include <hidl/1.6/radio_hidl_utils_1_6.h>
#include <telephony/ril.h>
#include <radionetworkcallback.h>

struct RadioNetworkResponseHook : public RadioResponseBase_1_6 {

    std::shared_ptr<android::hardware::radio::impl::RadioNetworkResponseCallback> mNetworkCb;

    // network
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioNetworkResponseCallback> networkCb);

    ::android::hardware::Return<void> getAvailableBandModesResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::RadioBandMode>& bandModes);
    ::android::hardware::Return<void> getAvailableNetworksResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::OperatorInfo>& networkInfos);
    ::android::hardware::Return<void> getCdmaRoamingPreferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            ::android::hardware::radio::V1_0::CdmaRoamingType type);
    ::android::hardware::Return<void> getImsRegistrationStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            bool isRegistered, ::android::hardware::radio::V1_0::RadioTechnologyFamily ratFamily);
    ::android::hardware::Return<void> getNetworkSelectionModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, bool manual);
    ::android::hardware::Return<void> getOperatorResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_string& longName, const ::android::hardware::hidl_string& shortName, const ::android::hardware::hidl_string& numeric);
    ::android::hardware::Return<void> getVoiceRadioTechnologyResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            ::android::hardware::radio::V1_0::RadioTechnology rat);
    ::android::hardware::Return<void> setBandModeResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setBarringPasswordResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setCdmaRoamingPreferenceResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setCellInfoListRateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setLocationUpdatesResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setNetworkSelectionModeAutomaticResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setSuppServiceNotificationsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> supplyNetworkDepersonalizationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int remainingRetries);
    ::android::hardware::Return<void> stopNetworkScanResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> getBarringInfoResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>& barringInfos);
    ::android::hardware::Return<void> setIndicationFilterResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setLinkCapacityReportingCriteriaResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setNetworkSelectionModeManualResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setSignalStrengthReportingCriteriaResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setSystemSelectionChannelsResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> startNetworkScanResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    ::android::hardware::Return<void> getAllowedNetworkTypesBitmapResponse(const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
            const ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_4::RadioAccessFamily> networkTypeBitmap);
    ::android::hardware::Return<void> getCellInfoListResponse_1_6(const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>& cellInfo);
    ::android::hardware::Return<void> getDataRegistrationStateResponse_1_6(const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_6::RegStateResult& dataRegResponse);
    ::android::hardware::Return<void> getSignalStrengthResponse_1_6(const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_6::SignalStrength& signalStrength);
    ::android::hardware::Return<void> getSystemSelectionChannelsResponse(const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::RadioAccessSpecifier>& specifiers);
    ::android::hardware::Return<void> getVoiceRegistrationStateResponse_1_6(const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_6::RegStateResult& voiceRegResponse);
    ::android::hardware::Return<void> isNrDualConnectivityEnabledResponse(const ::android::hardware::radio::V1_6::RadioResponseInfo& info, bool isEnabled);
    ::android::hardware::Return<void> setAllowedNetworkTypesBitmapResponse(const ::android::hardware::radio::V1_6::RadioResponseInfo& info);
    ::android::hardware::Return<void> setNrDualConnectivityStateResponse(const ::android::hardware::radio::V1_6::RadioResponseInfo& info);
};

struct RadioNetworkIndicationHook : public RadioIndicationBase_1_6 {
    std::shared_ptr<android::hardware::radio::impl::RadioNetworkIndicationCallback> mNetworkCb;

    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioNetworkIndicationCallback> networkCb);
    ::android::hardware::Return<void> barringInfoChanged (::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_5::BarringInfo>& barringInfos);
    ::android::hardware::Return<void> cdmaPrlChanged (::android::hardware::radio::V1_0::RadioIndicationType type, int32_t version);
    ::android::hardware::Return<void> imsNetworkStateChanged (::android::hardware::radio::V1_0::RadioIndicationType type);
    ::android::hardware::Return<void> networkStateChanged (::android::hardware::radio::V1_0::RadioIndicationType type);
    ::android::hardware::Return<void> registrationFailed (::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_5::CellIdentity& cellIdentity,
            const ::android::hardware::hidl_string& chosenPlmn,
            ::android::hardware::hidl_bitfield<::android::hardware::radio::V1_5::Domain> domain,
            int32_t causeCode, int32_t additionalCauseCode);
    ::android::hardware::Return<void> restrictedStateChanged (::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::PhoneRestrictedState state);
    ::android::hardware::Return<void> suppSvcNotify (::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::SuppSvcNotification& suppSvc);
    ::android::hardware::Return<void> voiceRadioTechChanged (::android::hardware::radio::V1_0::RadioIndicationType type,
            ::android::hardware::radio::V1_0::RadioTechnology rat);

    ::android::hardware::Return<void> currentLinkCapacityEstimate_1_6 (::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_6::LinkCapacityEstimate& lce);
    ::android::hardware::Return<void> cellInfoList_1_6 (::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::CellInfo>& records);
    ::android::hardware::Return<void> currentPhysicalChannelConfigs_1_6 (::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::PhysicalChannelConfig>& configs);
    ::android::hardware::Return<void> currentSignalStrength_1_6 (::android::hardware::radio::V1_0::RadioIndicationType type,
        const ::android::hardware::radio::V1_6::SignalStrength& signalStrength);
    ::android::hardware::Return<void> networkScanResult_1_6 (::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_6::NetworkScanResult& result);
};

struct RadioNetworkResponse {
    ::android::sp<RadioNetworkResponseHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioNetworkResponseCallback> mNetworkCb;

    ::android::sp<RadioNetworkResponseHook> hidl() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioNetworkResponseCallback> networkCb) {
        mNetworkCb = networkCb;
        if (mHidl) {
            mHidl->setResponseFunction(mNetworkCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioNetworkResponse>();
        instance->mHidl = ::android::sp<RadioNetworkResponseHook>::make();
        return instance;
    }
};

struct RadioNetworkIndication {
    ::android::sp<RadioNetworkIndicationHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioNetworkIndicationCallback> mNetworkCb;

    ::android::sp<RadioNetworkIndicationHook> hidl() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioNetworkIndicationCallback> networkCb) {
        mNetworkCb = networkCb;
        if (mHidl) {
            mHidl->setResponseFunction(mNetworkCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioNetworkIndication>();
        instance->mHidl = ::android::sp<RadioNetworkIndicationHook>::make();
        return instance;
    }
};
