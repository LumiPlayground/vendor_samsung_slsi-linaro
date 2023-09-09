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

#include "sitrilTestRadioPower.h"
#include "testUtil.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////////////////////////
// SET request|response //
static UINT8* buildSetRadioPowerRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_pwr_radio_power_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

/*
static UINT8* buildGetRadioStateRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, int radioState) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_pwr_get_radio_power_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_pwr_get_radio_power_rsp *rsp = (sit_pwr_get_radio_power_rsp *)rspData;
    rsp->radio_state = radioState;

    return rspData;
}
*/

static UINT8* buildRadioStateChangedInd(INT16 sitId, UINT32 *datalen, int radioState) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pwr_radio_state_changed_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pwr_radio_state_changed_ind *ind = (sit_pwr_radio_state_changed_ind *)rspData;
    ind->radio_state = radioState;

    return rspData;
}

// sitrilTestRadioPower class
void SitrilTestRadioPower::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    mpModemData = nullptr;
    mRspData = nullptr;
    mRspDataLen = 0;
}

void SitrilTestRadioPower::TearDown() {
    mSitRilTest = nullptr;

    if (mpModemData) { delete mpModemData; mpModemData = nullptr; }
    if (mRspData) { free(mRspData); mRspData = nullptr; }
    mRspDataLen = 0;
}

TEST_P(SitrilTestRadioPower, setRadio_off) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_RADIO_POWER;
    UINT16 sitId = SIT_SET_RADIO_POWER;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int radioPower = 0;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &radioPower, sizeof(radioPower), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pwr_set_radio_power_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildSetRadioPowerRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestRadioPower, radioStatechangedInd_stop) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_RADIO_STATE_CHANGED;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildRadioStateChangedInd(sitId, &mRspDataLen, SIT_PWR_RADIO_STATE_STOP_NETWORK);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestRadioPower, setRadio_on) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_RADIO_POWER;
    UINT16 sitId = SIT_SET_RADIO_POWER;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int radioPower = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &radioPower, sizeof(radioPower), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pwr_set_radio_power_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildSetRadioPowerRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestRadioPower, radioStatechangedInd_start) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_RADIO_STATE_CHANGED;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildRadioStateChangedInd(sitId, &mRspDataLen, SIT_PWR_RADIO_STATE_START_NETWORK);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestRadioPower, getRadioState) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_RADIO_STATE;
    //UINT16 sitId = SIT_GET_RADIO_POWER;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

#if 1
    // if RADIO_READY state, there is no IPC.
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

#else
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pwr_get_radio_power_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetRadioStateRsp(sitId, sitToken, &mRspDataLen, SIT_PWR_RADIO_SIM_STATE_ON);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
#endif
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);
    int radioState = *((int*)rilRspData);
    EXPECT_EQ(true, (radioState && (int)RADIO_STATE_ON) ||
            (radioState && RADIO_STATE_OFF) || (radioState && RADIO_STATE_UNAVAILABLE));
}

TEST_P(SitrilTestRadioPower, setShutDown) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SHUTDOWN;
    UINT16 sitId = SIT_SET_RADIO_POWER;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pwr_set_radio_power_req);
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

TEST_P(SitrilTestRadioPower, radioStatechangedInd_restart) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_RADIO_STATE_CHANGED;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildRadioStateChangedInd(sitId, &mRspDataLen, SIT_PWR_RADIO_STATE_START_NETWORK);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);

    // Wait to recover a radio state before previous test start
    sleep(3);
}
