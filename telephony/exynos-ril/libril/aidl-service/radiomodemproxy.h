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

#include <radiomodemcallback.h>
#include <radiocallbackadapter.h>
#include <radiointerface.h>

struct RadioModemProxy : public RadioInterface,
                         public android::hardware::radio::impl::RadioModemCallbackAdapter {
    using RadioInterface::RadioInterface;

    std::shared_ptr<android::hardware::radio::impl::RadioModemResponseCallback> mResponseCb;
    std::shared_ptr<android::hardware::radio::impl::RadioModemIndicationCallback> mIndicationCb;

    template<typename T, typename... Args>
    void onRequest(int request, Args&&... args) {
        auto command = getCommand<T>(request);
        if (command == nullptr) {
            command = std::make_shared<T>(mSlot, mResponseCb);
            RadioInterface::updateCommand(request, command);
        }
        RadioInterface::onRequest(command, args...);
    }

    template<typename T>
    void listen(int indication) {
        RadioInterface::updateIndication(indication,
                                        std::make_shared<T>(mSlot, mIndicationCb));
    }

    void setResponseFunctions(
            const std::shared_ptr<android::hardware::radio::impl::RadioModemResponseCallback>& responseCb,
            const std::shared_ptr<android::hardware::radio::impl::RadioModemIndicationCallback>& indicationCb);

    void onRadioServiceConnected() override;

    void enableModem(int32_t serial, bool on) override;
    void getBasebandVersion(int32_t serial) override;
    void getDeviceIdentity(int32_t serial) override;
    void getHardwareConfig(int32_t serial) override;
    void getModemActivityInfo(int32_t serial) override;
    void getModemStackStatus(int32_t serial) override;
    void getRadioCapability(int32_t serial) override;
    void nvReadItem(int32_t serial,
                ::aidl::android::hardware::radio::modem::NvItem itemId) override;
    void nvResetConfig(int32_t serial,
                ::aidl::android::hardware::radio::modem::ResetNvType resetType) override;
    void nvWriteCdmaPrl(int32_t serial, const std::vector<uint8_t>& prl) override;
    void nvWriteItem(int32_t serial,
                const ::aidl::android::hardware::radio::modem::NvWriteItem& item) override;
    void requestShutdown(int32_t serial) override;
    void responseAcknowledgement() override;
    void sendDeviceState(int32_t serial,
                ::aidl::android::hardware::radio::modem::DeviceStateType deviceStateType, bool state) override;
    void setRadioCapability(int32_t serial,
                const ::aidl::android::hardware::radio::modem::RadioCapability& rc) override;
    void setRadioPower(int32_t serial, bool powerOn, bool forEmergencyCall,
                            bool preferredForEmergencyCall) override;

    static auto make(uint32_t slot, const std::shared_ptr<RadioProxy>& radio) {
        return std::make_shared<RadioModemProxy>(slot, radio);
    }
};