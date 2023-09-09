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
 * protocolsmsbuilder.cpp
 *
 *  Created on: 2021. 06. 14.
 *      Author: aryaman.2019
 */

#include "protocolsmsbuilder.h"
#include "factory/sms/protocolsmsbuilderfactory.h"
#include "legacy/protocolsmsbuilderlegacy.h"
#include "json/protocolsmsbuilderjson.h"

ProtocolSmsBuilder::ProtocolSmsBuilder()
    : ProtocolSmsBuilderInterface() {
    auto factory = ProtocolSmsBuilderFactory::getInstance();
    protocolSmsBuilderInterface = factory->createProtocolSmsBuilder();
}

ProtocolSmsBuilder::~ProtocolSmsBuilder() {
    delete protocolSmsBuilderInterface;
    protocolSmsBuilderInterface = NULL;
}

ModemData *ProtocolSmsBuilder::BuildSendSms(const char *smsc, int smscLen, const char *pdu, int pduSize, bool bExpectMore) {
    return protocolSmsBuilderInterface->BuildSendSms(smsc, smscLen, pdu, pduSize, bExpectMore);
}

ModemData *ProtocolSmsBuilder::BuildSmsAck(int result, int tpid, int error) {
    return protocolSmsBuilderInterface->BuildSmsAck(result, tpid, error);
}

ModemData *ProtocolSmsBuilder::BuildSmsAck(int result, int tpid, const char *pdu, int pduSize) {
    return protocolSmsBuilderInterface->BuildSmsAck(result, tpid, pdu, pduSize);
}

ModemData *ProtocolSmsBuilder::BuildSmscAddress() {
    return protocolSmsBuilderInterface->BuildSmscAddress();
}

ModemData *ProtocolSmsBuilder::BuildSmscAddress(int sca_len, const char *sca) {
    return protocolSmsBuilderInterface->BuildSmscAddress(sca_len, sca);
}

ModemData *ProtocolSmsBuilder::BuildSmsMemoryStatus(int status) {
    return protocolSmsBuilderInterface->BuildSmsMemoryStatus(status);
}

ModemData *ProtocolSmsBuilder::BuildWriteSmsToSim(int status, int index, int pduSize, const char *pdu) {
    return protocolSmsBuilderInterface->BuildWriteSmsToSim(status, index, pduSize, pdu);
}

ModemData *ProtocolSmsBuilder::BuildDeleteSmsOnSim(int index) {
    return protocolSmsBuilderInterface->BuildDeleteSmsOnSim(index);
}

ModemData *ProtocolSmsBuilder::BuildGetBroadcastSmsConfig() {
    return protocolSmsBuilderInterface->BuildGetBroadcastSmsConfig();
}

ModemData *ProtocolSmsBuilder::BuildSetBroadcastSmsConfig(
    const RIL_GSM_BroadcastSmsConfigInfo *rgbsci, int num) {
    return protocolSmsBuilderInterface->BuildSetBroadcastSmsConfig(rgbsci, num);
}

ModemData *ProtocolSmsBuilder::BuildSmsBroadcastActivation(int bcst_act) {
    return protocolSmsBuilderInterface->BuildSmsBroadcastActivation(bcst_act);
}

ModemData *ProtocolSmsBuilder::BuildGetStoredSmsCount(int sim_id) {
    return protocolSmsBuilderInterface->BuildGetStoredSmsCount(sim_id);
}

// need to implement bExpectMore feature.
ModemData *ProtocolSmsBuilder::BuildSendCdmaSms(const char *msg, int msgLen, bool bExpectMore) {
    return protocolSmsBuilderInterface->BuildSendCdmaSms(msg, msgLen, bExpectMore);
}

ModemData *ProtocolSmsBuilder::BuildSendCdmaSmsAck(int tpid, int errClass, int errCode) {
    return protocolSmsBuilderInterface->BuildSendCdmaSmsAck(tpid, errClass, errCode);
}

ModemData *ProtocolSmsBuilder::BuildWriteCdmaSmsToRuim(int status, const char *msg, int msgLen) {
    return protocolSmsBuilderInterface->BuildWriteCdmaSmsToRuim(status, msg, msgLen);
}

ModemData *ProtocolSmsBuilder::BuildDeleteCdmaSmsOnRuim(int index) {
    return protocolSmsBuilderInterface->BuildDeleteCdmaSmsOnRuim(index);
}

ModemData *ProtocolSmsBuilder::BuildGetCdmaBroadcastSmsConfig() {
    return protocolSmsBuilderInterface->BuildGetCdmaBroadcastSmsConfig();
}

ModemData *ProtocolSmsBuilder::BuildSetCdmaBroadcastSmsConfig(
    const RIL_CDMA_BroadcastSmsConfigInfo *rcbsci, int num) {
    return protocolSmsBuilderInterface->BuildSetCdmaBroadcastSmsConfig(rcbsci, num);
}

ModemData *ProtocolSmsBuilder::BuildCdmaSmsBroadcastActivation(int act) {
    return protocolSmsBuilderInterface->BuildCdmaSmsBroadcastActivation(act);
}
