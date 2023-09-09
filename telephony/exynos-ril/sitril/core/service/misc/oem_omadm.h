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
 * oem_omadm.h
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#ifndef __OEM_OMADM_H__
#define __OEM_OMADM_H__

#include "servicehandlerbase.h"

/**
 * GetRadioNodeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetRadioNodeHandler)

/**
 * SetRadioNodeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetRadioNodeHandler)

/**
 * GetProvisionUpdateRequestHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetProvisionUpdateRequestHandler)

/**
 * SetProvisionUpdateDoneRequestHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetProvisionUpdateDoneRequestHandler)

/**
 * RadioConfigResetHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(RadioConfigResetHandler)

/**
 * GetPlmnNameFromSE13TableHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetPlmnNameFromSE13TableHandler)

/**
 * Ts25TableDumpHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(Ts25TableDumpHandler)

/**
 * MslCodeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(MslCodeHandler)

#endif /* __OEM_OMADM_H__ */
