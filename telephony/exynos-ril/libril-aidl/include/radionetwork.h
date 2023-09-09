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
#include <radiocallbackmanager.h>
#include <radionetworkcallback.h>

namespace android::hardware::radio::impl {
struct RadioNetwork : public aidl::android::hardware::radio::network::BnRadioNetwork {
    std::shared_ptr<RadioNetworkCallback> mRadio;
    uint32_t mSlot;
    RadioCallbackManager<RadioNetworkCallback, RadioNetworkResponseCallback,
                RadioNetworkIndicationCallback> mCallbacks;

    RadioNetwork(std::shared_ptr<RadioNetworkCallback> radio, uint32_t slot);

    ::ndk::ScopedAStatus getAllowedNetworkTypesBitmap(int32_t serial) override;
    ::ndk::ScopedAStatus getAvailableBandModes(int32_t serial) override;
    ::ndk::ScopedAStatus getAvailableNetworks(int32_t serial) override;
    ::ndk::ScopedAStatus getBarringInfo(int32_t serial) override;
    ::ndk::ScopedAStatus getCdmaRoamingPreference(int32_t serial) override;
    ::ndk::ScopedAStatus getCellInfoList(int32_t serial) override;
    ::ndk::ScopedAStatus getDataRegistrationState(int32_t serial) override;
    ::ndk::ScopedAStatus getImsRegistrationState(int32_t serial) override;
    ::ndk::ScopedAStatus getNetworkSelectionMode(int32_t serial) override;
    ::ndk::ScopedAStatus getOperator(int32_t serial) override;
    ::ndk::ScopedAStatus getSignalStrength(int32_t serial) override;
    ::ndk::ScopedAStatus getSystemSelectionChannels(int32_t serial) override;
    ::ndk::ScopedAStatus getVoiceRadioTechnology(int32_t serial) override;
    ::ndk::ScopedAStatus getVoiceRegistrationState(int32_t serial) override;
    ::ndk::ScopedAStatus isNrDualConnectivityEnabled(int32_t serial) override;
    ::ndk::ScopedAStatus responseAcknowledgement() override;
    ::ndk::ScopedAStatus setAllowedNetworkTypesBitmap(
            int32_t serial, int32_t networkTypeBitmap) override;
    ::ndk::ScopedAStatus setBandMode(
            int32_t serial, ::aidl::android::hardware::radio::network::RadioBandMode mode) override;
    ::ndk::ScopedAStatus setBarringPassword(int32_t serial, const std::string& facility,
                                            const std::string& oldPassword,
                                            const std::string& newPassword) override;
    ::ndk::ScopedAStatus setCdmaRoamingPreference(
            int32_t serial,
            ::aidl::android::hardware::radio::network::CdmaRoamingType type) override;
    ::ndk::ScopedAStatus setCellInfoListRate(int32_t serial, int32_t rate) override;
    ::ndk::ScopedAStatus setIndicationFilter(
            int32_t serial, int32_t indicationFilter) override;
    ::ndk::ScopedAStatus setLinkCapacityReportingCriteria(
            int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
            int32_t hysteresisUlKbps, const std::vector<int32_t>& thresholdsDownlinkKbps,
            const std::vector<int32_t>& thresholdsUplinkKbps,
            ::aidl::android::hardware::radio::AccessNetwork accessNetwork) override;
    ::ndk::ScopedAStatus setLocationUpdates(int32_t serial, bool enable) override;
    ::ndk::ScopedAStatus setNetworkSelectionModeAutomatic(int32_t serial) override;
    ::ndk::ScopedAStatus setNetworkSelectionModeManual(
            int32_t serial, const std::string& operatorNumeric,
            ::aidl::android::hardware::radio::AccessNetwork ran) override;
    ::ndk::ScopedAStatus setNrDualConnectivityState(
            int32_t serial,
            ::aidl::android::hardware::radio::network::NrDualConnectivityState nrSt) override;
    ::ndk::ScopedAStatus setResponseFunctions(
            const std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkResponse>&
                    radioNetworkResponse,
            const std::shared_ptr<
                    ::aidl::android::hardware::radio::network::IRadioNetworkIndication>&
                    radioNetworkIndication) override;
    ::ndk::ScopedAStatus setSignalStrengthReportingCriteria(
            int32_t serial,
            const std::vector<::aidl::android::hardware::radio::network::SignalThresholdInfo>&
                    signalThresholdInfos) override;
    ::ndk::ScopedAStatus setSuppServiceNotifications(int32_t serial, bool enable) override;
    ::ndk::ScopedAStatus setSystemSelectionChannels(
            int32_t serial, bool specifyChannels,
            const std::vector<::aidl::android::hardware::radio::network::RadioAccessSpecifier>&
                    specifiers) override;
    ::ndk::ScopedAStatus startNetworkScan(
            int32_t serial,
            const ::aidl::android::hardware::radio::network::NetworkScanRequest& request) override;
    ::ndk::ScopedAStatus stopNetworkScan(int32_t serial) override;
    ::ndk::ScopedAStatus supplyNetworkDepersonalization(int32_t serial,
                                                        const std::string& netPin) override;
    ::ndk::ScopedAStatus setUsageSetting(int32_t serial,
            ::aidl::android::hardware::radio::network::UsageSetting usageSetting) override;
    ::ndk::ScopedAStatus getUsageSetting(int32_t serial) override;

    static std::shared_ptr<RadioNetwork> make(std::shared_ptr<RadioNetworkCallback> radio, uint32_t slot);
};

}
