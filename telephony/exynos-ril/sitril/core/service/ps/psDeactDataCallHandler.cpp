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
 * psDeactDataCallHandler.cpp
 *
 *  Created on: 2020. 8. 19.
 *      Author: juneho.kwon
 */

#include "pshandler.h"
#include "rillog.h"
#include "message.h"
#include "PsTimeoutConfig.h"
#include "datacallreqdata.h"
#include "pdpcontext.h"
#include "protocolpsbuilder.h"
#include "psserviceproxy.h"

#include "pslog.h"

/**
* DeactDataCallHandler
*/

/*
   Allowed RadioError even on CardState::ABSENT are
    RadioError::NONE
    RadioError::INVALID_CALL_ID
    RadioError::SIM_ABSENT
    RadioError::RADIO_NOT_AVAILABLE
    RadioError:OEM_ERROR_1 ~ OEM_ERROR_25
*/

IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DeactDataCallHandler, MSG_PS_DEACT_DATA_CALL)
int DeactDataCallHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    DeactivateDataCallRequestData *rildata = NULL;

    // if (!psProxy.getRilRequestData(msg, rildata)) return -1;
    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    rildata = (DeactivateDataCallRequestData *)msg->GetRequestData();

    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    PsServiceProxy psProxy(mService);
    if (!psProxy.HasService()) {
        return -1;
    }


    int cid = rildata->GetCid();
    int reason = rildata->GetDisconnectReason();

    RilLogI("Try Deact data call : CID=%d, Reason=%d", cid, reason);
    PdpContext *pdpContext = psProxy.getPdpContext(cid);
    if (pdpContext == NULL) {
        RilLogE("CID=%d is not valid", cid);
        OnRequestComplete(RIL_E_INVALID_CALL_ID);
        return 0;
    }

    if (pdpContext->GetState() != PDP_CONTEXT_CONNECTED) {
        RilLogE("PDP Context is not CONNTECTED(state=%d)", pdpContext->GetState());
        OnRequestComplete(RIL_E_INVALID_CALL_ID);
        return 0;
    }

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildDeactDataCall(cid, reason);
    if (SendRequest(pModemData, TIMEOUT_DEACT_DATA_CALL, mMessageId) < 0) return -1;

    pdpContext->SetState(PDP_CONTEXT_DISCONNECTING);
    psProxy.setDeactivatingPdpContext(pdpContext);

    return 0;
}

int DeactDataCallHandler::OnResponse(Message *msg)
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

int DeactDataCallHandler::OnTimeout(Message *msg)
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
