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
 * miscnetworkhandlerfactory.cpp
 *
 *  Created on: 2020. 7. 6.
 *      Author: sungwoo48.choi
 */

#include "miscnetworkhandlerfactory.h"
#include "cdmainfo.h"
#include "messageid.h"
#include "miscnetwork.h"
#include "oem_miscnetwork.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(MiscNetworkHandlerFactory)

BEGIN_FACTORY_ENTRY(MiscNetworkHandlerFactory)
    // miscnetwork
    ADD_FACTORY_ENTRY(CancelAvailableNetworkstHandler, MSG_MISC_OEM_CANCEL_AVAILABLE_NETWORKS)
    ADD_FACTORY_ENTRY(SetPreferredCallCapabilityHandler, MSG_MISC_OEM_SET_PREFERRED_CALL_CAPABILITY)
    ADD_FACTORY_ENTRY(GetPreferredCallCapabilityHandler, MSG_MISC_OEM_GET_PREFERRED_CALL_CAPABILITY)
    ADD_FACTORY_ENTRY(SetVoiceOperationHandler, MSG_MISC_SET_VOICE_OPERATION)
    ADD_FACTORY_ENTRY(GetActivityInfotHandler, MSG_MISC_GET_ACTIVITY_INFO)
    ADD_FACTORY_ENTRY(LceStartHandler, MSG_MISC_LCE_START)
    ADD_FACTORY_ENTRY(LceStopHandler, MSG_MISC_LCE_STOP)
    ADD_FACTORY_ENTRY(LcePullDataHandler, MSG_MISC_PULL_LCEDATA)
    ADD_FACTORY_ENTRY(LceDataRecvHandler, MSG_MISC_IND_CURRENT_LINK_CAPACITY_ESTIMATE)
    ADD_FACTORY_ENTRY(GetNeighboringCellIdsHandler, MSG_MISC_GET_NEIGHBORING_CELL_IDS)
    ADD_FACTORY_ENTRY(SetLocationUpdatesHandler, MSG_MISC_SET_LOCATION_UPDATES)
    ADD_FACTORY_ENTRY(SetSuppSvcNotificationHandler, MSG_MISC_SET_SUPP_SVC_NOTIFICATION)
    ADD_FACTORY_ENTRY(EnableModemHandler, MSG_MISC_ENABLE_MODEM)
    ADD_FACTORY_ENTRY(GetModemStackStatusHandler, MSG_MISC_GET_MODEM_STACK_STATUS)
    ADD_FACTORY_ENTRY(NitzTimeHandler, MSG_MISC_UNSOL_NITZ_TIME_RECEIVED)
    ADD_FACTORY_ENTRY(GetNitzTimeHandler, MSG_MISC_GET_NITZ_TIME)
    ADD_FACTORY_ENTRY(ImsSupportServiceHandler, MSG_NET_OEM_GET_IMS_SUPPORT_SERVICE)
    ADD_FACTORY_ENTRY(UnsolSimFileInfoHandler, MSG_NET_IND_SIM_FILE_INFO)
    ADD_FACTORY_ENTRY(SetDataThrottlingHandler, MSG_MISC_SET_DATA_THROTTLING)
    ADD_FACTORY_ENTRY(GetSystemSelectionChannelsHandler, MSG_NET_GET_SYSTEM_SELECTION_CHANNELS)
    ADD_FACTORY_ENTRY(SetVoNREnabledHandler, MSG_NET_SET_VONR_ENABLED)
    ADD_FACTORY_ENTRY(GetVoNREnabledHandler, MSG_NET_GET_VONR_ENABLED)
    ADD_FACTORY_ENTRY(SetUsageSettingHandler, MSG_NET_SET_USAGE_SETTING)
    ADD_FACTORY_ENTRY(GetUsageSettingHandler, MSG_NET_GET_USAGE_SETTING)

    // cdmainfo
    ADD_FACTORY_ENTRY(GetCdmaSubscriptionSourceHandler, MSG_MISC_CDMA_GET_SUBSCRIPT_SOURCE)
    ADD_FACTORY_ENTRY(SetCdmaSubscriptionSourceHandler, MSG_MISC_CDMA_SET_SUBSCRIPT_SOURCE)
    ADD_FACTORY_ENTRY(CdmaSubscriptionHandler, MSG_MISC_GET_CDMA_SUBSCRIPTION)
    ADD_FACTORY_ENTRY(CdmaPrlChangedHandler, MSG_MISC_IND_CDMA_PRL_CHANGED)
    ADD_FACTORY_ENTRY(SetCdmaRoamingHandler, MSG_NET_CDMA_SET_ROAMING)
    ADD_FACTORY_ENTRY(QueryCdmaRoamingHandler, MSG_NET_CDMA_QUERY_ROAMING)
    ADD_FACTORY_ENTRY(SetCdmaHybridModeHandler, MSG_NET_SET_CDMA_HYBRID_MODE)
    ADD_FACTORY_ENTRY(GetCdmaHybridModeHandler, MSG_NET_GET_CDMA_HYBRID_MODE)
    ADD_FACTORY_ENTRY(CdmaBurstDtmfHandler, MSG_CS_CDMA_BURST_DTMF)
    ADD_FACTORY_ENTRY(CdmaSetPreferredVoicePrivacyModeHandler, MSG_CS_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE)
    ADD_FACTORY_ENTRY(CdmaQueryPreferredVoicePrivacyModeHandler, MSG_CS_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE)
    ADD_FACTORY_ENTRY(UnsolCdmaInfoRecHandler, MSG_CS_IND_CDMA_INFO_REC)
    ADD_FACTORY_ENTRY(UnsolCdmaCallWaitingHandler, MSG_CS_SS_IND_CDMA_CALL_WAITING)
    ADD_FACTORY_ENTRY(CdmaFlashHandler, MSG_CS_SS_CDMA_FLASH)
    ADD_FACTORY_ENTRY(UnsolCdmaOtaProvisionStatusHandler, MSG_CS_IND_CDMA_OTA_PROVISION_STATUS)

    // oem_miscnetwork
    ADD_FACTORY_ENTRY(GetManualRatModeHandler, MSG_NET_OEM_GET_MANUAL_RAT_MODE)
    ADD_FACTORY_ENTRY(SetManualRatModeHandler, MSG_NET_OEM_SET_MANUAL_RAT_MODE)
    ADD_FACTORY_ENTRY(GetFrequencyLockHandler, MSG_NET_OEM_GET_FREQUENCY_LOCK)
    ADD_FACTORY_ENTRY(SetFrequencyLockHandler, MSG_NET_OEM_SET_FREQUENCY_LOCK)
    ADD_FACTORY_ENTRY(GetFrequencyInfoHandler, MSG_NET_GET_FREQUENCY_INFO)
    ADD_FACTORY_ENTRY(UnsolFrequencyInfoHandler, MSG_NET_IND_FREQUENCY_INFO)
    ADD_FACTORY_ENTRY(B2B1ConfigInfoHandler, MSG_NET_IND_B2_B1_CONFIG)
    ADD_FACTORY_ENTRY(AcBarringInfoHandler, MSG_NET_IND_AC_BARRING_INFO)
END_FACTORY_ENTRY()
