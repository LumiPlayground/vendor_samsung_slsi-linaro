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
 * oem_deviceinfo.h
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */
#ifndef __OEM_DEVICEINFO_H__
#define __OEM_DEVICEINFO_H__

#include "servicehandlerbase.h"

/**
 * GetRfDesenseModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetRfDesenseModeHandler)

/**
 * SetRfDesenseModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetRfDesenseModeHandler)

/**
 * GetManualBandModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetManualBandModeHandler)

/**
 * SetManualBandModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetManualBandModeHandler)

/**
 * SetPSensorStatusHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetPSensorStatusHandler)

/**
 * SetSarStateHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSarStateHandler)

/**
 * GetSarStateHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetSarStateHandler)

/**
 * UnsolSarControlStateHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolSarControlStateHandler)

/**
 * UnsolSarRfConnectionHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolSarRfConnectionHandler)

/**
 * RssiScanHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(RssiScanHandler)

/**
 * RssiScanResultHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(RssiScanResultHandler)

#endif // __OEM_DEVICEINFO_H__
