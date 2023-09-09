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

struct HidlHalVoiceResponse : public HidlHalResponse, public CheckReturnStatus {
    using HidlHalResponse::HidlHalResponse;

    HidlHalVoiceResponse(const HidlHalResponse& hidlHal) : HidlHalResponse(hidlHal) {}

    int acceptCallResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int cancelPendingUssdResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responseLen);

    int conferenceResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int dialResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responselen);

    int emergencyDialResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int exitEmergencyCallbackModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int explicitCallTransferResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int getCallForwardStatusResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int getCallWaitingResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int getClipResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int getClirResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int getLastCallFailCauseResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int getMuteResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int getPreferredVoicePrivacyResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int getTTYModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int handleStkCallSetupRequestFromSimResponse(int slotId, int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);

    int hangupConnectionResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int hangupForegroundResumeBackgroundResponse(int slotId, int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);

    int hangupWaitingOrBackgroundResponse(int slotId, int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);

    int rejectCallResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int sendBurstDtmfResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int sendCDMAFeatureCodeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int sendDtmfResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int sendUssdResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responseLen);

    int separateConnectionResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int setCallForwardResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int setCallWaitingResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int setClirResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int setMuteResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int setPreferredVoicePrivacyResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int setTTYModeResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int startDtmfResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int stopDtmfResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);

    int switchWaitingOrHoldingAndActiveResponse(int slotId, int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen);
};
