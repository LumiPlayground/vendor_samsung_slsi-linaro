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

struct HidlHalMessagingResponse : public HidlHalResponse, public CheckReturnStatus {
    using HidlHalResponse::HidlHalResponse;

    HidlHalMessagingResponse(const HidlHalResponse &hidlHal)
        : HidlHalResponse(hidlHal) {}

    int acknowledgeIncomingGsmSmsWithPduResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int acknowledgeLastIncomingCdmaSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int acknowledgeLastIncomingGsmSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int deleteSmsOnRuimResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int deleteSmsOnSimResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int getCdmaBroadcastConfigResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int getGsmBroadcastConfigResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int getSmscAddressResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int reportSmsMemoryStatusResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int sendCdmaSmsExpectMoreResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int sendCdmaSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int sendImsSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int sendSmsExpectMoreResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int sendSmsResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int setCdmaBroadcastActivationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int setCdmaBroadcastConfigResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int setGsmBroadcastActivationResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int setGsmBroadcastConfigResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int setSmscAddressResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int writeSmsToRuimResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
    int writeSmsToSimResponse(int slotId, int responseType, int serial, RIL_Errno e, void *response, size_t responseLen);
};
