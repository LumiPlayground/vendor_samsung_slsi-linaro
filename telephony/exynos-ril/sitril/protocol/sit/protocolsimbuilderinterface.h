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
 * protocolsimbuilderinterface.h
 *
 *  Created on: 2021. 06. 08.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SIM_BUILDER_INTERFACE_H__
#define __PROTOCOL_SIM_BUILDER_INTERFACE_H__

#include <base/rildef.h>

class ModemData;

class ProtocolSimBuilderInterface
{
public:
    ProtocolSimBuilderInterface() {}
    virtual ~ProtocolSimBuilderInterface() {}

    virtual ModemData *BuildSimGetStatus() = 0;
    virtual ModemData *BuildSimVerifyPin(int nPinIndex, const char *pszPin, const char *pszAID) = 0;
    virtual ModemData *BuildSimVerifyPuk(int nPukIndex, const char *pszPuk, const char *pszNewPin, const char *pszAID) = 0;
    virtual ModemData *BuildSimChangePin(int nPinIndex, const char *pszOldPin, const char *pszNewPin, const char *pszAID) = 0;
    virtual ModemData *BuildSimVerifyNetworkLock(int nFac, const char *pszPassword, int nSvcClass, const char *pszAID) = 0;
    virtual ModemData *BuildSimIO(int nCmd, int nAppType, int nFileID, const char *pPath, int p1, int p2, int p3,
            int nDataLen, const char *pData, const char *pszPin2, const char *pszAID) = 0;
    virtual ModemData *BuildSimGetFacilityLock(char *pszCode, char *pszPassword, int nSvcClass, char *pszAID) = 0;
    virtual ModemData *BuildSimSetFacilityLock(char *pszCode, int nLockMode, char *pszPassword, int nSvcClass, char *pszAID) = 0;
    virtual ModemData *BuildSimGetIsimAuth(int nAuthType, BYTE *pAuth, int nAuthLengh) = 0;
    virtual ModemData *BuildSimGetSimAuth(int nAuthContext, BYTE *pAuth, int nAuthLengh, int nAppType) = 0;
    //virtual ModemData *BuildSimGetIsimGbaAuth(int nAuthType, BYTE nGbaType, BYTE nGbaTag, BYTE *pAuth, int nAuthLengh);
    virtual ModemData *BuildSimGetGbaAuth(const char *pGetGbaAuthdata, int dataSize) = 0;
    virtual ModemData *BuildSimTransmitApduBasic(int nSessionID, int cla, int instruction,
            int p1, int p2, int p3, const char *pszApduData) = 0;
    virtual ModemData *BuildSimOpenChannel(const char *pszAID) = 0;
    virtual ModemData *BuildSimOpenChannelWithP2(const char *pszAID, int p2) = 0;
    virtual ModemData *BuildSimCloseChannel(int nSessionID) = 0;
    virtual ModemData *BuildSimTransmitApduChannel(int nSessionID, int cla, int instruction,
            int p1, int p2, int p3, const char *pszApduData) = 0;
    virtual ModemData *BuildGetImsi(const char *pszAID) = 0;
    virtual ModemData *BuildSimGetATR() = 0;
    virtual ModemData *BuildSimReadPbEntry(int pbtype, int recordId) = 0;
    virtual ModemData *BuildSimUpdatePb3gEntry(int mode, int type, int recordId, int length, char *pb) = 0;
    virtual ModemData *BuildSimUpdatePbDelete(int mode, int type, int recordId) = 0;
    virtual ModemData *BuildSimGetPbStorageInfo(int pbType) = 0;
    virtual ModemData *BuildSim3GPbCapa() = 0;
    virtual ModemData *BuildSetCarrierRestrictions(int nAllowLength, CarrierInfo *pAllowed, int nExcludeLength, CarrierInfo *pExcluded,
            bool m_bAllowedPrioritized, int nMultiSimPolicy) = 0;
    virtual ModemData *BuildGetCarrierRestrictions() = 0;
    virtual ModemData *BuildSetUicc(int activeStatus) = 0;
    virtual ModemData *BuildSetSimCardPower(int isPowerUp) = 0;

    // Secure Element
    virtual ModemData *BuildOemSimRequest(int msgId, BYTE *pData, int nDataLength) = 0;

    // SIM lock info
    virtual ModemData *BuildGetSimLockInfo() = 0;

#ifdef AUTO_VERIFY_PIN_WITH_CP
    virtual ModemData *BuildSimVerifyEncryptedPin(int nPinLen,
            const char *pszEncryptedPin, const char *pszAID) = 0;
#endif
};

#endif /* __PROTOCOL_SIM_BUILDER_INTERFACE_H__ */
