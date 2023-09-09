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

#include "radiomessagingindication.h"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::messaging;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioMessagingIndication::setResponseFunction(const std::shared_ptr<aidl::IRadioMessagingIndication>& radioMessagingIndication) {
    execute([&]() {
        mIndication = radioMessagingIndication;
        if (mIndication) {
            linkToDeath(mIndication->asBinder().get());
        }
    });
}

void RadioMessagingIndication::cdmaNewSms(aidlCommon::RadioIndicationType type, const aidl::CdmaSmsMessage& msg) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->cdmaNewSms(type, msg);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingIndication::cdmaRuimSmsStorageFull(aidlCommon::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->cdmaRuimSmsStorageFull(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingIndication::newBroadcastSms(aidlCommon::RadioIndicationType type, const std::vector<uint8_t>& data) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->newBroadcastSms(type, data);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingIndication::newSms(aidlCommon::RadioIndicationType type, const std::vector<uint8_t>& pdu) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->newSms(type, pdu);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingIndication::newSmsOnSim(aidlCommon::RadioIndicationType type, int32_t recordNumber) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->newSmsOnSim(type, recordNumber);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingIndication::newSmsStatusReport(aidlCommon::RadioIndicationType type, const std::vector<uint8_t>& pdu) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->newSmsStatusReport(type, pdu);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingIndication::simSmsStorageFull(aidlCommon::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->simSmsStorageFull(type);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl
