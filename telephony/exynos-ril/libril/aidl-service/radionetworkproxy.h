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

#include <radionetworkcallback.h>
#include <radiocallbackadapter.h>
#include <radiointerface.h>
#include <vector>

struct RadioNetworkProxy : public RadioInterface,
                         public android::hardware::radio::impl::RadioNetworkCallbackAdapter {
    using RadioInterface::RadioInterface;

    std::shared_ptr<android::hardware::radio::impl::RadioNetworkResponseCallback> mResponseCb;
    std::shared_ptr<android::hardware::radio::impl::RadioNetworkIndicationCallback> mIndicationCb;

    void init();

    template<typename T, typename... Args>
    void onRequest(int request, Args&&... args) {
        auto command = getCommand<T>(request);
        if (command == nullptr) {
            command = std::make_shared<T>(mSlot, mResponseCb);
            RadioInterface::updateCommand(request, command);
        }
        RadioInterface::onRequest(command, args...);
    }

    template<typename T>
    std::shared_ptr<T> listen(int indication) {
        auto handler = std::make_shared<T>(mSlot, mIndicationCb);
        RadioInterface::updateIndication(indication, handler);
        return handler;
    }

    void setResponseFunctions(
            const std::shared_ptr<android::hardware::radio::impl::RadioNetworkResponseCallback>& responseCb,
            const std::shared_ptr<android::hardware::radio::impl::RadioNetworkIndicationCallback>& indicationCb);

    void getAllowedNetworkTypesBitmap(int32_t serial) override;
    void getAvailableBandModes(int32_t serial) override;
    void getAvailableNetworks(int32_t serial) override;
    void getBarringInfo(int32_t serial) override;
    void getCdmaRoamingPreference(int32_t serial) override;
    void getCellInfoList(int32_t serial) override;
    void getDataRegistrationState(int32_t serial) override;
    void getImsRegistrationState(int32_t serial) override;
    void getNetworkSelectionMode(int32_t serial) override;
    void getOperator(int32_t serial) override;
    void getSignalStrength(int32_t serial) override;
    void getSystemSelectionChannels(int32_t serial) override;
    void getVoiceRadioTechnology(int32_t serial) override;
    void getVoiceRegistrationState(int32_t serial) override;
    void isNrDualConnectivityEnabled(int32_t serial) override;
    void responseAcknowledgement() override;
    void setAllowedNetworkTypesBitmap(int32_t serial, int32_t networkTypeBitmap) override;
    void setBandMode(int32_t serial,
            ::aidl::android::hardware::radio::network::RadioBandMode mode) override;
    void setBarringPassword(int32_t serial, const std::string& facility,
            const std::string& oldPassword, const std::string& newPassword) override;
    void setCdmaRoamingPreference(int32_t serial,
            ::aidl::android::hardware::radio::network::CdmaRoamingType type) override;
    void setCellInfoListRate(int32_t serial, int32_t rate) override;
    void setIndicationFilter(int32_t serial, int32_t indicationFilter) override;
    void setLinkCapacityReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
            int32_t hysteresisUlKbps, const std::vector<int32_t>& thresholdsDownlinkKbps,
            const std::vector<int32_t>& thresholdsUplinkKbps,
            ::aidl::android::hardware::radio::AccessNetwork accessNetwork) override;
    void setLocationUpdates(int32_t serial, bool enable) override;
    void setNetworkSelectionModeAutomatic(int32_t serial) override;
    void setNetworkSelectionModeManual(int32_t serial, const std::string& operatorNumeric,
            ::aidl::android::hardware::radio::AccessNetwork ran) override;
    void setNrDualConnectivityState(int32_t serial,
            ::aidl::android::hardware::radio::network::NrDualConnectivityState nrSt) override;
    void setSignalStrengthReportingCriteria(int32_t serial,
            const std::vector<::aidl::android::hardware::radio::network::SignalThresholdInfo>&
            signalThresholdInfos) override;
    void setSuppServiceNotifications(int32_t serial, bool enable) override;
    void setSystemSelectionChannels(int32_t serial, bool specifyChannels,
            const std::vector<::aidl::android::hardware::radio::network::RadioAccessSpecifier>&
            specifiers) override;
    void startNetworkScan(int32_t serial,
            const ::aidl::android::hardware::radio::network::NetworkScanRequest& request) override;
    void stopNetworkScan(int32_t serial) override;
    void supplyNetworkDepersonalization(int32_t serial, const std::string& netPin) override;
    void setUsageSetting(int32_t serial,
            ::aidl::android::hardware::radio::network::UsageSetting usageSetting) override;
    void getUsageSetting(int32_t serial) override;

    static auto make(uint32_t slot, const std::shared_ptr<RadioProxy>& radio) {
        auto proxy = std::make_shared<RadioNetworkProxy>(slot, radio);
        proxy->init();
        return proxy;
    }
};
