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
 * uicchandlerfactory.cpp
 *
 *  Created on: 2020. 7. 14.
 *      Author: sungwoo48.choi
 */
#include "messageid.h"
#include "uicchandler_misc.h"
#include "uicchandler_oem.h"
#include "uicchandler_secureelement.h"
#include "uicchandler_bow.h"
#include "uicchandlerfactory.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(UiccHandlerFactory)

BEGIN_FACTORY_ENTRY(UiccHandlerFactory)
    ADD_FACTORY_ENTRY(EnableUiccAppHandler, MSG_SIM_ENABLE_UICC_APP)
    ADD_FACTORY_ENTRY(GetUiccAppEnablementHandler, MSG_SIM_GET_UICC_APP_ENABLEMENT)
    ADD_FACTORY_ENTRY(SetUiccHandler, MSG_SIM_SET_UICC)
    ADD_FACTORY_ENTRY(GetFacilityLockHandler, MSG_SIM_GET_FACILITY_LOCK)
    ADD_FACTORY_ENTRY(GetIsimAuthHandler, MSG_SIM_GET_ISIM_AUTH)
    ADD_FACTORY_ENTRY(GetSimAuthHandler, MSG_SIM_GET_SIM_AUTH)
    ADD_FACTORY_ENTRY(GetAtrHandler, MSG_SIM_GET_ATR)
    ADD_FACTORY_ENTRY(OpenSimChannelHandler, MSG_SIM_OPEN_CHANNEL)
    ADD_FACTORY_ENTRY(CloseSimChannelHandler, MSG_SIM_CLOSE_CHANNEL)
    ADD_FACTORY_ENTRY(TransmitSimApduChannelHandler, MSG_SIM_TRANSMIT_APDU_CHANNEL)
    ADD_FACTORY_ENTRY(TransmitSimApduBasicHandler, MSG_SIM_TRANSMIT_APDU_BASIC)
    ADD_FACTORY_ENTRY(SetCarrierRestrictionsHandler, MSG_SIM_SET_CARRIER_RESTRICTIONS)
    ADD_FACTORY_ENTRY(GetCarrierRestrictionsHandler, MSG_SIM_GET_CARRIER_RESTRICTIONS)
    ADD_FACTORY_ENTRY(SetSimCardPowerHandler, MSG_SIM_SET_SIM_CARD_POWER)
    ADD_FACTORY_ENTRY(UiccSubscriptionStatusChanged, MSG_SIM_IND_UICC_SUBSCRIPTION_STATUS_CHANGED)
    ADD_FACTORY_ENTRY(SimStatusChangedHandler, MSG_SIM_STATUS_CHANGED)
    ADD_FACTORY_ENTRY(SimDepersonalizationHandler, MSG_SIM_DEPERSONALIZATION)
    ADD_FACTORY_ENTRY(SimLockInfoHandler, MSG_SIM_GET_SIM_LOCK_INFO)
    ADD_FACTORY_ENTRY(SeOpenChannelHandler, MSG_SIM_OEM_OPEN_CHANNEL)
    ADD_FACTORY_ENTRY(SeTransmitApduLogicalHandler, MSG_SIM_OEM_TRANSMIT_APDU_LOGICAL)
    ADD_FACTORY_ENTRY(GetUiccCardPresentHandler, MSG_SIM_OEM_GET_CARD_PRESENT)
    ADD_FACTORY_ENTRY(SeResetHandler, MSG_SIM_OEM_SE_RESET)
    ADD_FACTORY_ENTRY(IccDepersonalizationHandler, MSG_SIM_OEM_ICC_DEPERSONALIZATION)

    // BOWHANDLER
    ADD_FACTORY_ENTRY(WifiChannelStatusReportHandler, MSG_SAT_WIFI_CHANNEL_STATUS_REPORT)
    ADD_FACTORY_ENTRY(WifiStatusResponseHandler, MSG_SAT_WIFI_STATUS_RESPONSE)
    ADD_FACTORY_ENTRY(OpenChannelResponseHandler, MSG_SAT_WIFI_OPEN_CHANNEL_RESPONSE)
    ADD_FACTORY_ENTRY(SendDataResponseHandler, MSG_SAT_WIFI_SEND_DATA_RESPONSE)
    ADD_FACTORY_ENTRY(ReceiveDataResponseHandler, MSG_SAT_WIFI_RECEIVE_DATA_RESPONSE)
    ADD_FACTORY_ENTRY(CloseChannelResponseHandler, MSG_SAT_WIFI_CLOSE_CHANNEL_RESPONSE)
    ADD_FACTORY_ENTRY(WiFiStatusRequestHandler, MSG_SAT_IND_WIFI_STATUS_REQUEST)
    ADD_FACTORY_ENTRY(OpenChannelRequestHandler, MSG_SAT_IND_WIFI_OPEN_CHANNEL_REQUEST)
    ADD_FACTORY_ENTRY(SendDataRequestHandler, MSG_SAT_IND_WIFI_SEND_DATA_REQUEST)
    ADD_FACTORY_ENTRY(ReceiveDataRequestHandler, MSG_SAT_IND_WIFI_RECEIVE_DATA_REQUEST)
    ADD_FACTORY_ENTRY(CloseChannelRequestHandler, MSG_SAT_IND_WIFI_CLOSE_CHANNEL_REQUEST)

    ADD_FACTORY_ENTRY(SimTrayStatusChangedHandler, MSG_IND_SIM_TRAY_STATUS)
END_FACTORY_ENTRY()
