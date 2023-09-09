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
#include <aidl-log.h>
#include "radionetwork.h"
#include "radionetworkindication.h"
#include "radionetworkresponse.h"

#define TAG "RadioNetwork"

namespace android::hardware::radio::impl {

using namespace std::string_literals;
using ::ndk::ScopedAStatus;
namespace aidl = ::aidl::android::hardware::radio::network;
namespace aidl_common = ::aidl::android::hardware::radio;
constexpr auto ok = &ScopedAStatus::ok;

RadioNetwork::RadioNetwork(std::shared_ptr<RadioNetworkCallback> radio, uint32_t slot)
    : mRadio(radio), mSlot(slot) {
    mCallbacks = {
        mRadio,
        std::make_shared<RadioNetworkResponse>(),
        std::make_shared<RadioNetworkIndication>()
    };
}

ScopedAStatus RadioNetwork::setResponseFunctions(
        const std::shared_ptr<aidl::IRadioNetworkResponse>& networkResponse,
        const std::shared_ptr<aidl::IRadioNetworkIndication>& networkIndication) {
#if VDBG
    RLOGD("%s::%s networkResponse=%p networkIndication=%p", TAG, __FUNCTION__,
            networkResponse.get(), networkIndication.get());
#endif
    mCallbacks.setResponseFunctions(networkResponse, networkIndication);
    return ok();
}

ScopedAStatus RadioNetwork::getAllowedNetworkTypesBitmap(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getAllowedNetworkTypesBitmap(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getAvailableBandModes(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getAvailableBandModes(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getAvailableNetworks(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getAvailableNetworks(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getBarringInfo(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getBarringInfo(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getCdmaRoamingPreference(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getCdmaRoamingPreference(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getCellInfoList(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getCellInfoList(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getDataRegistrationState(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getDataRegistrationState(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getImsRegistrationState(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getImsRegistrationState(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getNetworkSelectionMode(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getNetworkSelectionMode(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getOperator(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getOperator(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getSignalStrength(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getSignalStrength(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getSystemSelectionChannels(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getSystemSelectionChannels(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getVoiceRadioTechnology(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getVoiceRadioTechnology(serial);
    return ok();
}

ScopedAStatus RadioNetwork::getVoiceRegistrationState(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getVoiceRegistrationState(serial);
    return ok();
}

ScopedAStatus RadioNetwork::isNrDualConnectivityEnabled(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->isNrDualConnectivityEnabled(serial);
    return ok();
}

ScopedAStatus RadioNetwork::responseAcknowledgement() {
#if VDBG
    RLOGD("%s::%s serial", TAG, __FUNCTION__);
#endif
    mRadio->responseAcknowledgement();
    return ok();
}

ScopedAStatus RadioNetwork::setAllowedNetworkTypesBitmap(int32_t serial, int32_t ntype) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setAllowedNetworkTypesBitmap(serial, ntype);
    return ok();
}

ScopedAStatus RadioNetwork::setBandMode(int32_t serial, aidl::RadioBandMode mode) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setBandMode(serial, mode);
    return ok();
}

ScopedAStatus RadioNetwork::setBarringPassword(int32_t serial, const std::string& facility,
        const std::string& oldPw, const std::string& newPw) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setBarringPassword(serial, facility, oldPw, newPw);
    return ok();
}

ScopedAStatus RadioNetwork::setCdmaRoamingPreference(int32_t serial, aidl::CdmaRoamingType type) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setCdmaRoamingPreference(serial, type);
    return ok();
}

ScopedAStatus RadioNetwork::setCellInfoListRate(int32_t serial, int32_t rate) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setCellInfoListRate(serial, rate);
    return ok();
}

ScopedAStatus RadioNetwork::setIndicationFilter(int32_t serial, int32_t indFilter) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setIndicationFilter(serial, indFilter);
    return ok();
}

ScopedAStatus RadioNetwork::setLinkCapacityReportingCriteria(  //
        int32_t serial, int32_t hysteresisMs, int32_t hysteresisDlKbps, int32_t hysteresisUlKbps,
        const std::vector<int32_t>& thrDownlinkKbps, const std::vector<int32_t>& thrUplinkKbps,
        aidl_common::AccessNetwork accessNetwork) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setLinkCapacityReportingCriteria(  //
            serial, hysteresisMs, hysteresisDlKbps, hysteresisUlKbps, thrDownlinkKbps,
            thrUplinkKbps, accessNetwork);
    return ok();
}

ScopedAStatus RadioNetwork::setLocationUpdates(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setLocationUpdates(serial, enable);
    return ok();
}

ScopedAStatus RadioNetwork::setNetworkSelectionModeAutomatic(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setNetworkSelectionModeAutomatic(serial);
    return ok();
}

ScopedAStatus RadioNetwork::setNetworkSelectionModeManual(
        int32_t serial, const std::string& opNumeric, aidl_common::AccessNetwork ran) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setNetworkSelectionModeManual(serial, opNumeric, ran);
    return ok();
}

ScopedAStatus RadioNetwork::setNrDualConnectivityState(int32_t serial,
        aidl::NrDualConnectivityState st) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setNrDualConnectivityState(serial, st);
    return ok();
}

ScopedAStatus RadioNetwork::setSignalStrengthReportingCriteria(
        int32_t serial, const std::vector<aidl::SignalThresholdInfo>& infos) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setSignalStrengthReportingCriteria(serial, infos);
    return ok();
}

ScopedAStatus RadioNetwork::setSuppServiceNotifications(int32_t serial, bool enable) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setSuppServiceNotifications(serial, enable);
    return ok();
}

ScopedAStatus RadioNetwork::setSystemSelectionChannels(
        int32_t serial, bool specifyCh, const std::vector<aidl::RadioAccessSpecifier>& specifiers) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setSystemSelectionChannels(serial, specifyCh, specifiers);
    return ok();
}

ScopedAStatus RadioNetwork::startNetworkScan(int32_t serial, const aidl::NetworkScanRequest& req) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->startNetworkScan(serial, req);
    return ok();
}

ScopedAStatus RadioNetwork::stopNetworkScan(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->stopNetworkScan(serial);
    return ok();
}

ScopedAStatus RadioNetwork::supplyNetworkDepersonalization(int32_t serial, const std::string& nPin) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->supplyNetworkDepersonalization(serial, nPin);
    return ok();
}

// TODO(b/210498497): is there a cleaner way to send a response back to Android, even though these
// methods must never be called?
ScopedAStatus RadioNetwork::setUsageSetting(
        int32_t serial, ::aidl::android::hardware::radio::network::UsageSetting usageSetting) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setUsageSetting(serial, usageSetting);
    return ok();
}

ScopedAStatus RadioNetwork::getUsageSetting(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getUsageSetting(serial);
    return ok();
}

std::shared_ptr<RadioNetwork> RadioNetwork::make(std::shared_ptr<RadioNetworkCallback> radio, uint32_t slot) {
#if VDBG
    RLOGD("%s::%s slot %u", TAG, __FUNCTION__, slot);
#endif
    return ndk::SharedRefBase::make<RadioNetwork>(radio, slot);
}

}

