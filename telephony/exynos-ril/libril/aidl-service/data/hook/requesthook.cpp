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

#include <aidl/android/hardware/radio/data/DataRequestReason.h>
#include <aidl/android/hardware/radio/data/DataProfileInfo.h>
#include "radiodataproxy.h"

#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

using namespace ::android::hardware::radio;
namespace impl = android::hardware::radio::impl;
namespace aidl_data = ::aidl::android::hardware::radio::data;
using ::android::sp;

void RadioDataProxy::allocatePduSessionId(int32_t serial) {
    mHidlRadio->allocatePduSessionId(serial);
}
void RadioDataProxy::cancelHandover(int32_t serial,
                                    int32_t callId) {
    mHidlRadio->cancelHandover(serial, callId);
}
void RadioDataProxy::deactivateDataCall(
    int32_t serial, int32_t cid,
    ::aidl::android::hardware::radio::data::DataRequestReason reason) {
    mHidlRadio->deactivateDataCall_1_2(serial, cid, V1_2::DataRequestReason(reason));
}
void RadioDataProxy::getDataCallList(int32_t serial) {
    mHidlRadio->getDataCallList_1_6(serial);
}
void RadioDataProxy::getSlicingConfig(int32_t serial) {
    mHidlRadio->getSlicingConfig(serial);
}
void RadioDataProxy::releasePduSessionId(int32_t serial,
                                         int32_t id) {
    mHidlRadio->releasePduSessionId(serial, id);
}
void RadioDataProxy::responseAcknowledgement() {
    mHidlRadio->responseAcknowledgement();
}
void RadioDataProxy::setDataAllowed(int32_t serial,
                                    bool allow) {
    mHidlRadio->setDataAllowed(serial, allow);
}

void RadioDataProxy::setDataThrottling(
    int32_t serial,
    ::aidl::android::hardware::radio::data::DataThrottlingAction
        dataThrottlingAction,
    int64_t completionDurationMillis) {
    mHidlRadio->setDataThrottling(serial, V1_6::DataThrottlingAction(dataThrottlingAction), completionDurationMillis);
}

void RadioDataProxy::startHandover(int32_t serial,
                                   int32_t callId) {
    mHidlRadio->startHandover(serial, callId);
}
void RadioDataProxy::startKeepalive(
    int32_t serial,
    const ::aidl::android::hardware::radio::data::KeepaliveRequest &keepalive) {
    mHidlRadio->startKeepalive(serial, compat::toHidl(keepalive));
}
void RadioDataProxy::stopKeepalive(int32_t serial,
                                   int32_t sessionHandle) {
    mHidlRadio->stopKeepalive(serial, sessionHandle);
}
