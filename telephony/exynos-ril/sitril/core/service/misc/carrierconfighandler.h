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
 * carrierconfighandler.h
 *
 *  Created on: 2021. 1. 14.
 *      Author: jiyoung.bae
 */

#ifndef __CARRIERCONFIGHANDLER_H__
#define __CARRIERCONFIGHANDLER_H__

#include "servicehandlerbase.h"

/**
 * SetCpCarrierConfigHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCpCarrierConfigHandler)

/**
 * UnsolCpCarrierConfigSimInfoHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolCpCarrierConfigSimInfoHandler)

/**
 * UnsolCpCarrierConfigStatusHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolCpCarrierConfigStatusHandler)

#endif /* __CARRIERCONFIGHANDLER_H__ */
