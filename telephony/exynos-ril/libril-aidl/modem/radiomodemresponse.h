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
#pragma once

#include "radiomodemcallback.h"
#include "radioreturnstatus.h"
#include "radioresponseutil.h"
#include "binderdeathrecipient.h"
#include "serviceconnectionmanager.h"

namespace android::hardware::radio::impl {

struct RadioModemResponse : public RadioModemResponseCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemResponse> mResponse;
    ServiceConnectionManager mConnectionManager;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
        mConnectionManager.notifyRemoteDied();
    }
    void setServiceConnection(ServiceConnection *connection) {
        mConnectionManager.setServiceConnection(connection);
    }
    void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemResponse>&
                    radioModemResponse) override;
    void acknowledgeRequest(int32_t serial) override;
    void enableModemResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void getBasebandVersionResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        const std::string& version) override;
    void getDeviceIdentityResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        const std::string& imei, const std::string& imeisv,
        const std::string& esn, const std::string& meid) override;
    void getHardwareConfigResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        const std::vector<::aidl::android::hardware::radio::modem::HardwareConfig>& config) override;
    void getModemActivityInfoResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        const ::aidl::android::hardware::radio::modem::ActivityStatsInfo& activityInfo) override;
    void getModemStackStatusResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        bool isEnabled) override;
    void getRadioCapabilityResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        const ::aidl::android::hardware::radio::modem::RadioCapability& rc) override;
    void nvReadItemResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
                                    const std::string& result) override;
    void nvResetConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void nvWriteCdmaPrlResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void nvWriteItemResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void requestShutdownResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void sendDeviceStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
    void setRadioCapabilityResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
                                            const ::aidl::android::hardware::radio::modem::RadioCapability& rc) override;
    void setRadioPowerResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info) override;
};

}
