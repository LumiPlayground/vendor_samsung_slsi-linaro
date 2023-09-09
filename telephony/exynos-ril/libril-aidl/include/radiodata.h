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
#include <radiocallbackmanager.h>
#include <radiodatacallback.h>

namespace android::hardware::radio::impl {
struct RadioData : public aidl::android::hardware::radio::data::BnRadioData {
    std::shared_ptr<RadioDataCallback> mRadio;
    uint32_t mSlot;
    RadioCallbackManager<RadioDataCallback, RadioDataResponseCallback,
                RadioDataIndicationCallback> mCallbacks;

    RadioData(std::shared_ptr<RadioDataCallback> radio, uint32_t slot);

    ::ndk::ScopedAStatus allocatePduSessionId(int32_t serial) override;
    ::ndk::ScopedAStatus cancelHandover(int32_t serial, int32_t callId) override;
    ::ndk::ScopedAStatus deactivateDataCall(
            int32_t serial, int32_t cid,
            ::aidl::android::hardware::radio::data::DataRequestReason reason) override;
    ::ndk::ScopedAStatus getDataCallList(int32_t serial) override;
    ::ndk::ScopedAStatus getSlicingConfig(int32_t serial) override;
    ::ndk::ScopedAStatus releasePduSessionId(int32_t serial, int32_t id) override;
    ::ndk::ScopedAStatus responseAcknowledgement() override;
    ::ndk::ScopedAStatus setDataAllowed(int32_t serial, bool allow) override;
    ::ndk::ScopedAStatus setDataProfile(
            int32_t serial,
            const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo>& profiles)
            override;
    ::ndk::ScopedAStatus setDataThrottling(
            int32_t serial,
            ::aidl::android::hardware::radio::data::DataThrottlingAction dataThrottlingAction,
            int64_t completionDurationMillis) override;
    ::ndk::ScopedAStatus setInitialAttachApn(
            int32_t serial,
            const std::optional<::aidl::android::hardware::radio::data::DataProfileInfo>& dpInfo) override;
    ::ndk::ScopedAStatus setResponseFunctions(
            const std::shared_ptr<::aidl::android::hardware::radio::data::IRadioDataResponse>&
                    radioDataResponse,
            const std::shared_ptr<::aidl::android::hardware::radio::data::IRadioDataIndication>&
                    radioDataIndication) override;
    ::ndk::ScopedAStatus setupDataCall(
            int32_t serial, ::aidl::android::hardware::radio::AccessNetwork accessNetwork,
            const ::aidl::android::hardware::radio::data::DataProfileInfo& dataProfileInfo,
            bool roamingAllowed, ::aidl::android::hardware::radio::data::DataRequestReason reason,
            const std::vector<::aidl::android::hardware::radio::data::LinkAddress>& addresses,
            const std::vector<std::string>& dnses, int32_t pduSessionId,
            const std::optional<::aidl::android::hardware::radio::data::SliceInfo>& sliceInfo,
            bool matchAllRuleAllowed) override;
    ::ndk::ScopedAStatus startHandover(int32_t serial, int32_t callId) override;
    ::ndk::ScopedAStatus startKeepalive(
            int32_t serial,
            const ::aidl::android::hardware::radio::data::KeepaliveRequest& keepalive) override;
    ::ndk::ScopedAStatus stopKeepalive(int32_t serial, int32_t sessionHandle) override;

    static std::shared_ptr<RadioData> make(std::shared_ptr<RadioDataCallback> radio, uint32_t slot);
};

}
