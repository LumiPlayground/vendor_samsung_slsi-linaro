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
 * cdmainfo.h
 *
 *  Created on: 2020. 7. 13.
 *      Author: sungwoo48.choi
 */

#ifndef __CDMA_INFO_H__
#define __CDMA_INFO_H__

#include "servicehandlerbase.h"

/**
 * GetCdmaSubscriptionSourceHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetCdmaSubscriptionSourceHandler)

/**
 * SetCdmaSubscriptionSourceHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCdmaSubscriptionSourceHandler)

/**
 * CdmaSubscriptionHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CdmaSubscriptionHandler)

/**
 * CdmaPrlChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(CdmaPrlChangedHandler)

/**
 * SetCdmaRoamingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCdmaRoamingHandler)

/**
 * QueryCdmaRoamingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(QueryCdmaRoamingHandler)

/**
 * SetCdmaHybridModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCdmaHybridModeHandler)

/**
 * GetCdmaHybridModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetCdmaHybridModeHandler)

/**
 * CdmaBurstDtmfHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CdmaBurstDtmfHandler)

/**
 * CdmaSetPreferredVoicePrivacyModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CdmaSetPreferredVoicePrivacyModeHandler)

/**
 * CdmaQueryPreferredVoicePrivacyModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CdmaQueryPreferredVoicePrivacyModeHandler)

/**
 * UnsolCdmaInfoRecHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolCdmaInfoRecHandler)

/**
 * UnsolCdmaCallWaitingHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolCdmaCallWaitingHandler)

/**
 * CdmaFlashHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CdmaFlashHandler)

/**
 * UnsolCdmaOtaProvisionStatusHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolCdmaOtaProvisionStatusHandler)
#endif /* __CDMA_INFO_H__ */
