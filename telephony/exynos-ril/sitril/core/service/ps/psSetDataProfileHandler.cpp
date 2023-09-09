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
 * psSetDataProfileHandler.cpp
 *
 *  Created on: 2020. 9. 7.
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
#include "../../../base/modemconfiguration.h"

#include "pslog.h"

/**
* SetDataProfileHandler
*/
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetDataProfileHandler, MSG_PS_SET_DATA_PROFILE)

int SetDataProfileHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    SetDataProfileRequestData *rildata = NULL;

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    rildata = (SetDataProfileRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    PsServiceProxy psProxy(mService);
    if (!psProxy.HasService()) {
        return -1;
    }

    psProxy.clearDataProfileTable();
    int size = rildata->GetSize();
    int halVer = rildata->GetHalVersion();

    for (int i = 0; i < size; i++) {
        const DataV2_0::RIL_DataProfileInfo *dpi = rildata->GetDataProfileInfo(i);
        // send APN if persistent true only
        if (dpi != NULL) {
            ProtocolPsBuilder builder;
            ModemData *pModemData = builder.BuildSetDataProfile(dpi);
            if (pModemData == NULL) {
                RilLogE("rildata is NULL, skip this profile");
                continue;
            }

            if (SendRequest(pModemData) < 0) {
                RilLogW("[%s] Failed to send DataProfileInfo", mServiceName);
                delete pModemData;
                return -1;
            } else {
                delete pModemData;
            }

            if((halVer == HAL_VERSION_CODE(1, 5)||halVer == HAL_VERSION_CODE(2, 0)) && !dpi->apn.empty()) {
                psProxy.addDataProfile(*dpi);
            }
        }
    } // end for i ~

    if (halVer == HAL_VERSION_CODE(2, 0)) {
        psProxy.printDataProfileTable();
    }

    OnRequestComplete(RIL_E_SUCCESS);

    return 0;
}

int SetDataProfileHandler::OnResponse(Message *msg)
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
