/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#ifndef __DEVICE_INFO_H__
#define __DEVICE_INFO_H__

#include "servicehandlerbase.h"

/**
 * BasebandVersionHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(BasebandVersionHandler)

/**
 * ImeiHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ImeiHandler)

/**
 * ImeisvHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ImeisvHandler)

/**
 * DeviceIdentityHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DeviceIdentityHandler)

/**
 * ScreenStateHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ScreenStateHandler)

/**
 * DeviceStateHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DeviceStateHandler)

/**
 * PinControlHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(PinControlHandler)

/**
 * UnsolPinControlHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolPinControlHandler)

/**
 * SGCHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SGCHandler)

/**
 * DeviceInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DeviceInfoHandler)

/**
 * OpenCarrierInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(OpenCarrierInfoHandler)

/**
 * HwConfigHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(HwConfigHandler)

/**
 * HwConfigChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(HwConfigChangedHandler)

/**
 * ModemRestartHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(ModemRestartHandler)

/**
 * SetCarrierInfoImsiEncryptionHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCarrierInfoImsiEncryptionHandler)

/**
 * UnsolCarrierInfoImsiEncryptionHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolCarrierInfoImsiEncryptionHandler)

/**
 * SetIndicationFilterHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetIndicationFilterHandler)

/**
 * SignalStrengthReportingCriteriaHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SignalStrengthReportingCriteriaHandler)

/**
 * OemSignalStrengthReportingCriteriaHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(OemSignalStrengthReportingCriteriaHandler)

/**
 * LinkCapacityReportingCriteriaHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(LinkCapacityReportingCriteriaHandler)

/**
 * SetFeatureInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetFeatureInfoHandler)

/**
 * RfConnectionStateHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(RfConnectionStateHandler)

#endif /* __DEVICE_INFO_H__ */
