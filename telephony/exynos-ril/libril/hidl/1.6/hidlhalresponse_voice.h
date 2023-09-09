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

#include <hidl/1.6/hidlhalresponse_1_6.h>

struct HidlHalVoiceResponse_1_6 : public HidlHalResponse_1_6, public CheckReturnStatus {
    using HidlHalResponse_1_6::HidlHalResponse_1_6;

    HidlHalVoiceResponse_1_6(const HidlHalResponse_1_6& hidlHal) : HidlHalResponse_1_6(hidlHal) {}

    int getCurrentCallsResponse_1_6(int slotId, int responseType, int serial, RIL_Errno e, void *response,
            size_t responselen);
};
