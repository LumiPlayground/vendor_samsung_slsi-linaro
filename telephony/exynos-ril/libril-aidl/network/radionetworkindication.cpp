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

#include "radionetworkindication.h"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::network;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioNetworkIndication::setResponseFunction(
    const std::shared_ptr<aidl::IRadioNetworkIndication>& radioNetworkIndication) {
    execute([&]() {
        mIndication = radioNetworkIndication;
        if (mIndication) {
            linkToDeath(mIndication->asBinder().get());
        }
    });
}

void RadioNetworkIndication::barringInfoChanged(::aidl::android::hardware::radio::RadioIndicationType type,
                                                const ::aidl::android::hardware::radio::network::CellIdentity& cellIdentity,
                                                const std::vector<::aidl::android::hardware::radio::network::BarringInfo>& barringInfos) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->barringInfoChanged(type, cellIdentity, barringInfos);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::cdmaPrlChanged(::aidl::android::hardware::radio::RadioIndicationType type, int version) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->cdmaPrlChanged(type, version);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::cellInfoList(::aidl::android::hardware::radio::RadioIndicationType type,
                                          const std::vector<::aidl::android::hardware::radio::network::CellInfo>& records) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->cellInfoList(type, records);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::currentLinkCapacityEstimate(::aidl::android::hardware::radio::RadioIndicationType type,
                                                         const ::aidl::android::hardware::radio::network::LinkCapacityEstimate& lce) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->currentLinkCapacityEstimate(type, lce);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::currentPhysicalChannelConfigs(::aidl::android::hardware::radio::RadioIndicationType type,
                                                           const std::vector<::aidl::android::hardware::radio::network::PhysicalChannelConfig>& configs) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->currentPhysicalChannelConfigs(type, configs);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::currentSignalStrength(::aidl::android::hardware::radio::RadioIndicationType type,
                                                   const ::aidl::android::hardware::radio::network::SignalStrength& signalStrength) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->currentSignalStrength(type, signalStrength);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::imsNetworkStateChanged(::aidl::android::hardware::radio::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->imsNetworkStateChanged(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::networkScanResult(::aidl::android::hardware::radio::RadioIndicationType type,
                                               const ::aidl::android::hardware::radio::network::NetworkScanResult& result) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->networkScanResult(type, result);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::networkStateChanged(::aidl::android::hardware::radio::RadioIndicationType type) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->networkStateChanged(type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::nitzTimeReceived(::aidl::android::hardware::radio::RadioIndicationType type,
                                              const std::string& nitzTime, long receivedTimeMs, long ageMs) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->nitzTimeReceived(type, nitzTime, receivedTimeMs, ageMs);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::registrationFailed(::aidl::android::hardware::radio::RadioIndicationType type,
                                                const ::aidl::android::hardware::radio::network::CellIdentity& cellIdentity,
                                                const std::string& chosenPlmn, int domain,
                                                int causeCode, int additionalCauseCode) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->registrationFailed(type, cellIdentity, chosenPlmn, domain, causeCode, additionalCauseCode);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::restrictedStateChanged(::aidl::android::hardware::radio::RadioIndicationType type,
                                                    ::aidl::android::hardware::radio::network::PhoneRestrictedState state) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->restrictedStateChanged(type, state);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::suppSvcNotify(::aidl::android::hardware::radio::RadioIndicationType type,
                                           const ::aidl::android::hardware::radio::network::SuppSvcNotification& suppSvc) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->suppSvcNotify(type, suppSvc);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkIndication::voiceRadioTechChanged(::aidl::android::hardware::radio::RadioIndicationType type,
                                                   ::aidl::android::hardware::radio::RadioTechnology rat) {
    execute([&]() {
        if (mIndication) {
            auto astatus = mIndication->voiceRadioTechChanged(type, rat);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl
