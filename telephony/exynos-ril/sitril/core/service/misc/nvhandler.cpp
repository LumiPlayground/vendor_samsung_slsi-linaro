/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include "messageid.h"
#include "miscdatabuilder.h"
#include "nvhandler.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "service.h"
#include "rillog.h"

#define TIMEOUT_NV_RESET    5000

/**
 * NvResetConfigHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(NvResetConfigHandler, MSG_MISC_NV_RESET_CONFIG)

int NvResetConfigHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int type = rildata->GetInt();
    RilLogV("[%s] %s(), reset type = %d", mServiceName, __FUNCTION__, type);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildRadioConfigReset(type);
    if (SendRequest(pModemData, TIMEOUT_NV_RESET, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int NvResetConfigHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);
    return 0;
}
