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

#include <aidl/android/hardware/radio/modem/BnRadioModem.h>
#include <radiocallbackmanager.h>
#include <radiomodemcallback.h>

namespace android::hardware::radio::impl {
struct RadioModem : public aidl::android::hardware::radio::modem::BnRadioModem {
    std::shared_ptr<RadioModemCallback> mRadio;
    uint32_t mSlot;
    RadioCallbackManager<RadioModemCallback, RadioModemResponseCallback,
                RadioModemIndicationCallback> mCallbacks;

    RadioModem(std::shared_ptr<RadioModemCallback> radio, uint32_t slot);

    ::ndk::ScopedAStatus enableModem(int32_t serial, bool on) override;
    ::ndk::ScopedAStatus getBasebandVersion(int32_t serial) override;
    ::ndk::ScopedAStatus getDeviceIdentity(int32_t serial) override;
    ::ndk::ScopedAStatus getHardwareConfig(int32_t serial) override;
    ::ndk::ScopedAStatus getModemActivityInfo(int32_t serial) override;
    ::ndk::ScopedAStatus getModemStackStatus(int32_t serial) override;
    ::ndk::ScopedAStatus getRadioCapability(int32_t serial) override;
    ::ndk::ScopedAStatus nvReadItem(
            int32_t serial, ::aidl::android::hardware::radio::modem::NvItem itemId) override;
    ::ndk::ScopedAStatus nvResetConfig(
            int32_t serial, ::aidl::android::hardware::radio::modem::ResetNvType type) override;
    ::ndk::ScopedAStatus nvWriteCdmaPrl(int32_t serial, const std::vector<uint8_t>& prl) override;
    ::ndk::ScopedAStatus nvWriteItem(
            int32_t serial, const ::aidl::android::hardware::radio::modem::NvWriteItem& item) override;
    ::ndk::ScopedAStatus requestShutdown(int32_t serial) override;
    ::ndk::ScopedAStatus responseAcknowledgement() override;
    ::ndk::ScopedAStatus sendDeviceState(
            int32_t serial, ::aidl::android::hardware::radio::modem::DeviceStateType stateType,
            bool state) override;
    ::ndk::ScopedAStatus setRadioCapability(
            int32_t s, const ::aidl::android::hardware::radio::modem::RadioCapability& rc) override;
    ::ndk::ScopedAStatus setRadioPower(int32_t serial, bool powerOn, bool forEmergencyCall,
                                       bool preferredForEmergencyCall) override;
    ::ndk::ScopedAStatus setResponseFunctions(
            const std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemResponse>&
                    radioModemResponse,
            const std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemIndication>&
                    radioModemIndication) override;

    static std::shared_ptr<RadioModem> make(std::shared_ptr<RadioModemCallback> radio, uint32_t slot);
};

}  // namespace android::hardware::radio::impl
