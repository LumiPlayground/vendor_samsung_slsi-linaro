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
#include <ril_internal.h>
#include <slsi/radio_v1_6.h>

#include "commands.h"
#include "commandsinfo.h"
#include "unsolresponse.h"

using namespace android::hardware::radio::impl;
using ::android::sp;

void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioDataResponse> &response) {
    // HIDL response hook
    auto hidlResponse = response->hidl();

    android::updateCommandInfo(
        slot, {RIL_REQUEST_ALLOCATE_PDU_SESSION_ID,
               data::allocatePduSessionIdResponse(hidlResponse)});
    android::updateCommandInfo(slot, {RIL_REQUEST_RELEASE_PDU_SESSION_ID, data::releasePduSessionIdResponse(hidlResponse)});
    android::updateCommandInfo(slot, {RIL_REQUEST_START_HANDOVER, data::startHandoverResponse(hidlResponse)});
    android::updateCommandInfo(slot, {RIL_REQUEST_CANCEL_HANDOVER, data::cancelHandoverResponse(hidlResponse)});
    android::updateCommandInfo(slot, {ENCODE_REQUEST(RIL_REQUEST_SLICING_CONFIG, HAL_VERSION_CODE(1, 6)), data::getSlicingConfigResponse_1_6(hidlResponse)});
    android::updateCommandInfo(slot, {RIL_REQUEST_SET_DATA_THROTTLING, data::setDataThrottlingResponse(hidlResponse)});
    android::updateCommandInfo(
        slot,
        {ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 6)),
         data::setupDataCallResponse_1_6(hidlResponse)});
    android::updateCommandInfo(
        slot,
        {ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 5)),
         data::setupDataCallResponse_1_5(hidlResponse)});
    android::updateCommandInfo(slot,
                               {RIL_REQUEST_DEACTIVATE_DATA_CALL,
                                data::deactivateDataCallResponse(hidlResponse)});
    android::updateCommandInfo(slot, {ENCODE_REQUEST(RIL_REQUEST_DATA_CALL_LIST, HAL_VERSION_CODE(1, 6)), data::getDataCallListResponse_1_6(hidlResponse)});
    android::updateCommandInfo(
        slot,
        {RIL_REQUEST_ALLOW_DATA, data::setDataAllowedResponse(hidlResponse)});
    android::updateCommandInfo(slot,
                               {RIL_REQUEST_SET_DATA_PROFILE,
                                data::setDataProfileResponse_1_5(hidlResponse)});
    android::updateCommandInfo(slot,
                               {RIL_REQUEST_SET_INITIAL_ATTACH_APN,
                                data::setInitialAttachApnResponse_1_5(hidlResponse)});
    android::updateCommandInfo(slot,
                               {RIL_REQUEST_START_KEEPALIVE,
                                data::startKeepaliveResponse(hidlResponse)});
    android::updateCommandInfo(slot, {RIL_REQUEST_STOP_KEEPALIVE,
                                      data::stopKeepaliveResponse(hidlResponse)});
}

void updateUnsolResponseInfo(
    uint32_t slot, const std::shared_ptr<RadioDataIndication> &indication) {
    // HIDL response hook
    auto hidlResponse = indication->hidl();

    android::updateUnsolResponseInfo(slot, {RIL_UNSOL_DATA_CALL_LIST_CHANGED,
                                            data::dataCallListChangedInd_1_6(hidlResponse),
                                            android::DONT_WAKE});

    android::updateUnsolResponseInfo(slot, {RIL_UNSOL_PCO_DATA,
                                            data::pcoDataInd(hidlResponse),
                                            android::WAKE_PARTIAL});

    android::updateUnsolResponseInfo(
        slot, {RIL_UNSOL_KEEPALIVE_STATUS, data::keepaliveStatusInd(hidlResponse),
               android::WAKE_PARTIAL});
}
