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

#include <aidl/android/hardware/radio/config/SlotPortMapping.h>
#include "radioconfigproxy.h"
#include "structs.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
namespace impl = android::hardware::radio::impl;
namespace aidl_config = ::aidl::android::hardware::radio::config;
using ::android::sp;

// void RadioConfigProxy::getHalDeviceCapabilities(int32_t serial) {
//     using new interface
//     implement into radiomodemproxy.cpp
// }

void RadioConfigProxy::getNumOfLiveModems(int32_t serial) {
    if (mHidlRadioConfig) {
        mHidlRadioConfig->getModemsConfig(serial);
    }
}

void RadioConfigProxy::getPhoneCapability(int32_t serial) {
    if (mHidlRadioConfig) {
        mHidlRadioConfig->getPhoneCapability(serial);
    }
}

//void RadioConfigProxy::getSimSlotsStatus(int32_t serial) {
//    if (mHidlRadioConfig) {
//        mHidlRadioConfig->getSimSlotsStatus(serial);
//    }
//}

void RadioConfigProxy::setNumOfLiveModems(int32_t serial, int8_t numOfLiveModems) {
    if (mHidlRadioConfig) {
        mHidlRadioConfig->setModemsConfig(serial, {static_cast<uint8_t>(numOfLiveModems)});
    }
}

void RadioConfigProxy::setPreferredDataModem(int32_t serial, int8_t modemId) {
    if (mHidlRadioConfig) {
        mHidlRadioConfig->setPreferredDataModem(serial, modemId);
    }
}

//void RadioConfigProxy::setSimSlotsMapping(int32_t serial, const std::vector<aidl_config::SlotPortMapping>& slotMap) {
//    if (mHidlRadioConfig) {
//        mHidlRadioConfig->setSimSlotsMapping(serial, compat::toHidl(slotMap));
//    }
//}
