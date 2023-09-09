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
 * DetachHandler.cpp
 *
 *  Created on: 2020. 8. 19.
 *      Author: juneho.kwon
 */

#include "pshandler.h"
#include "rillog.h"
#include "requestdata.h"
#include "message.h"
#include "PsTimeoutConfig.h"
#include "protocolpsbuilder.h"
#include "psserviceproxy.h"

#include "pslog.h"

IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DetachHandler, MSG_PS_DETACH)
int DetachHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = NULL;
    rildata = (IntRequestData *)msg->GetRequestData();

    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int detachReason = rildata->GetInt();
    RilLogV("[%s] %s(), detachReason = %d", mServiceName, __FUNCTION__, detachReason);

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildDetach(detachReason);
    if (SendRequest(pModemData, TIMEOUT_DEFAULT_PS, mMessageId) < 0) { RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}



int DetachHandler::OnResponse(Message *msg)
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

    return psProxy.OnSimpleRequestDone(msg);
}

