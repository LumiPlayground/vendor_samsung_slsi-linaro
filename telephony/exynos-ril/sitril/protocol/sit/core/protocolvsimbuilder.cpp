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
 * protocolvsimbuilder.cpp
 *
 *  Created on: 2021. 06. 18.
 */

#include "protocolvsimbuilder.h"
#include "legacy/protocolvsimbuilderlegacy.h"
#include "json/protocolvsimbuilderjson.h"

ProtocolVsimBuilder::ProtocolVsimBuilder()
    : ProtocolVsimBuilderInterface() {
    if (IsPayloadTypeJson()) {
        protocolVsimBuilderInterface = new ProtocolVsimBuilderJson();
    } else {
        protocolVsimBuilderInterface = new ProtocolVsimBuilderLegacy();
    }
}

ProtocolVsimBuilder::~ProtocolVsimBuilder() {
    delete protocolVsimBuilderInterface;
    protocolVsimBuilderInterface = NULL;
}

ModemData *ProtocolVsimBuilder::BuildVsimNotification(int tid, int eventid, int simType) {
    return protocolVsimBuilderInterface->BuildVsimNotification(tid, eventid, simType);
}

ModemData *ProtocolVsimBuilder::BuildVsimOperation(int tid, int eventid, int result, int dataLen, const char *data) {
    return protocolVsimBuilderInterface->BuildVsimOperation(tid, eventid, result, dataLen, data);
}
