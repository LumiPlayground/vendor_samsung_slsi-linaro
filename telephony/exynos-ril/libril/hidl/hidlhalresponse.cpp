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
#include "hidlhalresponse.h"
#include <slsi/radio_v1_5.h>
#include <utils/Log.h>

using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;
using ::android::hardware::Return;
using android::sp;

HidlHalResponse::HidlHalResponse(const sp<IRadioResponse>& hidlHalResponse)
    : mRadioResponse(hidlHalResponse),
      mRadioResponseV1_1(V1_1::IRadioResponse::castFrom(hidlHalResponse).withDefault(nullptr)),
      mRadioResponseV1_2(V1_2::IRadioResponse::castFrom(hidlHalResponse).withDefault(nullptr)),
      mRadioResponseV1_3(V1_3::IRadioResponse::castFrom(hidlHalResponse).withDefault(nullptr)),
      mRadioResponseV1_4(V1_4::IRadioResponse::castFrom(hidlHalResponse).withDefault(nullptr)),
      mRadioResponseV1_5(V1_5::IRadioResponse::castFrom(hidlHalResponse).withDefault(nullptr)) {
}

HidlHalResponse::HidlHalResponse(const HidlHalResponse& o)
    : mRadioResponse(o.mRadioResponse),
      mRadioResponseV1_1(o.mRadioResponseV1_1),
      mRadioResponseV1_2(o.mRadioResponseV1_2),
      mRadioResponseV1_3(o.mRadioResponseV1_3),
      mRadioResponseV1_4(o.mRadioResponseV1_4),
      mRadioResponseV1_5(o.mRadioResponseV1_5) {
}

HidlHalIndication::HidlHalIndication(const sp<IRadioIndication>& hidlHalIndication)
    : mRadioIndication(hidlHalIndication),
      mRadioIndicationV1_1(V1_1::IRadioIndication::castFrom(hidlHalIndication).withDefault(nullptr)),
      mRadioIndicationV1_2(V1_2::IRadioIndication::castFrom(hidlHalIndication).withDefault(nullptr)),
      mRadioIndicationV1_3(V1_3::IRadioIndication::castFrom(hidlHalIndication).withDefault(nullptr)),
      mRadioIndicationV1_4(V1_4::IRadioIndication::castFrom(hidlHalIndication).withDefault(nullptr)),
      mRadioIndicationV1_5(V1_5::IRadioIndication::castFrom(hidlHalIndication).withDefault(nullptr)) {
}

HidlHalIndication::HidlHalIndication(const HidlHalIndication& o)
    : mRadioIndication(o.mRadioIndication),
      mRadioIndicationV1_1(o.mRadioIndicationV1_1),
      mRadioIndicationV1_2(o.mRadioIndicationV1_2),
      mRadioIndicationV1_3(o.mRadioIndicationV1_3),
      mRadioIndicationV1_4(o.mRadioIndicationV1_4),
      mRadioIndicationV1_5(o.mRadioIndicationV1_5) {
}

void CheckReturnStatus::checkReturnStatus(Return<void>& ret) {
    if (ret.isOk() == false) {
        RLOGE("checkReturnStatus: unable to call response/indication callback.");
        RLOGE("checkReturnStatus: remote process might be died.");
    }
}