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
 * miscserviceproxy.cpp
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#include "miscserviceproxy.h"
#include "miscservice.h"
#include "networkservice.h"

MiscServiceProxy::MiscServiceProxy(Service *service)
    : mService(NULL)
{
    // mService should be not NULL
    mService = MiscService::AsService(service);
}

MiscServiceProxy::~MiscServiceProxy()
{
    mService = NULL;
}

NetworkService *MiscServiceProxy::FindNetworkService()
{
    if (!HasService()) {
        return NULL;
    }
    return NetworkService::AsService(mService->GetCurrentService(RIL_SERVICE_NETWORK));
}
