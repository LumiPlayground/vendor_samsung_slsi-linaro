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
#include "radiosimindication.h"
#define TAG "RadioSimIndication"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::sim;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioSimIndication::setResponseFunction(
    const std::shared_ptr<aidl::IRadioSimIndication>& radioSimIndication) {
    execute([&]() {
        mIndication = radioSimIndication;
        if (mIndication) {
            linkToDeath(mIndication->asBinder().get());
        }
    });
}

void RadioSimIndication::carrierInfoForImsiEncryption(aidlCommon::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->carrierInfoForImsiEncryption(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::cdmaSubscriptionSourceChanged(aidlCommon::RadioIndicationType type,
                                                       aidl::CdmaSubscriptionSource cdmaSource) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->cdmaSubscriptionSourceChanged(type, cdmaSource);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::simPhonebookChanged(aidlCommon::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->simPhonebookChanged(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::simPhonebookRecordsReceived(aidlCommon::RadioIndicationType type,
                                                     aidl::PbReceivedStatus status, const std::vector<aidl::PhonebookRecordInfo>& records) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->simPhonebookRecordsReceived(type, status, records);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::simRefresh(aidlCommon::RadioIndicationType type,
                                    const aidl::SimRefreshResult& refreshResult) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->simRefresh(type, refreshResult);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::simStatusChanged(aidlCommon::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->simStatusChanged(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::stkEventNotify(aidlCommon::RadioIndicationType type,
                                        const std::string& cmd) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->stkEventNotify(type, cmd);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::stkProactiveCommand(aidlCommon::RadioIndicationType type,
                                             const std::string& cmd) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->stkProactiveCommand(type, cmd);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::stkSessionEnd(aidlCommon::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->stkSessionEnd(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::subscriptionStatusChanged(aidlCommon::RadioIndicationType type,
                                                   bool activate) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->subscriptionStatusChanged(type, activate);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimIndication::uiccApplicationsEnablementChanged(aidlCommon::RadioIndicationType type,
                                                           bool enabled) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->uiccApplicationsEnablementChanged(type, enabled);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl
