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
 * psSetpreferredDataModemHandler.cpp
 *
 *  Created on: 2020. 8. 19.
 *      Author: juneho.kwon
 */

#include "pshandler.h"
#include "rillog.h"
#include "requestdata.h"
#include "message.h"
#include "rilproperty.h"
#include "PsTimeoutConfig.h"
#include "protocoladapter.h"
#include "protocolpsbuilder.h"
#include "networkutils.h"
#include "psserviceproxy.h"

#include "pslog.h"

/**
* SetPreferredDataModemHandler
*/

IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetPreferredDataModemHandler, MSG_PS_SET_PREFERRED_DATA_MODEM)

int SetPreferredDataModemHandler::OnRequest(Message *msg)
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

    PsServiceProxy psProxy(mService);
    if (!psProxy.HasService()) {
        return -1;
    }

    int dataModemId = rildata->GetInt();

    if( dataModemId < 0 || dataModemId > 2) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    RilLog("DoSetPreferredDataModem: dataModemId=%d", dataModemId);
    psProxy.GetRilApplicationProperty()->Put("dds", dataModemId);
    NetworkUtils::setDefaultDataSubscription(dataModemId);

    ProtocolPsBuilder builder;
    ModemData *pModemData = builder.BuildSetPreferredDataModem(dataModemId);
    if (SendRequest(pModemData, TIMEOUT_DEFAULT_PS, mMessageId) < 0) return -1;
    return 0;
}



int SetPreferredDataModemHandler::OnResponse(Message *msg)
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

    ModemData *modemData = msg->GetModemData();
    if (modemData == NULL) {
        RilLogE("modemData is NULL");
        return -1;
    }

    ProtocolRespAdapter adapter(modemData);
    int errorCode = adapter.GetErrorCode();
    // acceptable error codes by VTS
    //   RadioError::NONE
    //   RadioError::RADIO_NOT_AVAILABLE
    //   RadioError::INTERNAL_ERR
    // special error codes that handled by PhoneSwitcher
    //   RadioError::OP_NOT_ALLOWED_DURING_VOICE_CALL
    //   RadioError::INVALID_SIM_STATE
    switch (errorCode) {
    case RIL_E_SUCCESS:
    case RIL_E_RADIO_NOT_AVAILABLE:
    case RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL:
    case RIL_E_INVALID_SIM_STATE:
        break;
    default:
        errorCode = RIL_E_INTERNAL_ERR;
        break;
    }
    OnRequestComplete(errorCode);

    return 0;
}

