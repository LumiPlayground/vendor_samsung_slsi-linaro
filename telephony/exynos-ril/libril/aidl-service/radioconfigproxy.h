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

#include <radioconfigcallback.h>
#include <radiocallbackadapter.h>
#include <radioconfiginterface.h>

struct RadioConfigProxy : public RadioConfigInterface,
                         public android::hardware::radio::impl::RadioConfigCallbackAdapter {
    using RadioConfigInterface::RadioConfigInterface;

    std::shared_ptr<android::hardware::radio::impl::RadioConfigResponseCallback> mResponseCb;
    std::shared_ptr<android::hardware::radio::impl::RadioConfigIndicationCallback> mIndicationCb;

    template<typename T, typename... Args>
    void onRequest(int request, Args&&... args) {
        auto command = getCommand<T>(request);
        if (command == nullptr) {
            command = std::make_shared<T>(mSlot, mResponseCb);
            RadioConfigInterface::updateCommand(request, command);
        }
        RadioConfigInterface::onRequest(command, args...);
    }

    template<typename T>
    void listen(int indication) {
        RadioConfigInterface::updateIndication(indication,
                                        std::make_shared<T>(mSlot, mIndicationCb));
    }

    void setResponseFunctions(
            const std::shared_ptr<android::hardware::radio::impl::RadioConfigResponseCallback>& responseCb,
            const std::shared_ptr<android::hardware::radio::impl::RadioConfigIndicationCallback>& indicationCb);

    void getHalDeviceCapabilities(int32_t serial) override;
    void getNumOfLiveModems(int32_t serial) override;
    void getPhoneCapability(int32_t serial) override;
    void getSimSlotsStatus(int32_t serial) override;
    void setNumOfLiveModems(int32_t serial, int8_t numOfLiveModems) override;
    void setPreferredDataModem(int32_t serial, int8_t modemId) override;
    void setSimSlotsMapping(int32_t serial,
            const std::vector<aidl::android::hardware::radio::config::SlotPortMapping>& slotMap)
            override;

    static auto make(uint32_t slot, const std::shared_ptr<RadioProxy>& radio) {
        return std::make_shared<RadioConfigProxy>(slot, radio);
    }
};