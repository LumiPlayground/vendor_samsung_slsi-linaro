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
  * datahandler.h
  *
  *  Created on: 2020. 8. 19.
  *      Author: juneho.kwon
  */


#ifndef __DATA_HANDLER_H__
#define __DATA_HANDLER_H__

#include "servicehandlerbase.h"

/**
 * GetDataCallListHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetDataCallListHandler)

/**
 * DetachHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DetachHandler)

/**
 * SetPreferredDataModemHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetPreferredDataModemHandler)

/**
 * SetDataProfileHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetDataProfileHandler)

/**
 * SetPdnIpAddressHandler
 */
//DECLARE_TINY_SERVICE_HANDLER_CLASS(SetPdnIpAddressHandler)

/**
 * DeactDataCallHandler
 */
class DeactDataCallHandler : public ServiceHandlerBase {
    DECLARE_TINY_SERVICE_HANDLER_INIT(DeactDataCallHandler)
protected:
    int OnTimeout(Message *msg);
};

/**
 * SetUrspHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetUrspHandler)


/**
 * DataCallListChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(DataCallListChangedHandler)

/**
 * IpAddrStatusHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(IpAddrStatusHandler)

/**
 * NasTimerStatusChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(NasTimerStatusChangedHandler)

/**
 * PcoDataHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(PcoDataHandler)

/**
 * DedicatedBearerInfoUpdatedLegacyHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(DedicatedBearerInfoUpdatedLegacyHandler)

/**
 * UnthrottleApnHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnthrottleApnHandler)

/**
 * AllocatePduSessionIdHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(AllocatePduSessionIdHandler)

/**
 * ReleasePduSessionIdHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ReleasePduSessionIdHandler)

/**
 * StartHandoverHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(StartHandoverHandler)

/**
 * CancelHandoverHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CancelHandoverHandler)

/**
 * PsDataOffStatusHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(PsDataOffStatusHandler)

/**
 * StartKeepaliveHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(StartKeepaliveHandler)

/**
 * StopKeepaliveHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(StopKeepaliveHandler)

/**
 * KeepaliveStatusHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(KeepaliveStatusHandler)

/**
 * PsDedicatedBearerInfoUpdatedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(PsDedicatedBearerInfoUpdatedHandler)

/**
 * PsSlicingConfigUpdatedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(PsSlicingConfigUpdatedHandler)

/**
 * CurrentRrcInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CurrentRrcInfoHandler)

#endif /* __DATA_HANDLER_H__ */
