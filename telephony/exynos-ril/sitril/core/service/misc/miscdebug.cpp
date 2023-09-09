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
 * miscdebug.cpp
 *
 *  Created on: 2020. 7. 8.
 *      Author: sungwoo48.choi
 */

#include "miscdebug.h"
#include <sstream>
#include "miscdatabuilder.h"
#include "protocoladapter.h"
#include "protocolmiscadapter.h"
#include "protocolmiscbuilder.h"
#include "protocolutils.h"
#include "requestdata.h"
#include "rillog.h"
#include "service.h"
#include "telephonystatuslogging.h"
#include <librilutils/systemproperty.h>
#include "sitdef.h"
#include <librilutils/textutils.h>
#include <unistd.h>

#define TIMEOUT_MISCDEBUG_DEFAULT   5000
#define TIMEOUT_ENG_MODE            5000
#define TIMEOUT_DEBUG_TRACE         5000

#define CHECK_NULL_MSG(msg) { if(msg==NULL) { \
                                  RilLogE("[%s] %s %s() msg is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_REQ(msg) { if(msg==NULL || msg->GetRequestData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or RequestData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }
#define CHECK_NULL_RSP(msg) { if(msg==NULL || msg->GetModemData()==NULL) {\
                                  RilLogE("[%s] %s %s() msg or ModemData is NULL", mServiceName, TAG, __FUNCTION__); return -1; } }

/**
 * SetEngModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetEngModeHandler, MSG_MISC_OEM_SET_ENG_MODE)

int SetEngModeHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    const char *MiscData = (char *) rildata->GetRawData();
    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData;

    const BYTE Monitoring = 0x02;
    const BYTE DefaultSubMode = 0x00;

    if (rildata->GetSize() == 1) {
        if (MiscData[0] == Monitoring) {
            pModemData = builder.SetEngMode(MiscData[0], DefaultSubMode);
        } else {
            pModemData = builder.SetEngMode(MiscData[0]);
        }

        RilLogV("engMode= %d", MiscData[0]);
    } else {
        pModemData = builder.SetEngMode(MiscData[0], MiscData[1]);
        RilLogV("engMode= %d, subMode = %d", MiscData[0], MiscData[1]);
    }

    if (SendRequest(pModemData, TIMEOUT_ENG_MODE, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetEngModeHandler::OnResponse(Message *msg)
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
 * SetScreenLineHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetScreenLineHandler, MSG_MISC_OEM_SET_SCREEN_LINE)

int SetScreenLineHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    const char *MiscData = (char *) rildata->GetRawData();
    BYTE scrLine = MiscData[0];

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.SetScrLine(scrLine);
    if (SendRequest(pModemData, TIMEOUT_ENG_MODE, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SetScreenLineHandler::OnResponse(Message *msg)
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
 * SetEngModeHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetEngStringInputHandler, MSG_MISC_OEM_SET_ENG_STRING_INPUT)

int SetEngStringInputHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    int len = rildata->GetSize();
    if (rildata->GetRawData() != NULL && len <= 255) {
        ProtocolMiscDebugBuilder builder;
        ModemData *pModemData = builder.SetEngStringInput(len, (char *) rildata->GetRawData());
        if (SendRequest(pModemData, TIMEOUT_ENG_MODE, mMessageId) < 0) {
            RilLogE("SendRequest error");
            return -1;
        }
    } else {
        RilLogE("Invalid input data");
        return -1;
    }
    return 0;
}

int SetEngStringInputHandler::OnResponse(Message *msg)
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
 * EngModeDisplayIndHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(EngModeDisplayIndHandler, MSG_MISC_UNSOL_OEM_DISPLAY_ENG)

int EngModeDisplayIndHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ProtocolMiscOemDisplayEngAdapter adapter(msg->GetModemData());
    MiscDataBuilder builder;
    const RilData *rildata = builder.BuildDisplayEngIndication(adapter.GetParameter(), adapter.GetParameterLength());
    if (rildata != NULL){
        OnUnsolicitedResponse(RIL_UNSOL_OEM_DISPLAY_ENG_MODE, rildata->GetData(), rildata->GetDataLength());
        delete rildata;
    }
    return 0;
}

/**
 * DebugTraceHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DebugTraceHandler, MSG_MISC_OEM_SET_DEBUG_TRACE)

int DebugTraceHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    const char *MiscData = (char *) rildata->GetRawData();
    BYTE debugTrace = MiscData[0];

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.SetDebugTrace(debugTrace);
    if (SendRequest(pModemData, TIMEOUT_DEBUG_TRACE, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int DebugTraceHandler::OnResponse(Message *msg)
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
 * SendATCommandHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SendATCommandHandler, MSG_MISC_FORWARDING_AT_COMMAND)

int SendATCommandHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata->GetSize() <= 0) {
        RilLogV(" [AT] > no data");
        return -1;
    }

    const int MAX_COMMAND_LENGTH = 1000;
    char command[MAX_COMMAND_LENGTH + 1] = {0, };
    int datalen = rildata->GetSize() > MAX_COMMAND_LENGTH ? MAX_COMMAND_LENGTH : rildata->GetSize();
    memcpy(command, rildata->GetRawData(), datalen);
    command[datalen] = 0;
    RilLogV(" [AT] > %s", command);

    ProtocolMiscDebugBuilder builder;
    ModemData *pModemData = builder.BuildATCommand(command);
    if (SendRequest(pModemData, TIMEOUT_MISCDEBUG_DEFAULT, mMessageId) < 0) {
        RilLogE("SendRequest error");
        return -1;
    }
    return 0;
}

int SendATCommandHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS);
    }
    else {
        OnRequestComplete(RIL_E_MODEM_ERR);
        char err[] = "\r\nERROR\r\n";
        RilLogV(" [AT] < %s (by RIL)", err);
        MiscDataBuilder builder;
        const RilData *rildata = builder.BuildATCommand(err);
        if (rildata != NULL){
            OnUnsolicitedResponse(RIL_UNSOL_OEM_FORWARDING_AT_COMMAND, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }

    return 0;
}

/**
 * UnsolATCommandHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(UnsolATCommandHandler, MSG_MISC_UNSOL_AT_COMMAND)

int UnsolATCommandHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ModemData *pModemData = msg->GetModemData();
    ProtocolMiscATCommandAdapter adapter(pModemData);
    const char *command = adapter.GetCommand();
    if (!TextUtils::IsEmpty(command)) {
        RilLogV(" [AT] < %s", command);
        MiscDataBuilder builder;
        const RilData *rildata = builder.BuildATCommand(command);
        if (rildata != NULL){
            OnUnsolicitedResponse(RIL_UNSOL_OEM_FORWARDING_AT_COMMAND, rildata->GetData(), rildata->GetDataLength());
            delete rildata;
        }
    }
    else {
        RilLogW(" [AT] < received but no data");
    }

    return 0;
}

/**
 * PhoneResetHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(PhoneResetHandler, MSG_MISC_PHONE_RESET)

int PhoneResetHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    ProtocolMiscPhoneResetAdapter adapter(msg->GetModemData());
    int reset_type = adapter.GetResetType() & 0xFF;
    int reset_cause = adapter.GetResetCause() & 0xFF;

    RilLogV("[%s] %s() : reset type:%d reset cause:%d", mServiceName, __FUNCTION__, reset_type, reset_cause);
    switch(reset_type)
    {
    case 0x01: // reset phone only
        RilLogW("[%s] %s() : @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", mServiceName, __FUNCTION__);
        RilLogW("[%s] %s() : Modem reset", mServiceName, __FUNCTION__);
        RilLogW("[%s] %s() : @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", mServiceName, __FUNCTION__);
        WaitForTimeUs(__FUNCTION__, 3'000'000U);
        mRilContext->ResetModem("SILENT_RESET_REQUESTED_BY_MODEM");
        break;
    case 0x02: // reboot (reset both phone and modem)
        RilLogW("[%s] %s() : Phone Reboot", mServiceName, __FUNCTION__);
        WaitForTimeUs(__FUNCTION__, 3'000'000U);
        RequestDeviceReset(0x02);
        break;
    case 0x03: // shutdown
        RilLogW("[%s] %s() : Phone Shutdown", mServiceName, __FUNCTION__);
        WaitForTimeUs(__FUNCTION__, 3'000'000U);
        RequestDeviceReset(0x03);
        break;
    default:
        break;
    }

    return 0;
}

void PhoneResetHandler::RequestDeviceReset(int reset_type)
{
    stringstream ss;
    ss << "broadcast -a com.samsung.slsi.action.POWER -n com.samsung.slsi.telephony.testmode/.TestModeReceiver";
    ss << " --ei reset_type ";
    ss << reset_type;
    string intent = ss.str();

    SystemProperty::Set(VENDOR_RIL_RESET_TYPE, reset_type);
    RilLogV("send device reset request, (reset type=%d)", reset_type);
    OnUnsolicitedResponse(RIL_UNSOL_OEM_AM, intent.c_str(), intent.length());
}

/**
 * DummyRequestHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DummyRequestHandler, MSG_MISC_SET_TTY)

int DummyRequestHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    RilLogV("[%s] request id=%d OnRequestComplete(RIL_E_SUCCESS)", GetName(), msg->GetMsgId());
    OnRequestComplete(RIL_E_SUCCESS);
    return 0;
}

int DummyRequestHandler::OnResponse(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return -1;
}

/**
 * TestUnsolHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(TestUnsolHandler, MSG_MISC_OEM_TEST_UNSOL)

int TestUnsolHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    void *rawData = rildata->GetRawData();
    int rawDataSize = rildata->GetSize();
    if (rawDataSize < (int)sizeof(RCM_IND_HEADER) || rawData == NULL) {
        RilLogE("rawData is invalid");
        return -1;
    }

    RCM_IND_HEADER  *rcm_hdr = (RCM_IND_HEADER *) rawData;
    char headerInfo[256];
    snprintf(headerInfo, sizeof(headerInfo), "[app/rildelivery%d][RX] %s, IND, %d bytes",
            mRilContext->GetRilSocketId(), ProtocolUtils::rcmMsgToString(rcm_hdr->id), rcm_hdr->length);

    int maxStartLogIdx = 1024 - 6;
    char rawDataLog[1024];
    for (int raw_idx = 0, log_idx = 0; raw_idx < rawDataSize && log_idx <= maxStartLogIdx; raw_idx++) {
        log_idx += snprintf(rawDataLog + log_idx, 4, " %02x", ((char *)rawData)[raw_idx]);

        if ((raw_idx + 1) % 16 == 0)
            log_idx += snprintf(rawDataLog + log_idx, 2, "\n");
        else if ((raw_idx + 1) % 8 == 0)
            log_idx += snprintf(rawDataLog + log_idx, 3, "  ");
    }

    RilLogI("%s", headerInfo);
    RilLogI("%s", rawDataLog);

    int error;
    if (!strcmp(ProtocolUtils::rcmMsgToString(rcm_hdr->id), "<unknown sit>")) {
        error = RIL_E_OEM_ERROR_1;  //  unknown sit error
    } else {
        error = RIL_E_SUCCESS;
        mRilContext->ProcessModemData(rawData, rawDataSize);
    }

    OnRequestComplete(RIL_E_SUCCESS, &error, sizeof(int));
    return 0;
}

int TestUnsolHandler::OnResponse(Message * /* msg */) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return -1;
}

/**
 * SetTelephonyStatusLoggingHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetTelephonyStatusLoggingHandler, MSG_MISC_OEM_SET_TELEPHONY_STATUS_LOGGING)

int SetTelephonyStatusLoggingHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_REQ(msg);

    IntsRequestData *rildata = (IntsRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    const int INDEX_ONOFF = 0;
    const int INDEX_DUMPALL = 1;
    const int INDEX_INTERVAL = 2;

    bool currentOn = (SystemProperty::Get(PROPERTY_TELEPHONY_STATUS_LOGGING_ONOFF) == "On");
    bool requestOn = (rildata->GetInt(INDEX_ONOFF) == 1) ? true : false;
    bool dumpAll = (rildata->GetInt(INDEX_DUMPALL) == 1) ? true : false;
    int interval = rildata->GetInt(INDEX_INTERVAL);

    RilLogI("[%s] currentOn = %d", mServiceName, currentOn);
    RilLogI("[%s] requestOn = %d", mServiceName, requestOn);

    int error = RIL_E_SUCCESS;
    if (currentOn == requestOn) {
        error = RIL_E_OEM_ERROR_1; /*duplicated request*/
    } else {
        TelephonyStatusLogging *logging = TelephonyStatusLogging::GetInstance();
        if (logging == nullptr) {
            error = RIL_E_OEM_ERROR_2; /*unsupported build mode*/
        } else {
            if (requestOn) {
                if (!logging->Start(dumpAll, interval))
                    error = RIL_E_OEM_ERROR_3; /*turn on failure*/
            } else {
                if (!logging->Stop())
                    error = RIL_E_OEM_ERROR_4; /*turn off failure*/
            }
        }
    }

    OnRequestComplete(RIL_E_SUCCESS, &error, sizeof(int));
    return 0;
}

int SetTelephonyStatusLoggingHandler::OnResponse(Message * /* msg */)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    return -1;
}
