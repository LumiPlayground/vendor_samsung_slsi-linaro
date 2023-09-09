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
 * PsDedicatedBearerInfoUpdatedHandler.cpp
 *
 *  Created on: 2020. 8. 19.
 *      Author: juneho.kwon
 */

#include "pshandler.h"
#include "rillog.h"
#include "service.h"
#include "pdpcontext.h"
#include "protocoladapter.h"
#include "protocolpsadapter.h"
#include "psserviceproxy.h"

#include "pslog.h"

/*
 * PsDedicatedBearerInfoUpdatedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(PsDedicatedBearerInfoUpdatedHandler, MSG_PS_DEDICATED_BEARER_INFO_UPDATED)


int PsDedicatedBearerInfoUpdatedHandler::OnIndication(Message *msg)
{
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

    if (pModemData == NULL) {
        return -1;
    }
    //notify IMS Stack
    RilLogI("[%s] %s %s %s()", mServiceName, TAG,"notifying IMS Stack", __FUNCTION__);
    ProtocolIndAdapter adapterRaw(msg->GetModemData());
    OnUnsolicitedResponse(RIL_UNSOL_OEM_IMS_BEARER_INFO, adapterRaw.GetParameter(), adapterRaw.GetParameterLength());

    ProtocolPsDedicatedBearInfoAdapter adapter(pModemData);
    PdpContext *pPdpContext = psProxy.getPdpContext(adapter.GetCid());

    if (pPdpContext == NULL) {
        RilLogE("PdpContext of Cid : %d is not available", adapter.GetCid());
        return -1;
    }

    pPdpContext->OnUpdateQosInfo(adapter.GetStatus(), adapter.GetQosSessionInfo());
    psProxy.OnNotifyDataCallList();

    return 0;

}

