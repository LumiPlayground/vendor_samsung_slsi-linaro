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

#include "radiomessagingcallback.h"
#include "radioreturnstatus.h"
#include "radioresponseutil.h"
#include "binderdeathrecipient.h"

namespace android::hardware::radio::impl {
struct RadioMessagingResponse : public RadioMessagingResponseCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::messaging::IRadioMessagingResponse> mResponse;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
    }
    void setResponseFunction(const std::shared_ptr<::aidl::android::hardware::radio::messaging::IRadioMessagingResponse> &radioMessagingResponse);

    void acknowledgeIncomingGsmSmsWithPduResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void acknowledgeLastIncomingCdmaSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void acknowledgeLastIncomingGsmSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void acknowledgeRequest(int32_t serial) override;
    void deleteSmsOnRuimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void deleteSmsOnSimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void getCdmaBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const std::vector<::aidl::android::hardware::radio::messaging::CdmaBroadcastSmsConfigInfo> &configs) override;
    void getGsmBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const std::vector<::aidl::android::hardware::radio::messaging::GsmBroadcastSmsConfigInfo> &configs) override;
    void getSmscAddressResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const std::string &smsc) override;
    void reportSmsMemoryStatusResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void sendCdmaSmsExpectMoreResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) override;
    void sendCdmaSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) override;
    void sendImsSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) override;
    void sendSmsExpectMoreResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) override;
    void sendSmsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, const ::aidl::android::hardware::radio::messaging::SendSmsResult &sms) override;
    void setCdmaBroadcastActivationResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void setCdmaBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void setGsmBroadcastActivationResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void setGsmBroadcastConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void setSmscAddressResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) override;
    void writeSmsToRuimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, int32_t index) override;
    void writeSmsToSimResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info, int32_t index) override;
};

}  // namespace android::hardware::radio::impl
