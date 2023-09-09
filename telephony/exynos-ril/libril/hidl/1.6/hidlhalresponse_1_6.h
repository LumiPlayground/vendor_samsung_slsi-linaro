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

#include <android/hardware/radio/1.6/IRadioResponse.h>
#include <android/hardware/radio/1.6/IRadioIndication.h>
#include <hidl/hidlhalresponse.h>

struct HidlHalResponse_1_6 : public HidlHalResponse {
    ::android::sp<::android::hardware::radio::V1_6::IRadioResponse> mRadioResponseV1_6;

    HidlHalResponse_1_6() = default;
    HidlHalResponse_1_6(::android::sp<::android::hardware::radio::V1_0::IRadioResponse> hidlHalResponse)
        : HidlHalResponse(hidlHalResponse),
          mRadioResponseV1_6(::android::hardware::radio::V1_6::IRadioResponse::castFrom(hidlHalResponse).
            withDefault(nullptr)) {
    }
    HidlHalResponse_1_6(const HidlHalResponse_1_6& o)
        : HidlHalResponse(static_cast<const HidlHalResponse&>(o)),
          mRadioResponseV1_6(o.mRadioResponseV1_6) {}
};

struct HidlHalIndication_1_6 : public HidlHalIndication {
    ::android::sp<::android::hardware::radio::V1_6::IRadioIndication> mRadioIndicationV1_6;

    HidlHalIndication_1_6() = default;
    HidlHalIndication_1_6(const ::android::sp<::android::hardware::radio::V1_0::IRadioIndication>& hidlHalIndication)
        : HidlHalIndication(hidlHalIndication),
          mRadioIndicationV1_6(::android::hardware::radio::V1_6::IRadioIndication::castFrom(hidlHalIndication).
            withDefault(nullptr)) {
    }
    HidlHalIndication_1_6(const HidlHalIndication_1_6& o)
        : HidlHalIndication(static_cast<const HidlHalIndication&>(o)),
          mRadioIndicationV1_6(o.mRadioIndicationV1_6) {}
};