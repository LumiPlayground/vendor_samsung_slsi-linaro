/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * cscserviceproxy.cpp
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */
#include "audioservice.h"
#include "cscservice.h"
#include "cscserviceproxy.h"
#include "networkservice.h"
#include "emergencycallhelper.h"

#define CHECK_NULL_SERVICE() { if(!HasService()) {return NULL;} }

CscServiceProxy::CscServiceProxy(Service *service)
    : mService(NULL) {
    // mService should be not NULL
    mService = CscService::AsService(service);
}

CscServiceProxy::~CscServiceProxy() {
    mService = NULL;
}

CallList *CscServiceProxy::GetCallList() {
    CHECK_NULL_SERVICE();
    return mService->m_currCallList;
}

CallId *CscServiceProxy::GetCallId() {
    CHECK_NULL_SERVICE();
    return &mService->m_CallId;
}

NetworkService *CscServiceProxy::FindNetworkService() {
    CHECK_NULL_SERVICE();
    return NetworkService::AsService(mService->GetCurrentService(RIL_SERVICE_NETWORK));
}

AudioService *CscServiceProxy::FindAudioService() {
    CHECK_NULL_SERVICE();
    return AudioService::AsService(mService->GetCurrentService(RIL_SERVICE_AUDIO));
}

bool CscServiceProxy::IsRadioOffOrNotAvailable() {
    if (!HasService()) return false;
    return mService->IsRadioOffOrNotAvailable();
}

EmergencyCallHelper *CscServiceProxy::GetEmergencyCallHelper() {
    CHECK_NULL_SERVICE();
    return mService->m_emergencyCallHelper;
}
