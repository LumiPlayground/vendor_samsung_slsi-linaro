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
 * psDataCallListChangedHandler.cpp
 *
 *  Created on: 2020. 8. 19.
 *      Author: juneho.kwon
 */

#include "pshandler.h"
#include "rillog.h"
#include "service.h"
#include "pdpcontext.h"
#include "protocolpsadapter.h"
#include "PsTestHandler.h"
#include "psserviceproxy.h"

#include "./pslog.h"

/*
 * DataCallListChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DataCallListChangedHandler, MSG_PS_DATA_CALL_LIST_CHANGED)


int DataCallListChangedHandler::OnIndication(Message *msg)
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
    pModemData = psProxy.getPsTestHandler()->replaceModemDataForTest(pModemData, 2);

    ProtocolPsDataCallListChangedAdapter adapter(pModemData);

    const DataCall *pDataCallList = adapter.GetDataCallList();
    int num = adapter.GetDataCallNum();
    bool bPdpActive = false;
    bool DataCallInformedArray[MAX_DATA_CALL_SIZE*2+1] = {false,};

    RilLog("DataCall Number=%d", num);
    for (int i = 0; i < num; i++) {
        const DataCall *dc = (pDataCallList + i);
        RilLog("DataCall : CID=%d Active=%d Status=0x%x", dc->cid, dc->active, dc->status);
        DataCallInformedArray[dc->cid] = true;

        // sync current data call list and PDP context
        // in case RIL PDP Context is active but notified data call is inactive.
        if (!psProxy.cleanUpPdpOnInactiveDc(dc)) bPdpActive = psProxy.updatePdpOnActiveDc(dc);
    } // end for i ~

    for (int i = 0; i < psProxy.getPdpContextSize(); i++) {
        PdpContext *pPdpContext = psProxy.getPdpContextByIndex(i);
        if (pPdpContext != NULL) {
            int cid = pPdpContext->GetCID();
            if (DataCallInformedArray[cid] == false) {
#ifndef DISABLE_EMULATE_IMS_PERMANENT_FAILURE
                num += psProxy.makePermanentFailureForIMS(pPdpContext);
#endif
            } else {
                ApnSetting *apnSetting = pPdpContext->GetApnSetting();
                if(apnSetting == NULL) RilLogV("[%s] %s() ApnSetting is null of cid=%d", mServiceName,  __FUNCTION__, cid);
                continue;
            }
            psProxy.cleanUpInactivePdp(pPdpContext);
        }
    }
    // All DataCall is lost
    if (num == 0) psProxy.resetPdpContext(true);
    RilProperty *property = psProxy.getRilContextProperty();
    property->Put(RIL_CONTEXT_PDP_ACTIVE, bPdpActive);

    psProxy.OnNotifyDataCallList();

    return 0;
}

