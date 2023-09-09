/* vendor/samsung_slsi/exynos-ril/libril/ril_ext_unsol_commands.h
**
** Copyright 2008, The Android Open Source Project
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

/*
** extension unsolicited id
*/
    {RIL_UNSOL_SUPP_SVC_RETURN_RESULT, vendor::radio::suppSvcReturnResult, android::WAKE_PARTIAL},
    {RIL_UNSOL_CALL_PRESENT_IND, vendor::radio::callPresentInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_WB_AMR_REPORT_IND, vendor::radio::wbAmrReportInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_VSIM_OPERATION_INDICATION, vendor::radio::vsimOperationInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_NAS_TIMER_STATUS_IND, vendor::radio::nasTimerStatusInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_EMERGENCY_ACT_INFO, vendor::radio::emergencyActInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_ICCID_INFO, vendor::radio::iccIdInfoInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_ON_USSD_WITH_DCS, vendor::radio::onUssdWithDcsInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_VOLTE_AVAILABLE_INFO, vendor::radio::volteAvailableInfoInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_EMERGENCY_SUPPORT_RAT_MODE, vendor::radio::emergencySupportRatModeInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_USSD_CANCELED, vendor::radio::ussdCanceledInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_NR_PHYSICAL_CHANNEL_CONFIGS, vendor::radio::currentPhysicalChannelConfigInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_IND_ENDC_CAPABILITY, vendor::radio::endcCapabilityInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_IND_SIM_TRAY_STATUS_CHANGED, vendor::radio::simTrayStatusChangedInd, android::WAKE_PARTIAL},
    {RIL_UNSOL_UNTHROTTLE_APN, vendor::radio::unthrottleApnInd, android::WAKE_PARTIAL},
