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

#include <hidl/1.6/radio_hidl_utils_1_6.h>
#include <telephony/ril.h>
#include "radiodatacallback.h"

struct RadioDataResponseHook : public RadioResponseBase_1_6 {

  std::shared_ptr<android::hardware::radio::impl::RadioDataResponseCallback>
      mDataCb;

  void setResponseFunction(
      std::shared_ptr<android::hardware::radio::impl::RadioDataResponseCallback>
          dataCb);

  // Ver >= 1.6
  ::android::hardware::Return<void> allocatePduSessionIdResponse(
      const ::android::hardware::radio::V1_6::RadioResponseInfo &info,
      int32_t id) override;

  ::android::hardware::Return<void> releasePduSessionIdResponse(
      const ::android::hardware::radio::V1_6::RadioResponseInfo &info) override;

  ::android::hardware::Return<void> startHandoverResponse(
      const ::android::hardware::radio::V1_6::RadioResponseInfo &info) override;

  ::android::hardware::Return<void> cancelHandoverResponse(
      const ::android::hardware::radio::V1_6::RadioResponseInfo &info) override;

  ::android::hardware::Return<void> getDataCallListResponse_1_6(
      const ::android::hardware::radio::V1_6::RadioResponseInfo &info,
      const ::android::hardware::hidl_vec<
          ::android::hardware::radio::V1_6::SetupDataCallResult> &dcResponse) override;

  ::android::hardware::Return<void> getSlicingConfigResponse(
      const ::android::hardware::radio::V1_6::RadioResponseInfo &info,
      const ::android::hardware::radio::V1_6::SlicingConfig &slicingConfig) override;

  ::android::hardware::Return<void> setDataThrottlingResponse(
      const ::android::hardware::radio::V1_6::RadioResponseInfo &info) override;

  ::android::hardware::Return<void> setupDataCallResponse_1_6(
      const ::android::hardware::radio::V1_6::RadioResponseInfo &info,
      const android::hardware::radio::V1_6::SetupDataCallResult &dcResponse) override;

  // Ver < 1.6
  ::android::hardware::Return<void> setupDataCallResponse_1_5(
      const ::android::hardware::radio::V1_0::RadioResponseInfo &info,
      const android::hardware::radio::V1_5::SetupDataCallResult &dcResponse) override;

  ::android::hardware::Return<void> deactivateDataCallResponse(
      const ::android::hardware::radio::V1_0::RadioResponseInfo &info) override;

  ::android::hardware::Return<void> setDataAllowedResponse(
      const ::android::hardware::radio::V1_0::RadioResponseInfo &info) override;

  ::android::hardware::Return<void> getDataCallListResponse_1_5(
      const ::android::hardware::radio::V1_0::RadioResponseInfo &info,
      const ::android::hardware::hidl_vec<
          ::android::hardware::radio::V1_5::SetupDataCallResult> &dcResponse) override;

  ::android::hardware::Return<void> setInitialAttachApnResponse_1_5(
      const ::android::hardware::radio::V1_0::RadioResponseInfo &info) override;

  ::android::hardware::Return<void> setDataProfileResponse_1_5(
      const ::android::hardware::radio::V1_0::RadioResponseInfo &info) override;

  ::android::hardware::Return<void> startKeepaliveResponse(
      const ::android::hardware::radio::V1_0::RadioResponseInfo &info,
      const ::android::hardware::radio::V1_1::KeepaliveStatus &activityInfo) override;

  ::android::hardware::Return<void> stopKeepaliveResponse(
      const ::android::hardware::radio::V1_0::RadioResponseInfo &info) override;
};

struct RadioDataResponse {
  ::android::sp<RadioDataResponseHook> mHidl;
  std::shared_ptr<android::hardware::radio::impl::RadioDataResponseCallback>
      mDataCb;

  ::android::sp<RadioDataResponseHook> hidl() { return mHidl; }
  void setResponseFunction(
      std::shared_ptr<android::hardware::radio::impl::RadioDataResponseCallback>
          dataCb) {
    mDataCb = dataCb;
    if (mHidl) {
      mHidl->setResponseFunction(mDataCb);
    }
  }
  static auto make() {
    auto instance = std::make_shared<RadioDataResponse>();
    instance->mHidl = ::android::sp<RadioDataResponseHook>::make();
    return instance;
  }
};

struct RadioDataIndicationHook : public RadioIndicationBase_1_6 {
  std::shared_ptr<android::hardware::radio::impl::RadioDataIndicationCallback>
      mDataCb;

  void setResponseFunction(
      std::shared_ptr<
          android::hardware::radio::impl::RadioDataIndicationCallback>
          dataCb);

  ::android::hardware::Return<void> dataCallListChanged_1_6(
      ::android::hardware::radio::V1_0::RadioIndicationType type,
      const ::android::hardware::hidl_vec<
          ::android::hardware::radio::V1_6::SetupDataCallResult> &dcList) override;

  ::android::hardware::Return<void> pcoData(::android::hardware::radio::V1_0::RadioIndicationType type,
                                            const ::android::hardware::radio::V1_0::PcoDataInfo &pco) override;

  // for unthrottleApn, Implement each 1.6 HIDL, and 2.0 AIDL direct path only,
  // parameter conversion is not reasonable to reuse for up version

  ::android::hardware::Return<void>
  keepaliveStatus(::android::hardware::radio::V1_0::RadioIndicationType type, const ::android::hardware::radio::V1_1::KeepaliveStatus &status);
};

struct RadioDataIndication {
  ::android::sp<RadioDataIndicationHook> mHidl;
  std::shared_ptr<android::hardware::radio::impl::RadioDataIndicationCallback>
      mDataCb;

  ::android::sp<RadioDataIndicationHook> hidl() { return mHidl; }
  void setResponseFunction(
      std::shared_ptr<
          android::hardware::radio::impl::RadioDataIndicationCallback>
          dataCb) {
    mDataCb = dataCb;
    if (mHidl) {
      mHidl->setResponseFunction(mDataCb);
    }
  }

  static auto make() {
    auto instance = std::make_shared<RadioDataIndication>();
    instance->mHidl = ::android::sp<RadioDataIndicationHook>::make();
    return instance;
  }
};

