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

#include <radiocallbackadapter.h>
#include <radiointerface.h>
#include <basecommand.h>
#include <ril_internal.h>

#include <map>

struct RadioSimProxy : public RadioInterface,
        public android::hardware::radio::impl::RadioSimCallbackAdapter {
    using RadioInterface::RadioInterface;

    std::shared_ptr<android::hardware::radio::impl::RadioSimResponseCallback> mResponseCb;
    std::shared_ptr<android::hardware::radio::impl::RadioSimIndicationCallback> mIndicationCb;

    template<typename T, typename... Args>
    void onRequest(int request, Args&&... args) {
        auto command = getCommand<T>(request);
        if (command == nullptr) {
            command = std::make_shared<T>(mSlot, mResponseCb);
            RadioInterface::updateCommand(request, command);
        }
        RadioInterface::onRequest(command, args...);
    }

    template<typename T>
    void listen(int indication) {
        RadioInterface::updateIndication(indication,
                std::make_shared<T>(mSlot, mIndicationCb));
    }

    void setResponseFunctions(
        const std::shared_ptr<android::hardware::radio::impl::RadioSimResponseCallback>&
                responseCb,
        const std::shared_ptr<android::hardware::radio::impl::RadioSimIndicationCallback>&
                indicationCb);

    void areUiccApplicationsEnabled(int32_t serial) override;
    void changeIccPin2ForApp(int32_t serial, const std::string& oldPin2,
            const std::string& newPin2, const std::string& aid) override;
    void changeIccPinForApp(int32_t serial, const std::string& oldPin,
            const std::string& newPin, const std::string& aid) override;
    void enableUiccApplications(int32_t serial, bool enable) override;
    void getAllowedCarriers(int32_t serial) override;
    void getCdmaSubscription(int32_t serial) override;
    void getCdmaSubscriptionSource(int32_t serial) override;
    void getFacilityLockForApp(int32_t serial, const std::string& facility,
            const std::string& password, int32_t serviceClass, const std::string& appId) override;
    void getIccCardStatus(int32_t serial) override;
    void getImsiForApp(int32_t serial, const std::string& aid) override;
    void getSimPhonebookCapacity(int32_t serial) override;
    void getSimPhonebookRecords(int32_t serial) override;
    void iccCloseLogicalChannel(int32_t serial, int32_t channelId) override;
    void iccIoForApp(
            int32_t serial, const ::aidl::android::hardware::radio::sim::IccIo& iccIo) override;
    void iccOpenLogicalChannel(int32_t serial, const std::string& aid,
            int32_t p2) override;
    void iccTransmitApduBasicChannel(int32_t serial,
            const ::aidl::android::hardware::radio::sim::SimApdu& message) override;
    void iccTransmitApduLogicalChannel(int32_t serial,
            const ::aidl::android::hardware::radio::sim::SimApdu& message) override;
    void reportStkServiceIsRunning(int32_t serial) override;
    void requestIccSimAuthentication(int32_t serial, int32_t authContext,
            const std::string& authData, const std::string& aid) override;
    void responseAcknowledgement() override;
    void sendEnvelope(int32_t serial, const std::string& command) override;
    void sendEnvelopeWithStatus(int32_t serial, const std::string& contents) override;
    void sendTerminalResponseToSim(int32_t serial,
            const std::string& commandResponse) override;
    void setAllowedCarriers(int32_t serial,
            const ::aidl::android::hardware::radio::sim::CarrierRestrictions& carriers,
            ::aidl::android::hardware::radio::sim::SimLockMultiSimPolicy multiSimPolicy) override;
    void setCarrierInfoForImsiEncryption(int32_t serial,
            const ::aidl::android::hardware::radio::sim::ImsiEncryptionInfo& imsiEncryptionInfo
            ) override;
    void setCdmaSubscriptionSource(int32_t serial,
            ::aidl::android::hardware::radio::sim::CdmaSubscriptionSource cdmaSub) override;
    void setFacilityLockForApp(int32_t serial, const std::string& facility,
            bool lockState, const std::string& passwd, int32_t serviceClass,
            const std::string& appId) override;
    void setSimCardPower(int32_t serial,
            ::aidl::android::hardware::radio::sim::CardPowerState powerUp) override;
    void setUiccSubscription(int32_t serial,
            const ::aidl::android::hardware::radio::sim::SelectUiccSub& uiccSub) override;
    void supplyIccPin2ForApp(int32_t serial, const std::string& pin2,
            const std::string& aid) override;
    void supplyIccPinForApp(int32_t serial, const std::string& pin,
            const std::string& aid) override;
    void supplyIccPuk2ForApp(int32_t serial, const std::string& puk2,
            const std::string& pin2, const std::string& aid) override;
    void supplyIccPukForApp(int32_t serial, const std::string& puk,
            const std::string& pin, const std::string& aid) override;
    void supplySimDepersonalization(int32_t serial,
            ::aidl::android::hardware::radio::sim::PersoSubstate persoType,
            const std::string& controlKey) override;
    void updateSimPhonebookRecords(int32_t serial,
            const ::aidl::android::hardware::radio::sim::PhonebookRecordInfo& recordInfo) override;

    static auto make(uint32_t slot, const std::shared_ptr<RadioProxy>& radio) {
        return std::make_shared<RadioSimProxy>(slot, radio);
    }
};