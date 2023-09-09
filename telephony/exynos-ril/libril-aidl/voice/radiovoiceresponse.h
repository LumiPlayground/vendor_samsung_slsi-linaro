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

#include "radiovoicecallback.h"
#include "radioreturnstatus.h"
#include "radioresponseutil.h"
#include "binderdeathrecipient.h"

namespace android::hardware::radio::impl {

struct RadioVoiceResponse : public RadioVoiceResponseCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceResponse> mResponse;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
    }
    void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceResponse>&
            radioVoiceResponse);

    void acceptCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void acknowledgeRequest(int32_t serial) override;

    void cancelPendingUssdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void conferenceResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void dialResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void emergencyDialResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void exitEmergencyCallbackModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)
        override;

    void explicitCallTransferResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void getCallForwardStatusResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::vector<::aidl::android::hardware::radio::voice::CallForwardInfo>& callForwardInfos)
        override;

    void getCallWaitingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enable,
            int32_t serviceClass) override;

    void getClipResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            ::aidl::android::hardware::radio::voice::ClipStatus status) override;

    void getClirResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info, int32_t n, int32_t m)
        override;

    void getCurrentCallsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::vector<::aidl::android::hardware::radio::voice::Call>& calls) override;

    void getLastCallFailCauseResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::voice::LastCallFailCauseInfo& failCauseinfo) override;

    void getMuteResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enable) override;

    void getPreferredVoicePrivacyResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            bool enable) override;

    void getTtyModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            ::aidl::android::hardware::radio::voice::TtyMode mode) override;

    void handleStkCallSetupRequestFromSimResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void hangupConnectionResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void hangupForegroundResumeBackgroundResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void hangupWaitingOrBackgroundResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)
        override;

    void isVoNrEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enable)
        override;

    void rejectCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void sendBurstDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void sendCdmaFeatureCodeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void sendDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void sendUssdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void separateConnectionResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void setCallForwardResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void setCallWaitingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void setClirResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void setMuteResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void setPreferredVoicePrivacyResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)
        override;

    void setTtyModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void setVoNrEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void startDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void stopDtmfResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;

    void switchWaitingOrHoldingAndActiveResponse(
            const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
};

}
