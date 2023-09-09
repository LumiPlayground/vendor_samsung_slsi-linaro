/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolsmsadapterjsonfactory.h"

ProtocolSendSmsRespAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolSendSmsRespAdapter(const ModemData *pModemData) {
    return new ProtocolSendSmsRespAdapterJson(pModemData);
}

ProtocolWriteSmsToSimRespAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolWriteSmsToSimRespAdapter(const ModemData *pModemData) {
    return new ProtocolWriteSmsToSimRespAdapterJson(pModemData);
}

ProtocolSmsCapacityOnSimRespAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolSmsCapacityOnSimRespAdapter(const ModemData *pModemData) {
    return new ProtocolSmsCapacityOnSimRespAdapterJson(pModemData);
}

ProtocolSmscAddrRespAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolSmscAddrRespAdapter(const ModemData *pModemData) {
    return new ProtocolSmscAddrRespAdapterJson(pModemData);
}

ProtocolNewSmsIndAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolNewSmsIndAdapter(const ModemData *pModemData) {
    return new ProtocolNewSmsIndAdapterJson(pModemData);
}

ProtocolGetBcstSmsConfRespAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolGetBcstSmsConfRespAdapter(const ModemData *pModemData) {
    return new ProtocolGetBcstSmsConfRespAdapterJson(pModemData);
}

ProtocolNewBcstSmsAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolNewBcstSmsAdapter(const ModemData *pModemData) {
    return new ProtocolNewBcstSmsAdapterJson(pModemData);
}

ProtocolCdmaSendSmsRespAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolCdmaSendSmsRespAdapter(const ModemData *pModemData) {
    return new ProtocolCdmaSendSmsRespAdapterJson(pModemData);
}

ProtocolCdmaNewSmsIndAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolCdmaNewSmsIndAdapter(const ModemData *pModemData) {
    return new ProtocolCdmaNewSmsIndAdapterJson(pModemData);
}

ProtocolCdmaWriteSmsToRuimRespAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolCdmaWriteSmsToRuimRespAdapter(const ModemData *pModemData) {
    return new ProtocolCdmaWriteSmsToRuimRespAdapterJson(pModemData);
}

ProtocolGetCdmaBcstSmsConfRespAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolGetCdmaBcstSmsConfRespAdapter(const ModemData *pModemData) {
    return new ProtocolGetCdmaBcstSmsConfRespAdapterJson(pModemData);
}

ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson *ProtocolSmsAdapterJsonFactory::createProtocolCdmaVoiceMsgWaitingInfoIndAdapter(const ModemData *pModemData) {
    return new ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson(pModemData);
}
