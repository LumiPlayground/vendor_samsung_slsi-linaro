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
#include "radiomodemresponse.h"
#define TAG "RadioModemResponse"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::modem;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioModemResponse::setResponseFunction(
        const std::shared_ptr<aidl::IRadioModemResponse>& radioModemResponse) {
    execute([&]() {
        mResponse = radioModemResponse;
        if (mResponse) {
            linkToDeath(mResponse->asBinder().get());
            mConnectionManager.notifyServiceConnected();
        } else {
            mConnectionManager.notifyServiceDisconnected();
        }
    });
}

void RadioModemResponse::acknowledgeRequest(int32_t serial) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acknowledgeRequest(serial);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::enableModemResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->enableModemResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::getBasebandVersionResponse(const aidlCommon::RadioResponseInfo& info,
                                            const std::string& version) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getBasebandVersionResponse(info, version);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::getDeviceIdentityResponse(const aidlCommon::RadioResponseInfo& info,
                                            const std::string& imei, const std::string& imeisv,
                                            const std::string& esn, const std::string& meid) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getDeviceIdentityResponse(info, imei, imeisv, esn, meid);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::getHardwareConfigResponse(const aidlCommon::RadioResponseInfo& info,
    const std::vector<aidl::HardwareConfig>& config) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getHardwareConfigResponse(info, config);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::getModemActivityInfoResponse(const aidlCommon::RadioResponseInfo& info,
                                            const aidl::ActivityStatsInfo& activityInfo) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getModemActivityInfoResponse(info, activityInfo);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::getModemStackStatusResponse(const aidlCommon::RadioResponseInfo& info,
    bool isEnabled) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getModemStackStatusResponse(info, isEnabled);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::getRadioCapabilityResponse(const aidlCommon::RadioResponseInfo& info,
                                            const aidl::RadioCapability& rc) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getRadioCapabilityResponse(info, rc);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::nvReadItemResponse(const aidlCommon::RadioResponseInfo& info,
                                    const std::string& result) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->nvReadItemResponse(info, result);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::nvResetConfigResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->nvResetConfigResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::nvWriteCdmaPrlResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->nvWriteCdmaPrlResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::nvWriteItemResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->nvWriteItemResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::requestShutdownResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->requestShutdownResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::sendDeviceStateResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->sendDeviceStateResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::setRadioCapabilityResponse(const aidlCommon::RadioResponseInfo& info,
                                            const aidl::RadioCapability& rc) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setRadioCapabilityResponse(info, rc);
            checkReturnStatus(astatus);
        }
    });
}

void RadioModemResponse::setRadioPowerResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setRadioPowerResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

}