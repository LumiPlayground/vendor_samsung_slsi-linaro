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

struct HidlHalDataIndication : public HidlHalIndication, public CheckReturnStatus {
    using HidlHalIndication::HidlHalIndication;

    HidlHalDataIndication(const HidlHalIndication& hidlHal) : HidlHalIndication(hidlHal) {}

int dataCallListChangedInd_1_4(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responselen);

int dataCallListChangedInd_1_5(int slotId, int indicationType,
                           int token, RIL_Errno e, void *response, size_t responselen);


int pcoDataInd(int slotId,
               int indicationType, int token, RIL_Errno e, void *response,
               size_t responselen);


int keepaliveStatusInd(int slotId,
                       int indicationType, int token, RIL_Errno e, void *response,
                       size_t responselen);


};