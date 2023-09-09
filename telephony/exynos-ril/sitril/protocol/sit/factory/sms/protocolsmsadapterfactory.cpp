/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolsmsadapterfactory.h"
#include "protocolsmsadapterjsonfactory.h"
#include "protocolsmsadapterlegacyfactory.h"
#include "commondef.h"

std::shared_ptr<ProtocolSmsAdapterFactory> ProtocolSmsAdapterFactory::protocolSmsAdapterFactory = nullptr;

void ProtocolSmsAdapterFactory::createInstance() {
    if (IsPayloadTypeJson()) protocolSmsAdapterFactory = std::make_shared<ProtocolSmsAdapterJsonFactory>();
    else protocolSmsAdapterFactory = std::make_shared<ProtocolSmsAdapterLegacyFactory>();
}

std::shared_ptr<ProtocolSmsAdapterFactory> ProtocolSmsAdapterFactory::getInstance() {
    return protocolSmsAdapterFactory;
}

ProtocolNewSmsOnSimIndAdapterInterface* ProtocolSmsAdapterFactory::createProtocolNewSmsOnSimIndAdapter(const ModemData *pModemData) {
    return new ProtocolNewSmsOnSimIndAdapterLegacy(pModemData);
}
