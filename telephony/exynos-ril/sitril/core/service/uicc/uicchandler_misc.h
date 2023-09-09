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
 * uicchandler.h
 *
 *  Created on: 2020. 7. 14.
 *      Author: sungwoo48.choi
 */

#ifndef __UICC_HANDLER_H__
#define __UICC_HANDLER_H__

#include "servicehandlerbase.h"

/**
 * EnableUiccAppHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(EnableUiccAppHandler)

/**
 * GetUiccAppEnablementHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetUiccAppEnablementHandler)

/**
 * SetUiccHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetUiccHandler)

/**
 * GetFacilityLockHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetFacilityLockHandler)

/**
 * GetIsimAuthHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetIsimAuthHandler)

/**
 * GetSimAuthHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetSimAuthHandler)

/**
 * GetAtrHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetAtrHandler)

/**
 * OpenSimChannelHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(OpenSimChannelHandler)

/**
 * CloseSimChannelHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CloseSimChannelHandler)

/**
 * TransmitSimApduChannelHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(TransmitSimApduChannelHandler)

/**
 * TransmitSimApduBasicHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(TransmitSimApduBasicHandler)

/**
 * SetCarrierRestrictionsHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCarrierRestrictionsHandler)

/**
 * GetCarrierRestrictionsHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetCarrierRestrictionsHandler)

/**
 * SetSimCardPowerHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSimCardPowerHandler)

/**
 * UiccSubscriptionStatusChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UiccSubscriptionStatusChanged)

/**
 * SimStatusChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(SimStatusChangedHandler)

/**
 * SimDepersonalizationHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SimDepersonalizationHandler)

/**
 * SimLockInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SimLockInfoHandler)

/**
 * SimTrayStatusChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(SimTrayStatusChangedHandler)

#endif /* __UICC_HANDLER_H__ */
