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
 * protocolradioconfigbuilderjson.h
 *
 *  Created on: 2021. 8. 6.
 */

#ifndef __PROTOCOL_RADIO_CONFIG_BUILDER_JSON_H__
#define __PROTOCOL_RADIO_CONFIG_BUILDER_JSON_H__

#include "protocoljsonbuilder.h"
#include "protocolradioconfigbuilderinterface.h"

class ProtocolRadioConfigBuilderJson : public ProtocolJsonBuilder, public ProtocolRadioConfigBuilderInterface {
  public:
    ProtocolRadioConfigBuilderJson()
        : ProtocolJsonBuilder(), ProtocolRadioConfigBuilderInterface() {}
    virtual ~ProtocolRadioConfigBuilderJson() {}

    ModemData *BuildGetPhoneCapability();
    ModemData *BuildSetModemsConfig(int config);
    ModemData *BuildSimGetSlotStatus();
    ModemData *BuildSimSetLogicalSlotMapping(int *pData, int nDataLength);
    ModemData *BuildSimSetLogicalSlotPortMapping(android::hardware::radio::config::V2_0::RIL_SlotPortMapping *pData, int nPortSize); // to do implement
};

#endif /* __PROTOCOL_RADIO_CONFIG_BUILDER_JSON_H__ */
