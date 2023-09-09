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
 * protocolsimbuilderjson.h
 *
 *  Created on: 2021. 7. 19.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SIM_BUILDER_JSON_H__
#define __PROTOCOL_SIM_BUILDER_JSON_H__

#include "protocoljsonbuilder.h"
#include "protocolsimbuilderinterface.h"

class ProtocolSimBuilderJson : public ProtocolSimBuilderInterface, public ProtocolJsonBuilder {
    typedef enum {
        PIN1_PUK1 = 0,
        PIN2_PUK2
    } SIM_PIN_PUK_INDEX;

  public:
    ProtocolSimBuilderJson()
        : ProtocolSimBuilderInterface(), ProtocolJsonBuilder() {}
    virtual ~ProtocolSimBuilderJson() {}

    ModemData *BuildSimGetStatus();
    ModemData *BuildSimVerifyPin(int nPinIndex, const char *pszPin, const char *pszAID);
    ModemData *BuildSimVerifyPuk(int nPukIndex, const char *pszPuk, const char *pszNewPin, const char *pszAID);
    ModemData *BuildSimChangePin(int nPinIndex, const char *pszOldPin, const char *pszNewPin, const char *pszAID);
    ModemData *BuildSimVerifyNetworkLock(int nFac, const char *pszPassword, int nSvcClass, const char *pszAID);
    ModemData *BuildSimIO(int nCmd, int nAppType, int nFileID, const char *pPath, int p1, int p2, int p3,
                          int nDataLen, const char *pData, const char *pszPin2, const char *pszAID);
    ModemData *BuildSimGetFacilityLock(char *pszCode, char *pszPassword, int nSvcClass, char *pszAID);
    ModemData *BuildSimSetFacilityLock(char *pszCode, int nLockMode, char *pszPassword, int nSvcClass, char *pszAID);
    ModemData *BuildSimGetIsimAuth(int nAuthType, BYTE *pAuth, int nAuthLengh);
    ModemData *BuildSimGetSimAuth(int nAuthContext, BYTE *pAuth, int nAuthLengh, int nAppType);
    //virtual ModemData *BuildSimGetIsimGbaAuth(int nAuthType, BYTE nGbaType, BYTE nGbaTag, BYTE *pAuth, int nAuthLengh);
    ModemData *BuildSimGetGbaAuth(const char *pGetGbaAuthdata, int dataSize);
    ModemData *BuildSimTransmitApduBasic(int nSessionID, int cla, int instruction,
                                         int p1, int p2, int p3, const char *pszApduData);
    ModemData *BuildSimOpenChannel(const char *pszAID);
    ModemData *BuildSimOpenChannelWithP2(const char *pszAID, int p2);
    ModemData *BuildSimCloseChannel(int nSessionID);
    ModemData *BuildSimTransmitApduChannel(int nSessionID, int cla, int instruction,
                                           int p1, int p2, int p3, const char *pszApduData);
    ModemData *BuildGetImsi(const char *pszAID);
    ModemData *BuildSimGetATR();
    ModemData *BuildSimReadPbEntry(int pbtype, int recordId);
    ModemData *BuildSimUpdatePb3gEntry(int mode, int type, int recordId, int length, char *pb);
    ModemData *BuildSimUpdatePbDelete(int mode, int type, int recordId);
    ModemData *BuildSimGetPbStorageInfo(int pbType);
    ModemData *BuildSim3GPbCapa();
    ModemData *BuildSetCarrierRestrictions(int nAllowLength, CarrierInfo *pAllowed, int nExcludeLength, CarrierInfo *pExcluded,
                                           bool m_bAllowedPrioritized, int nMultiSimPolicy);
    ModemData *BuildGetCarrierRestrictions();
    ModemData *BuildSetUicc(int activeStatus);
    ModemData *BuildSetSimCardPower(int isPowerUp);

    // Secure Element
    ModemData *BuildOemSimRequest(int msgId, BYTE *pData, int nDataLength);

    // SIM lock info
    ModemData *BuildGetSimLockInfo();

#ifdef AUTO_VERIFY_PIN_WITH_CP
    ModemData *BuildSimVerifyEncryptedPin(int nPinLen, const char *pszEncryptedPin,
            const char *pszAID);
#endif
};

#endif /* __PROTOCOL_SIM_BUILDER_JSON_H__ */
