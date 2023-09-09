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

namespace android::hardware::radio::impl {

struct RadioMessagingResponseCallback;
struct RadioMessagingIndicationCallback;

struct RadioMessagingCallback {
    virtual ~RadioMessagingCallback() = default;
    virtual void setResponseFunctions(
        const std::shared_ptr<RadioMessagingResponseCallback>& responseCb,
        const std::shared_ptr<RadioMessagingIndicationCallback>& indicationCb) = 0;
    virtual void acknowledgeIncomingGsmSmsWithPdu(int32_t serial, bool success,
                                                          const std::string& ackPdu) = 0;
    virtual void acknowledgeLastIncomingCdmaSms(int32_t serial,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsAck& smsAck) = 0;
    virtual void acknowledgeLastIncomingGsmSms(
            int32_t serial, bool success,
            ::aidl::android::hardware::radio::messaging::SmsAcknowledgeFailCause cause) = 0;
    virtual void deleteSmsOnRuim(int32_t serial, int32_t index) = 0;
    virtual void deleteSmsOnSim(int32_t serial, int32_t index) = 0;
    virtual void getCdmaBroadcastConfig(int32_t serial) = 0;
    virtual void getGsmBroadcastConfig(int32_t serial) = 0;
    virtual void getSmscAddress(int32_t serial) = 0;
    virtual void reportSmsMemoryStatus(int32_t serial, bool available) = 0;
    virtual void responseAcknowledgement() = 0;
    virtual void sendCdmaSms(int32_t serial,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage& sms) = 0;
    virtual void sendCdmaSmsExpectMore(int32_t serial,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage& sms) = 0;
    virtual void sendImsSms(int32_t serial,
            const ::aidl::android::hardware::radio::messaging::ImsSmsMessage& message) = 0;
    virtual void sendSms(int32_t serial,
            const ::aidl::android::hardware::radio::messaging::GsmSmsMessage& message) = 0;
    virtual void sendSmsExpectMore(int32_t serial,
            const ::aidl::android::hardware::radio::messaging::GsmSmsMessage& message) = 0;
    virtual void setCdmaBroadcastActivation(int32_t serial, bool activate) = 0;
    virtual void setCdmaBroadcastConfig(int32_t serial,
            const std::vector<::aidl::android::hardware::radio::messaging::CdmaBroadcastSmsConfigInfo>&
                    configInfo) = 0;
    virtual void setGsmBroadcastActivation(int32_t serial, bool activate) = 0;
    virtual void setGsmBroadcastConfig(int32_t serial,
            const std::vector<::aidl::android::hardware::radio::messaging::GsmBroadcastSmsConfigInfo>&
                    configInfo) = 0;
    virtual void setSmscAddress(int32_t serial, const std::string& smsc) = 0;
    virtual void writeSmsToRuim(int32_t serial,
            const ::aidl::android::hardware::radio::messaging::CdmaSmsWriteArgs& cdmaSms) = 0;
    virtual void writeSmsToSim(int32_t serial,
            const ::aidl::android::hardware::radio::messaging::SmsWriteArgs& smsWriteArgs) = 0;
};

struct RadioMessagingResponseCallback {
    virtual ~RadioMessagingResponseCallback() = default;
    virtual void setResponseFunction(const std::shared_ptr<::aidl::android::hardware::radio::messaging::IRadioMessagingResponse> &radioMessagingResponse)=0;
    virtual void acknowledgeIncomingGsmSmsWithPduResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void acknowledgeLastIncomingCdmaSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void acknowledgeLastIncomingGsmSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void acknowledgeRequest(int32_t serial) = 0;
    virtual void deleteSmsOnRuimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void deleteSmsOnSimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void getCdmaBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const std::vector<::aidl::android::hardware::radio::messaging::CdmaBroadcastSmsConfigInfo> &configs) = 0;
    virtual void getGsmBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const std::vector<::aidl::android::hardware::radio::messaging::GsmBroadcastSmsConfigInfo> &configs) = 0;
    virtual void getSmscAddressResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const std::string &smsc) = 0;
    virtual void reportSmsMemoryStatusResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void sendCdmaSmsExpectMoreResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) = 0;
    virtual void sendCdmaSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) = 0;
    virtual void sendImsSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) = 0;
    virtual void sendSmsExpectMoreResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) = 0;
    virtual void sendSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) = 0;
    virtual void setCdmaBroadcastActivationResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void setCdmaBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void setGsmBroadcastActivationResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void setGsmBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void setSmscAddressResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) = 0;
    virtual void writeSmsToRuimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, int32_t index) = 0;
    virtual void writeSmsToSimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, int32_t index) = 0;
};

struct RadioMessagingIndicationCallback {
    virtual ~RadioMessagingIndicationCallback() = default;
    virtual void setResponseFunction(
        const std::shared_ptr<::aidl::android::hardware::radio::messaging::IRadioMessagingIndication>
            &radioMessagingIndication)=0;
    virtual void cdmaNewSms(::aidl::android::hardware::radio::RadioIndicationType type, const ::aidl::android::hardware::radio::messaging::CdmaSmsMessage &msg) = 0;
    virtual void cdmaRuimSmsStorageFull(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
    virtual void newBroadcastSms(::aidl::android::hardware::radio::RadioIndicationType type, const std::vector<uint8_t> &data) = 0;
    virtual void newSms(::aidl::android::hardware::radio::RadioIndicationType type, const std::vector<uint8_t> &pdu) = 0;
    virtual void newSmsOnSim(::aidl::android::hardware::radio::RadioIndicationType type, int32_t recordNumber) = 0;
    virtual void newSmsStatusReport(::aidl::android::hardware::radio::RadioIndicationType type, const std::vector<uint8_t> &pdu) = 0;
    virtual void simSmsStorageFull(::aidl::android::hardware::radio::RadioIndicationType type) = 0;
};

}  // namespace android::hardware::radio::impl {
