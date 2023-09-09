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

struct RadioNetworkIndication : public RadioNetworkIndicationCallback,
                            public RadioReturnStatus,
                            public RemoteDiedHelper {
    std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkIndication> mIndication;

    SYNCHRONIZED_WRAPPER(so);

    void onRemoteDied() override {
        // no race-condition between onBinderDied and setResponseFunction.
        setResponseFunction(nullptr);
    }
    void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::network::IRadioNetworkIndication>&
                    radioNetworkIndication);
    void barringInfoChanged(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::CellIdentity& cellIdentity,
            const std::vector<::aidl::android::hardware::radio::network::BarringInfo>& barringInfos);
    void cdmaPrlChanged(::aidl::android::hardware::radio::RadioIndicationType type, int version);
    void cellInfoList(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::vector<::aidl::android::hardware::radio::network::CellInfo>& records);
    void currentLinkCapacityEstimate(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::LinkCapacityEstimate& lce);
    void currentPhysicalChannelConfigs(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::vector<::aidl::android::hardware::radio::network::PhysicalChannelConfig>& configs);
    void currentSignalStrength(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::SignalStrength& signalStrength);
    void imsNetworkStateChanged(::aidl::android::hardware::radio::RadioIndicationType type);
    void networkScanResult(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::NetworkScanResult& result);
    void networkStateChanged(::aidl::android::hardware::radio::RadioIndicationType type);
    void nitzTimeReceived(::aidl::android::hardware::radio::RadioIndicationType type,
            const std::string& nitzTime, long receivedTimeMs, long ageMs);
    void registrationFailed(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::CellIdentity& cellIdentity,
            const std::string& chosenPlmn, int domain,
            int causeCode, int additionalCauseCode);
    void restrictedStateChanged(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::network::PhoneRestrictedState state);
    void suppSvcNotify(::aidl::android::hardware::radio::RadioIndicationType type,
            const ::aidl::android::hardware::radio::network::SuppSvcNotification& suppSvc);
    void voiceRadioTechChanged(::aidl::android::hardware::radio::RadioIndicationType type,
            ::aidl::android::hardware::radio::RadioTechnology rat);
};

}  // namespace android::hardware::radio::impl
