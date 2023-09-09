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
#include <aidl-service/aidl-log.h>

#include "radioconfigresponse.h"
#include "commonStructs.h"
#include "structs.h"
#include "collections.h"
#include <ril_internal.h>

namespace aidl_config = ::aidl::android::hardware::radio::config;
using namespace ::android::hardware::radio::compat;
using namespace ::android::hardware::radio::impl;
using ::android::hardware::Return;
using ::android::hardware::Void;

#define TAG "RadioConfigIndicationHook"

void RadioConfigIndicationHook::setResponseFunction(std::shared_ptr<RadioConfigIndicationCallback> configCb) {
    RLOGD("%s::%s configCb=%p", TAG, __FUNCTION__, configCb.get());
    mConfigCb = configCb;
}
