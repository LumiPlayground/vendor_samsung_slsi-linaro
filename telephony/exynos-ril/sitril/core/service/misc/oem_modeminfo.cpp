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
 * oem_modeminfo.cpp
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#include "miscdata.h"
#include "miscdatabuilder.h"
#include "miscserviceproxy.h"
#include "networkservice.h"
#include "oem_modeminfo.h"
#include "protocoladapter.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "protocolnetbuilder.h"
#include "rillog.h"
#include "service.h"
#include <librilutils/systemproperty.h>

#define TIMEOUT_OEM_MODEM_INFO_DEFAULT  5000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * ModemInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ModemInfoHandler, MSG_MISC_OEM_MODEM_INFO)

int ModemInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    char *data = (char *)rildata->GetRawData();
    unsigned int datalen = rildata->GetSize();
    if (data == NULL || datalen < sizeof(int) * 2) {
        return -1;
    }

    int type = *((int *)data);
    unsigned int size = *((int *)data + 1);
    char *params = NULL;
    if (size + sizeof(int) * 2 > datalen) {
        return -1;
    }

    if (size > 0) {
        params = (char *)data + (sizeof(int) * 2);
    }
    RilLog("[%d] %s type=%d param length=%d", mService->GetRilSocketId(), __FUNCTION__, type, size);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildModemInfo(type, params, size);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int ModemInfoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolOemModemInfoAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        MiscDataBuilder builder;
        int command = adapter.GetCommandType();
        unsigned int size = adapter.GetSize();
        char *data = (char *)adapter.GetData();
        RilLog("[%d] ModemInfo: command=%d size=%u resp=%s", mService->GetRilSocketId(), command, size, data);

        const RilData *rildata = builder.BuildModemInfo(command, data, size);
        if (rildata != NULL) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }
    else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * ModemInfoReceivedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ModemInfoReceivedHandler, MSG_MISC_OEM_MODEM_INFO_RECEIVED)

int ModemInfoReceivedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolOemModemInfoIndAdapter adapter(pModemData);
    MiscDataBuilder builder;
    int command = adapter.GetCommandType();
    unsigned int size = adapter.GetSize();
    char *data = (char *)adapter.GetData();
    RilLogV("[%d] ModemInfo: command=%d size=%u", mService->GetRilSocketId(), command, size);
    const RilData *rildata = builder.BuildModemInfo(command, data, size);
    if (rildata != NULL) {
        OnUnsolicitedResponse(RIL_UNSOL_OEM_MODEM_INFO, rildata->GetData(), rildata->GetDataLength());
        delete rildata;
    }

    return 0;
}

/**
 * ModemResetHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(ModemResetHandler, MSG_MISC_OEM_MODEM_RESET)

int ModemResetHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolRadioPowerBuilder builder;
    ModemData *pModemData = builder.BuildRestartModem();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(ModemResetHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE3()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SwitchModemFunctionHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SwitchModemFunctionHandler, MSG_MISC_OEM_SET_FUNC_SWITCH_REQ)

int SwitchModemFunctionHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int feature = rildata->GetInt(0);
    BYTE enable = (BYTE)rildata->GetInt(1);
    RilLogV("[%d] Switch Modem feature=%d enable=%d", mService->GetRilSocketId(), feature, enable);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSwitchModemFunction(feature, enable);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int SwitchModemFunctionHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolOemSwitchModemFunctionAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        BYTE result = (BYTE)adapter.GetResult();
        RilLog("[%d] Switch Modem Function: result=%d", mService->GetRilSocketId(), result);
        OnRequestComplete(RIL_E_SUCCESS, &result, sizeof(BYTE));
    }
    else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetPdcpDiscardTimerHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetPdcpDiscardTimerHandler, MSG_MISC_OEM_SET_PDCP_DISCARD_TIMER)

int SetPdcpDiscardTimerHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int discardTimer = rildata->GetInt();
    RilLogV("[%d] Set PDCP Discard Timer=%d", mService->GetRilSocketId(), discardTimer);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetPdcpDiscardTimer(discardTimer);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetPdcpDiscardTimerHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE3()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetSelflogHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSelflogHandler, MSG_MISC_OEM_SET_SELFLOG)

int SetSelflogHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int mode = rildata->GetInt(0);
    int size = rildata->GetInt(1);
    RilLogV("[%s] %s(), mode = %s sise = %d", mServiceName, __FUNCTION__, (mode == 0) ? "START" : "STOP", size);

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.BuildSetSelflog(mode, size);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetSelflogHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscSetSelflogAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int result = adapter.GetSelflogResult();
        RilLogV("[%s] %s(), result = %d", mServiceName, __FUNCTION__, result);
        OnRequestComplete(errorCode, &result, sizeof(int));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetSelfLogProfileHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSelfLogProfileHandler, MSG_MISC_OEM_SET_SELFLOG_PROFILE)

int SetSelfLogProfileHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.BuildSetSelflogProfile();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetSelfLogProfileHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ProtocolMiscSetSelflogProfileAdapter)
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetSelfLogStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetSelfLogStatusHandler, MSG_MISC_OEM_GET_SELFLOG_STATUS)

int GetSelfLogStatusHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.BuildGetSelflogStatus();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetSelfLogStatusHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscGetSelflogStatusAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int status = adapter.GetSelflogStatus();
        RilLogV("[%s] %s(), status = %d", mServiceName, __FUNCTION__, status);
        OnRequestComplete(errorCode, &status, sizeof(int));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * UnsolSelflogStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolSelflogStatusHandler, MSG_MISC_UNSOL_SELFLOG_STATUS)

int UnsolSelflogStatusHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ProtocolMiscSelflogStatusAdapter adapter(msg->GetModemData());
    BYTE status = adapter.GetIndSelflogStatus();
    RilLogV("[%s] %s() : Status:%d", mServiceName, __FUNCTION__, status);
    OnUnsolicitedResponse(RIL_UNSOL_OEM_SELFLOG_STATUS, &status, 1);
    return 0;
}

/**
 * SetActivateVsimHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetActivateVsimHandler, MSG_MISC_SET_ACTIVATE_VSIM)

int SetActivateVsimHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    SetActivateVsimReqData *rildata = (SetActivateVsimReqData *)msg->GetRequestData();
    int slot = rildata->GetSlotId();
    const char *pIccid = rildata->GetIccid();
    int iccidLen = (pIccid == NULL)? 0 : strlen(pIccid);
    const char *pImsi = rildata->GetImsi();
    int imsiLen = (pImsi == NULL)? 0 : strlen(pImsi);
    const char *pHplmn = rildata->GetHomePlmn();
    int vsimState = rildata->GetVsimState();
    int vsimCardType = rildata->GetVsimCardType();

    RilLogV("[%s] %s(), %d, %s, %s, %s, %d, %d", mServiceName, __FUNCTION__, slot,
            pIccid, pImsi, pHplmn, vsimState, vsimCardType);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetActivateVsim(slot, iccidLen, pIccid,
            imsiLen, pImsi, pHplmn,
            vsimState, vsimCardType);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetActivateVsimHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscSetActiveVsimAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int isSuccsee = adapter.GetResult();
        RilLogV("[%s] %s(), isSuccess = %d", mServiceName, __FUNCTION__, isSuccsee);
        OnRequestComplete(RIL_E_SUCCESS, &isSuccsee, sizeof(int));
    }
    else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * GetCqiInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetCqiInfoHandler, MSG_MISC_OEM_GET_CQI_INFO)

int GetCqiInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildGetCqiInfo();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetCqiInfoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolOemGetCqiInfoAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int type = (int)adapter.GetCqiType();
        int cqiInfo0 = (int)adapter.GetCqiInfo0();
        int cqiInfo1 = (int)adapter.GetCqiInfo1();
        int ri = (int)adapter.GetRi();
        RilLogV("[%s] %s(), type = %d CQIInfo0 = %d CQIInfo1 = %d RI = %d", mServiceName, __FUNCTION__, type, cqiInfo0, cqiInfo1, ri);

        int response[] = {type, cqiInfo0, cqiInfo1, ri};
        OnRequestComplete(errorCode, response, sizeof(response));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetSarSettingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSarSettingHandler, MSG_MISC_OEM_SET_SAR_SETTING)

int SetSarSettingHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int dsi = rildata->GetInt();
    RilLogV("[%s] %s(), DSI = %d", mServiceName, __FUNCTION__, dsi);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetSarSetting(dsi);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetSarSettingHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetGmoSwitchHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetGmoSwitchHandler, MSG_MISC_OEM_SET_GMO_SWITCH)

int SetGmoSwitchHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int feature = rildata->GetInt();
    RilLogV("[%s] %s(), Feature = %d", mServiceName, __FUNCTION__, feature);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetGmoSwitch(feature);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetGmoSwitchHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetTcsFciHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetTcsFciHandler, MSG_MISC_OEM_SET_TCS_FCI_REQ)

int SetTcsFciHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == nullptr) {
        RilLogE("rildata is NULL");
        return -1;
    }

    const int minTcsFciDataLen = sizeof(int) * 2;
    void *data = rildata->GetRawData();
    if (data == nullptr || rildata->GetSize() < minTcsFciDataLen) {
        return -1;
    }

    int state = ((int *)data)[0];
    int len = ((int *)data)[1];
    char *fci = (char *)((int *)data + 2);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetTcsFci(state, len, fci);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetTcsFciHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ProtocolMiscSetTcsFciAdapter)
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetTcsFciHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetTcsFciHandler, MSG_MISC_OEM_GET_TCS_FCI_INFO)

int GetTcsFciHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildGetTcsFci();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetTcsFciHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscGetTcsFciAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        const char *fci = adapter.GetFci();
        RilLogV("[%s] %s(), FCI = %s", mServiceName, __FUNCTION__, fci);
        OnRequestComplete(errorCode, (void *)fci, MAX_FCI_LEN);
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetCABandwidthFilterHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetCABandwidthFilterHandler, MSG_MISC_OEM_SET_CA_BW_FILTER)

int SetCABandwidthFilterHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int enable = rildata->GetInt();
    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetCABandwidthFilter(enable);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetCABandwidthFilterHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * CABandwidthFilterIndHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CABandwidthFilterIndHandler, MSG_MISC_OEM_CA_BW_FILTER_IND)

int CABandwidthFilterIndHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolCaBandwidthFilterIndAdapter adapter(pModemData);
    int caConfig = adapter.GetCaConfig();
    int nrb = adapter.GetNRB();
    RilLogV("[%s] CA config=%d NRB=%d", mServiceName, caConfig, nrb);

    int response[] = { caConfig, nrb };
    OnUnsolicitedResponse(RIL_UNSOL_OEM_CA_BANDWIDTH_FILTER, response, sizeof(response));

    return 0;
}

/**
 * SetModemLogDumpHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetModemLogDumpHandler, MSG_MISC_SET_MODEM_LOG_DUMP)

int SetModemLogDumpHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.BuildSetModemLogDump();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetModemLogDumpHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ProtocolMiscSetModemLogDumpAdapter)
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetElevatortSensorHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetElevatortSensorHandler, MSG_MISC_OEM_SET_ELEVATOR_SENSOR)

int SetElevatortSensorHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int enable = rildata->GetInt();
    RilLogV("[%s] %s(), Feature = %d", mServiceName, __FUNCTION__, enable);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetElevatorSensor(enable);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetElevatortSensorHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * EndcCapabilityReceivedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(EndcCapabilityReceivedHandler, MSG_MISC_OEM_IND_NTW_ENDC_CAPABILITY)

int EndcCapabilityReceivedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscEndcCapabilityIndAdapter adapter(pModemData);
    int capability = adapter.GetCapability();
    int cause = adapter.GetCause();
    RilLogV("[%s] ENDCCapaInd Capability=%d Cause=%d", mServiceName, capability, cause);

    int response[] = {capability, cause};
    OnUnsolicitedResponse(RIL_UNSOL_IND_ENDC_CAPABILITY, response, sizeof(response));

    return 0;
}

/**
 * SetForbidLteCellHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetForbidLteCellHandler, MSG_MISC_OEM_SET_FORBID_LTE_CELL)

int SetForbidLteCellHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    void *data = rildata->GetRawData();
    unsigned int dataLen = rildata->GetSize();
    if (dataLen < sizeof(int)*3) {
        RilLogE("Data is invalid");
        return -1;
    }
    int mode = ((int *)data)[0];
    int cellId = ((int *)data)[1];
    int forbiddenTimer = ((int *)data)[2];

    char plmn[MAX_PLMN_LEN + 1] = {0, };
    unsigned int dataLenToCopyPlmn = dataLen - sizeof(int)*3;
    memcpy(plmn, (char *)((int *)data + 3), MIN(dataLenToCopyPlmn, MAX_PLMN_LEN));

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetForbidLteCell(mode, cellId, forbiddenTimer, plmn);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetForbidLteCellHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ProtocolMiscSetForbidLteCellAdapter)
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetApSystemTimeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetApSystemTimeHandler, MSG_MISC_SET_AP_SYSTEM_TIME)

int SetApSystemTimeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.BuildSetApSystemTime();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetApSystemTimeHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ProtocolMiscSetApSystemTimeAdapter)
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * BaseStationReceivedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(BaseStationReceivedHandler, MSG_MISC_OEM_IND_BASESTATION_INFO)

int BaseStationReceivedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscBaseStationIndAdapter adapter(pModemData);
    RilLogV("[%d] %s GetParameterLength()=%u", mService->GetRilSocketId(), __FUNCTION__, adapter.GetParameterLength());
    OnUnsolicitedResponse(RIL_UNSOL_OEM_BASESTATION_INFO, (void *)adapter.GetParameter(), adapter.GetParameterLength());

    return 0;
}

/**
 * GetModemActivityInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetModemActivityInfoHandler, MSG_MISC_OEM_GET_MODEM_ACTIVITY_INFO)

int GetModemActivityInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildGetModemActivityInfo();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetModemActivityInfoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscGetModemActivityInfoAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        RilLogV("[%d] %s GetParameterLength()=%u", mService->GetRilSocketId(), __FUNCTION__, adapter.GetParameterLength());
        OnRequestComplete(RIL_E_SUCCESS, (void *)adapter.GetParameter(), adapter.GetParameterLength());
    }
    else {
        OnRequestComplete(RIL_E_MODEM_ERR);
    }
    return 0;
}

/**
 * SetSvnHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSvnHandler, MSG_MISC_OEM_SET_SVN)

int SetSvnHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int svn = rildata->GetInt();
    if (svn < 0 || svn > 99) {
        OnRequestComplete(RIL_E_INVALID_ARGUMENTS);
        return 0;
    }

    char arg[2] = {0};
    arg[0] = (svn / 10) + '0';
    arg[1] = (svn % 10) + '0';

    RilLogV("[%s] %s(), SVN = %c %c", mServiceName, __FUNCTION__, arg[0], arg[1]);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildOemSetSvn(arg);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetSvnHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * DroppedMtCallInfoReceivedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DroppedMtCallInfoReceivedHandler, MSG_MISC_OEM_IND_DROPPED_MT_CALL_INFO)

int DroppedMtCallInfoReceivedHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolIndAdapter adapter(pModemData);
    RilLogV("[%d] %s GetParameterLength()=%u", mService->GetRilSocketId(), __FUNCTION__, adapter.GetParameterLength());
    OnUnsolicitedResponse(RIL_UNSOL_OEM_DROPPED_MT_CALL_INFO, (void *)adapter.GetParameter(), adapter.GetParameterLength());

    return 0;
}

/**
 * SetForbidFrequencyHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetForbidFrequencyHandler, MSG_MISC_OEM_SET_FORBID_FREQUENCY)

int SetForbidFrequencyHandler::OnRequest(Message *pMsg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(pMsg);

    IntsRequestData *rildata = (IntsRequestData *)pMsg->GetRequestData();
    int mode = rildata->GetInt(0);
    int frequency = rildata->GetInt(1);
    int forbiddenTimer = rildata->GetInt(2);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetForbidFrequency(mode, frequency, forbiddenTimer);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetForbidFrequencyHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ProtocolMiscSetForbidFrequencyAdapter)
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetForbidLteBandHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetForbidLteBandHandler, MSG_MISC_OEM_SET_FORBID_LTE_BAND)

int SetForbidLteBandHandler::OnRequest(Message *pMsg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(pMsg);

    IntsRequestData *rildata = (IntsRequestData *)pMsg->GetRequestData();
    int numBands = rildata->GetInt(0);
    int band[MAX_FORBID_LTE_BAND] = {0};
    if (numBands > 0 && numBands <= MAX_FORBID_LTE_BAND) {
        for (int i = 0; i < numBands; i ++) {
            band[i] = rildata->GetInt(i+1);
            RilLogV("band[%d]= %d", i, band[i]);
        }
    }

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetForbidLetBand(numBands, band);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetForbidLteBandHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ProtocolMiscSetForbidLteBandAdapter)
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetForbidLteTacHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetForbidLteTacHandler, MSG_MISC_OEM_SET_FORBID_LTE_TAC)

int SetForbidLteTacHandler::OnRequest(Message *pMsg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(pMsg);

    IntsRequestData *rildata = (IntsRequestData *)pMsg->GetRequestData();
    int mode = rildata->GetInt(0);
    int tac = rildata->GetInt(1);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetForbidLteTac(mode, tac);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetForbidLteTacHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ProtocolMiscSetForbidLteTacAdapter)
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetSelflogTcpDumpModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSelflogTcpDumpModeHandler, MSG_MISC_OEM_SET_SELFLOG_TCP_DUMP_MODE)

int SetSelflogTcpDumpModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int tcpDumpEnable = rildata->GetInt(0);
    int headerDumpEnable = rildata->GetInt(1);

    RilLogV("[%s] %s(), Model : %i, SwVer : %i", mServiceName, __FUNCTION__, tcpDumpEnable, headerDumpEnable);

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.BuildSetSelflogTcpDumpMode(tcpDumpEnable, headerDumpEnable);
    if (pModemData != NULL) {
        if (SendRequest(pModemData) < 0) {
            RilLogE("SendRequest error");
            delete pModemData;
            return -1;
        } else {
            delete pModemData;
        }
    }

    OnRequestComplete(RIL_E_SUCCESS);

    return 0;
}

int SetSelflogTcpDumpModeHandler::OnResponse(Message * /* msg */)
{
    return 0;
}

/**
 * SetAllDivSettingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetAllDivSettingHandler, MSG_MISC_OEM_SET_ALL_DIV_SETTING)

int SetAllDivSettingHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetAllDivSetting();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetAllDivSettingHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetBsrParaHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetBsrParaHandler, MSG_MISC_OEM_SET_BSR_PARA)

int SetBsrParaHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int switchStatus = rildata->GetInt(0);
    int packetPeriod = rildata->GetInt(1);
    int packetSize = rildata->GetInt(2);
    RilLogV("[%s] %s(), switchStatus = %d, packetPeriod = %d, packetSize = %d", mServiceName, __FUNCTION__, switchStatus, packetPeriod, packetSize);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetBsrPara(switchStatus, packetPeriod, packetSize);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetBsrParaHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetAheadRreHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetAheadRreHandler, MSG_MISC_OEM_SET_AHEAD_RRE)

int SetAheadRreHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int searchType = rildata->GetInt(0);
    int earfcn = rildata->GetInt(1);
    int globalCellId = rildata->GetInt(2);
    RilLogV("[%s] %s(), searchType = %d, earfcn = %d, globalCellId = %d", mServiceName, __FUNCTION__,
        searchType, earfcn, globalCellId);


    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetAheadRre(searchType, earfcn, globalCellId);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetAheadRreHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_GETRESULT_ON_RESPONSE(ProtocolMiscSetAheadRreAdapter)
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetDrdvHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetDrdvHandler, MSG_MISC_OEM_SET_DRDV)

int SetDrdvHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int flag = rildata->GetInt();

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetDrdv(flag);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetDrdvHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetEmcLabTestModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetEmcLabTestModeHandler, MSG_MISC_OEM_SET_EMC_LAB_TEST_MODE)

int SetEmcLabTestModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int value = rildata->GetInt();
    RilLogV("[%s] %s(), value = %d", mServiceName, __FUNCTION__, value);


    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetLabTestConfig(0x01, (BYTE)value);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    MiscServiceProxy misc(mService);
    if (misc.HasService()) {
        NetworkService* pNetworkService = misc.FindNetworkService();
        if (NULL != pNetworkService) {
            pNetworkService->SetEmcLabTestMode(value == 1);
            OnRequestComplete(RIL_E_SUCCESS);
        }
    } else {
        RilLogE("No network service");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetEmcLabTestModeHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * UpdateEmergencyNumberListHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UpdateEmergencyNumberListHandler, MSG_MISC_OEM_UPDATE_EMERGENCY_NUMBER_LIST)

int UpdateEmergencyNumberListHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    MiscServiceProxy misc(mService);
    if (misc.HasService()) {
        NetworkService* pNetworkService = misc.FindNetworkService();
        if (NULL != pNetworkService) {
            bool callByOpposite = false;
            RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
            if (NULL != rildata && rildata->GetSize() > 0) {
                callByOpposite = (*(BYTE *)rildata->GetRawData() == 1);
            }
            RilLogI("[%s] %s %s() callByOpposite:%d", mServiceName, TAG, __FUNCTION__, callByOpposite);

            if (!callByOpposite) {
                pNetworkService->ResetDbEccId(true /*forceToErase*/);
            }
            pNetworkService->UpdateDbEccInfo(true /* forceToUpdate */, callByOpposite);
            OnRequestComplete(RIL_E_SUCCESS);
        }
    } else {
        RilLogE("No network service");
        return -1;
    }

    return 0;
}

int UpdateEmergencyNumberListHandler::OnResponse(Message * /* msg */) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    return -1;
}

/**
 * SetForbidNrCellHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetForbidNrCellHandler, MSG_MISC_OEM_SET_FORBID_NR_CELL)

int SetForbidNrCellHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    void *data = rildata->GetRawData();
    unsigned int dataLen = rildata->GetSize();
    if (dataLen < sizeof(int)*3) {
        RilLogE("Data is invalid");
        return -1;
    }

    int mode = ((int *)data)[0];
    ULONG * cellId = (ULONG *) &(((int *)data)[1]);
    int  forbiddenTimer =((int *)data)[3];
    char plmn[MAX_PLMN_LEN + 1] = {0, };
    memcpy(plmn, (char *)((int *)data + 4), dataLen - sizeof(int)*2- sizeof(ULONG));
    RilLogI("mode=%d, cellId=%lld, forbiddenTimer=%d, plmn=%s", mode, *cellId, forbiddenTimer, plmn);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetForbidNrCell(mode, cellId, forbiddenTimer, plmn);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetForbidNrCellHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscSetForbidNrCellAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int result = adapter.GetResult();
        RilLogV("[%s] %s(), result = %d", mServiceName, __FUNCTION__, result);
        OnRequestComplete(errorCode, &result, sizeof(int));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * GetCdmaCapabilityInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetCdmaCapabilityInfoHandler, MSG_MISC_OEM_GET_CDMA_CAPABILITY_INFO)

int GetCdmaCapabilityInfoHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildGetCdmaCapabilityInfo();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int GetCdmaCapabilityInfoHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscGetCdmaCapabilityInfoAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        BYTE isCdmaSupported = adapter.GetCapability();
        RilLogV("[%s] %s(), isCdmaSupported = %d", mServiceName, __FUNCTION__, isCdmaSupported);

        OnRequestComplete(errorCode, &isCdmaSupported, sizeof(BYTE));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetSrsPwrLimitHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSrsPwrLimitHandler, MSG_MISC_OEM_SET_SRS_PWR_LIMIT)

int SetSrsPwrLimitHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("pMsg is NULL");
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    int level = rildata->GetInt();
    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetSrsPwrLimit(level);

    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int SetSrsPwrLimitHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("pMsg is NULL");
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

/**
 * SetTraceDumpCfgHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetTraceDumpCfgHandler, MSG_MISC_OEM_SET_TRACE_DUMP_CFG)

int SetTraceDumpCfgHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("pMsg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    const char *MiscData = (char *) rildata->GetRawData();
    BYTE config = MiscData[0];

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetTraceDumpCfg(config);

    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return -1;
    }

    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int SetTraceDumpCfgHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("pMsg is NULL");
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

/**
 * GetTraceDumpCfgHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetTraceDumpCfgHandler, MSG_MISC_OEM_GET_TRACE_DUMP_CFG)

int GetTraceDumpCfgHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildGetTraceDumpCfg();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int GetTraceDumpCfgHandler::OnResponse(Message *msg) {
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

    ProtocolMiscGetTraceDumpCfgAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        BYTE config = adapter.GetConfig();
        RilLogV("[%s] %s(), config = %d", mServiceName, __FUNCTION__, config);

        OnRequestComplete(errorCode, &config, sizeof(BYTE));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetUeOperationModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetUeOperationModeHandler, MSG_MISC_OEM_SET_UE_OPERATION_MODE)

int SetUeOperationModeHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int mode = rildata->GetInt();
    RilLogV("[%s] %s(), mode = %d", mServiceName, __FUNCTION__, mode);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetUeOperationMode(mode);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetUeOperationModeHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetUeOperationModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetUeOperationModeHandler, MSG_MISC_OEM_GET_UE_OPERATION_MODE)

int GetUeOperationModeHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildGetUeOperationMode();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int GetUeOperationModeHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ProtocolMiscGetUeOperationModeAdapter adapter(msg->GetModemData());
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        BYTE mode = adapter.GetMode();
        RilLogV("[%s] %s(), mode = %d", mServiceName, __FUNCTION__, mode);

        OnRequestComplete(errorCode, &mode, sizeof(BYTE));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetNrrrmMinReqEnableHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetNrrrmMinReqEnableHandler, MSG_MISC_OEM_SET_NRRRM_MIN_REQ_ENABLE)

int SetNrrrmMinReqEnableHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int enable = rildata->GetInt();
    RilLogV("[%s] %s(), enable = %d", mServiceName, __FUNCTION__, enable);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetNrrrmMinReqEnable(enable);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetNrrrmMinReqEnableHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetNrrrmMinReqEnableHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetNrrrmMinReqEnableHandler, MSG_MISC_OEM_GET_NRRRM_MIN_REQ_ENABLE)

int GetNrrrmMinReqEnableHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildGetNrrrmMinReqEnable();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int GetNrrrmMinReqEnableHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ProtocolMiscGetNrrrmMinReqEnableAdapter adapter(msg->GetModemData());
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        BYTE enable = adapter.GetEnable();
        RilLogV("[%s] %s(), mode = %d", mServiceName, __FUNCTION__, enable);

        OnRequestComplete(errorCode, &enable, sizeof(BYTE));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}


/**
 * SetPwrDetectThrHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetPwrDetectThrHandler, MSG_MISC_OEM_SET_PWR_DETECT_THR)

int SetPwrDetectThrHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int txPwrDetectThr = rildata->GetInt(0);
    int txPwrDifferenceThr = rildata->GetInt(1);
    RilLogV("[%s] %s(), txPwrDetectThr = %d, txPwrDifferenceThr = %d", mServiceName, __FUNCTION__, txPwrDetectThr, txPwrDifferenceThr);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetPwrDetectThr(txPwrDetectThr, txPwrDifferenceThr);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetPwrDetectThrHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()


/**
 * SetVcrtHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetVcrtHandler, MSG_MISC_OEM_SET_VCRT)

int SetVcrtHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int vcrtMode = rildata->GetInt();

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetVcrt(vcrtMode);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetVcrtHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SetEnableN28
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetEnableN28, MSG_MISC_OEM_SET_ENABLE_N28)

int SetEnableN28::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int enable_n28 = rildata->GetInt();
    RilLogV("[%s] %s(), enable_n28 = %d", mServiceName, __FUNCTION__, enable_n28);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetEnableN28(enable_n28);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int SetEnableN28::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ProtocolRespAdapter adapter(msg->GetModemData());
    OnRequestComplete(adapter.GetErrorCode());

    return 0;
}

/**
 * SetUlMimo
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetUlMimo, MSG_MISC_SET_UL_MIMO)

int SetUlMimo::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int disable_ul_mimo = rildata->GetInt();
    RilLogV("[%s] %s(), disable_ul_mimo = %d", mServiceName, __FUNCTION__, disable_ul_mimo);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetUlMimo(disable_ul_mimo);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int SetUlMimo::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_RSP(msg);

    ProtocolRespAdapter adapter(msg->GetModemData());
    OnRequestComplete(adapter.GetErrorCode());

    return 0;
}

/**
 * GetBinaryVersionInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetBinaryVersionInfoHandler, MSG_MISC_GET_BINARY_VERSION_INFO)

int GetBinaryVersionInfoHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildGetBinaryVersionInfo();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int GetBinaryVersionInfoHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ProtocolMiscGetBinaryVersionInfoAdapter adapter(msg->GetModemData());
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        MiscDataBuilder builder;
        int length = adapter.GetVersionInfoLength();
        const char *data = adapter.GetData();

        if (data != NULL) {
            RilLogV("[%d] BinaryVersionInfo:length=%d resp=%s", mService->GetRilSocketId(), length, data);
            OnRequestComplete(RIL_E_SUCCESS, (void *)data, length);
        } else {
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
    } else {
        OnRequestComplete(RIL_E_INTERNAL_ERR);
    }

    return 0;
}

/**
 * SetBtlBufMaxSizeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetBtlBufMaxSizeHandler, MSG_MISC_OEM_SET_BTL_BUFMAX_SIZE)

int SetBtlBufMaxSizeHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int size = rildata->GetInt();

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.BuildSetBtlBufMaxSize(size);
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetBtlBufMaxSizeHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()


/**
 * GetVcrtHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetVcrtHandler, MSG_MISC_OEM_GET_VCRT)

int GetVcrtHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildGetVcrt();
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

int GetVcrtHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ProtocolMiscGetVcrtAdapter adapter(msg->GetModemData());
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        BYTE vcrtMode = adapter.GetVcrtMode();
        RilLogV("[%s] %s(), vcrtMode = %d", mServiceName, __FUNCTION__, vcrtMode);
        OnRequestComplete(errorCode, &vcrtMode, sizeof(BYTE));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SetNrDisableBandHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetNrDisableBandHandler, MSG_MISC_SET_NR_DISABLE_BAND)

int SetNrDisableBandHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildSetNrDisableBand(rildata->GetRawData(), rildata->GetSize());
    if (SendRequest(pModemData, TIMEOUT_OEM_MODEM_INFO_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }

    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetNrDisableBandHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()
