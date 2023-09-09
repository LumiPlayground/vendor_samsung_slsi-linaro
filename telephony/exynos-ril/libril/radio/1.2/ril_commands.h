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
    {RIL_REQUEST_GET_SIM_STATUS, radio::getIccCardStatusResponse_1_2},
    {RIL_REQUEST_VOICE_REGISTRATION_STATE, radio::getVoiceRegistrationStateResponse_1_2},
    {RIL_REQUEST_DATA_REGISTRATION_STATE, radio::getDataRegistrationStateResponse_1_2},
    {RIL_REQUEST_GET_CELL_INFO_LIST, radio::getCellInfoListResponse_1_2},
    {RIL_REQUEST_SIGNAL_STRENGTH, radio::getSignalStrengthResponse_1_2},
    {RIL_REQUEST_SETUP_DATA_CALL, radio::setupDataCallResponse},
    {RIL_REQUEST_DEACTIVATE_DATA_CALL, radio::deactivateDataCallResponse},
    {RIL_REQUEST_START_NETWORK_SCAN, radio::startNetworkScanResponse},
    {RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, radio::setSignalStrengthReportingCriteriaResponse},
    {RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA, radio::setLinkCapacityReportingCriteriaResponse},
