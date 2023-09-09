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
#include <radionetworkcallback.h>
#include <radioserviceconnection.h>
#include <slsi/radio_v2_0.h>

namespace android::hardware::radio::impl::network {

struct BaseNetworkCommand : public BaseCommand {
    using CallbackType = android::hardware::radio::impl::RadioNetworkResponseCallback;
    std::shared_ptr<CallbackType>& mCb;

    BaseNetworkCommand(std::shared_ptr<CallbackType>& cb) : BaseCommand(), mCb(cb) {}

    BaseNetworkCommand(uint32_t slotId, std::shared_ptr<CallbackType>& cb)
            : BaseCommand(slotId), mCb(cb) {}

    BaseNetworkCommand(const VendorFunctions& vendorFunctions, std::shared_ptr<CallbackType>& cb)
            : BaseCommand(vendorFunctions), mCb(cb) {}
};

struct BaseNetworkIndication : public BaseIndication {
    using CallbackType = android::hardware::radio::impl::RadioNetworkIndicationCallback;
    std::shared_ptr<CallbackType>& mCb;

    BaseNetworkIndication(std::shared_ptr<CallbackType>& cb) : BaseNetworkIndication(0, cb) {}
    BaseNetworkIndication(uint32_t slotId, std::shared_ptr<CallbackType>& cb)
            : BaseIndication(slotId), mCb(cb) {}
};

/**
 * NitzTimeReceivedInd
 */
struct NitzTimeReceivedInd : public BaseNetworkIndication, public RadioServiceConnectionListener {
    using BaseNetworkIndication::BaseNetworkIndication;

    const char *TAG = "NitzTimeReceivedInd";
    std::shared_ptr<V2_0::RIL_NitzTime> mLastNitzTimeData;
    int onIndication(int slotId,int indicationType, int token, RIL_Errno e,
            void *response, size_t responselen);
    void onRadioServiceConnected();
    void onRadioServiceDionnected();
    void saveNitzTimeData(const V2_0::RIL_NitzTime *nitzTime);
    bool sendNitzTimeData(const V2_0::RIL_NitzTime *nitzTime, bool save);
};

/**
 * SetSignalStrengthReportingCriteriaCommand
 */
struct SetSignalStrengthReportingCriteriaCommand : public BaseNetworkCommand {
    using BaseNetworkCommand::BaseNetworkCommand;

    const char *TAG = "SetSignalStrengthReportingCriteriaCommand";
    void onRequest(int serial, const std::vector<::aidl::android::hardware::radio::network::SignalThresholdInfo>& signalThresholdInfos);
    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * SetUsageSettingCommand
 */
struct SetUsageSettingCommand : public BaseNetworkCommand {
    using BaseNetworkCommand::BaseNetworkCommand;

    const char *TAG = "SetUsageSettingCommand";
    void onRequest(int serial, ::aidl::android::hardware::radio::network::UsageSetting usageSetting);
    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

/**
 * GetUsageSettingCommand
 */
struct GetUsageSettingCommand : public BaseNetworkCommand {
    using BaseNetworkCommand::BaseNetworkCommand;

    const char *TAG = "GetUsageSettingCommand";
    void onRequest(int serial);
    int onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};

}