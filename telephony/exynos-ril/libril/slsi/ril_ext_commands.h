/* //device/libs/telephony/ril_ext_commands.h
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
    {RIL_REQUEST_QUERY_COLP, vendor::radio::queryCOLPResponse},
    {RIL_REQUEST_QUERY_COLR, vendor::radio::queryCOLRResponse},
    {RIL_REQUEST_SIM_GET_ATR, vendor::radio::iccGetAtrResponse},
    {RIL_REQUEST_SEND_ENCODED_USSD, vendor::radio::sendUSSDWithDcsResponse},
    {RIL_REQUEST_SET_DS_NETWORK_TYPE, vendor::radio::setDsNetworkTypeResponse},
    {RIL_LOCAL_REQUEST_VSIM_NOTIFICATION, vendor::radio::sendVsimNotificationResponse},
    {RIL_LOCAL_REQUEST_VSIM_OPERATION, vendor::radio::sendVsimOperationResponse},
    {RIL_REQUEST_SET_EMERGENCY_CALL_STATUS, vendor::radio::setEmcStatusResponse},
    {RIL_REQUEST_SET_FEMTO_CELL_SRCH, vendor::radio::setFemtoCellSearchResponse},
    {RIL_REQUEST_SET_CDMA_HYBRID_MODE, vendor::radio::setCdmaHybridModeResponse},
    {RIL_REQUEST_GET_CDMA_HYBRID_MODE, vendor::radio::getCdmaHybridModeResponse},
    {RIL_REQUEST_SET_VOICE_OPERATION, vendor::radio::setVoiceOperationResponse}, // 30
    {RIL_REQUEST_SET_DUAL_NETWORK_AND_ALLOW_DATA, vendor::radio::setDualNetworkTypeAndAllowDataResponse},
    {RIL_REQUEST_QUERY_BPLMN_SEARCH, vendor::radio::queryBplmnSearchResponse},
    {RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL_WITH_RAT, vendor::radio::setNetworkSelectionModeManualResponse},
    {RIL_REQUEST_DIAL_WITH_CALL_TYPE, vendor::radio::dialWithCallTypeResponse},
    {RIL_REQUEST_CHANGE_BARRING_PASSWORD_OVER_MMI, vendor::radio::setBarringPasswordOverMmiResponse},
    {RIL_REQUEST_DEACTIVATE_DATA_CALL_WITH_REASON, vendor::radio::deactivateDataCallWithReasonResponse},
    {RIL_REQUEST_GET_SIM_LOCK_STATUS, vendor::radio::getSimLockStatusResponse},
    {RIL_REQUEST_SET_ACTIVATE_VSIM, vendor::radio::setActivateVsimResponse},
    {RIL_REQUEST_SET_ENDC_MODE, vendor::radio::setEndcModeResponse},
    {RIL_REQUEST_GET_ENDC_MODE, vendor::radio::getEndcModeResponse},
    {RIL_REQUEST_GET_SMS_STORAGE_ON_SIM, vendor::radio::getSmsStorageOnSimResponse},
    {RIL_REQUEST_SET_NR_MODE, vendor::radio::setNrModeResponse},
    {RIL_REQUEST_GET_NR_MODE, vendor::radio::getNrModeResponse},