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

#include "radiosimcallback.h"
#include "radioreturnstatus.h"
#include "radioresponseutil.h"
#include "binderdeathrecipient.h"

namespace android::hardware::radio::impl {

struct RadioSimResponse : public RadioSimResponseCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimResponse> mResponse;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
    }
    void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimResponse>&
                    radioSimResponse);
    void acknowledgeRequest(int32_t serial);
    void areUiccApplicationsEnabledResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            bool enabled) override;
    void changeIccPin2ForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int remainingRetries) override;
    void changeIccPinForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int remainingRetries) override;
    void enableUiccApplicationsResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void getAllowedCarriersResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::CarrierRestrictions& carriers,
            const ::aidl::android::hardware::radio::sim::SimLockMultiSimPolicy& multiSimPolicy) override;
    void getCdmaSubscriptionResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::string& mdn, const std::string& hSid, const std::string& hNid,
            const std::string& min, const std::string& prl) override;
    void getCdmaSubscriptionSourceResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource source) override;
    void getFacilityLockForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int response) override;
    void getIccCardStatusResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::CardStatus& cardStatus) override;

    void getIMSIForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::string& imsi) override;
    void getSimPhonebookCapacityResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::PhonebookCapacity& capacity) override;
    void getSimPhonebookRecordsResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void iccCloseLogicalChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void iccIoForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& iccIo) override;
    void iccOpenLogicalChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info, int channelId,
            const std::vector<uint8_t>& selectResponse) override;
    void iccTransmitApduBasicChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& result) override;
    void iccTransmitApduLogicalChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& result) override;
    void reportStkServiceIsRunningResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void requestIccSimAuthenticationResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& result) override;
    void sendEnvelopeResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::string& commandResponse) override;
    void sendEnvelopeWithStatusResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& iccIo) override;
    void sendTerminalResponseToSimResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void setAllowedCarriersResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void setCarrierInfoForImsiEncryptionResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void setCdmaSubscriptionSourceResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void setFacilityLockForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info, int32_t retry) override;
    void setSimCardPowerResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void setUiccSubscriptionResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void supplyIccPin2ForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t remainingRetries) override;
    void supplyIccPinForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t remainingRetries) override;
    void supplyIccPuk2ForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t remainingRetries) override;
    void supplyIccPukForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t remainingRetries) override;
    void supplySimDepersonalizationResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::PersoSubstate persoType,
            int32_t remainingRetries) override;
    void updateSimPhonebookRecordsResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t updatedRecordIndex) override;
};

}
