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
#include "radiodataresponse.h"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::data;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioDataResponse::setResponseFunction(
    const std::shared_ptr<aidl::IRadioDataResponse>& radioDataResponse) {
    execute([&]() {
        mResponse = radioDataResponse;
        if (mResponse) {
            linkToDeath(mResponse->asBinder().get());
        }
    });
}

void RadioDataResponse::acknowledgeRequest(int32_t serial) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acknowledgeRequest(serial);
            checkReturnStatus(astatus);
        }
    });
}

void RadioDataResponse::allocatePduSessionIdResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info, int id) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->allocatePduSessionIdResponse(info, id);
            checkReturnStatus(astatus);
        }
    });
}

void RadioDataResponse::cancelHandoverResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->cancelHandoverResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioDataResponse::setInitialAttachApnResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setInitialAttachApnResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioDataResponse::deactivateDataCallResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->deactivateDataCallResponse(info);
            checkReturnStatus(astatus);
        }
    });
}
void RadioDataResponse::getDataCallListResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info,
    const std::vector<
        ::aidl::android::hardware::radio::data::SetupDataCallResult>
        dcResponse) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getDataCallListResponse(info, dcResponse);
            checkReturnStatus(astatus);
        }
    });
}

void RadioDataResponse::getSlicingConfigResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info,
    const ::aidl::android::hardware::radio::data::SlicingConfig slicingConfig) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getSlicingConfigResponse(info, slicingConfig);
            checkReturnStatus(astatus);
        }
    });
}

void RadioDataResponse::releasePduSessionIdResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->releasePduSessionIdResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioDataResponse::setDataAllowedResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setDataAllowedResponse(info);
            checkReturnStatus(astatus);
        }
    });
}
void RadioDataResponse::setDataThrottlingResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setDataThrottlingResponse(info);
            checkReturnStatus(astatus);
        }
    });
}
void RadioDataResponse::setDataProfileResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setDataProfileResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioDataResponse::setupDataCallResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info,
    const ::aidl::android::hardware::radio::data::SetupDataCallResult
        dcResponse) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setupDataCallResponse(info, dcResponse);
            checkReturnStatus(astatus);
        }
    });
}
void RadioDataResponse::startHandoverResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->startHandoverResponse(info);
            checkReturnStatus(astatus);
        }
    });
}
void RadioDataResponse::startKeepaliveResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info,
    const ::aidl::android::hardware::radio::data::KeepaliveStatus keepAliveStatus) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->startKeepaliveResponse(info, keepAliveStatus);
            checkReturnStatus(astatus);
        }
    });
}
void RadioDataResponse::stopKeepaliveResponse(
    const ::aidl::android::hardware::radio::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->stopKeepaliveResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

} // namespace android::hardware::radio::impl

