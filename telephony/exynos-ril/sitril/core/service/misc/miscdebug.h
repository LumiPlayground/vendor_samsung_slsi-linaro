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
 * miscdebug.h
 *
 *  Created on: 2020. 7. 8.
 *      Author: sungwoo48.choi
 */

#ifndef __MISC_DEBUG_H__
#define __MISC_DEBUG_H__

#include "servicehandlerbase.h"

/**
 * SetEngModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetEngModeHandler)

/**
 * SetScreenLineHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetScreenLineHandler)

/**
 * SetEngStringInputHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetEngStringInputHandler)

/**
 * EngModeDisplayIndHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(EngModeDisplayIndHandler)

/**
 * DebugTraceHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DebugTraceHandler)

/**
 * SendATCommandHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SendATCommandHandler)

/**
 * UnsolATCommandHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolATCommandHandler)

/**
 * PhoneResetHandler
 */
class PhoneResetHandler : public ServiceHandlerBase {
    DECLARE_TINY_UNSOL_SERVICE_HANDLER_INIT(PhoneResetHandler)
private:
    void RequestDeviceReset(int reset_type);
};

/**
 * DummyRequestHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DummyRequestHandler)

/**
 * TestUnsolHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(TestUnsolHandler)

/**
 * SetTelephonyStatusLoggingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetTelephonyStatusLoggingHandler)

#endif /* __MISC_DEBUG_H__ */
