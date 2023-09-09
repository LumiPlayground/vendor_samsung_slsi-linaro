/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */

#include <string>
#include "vendorstructs.h"

namespace android::hardware::radio::impl::config {

namespace aidl_config = ::aidl::android::hardware::radio::config;

aidl_config::SimPortInfo toAidl(const V2_0::RIL_PORT_INFO& info) {
    return {
        .logicalSlotId = static_cast<int32_t>(info.logicalSlotId),
        .iccId = static_cast<std::string>(info.iccid),
        .portActive = info.port_state,
    };
}

aidl_config::SimSlotStatus toAidl(const V2_0::RIL_SimSlotStatus& sst) {
    aidl_config::SimSlotStatus slotStatus = {};

    slotStatus.cardState = static_cast<int32_t>(sst.cardState);
    slotStatus.atr = static_cast<std::string>(sst.atr);
    slotStatus.eid = static_cast<std::string>(sst.eid);

    for (int portId = 0; portId < (int) sst.port_info.size(); portId++) {
        slotStatus.portInfo.push_back(toAidl(sst.port_info[portId]));
    }

    return slotStatus;
}

aidl_config::SimSlotStatus toAidl(const RIL_SimSlotStatus_1_2& sst) {
    aidl_config::SimSlotStatus slotStatus = {};

    slotStatus.cardState = static_cast<int32_t>(sst.cardState);
    slotStatus.atr = static_cast<std::string>(sst.atr);
    slotStatus.eid = static_cast<std::string>(sst.eid);

    bool portActive = (sst.slotState == PHYSICAL_SLOT_ACTIVE);
    slotStatus.portInfo.push_back(
            toAidl({sst.logicalSlotId, sst.iccid, portActive}));

    return slotStatus;
}

}

