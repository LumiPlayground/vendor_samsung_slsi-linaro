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

#include "radiomodem.h"
#include "radiomodemindication.h"
#include "radiomodemresponse.h"

#define TAG "RadioModem"

namespace android::hardware::radio::impl {

using namespace std::string_literals;
using ::ndk::ScopedAStatus;
namespace aidl = ::aidl::android::hardware::radio::modem;
constexpr auto ok = &ScopedAStatus::ok;

RadioModem::RadioModem(std::shared_ptr<RadioModemCallback> radio, uint32_t slot)
    : mRadio(radio), mSlot(slot) {
    mCallbacks = {
        mRadio,
        std::make_shared<RadioModemResponse>(),
        std::make_shared<RadioModemIndication>()
    };
}

ScopedAStatus RadioModem::setResponseFunctions(
        const std::shared_ptr<aidl::IRadioModemResponse>& modemResponse,
        const std::shared_ptr<aidl::IRadioModemIndication>& modemIndication) {
#if VDBG
    RLOGD("%s::%s modemResponse=%p modemIndication=%p", TAG, __FUNCTION__,
                        modemResponse.get(), modemIndication.get());
#endif
    mCallbacks.setResponseFunctions(modemResponse, modemIndication);
    return ok();
}

ScopedAStatus RadioModem::enableModem(int32_t serial, bool on) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->enableModem(serial, on);
    return ok();
}

ScopedAStatus RadioModem::getBasebandVersion(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getBasebandVersion(serial);
    return ok();
}

ScopedAStatus RadioModem::getDeviceIdentity(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getDeviceIdentity(serial);
    return ok();
}

ScopedAStatus RadioModem::getHardwareConfig(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getHardwareConfig(serial);
    return ok();
}

ScopedAStatus RadioModem::getModemActivityInfo(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getModemActivityInfo(serial);
    return ok();
}

ScopedAStatus RadioModem::getModemStackStatus(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getModemStackStatus(serial);
    return ok();
}

ScopedAStatus RadioModem::getRadioCapability(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->getRadioCapability(serial);
    return ok();
}

ScopedAStatus RadioModem::nvReadItem(int32_t serial, aidl::NvItem itemId) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->nvReadItem(serial, itemId);
    return ok();
}

ScopedAStatus RadioModem::nvResetConfig(int32_t serial, aidl::ResetNvType resetType) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->nvResetConfig(serial, resetType);
    return ok();
}

ScopedAStatus RadioModem::nvWriteCdmaPrl(int32_t serial, const std::vector<uint8_t>& prl) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->nvWriteCdmaPrl(serial, prl);
    return ok();
}

ScopedAStatus RadioModem::nvWriteItem(int32_t serial, const aidl::NvWriteItem& item) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->nvWriteItem(serial, item);
    return ok();
}

ScopedAStatus RadioModem::requestShutdown(int32_t serial) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->requestShutdown(serial);
    return ok();
}

ScopedAStatus RadioModem::responseAcknowledgement() {
    return ok();
}

ScopedAStatus RadioModem::sendDeviceState(int32_t serial, aidl::DeviceStateType type, bool state) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->sendDeviceState(serial, type, state);
    return ok();
}

ScopedAStatus RadioModem::setRadioCapability(int32_t serial, const aidl::RadioCapability& rc) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setRadioCapability(serial, rc);
    return ok();
}

ScopedAStatus RadioModem::setRadioPower(int32_t serial, bool powerOn, bool forEmergencyCall,
                                        bool preferredForEmergencyCall) {
#if VDBG
    RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
    mRadio->setRadioPower(serial, powerOn, forEmergencyCall, preferredForEmergencyCall);
    return ok();
}

std::shared_ptr<RadioModem> RadioModem::make(std::shared_ptr<RadioModemCallback> radio, uint32_t slot) {
#if VDBG
    RLOGD("%s::%s slot %u", TAG, __FUNCTION__, slot);
#endif
    return ndk::SharedRefBase::make<RadioModem>(radio, slot);
}

}
