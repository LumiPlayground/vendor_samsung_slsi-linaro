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
 * radioconfighandler.h
 *
 *  Created on: 2020. 7. 13.
 *      Author: sungwoo48.choi
 */

#ifndef __RADIO_CONFIG_HANDLER_H__
#define __RADIO_CONFIG_HANDLER_H__

#include "servicehandlerbase.h"

/**
 * GetPhoneCapabilityHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetPhoneCapabilityHandler)

/**
 * SetModemConfigHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetModemConfigHandler)

/**
 * SimSlotStatusHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SimSlotStatusHandler)

/**
 * SimSlotStatusChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(SimSlotStatusChangedHandler)

/**
 * SimSlotMappingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SimSlotMappingHandler)

#endif /* __RADIO_CONFIG_HANDLER_H__ */
