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
 * protocolsapadapter.cpp
 *
 *  Created on: 2021. 8. 2.
 *      Author: aryaman.2019
 */
#include "protocolsapadapter.h"
#include "legacy/protocolsapadapterlegacy.h"
#include "json/protocolsapadapterjson.h"
#include "commondef.h"

ProtocolSapConnectAdapter::ProtocolSapConnectAdapter(const ModemData *pModemData)
    : ProtocolSapConnectAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapConnectAdapterInterface = new ProtocolSapConnectAdapterJson(pModemData);
    } else {
        protocolSapConnectAdapterInterface = new ProtocolSapConnectAdapterLegacy(pModemData);
    }
}

ProtocolSapConnectAdapter::~ProtocolSapConnectAdapter() {
    delete protocolSapConnectAdapterInterface;
    protocolSapConnectAdapterInterface = NULL;
}

int ProtocolSapConnectAdapter::GetStatus() const {
    return protocolSapConnectAdapterInterface->GetStatus();
}

int ProtocolSapConnectAdapter::GetMaxMsgSize() const {
    return protocolSapConnectAdapterInterface->GetMaxMsgSize();
}

ProtocolSapDisconnectAdapter::ProtocolSapDisconnectAdapter(const ModemData *pModemData)
    : ProtocolSapDisconnectAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapDisconnectAdapterInterface = new ProtocolSapDisconnectAdapterJson(pModemData);
    } else {
        protocolSapDisconnectAdapterInterface = new ProtocolSapDisconnectAdapterLegacy(pModemData);
    }
}

ProtocolSapDisconnectAdapter::~ProtocolSapDisconnectAdapter() {
    delete protocolSapDisconnectAdapterInterface;
    protocolSapDisconnectAdapterInterface = NULL;
}

int ProtocolSapDisconnectAdapter::GetStatus() const {
    return protocolSapDisconnectAdapterInterface->GetStatus();
}

//ind
ProtocolSapTransferApduAdapter::ProtocolSapTransferApduAdapter(const ModemData *pModemData)
    : ProtocolSapTransferApduAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapTransferApduAdapterInterface = new ProtocolSapTransferApduAdapterJson(pModemData);
    } else {
        protocolSapTransferApduAdapterInterface = new ProtocolSapTransferApduAdapterLegacy(pModemData);
    }
}

ProtocolSapTransferApduAdapter::~ProtocolSapTransferApduAdapter() {
    delete protocolSapTransferApduAdapterInterface;
    protocolSapTransferApduAdapterInterface = NULL;
}

int ProtocolSapTransferApduAdapter::GetResult() const {
    return protocolSapTransferApduAdapterInterface->GetResult();
}

int ProtocolSapTransferApduAdapter::GetApduLen() const {
    return protocolSapTransferApduAdapterInterface->GetApduLen();
}

const char *ProtocolSapTransferApduAdapter::GetApdu() const {
    return protocolSapTransferApduAdapterInterface->GetApdu();
}

ProtocolSapTransferAtrAdapter::ProtocolSapTransferAtrAdapter(const ModemData *pModemData)
    : ProtocolSapTransferAtrAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapTransferAtrAdapterInterface = new ProtocolSapTransferAtrAdapterJson(pModemData);
    } else {
        protocolSapTransferAtrAdapterInterface = new ProtocolSapTransferAtrAdapterLegacy(pModemData);
    }
}

ProtocolSapTransferAtrAdapter::~ProtocolSapTransferAtrAdapter() {
    delete protocolSapTransferAtrAdapterInterface;
    protocolSapTransferAtrAdapterInterface = NULL;
}

int ProtocolSapTransferAtrAdapter::GetResult() const {
    return protocolSapTransferAtrAdapterInterface->GetResult();
}

int ProtocolSapTransferAtrAdapter::GetAtrLen() const {
    return protocolSapTransferAtrAdapterInterface->GetAtrLen();
}

const char *ProtocolSapTransferAtrAdapter::GetAtr() const {
    return protocolSapTransferAtrAdapterInterface->GetAtr();
}

ProtocolSapPowerSimOnOffAdapter::ProtocolSapPowerSimOnOffAdapter(const ModemData *pModemData)
    : ProtocolSapPowerSimOnOffAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapPowerSimOnOffAdapterInterface = new ProtocolSapPowerSimOnOffAdapterJson(pModemData);
    } else {
        protocolSapPowerSimOnOffAdapterInterface = new ProtocolSapPowerSimOnOffAdapterLegacy(pModemData);
    }
}

ProtocolSapPowerSimOnOffAdapter::~ProtocolSapPowerSimOnOffAdapter() {
    delete protocolSapPowerSimOnOffAdapterInterface;
    protocolSapPowerSimOnOffAdapterInterface = NULL;
}

int ProtocolSapPowerSimOnOffAdapter::GetResult() const {
    return protocolSapPowerSimOnOffAdapterInterface->GetResult();
}

ProtocolSapResetSimAdapter::ProtocolSapResetSimAdapter(const ModemData *pModemData)
    : ProtocolSapResetSimAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapResetSimAdapterInterface = new ProtocolSapResetSimAdapterJson(pModemData);
    } else {
        protocolSapResetSimAdapterInterface = new ProtocolSapResetSimAdapterLegacy(pModemData);
    }
}

ProtocolSapResetSimAdapter::~ProtocolSapResetSimAdapter() {
    delete protocolSapResetSimAdapterInterface;
    protocolSapResetSimAdapterInterface = NULL;
}

int ProtocolSapResetSimAdapter::GetResult() const {
    return protocolSapResetSimAdapterInterface->GetResult();
}

ProtocolSapTransferCardReaderStatusAdapter::ProtocolSapTransferCardReaderStatusAdapter(const ModemData *pModemData)
    : ProtocolSapTransferCardReaderStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapTransferCardReaderStatusAdapterInterface = new ProtocolSapTransferCardReaderStatusAdapterJson(pModemData);
    } else {
        protocolSapTransferCardReaderStatusAdapterInterface = new ProtocolSapTransferCardReaderStatusAdapterLegacy(pModemData);
    }
}

ProtocolSapTransferCardReaderStatusAdapter::~ProtocolSapTransferCardReaderStatusAdapter() {
    delete protocolSapTransferCardReaderStatusAdapterInterface;
    protocolSapTransferCardReaderStatusAdapterInterface = NULL;
}

int ProtocolSapTransferCardReaderStatusAdapter::GetResult() const {
    return protocolSapTransferCardReaderStatusAdapterInterface->GetResult();
}

ProtocolSapSetTranportProtocolAdapter::ProtocolSapSetTranportProtocolAdapter(const ModemData *pModemData)
    : ProtocolSapSetTranportProtocolAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapSetTranportProtocolAdapterInterface = new ProtocolSapSetTranportProtocolAdapterJson(pModemData);
    } else {
        protocolSapSetTranportProtocolAdapterInterface = new ProtocolSapSetTranportProtocolAdapterLegacy(pModemData);
    }
}

ProtocolSapSetTranportProtocolAdapter::~ProtocolSapSetTranportProtocolAdapter() {
    delete protocolSapSetTranportProtocolAdapterInterface;
    protocolSapSetTranportProtocolAdapterInterface = NULL;
}

int ProtocolSapSetTranportProtocolAdapter::GetResult() const {
    return protocolSapSetTranportProtocolAdapterInterface->GetResult();
}

ProtocolSapDisconnectTypeAdapter::ProtocolSapDisconnectTypeAdapter(const ModemData *pModemData)
    : ProtocolSapDisconnectTypeAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapDisconnectTypeAdapterInterface = new ProtocolSapDisconnectTypeAdapterJson(pModemData);
    } else {
        protocolSapDisconnectTypeAdapterInterface = new ProtocolSapDisconnectTypeAdapterLegacy(pModemData);
    }
}

ProtocolSapDisconnectTypeAdapter::~ProtocolSapDisconnectTypeAdapter() {
    delete protocolSapDisconnectTypeAdapterInterface;
    protocolSapDisconnectTypeAdapterInterface = NULL;
}

int ProtocolSapDisconnectTypeAdapter::GetDisconStatus() const {
    return protocolSapDisconnectTypeAdapterInterface->GetDisconStatus();
}

ProtocolSapStatusAdapter::ProtocolSapStatusAdapter(const ModemData *pModemData)
    : ProtocolSapStatusAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolSapStatusAdapterInterface = new ProtocolSapStatusAdapterJson(pModemData);
    } else {
        protocolSapStatusAdapterInterface = new ProtocolSapStatusAdapterLegacy(pModemData);
    }
}

ProtocolSapStatusAdapter::~ProtocolSapStatusAdapter() {
    delete protocolSapStatusAdapterInterface;
    protocolSapStatusAdapterInterface = NULL;
}

int ProtocolSapStatusAdapter::GetCardStatus() const {
    return protocolSapStatusAdapterInterface->GetCardStatus();
}
