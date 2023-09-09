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
#include "radiodataservice.h"
#include <ril_internal.h>
#include <aidl-utils.h>
#include "commandsinfo.h"

using android::hardware::radio::impl::RadioData;

#define TAG "RadioDataService"

bool RadioDataService::publish() {
    mRadioData = RadioData::make(mRadio->data(), mSlot);
    bool published = publishRadioHal<RadioData>(mRadioData, mSlot);
    RLOGD("%s::%s publish %s", TAG, __FUNCTION__, published ? "success" : "fail");
    if (published) {
        updateCommandsInfo(mSlot, mRadioResponse->data());
        updateUnsolResponseInfo(mSlot, mRadioIndication->data());
    } else {
        mRadioData = nullptr;
    }

    return published;
}
