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

struct HidlHalNetworkIndication : public HidlHalIndication, public CheckReturnStatus {
    using HidlHalIndication::HidlHalIndication;

    HidlHalNetworkIndication(const HidlHalIndication& hidlHal) : HidlHalIndication(hidlHal) {}

    int lceDataInd(int slotId, int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen);
    int barringInfoChangedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int cdmaPrlChangedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int cellInfoListInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int cellInfoListInd_1_2(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int cellInfoListInd_1_4(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int cellInfoListInd_1_5(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int currentPhysicalChannelConfigs(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int currentPhysicalChannelConfigs_1_4(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int currentSignalStrengthInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int currentSignalStrengthInd_1_2(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int currentSignalStrengthInd_1_4(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int imsNetworkStateChangedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int networkScanResultInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int networkScanResultInd_1_2(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int networkScanResultInd_1_4(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int networkScanResultInd_1_5(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int networkStateChangedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int nitzTimeReceivedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int registrationFailedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int restrictedStateChangedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int suppSvcNotifyInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
    int voiceRadioTechChangedInd(int slotId,
                            int indicationType, int token, RIL_Errno e, void *response,
                            size_t responselen);
};
