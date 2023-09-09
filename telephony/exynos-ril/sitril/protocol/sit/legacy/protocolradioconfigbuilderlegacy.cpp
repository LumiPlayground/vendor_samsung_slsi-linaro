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
 * protocolradioconfigbuilderlegacy.cpp
 *
 *  Created on: 2019. 8. 13.
 */

#include "protocolradioconfigbuilderlegacy.h"
#include "modemdata.h"

namespace configV2_0 = android::hardware::radio::config::V2_0;

ModemData *ProtocolRadioConfigBuilderLegacy::BuildGetPhoneCapability()
{
    null_data_format req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_GET_PHONE_CAPABILITY, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolRadioConfigBuilderLegacy::BuildSetModemsConfig(int numOfLiveModems)
{
    sit_misc_set_modems_config_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_MODEM_CONFIG, length);
    // single(0) or multi(1) even if more than 2
    req.config = (numOfLiveModems == 1) ? 0 : 1;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolRadioConfigBuilderLegacy::BuildSimGetSlotStatus()
{
    null_data_format req;
    int length = sizeof(null_data_format);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_GET_SLOT_STATUS, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolRadioConfigBuilderLegacy::BuildSimSetLogicalSlotMapping(int *pData, int nSlotSize)
{
    sit_sim_set_logical_to_physical_slot_mapping_req_v1_2 req;
    int length = sizeof(sit_sim_set_logical_to_physical_slot_mapping_req_v1_2);

    if (nSlotSize > MAX_SLOT_NUM || nSlotSize <= 0)
        return NULL;

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING, length);

    req.slotMaplen = (BYTE) nSlotSize;
    for(int i = 0; i < nSlotSize; i++)
        req.slotMap[i] = (BYTE) pData[i];

    ModemData *pModemData = new ModemData((char *)&req, length);

    return pModemData;
}

ModemData *ProtocolRadioConfigBuilderLegacy::BuildSimSetLogicalSlotPortMapping(configV2_0::RIL_SlotPortMapping *pData, int nPortSize)
{
    sit_sim_set_logical_to_physical_slot_mapping_req req;
    int length = sizeof(sit_sim_set_logical_to_physical_slot_mapping_req);

    if (nPortSize > MAX_SLOT_NUM || nPortSize <= 0)
        return NULL;

    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING, length);

    req.slotMaplen = (BYTE) nPortSize;
    for(int i = 0; i < nPortSize; i++) {
        req.slotMap[i].physicalSlotId = (BYTE) (pData[i].physicalSlotId);
        req.slotMap[i].portId = (BYTE) (pData[i].portId);
    }

    ModemData *pModemData = new ModemData((char *)&req, length);

    return pModemData;
}
