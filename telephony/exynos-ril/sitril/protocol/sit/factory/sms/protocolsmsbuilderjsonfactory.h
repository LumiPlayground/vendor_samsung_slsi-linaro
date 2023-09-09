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

#include "protocolsmsbuilderfactory.h"
#include "json/protocolsmsbuilderjson.h"

class ProtocolSmsBuilderJsonFactory : public ProtocolSmsBuilderFactory {

public:
    ProtocolSmsBuilderJson* createProtocolSmsBuilder();
};
