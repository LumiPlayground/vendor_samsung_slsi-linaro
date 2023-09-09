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
#include "radiovoiceresponse.h"
#define TAG "RadioVoiceResponse"

namespace android::hardware::radio::impl {

void RadioVoiceResponse::setResponseFunction(
    const std::shared_ptr<::aidl::android::hardware::radio::voice::IRadioVoiceResponse>&
        radioVoiceResponse) {
    execute([&]() {
        mResponse = radioVoiceResponse;
        if (mResponse) {
            linkToDeath(mResponse->asBinder().get());
        }
    });
}

void RadioVoiceResponse::acceptCallResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acceptCallResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::acknowledgeRequest(int32_t serial) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acknowledgeRequest(serial);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::cancelPendingUssdResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->cancelPendingUssdResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::conferenceResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->conferenceResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::dialResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->dialResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::emergencyDialResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->emergencyDialResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::exitEmergencyCallbackModeResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->exitEmergencyCallbackModeResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::explicitCallTransferResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->explicitCallTransferResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::getCallForwardStatusResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info,
    const std::vector<::aidl::android::hardware::radio::voice::CallForwardInfo>& callForwardInfos) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getCallForwardStatusResponse(info, callForwardInfos);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::getCallWaitingResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enable, int32_t serviceClass) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getCallWaitingResponse(info, enable, serviceClass);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::getClipResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info,
    ::aidl::android::hardware::radio::voice::ClipStatus status) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getClipResponse(info, status);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::getClirResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info, int32_t n, int32_t m) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getClirResponse(info, n, m);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::getCurrentCallsResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info,
    const std::vector<::aidl::android::hardware::radio::voice::Call>& calls) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getCurrentCallsResponse(info, calls);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::getLastCallFailCauseResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info,
    const ::aidl::android::hardware::radio::voice::LastCallFailCauseInfo& failCauseinfo) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getLastCallFailCauseResponse(info, failCauseinfo);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::getMuteResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enable) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getMuteResponse(info, enable);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::getPreferredVoicePrivacyResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enable) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getPreferredVoicePrivacyResponse(info, enable);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::getTtyModeResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info,
    ::aidl::android::hardware::radio::voice::TtyMode mode) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getTtyModeResponse(info, mode);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::handleStkCallSetupRequestFromSimResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->handleStkCallSetupRequestFromSimResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::hangupConnectionResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->hangupConnectionResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::hangupForegroundResumeBackgroundResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->hangupForegroundResumeBackgroundResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::hangupWaitingOrBackgroundResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->hangupWaitingOrBackgroundResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::isVoNrEnabledResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool enable) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->isVoNrEnabledResponse(info, enable);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::rejectCallResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->rejectCallResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::sendBurstDtmfResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendBurstDtmfResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::sendCdmaFeatureCodeResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendCdmaFeatureCodeResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::sendDtmfResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendDtmfResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::sendUssdResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendUssdResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::separateConnectionResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->separateConnectionResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::setCallForwardResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setCallForwardResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::setCallWaitingResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setCallWaitingResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::setClirResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setClirResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::setMuteResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setMuteResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::setPreferredVoicePrivacyResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setPreferredVoicePrivacyResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::setTtyModeResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setTtyModeResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::setVoNrEnabledResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setVoNrEnabledResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::startDtmfResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->startDtmfResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::stopDtmfResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->stopDtmfResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioVoiceResponse::switchWaitingOrHoldingAndActiveResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->switchWaitingOrHoldingAndActiveResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl
