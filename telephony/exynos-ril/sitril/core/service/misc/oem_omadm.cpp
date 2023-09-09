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
 * oem_omadm.cpp
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */
#include "oem_omadm.h"
#include <string>
#include "protocoladapter.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "rillog.h"
#include "service.h"
#include "ts25table.h"
#include "sitdef.h"
#include <librilutils/textutils.h>

#define TIMEOUT_OMADM_DEFAULT       50000
#define TIMEOUT_GET_RADIO_NODE      50000
#define TIMEOUT_SET_RADIO_NODE      50000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * GetRadioNodeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetRadioNodeHandler, MSG_MISC_OEM_GET_RADIO_NODE)

int GetRadioNodeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *) msg->GetRequestData();
    if (rildata->GetSize() != MAX_RADIO_NODE_DATA_LEN) {
        RilLogE("Invalid data size");
        return -1;
    }

    const char *path = (const char*)rildata->GetRawData();

    ProtocolOemOmaDmBuilder builder;
    ModemData *pModemData = builder.BuildGetRadioNode(path);
    if (SendRequest(pModemData, TIMEOUT_GET_RADIO_NODE, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetRadioNodeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscGetRadioNodeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        char value[MAX_RADIO_NODE_DATA_LEN] = {0, };
        strncpy(value, adapter.GetValue(), MAX_RADIO_NODE_DATA_LEN-1);
        OnRequestComplete(errorCode, value, sizeof(value));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetRadioNodeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetRadioNodeHandler, MSG_MISC_OEM_SET_RADIO_NODE)

int SetRadioNodeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata->GetSize()/MAX_RADIO_NODE_DATA_LEN != 2) {
        RilLogE("Invalid data size");
        return -1;
    }

    const char *path = (const char*)rildata->GetRawData();
    const char *value = (const char*)rildata->GetRawData() + MAX_RADIO_NODE_DATA_LEN;

    ProtocolOemOmaDmBuilder builder;
    ModemData *pModemData = builder.BuildSetRadioNode(path, value);
    if (SendRequest(pModemData, TIMEOUT_SET_RADIO_NODE, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetRadioNodeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);

    return 0;
}

/**
 * GetProvisionUpdateRequestHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetProvisionUpdateRequestHandler, MSG_MISC_OEM_GET_PROVISION_UPDATE_REQUEST)

int GetProvisionUpdateRequestHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolOemOmaDmBuilder builder;
    ModemData *pModemData = builder.BuildGetVoLteProvisionUpdate();
    if (SendRequest(pModemData, TIMEOUT_OMADM_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int GetProvisionUpdateRequestHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscGetVoLteProvisionUpdateAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int status = adapter.GetStatus();
        if (status == 0) {
            errorCode = RIL_E_GENERIC_FAILURE;
        }
    }
    OnRequestComplete(errorCode);
    return 0;
}

/**
 * SetProvisionUpdateDoneRequestHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetProvisionUpdateDoneRequestHandler, MSG_MISC_OEM_SET_PROVISION_UPDATE_DONE_REQUEST)

int SetProvisionUpdateDoneRequestHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolOemOmaDmBuilder builder;
    ModemData *pModemData = builder.BuildSetVoLteProvisionUpdate();
    if (SendRequest(pModemData, TIMEOUT_OMADM_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetProvisionUpdateDoneRequestHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscSetVoLteProvisionUpdateAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int result = adapter.GetResult();
        if (result == 0) {
            errorCode = RIL_E_GENERIC_FAILURE;
        }
    }
    OnRequestComplete(errorCode);
    return 0;
}

/**
 * RadioConfigResetHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(RadioConfigResetHandler, MSG_MISC_OEM_RADIO_CONFIG_RESET)

int RadioConfigResetHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    int type = rildata->GetInt();
    RilLogV("[%s] %s(), reset type = %d", mServiceName, __FUNCTION__, type);

    ProtocolMiscBuilder builder;
    ModemData *pModemData = builder.BuildRadioConfigReset(type);
    if (SendRequest(pModemData, TIMEOUT_OMADM_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int RadioConfigResetHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    OnRequestComplete(errorCode);
    return 0;
}

/**
 * GetPlmnNameFromSE13TableHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetPlmnNameFromSE13TableHandler, MSG_MISC_GET_PLMN_NAME_FROM_SE13TABLE)

static string GetNetworkName(int mcc, int mnc)
{
    TS25Record record;
    TS25Table *table = TS25Table::GetInstance();
    if (table != NULL) {
        record = table->GetRecord(mcc, mnc);
        if (TextUtils::IsEmpty(record.networkName)) {
            RilLogW("not found matched information for %d/%d", mcc, mnc);
        }
    }

    bool isUseLongName = TS25Record::isUsingLongNameOfT32Table(mcc, mnc);
    string ret;
    if (isUseLongName == true) ret = record.ppcin;
    else ret = record.networkName;

    return ret;
}

int GetPlmnNameFromSE13TableHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    // *(int *)data[0] - mcc
    // *(int *)data[1] - mnc
    if (rildata->GetSize() < (int)sizeof(short) * 2) {
        RilLogW("Invalid parameters");
        return -1;
    }

    int mcc = ((short *)rildata->GetRawData())[0] & 0xFFFF;
    int mnc = ((short *)rildata->GetRawData())[1] & 0xFFFF;
    char alphaEons[MAX_FULL_NAME_LEN];
    memset(alphaEons, 0, sizeof(alphaEons));
    string ret = GetNetworkName(mcc, mnc);
    if (!TextUtils::IsEmpty(ret)) {
        strncpy(alphaEons, ret.c_str(), MAX_FULL_NAME_LEN - 1);
        RilLogV("%d/%d %s", mcc, mnc, ret.c_str());
    }
    OnRequestComplete(RIL_E_SUCCESS, alphaEons, strlen(alphaEons)+1);
    return 0;
}

int GetPlmnNameFromSE13TableHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return ServiceHandlerBase::OnResponse(msg);
}

/**
 * Ts25TableDumpHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(Ts25TableDumpHandler, MSG_MISC_TS25TABLE_DUMP)

int Ts25TableDumpHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    TS25Table *ts25table = TS25Table::GetInstance();
    if (ts25table != NULL) {
        ts25table->Dump();
    }

    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

int Ts25TableDumpHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return ServiceHandlerBase::OnResponse(msg);
}

/**
 * MslCodeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(MslCodeHandler, MSG_MISC_OEM_GET_MSL_CODE)

int MslCodeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolOemOmaDmBuilder builder;
    ModemData *pModemData = builder.GetMslCode();
    if (SendRequest(pModemData, TIMEOUT_OMADM_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int MslCodeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscGetMslCodeAdapter adapter(pModemData);

    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        const char *mslCode = adapter.getMslCode();
        OnRequestComplete(RIL_E_SUCCESS, (void *)mslCode, 6); //length = 6;
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}
