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
 * protocolstkbuilder.cpp
 *
 *  Created on: 2021. 06. 17.
 *      Author: aryaman.2019
 */

#include "protocolstkbuilder.h"
#include "legacy/protocolstkbuilderlegacy.h"
#include "json/protocolstkbuilderjson.h"

ProtocolStkBuilder::ProtocolStkBuilder()
    : ProtocolStkBuilderInterface() {
    if (IsPayloadTypeJson()) {
        protocolStkBuilderInterface = new ProtocolStkBuilderJson();
    } else {
        protocolStkBuilderInterface = new ProtocolStkBuilderLegacy();
    }
}

ProtocolStkBuilder::~ProtocolStkBuilder() {
    delete protocolStkBuilderInterface;
    protocolStkBuilderInterface = NULL;
}

ModemData *ProtocolStkBuilder::BuildStkEnvelopeCommand(int nLength, BYTE *pEnvelopeCmd) {
    return protocolStkBuilderInterface->BuildStkEnvelopeCommand(nLength, pEnvelopeCmd);
}

ModemData *ProtocolStkBuilder::BuildStkTerminalResponse(int nLength, BYTE *pTerminalRsp) {
    return protocolStkBuilderInterface->BuildStkTerminalResponse(nLength, pTerminalRsp);
}

ModemData *ProtocolStkBuilder::BuildStkEnvelopeStatus(int nLength, BYTE *pEnvelopeStatus) {
    return protocolStkBuilderInterface->BuildStkEnvelopeStatus(nLength, pEnvelopeStatus);
}

ModemData *ProtocolStkBuilder::BuildStkCallSetup(int nUserOper) {
    return protocolStkBuilderInterface->BuildStkCallSetup(nUserOper);
}

ModemData *ProtocolStkBuilder::BuildOemSatWifiRequest(int msgId, BYTE *pData, int nDataLength) {
    return protocolStkBuilderInterface->BuildOemSatWifiRequest(msgId, pData, nDataLength);
}
