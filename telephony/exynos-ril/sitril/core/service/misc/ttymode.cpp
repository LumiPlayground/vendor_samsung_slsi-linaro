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
 * ttymode.cpp
 *
 *  Created on: 2020. 7. 7.
 *      Author: sungwoo48.choi
 */
#include "ttymode.h"
#include "miscdatabuilder.h"
#include "protocoladapter.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "rillog.h"
#include "service.h"

#define TIMEOUT_TTY_MODE_VERSION    5000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
/**
 * GetTtyModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetTtyModeHandler, MSG_MISC_QUERY_TTY)

int GetTtyModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.GetTtyMode();
    if (SendRequest(pModemData, TIMEOUT_TTY_MODE_VERSION, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetTtyModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscGetTtyAdapter adapter(pModemData);
    int ttymode = adapter.GetTtyMode();
    RilLogV("ttymode =%d", ttymode);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        MiscDataBuilder builder;
        const RilData *rildata = builder.BuildGetTtyModeResponse(ttymode);
        if (rildata != NULL){
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}


/**
 * SetTtyModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetTtyModeHandler, MSG_AUDIO_SET_TTY_MODE)

int SetTtyModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int ttyMode = rildata->GetInt();
    RilLogV("SetTtyMode request is = %d", ttyMode);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.SetTtyMode(ttyMode);
    if (SendRequest(pModemData, TIMEOUT_TTY_MODE_VERSION, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetTtyModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}
