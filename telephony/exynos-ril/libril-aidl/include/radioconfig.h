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

#include <aidl/android/hardware/radio/config/BnRadioConfig.h>
#include <radiocallbackmanager.h>
#include <radioconfigcallback.h>

namespace android::hardware::radio::impl {
struct RadioConfig : public aidl::android::hardware::radio::config::BnRadioConfig {
    std::shared_ptr<RadioConfigCallback> mRadio;
    uint32_t mSlot;
    RadioCallbackManager<RadioConfigCallback, RadioConfigResponseCallback,
                RadioConfigIndicationCallback> mCallbacks;

    RadioConfig(std::shared_ptr<RadioConfigCallback> radio, uint32_t slot);

    ::ndk::ScopedAStatus getHalDeviceCapabilities(int32_t serial) override;
    ::ndk::ScopedAStatus getNumOfLiveModems(int32_t serial) override;
    ::ndk::ScopedAStatus getPhoneCapability(int32_t serial) override;
    ::ndk::ScopedAStatus getSimSlotsStatus(int32_t serial) override;
    ::ndk::ScopedAStatus setNumOfLiveModems(int32_t serial, int8_t numOfLiveModems) override;
    ::ndk::ScopedAStatus setPreferredDataModem(int32_t serial, int8_t modemId) override;
    ::ndk::ScopedAStatus setResponseFunctions(
            const std::shared_ptr<aidl::android::hardware::radio::config::IRadioConfigResponse>&
                    radioConfigResponse,
            const std::shared_ptr<aidl::android::hardware::radio::config::IRadioConfigIndication>&
                    radioConfigIndication) override;
    ::ndk::ScopedAStatus setSimSlotsMapping(
            int32_t serial,
            const std::vector<aidl::android::hardware::radio::config::SlotPortMapping>& slotMap)
            override;

    static std::shared_ptr<RadioConfig> make(std::shared_ptr<RadioConfigCallback> radio, uint32_t slot);
};

}