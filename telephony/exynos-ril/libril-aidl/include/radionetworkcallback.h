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

#include <aidl/android/hardware/radio/network/BnRadioNetwork.h>

namespace android::hardware::radio::impl {

struct RadioNetworkResponseCallback;
struct RadioNetworkIndicationCallback;

struct RadioNetworkCallback {
    virtual ~RadioNetworkCallback() = default;
    virtual void setResponseFunctions(
        const std::shared_ptr<RadioNetworkResponseCallback>& responseCb,
        const std::shared_ptr<RadioNetworkIndicationCallback>& indicationCb) = 0;
    virtual void getAllowedNetworkTypesBitmap(int32_t serial) = 0;
    virtual void getAvailableBandModes(int32_t serial) = 0;
    virtual void getAvailableNetworks(int32_t serial) = 0;
    virtual void getBarringInfo(int32_t serial) = 0;
    virtual void getCdmaRoamingPreference(int32_t serial) = 0;
    virtual void getCellInfoList(int32_t serial) = 0;
    virtual void getDataRegistrationState(int32_t serial) = 0;
    virtual void getImsRegistrationState(int32_t serial) = 0;
    virtual void getNetworkSelectionMode(int32_t serial) = 0;
    virtual void getOperator(int32_t serial) = 0;
    virtual void getSignalStrength(int32_t serial) = 0;
    virtual void getSystemSelectionChannels(int32_t serial) = 0;
    virtual void getVoiceRadioTechnology(int32_t serial) = 0;
    virtual void getVoiceRegistrationState(int32_t serial) = 0;
    virtual void isNrDualConnectivityEnabled(int32_t serial) = 0;
    virtual void responseAcknowledgement() = 0;
    virtual void setAllowedNetworkTypesBitmap(int32_t serial, int32_t networkTypeBitmap) = 0;
    virtual void setBandMode(int32_t serial,
            ::aidl::android::hardware::radio::network::RadioBandMode mode) = 0;
    virtual void setBarringPassword(int32_t serial, const std::string& facility,
                                            const std::string& oldPassword,
                                            const std::string& newPassword) = 0;
    virtual void setCdmaRoamingPreference(int32_t serial,
            ::aidl::android::hardware::radio::network::CdmaRoamingType type) = 0;
    virtual void setCellInfoListRate(int32_t serial, int32_t rate) = 0;
    virtual void setIndicationFilter(int32_t serial, int32_t indicationFilter) = 0;
    virtual void setLinkCapacityReportingCriteria(
            int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
            int32_t hysteresisUlKbps, const std::vector<int32_t>& thresholdsDownlinkKbps,
            const std::vector<int32_t>& thresholdsUplinkKbps,
            ::aidl::android::hardware::radio::AccessNetwork accessNetwork) = 0;
    virtual void setLocationUpdates(int32_t serial, bool enable) = 0;
    virtual void setNetworkSelectionModeAutomatic(int32_t serial) = 0;
    virtual void setNetworkSelectionModeManual(int32_t serial, const std::string& operatorNumeric,
            ::aidl::android::hardware::radio::AccessNetwork ran) = 0;
    virtual void setNrDualConnectivityState(int32_t serial,
            ::aidl::android::hardware::radio::network::NrDualConnectivityState nrSt) = 0;
    virtual void setSignalStrengthReportingCriteria(int32_t serial,
            const std::vector<::aidl::android::hardware::radio::network::SignalThresholdInfo>&
                    signalThresholdInfos) = 0;
    virtual void setSuppServiceNotifications(int32_t serial, bool enable) = 0;
    virtual void setSystemSelectionChannels(int32_t serial, bool specifyChannels,
            const std::vector<::aidl::android::hardware::radio::network::RadioAccessSpecifier>&
                    specifiers) = 0;
    virtual void startNetworkScan(int32_t serial,
            const ::aidl::android::hardware::radio::network::NetworkScanRequest& request) = 0;
    virtual void stopNetworkScan(int32_t serial) = 0;
    virtual void supplyNetworkDepersonalization(int32_t serial, const std::string& netPin) = 0;
    virtual void setUsageSetting(int32_t serial,
            ::aidl::android::hardware::radio::network::UsageSetting usageSetting) = 0;
    virtual void  getUsageSetting(int32_t serial) = 0;
};

struct RadioNetworkResponseCallback {
    virtual ~RadioNetworkResponseCallback() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkResponse>&
                    radioNetworkResponse)=0;
    virtual void acknowledgeRequest(int32_t in_serial) = 0;
    virtual void getAllowedNetworkTypesBitmapResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            int networkTypeBitmap) = 0;
    virtual void getAvailableBandModesResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            const std::vector<::aidl::android::hardware::radio::network::RadioBandMode>& in_bandModes) = 0;
    virtual void getAvailableNetworksResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            const std::vector<::aidl::android::hardware::radio::network::OperatorInfo>& in_networkInfos) = 0;
    virtual void getBarringInfoResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            const ::aidl::android::hardware::radio::network::CellIdentity& in_cellIdentity,
            const std::vector<::aidl::android::hardware::radio::network::BarringInfo>& in_barringInfos) = 0;
    virtual void getCdmaRoamingPreferenceResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            ::aidl::android::hardware::radio::network::CdmaRoamingType in_type) = 0;
    virtual void getCellInfoListResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            const std::vector<::aidl::android::hardware::radio::network::CellInfo>& in_cellInfo) = 0;
    virtual void getDataRegistrationStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            const ::aidl::android::hardware::radio::network::RegStateResult& in_dataRegResponse) = 0;
    virtual void getImsRegistrationStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            bool isRegistered, ::aidl::android::hardware::radio::RadioTechnologyFamily in_ratFamily) = 0;
    virtual void getNetworkSelectionModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, bool manual) = 0;
    virtual void getOperatorResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            const std::string& in_longName, const std::string& in_shortName, const std::string& in_numeric) = 0;
    virtual void getSignalStrengthResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            const ::aidl::android::hardware::radio::network::SignalStrength& in_signalStrength) = 0;
    virtual void getSystemSelectionChannelsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            const std::vector<::aidl::android::hardware::radio::network::RadioAccessSpecifier>& in_specifiers) = 0;
    virtual void getVoiceRadioTechnologyResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            ::aidl::android::hardware::radio::RadioTechnology in_rat) = 0;
    virtual void getVoiceRegistrationStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            const ::aidl::android::hardware::radio::network::RegStateResult& in_voiceRegResponse) = 0;
    virtual void isNrDualConnectivityEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, bool in_isEnabled) = 0;
    virtual void setAllowedNetworkTypesBitmapResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setBandModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setBarringPasswordResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setCdmaRoamingPreferenceResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setCellInfoListRateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setIndicationFilterResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setLinkCapacityReportingCriteriaResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setLocationUpdatesResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setNetworkSelectionModeAutomaticResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setNetworkSelectionModeManualResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setNrDualConnectivityStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setSignalStrengthReportingCriteriaResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setSuppServiceNotificationsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setSystemSelectionChannelsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void startNetworkScanResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void stopNetworkScanResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void supplyNetworkDepersonalizationResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info, int in_remainingRetries) = 0;
    virtual void setUsageSettingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void getUsageSettingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
            ::aidl::android::hardware::radio::network::UsageSetting in_usageSetting) = 0;
};

struct RadioNetworkIndicationCallback {
    virtual ~RadioNetworkIndicationCallback() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkIndication>&
                    radioNetworkIndication)=0;
    virtual void barringInfoChanged(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::CellIdentity& cellIdentity,
            const std::vector<::aidl::android::hardware::radio::network::BarringInfo>& barringInfos) = 0;
    virtual void cdmaPrlChanged(::aidl::android::hardware::radio::RadioIndicationType type, int version) = 0;
    virtual void cellInfoList(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::vector<::aidl::android::hardware::radio::network::CellInfo>& records) = 0;
    virtual void currentLinkCapacityEstimate(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::LinkCapacityEstimate& lce) = 0;
    virtual void currentPhysicalChannelConfigs(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::vector<::aidl::android::hardware::radio::network::PhysicalChannelConfig>& configs) = 0;
    virtual void currentSignalStrength(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::SignalStrength& signalStrength) = 0;
    virtual void imsNetworkStateChanged(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void networkScanResult(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::NetworkScanResult& result) = 0;
    virtual void networkStateChanged(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void nitzTimeReceived(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::string& nitzTime, long receivedTimeMs, long ageMs) = 0;
    virtual void registrationFailed(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::CellIdentity& cellIdentity,
            const std::string& chosenPlmn, int domain, int causeCode, int additionalCauseCode) = 0;
    virtual void restrictedStateChanged(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::network::PhoneRestrictedState state) = 0;
    virtual void suppSvcNotify(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::SuppSvcNotification& suppSvc) = 0;
    virtual void voiceRadioTechChanged(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::RadioTechnology rat) = 0;
};

}  // namespace android::hardware::radio::impl {
