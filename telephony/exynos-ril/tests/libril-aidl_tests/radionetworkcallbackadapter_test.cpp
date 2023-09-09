/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <testutils.h>
#include <radiocallbackadapter.h>

using namespace android::hardware::radio::impl;
using ::aidl::android::hardware::radio::AccessNetwork;
using ::aidl::android::hardware::radio::RadioIndicationType;
using ::aidl::android::hardware::radio::RadioTechnology;
using ::aidl::android::hardware::radio::RadioTechnologyFamily;
using ::aidl::android::hardware::radio::network::CdmaRoamingType ;
using ::aidl::android::hardware::radio::network::NrDualConnectivityState ;
using ::aidl::android::hardware::radio::network::PhoneRestrictedState;
using ::aidl::android::hardware::radio::network::RadioBandMode;
using ::aidl::android::hardware::radio::network::UsageSetting;

TEST(RadioCallbackAdapter, RadioNetworkCallbackAdapter) {
    TestChecker checker;
    auto radio = std::make_shared<RadioNetworkCallbackAdapter>();
    radio->setResponseFunctions(nullptr, nullptr);
    radio->getAllowedNetworkTypesBitmap(0);
    radio->getAvailableBandModes(0);
    radio->getAvailableNetworks(0);
    radio->getBarringInfo(0);
    radio->getCdmaRoamingPreference(0);
    radio->getCellInfoList(0);
    radio->getDataRegistrationState(0);
    radio->getImsRegistrationState(0);
    radio->getNetworkSelectionMode(0);
    radio->getOperator(0);
    radio->getSignalStrength(0);
    radio->getSystemSelectionChannels(0);
    radio->getVoiceRadioTechnology(0);
    radio->getVoiceRegistrationState(0);
    radio->isNrDualConnectivityEnabled(0);
    radio->responseAcknowledgement();
    radio->setAllowedNetworkTypesBitmap(0, 0);
    radio->setBandMode(0, RadioBandMode(0));
    radio->setBarringPassword(0, "", "", "");
    radio->setCdmaRoamingPreference(0, CdmaRoamingType(0));
    radio->setCellInfoListRate(0, 0);
    radio->setIndicationFilter(0, 0);
    radio->setLinkCapacityReportingCriteria(0, 0, 0, 0, {}, {}, AccessNetwork(0));
    radio->setLocationUpdates(0, true);
    radio->setNetworkSelectionModeAutomatic(0);
    radio->setNetworkSelectionModeManual(0, "", AccessNetwork(0));
    radio->setNrDualConnectivityState(0, NrDualConnectivityState (0));
    radio->setSignalStrengthReportingCriteria(0, {});
    radio->setSuppServiceNotifications(0, true);
    radio->setSystemSelectionChannels(0, true, {});
    radio->startNetworkScan(0, {});
    radio->stopNetworkScan(0);
    radio->supplyNetworkDepersonalization(0, "");
    radio->setUsageSetting(0, UsageSetting(0));
    radio->getUsageSetting(0);
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioNetworkResponseCallbackAdapter) {
    TestChecker checker;
    auto response = std::make_shared<RadioNetworkResponseCallbackAdapter>();
    response->setResponseFunction(nullptr);
    response->acknowledgeRequest(0);
    response->getAllowedNetworkTypesBitmapResponse({}, 0);
    response->getAvailableBandModesResponse({}, {});
    response->getAvailableNetworksResponse({}, {});
    response->getBarringInfoResponse({}, {}, {});
    response->getCdmaRoamingPreferenceResponse({}, CdmaRoamingType(0));
    response->getCellInfoListResponse({}, {});
    response->getDataRegistrationStateResponse({}, {});
    response->getImsRegistrationStateResponse({}, true, RadioTechnologyFamily(0));
    response->getNetworkSelectionModeResponse({}, true);
    response->getOperatorResponse({}, "", "", "");
    response->getSignalStrengthResponse({}, {});
    response->getSystemSelectionChannelsResponse({}, {});
    response->getVoiceRadioTechnologyResponse({}, RadioTechnology(0));
    response->getVoiceRegistrationStateResponse({}, {});
    response->isNrDualConnectivityEnabledResponse({}, true);
    response->setAllowedNetworkTypesBitmapResponse({});
    response->setBandModeResponse({});
    response->setBarringPasswordResponse({});
    response->setCdmaRoamingPreferenceResponse({});
    response->setIndicationFilterResponse({});
    response->setLinkCapacityReportingCriteriaResponse({});
    response->setCellInfoListRateResponse({});
    response->setLocationUpdatesResponse({});
    response->setNetworkSelectionModeAutomaticResponse({});
    response->setNetworkSelectionModeManualResponse({});
    response->setNrDualConnectivityStateResponse({});
    response->setSignalStrengthReportingCriteriaResponse({});
    response->setSuppServiceNotificationsResponse({});
    response->setSystemSelectionChannelsResponse({});
    response->startNetworkScanResponse({});
    response->stopNetworkScanResponse({});
    response->supplyNetworkDepersonalizationResponse({}, 0);
    response->setUsageSettingResponse({});
    response->getUsageSettingResponse({}, UsageSetting(0));
    checker.set(true);
}

TEST(RadioCallbackAdapter, RadioNetworkIndicationCallbackAdapter) {
    TestChecker checker;
    auto indication = std::make_shared<RadioNetworkIndicationCallbackAdapter>();
    indication->setResponseFunction(nullptr);
    indication->barringInfoChanged(RadioIndicationType::UNSOLICITED, {}, {});
    indication->cdmaPrlChanged(RadioIndicationType::UNSOLICITED, 0);
    indication->cellInfoList(RadioIndicationType::UNSOLICITED, {});
    indication->currentLinkCapacityEstimate(RadioIndicationType::UNSOLICITED, {});
    indication->currentPhysicalChannelConfigs(RadioIndicationType::UNSOLICITED, {});
    indication->currentSignalStrength(RadioIndicationType::UNSOLICITED, {});
    indication->imsNetworkStateChanged(RadioIndicationType::UNSOLICITED);
    indication->networkScanResult(RadioIndicationType::UNSOLICITED, {});
    indication->networkStateChanged(RadioIndicationType::UNSOLICITED);
    indication->nitzTimeReceived(RadioIndicationType::UNSOLICITED, "", 0, 0);
    indication->registrationFailed(RadioIndicationType::UNSOLICITED, {}, "", 0, 0, 0);
    indication->restrictedStateChanged(RadioIndicationType::UNSOLICITED, PhoneRestrictedState(0));
    indication->suppSvcNotify(RadioIndicationType::UNSOLICITED, {});
    indication->restrictedStateChanged(RadioIndicationType::UNSOLICITED, PhoneRestrictedState(0));
    indication->voiceRadioTechChanged(RadioIndicationType::UNSOLICITED, RadioTechnology(0));
    checker.set(true);
}