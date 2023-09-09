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
#include <android/hardware/radio/1.6/types.h>
#include <slsi/radio_v1_6.h>
#include <slsi/radioconfig_v1_0.h>
#include <slsi/radioconfig_v1_1.h>
#include <slsi/radioconfig_v1_2.h>
#include <slsi/radioconfig_v1_3.h>
#include <slsi/radioconfig_v2_0.h>
#include <ril_internal.h>

#include "commandsinfo.h"
#include "commands.h"

using namespace android::hardware::radio::impl;
using ::android::sp;

void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioConfigResponse>& response) {
    auto hidlResponse = response->hidlConfig();
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_MODEMS_CONFIG, config::GetModemsConfigResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_PHONE_CAPABILITY, config::GetPhoneCapabilityResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_MODEMS_CONFIG, config::SetModemsConfigResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_PREFERRED_DATA_MODEM, config::SetPreferredDataModemResponse(hidlResponse) });
}

void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioConfigIndication>& /* indication */) {}
