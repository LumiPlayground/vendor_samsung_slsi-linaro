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
 * protocolradioconfigbuilder.cpp
 *
 *  Created on: 2021. 8. 2.
 */

#include "protocolradioconfigbuilder.h"
#include "legacy/protocolradioconfigbuilderlegacy.h"
#include "json/protocolradioconfigbuilderjson.h"

ProtocolRadioConfigBuilder::ProtocolRadioConfigBuilder()
    : ProtocolRadioConfigBuilderInterface() {
    if (IsPayloadTypeJson()) {
        protocolRadioConfigBuilderInterface = new ProtocolRadioConfigBuilderJson();
    } else {
        protocolRadioConfigBuilderInterface = new ProtocolRadioConfigBuilderLegacy();
    }
}

ProtocolRadioConfigBuilder::~ProtocolRadioConfigBuilder() {
    if (protocolRadioConfigBuilderInterface) {
        delete protocolRadioConfigBuilderInterface;
        protocolRadioConfigBuilderInterface = NULL;
    }
}

ModemData *ProtocolRadioConfigBuilder::BuildGetPhoneCapability() {
    return protocolRadioConfigBuilderInterface->BuildGetPhoneCapability();
}

ModemData *ProtocolRadioConfigBuilder::BuildSetModemsConfig(int numOfLiveModems) {
    return protocolRadioConfigBuilderInterface->BuildSetModemsConfig(numOfLiveModems);
}

ModemData *ProtocolRadioConfigBuilder::BuildSimGetSlotStatus() {
    return protocolRadioConfigBuilderInterface->BuildSimGetSlotStatus();
}

ModemData *ProtocolRadioConfigBuilder::BuildSimSetLogicalSlotMapping(int *pData, int nDataLength) {
    return protocolRadioConfigBuilderInterface->BuildSimSetLogicalSlotMapping(pData, nDataLength);
}

ModemData *ProtocolRadioConfigBuilder::BuildSimSetLogicalSlotPortMapping(
    android::hardware::radio::config::V2_0::RIL_SlotPortMapping *pData, int nPortSize) {
    return protocolRadioConfigBuilderInterface->BuildSimSetLogicalSlotPortMapping(pData, nPortSize);
}
