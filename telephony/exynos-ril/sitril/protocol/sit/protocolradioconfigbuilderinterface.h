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
 * protocolradioconfigbuilderinterface.h
 *
 *  Created on: 2021. 8. 2.
 */

#ifndef __PROTOCOL_RADIO_CONFIG_BUILDER_INTERFACE_H__
#define __PROTOCOL_RADIO_CONFIG_BUILDER_INTERFACE_H__

#include "protocolbuilder.h"
#include "rildef.h"

class ModemData;

class ProtocolRadioConfigBuilderInterface {
public:
    ProtocolRadioConfigBuilderInterface() {}
    virtual ~ProtocolRadioConfigBuilderInterface() {}

    virtual ModemData *BuildGetPhoneCapability() = 0;
    virtual ModemData *BuildSetModemsConfig(int config) = 0;
    virtual ModemData *BuildSimGetSlotStatus() = 0;
    virtual ModemData *BuildSimSetLogicalSlotMapping(int *pData, int nSlotSize) = 0;
    virtual ModemData *BuildSimSetLogicalSlotPortMapping(
                android::hardware::radio::config::V2_0::RIL_SlotPortMapping *pData, int nPortSize) = 0;
};

#endif /* __PROTOCOL_RADIO_CONFIG_BUILDER_INTERFACE_H__ */
