/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolembmsbuilder.h"
#include "legacy/protocolembmsbuilderlegacy.h"

ModemData *ProtocolEmbmsBuilder::BuildSetService(int state) {
    return protocolEmbmsBuilderInterface->BuildSetService(state);
}

ModemData *ProtocolEmbmsBuilder::BuildSetSession(int state, uint64_t tmgi, int saiListLen,
                                                 const uint32_t *pSaiList, int freqListLen, const uint32_t *pFreqList) {
    return protocolEmbmsBuilderInterface->BuildSetSession(state, tmgi, saiListLen, pSaiList,
                                                          freqListLen, pFreqList);
}

ModemData *ProtocolEmbmsBuilder::BuildGetSessionList(int state) {
    return protocolEmbmsBuilderInterface->BuildGetSessionList(state);
}

ModemData *ProtocolEmbmsBuilder::BuildSignalStrength() {
    return protocolEmbmsBuilderInterface->BuildSignalStrength();
}

ModemData *ProtocolEmbmsBuilder::BuildNetworkTime() {
    return protocolEmbmsBuilderInterface->BuildNetworkTime();
}

ProtocolEmbmsBuilder::ProtocolEmbmsBuilder() {
    protocolEmbmsBuilderInterface = new ProtocolEmbmsBuilderLegacy();
}

ProtocolEmbmsBuilder::~ProtocolEmbmsBuilder() {
    delete protocolEmbmsBuilderInterface;
    protocolEmbmsBuilderInterface = NULL;
}
