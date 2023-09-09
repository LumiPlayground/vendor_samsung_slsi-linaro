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
 * psGetDataCallListHandler.cpp
 *
 *  Created on: 2020. 8. 19.
 *      Author: juneho.kwon
 */

#include "pshandler.h"
#include "rillog.h"
#include "PsTimeoutConfig.h"
#include "pdpcontext.h"
#include "protocolpsbuilder.h"
#include "protocolpsadapter.h"
#include "psdatabuilder.h"
#include "rilapplication.h"
#include "PsTestHandler.h"
#include "psserviceproxy.h"
#include "../../../base/modemconfiguration.h"
#include "message.h"

#include "pslog.h"

/**
* GetDataCallListHandler
*/
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetDataCallListHandler, MSG_PS_GET_DATA_CALL_LIST)

int GetDataCallListHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildGetDataCallList();
    if (SendRequest(pModemData, TIMEOUT_DEACT_DATA_CALL, mMessageId) < 0) return -1;

    return 0;
}



int GetDataCallListHandler::OnResponse(Message *msg)
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
    pModemData = psProxy.getPsTestHandler()->replaceModemDataForTest(pModemData, 1);

    ProtocolPsDataCallListAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {

        PsDataCallListBuilder builder(RilApplication::RIL_HalVersionCode);
        builder.setIpv4DnsQueryFirst(psProxy.isSameTargetOperator(TARGET_OPER_CMCC) || psProxy.isSameTargetOperator(TARGET_OPER_CHNOPEN));
        for (int i = 0; i < psProxy.getPdpContextSize(); i++) {
            PdpContext *pPdpContext = psProxy.getPdpContextByIndex(i);
            if (pPdpContext != NULL) {
                int cid = pPdpContext->GetCID();
                const DataCall *dc = adapter.GetDataCallByCid(cid);
                if (dc != NULL) {
                    bool deact = pPdpContext->GetActive() == INACTIVE && dc->active == ACTIVE;
                    bool reset = pPdpContext->GetActive() >= ACTIVE_AND_LINKDOWN && dc->active == INACTIVE;

                    // RIL inactive / CP active -> request deact and update state
                    if (deact) {
                        RilLogV("Deactivate PDP Context{cid=%d,state=%d,active=%d}", pPdpContext->GetCID(), pPdpContext->GetState(), pPdpContext->GetActive());
                        psProxy.OnRequestDeactDataCall(cid);
                    }

                    // RIL active / CP inactive -> update state
                    if (deact || reset) {
                        RilLogV("Reset PDP Context{cid=%d,state=%d,active=%d}", pPdpContext->GetCID(), pPdpContext->GetState(), pPdpContext->GetActive());
                        pPdpContext->OnDeactivated();
                    }
                }

                if (pPdpContext->GetState() == PDP_CONTEXT_CONNECTED) {
                    pPdpContext->SetQosSession();
                    builder.AddDataCall(pPdpContext);
                }
            }
        } // end for i ~

        int halVer= RilApplication::RIL_HalVersionCode;
        const RilData *rildata = builder.Build(halVer);
        if (rildata != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    } else OnRequestComplete(RIL_E_GENERIC_FAILURE);


    return 0;
}
