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

#include <telephony/ril.h>
#include <ril_internal.h>

#include <string>
#include <vector>

struct VendorFunctions {
    RIL_RadioFunctions mVendorFunctions;
    uint32_t mSlotId = 0;

    VendorFunctions();
    VendorFunctions(uint32_t slotId);
    VendorFunctions(const RIL_RadioFunctions *vendorFunctions);
    VendorFunctions(const RIL_RadioFunctions *vendorFunctions, uint32_t slotId);
    uint32_t slotId() const { return mSlotId; }
    void onRequest(int request, void *data, size_t datalen, RIL_Token t);
    void onRequest(android::RequestInfo *pRI, void *data, size_t datalen);
    RIL_RadioState onRadioStateRequest();

    bool dispatchVoid(int serial, int request);
    bool dispatchVoid(android::RequestInfo *pRI);
    bool dispatchInts(int serial, int request, const std::vector<int32_t>& params);
    bool dispatchInts(android::RequestInfo *pRI, const std::vector<int32_t>& params);
    bool dispatchString(int serial, int request, const std::string& str);
    bool dispatchString(android::RequestInfo *pRI, const std::string& str);
    bool dispatchStrings(int serial, int request, const std::vector<std::string>& params);
    bool dispatchStrings(android::RequestInfo *pRI, const std::vector<std::string>& params);
    bool dispatchRaw(int serial, int request, const std::vector<uint8_t>& rawBytes);
    bool dispatchRaw(android::RequestInfo *pRI, const std::vector<uint8_t>& rawBytes);
};