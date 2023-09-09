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
 * protocolstkbuilderjson.h
 *
 *  Created on: 2021. 08. 12.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_STK_BUILDER_JSON_H__
#define __PROTOCOL_STK_BUILDER_JSON_H__

#include "protocoljsonbuilder.h"
#include "protocolstkbuilderinterface.h"

class ProtocolStkBuilderJson : public ProtocolStkBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolStkBuilderJson()
        : ProtocolStkBuilderInterface(), ProtocolJsonBuilder() {}
    virtual ~ProtocolStkBuilderJson() {}

  public:
    ModemData *BuildStkEnvelopeCommand(int nLength, BYTE *pEnvelopeCmd);
    ModemData *BuildStkTerminalResponse(int nLength, BYTE *pTerminalRsp);
    ModemData *BuildStkEnvelopeStatus(int nLength, BYTE *pEnvelopeStatus);

    ModemData *BuildStkCallSetup(int nUserOper);
    ModemData *BuildOemSatWifiRequest(int msgId, BYTE *pData, int nDataLength);
};

#endif /* __PROTOCOL_STK_BUILDER_JSON_H__ */
