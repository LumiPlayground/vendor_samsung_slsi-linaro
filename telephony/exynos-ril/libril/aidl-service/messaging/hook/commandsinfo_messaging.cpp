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

void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioMessagingResponse>& response) {
    // HIDL response hook
    auto hidlResponse = response->hidl();
    android::updateCommandInfo(slot, { RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU, messaging::AcknowledgeIncomingGsmSmsWithPduResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE, messaging::AcknowledgeLastIncomingCdmaSmsResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SMS_ACKNOWLEDGE, messaging::AcknowledgeLastIncomingGsmSmsResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM, messaging::DeleteSmsOnRuimResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_DELETE_SMS_ON_SIM, messaging::DeleteSmsOnSimResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG, messaging::GetCdmaBroadcastConfigResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG, messaging::GetGsmBroadcastConfigResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_SMSC_ADDRESS, messaging::GetSmscAddressResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_REPORT_SMS_MEMORY_STATUS, messaging::ReportSmsMemoryStatusResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE, messaging::SendCdmaSmsExpectMoreResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_SEND_SMS, messaging::SendCdmaSmsResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_IMS_SEND_SMS, messaging::SendImsSmsResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SEND_SMS_EXPECT_MORE, messaging::SendSmsExpectMoreResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SEND_SMS, messaging::SendSmsResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION, messaging::SetCdmaBroadcastActivationResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG, messaging::SetCdmaBroadcastConfigResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION, messaging::SetGsmBroadcastActivationResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG, messaging::SetGsmBroadcastConfigResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_SMSC_ADDRESS, messaging::SetSmscAddressResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM, messaging::WriteSmsToRuimResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_WRITE_SMS_TO_SIM, messaging::WriteSmsToSimResponse(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE, HAL_VERSION_CODE(1,6)), messaging::SendCdmaSmsExpectMoreResponse_1_6(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_CDMA_SEND_SMS, HAL_VERSION_CODE(1,6)), messaging::SendCdmaSmsResponse_1_6(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_SEND_SMS_EXPECT_MORE, HAL_VERSION_CODE(1,6)), messaging::SendSmsExpectMoreResponse_1_6(hidlResponse) });
    android::updateCommandInfo(slot, { ENCODE_REQUEST(RIL_REQUEST_SEND_SMS, HAL_VERSION_CODE(1,6)), messaging::SendSmsResponse_1_6(hidlResponse) });
}

void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioMessagingIndication>& indication) {
    // HIDL response hook
    auto hidlResponse = indication->hidl();
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_CDMA_NEW_SMS, messaging::CdmaNewSmsInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL, messaging::CdmaRuimSmsStorageFullInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS, messaging::NewBroadcastSmsInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_NEW_SMS, messaging::NewSmsInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM, messaging::NewSmsOnSimInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT, messaging::NewSmsStatusReportInd(hidlResponse), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_SIM_SMS_STORAGE_FULL, messaging::SimSmsStorageFullInd(hidlResponse), android::WAKE_PARTIAL });
}
