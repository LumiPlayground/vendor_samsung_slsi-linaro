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
 * networkregistrationinfo.h
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#ifndef __NETWORK_REGISTRATION_INFO_H__
#define __NETWORK_REGISTRATION_INFO_H__

#include "servicehandlerbase.h"

/**
 * VoiceRegistrationStateHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(VoiceRegistrationStateHandler)

/**
 * DataRegistrationStateHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DataRegistrationStateHandler)

/**
 * NetworkStateChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(NetworkStateChangedHandler)

/**
 * OperatorHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(OperatorHandler)

#endif /* __NETWORK_REGISTRATION_INFO_H__ */
