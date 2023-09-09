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
 *  Created on: 2021. 2. 22.
 *      Author: hjun78.park
 */

#ifndef __BOW_HANDLER_H__
#define __BOW_HANDLER_H__

#include "servicehandlerbase.h"

/**
 * WifiChannelStatusReportHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(WifiChannelStatusReportHandler)

/**
 * WifiStatusResponseHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(WifiStatusResponseHandler)

/**
 * OpenChannelResponseHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(OpenChannelResponseHandler)

/**
 * SendDataResponseHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SendDataResponseHandler)

/**
 * ReceiveDataResponseHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ReceiveDataResponseHandler)

/**
 * CloseChannelResponseHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CloseChannelResponseHandler)

/**
 * WiFiStatusRequestHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(WiFiStatusRequestHandler)

/**
 * OpenChannelRequestHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(OpenChannelRequestHandler)

/**
 * SendDataRequestHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(SendDataRequestHandler)

/**
 * ReceiveDataRequestHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(ReceiveDataRequestHandler)

/**
 * CloseChannelRequestHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(CloseChannelRequestHandler)

#endif /* __BOW_HANDLER_H__ */
