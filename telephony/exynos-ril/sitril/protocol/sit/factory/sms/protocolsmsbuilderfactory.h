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

#include "protocolsmsbuilderinterface.h"

class ProtocolSmsBuilderFactory {
private:
    static std::shared_ptr<ProtocolSmsBuilderFactory> protocolSmsBuilderFactory;

public:
    virtual ~ProtocolSmsBuilderFactory() { }

    static void createInstance();
    static std::shared_ptr<ProtocolSmsBuilderFactory> getInstance();

    virtual ProtocolSmsBuilderInterface* createProtocolSmsBuilder() = 0;
};
