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
 * oem_miscnetwork.cpp
 *
 *  Created on: 2020. 7. 14.
 *      Author: sungwoo48.choi
 */

#include "oem_miscnetwork.h"
#include "protocoladapter.h"
#include "protocolnetadapter.h"
#include "protocolnetbuilder.h"
#include "rilapplication.h"
#include "rillog.h"
#include "message.h"

#define TIMEOUT_OEM_MISC_NET_DEFAULT    30000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * GetManualRatModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetManualRatModeHandler, MSG_NET_OEM_GET_MANUAL_RAT_MODE)

int GetManualRatModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildGetManualRatMode();
    if (SendRequest(pModemData, TIMEOUT_OEM_MISC_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetManualRatModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetGetManualRatModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int retValue[2] = {0, 0};
        adapter.GetManualRatMode(retValue);
        RilLogV("[%s] %s() mode=%d, rat=%d", mServiceName, __FUNCTION__, retValue[0], retValue[1]);
        OnRequestComplete(RIL_E_SUCCESS, retValue, sizeof(int)*2);
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetManualRatModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetManualRatModeHandler, MSG_NET_OEM_SET_MANUAL_RAT_MODE)

int SetManualRatModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int mode = rildata->GetInt(0);
    int rat = rildata->GetInt(1);
    RilLogV("[%s] %s() mode=%d, rat=%d", mServiceName, __FUNCTION__, mode, rat);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetManualRatMode(mode, rat);
    if (SendRequest(pModemData, TIMEOUT_OEM_MISC_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetManualRatModeHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetSetManualRatModeAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int cause = adapter.GetCause();
        RilLogV("[%s] %s() cause=%d", mServiceName, __FUNCTION__, cause);
        OnRequestComplete(RIL_E_SUCCESS, &cause, sizeof(int));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * GetFrequencyLockHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetFrequencyLockHandler, MSG_NET_OEM_GET_FREQUENCY_LOCK)

int GetFrequencyLockHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildGetFrequencyLock();
    if (SendRequest(pModemData, TIMEOUT_OEM_MISC_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetFrequencyLockHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetGetFreqLockAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int retValue[7] = {0, 0, 0, 0, 0, 0, 0};
        adapter.GetFrequencyLock(retValue);
        RilLogV("[%s] %s() mode=%d, rat=%d, ltePci=%d, lteEarFcn=%d, gsmArfcn=%d, wcdmaPsc=%d, wcdmaUarfcn=%d",
                mServiceName, __FUNCTION__, retValue[0], retValue[1], retValue[2], retValue[3], retValue[4], retValue[5], retValue[6]);
        OnRequestComplete(RIL_E_SUCCESS, retValue, sizeof(int)*7);
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetFrequencyLockHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetFrequencyLockHandler, MSG_NET_OEM_SET_FREQUENCY_LOCK)

int SetFrequencyLockHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    int mode = rildata->GetInt(0);
    int rat = rildata->GetInt(1);
    int ltePci = rildata->GetInt(2);
    int lteEarfcn = rildata->GetInt(3);
    int gsmArfcn = rildata->GetInt(4);
    int wcdamPsc = rildata->GetInt(5);
    int wcdamUarfcn = rildata->GetInt(6);
    RilLogV("[%s] %s() mode=%d, rat=%d, ltePci=%d, lteEarfcn=%d, gsmArfcn=%d, wcdamPsc=%d, wcdamUarfcn=%d",
            mServiceName, __FUNCTION__, mode, rat, ltePci, lteEarfcn, gsmArfcn, wcdamPsc, wcdamUarfcn);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildSetFrequencyLock(mode, rat, ltePci, lteEarfcn, gsmArfcn, wcdamPsc, wcdamUarfcn);
    if (SendRequest(pModemData, TIMEOUT_OEM_MISC_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int SetFrequencyLockHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetSetFreqLockAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int result = adapter.GetResult();
        RilLogV("[%s] %s() result=%d", mServiceName, __FUNCTION__, result);
        OnRequestComplete(RIL_E_SUCCESS, &result, sizeof(int));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * GetFrequencyInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetFrequencyInfoHandler, MSG_NET_GET_FREQUENCY_INFO)

int GetFrequencyInfoHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    ProtocolNetworkModeBuilder builder;
    ModemData *pModemData = builder.BuildGetFrequencyInfo();
    if (SendRequest(pModemData, TIMEOUT_OEM_MISC_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int GetFrequencyInfoHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetGetFrequencyInfoAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int primaryRat = adapter.GetPrimaryRat();
        int primaryBand = adapter.GetPrimaryBand();
        int primaryFrequency = adapter.GetPrimaryFrequency();
        int secondaryRat = adapter.GetSecondaryRat();
        int secondaryBand = adapter.GetSecondaryBand();
        int secondaryFrequency = adapter.GetSecondaryFrequency();

        int response[] = {primaryRat, primaryBand, primaryFrequency, secondaryRat, secondaryBand, secondaryFrequency};
        OnRequestComplete(RIL_E_SUCCESS, response, sizeof(response));
    }
    else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * UnsolFrequencyInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolFrequencyInfoHandler, MSG_NET_IND_FREQUENCY_INFO)

int UnsolFrequencyInfoHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetworkFrequencyInfoIndAdapter adapter(pModemData);
    int primaryRat = adapter.GetPrimaryRat();
    int primaryBand = adapter.GetPrimaryBand();
    int primaryFrequency = adapter.GetPrimaryFrequency();
    int secondaryRat = adapter.GetSecondaryRat();
    int secondaryBand = adapter.GetSecondaryBand();
    int secondaryFrequency = adapter.GetSecondaryFrequency();

    int response[] = {primaryRat, primaryBand, primaryFrequency, secondaryRat, secondaryBand, secondaryFrequency};
    OnUnsolicitedResponse(RIL_UNSOL_OEM_FREQUENCY_INFO, response, sizeof(response));

    return 0;
}

/**
 * B2B1ConfigInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(B2B1ConfigInfoHandler, MSG_NET_IND_B2_B1_CONFIG)

int B2B1ConfigInfoHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetB1B2ConfigIndAdapter adapter(pModemData);
    RilLogI("getParameter=(%s) getParameterLength= %d()", adapter.GetParameter(), adapter.GetParameterLength());
    OnUnsolicitedResponse(RIL_UNSOL_B2_B1_CONFIG_INFO,(void *)adapter.GetParameter(), (int)adapter.GetParameterLength());

    return 0;
}

/**
 * AcBarringInfoHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(AcBarringInfoHandler, MSG_NET_IND_AC_BARRING_INFO)

int AcBarringInfoHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolNetAcBarringInfo adapter(pModemData);
    char acBarringInfo[5] = {0, 100, 100, 100, 100};
    adapter.GetAcBarringInfo(acBarringInfo, sizeof(char)*5);
    RilLogV("[%s] %s() RIL_UNSOL_OEM_AIMS_AC_BARRING_INFO forEmc(%d), forMoSig(%d), forMoData(%d), forMmtelVoice(%d), forMmtelVide(%d)",
            mServiceName, __FUNCTION__, acBarringInfo[0], acBarringInfo[1], acBarringInfo[2], acBarringInfo[3], acBarringInfo[4]);
    OnUnsolicitedResponse(RIL_UNSOL_OEM_AIMS_AC_BARRING_INFO, acBarringInfo, sizeof(char)*5);

    return 0;
}
