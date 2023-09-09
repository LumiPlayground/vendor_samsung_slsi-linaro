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
 * smshandlerfactory.cpp
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#include "smshandlerfactory.h"
#include "messageid.h"
#include "smshandler.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(SmsHandlerFactory)

BEGIN_FACTORY_ENTRY(SmsHandlerFactory)
    // Request and SolicitedResponse Handler
    ADD_FACTORY_ENTRY(SendSmsHandler, MSG_SMS_SEND)
    ADD_FACTORY_ENTRY(SendSmsMoreHandler, MSG_SMS_SEND_MORE)
    ADD_FACTORY_ENTRY(WriteSmsToSimHandler, MSG_SMS_WRITE_SMS_TO_SIM)
    ADD_FACTORY_ENTRY(DeleteSmsOnSimHandler, MSG_SMS_DELETE_SMS_ON_SIM)
    ADD_FACTORY_ENTRY(GetBcstSmsConfigHandler, MSG_SMS_GET_BROADCAST_SMS_CONFIG)
    ADD_FACTORY_ENTRY(SetBcstSmsConfigHandler, MSG_SMS_SET_BROADCAST_SMS_CONFIG)
    ADD_FACTORY_ENTRY(BcstActivationHandler, MSG_SMS_BROADCAST_ACTIVATION)
    ADD_FACTORY_ENTRY(GetSmscAddressHandler, MSG_SMS_GET_SMSC_ADDRESS)
    ADD_FACTORY_ENTRY(SetSmscAddressHandler, MSG_SMS_SET_SMSC_ADDRESS)
    ADD_FACTORY_ENTRY(ReportSmsMemoryStatusHandler, MSG_SMS_REPORT_SMS_MEMORY_STATUS)
    ADD_FACTORY_ENTRY(GetStorageCapacityHandler, MSG_SMS_GET_STORAGE_CAPACITY)

    // UnsolicitedResponse Handler
    ADD_FACTORY_ENTRY(NewSmsHandler, MSG_SMS_INCOMING_NEW_SMS)
    ADD_FACTORY_ENTRY(NewSmsStatusReportHandler, MSG_SMS_INCOMING_NEW_SMS_STATUS_REPORT)
    ADD_FACTORY_ENTRY(NewSmsOnSimHandler, MSG_SMS_INCOMING_NEW_SMS_ON_SIM)
    ADD_FACTORY_ENTRY(SimSmsStorageFullHandler, MSG_SMS_SIM_SMS_STORAGE_FULL)
    ADD_FACTORY_ENTRY(NewBcstSmsHandler, MSG_SMS_INCOMING_NEW_BROADCAST_SMS)

    // Misc Service Handler
    ADD_FACTORY_ENTRY(SmsAckHandler, MSG_MISC_SMS_ACKNOWLEDGE)
    ADD_FACTORY_ENTRY(SmsAckWithPduHandler, MSG_MISC_SMS_ACK_WITH_PDU)
    ADD_FACTORY_ENTRY(CdmaSmsAckHandler, MSG_MISC_SMS_CDMA_ACKNOWLEDGE)
END_FACTORY_ENTRY()
