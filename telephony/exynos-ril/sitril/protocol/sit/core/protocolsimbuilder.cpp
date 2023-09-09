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
 * protocolsimbuilder.cpp
 *
 *  Created on: 2021. 06. 08.
 *      Author: aryaman.2019
 */

#include "protocolsimbuilder.h"
#include "legacy/uicc/protocolsimbuilderlegacy.h"
#include "json/protocolsimbuilderjson.h"

ProtocolSimBuilder::ProtocolSimBuilder()
    : ProtocolSimBuilderInterface() {
    if (IsPayloadTypeJson()){
        protocolSimBuilderInterface = new ProtocolSimBuilderJson();
    } else{
        protocolSimBuilderInterface = new ProtocolSimBuilderLegacy();
    }
}

ProtocolSimBuilder::~ProtocolSimBuilder() {
    delete protocolSimBuilderInterface;
    protocolSimBuilderInterface = NULL;
}

ModemData *ProtocolSimBuilder::BuildSimGetStatus() {
    return protocolSimBuilderInterface->BuildSimGetStatus();
}

ModemData *ProtocolSimBuilder::BuildSimVerifyPin(int nPinIndex, const char *pszPin, const char *pszAID) {
    return protocolSimBuilderInterface->BuildSimVerifyPin(nPinIndex, pszPin, pszAID);
}

ModemData *ProtocolSimBuilder::BuildSimVerifyPuk(int nPukIndex, const char *pszPuk, const char *pszNewPin, const char *pszAID) {
    return protocolSimBuilderInterface->BuildSimVerifyPuk(nPukIndex, pszPuk, pszNewPin, pszAID);
}

ModemData *ProtocolSimBuilder::BuildSimChangePin(int nPinIndex, const char *pszOldPin, const char *pszNewPin, const char *pszAID) {
    return protocolSimBuilderInterface->BuildSimChangePin(nPinIndex, pszOldPin, pszNewPin, pszAID);
}

ModemData *ProtocolSimBuilder::BuildSimVerifyNetworkLock(int nFac, const char *pszPassword, int nSvcClass, const char *pszAID) {
    return protocolSimBuilderInterface->BuildSimVerifyNetworkLock(nFac, pszPassword, nSvcClass, pszAID);
}

ModemData *ProtocolSimBuilder::BuildSimIO(int nCmd, int nAppType, int nFileID, const char *pPath, int p1, int p2, int p3,
                                          int nDataLen, const char *pData, const char *pszPin2, const char *pszAID) {
    return protocolSimBuilderInterface->BuildSimIO(nCmd, nAppType, nFileID, pPath, p1, p2, p3, nDataLen, pData, pszPin2, pszAID);
}

ModemData *ProtocolSimBuilder::BuildSimGetFacilityLock(char *pszCode, char *pszPassword, int nSvcClass, char *pszAID) {
    return protocolSimBuilderInterface->BuildSimGetFacilityLock(pszCode, pszPassword, nSvcClass, pszAID);
}

ModemData *ProtocolSimBuilder::BuildSimSetFacilityLock(char *pszCode, int nLockMode, char *pszPassword, int nSvcClass, char *pszAID) {
    return protocolSimBuilderInterface->BuildSimSetFacilityLock(pszCode, nLockMode, pszPassword, nSvcClass, pszAID);
}

ModemData *ProtocolSimBuilder::BuildSimGetIsimAuth(int nAuthType, BYTE *pAuth, int nAuthLengh) {
    return protocolSimBuilderInterface->BuildSimGetIsimAuth(nAuthType, pAuth, nAuthLengh);
}

ModemData *ProtocolSimBuilder::BuildSimGetSimAuth(int nAuthContext, BYTE *pAuth, int nAuthLengh, int nAppType) {
    return protocolSimBuilderInterface->BuildSimGetSimAuth(nAuthContext, pAuth, nAuthLengh, nAppType);
}

ModemData *ProtocolSimBuilder::BuildSimTransmitApduBasic(int nSessionID, int cla, int instruction,
                                                         int p1, int p2, int p3, const char *pszApduData) {
    return protocolSimBuilderInterface->BuildSimTransmitApduBasic(nSessionID, cla, instruction, p1, p2, p3, pszApduData);
}

ModemData *ProtocolSimBuilder::BuildSimOpenChannel(const char *pszAID) {
    return protocolSimBuilderInterface->BuildSimOpenChannel(pszAID);
}

ModemData *ProtocolSimBuilder::BuildSimOpenChannelWithP2(const char *pszAID, int p2) {
    return protocolSimBuilderInterface->BuildSimOpenChannelWithP2(pszAID, p2);
}

ModemData *ProtocolSimBuilder::BuildSimCloseChannel(int nSessionID) {
    return protocolSimBuilderInterface->BuildSimCloseChannel(nSessionID);
}

ModemData *ProtocolSimBuilder::BuildSimTransmitApduChannel(int nSessionID, int cla, int instruction,
                                                           int p1, int p2, int p3, const char *pszApduData) {
    return protocolSimBuilderInterface->BuildSimTransmitApduChannel(nSessionID, cla, instruction, p1, p2, p3, pszApduData);
}

ModemData *ProtocolSimBuilder::BuildGetImsi(const char *pszAID) {
    return protocolSimBuilderInterface->BuildGetImsi(pszAID);
}
ModemData *ProtocolSimBuilder::BuildSimGetGbaAuth(const char *pGetGbaAuthdata, int dataSize) {
    return protocolSimBuilderInterface->BuildSimGetGbaAuth(pGetGbaAuthdata, dataSize);
}

ModemData *ProtocolSimBuilder::BuildSimGetATR() {
    return protocolSimBuilderInterface->BuildSimGetATR();
}

/* PhoneBook */
ModemData *ProtocolSimBuilder::BuildSimReadPbEntry(int pb_type, int recordId) {
    return protocolSimBuilderInterface->BuildSimReadPbEntry(pb_type, recordId);
}

ModemData *ProtocolSimBuilder::BuildSimUpdatePbDelete(int mode, int type, int recordId) {
    return protocolSimBuilderInterface->BuildSimUpdatePbDelete(mode, type, recordId);
}

ModemData *ProtocolSimBuilder::BuildSimUpdatePb3gEntry(int mode, int type, int recordId, int length, char *pb) {
    return protocolSimBuilderInterface->BuildSimUpdatePb3gEntry(mode, type, recordId, length, pb);
}

ModemData *ProtocolSimBuilder::BuildSimGetPbStorageInfo(int pbType) {
    return protocolSimBuilderInterface->BuildSimGetPbStorageInfo(pbType);
}

ModemData *ProtocolSimBuilder::BuildSim3GPbCapa() {
    return protocolSimBuilderInterface->BuildSim3GPbCapa();
}

ModemData *ProtocolSimBuilder::BuildSetCarrierRestrictions(int nAllowLength, CarrierInfo *pAllowed, int nExcludeLength, CarrierInfo *pExcluded,
                                                           bool m_bAllowedPrioritized, int nMultiSimPolicy) {
    return protocolSimBuilderInterface->BuildSetCarrierRestrictions(nAllowLength, pAllowed, nExcludeLength, pExcluded, m_bAllowedPrioritized, nMultiSimPolicy);
}

ModemData *ProtocolSimBuilder::BuildGetCarrierRestrictions() {
    return protocolSimBuilderInterface->BuildGetCarrierRestrictions();
}

ModemData *ProtocolSimBuilder::BuildSetUicc(int activeStatus) {
    return protocolSimBuilderInterface->BuildSetUicc(activeStatus);
}

ModemData *ProtocolSimBuilder::BuildSetSimCardPower(int isPowerUp) {
    return protocolSimBuilderInterface->BuildSetSimCardPower(isPowerUp);
}

// Secure Element
ModemData *ProtocolSimBuilder::BuildOemSimRequest(int msgId, BYTE *pData, int nDataLength) {
    return protocolSimBuilderInterface->BuildOemSimRequest(msgId, pData, nDataLength);
}

ModemData *ProtocolSimBuilder::BuildGetSimLockInfo() {
    return protocolSimBuilderInterface->BuildGetSimLockInfo();
}

#ifdef AUTO_VERIFY_PIN_WITH_CP
ModemData *ProtocolSimBuilder::BuildSimVerifyEncryptedPin(
        int nPinLen, const char *pszEncryptedPin, const char *pszAID) {
    return protocolSimBuilderInterface->BuildSimVerifyEncryptedPin(nPinLen, pszEncryptedPin, pszAID);
}
#endif
