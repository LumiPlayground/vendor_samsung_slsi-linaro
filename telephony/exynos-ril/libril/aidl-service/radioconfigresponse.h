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
#include <hidl/config/radio_config_hidl_utils_1_2.h>
#include <telephony/ril.h>
#include "radioconfigcallback.h"

struct RadioConfigResponseHook : public RadioConfigResponseBase_1_2 {

    std::shared_ptr<android::hardware::radio::impl::RadioConfigResponseCallback> mConfigCb;

    // config
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioConfigResponseCallback> configCb);

    ::android::hardware::Return<void> getModemsConfigResponse(
                                    const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                    const ::android::hardware::radio::config::V1_1::ModemsConfig& modemsConfig);

    ::android::hardware::Return<void> getPhoneCapabilityResponse(
                                    const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
                                    const ::android::hardware::radio::config::V1_1::PhoneCapability& phoneCapability);

    ::android::hardware::Return<void> setModemsConfigResponse(
                                    const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    ::android::hardware::Return<void> setPreferredDataModemResponse(
                                    const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
};

struct RadioConfigIndicationHook : public RadioConfigIndicationBase_1_2 {
    std::shared_ptr<android::hardware::radio::impl::RadioConfigIndicationCallback> mConfigCb;

    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioConfigIndicationCallback> configCb);
};

struct RadioConfigResponse {
    ::android::sp<RadioConfigResponseHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioConfigResponseCallback> mConfigCb;

    ::android::sp<RadioConfigResponseHook> hidlConfig() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioConfigResponseCallback> configCb) {
        mConfigCb = configCb;
        if (mHidl) {
            mHidl->setResponseFunction(mConfigCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioConfigResponse>();
        instance->mHidl = ::android::sp<RadioConfigResponseHook>::make();
        return instance;
    }
};

struct RadioConfigIndication {
    ::android::sp<RadioConfigIndicationHook> mHidl;
    std::shared_ptr<android::hardware::radio::impl::RadioConfigIndicationCallback> mConfigCb;

    ::android::sp<RadioConfigIndicationHook> hidlConfig() { return mHidl; }
    void setResponseFunction(
            std::shared_ptr<android::hardware::radio::impl::RadioConfigIndicationCallback> configCb) {
        mConfigCb = configCb;
        if (mHidl) {
            mHidl->setResponseFunction(mConfigCb);
        }
    }

    static auto make() {
        auto instance = std::make_shared<RadioConfigIndication>();
        instance->mHidl = ::android::sp<RadioConfigIndicationHook>::make();
        return instance;
    }
};