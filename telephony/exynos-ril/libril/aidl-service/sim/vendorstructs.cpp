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
#include "vendorstructs.h"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::sim;

aidl::AppStatus toAidl(const RIL_AppStatus& info) {
    aidl::AppStatus appStatus = {};

    appStatus.appType = static_cast<int32_t>(info.app_type);
    appStatus.appState = static_cast<int32_t>(info.app_state);
    appStatus.persoSubstate = aidl::PersoSubstate(info.perso_substate);
    if (info.aid_ptr != nullptr) {
        appStatus.aidPtr = static_cast<std::string>(info.aid_ptr);
    }
    if (info.app_label_ptr != nullptr) {
        appStatus.appLabelPtr = static_cast<std::string>(info.app_label_ptr);
    }
    appStatus.pin1Replaced = (info.pin1_replaced != 0);
    appStatus.pin1 = aidl::PinState(info.pin1);
    appStatus.pin2 = aidl::PinState(info.pin2);

    return appStatus;
}

aidl::CardStatus toAidl(const sim::V2_0::RIL_CardStatus& info) {
    aidl::CardStatus cardStatus = {};

    cardStatus.cardState = static_cast<int32_t>(info.card_state);
    cardStatus.universalPinState = aidl::PinState(info.universal_pin_state);
    cardStatus.gsmUmtsSubscriptionAppIndex = info.gsm_umts_subscription_app_index;
    cardStatus.cdmaSubscriptionAppIndex = info.cdma_subscription_app_index;
    cardStatus.imsSubscriptionAppIndex = info.ims_subscription_app_index;
    for (int i = 0; i < info.num_applications; i++) {
        cardStatus.applications.push_back(toAidl(info.applications[i]));
    }
    if (info.atr != nullptr) {
        cardStatus.atr = static_cast<std::string>(info.atr);
    }
    if (info.iccid != nullptr) {
        cardStatus.iccid = static_cast<std::string>(info.iccid);
    }
    if (info.eid != nullptr) {
        cardStatus.eid = static_cast<std::string>(info.eid);
    }
    cardStatus.slotMap = {static_cast<int32_t>(info.physicalSlotId), (info.portId)};

    return cardStatus;
}

}

