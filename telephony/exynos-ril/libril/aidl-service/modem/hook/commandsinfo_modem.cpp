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
#include <ril_internal.h>

#include "commandsinfo.h"
#include "commands.h"
#include "unsolresponse.h"

using namespace android::hardware::radio::impl;
using ::android::sp;

void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioModemResponse>& response) {
    // HIDL response hook
    auto hidlResponse = response->hidl();
    android::updateCommandInfo(slot, { RIL_REQUEST_ENABLE_MODEM, modem::EnableModemResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_BASEBAND_VERSION, modem::GetBasebandVersionResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_DEVICE_IDENTITY, modem::GetDeviceIdentityResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_HARDWARE_CONFIG, modem::GetHardwareConfigResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_ACTIVITY_INFO, modem::GetModemActivityInfoResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_RADIO_CAPABILITY, modem::GetRadioCapabilityResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_MODEM_STATUS, modem::GetModemStackStatusResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_NV_READ_ITEM, modem::NvReadItemResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_NV_RESET_CONFIG, modem::NvResetConfigResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_NV_WRITE_CDMA_PRL, modem::NvWriteCdmaPrlResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_NV_WRITE_ITEM, modem::NvWriteItemResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SHUTDOWN, modem::RequestShutdownResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SEND_DEVICE_STATE, modem::SendDeviceStateResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_RADIO_CAPABILITY, modem::SetRadioCapabilityResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_RADIO_POWER, modem::SetRadioPowerResponse(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_RADIO_POWER, HAL_VERSION_CODE(1,5)),
                                modem::SetRadioPowerResponse_1_5(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_RADIO_POWER, HAL_VERSION_CODE(1,6)),
                                modem::SetRadioPowerResponse_1_6(hidlResponse) });
}

void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioModemIndication>& indication) {
    // HIDL response hook
    auto hidlResponse = indication->hidl();
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_HARDWARE_CONFIG_CHANGED, modem::HardwareConfigChangedInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_MODEM_RESTART, modem::ModemResetInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RADIO_CAPABILITY, modem::RadioCapabilityIndicationInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, modem::RadioStateChangedInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RIL_CONNECTED, modem::RilConnectedInd(hidlResponse), android::WAKE_PARTIAL });
}