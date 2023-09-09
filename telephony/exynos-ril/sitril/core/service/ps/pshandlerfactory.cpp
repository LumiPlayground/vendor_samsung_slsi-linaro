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
 * pshandlerfactory.cpp
 *
 *  Created on: 2020. 8. 26.
 *      Author: juneho.kwon
 */

#include "pshandlerfactory.h"
#include "pshandler.h"
#include "messageid.h"
#include "service.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(pshandlerfactory)

BEGIN_FACTORY_ENTRY(pshandlerfactory)
    // Request and SolicitedResponse Handler
    ADD_FACTORY_ENTRY(GetDataCallListHandler, MSG_PS_GET_DATA_CALL_LIST);
	ADD_FACTORY_ENTRY(DeactDataCallHandler, MSG_PS_DEACT_DATA_CALL);
	ADD_FACTORY_ENTRY(DetachHandler, MSG_PS_DETACH);
	ADD_FACTORY_ENTRY(SetPreferredDataModemHandler, MSG_PS_SET_PREFERRED_DATA_MODEM);
	ADD_FACTORY_ENTRY(SetDataProfileHandler, MSG_PS_SET_DATA_PROFILE);
//	ADD_FACTORY_ENTRY(SetPdnIpAddressHandler, MSG_PS_SET_PDN_IP_ADDRESS);
    ADD_FACTORY_ENTRY(AllocatePduSessionIdHandler, MSG_PS_ALLOC_PDU_SESSION);
    ADD_FACTORY_ENTRY(ReleasePduSessionIdHandler, MSG_PS_REL_PDU_SESSION);
    ADD_FACTORY_ENTRY(StartHandoverHandler, MSG_PS_START_HANDOVER);
    ADD_FACTORY_ENTRY(CancelHandoverHandler, MSG_PS_CANCEL_HANDOVER);
    ADD_FACTORY_ENTRY(StartKeepaliveHandler, MSG_PS_START_KEEPALIVE);
    ADD_FACTORY_ENTRY(StopKeepaliveHandler, MSG_PS_STOP_KEEPALIVE);
    ADD_FACTORY_ENTRY(CurrentRrcInfoHandler, MSG_PS_RRC_INFO);

    // UnsolicitedResponse Handler
    ADD_FACTORY_ENTRY(DataCallListChangedHandler, MSG_PS_DATA_CALL_LIST_CHANGED);
    ADD_FACTORY_ENTRY(IpAddrStatusHandler, MSG_PS_IND_ADDR_STATUS);
    ADD_FACTORY_ENTRY(UnthrottleApnHandler, MSG_PS_IND_UNTHROTTLE_APN);
    ADD_FACTORY_ENTRY(NasTimerStatusChangedHandler, MSG_PS_NAS_TIMER_STATUS_CHANGED);
    ADD_FACTORY_ENTRY(PsDataOffStatusHandler, MSG_IND_PS_DATA_OFF_STATUS);
    ADD_FACTORY_ENTRY(KeepaliveStatusHandler, MSG_PS_IND_KEEPALIVE_STATUS);
    ADD_FACTORY_ENTRY(PsDedicatedBearerInfoUpdatedHandler, MSG_PS_DEDICATED_BEARER_INFO_UPDATED);
    ADD_FACTORY_ENTRY(PcoDataHandler, MSG_PS_IND_PCO_DATA);
    ADD_FACTORY_ENTRY(PsSlicingConfigUpdatedHandler, MSG_PS_SLICING_CONFIG_UPDATED);

    // Misc Handler

END_FACTORY_ENTRY()

