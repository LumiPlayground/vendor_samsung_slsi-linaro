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

#include "sitrilTestSms.h"
#include "testUtil.h"
#include "cdmasmsdata.h"

#define SMS_UT_BCST_INFO_NUM            3
#define SMS_UT_MSG_REF                  1
#define SMS_UT_SIM_IDX                  1
#define SMS_UT_SIM_ID                   1
#define SMS_UT_SMS_TOTAL_NUM_ON_SIM     5
#define SMS_UT_SMS_USED_NUM_ON_SIM      3
#define SMS_UT_CDMA_SMS_ERROR_CLASS     RIL_CDMA_SMS_ErrorClass::RIL_CDMA_SMS_NO_ERROR
#define SMS_UT_CDMA_SIM_IDX             1
#define SMS_UT_CDMA_SMS_TPID            1
#define SMS_UT_CDMA_VOICE_MSG_COUNT     1

#define BYTE_TO_CHAR(b) (b) <= 9 ? (char)((b) + '0') : (char)((b) + ('A' - 0xA))    // 0x01 -> '1'

// SMSC for unit test
#define SMS_UT_IS_SMSC_INTERNATIONAL true
#define SMS_UT_SMSC_BYTE_LEN 4
BYTE    SMS_UT_SMSC_BYTE[SMS_UT_SMSC_BYTE_LEN] = {
        SMS_UT_SMSC_BYTE_LEN - 1,
        (SMS_UT_IS_SMSC_INTERNATIONAL ? RIL_TOA_INTERNATIONAL : RIL_TOA_UNKNOWN),
        0X21, 0x43};
#define SMS_UT_SMSC_CHAR_LEN (SMS_UT_SMSC_BYTE_LEN * 2)
char    SMS_UT_SMSC_CHAR[SMS_UT_SMSC_CHAR_LEN] = {
        BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[0] >> 4), BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[0] % 16),
        BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[1] >> 4), BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[1] % 16),
        BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[2] >> 4), BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[2] % 16),
        BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[3] >> 4), BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[3] % 16)};
#define SMS_UT_SMSC_NUM_CHAR_LEN (SMS_UT_SMSC_CHAR_LEN - 4)
char    SMS_UT_SMSC_NUM_CHAR[SMS_UT_SMSC_NUM_CHAR_LEN] = {
        BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[2] % 16), BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[2] >> 4),
        BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[3] % 16), BYTE_TO_CHAR(SMS_UT_SMSC_BYTE[3] >> 4)};

// SMS Message for unit test
#define SMS_UT_SMS_CLASS    0       // 2 : write sms to sim
#define SMS_UT_SMS_BODY_BYTE_LEN 15
BYTE    SMS_UT_SMS_BODY_BYTE[SMS_UT_SMS_BODY_BYTE_LEN] = {
        SMS_UT_SMSC_BYTE[0], SMS_UT_SMSC_BYTE[1], SMS_UT_SMSC_BYTE[2], SMS_UT_SMSC_BYTE[3],
        0x04, 0x0b, 0xa1, 0x10, 0x50, 0x60,
        0x56, 0x32, 0xf6, 0x00, 0x08 | SMS_UT_SMS_CLASS};
#define SMS_UT_SMS_BODY_CHAR_LEN (SMS_UT_SMS_BODY_BYTE_LEN * 2)
char    SMS_UT_SMS_BODY_CHAR[SMS_UT_SMS_BODY_CHAR_LEN] = {
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[0] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[0] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[1] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[1] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[2] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[2] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[3] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[3] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[4] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[4] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[5] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[5] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[6] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[6] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[7] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[7] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[8] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[8] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[9] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[9] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[10] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[10] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[11] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[11] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[12] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[12] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[13] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[13] % 16),
        BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[14] >> 4), BYTE_TO_CHAR(SMS_UT_SMS_BODY_BYTE[14] % 16)};

// CDMA SMS Message for unit test
RIL_CDMA_SMS_Message SMS_UT_CDMA_SMS_MESSAGE = {
    .uTeleserviceID = 0x1002,
    .bIsServicePresent = 0x0,
    .uServicecategory = 0x0,
    .sAddress.digit_mode = RIL_CDMA_SMS_DigitMode::RIL_CDMA_SMS_DIGIT_MODE_4_BIT,
    .sAddress.number_mode = RIL_CDMA_SMS_NumberMode::RIL_CDMA_SMS_NUMBER_MODE_NOT_DATA_NETWORK,
    .sAddress.number_type = RIL_CDMA_SMS_NumberType::RIL_CDMA_SMS_NUMBER_TYPE_UNKNOWN,
    .sAddress.number_plan = RIL_CDMA_SMS_NumberPlan::RIL_CDMA_SMS_NUMBER_PLAN_UNKNOWN,
    .sAddress.number_of_digits = 11,
    .sAddress.digits[0] = 0xa,.sAddress.digits[1] = 0x1,
    .sAddress.digits[2] = 0xa,.sAddress.digits[3] = 0x5,
    .sAddress.digits[4] = 0xa,.sAddress.digits[5] = 0x6,
    .sAddress.digits[6] = 0x6,.sAddress.digits[7] = 0x5,
    .sAddress.digits[8] = 0x2,.sAddress.digits[9] = 0x3,
    .sAddress.digits[10] = 0x6,
    .sSubAddress.subaddressType = RIL_CDMA_SMS_SubaddressType::RIL_CDMA_SMS_SUBADDRESS_TYPE_NSAP,
    .sSubAddress.odd = 0x0,
    .sSubAddress.number_of_digits =0x0,
    .uBearerDataLen = 10,
    .aBearerData[0] = 0x0,.aBearerData[1] = 0x3,
    .aBearerData[2] = 0x20,.aBearerData[3] = 0x0,
    .aBearerData[4] = 0x10,.aBearerData[5] = 0x1,
    .aBearerData[6] = 0x3,.aBearerData[7] = 0x10,
    .aBearerData[8] = 0xc,.aBearerData[9] = 0x10};

#define SMS_UT_CDMA_SMS_BODY_BYTE_LEN   29
BYTE    SMS_UT_CDMA_SMS_BODY_BYTE[SMS_UT_CDMA_SMS_BODY_BYTE_LEN] = {
        0X00, 0X00, 0X02, 0X10, 0X02, 0X04, 0X07, 0X02, 0Xe8, 0x69,
        0x69, 0x99, 0x48, 0xd8, 0x06, 0x01, 0x00, 0x08, 0x0a, 0x00,
        0x03, 0x20, 0x00, 0x20, 0x01, 0x03, 0x10, 0x0c, 0x10};

RIL_CDMA_SMS_Ack SMS_UT_CDMA_SMS_ACK = {
    .uErrorClass = RIL_CDMA_SMS_ErrorClass::RIL_CDMA_SMS_NO_ERROR,
    .uSMSCauseCode = 0/* success */};

#define SMS_UT_CDMA_BCST_INFO_NUM 1
RIL_CDMA_BroadcastSmsConfigInfo SMS_UT_CDMA_BCST_SMS_CFG_INFO = {
    .service_category = 0,
    .language = 0,
    .selected = 1/* true */};
sit_sms_cdma_bcst_sms_cfg_item SMS_UT_CDMA_BCST_SMS_CFG_ITEMS[SMS_UT_CDMA_BCST_INFO_NUM] = {
    {(INT16)(SMS_UT_CDMA_BCST_SMS_CFG_INFO.service_category), (BYTE)(SMS_UT_CDMA_BCST_SMS_CFG_INFO.language)}};

RIL_CDMA_SMS_WriteArgs SMS_UT_CDMA_SMS_WRITEARGS = {
    .status = SmsRuimStatusType::RIL_RUIM_STATUS_RECEIVED_UNREAD,
    .message = SMS_UT_CDMA_SMS_MESSAGE};

// enum for unit test
typedef enum{
    SMS_UT_RESULT_SMS_HANDLED = 1
}SmsUtAckResult;

typedef enum{
    SMS_UT_NO_ERROR_CODE = RIL_E_SUCCESS
}SmsUtError;

static UINT8* buildSendSmsRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sms_send_sms_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sms_send_sms_rsp *rsp = (sit_sms_send_sms_rsp*)rspData;
    rsp->msg_ref = SMS_UT_MSG_REF;
    rsp->error_code = SmsUtError::SMS_UT_NO_ERROR_CODE;

    return rspData;
}

static UINT8* buildWriteSmsToSimRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sms_write_sms_to_sim_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sms_write_sms_to_sim_rsp *rsp = (sit_sms_write_sms_to_sim_rsp*)rspData;
    rsp->index = SMS_UT_SIM_IDX;

    return rspData;
}

static UINT8* buildGetBcstSmsCfgRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sms_get_bcst_sms_cfg_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sms_get_bcst_sms_cfg_rsp *rsp = (sit_sms_get_bcst_sms_cfg_rsp*)rspData;
    rsp->bcst_info_num = SMS_UT_BCST_INFO_NUM;
    memset(&(rsp->cfgitem), 0, SMS_UT_BCST_INFO_NUM * sizeof(sit_sms_bcst_sms_cfg_item));

    return rspData;
}

static UINT8* buildGetSmscAddrRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sms_get_smsc_addr_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sms_get_smsc_addr_rsp *rsp = (sit_sms_get_smsc_addr_rsp*)rspData;
    rsp->sca_len = SMS_UT_SMSC_BYTE_LEN;
    for (int i = 0; i < SMS_UT_SMSC_BYTE_LEN; i++) {
        rsp->sca[i] = SMS_UT_SMSC_BYTE[i];
    }

    return rspData;
}

static UINT8* buildGetStoredSmsCountRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sms_get_stored_sms_count_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);

    sit_sms_get_stored_sms_count_rsp *rsp = (sit_sms_get_stored_sms_count_rsp *) rspData;
    rsp->sim_id = SMS_UT_SIM_ID;
    rsp->total_num = SMS_UT_SMS_TOTAL_NUM_ON_SIM;
    rsp->used_num = SMS_UT_SMS_USED_NUM_ON_SIM;

    *datalen = len;

    return rspData;
}

static UINT8* buildCdmaSendSmsRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sms_cdma_send_sms_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sms_cdma_send_sms_rsp *rsp = (sit_sms_cdma_send_sms_rsp *) rspData;
    rsp->msg_ref = (INT16)SMS_UT_MSG_REF;
    rsp->error_class = (BYTE)SMS_UT_CDMA_SMS_ERROR_CLASS;
    rsp->error_code= (BYTE)SmsUtError::SMS_UT_NO_ERROR_CODE;

    return rspData;
}

static UINT8* buildCdmaGetBcstSmsCfgRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sms_cdma_get_bcst_sms_cfg_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sms_cdma_get_bcst_sms_cfg_rsp *rsp = (sit_sms_cdma_get_bcst_sms_cfg_rsp *) rspData;
    rsp->bcst_info_num = SMS_UT_CDMA_BCST_INFO_NUM;
    rsp->cfgitem[0] = SMS_UT_CDMA_BCST_SMS_CFG_ITEMS[0];

    return rspData;
}

static UINT8* buildCdmaWriteSmsToRuimRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sms_cdma_write_sms_to_ruim_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sms_cdma_write_sms_to_ruim_rsp *rsp = (sit_sms_cdma_write_sms_to_ruim_rsp *) rspData;
    rsp->index = SMS_UT_CDMA_SIM_IDX;

    return rspData;
}

static UINT8* buildNewSmsInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *indData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_sms_new_sms_ind);

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_sms_new_sms_ind *ind = (sit_sms_new_sms_ind*)indData;
    ind->msg_tpid = SMS_UT_MSG_REF;
    ind->tpdu_len = SMS_UT_SMS_BODY_BYTE_LEN;
    for (int i = 0; i < SMS_UT_SMS_BODY_BYTE_LEN; i++) {
        ind->tpdu[i] = SMS_UT_SMS_BODY_BYTE[i];
    }

    return indData;
}

static UINT8* buildNewSmsStatusReportInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *indData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_sms_new_sms_status_report_ind);

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_sms_new_sms_status_report_ind *ind = (sit_sms_new_sms_status_report_ind*)indData;
    ind->msg_tpid = SMS_UT_MSG_REF;
    ind->tpdu_len = SMS_UT_SMS_BODY_BYTE_LEN;
    for (int i = 0; i < SMS_UT_SMS_BODY_BYTE_LEN; i++) {
        ind->tpdu[i] = SMS_UT_SMS_BODY_BYTE[i];
    }

    return indData;
}

static UINT8* buildNewSmsOnSimInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *indData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_sms_new_sms_on_sim_ind);

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_sms_new_sms_on_sim_ind *ind = (sit_sms_new_sms_on_sim_ind*)indData;
    ind->index = SMS_UT_SIM_IDX;

    return indData;
}

static UINT8* buildNewBcstSmsInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *indData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_sms_new_bcst_sms_ind);

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_sms_new_bcst_sms_ind *ind = (sit_sms_new_bcst_sms_ind*)indData;
    ind->bcst_msg_len = SMS_UT_SMS_BODY_BYTE_LEN;
    for (int i = 0; i < SMS_UT_SMS_BODY_BYTE_LEN; i++) {
        ind->bcst_msg[i] = SMS_UT_SMS_BODY_BYTE[i];
    }

    return indData;
}

static UINT8* buildCdmaNewSmsInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *indData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_sms_cdma_new_sms_ind);

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_sms_cdma_new_sms_ind *ind = (sit_sms_cdma_new_sms_ind*)indData;
    ind->msg_tpid = SMS_UT_CDMA_SMS_TPID;
    ind->msg_len = SMS_UT_CDMA_SMS_BODY_BYTE_LEN;
    for (int i = 0; i < SMS_UT_CDMA_SMS_BODY_BYTE_LEN; i++) {
        ind->msg[i] = SMS_UT_CDMA_SMS_BODY_BYTE[i];
    }

    return indData;
}

static UINT8* buildCdmaVoiceMsgWaitingInfoInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *indData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_sms_cdma_voice_msg_waiting_info_ind);

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_sms_cdma_voice_msg_waiting_info_ind *ind = (sit_sms_cdma_voice_msg_waiting_info_ind*)indData;
    ind->msg_count = SMS_UT_CDMA_VOICE_MSG_COUNT;

    return indData;
}

// sitrilTestSms class
void SitrilTestSms::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();
}

void SitrilTestSms::TearDown() {
    mSitRilTest = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////
// SET reqeust|response //
TEST_P(SitrilTestSms, sendSms) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SEND_SMS;
    UINT16 sitId = SIT_SEND_SMS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    char *pStrings[2];
    pStrings[0] = (char*)SMS_UT_SMSC_CHAR;
    pStrings[1] = (char*)SMS_UT_SMS_BODY_CHAR;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pStrings, 2*sizeof(char*), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_send_sms_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSendSmsRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(sizeof(RIL_SMS_Response), rilRspDataLen);
    EXPECT_EQ(NULL, ((RIL_SMS_Response *)rilRspData)->ackPDU);
    EXPECT_EQ(SMS_UT_MSG_REF, ((RIL_SMS_Response *)rilRspData)->messageRef);
    EXPECT_EQ(SmsUtError::SMS_UT_NO_ERROR_CODE, ((RIL_SMS_Response *)rilRspData)->errorCode);

    delete pModemData;
    free(rspData);
}


TEST_P(SitrilTestSms, sendSmsExpectMore) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SEND_SMS_EXPECT_MORE;
    UINT16 sitId = SIT_SEND_SMS_EXPECT_MORE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    char *pStrings[2];
    pStrings[0] = (char*)SMS_UT_SMSC_CHAR;
    pStrings[1] = (char*)SMS_UT_SMS_BODY_CHAR;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pStrings, 2*sizeof(char*), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_send_sms_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSendSmsRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(sizeof(RIL_SMS_Response), rilRspDataLen);
    EXPECT_EQ(NULL, ((RIL_SMS_Response *)rilRspData)->ackPDU);
    EXPECT_EQ(SMS_UT_MSG_REF, ((RIL_SMS_Response *)rilRspData)->messageRef);
    EXPECT_EQ(SmsUtError::SMS_UT_NO_ERROR_CODE, ((RIL_SMS_Response *)rilRspData)->errorCode);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, sendSmsAck) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SMS_ACKNOWLEDGE;
    UINT16 sitId = SIT_SEND_SMS_ACK;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    int pInts[2];
    bool success = true;
    pInts[0] = success ? 1 : 0;
    pInts[1] = SmsUtAckResult::SMS_UT_RESULT_SMS_HANDLED;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pInts, 2*sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_send_sms_ack_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, writeSmsToSim) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_WRITE_SMS_TO_SIM;
    UINT16 sitId = SIT_WRITE_SMS_TO_SIM;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    RIL_SMS_WriteArgs args;
    args.status = 0;    // REC UNREAD
    args.smsc = (char*)SMS_UT_SMSC_CHAR;
    args.pdu = (char*)SMS_UT_SMS_BODY_CHAR;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &args, sizeof(args), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_write_sms_to_sim_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildWriteSmsToSimRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(sizeof(int), rilRspDataLen);
    EXPECT_EQ(SMS_UT_SIM_IDX, *((int *)rilRspData));

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, deleteSmsOnSim) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_DELETE_SMS_ON_SIM;
    UINT16 sitId = SIT_DELETE_SMS_ON_SIM;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    int index = SMS_UT_SIM_IDX;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_delete_sms_on_sim_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, getBcstSmsCfg) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG;
    UINT16 sitId = SIT_GET_BCST_SMS_CFG;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_get_bcst_sms_cfg_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    std::unique_ptr<ModemData> pModemData {new ModemData(txIpcRawData, sizeOfModemData)};
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetBcstSmsCfgRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    struct RespHookImpl : public SitrilTest::RilResponseHookBase {
        using SitrilTest::RilResponseHookBase::RilResponseHookBase;

        void onRequestComplete(const void *data, size_t datalen) {
            auto rilRspData = (RIL_GSM_BroadcastSmsConfigInfo **)data;
            EXPECT_EQ(SMS_UT_BCST_INFO_NUM * sizeof(RIL_GSM_BroadcastSmsConfigInfo *), datalen);
            for (int i = 0; i < SMS_UT_BCST_INFO_NUM; i++) {
                RIL_GSM_BroadcastSmsConfigInfo *resp = rilRspData[i];
                EXPECT_EQ((int16_t)0, (int16_t)resp->fromServiceId);
                EXPECT_EQ((int16_t)0, (int16_t)resp->toServiceId);
                EXPECT_EQ((uint8_t)0, (uint8_t)resp->fromCodeScheme);
                EXPECT_EQ((uint8_t)0, (uint8_t)resp->toCodeScheme);
                EXPECT_EQ((uint8_t)0, (uint8_t)resp->selected);
            }
       }
    } hook(*psitRilTest);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    free(rspData);
}

TEST_P(SitrilTestSms, setBcstSmsCfg) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG;
    UINT16 sitId = SIT_SET_BCST_SMS_CFG;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    RIL_GSM_BroadcastSmsConfigInfo gsmBci[SMS_UT_BCST_INFO_NUM];
    RIL_GSM_BroadcastSmsConfigInfo *gsmBciPtrs[SMS_UT_BCST_INFO_NUM];
    for (int i = 0; i < SMS_UT_BCST_INFO_NUM; i++) {
        gsmBciPtrs[i] = &gsmBci[i];
        memset(gsmBci + i, 0, sizeof(RIL_GSM_BroadcastSmsConfigInfo));
    }

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, gsmBciPtrs,
            SMS_UT_BCST_INFO_NUM * sizeof(RIL_GSM_BroadcastSmsConfigInfo *),
            mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_set_bcst_sms_cfg_req::hdr)
            + sizeof(sit_sms_set_bcst_sms_cfg_req::bcst_info_num)
            + (SMS_UT_BCST_INFO_NUM * sizeof(sit_sms_bcst_sms_cfg_item));
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, actBcstSms) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION;
    UINT16 sitId = SIT_ACT_BCST_SMS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    bool isAcivate = true;
    int activate = isAcivate ? 1 : 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &activate, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_act_bcst_sms_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, getSmscAddr) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_SMSC_ADDRESS;
    UINT16 sitId = SIT_GET_SMSC_ADDR;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_get_smsc_addr_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetSmscAddrRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(SMS_UT_SMSC_NUM_CHAR_LEN + 1U, rilRspDataLen);  // rilRspDataLen = smsc len + 1
    int i = 0;
    if (SMS_UT_IS_SMSC_INTERNATIONAL) {
        EXPECT_EQ('+', ((char *)rilRspData)[i]);
        i++;
    }
    for (int j = 0; j < SMS_UT_SMSC_NUM_CHAR_LEN; j++, i++) {
        EXPECT_EQ(SMS_UT_SMSC_NUM_CHAR[j] ,((char *)rilRspData)[i]);
    }

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, setSmscAddr) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_SMSC_ADDRESS;
    UINT16 sitId = SIT_SET_SMSC_ADDR;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    char *smscAddr = (char*)SMS_UT_SMSC_NUM_CHAR;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, smscAddr, sizeof(char *), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_set_smsc_addr_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildErrorRsp(sitId, sitToken, &rspDataLen, 2);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, sendSmsMemStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_REPORT_SMS_MEMORY_STATUS;
    UINT16 sitId = SIT_SEND_SMS_MEM_STATUS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    bool isAvailable = true;
    int available = isAvailable ? 1 : 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &available, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_send_sms_mem_status_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, sendAckIncomingSms) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU;
    UINT16 sitId = SIT_SEND_ACK_INCOMING_SMS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    char *pStrings[2];
    bool success = true;
    pStrings[0] = (char*)(success ? "1" : "0");
    pStrings[1] = (char*)SMS_UT_SMS_BODY_CHAR;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pStrings, 2 * sizeof(char *), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_send_ack_incoming_sms_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, getStoredSmsCount) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_SMS_STORAGE_ON_SIM;
    UINT16 sitId = SIT_GET_STORED_SMS_COUNT;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    int simId = SMS_UT_SIM_ID;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &simId, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_get_stored_sms_count_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetStoredSmsCountRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(sizeof(RIL_StorageStatus), rilRspDataLen);
    EXPECT_EQ(SMS_UT_SIM_ID, ((RIL_StorageStatus *)rilRspData)->sim_id);
    EXPECT_EQ(SMS_UT_SMS_TOTAL_NUM_ON_SIM, ((RIL_StorageStatus *)rilRspData)->total_num);
    EXPECT_EQ(SMS_UT_SMS_USED_NUM_ON_SIM, ((RIL_StorageStatus *)rilRspData)->used_num);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, cdmaSendSms) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SEND_SMS;
    UINT16 sitId = SIT_CDMA_SEND_SMS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &SMS_UT_CDMA_SMS_MESSAGE, sizeof(SMS_UT_CDMA_SMS_MESSAGE), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_cdma_send_sms_req::hdr)
            + sizeof(sit_sms_cdma_send_sms_req::msg_len)
            + SMS_UT_CDMA_SMS_BODY_BYTE_LEN;
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildCdmaSendSmsRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(sizeof(RIL_SMS_Response), rilRspDataLen);
    EXPECT_EQ(SMS_UT_MSG_REF, ((RIL_SMS_Response *)rilRspData)->messageRef);
    EXPECT_EQ(NULL, ((RIL_SMS_Response *)rilRspData)->ackPDU);
    EXPECT_EQ(SmsUtError::SMS_UT_NO_ERROR_CODE, ((RIL_SMS_Response *)rilRspData)->errorCode);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, cdmaSendSmsAck) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE;
    UINT16 sitId = SIT_CDMA_SEND_SMS_ACK;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &SMS_UT_CDMA_SMS_ACK, sizeof(SMS_UT_CDMA_SMS_ACK), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_cdma_send_sms_ack_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, cdmaGetBcstSmsCfg) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG;
    UINT16 sitId = SIT_CDMA_GET_BCST_SMS_CFG;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, NULL, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_cdma_get_bcst_sms_cfg_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    std::unique_ptr<ModemData> pModemData {new ModemData(txIpcRawData, sizeOfModemData)};

    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildCdmaGetBcstSmsCfgRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    struct RespHookImpl : public SitrilTest::RilResponseHookBase {
        using SitrilTest::RilResponseHookBase::RilResponseHookBase;

        void onRequestComplete(const void *data, size_t datalen) {
            auto rilRspData = (RIL_CDMA_BroadcastSmsConfigInfo **) data;
            EXPECT_EQ(sizeof(RIL_CDMA_BroadcastSmsConfigInfo *) * SMS_UT_CDMA_BCST_INFO_NUM, datalen);
            for (int i = 0; i < SMS_UT_CDMA_BCST_INFO_NUM; i++) {
                RIL_CDMA_BroadcastSmsConfigInfo *resp = rilRspData[i];
                EXPECT_EQ(SMS_UT_CDMA_BCST_SMS_CFG_ITEMS[i].svc_category, resp->service_category);
                EXPECT_EQ(SMS_UT_CDMA_BCST_SMS_CFG_ITEMS[i].language, resp->language);
            }
       }
    } hook(*psitRilTest);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    free(rspData);
}

TEST_P(SitrilTestSms, cdmaSetBcstSmsCfg) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG;
    UINT16 sitId = SIT_CDMA_SET_BCST_SMS_CFG;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    RIL_CDMA_BroadcastSmsConfigInfo cdmaBci[SMS_UT_CDMA_BCST_INFO_NUM];
    RIL_CDMA_BroadcastSmsConfigInfo *cdmaBciPtrs[SMS_UT_CDMA_BCST_INFO_NUM];
    for (int i = 0; i < SMS_UT_CDMA_BCST_INFO_NUM; i++) {
        cdmaBciPtrs[i] = &cdmaBci[i];
        cdmaBci[i].service_category = SMS_UT_CDMA_BCST_SMS_CFG_ITEMS[i].svc_category;
        cdmaBci[i].language = SMS_UT_CDMA_BCST_SMS_CFG_ITEMS[i].language;
        cdmaBci[i].selected = 1/* true */;
    }

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, cdmaBciPtrs, SMS_UT_CDMA_BCST_INFO_NUM * sizeof(RIL_CDMA_BroadcastSmsConfigInfo *), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_cdma_set_bcst_sms_cfg_req::hdr)
            + sizeof(sit_sms_cdma_set_bcst_sms_cfg_req::bcst_info_num)
            + (SMS_UT_CDMA_BCST_INFO_NUM * sizeof(sit_sms_cdma_bcst_sms_cfg_item));
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, cdmaActBcstSms) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION;
    UINT16 sitId = SIT_CDMA_ACT_BCST_SMS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    bool isAcivate = true;
    int activate = isAcivate ? 1 : 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &activate, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_cdma_act_bcst_sms_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, cdmaWriteSmsToRuim) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM;
    UINT16 sitId = SIT_CDMA_WRITE_SMS_TO_RUIM;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &SMS_UT_CDMA_SMS_WRITEARGS, sizeof(SMS_UT_CDMA_SMS_WRITEARGS), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_cdma_write_sms_to_ruim_req::hdr)
            + sizeof(sit_sms_cdma_write_sms_to_ruim_req::status)
            + sizeof(sit_sms_cdma_write_sms_to_ruim_req::msg_len)
            + SMS_UT_CDMA_SMS_BODY_BYTE_LEN;
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildCdmaWriteSmsToRuimRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(sizeof(int), rilRspDataLen);
    EXPECT_EQ(SMS_UT_CDMA_SIM_IDX, *((int *)rilRspData));

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSms, cdmaDeleteSmsOnRuim) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM;
    UINT16 sitId = SIT_CDMA_DELETE_SMS_ON_RUIM;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test REQ */
    int index = SMS_UT_CDMA_SIM_IDX;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sms_cdma_delete_sms_on_ruim_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

////////////////////////////////////////////////////////////////////////////////////////
// Indication //
TEST_P(SitrilTestSms, newSmsInd) {
    // Should be run After getSmscAddr test
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_NEW_SMS;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* indData = nullptr;
    UINT32 indDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    indData = buildNewSmsInd(sitId, &indDataLen);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_NEW_SMS, indData, indDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ SMS_UT_SMS_BODY_CHAR_LEN, indLen);
    for (int i = 0; i < SMS_UT_SMS_BODY_CHAR_LEN; i++) {
        EXPECT_EQ(SMS_UT_SMS_BODY_CHAR[i], ((char *)rilIndData)[i]);
    }

    free(indData);
}

TEST_P(SitrilTestSms, newSmsStatusReportInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_NEW_SMS_STATUS_REPORT;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* indData = nullptr;
    UINT32 indDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    indData = buildNewSmsStatusReportInd(sitId, &indDataLen);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT, indData, indDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ SMS_UT_SMS_BODY_CHAR_LEN, indLen);
    for (int i = 0; i < SMS_UT_SMS_BODY_CHAR_LEN; i++) {
        EXPECT_EQ(SMS_UT_SMS_BODY_CHAR[i], ((char *)rilIndData)[i]);
    }

    free(indData);
}

TEST_P(SitrilTestSms, newSmsOnSimInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_NEW_SMS_ON_SIM;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* indData = nullptr;
    UINT32 indDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    indData = buildNewSmsOnSimInd(sitId, &indDataLen);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM, indData, indDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ(sizeof(sit_sms_new_sms_on_sim_ind::index), indLen);
    EXPECT_EQ(SMS_UT_SIM_IDX, *((int *)rilIndData));

    free(indData);
}

TEST_P(SitrilTestSms, simSmsStorageFullInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SIM_SMS_STORAGE_FULL;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* indData = nullptr;
    UINT32 indDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    indData = buildEmptyInd(sitId, &indDataLen);
    psitRilTest->processInd(RIL_UNSOL_SIM_SMS_STORAGE_FULL, indData, indDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);

    free(indData);
}

TEST_P(SitrilTestSms, newBcstSmsInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_NEW_BCST_SMS;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* indData = nullptr;
    UINT32 indDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    indData = buildNewBcstSmsInd(sitId, &indDataLen);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS, indData, indDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ SMS_UT_SMS_BODY_BYTE_LEN, indLen);
    for (int i = 0; i < SMS_UT_SMS_BODY_BYTE_LEN; i++) {
        EXPECT_EQ(SMS_UT_SMS_BODY_BYTE[i], ((char *)rilIndData)[i]);
    }

    free(indData);
}

TEST_P(SitrilTestSms, cdmaNewSmsInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CDMA_NEW_SMS;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* indData = nullptr;
    UINT32 indDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    indData = buildCdmaNewSmsInd(sitId, &indDataLen);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_CDMA_NEW_SMS, indData, indDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    //printf("cdmaNewSmsInd indLen=%d\n", indLen); RIL_CDMA_SMS_Message = 372; solve the size
    //EXPECT_EQ(SMS_UT_SMS_BODY_BYTE_LEN, indLen);
    //printf("cdmaNewSmsInd rilIndData.uTeleserviceID=0x%x\n", ((RIL_CDMA_SMS_Message *)rilIndData)->uTeleserviceID);
    //printf("cdmaNewSmsInd rilIndData.bIsServicePresent=0x%x\n", ((RIL_CDMA_SMS_Message *)rilIndData)->bIsServicePresent);
    //printf("cdmaNewSmsInd rilIndData.uBearerDataLen=0x%x\n", ((RIL_CDMA_SMS_Message *)rilIndData)->uBearerDataLen);

    free(indData);
}

TEST_P(SitrilTestSms, cdmaRuimSmsStorageFullInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CDMA_RUIM_SMS_STORAGE_FULL;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* indData = nullptr;
    UINT32 indDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    indData = buildEmptyInd(sitId, &indDataLen);
    psitRilTest->processInd(RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL, indData, indDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);

    free(indData);
}

TEST_P(SitrilTestSms, cdmaVoiceMsgWaitingInfoInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CDMA_VOICE_MSG_WAITING_INFO;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* indData = nullptr;
    UINT32 indDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    indData = buildCdmaVoiceMsgWaitingInfoInd(sitId, &indDataLen);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_CDMA_NEW_SMS, indData, indDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)(sizeof(RIL_CDMA_SMS_Message)), indLen);
    EXPECT_EQ(TELESERVICE_IDENTIFIER_MWI, ((RIL_CDMA_SMS_Message *)rilIndData)->uTeleserviceID);
    EXPECT_EQ(1, ((RIL_CDMA_SMS_Message *)rilIndData)->uBearerDataLen);
    EXPECT_EQ(SMS_UT_CDMA_VOICE_MSG_COUNT, (((RIL_CDMA_SMS_Message *)rilIndData)->aBearerData)[0]);

    free(indData);
}
