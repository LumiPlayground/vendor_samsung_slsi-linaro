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

#include <aidl/android/hardware/radio/data/BnRadioData.h>

namespace android::hardware::radio::impl {

struct RadioDataResponseCallback;
struct RadioDataIndicationCallback;

struct RadioDataCallback {
    virtual ~RadioDataCallback() = default;
    virtual void setResponseFunctions(
        const std::shared_ptr<RadioDataResponseCallback>& responseCb,
        const std::shared_ptr<RadioDataIndicationCallback>& indicationCb) = 0;
    virtual void allocatePduSessionId(int32_t serial) = 0;
    virtual void cancelHandover(int32_t serial, int32_t callId) = 0;
    virtual void deactivateDataCall(
            int32_t serial, int32_t cid,
            ::aidl::android::hardware::radio::data::DataRequestReason reason) = 0;
    virtual void getDataCallList(int32_t serial) = 0;
    virtual void getSlicingConfig(int32_t serial) = 0;
    virtual void releasePduSessionId(int32_t serial, int32_t id) = 0;
    virtual void responseAcknowledgement() = 0;
    virtual void setDataAllowed(int32_t serial, bool allow) = 0;
    virtual void setDataProfile(int32_t serial,
            const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo>& profiles)
            = 0;
    virtual void setDataThrottling(int32_t serial,
            ::aidl::android::hardware::radio::data::DataThrottlingAction dataThrottlingAction,
            int64_t completionDurationMillis) = 0;
    virtual void setInitialAttachApn(
            int32_t serial,
            const std::optional<::aidl::android::hardware::radio::data::DataProfileInfo>& dpInfo) = 0;
    virtual void setupDataCall(
            int32_t serial, ::aidl::android::hardware::radio::AccessNetwork accessNetwork,
            const ::aidl::android::hardware::radio::data::DataProfileInfo& dataProfileInfo,
            bool roamingAllowed, ::aidl::android::hardware::radio::data::DataRequestReason reason,
            const std::vector<::aidl::android::hardware::radio::data::LinkAddress>& addresses,
            const std::vector<std::string>& dnses, int32_t pduSessionId,
            const std::optional<::aidl::android::hardware::radio::data::SliceInfo>& sliceInfo,
            bool matchAllRuleAllowed) = 0;
    virtual void startHandover(int32_t serial, int32_t callId) = 0;
    virtual void startKeepalive(int32_t serial,
            const ::aidl::android::hardware::radio::data::KeepaliveRequest& keepalive) = 0;
    virtual void stopKeepalive(int32_t serial, int32_t sessionHandle) = 0;
};

struct RadioDataResponseCallback {
    virtual ~RadioDataResponseCallback()=default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::data::IRadioDataResponse>&
                    radioDataResponse)=0;
    virtual void acknowledgeRequest(int32_t in_serial)=0;
    virtual void allocatePduSessionIdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,  int id)=0;
    virtual void cancelHandoverResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)=0;
    virtual void deactivateDataCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)=0;
    virtual void getDataCallListResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        const std::vector<::aidl::android::hardware::radio::data::SetupDataCallResult> dcResponse)=0;
    virtual void getSlicingConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        const ::aidl::android::hardware::radio::data::SlicingConfig slicingConfig)=0;
    virtual void releasePduSessionIdResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)=0;
    virtual void setDataAllowedResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)=0;
    virtual void setDataThrottlingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)=0;
    virtual void setInitialAttachApnResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)=0;
    virtual void setDataProfileResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)=0;
    virtual void setupDataCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        const ::aidl::android::hardware::radio::data::SetupDataCallResult dcResponse)=0;
    virtual void startHandoverResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)=0;
    virtual void startKeepaliveResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
        const::aidl::android::hardware::radio::data::KeepaliveStatus status)=0;
    virtual void stopKeepaliveResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info)=0;
};

struct RadioDataIndicationCallback {
    virtual ~RadioDataIndicationCallback()=default;
    virtual void setResponseFunction(
        const std::shared_ptr<::aidl::android::hardware::radio::data::IRadioDataIndication>&
                radioDataIndication)=0;
    virtual void dataCallListChanged(const ::aidl::android::hardware::radio::RadioIndicationType type,
        const std::vector<::aidl::android::hardware::radio::data::SetupDataCallResult> dcList)=0;
    virtual void keepaliveStatus(const ::aidl::android::hardware::radio::RadioIndicationType type,
        const::aidl::android::hardware::radio::data::KeepaliveStatus status)=0;
    virtual void pcoData(const ::aidl::android::hardware::radio::RadioIndicationType type,
        const::aidl::android::hardware::radio::data::PcoDataInfo pco)=0;
    virtual void unthrottleApn(const ::aidl::android::hardware::radio::RadioIndicationType type, ::aidl::android::hardware::radio::data::DataProfileInfo dp)=0;
};

}  // namespace android::hardware::radio::impl {
