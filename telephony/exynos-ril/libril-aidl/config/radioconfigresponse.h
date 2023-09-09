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

#include "radioconfigcallback.h"
#include "radioreturnstatus.h"
#include "radioresponseutil.h"
#include "binderdeathrecipient.h"

namespace android::hardware::radio::impl {

struct RadioConfigResponse : public RadioConfigResponseCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::config::IRadioConfigResponse> mResponse;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
    }
    void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::config::IRadioConfigResponse>&
                    radioConfigResponse);
    void getHalDeviceCapabilitiesResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool modemReducedFeatureSet1) override;
    void getNumOfLiveModemsResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info, int8_t numOfLiveModems) override;
    void getPhoneCapabilityResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info, const ::aidl::android::hardware::radio::config::PhoneCapability& phoneCapability) override;
    void getSimSlotsStatusResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info, const std::vector<::aidl::android::hardware::radio::config::SimSlotStatus>& slotStatus) override;
    void setNumOfLiveModemsResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void setPreferredDataModemResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void setSimSlotsMappingResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
};

}
