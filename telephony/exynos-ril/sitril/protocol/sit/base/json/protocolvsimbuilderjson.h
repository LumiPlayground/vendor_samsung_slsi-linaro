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
 * protocolvsimbuilderjson.h
 *
 *  Created on: 2021. 08. 18.
 */

#ifndef __PROTOCOL_VSIM_BUILDER_JSON_H__
#define __PROTOCOL_VSIM_BUILDER_JSON_H__

#include "protocoljsonbuilder.h"
#include "protocolvsimbuilderinterface.h"

class ProtocolVsimBuilderJson : public ProtocolVsimBuilderInterface, public ProtocolJsonBuilder {
  public:
    ProtocolVsimBuilderJson()
        : ProtocolVsimBuilderInterface(), ProtocolJsonBuilder() {}
    virtual ~ProtocolVsimBuilderJson() {}

  public:
    ModemData *BuildVsimNotification(int tid, int eventid, int simType);
    ModemData *BuildVsimOperation(int tid, int eventid, int result, int dataLen, const char *data);
};

#endif /* __PROTOCOL_VSIM_BUILDER_JSON_H__ */
