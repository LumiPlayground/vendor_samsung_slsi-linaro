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
#include <radiovoicecallback.h>

struct RadioVoiceResponseHook : public RadioResponseBase_1_6 {

    std::shared_ptr<android::hardware::radio::impl::RadioVoiceResponseCallback> mVoiceCb;

    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioVoiceResponseCallback> voiceCb);

    ::android::hardware::Return<void> acceptCallResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> cancelPendingUssdResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> conferenceResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> dialResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> emergencyDialResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> exitEmergencyCallbackModeResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> explicitCallTransferResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> getCallForwardStatusResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::CallForwardInfo>& callForwardInfos);
    ::android::hardware::Return<void> getCallWaitingResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            bool enable, int32_t serviceClass);
    ::android::hardware::Return<void> getClipResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            ::android::hardware::radio::V1_0::ClipStatus status);
    ::android::hardware::Return<void> getClirResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            int32_t n, int32_t m);
    ::android::hardware::Return<void> getCurrentCallsResponse_1_6(
            const ::android::hardware::radio::V1_6::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_6::Call>& calls);
    ::android::hardware::Return<void> getLastCallFailCauseResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::V1_0::LastCallFailCauseInfo& failCauseinfo);
    ::android::hardware::Return<void> getMuteResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            bool enable);
    ::android::hardware::Return<void> getPreferredVoicePrivacyResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            bool enable);
    ::android::hardware::Return<void> getTTYModeResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            ::android::hardware::radio::V1_0::TtyMode mode);
    ::android::hardware::Return<void> handleStkCallSetupRequestFromSimResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> hangupConnectionResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> hangupForegroundResumeBackgroundResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> hangupWaitingOrBackgroundResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> rejectCallResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> sendBurstDtmfResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> sendCDMAFeatureCodeResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> sendDtmfResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> sendUssdResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> separateConnectionResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setCallForwardResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setCallWaitingResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setClirResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setMuteResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setPreferredVoicePrivacyResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setTTYModeResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> startDtmfResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> stopDtmfResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> switchWaitingOrHoldingAndActiveResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
};

struct RadioVoiceIndicationHook : public RadioIndicationBase_1_6 {
    std::shared_ptr<android::hardware::radio::impl::RadioVoiceIndicationCallback> mVoiceCb;

    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioVoiceIndicationCallback> voiceCb);
    ::android::hardware::Return<void> callRing(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            bool isGsm,
            const ::android::hardware::radio::V1_0::CdmaSignalInfoRecord& record);
    ::android::hardware::Return<void> callStateChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType type);
    ::android::hardware::Return<void> cdmaCallWaiting(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_0::CdmaCallWaiting& callWaitingRecord);
    ::android::hardware::Return<void> cdmaInfoRec(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_0::CdmaInformationRecords& records);
    ::android::hardware::Return<void> cdmaOtaProvisionStatus(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            ::android::hardware::radio::V1_0::CdmaOtaProvisionStatus status);
    ::android::hardware::Return<void> currentEmergencyNumberList(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_4::EmergencyNumber>&
            emergencyNumberList);
    ::android::hardware::Return<void> enterEmergencyCallbackMode(
            ::android::hardware::radio::V1_0::RadioIndicationType type);
    ::android::hardware::Return<void> exitEmergencyCallbackMode(
            ::android::hardware::radio::V1_0::RadioIndicationType type);
    ::android::hardware::Return<void> indicateRingbackTone(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            bool start);
    ::android::hardware::Return<void> onSupplementaryServiceIndication(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::radio::V1_0::StkCcUnsolSsResult& ss);
    ::android::hardware::Return<void> onUssd(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            ::android::hardware::radio::V1_0::UssdModeType modeType,
            const ::android::hardware::hidl_string& msg);
    ::android::hardware::Return<void> resendIncallMute(
            ::android::hardware::radio::V1_0::RadioIndicationType type);
    ::android::hardware::Return<void> srvccStateNotify(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            ::android::hardware::radio::V1_0::SrvccState state);
    ::android::hardware::Return<void> stkCallControlAlphaNotify(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_string& alpha);
    ::android::hardware::Return<void> stkCallSetup(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            int64_t timeout);
};

struct RadioVoiceResponse {
    ::android::sp<RadioVoiceResponseHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioVoiceResponseCallback> mVoiceCb;

    ::android::sp<RadioVoiceResponseHook> hidl() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioVoiceResponseCallback> voiceCb) {
        mVoiceCb = voiceCb;
        if (mHidl) {
            mHidl->setResponseFunction(mVoiceCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioVoiceResponse>();
        instance->mHidl = ::android::sp<RadioVoiceResponseHook>::make();
        return instance;
    }
};

struct RadioVoiceIndication {
    ::android::sp<RadioVoiceIndicationHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioVoiceIndicationCallback> mVoiceCb;

    ::android::sp<RadioVoiceIndicationHook> hidl() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioVoiceIndicationCallback> voiceCb) {
        mVoiceCb = voiceCb;
        if (mHidl) {
            mHidl->setResponseFunction(mVoiceCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioVoiceIndication>();
        instance->mHidl = ::android::sp<RadioVoiceIndicationHook>::make();
        return instance;
    }
};
