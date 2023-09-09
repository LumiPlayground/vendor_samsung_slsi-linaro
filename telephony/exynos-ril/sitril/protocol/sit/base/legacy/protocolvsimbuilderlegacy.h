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
 * protocolvsimbuilderlegacy.h
 *
 *  Created on: 2016. 02. 26.
 */

#ifndef __PROTOCOL_VSIM_BUILDER_LEGACY_H__
#define __PROTOCOL_VSIM_BUILDER_LEGACY_H__

#include "protocolbuilder.h"
#include "protocolvsimbuilderinterface.h"

class ProtocolVsimBuilderLegacy : public ProtocolVsimBuilderInterface, public ProtocolBuilder {
  public:
    ProtocolVsimBuilderLegacy()
        : ProtocolVsimBuilderInterface(), ProtocolBuilder() {}
    virtual ~ProtocolVsimBuilderLegacy() {}

  public:
    ModemData *BuildVsimNotification(int tid, int eventid, int simType);
    ModemData *BuildVsimOperation(int tid, int eventid, int result, int dataLen, const char *data);
};

#endif /* __PROTOCOL_VSIM_BUILDER_LEGACY_H__ */
