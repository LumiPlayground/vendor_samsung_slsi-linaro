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
 * psNasTimerStatusChangedHandler.cpp
 *
 *  Created on: 2021. 2. 8.
 */

#include "pshandler.h"
#include "rillog.h"
#include "message.h"
#include "psdatabuilder.h"
#include "protocolpsadapter.h"
#include "psserviceproxy.h"

#include "pslog.h"

/*
 * NasTimerStatusChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(NasTimerStatusChangedHandler, MSG_PS_NAS_TIMER_STATUS_CHANGED)


int NasTimerStatusChangedHandler::OnIndication(Message *msg) {
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
    ProtocolPsNasTimerStatusAdapter adapter(pModemData);
    const SitNasTimerStatus *pNasTimerStatus = adapter.GetNasTimerStatus();

    if (pNasTimerStatus != NULL) {
        RilLogV("NasTimerStatus{type=0x%02X status=0x%02X value=%d apn=%s},size:%d",
                pNasTimerStatus->type, pNasTimerStatus->status, pNasTimerStatus->value,
                pNasTimerStatus->apn, sizeof(SitNasTimerStatus));

        PsDataNasTimerStatusBuilder builder;
        const RilData *rildata = builder.BuildNasTimerStatus(pNasTimerStatus);
        if (rildata != NULL) {
            OnUnsolicitedResponse(RIL_UNSOL_NAS_TIMER_STATUS_IND, rildata->GetData(), rildata->GetDataLength());
            OnUnsolicitedResponse(RIL_UNSOL_OEM_NAS_TIMER_STATUS, adapter.GetParameter(), adapter.GetParameterLength());
            // Framework will reprogress Pending Process, Setup_Data_Call for Attach, Unblock DataFlow
            // Or Block
            delete rildata;
        }
    } else {
        RilLogE("Invalid Nas Timer information");
    }

    return 0;
}
