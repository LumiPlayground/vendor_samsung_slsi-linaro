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

#include <android/hardware/radio/1.5/IRadioResponse.h>
#include <android/hardware/radio/1.5/IRadioIndication.h>
#include <telephony/ril.h>

struct HidlHalResponse {
    ::android::sp<::android::hardware::radio::V1_0::IRadioResponse> mRadioResponse;
    ::android::sp<::android::hardware::radio::V1_1::IRadioResponse> mRadioResponseV1_1;
    ::android::sp<::android::hardware::radio::V1_2::IRadioResponse> mRadioResponseV1_2;
    ::android::sp<::android::hardware::radio::V1_3::IRadioResponse> mRadioResponseV1_3;
    ::android::sp<::android::hardware::radio::V1_4::IRadioResponse> mRadioResponseV1_4;
    ::android::sp<::android::hardware::radio::V1_5::IRadioResponse> mRadioResponseV1_5;

    HidlHalResponse() = default;
    HidlHalResponse(const ::android::sp<::android::hardware::radio::V1_0::IRadioResponse>& hidlHalResponse);
    HidlHalResponse(const HidlHalResponse& o);
};

struct HidlHalIndication {
    ::android::sp<::android::hardware::radio::V1_0::IRadioIndication> mRadioIndication;
    ::android::sp<::android::hardware::radio::V1_1::IRadioIndication> mRadioIndicationV1_1;
    ::android::sp<::android::hardware::radio::V1_2::IRadioIndication> mRadioIndicationV1_2;
    ::android::sp<::android::hardware::radio::V1_3::IRadioIndication> mRadioIndicationV1_3;
    ::android::sp<::android::hardware::radio::V1_4::IRadioIndication> mRadioIndicationV1_4;
    ::android::sp<::android::hardware::radio::V1_5::IRadioIndication> mRadioIndicationV1_5;

    HidlHalIndication() = default;
    HidlHalIndication(const ::android::sp<::android::hardware::radio::V1_0::IRadioIndication>& hidlHalIndication);
    HidlHalIndication(const HidlHalIndication& o);
};

struct CheckReturnStatus {
    void checkReturnStatus(::android::hardware::Return<void>& ret);
};