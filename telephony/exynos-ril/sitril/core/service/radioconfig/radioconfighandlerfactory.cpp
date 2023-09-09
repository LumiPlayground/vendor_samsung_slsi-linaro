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
 * radioconfighandlerfactory.cpp
 *
 *  Created on: 2020. 7. 13.
 *      Author: sungwoo48.choi
 */


#include "messageid.h"
#include "radioconfighandler.h"
#include "radioconfighandlerfactory.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(RadioConfigHandlerFactory)

BEGIN_FACTORY_ENTRY(RadioConfigHandlerFactory)
    ADD_FACTORY_ENTRY(GetPhoneCapabilityHandler, MSG_MISC_GET_PHONE_CAPABILITY)
    ADD_FACTORY_ENTRY(SetModemConfigHandler, MSG_MISC_SET_MODEMS_CONFIG)
    ADD_FACTORY_ENTRY(SimSlotStatusHandler, MSG_SIM_GET_SLOT_STATUS)
    ADD_FACTORY_ENTRY(SimSlotStatusChangedHandler, MSG_SIM_SLOT_STATUS_CHANGED)
    ADD_FACTORY_ENTRY(SimSlotMappingHandler, MSG_SIM_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING)
END_FACTORY_ENTRY()
