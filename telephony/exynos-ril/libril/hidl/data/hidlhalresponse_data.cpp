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

#include <hidl-log.h>
#include <hidl/hidlhalresponse_data.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_internal.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalDataResponse"

using ::android::hardware::hidl_string;
using ::android::hardware::Return;

int HidlHalDataResponse::setupDataCallResponse_1_5(int slotId, int responseType,
                                                   int serial, RIL_Errno e,
                                                   void *response,
                                                   size_t responselen) {
#if VDBG
  RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif
  if (mRadioResponseV1_5 != NULL) {
    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);

    V1_5::SetupDataCallResult dcResult = {};
    std::vector<std::vector<std::string>> t;
    t.resize(4);
    // User v11 format, For V1_4, only two parameter are removed, and just type
    // change for Cause
    if (response == NULL || responselen != sizeof(RIL_Data_Call_Response_v11)) {
      RLOGE("%s: Invalid response", __FUNCTION__);
      if (e == RIL_E_SUCCESS)
        responseInfo.error = RadioError::INVALID_RESPONSE;
      dcResult.cause = V1_4::DataCallFailCause::ERROR_UNSPECIFIED;
      dcResult.type = V1_4::PdpProtocolType::UNKNOWN;
      dcResult.ifname = hidl_string();
      dcResult.addresses.resize(0);
      dcResult.dnses.resize(0);
      dcResult.gateways.resize(0);
      dcResult.pcscf.resize(0);
    } else {
      RLOGE("%s: convertRilDataCallToHal", __FUNCTION__);
      Radio2HalUtils::convertRilDataCallToHal(
          (RIL_Data_Call_Response_v11 *)response, dcResult, t, 0);
    }
    Return<void> retStatus =
        mRadioResponseV1_5->setupDataCallResponse_1_5(responseInfo, dcResult);
    checkReturnStatus(retStatus);
  } else {
    RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__,
          slotId);
    return radio::setupDataCallResponse_1_4(slotId, responseType, serial, e,
                                            response, responselen);
  }

  return 0;
}

int HidlHalDataResponse::deactivateDataCallResponse(int slotId,
                                                    int responseType,
                                                    int serial, RIL_Errno e,
                                                    void *response,
                                                    size_t responselen) {
#if VDBG
  RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

  if (mRadioResponseV1_2 != NULL) {
    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);
    Return<void> retStatus =
        mRadioResponseV1_2->deactivateDataCallResponse(responseInfo);
    checkReturnStatus(retStatus);
  } else {
    RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
  }

  return 0;
}

int HidlHalDataResponse::setDataAllowedResponse(int slotId, int responseType,
                                                int serial, RIL_Errno e,
                                                void *response,
                                                size_t responselen) {
#if VDBG
  RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

  if (mRadioResponse != NULL) {
    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);
    Return<void> retStatus =
        mRadioResponse->setDataAllowedResponse(responseInfo);
    checkReturnStatus(retStatus);
  } else {
    RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
  }

  return 0;
}

int HidlHalDataResponse::setInitialAttachApnResponse_1_5(int slotId,
                                                     int responseType,
                                                     int serial, RIL_Errno e,
                                                     void *response,
                                                     size_t responselen) {
#if VDBG
  RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

  if (mRadioResponseV1_5 != NULL) {
    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);
    Return<void> retStatus =
        mRadioResponseV1_5->setInitialAttachApnResponse_1_5(responseInfo);
    checkReturnStatus(retStatus);
  } else {
    RLOGE("%s: radioService[%d]->mRadioResponseV1_5 == NULL", __FUNCTION__,
          slotId);
    return radio::setInitialAttachApnResponse(slotId, responseType, serial, e,
                                              response, responselen);
  }

  return 0;
}

int HidlHalDataResponse::startKeepaliveResponse(int slotId, int responseType,
                                                int serial, RIL_Errno e,
                                                void *response,
                                                size_t responselen) {
#if VDBG
  RLOGD("%s: %d", __FUNCTION__, serial);
#endif
  RadioResponseInfo responseInfo = {};
  populateResponseInfo(responseInfo, serial, responseType, e);

  // If we don't have a radio service, there's nothing we can do
  if (mRadioResponseV1_1 == NULL) {
    RLOGE("%s: radioService[%d]->mRadioResponseV1_1 == NULL", __FUNCTION__,
          slotId);
    return 0;
  }

  V1_1::KeepaliveStatus ks = {};
  if (response == NULL || responselen != sizeof(V1_1::KeepaliveStatus)) {
    RLOGE("%s: invalid response - %d", __FUNCTION__, static_cast<int>(e));
    if (e == RIL_E_SUCCESS)
      responseInfo.error = RadioError::INVALID_RESPONSE;
  } else {
    Radio2HalUtils::convertRilKeepaliveStatusToHal(
        static_cast<RIL_KeepaliveStatus *>(response), ks);
  }

  Return<void> retStatus =
      mRadioResponseV1_1->startKeepaliveResponse(responseInfo, ks);
  checkReturnStatus(retStatus);
  return 0;
}

int HidlHalDataResponse::stopKeepaliveResponse(int slotId, int responseType,
                                               int serial, RIL_Errno e,
                                               void *response,
                                               size_t responselen) {
#if VDBG
  RLOGD("%s: %d", __FUNCTION__, serial);
#endif
  RadioResponseInfo responseInfo = {};
  populateResponseInfo(responseInfo, serial, responseType, e);

  // If we don't have a radio service, there's nothing we can do
  if (mRadioResponseV1_1 == NULL) {
    RLOGE("%s: radioService[%d]->mRadioResponseV1_1 == NULL", __FUNCTION__,
          slotId);
    return 0;
  }

  Return<void> retStatus =
      mRadioResponseV1_1->stopKeepaliveResponse(responseInfo);
  checkReturnStatus(retStatus);
  return 0;
}

int HidlHalDataResponse::setDataProfileResponse_1_5(int slotId, int responseType,
                                                int serial, RIL_Errno e,
                                                void *response,
                                                size_t responseLen) {
#if VDBG
  RLOGD("%s: serial %d", __FUNCTION__, serial);
#endif

  if (mRadioResponseV1_5 != NULL) {
    RadioResponseInfo responseInfo = {};
    populateResponseInfo(responseInfo, serial, responseType, e);
    Return<void> retStatus =
        mRadioResponseV1_5->setDataProfileResponse_1_5(responseInfo);
    checkReturnStatus(retStatus);
  } else {
    RLOGE("%s: radioService[%d]->mRadioResponse == NULL", __FUNCTION__, slotId);
  }

  return 0;
}

