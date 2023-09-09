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
 * dtmf.h
 *
 *  Created on: 2020. 7. 8.
 *      Author: sungwoo48.choi
 */

#ifndef __DTMF_H__
#define __DTMF_H__

#include "servicehandlerbase.h"

/**
 * DtmfHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DtmfHandler)

/**
 * DtmfStartHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DtmfStartHandler)

/**
 * DtmfStopHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DtmfStopHandler)

#endif /* __DTMF_H__ */
