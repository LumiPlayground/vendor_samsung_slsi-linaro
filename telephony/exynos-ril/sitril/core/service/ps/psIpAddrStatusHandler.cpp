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
 * psIpAddrStatusHandler.cpp
 *
 *  Created on: 2020. 8. 19.
 *      Author: juneho.kwon
 */

#include "pshandler.h"
#include "rillog.h"
#include "message.h"
#include "pdpcontext.h"
#include "protocolpsadapter.h"
#include "psserviceproxy.h"

#include "pslog.h"

/*
 * IpAddrStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(IpAddrStatusHandler, MSG_PS_IND_ADDR_STATUS)


int IpAddrStatusHandler::OnIndication(Message *msg)
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
    ProtocolPsIpAddrStatusAdapter adapter(pModemData);

    PdpContext *pPdpContext = psProxy.getPdpContext(adapter.GetCid());
    if (pPdpContext == NULL) {
        RilLogE("PdpContext of Cid : %d is not available", adapter.GetCid());
        return -1;
    }

    char ipv4[MAX_IPV4_ADDR_LEN] = {0};
    char ipv6[MAX_IPV6_ADDR_LEN] = {0};
    adapter.GetAddrInfo(ipv4, ipv6);

    // Update DataCall
    pPdpContext->OnUpdated(adapter.GetStatus(), ipv4, ipv6);

    // Notify DataCall
    psProxy.OnNotifyDataCallList();

    return 0;
}

