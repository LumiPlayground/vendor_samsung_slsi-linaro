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

#include <radiomessagingcallback.h>
#include <radiocallbackadapter.h>
#include <radiointerface.h>

struct RadioMessagingProxy : public RadioInterface,
                             public android::hardware::radio::impl::RadioMessagingCallbackAdapter {
    using RadioInterface::RadioInterface;

    std::shared_ptr<android::hardware::radio::impl::RadioMessagingResponseCallback> mResponseCb;
    std::shared_ptr<android::hardware::radio::impl::RadioMessagingIndicationCallback> mIndicationCb;

    template <typename T, typename... Args>
    void onRequest(int request, Args &&...args) {
        auto command = getCommand<T>(request);
        if (command == nullptr) {
            command = std::make_shared<T>(mSlot, mResponseCb);
            RadioInterface::updateCommand(request, command);
        }
        RadioInterface::onRequest(command, args...);
    }

    template <typename T>
    void listen(int indication) {
        RadioInterface::updateIndication(indication,
                                         std::make_shared<T>(mSlot, mIndicationCb));
    }

    void setResponseFunctions(
        const std::shared_ptr<android::hardware::radio::impl::RadioMessagingResponseCallback> &responseCb,
        const std::shared_ptr<android::hardware::radio::impl::RadioMessagingIndicationCallback> &indicationCb);

    void acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success,
                                          const std::string &ackPdu) override;
    void acknowledgeLastIncomingCdmaSms(int32_t serial,
                                        const ::aidl::android::hardware::radio::messaging::CdmaSmsAck &smsAck) override;
    void acknowledgeLastIncomingGsmSms(
        int32_t serial, bool success,
        ::aidl::android::hardware::radio::messaging::SmsAcknowledgeFailCause cause) override;
    void deleteSmsOnRuim(int32_t serial, int32_t index) override;
    void deleteSmsOnSim(int32_t serial, int32_t index) override;
    void getCdmaBroadcastConfig(int32_t serial) override;
    void getGsmBroadcastConfig(int32_t serial) override;
    void getSmscAddress(int32_t serial) override;
    void reportSmsMemoryStatus(int32_t serial, bool available) override;
    void responseAcknowledgement() override;
    void sendCdmaSms(int32_t serial,
                     const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage &sms) override;
    void sendCdmaSmsExpectMore(int32_t serial,
                               const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage &sms) override;
    void sendImsSms(int32_t serial,
                    const ::aidl::android::hardware::radio::messaging::ImsSmsMessage &message) override;
    void sendSms(int32_t serial,
                 const ::aidl::android::hardware::radio::messaging::GsmSmsMessage &message) override;
    void sendSmsExpectMore(int32_t serial,
                           const ::aidl::android::hardware::radio::messaging::GsmSmsMessage &message) override;
    void setCdmaBroadcastActivation(int32_t serial, bool activate) override;
    void setCdmaBroadcastConfig(int32_t serial,
                                const std::vector<::aidl::android::hardware::radio::messaging::CdmaBroadcastSmsConfigInfo> &
                                    configInfo) override;
    void setGsmBroadcastActivation(int32_t serial, bool activate) override;
    void setGsmBroadcastConfig(int32_t serial,
                               const std::vector<::aidl::android::hardware::radio::messaging::GsmBroadcastSmsConfigInfo> &
                                   configInfo) override;
    void setSmscAddress(int32_t serial, const std::string &smsc) override;
    void writeSmsToRuim(int32_t serial,
                        const ::aidl::android::hardware::radio::messaging::CdmaSmsWriteArgs &cdmaSms) override;
    void writeSmsToSim(int32_t serial,
                       const ::aidl::android::hardware::radio::messaging::SmsWriteArgs &smsWriteArgs) override;

    static auto make(uint32_t slot, const std::shared_ptr<RadioProxy> &radio) {
        return std::make_shared<RadioMessagingProxy>(slot, radio);
    }
};
