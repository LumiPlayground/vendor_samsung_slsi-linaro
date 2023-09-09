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
 * radiopower.h
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#ifndef __RADIO_POWER_H__
#define __RADIO_POWER_H__

#include "servicehandlerbase.h"

/**
 * SetRadioPowerHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetRadioPowerHandler)

/**
 * GetRadioStateHandler
 */
class GetRadioStateHandler : public ServiceHandlerBase {
    DECLARE_TINY_SERVICE_HANDLER_INIT(GetRadioStateHandler)
protected:
    int OnTimeout(Message *msg);
};

/**
 * RadioStateChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(RadioStateChangedHandler)

/**
 * ShutdownHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ShutdownHandler)

#endif /* __RADIO_POWER_H__ */
