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
 * psSetPdnIpAddressHandler.cpp
 *
 *  Created on: 2020. 8. 19.
 *      Author: juneho.kwon
 */

#include "pshandler.h"
#include "rillog.h"
#include "PsTimeoutConfig.h"
#include "datacallreqdata.h"
#include "protocolpsbuilder.h"
#include "protocolpsadapter.h"
#include "message.h"
#include "psserviceproxy.h"

#include "pslog.h"

/**
* SetPdnIpAddressHandler
*/
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetPdnIpAddressHandler, MSG_PS_SET_PDN_IP_ADDRESS)

int SetPdnIpAddressHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    SetPdnIpAddressRequestData *rildata = (SetPdnIpAddressRequestData *)msg->GetRequestData();

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildSetPdnIpAddress(rildata->m_SetPdnIpAddressReq);
    if (SendRequest(pModemData, TIMEOUT_DEFAULT_PS, mMessageId) < 0) return -1;

    return 0;
}



int SetPdnIpAddressHandler::OnResponse(Message *msg)
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

    psProxy.OnDeactDataCallComplete(psProxy.getDeactivatingPdpContext());

    return 0;

}

