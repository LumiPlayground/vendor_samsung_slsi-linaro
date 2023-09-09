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
 * protocolstkbuilderinterface.h
 *
 *  Created on: 2021. 06. 17.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_STK_BUILDER_INTERFACE_H__
#define __PROTOCOL_STK_BUILDER_INTERFACE_H__

#include <base/types.h>

class ModemData;

class ProtocolStkBuilderInterface
{
public:
    ProtocolStkBuilderInterface() {}
    virtual ~ProtocolStkBuilderInterface() {}

public:
    virtual ModemData *BuildStkEnvelopeCommand(int nLength, BYTE *pEnvelopeCmd) = 0;
    virtual ModemData *BuildStkTerminalResponse(int nLength, BYTE *pTerminalRsp) = 0;
    virtual ModemData *BuildStkEnvelopeStatus(int nLength, BYTE *pEnvelopeStatus) = 0;

    virtual ModemData *BuildStkCallSetup(int nUserOper) = 0;
    virtual ModemData *BuildOemSatWifiRequest(int msgId, BYTE *pData, int nDataLength) = 0;
};

#endif /* __PROTOCOL_STK_BUILDER_INTERFACE_H__ */
