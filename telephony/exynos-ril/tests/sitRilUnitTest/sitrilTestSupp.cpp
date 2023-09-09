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

#include <android/log.h>
#include "sitrilTestSupp.h"
#include "testUtil.h"

#define DBG true
#define LOG_TAG    "SUPP_TEST"
#define LOGI(...)  if(DBG) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#define BOOL_TO_INT(x) (x ? 1 : 0)


#define TEST_STR_MAX_SIZE  (40)

char FORWARDED_NUMBER[TEST_STR_MAX_SIZE] =  "123456789";
char CB_FACILITY_BA_ALL[TEST_STR_MAX_SIZE] =  "330";
char OLD_PWD[TEST_STR_MAX_SIZE] =  "1234";
char NEW_PWD[TEST_STR_MAX_SIZE] =  "9876";
char NEW_PWD_AGAIN[TEST_STR_MAX_SIZE] =  "9876";
char USSD_STRING[TEST_STR_MAX_SIZE] =  "*#12345#";
char USSD_DATA_IND[TEST_STR_MAX_SIZE] =  "Ussd data";
char USSD_REQUEST_STR[TEST_STR_MAX_SIZE] =  "1";

// sitrilTestSupp class
void SitrilTestSupp::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();
}

void SitrilTestSupp::TearDown() {
    mSitRilTest = nullptr;
}

static void cleanup(SitrilTest *psitRilTest, ModemData *pModemData, UINT8* rspData) {
    if (pModemData != nullptr) {
        delete pModemData;
    }
    if (rspData != nullptr) {
        free(rspData);
    }
    psitRilTest->resetTestData();
}

static void sendErrorRsp(SitrilTest *psitRilTest, int testRilCmdId, INT16 sitId, UINT sitToken, BYTE error) {
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    rspData = buildErrorRsp(sitId, sitToken, &rspDataLen, error);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
}

static UINT8* buildGetCallWaitingRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_get_call_waiting_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_ss_get_call_waiting_rsp* rsp = (sit_ss_get_call_waiting_rsp*) rspData;
    rsp->status = 1;
    rsp->service_class = SIT_SS_SERVICE_CLASS_VOICE;
    return rspData;
}

static UINT8* buildSetCallWaitingRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_set_call_waiting_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildGetCallForwardStatusRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, UINT32 cfNum) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_get_call_forward_status_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_ss_get_call_forward_status_rsp* rsp = (sit_ss_get_call_forward_status_rsp*) rspData;
    rsp->call_forward_num = cfNum;
    rsp->record[0].status = SIT_SS_CALL_FARWARD_STATUS_ACTIVE;
    rsp->record[0].reason = SIT_SS_CALL_FORWARD_READON_UNCONDITIONAL;
    rsp->record[0].num_len = strlen(FORWARDED_NUMBER);
    strncpy((char *)rsp->record[0].number, FORWARDED_NUMBER, rsp->record[0].num_len);
    rsp->record[0].service_class = SIT_SS_SERVICE_CLASS_VOICE;
    return rspData;

}

static UINT8* buildSetCallForwardRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_set_call_forward_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildGetClirRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, INT32 clirStatus) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_get_clir_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_ss_get_clir_rsp* rsp = (sit_ss_get_clir_rsp*) rspData;
    rsp->clir_status = clirStatus;

    return rspData;
}

static UINT8* buildGetClipRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_get_clip_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_ss_get_clip_rsp* rsp = (sit_ss_get_clip_rsp*) rspData;
    rsp->status = SIT_SS_GET_CLIP_STATUS_UNKNOWN;

    return rspData;
}

static UINT8* buildGetColrRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_get_colr_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_ss_get_colr_rsp* rsp = (sit_ss_get_colr_rsp*) rspData;
    rsp->status = 2;

    return rspData;
}

static UINT8* buildGetColpRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_get_colp_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_ss_get_colp_rsp* rsp = (sit_ss_get_colp_rsp*) rspData;
    rsp->status = 2;

    return rspData;
}

static UINT8* buildSetBarringPasswordOverMmiRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_change_barring_pwd_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildSendUssdRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_send_ussd_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildCancelPendingUssdRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_cancel_ussd_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildOnUssdInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_ussd_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_ss_ussd_ind *ind = (sit_ss_ussd_ind*) rspData;
    ind->status = SIT_SS_USSD_IND_STATUS_USSD_REQUEST;
    ind->dcs = 0;
    ind->ussd_len = strlen(USSD_DATA_IND);
    strncpy((char *)ind->ussd, USSD_DATA_IND, ind->ussd_len);

    return rspData;
}

static UINT8* buildSuppSvcNotifyInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_supp_svc_notification_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_ss_supp_svc_notification_ind *ind = (sit_ss_supp_svc_notification_ind*) rspData;
    ind->noti_type = SIT_SS_SSNOTI_TYPE_MT;

    return rspData;
}

static UINT8* buildOnSupplementaryServiceIndicationInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_on_ss_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_ss_on_ss_ind *ind = (sit_ss_on_ss_ind*) rspData;
    ind->service_type = 0;

    return rspData;
}

TEST_P(SitrilTestSupp, getCallWaiting) {
    LOGI("====== START getCallWaiting ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_QUERY_CALL_WAITING;
    UINT16 sitId = SIT_GET_CALL_WAITING;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    int serviceClass = RIL_SS_CLASS_VOICE;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &serviceClass, sizeof(int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_get_call_waiting_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetCallWaitingRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    int *pInts = (int*) rilRspData;
    EXPECT_EQ(pInts[0], 1);
    EXPECT_EQ(pInts[1], RIL_SS_CLASS_VOICE);

    /* ERROR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &serviceClass, sizeof(int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, setCallWaiting) {
    LOGI("====== START setCallWaiting ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_SET_CALL_WAITING;
    UINT16 sitId = SIT_SET_CALL_WAITING;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    int pInts[2];
    pInts[0] = BOOL_TO_INT(true);
    pInts[1] = RIL_SS_CLASS_VOICE;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pInts, 2 * sizeof(int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_set_call_waiting_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSetCallWaitingRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, getCallForwardStatus) {
    LOGI("====== START getCallForwardStatus ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_QUERY_CALL_FORWARD_STATUS;
    UINT16 sitId = SIT_GET_CALL_FORWARD_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    RIL_CallForwardInfo reqData;
    reqData.reason = RIL_SS_CF_REASON_UNCONDITIONAL;
    reqData.serviceClass = RIL_SS_CLASS_VOICE;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData,
            sizeof(RIL_CallForwardInfo), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_get_call_forward_status_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    UINT32 cfNum = 1;
    rspData = buildGetCallForwardStatusRsp(sitId, sitToken, &rspDataLen, cfNum);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    RIL_CallForwardInfo* respCfData = ((RIL_CallForwardInfo **) rilRspData)[0];
    EXPECT_EQ(respCfData->status, RIL_SS_STATUS_ACTIVE);
    EXPECT_EQ(respCfData->reason, RIL_SS_CF_REASON_UNCONDITIONAL);
    EXPECT_STREQ(respCfData->number, FORWARDED_NUMBER);

    /* MaxCfNum on REQ */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData,
            sizeof(RIL_CallForwardInfo), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    cfNum = MAX_CALL_FORWARD_STATUS_NUM;
    rspData = buildGetCallForwardStatusRsp(sitId, sitToken, &rspDataLen, cfNum);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_INTERNAL_ERR, psitRilTest->getRilRspErrno());

    /* ERROR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData,
            sizeof(RIL_CallForwardInfo), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, setCallForward) {
    LOGI("====== START setCallForward ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_SET_CALL_FORWARD;
    UINT16 sitId = SIT_SET_CALL_FORWARD;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    RIL_CallForwardInfo reqData;
    reqData.status = RIL_SS_STATUS_ACTIVE;
    reqData.reason = RIL_SS_CF_REASON_UNCONDITIONAL;
    reqData.serviceClass = RIL_SS_CLASS_VOICE;
    reqData.number = FORWARDED_NUMBER;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData,
            sizeof(RIL_CallForwardInfo), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_set_call_forward_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSetCallForwardRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    /* ERROR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData,
            sizeof(RIL_CallForwardInfo), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, getClir) {
    LOGI("====== START getClir ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_GET_CLIR;
    UINT16 sitId = SIT_GET_CLIR;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_get_clir_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    INT32 clirStatus = SIT_SS_GET_CLIR_STATUS_UNKNOWN;
    rspData = buildGetClirRsp(sitId, sitToken, &rspDataLen, clirStatus);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    int *pInts = (int*) rilRspData;
    EXPECT_EQ(pInts[0], CLIR_DEFAULT);
    EXPECT_EQ(pInts[1], CLIR_UNKNOWN);

    /* CLIR_PROVISIONED on RSP */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    clirStatus = SIT_SS_GET_CLIR_STATUS_PROVISIONED;
    rspData = buildGetClirRsp(sitId, sitToken, &rspDataLen, clirStatus);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    pInts = (int*) rilRspData;
    EXPECT_EQ(pInts[0], CLIR_INVOCATION);
    EXPECT_EQ(pInts[1], CLIR_PROVISIONED);

    /* CLIR_TEMP_ALLOWED on RSP */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    clirStatus = SIT_SS_GET_CLIR_STATUS_TEMP_ALLOWED;
    rspData = buildGetClirRsp(sitId, sitToken, &rspDataLen, clirStatus);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    pInts = (int*) rilRspData;
    EXPECT_EQ(pInts[1], CLIR_TEMP_ALLOWED);

    /* ERROR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);
    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, setClir) {
    LOGI("====== START setClir ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_SET_CLIR;

    /* REQ */
    int pInt = CLIR_DEFAULT;
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId,
            &pInt, sizeof(int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    /* RSP */
    UINT32 rilRspDataLen = 0;
    void* rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
}

TEST_P(SitrilTestSupp, getClip) {
    LOGI("====== START getClip ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_QUERY_CLIP;
    UINT16 sitId = SIT_GET_CLIP;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_get_clip_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetClipRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    int *pInt = (int*) rilRspData;
    EXPECT_EQ(*pInt, CLIP_UNKNOWN);

    /* ERROR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, queryCOLR) {
    LOGI("====== START queryCOLR ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_QUERY_COLR;
    UINT16 sitId = SIT_GET_COLR;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_get_colr_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetColrRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    int *pInt = (int*) rilRspData;
    EXPECT_EQ(*pInt, 2);

    /* ERROR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, queryCOLP) {
    LOGI("====== START queryCOLP ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_QUERY_COLP;
    UINT16 sitId = SIT_GET_COLP;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId,
            nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_get_colp_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetColpRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    int *pInt = (int*) rilRspData;
    EXPECT_EQ(*pInt, 2);

    /* ERROR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, setBarringPasswordOverMmi) {
    LOGI("====== START setBarringPasswordOverMmi ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_CHANGE_BARRING_PASSWORD_OVER_MMI;
    UINT16 sitId = SIT_CHG_BARRING_PWD;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    char *pStrings[4];
    pStrings[0] = (char*)CB_FACILITY_BA_ALL;
    pStrings[1] = (char*)OLD_PWD;
    pStrings[2] = (char*)NEW_PWD;
    pStrings[3] = (char*)NEW_PWD_AGAIN;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pStrings, 4 * sizeof(char*), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_change_barring_pwd_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSetBarringPasswordOverMmiRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    /* RIL_E_CANCELLED on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pStrings, 4 * sizeof(char*), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_PASSWORD_INCORRECT);

    /* RIL_E_INTERNAL_ERR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pStrings, 4 * sizeof(char*), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_UNDEFINED_CMD);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, sendUssd) {
    LOGI("====== START sendUssd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_SEND_USSD;
    UINT16 sitId = SIT_SEND_USSD;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    char *pString = (char*)USSD_STRING;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pString, sizeof(char*), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_send_ussd_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSendUssdRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    /* ERROR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pString, sizeof(char*), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    /* RIL_E_MODEM_ERR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pString, sizeof(char*), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_UNDEFINED_CMD);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, cancelPendingUssd) {
    LOGI("====== START cancelPendingUssd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_CANCEL_USSD;
    UINT16 sitId = SIT_CANCEL_USSD;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* REQ */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_cancel_ussd_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* RSP */
    sitToken = pModemData->GetToken();
    rspData = buildCancelPendingUssdRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    /* ERROR on RSP - error code */
    cleanup(psitRilTest, pModemData, rspData);
    pModemData = nullptr;
    rspData = nullptr;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSupp, onUssdInd) {
    LOGI("====== START onUssdInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    UINT16 sitId = SIT_IND_ON_USSD;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* IND */
    rspData = buildOnUssdInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_ON_USSD, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSupp, suppSvcNotifyInd) {
    LOGI("====== START suppSvcNotifyInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    UINT16 sitId = SIT_IND_SUPP_SVC_NOTIFICATION;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* IND */
    rspData = buildSuppSvcNotifyInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_SUPP_SVC_NOTIFICATION, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);

    RIL_SuppSvcNotification *pCur = (RIL_SuppSvcNotification*) rilIndData;
    EXPECT_EQ(pCur->notificationType, RIL_SSNOTI_TYPE_MT);

    free(rspData);
}

TEST_P(SitrilTestSupp, onSupplementaryServiceIndicationInd) {
    LOGI("====== START onSupplementaryServiceIndicationInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    UINT16 sitId = SIT_IND_ON_SS;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* IND */
    rspData = buildOnSupplementaryServiceIndicationInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_ON_SS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);

    RIL_StkCcUnsolSsResponse *pCur = (RIL_StkCcUnsolSsResponse*) rilIndData;
    EXPECT_EQ(pCur->serviceType, SS_CFU);

    free(rspData);
}
