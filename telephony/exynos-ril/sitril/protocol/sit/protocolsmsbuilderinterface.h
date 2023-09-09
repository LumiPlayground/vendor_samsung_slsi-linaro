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
 * protocolsmsbuilderinterface.h
 *
 *  Created on: 2021. 06. 14.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SMS_BUILDER_INTERFACE_H__
#define __PROTOCOL_SMS_BUILDER_INTERFACE_H__

#include <base/rildef.h>

class ModemData;

class ProtocolSmsBuilderInterface
{
public:
    ProtocolSmsBuilderInterface() {}
    virtual ~ProtocolSmsBuilderInterface() {}

public:
    virtual ModemData *BuildSendSms(const char *smsc, int smscLen, const char *pdu, int pduSize, bool bExpectMore) = 0;
    virtual ModemData *BuildSmsAck(int result, int tpid, int error) = 0;
    virtual ModemData *BuildSmsAck(int result, int tpid, const char *pdu, int pduSize) = 0;
    virtual ModemData *BuildSmscAddress() = 0;
    virtual ModemData *BuildSmscAddress(int sca_len, const char *sca) = 0;
    virtual ModemData *BuildSmsMemoryStatus(int status) = 0;
    virtual ModemData *BuildWriteSmsToSim(int status, int index, int pduSize, const char * pdu) = 0;
    virtual ModemData *BuildDeleteSmsOnSim(int index) = 0;
    virtual ModemData *BuildGetBroadcastSmsConfig() = 0;
    virtual ModemData *BuildSetBroadcastSmsConfig(const RIL_GSM_BroadcastSmsConfigInfo *rgbsci, int num) = 0;
    virtual ModemData *BuildSmsBroadcastActivation(int bcst_act) = 0;
    virtual ModemData *BuildGetStoredSmsCount(int sim_id) = 0;

    virtual ModemData *BuildSendCdmaSms(const char *msg, int msgLen, bool bExpectMore) = 0;
    virtual ModemData *BuildSendCdmaSmsAck(int tpid, int errClass, int errCode) = 0;
    virtual ModemData *BuildWriteCdmaSmsToRuim(int status, const char *msg, int msgLen) = 0;
    virtual ModemData *BuildDeleteCdmaSmsOnRuim(int index) = 0;
    virtual ModemData *BuildGetCdmaBroadcastSmsConfig() = 0;
    virtual ModemData *BuildSetCdmaBroadcastSmsConfig(const RIL_CDMA_BroadcastSmsConfigInfo *rcbsci, int num) = 0;
    virtual ModemData *BuildCdmaSmsBroadcastActivation(int act) = 0;
};
#endif /* __PROTOCOL_SMS_BUILDER_INTERFACE_H__ */
