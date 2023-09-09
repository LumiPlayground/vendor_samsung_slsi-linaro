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
/*
 * radio_config_indication_1_2.cpp
 */
#include "radio_config_hidl_utils_1_2.h"

using ::android::sp;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

/* 1.2 Apis */
Return<void> RadioConfigIndicationBase_1_2::simSlotsStatusChanged_1_2(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<::android::hardware::radio::config::V1_2::SimSlotStatus>& /*slotStatus*/) {
    return Void();
}

/* 1.0 Apis */
Return<void> RadioConfigIndicationBase_1_2::simSlotsStatusChanged(
        RadioIndicationType /*type*/,
        const ::android::hardware::hidl_vec<::android::hardware::radio::config::V1_0::SimSlotStatus>& /*slotStatus*/) {
    return Void();
}