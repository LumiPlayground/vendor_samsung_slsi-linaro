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
 * protocolsapbuilder.cpp
 *
 *  Created on: 2021. 08. 2.
 *      Author: aryaman.2019
 */

#include "protocolsapbuilder.h"
#include "legacy/protocolsapbuilderlegacy.h"
#include "json/protocolsapbuilderjson.h"

ProtocolSapBuilder::ProtocolSapBuilder() {
    if (IsPayloadTypeJson())
        protocolSapBuilderInterface = new ProtocolSapBuilderJson();
    else
        protocolSapBuilderInterface = new ProtocolSapBuilderLegacy();
}

ProtocolSapBuilder::~ProtocolSapBuilder() {
    delete protocolSapBuilderInterface;
    protocolSapBuilderInterface = NULL;
}

ModemData *ProtocolSapBuilder::BuildSapConnect(int maxMsgSize) {
    return protocolSapBuilderInterface->BuildSapConnect(maxMsgSize);
}

ModemData *ProtocolSapBuilder::BuildSapDisconnect() {
    return protocolSapBuilderInterface->BuildSapDisconnect();
}

ModemData *ProtocolSapBuilder::BuildSapTransferApdu(int apduLeng, char *apdu) {
    return protocolSapBuilderInterface->BuildSapTransferApdu(apduLeng, apdu);
}

ModemData *ProtocolSapBuilder::BuildSapTransferAtr() {
    return protocolSapBuilderInterface->BuildSapTransferAtr();
}

ModemData *ProtocolSapBuilder::BuildSapPowerSimOnOff(bool state) {
    return protocolSapBuilderInterface->BuildSapPowerSimOnOff(state);
}

ModemData *ProtocolSapBuilder::BuildSapResetSim() {
    return protocolSapBuilderInterface->BuildSapResetSim();
}

ModemData *ProtocolSapBuilder::BuildSapTransferCardReaderStatus() {
    return protocolSapBuilderInterface->BuildSapTransferCardReaderStatus();
}

ModemData *ProtocolSapBuilder::BuildSapTranportProtocol(int protocol) {
    return protocolSapBuilderInterface->BuildSapTranportProtocol(protocol);
}
