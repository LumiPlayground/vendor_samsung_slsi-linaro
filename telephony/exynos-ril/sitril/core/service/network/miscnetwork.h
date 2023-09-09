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
 * miscnetwork.h
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#ifndef __MISC_NETWORK_H__
#define __MISC_NETWORK_H__

#include "servicecallback.h"
#include "servicehandlerbase.h"

/**
 * CancelAvailableNetworkstHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CancelAvailableNetworkstHandler)

/**
 * SetPreferredCallCapabilityHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetPreferredCallCapabilityHandler)

/**
 * GetPreferredCallCapabilityHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetPreferredCallCapabilityHandler)

/**
 * SetVoiceOperationHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetVoiceOperationHandler)

/**
 * GetActivityInfotHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetActivityInfotHandler)

/**
 * LceStartHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(LceStartHandler)

/**
 * LceStopHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(LceStopHandler)

/**
 * LcePullDataHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(LcePullDataHandler)

/**
 * LceDataRecvHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(LceDataRecvHandler)

/**
 * GetNeighboringCellIdsHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetNeighboringCellIdsHandler)

/**
 * SetLocationUpdatesHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetLocationUpdatesHandler)

/**
 * SetSuppSvcNotificationHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSuppSvcNotificationHandler)

/**
 * EnableModemHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(EnableModemHandler)

/**
 * GetModemStackStatusHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetModemStackStatusHandler)

/**
 * NitzTimeHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(NitzTimeHandler)

/**
 * GetNitzTimeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetNitzTimeHandler)

/**
 * ImsSupportServiceHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ImsSupportServiceHandler)

/**
 * UnsolSimFileInfoHandler
 */
class UnsolSimFileInfoHandler : public ServiceHandlerBase,
        public ServiceCallback::SimStatusListener {
    DECLARE_TINY_UNSOL_SERVICE_HANDLER_INIT(UnsolSimFileInfoHandler)
protected:
    void OnCreate();
    void OnDestroy();
public:
    void OnSimStatusChanged(int cardState, int appState);
};

/**
 * SetDataThrottlingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetDataThrottlingHandler)

/**
 * SetVoNREnabledHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetVoNREnabledHandler)

/**
 * GetVoNREnabledHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetVoNREnabledHandler)


/**
 * GetSystemSelectionChannelsHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetSystemSelectionChannelsHandler)

/**
 * SetUsageSettingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetUsageSettingHandler)

/**
 * GetUsageSettingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetUsageSettingHandler)

#endif /* __MISC_NETWORK_H__ */
