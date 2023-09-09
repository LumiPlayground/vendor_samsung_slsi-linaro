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
 * protocolvsimbuilderinterface.h
 *
 *  Created on: 2021. 06. 18.
 */

#ifndef __PROTOCOL_VSIM_BUILDER_INTERFACE_H__
#define __PROTOCOL_VSIM_BUILDER_INTERFACE_H__

class ModemData;

class ProtocolVsimBuilderInterface
{
public:
    ProtocolVsimBuilderInterface() {}
    virtual ~ProtocolVsimBuilderInterface() {}

public:
    virtual ModemData *BuildVsimNotification(int tid, int eventid, int simType) = 0;
    virtual ModemData *BuildVsimOperation(int tid, int eventid, int result, int dataLen, const char *data) = 0;
};

#endif /* __PROTOCOL_VSIM_BUILDER_INTERFACE_H__ */
