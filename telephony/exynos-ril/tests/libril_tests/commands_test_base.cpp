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
#include "commands_test_base.h"

static VendorFunctionExcutor sVendorFunctionExcutor;

#if defined(ANDROID_MULTI_SIM)
static void onTestRequest(int request, void *data, size_t datalen, RIL_Token t,
        RIL_SOCKET_ID socket_id) {
    sVendorFunctionExcutor.onRequest(request, data, datalen, t, socket_id);
}
static RIL_RadioState onTestStateRequest(RIL_SOCKET_ID socket_id) {
    return sVendorFunctionExcutor.onStateRequest(socket_id);
}
#else
static void onTestRequest(int request, void *data, size_t datalen, RIL_Token t) {
    sVendorFunctionExcutor.onRequest(request, data, datalen, t, RIL_SOCKET_1);
}
static RIL_RadioState onTestStateRequest() {
    return sVendorFunctionExcutor.onStateRequest(RIL_SOCKET_1);
}
#endif

static RIL_RadioFunctions sTestVendorFunctions = {
    RIL_VERSION,
    onTestRequest,
    onTestStateRequest,
    nullptr,
    nullptr,
    nullptr,
};

namespace libriltest::commands {

VendorFunctions setupVendorFunctions(OnRequestListener *listener/*  = nullptr */, uint32_t slot/*  = 0 */) {
    sVendorFunctionExcutor.setOnRequestListener(listener);
    return VendorFunctions(&sTestVendorFunctions, slot);
}

}