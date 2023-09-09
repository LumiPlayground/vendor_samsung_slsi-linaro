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

#include "sitrilTestVsim.h"
#include "testUtil.h"

////////////////////////////////////////////////////////////////////////////////////////
// SIT build
static UINT8* buildVsimOperationInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = 0;
    const UINT16 payloadDataLen = 10;
    BYTE payloadData[payloadDataLen] = { 0, 1, 2, 3, 4 ,5, 6, 7, 8, 9 };

    len = (UINT16)sizeof(sit_vsim_opertaion_ind) - (UINT16)sizeof(sit_vsim_opertaion_ind::data)
        + payloadDataLen;

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_vsim_opertaion_ind *ind = (sit_vsim_opertaion_ind*)rspData;
    ind->tid = 1;
    ind->event_id = SIT_VSIM_OPERATION_ATR;
    ind->result = 2;
    ind->datalength = payloadDataLen;
    memcpy(ind->data, payloadData, payloadDataLen);

    return rspData;
}


////////////////////////////////////////////////////////////////////////////////////////
// test
void SitrilTestVsim::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    mpModemData = nullptr;
    mRspData = nullptr;
    mRspDataLen = 0;
}

void SitrilTestVsim::TearDown() {
    mSitRilTest = nullptr;

    if (mpModemData) { delete mpModemData; mpModemData = nullptr; }
    if (mRspData) { free(mRspData); mRspData = nullptr; }
    mRspDataLen = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
// request|response
TEST_P(SitrilTestVsim, oemVsimNoti) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_VSIM_NOTIFICATION;
    UINT16 sitId = SIT_VSIM_NOTIFICATION;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int notiData[3] = {1, REQUEST_TYPE_ENABLE_EXTERNAL_SIM, 2};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &notiData, sizeof(notiData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_vsim_notification_req);
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
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestVsim, oemVsimOperation) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_VSIM_OPERATION;
    UINT16 sitId = SIT_VSIM_OPERATION;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const int testDataLen = 6;
    char testData[testDataLen] = "ABCDE";
    RIL_VsimOperationEvent opData = { 1, REQUEST_TYPE_ATR_EVENT, 2, testDataLen, testData};
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &opData, sizeof(opData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_vsim_opertaion_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
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
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestVsim, rilVsimNoti) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_LOCAL_REQUEST_VSIM_NOTIFICATION;
    //UINT16 sitId = SIT_VSIM_NOTIFICATION;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int notiData[3] = {1, REQUEST_TYPE_ENABLE_EXTERNAL_SIM, 2};
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &notiData, sizeof(notiData), mPhoneIndex);

    // It is deprecated, so NOT_SUPPORT error rsp would come.
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

TEST_P(SitrilTestVsim, rilVsimOperation) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_LOCAL_REQUEST_VSIM_OPERATION;
    //UINT16 sitId = SIT_VSIM_OPERATION;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const int testDataLen = 6;
    char testData[testDataLen] = "ABCDE";
    RIL_VsimOperationEvent opData = { 1, REQUEST_TYPE_ATR_EVENT, 2, testDataLen, testData};
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &opData, sizeof(opData), mPhoneIndex);

    // It is deprecated, so NOT_SUPPORT error rsp would come.
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

////////////////////////////////////////////////////////////////////////////////////////
// indication
TEST_P(SitrilTestVsim, vsimOperationInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_VSIM_OPERATION;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildVsimOperationInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_VSIM_OPERATION, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}
