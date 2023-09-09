/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolsmsadapterlegacyfactory.h"

ProtocolSendSmsRespAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolSendSmsRespAdapter(const ModemData *pModemData) {
    return new ProtocolSendSmsRespAdapterLegacy(pModemData);
}

ProtocolWriteSmsToSimRespAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolWriteSmsToSimRespAdapter(const ModemData *pModemData) {
    return new ProtocolWriteSmsToSimRespAdapterLegacy(pModemData);
}

ProtocolSmsCapacityOnSimRespAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolSmsCapacityOnSimRespAdapter(const ModemData *pModemData) {
    return new ProtocolSmsCapacityOnSimRespAdapterLegacy(pModemData);
}

ProtocolSmscAddrRespAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolSmscAddrRespAdapter(const ModemData *pModemData) {
    return new ProtocolSmscAddrRespAdapterLegacy(pModemData);
}

ProtocolNewSmsIndAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolNewSmsIndAdapter(const ModemData *pModemData) {
    return new ProtocolNewSmsIndAdapterLegacy(pModemData);
}

ProtocolGetBcstSmsConfRespAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolGetBcstSmsConfRespAdapter(const ModemData *pModemData) {
    return new ProtocolGetBcstSmsConfRespAdapterLegacy(pModemData);
}

ProtocolNewBcstSmsAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolNewBcstSmsAdapter(const ModemData *pModemData) {
    return new ProtocolNewBcstSmsAdapterLegacy(pModemData);
}

ProtocolCdmaSendSmsRespAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolCdmaSendSmsRespAdapter(const ModemData *pModemData) {
    return new ProtocolCdmaSendSmsRespAdapterLegacy(pModemData);
}

ProtocolCdmaNewSmsIndAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolCdmaNewSmsIndAdapter(const ModemData *pModemData) {
    return new ProtocolCdmaNewSmsIndAdapterLegacy(pModemData);
}

ProtocolCdmaWriteSmsToRuimRespAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolCdmaWriteSmsToRuimRespAdapter(const ModemData *pModemData) {
    return new ProtocolCdmaWriteSmsToRuimRespAdapterLegacy(pModemData);
}

ProtocolGetCdmaBcstSmsConfRespAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolGetCdmaBcstSmsConfRespAdapter(const ModemData *pModemData) {
    return new ProtocolGetCdmaBcstSmsConfRespAdapterLegacy(pModemData);
}

ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy *ProtocolSmsAdapterLegacyFactory::createProtocolCdmaVoiceMsgWaitingInfoIndAdapter(const ModemData *pModemData) {
    return new ProtocolCdmaVoiceMsgWaitingInfoIndAdapterLegacy(pModemData);
}
