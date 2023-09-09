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
#include "radiosimresponse.h"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::sim;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioSimResponse::setResponseFunction(
    const std::shared_ptr<aidl::IRadioSimResponse>& radioSimResponse) {
    execute([&]() {
        mResponse = radioSimResponse;
        if (mResponse) {
            linkToDeath(mResponse->asBinder().get());
        }
    });
}

void RadioSimResponse::acknowledgeRequest(int32_t serial) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acknowledgeRequest(serial);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::areUiccApplicationsEnabledResponse(
    const aidlCommon::RadioResponseInfo& info, bool enabled) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->areUiccApplicationsEnabledResponse(info, enabled);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::changeIccPin2ForAppResponse(
    const aidlCommon::RadioResponseInfo& info, int remainingRetries) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->changeIccPin2ForAppResponse(info, remainingRetries);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::changeIccPinForAppResponse(
    const aidlCommon::RadioResponseInfo& info, int remainingRetries) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->changeIccPinForAppResponse(info, remainingRetries);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::enableUiccApplicationsResponse(
    const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->enableUiccApplicationsResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::getAllowedCarriersResponse(
    const aidlCommon::RadioResponseInfo& info,
    const aidl::CarrierRestrictions& carriers,
    const aidl::SimLockMultiSimPolicy& multiSimPolicy) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getAllowedCarriersResponse(info, carriers, multiSimPolicy);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::getCdmaSubscriptionResponse(
    const aidlCommon::RadioResponseInfo& info,
    const std::string& mdn, const std::string& hSid, const std::string& hNid,
    const std::string& min, const std::string& prl) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getCdmaSubscriptionResponse(info, mdn, hSid, hNid, min, prl);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::getCdmaSubscriptionSourceResponse(
    const aidlCommon::RadioResponseInfo& info, aidl::CdmaSubscriptionSource source) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getCdmaSubscriptionSourceResponse(info, source);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::getFacilityLockForAppResponse(
    const aidlCommon::RadioResponseInfo& info, int response) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getFacilityLockForAppResponse(info, response);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::getIccCardStatusResponse(
    const aidlCommon::RadioResponseInfo& info,
    const aidl::CardStatus& cardStatus) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getIccCardStatusResponse(info, cardStatus);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::getIMSIForAppResponse(
    const aidlCommon::RadioResponseInfo& info, const std::string& imsi) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getImsiForAppResponse(info, imsi);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::getSimPhonebookCapacityResponse(
    const aidlCommon::RadioResponseInfo& info,
    const aidl::PhonebookCapacity& capacity) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getSimPhonebookCapacityResponse(info, capacity);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::getSimPhonebookRecordsResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getSimPhonebookRecordsResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::iccCloseLogicalChannelResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->iccCloseLogicalChannelResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::iccIoForAppResponse(
    const aidlCommon::RadioResponseInfo& info,
    const aidl::IccIoResult& iccIo) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->iccIoForAppResponse(info, iccIo);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::iccOpenLogicalChannelResponse(
    const aidlCommon::RadioResponseInfo& info,
    int channelId, const std::vector<uint8_t>& selectResponse) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->iccOpenLogicalChannelResponse(info, channelId, selectResponse);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::iccTransmitApduBasicChannelResponse(
    const aidlCommon::RadioResponseInfo& info,
    const aidl::IccIoResult& result) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->iccTransmitApduBasicChannelResponse(info, result);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::iccTransmitApduLogicalChannelResponse(
    const aidlCommon::RadioResponseInfo& info,
    const aidl::IccIoResult& result) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->iccTransmitApduLogicalChannelResponse(info, result);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::reportStkServiceIsRunningResponse(
    const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->reportStkServiceIsRunningResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::requestIccSimAuthenticationResponse(
    const aidlCommon::RadioResponseInfo& info,
    const aidl::IccIoResult& result) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->requestIccSimAuthenticationResponse(info, result);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::sendEnvelopeResponse(
    const aidlCommon::RadioResponseInfo& info,
    const std::string& commandResponse) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendEnvelopeResponse(info, commandResponse);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::sendEnvelopeWithStatusResponse(
    const aidlCommon::RadioResponseInfo& info,
    const aidl::IccIoResult& iccIo) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendEnvelopeWithStatusResponse(info, iccIo);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::sendTerminalResponseToSimResponse(
    const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendTerminalResponseToSimResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::setAllowedCarriersResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setAllowedCarriersResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::setCarrierInfoForImsiEncryptionResponse(
    const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setCarrierInfoForImsiEncryptionResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::setCdmaSubscriptionSourceResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setCdmaSubscriptionSourceResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::setFacilityLockForAppResponse(
    const aidlCommon::RadioResponseInfo& info, int32_t retry) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setFacilityLockForAppResponse(info, retry);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::setSimCardPowerResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setSimCardPowerResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::setUiccSubscriptionResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setUiccSubscriptionResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::supplyIccPin2ForAppResponse(
    const aidlCommon::RadioResponseInfo& info, int32_t remainingRetries) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->supplyIccPin2ForAppResponse(info, remainingRetries);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::supplyIccPinForAppResponse(
    const aidlCommon::RadioResponseInfo& info, int32_t remainingRetries) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->supplyIccPinForAppResponse(info, remainingRetries);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::supplyIccPuk2ForAppResponse(
    const aidlCommon::RadioResponseInfo& info, int32_t remainingRetries) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->supplyIccPuk2ForAppResponse(info, remainingRetries);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::supplyIccPukForAppResponse(
    const aidlCommon::RadioResponseInfo& info, int32_t remainingRetries) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->supplyIccPukForAppResponse(info, remainingRetries);
            checkReturnStatus(astatus);
        }
    });
}

void RadioSimResponse::supplySimDepersonalizationResponse(
    const aidlCommon::RadioResponseInfo& info,
    aidl::PersoSubstate persoType, int32_t remainingRetries) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->supplySimDepersonalizationResponse(info, persoType, remainingRetries);
            checkReturnStatus(astatus);
        }
    });
}
void RadioSimResponse::updateSimPhonebookRecordsResponse(
    const aidlCommon::RadioResponseInfo& info, int32_t updatedRecordIndex) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->updateSimPhonebookRecordsResponse(info, updatedRecordIndex);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl
