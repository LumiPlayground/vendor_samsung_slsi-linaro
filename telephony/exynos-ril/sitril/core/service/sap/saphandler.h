/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __SAP_HANDLER_H__
#define __SAP_HANDLER_H__

#include "servicehandlerbase.h"

/**
 * SapConnectHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SapConnectHandler)

/**
 * SapDisconnectHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SapDisconnectHandler)

/**
 * SapApduHandler
 */
class SapApduHandler : public ServiceHandlerBase {
    DECLARE_TINY_SERVICE_HANDLER_INIT(SapApduHandler)
private:
    int mApduType;
};

/**
 * SapTransferAtrHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SapTransferAtrHandler)

/**
 * SapPowerHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SapPowerHandler)

/**
 * SapResetSimHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SapResetSimHandler)

/**
 * SapTransferCardReaderStatusHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SapTransferCardReaderStatusHandler)

/**
 * SapSetTransferProtocolHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SapSetTransferProtocolHandler)

/**
 * SapDisconnectIndHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(SapDisconnectIndHandler)

/**
 * SapStatusIndHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(SapStatusIndHandler)

#endif /* __SAP_HANDLER_H__ */
