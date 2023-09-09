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
#include "radiomodemcallback.h"

struct RadioModemResponseHook : public RadioResponseBase_1_6 {

    std::shared_ptr<android::hardware::radio::impl::RadioModemResponseCallback> mModemCb;

    // modem
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioModemResponseCallback> modemCb);

    ::android::hardware::Return<void> enableModemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> getBasebandVersionResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                        const ::android::hardware::hidl_string& version);
    ::android::hardware::Return<void> getDeviceIdentityResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                    const ::android::hardware::hidl_string& imei,
                                    const ::android::hardware::hidl_string& imeisv,
                                    const ::android::hardware::hidl_string& esn,
                                    const ::android::hardware::hidl_string& meid);
    ::android::hardware::Return<void> getHardwareConfigResponse(
                                    const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                    const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::HardwareConfig>& config);
    ::android::hardware::Return<void> getModemActivityInfoResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                        const ::android::hardware::radio::V1_0::ActivityStatsInfo& activityInfo);
    ::android::hardware::Return<void> getModemStackStatusResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info, const bool enabled);
    ::android::hardware::Return<void> getRadioCapabilityResponse(
                                    const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                    const android::hardware::radio::V1_0::RadioCapability& rc);
    ::android::hardware::Return<void> nvReadItemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                    const ::android::hardware::hidl_string& result);
    ::android::hardware::Return<void> nvResetConfigResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> nvWriteCdmaPrlResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> nvWriteItemResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> requestShutdownResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> sendDeviceStateResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setRadioCapabilityResponse(
                                    const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                    const android::hardware::radio::V1_0::RadioCapability& rc);
    ::android::hardware::Return<void> setRadioPowerResponse(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setRadioPowerResponse_1_5(const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
    ::android::hardware::Return<void> setRadioPowerResponse_1_6(
                                    const ::android::hardware::radio::V1_6::RadioResponseInfo& info);
};

struct RadioModemIndicationHook : public RadioIndicationBase_1_6 {
    std::shared_ptr<android::hardware::radio::impl::RadioModemIndicationCallback> mModemCb;

    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioModemIndicationCallback> modemCb);
    ::android::hardware::Return<void> hardwareConfigChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::V1_0::HardwareConfig>& configs);
    ::android::hardware::Return<void> modemReset(::android::hardware::radio::V1_0::RadioIndicationType type,
                            const ::android::hardware::hidl_string& reason);
    ::android::hardware::Return<void> radioCapabilityIndication(
            ::android::hardware::radio::V1_0::RadioIndicationType type,
            const android::hardware::radio::V1_0::RadioCapability& rc);
    ::android::hardware::Return<void> radioStateChanged(
            ::android::hardware::radio::V1_0::RadioIndicationType type, ::android::hardware::radio::V1_0::RadioState radioState);
    ::android::hardware::Return<void> rilConnected(::android::hardware::radio::V1_0::RadioIndicationType type);
};

struct RadioModemResponse {
    ::android::sp<RadioModemResponseHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioModemResponseCallback> mModemCb;

    ::android::sp<RadioModemResponseHook> hidl() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioModemResponseCallback> modemCb) {
        mModemCb = modemCb;
        if (mHidl) {
            mHidl->setResponseFunction(mModemCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioModemResponse>();
        instance->mHidl = ::android::sp<RadioModemResponseHook>::make();
        return instance;
    }
};

struct RadioModemIndication {
    ::android::sp<RadioModemIndicationHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioModemIndicationCallback> mModemCb;

    ::android::sp<RadioModemIndicationHook> hidl() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioModemIndicationCallback> modemCb) {
        mModemCb = modemCb;
        if (mHidl) {
            mHidl->setResponseFunction(mModemCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioModemIndication>();
        instance->mHidl = ::android::sp<RadioModemIndicationHook>::make();
        return instance;
    }
};