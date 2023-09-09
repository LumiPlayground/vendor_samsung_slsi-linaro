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
#include "vendorstructs.h"

namespace android::hardware::radio::impl {

using ::aidl::android::hardware::radio::AccessNetwork;
namespace aidl = ::aidl::android::hardware::radio::modem;

aidl::ActivityStatsTechSpecificInfo toAidl(const modem::V2_0::RIL_ActivityStatsTechSpecificInfo& info) {
    return {
            .rat = AccessNetwork(info.rat),
            .frequencyRange = info.frequencyRange,
            .txmModetimeMs = info.txmModetimeMs,
            .rxModeTimeMs = info.rxModeTimeMs,
        };
}

aidl::ActivityStatsInfo toAidl(const modem::V2_0::RIL_ActivityStatsInfo& info) {
    aidl::ActivityStatsInfo activityStatsInfo = {
            .sleepModeTimeMs = static_cast<int32_t>(info.sleepModeTimeMs),
            .idleModeTimeMs = static_cast<int32_t>(info.idleModeTimeMs),
    };
    for (auto& techSpecificInfo : info.techSpecificInfo) {
        activityStatsInfo.techSpecificInfo.push_back(toAidl(techSpecificInfo));
    }
    return activityStatsInfo;
}

}