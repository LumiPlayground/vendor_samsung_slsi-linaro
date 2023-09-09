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
    {ENCODE_REQUEST(RIL_REQUEST_SIGNAL_STRENGTH, HAL_VERSION_CODE(1,6)), radio::getSignalStrengthResponse_1_6},
    {ENCODE_REQUEST(RIL_REQUEST_VOICE_REGISTRATION_STATE, HAL_VERSION_CODE(1,6)), radio::getVoiceRegistrationStateResponse_1_6},
    {ENCODE_REQUEST(RIL_REQUEST_DATA_REGISTRATION_STATE, HAL_VERSION_CODE(1,6)), radio::getDataRegistrationStateResponse_1_6},
    {RIL_REQUEST_RADIO_POWER, radio::setRadioPowerResponse_1_6},
    {RIL_REQUEST_SEND_SMS, radio::sendSmsResponse_1_6},
    {RIL_REQUEST_SEND_SMS_EXPECT_MORE, radio::sendSmsExpectMoreResponse_1_6},
    {ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1,6)), radio::setupDataCallResponse_1_6},
    {ENCODE_REQUEST(RIL_REQUEST_DATA_CALL_LIST, HAL_VERSION_CODE(1,6)), radio::getDataCallListResponse_1_6},
    {RIL_REQUEST_CDMA_SEND_SMS, radio::sendCdmaSmsResponse_1_6},
    {RIL_REQUEST_SET_SIM_CARD_POWER, radio::setSimCardPowerResponse_1_6},
    {RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION, radio::setCarrierInfoForImsiEncryptionResponse},
    {RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE, radio::sendCdmaSmsExpectMoreResponse_1_6},
    {RIL_REQUEST_ENABLE_NR_DUAL_CONNECTIVITY, radio::setNrDualConnectivityStateResponse},
    {RIL_REQUEST_IS_NR_DUAL_CONNECTIVITY_ENABLED, radio::isNrDualConnectivityEnabledResponse},
    {RIL_REQUEST_ALLOCATE_PDU_SESSION_ID, radio::allocatePduSessionIdResponse},
    {RIL_REQUEST_RELEASE_PDU_SESSION_ID, radio::releasePduSessionIdResponse},
    {RIL_REQUEST_START_HANDOVER, radio::startHandoverResponse},
    {RIL_REQUEST_CANCEL_HANDOVER, radio::cancelHandoverResponse},
    {RIL_REQUEST_SET_ALLOWED_NETWORK_TYPES_BITMAP, radio::setAllowedNetworkTypesBitmapResponse},
    {RIL_REQUEST_GET_ALLOWED_NETWORK_TYPES_BITMAP, radio::getAllowedNetworkTypesBitmapResponse},
    {RIL_REQUEST_SET_DATA_THROTTLING, radio::setDataThrottlingResponse},
    {RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS, radio::getSystemSelectionChannelsResponse},
    {ENCODE_REQUEST(RIL_REQUEST_GET_CURRENT_CALLS, HAL_VERSION_CODE(1,6)), radio::getCurrentCallsResponse_1_6},
    {ENCODE_REQUEST(RIL_REQUEST_GET_CELL_INFO_LIST, HAL_VERSION_CODE(1,6)), radio::getCellInfoListResponse_1_6},
    {RIL_REQUEST_EMERGENCY_DIAL, radio::emergencyDialResponse},
    {RIL_REQUEST_GET_SIM_PHONEBOOK_RECORDS, radio::getSimPhonebookRecordsResponse},
    {RIL_REQUEST_GET_SIM_PHONEBOOK_CAPACITY, radio::getSimPhonebookCapacityResponse},
    {RIL_REQUEST_UPDATE_SIM_PHONEBOOK_RECORDS, radio::updateSimPhonebookRecordsResponse},
    {ENCODE_REQUEST(RIL_REQUEST_SLICING_CONFIG, HAL_VERSION_CODE(1,6)), radio::getSlicingConfigResponse},

