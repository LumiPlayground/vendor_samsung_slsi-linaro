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
 * smshandler.h
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#ifndef __SMS_HANDLER_H__
#define __SMS_HANDLER_H__

#include "servicehandlerbase.h"

/**
 * SendSmsHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SendSmsHandler)

/**
 * SendSmsMoreHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SendSmsMoreHandler)

/**
 * WriteSmsToSimHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(WriteSmsToSimHandler)

/**
 * DeleteSmsOnSimHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(DeleteSmsOnSimHandler)

/**
 * GetBcstSmsConfigHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetBcstSmsConfigHandler)

/**
 * SetBcstSmsConfigHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetBcstSmsConfigHandler)

/**
 * BcstActivationHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(BcstActivationHandler)

/**
 * GetSmscAddressHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetSmscAddressHandler)

/**
 * SetSmscAddressHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSmscAddressHandler)

/**
 * ReportSmsMemoryStatusHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ReportSmsMemoryStatusHandler)

/**
 * GetStorageCapacityHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetStorageCapacityHandler)

/**
 * NewSmsHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(NewSmsHandler)

/**
 * NewSmsStatusReportHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(NewSmsStatusReportHandler)

/**
 * NewSmsOnSimHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(NewSmsOnSimHandler)

/**
 * SimSmsStorageFullHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(SimSmsStorageFullHandler)

/**
 * NewBcstSmsHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(NewBcstSmsHandler)

/**
 * SmsAckHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SmsAckHandler)

/**
 * SmsAckWithPduHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SmsAckWithPduHandler)

/**
 * CdmaSmsAckHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(CdmaSmsAckHandler)


#endif /* __SMS_HANDLER_H__ */
