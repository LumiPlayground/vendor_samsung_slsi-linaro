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
#include "radiomessagingcallback.h"

struct RadioMessagingResponseHook : public RadioResponseBase_1_6 {
    std::shared_ptr<android::hardware::radio::impl::RadioMessagingResponseCallback> mMessagingCb;

    void setResponseFunction(
        std::shared_ptr<android::hardware::radio::impl::RadioMessagingResponseCallback> messagingCb);

    ::android::hardware::Return<void> acknowledgeIncomingGsmSmsWithPduResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> acknowledgeLastIncomingCdmaSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> acknowledgeLastIncomingGsmSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> deleteSmsOnRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> deleteSmsOnSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> getCdmaBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CdmaBroadcastSmsConfigInfo> &configs);
    ::android::hardware::Return<void> getGsmBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::GsmBroadcastSmsConfigInfo> &configs);
    ::android::hardware::Return<void> getSmscAddressResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, const ::android::hardware::hidl_string &smsc);
    ::android::hardware::Return<void> reportSmsMemoryStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> sendCdmaSmsExpectMoreResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, const ::android::hardware::radio::V1_0::SendSmsResult &sms);
    ::android::hardware::Return<void> sendCdmaSmsExpectMoreResponse_1_6(const ::android::hardware::radio::V1_6::RadioResponseInfo &info, const ::android::hardware::radio::V1_0::SendSmsResult &sms);
    ::android::hardware::Return<void> sendCdmaSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, const ::android::hardware::radio::V1_0::SendSmsResult &sms);
    ::android::hardware::Return<void> sendCdmaSmsResponse_1_6(const ::android::hardware::radio::V1_6::RadioResponseInfo &info, const ::android::hardware::radio::V1_0::SendSmsResult &sms);
    ::android::hardware::Return<void> sendImsSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, const ::android::hardware::radio::V1_0::SendSmsResult &sms);
    ::android::hardware::Return<void> sendSMSExpectMoreResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, const ::android::hardware::radio::V1_0::SendSmsResult &sms);
    ::android::hardware::Return<void> sendSmsExpectMoreResponse_1_6(const ::android::hardware::radio::V1_6::RadioResponseInfo &info, const ::android::hardware::radio::V1_0::SendSmsResult &sms);
    ::android::hardware::Return<void> sendSmsResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, const ::android::hardware::radio::V1_0::SendSmsResult &sms);
    ::android::hardware::Return<void> sendSmsResponse_1_6(const ::android::hardware::radio::V1_6::RadioResponseInfo &info, const ::android::hardware::radio::V1_0::SendSmsResult &sms);
    ::android::hardware::Return<void> setCdmaBroadcastActivationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> setCdmaBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> setGsmBroadcastActivationResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> setGsmBroadcastConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> setSmscAddressResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info);
    ::android::hardware::Return<void> writeSmsToRuimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, uint32_t index);
    ::android::hardware::Return<void> writeSmsToSimResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo &info, int32_t index);
};

struct RadioMessagingIndicationHook : public RadioIndicationBase_1_6 {
    std::shared_ptr<android::hardware::radio::impl::RadioMessagingIndicationCallback> mMessagingCb;

    void setResponseFunction(
        std::shared_ptr<android::hardware::radio::impl::RadioMessagingIndicationCallback> messagingCb);

    ::android::hardware::Return<void> cdmaNewSms(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_0::CdmaSmsMessage &msg);
    ::android::hardware::Return<void> cdmaRuimSmsStorageFull(::android::hardware::radio::V1_0::RadioIndicationType type);
    ::android::hardware::Return<void> newBroadcastSms(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<uint8_t> &data);
    ::android::hardware::Return<void> newSms(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<uint8_t> &pdu);
    ::android::hardware::Return<void> newSmsOnSim(::android::hardware::radio::V1_0::RadioIndicationType type, int32_t recordNumber);
    ::android::hardware::Return<void> newSmsStatusReport(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::hidl_vec<uint8_t> &pdu);
    ::android::hardware::Return<void> simSmsStorageFull(::android::hardware::radio::V1_0::RadioIndicationType type);
};

struct RadioMessagingResponse {
    ::android::sp<RadioMessagingResponseHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioMessagingResponseCallback> mMessagingCb;

    ::android::sp<RadioMessagingResponseHook> hidl() {
        return mHidl;
    }
    void setResponseFunction(
        std::shared_ptr<android::hardware::radio::impl::RadioMessagingResponseCallback> messagingCb) {
        mMessagingCb = messagingCb;
        if (mHidl) {
            mHidl->setResponseFunction(mMessagingCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioMessagingResponse>();
        instance->mHidl = ::android::sp<RadioMessagingResponseHook>::make();
        return instance;
    }
};

struct RadioMessagingIndication {
    ::android::sp<RadioMessagingIndicationHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioMessagingIndicationCallback> mMessagingCb;

    ::android::sp<RadioMessagingIndicationHook> hidl() {
        return mHidl;
    }
    void setResponseFunction(
        std::shared_ptr<android::hardware::radio::impl::RadioMessagingIndicationCallback> messagingCb) {
        mMessagingCb = messagingCb;
        if (mHidl) {
            mHidl->setResponseFunction(mMessagingCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioMessagingIndication>();
        instance->mHidl = ::android::sp<RadioMessagingIndicationHook>::make();
        return instance;
    }
};
