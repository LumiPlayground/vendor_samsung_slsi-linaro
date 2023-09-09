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
 * dtmf.cpp
 *
 *  Created on: 2020. 7. 8.
 *      Author: sungwoo48.choi
 */

#include "dtmf.h"
#include "callreqdata.h"
#include "miscservice.h"
#include "protocoladapter.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "rillog.h"

#define MISC_DTMF_TIMEOUT   15000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * DtmfHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DtmfHandler, MSG_MISC_DTMF)

int DtmfHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    DtmfInfo* pDtmfReqData = (DtmfInfo*)msg->GetRequestData();
    ProtocolDtmfBuilder builder;
    ModemData *pModemData = builder.BuildDtmf(strlen(pDtmfReqData->m_szDtmf), pDtmfReqData->m_szDtmf);
    RilLogI("[%s] do Dtmf: %s", TAG, pDtmfReqData->m_szDtmf);
    if (SendRequest(pModemData, MISC_DTMF_TIMEOUT, mMessageId) < 0 ) {
        return -1;
    }

    return 0;
}

int DtmfHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    } else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * DtmfStartHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DtmfStartHandler, MSG_MISC_DTMF_START)

int DtmfStartHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    DtmfInfo* rildata = (DtmfInfo*)msg->GetRequestData();

    MiscService *misc = MiscService::AsService(mService);
    if (misc == NULL) {
        return -1;
    }

    int cardState = misc->GetUiccCardState();
    // On Sim absent, requesting command into CP cause IPC TIMEOUT
    // So return early,
    if( cardState != RIL_CARDSTATE_PRESENT ) {
        RilLogI("[%s] %s cardState(%d) is not Present(%d)",
                TAG, __FUNCTION__, cardState, RIL_CARDSTATE_PRESENT);
        OnRequestComplete(RIL_E_INVALID_MODEM_STATE);
        return 0;
    }

    ProtocolDtmfBuilder builder;
    ModemData *pModemData = builder.BuildDtmfStart(false, rildata->m_szDtmf[0]);
    RilLogV("[%s] Start Dtmf: %c", TAG, rildata->m_szDtmf[0]);
    if (SendRequest(pModemData, MISC_DTMF_TIMEOUT, mMessageId) < 0 ) {
        return -1;
    }

    return 0;
}

int DtmfStartHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    } else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * DtmfStopHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DtmfStopHandler, MSG_MISC_DTMF_STOP)

int DtmfStopHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    MiscService *misc = MiscService::AsService(mService);
    if (misc == NULL) {
        return -1;
    }

    int cardState = misc->GetUiccCardState();
    // On Sim absent, requesting command into CP cause IPC TIMEOUT
    // So return early,
    if( cardState != RIL_CARDSTATE_PRESENT ) {
        RilLogI("[%s] %s cardState(%d) is not Present(%d)",
                TAG, __FUNCTION__, cardState, RIL_CARDSTATE_PRESENT);
        OnRequestComplete(RIL_E_INVALID_MODEM_STATE);
        return 0;
    }

    ProtocolDtmfBuilder builder;
    ModemData *pModemData = builder.BuildDtmfStop();
    if (SendRequest(pModemData, MISC_DTMF_TIMEOUT, mMessageId) < 0 ) {
        return -1;
    }

    return 0;
}

int DtmfStopHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    } else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }
    return 0;
}
