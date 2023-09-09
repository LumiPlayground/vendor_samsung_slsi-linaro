/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * networkhandlerfactory.cpp
 *
 *  Created on: 2020. 7. 6.
 *      Author: sungwoo48.choi
 */

#include "networkhandlerfactory.h"
#include "cdmainfo.h"
#include "messageid.h"
#include "networkhandler.h"
#include "networkregistrationinfo.h"
#include "radiopower.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(NetworkHandlerFactory)

BEGIN_FACTORY_ENTRY(NetworkHandlerFactory)
    // networkregistrationinfo
    ADD_FACTORY_ENTRY(VoiceRegistrationStateHandler, MSG_NET_VOICE_REGISTRATION_STATE)
    ADD_FACTORY_ENTRY(DataRegistrationStateHandler, MSG_NET_DATA_REGISTRATION_STATE)
    ADD_FACTORY_ENTRY(NetworkStateChangedHandler, MSG_NET_NETWORK_STATE_CHANGED)
    ADD_FACTORY_ENTRY(OperatorHandler, MSG_NET_OPERATOR)

    // radiopower
    ADD_FACTORY_ENTRY(SetRadioPowerHandler, MSG_NET_RADIO_POWER)
    ADD_FACTORY_ENTRY(GetRadioStateHandler, MSG_NET_GET_RADIO_STATE)
    ADD_FACTORY_ENTRY(RadioStateChangedHandler, MSG_NET_RADIO_STATE_CHANGED)
    ADD_FACTORY_ENTRY(ShutdownHandler, MSG_NET_SHUTDOWN)

    // networkhandler
    ADD_FACTORY_ENTRY(GetVoiceRadioTechHandler, MSG_NET_VOICE_RADIO_TECH)
    ADD_FACTORY_ENTRY(SetBandModeHandler, MSG_NET_SET_BAND_MODE)
    ADD_FACTORY_ENTRY(QueryAvailableBandModeHandler, MSG_NET_QUERY_AVAILABLE_BAND_MODE)
    ADD_FACTORY_ENTRY(AllowDataModeHandler, MSG_NET_ALLOW_DATA)
    ADD_FACTORY_ENTRY(SetUnsolCellInfoListRateHandler, MSG_NET_SET_UNSOL_CELL_INFO_LIST_RATE)
    ADD_FACTORY_ENTRY(GetDuplexModeHandler, MSG_NET_GET_DUPLEX_MODE)
    ADD_FACTORY_ENTRY(SetDuplexModeHandler, MSG_NET_SET_DUPLEX_MODE)
    ADD_FACTORY_ENTRY(QueryNetworkSelectionModeHandler, MSG_NET_QUERY_NETWORK_SELECTION_MODE)
    ADD_FACTORY_ENTRY(SetNetworkSelectionAutoHandler, MSG_NET_SET_NETWORK_SELECTION_AUTO)
    ADD_FACTORY_ENTRY(GetCellInfoListHandler, MSG_NET_GET_CELL_INFO_LIST)
    ADD_FACTORY_ENTRY(CellInfoListReceivedHandler, MSG_NET_CELL_INFO_LIST_RECEIVED)
    ADD_FACTORY_ENTRY(SetDSNetworkTypeHandler, MSG_NET_SET_DS_NTW_TYPE)
    ADD_FACTORY_ENTRY(GetPreferredNetworkTypeHandler, MSG_NET_GET_PREF_NETWORK_TYPE)
    ADD_FACTORY_ENTRY(SetPreferredNetworkTypeHandler, MSG_NET_SET_PREF_NETWORK_TYPE)
    ADD_FACTORY_ENTRY(SetDualNetworkTypeAndAllowDataHandler, MSG_NET_SET_DUAL_NTW_AND_ALLOW_DATA)
    ADD_FACTORY_ENTRY(StartNetworkScanHandler, MSG_NET_START_NETWORK_SCAN)
    ADD_FACTORY_ENTRY(StopNetworkScanHandler, MSG_NET_STOP_NETWORK_SCAN)
    ADD_FACTORY_ENTRY(NetworkScanResultHandler, MSG_NET_IND_NETWORK_SCAN_RESULT)
    ADD_FACTORY_ENTRY(SetEndcModeHandler, MSG_NET_OEM_SET_ENDC_MODE)
    ADD_FACTORY_ENTRY(GetEndcModeHandler, MSG_NET_OEM_GET_ENDC_MODE)
    ADD_FACTORY_ENTRY(SetNrModeHandler, MSG_NET_SET_NR_MODE)
    ADD_FACTORY_ENTRY(GetNrModeHandler, MSG_NET_GET_NR_MODE)
    ADD_FACTORY_ENTRY(SetNrSilenceModeHandler, MSG_NET_SET_NR_SILENCE_MODE)
    ADD_FACTORY_ENTRY(GetNrSilenceModeHandler, MSG_NET_GET_NR_SILENCE_MODE)
    ADD_FACTORY_ENTRY(SetPsServiceHandler, MSG_NET_OEM_SET_PS_SERVICE)
    ADD_FACTORY_ENTRY(GetPsServiceHandler, MSG_NET_OEM_GET_PS_SERVICE)
    ADD_FACTORY_ENTRY(SetMicroCellSearchHandler, MSG_NET_SET_MC_SRCH)
    ADD_FACTORY_ENTRY(GetRadioCapabilityHandler, MSG_NET_GET_RC_NTW_TYPE)
    ADD_FACTORY_ENTRY(SetRadioCapabilityHandler, MSG_NET_SET_RC_NTW_TYPE)
    ADD_FACTORY_ENTRY(RadioCapabilityChangedHandler, MSG_NET_RC_INFO_RECV)
    ADD_FACTORY_ENTRY(TotalOostHandler, MSG_NET_IND_TOTAL_OOS)
    ADD_FACTORY_ENTRY(MccIndtHandler, MSG_NET_IND_MCC)
    ADD_FACTORY_ENTRY(SetSysSelChannelsHandler, MSG_NET_SET_SYSTEM_SELECTION_CHANNELS)
    ADD_FACTORY_ENTRY(GetBarringInfoHandler, MSG_NET_GET_BARRING_INFO)
    ADD_FACTORY_ENTRY(BarringInfoChangedHandler, MSG_NET_IND_BARRING_INFO)
    ADD_FACTORY_ENTRY(UnsolRegistrationFailedHandler, MSG_NET_IND_REG_FAILED)
    ADD_FACTORY_ENTRY(QueryEmergencyCallAvailableRadioTechHandler, MSG_NET_QUERY_AVAILABLE_EMERGENCY_CALL_STATUS)
    ADD_FACTORY_ENTRY(SetEmergencyCallStatusHandler, MSG_NET_SET_EMERGENCY_CALL_STATUS)
    ADD_FACTORY_ENTRY(EmergencyActInfoReceivedHandler, MSG_NET_EMERGENCY_ACT_INFO_RECEIVED)
    ADD_FACTORY_ENTRY(SetAllowedNetworkTypeBitmapHandler, MSG_NET_SET_ALLOWED_NETWORK)
    ADD_FACTORY_ENTRY(GetAllowedNetworkTypeBitmapHandler, MSG_NET_GET_ALLOWED_NETWORK)
    ADD_FACTORY_ENTRY(SetPlmnListHandler, MSG_NET_SET_PLMN_LIST)
    ADD_FACTORY_ENTRY(GetPlmnListHandler, MSG_NET_GET_PLMN_LIST)
    ADD_FACTORY_ENTRY(SetImsRegistrationStateHandler, MSG_NET_SET_IMS_REGISTRATION_STATE)
    ADD_FACTORY_ENTRY(SetMobileDataStateHandler, MSG_NET_SET_MOBILE_DATA_STATE)
    ADD_FACTORY_ENTRY(CurrentPhysicalChannelConfigsHandler, MSG_NET_PHYSICAL_CHANNEL_CONFIGS_RECV)
    ADD_FACTORY_ENTRY(DsdsStatusChangedHandler, MSG_NET_IND_DSDS_STATUS)
END_FACTORY_ENTRY()
