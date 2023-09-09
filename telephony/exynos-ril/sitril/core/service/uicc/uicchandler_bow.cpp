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
 * uicchandler_bow.cpp (BIP over WIFI)
 *
 *  Created on: 2021. 2. 22.
 *      Author: hjun78.park
 */

#include "uicchandler_bow.h"
#include "rillog.h"
#include "protocolstkbuilder.h"
#include "protocolstkadapter.h"
#include "simservice.h"
#include "sitdef.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define PARAM_NULL(msg)         { if(msg==NULL) { RilLogE("%s::%s() Parameter = NULL", mServiceName, __FUNCTION__); return -1; } }
#define NULL_REQ(msg)           { if(msg==NULL || msg->GetRequestData()==NULL) { RilLogE("%s::%s() RequestData = NULL", mServiceName, __FUNCTION__); return -1; } }
#define NULL_RSP(msg)           { if(msg==NULL || msg->GetModemData()==NULL) { RilLogE("%s::%s() ModemData = NULL", mServiceName, __FUNCTION__); return -1; } }
#define ENTER_FUNC()            { RilLogI("%s::%s() [<-- ", mServiceName, __FUNCTION__); }
#define LEAVE_FUNC()            { RilLogI("%s::%s() [--> ", mServiceName, __FUNCTION__); }
#define NOT_IMPLEMENT()         { RilLogE("%s::%s() Not Implemented", mServiceName, __FUNCTION__); }

#define LOGI(format, ...)       RilLogI("%s::%s() " format, mServiceName, __FUNCTION__, ##__VA_ARGS__)
#define LOGV(format, ...)       RilLogV("%s::%s() " format, mServiceName, __FUNCTION__, ##__VA_ARGS__)
#undef LOGD
#define LOGD(format, ...)       RilLog("%s::%s() " format, mServiceName, __FUNCTION__, ##__VA_ARGS__)
#define LOGE(format, ...)       RilLogE("%s::%s() " format, mServiceName, __FUNCTION__, ##__VA_ARGS__)

/**
 * WifiChannelStatusReportHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(WifiChannelStatusReportHandler, MSG_SAT_WIFI_CHANNEL_STATUS_REPORT)
int WifiChannelStatusReportHandler::OnRequest(Message *pMsg)
{
    int nResult = -1;
    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        LOGE("rildata is NULL");
        return -1;
    }

    BYTE event = *((BYTE *) rildata->GetRawData());
    char szEvent[16];
    memset(szEvent, 0, 16);
    switch(event) {
    case WIFI_CHANNEL_EVENT_RECEIVED: strcpy(szEvent, "Received"); break;
    case WIFI_CHANNEL_EVENT_CONNECTED: strcpy(szEvent, "Connected"); break;
    case WIFI_CHANNEL_EVENT_CLOSED: strcpy(szEvent, "Closed"); break;
    default: strcpy(szEvent, "Unknown"); break;
    }

    LOGV("Channel Event : %d(%s)", event, szEvent);

    ProtocolStkBuilder builder;
    ModemData *pModemData = builder.BuildOemSatWifiRequest(RIL_REQUEST_OEM_WIFI_CHANNEL_STATUS_REPORT
            , (BYTE *) rildata->GetRawData(), rildata->GetSize());
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

int WifiChannelStatusReportHandler::OnResponse(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkResponseAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    OnRequestComplete(uErrCode==RIL_E_SUCCESS? RIL_E_SUCCESS : RIL_E_GENERIC_FAILURE);

    LEAVE_FUNC();
    return 0;
}

/**
 * WifiStatusResponseHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(WifiStatusResponseHandler, MSG_SAT_WIFI_STATUS_RESPONSE)
int WifiStatusResponseHandler::OnRequest(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    int nResult = -1;
    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        LOGE("rildata is NULL");
        return -1;
    }

    ProtocolStkBuilder builder;
    ModemData *pModemData = builder.BuildOemSatWifiRequest(RIL_REQUEST_OEM_WIFI_STATUS_RESPONSE
            , (BYTE *) rildata->GetRawData(), rildata->GetSize());
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

int WifiStatusResponseHandler::OnResponse(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkResponseAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    OnRequestComplete(uErrCode==RIL_E_SUCCESS? RIL_E_SUCCESS : RIL_E_GENERIC_FAILURE);

    LEAVE_FUNC();
    return 0;
}

/**
 * OpenChannelResponseHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(OpenChannelResponseHandler, MSG_SAT_WIFI_OPEN_CHANNEL_RESPONSE)
int OpenChannelResponseHandler::OnRequest(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    int nResult = -1;
    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        LOGE("rildata is NULL");
        return -1;
    }

    LOGV("Response: %s", *((BYTE *) rildata->GetRawData())? "Failure" : "Success");

    ProtocolStkBuilder builder;
    ModemData *pModemData = builder.BuildOemSatWifiRequest(RIL_REQUEST_OEM_WIFI_OPEN_CHANNEL_RESPONSE
            , (BYTE *) rildata->GetRawData(), rildata->GetSize());
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

int OpenChannelResponseHandler::OnResponse(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkResponseAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *) adapter.GetParameter(), adapter.GetParameterLength());
    } else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    LEAVE_FUNC();
    return 0;
}

/**
 * SendDataResponseHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SendDataResponseHandler, MSG_SAT_WIFI_SEND_DATA_RESPONSE)
int SendDataResponseHandler::OnRequest(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    int nResult = -1;
    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        LOGE("rildata is NULL");
        return -1;
    }

    LOGV("Response: %s", *((BYTE *) rildata->GetRawData())? "Failure" : "Success");

    ProtocolStkBuilder builder;
    ModemData *pModemData = builder.BuildOemSatWifiRequest(RIL_REQUEST_OEM_WIFI_SEND_DATA_RESPONSE
            , (BYTE *) rildata->GetRawData(), rildata->GetSize());
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

int SendDataResponseHandler::OnResponse(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkResponseAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *) adapter.GetParameter(), adapter.GetParameterLength());
    } else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    LEAVE_FUNC();
    return 0;
}

/**
 * ReceiveDataResponseHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ReceiveDataResponseHandler, MSG_SAT_WIFI_RECEIVE_DATA_RESPONSE)
int ReceiveDataResponseHandler::OnRequest(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    int nResult = -1;
    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        LOGE("rildata is NULL");
        return -1;
    }

    ProtocolStkBuilder builder;
    ModemData *pModemData = builder.BuildOemSatWifiRequest(RIL_REQUEST_OEM_WIFI_RECEIVE_DATA_RESPONSE
            , (BYTE *) rildata->GetRawData(), rildata->GetSize());
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

int ReceiveDataResponseHandler::OnResponse(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkResponseAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *) adapter.GetParameter(), adapter.GetParameterLength());
    } else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    LEAVE_FUNC();
    return 0;
}

/**
 * CloseChannelResponseHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CloseChannelResponseHandler, MSG_SAT_WIFI_CLOSE_CHANNEL_RESPONSE)
int CloseChannelResponseHandler::OnRequest(Message *pMsg)
{
    ENTER_FUNC();
    PARAM_NULL(pMsg);

    int nResult = -1;
    RawRequestData *rildata = (RawRequestData *)pMsg->GetRequestData();
    if (rildata == NULL) {
        LOGE("rildata is NULL");
        return -1;
    }

    LOGV("Response: %s", *((BYTE *) rildata->GetRawData())? "Failure" : "Success");

    ProtocolStkBuilder builder;
    ModemData *pModemData = builder.BuildOemSatWifiRequest(RIL_REQUEST_OEM_WIFI_CLOSE_CHANNEL_RESPONSE
            , (BYTE *) rildata->GetRawData(), rildata->GetSize());
    nResult = SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, mMessageId);

    LEAVE_FUNC();
    return (nResult<0)? -1: 0;
}

int CloseChannelResponseHandler::OnResponse(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkResponseAdapter adapter(pMsg->GetModemData());
    UINT uErrCode = adapter.GetErrorCode();
    if (uErrCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, (void *) adapter.GetParameter(), adapter.GetParameterLength());
    } else {
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
    }

    LEAVE_FUNC();
    return 0;
}

/**
 * WiFiStatusRequestHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(WiFiStatusRequestHandler, MSG_SAT_IND_WIFI_STATUS_REQUEST)
int WiFiStatusRequestHandler::OnIndication(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    OnUnsolicitedResponse(RIL_UNSOL_OEM_WIFI_STATUS_REQUEST);

    LEAVE_FUNC();
    return 0;
}

/**
 * OpenChannelRequestHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(OpenChannelRequestHandler, MSG_SAT_IND_WIFI_OPEN_CHANNEL_REQUEST)
int OpenChannelRequestHandler::OnIndication(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkIndAdapter adapter(pMsg->GetModemData());
    OnUnsolicitedResponse(RIL_UNSOL_OEM_WIFI_OPEN_CHANNEL_REQUEST, adapter.GetParameter(), adapter.GetParameterLength());

    LEAVE_FUNC();
    return 0;
}

/**
 * SendDataRequestHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SendDataRequestHandler, MSG_SAT_IND_WIFI_SEND_DATA_REQUEST)
int SendDataRequestHandler::OnIndication(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkIndAdapter adapter(pMsg->GetModemData());
    OnUnsolicitedResponse(RIL_UNSOL_OEM_WIFI_SEND_DATA_REQUEST, adapter.GetParameter(), adapter.GetParameterLength());

    LEAVE_FUNC();
    return 0;
}

/**
 * ReceiveDataRequestHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ReceiveDataRequestHandler, MSG_SAT_IND_WIFI_RECEIVE_DATA_REQUEST)
int ReceiveDataRequestHandler::OnIndication(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkIndAdapter adapter(pMsg->GetModemData());
    OnUnsolicitedResponse(RIL_UNSOL_OEM_WIFI_RECEIVE_DATA_REQUEST, adapter.GetParameter(), adapter.GetParameterLength());

    LEAVE_FUNC();
    return 0;
}

/**
 * CloseChannelRequestHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CloseChannelRequestHandler, MSG_SAT_IND_WIFI_CLOSE_CHANNEL_REQUEST)
int CloseChannelRequestHandler::OnIndication(Message *pMsg)
{
    ENTER_FUNC();
    NULL_RSP(pMsg);

    ProtocolStkIndAdapter adapter(pMsg->GetModemData());
    OnUnsolicitedResponse(RIL_UNSOL_OEM_WIFI_CLOSE_CHANNEL_REQUEST, adapter.GetParameter(), adapter.GetParameterLength());

    LEAVE_FUNC();
    return 0;
}
