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
#include "sitrilTestCs.h"
#include "sitrilTestSim.h"
#include <librilutils/systemproperty.h>
#include "testUtil.h"

#define DBG false
#define LOG_TAG    "CS_TEST"
#define LOGI(...)  if(DBG) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#define TEST_STR_MAX_SIZE  (40)
#define INVALID_DATA (0xff)

char CALL_NAME[TEST_STR_MAX_SIZE] = "CS TEST";
char INTERNATIONAL_NORMAL_CALL_NUMBER[TEST_STR_MAX_SIZE] =  "+123456789";
char NORMAL_CALL_NUMBER[TEST_STR_MAX_SIZE] =  "123456789";
char FORWARD_CALL_NUMBER[TEST_STR_MAX_SIZE] =  "987654321";
char EMERGENCY_CALL_NUMBER[TEST_STR_MAX_SIZE] =  "112";
char EMERGENCY_CALL_NUMBER2[TEST_STR_MAX_SIZE] =  "115";
char EMERGENCY_SUB_SERVICE[TEST_STR_MAX_SIZE] = "police";
char EMERGENCY_SUB_SERVICE2[TEST_STR_MAX_SIZE] = "gas";
char TEST_MCC[TEST_STR_MAX_SIZE] =  "123";
char TEST_MNC[TEST_STR_MAX_SIZE] =  "456";
char UUS_DATA[TEST_STR_MAX_SIZE] =  "TEST";
char VENDOR_RIL_DB_ECC_ID_TYPE[TEST_STR_MAX_SIZE] = "persist.vendor.ril.db_ecc.id.type";
char PROPERTY_ICC_OPERATOR_NUMERIC[TEST_STR_MAX_SIZE] = "gsm.sim.operator.numeric";

enum {
    DB_ECC_ID_TYPE_NO_CUSTOMIZATION = 0x00,
    DB_ECC_ID_TYPE_SIM_BASED_ONLY,
    DB_ECC_ID_TYPE_NET_BASED_ONLY,
    DB_ECC_ID_TYPE_SIM_PREFER,
    DB_ECC_ID_TYPE_NET_PREFER,
    DB_ECC_ID_TYPE_SIM_NET_COMBINE,
};

enum {
    HAL_VERSION_V1_1 = 1,
    HAL_VERSION_V1_2 = 2,
    HAL_VERSION_V1_3 = 3,
    HAL_VERSION_V1_4 = 4,
    HAL_VERSION_V1_5 = 5,
    HAL_VERSION_V1_6 = 6,
};

static void cleanup(SitrilTest *psitRilTest, UINT8* rspData, ModemData *pModemData = nullptr) {
    if (pModemData != nullptr) {
        delete pModemData;
    }
    if (rspData != nullptr) {
        free(rspData);
    }
    psitRilTest->resetTestData();
}

static void sendEmptyRsp(SitrilTest *psitRilTest, UINT16 sitId, UINT sitToken, int testRilCmdId) {
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);

    cleanup(psitRilTest, rspData);
    rspData = nullptr;
}

static UINT8* buildGetVoiceRegistrationRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, int rat, int state) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_cs_reg_state_v1_1)+sizeof(RCM_HEADER);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_cs_reg_state_v1_1 *rsp = (sit_cs_reg_state_v1_1*)(rspData + sizeof(RCM_HEADER));
    rsp->reg_state = state;
    rsp->rej_cause = SIT_NET_REJ_CAUSE_GENERAL;
    rsp->rat = rat;

    return rspData;
}

static void sendGetVoiceRegistration(SitrilTest *psitRilTest, int phoneIndex, int rat, int state) {
    LOGI("====== START sendGetVoiceRegistration ======");
    int testRilCmdId = RIL_REQUEST_VOICE_REGISTRATION_STATE;
    UINT16 sitId = SIT_GET_CS_REG_STATE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    //////////////////////////////////////////////
    // RIL_REQUEST_VOICE_REGISTRATION_STATE REQ //
    //////////////////////////////////////////////
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    //////////////////////////////////////////////
    // RIL_REQUEST_VOICE_REGISTRATION_STATE RSP //
    //////////////////////////////////////////////
    sitToken = pModemData->GetToken();
    rspData = buildGetVoiceRegistrationRsp(sitId, sitToken, &rspDataLen, rat, state);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    LOGI("====== END sendGetVoiceRegistration ======");
}

typedef enum {
    CALL_LIST_INVALID_LENGTH,
    CALL_LIST_INVALID_NUM,
    CALL_LIST_UNKNOWN,
} CALLLIST_RSP_TYPE;

static UINT8* buildGetCurCallListRsp_V1_1(INT16 sitId, UINT sitToken, UINT32 *dataLen,
                                          UINT32 callCount, BYTE state) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)(sizeof(RCM_HEADER) + sizeof(INT32) + (sizeof(sit_call_info_type_v1_1) * callCount));
    rspData = allocRspModemMem(len, sitId, sitToken);
    *dataLen = len;

    sit_call_get_current_calls_rsp_v1_1 *rsp = (sit_call_get_current_calls_rsp_v1_1*)rspData;
    // need to make verifying data
    rsp->number = callCount;
    for (UINT32 i = 0; i < callCount; i++) {
        rsp->record[i].state = state;
        rsp->record[i].call_type = SIT_CALL_CALL_TYPE_CDMA_EMERGENCY;
        rsp->record[i].num_len = strlen(NORMAL_CALL_NUMBER);
        strncpy((char *)rsp->record[i].num, NORMAL_CALL_NUMBER, rsp->record[i].num_len);
        rsp->record[i].name_len = strlen(CALL_NAME);
        strncpy((char *)rsp->record[i].name, CALL_NAME, rsp->record[i].name_len);
        rsp->record[i].uus_type = SIT_CALL_UUS_DCS_USP;
        rsp->record[i].uus_dcs = SIT_CALL_UUS_TYPE_TYPE1_REQUIRED;
        rsp->record[i].uus_data_len = strlen(UUS_DATA);
        strncpy((char *)rsp->record[i].uus_data, UUS_DATA, strlen(UUS_DATA));
    }

    return rspData;
}

static void sendGetCurCallList_V1_1(SitrilTest *psitRilTest, UINT32 callCount, BYTE state, int phoneIndex) {
    int testRilCmdId = RIL_REQUEST_GET_CURRENT_CALLS;
    UINT16 sitId = SIT_GET_CURRENT_CALLS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS REQ //
    ///////////////////////////////////////
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS RSP //
    ///////////////////////////////////////
    sitToken = pModemData->GetToken();
    rspData = buildGetCurCallListRsp_V1_1(sitId, sitToken, &rspDataLen, callCount, state);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    for (UINT32 i = 0; i < callCount; i++) {
        RIL_Call *respCalls_V1_1 = ((RIL_Call **)rilRspData)[i];
        EXPECT_EQ(respCalls_V1_1->state, state);
        EXPECT_STREQ(respCalls_V1_1->number, NORMAL_CALL_NUMBER);
    }

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
}

static UINT8* buildInvalidGetCurCallListRsp_V1_1(INT16 sitId, UINT sitToken, UINT32 *dataLen,
                                                int rspType) {
    int callListCount = (rspType == CALL_LIST_INVALID_NUM ? 10 : 1);
    UINT8 *rspData = nullptr;

    int extraLen = (rspType == CALL_LIST_INVALID_LENGTH ? -1 :
                    (sizeof(sit_call_info_type_v1_1) * callListCount));
    UINT16 len = (UINT16)(sizeof(RCM_HEADER) + sizeof(INT32) + extraLen);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *dataLen = len;

    sit_call_get_current_calls_rsp_v1_1 *rsp = (sit_call_get_current_calls_rsp_v1_1*)rspData;
    rsp->number = callListCount;
    if (rspType == CALL_LIST_UNKNOWN) {
        rsp->record[0].number_presentation = SIT_CALL_PRESENTATION_RESTRICTED;
        rsp->record[0].name_presentation = SIT_CALL_PRESENTATION_RESTRICTED;
    }

    return rspData;
}

static void sendInvalidGetCurCallList_V1_1(SitrilTest *psitRilTest, int rspType,
                                            RIL_Errno expectResult, int phoneIndex) {
    int testRilCmdId = RIL_REQUEST_GET_CURRENT_CALLS;
    UINT16 sitId = SIT_GET_CURRENT_CALLS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS REQ //
    ///////////////////////////////////////
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS RSP //
    ///////////////////////////////////////
    sitToken = pModemData->GetToken();
    rspData = buildInvalidGetCurCallListRsp_V1_1(sitId, sitToken, &rspDataLen, rspType);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ(expectResult, psitRilTest->getRilRspErrno());

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
}

static UINT8* buildGetCurCallListRsp_V1_2(INT16 sitId, UINT sitToken, UINT32 *dataLen,
                                          UINT32 callCount, BYTE state) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)(sizeof(RCM_HEADER) + sizeof(INT32) + (sizeof(sit_call_info_type_v1_2) * callCount));
    rspData = allocRspModemMem(len, sitId, sitToken);
    *dataLen = len;
    LOGI("buildGetCurCallListRsp_V1_2 callCount:%d, state:%d, len: %d", callCount, state, len);

    sit_call_get_current_calls_rsp_v1_2 *rsp = (sit_call_get_current_calls_rsp_v1_2*)rspData;
    rsp->number = callCount;
    for (UINT32 i = 0; i < callCount; i++) {
        rsp->record[i].state = state;
        rsp->record[i].call_type = SIT_CALL_CALL_TYPE_CDMA_EMERGENCY;
        rsp->record[i].num_len = strlen(NORMAL_CALL_NUMBER);
        rsp->record[i].name_len = strlen(CALL_NAME);
        strncpy((char *)rsp->record[i].num, NORMAL_CALL_NUMBER, rsp->record[i].num_len);
        strncpy((char *)rsp->record[i].name, CALL_NAME, rsp->record[i].name_len);
        rsp->record[i].uus_type = SIT_CALL_UUS_DCS_USP;
        rsp->record[i].uus_dcs = SIT_CALL_UUS_TYPE_TYPE1_REQUIRED;
        rsp->record[i].uus_data_len = strlen(UUS_DATA);
        strncpy((char *)rsp->record[i].uus_data, UUS_DATA, strlen(UUS_DATA));
    }

    return rspData;
}

static UINT8* buildInvalidGetCurCallListRsp_V1_2(INT16 sitId, UINT sitToken, UINT32 *dataLen,
                                                int rspType, BYTE error) {
    int callListCount = (rspType == CALL_LIST_INVALID_NUM ? 10 : 1);
    UINT8 *rspData = nullptr;

    int extraLen = (rspType == CALL_LIST_INVALID_LENGTH ? 1 :
                    (sizeof(sit_call_info_type_v1_2) * callListCount));
    UINT16 len = (UINT16)(sizeof(RCM_HEADER) + sizeof(INT32) + extraLen);
    rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
    *dataLen = len;

    sit_call_get_current_calls_rsp_v1_2 *rsp = (sit_call_get_current_calls_rsp_v1_2*)rspData;
    rsp->number = callListCount;
    if (rspType == CALL_LIST_UNKNOWN) {
        rsp->record[0].number_presentation = SIT_CALL_PRESENTATION_RESTRICTED;
        rsp->record[0].name_presentation = SIT_CALL_PRESENTATION_RESTRICTED;
    }

    return rspData;
}

static void sendGetCurCallList_V1_2(SitrilTest *psitRilTest, UINT32 callCount, BYTE state, int phoneIndex) {
    int testRilCmdId = RIL_REQUEST_GET_CURRENT_CALLS;
    UINT16 sitId = SIT_GET_CURRENT_CALLS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS REQ //
    ///////////////////////////////////////
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS RSP //
    ///////////////////////////////////////
    sitToken = pModemData->GetToken();
    rspData = buildGetCurCallListRsp_V1_2(sitId, sitToken, &rspDataLen, callCount, state);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    for (UINT32 i = 0; i < callCount; i++) {
        RIL_Call_V1_2 *respCalls_V1_2 = ((RIL_Call_V1_2 **)rilRspData)[i];
        EXPECT_EQ(respCalls_V1_2->state, state);
        EXPECT_STREQ(respCalls_V1_2->number, NORMAL_CALL_NUMBER);
    }

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
}

static void sendInvalidGetCurCallList_V1_2(SitrilTest *psitRilTest, int rspType,
                                            RIL_Errno expectResult, int phoneIndex, BYTE error = 0) {
    int testRilCmdId = RIL_REQUEST_GET_CURRENT_CALLS;
    UINT16 sitId = SIT_GET_CURRENT_CALLS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS REQ //
    ///////////////////////////////////////
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS RSP //
    ///////////////////////////////////////
    sitToken = pModemData->GetToken();
    rspData = buildInvalidGetCurCallListRsp_V1_2(sitId, sitToken, &rspDataLen, rspType, error);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ(expectResult, psitRilTest->getRilRspErrno());

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
}

static UINT8* buildGetCurCallListRsp_V1_6(INT16 sitId, UINT sitToken, UINT32 *dataLen,
                                          UINT32 callCount, BYTE state) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)(sizeof(RCM_HEADER) + sizeof(INT32) + (sizeof(sit_call_info_type_v1_6) * callCount));
    rspData = allocRspModemMem(len, sitId, sitToken);
    *dataLen = len;
    LOGI("buildGetCurCallListRsp_V1_6 callCount:%d, state:%d, len: %d", callCount, state, len);

    sit_call_get_current_calls_rsp_v1_6 *rsp = (sit_call_get_current_calls_rsp_v1_6*)rspData;
    rsp->number = callCount;
    for (UINT32 i = 0; i < callCount; i++) {
        rsp->record[i].state = state;
        rsp->record[i].call_type = SIT_CALL_CALL_TYPE_CDMA_EMERGENCY;
        rsp->record[i].num_len = strlen(NORMAL_CALL_NUMBER);
        rsp->record[i].name_len = strlen(CALL_NAME);
        strncpy((char *)rsp->record[i].num, NORMAL_CALL_NUMBER, rsp->record[i].num_len);
        strncpy((char *)rsp->record[i].name, CALL_NAME, rsp->record[i].name_len);
        rsp->record[i].uus_type = SIT_CALL_UUS_DCS_USP;
        rsp->record[i].uus_dcs = SIT_CALL_UUS_TYPE_TYPE1_REQUIRED;
        rsp->record[i].uus_data_len = strlen(UUS_DATA);
        strncpy((char *)rsp->record[i].uus_data, UUS_DATA, strlen(UUS_DATA));
        rsp->record[i].forwarded_num_len = strlen(FORWARD_CALL_NUMBER);
        strncpy((char *)rsp->record[i].forwarded_num, FORWARD_CALL_NUMBER, rsp->record[i].forwarded_num_len);
    }

    return rspData;
}

static void sendGetCurCallList_V1_6(SitrilTest *psitRilTest, UINT32 callCount, BYTE state, int phoneIndex) {
    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_GET_CURRENT_CALLS, HAL_VERSION_CODE(1, 6));
    UINT16 sitId = SIT_GET_CURRENT_CALLS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS REQ //
    ///////////////////////////////////////
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS RSP //
    ///////////////////////////////////////
    sitToken = pModemData->GetToken();
    rspData = buildGetCurCallListRsp_V1_6(sitId, sitToken, &rspDataLen, callCount, state);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    for (UINT32 i = 0; i < callCount; i++) {
        RIL_Call_V1_6 *respCalls_V1_6 = ((RIL_Call_V1_6 **)rilRspData)[i];
        EXPECT_EQ(respCalls_V1_6->state, state);
        EXPECT_STREQ(respCalls_V1_6->number, NORMAL_CALL_NUMBER);
    }

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
}

static UINT8* buildLastCallFailCauseRsp(INT16 sitId, UINT sitToken, UINT32 *dataLen, int cause) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_call_get_last_call_fail_cause_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *dataLen = len;
    sit_call_get_last_call_fail_cause_rsp *rsp = (sit_call_get_last_call_fail_cause_rsp*)rspData;
    rsp->last_call_fail_cause = cause;

    return rspData;
}

static void sendLastCallFailCause(SitrilTest *psitRilTest, int cause, int phoneIndex) {
    int testRilCmdId = RIL_REQUEST_LAST_CALL_FAIL_CAUSE;
    UINT16 sitId = SIT_GET_LAST_CALL_FAIL_CAUSE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    //////////////////////////////////////////
    // RIL_REQUEST_LAST_CALL_FAIL_CAUSE REQ //
    //////////////////////////////////////////
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    //////////////////////////////////////////
    // RIL_REQUEST_LAST_CALL_FAIL_CAUSE RSP //
    //////////////////////////////////////////
    sitToken = pModemData->GetToken();
    rspData = buildLastCallFailCauseRsp(sitId, sitToken, &rspDataLen, cause);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
    ASSERT_NE((unsigned int)0, rilRspDataLen);
    RIL_LastCallFailCauseInfo *pFailCauseInfo = (RIL_LastCallFailCauseInfo*)rilRspData;
    EXPECT_EQ(pFailCauseInfo->cause_code, cause);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
}

static void sendSetRadioPower(SitrilTest *psitRilTest, int phoneIndex, unsigned int on){
    LOGI("====== START sendSetRadioPower ======");

    int testRilCmdId = RIL_REQUEST_RADIO_POWER;
    UINT16 sitId = SIT_SET_RADIO_POWER;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT16 rspDataLen = 0;

    rspData = buildIndRadioStateChanged(&rspDataLen, (on == 1 ? 0 : 2));
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, rspData, rspDataLen);

    unsigned int radioPower[3] = {on, 1, 1};
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, radioPower, sizeof(radioPower), phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pwr_set_radio_power_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);
    cleanup(psitRilTest, rspData, pModemData);
    rspData = nullptr;
    pModemData = nullptr;

    rspData = buildIndRadioStateChanged(&rspDataLen, (on == 1 ? 2 : 0));
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, rspData, rspDataLen);
    cleanup(psitRilTest, rspData, pModemData);
    rspData = nullptr;
    pModemData = nullptr;

    LOGI("====== END sendSetRadioPower ======");
}

static void sendSetEmcLabTestMode(SitrilTest *psitRilTest, int phoneIndex) {
    LOGI("====== START sendSetEmcLabTestMode ======");
    int testRilCmdId = RIL_REQUEST_OEM_SET_EMC_LAB_TEST_MODE;

    ///////////////////////////////////////////
    // RIL_REQUEST_OEM_SET_EMC_LAB_TEST_MODE //
    ///////////////////////////////////////////
    int value = 1;
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    cleanup(psitRilTest, nullptr);
    LOGI("====== END sendSetEmcLabTestMode ======");
}

static void sendUpdateEmergencyNumberList(SitrilTest *psitRilTest, int phoneIndex, int mode) {
    LOGI("====== START sendUpdateEmergencyNumberList ======");
    int testRilCmdId = RIL_REQUEST_OEM_UPDATE_EMERGENCY_NUMBER_LIST;

    SystemProperty::Set(VENDOR_RIL_DB_ECC_ID_TYPE, mode);
    /////////////////////////////////////
    // RIL_REQUEST_OEM_UPDATE_EMERGENCY_NUMBER_LIST //
    /////////////////////////////////////
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    cleanup(psitRilTest, nullptr);
    LOGI("====== END sendUpdateEmergencyNumberList ======");
}

static UINT8* buildGetSimStatusRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, tSimStatusParams *pParams) {
    UINT8 *rspData = nullptr;
    if (pParams == nullptr) return nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_sim_status_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // need to make verifying data
    // sit_sim_get_sim_status_rsp *rsp = (sit_sim_get_sim_status_rsp*)rspData;
    sit_sim_get_sim_status_rsp *rsp = (sit_sim_get_sim_status_rsp*)rspData;
    rsp->card_state = pParams->card_state;

    if (rsp->card_state == 1) {
        rsp->universal_pin_state = pParams->universal_pin_state;
        rsp->application_num = pParams->application_num;

        rsp->apps_status_info[0].apps_type = RIL_APPTYPE_USIM;
        rsp->apps_status_info[0].apps_state = pParams->apps_state;
        rsp->apps_status_info[0].perso_substate = pParams->perso_substate;
        rsp->apps_status_info[0].aid_len = 16;
        memcpy(rsp->apps_status_info[0].AID, USIM_AIDData, 16);

        rsp->apps_status_info[1].apps_type = RIL_APPTYPE_ISIM;
        rsp->apps_status_info[1].apps_state = pParams->apps_state;
        rsp->apps_status_info[1].perso_substate = pParams->perso_substate;
        rsp->apps_status_info[1].aid_len = 16;
        memcpy(rsp->apps_status_info[1].AID, ISIM_AIDData, 16);
    }

    return rspData;
}

static void sendGetPinLockSimStatus(SitrilTest *psitRilTest, int phoneIndex) {
    int testRilCmdId = RIL_REQUEST_GET_SIM_STATUS;
    UINT16 sitId = SIT_GET_SIM_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    ////////////////////////////////////
    // RIL_REQUEST_GET_SIM_STATUS REQ //
    ////////////////////////////////////
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, phoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    ////////////////////////////////////
    // RIL_REQUEST_GET_SIM_STATUS RSP //
    ////////////////////////////////////
    tSimStatusParams param;
    param.apps_state = RIL_APPSTATE_PIN;
    param.universal_pin_state = RIL_PINSTATE_ENABLED_NOT_VERIFIED;
    sitToken = pModemData->GetToken();
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
}

static UINT8* buildSendCallConfirmRsp(INT16 sitId, UINT sitToken, UINT32 *dataLen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_call_send_call_confirm_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *dataLen = len;
    sit_call_send_call_confirm_rsp *rsp = (sit_call_send_call_confirm_rsp*)rspData;
    rsp->result = 1; // 0x00: Disable, 0x01: Enable

    return rspData;
}

static UINT8* buildExitEcbmModeRsp(INT16 sitId, UINT sitToken, UINT32 *dataLen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_call_exit_emergency_cb_mode_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *dataLen = len;
    sit_call_exit_emergency_cb_mode_rsp *rsp = (sit_call_exit_emergency_cb_mode_rsp*)rspData;
    rsp->result = 1; // 0x00: Fail, 0x01: Success

    return rspData;
}

static UINT8* buildSoundRingbackToneInd(INT16 sitId, UINT32 *dataLen, int state, int flag) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_snd_ringback_tone_with_flag_ind);

    rspData = allocIndModemMem(len, sitId);
    *dataLen = len;

    sit_snd_ringback_tone_with_flag_ind *ind = (sit_snd_ringback_tone_with_flag_ind*)rspData;
    ind->ringback_state = state;
    ind->flag = flag;

    return rspData;
}

static UINT8* buildEmergencySupportRatModeInd(INT16 sitId, UINT32 *dataLen, int mode) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_call_emergency_support_rat_mode_ind);

    rspData = allocIndModemMem(len, sitId);
    *dataLen = len;

    sit_call_emergency_support_rat_mode_ind *ind = (sit_call_emergency_support_rat_mode_ind*)rspData;
    ind->available_tech = mode;

    return rspData;
}

static void sendEmergencySupportRatModeInd(SitrilTest *psitRilTest, int mode) {
    UINT16 sitId = SIT_IND_EMERGENCY_SUPPORT_RAT_MODE;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    rspData = buildEmergencySupportRatModeInd(sitId, &rspDataLen, mode);
    psitRilTest->processInd(RIL_UNSOL_EMERGENCY_SUPPORT_RAT_MODE, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    int supportRatMode = ((int*)rilIndData)[0];
    if (mode == INVALID_DATA) {
        // INVALID_DATA is converted to default value
        EXPECT_EQ(supportRatMode, SIT_SUPPORT_RAT_MODE_3GPP);
    } else {
        EXPECT_EQ(supportRatMode, mode); // SIT and RIL value is same.
    }

    cleanup(psitRilTest, rspData);
    rspData = nullptr;
}

static UINT8* sendEmergencyCallListInd_V1_1(INT16 sitId, UINT32 *dataLen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_call_emergency_call_list_ind_v1_1);

    rspData = allocIndModemMem(len, sitId);
    *dataLen = len;

    sit_call_emergency_call_list_ind_v1_1 *ind = (sit_call_emergency_call_list_ind_v1_1*)rspData;
    strncpy((char*)ind->mccmnc, TEST_MCC, 3);
    strncpy((char*)ind->mccmnc+3, TEST_MNC, 3);
    ind->num = 1;
    ind->number_info[0].emc_number_len = strlen(EMERGENCY_CALL_NUMBER);
    strncpy((char*)ind->number_info[0].emc_number, EMERGENCY_CALL_NUMBER, ind->number_info[0].emc_number_len);

    return rspData;
}

static UINT8* sendEmergencyCallListInd_V1_4(INT16 sitId, UINT32 *dataLen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_call_emergency_call_list_ind_v1_4);

    rspData = allocIndModemMem(len, sitId);
    *dataLen = len;

    sit_call_emergency_call_list_ind_v1_4 *ind = (sit_call_emergency_call_list_ind_v1_4*)rspData;
    strncpy((char*)ind->mcc, TEST_MCC, 3);
    strncpy((char*)ind->mnc, TEST_MNC, 3);
    ind->num = 1;
    ind->number_info[0].emc_number_len = strlen(EMERGENCY_CALL_NUMBER);
    strncpy((char*)ind->number_info[0].emc_number, EMERGENCY_CALL_NUMBER, ind->number_info[0].emc_number_len);
    ind->number_info[0].source = RIL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING;

    return rspData;
}

static UINT8 *sendEmergencyCallListInd_V1_6(INT16 sitId, UINT32 *dataLen) {
    LOGI("====== START sendEmergencyCallListInd_V1_6 ======");
    UINT8 *rspData = nullptr;
    sit_call_emergency_call_list_ind_v1_6 ind_v1_6;
    int sizeEnl = sizeof(ind_v1_6.hdr) + sizeof(ind_v1_6.mcc) +
                  sizeof(ind_v1_6.mnc) + sizeof(ind_v1_6.enlNum) +
                  sizeof(sit_call_emergency_call_number_info_v1_4);
    UINT16 len = sizeEnl + sizeof(ind_v1_6.eenl.eenlv) + sizeof(ind_v1_6.eenl.eenlNum) +
                 22;  // 11 * 2 = 1(emergency number len) + 3(emergency number) + 1(sub service len) + 6(sub service)

    rspData = allocIndModemMem(len, sitId);
    *dataLen = len;

    sit_call_emergency_call_list_ind_v1_6 *ind = (sit_call_emergency_call_list_ind_v1_6 *)rspData;
    sit_call_emergency_call_eenl *eenl = (sit_call_emergency_call_eenl *)(rspData + sizeEnl);
    strncpy((char *)ind->mcc, TEST_MCC, 3);
    strncpy((char *)ind->mnc, TEST_MNC, 3);
    ind->enlNum = 1;
    ind->enl_records[0].emc_number_len = strlen(EMERGENCY_CALL_NUMBER);
    strncpy((char *)ind->enl_records[0].emc_number, EMERGENCY_CALL_NUMBER, ind->enl_records[0].emc_number_len);
    ind->enl_records[0].source = RIL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING;

    eenl->eenlv = 0;
    eenl->eenlNum = 2;
    eenl->eenl_records[0] = strlen(EMERGENCY_CALL_NUMBER);
    strncpy((char *)&eenl->eenl_records[1], EMERGENCY_CALL_NUMBER, (int)eenl->eenl_records[0]);
    eenl->eenl_records[4] = strlen(EMERGENCY_SUB_SERVICE);
    strncpy((char *)&eenl->eenl_records[5], EMERGENCY_SUB_SERVICE, (int)eenl->eenl_records[4]);

    eenl->eenl_records[11] = strlen(EMERGENCY_CALL_NUMBER);
    strncpy((char *)&eenl->eenl_records[12], EMERGENCY_CALL_NUMBER, (int)eenl->eenl_records[11]);
    eenl->eenl_records[15] = strlen(EMERGENCY_SUB_SERVICE);
    strncpy((char *)&eenl->eenl_records[16], "", (int)eenl->eenl_records[15]);

    LOGI("====== END sendEmergencyCallListInd_V1_6 ======");
    return rspData;
}

static void sendEmergencyCallListInd(SitrilTest *psitRilTest, int version) {
    UINT16 sitId = SIT_IND_EMERGENCY_CALL_LIST;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    if (version == HAL_VERSION_V1_6) {
        rspData = sendEmergencyCallListInd_V1_6(sitId, &rspDataLen);
    } else if (version == HAL_VERSION_V1_4) {
        rspData = sendEmergencyCallListInd_V1_4(sitId, &rspDataLen);
    } else {
        rspData = sendEmergencyCallListInd_V1_1(sitId, &rspDataLen);
    }
    psitRilTest->processInd(RIL_UNSOL_EMERGENCY_NUMBER_LIST, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    int num = indLen / sizeof(RIL_EmergencyNumber);
    for (int i = 0; i < num; i++) {
        RIL_EmergencyNumber *p_cur = &((RIL_EmergencyNumber *)rilIndData)[i];
        //LOGI("sendEmergencyCallListInd number:%s, source:%d", p_cur->number, p_cur->sources);
        if (p_cur->sources == RIL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING) {
            EXPECT_STREQ(EMERGENCY_CALL_NUMBER, p_cur->number);
        }
    }

    cleanup(psitRilTest, rspData);
    rspData = nullptr;
}

static UINT8* buildAudioWbamrReportInd(INT16 sitId, UINT32 *dataLen, int status, int callType) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_snd_wb_amr_report_with_calltype_ind);

    rspData = allocIndModemMem(len, sitId);
    *dataLen = len;

    sit_snd_wb_amr_report_with_calltype_ind *ind = (sit_snd_wb_amr_report_with_calltype_ind*)rspData;
    ind->status = (BYTE) status;
    ind->call_type = (BYTE) callType;

    return rspData;
}

static void sendAudioQuality(SitrilTest *psitRilTest, int status, int callType) {
    UINT16 sitId = SIT_IND_WB_AMR_REPORT;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    rspData = buildAudioWbamrReportInd(sitId, &rspDataLen, status, callType);
    psitRilTest->processInd(RIL_UNSOL_WB_AMR_REPORT_IND, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    cleanup(psitRilTest, rspData);
    rspData = nullptr;
}

static void sendErrorRsp(SitrilTest *psitRilTest, int testRilCmdId, INT16 sitId, UINT sitToken, BYTE error) {
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    rspData = buildErrorRsp(sitId, sitToken, &rspDataLen, error);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    cleanup(psitRilTest, rspData);
    rspData = nullptr;
}

// sitrilTestCs class
void SitrilTestCs::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();
}

void SitrilTestCs::TearDown() {
    mSitRilTest = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////
// SET reqeust|response //

TEST_P(SitrilTestCs, callList_V1_1) {
    LOGI("====== START callList_V1_1 ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);


    sendGetCurCallList_V1_1(psitRilTest, 1, SIT_CALL_STATE_ACTIVE, mPhoneIndex);
    sendGetCurCallList_V1_1(psitRilTest, 0, (BYTE)-1, mPhoneIndex);

    // ERROR CASES //
    sendInvalidGetCurCallList_V1_1(psitRilTest, CALL_LIST_UNKNOWN, RIL_E_SUCCESS, mPhoneIndex);
    LOGI("====== END callList_V1_1 ======");
}

TEST_P(SitrilTestCs, callList_V1_2) {
    LOGI("====== START callList_V1_2 ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendAudioQuality(psitRilTest, AUDIO_QUALITY_NB, AUDIO_CALL_TYPE_GSM);
    sendGetCurCallList_V1_2(psitRilTest, 1, SIT_CALL_STATE_ACTIVE, mPhoneIndex);
    sendGetCurCallList_V1_2(psitRilTest, 0, (BYTE)-1, mPhoneIndex);

    // ERROR CASES //
    sendInvalidGetCurCallList_V1_2(psitRilTest, CALL_LIST_INVALID_LENGTH, RIL_E_SUCCESS, mPhoneIndex);
    sendInvalidGetCurCallList_V1_2(psitRilTest, CALL_LIST_INVALID_NUM, RIL_E_SUCCESS, mPhoneIndex);
    sendInvalidGetCurCallList_V1_2(psitRilTest, CALL_LIST_UNKNOWN, RIL_E_SUCCESS, mPhoneIndex);

    psitRilTest->processSitrilStateUpdate(0, 0, RIL_CARDSTATE_PRESENT);
    sendInvalidGetCurCallList_V1_2(psitRilTest, CALL_LIST_INVALID_LENGTH, RIL_E_INTERNAL_ERR, mPhoneIndex);
    sendInvalidGetCurCallList_V1_2(psitRilTest, CALL_LIST_INVALID_NUM, RIL_E_INVALID_ARGUMENTS, mPhoneIndex);
    sendInvalidGetCurCallList_V1_2(psitRilTest, CALL_LIST_INVALID_NUM, RIL_E_SUCCESS, mPhoneIndex, RIL_E_RADIO_NOT_AVAILABLE);

    LOGI("====== END callList_V1_2 ======");
}

TEST_P(SitrilTestCs, callList_V1_6) {
    LOGI("====== START callList_V1_6 ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // send invalid audio quality
    sendAudioQuality(psitRilTest, -1, AUDIO_CALL_TYPE_GSM);
    sendGetCurCallList_V1_6(psitRilTest, 1, SIT_CALL_STATE_ACTIVE, mPhoneIndex);
    sendGetCurCallList_V1_6(psitRilTest, 0, (BYTE)-1, mPhoneIndex);

    // ERROR CASES //
    sendInvalidGetCurCallList_V1_1(psitRilTest, CALL_LIST_UNKNOWN, RIL_E_SUCCESS, mPhoneIndex);
    LOGI("====== END callList_V1_1 ======");
}

TEST_P(SitrilTestCs, dial) {
    LOGI("====== START dial ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_DIAL;
    UINT16 sitId = SIT_DIAL;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    RIL_Dial reqData;
    reqData.address = NORMAL_CALL_NUMBER;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_dial_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sit_call_dial_req *req = (sit_call_dial_req*)txIpcRawData;
    EXPECT_EQ(req->call_type, SIT_CALL_CALL_TYPE_VOICE);
    EXPECT_EQ(req->num_type, SIT_CALL_NUMBER_TYPE_NATIONAL);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;

    reqData.address = INTERNATIONAL_NORMAL_CALL_NUMBER;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    req = (sit_call_dial_req*)txIpcRawData;
    EXPECT_EQ(req->call_type, SIT_CALL_CALL_TYPE_VOICE);
    EXPECT_EQ(req->num_type, SIT_CALL_NUMBER_TYPE_INTERNATIONAL);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END dial ======");
}

TEST_P(SitrilTestCs, dial_cdma) {
    LOGI("====== START dial_cdma ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_DIAL;
    UINT16 sitId = SIT_DIAL;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendGetVoiceRegistration(psitRilTest, mPhoneIndex, SIT_RAT_TYPE_IS95A, SIT_NET_REG_STATE_REGISTERED);

    // REQ //
    RIL_Dial reqData;
    reqData.address = NORMAL_CALL_NUMBER;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_dial_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sit_call_dial_req *req = (sit_call_dial_req*)txIpcRawData;
    EXPECT_EQ(req->call_type, SIT_CALL_CALL_TYPE_CDMA_VOICE);
    EXPECT_EQ(req->num_type, SIT_CALL_NUMBER_TYPE_NATIONAL);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;

    sendGetVoiceRegistration(psitRilTest, mPhoneIndex, SIT_RAT_TYPE_UMTS, SIT_NET_REG_STATE_REGISTERED);
    LOGI("====== END dial_cdma ======");
}

TEST_P(SitrilTestCs, emergencyDial) {
    LOGI("====== START emergencyDial ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_EMERGENCY_DIAL;
    UINT16 sitId = SIT_DIAL;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_SIM_NET_COMBINE);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_1);
    sendSetEmcLabTestMode(psitRilTest, mPhoneIndex);

    // REQ //
    RIL_EmergencyDial reqData;
    reqData.dialInfo.address = EMERGENCY_CALL_NUMBER2;
    reqData.dialInfo.clir = 2;
    reqData.dialInfo.uusInfo = NULL;
    reqData.categories = 0;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_dial_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sit_call_dial_req *req = (sit_call_dial_req*)txIpcRawData;
    EXPECT_EQ(req->call_type, SIT_CALL_CALL_TYPE_EMERGENCY);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    LOGI("====== END emergencyDial ======");
}

TEST_P(SitrilTestCs, emergencyDial_cdma) {
    LOGI("====== START emergencyDial_cdma ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_EMERGENCY_DIAL;
    UINT16 sitId = SIT_DIAL;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_SIM_NET_COMBINE);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_1);
    sendSetEmcLabTestMode(psitRilTest, mPhoneIndex);
    sendGetVoiceRegistration(psitRilTest, mPhoneIndex, SIT_RAT_TYPE_EHRPD, SIT_NET_REG_STATE_REGISTERED);

    // REQ //
    RIL_EmergencyDial reqData;
    reqData.dialInfo.address = EMERGENCY_CALL_NUMBER2;
    reqData.dialInfo.clir = 2;
    reqData.dialInfo.uusInfo = NULL;
    reqData.categories = 0;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_dial_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sit_call_dial_req *req = (sit_call_dial_req*)txIpcRawData;
    EXPECT_EQ(req->call_type, SIT_CALL_CALL_TYPE_CDMA_EMERGENCY);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    sendGetVoiceRegistration(psitRilTest, mPhoneIndex, SIT_RAT_TYPE_UMTS, SIT_NET_REG_STATE_REGISTERED);
    LOGI("====== END emergencyDial_cdma ======");
}

TEST_P(SitrilTestCs, emergencyDial_test) {
    LOGI("====== START emergencyDial_test ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_EMERGENCY_DIAL;
    UINT16 sitId = SIT_DIAL;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    RIL_EmergencyDial reqData;
    reqData.dialInfo.address = EMERGENCY_CALL_NUMBER;
    reqData.dialInfo.clir = CLIR_DEFAULT;
    reqData.dialInfo.uusInfo = NULL;
    reqData.categories = 0;
    reqData.isTesting = true;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_dial_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sit_call_dial_req *req = (sit_call_dial_req*)txIpcRawData;
    ASSERT_NE(req->call_type, SIT_CALL_CALL_TYPE_EMERGENCY);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END emergencyDial_test ======");
}

TEST_P(SitrilTestCs, emergencyDial_helper) {
    LOGI("====== START emergencyDial_helper ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_EMERGENCY_DIAL;
    //UINT16 sitId = SIT_DIAL;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendSetRadioPower(psitRilTest, mPhoneIndex, 1);
    //sendSetRadioPower(psitRilTest, mPhoneIndex, 0);
    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_SIM_NET_COMBINE);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_1);
    sendSetEmcLabTestMode(psitRilTest, mPhoneIndex);
    SystemProperty::Set(PROPERTY_ICC_OPERATOR_NUMERIC, "00000");
    sendGetVoiceRegistration(psitRilTest, mPhoneIndex, SIT_RAT_TYPE_UMTS, SIT_NET_REG_STATE_NOT_REG_NO_SEARCH);

    // REQ //
    RIL_EmergencyDial reqData;
    reqData.dialInfo.address = EMERGENCY_CALL_NUMBER2;
    reqData.dialInfo.clir = 2;
    reqData.dialInfo.uusInfo = NULL;
    reqData.categories = 0;
    reqData.isTesting = true;
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    cleanup(psitRilTest, rspData);

    ///////////////////////////////////////
    // RIL_REQUEST_GET_CURRENT_CALLS REQ //
    ///////////////////////////////////////
    LOGI("====== Send call list req ======");
    testRilCmdId = RIL_REQUEST_GET_CURRENT_CALLS;
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    sendGetVoiceRegistration(psitRilTest, mPhoneIndex, SIT_RAT_TYPE_UMTS, SIT_NET_REG_STATE_REGISTERED);
    cleanup(psitRilTest, rspData);
    rspData = nullptr;
    LOGI("====== END emergencyDial_helper ======");
}

TEST_P(SitrilTestCs, answer) {
    LOGI("====== START answer ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_ANSWER;
    UINT16 sitId = SIT_ANSWER;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_answer_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END answer ======");
}

TEST_P(SitrilTestCs, explictCallTransfer) {
    LOGI("====== START explictCallTransfer ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_EXPLICIT_CALL_TRANSFER;
    UINT16 sitId = SIT_SEND_EXPLICIT_CALL_TRANSFER;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    psitRilTest->processSitrilStateUpdate(0, 0, RIL_CARDSTATE_PRESENT);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_explicit_call_transfer_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on REQ - wrong card state
    psitRilTest->processSitrilStateUpdate(0, 0, RIL_CARDSTATE_ABSENT);
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    ASSERT_NE(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processSitrilStateUpdate(0, 0, RIL_CARDSTATE_PRESENT);
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END  ======");
}

TEST_P(SitrilTestCs, hangupSingle) {
    LOGI("====== START hangupSignle ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_HANGUP;
    UINT16 sitId = SIT_HANGUP;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // Add 1 active call list
    sendAudioQuality(psitRilTest, AUDIO_QUALITY_WB, AUDIO_CALL_TYPE_GSM);
    sendGetCurCallList_V1_2(psitRilTest, 1, SIT_CALL_STATE_ACTIVE, mPhoneIndex);

    // REQ //
    unsigned int index = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(index), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_hangup_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sit_call_hangup_req *req = (sit_call_hangup_req*)txIpcRawData;
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ req->index, index - 1);
    EXPECT_EQ(req->call_type, SIT_CALL_HANGUP_CALL_TYPE_SINGLE);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    LOGI("====== END hangupSingle ======");
}

TEST_P(SitrilTestCs, hangupMultiparty) {
    LOGI("====== START hangupMultiparty ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_HANGUP;
    UINT16 sitId = SIT_HANGUP;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // Add 2 active call list
    sendGetCurCallList_V1_2(psitRilTest, 2, SIT_CALL_STATE_ACTIVE, mPhoneIndex);

    // REQ //
    unsigned int index = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(index), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_hangup_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sit_call_hangup_req *req = (sit_call_hangup_req*)txIpcRawData;
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ req->index, index - 1);
    EXPECT_EQ(req->call_type, SIT_CALL_HANGUP_CALL_TYPE_MULTI);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on REQ - empty call list
    sendGetCurCallList_V1_2(psitRilTest, 0, (BYTE)-1, mPhoneIndex);
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(index), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    ASSERT_NE(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    sendGetCurCallList_V1_2(psitRilTest, 2, SIT_CALL_STATE_ACTIVE, mPhoneIndex);
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(index), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END hangupMultiparty ======");
}

TEST_P(SitrilTestCs, lastCallFailCause) {
    LOGI("====== START lastCallFailCause ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_LAST_CALL_FAIL_CAUSE;
    UINT16 sitId = SIT_GET_LAST_CALL_FAIL_CAUSE;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_UNOBTAINABLE_NUMBER, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_NORMAL, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_BUSY, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_NORMAL_UNSPECIFIED, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CONGESTION, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_ACM_LIMIT_EXCEEDED, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CALL_BARRED, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_FDN_BLOCKED, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_IMSI_UNKNOWN_IN_VLR, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_IMEI_NOT_ACCEPTED, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_LOCKED_UNTIL_POWER_CYCLE, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_DROP, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_INTERCEPT, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_REORDER, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_SO_REJECT, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_RETRY_ORDER, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_ACCESS_FAILURE, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_PREEMPTED, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_NOT_EMERGENCY, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_CDMA_ACCESS_BLOCKED, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, SIT_CALL_LAST_CALL_FAIL_ERROR_UNSPECIFIED, mPhoneIndex);
    sendLastCallFailCause(psitRilTest, 6, mPhoneIndex);

    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_NO_SUCH_ELEMENT);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    // RCM_E_RADIO_NOT_AVAILABLE ERROR on RSP, but success rsp - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    sizeOfModemData = sizeof(null_data_format);
    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;
    rspData = nullptr;
    rspDataLen = 0;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    rspData = buildErrorRsp(sitId, pModemData->GetToken(), &rspDataLen, RCM_E_RADIO_NOT_AVAILABLE);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END lastCallFailCause ======");
}

TEST_P(SitrilTestCs, udub) {
    LOGI("====== START udub ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_UDUB;
    UINT16 sitId = SIT_UDUB;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_udub_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END udub ======");
}

TEST_P(SitrilTestCs, hangupFgResumeBg) {
    LOGI("====== START hangupFgResumeBg ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND;
    UINT16 sitId = SIT_HANGUP_FORGROUND_RESUME_BACKGROUND;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_hangup_foreground_resume_background_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END hangupFgResumeBg ======");
}

TEST_P(SitrilTestCs, hangupWaitingOrBg) {
    LOGI("====== START hangupWaitingOrBg ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    int testRilCmdId = RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND;
    UINT16 sitId = SIT_HANGUP_WAITING_OR_BACKGROUND;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_hangup_waiting_or_background_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END hangupWaitingOrBg ======");
}

TEST_P(SitrilTestCs, switchWatingOrHoldingAndActive) {
    LOGI("====== START switchWatingOrHoldingAndActive ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE;
    UINT16 sitId = SIT_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_switch_waiting_or_holding_and_active_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END switchWatingOrHoldingAndActive ======");
}

TEST_P(SitrilTestCs, conference) {
    LOGI("====== START conference ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_CONFERENCE;
    UINT16 sitId = SIT_CONFERENCE;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_conference_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END conference ======");
}

TEST_P(SitrilTestCs, separateConnection) {
    LOGI("====== START separateConnection ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_SEPARATE_CONNECTION;
    UINT16 sitId = SIT_SEPARATE_CONNECTION;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // Add 2 active call list
    sendGetCurCallList_V1_2(psitRilTest, 2, SIT_CALL_STATE_ACTIVE, mPhoneIndex);

    // REQ //
    unsigned int index = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(index), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_separate_connection_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sit_ss_separate_connection_req *req = (sit_ss_separate_connection_req*)txIpcRawData;
    EXPECT_EQ((unsigned int)/* NEED TO CHECK */ req->index, index - 1);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on REQ - empty call list
    sendGetCurCallList_V1_2(psitRilTest, 0, (BYTE)-1, mPhoneIndex);
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(index), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    ASSERT_NE(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on REQ - less than 2 call list
    sendGetCurCallList_V1_2(psitRilTest, 1, SIT_CALL_STATE_ACTIVE, mPhoneIndex);
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(index), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    ASSERT_NE(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    sendGetCurCallList_V1_2(psitRilTest, 2, SIT_CALL_STATE_ACTIVE, mPhoneIndex);
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &index, sizeof(index), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    // make empty call list
    sendGetCurCallList_V1_2(psitRilTest, 0, (BYTE)-1, mPhoneIndex);

    LOGI("====== END separateConnection ======");
}

TEST_P(SitrilTestCs, sendCallConfirm) {
    LOGI("====== START sendCallConfirm ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_SEND_CALL_CONFIRM;
    UINT16 sitId = SIT_CALL_CONFIRM;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sitToken = pModemData->GetToken();
    rspData = buildSendCallConfirmRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((unsigned int)0, rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
    int *p_int = (int*)rilRspData;
    EXPECT_EQ(p_int[0], 1); // 0x00: Disable, 0x01: Enable

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== END sendCallConfirm ======");
}

TEST_P(SitrilTestCs, exitEcbmMode) {
    LOGI("====== START exitEcbmMode ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE;
    UINT16 sitId = SIT_EXIT_EMERGENCY_CB_MODE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // REQ //
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_exit_emergency_cb_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sitToken = pModemData->GetToken();
    rspData = buildExitEcbmModeRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((unsigned int)0, rilRspDataLen);
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
    int *p_int = (int*)rilRspData;
    EXPECT_EQ(p_int[0], 1); // 0x00: Fail, 0x01: Success

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code(RCM_E_UNDEFINED_CMD)
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_UNDEFINED_CMD);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code(RCM_E_RADIO_NOT_AVAILABLE)
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RCM_E_RADIO_NOT_AVAILABLE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code(RIL_E_GENERIC_FAILURE)
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RIL_E_GENERIC_FAILURE);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code(RCM_E_SIM_MEMORY_FULL)
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RIL_E_DIAL_MODIFIED_TO_USSD);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code()
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RIL_E_MODEM_ERR);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////
    LOGI("====== END exitEcbmMode ======");
}

TEST_P(SitrilTestCs, dtmf) {
    LOGI("====== START dtmf ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    int testRilCmdId = RIL_REQUEST_DTMF;
    UINT16 sitId = SIT_DTMF;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processSitrilStateUpdate(0, 0, RIL_CARDSTATE_PRESENT);

    // REQ //
    char dtmf[] = "1";
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, dtmf, strlen(dtmf), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_dtmf_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code(RIL_E_INTERNAL_ERR)
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, dtmf, strlen(dtmf), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RIL_E_INTERNAL_ERR);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////


    // REQ //
    testRilCmdId = RIL_REQUEST_DTMF_START;
    sitId = SIT_DTMF_START;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, dtmf, strlen(dtmf), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    sizeOfModemData = sizeof(sit_call_dtmf_start_req);
    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code(RIL_E_INTERNAL_ERR)
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, dtmf, strlen(dtmf), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RIL_E_INTERNAL_ERR);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////


    // REQ //
    testRilCmdId = RIL_REQUEST_DTMF_STOP;
    sitId = SIT_DTMF_STOP;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    sizeOfModemData = sizeof(sit_call_dtmf_stop_req);
    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // RSP //
    sendEmptyRsp(psitRilTest, sitId, pModemData->GetToken(), testRilCmdId);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on RSP - error code(RIL_E_INTERNAL_ERR)
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    sendErrorRsp(psitRilTest, testRilCmdId, sitId, pModemData->GetToken(), RIL_E_INTERNAL_ERR);

    cleanup(psitRilTest, rspData, pModemData);
    pModemData = nullptr;
    rspData = nullptr;
    ///////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on REQ - absent
    psitRilTest->processSitrilStateUpdate(0, 0, RIL_CARDSTATE_ABSENT);
    testRilCmdId = RIL_REQUEST_DTMF_START;
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &dtmf, sizeof(dtmf), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    ASSERT_NE(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    // ERROR on REQ - absent
    psitRilTest->processSitrilStateUpdate(0, 0, RIL_CARDSTATE_ABSENT);
    testRilCmdId = RIL_REQUEST_DTMF_STOP;
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    ASSERT_NE(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());
    ///////////////////////////////////////////////////////////////////////////////

    LOGI("====== end dtmf ======");
}

////////////////////////////////////////////////////////////////////////////////////////
// Indication //
TEST_P(SitrilTestCs, ringbackToneInd) {
    LOGI("====== START ringbackToneInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_RINGBACK_TONE;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    rspData = buildSoundRingbackToneInd(sitId, &rspDataLen, RIL_SND_RINGBACK_TONE_END,
                                        RINGBACK_FLAG_PLAY_OUTBAND_BY_UE);
    psitRilTest->processInd(RIL_UNSOL_RINGBACK_TONE, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    int state = ((int*)rilIndData)[0];
    EXPECT_EQ(state, RIL_SND_RINGBACK_TONE_END);

    cleanup(psitRilTest, rspData);
    rspData = nullptr;

    rspData = buildSoundRingbackToneInd(sitId, &rspDataLen, RIL_SND_RINGBACK_TONE_START,
                                        RINGBACK_FLAG_PLAY_INBAND_BY_NW);
    psitRilTest->processInd(RIL_UNSOL_OEM_RINGBACK_TONE_BY_NETWORK, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    indLen = 0;
    rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    state = ((int*)rilIndData)[0];
    //int flag  = ((int*)rilIndData)[1];
    EXPECT_EQ(state, RIL_SND_RINGBACK_TONE_START);

    cleanup(psitRilTest, rspData);
    rspData = nullptr;
    LOGI("====== END ringbackToneInd ======");
}

TEST_P(SitrilTestCs, callStateChangeInd) {
    LOGI("====== START callStateChangeInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CALL_STATE_CHANGED;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    rspData = buildEmptyInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);

    cleanup(psitRilTest, rspData);
    rspData = nullptr;
    LOGI("====== END callStateChangeInd ======");
}

TEST_P(SitrilTestCs, callRingInd) {
    LOGI("====== START callRingInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CALL_RING;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    rspData = buildEmptyInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_CALL_RING, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);

    cleanup(psitRilTest, rspData);
    rspData = nullptr;
    LOGI("====== END callRingInd ======");
}

TEST_P(SitrilTestCs, emergencySupportRatInd) {
    LOGI("====== START emergencySupportRatInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendEmergencySupportRatModeInd(psitRilTest, SIT_SUPPORT_RAT_MODE_3GPP);
    sendEmergencySupportRatModeInd(psitRilTest, SIT_SUPPORT_RAT_MODE_3GPP2);
    sendEmergencySupportRatModeInd(psitRilTest, SIT_SUPPORT_RAT_MODE_ALL);
    sendEmergencySupportRatModeInd(psitRilTest, INVALID_DATA);

    LOGI("====== END emergencySupportRatInd ======");
}

TEST_P(SitrilTestCs, emergencyCallListInd_V1_1) {
    LOGI("====== START emergencyCallListInd_V1_1 ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_NO_CUSTOMIZATION);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_1);

    LOGI("====== END emergencyCallListInd_V1_1 ======");
}

TEST_P(SitrilTestCs, emergencyCallListInd_V1_4) {
    LOGI("====== START emergencyCallListInd_V1_4 ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_NO_CUSTOMIZATION);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_4);

    psitRilTest->processSitrilStateUpdate(0, 0, RIL_CARDSTATE_PRESENT);
    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_SIM_BASED_ONLY);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_4);

    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_NET_BASED_ONLY);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_4);

    // make PIN lock
    sendGetPinLockSimStatus(psitRilTest, mPhoneIndex);
    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_SIM_PREFER);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_4);
    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_NET_PREFER);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_4);
    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_SIM_NET_COMBINE);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_4);
    LOGI("====== END emergencyCallListInd_V1_4 ======");
}

TEST_P(SitrilTestCs, emergencyCallListInd_V1_6) {
    LOGI("====== START emergencyCallListInd_V1_6 ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sendUpdateEmergencyNumberList(psitRilTest, mPhoneIndex, DB_ECC_ID_TYPE_NET_PREFER);
    sendEmergencyCallListInd(psitRilTest, HAL_VERSION_V1_6);

    LOGI("====== END emergencyCallListInd_V1_1 ======");
}

TEST_P(SitrilTestCs, callPresentInd) {
    LOGI("====== START callPresentInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CALL_PRESENT_IND;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    rspData = buildEmptyInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_CALL_PRESENT_IND, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);

    free(rspData);
    LOGI("====== END callPresentInd ======");
}

TEST_P(SitrilTestCs, enterEcbmModeInd) {
    LOGI("====== START enterEcbmModeInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_ENTER_EMERGENCY_CB_MODE;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    rspData = buildEmptyInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);

    free(rspData);
    LOGI("====== END enterEcbmModeInd ======");
}

TEST_P(SitrilTestCs, exitEcbmModeInd) {
    LOGI("====== START exitEcbmModeInd ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_EXIT_EMERGENCY_CB_MODE;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    rspData = buildEmptyInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);

    cleanup(psitRilTest, rspData);
    rspData = nullptr;
    LOGI("====== END exitEcbmModeInd ======");
}
