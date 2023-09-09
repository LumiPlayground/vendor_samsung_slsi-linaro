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

#include "radioconfigindication.h"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::config;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioConfigIndication::setResponseFunction(
        const std::shared_ptr<aidl::IRadioConfigIndication>& radioConfigIndication) {
    execute([&]() {
        mIndication = radioConfigIndication;
        if (mIndication) {
            linkToDeath(mIndication->asBinder().get());
        }
    });
}

void RadioConfigIndication::simSlotsStatusChanged(aidlCommon::RadioIndicationType type,
        const std::vector<aidl::SimSlotStatus>& slotStatus) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->simSlotsStatusChanged(type, slotStatus);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl