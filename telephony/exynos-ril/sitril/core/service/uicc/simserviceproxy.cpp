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
 * simserviceproxy.cpp
 *
 *  Created on: 2020. 8. 11.
 *      Author: saesaem.lee
 */
#include "simservice.h"
#include <librilutils/systemproperty.h>
#include "simserviceproxy.h"


SimServiceProxy::SimServiceProxy(Service *service)
    : mService(NULL) {
    // mService should be not NULL
    mService = SimService::AsService(service);
}

SimServiceProxy::~SimServiceProxy() {
    mService = NULL;
}

int SimServiceProxy::GetCurrentUiccEnableState() {
    return mService->m_nCurrentUiccEnableState;
}

int SimServiceProxy::GetReqUiccEnableState() {
    return mService->m_nReqUiccEnableState;
}

void SimServiceProxy::SetReqUiccEnableState(int enableState) {
    mService->m_nReqUiccEnableState = enableState;
}

void SimServiceProxy::SetCurrentUiccEnableState(int enableState) {
    mService->m_nCurrentUiccEnableState = enableState;
    if (GetReqUiccEnableState() ==  SimService::UICC_APP_STATE_DISABLED) {
        V2_0::RIL_CardStatus status = mService->GetRilCardStatus();
        SystemProperty::Set(mService->m_szLastDisabledIccid, status.iccid);
    } else {
        SystemProperty::Set(mService->m_szLastDisabledIccid, "");
    }
}
