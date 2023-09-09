/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#pragma once

#include "protocolsmsadapterfactory.h"
#include "json/protocolsmsadapterjson.h"

class ProtocolSmsAdapterJsonFactory : public ProtocolSmsAdapterFactory {

public:

    ProtocolSendSmsRespAdapterJson*
         createProtocolSendSmsRespAdapter(const ModemData *pModemData);
    ProtocolWriteSmsToSimRespAdapterJson*
         createProtocolWriteSmsToSimRespAdapter(const ModemData *pModemData);
    ProtocolSmsCapacityOnSimRespAdapterJson*
         createProtocolSmsCapacityOnSimRespAdapter(const ModemData *pModemData);
    ProtocolSmscAddrRespAdapterJson*
         createProtocolSmscAddrRespAdapter(const ModemData *pModemData);
    ProtocolNewSmsIndAdapterJson*
         createProtocolNewSmsIndAdapter(const ModemData *pModemData);
    ProtocolGetBcstSmsConfRespAdapterJson*
         createProtocolGetBcstSmsConfRespAdapter(const ModemData *pModemData);
    ProtocolNewBcstSmsAdapterJson*
         createProtocolNewBcstSmsAdapter(const ModemData *pModemData);
    ProtocolCdmaSendSmsRespAdapterJson*
         createProtocolCdmaSendSmsRespAdapter(const ModemData *pModemData);
    ProtocolCdmaNewSmsIndAdapterJson*
         createProtocolCdmaNewSmsIndAdapter(const ModemData *pModemData);
    ProtocolCdmaWriteSmsToRuimRespAdapterJson*
         createProtocolCdmaWriteSmsToRuimRespAdapter(const ModemData *pModemData);
    ProtocolGetCdmaBcstSmsConfRespAdapterJson*
         createProtocolGetCdmaBcstSmsConfRespAdapter(const ModemData *pModemData);
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson*
         createProtocolCdmaVoiceMsgWaitingInfoIndAdapter(const ModemData *pModemData);
};
