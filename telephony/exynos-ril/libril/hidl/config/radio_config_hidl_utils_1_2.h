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
#include <android/hardware/radio/config/1.2/IRadioConfigResponse.h>
#include <android/hardware/radio/config/1.2/IRadioConfigIndication.h>

using namespace android::hardware::radio::V1_0;
using namespace ::android::hardware::radio::config::V1_2;
using namespace ::android::hardware::radio::config::V1_1;
using namespace ::android::hardware::radio::config::V1_0;

struct RadioConfigResponseBase_1_2 : public ::android::hardware::radio::config::V1_2::IRadioConfigResponse {
    RadioConfigResponseBase_1_2() = default;
    virtual ~RadioConfigResponseBase_1_2() = default;

    /* 1.2 Api */
    virtual ::android::hardware::Return<void> getSimSlotsStatusResponse_1_2(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::config::V1_2::SimSlotStatus>& slotStatus);

    /* 1.1 Api */
    virtual ::android::hardware::Return<void> getPhoneCapabilityResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const android::hardware::radio::config::V1_1::PhoneCapability& phoneCapability);

    virtual ::android::hardware::Return<void> setPreferredDataModemResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> setModemsConfigResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);

    virtual ::android::hardware::Return<void> getModemsConfigResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::radio::config::V1_1::ModemsConfig& modemsConfig);

    /* 1.0 Api */
    virtual ::android::hardware::Return<void> getSimSlotsStatusResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info,
            const ::android::hardware::hidl_vec<::android::hardware::radio::config::V1_0::SimSlotStatus>& slotStatus);

    virtual ::android::hardware::Return<void> setSimSlotsMappingResponse(
            const ::android::hardware::radio::V1_0::RadioResponseInfo& info);
};

struct RadioConfigIndicationBase_1_2 : public ::android::hardware::radio::config::V1_2::IRadioConfigIndication {
    RadioConfigIndicationBase_1_2() = default;
    virtual ~RadioConfigIndicationBase_1_2() = default;

    /* 1.2 Api */
    virtual ::android::hardware::Return<void> simSlotsStatusChanged_1_2(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::config::V1_2::SimSlotStatus>& slotStatus);

    /* 1.0 Api */
    virtual ::android::hardware::Return<void> simSlotsStatusChanged(
            RadioIndicationType type,
            const ::android::hardware::hidl_vec<::android::hardware::radio::config::V1_0::SimSlotStatus>& slotStatus);
};