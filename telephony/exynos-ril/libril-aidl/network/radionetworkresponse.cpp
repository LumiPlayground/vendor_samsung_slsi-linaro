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
#include "radionetworkresponse.h"
#define TAG "RadioNetworkResponse"

namespace android::hardware::radio::impl {

namespace aidl = ::aidl::android::hardware::radio::network;
namespace aidlCommon = ::aidl::android::hardware::radio;

void RadioNetworkResponse::setResponseFunction(
    const std::shared_ptr<aidl::IRadioNetworkResponse>& radioNetworkResponse) {
    execute([&]() {
        mResponse = radioNetworkResponse;
        if (mResponse) {
            linkToDeath(mResponse->asBinder().get());
        }
    });
}

void RadioNetworkResponse::acknowledgeRequest(int32_t serial) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->acknowledgeRequest(serial);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getAllowedNetworkTypesBitmapResponse(const aidlCommon::RadioResponseInfo& info,
                                                                int networkTypeBitmap) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getAllowedNetworkTypesBitmapResponse(info, networkTypeBitmap);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getAvailableBandModesResponse(const aidlCommon::RadioResponseInfo& info,
                                                         const std::vector<aidl::RadioBandMode>& bandModes) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getAvailableBandModesResponse(info, bandModes);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getAvailableNetworksResponse(const aidlCommon::RadioResponseInfo& info,
                                                        const std::vector<aidl::OperatorInfo>& networkInfos) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getAvailableNetworksResponse(info, networkInfos);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getBarringInfoResponse(const aidlCommon::RadioResponseInfo& info,
                                                  const aidl::CellIdentity& cellIdentity,
                                                  const std::vector<aidl::BarringInfo>& barringInfos) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getBarringInfoResponse(info, cellIdentity, barringInfos);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getCdmaRoamingPreferenceResponse(const aidlCommon::RadioResponseInfo& info,
                                                            aidl::CdmaRoamingType type) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getCdmaRoamingPreferenceResponse(info, type);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getCellInfoListResponse(const aidlCommon::RadioResponseInfo& info,
                                                   const std::vector<aidl::CellInfo>& cellInfo) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getCellInfoListResponse(info, cellInfo);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getDataRegistrationStateResponse(const aidlCommon::RadioResponseInfo& info,
                                                            const aidl::RegStateResult& dataRegResponse) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getDataRegistrationStateResponse(info, dataRegResponse);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getImsRegistrationStateResponse(const aidlCommon::RadioResponseInfo& info,
                                                           bool isRegistered, aidlCommon::RadioTechnologyFamily ratFamily) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getImsRegistrationStateResponse(info, isRegistered, ratFamily);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getNetworkSelectionModeResponse(const aidlCommon::RadioResponseInfo& info, bool manual) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getNetworkSelectionModeResponse(info, manual);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getOperatorResponse(const aidlCommon::RadioResponseInfo& info,
                                               const std::string& longName, const std::string& shortName, const std::string& numeric) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getOperatorResponse(info, longName, shortName, numeric);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getSignalStrengthResponse(const aidlCommon::RadioResponseInfo& info,
                                                     const aidl::SignalStrength& signalStrength) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getSignalStrengthResponse(info, signalStrength);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getSystemSelectionChannelsResponse(const aidlCommon::RadioResponseInfo& info,
                                                              const std::vector<aidl::RadioAccessSpecifier>& specifiers) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getSystemSelectionChannelsResponse(info, specifiers);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getVoiceRadioTechnologyResponse(const aidlCommon::RadioResponseInfo& info,
                                                           aidlCommon::RadioTechnology rat) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getVoiceRadioTechnologyResponse(info, rat);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getVoiceRegistrationStateResponse(const aidlCommon::RadioResponseInfo& info,
                                                             const aidl::RegStateResult& voiceRegResponse) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getVoiceRegistrationStateResponse(info, voiceRegResponse);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::isNrDualConnectivityEnabledResponse(const aidlCommon::RadioResponseInfo& info, bool isEnabled) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->isNrDualConnectivityEnabledResponse(info, isEnabled);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setAllowedNetworkTypesBitmapResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setAllowedNetworkTypesBitmapResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setBandModeResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setBandModeResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setBarringPasswordResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setBarringPasswordResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setCdmaRoamingPreferenceResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setCdmaRoamingPreferenceResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setCellInfoListRateResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setCellInfoListRateResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setIndicationFilterResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setIndicationFilterResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setLinkCapacityReportingCriteriaResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setLinkCapacityReportingCriteriaResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setLocationUpdatesResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setLocationUpdatesResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setNetworkSelectionModeAutomaticResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setNetworkSelectionModeAutomaticResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setNetworkSelectionModeManualResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setNetworkSelectionModeManualResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setNrDualConnectivityStateResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setNrDualConnectivityStateResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setSignalStrengthReportingCriteriaResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setSignalStrengthReportingCriteriaResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setSuppServiceNotificationsResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setSuppServiceNotificationsResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setSystemSelectionChannelsResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setSystemSelectionChannelsResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::startNetworkScanResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->startNetworkScanResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::stopNetworkScanResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->stopNetworkScanResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::supplyNetworkDepersonalizationResponse(const aidlCommon::RadioResponseInfo& info, int remainingRetries) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->supplyNetworkDepersonalizationResponse(info, remainingRetries);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::setUsageSettingResponse(const aidlCommon::RadioResponseInfo& info) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->setUsageSettingResponse(info);
            checkReturnStatus(astatus);
        }
    });
}

void RadioNetworkResponse::getUsageSettingResponse(const aidlCommon::RadioResponseInfo& info,
                                                   aidl::UsageSetting usageSetting) {
    execute([&]() {
        if (mResponse) {
            auto astatus = mResponse->getUsageSettingResponse(info, usageSetting);
            checkReturnStatus(astatus);
        }
    });
}

}  // namespace android::hardware::radio::impl
