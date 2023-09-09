/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "saphandler.h"
#include "saphandlerfactory.h"
#include "messageid.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(SapHandlerFactory)

BEGIN_FACTORY_ENTRY(SapHandlerFactory)
    ADD_FACTORY_ENTRY(SapConnectHandler, MSG_SAP_CONNECT)
    ADD_FACTORY_ENTRY(SapDisconnectHandler, MSG_SAP_DISCONNECT)
    ADD_FACTORY_ENTRY(SapApduHandler, MSG_SAP_APDU)
    ADD_FACTORY_ENTRY(SapTransferAtrHandler, MSG_SAP_TRANSFER_ATR)
    ADD_FACTORY_ENTRY(SapPowerHandler, MSG_SAP_POWER)
    ADD_FACTORY_ENTRY(SapResetSimHandler, MSG_SAP_RESET_SIM)
    ADD_FACTORY_ENTRY(SapTransferCardReaderStatusHandler, MSG_SAP_TRANSFER_CARD_READER_STATUS)
    ADD_FACTORY_ENTRY(SapSetTransferProtocolHandler, MSG_SAP_SET_TRANSFER_PROTOCOL)

    //Indication
    ADD_FACTORY_ENTRY(SapDisconnectIndHandler, MSG_SAP_DISCONNECT_IND)
    ADD_FACTORY_ENTRY(SapStatusIndHandler, MSG_SAP_STATUS_IND)
END_FACTORY_ENTRY()
