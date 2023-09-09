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

struct HidlHalSimResponse : public HidlHalResponse, public CheckReturnStatus {
    using HidlHalResponse::HidlHalResponse;

    HidlHalSimResponse(const HidlHalResponse& hidlHal) : HidlHalResponse(hidlHal) {}

    int areUiccApplicationsEnabledResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen);
    int changeIccPin2ForAppResponse(int slotId,
                                int responseType, int serial, RIL_Errno e, void *response,
                                size_t responseLen);
    int changeIccPinForAppResponse(int slotId,
                             int responseType, int serial, RIL_Errno e, void *response,
                             size_t responseLen);
    int enableUiccApplicationsResponse(int slotId,
                                int responseType, int serial, RIL_Errno e,
                                void *response, size_t responseLen);
    int getAllowedCarriersResponse(int slotId,
                                      int responseType, int serial, RIL_Errno e,
                                      void *response, size_t responseLen);
    int getAllowedCarriersResponse_1_4(int slotId,
                                    int responseType, int serial, RIL_Errno e,
                                    void *response, size_t responseLen);
    int getCDMASubscriptionResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen);
    int getCdmaSubscriptionSourceResponse(int slotId,
                                int responseType, int serial, RIL_Errno e,
                                void *response, size_t responseLen);
    int getFacilityLockForAppResponse(int slotId,
                      int responseType, int serial, RIL_Errno e,
                      void *response, size_t responseLen);
    int getIMSIForAppResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen);
    int getSimPhonebookCapacityResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen);
    int getSimPhonebookRecordsResponse(int slotId,
                            int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen);
    int iccCloseLogicalChannelResponse(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responseLen);
    int iccIoForAppResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responseLen);
    int iccOpenLogicalChannelResponse(int slotId,
                         int responseType, int serial, RIL_Errno e, void *response,
                         size_t responseLen);
    int iccTransmitApduBasicChannelResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responseLen);
    int iccTransmitApduLogicalChannelResponse(int slotId,
                           int responseType, int serial, RIL_Errno e,
                           void *response, size_t responseLen);
    int reportStkServiceIsRunningResponse(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responseLen);
    int requestIccSimAuthenticationResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responseLen);
    int sendEnvelopeResponse(int slotId,
                         int responseType, int serial, RIL_Errno e, void *response,
                         size_t responseLen);
    int sendEnvelopeWithStatusResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responseLen);
    int sendTerminalResponseToSimResponse(int slotId,
                           int responseType, int serial, RIL_Errno e,
                           void *response, size_t responseLen);
    int setAllowedCarriersResponse_1_4(int slotId,
                        int responseType, int serial, RIL_Errno e,
                            void *response, size_t responseLen);
    int setCarrierInfoForImsiEncryptionResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responseLen);
    int setCdmaSubscriptionSourceResponse(int slotId,
                         int responseType, int serial, RIL_Errno e, void *response,
                         size_t responseLen);
    int setFacilityLockForAppResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responseLen);
    int setSimCardPowerResponse(int slotId,
                           int responseType, int serial, RIL_Errno e,
                           void *response, size_t responseLen);
    int setUiccSubscriptionResponse(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responseLen);
    int supplyIccPin2ForAppResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responseLen);
    int supplyIccPinForAppResponse(int slotId,
                         int responseType, int serial, RIL_Errno e, void *response,
                         size_t responseLen);
    int supplyIccPuk2ForAppResponse(int slotId,
                            int responseType, int serial, RIL_Errno e, void *response,
                            size_t responseLen);
    int supplyIccPukForAppResponse(int slotId,
                              int responseType, int serial, RIL_Errno e,
                              void *response, size_t responseLen);
    int supplySimDepersonalizationResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responseLen);
    int updateSimPhonebookRecordsResponse(int slotId,
                               int responseType, int serial, RIL_Errno e,
                               void *response, size_t responseLen);
};
