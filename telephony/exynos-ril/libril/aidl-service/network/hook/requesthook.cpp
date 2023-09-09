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

#include "radionetworkproxy.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
namespace impl = android::hardware::radio::impl;
namespace aidl_common = ::aidl::android::hardware::radio;
namespace aidl_network = ::aidl::android::hardware::radio::network;
using android::hardware::radio::compat::toRadioAccessNetworks;
using ::android::sp;

void RadioNetworkProxy::getAllowedNetworkTypesBitmap(int32_t serial) {
    mHidlRadio->getAllowedNetworkTypesBitmap(serial);
}

void RadioNetworkProxy::getAvailableBandModes(int32_t serial) {
    mHidlRadio->getAvailableBandModes(serial);
}

void RadioNetworkProxy::getAvailableNetworks(int32_t serial) {
    mHidlRadio->getAvailableNetworks(serial);
}

void RadioNetworkProxy::getBarringInfo(int32_t serial) {
    mHidlRadio->getBarringInfo(serial);
}

void RadioNetworkProxy::getCdmaRoamingPreference(int32_t serial) {
    mHidlRadio->getCdmaRoamingPreference(serial);
}

void RadioNetworkProxy::getCellInfoList(int32_t serial) {
    mHidlRadio->getCellInfoList_1_6(serial);
}

void RadioNetworkProxy::getDataRegistrationState(int32_t serial) {
    mHidlRadio->getDataRegistrationState_1_6(serial);
}

void RadioNetworkProxy::getImsRegistrationState(int32_t serial) {
    mHidlRadio->getImsRegistrationState(serial);
}

void RadioNetworkProxy::getNetworkSelectionMode(int32_t serial) {
    mHidlRadio->getNetworkSelectionMode(serial);
}

void RadioNetworkProxy::getOperator(int32_t serial) {
    mHidlRadio->getOperator(serial);
}

void RadioNetworkProxy::getSignalStrength(int32_t serial) {
    mHidlRadio->getSignalStrength_1_6(serial);
}

void RadioNetworkProxy::getSystemSelectionChannels(int32_t serial) {
    mHidlRadio->getSystemSelectionChannels(serial);
}

void RadioNetworkProxy::getVoiceRadioTechnology(int32_t serial) {
    mHidlRadio->getVoiceRadioTechnology(serial);
}

void RadioNetworkProxy::getVoiceRegistrationState(int32_t serial) {
    mHidlRadio->getVoiceRegistrationState_1_6(serial);
}

void RadioNetworkProxy::isNrDualConnectivityEnabled(int32_t serial) {
    mHidlRadio->isNrDualConnectivityEnabled(serial);
}

void RadioNetworkProxy::responseAcknowledgement() {
    mHidlRadio->responseAcknowledgement();
}

void RadioNetworkProxy::setAllowedNetworkTypesBitmap(int32_t serial, int32_t networkTypeBitmap) {
    const auto raf = compat::toHidlBitfield<V1_4::RadioAccessFamily>(networkTypeBitmap);
    mHidlRadio->setAllowedNetworkTypesBitmap(serial, raf);
}

void RadioNetworkProxy::setBandMode(int32_t serial, aidl_network::RadioBandMode mode) {
    mHidlRadio->setBandMode(serial, V1_0::RadioBandMode(mode));
}

void RadioNetworkProxy::setBarringPassword(int32_t serial, const std::string& facility,
        const std::string& oldPassword, const std::string& newPassword) {
    mHidlRadio->setBarringPassword(serial, facility, oldPassword, newPassword);
}

void RadioNetworkProxy::setCdmaRoamingPreference(int32_t serial,
        aidl_network::CdmaRoamingType type) {
    mHidlRadio->setCdmaRoamingPreference(serial, V1_0::CdmaRoamingType(type));
}

void RadioNetworkProxy::setCellInfoListRate(int32_t serial, int32_t rate) {
    mHidlRadio->setCellInfoListRate(serial, rate);
}

void RadioNetworkProxy::setIndicationFilter(int32_t serial, int32_t indicationFilter) {
    mHidlRadio->setIndicationFilter_1_5(serial, compat::toHidlBitfield<V1_5::IndicationFilter>(indicationFilter));
}

void RadioNetworkProxy::setLinkCapacityReportingCriteria(int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps,
        int32_t hysteresisUlKbps, const std::vector<int32_t>& thresholdsDownlinkKbps,
        const std::vector<int32_t>& thresholdsUplinkKbps,
        aidl_common::AccessNetwork accessNetwork) {
    mHidlRadio->setLinkCapacityReportingCriteria_1_5(serial, hysteresisMs, hysteresisDlKbps,
            hysteresisUlKbps, thresholdsDownlinkKbps, thresholdsUplinkKbps, V1_5::AccessNetwork(accessNetwork));
}

void RadioNetworkProxy::setLocationUpdates(int32_t serial, bool enable) {
    mHidlRadio->setLocationUpdates(serial, enable);
}

void RadioNetworkProxy::setNetworkSelectionModeAutomatic(int32_t serial) {
    mHidlRadio->setNetworkSelectionModeAutomatic(serial);
}

void RadioNetworkProxy::setNetworkSelectionModeManual(int32_t serial, const std::string& operatorNumeric,
        aidl_common::AccessNetwork ran) {
    mHidlRadio->setNetworkSelectionModeManual_1_5(serial, operatorNumeric, toRadioAccessNetworks(ran));
}

void RadioNetworkProxy::setNrDualConnectivityState(int32_t serial,
        aidl_network::NrDualConnectivityState nrSt) {
    mHidlRadio->setNrDualConnectivityState(serial, V1_6::NrDualConnectivityState(nrSt));
}

void RadioNetworkProxy::setSuppServiceNotifications(int32_t serial, bool enable) {
    mHidlRadio->setSuppServiceNotifications(serial, enable);
}

void RadioNetworkProxy::setSystemSelectionChannels(int32_t serial, bool specifyChannels,
        const std::vector<::aidl::android::hardware::radio::network::RadioAccessSpecifier>&
        specifiers) {
    mHidlRadio->setSystemSelectionChannels_1_5(serial, specifyChannels, compat::toHidl(specifiers));
}

void RadioNetworkProxy::startNetworkScan(int32_t serial,
        const aidl_network::NetworkScanRequest& request) {
    mHidlRadio->startNetworkScan_1_5(serial, compat::toHidl(request));
}

void RadioNetworkProxy::stopNetworkScan(int32_t serial) {
    mHidlRadio->stopNetworkScan(serial);
}

void RadioNetworkProxy::supplyNetworkDepersonalization(int32_t serial, const std::string& netPin) {
    mHidlRadio->supplyNetworkDepersonalization(serial, netPin);
}

