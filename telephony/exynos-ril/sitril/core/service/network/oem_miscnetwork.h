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
 * oem_miscnetwork.h
 *
 *  Created on: 2020. 7. 14.
 *      Author: sungwoo48.choi
 */

#ifndef __OEM_MISC_NETWORK_H__
#define __OEM_MISC_NETWORK_H__

#include "servicehandlerbase.h"

/**
 * GetManualRatModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetManualRatModeHandler)

/**
 * SetManualRatModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetManualRatModeHandler)

/**
 * GetFrequencyLockHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetFrequencyLockHandler)

/**
 * SetFrequencyLockHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetFrequencyLockHandler)

/**
 * GetFrequencyInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetFrequencyInfoHandler)

/**
 * UnsolFrequencyInfoHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolFrequencyInfoHandler)

/**
 * B2B1ConfigInfoHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(B2B1ConfigInfoHandler)

/**
 * AcBarringInfoHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(AcBarringInfoHandler)

#endif /* __OEM_MISC_NETWORK_H__ */
