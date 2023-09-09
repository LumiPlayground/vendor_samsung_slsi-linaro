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

#include "radiodataindication.h"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::data;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioDataIndication::setResponseFunction(const std::shared_ptr<aidl::IRadioDataIndication>& radioDataIndication) {
    execute([&]() {
        mIndication = radioDataIndication;
        if (mIndication) {
            linkToDeath(mIndication->asBinder().get());
        }
    });
}
void RadioDataIndication::dataCallListChanged(
    const ::aidl::android::hardware::radio::RadioIndicationType type,
    const std::vector<
        ::aidl::android::hardware::radio::data::SetupDataCallResult>
        dcList) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->dataCallListChanged(type, dcList);
            checkReturnStatus(astatus);
        }
    });
}
void RadioDataIndication::keepaliveStatus(
    const ::aidl::android::hardware::radio::RadioIndicationType type,
    const ::aidl::android::hardware::radio::data::KeepaliveStatus keepAliveStatus) {
    execute([&]() {
        auto astatus = mIndication->keepaliveStatus(type, keepAliveStatus);
        checkReturnStatus(astatus);
    });
}
void RadioDataIndication::pcoData(
    const ::aidl::android::hardware::radio::RadioIndicationType type,
    const ::aidl::android::hardware::radio::data::PcoDataInfo pco) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->pcoData(type, pco);
            checkReturnStatus(astatus);
        }
    });
}
void RadioDataIndication::unthrottleApn(
    const ::aidl::android::hardware::radio::RadioIndicationType type,
    const ::aidl::android::hardware::radio::data::DataProfileInfo dp) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->unthrottleApn(type, dp);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl
