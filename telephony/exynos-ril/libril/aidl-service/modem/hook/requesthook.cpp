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

#include <aidl/android/hardware/radio/modem/ActivityStatsInfo.h>
#include <aidl/android/hardware/radio/modem/HardwareConfig.h>
#include <aidl/android/hardware/radio/modem/HardwareConfigModem.h>
#include <aidl/android/hardware/radio/modem/HardwareConfigSim.h>
#include <aidl/android/hardware/radio/modem/NvWriteItem.h>
#include <aidl/android/hardware/radio/modem/RadioCapability.h>
#include "radiomodemproxy.h"
#include "structs.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
namespace impl = android::hardware::radio::impl;
namespace aidl_modem = ::aidl::android::hardware::radio::modem;
using ::android::sp;

void RadioModemProxy::enableModem(int32_t serial, bool on) {
    mHidlRadio->enableModem(serial, on);
}

void RadioModemProxy::getBasebandVersion(int32_t serial) {
    mHidlRadio->getBasebandVersion(serial);
}

void RadioModemProxy::getDeviceIdentity(int32_t serial) {
    mHidlRadio->getDeviceIdentity(serial);
}

void RadioModemProxy::getHardwareConfig(int32_t serial) {
    mHidlRadio->getHardwareConfig(serial);
}

void RadioModemProxy::getModemStackStatus(int32_t serial) {
    mHidlRadio->getModemStackStatus(serial);
}

void RadioModemProxy::getRadioCapability(int32_t serial) {
    mHidlRadio->getRadioCapability(serial);
}

void RadioModemProxy::nvReadItem(int32_t serial, aidl_modem::NvItem itemId) {
    mHidlRadio->nvReadItem(serial, V1_0::NvItem(itemId));
}

void RadioModemProxy::nvResetConfig(int32_t serial, aidl_modem::ResetNvType resetType) {
    mHidlRadio->nvResetConfig(serial, V1_0::ResetNvType(resetType));
}

void RadioModemProxy::nvWriteCdmaPrl(int32_t serial, const std::vector<uint8_t>& prl) {
    mHidlRadio->nvWriteCdmaPrl(serial, prl);
}

void RadioModemProxy::nvWriteItem(int32_t serial, const aidl_modem::NvWriteItem& item) {
    mHidlRadio->nvWriteItem(serial, compat::toHidl(item));
}

void RadioModemProxy::requestShutdown(int32_t serial) {
    mHidlRadio->requestShutdown(serial);
}

void RadioModemProxy::responseAcknowledgement() {
    mHidlRadio->responseAcknowledgement();
}

void RadioModemProxy::sendDeviceState(int32_t serial, aidl_modem::DeviceStateType deviceStateType, bool state) {
    mHidlRadio->sendDeviceState(serial, V1_0::DeviceStateType(deviceStateType), state);
}

void RadioModemProxy::setRadioCapability(int32_t serial, const aidl_modem::RadioCapability& rc) {
    mHidlRadio->setRadioCapability(serial, compat::toHidl(rc));
}

// void RadioModemProxy::setRadioPower(int32_t serial, bool powerOn, bool forEmergencyCall,
//                         bool preferredForEmergencyCall) {
//     mHidlRadio->setRadioPower_1_6(serial, powerOn, forEmergencyCall, preferredForEmergencyCall);
// }