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
#include "radioconfigresponse.h"
#define TAG "RadioConfigResponse"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::config;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioConfigResponse::setResponseFunction(
        const std::shared_ptr<aidl::IRadioConfigResponse>& radioConfigResponse) {
    execute([&]() {
        mResponse = radioConfigResponse;
        if (mResponse) {
            linkToDeath(mResponse->asBinder().get());
        }
    });
}

void RadioConfigResponse::getHalDeviceCapabilitiesResponse(const aidlCommon::RadioResponseInfo& info, bool modemReducedFeatureSet1) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getHalDeviceCapabilitiesResponse(info, modemReducedFeatureSet1);
            checkReturnStatus(astatus);
        }
    });
}

void RadioConfigResponse::getNumOfLiveModemsResponse(const aidlCommon::RadioResponseInfo& info, int8_t numOfLiveModems) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getNumOfLiveModemsResponse(info, numOfLiveModems);
            checkReturnStatus(astatus);
        }
    });
}

void RadioConfigResponse::getPhoneCapabilityResponse(const aidlCommon::RadioResponseInfo& info, const aidl::PhoneCapability& phoneCapability) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getPhoneCapabilityResponse(info, phoneCapability);
            checkReturnStatus(astatus);
        }
    });
}

void RadioConfigResponse::getSimSlotsStatusResponse(const aidlCommon::RadioResponseInfo& info, const std::vector<aidl::SimSlotStatus>& slotStatus) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getSimSlotsStatusResponse(info, slotStatus);
            checkReturnStatus(astatus);
        }
    });
}

void RadioConfigResponse::setNumOfLiveModemsResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setNumOfLiveModemsResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioConfigResponse::setPreferredDataModemResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setPreferredDataModemResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioConfigResponse::setSimSlotsMappingResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setSimSlotsMappingResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

}