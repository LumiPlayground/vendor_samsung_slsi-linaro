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

#include "radiomodemindication.h"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::modem;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioModemIndication::setResponseFunction(
        const std::shared_ptr<aidl::IRadioModemIndication>& radioModemIndication) {
    execute([&]() {
        mIndication = radioModemIndication;
        if (mIndication) {
            linkToDeath(mIndication->asBinder().get());
        }
    });
}

void RadioModemIndication::hardwareConfigChanged(aidlCommon::RadioIndicationType type,
                                                 const std::vector<aidl::HardwareConfig>& configs) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->hardwareConfigChanged(type, configs);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemIndication::modemReset(aidlCommon::RadioIndicationType type,
                                      const std::string& reason) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->modemReset(type, reason);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemIndication::radioCapabilityIndication(aidlCommon::RadioIndicationType type,
                                                     const aidl::RadioCapability& rc) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->radioCapabilityIndication(type, rc);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemIndication::radioStateChanged(aidlCommon::RadioIndicationType type,
                                             aidl::RadioState radioState) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->radioStateChanged(type, radioState);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemIndication::rilConnected(aidlCommon::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->rilConnected(type);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl