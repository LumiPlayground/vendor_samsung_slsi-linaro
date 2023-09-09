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
 * psDedicatedBearerInfoUpdatedLegacyHandler.cpp
 *
 *  Created on: 2021. 2. 8.
 */

#include "pshandler.h"
#include "rillog.h"
#include "protocolpsadapter.h"
#include "psserviceproxy.h"
#include "pslog.h"

/*
 * DedicatedBearerInfoUpdatedLegacyHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DedicatedBearerInfoUpdatedLegacyHandler, MSG_PS_DEDICATED_BEARER_INFO_UPDATED)


int DedicatedBearerInfoUpdatedLegacyHandler::OnIndication(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    PsServiceProxy psProxy(mService);
    if (!psProxy.HasService()) {
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    ProtocolPsDedicatedBearInfoAdapter adapter(pModemData);
    const DedicatedBearerInfo *pDedicatedBearerInfo = adapter.GetDedicatedBearerInfo();
    if (pDedicatedBearerInfo != NULL) {
        RilLogV("DedicatedBearerInfo{status=0x%02X type=0x%02X QCI=0x%02X DL GBR=%d UL GBR=%d DL MAX GBR=0x%d UL MAX GBR=0x%d}",
                pDedicatedBearerInfo->status, pDedicatedBearerInfo->type, pDedicatedBearerInfo->qci,
                pDedicatedBearerInfo->dl_gbr, pDedicatedBearerInfo->ul_gbr,
                pDedicatedBearerInfo->dl_max_gbr, pDedicatedBearerInfo->ul_max_gbr);
       // OnUnsolicitedResponse(RIL_UNSOL_OEM_IMS_DEDICATED_PDN_INFO, pDedicatedBearerInfo, sizeof(DedicatedBearerInfo));
    } else {
        RilLogE("Invalid Dedicated Bearer information");
       // OnUnsolicitedResponse(RIL_UNSOL_OEM_IMS_DEDICATED_PDN_INFO);
    }

    return 0;
}
