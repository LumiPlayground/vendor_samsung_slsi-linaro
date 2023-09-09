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
 * protocolstkbuilderlegacy.h
 *
 *  Created on: 2014. 12. 2.
 *      Author: sungwoo48.choi
 */

#ifndef __PROTOCOL_STK_BUILDER_LEGACY_H__
#define __PROTOCOL_STK_BUILDER_LEGACY_H__

#include "protocolbuilder.h"
#include "protocolstkbuilderinterface.h"

class ProtocolStkBuilderLegacy : public ProtocolStkBuilderInterface, public ProtocolBuilder {
  public:
    ProtocolStkBuilderLegacy()
        : ProtocolStkBuilderInterface(), ProtocolBuilder() {}
    virtual ~ProtocolStkBuilderLegacy() {}

  public:
    ModemData *BuildStkEnvelopeCommand(int nLength, BYTE *pEnvelopeCmd);
    ModemData *BuildStkTerminalResponse(int nLength, BYTE *pTerminalRsp);
    ModemData *BuildStkEnvelopeStatus(int nLength, BYTE *pEnvelopeStatus);

    ModemData *BuildStkCallSetup(int nUserOper);
    ModemData *BuildOemSatWifiRequest(int msgId, BYTE *pData, int nDataLength);
};

#endif /* __PROTOCOL_STK_BUILDER_LEGACY_H__ */
