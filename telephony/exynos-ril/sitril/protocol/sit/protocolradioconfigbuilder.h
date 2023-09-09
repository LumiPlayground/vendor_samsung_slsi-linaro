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
 * protocolradioconfigbuilder.h
 *
 *  Created on: 2019. 8. 13.
 */

#ifndef __PROTOCOL_RADIO_CONFIG_BUILDER_H__
#define __PROTOCOL_RADIO_CONFIG_BUILDER_H__

#include "protocolradioconfigbuilderinterface.h"
#include <base/types.h>

class ProtocolRadioConfigBuilder : public ProtocolRadioConfigBuilderInterface {
private:
    ProtocolRadioConfigBuilderInterface *protocolRadioConfigBuilderInterface = NULL;
public:
    ProtocolRadioConfigBuilder();
    ProtocolRadioConfigBuilder(const ProtocolRadioConfigBuilder&) = delete;
    ProtocolRadioConfigBuilder& operator=(ProtocolRadioConfigBuilder const&) = delete;
    virtual ~ProtocolRadioConfigBuilder();

    ModemData *BuildGetPhoneCapability();
    ModemData *BuildSetModemsConfig(int config);
    ModemData *BuildSimGetSlotStatus();
    ModemData *BuildSimSetLogicalSlotMapping(int *pData, int nDataLength);
    ModemData *BuildSimSetLogicalSlotPortMapping(android::hardware::radio::config::V2_0::RIL_SlotPortMapping *pData, int nPortSize);
};

#endif /* __PROTOCOL_RADIO_CONFIG_BUILDER_H__ */
