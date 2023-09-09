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
#include "radiosim.h"
#include "radiosimindication.h"
#include "radiosimresponse.h"

#define TAG "RadioSim"

namespace android::hardware::radio::impl {

using namespace std::string_literals;
using ::ndk::ScopedAStatus;
namespace aidl = ::aidl::android::hardware::radio::sim;
constexpr auto ok = &ScopedAStatus::ok;

RadioSim::RadioSim(std::shared_ptr<RadioSimCallback> radio, uint32_t slot)
    : mRadio(radio), mSlot(slot) {
    mCallbacks = {
        mRadio,
        std::make_shared<RadioSimResponse>(),
        std::make_shared<RadioSimIndication>()
    };
}

ScopedAStatus RadioSim::setResponseFunctions(
        const std::shared_ptr<aidl::IRadioSimResponse>& simResponse,
        const std::shared_ptr<aidl::IRadioSimIndication>& simIndication) {
#if VDBG
    RLOGD("%s::%s simResponse=%p simIndication=%p", TAG, __FUNCTION__,
            simResponse.get(), simIndication.get());
#endif
    mCallbacks.setResponseFunctions(simResponse, simIndication);
    return ok();
}
ScopedAStatus RadioSim::areUiccApplicationsEnabled(int32_t serial) {
    mRadio->areUiccApplicationsEnabled(serial);
    return ok();
}

ScopedAStatus RadioSim::changeIccPin2ForApp(int32_t serial, const std::string& oldPin2,
        const std::string& newPin2, const std::string& aid) {
    mRadio->changeIccPin2ForApp(serial, oldPin2, newPin2, aid);
    return ok();
}

ScopedAStatus RadioSim::changeIccPinForApp(int32_t serial, const std::string& oldPin,
        const std::string& newPin, const std::string& aid) {
    mRadio->changeIccPinForApp(serial, oldPin, newPin, aid);
    return ok();
}

ScopedAStatus RadioSim::enableUiccApplications(int32_t serial, bool enable) {
    mRadio->enableUiccApplications(serial, enable);
    return ok();
}

ScopedAStatus RadioSim::getAllowedCarriers(int32_t serial) {
    mRadio->getAllowedCarriers(serial);
    return ok();
}

ScopedAStatus RadioSim::getCdmaSubscription(int32_t serial) {
    mRadio->getCdmaSubscription(serial);
    return ok();
}

ScopedAStatus RadioSim::getCdmaSubscriptionSource(int32_t serial) {
    mRadio->getCdmaSubscriptionSource(serial);
    return ok();
}

ScopedAStatus RadioSim::getFacilityLockForApp(int32_t serial,
        const std::string& facility, const std::string& password, int32_t serviceClass,
        const std::string& appId) {
    mRadio->getFacilityLockForApp(serial, facility, password, serviceClass, appId);
    return ok();
}

ScopedAStatus RadioSim::getIccCardStatus(int32_t serial) {
    mRadio->getIccCardStatus(serial);
    return ok();
}

ScopedAStatus RadioSim::getImsiForApp(int32_t serial, const std::string& aid) {
    mRadio->getImsiForApp(serial, aid);
    return ok();
}

ScopedAStatus RadioSim::getSimPhonebookCapacity(int32_t serial) {
    mRadio->getSimPhonebookCapacity(serial);
    return ok();
}

ScopedAStatus RadioSim::getSimPhonebookRecords(int32_t serial) {
    mRadio->getSimPhonebookRecords(serial);
    return ok();
}

ScopedAStatus RadioSim::iccCloseLogicalChannel(int32_t serial, int32_t channelId) {
    mRadio->iccCloseLogicalChannel(serial, channelId);
    return ok();
}

ScopedAStatus RadioSim::iccIoForApp(int32_t serial, const aidl::IccIo& iccIo) {
    mRadio->iccIoForApp(serial, iccIo);
    return ok();
}

ScopedAStatus RadioSim::iccOpenLogicalChannel(int32_t serial, const std::string& aid,
        int32_t p2) {
    mRadio->iccOpenLogicalChannel(serial, aid, p2);
    return ok();
}

ScopedAStatus RadioSim::iccTransmitApduBasicChannel(int32_t serial,
        const aidl::SimApdu& message) {
    mRadio->iccTransmitApduBasicChannel(serial, message);
    return ok();
}

ScopedAStatus RadioSim::iccTransmitApduLogicalChannel(int32_t serial,
        const aidl::SimApdu& message) {
    mRadio->iccTransmitApduLogicalChannel(serial, message);
    return ok();
}

ScopedAStatus RadioSim::reportStkServiceIsRunning(int32_t serial) {
    mRadio->reportStkServiceIsRunning(serial);
    return ok();
}

ScopedAStatus RadioSim::requestIccSimAuthentication(int32_t serial,
        int32_t authContext, const std::string& authData, const std::string& aid) {
    mRadio->requestIccSimAuthentication(serial, authContext, authData, aid);
    return ok();
}

ScopedAStatus RadioSim::responseAcknowledgement() {
    return ok();
}

ScopedAStatus RadioSim::sendEnvelope(int32_t serial, const std::string& contents) {
    mRadio->sendEnvelope(serial, contents);
    return ok();
}

ScopedAStatus RadioSim::sendEnvelopeWithStatus(int32_t serial,
        const std::string& contents) {
    mRadio->sendEnvelopeWithStatus(serial, contents);
    return ok();
}
ScopedAStatus RadioSim::sendTerminalResponseToSim(int32_t serial,
        const std::string& contents) {
    mRadio->sendTerminalResponseToSim(serial, contents);
    return ok();
}

ScopedAStatus RadioSim::setAllowedCarriers(int32_t serial,
        const aidl::CarrierRestrictions& carriers, aidl::SimLockMultiSimPolicy multiSimPolicy) {
    mRadio->setAllowedCarriers(serial, carriers, multiSimPolicy);
    return ok();
}

ScopedAStatus RadioSim::setCarrierInfoForImsiEncryption(int32_t serial
        , const aidl::ImsiEncryptionInfo& imsiEncryptionInfo) {
    mRadio->setCarrierInfoForImsiEncryption(serial, imsiEncryptionInfo);
    return ok();
}

ScopedAStatus RadioSim::setCdmaSubscriptionSource(int32_t serial,
        aidl::CdmaSubscriptionSource cdmaSub) {
    mRadio->setCdmaSubscriptionSource(serial, cdmaSub);
    return ok();
}

ScopedAStatus RadioSim::setFacilityLockForApp(int32_t serial,
        const std::string& facility, bool lockState, const std::string& password,
        int32_t serviceClass, const std::string& appId) {
    mRadio->setFacilityLockForApp(serial, facility, lockState, password, serviceClass, appId);
    return ok();
}

ScopedAStatus RadioSim::setSimCardPower(int32_t serial, aidl::CardPowerState powerUp) {
    mRadio->setSimCardPower(serial, powerUp);
    return ok();
}

ScopedAStatus RadioSim::setUiccSubscription(int32_t serial,
        const aidl::SelectUiccSub& uiccSub) {
    mRadio->setUiccSubscription(serial, uiccSub);
    return ok();
}

ScopedAStatus RadioSim::supplyIccPin2ForApp(int32_t serial, const std::string& pin2,
        const std::string& aid) {
    mRadio->supplyIccPin2ForApp(serial, pin2, aid);
    return ok();
}

ScopedAStatus RadioSim::supplyIccPinForApp(int32_t serial, const std::string& pin,
        const std::string& aid) {
    mRadio->supplyIccPinForApp(serial, pin, aid);
    return ok();
}

ScopedAStatus RadioSim::supplyIccPuk2ForApp(int32_t serial, const std::string& puk2,
        const std::string& pin2, const std::string& aid) {
    mRadio->supplyIccPuk2ForApp(serial, puk2, pin2, aid);
    return ok();
}

ScopedAStatus RadioSim::supplyIccPukForApp(int32_t serial, const std::string& puk,
        const std::string& pin, const std::string& aid) {
    mRadio->supplyIccPukForApp(serial, puk, pin, aid);
    return ok();
}

ScopedAStatus RadioSim::supplySimDepersonalization(int32_t serial,
        aidl::PersoSubstate persoType, const std::string& controlKey) {
    mRadio->supplySimDepersonalization(serial, persoType, controlKey);
    return ok();
}

ScopedAStatus RadioSim::updateSimPhonebookRecords(int32_t serial,
        const aidl::PhonebookRecordInfo& recordInfo) {
    mRadio->updateSimPhonebookRecords(serial, recordInfo);
    return ok();
}

std::shared_ptr<RadioSim>
RadioSim::make(std::shared_ptr<RadioSimCallback> radio, uint32_t slot) {
    return ndk::SharedRefBase::make<RadioSim>(radio, slot);
}

}
