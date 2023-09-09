/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolsmsbuilderfactory.h"
#include "protocolsmsbuilderjsonfactory.h"
#include "protocolsmsbuilderlegacyfactory.h"
#include "commondef.h"

std::shared_ptr<ProtocolSmsBuilderFactory> ProtocolSmsBuilderFactory::protocolSmsBuilderFactory = nullptr;

void ProtocolSmsBuilderFactory::createInstance() {
    if (IsPayloadTypeJson()) protocolSmsBuilderFactory = std::make_shared<ProtocolSmsBuilderJsonFactory>();
    else protocolSmsBuilderFactory = std::make_shared<ProtocolSmsBuilderLegacyFactory>();
}

std::shared_ptr<ProtocolSmsBuilderFactory> ProtocolSmsBuilderFactory::getInstance() {
    return protocolSmsBuilderFactory;
}
