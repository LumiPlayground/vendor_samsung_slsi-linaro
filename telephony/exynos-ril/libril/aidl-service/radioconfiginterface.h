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
#include <radioproxy.h>
#include <telephony/ril.h>
#include <ril_internal.h>
#include <basecommand.h>

#include <map>

struct RadioConfigInterface {
    uint32_t mSlot;
    std::shared_ptr<RadioResponse> mRadioResponse;
    std::shared_ptr<RadioIndication> mRadioIndication;
    ::android::sp<::android::hardware::radio::config::V1_1::IRadioConfig> mHidlRadioConfig;
    std::map<int, std::shared_ptr<BaseCommand>> mCommand;
    std::map<int, std::shared_ptr<BaseIndication>> mIndication;

    RadioConfigInterface(uint32_t slot) : mSlot(slot) {}
    RadioConfigInterface(uint32_t slot,
                    const std::shared_ptr<RadioProxy>& radio)
                    : mSlot(slot) {
        mHidlRadioConfig = radio->hidlConfig();
        mRadioResponse = radio->response();
        mRadioIndication = radio->indication();
    }

    void hidl(const ::android::sp<::android::hardware::radio::config::V1_1::IRadioConfig>& hidlRadio) { mHidlRadioConfig = hidlRadio ;}

    template<typename T, typename... Args>
    void onRequest(T&& command, Args&&... args) {
        if (command) {
            command->onRequest(args...);
        }
    }

    template<typename T, typename... Args>
    void onRequest(int request, Args&&... args) {
        auto command = getCommand<T>(request);
        if (command) {
            command->onRequest(args...);
        }
    }

    template<typename T>
    std::shared_ptr<T> getCommand(int request) {
        auto it = mCommand.find(request);
        if (it != mCommand.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    void updateCommand(int request, T&& command) {
        if (command) {
            command->setRequestId(request);
            mCommand[request] = command;
        }
    }

    template<typename T>
    void updateIndication(int id, T&& indication) {
        if (indication) {
            indication->setIndicationId(id);
            indication->listen();
            mIndication[id] = indication;
        }
    }
};