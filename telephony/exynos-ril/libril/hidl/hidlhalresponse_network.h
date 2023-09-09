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

#include "hidlhalresponse.h"

struct HidlHalNetworkResponse : public HidlHalResponse, public CheckReturnStatus {
    using HidlHalResponse::HidlHalResponse;

    HidlHalNetworkResponse(const HidlHalResponse& hidlHal) : HidlHalResponse(hidlHal) {}

    int getAvailableBandModesResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getAvailableNetworksResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getBarringInfoResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getCdmaRoamingPreferenceResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getCellInfoListResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getCellInfoListResponse_1_2(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getCellInfoListResponse_1_4(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getCellInfoListResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getDataRegistrationStateResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getDataRegistrationStateResponse_1_2(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getDataRegistrationStateResponse_1_4(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getDataRegistrationStateResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getImsRegistrationStateResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getNetworkSelectionModeResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getOperatorResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getSignalStrengthResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getSignalStrengthResponse_1_2(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getSignalStrengthResponse_1_4(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getVoiceRadioTechnologyResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getVoiceRegistrationStateResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getVoiceRegistrationStateResponse_1_2(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int getVoiceRegistrationStateResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setBandModeResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setBarringPasswordResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setCdmaRoamingPreferenceResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setCellInfoListRateResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setIndicationFilterResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setIndicationFilterResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setLinkCapacityReportingCriteriaResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setLinkCapacityReportingCriteriaResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setLocationUpdatesResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setNetworkSelectionModeAutomaticResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setNetworkSelectionModeManualResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setNetworkSelectionModeManualResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setSignalStrengthReportingCriteriaResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setSignalStrengthReportingCriteriaResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setSuppServiceNotificationsResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setSystemSelectionChannelsResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int setSystemSelectionChannelsResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int startNetworkScanResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int startNetworkScanResponse_1_4(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int startNetworkScanResponse_1_5(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int stopNetworkScanResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
    int supplyNetworkDepersonalizationResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responselen);
};
