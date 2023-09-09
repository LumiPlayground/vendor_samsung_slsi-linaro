/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "vsimhandler.h"
#include "protocoladapter.h"
#include "protocolvsimbuilder.h"
#include "protocolvsimadapter.h"
#include "vsimdata.h"
#include "vsimdatabuilder.h"
#include "rillog.h"
#include "service.h"

#define TIMEOUT_VSIM_DEFAULT     30000

/**
 * VsimNotificationHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(VsimNotificationHandler, MSG_VSIM_NOTIFICATION)

int VsimNotificationHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int tid = rildata->GetInt(0);
    int eventid = rildata->GetInt(1);
    int simtype = rildata->GetInt(2);

    RilLog("[%s] tid: %d , eventid: %d, simtype: %d", __FUNCTION__, tid, eventid, simtype);

    ProtocolVsimBuilder builder;
    ModemData *pModemData = builder.BuildVsimNotification(tid, eventid, simtype);

    if (SendRequest(pModemData, TIMEOUT_VSIM_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int VsimNotificationHandler::OnResponse(Message *msg)
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

    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);

// MOX: Automatic Verifying PIN
#ifdef _AUTO_VERIFY_PIN_
        SimService *pSimService = (SimService *) GetRilContext()->GetService(RIL_SERVICE_SIM);
        if(pSimService) pSimService->SaveAutoVerifyPin();
        else RilLogE("pSimService is NULL");

        pSimService = (SimService *) GetOppositeService(RIL_SERVICE_SIM);
        if(pSimService) pSimService->SaveAutoVerifyPin();
        else RilLogE("GetOppositeService()::pSimService is NULL");
#endif

        RilProperty *property = mService->GetRilApplicationProperty();
        property->Put(RIL_UIM_REMOTE_SLOT, mService->GetRilSocketId() == 0 ? "0" : "1");

        // For enable/disable vsim without silent reset
        // guarantee nv file sync : cp send response right after NV file update, but for sure, add 500 msec
        // WaitForTimeUs(500'000U);
        // RilReset("VSIM_NOTIFICATION_RESET");
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    return 0;
}

/**
 * VsimOperationHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(VsimOperationHandler, MSG_VSIM_OPERATION)

int VsimOperationHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    VsimOperationData *rildata = (VsimOperationData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int tid = rildata->GetTid();
    int eventid = rildata->GetEventId();
    int result = rildata->GetResult();
    int datalen = rildata->GetDataLength();
    char* pData = rildata->GetData();

    RilLog("[%s] tid : %d , eventid :%d, result: %d, datalen: %d", __FUNCTION__, tid, eventid, result, datalen);

    ProtocolVsimBuilder builder;
    ModemData *pModemData = builder.BuildVsimOperation(tid, eventid, result, datalen, pData);

    if (SendRequest(pModemData, TIMEOUT_VSIM_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int VsimOperationHandler::OnResponse(Message *msg)
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

    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    return 0;
}


/**
 * VsimOperationIndHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(VsimOperationIndHandler, MSG_VSIM_OPERATION_IND)

int VsimOperationIndHandler::OnIndication(Message *msg)
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

    // notify to the upper layer with data
    ProtocolVsimOperationAdapter adapter(pModemData);

    int tid = adapter.GetTransactionId();
    int eventid = adapter.GetEventId();
    int result = adapter.GetResult();
    const char* pData = adapter.GetOperationData();  // null-terminated HEX string
    int datalen = adapter.GetOperationDataLength();  // a length of HEX string
    RilLog("[%s] tid : %d , eventid(%d), result(%d), datalen(%d)", __FUNCTION__, tid, eventid, result, datalen);

    if ( eventid == -1 )
    {
        RilLogE("%s : invalid eventid, ignore CP's indication", __FUNCTION__);
        return 0;
    }

    VsimDataBuilder builder;

    // External path
    const RilData *rildataExt = builder.BuildVsimOperationExt(tid, eventid, result, datalen, pData);
    if (rildataExt != NULL) {
        OnUnsolicitedResponse(RIL_UNSOL_OEM_VSIM_OPERATION, rildataExt->GetData(), rildataExt->GetDataLength());
        delete rildataExt;
    }

    return 0;
}
