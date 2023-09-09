/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include "sapservice.h"

SapService::SapService(RilContext* pRilContext)
    : Service(pRilContext, RIL_SERVICE_SAP)
{
}

SapService::~SapService() {
}

int SapService::OnCreate(RilContext * /* pRilContext */)
{
    return 0;
}

void SapService::OnDestroy()
{

}

bool SapService::IsPossibleToPassInRadioOffState(int /* request_id */) {
    return true;
}

bool SapService::IsPossibleToPassInRadioUnavailableState(int /* request_id */) {
    return true;
}
