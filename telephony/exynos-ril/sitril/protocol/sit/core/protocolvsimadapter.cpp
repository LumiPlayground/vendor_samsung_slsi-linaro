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
 * protocolvsimadapter.cpp
 *
 *  Created on: 2021. 06. 18.
 */

#include "protocolvsimadapter.h"
#include "legacy/protocolvsimadapterlegacy.h"
#include "json/protocolvsimadapterjson.h"
#include "commondef.h"

/**
 * ProtocolVsimOperationAdapter
 */
ProtocolVsimOperationAdapter::ProtocolVsimOperationAdapter(const ModemData *pModemData)
    : ProtocolVsimOperationAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolVsimOperationAdapterInterface = new ProtocolVsimOperationAdapterJson(pModemData);
    } else {
        protocolVsimOperationAdapterInterface = new ProtocolVsimOperationAdapterLegacy(pModemData);
    }
}

ProtocolVsimOperationAdapter::~ProtocolVsimOperationAdapter() {
    delete protocolVsimOperationAdapterInterface;
    protocolVsimOperationAdapterInterface = NULL;
}

int ProtocolVsimOperationAdapter::GetTransactionId() const {
    return protocolVsimOperationAdapterInterface->GetTransactionId();
}

int ProtocolVsimOperationAdapter::GetEventId() const {
    return protocolVsimOperationAdapterInterface->GetEventId();
}

int ProtocolVsimOperationAdapter::GetResult() const {
    return protocolVsimOperationAdapterInterface->GetResult();
}

int ProtocolVsimOperationAdapter::GetOperationDataLength() {
    return protocolVsimOperationAdapterInterface->GetOperationDataLength();
}

const char *ProtocolVsimOperationAdapter::GetOperationData() {
    return protocolVsimOperationAdapterInterface->GetOperationData();
}
