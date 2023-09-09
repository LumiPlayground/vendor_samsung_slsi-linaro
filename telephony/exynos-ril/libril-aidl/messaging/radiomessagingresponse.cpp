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
#include "radiomessagingresponse.h"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::messaging;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioMessagingResponse::setResponseFunction(const std::shared_ptr<aidl::IRadioMessagingResponse>& radioMessagingResponse) {
    execute([&]() {
        mResponse = radioMessagingResponse;
        if (mResponse) {
            linkToDeath(mResponse->asBinder().get());
        }
    });
}

void RadioMessagingResponse::acknowledgeIncomingGsmSmsWithPduResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acknowledgeIncomingGsmSmsWithPduResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::acknowledgeLastIncomingCdmaSmsResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acknowledgeLastIncomingCdmaSmsResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::acknowledgeLastIncomingGsmSmsResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acknowledgeLastIncomingGsmSmsResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::acknowledgeRequest(int32_t serial) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acknowledgeRequest(serial);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::deleteSmsOnRuimResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->deleteSmsOnRuimResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::deleteSmsOnSimResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->deleteSmsOnSimResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::getCdmaBroadcastConfigResponse(const aidlCommon::RadioResponseInfo& info, const std::vector<aidl::CdmaBroadcastSmsConfigInfo>& configs) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getCdmaBroadcastConfigResponse(info, configs);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::getGsmBroadcastConfigResponse(const aidlCommon::RadioResponseInfo& info, const std::vector<aidl::GsmBroadcastSmsConfigInfo>& configs) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getGsmBroadcastConfigResponse(info, configs);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::getSmscAddressResponse(const aidlCommon::RadioResponseInfo& info, const std::string& smsc) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getSmscAddressResponse(info, smsc);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::reportSmsMemoryStatusResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->reportSmsMemoryStatusResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::sendCdmaSmsExpectMoreResponse(const aidlCommon::RadioResponseInfo& info, const aidl::SendSmsResult& sms) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendCdmaSmsExpectMoreResponse(info, sms);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::sendCdmaSmsResponse(const aidlCommon::RadioResponseInfo& info, const aidl::SendSmsResult& sms) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendCdmaSmsResponse(info, sms);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::sendImsSmsResponse(const aidlCommon::RadioResponseInfo& info, const aidl::SendSmsResult& sms) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendImsSmsResponse(info, sms);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::sendSmsExpectMoreResponse(const aidlCommon::RadioResponseInfo& info, const aidl::SendSmsResult& sms) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendSmsExpectMoreResponse(info, sms);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::sendSmsResponse(const aidlCommon::RadioResponseInfo& info, const aidl::SendSmsResult& sms) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendSmsResponse(info, sms);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::setCdmaBroadcastActivationResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setCdmaBroadcastActivationResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::setCdmaBroadcastConfigResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setCdmaBroadcastConfigResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::setGsmBroadcastActivationResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setGsmBroadcastActivationResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::setGsmBroadcastConfigResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setGsmBroadcastConfigResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::setSmscAddressResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setSmscAddressResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::writeSmsToRuimResponse(const aidlCommon::RadioResponseInfo& info, int32_t index) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->writeSmsToRuimResponse(info, index);
            checkReturnStatus(astatus);
        }
    });
}

void RadioMessagingResponse::writeSmsToSimResponse(const aidlCommon::RadioResponseInfo& info, int32_t index) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->writeSmsToSimResponse(info, index);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl
