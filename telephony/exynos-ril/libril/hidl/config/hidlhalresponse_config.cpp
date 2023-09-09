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
#include "hidlhalresponse_config.h"
#include "ril_service_common.h"
#include "radioconfig_service.h"
#include <slsi/radio_v1_5.h>

#define TAG "HidlHalRadioConfigResponse"

using namespace android::hardware::radio::V1_0;
using namespace android::hardware::radio::config;
using namespace android::hardware::radio::config::V1_0;

using ::android::hardware::Return;
using ::android::hardware::hidl_vec;
using android::sp;

void populateResponseInfoForRadioConfig(RadioResponseInfo& responseInfo, int serial, int responseType,RIL_Errno e);

HidlHalRadioConfigResponse::HidlHalRadioConfigResponse(const sp<IRadioConfigResponse>& hidlHalRadioConfigResponse)
    : mRadioConfigResponse(hidlHalRadioConfigResponse),
      mRadioConfigResponseV1_1(V1_1::IRadioConfigResponse::castFrom(hidlHalRadioConfigResponse).withDefault(nullptr)),
      mRadioConfigResponseV1_2(V1_2::IRadioConfigResponse::castFrom(hidlHalRadioConfigResponse).withDefault(nullptr)) {
}

HidlHalRadioConfigResponse::HidlHalRadioConfigResponse(const HidlHalRadioConfigResponse& o)
    : mRadioConfigResponse(o.mRadioConfigResponse),
      mRadioConfigResponseV1_1(o.mRadioConfigResponseV1_1),
      mRadioConfigResponseV1_2(o.mRadioConfigResponseV1_2) {
}

void HidlHalRadioConfigResponse::checkReturnStatus(Return<void>& ret) {
    if (ret.isOk() == false) {
        RLOGE("checkReturnStatus: unable to call response/indication callback.");
        RLOGE("checkReturnStatus: remote process might be died.");
    }
}

int HidlHalRadioConfigResponse::getPhoneCapabilityResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    RadioResponseInfo info = {};
    populateResponseInfoForRadioConfig(info, serial, responseType, e);
    V1_1::PhoneCapability phoneCapability;

    if (mRadioConfigResponseV1_1 != NULL) {
        if (e != RIL_E_SUCCESS) {
            // maxActiveData = 0, maxActiveInternetData=0, isInternetLingeringSupported = 0
            phoneCapability.maxActiveData = 0;
            phoneCapability.maxActiveInternetData = 0;
            phoneCapability.isInternetLingeringSupported = false;
            phoneCapability.logicalModemList.resize(0);
        } else if (response == NULL || responselen == 0 || responselen != sizeof(RIL_PhoneCapability)) {
            RLOGE("getPhoneCapabilityResponse: Invalid response");
            if (e == RIL_E_SUCCESS) info.error = RadioError::INVALID_RESPONSE;
            phoneCapability.maxActiveData = 0;
            phoneCapability.maxActiveInternetData = 0;
            phoneCapability.isInternetLingeringSupported = false;
            phoneCapability.logicalModemList.resize(0);
        } else {
            RIL_PhoneCapability *rilPhoneCapability = (RIL_PhoneCapability *)response;
            phoneCapability.maxActiveData = rilPhoneCapability->maxActiveData;
            phoneCapability.maxActiveInternetData = rilPhoneCapability->maxActiveInternetData;
            phoneCapability.isInternetLingeringSupported = rilPhoneCapability->isInternetLingeringSupported;
            phoneCapability.logicalModemList.resize(0);
            uint32_t num = rilPhoneCapability->len_logicalModemList;
            if (num > 0) {
                phoneCapability.logicalModemList.resize(num);
                for (uint32_t i = 0; i < num; i++) {
                    phoneCapability.logicalModemList[i].modemId =
                        rilPhoneCapability->logicalModemList[i].modemId;
                } // end for i ~

            }
        }
        Return<void> retStatus = mRadioConfigResponseV1_1->getPhoneCapabilityResponse(info, phoneCapability);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioConfigService[%d]->mRadioConfigResponseV1_1 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalRadioConfigResponse::setPreferredDataModemResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    RadioResponseInfo info = {};
    populateResponseInfoForRadioConfig(info, serial, responseType, e);

    if (mRadioConfigResponseV1_1 != NULL) {
        Return<void> retStatus = mRadioConfigResponseV1_1->setPreferredDataModemResponse(info);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioConfigService[%d]->mRadioConfigResponseV1_1 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalRadioConfigResponse::setModemsConfigResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    RadioResponseInfo info = {};
    populateResponseInfoForRadioConfig(info, serial, responseType, e);

    if (mRadioConfigResponseV1_1 != NULL) {
        Return<void> retStatus = mRadioConfigResponseV1_1->setModemsConfigResponse(info);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioConfigService[%d]->mRadioConfigResponseV1_1 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalRadioConfigResponse::getModemsConfigResponse(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    RadioResponseInfo info = {};
    populateResponseInfoForRadioConfig(info, serial, responseType, e);
    V1_1::ModemsConfig modemsConfig;

    if (mRadioConfigResponseV1_1 != NULL) {
        info.error = RadioError::REQUEST_NOT_SUPPORTED;
        modemsConfig.numOfLiveModems = 0;
        Return<void> retStatus = mRadioConfigResponseV1_1->getModemsConfigResponse(info, modemsConfig);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioConfigService[%d]->mRadioConfigResponseV1_1 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}
