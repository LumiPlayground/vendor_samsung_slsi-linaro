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
 * protocolradioconfigbuilderjson.cpp
 *
 *  Created on: 2021. 8. 6.
 */

#include "protocolradioconfigbuilderjson.h"
#include "simjsondef.h"
#include "miscjsondef.h"

namespace configV2_0 = android::hardware::radio::config::V2_0;

ModemData *ProtocolRadioConfigBuilderJson::BuildGetPhoneCapability()
{
    return  GetJsonNullReqModemData(SIT_GET_PHONE_CAPABILITY);
}

ModemData *ProtocolRadioConfigBuilderJson::BuildSetModemsConfig(int numOfLiveModems) {
    Json::Value jsonReqData;
    // single(0) or multi(1) even if more than 2
    jsonReqData[ns_misc_set_modem_config_req::key_config] = (numOfLiveModems == 1) ? 0 : 1;

    return GetJsonReqModemData(jsonReqData, SIT_SET_MODEM_CONFIG);
}

ModemData *ProtocolRadioConfigBuilderJson::BuildSimGetSlotStatus()
{
    return GetJsonNullReqModemData(SIT_GET_SLOT_STATUS);
}

ModemData *ProtocolRadioConfigBuilderJson::BuildSimSetLogicalSlotMapping(int *pData, int nDataLength)
{
    if (nDataLength > MAX_SLOT_NUM || nDataLength <= 0)  return NULL;

    Json::Value jsonReqData;
    for(int i = 0; i < nDataLength; i++) {
        jsonReqData[ns_sim_set_logical_to_physical_slot_mapping_req::key_slot_map].append(pData[i]);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING);
}

ModemData *ProtocolRadioConfigBuilderJson::BuildSimSetLogicalSlotPortMapping(configV2_0::RIL_SlotPortMapping *pData, int nPortSize)
{
    if (nPortSize > MAX_PORT_NUM || nPortSize <= 0)  return NULL;
    if(pData == NULL) return NULL;

    Json::Value jsonReqData;
    for(int i = 0; i < nPortSize; i++) {
        jsonReqData[ns_sim_set_logical_to_physical_slot_mapping_req::key_slot_map].append((int)pData[i].physicalSlotId);
    }

    return GetJsonReqModemData(jsonReqData, SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING);
}
