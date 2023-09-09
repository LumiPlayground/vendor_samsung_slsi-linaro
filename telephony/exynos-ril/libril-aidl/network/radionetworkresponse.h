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

#include "radionetworkcallback.h"
#include "radioreturnstatus.h"
#include "radioresponseutil.h"
#include "binderdeathrecipient.h"

namespace android::hardware::radio::impl {

struct RadioNetworkResponse : public RadioNetworkResponseCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkResponse> mResponse;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
    }
    void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkResponse>&
                    radioNetworkResponse);
    void acknowledgeRequest(int32_t serial) override;
    void getAllowedNetworkTypesBitmapResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            int networkTypeBitmap);
    void getAvailableBandModesResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::vector<::aidl::android::hardware::radio::network::RadioBandMode>& bandModes);
    void getAvailableNetworksResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::vector<::aidl::android::hardware::radio::network::OperatorInfo>& networkInfos);
    void getBarringInfoResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::network::CellIdentity& cellIdentity,
            const std::vector<::aidl::android::hardware::radio::network::BarringInfo>& barringInfos);
    void getCdmaRoamingPreferenceResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            ::aidl::android::hardware::radio::network::CdmaRoamingType type);
    void getCellInfoListResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::vector<::aidl::android::hardware::radio::network::CellInfo>& cellInfo);
    void getDataRegistrationStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::network::RegStateResult& dataRegResponse);
    void getImsRegistrationStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            bool isRegistered, ::aidl::android::hardware::radio::RadioTechnologyFamily ratFamily);
    void getNetworkSelectionModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool manual);
    void getOperatorResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::string& longName, const std::string& shortName, const std::string& numeric);
    void getSignalStrengthResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::network::SignalStrength& signalStrength);
    void getSystemSelectionChannelsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const std::vector<::aidl::android::hardware::radio::network::RadioAccessSpecifier>& specifiers);
    void getVoiceRadioTechnologyResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            ::aidl::android::hardware::radio::RadioTechnology rat);
    void getVoiceRegistrationStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            const ::aidl::android::hardware::radio::network::RegStateResult& voiceRegResponse);
    void isNrDualConnectivityEnabledResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info, bool isEnabled);
    void setAllowedNetworkTypesBitmapResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setBandModeResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setBarringPasswordResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setCdmaRoamingPreferenceResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setCellInfoListRateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setIndicationFilterResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setLinkCapacityReportingCriteriaResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setLocationUpdatesResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setNetworkSelectionModeAutomaticResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setNetworkSelectionModeManualResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setNrDualConnectivityStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setSignalStrengthReportingCriteriaResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setSuppServiceNotificationsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void setSystemSelectionChannelsResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void startNetworkScanResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void stopNetworkScanResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void supplyNetworkDepersonalizationResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info, int remainingRetries);
    void setUsageSettingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info);
    void getUsageSettingResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& info,
            ::aidl::android::hardware::radio::network::UsageSetting usageSetting);
};

}
