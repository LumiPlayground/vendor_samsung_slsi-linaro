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
 * testpacketbuilder.h
 *
 *  Created on: 2016. 4. 18.
 */

#ifndef TestPacketBuilder_H
#define TestPacketBuilder_H

#include "protocolbuilder.h"

class PdpContext;

class TestPacketBuilder : public ProtocolBuilder
{
public:
    TestPacketBuilder() : ProtocolBuilder() {}
    virtual ~TestPacketBuilder() {}

public:
    virtual int BuildNasTimerStartInd(char *buffer, unsigned int length);
    virtual int BuildNasTimerExpiredInd(char *buffer, unsigned int length);
    virtual size_t BuildPcoDataInd(char *buffer, unsigned int length);
    INT32 DumpData(char *buffer,UINT32 bufLen, BOOL rx);
};

#endif /* TestPacketBuilder_H */
