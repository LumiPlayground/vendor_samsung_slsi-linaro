/*
 *  SIT RIL network Unit test
 *
 *  Copyright Samsung Electronics Co., LTD.
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#include "sitrilTestSap.h"
#include "testUtil.h"
#include <hardware/ril/librilutils/proto/sap-api.pb.h>
#include <string>

#define TEST_SAP_MAX_MSG_SIZE (100)
#define TEST_SAP_SIT_DISCONNECT_SUCCESS (1)


static const UINT16 testDataLen = 6;
static BYTE testData[testDataLen] = {1, 2, 3, 4, 5, 6};

/////////////////////////////////////////////////////////////////////////////////////////////////
static pb_bytes_array_t *makePayload(uint16_t reqLen, uint8_t *reqPtr) {
    pb_bytes_array_t *payload = (pb_bytes_array_t *) malloc(sizeof(pb_bytes_array_t) - 1 + reqLen);
    if (payload == NULL) {
        return nullptr;
    }
    payload->size = reqLen;
    memcpy(payload->bytes, reqPtr, reqLen);

    return payload;
}

// protocol buffer encode functions
static pb_bytes_array_t *encodeConnectReq(int32_t maxMsgSize) {
    RIL_SIM_SAP_CONNECT_REQ req;
    memset(&req, 0, sizeof(RIL_SIM_SAP_CONNECT_REQ));
    req.max_message_size = maxMsgSize;

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_CONNECT_REQ_fields, &req)) {
        return nullptr;
    }

    uint8_t *buffer = (uint8_t *)calloc(1, encodedSize);
    if (buffer == nullptr) {
        return nullptr;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, encodedSize);

    if (!pb_encode(&stream, RIL_SIM_SAP_CONNECT_REQ_fields, &req)) {
        free(buffer);
        return nullptr;
    }

    pb_bytes_array_t *payload = makePayload(stream.bytes_written, buffer);
    free(buffer);
    return payload;
}

static pb_bytes_array_t *encodeDisconnectReq() {
    RIL_SIM_SAP_DISCONNECT_REQ req;
    memset(&req, 0, sizeof(RIL_SIM_SAP_DISCONNECT_REQ));

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_DISCONNECT_REQ_fields, &req)) {
        return nullptr;
    }

    uint8_t *buffer = (uint8_t *)calloc(1, encodedSize);
    if (buffer == NULL) {
        return nullptr;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, encodedSize);

    if (!pb_encode(&stream, RIL_SIM_SAP_DISCONNECT_REQ_fields, &req)) {
        free(buffer);
        return nullptr;
    }

    pb_bytes_array_t *payload = makePayload(stream.bytes_written, buffer);
    free(buffer);
    return payload;
}

static pb_bytes_array_t *encodeApduReq(BYTE type, BYTE *command, uint16_t commandLen) {
    RIL_SIM_SAP_APDU_REQ req;
    memset(&req, 0, sizeof(RIL_SIM_SAP_APDU_REQ));
    req.type = (RIL_SIM_SAP_APDU_REQ_Type)type;

    req.command = (pb_bytes_array_t *)malloc(sizeof(pb_bytes_array_t) - 1 + commandLen);
    if (req.command == NULL) {
        return nullptr;
    }
    req.command->size = commandLen;
    memcpy(req.command->bytes, command, commandLen);

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_APDU_REQ_fields, &req)) {
        return nullptr;
    }

    uint8_t *buffer = (uint8_t *)calloc(1, encodedSize);
    if (buffer == NULL) {
        return nullptr;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, encodedSize);

    if (!pb_encode(&stream, RIL_SIM_SAP_APDU_REQ_fields, &req)) {
        free(buffer);
        return nullptr;
    }

    pb_bytes_array_t *payload = makePayload(stream.bytes_written, buffer);
    free(buffer);
    return payload;
}

static pb_bytes_array_t *encodelTransferAtrReq() {
    RIL_SIM_SAP_TRANSFER_ATR_REQ req;
    memset(&req, 0, sizeof(RIL_SIM_SAP_TRANSFER_ATR_REQ));

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_TRANSFER_ATR_REQ_fields, &req)) {
        return nullptr;
    }

    uint8_t *buffer = (uint8_t *)calloc(1, encodedSize);
    if (buffer == NULL) {
        return nullptr;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, encodedSize);

    if (!pb_encode(&stream, RIL_SIM_SAP_TRANSFER_ATR_REQ_fields, &req)) {
        free(buffer);
        return nullptr;
    }

    pb_bytes_array_t *payload = makePayload(stream.bytes_written, buffer);
    free(buffer);
    return payload;
}

static pb_bytes_array_t *encodePowerReq(bool state) {
    RIL_SIM_SAP_POWER_REQ req;
    memset(&req, 0, sizeof(RIL_SIM_SAP_POWER_REQ));
    req.state = state;

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_POWER_REQ_fields, &req)) {
        return nullptr;
    }

    uint8_t *buffer = (uint8_t *)calloc(1, encodedSize);
    if (buffer == NULL) {
        return nullptr;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, encodedSize);

    if (!pb_encode(&stream, RIL_SIM_SAP_POWER_REQ_fields, &req)) {
        free(buffer);
        return nullptr;
    }

    pb_bytes_array_t *payload = makePayload(stream.bytes_written, buffer);
    free(buffer);
    return payload;
}

static pb_bytes_array_t *encodeResetSimReq() {
    RIL_SIM_SAP_RESET_SIM_REQ req;
    memset(&req, 0, sizeof(RIL_SIM_SAP_RESET_SIM_REQ));

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_RESET_SIM_REQ_fields, &req)) {
        return nullptr;
    }

    uint8_t *buffer = (uint8_t *)calloc(1, encodedSize);
    if (buffer == NULL) {
        return nullptr;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, encodedSize);

    if (!pb_encode(&stream, RIL_SIM_SAP_RESET_SIM_REQ_fields, &req)) {
        free(buffer);
        return nullptr;
    }

    pb_bytes_array_t *payload = makePayload(stream.bytes_written, buffer);
    free(buffer);
    return payload;
}

static pb_bytes_array_t *encodeTransferCardReaderStatusReq() {
    RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ req;
    memset(&req, 0, sizeof(RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ));

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ_fields,
            &req)) {
        return nullptr;
    }

    uint8_t *buffer = (uint8_t *)calloc(1, encodedSize);
    if (buffer == NULL) {
        return nullptr;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, encodedSize);

    if (!pb_encode(&stream, RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_REQ_fields, &req)) {
        free(buffer);
        return nullptr;
    }

    pb_bytes_array_t *payload = makePayload(stream.bytes_written, buffer);
    free(buffer);
    return payload;
}

static pb_bytes_array_t *encodeSetTransferProtocolReq(int32_t transferProtocol) {
    RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ req;
    memset(&req, 0, sizeof(RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ));
    req.protocol = (RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ_Protocol)transferProtocol;

    size_t encodedSize = 0;
    if (!pb_get_encoded_size(&encodedSize, RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ_fields, &req)) {
        return nullptr;
    }

    uint8_t *buffer = (uint8_t *)calloc(1, encodedSize);
    if (buffer == NULL) {
        return nullptr;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, encodedSize);

    if (!pb_encode(&stream, RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ_fields, &req)) {
        free(buffer);
        return nullptr;
    }

    pb_bytes_array_t *payload = makePayload(stream.bytes_written, buffer);
    free(buffer);
    return payload;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// build SIT rsp functions
static UINT8* buildConnectRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE status) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sap_connect_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sap_connect_rsp *rsp = (sit_sap_connect_rsp *)rspData;
    rsp->status = status;
    rsp->max_msg_size = TEST_SAP_MAX_MSG_SIZE;

    return rspData;
}

static UINT8* buildDisconnectRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE status) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sap_disconnect_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sap_disconnect_rsp *rsp = (sit_sap_disconnect_rsp *)rspData;
    rsp->status = status;    // success

    return rspData;
}

static UINT8* buildApduRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE result) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sap_transfer_apdu_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sap_transfer_apdu_rsp *rsp = (sit_sap_transfer_apdu_rsp *)rspData;
    rsp->result = result;
    rsp->apdu_len = testDataLen;
    memcpy(rsp->apdu, testData, testDataLen);

    return rspData;
}

static UINT8* buildTransferAtrRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE result) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sap_transfer_atr_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sap_transfer_atr_rsp *rsp = (sit_sap_transfer_atr_rsp *)rspData;
    rsp->result = result;
    rsp->atr_len = testDataLen;
    memcpy(rsp->atr, testData, testDataLen);

    return rspData;
}

static UINT8* buildPowerRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE result) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sap_power_sim_onoff_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sap_power_sim_onoff_rsp *rsp = (sit_sap_power_sim_onoff_rsp *)rspData;
    rsp->result = result;

    return rspData;
}

static UINT8* buildResetSimRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE result) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sap_reset_sim_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sap_reset_sim_rsp *rsp = (sit_sap_reset_sim_rsp *)rspData;
    rsp->result = result;

    return rspData;
}

static UINT8* buildTransferCardReaderStatusRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE result) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sap_transfer_card_reader_status_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sap_transfer_card_reader_status_rsp *rsp = (sit_sap_transfer_card_reader_status_rsp *)rspData;
    rsp->result = result;

    return rspData;
}

static UINT8* buildSetTransferProtocolRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE result) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sap_tranport_protocol_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sap_tranport_protocol_rsp *rsp = (sit_sap_tranport_protocol_rsp *)rspData;
    rsp->result = result;

    return rspData;
}

////////////////////////////////////////////////////////////////////////////////////////
// build SIT ind functions


////////////////////////////////////////////////////////////////////////////////////////
// SitrilTestSap class
void SitrilTestSap::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    mpModemData = nullptr;
    mRspData = nullptr;
    mRspDataLen = 0;

    mSapReqPbPtr = nullptr;
}

void SitrilTestSap::TearDown() {
    mSitRilTest = nullptr;
    FreeData();
}

void SitrilTestSap::FreeData() {
    if (mpModemData) { delete mpModemData; mpModemData = nullptr; }
    if (mRspData) { free(mRspData); mRspData = nullptr; }
    mRspDataLen = 0;

    if(mSapReqPbPtr) { free(mSapReqPbPtr); mSapReqPbPtr = nullptr; }
}

////////////////////////////////////////////////////////////////////////////////////////
// SET request|response
TEST_P(SitrilTestSap, connectReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = MsgId_RIL_SIM_SAP_CONNECT;    //RIL_REQUEST_SIM_SAP_CONNECT
    UINT16 sitId = SIT_SAP_CONNECT;
    UINT sitToken = TOKEN_INVALID;

    int32_t maxMsgSize = TEST_SAP_MAX_MSG_SIZE;
    mSapReqPbPtr = encodeConnectReq(maxMsgSize);
    ASSERT_NE(nullptr, mSapReqPbPtr);

    psitRilTest->processSapReq(GetRandomSerialNumber(), testRilCmdId, mSapReqPbPtr->bytes, mSapReqPbPtr->size, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sap_connect_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, txIpcRawDataLen);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildConnectRsp(sitId, sitToken, &mRspDataLen, RIL_SIM_SAP_CONNECT_RSP_Response_RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestSap, disconnectReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = MsgId_RIL_SIM_SAP_DISCONNECT;    //RIL_REQUEST_SIM_SAP_DISCONNECT
    UINT16 sitId = SIT_SAP_DISCONNECT;
    UINT sitToken = TOKEN_INVALID;

    mSapReqPbPtr = encodeDisconnectReq();
    ASSERT_NE(nullptr, mSapReqPbPtr);

    psitRilTest->processSapReq(GetRandomSerialNumber(), testRilCmdId, mSapReqPbPtr->bytes, mSapReqPbPtr->size, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, txIpcRawDataLen);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildDisconnectRsp(sitId, sitToken, &mRspDataLen, TEST_SAP_SIT_DISCONNECT_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestSap, apduReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = MsgId_RIL_SIM_SAP_APDU;    //RIL_REQUEST_SIM_SAP_APDU
    UINT16 sitId = SIT_SAP_TRANSFER_APDU;
    UINT sitToken = TOKEN_INVALID;

    int32_t pduType = (int32_t)RIL_SIM_SAP_APDU_REQ_Type_RIL_TYPE_APDU;
    BYTE *apduData = testData;
    int32_t apduLen = testDataLen;

    mSapReqPbPtr = encodeApduReq(pduType, apduData, apduLen);
    ASSERT_NE(nullptr, mSapReqPbPtr);

    psitRilTest->processSapReq(GetRandomSerialNumber(), testRilCmdId, mSapReqPbPtr->bytes, mSapReqPbPtr->size, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    //UINT32 sizeOfModemData = sizeof(sit_sap_transfer_apdu_req);
    UINT32 sizeOfModemData = sizeof(RCM_HEADER) + apduLen + sizeof(UINT16);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, txIpcRawDataLen);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildApduRsp(sitId, sitToken, &mRspDataLen, RIL_SIM_SAP_APDU_RSP_Response_RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestSap, transferAtrReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = MsgId_RIL_SIM_SAP_TRANSFER_ATR;    //RIL_REQUEST_SIM_SAP_TRANSFER_ATR
    UINT16 sitId = SIT_SAP_TRANSFER_ATR;
    UINT sitToken = TOKEN_INVALID;

    mSapReqPbPtr = encodelTransferAtrReq();
    ASSERT_NE(nullptr, mSapReqPbPtr);

    psitRilTest->processSapReq(GetRandomSerialNumber(), testRilCmdId, mSapReqPbPtr->bytes, mSapReqPbPtr->size, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, txIpcRawDataLen);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildTransferAtrRsp(sitId, sitToken, &mRspDataLen, RIL_SIM_SAP_TRANSFER_ATR_RSP_Response_RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestSap, powerReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = MsgId_RIL_SIM_SAP_POWER;    //RIL_REQUEST_SIM_SAP_POWER
    UINT16 sitIds[2] = {SIT_SAP_POWER_SIM_OFF, SIT_SAP_POWER_SIM_ON};
    UINT sitToken = TOKEN_INVALID;
    bool powerStates[2] = { false, true };

    for (int i = 0; i < 2; ++i) {
        UINT16 sitId = sitIds[i];
        bool powerState = powerStates[i];

        mSapReqPbPtr = encodePowerReq(powerState);
        ASSERT_NE(nullptr, mSapReqPbPtr);

        psitRilTest->processSapReq(GetRandomSerialNumber(), testRilCmdId, mSapReqPbPtr->bytes, mSapReqPbPtr->size, mPhoneIndex);
        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        UINT32 sizeOfModemData = sizeof(null_data_format);
        UINT32 txIpcRawDataLen = 0;
        char *txIpcRawData = nullptr;

        txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
        EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

        mpModemData = new ModemData(txIpcRawData, txIpcRawDataLen);
        ASSERT_EQ(mpModemData->GetMessageId(), sitId);

        /* test RSP */
        sitToken = mpModemData->GetToken();
        mRspData = buildPowerRsp(sitId, sitToken, &mRspDataLen, RIL_SIM_SAP_POWER_RSP_Response_RIL_E_SUCCESS);
        psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        UINT32 rilRspDataLen = 0;
        void *rilRspData= nullptr;
        rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
        EXPECT_NE((unsigned int)0, rilRspDataLen);

        FreeData();
        mSitRilTest->resetTestData();
    }
}

TEST_P(SitrilTestSap, resetSimReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = MsgId_RIL_SIM_SAP_RESET_SIM;    //RIL_REQUEST_SIM_SAP_RESET_SIM
    UINT16 sitId = SIT_SAP_RESET_SIM;
    UINT sitToken = TOKEN_INVALID;

    mSapReqPbPtr = encodeResetSimReq();
    ASSERT_NE(nullptr, mSapReqPbPtr);

    psitRilTest->processSapReq(GetRandomSerialNumber(), testRilCmdId, mSapReqPbPtr->bytes, mSapReqPbPtr->size, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, txIpcRawDataLen);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildResetSimRsp(sitId, sitToken, &mRspDataLen, RIL_SIM_SAP_RESET_SIM_RSP_Response_RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestSap, transferCardReaderStatusReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = MsgId_RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS;    //RIL_REQUEST_SIM_SAP_TRANSFER_CARD_READER_STATUS
    UINT16 sitId = SIT_SAP_TRANSFER_CARD_READER_STATUS;
    UINT sitToken = TOKEN_INVALID;

    mSapReqPbPtr = encodeTransferCardReaderStatusReq();
    ASSERT_NE(nullptr, mSapReqPbPtr);

    psitRilTest->processSapReq(GetRandomSerialNumber(), testRilCmdId, mSapReqPbPtr->bytes, mSapReqPbPtr->size, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, txIpcRawDataLen);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildTransferCardReaderStatusRsp(sitId, sitToken, &mRspDataLen, RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS_RSP_Response_RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestSap, setTransferProtocolReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = MsgId_RIL_SIM_SAP_SET_TRANSFER_PROTOCOL;    //RIL_REQUEST_SIM_SAP_SET_TRANSFER_PROTOCOL
    UINT16 sitId = SIT_SAP_SET_TRANPORT_PROTOCOL;
    UINT sitToken = TOKEN_INVALID;

    int32_t transferProtocol = (int32_t)RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_REQ_Protocol_t0;
    mSapReqPbPtr = encodeSetTransferProtocolReq(transferProtocol);
    ASSERT_NE(nullptr, mSapReqPbPtr);

    psitRilTest->processSapReq(GetRandomSerialNumber(), testRilCmdId, mSapReqPbPtr->bytes, mSapReqPbPtr->size, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sap_tranport_protocol_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    EXPECT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, txIpcRawDataLen);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildSetTransferProtocolRsp(sitId, sitToken, &mRspDataLen, RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP_Response_RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

////////////////////////////////////////////////////////////////////////////////////////
// Indication
TEST_P(SitrilTestSap, disconnectInd) {
    //RIL_UNSOL_RESPONSE_SIM_SAP_DISCONNECT
}

TEST_P(SitrilTestSap, sapStatusInd) {
    //RIL_UNSOL_RESPONSE_SIM_SAP_STATUS
}

TEST_P(SitrilTestSap, sapErrorInd) {
    //RIL_UNSOL_RESPONSE_SIM_SAP_ERROR_RESP
}
