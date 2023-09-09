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

#include <android/hardware/radio/config/1.1/IRadioConfig.h>

struct TestRadioConfigBase_1_1 : public ::android::hardware::radio::config::V1_1::IRadioConfig {
    virtual ::android::hardware::Return<void> setResponseFunctions(const ::android::sp<::android::hardware::radio::config::V1_0::IRadioConfigResponse>& /* radioConfigResponse */,
            const ::android::sp<::android::hardware::radio::config::V1_0::IRadioConfigIndication>& /* radioConfigIndication */) override { return {}; }
    virtual ::android::hardware::Return<void> getSimSlotsStatus(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setSimSlotsMapping(int32_t /* serial */, const ::android::hardware::hidl_vec<uint32_t>& /* slotMap */) override { return {}; }
    virtual ::android::hardware::Return<void> getPhoneCapability(int32_t /* serial */) override { return {}; }
    virtual ::android::hardware::Return<void> setPreferredDataModem(int32_t /* serial */, uint8_t /* modemId */) override { return {}; }
    virtual ::android::hardware::Return<void> setModemsConfig(int32_t /* serial */, const ::android::hardware::radio::config::V1_1::ModemsConfig& /* modemsConfig */) override { return {}; }
    virtual ::android::hardware::Return<void> getModemsConfig(int32_t /* serial */) override { return {}; }
};