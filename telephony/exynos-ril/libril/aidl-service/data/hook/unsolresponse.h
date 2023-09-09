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

#include <hidl/1.6/hidlhalindication_data.h>
#include <hidl/hidlhalindication_data.h>

namespace android::hardware::radio::impl::data {

struct BaseDataUnsolResponse {
  ::android::sp<RadioDataIndicationHook> mHook;
  BaseDataUnsolResponse(const ::android::sp<RadioDataIndicationHook> &hook)
      : mHook(hook) {}
};

//Under Ver 1.6

struct dataCallListChangedInd_1_4 : public BaseDataUnsolResponse {
  using BaseDataUnsolResponse::BaseDataUnsolResponse;

  int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                 void *response, size_t responselen) {
    return HidlHalDataIndication(mHook).dataCallListChangedInd_1_4(
        slotId, indicationType, token, e, response, responselen);
  }
};

struct dataCallListChangedInd_1_5 : public BaseDataUnsolResponse {
  using BaseDataUnsolResponse::BaseDataUnsolResponse;

  int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                 void *response, size_t responselen) {
    return HidlHalDataIndication(mHook).dataCallListChangedInd_1_5(
        slotId, indicationType, token, e, response, responselen);
  }
};

struct keepaliveStatusInd : public BaseDataUnsolResponse {
    using BaseDataUnsolResponse::BaseDataUnsolResponse;

    int operator()(int slotId, int indicationType, int token, RIL_Errno e,
        void* response, size_t responselen) {
        return HidlHalDataIndication(mHook).keepaliveStatusInd(
            slotId, indicationType, token, e, response, responselen);
    }
};

//Ver 1.6

struct dataCallListChangedInd_1_6 : public BaseDataUnsolResponse {
  using BaseDataUnsolResponse::BaseDataUnsolResponse;

  int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                 void *response, size_t responselen) {
    return HidlHalDataIndication_1_6(mHook).dataCallListChangedInd_1_6(
        slotId, indicationType, token, e, response, responselen);
  }
};

struct pcoDataInd : public BaseDataUnsolResponse {
  using BaseDataUnsolResponse::BaseDataUnsolResponse;

  int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                 void *response, size_t responselen) {
    return HidlHalDataIndication(mHook).pcoDataInd(
        slotId, indicationType, token, e, response, responselen);
  }
};

struct unthrottleApn : public BaseDataUnsolResponse {
  using BaseDataUnsolResponse::BaseDataUnsolResponse;

  int operator()(int slotId, int indicationType, int token, RIL_Errno e,
                 void *response, size_t responselen) {
    return HidlHalDataIndication_1_6(mHook).unthrottleApn(
        slotId, indicationType, token, e, response, responselen);
  }
};

} // namespace android::hardware::radio::impl::data

