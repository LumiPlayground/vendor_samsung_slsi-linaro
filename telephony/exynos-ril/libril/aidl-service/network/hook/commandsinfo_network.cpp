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
#include <android/hardware/radio/1.6/types.h>
#include <slsi/radio_v1_6.h>
#include <ril_internal.h>

#include "commandsinfo.h"
#include "commands.h"
#include "unsolresponse.h"

using namespace android::hardware::radio::impl;
using ::android::sp;

void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioNetworkResponse>& response) {
    // HIDL response hook
    auto hidlResponse = response->hidl();
    android::updateCommandInfo(slot, { RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE, network::GetAvailableBandModesResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_QUERY_AVAILABLE_NETWORKS, network::GetAvailableNetworksResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_BARRING_INFO, network::GetBarringInfoResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE, network::GetCdmaRoamingPreferenceResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_CELL_INFO_LIST, network::GetCellInfoListResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_DATA_REGISTRATION_STATE, network::GetDataRegistrationStateResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_IMS_REGISTRATION_STATE, network::GetImsRegistrationStateResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE, network::GetNetworkSelectionModeResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_OPERATOR, network::GetOperatorResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION, network::SetSuppServiceNotificationsResponse(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_SIGNAL_STRENGTH, HAL_VERSION_CODE(1,4)),
            network::GetSignalStrengthResponse_1_4(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_VOICE_RADIO_TECH, network::GetVoiceRadioTechnologyResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_VOICE_REGISTRATION_STATE, network::GetVoiceRegistrationStateResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_BAND_MODE, network::SetBandModeResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CHANGE_BARRING_PASSWORD, network::SetBarringPasswordResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE, network::SetCdmaRoamingPreferenceResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE, network::SetCellInfoListRateResponse(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER, HAL_VERSION_CODE(1,5)),
            network::SetIndicationFilterResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA, HAL_VERSION_CODE(1,5)),
            network::SetLinkCapacityReportingCriteriaResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_LOCATION_UPDATES, network::SetLocationUpdatesResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC, network::SetNetworkSelectionModeAutomaticResponse(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, HAL_VERSION_CODE(1,5)),
            network::SetNetworkSelectionModeManualResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, HAL_VERSION_CODE(1,5)),
            network::SetSignalStrengthReportingCriteriaResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, HAL_VERSION_CODE(1,5)),
            network::SetSystemSelectionChannelsResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1,5)),
                network::StartNetworkScanResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_STOP_NETWORK_SCAN, network::StopNetworkScanResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION, network::SupplyNetworkDepersonalizationResponse(hidlResponse) });

    android::updateCommandInfo(slot, { RIL_REQUEST_GET_ALLOWED_NETWORK_TYPES_BITMAP, network::GetAllowedNetworkTypesBitmapResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS, network::GetSystemSelectionChannelsResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_IS_NR_DUAL_CONNECTIVITY_ENABLED, network::IsNrDualConnectivityEnabledResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_ALLOWED_NETWORK_TYPES_BITMAP, network::SetAllowedNetworkTypesBitmapResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_ENABLE_NR_DUAL_CONNECTIVITY, network::SetNrDualConnectivityStateResponse(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_GET_CELL_INFO_LIST, HAL_VERSION_CODE(1,6)),
            network::GetCellInfoListResponse_1_6(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_DATA_REGISTRATION_STATE, HAL_VERSION_CODE(1,6)),
            network::GetDataRegistrationStateResponse_1_6(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_SIGNAL_STRENGTH, HAL_VERSION_CODE(1,6)),
            network::GetSignalStrengthResponse_1_6(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_VOICE_REGISTRATION_STATE, HAL_VERSION_CODE(1,6)),
            network::GetVoiceRegistrationStateResponse_1_6(hidlResponse) });
}

void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioNetworkIndication>& indication) {
    // HIDL response hook
    auto hidlResponse = indication->hidl();
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_CDMA_PRL_CHANGED, network::CdmaPrlChangedInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED, network::ImsNetworkStateChangedInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, network::NetworkStateChangedInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESTRICTED_STATE_CHANGED, network::RestrictedStateChangedInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_SUPP_SVC_NOTIFICATION, network::SuppSvcNotifyInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_VOICE_RADIO_TECH_CHANGED, network::VoiceRadioTechChangedInd(hidlResponse), android::WAKE_PARTIAL });

    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_BARRING_INFO_CHANGED, network::BarringInfoChangedInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_REGISTRATION_FAILED,
            network::RegistrationFailedInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_LCEDATA_RECV,
            network::CurrentLinkCapacityEstimate_1_6(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_CELL_INFO_LIST,
            network::CellInfoListInd_1_6(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG,
            network::CurrentPhysicalChannelConfigs_1_6(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_SIGNAL_STRENGTH, network::CurrentSignalStrengthInd_1_6(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_NETWORK_SCAN_RESULT,
            network::NetworkScanResultInd_1_6(hidlResponse), android::WAKE_PARTIAL });
}
