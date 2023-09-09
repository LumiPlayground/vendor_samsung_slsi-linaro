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

#include <basecommand.h>
#include <radiodatacallback.h>
#include <slsi/radio_v2_0.h>


namespace android::hardware::radio::impl::data {

namespace aidl_radio = ::aidl::android::hardware::radio;
namespace aidl_data = ::aidl::android::hardware::radio::data;

/* Legacy compatible code
 * This is cloned from HIDL-RIL conversion
 * In the futuer, when HIDL deprecation, these codes will remain
 */

void freeSetupDataCallInfo_V2_0(V2_0::RIL_SetupDataCallInfo &setupDataCallInfo,
                                const std::vector<aidl_data::LinkAddress> &addresses);
bool fillSetupDataCallInfo_V2_0(V2_0::RIL_SetupDataCallInfo &setupDataCallInfo, aidl_radio::AccessNetwork accessNetwork,
                                const aidl_data::DataProfileInfo& dataProfileInfo, bool roamingAllowed, aidl_data::DataRequestReason reason,
                                const std::vector<aidl_data::LinkAddress>& addresses, const std::vector<std::string>& dnses, int32_t pduSessionId,
                                const std::optional<aidl_data::SliceInfo>& sliceInfo, const std::optional<aidl_data::TrafficDescriptor> trafficDescriptor, bool matchAllRuleAllowed,
                                bool AlwaysOnPdu, android::RequestInfo *pRI);

// For AIDL
bool copyAidlStringToRil(char **dest, const std::string &src, android::RequestInfo *pRI, bool allowEmpty);
bool convertHalTDToRil(RIL_TrafficDescriptor_aosp & outTdInfo, RIL_OptionalInfoFlag & outTdFlag,
                        const std::optional<aidl_data::TrafficDescriptor> trafficDescriptor);
bool convertHalTDToRil(std::optional<DataV2_0::RIL_TrafficDescriptor>& outTdInfo,
                        const std::optional<aidl_data::TrafficDescriptor> &trafficDescriptor);
bool convertHalLinkAddressToRil(RIL_LinkAddress& out, const aidl_data::LinkAddress& linkAddress, RequestInfo *pRI);
bool convertHalDataProfileToRilV2_0(V2_0::RIL_DataProfileInfo& out, const aidl_data::DataProfileInfo& dataProfileInfo);

struct BaseDataCommand : public BaseCommand {
    using CallbackType =
        android::hardware::radio::impl::RadioDataResponseCallback;
    std::shared_ptr<CallbackType> &mCb;

    BaseDataCommand(std::shared_ptr<CallbackType> &cb)
        : BaseCommand(), mCb(cb) {}

    BaseDataCommand(uint32_t slotId, std::shared_ptr<CallbackType> &cb)
        : BaseCommand(slotId), mCb(cb) {}

    BaseDataCommand(const VendorFunctions &vendorFunctions,
                    std::shared_ptr<CallbackType> &cb)
        : BaseCommand(vendorFunctions), mCb(cb) {}
};

struct BaseDataIndication : public BaseIndication {
    using CallbackType =
        android::hardware::radio::impl::RadioDataIndicationCallback;
    std::shared_ptr<CallbackType> &mCb;

    BaseDataIndication(std::shared_ptr<CallbackType> &cb)
        : BaseDataIndication(0, cb) {}
    BaseDataIndication(uint32_t slotId, std::shared_ptr<CallbackType> &cb)
        : BaseIndication(slotId), mCb(cb) {}
};

/**
 * SetupDataCallCommand
 */
struct SetupDataCallCommand : public BaseDataCommand {
    using BaseDataCommand::BaseDataCommand;

    const char *TAG = "SetupDataCallCommand";
    void onRequest( int32_t serial, aidl_radio::AccessNetwork accessNetwork, const aidl_data::DataProfileInfo &dataProfileInfo, bool roamingAllowed,
                    aidl_data::DataRequestReason reason, const std::vector<aidl_data::LinkAddress> &addresses, const std::vector<std::string> &dnses,
                    int32_t pduSessionId, const std::optional<aidl_data::SliceInfo> &sliceInfo, bool matchAllRuleAllowed); // TODO

    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * SetDataProfileCommand
 */
struct SetDataProfileCommand : public BaseDataCommand {
    using BaseDataCommand::BaseDataCommand;

    const char *TAG = "SetDataProfileCommand";
    void onRequest(int32_t serial, const std::vector<::aidl::android::hardware::radio::data::DataProfileInfo> &profiles);

    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * SetInitialAttachApnCommand
 */
struct SetInitialAttachApnCommand : public BaseDataCommand {
    using BaseDataCommand::BaseDataCommand;

    const char *TAG = "setInitialAttachApn";
    void onRequest(int32_t serial, const std::optional<::aidl::android::hardware::radio::data::DataProfileInfo> &dpInfo);

    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * UnthrottleApnInd
 */
struct UnthrottleApnInd : public BaseDataIndication {
    using BaseDataIndication::BaseDataIndication;

    const char *TAG = "UnthrottleApnInd";
    int onIndication(int slotId, int indicationType, int token, RIL_Errno e,
                     void *response, size_t responselen);
};
}  // namespace android::hardware::radio::impl::data
