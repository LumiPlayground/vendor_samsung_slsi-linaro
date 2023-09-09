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

void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioSimResponse>& response) {
    auto hidlResponse = response->hidl();
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT,
            sim::AreUiccApplicationsEnabledResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CHANGE_SIM_PIN2,
            sim::ChangeIccPin2ForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CHANGE_SIM_PIN,
        sim::ChangeIccPinForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_ENABLE_UICC_APPLICATIONS,
        sim::EnableUiccApplicationsResponse(hidlResponse) });
    android::updateCommandInfo(slot,
            { ENCODE_REQUEST(RIL_REQUEST_GET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1, 4)),
            sim::GetAllowedCarriersResponse_1_4(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_SUBSCRIPTION,
        sim::GetCdmaSubscriptionResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE,
        sim::GetCdmaSubscriptionSourceResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_QUERY_FACILITY_LOCK,
        sim::GetFacilityLockForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_IMSI,
        sim::GetIMSIForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_SIM_PHONEBOOK_CAPACITY,
            sim::GetSimPhonebookCapacityResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_GET_SIM_PHONEBOOK_RECORDS,
            sim::GetSimPhonebookRecordsResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SIM_CLOSE_CHANNEL,
        sim::IccCloseLogicalChannelResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SIM_IO,
        sim::IccIoForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SIM_OPEN_CHANNEL,
        sim::IccOpenLogicalChannelResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC,
        sim::IccTransmitApduBasicChannelResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL,
        sim::IccTransmitApduLogicalChannelResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING,
        sim::ReportStkServiceIsRunningResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SIM_AUTHENTICATION,
        sim::RequestIccSimAuthenticationResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND,
        sim::SendEnvelopeResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS,
        sim::SendEnvelopeWithStatusResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE,
        sim::SendTerminalResponseToSimResponse(hidlResponse) });
    android::updateCommandInfo(slot,
            { ENCODE_REQUEST(RIL_REQUEST_SET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1, 4)),
            sim::SetAllowedCarriersResponse_1_4(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION,
        sim::SetCarrierInfoForImsiEncryptionResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE,
        sim::SetCdmaSubscriptionSourceResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_FACILITY_LOCK,
        sim::SetFacilityLockForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot,
            { ENCODE_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, HAL_VERSION_CODE(1,6)),
            sim::SetSimCardPowerResponse_1_6(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SET_UICC_SUBSCRIPTION,
        sim::SetUiccSubscriptionResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_ENTER_SIM_PIN2,
        sim::SupplyIccPin2ForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_ENTER_SIM_PIN,
        sim::SupplyIccPinForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_ENTER_SIM_PUK2,
        sim::SupplyIccPuk2ForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_ENTER_SIM_PUK,
        sim::SupplyIccPukForAppResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION,
        sim::SupplySimDepersonalizationResponse(hidlResponse) });
    android::updateCommandInfo(slot, { RIL_REQUEST_UPDATE_SIM_PHONEBOOK_RECORDS,
            sim::UpdateSimPhonebookRecordsResponse(hidlResponse) });
}

void updateUnsolResponseInfo(uint32_t slot,
        const std::shared_ptr<RadioSimIndication>& indication) {
    auto hidlIndication = indication->hidl();
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_CARRIER_INFO_IMSI_ENCRYPTION,
        sim::carrierInfoForImsiEncryption(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED,
        sim::cdmaSubscriptionSourceChanged(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_CHANGED,
            sim::simPhonebookChanged(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_RECORDS_RECEIVED,
            sim::simPhonebookRecordsReceived(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_SIM_REFRESH,
            sim::simRefresh(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED,
        sim::simStatusChanged(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_STK_EVENT_NOTIFY,
        sim::stkEventNotify(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_STK_PROACTIVE_COMMAND,
        sim::stkProactiveCommand(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_STK_SESSION_END,
        sim::stkSessionEnd(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED,
        sim::subscriptionStatusChanged(hidlIndication), android::WAKE_PARTIAL });
    android::updateUnsolResponseInfo(slot, { RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED,
        sim::uiccApplicationsEnablementChanged(hidlIndication), android::WAKE_PARTIAL });
}
