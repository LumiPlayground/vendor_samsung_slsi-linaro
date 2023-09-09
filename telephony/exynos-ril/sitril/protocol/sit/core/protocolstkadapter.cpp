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
 * protocolstkadapter.cpp
 *
 *  Created on: 2021. 06. 17.
 *      Author: aryaman.2019
 */

#include "protocolstkadapter.h"
#include "legacy/protocolstkadapterlegacy.h"
#include "json/protocolstkadapterjson.h"
#include "commondef.h"

/**
 * ProtocolStkResponseAdapter
 */
ProtocolStkResponseAdapter::ProtocolStkResponseAdapter(const ModemData *pModemData)
    : ProtocolStkResponseAdapterInterface() {
    protocolStkResponseAdapterInterface = new ProtocolStkResponseAdapterLegacy(pModemData);
}

ProtocolStkResponseAdapter::~ProtocolStkResponseAdapter() {
    delete protocolStkResponseAdapterInterface;
    protocolStkResponseAdapterInterface = NULL;
}

/**
 * ProtocolStkResponseAdapter
 */
ProtocolStkIndAdapter::ProtocolStkIndAdapter(const ModemData *pModemData)
    : ProtocolStkIndAdapterInterface() {
    protocolStkIndAdapterInterface = new ProtocolStkIndAdapterLegacy(pModemData);
}

ProtocolStkIndAdapter::~ProtocolStkIndAdapter() {
    delete protocolStkIndAdapterInterface;
    protocolStkIndAdapterInterface = NULL;
}

/**
 * ProtocolStkEnvelopeCommandAdapter
 */
ProtocolStkEnvelopeCommandAdapter::ProtocolStkEnvelopeCommandAdapter(const ModemData *pModemData)
    : ProtocolStkEnvelopeCommandAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolStkEnvelopeCommandAdapterInterface = new ProtocolStkEnvelopeCommandAdapterJson(pModemData);
    } else {
        protocolStkEnvelopeCommandAdapterInterface = new ProtocolStkEnvelopeCommandAdapterLegacy(pModemData);
    }
}

ProtocolStkEnvelopeCommandAdapter::~ProtocolStkEnvelopeCommandAdapter() {
    delete protocolStkEnvelopeCommandAdapterInterface;
    protocolStkEnvelopeCommandAdapterInterface = NULL;
}

/**
 * ProtocolStkTerminalRspAdapter
 */
ProtocolStkTerminalRspAdapter::ProtocolStkTerminalRspAdapter(const ModemData *pModemData)
    : ProtocolStkTerminalRspAdapterInterface() {
    protocolStkTerminalRspAdapterInterface = new ProtocolStkTerminalRspAdapterLegacy(pModemData);
}

ProtocolStkTerminalRspAdapter::~ProtocolStkTerminalRspAdapter() {
    delete protocolStkTerminalRspAdapterInterface;
    protocolStkTerminalRspAdapterInterface = NULL;
}

/**
 * ProtocolStkEnvelopeStatusAdapter
 */
ProtocolStkEnvelopeStatusAdapter::ProtocolStkEnvelopeStatusAdapter(const ModemData *pModemData)
    : ProtocolStkEnvelopeStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolStkEnvelopeStatusAdapterInterface = new ProtocolStkEnvelopeStatusAdapterJson(pModemData);
    } else {
        protocolStkEnvelopeStatusAdapterInterface = new ProtocolStkEnvelopeStatusAdapterLegacy(pModemData);
    }
}

ProtocolStkEnvelopeStatusAdapter::~ProtocolStkEnvelopeStatusAdapter() {
    delete protocolStkEnvelopeStatusAdapterInterface;
    protocolStkEnvelopeStatusAdapterInterface = NULL;
}

/**
 * ProtocolStkProactiveCommandAdapter
 */
ProtocolStkProactiveCommandAdapter::ProtocolStkProactiveCommandAdapter(const ModemData *pModemData)
    : ProtocolStkProactiveCommandAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolStkProactiveCommandAdapterInterface = new ProtocolStkProactiveCommandAdapterJson(pModemData);
    } else {
        protocolStkProactiveCommandAdapterInterface = new ProtocolStkProactiveCommandAdapterLegacy(pModemData);
    }
}

ProtocolStkProactiveCommandAdapter::~ProtocolStkProactiveCommandAdapter() {
    delete protocolStkProactiveCommandAdapterInterface;
    protocolStkProactiveCommandAdapterInterface = NULL;
}

/**
 * ProtocolStkSimRefreshAdapter
 */
ProtocolStkSimRefreshAdapter::ProtocolStkSimRefreshAdapter(const ModemData *pModemData)
    : ProtocolStkSimRefreshAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolStkSimRefreshAdapterInterface = new ProtocolStkSimRefreshAdapterJson(pModemData);
    } else {
        protocolStkSimRefreshAdapterInterface = new ProtocolStkSimRefreshAdapterLegacy(pModemData);
    }
}
ProtocolStkSimRefreshAdapter::~ProtocolStkSimRefreshAdapter() {
    delete protocolStkSimRefreshAdapterInterface;
    protocolStkSimRefreshAdapterInterface = NULL;
}

/**
 * ProtocolStkCcAlphaNtfAdapter
 */
ProtocolSsReturnResultAdapter::ProtocolSsReturnResultAdapter(const ModemData *pModemData)
    : ProtocolSsReturnResultAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSsReturnResultAdapterInterface = new ProtocolSsReturnResultAdapterJson(pModemData);
    } else {
        protocolSsReturnResultAdapterInterface = new ProtocolSsReturnResultAdapterLegacy(pModemData);
    }
}
ProtocolSsReturnResultAdapter::~ProtocolSsReturnResultAdapter() {
    delete protocolSsReturnResultAdapterInterface;
    protocolSsReturnResultAdapterInterface = NULL;
}

/**
 * ProtocolStkCcAlphaNtfAdapter
 */
ProtocolStkCcAlphaNtfAdapter::ProtocolStkCcAlphaNtfAdapter(const ModemData *pModemData)
    : ProtocolStkCcAlphaNtfAdapterInterface() {
    protocolStkCcAlphaNtfAdapterInterface = new ProtocolStkCcAlphaNtfAdapterLegacy(pModemData);
}

ProtocolStkCcAlphaNtfAdapter::~ProtocolStkCcAlphaNtfAdapter() {
    delete protocolStkCcAlphaNtfAdapterInterface;
    protocolStkCcAlphaNtfAdapterInterface = NULL;
}

int ProtocolStkCcAlphaNtfAdapter::GetAlphaLength() const {
    return protocolStkCcAlphaNtfAdapterInterface->GetAlphaLength();
}

BYTE *ProtocolStkCcAlphaNtfAdapter::GetAlpha() const {
    return protocolStkCcAlphaNtfAdapterInterface->GetAlpha();
}
