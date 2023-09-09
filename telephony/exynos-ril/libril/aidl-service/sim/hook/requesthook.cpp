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

#include <aidl/android/hardware/radio/sim/IccIo.h>
#include <aidl/android/hardware/radio/sim/SimApdu.h>
#include <aidl/android/hardware/radio/sim/CarrierRestrictions.h>
#include <aidl/android/hardware/radio/sim/SimLockMultiSimPolicy.h>
#include <aidl/android/hardware/radio/sim/ImsiEncryptionInfo.h>
#include <aidl/android/hardware/radio/sim/CdmaSubscriptionSource.h>
#include <aidl/android/hardware/radio/sim/SelectUiccSub.h>
#include <aidl/android/hardware/radio/sim/PersoSubstate.h>
#include <aidl/android/hardware/radio/sim/PhonebookRecordInfo.h>

#include "radiosimproxy.h"
#include "structs.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
namespace impl = android::hardware::radio::impl;
namespace aidl_sim = ::aidl::android::hardware::radio::sim;
using ::android::sp;

void RadioSimProxy::areUiccApplicationsEnabled(int32_t serial) {
    if (mHidlRadio) {
        mHidlRadio->areUiccApplicationsEnabled(serial);
    }
}

void RadioSimProxy::changeIccPin2ForApp(int32_t serial,
        const std::string& oldPin2, const std::string& newPin2, const std::string& aid) {
    if (mHidlRadio) {
        mHidlRadio->changeIccPin2ForApp(serial, oldPin2, newPin2, aid);
    }
}

void RadioSimProxy::changeIccPinForApp(int32_t serial,
        const std::string& oldPin, const std::string& newPin, const std::string& aid) {
    if (mHidlRadio) {
        mHidlRadio->changeIccPinForApp(serial, oldPin, newPin, aid);
    }
}

void RadioSimProxy::enableUiccApplications(int32_t serial, bool enable) {
    if (mHidlRadio) {
        mHidlRadio->enableUiccApplications(serial, enable);
    }
}

void RadioSimProxy::getAllowedCarriers(int32_t serial) {
    if (mHidlRadio) {
        mHidlRadio->getAllowedCarriers_1_4(serial);
    }
}

void RadioSimProxy::getCdmaSubscription(int32_t serial) {
    if (mHidlRadio) {
        mHidlRadio->getCDMASubscription(serial);
    }
}

void RadioSimProxy::getCdmaSubscriptionSource(int32_t serial) {
    if (mHidlRadio) {
        mHidlRadio->getCdmaSubscriptionSource(serial);
    }
}

void RadioSimProxy::getFacilityLockForApp(int32_t serial, const std::string& facility,
        const std::string& password, int32_t serviceClass, const std::string& appId) {
    if (mHidlRadio) {
        mHidlRadio->getFacilityLockForApp(serial, facility, password, serviceClass, appId);
    }
}

void RadioSimProxy::getImsiForApp(int32_t serial, const std::string& aid) {
    if (mHidlRadio) {
        mHidlRadio->getImsiForApp(serial, aid);
    }
}

void RadioSimProxy::getSimPhonebookCapacity(int32_t serial) {
    if (mHidlRadio) {
        mHidlRadio->getSimPhonebookCapacity(serial);
    }
}

void RadioSimProxy::getSimPhonebookRecords(int32_t serial) {
    if (mHidlRadio) {
        mHidlRadio->getSimPhonebookRecords(serial);
    }
}

void RadioSimProxy::iccCloseLogicalChannel(int32_t serial, int32_t channelId) {
    if (mHidlRadio) {
        mHidlRadio->iccCloseLogicalChannel(serial, channelId);
    }
}

void RadioSimProxy::iccIoForApp(int32_t serial, const aidl_sim::IccIo& iccIo) {
    if (mHidlRadio) {
        mHidlRadio->iccIOForApp(serial, compat::toHidl(iccIo));
    }
}

void RadioSimProxy::iccOpenLogicalChannel(int32_t serial,
        const std::string& aid, int32_t p2) {
    if (mHidlRadio) {
        mHidlRadio->iccOpenLogicalChannel(serial, aid, p2);
    }
}

void RadioSimProxy::iccTransmitApduBasicChannel(int32_t serial,
        const aidl_sim::SimApdu& message) {
    if (mHidlRadio) {
        mHidlRadio->iccTransmitApduBasicChannel(serial, compat::toHidl(message));
    }
}

void RadioSimProxy::iccTransmitApduLogicalChannel(int32_t serial,
        const aidl_sim::SimApdu& message) {
    if (mHidlRadio) {
        mHidlRadio->iccTransmitApduLogicalChannel(serial, compat::toHidl(message));
    }
}

void RadioSimProxy::reportStkServiceIsRunning(int32_t serial) {
    if (mHidlRadio) {
        mHidlRadio->reportStkServiceIsRunning(serial);
    }
}

void RadioSimProxy::requestIccSimAuthentication(int32_t serial,
        int32_t authContext, const std::string& authData, const std::string& aid) {
    if (mHidlRadio) {
        mHidlRadio->requestIccSimAuthentication(serial, authContext, authData, aid);
    }
}

void RadioSimProxy::responseAcknowledgement() {
    if (mHidlRadio) {
        mHidlRadio->responseAcknowledgement();
    }
}

void RadioSimProxy::sendEnvelope(int32_t serial, const std::string& command) {
    if (mHidlRadio) {
        mHidlRadio->sendEnvelope(serial, command);
    }
}

void RadioSimProxy::sendEnvelopeWithStatus(int32_t serial,
        const std::string& contents) {
    if (mHidlRadio) {
        mHidlRadio->sendEnvelopeWithStatus(serial, contents);
    }
}

void RadioSimProxy::sendTerminalResponseToSim(int32_t serial,
        const std::string& commandResponse) {
    if (mHidlRadio) {
        mHidlRadio->sendTerminalResponseToSim(serial, commandResponse);
    }
}

void RadioSimProxy::setAllowedCarriers(int32_t serial,
        const aidl_sim::CarrierRestrictions& carriers,
        aidl_sim::SimLockMultiSimPolicy multiSimPolicy) {
    if (mHidlRadio) {
        mHidlRadio->setAllowedCarriers_1_4(serial, compat::toHidl(carriers),
                (V1_4::SimLockMultiSimPolicy)multiSimPolicy);
    }
}

void RadioSimProxy::setCarrierInfoForImsiEncryption(int32_t serial,
        const aidl_sim::ImsiEncryptionInfo& imsiEncryptionInfo) {
    if (mHidlRadio) {
        mHidlRadio->setCarrierInfoForImsiEncryption_1_6(serial,
                compat::toHidl_1_6(imsiEncryptionInfo));
    }
}

void RadioSimProxy::setCdmaSubscriptionSource(int32_t serial,
        aidl_sim::CdmaSubscriptionSource cdmaSub) {
    if (mHidlRadio) {
        mHidlRadio->setCdmaSubscriptionSource(serial, (V1_0::CdmaSubscriptionSource)cdmaSub);
    }
}

void RadioSimProxy::setFacilityLockForApp(int32_t serial, const std::string& facility,
        bool lockState, const std::string& passwd, int32_t serviceClass, const std::string& appId) {
    if (mHidlRadio) {
        mHidlRadio->setFacilityLockForApp(serial, facility, lockState, passwd, serviceClass, appId);
    }
}

void RadioSimProxy::setSimCardPower(int32_t serial, aidl_sim::CardPowerState powerUp) {
    if (mHidlRadio) {
        mHidlRadio->setSimCardPower_1_6(serial, (const V1_1::CardPowerState)powerUp);
    }
}

void RadioSimProxy::setUiccSubscription(int32_t serial,
        const aidl_sim::SelectUiccSub& uiccSub) {
    if (mHidlRadio) {
        mHidlRadio->setUiccSubscription(serial, compat::toHidl(uiccSub));
    }
}

void RadioSimProxy::supplyIccPin2ForApp(int32_t serial,
        const std::string& pin2, const std::string& aid) {
    if (mHidlRadio) {
        mHidlRadio->supplyIccPin2ForApp(serial, pin2, aid);
    }
}

void RadioSimProxy::supplyIccPinForApp(int32_t serial,
        const std::string& pin, const std::string& aid) {
    if (mHidlRadio) {
        mHidlRadio->supplyIccPinForApp(serial, pin, aid);
    }
}

void RadioSimProxy::supplyIccPuk2ForApp(int32_t serial,
        const std::string& puk2, const std::string& pin2, const std::string& aid) {
    if (mHidlRadio) {
        mHidlRadio->supplyIccPuk2ForApp(serial, puk2, pin2, aid);
    }
}

void RadioSimProxy::supplyIccPukForApp(int32_t serial,
        const std::string& puk, const std::string& pin, const std::string& aid) {
    if (mHidlRadio) {
        mHidlRadio->supplyIccPukForApp(serial, puk, pin, aid);
    }
}

void RadioSimProxy::supplySimDepersonalization(int32_t serial,
        aidl_sim::PersoSubstate persoType, const std::string& controlKey) {
    if (mHidlRadio) {
        mHidlRadio->supplySimDepersonalization(serial,
                (::android::hardware::radio::V1_5::PersoSubstate)persoType, controlKey);
    }
}

void RadioSimProxy::updateSimPhonebookRecords(int32_t serial,
        const aidl_sim::PhonebookRecordInfo& recordInfo) {
    if (mHidlRadio) {
        mHidlRadio->updateSimPhonebookRecords(serial, compat::toHidl(recordInfo));
    }
}
