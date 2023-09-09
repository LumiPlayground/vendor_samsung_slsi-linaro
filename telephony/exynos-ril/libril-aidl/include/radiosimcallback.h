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

#include <aidl/android/hardware/radio/sim/BnRadioSim.h>

namespace android::hardware::radio::impl {

struct RadioSimResponseCallback;
struct RadioSimIndicationCallback;

struct RadioSimCallback {
    virtual ~RadioSimCallback() = default;
    virtual void setResponseFunctions(
            const std::shared_ptr<RadioSimResponseCallback>& responseCb,
            const std::shared_ptr<RadioSimIndicationCallback>& indicationCb) = 0;
    virtual void areUiccApplicationsEnabled(int32_t serial) = 0;
    virtual void changeIccPin2ForApp(int32_t serial, const std::string& oldPin2,
                                             const std::string& newPin2,
                                             const std::string& aid) = 0;
    virtual void changeIccPinForApp(int32_t serial, const std::string& oldPin,
                                            const std::string& newPin,
                                            const std::string& aid) = 0;
    virtual void enableUiccApplications(int32_t serial, bool enable) = 0;
    virtual void getAllowedCarriers(int32_t serial) = 0;
    virtual void getCdmaSubscription(int32_t serial) = 0;
    virtual void getCdmaSubscriptionSource(int32_t serial) = 0;
    virtual void getFacilityLockForApp(int32_t serial, const std::string& facility,
                                               const std::string& password, int32_t serviceClass,
                                               const std::string& appId) = 0;
    virtual void getIccCardStatus(int32_t serial) = 0;
    virtual void getImsiForApp(int32_t serial, const std::string& aid) = 0;
    virtual void getSimPhonebookCapacity(int32_t serial) = 0;
    virtual void getSimPhonebookRecords(int32_t serial) = 0;
    virtual void iccCloseLogicalChannel(int32_t serial, int32_t channelId) = 0;
    virtual void iccIoForApp(int32_t serial,
            const ::aidl::android::hardware::radio::sim::IccIo& iccIo) = 0;
    virtual void iccOpenLogicalChannel(int32_t serial, const std::string& aid,
                                               int32_t p2) = 0;
    virtual void iccTransmitApduBasicChannel(
            int32_t serial, const ::aidl::android::hardware::radio::sim::SimApdu& message) = 0;
    virtual void iccTransmitApduLogicalChannel(
            int32_t serial, const ::aidl::android::hardware::radio::sim::SimApdu& message) = 0;
    virtual void reportStkServiceIsRunning(int32_t serial) = 0;
    virtual void requestIccSimAuthentication(int32_t serial, int32_t authContext,
                                                     const std::string& authData,
                                                     const std::string& aid) = 0;
    virtual void responseAcknowledgement() = 0;
    virtual void sendEnvelope(int32_t serial, const std::string& command) = 0;
    virtual void sendEnvelopeWithStatus(int32_t serial,
                                                const std::string& contents) = 0;
    virtual void sendTerminalResponseToSim(int32_t serial,
                                                   const std::string& commandResponse) = 0;
    virtual void setAllowedCarriers(int32_t serial,
            const ::aidl::android::hardware::radio::sim::CarrierRestrictions& carriers,
            ::aidl::android::hardware::radio::sim::SimLockMultiSimPolicy multiSimPolicy) = 0;
    virtual void setCarrierInfoForImsiEncryption(int32_t serial,
            const ::aidl::android::hardware::radio::sim::ImsiEncryptionInfo& imsiEncryptionInfo)
            = 0;
    virtual void setCdmaSubscriptionSource(int32_t serial,
            ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource cdmaSub) = 0;
    virtual void setFacilityLockForApp(  //
            int32_t serial, const std::string& facility, bool lockState, const std::string& passwd,
            int32_t serviceClass, const std::string& appId) = 0;
    virtual void setSimCardPower(
            int32_t serial, ::aidl::android::hardware::radio::sim::CardPowerState powerUp) = 0;
    virtual void setUiccSubscription(int32_t serial,
            const ::aidl::android::hardware::radio::sim::SelectUiccSub& uiccSub) = 0;
    virtual void supplyIccPin2ForApp(int32_t serial, const std::string& pin2,
                                             const std::string& aid) = 0;
    virtual void supplyIccPinForApp(int32_t serial, const std::string& pin,
                                            const std::string& aid) = 0;
    virtual void supplyIccPuk2ForApp(int32_t serial, const std::string& puk2,
                                             const std::string& pin2,
                                             const std::string& aid) = 0;
    virtual void supplyIccPukForApp(int32_t serial, const std::string& puk,
                                            const std::string& pin,
                                            const std::string& aid) = 0;
    virtual void supplySimDepersonalization(
            int32_t serial, ::aidl::android::hardware::radio::sim::PersoSubstate persoType,
            const std::string& controlKey) = 0;
    virtual void updateSimPhonebookRecords(int32_t serial,
            const ::aidl::android::hardware::radio::sim::PhonebookRecordInfo& recordInfo) = 0;
};

struct RadioSimResponseCallback {
    virtual ~RadioSimResponseCallback() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimResponse>&
                    radioSimResponse)=0;
    virtual void acknowledgeRequest(int32_t serial)=0;
    virtual void areUiccApplicationsEnabledResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enabled) = 0;
    virtual void changeIccPin2ForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info, int remainingRetries) = 0;
    virtual void changeIccPinForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info, int remainingRetries) = 0;
    virtual void enableUiccApplicationsResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void getAllowedCarriersResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::CarrierRestrictions& carriers,
            const ::aidl::android::hardware::radio::sim::SimLockMultiSimPolicy& multiSimPolicy) = 0;
    virtual void getCdmaSubscriptionResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::string& mdn, const std::string& hSid, const std::string& hNid,
            const std::string& min, const std::string& prl) = 0;
    virtual void getCdmaSubscriptionSourceResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource source) = 0;
    virtual void getFacilityLockForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int response) = 0;
    virtual void getIccCardStatusResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::CardStatus& cardStatus) = 0;
    virtual void getIMSIForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::string& imsi) = 0;
    virtual void getSimPhonebookCapacityResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::PhonebookCapacity& capacity) = 0;
    virtual void getSimPhonebookRecordsResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void iccCloseLogicalChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void iccIoForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& iccIo) = 0;
    virtual void iccOpenLogicalChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info, int channelId,
            const std::vector<uint8_t>& selectResponse) = 0;
    virtual void iccTransmitApduBasicChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& result) = 0;
    virtual void iccTransmitApduLogicalChannelResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& result) = 0;
    virtual void reportStkServiceIsRunningResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void requestIccSimAuthenticationResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& result) = 0;
    virtual void sendEnvelopeResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::string& commandResponse) = 0;
    virtual void sendEnvelopeWithStatusResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::IccIoResult& iccIo) = 0;
    virtual void sendTerminalResponseToSimResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setAllowedCarriersResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setCarrierInfoForImsiEncryptionResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setCdmaSubscriptionSourceResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setFacilityLockForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info, int32_t retry) = 0;
    virtual void setSimCardPowerResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void setUiccSubscriptionResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) = 0;
    virtual void supplyIccPin2ForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t remainingRetries) = 0;
    virtual void supplyIccPinForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t remainingRetries) = 0;
    virtual void supplyIccPuk2ForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t remainingRetries) = 0;
    virtual void supplyIccPukForAppResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t remainingRetries) = 0;
    virtual void supplySimDepersonalizationResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::sim::PersoSubstate persoType,
            int32_t remainingRetries) = 0;
    virtual void updateSimPhonebookRecordsResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int32_t updatedRecordIndex) = 0;
};

struct RadioSimIndicationCallback {
    virtual ~RadioSimIndicationCallback() = default;
    virtual void setResponseFunction(
        const std::shared_ptr<::aidl::android::hardware::radio::sim::IRadioSimIndication>&
                radioSimIndication)=0;
    virtual void carrierInfoForImsiEncryption(
            ::aidl::android::hardware::radio::RadioIndicationType info) = 0;
    virtual void cdmaSubscriptionSourceChanged(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource cdmaSource) = 0;
    virtual void simPhonebookChanged(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void simPhonebookRecordsReceived(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::sim::PbReceivedStatus status,
            const std::vector<::aidl::android::hardware::radio::sim::PhonebookRecordInfo>& records) = 0;
    virtual void simRefresh(
            ::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::sim::SimRefreshResult& refreshResult) = 0;
    virtual void simStatusChanged(
            ::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void stkEventNotify(
            ::aidl::android::hardware::radio::RadioIndicationType type, const std::string& cmd) = 0;
    virtual void stkProactiveCommand(
            ::aidl::android::hardware::radio::RadioIndicationType type, const std::string& cmd) = 0;
    virtual void stkSessionEnd(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void subscriptionStatusChanged(
            ::aidl::android::hardware::radio::RadioIndicationType type, bool activate) = 0;
    virtual void uiccApplicationsEnablementChanged(
            ::aidl::android::hardware::radio::RadioIndicationType type, bool enabled) = 0;
};

}  // namespace android::hardware::radio::impl
