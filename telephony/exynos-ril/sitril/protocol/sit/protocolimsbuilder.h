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
 * protocolcallbuilder.h
 *
 *  Created on: 2014. 12. 2.
 *      Author: sungwoo48.choi
 */

#ifndef __PROTOCOL_IMS_BUILDER_H__
#define __PROTOCOL_IMS_BUILDER_H__

#include "protocolbuilder.h"
#include <slsi/radio_v2_1.h>

class ModemData;

class ProtocolImsBuilder  : public ProtocolBuilder
{
public:
    ProtocolImsBuilder() {}
    virtual ~ProtocolImsBuilder() {}
public:
    virtual ModemData *BuildEmergencyCallStatus(BYTE nStatus, BYTE nRat);

private:
    static const int MAX_IMS_RCM_SIZE = (64 * 1024);  // max size 64 KB for supporting RCS multiframe packet

public:
//AIMS support start ---------------------
    ModemData *BuildAimsPDU(int requestId, void *data, unsigned int datalen);
    ModemData *BuildAimsIndPDU(int requestId, void *data, unsigned int datalen);
//AIMS support end ---------------------
//APIMS support start ------------------
    ModemData *BuildImsRegistrationInfo(int regState, int accessNetwork, int suggestedAction, int capabilities);
    ModemData *BuildImsStartImsTraffic(int token, int trafficType, int accessNetwork, int direction);
    ModemData *BuildImsStopImsTraffic(int token);
    ModemData *BuildImsTriggerFallback(int reason);
    ModemData *BuildImsSetSrvccCallInfo(const std::vector<RilImsV2_1::RIL_SrvccCall>& data);
    ModemData *BuildImsMtCallGuardtmrStatus(uint8_t status);
//APIMS support end -------------------
};

#endif /* __PROTOCOL_IMS_BUILDER_H__ */
