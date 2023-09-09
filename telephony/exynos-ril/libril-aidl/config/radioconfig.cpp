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
#include <aidl-log.h>
#include "radioconfig.h"
#include "radioconfigindication.h"
#include "radioconfigresponse.h"

#define TAG "RadioConfig"

namespace android::hardware::radio::impl {

using namespace std::string_literals;
using ::ndk::ScopedAStatus;
namespace aidl = ::aidl::android::hardware::radio::config;
constexpr auto ok = &ScopedAStatus::ok;

RadioConfig::RadioConfig(std::shared_ptr<RadioConfigCallback> radio, uint32_t slot)
    : mRadio(radio), mSlot(slot) {
    mCallbacks = {
        mRadio,
        std::make_shared<RadioConfigResponse>(),
        std::make_shared<RadioConfigIndication>()
    };
}

ScopedAStatus RadioConfig::setResponseFunctions(
        const std::shared_ptr<aidl::IRadioConfigResponse>& configResponse,
        const std::shared_ptr<aidl::IRadioConfigIndication>& configIndication) {
#if VDBG
    RLOGD("%s::%s configResponse=%p configIndication=%p", TAG, __FUNCTION__,
                        configResponse.get(), configIndication.get());
#endif
    mCallbacks.setResponseFunctions(configResponse, configIndication);
    return ok();
}

ScopedAStatus RadioConfig::getHalDeviceCapabilities(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getHalDeviceCapabilities(serial);
    return ok();
}

ScopedAStatus RadioConfig::getNumOfLiveModems(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getNumOfLiveModems(serial);
    return ok();
}

ScopedAStatus RadioConfig::getPhoneCapability(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getPhoneCapability(serial);
    return ok();
}

ScopedAStatus RadioConfig::getSimSlotsStatus(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getSimSlotsStatus(serial);
    return ok();
}

ScopedAStatus RadioConfig::setNumOfLiveModems(int32_t serial, int8_t numOfLiveModems) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setNumOfLiveModems(serial, numOfLiveModems);
    return ok();
}

ScopedAStatus RadioConfig::setPreferredDataModem(int32_t serial, int8_t modemId) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setPreferredDataModem(serial, modemId);
    return ok();
}

ScopedAStatus RadioConfig::setSimSlotsMapping(int32_t serial, const std::vector<aidl::SlotPortMapping>& slotMap) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setSimSlotsMapping(serial, slotMap);
    return ok();
}

std::shared_ptr<RadioConfig> RadioConfig::make(std::shared_ptr<RadioConfigCallback> radio, uint32_t slot) {
#if VDBG
    RLOGD("%s::%s slot %u", TAG, __FUNCTION__, slot);
#endif
    return ndk::SharedRefBase::make<RadioConfig>(radio, slot);
}

}