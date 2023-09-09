/* //device/libs/telephony/ril_unsol_commands.h
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
    {RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, radio::radioStateChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, radio::callStateChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, radio::networkStateChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESPONSE_NEW_SMS, radio::newSmsInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT, radio::newSmsStatusReportInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM, radio::newSmsOnSimInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_ON_USSD, radio::onUssdInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_ON_USSD_REQUEST, radio::onUssdInd, android::DONT_WAKE},
    {RIL_UNSOL_NITZ_TIME_RECEIVED, radio::nitzTimeReceivedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_SIGNAL_STRENGTH, radio::currentSignalStrengthInd, android::DONT_WAKE},
    {RIL_UNSOL_DATA_CALL_LIST_CHANGED, radio::dataCallListChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_SUPP_SVC_NOTIFICATION, radio::suppSvcNotifyInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_STK_SESSION_END, radio::stkSessionEndInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_STK_PROACTIVE_COMMAND, radio::stkProactiveCommandInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_STK_EVENT_NOTIFY, radio::stkEventNotifyInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_STK_CALL_SETUP, radio::stkCallSetupInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_SIM_SMS_STORAGE_FULL, radio::simSmsStorageFullInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_SIM_REFRESH, radio::simRefreshInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_CALL_RING, radio::callRingInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, radio::simStatusChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESPONSE_CDMA_NEW_SMS, radio::cdmaNewSmsInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS, radio::newBroadcastSmsInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL, radio::cdmaRuimSmsStorageFullInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESTRICTED_STATE_CHANGED, radio::restrictedStateChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE, radio::enterEmergencyCallbackModeInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_CDMA_CALL_WAITING, radio::cdmaCallWaitingInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_CDMA_OTA_PROVISION_STATUS, radio::cdmaOtaProvisionStatusInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_CDMA_INFO_REC, radio::cdmaInfoRecInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_OEM_HOOK_RAW, NULL, android::WAKE_PARTIAL},
    {RIL_UNSOL_RINGBACK_TONE, radio::indicateRingbackToneInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESEND_INCALL_MUTE, radio::resendIncallMuteInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED, radio::cdmaSubscriptionSourceChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_CDMA_PRL_CHANGED, radio::cdmaPrlChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE, radio::exitEmergencyCallbackModeInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RIL_CONNECTED, radio::rilConnectedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_VOICE_RADIO_TECH_CHANGED, radio::voiceRadioTechChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_CELL_INFO_LIST, radio::cellInfoListInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED, radio::imsNetworkStateChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED, radio::subscriptionStatusChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_SRVCC_STATE_NOTIFY, radio::srvccStateNotifyInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_HARDWARE_CONFIG_CHANGED, radio::hardwareConfigChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_DC_RT_INFO_CHANGED, NULL, android::WAKE_PARTIAL},
    {RIL_UNSOL_RADIO_CAPABILITY, radio::radioCapabilityIndicationInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_ON_SS, radio::onSupplementaryServiceIndicationInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_STK_CC_ALPHA_NOTIFY, radio::stkCallControlAlphaNotifyInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_LCEDATA_RECV, radio::lceDataInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_PCO_DATA, radio::pcoDataInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_MODEM_RESTART, radio::modemResetInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION, radio::carrierInfoForImsiEncryption, android::WAKE_PARTIAL},
    {RIL_UNSOL_NETWORK_SCAN_RESULT, radio::networkScanResultInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_EMERGENCY_NUMBER_LIST, radio::currentEmergencyNumberListInd, android::WAKE_PARTIAL},
