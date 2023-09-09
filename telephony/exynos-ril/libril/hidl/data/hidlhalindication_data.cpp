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
#include <hidl/hidlhalindication_data.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_service_common.h"
#include "ril_service_legacy.h"

#define TAG "HidlHalDataIndication"

using ::android::hardware::hidl_string;
using ::android::hardware::Return;

int HidlHalDataIndication::dataCallListChangedInd_1_4(int slotId,
                                                      int indicationType,
                                                      int token, RIL_Errno e,
                                                      void *response,
                                                      size_t responselen) {

  int ret = 0;
  if (mRadioIndicationV1_4 != NULL) {
    // null response is valid, it means empty datacall, no active datacall,
    // still use v11
    if ((response == NULL && responselen != 0) ||
        responselen % sizeof(RIL_Data_Call_Response_v11) != 0) {
      RLOGE("%s: invalid response", __FUNCTION__);
      return 0;
    }

    // In current RIL_Data_Call_Response_v11 is used here also.
    if (responselen % sizeof(RIL_Data_Call_Response_V1_4) != 0) {
      RLOGD("%s: RIL_Data_Call_Response_V1_4 is not used", __FUNCTION__);
    }

    hidl_vec<V1_4::SetupDataCallResult> dcList;
    vector<vector<string>> tmp;
    Radio2HalUtils::convertRilDataCallListToHal(response, responselen, dcList,
                                                tmp);
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    Return<void> retStatus = mRadioIndicationV1_4->dataCallListChanged_1_4(
        convertIntToRadioIndicationType(indicationType), dcList);
    checkReturnStatus(retStatus);
  } else {
    // In current response is RIL_Data_Call_Response_v11 is used here also.
    // No conversion is needed.
    // When RIL_Data_Call_Response_V1_4 is implemented, convertDataCallList
    // shall be implemented. RadioConvUtils::convertDataCallList(dataCallList,
    // (RIL_Data_Call_Response_V1_4*)response, num);
    ret = radio::dataCallListChangedInd(slotId, indicationType, token, e,
                                        response, responselen);
    RLOGE("%s: radioService[%d]->mRadioIndicationV1_4 == NULL, ret = %d",
          __FUNCTION__, slotId, ret);
  }

  return ret;
}

int HidlHalDataIndication::dataCallListChangedInd_1_5(int slotId,
                                                      int indicationType,
                                                      int token, RIL_Errno e,
                                                      void *response,
                                                      size_t responselen) {

  int ret = 0;
  if (mRadioIndicationV1_5 != NULL) {
    // null response is valid, it means empty datacall, no active datacall,
    // still use v11
    if ((response == NULL && responselen != 0) ||
        responselen % sizeof(RIL_Data_Call_Response_v11) != 0) {
      RLOGE("%s: invalid response : %p, len:%zu", __FUNCTION__, response,
            responselen);
      return 0;
    }

    // In current RIL_Data_Call_Response_v11 is used here also.
    if (responselen % sizeof(RIL_Data_Call_Response_V1_5) != 0) {
      RLOGD("%s: RIL_Data_Call_Response_V1_5 is not used. instead use v11",
            __FUNCTION__);
    }

    hidl_vec<V1_5::SetupDataCallResult> dcList;
    vector<vector<string>> tmp;
    Radio2HalUtils::convertRilDataCallListToHal(response, responselen, dcList,
                                                tmp);
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    Return<void> retStatus = mRadioIndicationV1_5->dataCallListChanged_1_5(
        convertIntToRadioIndicationType(indicationType), dcList);
    checkReturnStatus(retStatus);
  } else {
    // In current response is RIL_Data_Call_Response_v11 is used here also.
    // No conversion is needed.
    // When RIL_Data_Call_Response_V1_5 is implemented, convertDataCallList
    // shall be implemented. RadioConvUtils::convertDataCallList(dataCallList,
    // (RIL_Data_Call_Response_V1_5*)response, num);
    ret = radio::dataCallListChangedInd_1_4(slotId, indicationType, token, e,
                                        response, responselen);
    RLOGE("%s: radioService[%d]->mRadioIndicationV1_5 == NULL, ret = %d",
          __FUNCTION__, slotId, ret);
  }

  return ret;
}

int HidlHalDataIndication::pcoDataInd(int slotId, int indicationType, int token,
                                      RIL_Errno e, void *response,
                                      size_t responselen) {

  if (mRadioIndication != NULL) {
    if (response == NULL || responselen != sizeof(RIL_PCO_Data)) {
      RLOGE("%s: invalid response", __FUNCTION__);
      return 0;
    }

    PcoDataInfo pco = {};
    RIL_PCO_Data *rilPcoData = (RIL_PCO_Data *)response;
    pco.cid = rilPcoData->cid;
    pco.bearerProto =
        Radio2HalUtils::convertCharPtrToHidlString(rilPcoData->bearer_proto);
    pco.pcoId = rilPcoData->pco_id;
    pco.contents.setToExternal((uint8_t *)rilPcoData->contents,
                               rilPcoData->contents_length);

#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    Return<void> retStatus = mRadioIndication->pcoData(
        convertIntToRadioIndicationType(indicationType), pco);
    checkReturnStatus(retStatus);
  } else {
    RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__,
          slotId);
  }

  return 0;
}

int HidlHalDataIndication::keepaliveStatusInd(int slotId, int indicationType,
                                              int token, RIL_Errno e,
                                              void *response,
                                              size_t responselen) {

#if VDBG
  RLOGD("%s: token=%d", __FUNCTION__, token);
#endif
  if (mRadioIndication == NULL) {
    RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__,
          slotId);
    return 0;
  }

  auto ret = V1_1::IRadioIndication::castFrom(mRadioIndication);
  if (!ret.isOk()) {
    RLOGE("%s: ret.isOk() == false for radioService[%d]", __FUNCTION__, slotId);
    return 0;
  }
  android::sp<V1_1::IRadioIndication> radioIndicationV1_1 = ret;

  if (response == NULL || responselen != sizeof(V1_1::KeepaliveStatus)) {
    RLOGE("%s: invalid response", __FUNCTION__);
    return 0;
  }

  V1_1::KeepaliveStatus ks;
  Radio2HalUtils::convertRilKeepaliveStatusToHal(
      static_cast<RIL_KeepaliveStatus *>(response), ks);

  Return<void> retStatus = radioIndicationV1_1->keepaliveStatus(
      convertIntToRadioIndicationType(indicationType), ks);
  checkReturnStatus(retStatus);
  return 0;
}

