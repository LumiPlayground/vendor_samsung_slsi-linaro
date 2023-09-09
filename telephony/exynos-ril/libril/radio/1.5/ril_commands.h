/* //device/libs/telephony/ril_commands.h
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
    {RIL_REQUEST_RADIO_POWER, radio::setRadioPowerResponse_1_5},
    {RIL_REQUEST_VOICE_REGISTRATION_STATE, radio::getVoiceRegistrationStateResponse_1_5},
    {RIL_REQUEST_DATA_REGISTRATION_STATE, radio::getDataRegistrationStateResponse_1_5},
    {RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, radio::setSignalStrengthReportingCriteriaResponse_1_5},
    {RIL_REQUEST_ENABLE_UICC_APPLICATIONS, radio::enableUiccApplicationsResponse},
    {RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT, radio::areUiccApplicationsEnabledResponse},
    {RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, radio::setSystemSelectionChannelsResponse_1_5},
    {RIL_REQUEST_START_NETWORK_SCAN, radio::startNetworkScanResponse_1_5},
    {RIL_REQUEST_SETUP_DATA_CALL, radio::setupDataCallResponse_1_5},
    {RIL_REQUEST_SET_INITIAL_ATTACH_APN, radio::setInitialAttachApnResponse_1_5},
    {RIL_REQUEST_SET_DATA_PROFILE, radio::setDataProfileResponse_1_5},
    {RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER, radio::setIndicationFilterResponse_1_5},
    {RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, radio::setNetworkSelectionModeManualResponse_1_5},
    {RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA, radio::setLinkCapacityReportingCriteriaResponse_1_5},
    {RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE, radio::sendCdmaSmsExpectMoreResponse},
    {RIL_REQUEST_GET_BARRING_INFO, radio::getBarringInfoResponse},
    {RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION, radio::supplySimDepersonalizationResponse},
    {RIL_REQUEST_GET_CELL_INFO_LIST, radio::getCellInfoListResponse_1_5},
    {RIL_REQUEST_GET_SIM_STATUS, radio::getIccCardStatusResponse_1_5},

