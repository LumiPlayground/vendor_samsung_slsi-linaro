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

#include <hidl/1.6/radio_hidl_utils_1_6.h>
#include <telephony/ril.h>
#include <radiosimcallback.h>

struct RadioSimResponseHook : public RadioResponseBase_1_6 {

    std::shared_ptr<android::hardware::radio::impl::RadioSimResponseCallback> mSimCb;

    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioSimResponseCallback> simCb);

    ::android::hardware::Return<void>  areUiccApplicationsEnabledResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            bool enabled) override;
    ::android::hardware::Return<void>  changeIccPin2ForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            int remainingRetries) override;
    ::android::hardware::Return<void>  changeIccPinForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            int remainingRetries) override;
    ::android::hardware::Return<void>  enableUiccApplicationsResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info) override;
    ::android::hardware::Return<void>  getAllowedCarriersResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_4::CarrierRestrictionsWithPriority& carriers,
            ::android::hardware::radio::V1_4::SimLockMultiSimPolicy multiSimPolicy) override;
    ::android::hardware::Return<void>  getCDMASubscriptionResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_string& mdn,
            const ::android::hardware::hidl_string& hSid,
            const ::android::hardware::hidl_string& hNid,
            const ::android::hardware::hidl_string& min,
            const ::android::hardware::hidl_string& prl) override;
    ::android::hardware::Return<void>  getCdmaSubscriptionSourceResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            ::android::hardware::radio::V1_0::CdmaSubscriptionSource source) override;
    ::android::hardware::Return<void>  getFacilityLockForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int response) override;
    ::android::hardware::Return<void>  getIMSIForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_string& imsi) override;
    ::android::hardware::Return<void>  getSimPhonebookCapacityResponse(
            const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_6::PhonebookCapacity& capacity) override;
    ::android::hardware::Return<void>  getSimPhonebookRecordsResponse(
            const ::android::hardware::radio::V1_6::RadioResponseInfo& info) override;
    ::android::hardware::Return<void>  iccCloseLogicalChannelResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info) override;
    ::android::hardware::Return<void>  iccIOForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::IccIoResult& iccIo) override;
    ::android::hardware::Return<void>  iccOpenLogicalChannelResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int channelId,
            const ::android::hardware::hidl_vec<int8_t>& selectResponse) override;
    ::android::hardware::Return<void>  iccTransmitApduBasicChannelResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::IccIoResult& result) override;
    ::android::hardware::Return<void>  iccTransmitApduLogicalChannelResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::IccIoResult& result) override;
    ::android::hardware::Return<void>  reportStkServiceIsRunningResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info) override;
    ::android::hardware::Return<void>  requestIccSimAuthenticationResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::IccIoResult& result) override;
    ::android::hardware::Return<void>  sendEnvelopeResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_string& commandResponse) override;
    ::android::hardware::Return<void>  sendEnvelopeWithStatusResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::IccIoResult& iccIo) override;
    ::android::hardware::Return<void>  sendTerminalResponseToSimResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info) override;
    ::android::hardware::Return<void>  setAllowedCarriersResponse_1_4(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info) override;
    ::android::hardware::Return<void>  setCarrierInfoForImsiEncryptionResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info) override;
    ::android::hardware::Return<void>  setCdmaSubscriptionSourceResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info) override;
    ::android::hardware::Return<void>  setFacilityLockForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info, int32_t retry) override;
    ::android::hardware::Return<void>  setUiccSubscriptionResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info) override;
    ::android::hardware::Return<void>  supplyIccPin2ForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            int32_t remainingRetries) override;
    ::android::hardware::Return<void>  supplyIccPinForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            int32_t remainingRetries) override;
    ::android::hardware::Return<void>  supplyIccPuk2ForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            int32_t remainingRetries) override;
    ::android::hardware::Return<void>  supplyIccPukForAppResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            int32_t remainingRetries) override;
    ::android::hardware::Return<void>  supplySimDepersonalizationResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            ::android::hardware::radio::V1_5::PersoSubstate persoType,
            int32_t remainingRetries) override;
    ::android::hardware::Return<void>  updateSimPhonebookRecordsResponse(
            const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
            int32_t updatedRecordIndex) override;
    ::android::hardware::Return<void>  setSimCardPowerResponse_1_6(
            const ::android::hardware::radio::V1_6::RadioResponseInfo& info) override;
};

struct RadioSimIndicationHook : public RadioIndicationBase_1_6 {
    std::shared_ptr<android::hardware::radio::impl::RadioSimIndicationCallback> mSimCb;

    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioSimIndicationCallback> simCb);

    ::android::hardware::Return<void> carrierInfoForImsiEncryption(
            ::android::hardware::radio::V1_0::RadioIndicationType type) override;
    ::android::hardware::Return<void> cdmaSubscriptionSourceChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::CdmaSubscriptionSource cdmaSource) override;
    ::android::hardware::Return<void> simPhonebookChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType type) override;
    ::android::hardware::Return<void> simPhonebookRecordsReceived(
            ::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_6::PbReceivedStatus status,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::PhonebookRecordInfo>& records) override;
    ::android::hardware::Return<void> simRefresh(
            ::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::SimRefreshResult& refreshResult) override;
    ::android::hardware::Return<void> simStatusChanged(::android::hardware::radio::V1_0::RadioIndicationType type) override;
    ::android::hardware::Return<void> stkEventNotify(
            ::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_string& cmd) override;
    ::android::hardware::Return<void> stkProactiveCommand(
            ::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_string& cmd) override;
    ::android::hardware::Return<void> stkSessionEnd(::android::hardware::radio::V1_0::RadioIndicationType type) override;
    ::android::hardware::Return<void> subscriptionStatusChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType type, bool activate) override;
    ::android::hardware::Return<void> uiccApplicationsEnablementChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType type, bool enabled) override;

};

struct RadioSimResponse {
    ::android::sp<RadioSimResponseHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioSimResponseCallback> mSimCb;

    ::android::sp<RadioSimResponseHook> hidl() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioSimResponseCallback> simCb) {
        mSimCb = simCb;
        if (mHidl) {
            mHidl->setResponseFunction(mSimCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioSimResponse>();
        instance->mHidl = ::android::sp<RadioSimResponseHook>::make();
        return instance;
    }
};

struct RadioSimIndication {
    ::android::sp<RadioSimIndicationHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioSimIndicationCallback> mSimCb;

    ::android::sp<RadioSimIndicationHook> hidl() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioSimIndicationCallback> simCb) {
        mSimCb = simCb;
        if (mHidl) {
            mHidl->setResponseFunction(mSimCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioSimIndication>();
        instance->mHidl = ::android::sp<RadioSimIndicationHook>::make();
        return instance;
    }
};
