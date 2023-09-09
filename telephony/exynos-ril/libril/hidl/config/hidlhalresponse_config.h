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

#include <android/hardware/radio/config/1.2/IRadioConfigResponse.h>
#include <telephony/ril.h>

struct HidlHalRadioConfigResponse {
    ::android::sp<::android::hardware::radio::config::V1_0::IRadioConfigResponse> mRadioConfigResponse;
    ::android::sp<::android::hardware::radio::config::V1_1::IRadioConfigResponse> mRadioConfigResponseV1_1;
    ::android::sp<::android::hardware::radio::config::V1_2::IRadioConfigResponse> mRadioConfigResponseV1_2;

    HidlHalRadioConfigResponse() = default;
    HidlHalRadioConfigResponse(const ::android::sp<::android::hardware::radio::config::V1_0::IRadioConfigResponse>& hidlHalRadioConfigResponse);
    HidlHalRadioConfigResponse(const HidlHalRadioConfigResponse& o);

    void checkReturnStatus(::android::hardware::Return<void>& ret);

    int getPhoneCapabilityResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);

    int setPreferredDataModemResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);

    int setModemsConfigResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);

    int getModemsConfigResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);

};