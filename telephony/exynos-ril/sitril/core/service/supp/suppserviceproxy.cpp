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
 * suppserviceproxy.cpp
 */

#include "supplementaryservice.h"
#include "suppserviceproxy.h"

SuppServiceProxy::SuppServiceProxy(Service *service)
    : mService(NULL) {
    // mService should be not NULL
    mService = SupplementaryService::AsService(service);
}

SuppServiceProxy::~SuppServiceProxy() {
    mService = NULL;
}

int SuppServiceProxy::GetValidErrors(int errorCode) {
    if (!HasService()) {
        return RIL_E_INTERNAL_ERR;
    }
    return mService->GetValidErrors(errorCode);
}

bool SuppServiceProxy::IsOperatorUsingUnknownServiceClass() {
    if (!HasService()) {
        return false;
    }
    return mService->IsOperatorUsingUnknownServiceClass();
}

ClirInfo *SuppServiceProxy::GetClirInfo() {
    if (!HasService()) {
        return NULL;
    }
    return &mService->m_clirInfo;
}

bool SuppServiceProxy::IsUssdUserInitiated() {
    if (!HasService()) {
        return true;
    }
    return mService->m_ussdUserInitiated;
}

void SuppServiceProxy::SetUssdUserInitiated(bool ussdUserInitiated) {
    if (!HasService()) {
        return;
    }
    mService->m_ussdUserInitiated = ussdUserInitiated;
}
