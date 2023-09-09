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

#include <aidl/android/hardware/radio/messaging/BnRadioMessaging.h>
#include <radiocallbackmanager.h>
#include <radiomessagingcallback.h>

namespace android::hardware::radio::impl {
struct RadioMessaging : public aidl::android::hardware::radio::messaging::BnRadioMessaging {
    std::shared_ptr<RadioMessagingCallback> mRadio;
    RadioCallbackManager<RadioMessagingCallback, RadioMessagingResponseCallback,
                RadioMessagingIndicationCallback> mCallbacks;

    RadioMessaging(std::shared_ptr<RadioMessagingCallback> radio, uint32_t slot);

    ::ndk::ScopedAStatus acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success,
                                                          const std::string& ackPdu) override;
    ::ndk::ScopedAStatus acknowledgeLastIncomingCdmaSms(
            int32_t serial,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsAck& smsAck) override;
    ::ndk::ScopedAStatus acknowledgeLastIncomingGsmSms(
            int32_t serial, bool success,
            ::aidl::android::hardware::radio::messaging::SmsAcknowledgeFailCause cause) override;
    ::ndk::ScopedAStatus deleteSmsOnRuim(int32_t serial, int32_t index) override;
    ::ndk::ScopedAStatus deleteSmsOnSim(int32_t serial, int32_t index) override;
    ::ndk::ScopedAStatus getCdmaBroadcastConfig(int32_t serial) override;
    ::ndk::ScopedAStatus getGsmBroadcastConfig(int32_t serial) override;
    ::ndk::ScopedAStatus getSmscAddress(int32_t serial) override;
    ::ndk::ScopedAStatus reportSmsMemoryStatus(int32_t serial, bool available) override;
    ::ndk::ScopedAStatus responseAcknowledgement() override;
    ::ndk::ScopedAStatus sendCdmaSms(
            int32_t serial,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage& sms) override;
    ::ndk::ScopedAStatus sendCdmaSmsExpectMore(
            int32_t serial,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage& sms) override;
    ::ndk::ScopedAStatus sendImsSms(
            int32_t serial,
            const ::aidl::android::hardware::radio::messaging::ImsSmsMessage& message) override;
    ::ndk::ScopedAStatus sendSms(
            int32_t serial,
            const ::aidl::android::hardware::radio::messaging::GsmSmsMessage& message) override;
    ::ndk::ScopedAStatus sendSmsExpectMore(
            int32_t serial,
            const ::aidl::android::hardware::radio::messaging::GsmSmsMessage& message) override;
    ::ndk::ScopedAStatus setCdmaBroadcastActivation(int32_t serial, bool activate) override;
    ::ndk::ScopedAStatus setCdmaBroadcastConfig(
            int32_t serial,
            const std::vector<
                    ::aidl::android::hardware::radio::messaging::CdmaBroadcastSmsConfigInfo>&
                    configInfo) override;
    ::ndk::ScopedAStatus setGsmBroadcastActivation(int32_t serial, bool activate) override;
    ::ndk::ScopedAStatus setGsmBroadcastConfig(
            int32_t serial,
            const std::vector<
                    ::aidl::android::hardware::radio::messaging::GsmBroadcastSmsConfigInfo>&
                    configInfo) override;
    ::ndk::ScopedAStatus setSmscAddress(int32_t serial, const std::string& smsc) override;
    ::ndk::ScopedAStatus writeSmsToRuim(
            int32_t serial,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsWriteArgs& cdmaSms) override;
    ::ndk::ScopedAStatus writeSmsToSim(
            int32_t serial,
            const ::aidl::android::hardware::radio::messaging::SmsWriteArgs& smsWriteArgs) override;

    ::ndk::ScopedAStatus setResponseFunctions(
        const std::shared_ptr<
            ::aidl::android::hardware::radio::messaging::IRadioMessagingResponse> &
            radioMessagingResponse,
        const std::shared_ptr<
            ::aidl::android::hardware::radio::messaging::IRadioMessagingIndication> &
            radioMessagingIndication) override;
    static std::shared_ptr<RadioMessaging> make(std::shared_ptr<RadioMessagingCallback> radio, uint32_t slot);
};

}
