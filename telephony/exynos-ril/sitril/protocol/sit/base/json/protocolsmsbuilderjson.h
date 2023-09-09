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
 * protocolsmsbuilderjson.h
 *
 *  Created on: 2021. 08. 27.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SMS_BUILDER_JSON_H__
#define __PROTOCOL_SMS_BUILDER_JSON_H__

#include "protocoljsonbuilder.h"
#include "protocolsmsbuilderinterface.h"

class ProtocolSmsBuilderJson : public ProtocolSmsBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolSmsBuilderJson()
        : ProtocolSmsBuilderInterface(), ProtocolJsonBuilder() {}
    virtual ~ProtocolSmsBuilderJson() {}

  public:
    ModemData *BuildSendSms(const char *smsc, int smscLen, const char *pdu, int pduSize, bool bExpectMore);
    ModemData *BuildSmsAck(int result, int tpid, int error);
    ModemData *BuildSmsAck(int result, int tpid, const char *pdu, int pduSize);
    ModemData *BuildSmscAddress();
    ModemData *BuildSmscAddress(int sca_len, const char *sca);
    ModemData *BuildSmsMemoryStatus(int status);
    ModemData *BuildWriteSmsToSim(int status, int index, int pduSize, const char *pdu);
    ModemData *BuildDeleteSmsOnSim(int index);
    ModemData *BuildGetBroadcastSmsConfig();
    ModemData *BuildSetBroadcastSmsConfig(const RIL_GSM_BroadcastSmsConfigInfo *rgbsci, int num);
    ModemData *BuildSmsBroadcastActivation(int bcst_act);
    ModemData *BuildGetStoredSmsCount(int sim_id);

    ModemData *BuildSendCdmaSms(const char *msg, int msgLen, bool bExpectMore);
    ModemData *BuildSendCdmaSmsAck(int tpid, int errClass, int errCode);
    ModemData *BuildWriteCdmaSmsToRuim(int status, const char *msg, int msgLen);
    ModemData *BuildDeleteCdmaSmsOnRuim(int index);
    ModemData *BuildGetCdmaBroadcastSmsConfig();
    ModemData *BuildSetCdmaBroadcastSmsConfig(const RIL_CDMA_BroadcastSmsConfigInfo *rcbsci, int num);
    ModemData *BuildCdmaSmsBroadcastActivation(int act);
};
#endif /* __PROTOCOL_SMS_BUILDER_JSON_H__ */
