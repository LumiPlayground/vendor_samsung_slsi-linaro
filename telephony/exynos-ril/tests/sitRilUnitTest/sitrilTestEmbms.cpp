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

#include "sitrilTestEmbms.h"
#include "testUtil.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////
// hard coded variables


// SitrilTestEmbms class
void SitrilTestEmbms::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();
}

void SitrilTestEmbms::TearDown() {
    mSitRilTest = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////
// Build Ril Response
static UINT8* buildGetSessionListRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_embms_ssesion_list_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_embms_ssesion_list_rsp *rsp = (sit_embms_ssesion_list_rsp*) rspData;
    rsp->session_state = 1;
    rsp->oos_reason = 0;
    rsp->number_record = 1;
    rsp->tmgi[0] = 0x00;
    rsp->tmgi[1] = 0x00;
    rsp->tmgi[2] = 0x01;
    rsp->tmgi[3] = 0x13;
    rsp->tmgi[4] = 0x01;
    rsp->tmgi[5] = 0x93;

    return rspData;
}

static UINT8* buildSessionListInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_embms_ssesion_list_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_embms_ssesion_list_rsp *rsp = (sit_embms_ssesion_list_rsp*) rspData;
    rsp->session_state = 1;
    rsp->oos_reason = 0;
    rsp->number_record = 1;
    rsp->tmgi[0] = 0x00;
    rsp->tmgi[1] = 0x00;
    rsp->tmgi[2] = 0x01;
    rsp->tmgi[3] = 0x13;
    rsp->tmgi[4] = 0x01;
    rsp->tmgi[5] = 0x93;

    return rspData;
}

static UINT8* buildGetSignalStrengthRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_embms_signal_strength_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_embms_signal_strength_rsp *rsp = (sit_embms_signal_strength_rsp*) rspData;
    rsp->number_record = 1;
    rsp->signal_record[0].SNR = 1;
    rsp->signal_record[0].MBSFNAreaId = 1;
    rsp->signal_record[0].ESNR = 1;
    rsp->signal_record[0].num_tmgi = 1;
    rsp->signal_record[0].tmgi[0][0] = 0x00;
    rsp->signal_record[0].tmgi[0][1] = 0x00;
    rsp->signal_record[0].tmgi[0][2] = 0x01;
    rsp->signal_record[0].tmgi[0][3] = 0x13;
    rsp->signal_record[0].tmgi[0][4] = 0x01;
    rsp->signal_record[0].tmgi[0][5] = 0x93;

    return rspData;
}

static UINT8* buildSignalStrengthInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_embms_signal_strength_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_embms_signal_strength_rsp *rsp = (sit_embms_signal_strength_rsp*) rspData;
    rsp->number_record = 1;
    rsp->signal_record[0].SNR = 1;
    rsp->signal_record[0].MBSFNAreaId = 1;
    rsp->signal_record[0].ESNR = 1;
    rsp->signal_record[0].num_tmgi = 1;
    rsp->signal_record[0].tmgi[0][0] = 0x00;
    rsp->signal_record[0].tmgi[0][1] = 0x00;
    rsp->signal_record[0].tmgi[0][2] = 0x01;
    rsp->signal_record[0].tmgi[0][3] = 0x13;
    rsp->signal_record[0].tmgi[0][4] = 0x01;
    rsp->signal_record[0].tmgi[0][5] = 0x93;

    return rspData;
}

static UINT8* buildCoverageInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_embms_coverage_rsp);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_embms_coverage_rsp *rsp = (sit_embms_coverage_rsp*) rspData;
    rsp->coverage = 2;// EMBMS_FULL_COVERAGE

    return rspData;
}

static UINT8* buildSaiListInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_embms_sailist);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_embms_sailist *rsp = (sit_embms_sailist*) rspData;

    rsp->IntraSaiListNum = 3;
    rsp->InterSaiListNum = 1;
    rsp->IntraSaiList[0] = 0;
    rsp->IntraSaiList[0] = 2;
    rsp->IntraSaiList[0] = 3;
    rsp->InterSaiList[0].Frequency = 0;
    rsp->InterSaiList[0].InterSaiNumber = 0;
    rsp->InterSaiList[0].MultiBandInfoNumber = 0;
    rsp->InterSaiList[0].InterSaiInfo[58] = 0x146e;
    rsp->InterSaiList[0].InterSaiInfo[60] = 0x3;
    rsp->InterSaiList[0].InterSaiInfo[62] = 0x1;
    rsp->InterSaiList[0].InterSaiInfo[63] = 0x68;

    return rspData;
}

static UINT8* buildGlobalCellIdInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_embms_global_cellid_rsp);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_embms_global_cellid_rsp *rsp = (sit_embms_global_cellid_rsp*) rspData;
    strncpy((char *)rsp->mcc, (const char*)"311", strlen("311"));
    strncpy((char *)rsp->mnc, (const char*)"390", strlen("390"));
    rsp->cellId = 0x186242;

    return rspData;
}

static UINT8* buildGetEmbmsNetworkTimeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_embms_network_time_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_embms_network_time_rsp *rsp = (sit_embms_network_time_rsp*) rspData;
    rsp->Sib16Acquired = 0x00;
    rsp->TimeInfoType = 0x01;
    rsp->daylight_valid = 0x02;
    rsp->year = 0x03;
    rsp->month = 0x04;
    rsp->day = 0x05;
    rsp->hour = 0x06;
    rsp->minute = 0x07;
    rsp->second = 0x08;
    rsp->time_zone = 0x09;
    rsp->daylight_adjust = 0x01;
    rsp->day_of_week = 0x02;
    strncpy((char *)rsp->PLMN, (const char*)"123456", strlen("123456"));
    rsp->AbsoluteTime = 0x03;
    rsp->LeapSecond = 0x04;

    return rspData;
}

static UINT8* buildGetPreferredNtwTypeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_net_get_pref_network_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_net_get_pref_network_rsp *rsp = (sit_net_get_pref_network_rsp *)rspData;
    rsp->pref_net_type = 1;

    return rspData;
}

////////////////////////////////////////////////////////////////////////////////////////
// Build Indication

////////////////////////////////////////////////////////////////////////////////////////
// SET reqeust|response //

TEST_P(SitrilTestEmbms, enableService) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_EMBMS_ENABLE_SERVICE;
    UINT16 sitId = SIT_SET_EMBMS_SERVICE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int state = ENABLE_SERVICE;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&state, sizeof(int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_embms_set_service_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestEmbms, disableService) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_EMBMS_DISABLE_SERVICE;
    UINT16 sitId = SIT_SET_EMBMS_SERVICE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int state = DISABLE_SERVICE;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&state, sizeof(int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_embms_set_service_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestEmbms, startSession) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_EMBMS_SET_SESSION;
    UINT16 sitId = SIT_SET_EMBMS_SESSION;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    RIL_EmbmsSessionData requestData = {};

    requestData.state = 1;
    requestData.priority = 0;
    requestData.tmgi = (uint64_t)0x1130193;
    requestData.infobind.uSAICount = 1;
    requestData.infobind.uFreqCount = 2;
    requestData.infobind.nFreqList[0] = 2050;
    requestData.infobind.nFreqList[1] = 5230;   // uint32_t nFreqList[8];

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&requestData, sizeof(requestData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_embms_set_session_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

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

TEST_P(SitrilTestEmbms, getSessionList) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_EMBMS_GET_SESSION_LIST;
    UINT16 sitId = SIT_GET_EMBMS_SESSION_LIST;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int state = ENABLE_SERVICE;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&state, sizeof(int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_embms_ssesion_list_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildGetSessionListRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestEmbms, getSignalStrength) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_EMBMS_GET_SIGNAL_STRENGTH;
    UINT16 sitId = SIT_GET_EMBMS_SIGNAL_STRENGTH;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetSignalStrengthRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestEmbms, getNetworkTime) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_EMBMS_GET_NETWORK_TIME;
    UINT16 sitId = SIT_GET_EMBMS_NETWORK_TIME;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetEmbmsNetworkTimeRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestEmbms, checkAvailableEmbms) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_EMBMS_CHECK_AVAIABLE_EMBMS;
    UINT16 sitId = SIT_GET_PREFERRED_NTW_TYPE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetPreferredNtwTypeRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestEmbms, sessionListUpdateInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_EMBMS_SESSION_LIST;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSessionListInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_EMBMS_SESSION_LIST, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestEmbms, signalStrengthInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_EMBMS_SIGNAL_STRENGTH;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSignalStrengthInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_EMBMS_SIGNAL_STRENGTH, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestEmbms, coverageInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_EMBMS_COVERAGE;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildCoverageInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_EMBMS_COVERAGE, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestEmbms, saiListInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_EMBMS_SAI_LIST;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSaiListInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_EMBMS_SAI_LIST, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestEmbms, globalCellIdInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_EMBMS_GLOBAL_CELL_ID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildGlobalCellIdInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_EMBMS_GLOBAL_CELL_ID, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

