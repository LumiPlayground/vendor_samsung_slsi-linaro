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
#include "radiodata.h"
#include "radiodataindication.h"
#include "radiodataresponse.h"

#define TAG "RadioData"

namespace android::hardware::radio::impl {

using namespace std::string_literals;
using ::ndk::ScopedAStatus;
namespace aidl = ::aidl::android::hardware::radio::data;
constexpr auto ok = &ScopedAStatus::ok;

RadioData::RadioData(std::shared_ptr<RadioDataCallback> radio, uint32_t slot)
    : mRadio(radio), mSlot(slot) {
    mCallbacks = {
        mRadio,
        std::make_shared<RadioDataResponse>(),
        std::make_shared<RadioDataIndication>()
    };
}

ScopedAStatus RadioData::setResponseFunctions(
        const std::shared_ptr<aidl::IRadioDataResponse>& dataResponse,
        const std::shared_ptr<aidl::IRadioDataIndication>& dataIndication) {
#if VDBG
    RLOGD("%s::%s dataResponse=%p dataIndication=%p", TAG, __FUNCTION__,
                        dataResponse.get(), dataIndication.get());
#endif
    mCallbacks.setResponseFunctions(dataResponse, dataIndication);
    return ok();
}

ScopedAStatus RadioData::allocatePduSessionId(int32_t serial) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->allocatePduSessionId(serial);
  return ok();
}

ScopedAStatus RadioData::cancelHandover(int32_t serial, int32_t callId) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->cancelHandover(serial, callId);
  return ok();
}

ScopedAStatus RadioData::deactivateDataCall(
    int32_t serial, int32_t cid,
    ::aidl::android::hardware::radio::data::DataRequestReason reason) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->deactivateDataCall(serial, cid, reason);
  return ok();
}

ScopedAStatus RadioData::getDataCallList(int32_t serial) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->getDataCallList(serial);
  return ok();
}

ScopedAStatus RadioData::getSlicingConfig(int32_t serial) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->getSlicingConfig(serial);
  return ok();
}

ScopedAStatus RadioData::releasePduSessionId(int32_t serial, int32_t id) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->releasePduSessionId(serial, id);
  return ok();
}

ScopedAStatus RadioData::responseAcknowledgement() { return ok(); }

ScopedAStatus RadioData::setDataThrottling(
    int32_t serial,
    ::aidl::android::hardware::radio::data::DataThrottlingAction
        dataThrottlingAction,
    int64_t completionDurationMillis) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->setDataThrottling(serial, dataThrottlingAction,
                            completionDurationMillis);
  return ok();
}

ScopedAStatus RadioData::setDataAllowed(int32_t serial, bool allow) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->setDataAllowed(serial, allow);
  return ok();
}

ScopedAStatus RadioData::setDataProfile(
    int32_t serial,
    const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo>
        &profiles) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->setDataProfile(serial, profiles);
  return ok();
}

ScopedAStatus RadioData::setInitialAttachApn(
    int32_t serial,
    const std::optional<::aidl::android::hardware::radio::data::DataProfileInfo> &dpInfo) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->setInitialAttachApn(serial, dpInfo);
  return ok();
}

ScopedAStatus RadioData::setupDataCall(
    int32_t serial,
    ::aidl::android::hardware::radio::AccessNetwork accessNetwork,
    const ::aidl::android::hardware::radio::data::DataProfileInfo
        &dataProfileInfo,
    bool roamingAllowed,
    ::aidl::android::hardware::radio::data::DataRequestReason reason,
    const std::vector<::aidl::android::hardware::radio::data::LinkAddress>
        &addresses,
    const std::vector<std::string> &dnses, int32_t pduSessionId,
    const std::optional<::aidl::android::hardware::radio::data::SliceInfo>
        &sliceInfo,
    bool matchAllRuleAllowed) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->setupDataCall(serial, accessNetwork, dataProfileInfo, roamingAllowed,
                        reason, addresses, dnses, pduSessionId, sliceInfo, matchAllRuleAllowed);
  return ok();
}

ScopedAStatus RadioData::startHandover(int32_t serial, int32_t callId) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->startHandover(serial, callId);
  return ok();
}

ScopedAStatus RadioData::startKeepalive(
    int32_t serial,
    const ::aidl::android::hardware::radio::data::KeepaliveRequest &keepalive) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->startKeepalive(serial, keepalive);
  return ok();
}

ScopedAStatus RadioData::stopKeepalive(int32_t serial, int32_t sessionHandle) {
#if VDBG
  RLOGD("%s::%s serial %d", TAG, __FUNCTION__, serial);
#endif
  mRadio->stopKeepalive(serial, sessionHandle);
  return ok();
}

std::shared_ptr<RadioData>
RadioData::make(std::shared_ptr<RadioDataCallback> radio, uint32_t slot) {
  return ndk::SharedRefBase::make<RadioData>(radio, slot);
}

} // namespace android::hardware::radio::impl
