/* copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#ifndef __RADIOCONFIG_V2_0_H__
#define __RADIOCONFIG_V2_0_H__

#include <string>
#include <vector>

#define MAX_PORT_NUM 4

namespace android::hardware::radio::config::V2_0 {

typedef enum {
    PORT_INACTIVE = 0,
    PORT_ACTIVE,
} PORT_STATE;

typedef struct {
    uint32_t logicalSlotId;
    /**
     * Integrated Circuit Card IDentifier (ICCID) is Unique Identifier of the SIM CARD. File is
     * located in the SIM card at EFiccid (0x2FE2) as per ETSI 102.221. The ICCID is defined by
     * the ITU-T recommendation E.118 ISO/IEC 7816.
     *
     * This data is applicable only when cardState is CardState:PRESENT.
     */
    std::string iccid;
    bool port_state;
} RIL_PORT_INFO;

typedef struct {
    RIL_CardState cardState;        //Card state in the physical slot
    /**
     * An Answer To Reset (ATR) is a message output by a Smart Card conforming to ISO/IEC 7816
     * standards, following electrical reset of the card's chip. The ATR conveys information about
     * the communication parameters proposed by the card, and the card's nature and state.
     *
     * This data is applicable only when cardState is CardState:PRESENT.
     */
    std::string atr;
    /**
     * The EID is the eUICC identifier. The EID shall be stored within the ECASD and can be
     * retrieved by the Device at any time using the standard GlobalPlatform GET DATA command.
     *
     * This data is mandatory and applicable only when cardState is CardState:PRESENT and SIM card
     * supports eUICC.
     */
    std::string eid;
    std::vector<RIL_PORT_INFO> port_info;
} RIL_SimSlotStatus;

typedef struct {
    int sstVer;
    std::vector<RIL_SimSlotStatus> mSimSlotStatus;
} RIL_SimSlotStatusResult;

typedef struct {
    int32_t physicalSlotId;
    int32_t portId;
} RIL_SlotPortMapping;

typedef struct {
    int32_t num_slots;
    RIL_SlotPortMapping slotPortMap[MAX_PORT_NUM];
} RIL_SlotPortMappingReq;

}

#endif // __RADIOCONFIG_V2_0_H__

