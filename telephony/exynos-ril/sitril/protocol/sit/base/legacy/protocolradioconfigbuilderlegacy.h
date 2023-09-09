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
 * protocolradioconfigbuilderlegacy.h
 *
 *  Created on: 2019. 8. 13.
 */

#ifndef __PROTOCOL_RADIO_CONFIG_BUILDER_LEGACY_H__
#define __PROTOCOL_RADIO_CONFIG_BUILDER_LEGACY_H__

#include "protocolbuilder.h"
#include "rildef.h"
#include "protocolradioconfigbuilderinterface.h"

class ProtocolRadioConfigBuilderLegacy : public ProtocolBuilder, public ProtocolRadioConfigBuilderInterface {
  public:
    ProtocolRadioConfigBuilderLegacy()
        : ProtocolBuilder(), ProtocolRadioConfigBuilderInterface() {}
    virtual ~ProtocolRadioConfigBuilderLegacy() {}

    ModemData *BuildGetPhoneCapability();
    ModemData *BuildSetModemsConfig(int config);
    ModemData *BuildSimGetSlotStatus();
    ModemData *BuildSimSetLogicalSlotMapping(int *pData, int nSlotSize);
    ModemData *BuildSimSetLogicalSlotPortMapping(
                android::hardware::radio::config::V2_0::RIL_SlotPortMapping *pData, int nPortSize);

};

#endif /* __PROTOCOL_RADIO_CONFIG_BUILDER_LEGACY_H__ */
