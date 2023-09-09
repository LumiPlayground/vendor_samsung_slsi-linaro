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
#include "vendorfunctions.h"
#include "ril_internal.h"

extern RIL_RadioFunctions *s_vendorFunctions;

VendorFunctions::VendorFunctions() : VendorFunctions(s_vendorFunctions, 0) {}

VendorFunctions::VendorFunctions(uint32_t slotId)
    : VendorFunctions(s_vendorFunctions, slotId) {}

VendorFunctions::VendorFunctions(const RIL_RadioFunctions *vendorFunctions)
    : VendorFunctions(vendorFunctions, 0) {}

VendorFunctions::VendorFunctions(const RIL_RadioFunctions *vendorFunctions, uint32_t slotId)
    : mSlotId(slotId) {
    mVendorFunctions = {};
    if (vendorFunctions) {
        mVendorFunctions.onRequest = vendorFunctions->onRequest;
        mVendorFunctions.onStateRequest = vendorFunctions->onStateRequest;
    }
}

void VendorFunctions::onRequest(int request, void *data, size_t datalen, RIL_Token t) {
    if (mVendorFunctions.onRequest) {
#if defined(ANDROID_MULTI_SIM)
        mVendorFunctions.onRequest(request, data, datalen, t, (RIL_SOCKET_ID)mSlotId);
#else
        mVendorFunctions.onRequest(request, data, datalen, t);
#endif
    }
}

void VendorFunctions::onRequest(android::RequestInfo *pRI, void *data, size_t datalen) {
    if (android::addRequestToList(pRI)) {
        int request = pRI->pCI->requestNumber;
        onRequest(request, data, datalen, pRI);
    }
}

RIL_RadioState VendorFunctions::onRadioStateRequest() {
    if (mVendorFunctions.onRequest) {
#if defined(ANDROID_MULTI_SIM)
        return mVendorFunctions.onStateRequest((RIL_SOCKET_ID)mSlotId);
#else
        return mVendorFunctions.onStateRequest();
#endif
    }
    return RADIO_STATE_UNAVAILABLE;
}

bool VendorFunctions::dispatchVoid(int serial, int request) {
    android::RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return false;
    }
    onRequest(request, NULL, 0, pRI);
    return true;
}

bool VendorFunctions::dispatchVoid(android::RequestInfo *pRI) {
    if (pRI == NULL || pRI->pCI == NULL || pRI->pCI->responseFunction == NULL) {
        return false;
    }
    onRequest(pRI, NULL, 0);
    return true;
}

bool VendorFunctions::dispatchInts(int serial, int request, const std::vector<int32_t>& params) {
    android::RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return false;
    }
    dispatchInts(pRI, params);
    return true;
}

bool VendorFunctions::dispatchInts(android::RequestInfo *pRI, const std::vector<int32_t>& params) {
    if (pRI == NULL || pRI->pCI == NULL || pRI->pCI->responseFunction == NULL) {
        return false;
    }
    onRequest(pRI, (void *)params.data(), sizeof(int32_t) * params.size());
    return true;
}

bool VendorFunctions::dispatchString(int serial, int request, const std::string& str) {
    android::RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return false;
    }
    dispatchString(pRI, str);
    return true;
}

bool VendorFunctions::dispatchString(android::RequestInfo *pRI, const std::string& str) {
    if (pRI == NULL || pRI->pCI == NULL || pRI->pCI->responseFunction == NULL) {
        return false;
    }
    onRequest(pRI, (void *)str.c_str(), str.length());
    return true;
}

bool VendorFunctions::dispatchStrings(int serial, int request, const std::vector<std::string>& params) {
    android::RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return false;
    }
    dispatchStrings(pRI, params);
    return true;
}

bool VendorFunctions::dispatchStrings(android::RequestInfo *pRI, const std::vector<std::string>& params) {
    if (pRI == NULL || pRI->pCI == NULL || pRI->pCI->responseFunction == NULL) {
        return false;
    }

    std::vector<const char *> ptr(params.size());
    size_t i = 0;
    for (auto& s : params) {
        ptr[i++] = s.c_str();
    }
    onRequest(pRI, (void *)ptr.data(), sizeof(const char *) * ptr.size());
    return true;
}

bool VendorFunctions::dispatchRaw(int serial, int request, const std::vector<uint8_t>& rawBytes) {
    android::RequestInfo *pRI = android::addRequestToList(serial, mSlotId, request);
    if (pRI == NULL) {
        return false;
    }
    dispatchRaw(pRI, rawBytes);
    return true;
}

bool VendorFunctions::dispatchRaw(android::RequestInfo *pRI, const std::vector<uint8_t>& rawBytes) {
    if (pRI == NULL || pRI->pCI == NULL || pRI->pCI->responseFunction == NULL) {
        return false;
    }
    onRequest(pRI, (void *)rawBytes.data(), rawBytes.size());
    return true;
}