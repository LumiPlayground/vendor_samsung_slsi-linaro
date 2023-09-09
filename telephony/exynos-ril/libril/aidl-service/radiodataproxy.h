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

#include <radiodatacallback.h>
#include <radiocallbackadapter.h>
#include <radiointerface.h>

struct RadioDataProxy
    : public RadioInterface,
      public android::hardware::radio::impl::RadioDataCallbackAdapter {
  using RadioInterface::RadioInterface;

  std::shared_ptr<android::hardware::radio::impl::RadioDataResponseCallback>
      mResponseCb;
  std::shared_ptr<android::hardware::radio::impl::RadioDataIndicationCallback>
      mIndicationCb;

  template <typename T, typename... Args>
  void onRequest(int request, Args &&...args) {
    auto command = getCommand<T>(request);
    if (command == nullptr) {
      command = std::make_shared<T>(mSlot, mResponseCb);
      RadioInterface::updateCommand(request, command);
    }
    RadioInterface::onRequest(command, args...);
  }

  template <typename T> void listen(int indication) {
    RadioInterface::updateIndication(indication,
                                     std::make_shared<T>(mSlot, mIndicationCb));
  }

  void setResponseFunctions(
      const std::shared_ptr<
          android::hardware::radio::impl::RadioDataResponseCallback>
          &responseCb,
      const std::shared_ptr<
          android::hardware::radio::impl::RadioDataIndicationCallback>
          &indicationCb);

  void allocatePduSessionId(int32_t serial) override;
  void cancelHandover(int32_t serial, int32_t callId) override;
  void
  deactivateDataCall(int32_t serial, int32_t cid,
                     ::aidl::android::hardware::radio::data::DataRequestReason
                         reason) override;
  void getDataCallList(int32_t serial) override;
  void getSlicingConfig(int32_t serial) override;
  void releasePduSessionId(int32_t serial, int32_t id) override;
  void responseAcknowledgement() override;
  void setDataAllowed(int32_t serial, bool allow) override;
  void setDataProfile(
      int32_t serial,
      const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo>
          &profiles) override;
  void
  setDataThrottling(int32_t serial,
                    ::aidl::android::hardware::radio::data::DataThrottlingAction
                        dataThrottlingAction,
                    int64_t completionDurationMillis) override;
  void setInitialAttachApn(
      int32_t serial,
      const std::optional<::aidl::android::hardware::radio::data::DataProfileInfo> &dpInfo)
      override;
  void setupDataCall(
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
      bool matchAllRuleAllowed) override;
  void startHandover(int32_t serial, int32_t callId) override;
  void
  startKeepalive(int32_t serial,
                 const ::aidl::android::hardware::radio::data::KeepaliveRequest
                     &keepalive) override;
  void stopKeepalive(int32_t serial, int32_t sessionHandle) override;

  static auto make(uint32_t slot, const std::shared_ptr<RadioProxy> &radio) {
    return std::make_shared<RadioDataProxy>(slot, radio);
  }
};

