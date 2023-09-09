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

#include "sitrilTestIms.h"
#include "testUtil.h"
#include "protocoladapter.h"

#define TEST_AUTH_RSP "test"

void SitrilTestIms::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    mTestData = nullptr;
    mpModemData = nullptr;
    mRspData = nullptr;
    mRspDataLen = 0;
}

void SitrilTestIms::TearDown() {
    mSitRilTest = nullptr;
    FreeData();
}

void SitrilTestIms::FreeData() {
    if (mTestData) { delete mTestData; mTestData = nullptr; }
    if (mpModemData) { delete mpModemData; mpModemData = nullptr; }
    if (mRspData) { free(mRspData); mRspData = nullptr; }
    mRspDataLen = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
#if 0
static UINT8* buildImsSimAuthRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, int error) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_sim_get_sim_auth_rsp);

    if (error > 0) {
        rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
        *datalen = len;
        return rspData;
    }

    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // need to make verifying data
    sit_sim_get_sim_auth_rsp *rsp = (sit_sim_get_sim_auth_rsp*)rspData;
    rsp->auth_type = 2; //3G auth
    rsp->auth_len = 4; //
    memcpy(rsp->auth, TEST_AUTH_RSP, 4);
    return rspData;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////
// build modem ind
UINT8* buildSrvccHoInd(INT16 sitId, UINT32 *datalen, UINT32 srvccResult) {
    UINT8 *indData = nullptr;
    const int sizeOfSrvccHoData = 3;
    BYTE srvccHoData[sizeOfSrvccHoData] = {1, 2, 3};
    int sizeOfPayload = sizeOfSrvccHoData;

    // update srvccResult
    srvccHoData[1] = (BYTE)srvccResult;
    UINT16 len = (UINT16)sizeof(null_ind_data_format) + sizeOfPayload;

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    memcpy(indData+(UINT16)sizeof(null_ind_data_format), srvccHoData, sizeOfPayload);

    return indData;
}

UINT8* buildAimsRegInd(INT16 sitId, UINT32 *datalen, UINT32 aimsRegState) {
    UINT8 *indData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ims_ind_reg);

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    // need to make verifying data
    sit_ims_ind_reg *ind = (sit_ims_ind_reg*)indData;
    ind->state = (BYTE)aimsRegState;
    ind->feature = 1;
    ind->ecmp_state = 2;

    return indData;
}

////////////////////////////////////////////////////////////////////////////////////////
// request|response

TEST_P(SitrilTestIms, oemImsSimAuth) {
    // Need to test using a correct data
#if 0
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_SIM_AUTH;
    UINT16 sitId = SIT_GET_SIM_AUTH;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const int testDataLen = 6;
    char testData[testDataLen] = "ABCDE";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_sim_auth_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildImsSimAuthRsp(sitId, sitToken, &mRspDataLen, RCM_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
#endif
}

TEST_P(SitrilTestIms, oemImsGetGbaAuth) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_GET_GBA_AUTH;
    UINT16 sitId = SIT_GET_GBA_CONTEXT;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    /*
        BYTE auth_type;
        BYTE gba_type;
        BYTE gba_tag;
        BYTE data1_len;
        BYTE data1[255];
        BYTE data2_len;
        BYTE data2[255];
    */
    const int testDataLen = 516;
    char testData[testDataLen] = {};
    testData[0] = 1; testData[1] = 1;
    testData[4] = 1; testData[5] = 'Z';
    testData[4+255] = 1; testData[4+255+1] = 'Z';
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_sim_gba_auth_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspDataLen = 260 + 12;
    mRspData = allocRspModemMem(mRspDataLen, sitId, sitToken);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestIms, oemImsRegState) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_IMS_REGISTRATION_STATE;
    //UINT16 sitId = 0;    // No SIT command, It returns RIL_E_GENERIC_FAILURE
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // It returns RIL_E_GENERIC_FAILURE
}


TEST_P(SitrilTestIms, oemAimsSetCallWaiting) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_AIMS_SET_CALL_WAITING;
    UINT16 sitId = SIT_AIMS_SET_CALL_WAITING;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    sit_aims_set_call_waiting_ex testData = { 1, 2, "ABCDE"};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format) + sizeof(testData);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestIms, oemAimsDial) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_AIMS_DIAL;
    UINT16 sitId = SIT_AIMS_DIAL;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const int testDataLen = 6;
    char testData[testDataLen] = "ABCDE";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format) + testDataLen;
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildDummyRawRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestIms, oemAimsRegIndReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_AIMS_REGISTER_IND;
    //UINT16 sitId = SIT_AIMS_REGISTER_IND;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const int testDataLen = 6;
    char testData[testDataLen] = "ABCDE";

    // It send IPC REQ but IPC RSP is not expected, RIL complete comes.
    // modem data can be not confirmed in the case which IPC RSP is not expected, (= SendRequest(pModemData)is used)
    // In order to support this in test unit, sitril code modification is needed.
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}


TEST_P(SitrilTestIms, oemAimsStackStart) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_AIMS_STACK_START_REQ;
    UINT16 sitId = SIT_AIMS_STACK_START_REQ;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const int testDataLen = 6;
    char testData[testDataLen] = "ABCDE";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format) + testDataLen;
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildDummyRawRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is not null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((unsigned int)0, rilRspDataLen);
}

////////////////////////////////////////////////////////////////////////////////////////
// indication
TEST_P(SitrilTestIms, oemImsSrvccHoInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SRVCC_HO;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    RIL_SrvccState testVector[] = { HANDOVER_FAILED, HANDOVER_STARTED, HANDOVER_COMPLETED };
    size_t size = sizeof(testVector) / sizeof(testVector);
    for (size_t i = 0; i < size; i++) {
        mRspData = buildSrvccHoInd(sitId, &mRspDataLen, testVector[i]);
        psitRilTest->processInd(RIL_UNSOL_OEM_IMS_SET_SRVCC_INFO, mRspData, mRspDataLen);

        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        // ind data is not null
        UINT32 indLen = 0;
        const void *rilIndData= nullptr;
        rilIndData = psitRilTest->getRilIndData(0, &indLen);
        EXPECT_NE((UINT32)0, indLen);

        FreeData();
        mSitRilTest->resetTestData();
    }
}

TEST_P(SitrilTestIms, oemAimsRegInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_AIMS_REGISTRATION;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    BYTE testVector[] = { SITRIL_AIMS_IMSREG_STATE_REGISTERED, SITRIL_AIMS_IMSREG_STATE_NOT_REGISTERED };
    size_t size = sizeof(testVector) / sizeof(testVector);
    for (size_t i = 0; i < size; i++) {
        mRspData = buildAimsRegInd(sitId, &mRspDataLen, testVector[i]);
        psitRilTest->processInd(RIL_UNSOL_OEM_AIMS_REGISTRATION, mRspData, mRspDataLen);

        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        // ind data is not null
        UINT32 indLen = 0;
        const void *rilIndData= nullptr;
        rilIndData = psitRilTest->getRilIndData(0, &indLen);
        EXPECT_NE((UINT32)0, indLen);

        FreeData();
        mSitRilTest->resetTestData();
    }
}

TEST_P(SitrilTestIms, oemAimsCallRingInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_AIMS_CALL_RING;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildDummyRawInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_AIMS_CALL_RING, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // ind data is not null
    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestIms, UpdateImsRegistrationInfoHandler) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_UPDATE_REGISTRATION_INFO;
    uint16_t sitId = SIT_IMS_SET_REGISTRATION_STATUS;
    unsigned int sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    RilImsV2_1::RIL_UpdateImsRegistration testVector = {
        RilImsV2_1::RIL_ImsRegState::NOT_REGISTERED,
        2, ACCESS_NETWORK_GERAN,
        RilImsV2_1::RIL_SuggestedAction::TRIGGER_PLMN_BLOCK_WITH_TIMEOUT};

    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    uint32_t sizeOfModemData = sizeof(sit_ims_set_registration_status_req);
    uint32_t txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(static_cast<unsigned int>(0), rilRspDataLen);
}

TEST_P(SitrilTestIms, UpdateImsRegistrationInfoHandler_oem) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_UPDATE_REGISTRATION_INFO;
    uint16_t sitId = SIT_IMS_SET_REGISTRATION_STATUS;
    unsigned int sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testVector[4] = { 0, 1, 2, 3 };
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ims_set_registration_status_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    uint32_t rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(static_cast<unsigned int>(0), rilRspDataLen);
}

TEST_P(SitrilTestIms, TriggerImsDeregistrationInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    uint16_t sitId = SIT_IND_IMS_DEREGISTERTAION;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    uint8_t *rspData = nullptr;
    uint16_t len = static_cast<uint16_t>(sizeof(sit_ims_trigger_ims_deregistration_ind));

    rspData = allocIndModemMem(len, sitId);
    mRspDataLen = len;

    sit_ims_trigger_ims_deregistration_ind *ind =
        reinterpret_cast<sit_ims_trigger_ims_deregistration_ind *>(rspData);
    ind->reason = 1;
    mRspData = rspData;

    psitRilTest->processInd(RIL_UNSOL_TRIGGER_IMS_DEREGISTRATION, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    uint32_t indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE(static_cast<uint32_t>(0), indLen);
}

TEST_P(SitrilTestIms, StartImsTrafficHandler_oem) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_START_IMS_TRAFFIC;
    uint16_t sitId = SIT_IMS_START_TRAFFIC;
    unsigned int sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testVector[4] = { 3, 2, 1, 0 };
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ims_start_ims_traffic_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspDataLen = static_cast<uint16_t>(sizeof(sit_ims_start_ims_traffic_rsp));
    UINT8 *rspData = nullptr;
    rspData = allocRspModemMem(mRspDataLen, sitId, sitToken);

    sit_ims_start_ims_traffic_rsp *rsp =
        reinterpret_cast<sit_ims_start_ims_traffic_rsp *>(rspData);
    rsp->connectionFailureReason = 0;
    rsp->causeCode = 1;
    rsp->waitTimeMillis = 2;
    mRspData = rspData;
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE(static_cast<UINT32>(0), rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(RilImsV2_1::RIL_ConnectionFailureInfo);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    RilImsV2_1::RIL_ConnectionFailureInfo *data =
        reinterpret_cast<RilImsV2_1::RIL_ConnectionFailureInfo *>(rilRspData);
    EXPECT_EQ(0, data->failureReason);
    EXPECT_EQ(1, data->causeCode);
    EXPECT_EQ(2, data->waitTimeMillis);
}

TEST_P(SitrilTestIms, StopImsTrafficHandler_oem) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_STOP_IMS_TRAFFIC;
    uint16_t sitId = SIT_IMS_STOP_TRAFFIC;
    unsigned int sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testVector = { 2 };
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ims_stop_ims_traffic_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    uint32_t rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(static_cast<unsigned int>(0), rilRspDataLen);
}

TEST_P(SitrilTestIms, OnConnectionSetupFailureInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    uint16_t sitId = SIT_IND_IMS_RRC_CONNECTION_FAILURE;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    uint8_t *rspData = nullptr;
    uint16_t len = static_cast<uint16_t>(sizeof(sit_ims_rrc_connection_failure_ind));

    rspData = allocIndModemMem(len, sitId);
    mRspDataLen = len;

    sit_ims_rrc_connection_failure_ind *ind =
        reinterpret_cast<sit_ims_rrc_connection_failure_ind *>(rspData);
    ind->token = 0;
    ind->connectionFailureReason = 1;
    ind->causeCode = 2;
    ind->waitTimeMillis = 3;
    mRspData = rspData;

    psitRilTest->processInd(RIL_UNSOL_CONNECTION_SETUP_FAILURE, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    uint32_t indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE(static_cast<uint32_t>(0), indLen);

    const int32_t *data = reinterpret_cast<const int32_t *>(rilIndData);
    EXPECT_EQ(ind->token, data[0]);
    EXPECT_EQ(ind->connectionFailureReason, data[1]);
    EXPECT_EQ(ind->causeCode, data[2]);
    EXPECT_EQ(ind->waitTimeMillis, data[3]);

    const RilImsV2_1::RIL_OnConnectionSetupFailure data1 = *(reinterpret_cast<const RilImsV2_1::RIL_OnConnectionSetupFailure *>(rilIndData));
    EXPECT_EQ(ind->token, data1.token);
    EXPECT_EQ(ind->connectionFailureReason, data1.info.failureReason);
    EXPECT_EQ(ind->causeCode, data1.info.causeCode);
    EXPECT_EQ(ind->waitTimeMillis, data1.info.waitTimeMillis);
}

TEST_P(SitrilTestIms, TriggerEpsFallbackHandler) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_TRIGGER_EPS_FALLBACK;
    uint16_t sitId = SIT_SET_TRIGGER_EPSFB;
    unsigned int sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testVector = 1;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_set_trigger_epsfb_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    ProtocolReqAdapter testAdapter(mpModemData);
    EXPECT_EQ((*(reinterpret_cast<const char *>(testAdapter.GetParameter()))), static_cast<const char>(testVector));

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    uint32_t rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(static_cast<unsigned int>(0), rilRspDataLen);
}

TEST_P(SitrilTestIms, TriggerEpsFallbackHandler_oem) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_TRIGGER_EPS_FALLBACK;
    uint16_t sitId = SIT_SET_TRIGGER_EPSFB;
    unsigned int sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    uint8_t testVector = 2;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_set_trigger_epsfb_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    ProtocolReqAdapter testAdapter(mpModemData);
    EXPECT_EQ((*(reinterpret_cast<const unsigned char *>(testAdapter.GetParameter()))), testVector);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    uint32_t rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(static_cast<unsigned int>(0), rilRspDataLen);
}

TEST_P(SitrilTestIms, IndTriggerEpsFallbackInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    uint16_t sitId = SIT_SET_TRIGGER_EPSFB;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    uint8_t *rspData = nullptr;
    uint16_t len = static_cast<uint16_t>(sizeof(sit_set_trigger_epsfb_ind));

    rspData = allocIndModemMem(len, sitId);
    mRspDataLen = len;

    sit_set_trigger_epsfb_ind *ind =
        reinterpret_cast<sit_set_trigger_epsfb_ind *>(rspData);
    ind->result = 0;
    ind->rat = 1;
    mRspData = rspData;

    psitRilTest->processInd(RIL_UNSOL_OEM_IMS_TRIGGER_EPS_FALLBACK, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    uint32_t indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE(static_cast<uint32_t>(0), indLen);

    const RilImsV2_1::RIL_TriggerFpsFallbackInd data = *(reinterpret_cast<const RilImsV2_1::RIL_TriggerFpsFallbackInd *>(rilIndData));
    EXPECT_EQ(ind->result, data.result);
    EXPECT_EQ(ind->rat, data.rat);
}

TEST_P(SitrilTestIms, SetSrvccCallInfoHandler_oem) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_SET_SRVCC_CALL_INFO;
    uint16_t sitId = SIT_SET_SRVCC_CALL_LIST;
    unsigned int sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int32_t size = 4;
    RilImsV2_1::RIL_SrvccCallListOem testVector = {
        size,
        { { 0, static_cast<uint8_t>(0), static_cast<uint8_t>(0), static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), true, false, static_cast<uint8_t>(11), "01012341234",
            0, static_cast<uint8_t>(3), "AAA", 0 },
        { 1, static_cast<uint8_t>(1), static_cast<uint8_t>(1), static_cast<uint8_t>(1),
            static_cast<uint8_t>(1), false, true, static_cast<uint8_t>(11), "01043214321",
            1, static_cast<uint8_t>(3), "BBB", 1 },
        { 2, static_cast<uint8_t>(0), static_cast<uint8_t>(2), static_cast<uint8_t>(0),
            static_cast<uint8_t>(2), true, true, static_cast<uint8_t>(11), "01011112222",
            2, static_cast<uint8_t>(3), "CCC", 2 },
        { 3, static_cast<uint8_t>(1), static_cast<uint8_t>(3), static_cast<uint8_t>(1),
            static_cast<uint8_t>(0), false, false, static_cast<uint8_t>(11), "01033334444",
            3, static_cast<uint8_t>(3), "DDD", 3 }, }
    };

    psitRilTest->processExternReq(GetRandomSerialNumber(),
            testRilCmdId, &testVector, (sizeof(RilImsV2_1::RIL_SrvccCallListOem::callListNum) +
                (sizeof(RilImsV2_1::RIL_OemSrvccCall) * size)), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(RCM_HEADER) +
        sizeof(sit_ims_set_srvcc_call_list_req::callListNum) +
        (sizeof(current_call_list) * size);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    ProtocolReqAdapter testAdapter(mpModemData);
    const sit_ims_set_srvcc_call_list_req result =
        *(reinterpret_cast<const sit_ims_set_srvcc_call_list_req *>(mpModemData->GetRawData()));
    EXPECT_EQ(static_cast<int32_t>(size), result.callListNum);

    for (int i = 0; i < result.callListNum; i++) {
        const current_call_list to =
            *(reinterpret_cast<const current_call_list *>(mpModemData->GetRawData() +
                        sizeof(RCM_HEADER) + sizeof(sit_ims_set_srvcc_call_list_req::callListNum) +
                        (sizeof(current_call_list) * i)));

        EXPECT_EQ(to.index, testVector.callList[i].index);
        EXPECT_EQ(to.callType, static_cast<uint8_t>(testVector.callList[i].callType));
        EXPECT_EQ(to.state, static_cast<uint8_t>(testVector.callList[i].callState));
        EXPECT_EQ(to.subState, static_cast<uint8_t>(testVector.callList[i].callSubstate));
        EXPECT_EQ(to.ringBackTone, static_cast<uint8_t>(testVector.callList[i].ringbackToneType));
        EXPECT_EQ(to.isMpty, testVector.callList[i].isMpty);
        EXPECT_EQ(to.isMt, testVector.callList[i].isMT);
        EXPECT_EQ(to.numberLen, static_cast<uint8_t>(testVector.callList[i].numberLen));
        EXPECT_STREQ(to.number, testVector.callList[i].number);
        EXPECT_EQ(to.numberPresentation, testVector.callList[i].numPresentation);
        EXPECT_EQ(to.nameLen, static_cast<uint8_t>(testVector.callList[i].nameLen));
        EXPECT_STREQ(to.name, testVector.callList[i].name);
        EXPECT_EQ(to.namePresentation, testVector.callList[i].namePresentation);
    }

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    uint32_t rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(static_cast<unsigned int>(0), rilRspDataLen);
}

// This TC will be enabled for for RIL_REQUEST_SET_SRVCC_CALL_INFO.
#if 0
TEST_P(SitrilTestIms, SetSrvccCallInfoHandler) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_SRVCC_CALL_INFO;
    uint16_t sitId = SIT_SET_SRVCC_CALL_LIST;
    unsigned int sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    RilImsV2_1::RIL_SrvccCall testVector[] = {
        {0, RilImsV2_1::RIL_CallType::NORMAL, 0, RilImsV2_1::RIL_CallSubState::NONE,
            RilImsV2_1::RIL_ToneType::LOCAL, true, false, "01012341234", 0, "AAA", 0},
        {1, RilImsV2_1::RIL_CallType::EMERGENCY, 1, RilImsV2_1::RIL_CallSubState::PREALERTING,
            RilImsV2_1::RIL_ToneType::NETWORK, false, true, "01043214321", 1, "BBB", 1},
        {2, RilImsV2_1::RIL_CallType::NORMAL, 2, RilImsV2_1::RIL_CallSubState::NONE,
            RilImsV2_1::RIL_ToneType::NONE, true, true, "01011112222", 2, "CCC", 2},
        {3, RilImsV2_1::RIL_CallType::EMERGENCY, 3, RilImsV2_1::RIL_CallSubState::PREALERTING,
            RilImsV2_1::RIL_ToneType::NONE, false, false, "01033334444", 3, "DDD", 3},
    };


    int num = sizeof(testVector) / sizeof(testVector[0]);
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &testVector,
            sizeof(testVector), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(RCM_HEADER) +
        sizeof(sit_ims_set_srvcc_call_list_req::callListNum) +
        (sizeof(current_call_list) * num);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    ProtocolReqAdapter testAdapter(mpModemData);
    const sit_ims_set_srvcc_call_list_req result =
        *(reinterpret_cast<const sit_ims_set_srvcc_call_list_req *>(mpModemData->GetRawData()));

    EXPECT_EQ(static_cast<int32_t>(num), result.callListNum);

    for (int i = 0; i < result.callListNum; i++) {
        const current_call_list to =
            *(reinterpret_cast<const current_call_list *>(mpModemData->GetRawData() +
                        sizeof(RCM_HEADER) + sizeof(sit_ims_set_srvcc_call_list_req::callListNum) +
                        (sizeof(current_call_list) * i)));

        EXPECT_EQ(to.index, testVector[i].index);
        EXPECT_EQ(to.callType, static_cast<uint8_t>(testVector[i].callType));
        EXPECT_EQ(to.state, static_cast<uint8_t>(testVector[i].callState));
        EXPECT_EQ(to.subState, static_cast<uint8_t>(testVector[i].callSubstate));
        EXPECT_EQ(to.ringBackTone, static_cast<uint8_t>(testVector[i].ringbackToneType));
        EXPECT_EQ(to.isMpty, testVector[i].isMpty);
        EXPECT_EQ(to.isMt, testVector[i].isMT);
        EXPECT_EQ(to.numberLen, static_cast<uint8_t>(testVector[i].number.length()));
        EXPECT_STREQ(to.number, testVector[i].number.c_str());
        EXPECT_EQ(to.numberPresentation, testVector[i].numPresentation);
        EXPECT_EQ(to.nameLen, static_cast<uint8_t>(testVector[i].name.length()));
        EXPECT_STREQ(to.name, testVector[i].name.c_str());
        EXPECT_EQ(to.namePresentation, testVector[i].namePresentation);
    }

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    uint32_t rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(static_cast<unsigned int>(0), rilRspDataLen);
}
#endif

TEST_P(SitrilTestIms, MtCallGuardtmrStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_MT_CALL_GUARDTMR_STATUS_IND;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testVector = { 2 };
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &testVector, sizeof(testVector), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

TEST_P(SitrilTestIms, MtCsfbInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    uint16_t sitId = SIT_IND_MT_CSFB;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    uint8_t *rspData = nullptr;
    uint16_t len = static_cast<uint16_t>(sizeof(null_ind_data_format));

    rspData = allocIndModemMem(len, sitId);
    mRspDataLen = len;
    mRspData = rspData;

    psitRilTest->processInd(RIL_UNSOL_OEM_IMS_MT_CSFB, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    uint32_t indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ(static_cast<uint32_t>(0), indLen);
}