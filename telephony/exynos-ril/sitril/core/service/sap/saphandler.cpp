/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "saphandler.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "protocolsapadapter.h"
#include "protocolsapbuilder.h"
#include "rillog.h"
#include "service.h"
#include "rilapplication.h"
#include <hardware/ril/librilutils/proto/sap-api.pb.h>

#define TIMEOUT_SAP_DEFAULT     5000

static std::shared_ptr<uint8_t> DecodeMessage(int msgId, uint8_t *payloadPtr, size_t payloadLen) {
    RilLogI("%s()", __FUNCTION__);

     std::shared_ptr<uint8_t> requestPtr;
    pb_istream_t stream;

    /* Create the stream */
    stream = pb_istream_from_buffer(payloadPtr, payloadLen);

    /* Decode based on the message id */
    switch (msgId)
    {
        case MSG_SAP_CONNECT:
            requestPtr = std::shared_ptr<uint8_t>(new uint8_t[sizeof(RIL_SIM_SAP_CONNECT_REQ)] {},
                                                    std::default_delete<uint8_t[]>());
            if (!pb_decode(&stream, RIL_SIM_SAP_CONNECT_REQ_fields, requestPtr.get())) {
                RilLogE("Error decoding RIL_SIM_SAP_CONNECT_REQ");
                return nullptr;
            }
            break;
        case MSG_SAP_DISCONNECT:
            requestPtr = std::shared_ptr<uint8_t>(new uint8_t[sizeof(RIL_SIM_SAP_DISCONNECT_REQ)] {},
                                                    std::default_delete<uint8_t[]>());
            if (!pb_decode(&stream, RIL_SIM_SAP_DISCONNECT_REQ_fields, requestPtr.get())) {
                RilLogE("Error decoding RIL_SIM_SAP_DISCONNECT_REQ");
                return nullptr;
            }
            break;
        case MSG_SAP_APDU:
            requestPtr = std::shared_ptr<uint8_t>(new uint8_t[sizeof(RIL_SIM_SAP_APDU_REQ)] {},
                                                    std::default_delete<uint8_t[]>());
            if (!pb_decode(&stream, RIL_SIM_SAP_APDU_REQ_fields, requestPtr.get())) {
                RilLogE("Error decoding RIL_SIM_SAP_APDU_REQ");
                return nullptr;
            }
            break;
        case MSG_SAP_TRANSFER_ATR:
            requestPtr = std::shared_ptr<uint8_t>(new uint8_t[sizeof(RIL_SIM_SAP_TRANSFER_ATR_REQ)] {},
                                                    std::default_delete<uint8_t[]>());
            if (!pb_decode(&stream, RIL_SIM_SAP_TRANSFER_ATR_REQ_fields, requestPtr.get())) {
                RilLogE("Error decoding RIL_SIM_SAP_TRANSFER_ATR_REQ");
                return nullptr;
            }
            break;
        case MSG_SAP_POWER:
            requestPtr = std::shared_ptr<uint8_t>(new uint8_t[sizeof(RIL_SIM_SAP_POWER_REQ)] {},
                                                    std::default_delete<uint8_t[]>());
            if (!pb_decode(&stream, RIL_SIM_SAP_POWER_REQ_fields, requestPtr.get())) {
                RilLogE("Error decoding RIL_SIM_SAP_POWER_REQ");
                return nullptr;
            }
            break;
        case MSG_SAP_RESET_SIM:
            requestPtr = std::shared_ptr<uint8_t>(new uint8_t[sizeof(RIL_SIM_SAP_RESET_SIM_REQ)] {},
                                                    std::default_delete<uint8_t[]>());
            if (!pb_decode(&stream, RIL_SIM_SAP_RESET_SIM_REQ_fields, requestPtr.get())) {
                RilLogE("Error decoding RIL_SIM_SAP_RESET_SIM_REQ");
                return nullptr;
            }
            break;
        case MSG_SAP_TRANSFER_CARD_READER_STATUS:
            requestPtr = std::shared_ptr<uint8_t>(new uint8_t[sizeof(RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ)] {},
                                                    std::default_delete<uint8_t[]>());
            if (!pb_decode(&stream, RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ_fields, requestPtr.get())) {
                RilLogE("Error decoding RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ");
                return nullptr;
            }
            break;
        case MSG_SAP_SET_TRANSFER_PROTOCOL:
            requestPtr = std::shared_ptr<uint8_t>(new uint8_t[sizeof(RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ)] {},
                                                    std::default_delete<uint8_t[]>());
            if (!pb_decode(&stream, RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ_fields, requestPtr.get())) {
                RilLogE("Error decoding RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ");
                return nullptr;
            }
            break;
        default:
            RilLogE("DecodeMessage: unsupported msgId=%d", msgId);
            break;
    }
    return requestPtr;
} /* sapDecodeMessage */

/**
 * SapConnectHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SapConnectHandler, MSG_SAP_CONNECT)

int SapConnectHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    auto data = DecodeMessage(msg->GetMsgId(), (uint8_t *) rildata->GetRawData(), rildata->GetSize());
    if (data == NULL) {
        RilLogE("data is NULL");
        return -1;
    }

    RIL_SIM_SAP_CONNECT_REQ *req = (RIL_SIM_SAP_CONNECT_REQ *) data.get();
    RilLogV("max_message_size=%d", req->max_message_size);

    ProtocolSapBuilder builder;
    ModemData *pModemData = builder.BuildSapConnect(req->max_message_size);

    if (SendRequest(pModemData, TIMEOUT_SAP_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SapConnectHandler::OnResponse(Message *msg)
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

    ProtocolSapConnectAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        /***** Encode RIL_SIM_SAP_CONNECT_RSP *****/
        RIL_SIM_SAP_CONNECT_RSP rsp;
        memset(&rsp, 0, sizeof(RIL_SIM_SAP_CONNECT_RSP));
        rsp.response = (RIL_SIM_SAP_CONNECT_RSP_Response)adapter.GetStatus();
        if (rsp.response != RIL_SIM_SAP_CONNECT_RSP_Response_RIL_E_SAP_CONNECT_FAILURE) {
            rsp.has_max_message_size = true;
        } else {
            rsp.has_max_message_size = false;
        }
        rsp.max_message_size = adapter.GetMaxMsgSize();
        RilLogV("Connect response=%d, size=%d", rsp.response, rsp.max_message_size);

        size_t encodedSize = 0;
        if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_CONNECT_RSP_fields, &rsp)) {
            RilLogE("SapImpl::connectRsp: Error getting encoded size for RIL_SIM_SAP_CONNECT_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }

        auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[encodedSize] {});
        pb_ostream_t stream = pb_ostream_from_buffer(buffer.get(), encodedSize);

        RilLogV("SapImpl::connectRsp calling pb_encode");
        if (!pb_encode(&stream, RIL_SIM_SAP_CONNECT_RSP_fields, &rsp)) {
            RilLogE("SapImpl::connectRsp: Error encoding RIL_SIM_SAP_CONNECT_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }
        /***** Encode RIL_SIM_SAP_CONNECT_RSP done *****/

        OnRequestComplete(RIL_E_SUCCESS, buffer.get(), stream.bytes_written);
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SapDisconnectHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SapDisconnectHandler, MSG_SAP_DISCONNECT)

int SapDisconnectHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    auto data = DecodeMessage(msg->GetMsgId(), (uint8_t *) rildata->GetRawData(), rildata->GetSize());
    if (data == NULL) {
        RilLogE("data is NULL");
        return -1;
    }

    RIL_SIM_SAP_DISCONNECT_REQ *req = (RIL_SIM_SAP_DISCONNECT_REQ *) data.get();
    RilLogV("dummy_field=%d", req->dummy_field);

    ProtocolSapBuilder builder;
    ModemData *pModemData = builder.BuildSapDisconnect();

    if (SendRequest(pModemData, TIMEOUT_SAP_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SapDisconnectHandler::OnResponse(Message *msg)
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

    ProtocolSapDisconnectAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int status = adapter.GetStatus();
        RilLogV("Disconnect status=%s", (status == 0) ? "Success" : "Failure");
        // 0 : sucess, 1 : failure
        if (status == 0) {
            OnRequestComplete(RIL_E_SUCCESS);
        } else {
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SapApduHandler
 */
const char *SapApduHandler::TAG = "SapApduHandler";
SapApduHandler *SapApduHandler::Create(Service *service) {
    return new SapApduHandler(service);
}

SapApduHandler::SapApduHandler(Service *service)
    : ServiceHandlerBase(MSG_SAP_APDU, service), mApduType(0) {
}

SapApduHandler::~SapApduHandler() {}

int SapApduHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    auto data = DecodeMessage(msg->GetMsgId(), (uint8_t *) rildata->GetRawData(), rildata->GetSize());
    if (data == NULL) {
        RilLogE("data is NULL");
        return -1;
    }

    RIL_SIM_SAP_APDU_REQ *req = (RIL_SIM_SAP_APDU_REQ *) data.get();
    RilLogV("Apdu type=%d", req->type);
    mApduType = req->type;
    if (req->command == NULL) {
        RilLogE("data is NULL");
        return -1;
    }

    int len = req->command->size;
    for (int i = 0; i < len; i++) RilLogV("command=%d", req->command->bytes[i]);

    ProtocolSapBuilder builder;
    ModemData *pModemData = builder.BuildSapTransferApdu((int) req->command->size, (char*) req->command->bytes);

    if (SendRequest(pModemData, TIMEOUT_SAP_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SapApduHandler::OnResponse(Message *msg)
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

    ProtocolSapTransferApduAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        /***** Encode RIL_SIM_SAP_APDU_RSP *****/
        RIL_SIM_SAP_APDU_RSP rsp;
        memset(&rsp, 0, sizeof(RIL_SIM_SAP_APDU_RSP));
        int apduLen = (int)adapter.GetApduLen();
        auto apdu = std::unique_ptr<uint8_t[]>(new uint8_t[sizeof(pb_bytes_array_t) - 1 + apduLen] {});
        rsp.type = (RIL_SIM_SAP_APDU_RSP_Type)mApduType;
        rsp.response = (RIL_SIM_SAP_APDU_RSP_Response)adapter.GetResult();
        rsp.apduResponse = (pb_bytes_array_t *)apdu.get();
        rsp.apduResponse->size = apduLen;
        memcpy(rsp.apduResponse->bytes, adapter.GetApdu(), apduLen);
        RilLogV("Transfer APDU type=%d, result=%d, length=%d", rsp.type, rsp.response, apduLen);

        size_t encodedSize = 0;
        if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_APDU_RSP_fields, &rsp)) {
            RilLogE("SapImpl::apduRsp: Error getting encoded size for RIL_SIM_SAP_APDU_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }

        auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[encodedSize] {});
        pb_ostream_t stream = pb_ostream_from_buffer(buffer.get(), encodedSize);

        RilLogV("SapImpl::apduRsp calling pb_encode");
        if (!pb_encode(&stream, RIL_SIM_SAP_APDU_RSP_fields, &rsp)) {
            RilLogE("SapImpl::apduRsp: Error encoding RIL_SIM_SAP_APDU_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }
        /***** Encode RIL_SIM_SAP_APDU_RSP done *****/

        OnRequestComplete(RIL_E_SUCCESS, buffer.get(), stream.bytes_written);
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SapTransferAtrHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SapTransferAtrHandler, MSG_SAP_TRANSFER_ATR)

int SapTransferAtrHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    auto data = DecodeMessage(msg->GetMsgId(), (uint8_t *) rildata->GetRawData(), rildata->GetSize());
    if (data == NULL) {
        RilLogE("data is NULL");
        return -1;
    }

    RIL_SIM_SAP_TRANSFER_ATR_REQ *req = (RIL_SIM_SAP_TRANSFER_ATR_REQ *) data.get();
    RilLogV("dummy_field=%d", req->dummy_field);

    ProtocolSapBuilder builder;
    ModemData *pModemData = builder.BuildSapTransferAtr();

    if (SendRequest(pModemData, TIMEOUT_SAP_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SapTransferAtrHandler::OnResponse(Message *msg)
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

    ProtocolSapTransferAtrAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        /***** Encode RIL_SIM_SAP_TRANSFER_ATR_RSP *****/
        RIL_SIM_SAP_TRANSFER_ATR_RSP rsp;
        memset(&rsp, 0, sizeof(RIL_SIM_SAP_TRANSFER_ATR_RSP));

        int atrLen = (int)adapter.GetAtrLen();
        auto atr = std::unique_ptr<uint8_t[]>(new uint8_t[sizeof(pb_bytes_array_t) - 1 + atrLen] {});
        rsp.response = (RIL_SIM_SAP_TRANSFER_ATR_RSP_Response)adapter.GetResult();
        rsp.atr = (pb_bytes_array_t *)atr.get();
        rsp.atr->size = atrLen;
        memcpy(rsp.atr->bytes, adapter.GetAtr(), atrLen);
        RilLogV("Transfer ATR response=%d, length=%d", rsp.response, atrLen);

        size_t encodedSize = 0;
        if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_TRANSFER_ATR_RSP_fields, &rsp)) {
            RilLogE("SapImpl::transferAtrRsp: Error getting encoded size for RIL_SIM_SAP_TRANSFER_ATR_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }

        auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[encodedSize] {});
        pb_ostream_t stream = pb_ostream_from_buffer(buffer.get(), encodedSize);

        RilLogV("SapImpl::transferAtrRsp calling pb_encode");
        if (!pb_encode(&stream, RIL_SIM_SAP_TRANSFER_ATR_RSP_fields, &rsp)) {
            RilLogE("SapImpl::transferAtrRsp: Error encoding RIL_SIM_SAP_TRANSFER_ATR_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }
        /***** Encode RIL_SIM_SAP_TRANSFER_ATR_RSP done *****/

        OnRequestComplete(RIL_E_SUCCESS, buffer.get(), stream.bytes_written);
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SapPowerHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SapPowerHandler, MSG_SAP_POWER)

int SapPowerHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    auto data = DecodeMessage(msg->GetMsgId(), (uint8_t *) rildata->GetRawData(), rildata->GetSize());
    if (data == NULL) {
        RilLogE("data is NULL");
        return -1;
    }

    RIL_SIM_SAP_POWER_REQ *req = (RIL_SIM_SAP_POWER_REQ *) data.get();
    RilLogV("state=%d", req->state ? 1 : 0);

    ProtocolSapBuilder builder;
    ModemData *pModemData = builder.BuildSapPowerSimOnOff(req->state ? true : false);

    if (SendRequest(pModemData, TIMEOUT_SAP_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SapPowerHandler::OnResponse(Message *msg)
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

    ProtocolSapPowerSimOnOffAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        /***** Encode RIL_SIM_SAP_POWER_RSP *****/
        RIL_SIM_SAP_POWER_RSP rsp;
        memset(&rsp, 0, sizeof(RIL_SIM_SAP_POWER_RSP));

        rsp.response = (RIL_SIM_SAP_POWER_RSP_Response)adapter.GetResult();
        RilLogV("SAP SIM Power response=%d", rsp.response);

        size_t encodedSize = 0;
        if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_POWER_RSP_fields, &rsp)) {
            RilLogE("SapImpl::powerRsp: Error getting encoded size for RIL_SIM_SAP_POWER_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }

        auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[encodedSize] {});
        pb_ostream_t stream = pb_ostream_from_buffer(buffer.get(), encodedSize);

        RilLogV("SapImpl::powerRsp calling pb_encode");
        if (!pb_encode(&stream, RIL_SIM_SAP_POWER_RSP_fields, &rsp)) {
            RilLogE("SapImpl::powerRsp: Error encoding RIL_SIM_SAP_POWER_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }
        /***** Encode RIL_SIM_SAP_POWER_RSP done *****/

        OnRequestComplete(RIL_E_SUCCESS, buffer.get(), stream.bytes_written);
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SapResetSimHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SapResetSimHandler, MSG_SAP_RESET_SIM)

int SapResetSimHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    auto data = DecodeMessage(msg->GetMsgId(), (uint8_t *) rildata->GetRawData(), rildata->GetSize());
    if (data == NULL) {
        RilLogE("data is NULL");
        return -1;
    }

    RIL_SIM_SAP_RESET_SIM_REQ *req = (RIL_SIM_SAP_RESET_SIM_REQ *) data.get();
    RilLogV("dummy_field=%d", req->dummy_field);

    ProtocolSapBuilder builder;
    ModemData *pModemData = builder.BuildSapResetSim();

    if (SendRequest(pModemData, TIMEOUT_SAP_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SapResetSimHandler::OnResponse(Message *msg)
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

    ProtocolSapResetSimAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        /***** Encode RIL_SIM_SAP_RESET_SIM_RSP *****/
        RIL_SIM_SAP_RESET_SIM_RSP rsp;
        memset(&rsp, 0, sizeof(RIL_SIM_SAP_RESET_SIM_RSP));

        rsp.response = (RIL_SIM_SAP_RESET_SIM_RSP_Response)adapter.GetResult();
        RilLogV("Reset SIM response=%d", rsp.response);

        size_t encodedSize = 0;
        if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_RESET_SIM_RSP_fields, &rsp)) {
            RilLogE("SapImpl::resetSimRsp: Error getting encoded size for RIL_SIM_SAP_RESET_SIM_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }

        auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[encodedSize] {});
        pb_ostream_t stream = pb_ostream_from_buffer(buffer.get(), encodedSize);

        RilLogV("SapImpl::resetSimRsp calling pb_encode");
        if (!pb_encode(&stream, RIL_SIM_SAP_RESET_SIM_RSP_fields, &rsp)) {
            RilLogE("SapImpl::resetSimRsp: Error encoding RIL_SIM_SAP_RESET_SIM_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }
        /***** Encode RIL_SIM_SAP_RESET_SIM_RSP done *****/

        OnRequestComplete(RIL_E_SUCCESS, buffer.get(), stream.bytes_written);
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SapTransferCardReaderStatusHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SapTransferCardReaderStatusHandler, MSG_SAP_TRANSFER_CARD_READER_STATUS)

int SapTransferCardReaderStatusHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    auto data = DecodeMessage(msg->GetMsgId(), (uint8_t *) rildata->GetRawData(), rildata->GetSize());
    if (data == NULL) {
        RilLogE("data is NULL");
        return -1;
    }

    RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ *req = (RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ *) data.get();
    RilLogV("dummy_field=%d", req->dummy_field);

    ProtocolSapBuilder builder;
    ModemData *pModemData = builder.BuildSapTransferCardReaderStatus();

    if (SendRequest(pModemData, TIMEOUT_SAP_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SapTransferCardReaderStatusHandler::OnResponse(Message *msg)
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

    ProtocolSapTransferCardReaderStatusAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        /***** Encode RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP *****/
        RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP rsp;
        memset(&rsp, 0, sizeof(RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP));

        int cardState = mRilContext->GetUiccCardState();
        rsp.response = (RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_Response)adapter.GetResult();
        rsp.has_CardReaderStatus = (cardState == RIL_CARDSTATE_PRESENT ? true : false);
        //TODO: need to check CardReaderStatus
        rsp.CardReaderStatus = 1;
        RilLogV("Transfer Card Reader Status response=%d, hasStatus=%d, readerStatus=%d",
                rsp.response, rsp.has_CardReaderStatus, rsp.CardReaderStatus);

        size_t encodedSize = 0;
        if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_fields, &rsp)) {
            RilLogE("SapImpl::transferCardReaderStatusRsp: Error getting encoded size for RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }

        auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[encodedSize] {});
        pb_ostream_t stream = pb_ostream_from_buffer(buffer.get(), encodedSize);

        RilLogV("SapImpl::transferCardReaderStatusRsp calling pb_encode");
        if (!pb_encode(&stream, RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_fields, &rsp)) {
            RilLogE("SapImpl::transferCardReaderStatusRsp: Error encoding RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }
        /***** Encode RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP done *****/

        OnRequestComplete(RIL_E_SUCCESS, buffer.get(), stream.bytes_written);
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SapSetTransferProtocolHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SapSetTransferProtocolHandler, MSG_SAP_SET_TRANSFER_PROTOCOL)

int SapSetTransferProtocolHandler::OnRequest(Message *msg)
{
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);

    if (msg == NULL) {
        RilLogE("msg is NULL");
        return -1;
    }

    RawRequestData *rildata = (RawRequestData *)msg->GetRequestData();
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    auto data = DecodeMessage(msg->GetMsgId(), (uint8_t *) rildata->GetRawData(), rildata->GetSize());
    if (data == NULL) {
        RilLogE("data is NULL");
        return -1;
    }

    RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ *req = (RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ *) data.get();
    RilLogV("protocol=%d", req->protocol);

    ProtocolSapBuilder builder;
    ModemData *pModemData = builder.BuildSapTranportProtocol(req->protocol);

    if (SendRequest(pModemData, TIMEOUT_SAP_DEFAULT, mMessageId) < 0) {
        return -1;
    }

    return 0;
}

int SapSetTransferProtocolHandler::OnResponse(Message *msg)
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

    ProtocolSapSetTranportProtocolAdapter adapter(pModemData);
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        /***** Encode RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP *****/
        RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP rsp;
        memset(&rsp, 0, sizeof(RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP));

        rsp.response = (RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP_Response)adapter.GetResult();
        RilLogV("Tranport Protocol response=%d", rsp.response);

        size_t encodedSize = 0;
        if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP_fields, &rsp)) {
            RilLogE("SapImpl::setTransferProtocolRsp: Error getting encoded size for RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }

        auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[encodedSize] {});
        pb_ostream_t stream = pb_ostream_from_buffer(buffer.get(), encodedSize);

        RilLogV("SapImpl::setTransferProtocolRsp calling pb_encode");
        if (!pb_encode(&stream, RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP_fields, &rsp)) {
            RilLogE("SapImpl::setTransferProtocolRsp: Error encoding RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP");
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
            return 0;
        }
        /***** Encode RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP done *****/

        OnRequestComplete(RIL_E_SUCCESS, buffer.get(), stream.bytes_written);
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * SapDisconnectIndHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SapDisconnectIndHandler, MSG_SAP_DISCONNECT_IND)

int SapDisconnectIndHandler::OnIndication(Message *msg)
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

    ProtocolSapDisconnectTypeAdapter adapter(msg->GetModemData());
    /***** Encode RIL_SIM_SAP_DISCONNECT_IND *****/
    RIL_SIM_SAP_DISCONNECT_IND ind;
    memset(&ind, 0, sizeof(RIL_SIM_SAP_DISCONNECT_IND));

    ind.disconnectType = (RIL_SIM_SAP_DISCONNECT_IND_DisconnectType)adapter.GetDisconStatus();
    RilLogV("Disconnect indication disconType=%d", ind.disconnectType);

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_DISCONNECT_IND_fields, &ind)) {
        RilLogE("SapImpl::unsolDisconnect: Error getting encoded size for RIL_SIM_SAP_DISCONNECT_IND");
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
        return 0;
    }

    auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[encodedSize] {});
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.get(), encodedSize);

    RilLogV("SapImpl::unsolDisconnect calling pb_encode");
    if (!pb_encode(&stream, RIL_SIM_SAP_DISCONNECT_IND_fields, &ind)) {
        RilLogE("SapImpl::unsolDisconnect: Error encoding RIL_SIM_SAP_DISCONNECT_IND");
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
        return 0;
    }
    /***** Encode RIL_SIM_SAP_DISCONNECT_IND done *****/

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_SAP_DISCONNECT, buffer.get(), stream.bytes_written);
    return 0;
}

/**
 * SapStatusIndHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(SapStatusIndHandler, MSG_SAP_STATUS_IND)

int SapStatusIndHandler::OnIndication(Message *msg)
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

    ProtocolSapStatusAdapter adapter(msg->GetModemData());
    /***** Encode RIL_SIM_SAP_STATUS_IND *****/
    RIL_SIM_SAP_STATUS_IND ind;
    memset(&ind, 0, sizeof(RIL_SIM_SAP_STATUS_IND));

    ind.statusChange = (RIL_SIM_SAP_STATUS_IND_Status)adapter.GetCardStatus();
    RilLogV("Card Status indication cardStatus=%d", ind.statusChange);

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_STATUS_IND_fields, &ind)) {
        RilLogE("SapImpl::unsolStatus: Error getting encoded size for RIL_SIM_SAP_STATUS_IND");
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
        return 0;
    }

    auto buffer = std::unique_ptr<uint8_t[]>(new uint8_t[encodedSize] {});
    pb_ostream_t stream = pb_ostream_from_buffer(buffer.get(), encodedSize);

    RilLogV("SapImpl::unsolStatus calling pb_encode");
    if (!pb_encode(&stream, RIL_SIM_SAP_STATUS_IND_fields, &ind)) {
        RilLogE("SapImpl::unsolStatus: Error encoding RIL_SIM_SAP_STATUS_IND");
        OnRequestComplete(RIL_E_GENERIC_FAILURE);
        return 0;
    }
    /***** Encode RIL_SIM_SAP_STATUS_IND done *****/

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_SAP_STATUS, buffer.get(), stream.bytes_written);
    return 0;
}
