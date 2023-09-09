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

#include <android/binder_manager.h>
#include <android/binder_process.h>

using namespace std::string_literals;

template<typename T>
bool publishRadioHal(std::shared_ptr<T>& aidlHal, const std::string& instance) {
    if (!AServiceManager_isDeclared(instance.c_str())) {
        return false;
    }
    const auto status = AServiceManager_addService(aidlHal->asBinder().get(), instance.c_str());
    return (status == STATUS_OK);
}

template<typename T>
bool publishRadioHal(std::shared_ptr<T>& aidlHal, uint32_t slot) {
    const auto instance = T::descriptor + "/slot"s + std::to_string(slot + 1);
    if (!AServiceManager_isDeclared(instance.c_str())) {
        return false;
    }
    const auto status = AServiceManager_addService(aidlHal->asBinder().get(), instance.c_str());
    return (status == STATUS_OK);
}

namespace android::hardware::radio::impl {
inline void joinThreadPool() {
    ABinderProcess_joinThreadPool();
}

}
