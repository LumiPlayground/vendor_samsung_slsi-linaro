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
 * smshandler.cpp
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#include "smshandler.h"
#include "protocoladapter.h"
#include "protocolsmsadapter.h"
#include "protocolsmsbuilder.h"
#include "rillog.h"
#include "service.h"
#include "smsserviceproxy.h"
#include "sms_util.h"
#include "smsdata.h"
#include "cdmasmsdata.h"
#include "util.h"
#include <librilutils/systemproperty.h>

extern BYTE g_nLastTpidNewSms;

#define TIMEOUT_SMS_DEFAULT     10000
#define TIMEOUT_SMS_SEND        200000

#undef RilLogI
#define RilLogI(format, ...) \
        CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) \
        CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) \
        CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) \
        CRilLog::Log(RIL_LOG_CAT_SMS, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * SendSmsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SendSmsHandler, MSG_SMS_SEND)

int SendSmsHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    GsmSmsMessage *rildata = (GsmSmsMessage *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    const char *smsc;
    BYTE smscLen;

    SmsServiceProxy serviceProxy(mService);

    if (rildata->mSmsc->UseRilSmsc() && serviceProxy.GetSmscAddressLength() != 0) {
        smsc = serviceProxy.GetSmscAddress();
        smscLen = serviceProxy.GetSmscAddressLength();
    }
    else {
        smsc = rildata->mSmsc->mAddr;
        smscLen = rildata->mSmsc->mLen;
    }

    const char *pdu = rildata->mPdu->mData;
    int pduSize = rildata->mPdu->mLen;

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSendSms(smsc, smscLen, pdu, pduSize, false);

    if (SendRequest(pModemData, TIMEOUT_SMS_SEND, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

template <typename T>
int SendSmsHandlerCommon(Message *msg, const char *mServiceName, T *handler) {
    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolSendSmsRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int tp_errcause = adapter.GetSmsRspErrorCode();
    int messageRef = adapter.GetRef();

    RIL_SMS_Response resp;
    resp.ackPDU = NULL;
    resp.messageRef = messageRef;
    resp.errorCode = tp_errcause;

    RilLogV("RilErrorCode(%d) ProtocolErrorCode(%d)", errorCode, tp_errcause);
    if (errorCode == RIL_E_SUCCESS) {
        if(tp_errcause == 0) {
            handler->OnRequestComplete(RIL_E_SUCCESS, &resp, sizeof(RIL_SMS_Response));
        }
        else {
            if (tp_errcause == RP_CAUSE_CONGESTION) {
                handler->OnRequestComplete(RIL_E_SMS_SEND_FAIL_RETRY, &resp, sizeof(RIL_SMS_Response));
            }
            else {
                handler->OnRequestComplete(RIL_E_GENERIC_FAILURE, &resp, sizeof(RIL_SMS_Response));
            }
        }
    }
    else if (errorCode == RIL_E_SMS_SEND_FAIL_RETRY) {
        handler->OnRequestComplete(RIL_E_SMS_SEND_FAIL_RETRY, &resp, sizeof(RIL_SMS_Response));
    }
    else {
        handler->OnRequestComplete(errorCode, NULL, 0);
    }
    return 0;
}

int SendSmsHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    int ret = SendSmsHandlerCommon(msg, mServiceName, this);
    if (ret < 0)
        return ret;
    return 0;
}

/**
 * SendSmsMoreHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SendSmsMoreHandler, MSG_SMS_SEND_MORE)

int SendSmsMoreHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    GsmSmsMessage *rildata = (GsmSmsMessage *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    const char *smsc;
    BYTE smscLen;

    SmsServiceProxy serviceProxy(mService);

    if (rildata->mSmsc->UseRilSmsc() && serviceProxy.GetSmscAddressLength() != 0) {
        smsc = serviceProxy.GetSmscAddress();
        smscLen = serviceProxy.GetSmscAddressLength();
    }
    else {
        smsc = rildata->mSmsc->mAddr;
        smscLen = rildata->mSmsc->mLen;
    }

    const char *pdu = rildata->mPdu->mData;
    int pduSize = rildata->mPdu->mLen;

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSendSms(smsc, smscLen, pdu, pduSize, true);

    if (SendRequest(pModemData, TIMEOUT_SMS_SEND, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

int SendSmsMoreHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    int ret = SendSmsHandlerCommon(msg, mServiceName, this);
    if (ret < 0)
        return ret;
    return 0;
}

/**
 * WriteSmsToSimHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(WriteSmsToSimHandler, MSG_SMS_WRITE_SMS_TO_SIM)

int WriteSmsToSimHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    SimSmsMessage* data= (SimSmsMessage *)msg->GetRequestData();
    if (data == NULL) {
        RilLogE("[%s] data is NULL", mServiceName);
        return -1;
    }

    int pduSize = data->GetLength();
    char *pdu = NULL;
    pdu = new char[pduSize];
    memcpy(&pdu[0], data->GetRawByte(), pduSize);

    if (SMS_DBG) {
        char *pdu_tmp = NULL;
        pdu_tmp = new char[(pduSize*2)+1];
        Value2HexString(pdu_tmp, (BYTE *)pdu, pduSize);
        RilLogV("[8] PDU (ALL) = %s, pdusize=%d", pdu_tmp, pduSize);
        delete [] pdu_tmp;
    }

    int status = data->mStatus;
    int index = data->mSimIndex;

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildWriteSmsToSim(status, index, pduSize, pdu);

    delete [] pdu;

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

int WriteSmsToSimHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolWriteSmsToSimRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    int index = adapter.GetIndex();

    SmsServiceProxy serviceProxy(mService);

    RilLogV("RilErrorCode(%d)", errorCode);
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(errorCode, &index, sizeof(index));
    }
    else {
        OnRequestComplete(errorCode);
    }

    if (serviceProxy.GetSmsClass() == MESSAGE_CLASS_2) {
        if (errorCode == RIL_E_SUCCESS) {
            // RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM issuing to F/W
            OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM, &index, sizeof(index));
        } else {
            // SEND_SMS_ACK issuing to Network
            int data[2];
            data[0] = FAILED_RECEIPT;
            data[1] = TP_CAUSE_UNSPECIFIED_ERROR;
            RequestData *req = RilParser::CreateSmsAck(
                    RIL_REQUEST_SMS_ACKNOWLEDGE, 0, (char *)(&data[0]), (sizeof(data[0])*2));
            if (req != NULL) {
                Message *msg =
                        Message::ObtainMessage(req, RIL_SERVICE_MISC, MSG_MISC_SMS_ACKNOWLEDGE);
                if (mService->GetRilContext()->GetServiceManager()->SendMessage(msg) < 0) {
                    delete msg;
                }
            }
        }
        // Reset the class variable so that next write to sim operation will not be effected
        serviceProxy.SetSmsClass(MESSAGE_CLASS_1);
    }
    return 0;
}

/**
 * DeleteSmsOnSimHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DeleteSmsOnSimHandler, MSG_SMS_DELETE_SMS_ON_SIM)

int DeleteSmsOnSimHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    IntRequestData* data= (IntRequestData *)msg->GetRequestData();
    int index = data->GetInt();

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildDeleteSmsOnSim(index);

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(DeleteSmsOnSimHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetBcstSmsConfigHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetBcstSmsConfigHandler, MSG_SMS_GET_BROADCAST_SMS_CONFIG)

int GetBcstSmsConfigHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildGetBroadcastSmsConfig();

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

int GetBcstSmsConfigHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolGetBcstSmsConfRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    RilLogV("RilErrorCode(%d)", errorCode);
    if (errorCode == RIL_E_SUCCESS) {
        OnRequestComplete(RIL_E_SUCCESS, adapter.GetConfigsInfoPointers(),
                adapter.GetConfigsNumber() * sizeof(RIL_GSM_BroadcastSmsConfigInfo *));
    } else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetBcstSmsConfigHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetBcstSmsConfigHandler, MSG_SMS_SET_BROADCAST_SMS_CONFIG)

int SetBcstSmsConfigHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    BroadcastSmsConfigsRequestData *rildata =
            (BroadcastSmsConfigsRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSetBroadcastSmsConfig(rildata->GetConfigsInfo(),
            rildata->GetConfigsNumber());

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SetBcstSmsConfigHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * BcstActivationHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(BcstActivationHandler, MSG_SMS_BROADCAST_ACTIVATION)

int BcstActivationHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    IntRequestData* rildata= (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    int act = rildata->GetInt();
    if (act != 0/*SMS_BCST_ACT_ACTIVATE*/ && act != 1/*SMS_BCST_ACT_DEACTIVATE*/) {
        RilLogE("[%s] Undefined activation code(%d)!!!", mServiceName, act);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSmsBroadcastActivation(act);

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

int BcstActivationHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    RilLogV("RilErrorCode(%d)", errorCode);
    OnRequestComplete(adapter.GetErrorCode());

    return 0;
}

/**
 * GetSmscAddressHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetSmscAddressHandler, MSG_SMS_GET_SMSC_ADDRESS)

int GetSmscAddressHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if(msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSmscAddress();

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

int GetSmscAddressHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if ((pModemData == NULL) || (pModemData->GetRawData() == NULL)) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolSmscAddrRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    RilLogV("RilErrorCode(%d)", errorCode);
    if (errorCode == RIL_E_SUCCESS) {
        const char *smsc = adapter.GetSmsc();
        int smscLength = adapter.GetSmscLength();
        char sca[MAX_GSM_SMS_SERVICE_CENTER_ADDR * 2] = {0, };
        int scaSize = (int)(sizeof(sca)/sizeof(sca[0]));
        int scaLen = 0;
        if (smsc != NULL) {
            SmsServiceProxy serviceProxy(mService);
            serviceProxy.SetSmscAddressLength(smscLength);
            serviceProxy.SetSmscAddress(smsc, smscLength);

            scaLen = ConvertSmscBcdToNumber(smsc, smscLength, sca, scaSize);
            if (scaLen > 0) {
                const char *format = "vendor.ril.sms.sim%d.smsc";
                char buf[100] = {0, };
                snprintf(buf, sizeof(buf), format, mService->GetRilSocketId());
                RilLogV("SMSC=%s", sca);
                RilLogV("SystemProperty set(%s,%s)", buf, sca);
                SystemProperty::Set(buf, sca);
            }
            else {
                RilLogW("Failed to convert SMS PDU to String");
            }
        }
        else {
            RilLogW("SMSC is null");
        }

        OnRequestComplete(RIL_E_SUCCESS, sca, scaLen);
    }
    else {
        OnRequestComplete(errorCode);
    }
    return 0;
}

/**
 * SetSmscAddressHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SetSmscAddressHandler, MSG_SMS_SET_SMSC_ADDRESS)

int SetSmscAddressHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if(msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    StringRequestData *rildata = (StringRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    const char *sca = rildata->GetString();
    if (sca == NULL) {
        RilLogE("[%s] rildata->GetString() is NULL", mServiceName);
        return -1;
    }
    int scaLen = strlen(sca);

    if (!IsSmscLenValid(sca, scaLen)) {
        RilLogE("[%s] SMSC length exceeded maximum size", mServiceName);
        return -1;
    }

    char smscPdu[MAX_GSM_SMS_SERVICE_CENTER_ADDR] = {0, };
    int smscPduSize = (int)(sizeof(smscPdu)/sizeof(smscPdu[0]));

    int smscPduLen = ConvertSmscNumberToBcd(sca, scaLen, smscPdu, smscPduSize);
    if (smscPduLen < 0) {
        RilLogE("[%s] ConvertSmscNumberToBcd Failed", mServiceName);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSmscAddress(smscPduLen, smscPdu);

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

int SetSmscAddressHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    RilLogV("RilErrorCode(%d)", errorCode);
    OnRequestComplete(errorCode);

    if(errorCode == RIL_E_SUCCESS){
        /*  SMSC consistency management btwn RIL and Frameworks  */
        RequestData *req = new RequestData(RIL_REQUEST_GET_SMSC_ADDRESS, 0);

        if (req != NULL) {
            Message *requestMsg =
                    Message::ObtainMessage(req, RIL_SERVICE_SMS, MSG_SMS_GET_SMSC_ADDRESS);
            if (mService->GetRilContext()->GetServiceManager()->SendMessage(requestMsg) < 0) {
                if (requestMsg) {
                    delete requestMsg;
                }
            }
        }
    }
    return 0;
}

/**
 * ReportSmsMemoryStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(
        ReportSmsMemoryStatusHandler, MSG_SMS_REPORT_SMS_MEMORY_STATUS)

int ReportSmsMemoryStatusHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    IntRequestData *rildata = (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    int mem_status = rildata->GetInt();

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSmsMemoryStatus(mem_status);

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(ReportSmsMemoryStatusHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * GetStorageCapacityHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(GetStorageCapacityHandler, MSG_SMS_GET_STORAGE_CAPACITY)

int GetStorageCapacityHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    IntRequestData* rildata= (IntRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    int simId = rildata->GetInt();

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildGetStoredSmsCount(simId);

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

int GetStorageCapacityHandler::OnResponse(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolSmsCapacityOnSimRespAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();

    RilLogV("RilErrorCode(%d)", errorCode);
    if (errorCode == RIL_E_SUCCESS) {
        RIL_StorageStatus resp;
        resp.sim_id = adapter.GetSimId();
        resp.total_num = adapter.GetTotalNum();
        resp.used_num = adapter.GetUsedNum();

        RilLogI("[%s] sim id = %d", mServiceName, resp.sim_id);
        RilLogI("[%s] total no = %d", mServiceName, resp.total_num);
        RilLogI("[%s] used no = %d", mServiceName, resp.used_num);

        OnRequestComplete(errorCode, &resp, sizeof(RIL_StorageStatus));
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * NewSmsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(NewSmsHandler, MSG_SMS_INCOMING_NEW_SMS)

int NewSmsHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    SmsServiceProxy serviceProxy(mService);

    ProtocolNewSmsIndAdapter adapter(pModemData);
    char *pdu_tmp = NULL;
    pdu_tmp = (char *)adapter.GetPdu();
    g_nLastTpidNewSms = adapter.GetTpid();

    PduParser  SmsParser;
    serviceProxy.SetSmsClass(SmsParser.GetSmsClass(pdu_tmp));

    if (serviceProxy.GetSmsClass() == MESSAGE_CLASS_2) {
        RIL_SMS_WriteArgs data;
        data.smsc = NULL;
        int sca_len;

        data.status = 0x00/*SIM_STATUS_RECEIVED_UNREAD*/;
        sca_len = (SmsParser.GetScaLen(pdu_tmp)+1)*2;

        data.smsc = new char[sca_len+1];
        memset(&(data.smsc)[0], 0, sca_len+1);
        memcpy(&(data.smsc)[0], pdu_tmp, sca_len);
        data.pdu = pdu_tmp + sca_len ;

        if (SMS_DBG) {
            RilLogV("[7-1] PDU = %s", data.pdu);
            RilLogV("[7-2] PDU_SMSC = %s, strlen(data.smsc)=%d", data.smsc, strlen(data.smsc));
        }

        RequestData *req = RilParser::CreateSimSmsData(
                RIL_REQUEST_WRITE_SMS_TO_SIM, 0, (char *)(&data), sizeof(data));
        delete [] data.smsc;

        if (req != NULL) {
            Message *msg = Message::ObtainMessage(req, RIL_SERVICE_SMS, MSG_SMS_WRITE_SMS_TO_SIM);
            if (mService->GetRilContext()->GetServiceManager()->SendMessage(msg) < 0) {
                delete msg;
            }
        }
    }
    else {
        OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS, adapter.GetPdu(), adapter.GetPduSize());
    }
    return 0;
}

/**
 * NewSmsStatusReportHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(
        NewSmsStatusReportHandler, MSG_SMS_INCOMING_NEW_SMS_STATUS_REPORT)

int NewSmsStatusReportHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolNewSmsIndAdapter adapter(pModemData);
    g_nLastTpidNewSms = adapter.GetTpid();

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT, adapter.GetPdu(),
            adapter.GetPduSize());

    return 0;
}

/**
 * NewSmsOnSimHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(NewSmsOnSimHandler, MSG_SMS_INCOMING_NEW_SMS_ON_SIM)

int NewSmsOnSimHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolNewSmsOnSimIndAdapter adapter(pModemData);
    int index = adapter.GetIndex();

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM, &index, sizeof(index));

    return 0;
}

/**
 * SimSmsStorageFullHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SimSmsStorageFullHandler, MSG_SMS_SIM_SMS_STORAGE_FULL)

int SimSmsStorageFullHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    OnUnsolicitedResponse(RIL_UNSOL_SIM_SMS_STORAGE_FULL, NULL, 0);

    return 0;
}

/**
 * NewBcstSmsHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(NewBcstSmsHandler, MSG_SMS_INCOMING_NEW_BROADCAST_SMS)

int NewBcstSmsHandler::OnIndication(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const ModemData *pModemData = msg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("[%s] pModemData is NULL", mServiceName);
        return -1;
    }

    ProtocolNewBcstSmsAdapter adapter(pModemData);

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS, adapter.GetBcst(),
            adapter.GetBcstLen());

    return 0;
}

/**
 * SmsAckHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SmsAckHandler, MSG_MISC_SMS_ACKNOWLEDGE)

int SmsAckHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const SmsAcknowledge *rildata = (const SmsAcknowledge *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    int result = rildata->mResult;
    int failcause = rildata->mFailureCause;

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSmsAck(result, g_nLastTpidNewSms, failcause);

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0 ) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SmsAckHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * SmsAckWithPduHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SmsAckWithPduHandler, MSG_MISC_SMS_ACK_WITH_PDU)

int SmsAckWithPduHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    const SmsAcknowledgePdu *rildata = (const SmsAcknowledgePdu *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    const char *pdu = rildata->mPdu->mData;
    int pduSize = rildata->mPdu->mLen;

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSmsAck(rildata->mResult, g_nLastTpidNewSms, pdu, pduSize);

    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0 ) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(SmsAckWithPduHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()

/**
 * CdmaSmsAckHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(CdmaSmsAckHandler, MSG_MISC_SMS_CDMA_ACKNOWLEDGE)

int CdmaSmsAckHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("[%s] msg is NULL", mServiceName);
        return -1;
    }

    CdmaSmsAckRequestData *rildata = (CdmaSmsAckRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("[%s] rildata is NULL", mServiceName);
        return -1;
    }

    ProtocolSmsBuilder builder;
    ModemData *pModemData = builder.BuildSendCdmaSmsAck(g_nLastTpidNewSms, rildata->GetErrorClass(),
            rildata->GetErrorCode());
    if (SendRequest(pModemData, TIMEOUT_SMS_DEFAULT, mMessageId) < 0 ) {
        RilLogE("[%s] SendRequest Failed", mServiceName);
        return -1;
    }
    return 0;
}

TINY_SERVICE_HANDLER_ON_RESPONSE_BEGIN(CdmaSmsAckHandler)
    IMPLEMENT_TINY_SERVICE_HANDLER_SIMPLE_ON_RESPONSE2()
TINY_SERVICE_HANDLER_ON_RESPONSE_END()
