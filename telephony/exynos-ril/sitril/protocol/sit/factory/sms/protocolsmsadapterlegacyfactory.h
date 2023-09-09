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
#include "legacy/protocolsmsadapterlegacy.h"

class ProtocolSmsAdapterLegacyFactory : public ProtocolSmsAdapterFactory {
public:

    ProtocolSendSmsRespAdapterLegacy*
         createProtocolSendSmsRespAdapter(const ModemData *pModemData);
    ProtocolWriteSmsToSimRespAdapterLegacy*
         createProtocolWriteSmsToSimRespAdapter(const ModemData *pModemData);
    ProtocolSmsCapacityOnSimRespAdapterLegacy*
         createProtocolSmsCapacityOnSimRespAdapter(const ModemData *pModemData);
    ProtocolSmscAddrRespAdapterLegacy*
         createProtocolSmscAddrRespAdapter(const ModemData *pModemData);
    ProtocolNewSmsIndAdapterLegacy*
         createProtocolNewSmsIndAdapter(const ModemData *pModemData);
    ProtocolGetBcstSmsConfRespAdapterLegacy*
         createProtocolGetBcstSmsConfRespAdapter(const ModemData *pModemData);
    ProtocolNewBcstSmsAdapterLegacy*
         createProtocolNewBcstSmsAdapter(const ModemData *pModemData);
    ProtocolCdmaSendSmsRespAdapterLegacy*
         createProtocolCdmaSendSmsRespAdapter(const ModemData *pModemData);
    ProtocolCdmaNewSmsIndAdapterLegacy*
         createProtocolCdmaNewSmsIndAdapter(const ModemData *pModemData);
    ProtocolCdmaWriteSmsToRuimRespAdapterLegacy*
         createProtocolCdmaWriteSmsToRuimRespAdapter(const ModemData *pModemData);
    ProtocolGetCdmaBcstSmsConfRespAdapterLegacy*
         createProtocolGetCdmaBcstSmsConfRespAdapter(const ModemData *pModemData);
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy*
         createProtocolCdmaVoiceMsgWaitingInfoIndAdapter(const ModemData *pModemData);
};
