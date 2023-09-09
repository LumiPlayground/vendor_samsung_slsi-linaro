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
#pragma once

#include <slsi/radioconfig_v1_2.h>
#include <slsi/radioconfig_v2_0.h>
#include <aidl/android/hardware/radio/config/SimSlotStatus.h>
#include <aidl/android/hardware/radio/config/SimPortInfo.h>

namespace android::hardware::radio::impl::config {

namespace V2_0 = android::hardware::radio::config::V2_0;

::aidl::android::hardware::radio::config::SimPortInfo toAidl(
            const android::hardware::radio::config::V2_0::RIL_PORT_INFO& info);
::aidl::android::hardware::radio::config::SimSlotStatus toAidl(
            const android::hardware::radio::config::V2_0::RIL_SimSlotStatus& sst);
::aidl::android::hardware::radio::config::SimSlotStatus toAidl(const RIL_SimSlotStatus_1_2& sst);

}
