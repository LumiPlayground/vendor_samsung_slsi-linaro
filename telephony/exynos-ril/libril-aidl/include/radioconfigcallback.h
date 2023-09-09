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

#include <aidl/android/hardware/radio/config/BnRadioConfig.h>

namespace android::hardware::radio::impl {

struct RadioConfigResponseCallback;
struct RadioConfigIndicationCallback;

struct RadioConfigCallback {
    virtual ~RadioConfigCallback() = default;
    virtual void setResponseFunctions(
        const std::shared_ptr<RadioConfigResponseCallback>& responseCb,
        const std::shared_ptr<RadioConfigIndicationCallback>& indicationCb) = 0;
    virtual void getHalDeviceCapabilities(int32_t serial) = 0;
    virtual void getNumOfLiveModems(int32_t serial) = 0;
    virtual void getPhoneCapability(int32_t serial) = 0;
    virtual void getSimSlotsStatus(int32_t serial) = 0;
    virtual void setNumOfLiveModems(int32_t serial, int8_t numOfLiveModems) = 0;
    virtual void setPreferredDataModem(int32_t serial, int8_t modemId) = 0;
    virtual void setSimSlotsMapping(int32_t serial,
            const std::vector<aidl::android::hardware::radio::config::SlotPortMapping>& slotMap)
            = 0;
};

struct RadioConfigResponseCallback {
    virtual ~RadioConfigResponseCallback() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::config::IRadioConfigResponse>&
                    radioConfigResponse) = 0;
    virtual void getHalDeviceCapabilitiesResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        bool modemReducedFeatureSet1) = 0;
    virtual void getNumOfLiveModemsResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        int8_t numOfLiveModems) = 0;
    virtual void getPhoneCapabilityResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        const ::aidl::android::hardware::radio::config::PhoneCapability& phoneCapability) = 0;
    virtual void getSimSlotsStatusResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        const std::vector<::aidl::android::hardware::radio::config::SimSlotStatus>& slotStatus) = 0;
    virtual void setNumOfLiveModemsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setPreferredDataModemResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setSimSlotsMappingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
};

struct RadioConfigIndicationCallback {
    virtual ~RadioConfigIndicationCallback() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::config::IRadioConfigIndication>&
                    radioConfigIndication)=0;
    virtual void simSlotsStatusChanged(
        ::aidl::android::hardware::radio::RadioIndicationType in_type,
        const std::vector<::aidl::android::hardware::radio::config::SimSlotStatus>& slotStatus) = 0;
};

}  // namespace android::hardware::radio::impl {